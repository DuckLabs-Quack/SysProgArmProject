#include "circularBuffer.h"

void queue_init(queue_t* queue) {
	queue->insert = 0;
	queue->retrieve = 0;
	queue->isFull = 0;
	mutex_init(&queue->mutex);
}

int queue_put(queue_t* queue, int item) {
	
	OS_mutex_acquire(&queue->mutex);
	
	int status = 0;
	
	if (!queue->isFull) {
		queue->data[queue->insert] = item;
		queue->insert = (queue->insert + 1) % 10;

		if (queue->insert == queue->retrieve) {
			queue->isFull = 1;
		}
		status = 1;
	}
	
	OS_mutex_release(&queue->mutex);
	return status;
}

int queue_get(queue_t* queue, int* item) {
	
	OS_mutex_acquire(&queue->mutex);
	
	int status = 0;
	
	if (queue->insert == queue->retrieve && !queue->isFull) {
		status = 0;
		*item = 0;
	} 
	else {
		queue->isFull = 0;
		int value = queue->data[queue->retrieve];
		queue->retrieve = (queue->retrieve + 1) % 10;
	
		*item = value;
		status = 1;
	}
	
	OS_mutex_release(&queue->mutex);
	return status;
}
