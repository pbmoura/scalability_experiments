#include <unistd.h>
#include "common.c"
//#include "Linked_list.c"
#include "HashTable.c"

struct HashTable* table;
unsigned long responses=0;
int sock;
//sem_t *sem_read;

typedef struct req_arg {
	unsigned long counter, delay, start_time;
}req_arg;

void print_table() {
	int i;
	Linked_list *list;
	req_arg *arg;
	HashItem *item;
	printf("### start hash table ###");
	for(i=0; i<table->size; i++) {
		printf("\n#%i\n", i);
		list = table->items[i];
		while(list != NULL) {
			item = (HashItem*)(list->value);
			arg = (req_arg*)(item->value);
			printf("  (%ld, %lu, %lu, %lu)\n", item->key, arg->counter, arg->delay, arg->start_time);
			iterate(&list);
		}
	}
	printf("\n### end hash table ###\n");
}

void *handle_responses(void* arg) {
	unsigned long end_time, duration;
	unsigned long reply;
	int bytes_read;
	struct req_arg *args;
	sem_t *sem;
	sem = createsemaphore("/sem_trace", 1);

	while(1) {
		//sem_wait(sem_read);
		bytes_read = read(sock, &reply, sizeof(reply));
		switch(bytes_read) {
			case -1:
				fprintf(stderr, "ERROR reading from socket: %d\n", errno);
				break;
			case 0: break;
			default:
				end_time = time_millis();
				fprintf(stderr, "%ld received reply %lu\n", time_millis(), reply);
				args = get(table, reply);
				if (args == NULL ) {
					fprintf(stderr, "%ld got NULL with %lu\n", time_millis(), reply);
				} else {
					duration = end_time - args->start_time;
					printf("%lu %lu %lu %lu\n", args->start_time, args->delay, duration, reply);
					fflush(stdout);
					sem_wait(sem);
					responses++;
					sem_post(sem);
				}
				free(args);
		}
	}
}

int main(int argc, char* argv[]) {
	char* load_balancer = argv[1];
	char* file_name = argv[2];
	int ret;
	FILE *fd;
	pthread_t t1;
	//Linked_list *threads, *node;
	unsigned long counter = 0;
	struct req_arg *args;
	long now, last_req, req_time, delay, sleep;

	fd = fopen(file_name,"r");
	if (fd == NULL) {
		fprintf(stdout, "Error opening file %s\n", file_name);
		exit(1);
	}

	table = createHashTable(100);

	//sem_read = createsemaphore("/sem_read", 0);

	sock = connectTo(load_balancer, PORT_LB, "to lb");

	ret = pthread_create(&t1, NULL, handle_responses, NULL);
	if (ret != 0)
		fprintf(stderr, "ERROR creating thread %i\n", ret);

	delay = 0;
	last_req = time_micro();
	while(EOF != fscanf(fd, "%ld\n", &sleep)) {
		now = time_micro();
		sleep -= (now - last_req - delay);
		delay = 0;
		if (sleep > 0) {
			usleep(sleep);
			last_req = now + sleep;
		} else {
			delay = sleep;
			last_req = now;
		}
		//usleep(delay - (now - last_req));
		req_time = time_millis();
		//last_req = req_time - (req_time - now - delay);
		write(sock, &counter, sizeof(counter));
		//sem_post(sem_read);
		fprintf(stderr, "%ld running request %lu sleep %ld  %ld  %ld\n", req_time, counter, sleep, now, last_req);
		args = malloc(sizeof(struct req_arg));
		args->counter = counter;
		args->delay = sleep;
		args->start_time = last_req/1000;
		put(table, counter++, args);
	}
	fprintf(stderr, "%lu workload finished\n", time_millis());
	while (responses < counter) {
		fprintf(stderr, "%lu < %lu\n", responses, counter);
		usleep(1000000);
	}
}
