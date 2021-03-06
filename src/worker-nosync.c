#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>

#include "common.c"
#include "Queue.c"

Queue *Q;
Linked_list *tail = NULL;
char *hostname;
pthread_mutex_t m_peers;
long task_time_micro, num_peers, contention, coherency;
int sock=0, working=0;
sem_t *sem_read;
pthread_t t_enqueue;

void* enqueue_requests(void* arg) {
	unsigned long data;
	int bytes_read;
	while (1) {
		fprintf(stderr, "%ld waiting for request\n", time_millis());
		//if(working == 0) {
		//	fprintf(stderr, "before sem_wait\n");
		//	sem_wait(sem_read);
		//	fprintf(stderr, "after sem_wait\n");
		//}
		bytes_read = read(sock, &data, sizeof(data));
		switch(bytes_read) {
			case -1:
				fprintf(stderr, "ERROR reading from lb: %d\n", errno);
				break;
			case 0: break;
			default:
				fprintf(stderr, "%ld LISTEN got request %lu\n", time_millis(), data);
				Enqueue(Q, (void*)data);
		}
	}
}

void* accept_connections(void *arg) {
	int pt_result, listenfd = *((int*)arg);
	while(1) {
		fprintf(stderr, "%ld accepting\n", time_millis());
		sock = accept(listenfd, (struct sockaddr*) NULL, NULL );
		if (sock == -1) {
			perror("ERROR accept");
			exit(1);
		}
		fprintf(stderr, "%ld accepted\n", time_millis());
		//sem_post(sem_read);
		//fprintf(stderr, "sem_post\n");
		//working = 1;
		if((pt_result = pthread_create(&t_enqueue, NULL, enqueue_requests, NULL)) != 0)
			fprintf(stderr, "ERROR creating thread enqueue_requests: %d\n", pt_result);
	}
}

void* process_requests(void *arg) {
	//int sock = *((int*)arg);
	int i, sem_value;
	unsigned long data;
	long st, et, task_time;
	while (1) {
		fprintf(stderr, "%ld process_request\n", time_millis());
		DequeueElement(Q, (void**)&data);
		fprintf(stderr, "%ld processing %lu\n", time_millis(), data);
		st = time_millis();
		//task execution
		task_time = task_time_micro + (num_peers-1)*contention + (num_peers-1)*num_peers*coherency;
		fprintf(stderr, "sleeping for %ld micro\n", task_time);
		usleep(task_time);
		fprintf(stderr, "%ld replying %lu\n", time_millis(), data);
		if(write(sock, &data, sizeof(data)) == -1)
			fprintf(stderr, "ERROR writing to lb: %d\n", errno);
		et = time_millis();
		fprintf(stdout, "%ld %ld %d %d\n", st, et - st, data, sock);
		fflush(stdout);
	}
	return 0;
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
			tail = NULL;
			num_peers = 0;
			//working = 0;
			pthread_cancel(t_enqueue);
		} else {
			fprintf(stderr, "%ld removing %s from my peers\n", time_millis(),
					name);
			removeNode(name, tail);
			num_peers--;
		}
	}
	close(sock);
	pthread_mutex_unlock(&m_peers);
	pthread_exit(NULL);
}

void* manage_peers_listen(void* arg) {
	int listenfd = 0, connfd = 0, ret;
	socketlisten(&listenfd, atoi(PORT_MN));
	pthread_t t1;
	while (1) {
		fprintf(stderr, "%ld peers listen\n", time_millis());
		connfd = accept(listenfd, (struct sockaddr*) NULL, NULL );
		if (connfd == -1)
			perror("ERROR peers listen");
		else {
			ret = pthread_create(&t1, NULL, manage_peers, (void*) connfd);
			if (ret != 0)
				fprintf(stderr, "ERROR creating manage_peers thread: %d\n", ret);
		}
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

	Q = createQueue(sizeof(int));
	pthread_mutex_init(&m_peers, NULL );
	num_peers = 1;

	//get hostname
	hostname = malloc(8);
	gethostname(hostname, 8);
	fprintf(stderr, "my hostname is %s\n", hostname);
	sem_read = createsemaphore("/sem_read", 0);

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
		//receives connections from load balancer
	pthread_create(&t1, NULL, manage_peers_listen, NULL );

	socketlisten(&listenfd, atoi(PORT));
	//if((pt_result = pthread_create(&t2, NULL, accept_connections, (void*)&listenfd )) != 0)
	//	fprintf(stderr, "ERROR opening accept thread: %i\n", pt_result);

	for (i = 0; i < n_processes; i++)
		if((pt_result = pthread_create(&t2, NULL, process_requests, NULL)) != 0)
			fprintf(stderr, "ERROR opening thread: %i\n", pt_result);
	fprintf(stderr, "%ld created %d threads\n", time_millis(), n_processes);
	//enqueue_requests();
	accept_connections((void*)&listenfd);
}
