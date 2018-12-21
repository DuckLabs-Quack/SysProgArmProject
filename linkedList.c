#include "linkedList.h"
#include <stdlib.h>

void linked_list_init(linked_list_t* linked_list) {
    // Initialise the linked_list
    linked_list->head = NULL;
}

void* linked_list_remove(linked_list_t* linked_list) {
    // Return the head of the list of blocks
    // Update the head pointer
    linked_list_element_t* oldHead = linked_list->head;
		if (oldHead == NULL) {
			return NULL;
		}
    linked_list->head = oldHead->next;
		void* item = oldHead->item;
		free(oldHead);
    return item;
}

void linked_list_add(linked_list_t* linked_list, void* newItem) {
		linked_list_element_t* newElement = malloc(sizeof(linked_list_element_t));
		newElement->item = newItem;
	  newElement->next = linked_list->head;
		// Add the new item to the head of the list
		linked_list->head = newElement;
    
}

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
			// Add the new item to the end of the list
			currentElement->next = newElement;
	}
}