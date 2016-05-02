#include "common.c"
#include "Linked_list.c"

#define MAX_REQ 1000

int hasnext, counter=0;
char* load_balancer;
FILE *fd;
sem_t* sem;

void *request(void* arg) {
	unsigned long delay = *((unsigned*)arg);
	long st, duration;
	int sock, reply;

	counter++;
	st = time_millis();
	sock = connectTo(load_balancer, PORT_LB);
	write(sock, &counter, sizeof(int));
	read(sock, &reply, sizeof(int));
	close(sock);
	duration = time_millis() - st;
	printf("%ld %lu %ld %d\n", st, delay, duration, reply);
}

void *batch(void* arg) {
	int execution = (int)arg;
	Linked_list *threads, *node;
	int ret, matches, req_count=0;
	unsigned long delay;
	pthread_t *t1;

	threads = createList(NULL, NULL);
	do {
		req_count++;
		//fprintf(stderr, "will scanf\n");
		matches = fscanf(fd, "%lu\n", &delay);
		fprintf(stderr, "%lu waiting %ld\n", time_millis(), delay);
		usleep(delay);
		t1 = malloc(sizeof(pthread_t));
		ret = pthread_create(t1, NULL, request, (void *) &delay);
		if (ret != 0)
			fprintf(stderr, "ERROR creating request thread %i\n", ret);
		else
			insertAfter(t1, threads);
	} while (matches != EOF && req_count < MAX_REQ);
	hasnext = matches != EOF;
	sem_post(sem);
	for(node = threads->next; node != NULL; node = node->next) {
		fprintf(stderr, "%lu join request %d ", time_millis(), execution);
		t1 = (pthread_t*)(node->value);
		fprintf(stderr, "%ld\n", t1);
		pthread_join(*t1, NULL);
	}
	return (void*)&matches;
}

int main(int argc, char* argv[]) {
	load_balancer = argv[1];
	char* file_name = argv[2];
	int ret, counter=0;
	pthread_t *t1;
	Linked_list *threads, *node;

	sem = createsemaphore("/sem_trace", 0);
	threads = createList(NULL, NULL);

	fd = fopen(file_name,"r");
	if (fd == NULL) {
		fprintf(stdout, "Error opening file %s\n", file_name);
		exit(1);
	}
	fprintf(stderr, "opened file %d\n", fd);
	do {
		counter++;
		fprintf(stderr, "%lu new batch\n", time_millis());
		t1 = malloc(sizeof(pthread_t));
		ret = pthread_create(t1, NULL, batch, (void*)counter);
		if (ret != 0)
			fprintf(stderr, "ERROR creating batch thread %i\n", ret);
		else
			insertAfter(t1, threads);
		sem_wait(sem);
	} while (hasnext);
	fclose(fd);
	fprintf(stderr, "%lu workload finished\n", time_millis());
	for(node = threads->next; node != NULL; node = node->next) {
		fprintf(stderr, "%lu join batch", time_millis());
		t1 = (pthread_t*)(node->value);
		fprintf(stderr, "%ld\n", t1);
		pthread_join(*t1, NULL);
	}
}
