#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>
#include <stddef.h>

/* FPU stack frame. Since the FPU stacks additional registers on top of the current stack, as well as 
	 additional registers at the bottom, more registers have to be added to the original stack frame. 
	 Registers s0-s15, fpscr, and rempty are automatically stacked by the CPU on top of the current stack. 
	 The FPU also has additional registers s16-s31 and they are included in the stack since lazy stacking is disabled,
	 and all of the tasks are assumed to be using the FPU. 
	 
	 NOTE: Lazy stacking is disabled due to it stacking and unstacking the incorrect amount of registers during a context switch,
				 causing hard faults. */
typedef struct s_StackFrameFPU {
	volatile uint32_t s16;
	volatile uint32_t s17;
	volatile uint32_t s18;
	volatile uint32_t s19;
	volatile uint32_t s20;
	volatile uint32_t s21;
	volatile uint32_t s22;
	volatile uint32_t s23;
	volatile uint32_t s24;
	volatile uint32_t s25;
	volatile uint32_t s26;
	volatile uint32_t s27;
	volatile uint32_t s28;
	volatile uint32_t s29;
	volatile uint32_t s30;
	volatile uint32_t s31;
	
	volatile uint32_t r4;
	volatile uint32_t r5;
	volatile uint32_t r6;
	volatile uint32_t r7;
	volatile uint32_t r8;
	volatile uint32_t r9;
	volatile uint32_t r10;
	volatile uint32_t r11;
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr;
	volatile uint32_t pc;
	volatile uint32_t psr;
	volatile uint32_t s0;
	volatile uint32_t s1;
	volatile uint32_t s2;
	volatile uint32_t s3;
	volatile uint32_t s4;
	volatile uint32_t s5;
	volatile uint32_t s6;
	volatile uint32_t s7;
	volatile uint32_t s8;
	volatile uint32_t s9;
	volatile uint32_t s10;
	volatile uint32_t s11;
	volatile uint32_t s12;
	volatile uint32_t s13;
	volatile uint32_t s14;
	volatile uint32_t s15;
	volatile uint32_t fpscr;
	volatile uint32_t rempty;
} OS_StackFrameFPU_t;

/* Describes a single stack frame, as found at the top of the stack of a task
   that is not currently running.  Registers r0-r3, r12, lr, pc and psr are stacked
	 automatically by the CPU on entry to handler mode.  Registers r4-r11 are subsequently
	 stacked by the task switcher.  That's why the order is a bit weird. */
typedef struct s_StackFrame {
	volatile uint32_t r4;
	volatile uint32_t r5;
	volatile uint32_t r6;
	volatile uint32_t r7;
	volatile uint32_t r8;
	volatile uint32_t r9;
	volatile uint32_t r10;
	volatile uint32_t r11;
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr;
	volatile uint32_t pc;
	volatile uint32_t psr;
} OS_StackFrame_t;

typedef struct {
	/* Task stack pointer.  It's important that this is the first entry in the structure,
	   so that a simple double-dereference of a TCB pointer yields a stack pointer. */
	void * volatile sp;
	/* This field is intended to describe the state of the thread - whether it's yielding,
	   runnable, or whatever.  Only one bit of this field is currently defined (see the #define
	   below), so you can use the remaining 31 bits for anything you like. */
	uint32_t volatile state;
	/* The remaining fields are provided for expandability.  None of them have a dedicated
	   purpose, but their names might imply a use.  Feel free to use these fields for anything
	   you like. */
	uint32_t volatile priority;
	uint32_t volatile data;
} OS_TCB_t;

/* Constants that define bits in a thread's 'state' field. */
#define TASK_STATE_YIELD    (1UL << 0) // Bit zero is the 'yield' flag
#define TASK_STATE_SLEEP 		(1UL << 1)
#define TASK_STATE_WAIT 		(1UL << 2)

#endif /* _TASK_H_ */
