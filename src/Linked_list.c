#include <stdio.h>


 typedef struct Linked_list {
	char* name;
	struct Linked_list *next;
}Linked_list;


Linked_list* createList(char* name, Linked_list* next) {
	Linked_list* list = malloc(sizeof(Linked_list));
	list->name = name;
	list->next = next;
	return list;
}

Linked_list* insertAfter(char* name, Linked_list* list) {
	Linked_list* node = createList(name, list->next);
	list->next = node;
	return node;
}

Linked_list* createCircularList(char* name) {
	Linked_list* list = createList(name, NULL);
	list->next = list;
	return list;
}

void iterate(Linked_list* list) {
	list = list->next;
}

Linked_list* removeNext(Linked_list* list) {
	Linked_list *node = list->next;
	if (node == list)
		list = NULL;
	else
		list->next = node->next;
	return node;
}
