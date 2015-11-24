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
Linked_list *tail = NULL;
sem_t *sem_work;
char *hostname;
int counter;
int is_first;
pthread_mutex_t m_peers;

void enqueue_requests(int listen) {
	int sock;
	while (1) {
		sock = accept(listen, (struct sockaddr*) NULL, NULL );
		fprintf(stderr, "LISTEN got request %d %ld\n", sock, time_millis());
		Enqueue(Q, sock);
	}
}

void synchronize(int serialize, char *port) {
	int units, serverfd;
	Linked_list *node;
	if (tail == NULL)
		return;
	node = tail->next;
	//iterate(hosts);
	do { //while (strcmp(hostname, hosts->name) != 0) {
		fprintf(stderr, "synch %d %s\n", serialize, node->name);
		if (serialize)
			sem_wait(sem_work);
		serverfd = connectTo(node->name, port);
		write(serverfd, &units, sizeof(int));
		read(serverfd, &units, sizeof(int));
		close(serverfd);
		iterate(node);
	} while (node != tail);
}

void* process_requests(void *arg) {
	int i, sock, units, sem_value;
	long st, et;
	while (1) {
		fprintf(stderr, "%ld process_request\n", time_millis());
		DequeueElement(Q, &sock);
		fprintf(stderr, "%ld processing %d\n", time_millis(), sock);
		st = time_millis();
		read(sock, &units, sizeof(units));
		//contention - linear increase
		synchronize(0, PORT_SER);
		//task execution
		usleep(units * 1000);
		//coherency - quadratic increase
		synchronize(1, PORT_PAR);
		fprintf(stderr, "%ld replying %d\n", time_millis(), sock);
		write(sock, &units, sizeof(int));
		et = time_millis();
		//fprintf(stdout, "%d %ld %ld %d\n", counter, st, et-st, sock);
		fprintf(stderr, "%d %ld %ld %d\n", counter, st, et-st, sock);
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

void add_peer(char* name) {
	fprintf(stderr, "including %s as a peer\n", name);
	if (tail == NULL )
		tail = createCircularList(name);
	else
		tail = insertAfter(name, tail);
}

void* manage_peers(void* arg) {
	int sock = (int) arg;
	int action, size;
	char* name;
	read(sock, &action, sizeof(int));
	read(sock, &size, sizeof(int));
	name = malloc(size);
	read(sock, name, size);
	close(sock);
	pthread_mutex_lock(&m_peers);
	if (action > 0) {
		add_peer(name);
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
	socketlisten(&listenfd, atoi(PORT_MN));
	pthread_t t1;
	while (1) {
		connfd = accept(listenfd, (struct sockaddr*) NULL, NULL );
		pthread_create(&t1, NULL, manage_peers, (void*) connfd);
	}
}

int main(int argc, char *argv[]) {
	int contention = atoi(argv[1]) * 1000;
	int coherency = atoi(argv[2]) * 1000;
	int n_processes = atoi(argv[3]);
	char *peers;
	int listenfd = 0, connfd = 0;
	int i, units;
	int load = 0;
	char *token;
	Linked_list *hosts;
	pid_t pid;
	pthread_t t1, t2;

	Q = createQueue(50);
	pthread_mutex_init(&m_peers, NULL );

//get hostname
	hostname = malloc(8);
	gethostname(hostname, 8);

	if (argc > 4) {
		peers = argv[4];
		//starts a circular liked list from peers
		token = strtok(peers, ",");
		tail = createCircularList(token);

		is_first = strcmp(hostname, token) ? 0 : 1;
		sem_work = createsemaphore("/sem_work", is_first);

		//fill a circular liked list with node names from peers
		token = strtok(NULL, ",");
		while (token != NULL ) {
			tail = insertAfter(token, tail);
			token = strtok(NULL, ",");
		}

		//finds the position of this node in the list
		hosts = tail;
		do { //while (strcmp(hostname, hosts->name) != 0) {
			if (strcmp(hosts->next->name, hostname) == 0) {
				fprintf(stderr, "removing hostname from peers list\n");
				if (removeNext(hosts) == tail)
					tail = hosts;
				//return 1;
			} // else {
			iterate(hosts);
			//}
		} while (hosts != tail);
		fprintf(stderr, "Found hostname %s\n", hosts->name);
	} else {

	}
	fflush(stdout);
	if (fork()) {
		//receives connections from peers
		if (fork())
			receive_sync(0, PORT_SER, contention);
		else
			receive_sync(1, PORT_PAR, coherency);

	} else {
		//receives connections from load balancer
		pthread_create(&t1, NULL, manage_peers_listen, NULL );

		socketlisten(&listenfd, atoi(PORT));

		for (i = 0; i < n_processes; i++)
			pthread_create(&t2, NULL, process_requests, NULL );
		enqueue_requests(listenfd);
	}
}
