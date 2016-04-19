#include <stdio.h>
#include "../src/ThreadPool.c"
#include "../src/common.c"


void task_fun(void* arg) {
	int i;;
	char* msg = (char*)arg;


	printf("started %s at %lu\n", msg, time_millis());
//	for(i=0; i<100000000; i++);
	sleep(3);
	printf("finished %s at %lu\n", msg, time_millis());
}

int main(int argc, char* argv[]) {
	struct ThreadPool *pool;
	struct Task *task;

	pool = CreateThreadPool(2000);

	task = malloc(sizeof(Task));
	task->function = *task_fun;
	task->arg = &"a";

	AddTask(pool, task);

	task = malloc(sizeof(Task));
	task->function = *task_fun;
	task->arg = &"b";

	AddTask(pool, task);

	task = malloc(sizeof(Task));
	task->function = *task_fun;
	task->arg = &"c";

	AddTask(pool, task);

	task = malloc(sizeof(Task));
		task->function = *task_fun;
		task->arg = &"d";

		AddTask(pool, task);


//	sleep(5);

	WaitCompletion(pool);
	return 0;
}
