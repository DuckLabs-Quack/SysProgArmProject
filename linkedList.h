#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stddef.h>

/* Linked list data structure (Singly linked list).*/
typedef struct linked_list_element linked_list_element_t;

struct linked_list_element {
	void* item;
	linked_list_element_t* next;
};


typedef struct {
	linked_list_element_t* head;
} linked_list_t;


void linked_list_init(linked_list_t* linked_list);
void* linked_list_remove(linked_list_t* linked_list);
void linked_list_append(linked_list_t* linked_list, void* item);

#endif /* LINKEDLIST_H */