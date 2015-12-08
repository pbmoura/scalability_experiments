#include <stdio.h>


 typedef struct Linked_list {
	void* value;
	struct Linked_list *next;
}Linked_list;


Linked_list* createList(void* value, Linked_list* next) {
	Linked_list* list = malloc(sizeof(Linked_list));
	list->value = value;
	list->next = next;
	return list;
}

Linked_list* insertAfter(void* value, Linked_list* list) {
	Linked_list* node = createList(value, list->next);
	list->next = node;
	return node;
}

Linked_list* createCircularList(char* value) {
	Linked_list* list = createList(value, NULL);
	list->next = list;
	return list;
}

void iterate(Linked_list** list) {
	*list = (*list)->next;
}

Linked_list* removeNext(Linked_list* list) {
	Linked_list *node = list->next;
	if (node == list)
		list = NULL;
	else
		list->next = node->next;
	return node;
}

Linked_list* removeNode(char* value, Linked_list *list) {
	Linked_list *iterator, *node;
	iterator = list;
	while(iterator->next != list) {
		if(strcmp(iterator->next->value, value) == 0) {
			return removeNext(iterator);
		}
		iterate(&iterator);
	}
	return NULL;
}

void destroyList(Linked_list *list) {
	Linked_list *node;
	while(list != NULL) {
		node = list;
		iterate(&list);
		free(node);
	}
}
