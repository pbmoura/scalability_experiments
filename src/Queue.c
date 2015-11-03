#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
/*Queue has five properties. capacity stands for the maximum number of elements Queue can hold.
 *   Size stands for the current size of the Queue and elements is the array of elements. front is the
 *    index of first element (the index at which we remove the element) and rear is the index of last element
 *     (the index at which we insert the element) */
typedef struct Queue
{
        int capacity;
        int size;
        int front;
        int rear;
        int *elements;

	pthread_mutex_t mutex_front, mutex_rear;
	sem_t *sem;
}Queue;
/* crateQueue function takes argument the maximum number of elements the Queue can hold, creates
 *    a Queue according to it and returns a pointer to the Queue. */
Queue * createQueue(int maxElements)
{
	char *sem_name;
        /* Create a Queue */
        Queue *Q;
        Q = (Queue *)malloc(sizeof(Queue));
        /* Initialise its properties */
        Q->elements = (int *)malloc(sizeof(int)*maxElements);
        Q->size = 0;
        Q->capacity = maxElements;
        Q->front = 0;
        Q->rear = -1;

	sem_name = malloc(23);
	sprintf(sem_name, "/s%ld", (long)Q);

	pthread_mutex_init(&(Q->mutex_front), NULL);
	pthread_mutex_init(&(Q->mutex_rear), NULL);

	Q->sem = sem_open(sem_name, O_CREAT | O_EXCL, 0644, 0);
	sem_unlink(sem_name);

        /* Return the pointer */
        return Q;
}
void Dequeue(Queue *Q)
{
        /* If Queue size is zero then it is empty. So we cannot pop */
        if(Q->size==0)
        {
                printf("Queue is Empty\n");
                return;
        }
        // Removing an element is equivalent to incrementing index of front by one 
        else
        {
                Q->size--;
                Q->front++;
                /* As we fill elements in circular fashion */
                if(Q->front==Q->capacity)
                {
                        Q->front=0;
                }
        }
        return;
}
int front(Queue *Q)
{
        if(Q->size==0)
        {
                printf("Queue is Empty\n");
                exit(0);
        }
        /* Return the element which is at the front*/
        return Q->elements[Q->front];
}
void DequeueElement(Queue *Q, int *element)
{
	sem_wait(Q->sem);
	pthread_mutex_lock(&(Q->mutex_front));
	*element = front(Q);
	Dequeue(Q);
	pthread_mutex_unlock(&(Q->mutex_front));
}
void Enqueue(Queue *Q,int element)
{
        /* If the Queue is full, we cannot push an element into it as there is no space for it.*/
        if(Q->size == Q->capacity)
        {
                printf("Queue is Full\n");
        }
        else
        {
		pthread_mutex_lock(&(Q->mutex_rear));
                Q->size++;
                Q->rear = Q->rear + 1;
                /* As we fill the queue in circular fashion */
                if(Q->rear == Q->capacity)
                {
                        Q->rear = 0;
                }
                /* Insert the element in its rear side */ 
                Q->elements[Q->rear] = element;
		pthread_mutex_unlock(&(Q->mutex_rear));
		sem_post(Q->sem);
        }
        return;
}
int QueueIsEmpty(Queue *Q)
{
	return Q->size == 0;
}

