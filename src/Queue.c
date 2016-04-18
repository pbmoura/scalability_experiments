#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<fcntl.h>

#include "Linked_list.c"

typedef struct Queue {
	struct Linked_list *front;
	struct Linked_list *rear;
	size_t element_size;

	pthread_mutex_t mutex;
	sem_t *sem;
} Queue;

Queue * createQueue(size_t elementSize) {
	char *sem_name;
	Queue *Q;
	Q = (Queue *) malloc(sizeof(Queue));
	Q->front = createList(NULL, NULL);
	Q->rear = Q->front;
	Q->element_size = elementSize;

	sem_name = malloc(23);
	sprintf(sem_name, "/s%ld", (long) Q);

	pthread_mutex_init(&(Q->mutex), NULL );

	Q->sem = sem_open(sem_name, O_CREAT | O_EXCL, 0644, 0);
	sem_unlink(sem_name);

	return Q;
}
void Dequeue(Queue *Q) {
	if (QueueIsEmpty(Q)) {
		fprintf(stderr, "ERROR: Dequeueing from empty queue\n");
		return;
	} else {
		Q->front->next = Q->front->next->next;
		if (Q->front->next == NULL)
			Q->rear = Q->front;

	}
	return;
}
void *front(Queue *Q) {
	if (QueueIsEmpty(Q)) {
		printf("ERROR: Reading from empty Queue\n");
		exit(0);
	}
	return Q->front->next->value;
}
void DequeueElement(Queue *Q, void** element) {
	sem_wait(Q->sem);
	pthread_mutex_lock(&(Q->mutex));
	*element = front(Q);
	Dequeue(Q);
	pthread_mutex_unlock(&(Q->mutex));
}
void Enqueue(Queue *Q, void* element) {
	pthread_mutex_lock(&(Q->mutex));
	insertAfter(element, Q->rear);
	Q->rear = Q->rear->next;
	pthread_mutex_unlock(&(Q->mutex));
	sem_post(Q->sem);
	return;
}
int QueueIsEmpty(Queue *Q) {
	return (Q->front == Q->rear);
	//return QueueSize(Q);
}
int QueueSize(Queue *Q) {
	int size;
	sem_getvalue(Q->sem, &size);
	return size;
}
