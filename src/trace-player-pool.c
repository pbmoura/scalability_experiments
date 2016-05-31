#include <signal.h>
#include "common.c"
#include "Queue.c"

#define NUM_THREADS 3000

char* load_balancer;
unsigned long returned = 0;
sem_t *sem, *sem_result;
Queue *queue;

typedef struct req_arg {
	unsigned long counter, delay;
}req_arg;

void* request(void* arg) {
	struct req_arg *args;
	long st, duration;
	int sock, queue_size;
	unsigned long reply;

	while(1) {
		DequeueElement(queue, (void**)&args);
		queue_size = QueueSize(queue);

		fprintf(stderr, "%lu running request %lu after %lu\n", time_millis(), args->counter, args->delay);
		fprintf(stderr, "queue size: %d\n", queue_size);

		st = time_millis();
		sock = connectTo(load_balancer, PORT_LB, "to lb");
		write(sock, &(args->counter), sizeof(args->counter));
		read(sock, &reply, sizeof(reply));
		duration = time_millis() - st;
		close(sock);
		sem_wait(sem_result);
		fprintf(stdout, "%ld %lu %ld %lu\n", st, args->delay, duration, reply);
		fflush(stdout);
		sem_post(sem_result);
		sem_wait(sem);
		returned++;
		sem_post(sem);
		free(args);
	}
}

int main(int argc, char* argv[]) {
	load_balancer = argv[1];
	char* file_name = argv[2];
	int i;
	FILE *fd;
	unsigned long delay;
	unsigned long counter = 0;
	struct req_arg *args;
	pthread_t t1;
	int cr_t;
	time_t t;
	//signal(SIGPIPE, SIG_IGN);
	
	fprintf(stderr, "%ld load balancer: '%s'\n", time_millis(), load_balancer);

	queue = createQueue(sizeof(args));

	fprintf(stderr, "%ld created queue\n", time_millis());
	sem = createsemaphore("/sem_trace", 1);
	sem_result = createsemaphore("/sem_result", 1);

	fprintf(stderr, "%ld created semaphores\n", time_millis());
	fd = fopen(file_name,"r");
	if (fd == NULL) {
		fprintf(stderr, "Error opening file %s\n", file_name);
		exit(1);
	}

	fprintf(stderr, "%ld trace file opened\n", time_millis());
	for (i=0; i<NUM_THREADS; i++) 
		if ((cr_t = pthread_create(&t1, NULL, request, NULL)) != 0)
			fprintf(stderr, "ERROR creating thread: %i\n", cr_t);
	fprintf(stderr, "%ld created threads\n", time_millis());
	//srand((unsigned) time(&t));	
	//usleep(rand()*10);
	while(EOF != fscanf(fd, "%lu\n", &delay)) {
		usleep(delay);
		args = malloc(sizeof(struct req_arg));
		args->counter = counter++;
		args->delay = delay;
		Enqueue(queue, (void*)args);
	}
	fprintf(stderr, "%lu workload finished\n", time_millis());
	while (returned < counter) {
		fprintf(stderr, "%lu < %lu\n", returned, counter);
		sleep(1);
	}
	fprintf(stderr, "finished with %lu of %lu\n", returned, counter);
}
