#include <stdio.h>
#include "../src/HashTable.c"

void print_table(HashTable* table) {
        int i;
        Linked_list *list;
        HashItem *item;
        printf("### start hash table ###");
        for(i=0; i<table->size; i++) {
                printf("\n#%i\n", i);
                list = table->items[i];
                while(list != NULL) {
                        item = (HashItem*)(list->value);
                        printf("  (%lu, %d)", item->key, *((int*)item->value));
                        iterate(&list);
                }
        }
        printf("\n### end hash table ###\n");
}


int main(int argc, char* argv[]) {
	HashTable* hashtable = createHashTable(10);
	int i;
	int* o;
	for(i=0; i<250; i++) {
		o = malloc(sizeof(int));
		*o = i+1000;
		put(hashtable, i, o);
	}
	print_table(hashtable);
	o = get(hashtable, 0);
	printf("%d\n", *o);
	o = get(hashtable, 56);
	printf("%d\n", *o);
	destroyHashTable(hashtable);
}
	
