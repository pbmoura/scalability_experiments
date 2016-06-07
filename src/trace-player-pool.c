#include <signal.h>
#include "common.c"
#include "Queue.c"

#define NUM_THREADS 3000

char* load_balancer;
unsigned long returned = 0;
sem_t *sem_counter, *sem_result, *sem_request;
Queue *queue;
long now, last_request;

typedef struct req_arg {
	long counter, delay;
}req_arg;

void* request(void* arg) {
	struct req_arg *args;
	long st, duration;
	int sock, queue_size;
	unsigned long reply;

	sock = 0;
	while(1) {
		DequeueElement(queue, (void**)&args);
		sem_wait(sem_request);
		//t1 = time_micro();
		queue_size = QueueSize(queue);

		if(sock == 0)
			sock = connectTo(load_balancer, PORT_LB, "to lb");
		fprintf(stderr, "%lu running request %lu sleep %lu\n", time_millis(), args->counter, args->delay);
		//fprintf(stderr, "queue size: %d\n", queue_size);

		st = time_millis();
		now = time_micro();
		fprintf(stderr, "%lu delta %ld in request %lu\n", time_millis(), args->counter, (now-last_request));
		usleep(args->delay - (now - last_request));
		write(sock, &(args->counter), sizeof(args->counter));
		last_request = time_micro();
		sem_post(sem_request);
		fprintf(stderr, "%lu released request %lu\n", time_millis(), args->counter);
		//fprintf(stderr, "intra time: %ld\n", t2-t1);
		//fprintf(stderr, "inter time: %ld\n", t1-t3);
		read(sock, &reply, sizeof(reply));
		duration = time_millis() - st;
		//close(sock);
		sem_wait(sem_result);
		fprintf(stdout, "%ld %lu %ld %lu %d\n", st, args->delay, duration, reply, queue_size);
		fflush(stdout);
		sem_post(sem_result);
		sem_wait(sem_counter);
		returned++;
		sem_post(sem_counter);
		//free(args);
	}
}

int main(int argc, char* argv[]) {
	load_balancer = argv[1];
	char* file_name = argv[2];
	int i;
	FILE *fd;
	long delay;
	long last_req_time, now;
	long counter = 0;
	struct req_arg *args;
	pthread_t t1;
	int cr_t;
	time_t t;
	//signal(SIGPIPE, SIG_IGN);
	
	fprintf(stderr, "%ld load balancer: '%s'\n", time_millis(), load_balancer);

	queue = createQueue(sizeof(args));

	fprintf(stderr, "%ld created queue\n", time_millis());
	sem_counter = createsemaphore("/sem_counter", 1);
	sem_result = createsemaphore("/sem_result", 1);
	sem_request = createsemaphore("/sem_request", 1);

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
	last_request = time_micro();
	while(EOF != fscanf(fd, "%ld\n", &delay)) {
		fprintf(stderr, "read delay %ld\n", delay);
		//now = time_micro();
		//delay -= (now - last_req_time);
		//fprintf(stderr, "%ld micro now\n", now);
		//fprintf(stderr, "delta %ld\n", (now - last_req_time));
		//fprintf(stderr, "sleeping %ld\n", delay - (now - last_req_time));
		//usleep(delay - (now - last_req_time));
		//usleep(delay - 108);
		//last_req_time = time_micro();
		//t3 = time_micro();
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
