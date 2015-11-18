#include <sys/types.h>
#include "common.c"
#include "Queue.c"

Queue *nodes;

void* process_request(void* arg) {
	int socket = *((int*)arg);
	int qtd, i, size;
	char* node;
	printf("processing %d\n", socket);
	read(socket, &qtd, sizeof(int));
	printf("requested %d\n", qtd);
	if(qtd > 0) {
		puts("sending");
		for(i = 0; i<qtd; i++) {
			DequeueElement(nodes, &node);
			size = sizeof(node);
			write(socket, &size, sizeof(int));
			write(socket, node, size);
			puts("sent");
		}
	} else {
		puts("receiving");
		for(i = 0; i>qtd; i--) {
			read(socket, &size, sizeof(int));
			node = malloc(size);
			read(socket, node, sizeof(node));
			Enqueue(nodes, node);
			printf("received %s\n", node);
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	int workers_n = atoi(argv[1]);  //number of workers
	char *hostnames = argv[2];  //comma separated list of workers' hostnames

	char *token;  //for hosts tokenization
	//char* hosts[workers_n], *host_name; //workers' socket names
	//int sock; //socket descriptor
	int data;
	pid_t pid;
	int listenfd = 0, connfd = 0, counter = 0;
	struct sockaddr_in serv_addr;
	pthread_t t1;

	puts("processing node list");
	nodes = createQueue(workers_n);
	token = strtok(hostnames, ",");
	while (token != NULL ) {
		printf("node %s\n", token);
		Enqueue(nodes, token);
		token = strtok(NULL, ",");

	}
	puts("listen");
	socketlisten(&listenfd, atoi(PORT_PM));

	fflush(stdout);
	while (1) {
		puts("accepting");
		connfd = accept(listenfd, (struct sockaddr*) NULL, NULL );
		printf("accepted %d\n", connfd);
		pthread_create(&t1, NULL, process_request, &connfd);
	}
}
