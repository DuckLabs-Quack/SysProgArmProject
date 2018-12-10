#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stddef.h>

typedef struct {
	void* head;
} linked_list_t;

void linked_list_init(linked_list_t* linked_list);
void* linked_list_allocate(linked_list_t* linked_list);
void linked_list_deallocate(linked_list_t* linked_list, void* item);

#define linked_list_add linked_list_deallocate

#endif /* LINKEDLIST_H */