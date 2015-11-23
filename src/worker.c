#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <pthread.h>

#include "common.c"
#include "Queue.c"
#include "Linked_list.c"

/*struct node {
	char* name;
	struct node *next;
};*/

Queue *Q;
Linked_list *hosts, *tail;
sem_t *sem_work;
char *hostname;
int counter;
int is_first;
pthread_mutex_t m_peers;

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
	iterate(hosts);
	while (strcmp(hostname, hosts->name) != 0) {
		if (serialize)
			sem_wait(sem_work);
		serverfd = connectTo(hosts->name, port);
		write(serverfd, &units, sizeof(int));
		read(serverfd, &units, sizeof(int));
		close(serverfd);
		iterate(hosts);
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

void* manage_peers(void* arg) {
	int sock = (int)arg;
	int action, size;
	char* name;
	read(sock, &action, sizeof(int));
	read(sock, &size, sizeof(int));
	name = malloc(size);
	read(sock, name, size);
	pthread_mutex_lock(&m_peers);
	if (action > 0) {
		fprintf(stderr, "including %s as a peer\n", name);
		tail = insertAfter(name, tail);
	} else {
		if (strcmp(hostname, name) == 0) {
			fprintf(stderr, "going back to pool. destroying my peers list\n");
			destroyList(tail);
		} else {
			fprintf(stderr, "removing %s from my peers\n", name);
			removeNode(name, tail);
		}
	}
	pthread_mutex_unlock(&m_peers);
}

void* manage_peers_listen(void* arg) {
	int listenfd = 0, connfd = 0;
	socketlisten(&listenfd, PORT_MN);
	pthread_t t1;
	while (1) {
		connfd = accept(listenfd, (struct sockaddr*) NULL, NULL );
		pthread_create(&t1, NULL, manage_peers, (void*)connfd);
	}
}

int main(int argc, char *argv[]) {
	int contention = atoi(argv[1]) * 1000;
	int coherency = atoi(argv[2]) * 1000;
	char *peers = argv[3];
	int n_processes = atoi(argv[4]);
	counter = atoi(argv[5]);
	int listenfd = 0, connfd = 0;
	int i, units;
	int load = 0;
	char *token;
	pid_t pid;
	pthread_t t1, t2;

	Q = createQueue(50);
	pthread_mutex_init(&m_peers, NULL);

	//get hostname
	hostname = malloc(8);
	gethostname(hostname, 8);

	//starts a circular liked list from peers
	token = strtok(peers, ",");
	tail = createCircularList(token);

	is_first = strcmp(hostname, token)?0:1;
	sem_work = createsemaphore("/sem_work", is_first);

	//fill a circular liked list with node names from peers
	token = strtok(NULL, ",");
	while (token != NULL ) {
		tail = insertAfter(token, tail);
		token = strtok(NULL, ",");
	}

	//finds the position of this node in the list
	hosts = tail;
	while (strcmp(hostname, hosts->name) != 0) {
		if (hosts->next == tail) {
			fprintf(stderr, "ERROR hostname not found in list\n");
			return 1;
		} else {
			iterate(hosts);
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
		pthread_create(&t1, NULL, manage_peers_listen, NULL);

		socketlisten(&listenfd, atoi(PORT));

		for(i=0; i < n_processes; i++)
			pthread_create(&t2, NULL, process_requests, NULL );
		enqueue_requests(listenfd);
	}
}
