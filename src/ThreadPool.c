#include <stdlib.h>
#include <pthread.h>
#include "Queue.c"

typedef struct Task {
	void (*function) (void*);
	void* arg;
}Task;

typedef struct ThreadPool {
	struct Queue *tasks;
	pthread_t *threads;
	int size, running;
	pthread_mutex_t mutex;
}ThreadPool;

void incRunning(ThreadPool* pool) {
	pthread_mutex_lock(&(pool->mutex));
	pool->running++;
	pthread_mutex_unlock(&(pool->mutex));
}

void decRunning(ThreadPool* pool) {
	pthread_mutex_lock(&(pool->mutex));
	pool->running--;
	pthread_mutex_unlock(&(pool->mutex));

}

void* threadLoop(void* arg) {
	Task *task;
	struct ThreadPool* pool = (struct ThreadPool*)arg;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	while(1) {
		DequeueElement(pool->tasks, (void**)&task);
		incRunning(pool);
		(void)task->function(task->arg);
		decRunning(pool);
	}
}

ThreadPool* CreateThreadPool(int poolSize) {
	int i;
	struct ThreadPool *pool = malloc(sizeof(ThreadPool));

	pool->threads = malloc(poolSize * sizeof(pthread_t));
	pool->tasks = createQueue(sizeof(Task*));
	pool->size = poolSize;
	pool->running = 0;

	pthread_mutex_init(&(pool->mutex), NULL );

	for(i=0; i<poolSize; i++) {
		pthread_create(&(pool->threads[i]), NULL, threadLoop, (void*)pool);
	}
	return pool;
}

void DestroyThreadPool(ThreadPool *pool) {
	int i;
	DestroyQueue(pool->tasks);
	for(i=0; i<pool->size; i++) {
		pthread_cancel(pool->threads[i]);
	}
}

void AddTask(ThreadPool* pool, Task* task) {
	Enqueue(pool->tasks, task);
}

void WaitCompletion(ThreadPool* pool) {
	while((!QueueIsEmpty(pool->tasks)) || (pool->running > 0)) {
		sleep(1);
	}
}
