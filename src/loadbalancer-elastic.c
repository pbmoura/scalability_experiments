#include <sys/types.h>
#include <sys/mman.h>
#include <math.h>
#include "common.c"
#include "Linked_list.c"

int load = 0;
int num_workers = 0;
Linked_list *workers = NULL;
sem_t *sem_load, *sem_workers;
double contention, coherency, s1, x1;

double service_time(int w) { //n is the number of workers
	double n = (double) w;
	printf("service time for %d\n", w);
	return s1 + s1 * contention * (n - 1) + s1 * coherency * n * (n - 1);
}

double arrival_rate(int current_load, double service_time) { //t is the service time
	printf("arrival rate for %d %f\n", current_load, service_time);
	return current_load / service_time;
}

double estimate_workers(double x) { //x is required throughput
	double c = (x / x1);
	double delta = pow(contention - coherency - 1 / c, 2.0)
			- 4 * coherency * (1 - contention);
	double w = (-contention + coherency + 1 / c - sqrt(delta))
			/ (2 * coherency);
	printf("estimate workers for %f\n", x);
	return floor(w * 10) / 10;
}

void add_worker(char* name) {
	printf("adding worker %s\n", name);
	sem_wait(sem_workers);
	if (workers == NULL ) {
		puts("creating");
		workers = createCircularList(name);
	} else {
		puts("inserting");
		workers = insertAfter(name, workers);
	}
	num_workers++;
	sem_post(sem_workers);
}

char* remove_worker() {
	Linked_list *node;
	sem_wait(sem_workers);
	node = removeNext(workers);
	num_workers--;
	sem_post(sem_workers);
	return node->name;
}

char* next_worker() {
	sem_wait(sem_workers);
	char* name = workers->name;
	iterate(workers);
	sem_post(sem_workers);
	return name;
}

void request_workers(char* host, int qtd) {
	int i, size;
	char* node;
	printf("requesting %d\n", qtd);
	int sock = connectTo(host, PORT_PM);
	write(sock, &qtd, sizeof(int));
	for (i = 0; i < qtd; i++) {
		read(sock, &size, sizeof(int));
		node = malloc(size);
		read(sock, node, size);
		printf("received from pool %s\n", node);
		add_worker(node);
	}
}

void release_workers(char* host, int qtd) {
	int i, size;
	char* node;
	printf("releasing %d\n", qtd);
	int sock = connectTo(host, PORT_PM);
	write(sock, &qtd, sizeof(int));
	for (i = 0; i > qtd; i--) {
		node = remove_worker();
		size = sizeof(node);
		write(sock, &size, sizeof(int));
		write(sock, node, size);
	}
}

int estimate_num_workers(int current_num_workers, int current_load) {
	int n;
	if (load == 0)
		n = 1;
	else {
		n = ceil(estimate_workers(arrival_rate(current_load, service_time(current_num_workers))));
		if (n < 1)
			n = 1;
	}
	return n;
}

void* monitoring(void* arg) {
	char* host_name = (char*) arg;
	int n, diff;
	while (1) {
		n = estimate_num_workers(num_workers, load);
		printf("monitoring %d %d\n", num_workers, n);
		diff = n - num_workers;
		if (diff > 0) {
			request_workers(host_name, diff);
		} else if (diff < 0) {
			release_workers(host_name, diff);
		}
	}
	return 0;
}

void arrival() {
	sem_wait(sem_load);
	load++;
	sem_post(sem_load);
	fprintf(stderr, "arrival %d %d\n", load, num_workers);
}

void departure() {
	sem_wait(sem_load);
	load--;
	sem_post(sem_load);
	fprintf(stderr, "departure %d %d\n", load, num_workers);
}

void *handle_request(void *arg) {
	int sock = 0;
	int connfd = (int) arg;
	char *worker;
	int data;
	long st, et;
	worker = next_worker();
	fprintf(stderr, "handling to %s\n", worker);
	fprintf(stderr, "connection at %ld\n", time_millis());
	arrival();
	st = time_millis();
	read(connfd, &data, sizeof(data));
	sock = connectTo(worker, PORT);
	write(sock, &data, sizeof(data));
	read(sock, &data, sizeof(data));
	write(connfd, &data, sizeof(data));
	et = time_millis();
	departure();
	fprintf(stdout, "%ld %ld %s\n", st, et, worker);
	fflush(stdout);
	return NULL ;
}

int main(int argc, char *argv[]) {
	char *pool_manager = argv[1];  //pool manager's hostname
	contention = atof(argv[2]);
	coherency = atof(argv[3]);
	s1 = atof(argv[4]);
	x1 = atof(argv[5]);

	int listenfd = 0, connfd = 0;
	pthread_t t_mon, t_req;

	sem_load = createsemaphore("/sem_load", 1);
	sem_workers = createsemaphore("/sem_workers", 1);

	request_workers(pool_manager, 1);

	socketlisten(&listenfd, atoi(PORT_LB));

	fflush(stdout);
	pthread_create(&t_mon, NULL, monitoring, (void *) pool_manager);
	while (1) {
		connfd = accept(listenfd, (struct sockaddr*) NULL, NULL );
		fprintf(stderr, "accepted\n");
		pthread_create(&t_req, NULL, handle_request, (void *) connfd);
	}
}
