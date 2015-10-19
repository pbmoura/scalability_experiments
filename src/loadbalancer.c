#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "common.c"

int main(int argc, char *argv[]) {
	int workers_n = atoi(argv[1]);  //number of workers
	char *hostnames = argv[2];  //comma separated list of workers' hostnames
	int iteration = atoi(argv[3]); // iteration counter.

	char *token;  //for hosts tokenization
	char* hosts[workers_n], *host_name; //workers' socket names
	int sock; //socket descriptor
	int data;
	pid_t pid;
	int listenfd = 0, connfd = 0, counter = 0;
	struct sockaddr_in serv_addr;
	long st, et;

	data = 0;
	token = strtok(hostnames, ",");
	while (token != NULL ) {
		hosts[data++] = token;
		token = strtok(NULL, ",");
	}

	socketlisten(&listenfd, atoi(PORT_LB));

	fflush(stdout);
	while (1) {
		host_name = hosts[counter];
		counter = counter == workers_n - 1 ? 0 : counter + 1;
		fprintf(stderr, "next worker: %s\n", host_name);
		connfd = accept(listenfd, (struct sockaddr*) NULL, NULL );
		if (!(pid = fork())) {
			fprintf(stderr, "connection at %ld\n", time_millis());
			st = time_millis();
			read(connfd, &data, sizeof(data));
			sock = connectTo(host_name, PORT);
			write(sock, &data, sizeof(data));
			read(sock, &data, sizeof(data));
			write(connfd, &data, sizeof(data));
			et = time_millis();
			fprintf(stdout, "%d %ld %ld %s\n", iteration, st, et, host_name);
			return 0;
		} else {
			if (pid == -1)
				fprintf(stderr, "ERROR creating process: %d\n", errno);
		}
	}
}
