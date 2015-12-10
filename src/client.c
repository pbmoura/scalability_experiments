#include <sys/types.h>
#include <sys/mman.h>
#include "common.c"

int main(int argc, char *argv[]) {
	int clients = atoi(argv[1]); //simultaneous clients
	int requests = atoi(argv[2]); // requests per client
	char* st = argv[3];   //workload per request. just to print
	char* load_balancer = argv[4]; //name of the node that must receive the requests
	char* workers_n = argv[5];  //number of workers. not used. just to print
	char* workers_p = argv[6]; //not user. just to print
	char* contention = argv[7]; //not user. just to print
	char* coherency = argv[8];
	char* counter = argv[9];
	int sock; //socket descriptor
	int msg, reply; //reply from load balancer
	int i, j; //loop iterators
	long start_time, elapsed_time;
	float throughput;
	pid_t pid;

	for (i = 0; i < clients; i++) {
		if (!(pid = fork())) {
			for (j = 0; j < requests; j++) {
				start_time = time_millis();
				fprintf(stderr, "connecting to %s\n", load_balancer);
				sock = connectTo(load_balancer, PORT_LB);
				//send request
				msg = i*100+j;
				fprintf(stderr,"sending request %d %d\n", i, j);
				write(sock, &msg, sizeof(int));
				fprintf(stderr,"sent request %d %d\n", i, j);
				//wait for completion
				read(sock, &reply, sizeof(int));
				close(sock);
				elapsed_time = time_millis() - start_time;
				printf("%d %d %d %s %s %s %s %s %ld %ld %s\n", i + 1, j + 1,
						requests, st, workers_n, workers_p, contention, coherency, elapsed_time, start_time, counter);
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
