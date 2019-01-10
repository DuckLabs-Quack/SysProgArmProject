#ifndef MUTEX_H
#define MUTEX_H

#include <stdint.h>
#include "os_internal.h"
#include "linkedList.h"
//#include "waitList.h"

typedef struct {
	OS_TCB_t* tcb;
	uint32_t counter;
	linked_list_t waitlist;
	//wait_list_t waitlist;
} OS_mutex_t;

void mutex_init(OS_mutex_t* mutex);
void OS_mutex_acquire(OS_mutex_t* mutex);
void OS_mutex_release(OS_mutex_t* mutex);

#endif /* MUTEX_H */
