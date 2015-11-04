#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <pthread.h>

#include "common.c"
#include "Queue.c"

#define PORT_SER "8889"
#define PORT_PAR "8890"

struct node {
	char* name;
	struct node *next;
};

Queue *Q;
struct node *hosts;
sem_t *sem_work;
char *hostname;
int counter;
int is_first;

void enqueue_requests(int listen) {
	int sock;
	while (1) {
		sock = accept(listen, (struct sockaddr*) NULL, NULL );
		fprintf(stderr, "LISTEN got request at %ld\n", time_millis());
		Enqueue(Q, sock);
	}
}

void synchronize(int serialize, char *port) {
	int units, serverfd;
	hosts = hosts->next;
	while (strcmp(hostname, hosts->name) != 0) {
		if (serialize)
			sem_wait(sem_work);
		serverfd = connectTo(hosts->name, port);
		write(serverfd, &units, sizeof(int));
		read(serverfd, &units, sizeof(int));
		close(serverfd);
		hosts = hosts->next;
	}
}

void* process_requests(void *arg) {
	int i, sock, units, sem_value;
	long st, et;
	while (1) {
		DequeueElement(Q, &sock);
		st = time_millis();
		read(sock, &units, sizeof(units));
		//contention - linear increase
		synchronize(0, PORT_SER);
		//task execution
		usleep(units * 1000);
		//coherency - quadratic increase
		synchronize(1, PORT_PAR);
		write(sock, &units, sizeof(int));
		et = time_millis();
		fprintf(stdout, "%d %ld %ld\n", counter, st, et);
		fflush(stdout);
	}
}

void receive_sync(int serialize, char* port, int time) {
	int listenfd = 0, connfd = 0, units;
	pid_t pid;

	socketlisten(&listenfd, atoi(port));

	while (1) {
		connfd = accept(listenfd, (struct sockaddr*) NULL, NULL );
		if (!(pid = fork())) {
			read(connfd, &units, sizeof(units));
			usleep(time);
			write(connfd, &units, sizeof(units));
			if (serialize)
				sem_post(sem_work);
			return;
		} else {
			if (pid == -1)
				fprintf(stderr, "ERROR creating process: %d\n", errno);
		}
	}
}


int main(int argc, char *argv[]) {
	int contention = atoi(argv[1]) * 1000;
	int coherency = atoi(argv[2]) * 1000;
	char *peers = argv[3];
	counter = atoi(argv[4]); 
	int listenfd = 0, connfd = 0;
	int i, units;
	int load = 0;
	char *token;
	struct node *next;
	pid_t pid;
	pthread_t t1;

	//get hostname
	hosts = malloc(sizeof(struct node));
	Q = createQueue(50);
	hostname = malloc(8);
	gethostname(hostname, 8);

	//starts a circular liked list from peers
	token = strtok(peers, ",");
	hosts->name = token;
	hosts->next = hosts;

	is_first = strcmp(hostname, token)?0:1;
	//create shared semaphore for serial synchronization.
	sem_work = createsemaphore("/sem_work", is_first);
	/*sem_work = sem_open("/sem_work", O_CREAT | O_EXCL, 0644, is_first);
	if (sem_work == SEM_FAILED ) {
		fprintf(stderr, "Error creating semaphore %d\n", errno);
		return 1;
	}
	sem_unlink("/sem_work");*/

	//creates a circular liked list from peers
	token = strtok(NULL, ",");
	while (token != NULL ) {
		next = malloc(sizeof(struct node));
		next->name = token;
		next->next = hosts->next;
		hosts->next = next;
		hosts = next;
		token = strtok(NULL, ",");
	}

	//finds the position of this node in the list
	while (strcmp(hostname, hosts->name) != 0) {
		if (hosts->next == next) {
			fprintf(stderr, "ERROR hostname not found in list\n");
			return 1;
		} else {
			hosts = hosts->next;
		}
	}
	fprintf(stderr, "Found hostname %s\n", hosts->name);

	fflush(stdout);
	if (fork()) {
		//receives connections from peers
		if(fork())
			receive_sync(0, PORT_SER, contention);
		else
			receive_sync(1, PORT_PAR, coherency);

	} else {
		//receives connections from load balancer

		socketlisten(&listenfd, atoi(PORT));

		i = pthread_create(&t1, NULL, process_requests, NULL );
		enqueue_requests(listenfd);
	}
}
