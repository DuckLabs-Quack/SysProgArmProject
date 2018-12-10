#ifndef __fixedPri_h__
#define __fixedPri_h__

#include "os.h"
#include "linkedList.h"

// How many tasks can this scheduler cope with?
#define SIMPLE_RR_MAX_TASKS 8

extern OS_Scheduler_t const fixedPriScheduler;

#endif /* __fixedPri_h__ */