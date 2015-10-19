#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>

#include "common.c"

int main(int argc, char *argv[]) {
	int clients = atoi(argv[1]); //simultaneous clients
	int requests = atoi(argv[2]); // requests per client
	int units = atoi(argv[3]);   //workload per request
	char* load_balancer = argv[4]; //name of the node that must receive the requests
	int workers_n = atoi(argv[5]);  //number of workers. not used. just to print
	int counter = atoi(argv[6]); //execution identifier
	int sock; //socket descriptor
	int reply; //reply from load balancer
	int i, j; //loop iterators
	long start_time, elapsed_time;
	float throughput;
	pid_t pid;

	for (i = 0; i < clients; i++) {
		if (!(pid = fork())) {
			for (j = 0; j < requests; j++) {
				start_time = time_millis();

				sock = connectTo(load_balancer, PORT_LB);
				//send request
				write(sock, &units, sizeof(units));
				//wait for completion
				read(sock, &reply, sizeof(reply));
				close(sock);
				elapsed_time = time_millis() - start_time;
				printf("%d %d %d %d %d %d %ld %ld\n", counter, i + 1, j + 1,
						requests, units, workers_n, elapsed_time, start_time);
			}
			return 0;
		} else {
			if (pid == -1)
				fprintf(stderr, "ERROR creating process: %d\n", errno);

		}
	}
	for (i = 0; i < clients; i++)
		wait(NULL );

}
