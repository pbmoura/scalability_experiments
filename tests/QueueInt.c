#include "../src/Queue.c"

int main(int argc, char *argv[]) {
	Queue *queue;
	int n;

	queue = createQueue(5, sizeof(int));
	for(n = 1; n<4; n++) {
		printf("enqueueing %i\n", n);
		Enqueue(queue, n);
	}
	printf("elements %i\n", *((int*)(queue->elements)));
	for(n = 1; n<4; n++) {
		DequeueElement(queue, n);
		printf("got %i\n", n);
	}
}
