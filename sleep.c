#include <stdint.h>
#include "sleep.h"
#include "os_internal.h"

/* Sets the current task to sleep for a time in milliseconds. The current task will wake up after the elapsed time.
	 If another task is running when it wakes up, the task will be put into the wait list and notified when it can run. */
void OS_sleep(uint32_t time) {
	OS_TCB_t* task_pointer = OS_currentTCB();
	task_pointer->data = time + OS_elapsedTicks();
	task_pointer->state = TASK_STATE_SLEEP;
	
	OS_yield();
} 