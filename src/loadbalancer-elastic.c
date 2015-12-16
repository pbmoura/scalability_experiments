#include <sys/types.h>
#include <sys/mman.h>
#include <math.h>
#include "common.c"
#include "Linked_list.c"
#include "Node.c"

int load = 0;
int num_workers = 0;
Linked_list *workers = NULL;
sem_t *sem_load, *sem_workers;
char* pool_manager;

void init(int argc, char *argv[]);
void verify_num_workers();

void send_worker(char* dest, int action, char* node) {
	fprintf(stderr, "informing of action %d in node %s to %s\n", action, node, dest);
	int sock = connectTo(dest, PORT_MN);
	int size = sizeof(node);
	write(sock, &action, sizeof(int));
	if (action != 0) {
		write(sock, &size, sizeof(int));
		write(sock, node, size);
	}
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
	Node* node = createNode(name);
	fprintf(stderr, "%ld adding worker %s\n", time_millis(), name);

	sem_wait(sem_workers);
	if (workers == NULL ) {
		fprintf(stderr, "creating list\n");
		workers = createList(node, NULL);
	} else {
		fprintf(stderr, "inserting in list\n");
		update_workers(1, name);
		send_workers(name);
		workers = createList(node, workers);
	}
	num_workers++;
	sem_post(sem_workers);
}

char* remove_worker() {
	Linked_list *node;
	char* name;
	sem_wait(sem_workers);
	//node = removeNext(workers);
	node = workers;
	name = ((Node*)node->value)->name;
	free(node);
	iterate(&workers);
	update_workers(-1, name);
	num_workers--;
	sem_post(sem_workers);
	return name;
}

Node* next_worker() {
	Linked_list* iterator;
	Node* node;

	iterator = workers;
	node = iterator->value;
	do {
		fprintf(stderr, "next_worker loop %i\n", ((Node*)iterator->value)->queue_size);
		if (((Node*)iterator->value)->queue_size < node->queue_size)
			node = iterator->value;
		iterate(&iterator);
	} while (iterator != NULL);
	fprintf(stderr, "next_worker returning %s %i\n", node->name, node->queue_size);
	return node;
}

void request_workers(char* host, int qtd) {
	int i, size;
	char* node;
	fprintf(stderr, "%ld requesting %d\n", time_millis(), qtd);
	int sock = connectTo(host, PORT_PM);
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
	fprintf(stderr, "%ld releasing %d\n", time_millis(), qtd);
	int sock = connectTo(host, PORT_PM);
	write(sock, &qtd, sizeof(int));
	for (i = 0; i > qtd; i--) {
		node = remove_worker();
		fprintf(stderr, "%ld removing worker %s\n", time_millis(), node);
		size = sizeof(node);
		write(sock, &size, sizeof(int));
		write(sock, node, size);
	}
}

void* monitoring(void* arg) {
	char* host_name = (char*) arg;
	while (1) {
		usleep(500000);
		verify_num_workers();
	}
	return 0;
}

void arrival() {
	sem_wait(sem_load);
	load++;
	//verify_num_workers();
	sem_post(sem_load);
	fprintf(stderr, "%ld arrival %d %d\n", time_millis(), load, num_workers);
}

void departure() {
	sem_wait(sem_load);
	load--;
	sem_post(sem_load);
	fprintf(stderr, "%ld departure %d %d\n", time_millis(), load, num_workers);
}

void *handle_request(void *arg) {
	int sock = 0;
	int connfd =  *((int*)arg);
	Node *worker;
	int data;
	long st, et;
	worker = next_worker();
	fprintf(stderr, "%ld handling to %s\n", time_millis(), worker->name);
	arrival();
	st = time_millis();
	read(connfd, &data, sizeof(data));
	sock = connectTo(worker->name, PORT);
	arrivalNode(worker);
	write(sock, &data, sizeof(data));
	read(sock, &data, sizeof(data));
	departureNode(worker);
	close(sock);
	write(connfd, &data, sizeof(data));
	et = time_millis();
	departure();
	fprintf(stdout, "%ld %ld %s %d\n", st, et - st, worker->name, connfd);
	fflush(stdout);
	return NULL ;
}

int main(int argc, char *argv[]) {
	pool_manager = argv[1];  //pool manager's hostname
//	contention = atof(argv[2]);
//	coherency = atof(argv[3]);
//	s1 = atof(argv[4]);
//	x1 = atof(argv[5]);

	init(argc, argv);

	int listenfd = 0, connfd = 0;
	pthread_t t_mon, t_req;

//	max_workers = usl_peak();
//	max_throughput = usl(max_workers);
//	fprintf(stderr, "limit of %f workers at %f req/s\n", max_workers, max_throughput);

	sem_load = createsemaphore("/sem_load", 1);
	sem_workers = createsemaphore("/sem_workers", 1);

	request_workers(pool_manager, 1);

	socketlisten(&listenfd, atoi(PORT_LB));

	fflush(stdout);
	pthread_create(&t_mon, NULL, monitoring, (void *) pool_manager);
	while (1) {
		connfd = accept(listenfd, (struct sockaddr*) NULL, NULL );
		//fprintf(stderr, "accepted\n");
		pthread_create(&t_req, NULL, handle_request, (void *) &connfd);
	}
}
