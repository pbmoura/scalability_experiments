#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

typedef struct Queue
{
        int capacity;
        int size;
        int front;
        int rear;
        long *elements;

	pthread_mutex_t mutex;
	sem_t *sem;
}Queue;

Queue * createQueue(int maxElements)
{
	char *sem_name;
        Queue *Q;
        Q = (Queue *)malloc(sizeof(Queue));
        Q->elements = (long *)malloc(sizeof(long)*maxElements);
        Q->size = 0;
        Q->capacity = maxElements;
        Q->front = 0;
        Q->rear = -1;

	sem_name = malloc(23);
	sprintf(sem_name, "/s%ld", (long)Q);

	pthread_mutex_init(&(Q->mutex), NULL);

	Q->sem = sem_open(sem_name, O_CREAT | O_EXCL, 0644, 0);
	sem_unlink(sem_name);

        return Q;
}
void Dequeue(Queue *Q)
{
        if(Q->size==0)
        {
                printf("Queue is Empty\n");
                return;
        }
        else
        {
                Q->size--;
                Q->front++;
                if(Q->front==Q->capacity)
                {
                        Q->front=0;
                }
        }
        return;
}
long front(Queue *Q)
{
        if(Q->size==0)
        {
                printf("Queue is Empty\n");
                exit(0);
        }
        return Q->elements[Q->front];
}
void DequeueElement(Queue *Q, long *element)
{
	sem_wait(Q->sem);
	pthread_mutex_lock(&(Q->mutex));
	*element = front(Q);
	Dequeue(Q);
	pthread_mutex_unlock(&(Q->mutex));
}
void Enqueue(Queue *Q, long element)
{
        if(Q->size == Q->capacity)
        {
                printf("Queue is Full\n");
        }
        else
        {
		pthread_mutex_lock(&(Q->mutex));
                Q->size++;
                Q->rear = Q->rear + 1;
                if(Q->rear == Q->capacity)
                {
                        Q->rear = 0;
                }
                Q->elements[Q->rear] = element;
		pthread_mutex_unlock(&(Q->mutex));
		sem_post(Q->sem);
        }
        return;
}
int QueueIsEmpty(Queue *Q)
{
	return Q->size == 0;
}
