#include <sys/types.h>
#include "common.c"
#include "Queue.c"

Queue *nodes;

void* process_request(void* arg) {
	int socket = *((int*)arg);
	int qtd, i, size;
	char* node;
	read(socket, &qtd, sizeof(int));
	fprintf(stderr, "%ld requested %d\n", time_millis(), qtd);
	if(qtd > 0) {
		for(i = 0; i<qtd; i++) {
			fprintf(stderr, "getting node %i\n", i);
			DequeueElement(nodes, (void*)&node);
			fprintf(stderr, "got %s\n", node);
			size = sizeof(node);
			write(socket, &size, sizeof(int));
			write(socket, node, size);
			fprintf(stderr, "sent %s\n", node);
		}
	} else {
		for(i = 0; i>qtd; i--) {
			read(socket, &size, sizeof(int));
			node = malloc(size);
			read(socket, node, sizeof(node));
			Enqueue(nodes, node);
			fprintf(stderr, "received %s\n", node);
		}
		close(socket);
	}
	return 0;
}

int main(int argc, char *argv[]) {
	int workers_n = atoi(argv[1]);  //number of workers
	char *hostnames = argv[2];  //comma separated list of workers' hostnames
	char *token;
	int data;
	pid_t pid;
	int listenfd = 0, connfd = 0, counter = 0;
	struct sockaddr_in serv_addr;
	pthread_t t1;

	nodes = createQueue(sizeof(char*));
	token = strtok(hostnames, ",");
	while (token != NULL ) {
		fprintf(stderr, "adding node %s\n", token);
		Enqueue(nodes, token);
		token = strtok(NULL, ",");

	}
	socketlisten(&listenfd, atoi(PORT_PM));

	fflush(stdout);
	while (1) {
		connfd = accept(listenfd, (struct sockaddr*) NULL, NULL );
		pthread_create(&t1, NULL, process_request, &connfd);
	}
}
