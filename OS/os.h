#ifndef _OS_H_
#define _OS_H_

#include "task.h"
#include "../stack.h"
/********************/
/* Type definitions */
/********************/

/* A set of numeric constants giving the appropriate SVC numbers for various callbacks. 
   If this list doesn't match the SVC dispatch table in os_asm.s, BIG TROUBLE will ensue. */
enum OS_SVC_e {
	OS_SVC_ENABLE_SYSTICK=0x00,
	OS_SVC_ADD_TASK,
	OS_SVC_EXIT,
	OS_SVC_YIELD,
	OS_SVC_SCHEDULE,
	OS_SVC_WAIT,
	OS_SVC_NOTIFY,
	SVC_INTERRUPT_TEST
};

/* A structure to hold callbacks for a scheduler, plus a 'preemptive' flag */
typedef struct {
	uint_fast8_t preemptive;
	OS_TCB_t const * (* scheduler_callback)(void);
	void (* addtask_callback)(OS_TCB_t * const newTask);
	void (* taskexit_callback)(OS_TCB_t * const task);
	void (* wait_callback)(void* const reason, uint32_t value);
	void (* notify_callback)(void* const reason);
	void (* ISR_notify_callback)(void* const reason);
} OS_Scheduler_t;

/***************************/
/* OS management functions */
/***************************/

/* Initialises the OS.  Must be called before OS_start().  The argument is a pointer to an
   OS_Scheduler_t structure (see above). */
void OS_init(OS_Scheduler_t const * scheduler);

/* Starts the OS kernel.  Never returns. */
void OS_start(void);

/* Returns a pointer to the TCB of the currently running task. */
OS_TCB_t * OS_currentTCB(void);

/* Returns the number of elapsed systicks since the last reboot (modulo 2^32). */
uint32_t OS_elapsedTicks(void);

/******************************************/
/* Task creation and management functions */
/******************************************/

/* Initialises a task control block (TCB) and its associated stack.  The stack is prepared
   with a frame such that when this TCB is first used in a context switch, the given function will be
   executed.  If and when the function exits, a SVC call will be issued to kill the task, and a callback
   will be executed.
   The first argument is a pointer to a TCB structure to initialise.
   The second argument is a pointer to the TOP OF a region of memory to be used as a stack (stacks are full descending).
     Note that the stack MUST be 8-byte aligned.  This means if (for example) malloc() is used to create a stack,
     the result must be checked for alignment, and then the stack size must be added to the pointer for passing
     to this function.
   The third argument is a pointer to the function that the task should execute.
   The fourth argument is a void pointer to data that the task should receive. */
void OS_initialiseTCB(OS_TCB_t * TCB, uint32_t * const stack, void (* const func)(void const * const), void const * const data);

/* SVC delegate to add a task. */
void __svc(OS_SVC_ADD_TASK) OS_addTask(OS_TCB_t const * const);

/************************/
/* Scheduling functions */
/************************/

/* SVC delegate to yield the current task. */
void __svc(OS_SVC_YIELD) OS_yield(void);

/* SVC delegate to set the current task to wait. */
void __svc(OS_SVC_WAIT) OS_wait(void* reason, uint32_t value);

/* SVC delegate for notifying a task. */
void __svc(OS_SVC_NOTIFY) OS_notify(void* reason);

/* ISR deferred nofity. */
void  OS_ISR_notify(void* reason);

/****************/
/* Declarations */
/****************/

/* Idle task TCB */
extern OS_TCB_t const * const OS_idleTCB_p;

/* Check value return function. */
uint32_t OS_checkValue(void);

/* Stack frame for the ISR return function. */
stack_t* OS_get_pending_ISR_notify(void);

#endif /* _OS_H_ */

