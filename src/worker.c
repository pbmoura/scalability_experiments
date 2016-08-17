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
long contention, coherency, num_peers = 1;
long task_time_micro;
typedef struct sync_args {
	char* port;
	long sync_time;
	int connfd;
}sync_args;

void enqueue_requests(int listen) {
	int sock;
	while (1) {
		sock = accept(listen, (struct sockaddr*) NULL, NULL );
		fprintf(stderr, "%ld LISTEN got request %d\n", time_millis(), sock);
		Enqueue(Q, sock);
	}
}

void synchronize(int hops, char* port) {
	int serverfd;
	char* name;
	Linked_list *node;
	if (tail == NULL ) {
		fprintf(stderr, "No node to sync.\n");
		return;
	}
	node = tail->next;
	do { //while (strcmp(hostname, hosts->name) != 0) {
		name = (char*)node->value;
		fprintf(stderr, "%ld synch %d %s\n", time_millis(), hops, name);
		serverfd = connectTo(name, port, "synchronize");
		write(serverfd, &hops, sizeof(int));
		read(serverfd, &hops, sizeof(int));
		close(serverfd);
		//node = node->next;
		iterate(&node);
	} while (node != tail);
}

void* process_requests(void *arg) {
	int i, sock, data, sem_value;
	long st, et, task_time;
	while (1) {
		fprintf(stderr, "%ld process_request\n", time_millis());
		DequeueElement(Q, (void**)&sock);
		fprintf(stderr, "%ld processing %d\n", time_millis(), sock);
		st = time_millis();
		read(sock, &data, sizeof(data));
		fprintf(stderr, "%ld read %d\n", time_millis(), data);
		//contention
		//synchronize(1, PORT_SER);
		//task execution
		fprintf(stderr, "%ld task_time params %ld %ld %ld %ld\n", time_millis(), task_time_micro, num_peers, contention, coherency);
		task_time = task_time_micro + (num_peers-1)*contention + (num_peers-1)*num_peers*coherency;
		fprintf(stderr, "sleeping for %ld micro\n", task_time);
		usleep(task_time);
		//coherency
		//synchronize(2, PORT_PAR);
		fprintf(stderr, "%ld replying %d\n", time_millis(), sock);
		write(sock, &data, sizeof(int));
		et = time_millis();
		fprintf(stdout, "%ld %ld %d %d\n", st, et - st, data, sock);
		fflush(stdout);
	}
	return 0;
}

void* process_sync(void* args) {
	int hops;
	char* port = ((sync_args*)args)->port;
	long sync_time = ((sync_args*)args)->sync_time;
	int connfd = ((sync_args*)args)->connfd;
	
	read(connfd, &hops, sizeof(hops));
	fprintf(stderr, "%ld receiving sync %d\n", time_millis(), hops);
	usleep(sync_time);
	if (hops > 1) {
		synchronize(hops-1, port);
	} 
	write(connfd, &hops, sizeof(hops));
}

void receive_sync(char* port, long sync_time) {
	int listenfd = 0, connfd = 0, pt_result;
	pid_t pid;
	sync_args* args;
	pthread_t t1;

	socketlisten(&listenfd, atoi(port));

	while (1) {
		connfd = accept(listenfd, (struct sockaddr*) NULL, NULL );
		args = malloc(sizeof(sync_args));
		args->port = port;
		args->sync_time = sync_time;
		args->connfd = connfd;
		if ((pt_result = pthread_create(&t1, NULL, process_sync, (void*)args )) != 0)
			fprintf(stderr, "ERROR creating process_sync thread: %i\n", pt_result);
		/*if (!(pid = fork())) {
			read(connfd, &hops, sizeof(hops));
			fprintf(stderr, "%ld receiving sync %d\n", time_millis(), hops);
			usleep(sync_time);
			if (hops > 1) {
				synchronize(hops-1, port);
			} 
			write(connfd, &hops, sizeof(hops));
			return;
		} else {
			if (pid == -1)
				fprintf(stderr, "%ld ERROR creating process: %d\n",
						time_millis(), errno);
		}*/
	}
}

void add_peer(char* name) {
	fprintf(stderr, "%ld including %s as a peer\n", time_millis(), name);
	if (tail == NULL )
		tail = createCircularList(name);
	else
		tail = insertAfter(name, tail);
	num_peers++;
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
			num_peers = 0;
		} else {
			fprintf(stderr, "%ld removing %s from my peers\n", time_millis(),
					name);
			removeNode(name, tail);
			num_peers--;
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
	contention = atol(argv[1]);
	coherency = atol(argv[2]);
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
	int pt_result;

	task_time_micro = lrint(s1 * 1000000);
	fprintf(stderr, "task_time_micro = %ld\n", task_time_micro);

	Q = createQueue(sizeof(int));
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
		if (fork())
			receive_sync(PORT_SER, contention);
		else
			receive_sync(PORT_PAR, coherency);
	} else {
		//receives connections from load balancer
		pthread_create(&t1, NULL, manage_peers_listen, NULL );

		socketlisten(&listenfd, atoi(PORT));

		for (i = 0; i < n_processes; i++)
			if((pt_result = pthread_create(&t2, NULL, process_requests, NULL )) != 0)
				fprintf(stderr, "ERROR opening thread: %i\n", pt_result);
		enqueue_requests(listenfd);
	}
}
