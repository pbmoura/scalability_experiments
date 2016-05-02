#include <signal.h>
#include "common.c"
#include "ThreadPool.c"

char* load_balancer;
unsigned long returned = 0;
sem_t *sem, *sem_result;

typedef struct req_arg {
	unsigned long counter, delay;
}req_arg;

void request(void* arg) {
	unsigned long delay = ((struct req_arg*)arg)->delay;
	unsigned long counter = ((struct req_arg*)arg)->counter;
	long st, duration;
	int sock;
	unsigned long reply;

	fprintf(stderr, "%lu running request %lu after %lu\n", time_millis(), counter, delay);

	st = time_millis();
	sock = connectTo(load_balancer, PORT_LB, "to lb");
	write(sock, &counter, sizeof(counter));
	read(sock, &reply, sizeof(reply));
	duration = time_millis() - st;
	close(sock);
	sem_wait(sem_result);
	fprintf(stdout, "%ld %lu %ld %lu\n", st, delay, duration, reply);
	fflush(stdout);
	sem_post(sem_result);
	sem_wait(sem);
	returned++;
	sem_post(sem);
	free(arg);
}

int main(int argc, char* argv[]) {
	load_balancer = argv[1];
	char* file_name = argv[2];
	int i;
	FILE *fd;
	unsigned long delay;
	unsigned long counter = 0;
	struct req_arg *args;
	struct ThreadPool *pool;
	struct Task *aTask;

	//signal(SIGPIPE, SIG_IGN);

	pool = CreateThreadPool(2000);

	fd = fopen(file_name,"r");
	if (fd == NULL) {
		fprintf(stderr, "Error opening file %s\n", file_name);
		exit(1);
	}

	sem = createsemaphore("/sem_trace", 1);
	sem_result = createsemaphore("/sem_result", 1);

	while(EOF != fscanf(fd, "%lu\n", &delay)) {
		usleep(delay);
		args = malloc(sizeof(struct req_arg));
		args->counter = counter++;
		args->delay = delay;
		aTask = malloc(sizeof(Task));
		aTask->function = *request;
		aTask->arg = args;
		AddTask(pool, aTask);
	}
	fprintf(stderr, "%lu workload finished\n", time_millis());
	while (returned < counter) {
		fprintf(stderr, "%lu < %lu\n", returned, counter);
		sleep(1);
	}
	fprintf(stderr, "finished with %lu of %lu\n", returned, counter);
	DestroyThreadPool(pool);
}
