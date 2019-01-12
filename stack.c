#include "stack.h"

/* LIFO stack frame data structure.
	 Used for when interrupts are called and want to notify the OS. 
	 Stores the current task waiting list when an interrupt occurs so it can be restored after the interrupt has finished. 
		
	 This is to protect the current waiting list from being changed on an interrupt as notify will remove the head of the wait list. 
	 Without this, the task will resume after an interrupt, finishing and notifying the OS with the head pointer 
	 being changed by the interrupt to point to random memory that is no longer in use. */
	
/* Initialisation function */
void stack_init(stack_t* stack) {
	stack->counter = 0;
	stack->maxSize = 10;
}

/* Checks if the stack is full */
int stack_is_full(stack_t* stack) {
	
	if (stack->counter == stack->maxSize) {
		return 1;
	}
	
	return 0;
}

/* Checks if the stack is empty */
int stack_is_empty(stack_t* stack) {
	if (stack->counter == 0) {
		return 1;
	}
	
	return 0;
}

/* Push the wait list onto the stack */
int stack_push(stack_t* stack, void* waitlist) {
	
	/* Check if the stack is full before pushing */
	/* Returns 0 if it failed, and 1 if it succeeded */
	if (stack_is_full(stack)) {
		return 0;
	}
	
	stack->list[stack->counter++] = waitlist;
	
	return 1;
}

/* Pop the wait list from the stack */
void* stack_pop(stack_t* stack) {
	
	/* Check if the stack is empty before popping */
	/* Returns 0 if it failed, and the item if it succeeded */
	if (stack_is_empty(stack)) {
		return 0;
	}
	
	void* task = stack->list[--(stack->counter)];
	
	return task;
}