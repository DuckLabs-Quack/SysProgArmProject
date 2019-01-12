#include "circularBuffer.h"

/* Circular buffer data structure.
	 Used for sending data (floats) between tasks i.e. task communication. 
	 Has a mutex to prevent simultaneous reading/writing between tasks. */

/* Initialisation function. */
void queue_init(queue_t* queue) {
	queue->insert = 0;
	queue->retrieve = 0;
	queue->isFull = 0;
	mutex_init(&queue->mutex);
}

/* Puts an item into the buffer (queue). */
int queue_put(queue_t* queue, float item) {
	
	OS_mutex_acquire(&queue->mutex);
	
	/* If the status is 1 then the item has successfully been put into the buffer. */
	int status = 0;
	
	if (!queue->isFull) {
		/* Puts the item into the buffer at the next available space and increment the variable 'insert' that tracks the location 
			 of where the next item will be put. Insert will loop back to value 0 when it goes above value 10. */
		queue->data[queue->insert] = item;
		queue->insert = (queue->insert + 1) % 10;
		
		/* When both retrieve (variable that tracks how many items have been removed from the buffer) 
			 and insert are the same, the buffer is full and will not allow any more items to be inserted. 
			 
			 e.g. Putting 10 times and not getting any items will cause insert to loop back to value 0 and match the retrieve value. */
		if (queue->insert == queue->retrieve) {
			queue->isFull = 1;
		}
		status = 1;
	}
	
	OS_mutex_release(&queue->mutex);
	return status;
}

/* Gets an item from the buffer (queue). */
int queue_get(queue_t* queue, float* item) {
	
	OS_mutex_acquire(&queue->mutex);
	
	/* If the status is 1 then the item has successfully been taken out of the buffer. */
	int status = 0;
	
	/* In the case of an empty buffer */
	if (queue->insert == queue->retrieve && !queue->isFull) {
		status = 0;
		*item = 0;
	} 
	else {
		queue->isFull = 0;
		
		/* Gets the item from the buffer at the current taken space and increment the variable 'retrieve' that tracks 
			 how many items have been removed from the buffer. Retrieve will loop back to value 0 when it goes above value 10. */	
		float value = queue->data[queue->retrieve];
		queue->retrieve = (queue->retrieve + 1) % 10;
	
		*item = value;
		status = 1;
	}
	
	OS_mutex_release(&queue->mutex);
	return status;
}
