#include "common.c"
#include "Linked_list.c"

#define MAX 100000

int counter = 0;
char* load_balancer;

void *request(void* arg) {
	unsigned long delay = *((unsigned*)arg);
	long st, duration;
	int sock, reply;

	st = time_millis();
	sock = connectTo(load_balancer, PORT_LB);
	write(sock, &counter, sizeof(int));
	read(sock, &reply, sizeof(int));
	close(sock);
	duration = time_millis() - st;
	printf("%ld %lu %ld %d\n", st, delay, duration, reply);
}

int main(int argc, char* argv[]) {
	load_balancer = argv[1];
	char* file_name = argv[2];
	int i;
	int ret;
	FILE *fd;
	unsigned long delay;
	pthread_t *t1;
	Linked_list *threads, *node;
	void *res;

	threads = createList(NULL, NULL);
	
	fd = fopen(file_name,"r");
	if (fd == NULL) {
		fprintf(stdout, "Error opening file %s\n", file_name);
		exit(1);
	}


	while(EOF != fscanf(fd, "%lu\n", &delay)) {
		fprintf(stderr, "%lu waiting %ld\n", time_millis(), delay);
		counter++;
		usleep(delay);
		t1 = malloc(sizeof(pthread_t));
		ret = pthread_create(t1, NULL, request, (void *) &delay);
		if (ret != 0)
			fprintf(stderr, "ERROR creating thread %i\n", ret);
		else
			insertAfter(t1, threads);
	}
	fprintf(stderr, "%lu workload finished\n", time_millis());
	for(node = threads->next; node != NULL; node = node->next) {
		fprintf(stderr, "%lu join ", time_millis());
		t1 = (pthread_t*)(node->value);
		fprintf(stderr, "%ld\n", t1);
		pthread_join(*t1, &res);
	}
}
