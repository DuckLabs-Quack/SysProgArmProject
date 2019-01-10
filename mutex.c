#include "mutex.h"
#include "stm32f4xx.h"

void mutex_init(OS_mutex_t* mutex) {
	mutex->counter = 0;
	mutex->tcb = 0;
	linked_list_init(&mutex->waitlist);
}

void OS_mutex_acquire(OS_mutex_t* mutex) {
	uint32_t loaded_value;
	int locked_with_own_tcb = 0;
	
	do {
		loaded_value = __LDREXW((uint32_t*) &(mutex->tcb));
		
		OS_TCB_t* currentTCB = OS_currentTCB();

		if (loaded_value == 0) {
			uint32_t store_failed = __STREXW((uint32_t) currentTCB, (uint32_t*) &(mutex->tcb));
			locked_with_own_tcb = !store_failed;
		} else  if (loaded_value == (uint32_t) currentTCB) {
			locked_with_own_tcb = 1;
		} else {
			OS_wait(&mutex->waitlist, OS_checkValue());
		}
		
	} while (!locked_with_own_tcb);
	__DMB();
	
	mutex->counter++;
}

void OS_mutex_release(OS_mutex_t* mutex) {
	mutex->counter--;
	
	if (mutex->counter == 0) {
		mutex->tcb = 0;
		OS_notify(&mutex->waitlist);
	}
}
