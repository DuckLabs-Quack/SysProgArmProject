#include "linkedList.h"
#include <stdlib.h>

/* Linked list data structure (Singly linked list).
	 Used to store the tasks which are currently waiting. */
		
/* Initialisation function. */
void linked_list_init(linked_list_t* linked_list) {
    linked_list->head = NULL;
}

/* Returns the head of the linked list and updates the head pointer.
	 When used for the wait list, the item returned is a task. */
void* linked_list_remove(linked_list_t* linked_list) {
    linked_list_element_t* oldHead = linked_list->head;
		if (oldHead == NULL) {
			return NULL;
		}
    linked_list->head = oldHead->next;
		void* item = oldHead->item;
		free(oldHead);
    return item;
}

/* Appends the new item to the end of the list. */
void linked_list_append(linked_list_t* linked_list, void* newItem) {
		linked_list_element_t* newElement = malloc(sizeof(linked_list_element_t));
		newElement->item = newItem;
	  newElement->next = NULL;
	
		if (linked_list->head == NULL) {
			linked_list->head = newElement;
		} else {
			linked_list_element_t* currentElement = linked_list->head;
		
			while (currentElement->next != NULL) {
				currentElement = currentElement->next;
			}
			
			currentElement->next = newElement;
		}
}