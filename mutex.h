#ifndef MUTEX_H
#define MUTEX_H

#endif /* MUTEX_H */

#include <stdint.h>
#include "os_internal.h"

typedef struct {
	OS_TCB_t* tcb;
	uint32_t counter;
} OS_mutex_t;

void init_mutex(OS_mutex_t* mutex);
void OS_mutex_acquire(OS_mutex_t* mutex);
void OS_mutex_release(OS_mutex_t* mutex);
