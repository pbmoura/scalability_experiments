#include "../src/Queue.c"

int main(int argc, char *argv[]) {
	Queue *queue;
	int n, *i;

	queue = createQueue(5, sizeof(int));
	for(n = 1; n<4; n++) {
		printf("enqueueing %i\n", n);
		Enqueue(queue, n);
	}
	printf("elements %i %li\n", *((int*)(queue->elements)), ((int*)(queue->elements)));
	for(n = 1; n<4; n++) {
		DequeueElement(queue, &i);
		printf("got %i\n", *i);
	}
}
