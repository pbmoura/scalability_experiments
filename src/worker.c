#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <pthread.h>
#include <math.h>

#include "common.c"
#include "Queue.c"
#include "Linked_list.c"

Queue *Q;
Linked_list *tail = NULL;
char *hostname;
pthread_mutex_t m_peers;
long task_time_micro;

void enqueue_requests(int listen) {
	int sock;
	while (1) {
		sock = accept(listen, (struct sockaddr*) NULL, NULL );
		fprintf(stderr, "%ld LISTEN got request %d\n", time_millis(), sock);
		Enqueue(Q, sock);
	}
}

void synchronize(int hops) {
	int serverfd;
	Linked_list *node;
	if (tail == NULL )
		return;
	node = tail->next;
	//iterate(hosts);
	do { //while (strcmp(hostname, hosts->name) != 0) {
		fprintf(stderr, "%ld synch %d %s\n", time_millis(), hops, node->name);
		serverfd = connectTo(node->name, PORT_SYN);
		write(serverfd, &hops, sizeof(int));
		read(serverfd, &hops, sizeof(int));
		close(serverfd);
		iterate(node);
	} while (node != tail);
}

void* process_requests(void *arg) {
	int i, sock, data, sem_value;
	long st, et;
	while (1) {
		fprintf(stderr, "%ld process_request\n", time_millis());
		DequeueElement(Q, &sock);
		fprintf(stderr, "%ld processing %d\n", time_millis(), sock);
		st = time_millis();
		read(sock, &data, sizeof(data));
		fprintf(stderr, "%ld read %d\n", time_millis(), data);
		//contention
		synchronize(1);
		//task execution
		usleep(task_time_micro);
		//coherency
		synchronize(2);
		fprintf(stderr, "%ld replying %d\n", time_millis(), sock);
		write(sock, &data, sizeof(int));
		et = time_millis();
		fprintf(stdout, "%ld %ld %d %d\n", st, et - st, data, sock);
		fflush(stdout);
	}
	return 0;
}

void receive_sync(int serialize, char* port, int time) {
	int listenfd = 0, connfd = 0, hops;
	pid_t pid;

	socketlisten(&listenfd, atoi(port));

	while (1) {
		connfd = accept(listenfd, (struct sockaddr*) NULL, NULL );
		if (!(pid = fork())) {
			read(connfd, &hops, sizeof(hops));
			fprintf(stderr, "%ld receiving sync %d\n", time_millis(), hops);
			usleep(time);
			if (hops > 1)
				synchronize(hops-1);
			write(connfd, &hops, sizeof(hops));
			return;
		} else {
			if (pid == -1)
				fprintf(stderr, "%ld ERROR creating process: %d\n",
						time_millis(), errno);
		}
	}
}

void add_peer(char* name) {
	fprintf(stderr, "%ld including %s as a peer\n", time_millis(), name);
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
	pthread_mutex_lock(&m_peers);
	if (action > 0) {
		add_peer(name);
	} else {
		if (strcmp(hostname, name) == 0) {
			fprintf(stderr,
					"%ld going back to pool. destroying my peers list\n",
					time_millis());
			destroyList(tail);
		} else {
			fprintf(stderr, "%ld removing %s from my peers\n", time_millis(),
					name);
			removeNode(name, tail);
		}
	}
	close(sock);
	pthread_mutex_unlock(&m_peers);
	return 0;
}

void* manage_peers_listen(void* arg) {
	int listenfd = 0, connfd = 0;
	socketlisten(&listenfd, atoi(PORT_MN));
	pthread_t t1;
	while (1) {
		connfd = accept(listenfd, (struct sockaddr*) NULL, NULL );
		pthread_create(&t1, NULL, manage_peers, (void*) connfd);
	}
	return 0;
}

int main(int argc, char *argv[]) {
	int contention = atoi(argv[1]) * 1000;
	int coherency = atoi(argv[2]) * 1000;
	int n_processes = atoi(argv[3]);
	double s1 = atof(argv[4]);
	char *peers;
	int listenfd = 0, connfd = 0;
	int i, units;
	int load = 0;
	char *token;
	Linked_list *hosts;
	pid_t pid;
	pthread_t t1, t2;

	task_time_micro = lrint(s1 * 1000000);

	Q = createQueue(50);
	pthread_mutex_init(&m_peers, NULL );

//get hostname
	hostname = malloc(8);
	gethostname(hostname, 8);

	if (argc > 5) {
		peers = argv[5];
		//starts a circular liked list from peers
		token = strtok(peers, ",");

		//fill a circular liked list with node names from peers
		while (token != NULL ) {
			if (strcmp(token, hostname) != 0)
				add_peer(token);
			token = strtok(NULL, ",");
		}

		//finds the position of this node in the list
		/*hosts = tail;
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
		fprintf(stderr, "Found hostname %s\n", hosts->name);*/
	} else {

	}
	fflush(stdout);
	if (fork()) {
		//receives connections from peers
		receive_sync(0, PORT_SYN, contention);
	} else {
		//receives connections from load balancer
		pthread_create(&t1, NULL, manage_peers_listen, NULL );

		socketlisten(&listenfd, atoi(PORT));

		for (i = 0; i < n_processes; i++)
			pthread_create(&t2, NULL, process_requests, NULL );
		enqueue_requests(listenfd);
	}
}
