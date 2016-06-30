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
	fprintf(stderr, "%lu - %d threads running - inc / queue size %d\n", time_millis(), pool->running, QueueSize(pool->tasks));
	pthread_mutex_unlock(&(pool->mutex));
}

void decRunning(ThreadPool* pool) {
	pthread_mutex_lock(&(pool->mutex));
	pool->running--;
	//fprintf(stderr, "%d threads running - dec\n", pool->running);
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
	int i, ret;
	struct ThreadPool *pool = malloc(sizeof(ThreadPool));

	pool->threads = malloc(poolSize * sizeof(pthread_t));
	pool->tasks = createQueue(sizeof(Task*));
	pool->size = poolSize;
	pool->running = 0;

	pthread_mutex_init(&(pool->mutex), NULL );

	for(i=0; i<poolSize; i++) {
		ret = pthread_create(&(pool->threads[i]), NULL, threadLoop, (void*)pool);
		if (ret != 0) {
			//i--;
			fprintf(stderr, "ERROR creating thread %d: %d\n", i, ret);
		}
			
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

void AddTask(ThreadPool* pool, void (*function) (void*) fun, void* arg) {
	Task *task;
	task = malloc(sizeof(Task));
	task->function = fun;
	task->arg = arg;
	Enqueue(pool->tasks, task);
}

void WaitCompletion(ThreadPool* pool) {
	fprintf(stderr, "WaitCompletion: QueueIsEmpty %d - running %d\n", QueueIsEmpty(pool->tasks), pool->running);
	while((!QueueIsEmpty(pool->tasks)) || (pool->running > 0)) {
		fprintf(stderr, "WaitCompletion: QueueIsEmpty %d - running %d\n", QueueIsEmpty(pool->tasks), pool->running);
		sleep(1);
	}
}
