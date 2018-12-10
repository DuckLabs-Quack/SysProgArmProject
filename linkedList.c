#include "linkedList.h"

void linked_list_init(linked_list_t* linked_list) {
    // Initialise the linked_list
    linked_list->head = NULL;
}

void* linked_list_allocate(linked_list_t* linked_list) {
    // Return the head of the list of blocks
    // Update the head pointer
    void* oldHead = linked_list->head;
    linked_list->head = *((void**)oldHead);
    return oldHead;
    
}

void linked_list_deallocate(linked_list_t* linked_list, void* item) {
    // Add the new item to the head of the list
    void* oldHead = linked_list->head;
    linked_list->head = item;
    *((void**)item) = oldHead;
    
}