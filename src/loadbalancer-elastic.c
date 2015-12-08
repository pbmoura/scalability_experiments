#include <sys/types.h>
#include <sys/mman.h>
#include <math.h>
#include "common.c"
#include "Linked_list.c"

int load = 0;
int num_workers = 0;
Linked_list *workers = NULL;
sem_t *sem_load, *sem_workers;
double contention, coherency, s1, x1, max_throughput, max_workers;

int usl_peak() {
	return lrint(sqrt((1-contention)/coherency));
}

double usl(int n) {
	return n/(1 + contention*(n-1) + n*coherency*(n-1));
}

double service_time(int w) { //w is the number of workers
	double n = (double) w;
	fprintf(stderr,"service time for %d\n", w);
	return s1 + s1 * contention * (n - 1) + s1 * coherency * n * (n - 1);
}

double arrival_rate(int current_load, double service_time) { //t is the service time
	fprintf(stderr,"arrival rate for %d %f\n", current_load, service_time);
	return current_load / service_time;
}

double estimate_workers(double x) { //x is required throughput
	double w, delta, c = (x / x1);
	fprintf(stderr, "workers estimate for %f\n", x);
	if (x >= max_throughput) {
		fprintf(stderr, "load exceeds limit\n");
		w = max_workers;
	} else {
		delta = pow(contention - coherency - 1 / c, 2.0)
			- 4 * coherency * (1 - contention);
		w = (-contention + coherency + 1 / c - sqrt(delta))
			/ (2 * coherency);
	}
	fprintf(stderr, "estimating %f workers with delta %f\n", w, delta);
	return floor(w * 10) / 10;
}

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
		send_worker(iterator->value, action, node);
		iterate(&iterator);
	} while (iterator != workers);
}

void send_workers(char* node) {
	Linked_list *iterator;

	iterator = workers;
	do {
		send_worker(node, 1, iterator->value);
		iterate(&iterator);
	} while (iterator != workers);
}

void add_worker(char* name) {
	fprintf(stderr, "%ld adding worker %s\n", time_millis(), name);
	sem_wait(sem_workers);
	if (workers == NULL ) {
		fprintf(stderr, "creating list\n");
		workers = createCircularList(name);
	} else {
		fprintf(stderr, "inserting in list\n");
		update_workers(1, name);
		send_workers(name);
		workers = insertAfter(name, workers);
	}
	num_workers++;
	sem_post(sem_workers);
}

char* remove_worker() {
	Linked_list *node;
	sem_wait(sem_workers);
	node = removeNext(workers);
	update_workers(-1, node->value);
	num_workers--;
	sem_post(sem_workers);
	return node->value;
}

char* next_worker() {
	sem_wait(sem_workers);
	char* name = workers->value;
	iterate(&workers);
	sem_post(sem_workers);
	return name;
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

int estimate_num_workers(int current_num_workers, int current_load) {
	int n;
	if (load == 0)
		n = 1;
	else {
		n = ceil(
				estimate_workers(
						arrival_rate(current_load,
								service_time(current_num_workers))));
		if (n < 1)
			n = 1;
	}
	return n;
}

void* monitoring(void* arg) {
	char* host_name = (char*) arg;
	int n, diff;
	while (1) {
		usleep(500000);
		n = estimate_num_workers(num_workers, load);
		//printf("monitoring %d %d\n", num_workers, n);
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
	char *worker;
	int data;
	long st, et;
	worker = next_worker();
	fprintf(stderr, "%ld handling to %s\n", time_millis(), worker);
	arrival();
	st = time_millis();
	read(connfd, &data, sizeof(data));
	sock = connectTo(worker, PORT);
	write(sock, &data, sizeof(data));
	read(sock, &data, sizeof(data));
	close(sock);
	write(connfd, &data, sizeof(data));
	et = time_millis();
	departure();
	fprintf(stdout, "%ld %ld %s %d\n", st, et - st, worker, connfd);
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

	max_workers = usl_peak();
	max_throughput = usl(max_workers);
	fprintf(stderr, "limit of %f workers at %f req/s\n", max_workers, max_throughput);

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
