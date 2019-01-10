#include "interrupt.h"
#include "mutex.h"
#include <stdio.h>

void _svc_interrupt_test(_OS_SVC_StackFrame_t* const stack) {
	printf("Interrupt start\r\n");
	OS_mutex_t* mutex = (void*) stack->r0;
	
	OS_ISR_notify(&(mutex->waitlist));
	printf("Interrupt end\r\n");
}