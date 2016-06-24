#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "Linked_list.c"

typedef struct HashItem {
	unsigned long key;
	void * value;
}HashItem;

typedef struct HashTable {
	struct Linked_list **items;
	int size;
	pthread_mutex_t mutex;
}HashTable;

long hashFunction(HashTable* table, unsigned long key) {
	return key % table->size;
}

HashTable* createHashTable(int size) {
	HashTable* table = malloc(sizeof(HashTable));
	table->items = malloc(size * sizeof(Linked_list*));
	memset(table->items, 0, sizeof(table->items));
	table->size = size;
	pthread_mutex_init(&(table->mutex), NULL);
	return table;
}

HashItem* createHashItem(unsigned long key, void* value) {
	HashItem* item = malloc(sizeof(HashItem));
	item->key = key;
	item->value = value;
	return item;
}

void put(HashTable* table, unsigned long key, void* value) {
	unsigned long hash = hashFunction(table, key);
	HashItem* item = createHashItem(key, value);
	pthread_mutex_lock(&(table->mutex));
	table->items[hash] = createList(item, table->items[hash]);
	pthread_mutex_unlock(&(table->mutex));
}

void* get(HashTable* table, unsigned long key) {
	unsigned long hash = hashFunction(table, key);
	Linked_list *list = malloc(sizeof(Linked_list));
	void* value;
	HashItem* item;
	//pthread_mutex_lock(&(table->mutex));
	list->next = table->items[hash];
	while (list->next != NULL) {
		item = (struct HashItem*)list->next->value;
		if (item->key == key) {
			value = item->value;
			removeNext(list);
			//pthread_mutex_unlock(&(table->mutex));
			return value;
		} else {
			iterate(&list);
		}
	}
	//pthread_mutex_unlock(&(table->mutex));
	return NULL;
}
	
void destroyHashTable(HashTable* table) {
	int i;
	for(i = 0; i < table->size; i++) 
		destroyList(table->items[i]);
	free(table->items);
	free(table);
}
