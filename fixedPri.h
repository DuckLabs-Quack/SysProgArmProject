#ifndef __fixedPri_h__
#define __fixedPri_h__

#include "os.h"
#include "linkedList.h"

/* Adjust this constant to change how many tasks the scheduler can handle.
	 NOTE: The scheduler is designed for a small number of tasks. 
	 Increasing it to a large number will cause slower performance of tasks. */
#define FIX_PRIO_MAX_TASKS 8

extern OS_Scheduler_t const fixedPriScheduler;

#endif /* __fixedPri_h__ */