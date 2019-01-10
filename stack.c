#include "stack.h"

void stack_init(stack_t* stack) {
	stack->counter = 0;
	stack->maxSize = 10;
}

int stack_is_full(stack_t* stack) {
	// Checks if the wait list is full
	if (stack->counter == stack->maxSize) {
		return 1;
	}
	
	return 0;
}

int stack_is_empty(stack_t* stack) {
	// Checks if the wait list is empty
	if (stack->counter == 0) {
		return 1;
	}
	
	return 0;
}

int stack_push(stack_t* stack, void* waitlist) {
	
	// Check if wait list is full before pushing
	if (stack_is_full(stack)) {
		return 1;
	}
	
	stack->list[stack->counter++] = waitlist;
	
	return 0;
}

void* stack_pop(stack_t* stack) {
	
	// Check if stack is empty before popping
	if (stack_is_empty(stack)) {
		return 0;
	}
	
	void* task = stack->list[--(stack->counter)];
	
	return task;
}