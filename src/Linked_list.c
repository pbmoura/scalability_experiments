#include <stdio.h>


 typedef struct Linked_list {
	char* name;
	struct Linked_list *next;
}Linked_list;


Linked_list* createList(char* name) {
	Linked_list* list = malloc(sizeof(Linked_list));
	list->name = name;
	list->next = NULL;
	return list;
}

Linked_list* appendNode(char* name, Linked_list* next) {
	Linked_list* list = createList(name);
	list->next = next;
	return list;
}

Linked_list* insertNode(char* name, Linked_list* list) {
	Linked_list* node = appendNode(name, list->next);
	list->next = node;
	return node;
}

Linked_list* createCircularList(char* name) {
	Linked_list* list = createList(name);
	list->next = list;
	return list;
}

void iterate(Linked_list* list) {
	list = list->next;
}
