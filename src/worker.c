#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#include "common.c"
#include "Queue.c"

#define PORT_SYNC "8889"

struct node {
	char* name;
	struct node *next;
};

Queue *Q;
struct node *hosts;
sem_t *sem_work;
char *hostname;
int counter;

void enqueue_requests(int listen) {
	int sock;
	while (1) {
		sock = accept(listen, (struct sockaddr*) NULL, NULL );
		fprintf(stderr, "got request at %ld\n", time_millis());
		Enqueue(Q, sock);
	}
}

void* process_requests(void *arg) {
	int i, sock, units, serverfd;
	long st, et;
	while (1) {
		DequeueElement(Q, &sock);
		st = time_millis();
		read(sock, &units, sizeof(units));
		usleep(units * 1000);
		hosts = hosts->next;
		//send sync messages
		while (strcmp(hostname, hosts->name) != 0) {
			sem_wait(sem_work);
			serverfd = connectTo(hosts->name, PORT_SYNC);
			write(serverfd, &units, sizeof(int));
			read(serverfd, &units, sizeof(int));
			close(serverfd);
			hosts = hosts->next;
		}
		write(sock, &units, sizeof(int));
		et = time_millis();
		fprintf(stdout, "%d %ld %ld\n", counter, st, et);
		fflush(stdout);
	}
}

int main(int argc, char *argv[]) {
	int coherency = atoi(argv[1]);
	char *peers = argv[2];
	int listenfd = 0, connfd = 0;
	int i, units;
	int load = 0;
	char *token;
	struct node *next;
	pid_t pid;
	pthread_t t1;

	hosts = malloc(sizeof(struct node));
	Q = createQueue(50);
	hostname = malloc(8);
	gethostname(hostname, 8);

	counter = atoi(argv[3]);

	//starts a circular liked list from peers
	token = strtok(peers, ",");
	hosts->name = token;
	hosts->next = hosts;

	//create shared semaphore for serial synchronization.
	sem_work = sem_open("/sem_work", O_CREAT | O_EXCL, 0644,
			strcmp(hostname, token) ? 0 : 1);
	if (sem_work == SEM_FAILED ) {
		fprintf(stderr, "Error creating semaphore %d\n", errno);
		return 1;
	}
	sem_unlink("/sem_work");

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
	printf("Found hostname %s\n", hosts->name);

	fflush(stdout);
	if (fork()) {
		//receives connections from peers

		socketlisten(&listenfd, atoi(PORT_SYNC));

		while (1) {
			connfd = accept(listenfd, (struct sockaddr*) NULL, NULL );
			if (!(pid = fork())) {
				fprintf(stderr, "got synch at %ld\n", time_millis());
				read(connfd, &units, sizeof(units));
				usleep(coherency * 1000);
				write(connfd, &load, sizeof(load));
				sem_post(sem_work);
				return 0;
			} else {
				if (pid == -1)
					fprintf(stderr, "ERROR creating process: %d\n", errno);
			}
		}
	} else {
		//receives connections from load balancer

		socketlisten(&listenfd, atoi(PORT));

		i = pthread_create(&t1, NULL, process_requests, NULL );
		enqueue_requests(listenfd);
	}
}
