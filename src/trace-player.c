#include "common.c"
#include "Linked_list.c"

#define MAX 100000
char* load_balancer;
unsigned long responses=0;
sem_t *sem;

typedef struct req_arg {
	unsigned long counter, delay;
}req_arg;

void *request(void* arg) {
	unsigned long delay = ((struct req_arg*)arg)->delay;
	unsigned long counter = ((struct req_arg*)arg)->counter;
	long st, duration;
	int sock;
	unsigned long reply;

	st = time_millis();
	sock = connectTo(load_balancer, PORT_LB);
	write(sock, &counter, sizeof(counter));
	read(sock, &reply, sizeof(reply));
	duration = time_millis() - st;
	close(sock);
	printf("%ld %lu %ld %lu\n", st, delay, duration, reply);
	free(arg);
	sem_wait(sem);
	responses++;
	sem_post(sem);
}

int main(int argc, char* argv[]) {
	load_balancer = argv[1];
	char* file_name = argv[2];
	int i;
	int ret;
	FILE *fd;
	unsigned long delay;
	pthread_t t1;
	//Linked_list *threads, *node;
	void *res;
	unsigned long counter = 0;
	struct req_arg *args;

	fd = fopen(file_name,"r");
	if (fd == NULL) {
		fprintf(stdout, "Error opening file %s\n", file_name);
		exit(1);
	}

	//threads = createList(NULL, NULL);
	sem = createsemaphore("/sem_trace", 1);

	while(EOF != fscanf(fd, "%lu\n", &delay)) {
		//fprintf(stderr, "%lu waiting %ld\n", time_millis(), delay);
		usleep(delay);
		//t1 = malloc(sizeof(pthread_t));
		args = malloc(sizeof(struct req_arg));
		args->counter = counter++;
		args->delay = delay;
		ret = pthread_create(&t1, NULL, request, (void *) args);
		if (ret != 0)
			fprintf(stderr, "ERROR creating thread %i\n", ret);
		//else
		//	insertAfter(t1, threads);
	}
	fprintf(stderr, "%lu workload finished\n", time_millis());
	while (responses < counter) {
		fprintf(stderr, "%lu < %lu\n", responses, counter);
		sleep(1);
	}
	/*for(node = threads->next; node != NULL; node = node->next) {
		fprintf(stderr, "%lu join ", time_millis());
		t1 = (pthread_t*)(node->value);
		fprintf(stderr, "%ld\n", t1);
		pthread_join(*t1, &res);
	}*/
}
