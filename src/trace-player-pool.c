#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "common.c"
#include "Queue.c"
#include "HashTable.c"

#define NUM_THREADS 5

char* load_balancer;
unsigned long returned = 0;
sem_t *sem_counter, *sem_result, *sem_request, *sem_response;
Queue *queue;
long now, last_request;
HashTable *table;
int sock;

typedef struct req_arg {
	long counter, delay, start_time;
}req_arg;

void *handle_responses(void* arg) {
	long end_time, duration, reply;
	struct req_arg *args;
	int queue_size;

	while(1) {
		sem_wait(sem_response);
		if(read(sock, &reply, sizeof(reply)) != -1) {
			end_time = time_millis();
			args = get(table, reply);
			duration = end_time - args->start_time;
			sem_wait(sem_result);
			fprintf(stdout, "%ld %lu %ld %lu %d\n", args->start_time, args->delay, duration, reply, queue_size);
			fflush(stdout);
			sem_post(sem_result);
			sem_wait(sem_counter);
			returned++;
			sem_post(sem_counter);
			free(args);
		} else {
			fprintf(stderr, "ERROR reading from socket %d\n", errno);
		}
	}
}

void* request(void* arg) {
	int id = *((int*)arg);
	struct req_arg *args;
	long st;
	int queue_size, sem_request_value;
	long delay, t1;

	fprintf(stderr, "%ld theard %d\n", time_millis(), id);

	while(1) {
		sem_wait(sem_request);
		t1 = time_millis();
		DequeueElement(queue, (void**)&args);
		fprintf(stderr, "%ld - %ld thread %d starting request %lu\n", time_millis(), t1, id, args->counter);
		//t1 = time_micro();
		queue_size = QueueSize(queue);

		//fprintf(stderr, "queue size: %d\n", queue_size);

		st = time_millis();
		now = time_micro();
		delay = (args->delay > (now - last_request))?args->delay - (now - last_request):0; 
		usleep(delay);
		fprintf(stderr, "%lu running request %lu sleep %ld delta %ld\n", time_millis(), args->counter, delay, now-last_request);
		write(sock, &(args->counter), sizeof(args->counter));
		last_request = time_micro();
		sem_post(sem_response);
		sem_post(sem_request);
		sem_getvalue(sem_request, &sem_request_value);
		fprintf(stderr, "%lu released sem_request %lu queue %d\n", time_millis(), args->counter, sem_request_value);
		args->start_time = last_request/1000;
		put(table, args->counter, args);

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
	table = createHashTable(100);

	fprintf(stderr, "%ld created queue\n", time_millis());
	sem_counter = createsemaphore("/sem_counter", 1);
	sem_result = createsemaphore("/sem_result", 1);
	sem_request = createsemaphore("/sem_request", 1);
	sem_response = createsemaphore("/sem_response", 0);

	fprintf(stderr, "%ld created semaphores\n", time_millis());
	fd = fopen(file_name,"r");
	if (fd == NULL) {
		fprintf(stderr, "Error opening file %s\n", file_name);
		exit(1);
	}

	fprintf(stderr, "connecting to lb\n");
	sock = connectTo(load_balancer, PORT_LB, "to lb");

	fprintf(stderr, "%ld trace file opened\n", time_millis());
	for (i=0; i<NUM_THREADS; i++) 
		if ((cr_t = pthread_create(&t1, NULL, request, (void*)&i)) != 0)
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
		//usleep(500000);
	}
	fprintf(stderr, "%lu workload finished\n", time_millis());
	while (returned < counter) {
		fprintf(stderr, "%lu < %lu\n", returned, counter);
		sleep(1);
	}
	fprintf(stderr, "finished with %lu of %lu\n", returned, counter);
}
