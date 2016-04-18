#include "../src/Queue.c"
#include <string.h>

int main(int argc, char *argv[]) {
	char* names = argv[1];
	char* name, **name_p;
	Queue* queue;
	struct Linked_list *iterator;

	queue = createQueue(sizeof(char*));
	name = strtok(names, ",");
	while (name != NULL) {
		printf("enqueueing %s - %i\n", name, QueueSize(queue));
		Enqueue(queue, name);
		name = strtok(NULL, ",");
	}
	for(iterator = queue->front->next; iterator != NULL; iterator = iterator->next) {
		printf("element %s\n", (char*)(iterator->value));
	}
//	printf("front %s\n", (char*)((Linked_list*)(front(queue)))->value);
	while(!QueueIsEmpty(queue)) {
		printf("dequeued ");
		fflush(stdout);
		DequeueElement(queue, (void**)&name);
//		name = *name_p;
		printf("%s - %i\n", name, QueueSize(queue));
	}
}
