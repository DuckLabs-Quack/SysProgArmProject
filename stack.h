#ifndef STACK_H
#define STACK_H

#include <stdint.h>
#include "fixedPri.h"

//typedef char waiting_array_t[FIX_PRIO_MAX_TASKS];

// LIFO stack structure
typedef struct {
	void* list[FIX_PRIO_MAX_TASKS];
	uint32_t counter;
	uint32_t maxSize;
} stack_t;

void stack_init(stack_t* stack);
int stack_is_full(stack_t* stack);
int stack_is_empty(stack_t* stack);
int stack_push(stack_t* stack, void* task);
void* stack_pop(stack_t* stack);

#endif /* STACK_H */