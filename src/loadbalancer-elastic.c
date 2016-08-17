#include <sys/types.h>
#include <sys/mman.h>
#include <math.h>
#include "common.c"
#include "Linked_list.c"
#include "Node.c"

int load = 0, connfd = 0, num_workers = 0;
Linked_list *workers = NULL;
sem_t *sem_load, *sem_workers, *sem_reply;
char* pool_manager;
unsigned int monitoring_interval;
void* handle_request(void *arg);

//abstract functions
void init(int argc, char *argv[]);
void verify_num_workers();
void onarrival();

void send_worker(char* dest, int action, char* node) {
	//fprintf(stderr, "informing of action %d in node %s to %s\n", action, node, dest);
	int sock = connectTo(dest, PORT_MN, "send_worker");
	int size = sizeof(node);
	write(sock, &action, sizeof(int));
	if (action != 0) {
		write(sock, &size, sizeof(int));
		write(sock, node, size);
	}
	close(sock);
}

void update_workers(int action, char* node) {
	Linked_list *iterator;

	iterator = workers;
	do {
		send_worker(((Node*)iterator->value)->name, action, node);
		iterate(&iterator);
	} while (iterator != NULL);
}

void send_workers(char* node) {
	Linked_list *iterator;

	iterator = workers;
	do {
		send_worker(node, 1, ((Node*)iterator->value)->name);
		iterate(&iterator);
	} while (iterator != NULL);
}

void add_worker(char* name) {
	Node* node;
	int sock, ret;
	char label[25];
	pthread_t t_req;
	//sprintf(label, "add_worker %s", name);
	sock = connectTo(name, PORT, label);
	node = createNode(name, sock, 0);
	ret = pthread_create(&t_req, NULL, handle_request, (void*)node);
	if (ret != 0)
		fprintf(stderr, "%ld ERROR creating thread: %d\n", time_millis(), ret);
	node->thread = t_req;
	//fprintf(stderr, "%ld adding worker %s\n", time_millis(), name);

	sem_wait(sem_workers);
	if (workers == NULL ) {
		workers = createList(node, NULL);
	} else {
		update_workers(1, name);
		send_workers(name);
		workers = createList(node, workers);
	}
	num_workers++;
	sem_post(sem_workers);
}

char* remove_worker() {
	//Linked_list *node;
	char* name;
	Node* node;
	sem_wait(sem_workers);
	//node = removeNext(workers);
	node = (Node*)(workers->value);
	name = node->name;
	update_workers(-1, name);
	pthread_cancel(node->thread);
	free(node);
	iterate(&workers);
	num_workers--;
	sem_post(sem_workers);
	return name;
}

Node* next_worker() {
	Linked_list* iterator;
	Node* node;

	sem_wait(sem_workers);
	iterator = workers;
	node = iterator->value;
	do {
		if (((Node*)iterator->value)->queue_size < node->queue_size)
			node = iterator->value;
		iterate(&iterator);
	} while (iterator != NULL);
	//fprintf(stderr, "next_worker returning %s %i\n", node->name, node->queue_size);
	arrivalNode(node);
	sem_post(sem_workers);
	return node;
}

void request_workers(char* host, int qtd) {
	int i, size;
	char* node;
	fprintf(stderr, "%ld requesting %d %d %d\n", time_millis(), qtd, load, num_workers);
	int sock = connectTo(host, PORT_PM, "request_workers");
	write(sock, &qtd, sizeof(int));
	for (i = 0; i < qtd; i++) {
		read(sock, &size, sizeof(int));
		node = malloc(size);
		read(sock, node, size);
		fprintf(stderr, "%ld received from pool %s\n", time_millis(), node);
		add_worker(node);
	}
	close(sock);
	fflush(stderr);
}

void release_workers(char* host, int qtd) {
	int i, size;
	char* node;
	//fprintf(stderr, "%ld releasing %d %d %d\n", time_millis(), qtd, load, num_workers);
	int sock = connectTo(host, PORT_PM, "release_workers");
	write(sock, &qtd, sizeof(int));
	for (i = 0; i > qtd; i--) {
		node = remove_worker();
		size = sizeof(node);
		write(sock, &size, sizeof(int));
		write(sock, node, size);
		fprintf(stderr, "%ld removing worker %s\n", time_millis(), node);
		free(node);
	}
	close(sock);
}

void* monitoring(void* arg) {
	while (1) {
		usleep(monitoring_interval * 1000);
		fprintf(stderr, "%ld monitoring %d %d\n", time_millis(), load, num_workers);
		verify_num_workers();
	}
	return 0;
}

void arrival() {
	sem_wait(sem_load);
	load++;
	//verify_num_workers();
	sem_post(sem_load);
	onarrival();
	fprintf(stderr, "%ld arrival %d %d\n", time_millis(), load, num_workers);
}

void departure() {
	sem_wait(sem_load);
	load--;
	//verify_num_workers();
	sem_post(sem_load);
	fprintf(stderr, "%ld departure %d %d\n", time_millis(), load, num_workers);
}

void* handle_request(void *arg) {
	Node *worker = (Node*)arg;
	int sock = 0, bytes_read;
	unsigned long data;
	long et;

	while(1) {
		//sem_wait(worker->sem_read);
		bytes_read = read(worker->socket, &data, sizeof(data));
		switch(bytes_read) {
			case -1: fprintf(stderr, "ERROR reading from worker %s: %d\n", worker->name, errno);
				break;
			case 0: //fprintf(stderr, "read 0 from workers\n");
				break;
			default:
				fprintf(stderr, "%ld got reply %lu\n", time_millis(), data);
				pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
				departureNode(worker);
				sem_wait(sem_reply);
				write(connfd, &data, sizeof(data));
				sem_post(sem_reply);
				et = time_millis();
				departure();
				fprintf(stdout, "%ld %lu %d\n", et, data, worker->socket);
				fflush(stdout);
				if (worker->queue_size == 0)
					pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		}
	}
}

void* initial_monitoring(void* arg) {
	pthread_t t_mon;
	sleep(60);
	verify_num_workers();
	pthread_create(&t_mon, NULL, monitoring, NULL);
}

int main(int argc, char *argv[]) {
	//struct ThreadPool *pool;
	struct Task *aTask;
	int listenfd = 0;
	Node *worker;
	unsigned long data;
	pthread_t t_mon, t_req;
	pool_manager = argv[1];  //pool manager's hostname
	monitoring_interval = atoi(argv[2]); //monitoring loop interval
	fprintf(stderr, "interval %u\n", monitoring_interval);

	init(argc, argv);


	sem_load = createsemaphore("/sem_load", 1);
	sem_workers = createsemaphore("/sem_workers", 1);
	sem_reply = createsemaphore("sem_reply", 1);

	request_workers(pool_manager, 1);

	socketlisten(&listenfd, atoi(PORT_LB));

	fflush(stdout);
	//pthread_create(&t_mon, NULL, monitoring, NULL);
	pthread_create(&t_mon, NULL, initial_monitoring, NULL);
	fprintf(stderr, "%ld accepting\n", time_millis());
	connfd = accept(listenfd, (struct sockaddr*) NULL, NULL );
	if (connfd == -1) {
		perror("ERROR accept");
		exit(1);
	}
	while (1) {
		read(connfd, &data, sizeof(data));
		worker = next_worker();
		arrival();
		fprintf(stderr, "%ld handling request %lu to %s\n", time_millis(), data, worker->name);
		if(write(worker->socket, &data, sizeof(data)) == -1)
			fprintf(stderr, "ERROR writing to %s: %d\n", worker->name, errno);
		//sem_post(worker->sem_read);
	}
}
