#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "common.c"

struct conn_t {
	int fd;
	char* hostname;
};

int load = 0;
sem_t *sem_load;

void arrival() {
	sem_wait(sem_load);
	load++;
	sem_post(sem_load);
	fprintf(stderr,"arrival %d\n", load);
}

void departure() {
	sem_wait(sem_load);
	load--;
	sem_post(sem_load);
	fprintf(stderr,"departure %d\n", load);
}

void *handle_request(void *arg) {
	int sock = 0;
	struct conn_t *conn = (struct conn_t *) arg;
	int data;
	long st, et;
	fprintf(stderr,"handling to %s\n", conn->hostname);
	fprintf(stderr, "connection at %ld\n", time_millis());
	arrival();
	st = time_millis();
	read(conn->fd, &data, sizeof(data));
	sock = connectTo(conn->hostname, PORT);
	write(sock, &data, sizeof(data));
	read(sock, &data, sizeof(data));
	write(conn->fd, &data, sizeof(data));
	et = time_millis();
	departure();
	fprintf(stdout, "%ld %ld %s\n", st, et, conn->hostname);
	fflush(stdout);
	free(conn);
	return NULL;
}

int main(int argc, char *argv[]) {
	int workers_n = atoi(argv[1]);  //number of workers
	char *hostnames = argv[2];  //comma separated list of workers' hostnames
	int iteration = atoi(argv[3]); // iteration counter.

	char *token;  //for hosts tokenization
	char* hosts[workers_n]; //workers' socket names
	int i;
	int listenfd = 0, counter = 0;
	struct conn_t *conn;
	pthread_t t1;

	sem_load = createsemaphore("/sem_load", 1);

	i = 0;
	token = strtok(hostnames, ",");
	while (token != NULL ) {
		hosts[i++] = token;
		token = strtok(NULL, ",");
	}

	socketlisten(&listenfd, atoi(PORT_LB));

	fflush(stdout);
	while (1) {
		conn = malloc(sizeof(struct conn_t));
		conn->hostname = hosts[counter];
		counter = counter == workers_n - 1 ? 0 : counter + 1;
		fprintf(stderr, "next worker: %s\n", conn->hostname);
		conn->fd = accept(listenfd, (struct sockaddr*) NULL, NULL );
		fprintf(stderr,"accepted\n");
		i = pthread_create(&t1, NULL, handle_request, (void *)conn);
		fprintf(stderr,"pthread_create %d\n", i);
	}
}
