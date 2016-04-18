#include "../src/Queue.c"

int main(int argc, char *argv[]) {
	Queue *queue;
	int n, i;

	queue = createQueue(sizeof(int));
	for(n = 1; n<4; n++) {
		printf("enqueueing %i\n", n);
		Enqueue(queue, n);
	}
	//printf("elements %i %li\n", *((int*)(queue->elements)), ((int*)(queue->elements)));
	while(!QueueIsEmpty(queue)) {
		DequeueElement(queue, &i);
		printf("got %i\n", i);
	}

	for(n = 1; n<4; n++) {
		printf("enqueueing %i\n", n);
		Enqueue(queue, n);
	}
	while(!QueueIsEmpty(queue)) {
		DequeueElement(queue, &i);
		printf("got %i\n", i);
	}
}
