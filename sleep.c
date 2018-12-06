#include <stdint.h>
#include "sleep.h"
#include "os_internal.h"

void OS_sleep(uint32_t time) {
	OS_TCB_t* task_pointer = OS_currentTCB();
	task_pointer->data = time + OS_elapsedTicks();
	task_pointer->state = TASK_STATE_SLEEP;
	
	OS_yield();
} 