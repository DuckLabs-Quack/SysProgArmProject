#include "mutex.h"
#include "stm32f4xx.h"

/* Mutex functions.
	 Used to lock a task or a resource so that only one task can execute or have access to a resource.
	 After finishing whatever requires a mutex, it must be released so that other tasks can acquire it else
	 a deadlock will occur. */

/* Initialisation function */
void mutex_init(OS_mutex_t* mutex) {
	mutex->counter = 0;
	mutex->tcb = 0;
	linked_list_init(&mutex->waitlist);
}

/* Mutex acquire function.
	 If the mutex is already in use, the task will wait for the mutex until it has been released. */
void OS_mutex_acquire(OS_mutex_t* mutex) {
	uint32_t loaded_value;
	int locked_with_own_tcb = 0;
	
	do {
		/* Get the check value from the OS at this point. 
			 If the value increases past this point then an interrupt or preemption has occured. */
		uint32_t checkValue = OS_checkValue();
		
		/* Loads exclusively the value of the mutex to see if it contains a TCB. */
		loaded_value = __LDREXW((uint32_t*) &(mutex->tcb));
		
		OS_TCB_t* currentTCB = OS_currentTCB();
		
		if (loaded_value == 0) {
			/* If it is empty, then the mutex is free so try and store exclusively the current TCB into the mutex */
			uint32_t store_failed = __STREXW((uint32_t) currentTCB, (uint32_t*) &(mutex->tcb));
			
			/* If the store is successful (store_failed = 0) then the current task now has the mutex. 
				 If it failed (store_failed = 1) then it will loop again and try to lock again. */
			locked_with_own_tcb = !store_failed;
		} else  if (loaded_value == (uint32_t) currentTCB) {
			/* This is in the case of when the TCB already has the mutex, so it will do nothing and increment the mutex counter. */
			locked_with_own_tcb = 1;
		} else {
			/* The mutex is in already in use, so add the task to the wait list for the mutex. */
			OS_wait(&mutex->waitlist, checkValue);
		}
		
	} while (!locked_with_own_tcb);
	__DMB();
	
	mutex->counter++;
}

/* Mutex release function. */
void OS_mutex_release(OS_mutex_t* mutex) {
	mutex->counter--;
	
	/* When the mutex has been released, notify the waiting tasks. */
	if (mutex->counter == 0) {
		mutex->tcb = 0;
		OS_notify(&mutex->waitlist);
	}
}
