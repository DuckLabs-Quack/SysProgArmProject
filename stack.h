#ifndef STACK_H
#define STACK_H

#include <stdint.h>

/* LIFO stack frame data structure definition. 
	 Size of the stack set to 10 by default. 
	 Having a dynamic size will be unsafe and will have the same problems as before with the wait list and interrupts that notify. */
typedef struct {
	void* list[10];
	uint32_t counter;
	uint32_t maxSize;
} stack_t;

void stack_init(stack_t* stack);
int stack_is_full(stack_t* stack);
int stack_is_empty(stack_t* stack);
int stack_push(stack_t* stack, void* waitlist);
void* stack_pop(stack_t* stack);

#endif /* STACK_H */