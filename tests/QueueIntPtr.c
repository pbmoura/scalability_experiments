#include "../src/Queue.c"

int main(int argc, char *argv[]) {
	Queue *queue;
	int n[] = {1, 2, 3, 4};
	int* i, j;

	queue = createQueue(5, sizeof(int*));
	i = n;
	for(j = 0; j<4; j++) {
		printf("enqueueing %i\n", *i);
		Enqueue(queue, i++);
	}
	printf("elements %i\n", **((int**)(queue->elements)));
	for(j = 0; j<4; j++) {
		DequeueElement(queue, &i);
		printf("got %i\n", *i);
	}
}
