#ifndef __circularBuffer_h__
#define __circularBuffer_h__

#include "mutex.h"

/* Circular buffer data structure definition. 
	 The size of the buffer is set to 10 by default. */
typedef struct {
	float data[10];
	int insert;
	int retrieve;
	int isFull;
	OS_mutex_t mutex;
} queue_t;

void queue_init(queue_t* queue);
int queue_put(queue_t* queue, float item);
int queue_get(queue_t* queue, float* item);

#endif /* __circularBuffer_h__ */