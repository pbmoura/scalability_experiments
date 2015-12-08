#include "../src/Queue.c"
#include <string.h>

int main(int argc, char *argv[]) {
	int num = atoi(argv[1]);
	char* names = argv[2];
	char* name;
	Queue* queue;

	queue = createQueue(num, sizeof(char*));
	name = strtok(names, ",");
	while (name != NULL) {
		printf("enqueueing %s\n", name);
		Enqueue(queue, name);
		name = strtok(NULL, ",");
	}
	printf("elements[0] %s\n", (char*)queue->elements);
	while(!QueueIsEmpty(queue)) {
		DequeueElement(queue, name);
		printf("dequeued %s\n", name);
	}
}
