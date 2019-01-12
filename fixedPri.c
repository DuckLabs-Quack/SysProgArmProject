#include "fixedPri.h"
#include "stm32f4xx.h"
#include <stdio.h>

#include "mutex.h"
#include "stack.h"

/* DEBUGGING PRINTING DEFINITION. Currently disabled. */
#define USE_DEBUG_PF 0
#if USE_DEBUG_PF
#define DEBUG_PF(...) printf(__VA_ARGS__)
#else
#define DEBUG_PF(...)
#endif
typedef linked_list_t waitlist_t;

/* Fixed-priority scheduler. 
	 The tasks are given a priority (the lower the priority number, the higher priority it has) and sorted in priority order. 
	 Tasks must sleep/wait to allow for lower priority tasks to run else the highest priority tasks will always run. 
	 
	 Tasks that are waiting (waiting for the mutex) are sorted in TIME order. This is to allow for lower priority tasks 
	 to run and acquire a mutex if a higher priority task is sleeping/waiting without priority inheritence. 
	 However, it still ensures that higher priority tasks will run over lower priority tasks if they are runnable. */

/* Prototypes (functions are static, so these aren't in the header file) */
static OS_TCB_t const* fixedPri_scheduler(void);
static void fixedPri_addTask(OS_TCB_t* const tcb);
static void fixedPri_taskExit(OS_TCB_t* const tcb);
static void fixedPri_wait(void* const reason, uint32_t value);
static void fixedPri_notify(void* const reason);
static void fixedPri_ISR_notify(void* const reason);

static OS_TCB_t* tasks[FIX_PRIO_MAX_TASKS] = {0};

/* Debugging functions for printing variables. */
void printTasks() {
	for(int i = 0; i < FIX_PRIO_MAX_TASKS; i++) {
		DEBUG_PF("%u\r\n", tasks[i]->priority);
	}
}

void printStackPointers();
uint32_t taskID(OS_TCB_t* task);
uint32_t getCurrentTaskID();

void printWL(waitlist_t* wl) {
	linked_list_element_t* cur = wl->head;
	
	while (cur != NULL) {
		DEBUG_PF("WL %u\r\n", taskID(cur->item));
		cur = cur->next;
	}
}


/* Scheduler block for the binary heap scheduler */
OS_Scheduler_t const fixedPriScheduler = {
	.preemptive = 1,
	.scheduler_callback = fixedPri_scheduler,
	.addtask_callback = fixedPri_addTask,
	.taskexit_callback = fixedPri_taskExit,
	.wait_callback = fixedPri_wait,
	.notify_callback = fixedPri_notify,
	.ISR_notify_callback = fixedPri_ISR_notify
};

/* Scheduler */
static OS_TCB_t const* fixedPri_scheduler(void) {
	
	/* Before checking the tasks, check the ISR wait list and notify the tasks on the list. 
		 This is to prevent the wait list from changing when an ISR notifies waiting tasks. */
	stack_t* pendingISRnotify = OS_get_pending_ISR_notify();
	
	while (!stack_is_empty(pendingISRnotify)) {
		waitlist_t* waitlist = stack_pop(pendingISRnotify);
		fixedPri_notify(waitlist);
	}
	
	/* Once the stacked wait list is empty and all tasks are notified on the list,
		 resume normal scheduling. */
	
	for (int i = 0; i <= FIX_PRIO_MAX_TASKS; i++) {
		if (tasks[i] == NULL) {
	      break;
		}
		
		tasks[i]->state &= ~TASK_STATE_YIELD;
		
		if (tasks[i]->state != TASK_STATE_SLEEP && tasks[i]->state != TASK_STATE_WAIT) {
			return tasks[i];
		} else if (tasks[i]->state == TASK_STATE_SLEEP){
			if (tasks[i]->data < OS_elapsedTicks()) {
				tasks[i]->state &= ~TASK_STATE_SLEEP;
				return tasks[i];
			}
		}
	}
	/* No tasks in the list, so return the idle task. */
	return OS_idleTCB_p;
}

