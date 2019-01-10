#ifndef STACK_H
#define STACK_H

#include <stdint.h>

// LIFO stack structure
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