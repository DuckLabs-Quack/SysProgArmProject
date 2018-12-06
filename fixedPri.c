#include "fixedPri.h"
#include "stm32f4xx.h"

/* Prototypes (functions are static, so these aren't in the header file) */
static OS_TCB_t const* fixedPri_scheduler(void);
static void fixedPri_addTask(OS_TCB_t* const tcb);
static void fixedPri_taskExit(OS_TCB_t* const tcb);
static void fixedPri_wait(OS_TCB_t* const tcb);
static void fixedPri_notify(OS_TCB_t* const tcb);

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
	static int i = 0;
	// Clear the yield flag if it's set - we simply don't care
	OS_currentTCB()->state &= ~TASK_STATE_YIELD;
	for (int j = 1; j <= SIMPLE_RR_MAX_TASKS; j++) {
		i = (i + 1) % SIMPLE_RR_MAX_TASKS;
		if (tasks[i] != 0 && tasks[i]->state != TASK_STATE_SLEEP && tasks[i]->state != TASK_STATE_WAIT) {
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
static void fixedPri_wait(OS_TCB_t* const tcb) {
	OS_TCB_t* current_task = OS_currentTCB();
	current_task->data = tcb->data;
	current_task->state = TASK_STATE_WAIT;
	
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/* Notify */
static void fixedPri_notify(OS_TCB_t* const tcb) {
	for (int i = 0; i < SIMPLE_RR_MAX_TASKS; i++) {
		if (tasks[i]->state == TASK_STATE_WAIT && tasks[i]->data == tcb->data) {
			tasks[i]->state &= ~TASK_STATE_WAIT;
			tasks[i]->data = 0;
		}
	}
}