/* Add task and sorts into priority order. 
	 Since there are only a small amount of tasks, there is no need to use a different data structure to insert and sort the tasks. */
static void fixedPri_addTask(OS_TCB_t* const tcb) {	
	for (int i = 0; i < FIX_PRIO_MAX_TASKS; i++) {
		if (tasks[i] == 0) {
			tasks[i] = tcb;
			return;
		}
		else if (tcb->priority < tasks[i]->priority) {
			/* Sorts the tasks in priority order. */
			OS_TCB_t* tmp_a = tcb;
			OS_TCB_t* tmp_b;
			for (int j = i; j<FIX_PRIO_MAX_TASKS; j++) {
				if (tasks[j] == 0) {
					tasks[j] = tmp_a;
					break;
				} else {
					tmp_b = tasks[j];
					tasks[j] = tmp_a;
					tmp_a = tmp_b;
				}
			}
			return;
		}
	}
}

/* Task exit */
static void fixedPri_taskExit(OS_TCB_t* const tcb) {
	/* Remove the given TCB from the list of tasks so it won't be run again. */
	for (int i = 0; i < FIX_PRIO_MAX_TASKS; i++) {
		if (tasks[i] == tcb) {
			tasks[i] = 0;
		}
	}	
}

/* Wait. */
static void fixedPri_wait(void* const reason, uint32_t value) {
	DEBUG_PF("TASK %u: enter wait\r\n", getCurrentTaskID());
	
	/* Compare the check value here. If it has changed then a preemption or an interrupt
		 has occured, so leave the task in its current state. */
	if (value != OS_checkValue()) {
		return;
	}
	
	OS_TCB_t* current_task = OS_currentTCB();
	current_task->state = TASK_STATE_WAIT;
	
	/* The reason passed in is a pointer to the wait list. */
	waitlist_t* waitlist = (void*) reason;
	
	DEBUG_PF("WL wait start\r\n");
	printWL(waitlist);
	
	/* Adds the current task to the wait list. */
	linked_list_append(waitlist, current_task);
	
	DEBUG_PF("WL wait end\r\n");
	printWL(waitlist);
	DEBUG_PF("TASK %u: wait\r\n", getCurrentTaskID());
	
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	
}

/* Notify.
	 Since all tasks are stored in a wait list, there is no need to loop over all the tasks
	 to check for the wait status. */
static void fixedPri_notify(void* const reason) {
	/* The reason passed in is a pointer to the wait list. */
	waitlist_t* waitlist = (void*)reason;
	
	DEBUG_PF("WL notify start\r\n");
	printWL(waitlist);
	
	/* Remove the current task from the wait list. */
	OS_TCB_t* waitingTask = linked_list_remove(waitlist);
	
	DEBUG_PF("WL notify end\r\n");
	printWL(waitlist);
	
	/* Changes the state of the task to no longer be waiting and notify the waiting task. */
	if (waitingTask != NULL) {
		DEBUG_PF("TASK %u: notify : %u\r\n", getCurrentTaskID(), taskID(waitingTask));
		waitingTask->state &= ~TASK_STATE_WAIT;
	}
	
}

/* ISR Notify.
	 All interrupts that notify tasks MUST call this function instead of OS_notify() */
static void fixedPri_ISR_notify(void* const reason) {
	DEBUG_PF("WL ISR notify start\r\n");
	
	/* The reason passed in is a pointer to the wait list. */
	waitlist_t* waitlist = (void*)reason;
	
	stack_t* pendingISRnotify = OS_get_pending_ISR_notify();
	
	printWL(waitlist);
	
	/* Push the wait list to the ISR wait list stack so that the scheduler can handle the interrupt notify. */
	if (!stack_is_full(pendingISRnotify)) {
		stack_push(pendingISRnotify, waitlist);
	}
}
