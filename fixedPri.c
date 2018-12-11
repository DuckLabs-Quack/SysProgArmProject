#include "fixedPri.h"
#include "stm32f4xx.h"
#include <stdio.h>

#include "linkedList.h"
#include "mutex.h"

/* Prototypes (functions are static, so these aren't in the header file) */
static OS_TCB_t const* fixedPri_scheduler(void);
static void fixedPri_addTask(OS_TCB_t* const tcb);
static void fixedPri_taskExit(OS_TCB_t* const tcb);
static void fixedPri_wait(void* const reason);
static void fixedPri_notify(void* const reason);

static OS_TCB_t* tasks[SIMPLE_RR_MAX_TASKS] = {0};

void printTasks() {
	for(int i = 0; i < SIMPLE_RR_MAX_TASKS; i++) {
		printf("%u\r\n", tasks[i]->priority);
	}
}

/* Scheduler block for the binary heap scheduler */
OS_Scheduler_t const fixedPriScheduler = {
	.preemptive = 1,
	.scheduler_callback = fixedPri_scheduler,
	.addtask_callback = fixedPri_addTask,
	.taskexit_callback = fixedPri_taskExit,
	.wait_callback = fixedPri_wait,
	.notify_callback = fixedPri_notify
};

/* Scheduler */
static OS_TCB_t const* fixedPri_scheduler(void) {
	for (int i = 0; i <= SIMPLE_RR_MAX_TASKS; i++) {
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
	// No tasks in the list, so return the idle task
	return OS_idleTCB_p;
}

/* Add task */
static void fixedPri_addTask(OS_TCB_t* const tcb) {	
	for (int i = 0; i < SIMPLE_RR_MAX_TASKS; i++) {
		if (tasks[i] == 0) {
			tasks[i] = tcb;
			return;
		}
		else if (tcb->priority < tasks[i]->priority) {
			OS_TCB_t* tmp_a = tcb;
			OS_TCB_t* tmp_b;
			for (int j = i; j<SIMPLE_RR_MAX_TASKS; j++) {
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
	// Remove the given TCB from the list of tasks so it won't be run again
	for (int i = 0; i < SIMPLE_RR_MAX_TASKS; i++) {
		if (tasks[i] == tcb) {
			tasks[i] = 0;
		}
	}	
}

/* Wait */
static void fixedPri_wait(void* const reason) {
	OS_TCB_t* current_task = OS_currentTCB();
	//current_task->data = tcb->data;
	current_task->state = TASK_STATE_WAIT;
	
	OS_mutex_t* mutex = (void*)reason;
	
	linked_list_append(&mutex->waitlist, current_task);
	
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/* Notify */
static void fixedPri_notify(void* const reason) {
	OS_mutex_t* mutex = (void*)reason;
	OS_TCB_t* waitingTask = linked_list_remove(&mutex->waitlist);
	if (waitingTask != NULL) {
		waitingTask->state &= ~TASK_STATE_WAIT;
	}
	/*
	for (int i = 0; i < SIMPLE_RR_MAX_TASKS; i++) {
		//if (tasks[i]->state == TASK_STATE_WAIT && tasks[i]->data == tcb->data) {
		if (tasks[i]->state == TASK_STATE_WAIT) {
			tasks[i]->state &= ~TASK_STATE_WAIT;
			tasks[i]->data = 0;
		}
	}
	*/
}
