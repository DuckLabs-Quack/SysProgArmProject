#ifndef __circularBuffer_h__
#define __circularBuffer_h__

#include "mutex.h"

typedef struct {
	int data[10];
	int insert;
	int retrieve;
	int isFull;
	OS_mutex_t mutex;
} queue_t;

void queue_init(queue_t* queue);
int queue_put(queue_t* queue, int item);
int queue_get(queue_t* queue, int* item);

#endif /* __circularBuffer_h__ */