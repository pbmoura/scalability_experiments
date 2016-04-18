#include "../src/Queue.c"

int main(int argc, char *argv[]) {
	Queue *queue;
	int n[] = {1, 2, 3, 4};
	int* i, j;

	queue = createQueue(sizeof(int*));
	i = n;
	for(j = 0; j<4; j++) {
		printf("enqueueing %i %li\n", *i, i);
		Enqueue(queue, i++);
	}
	//printf("elements %i %li\n", **((int**)(queue->elements)), *((int**)(queue->elements)));
	while(!QueueIsEmpty(queue)) {
		DequeueElement(queue, &i);
		printf("got %i\n", *i);
	}
}
