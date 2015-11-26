#include "common.c"

int main(int argc, char* argv[]) {
	char* load_balancer = argv[1];
	char* file_name = argv[2];
	int sock, counter = 0, i, reply;
	FILE *fd;
	unsigned long delay;
	long int st, duration;
	fd = fopen(file_name,"r");
	if (fd == NULL) {
		fprintf(stdout, "Error opening file %s\n", file_name);
		exit(1);
	}


	while(EOF != fscanf(fd, "%lu\n", &delay)) {
		fprintf(stderr, "waiting %ld\n", delay);
		counter++;
		usleep(1000 * delay);
		if (!fork()) {
			st = time_millis();
			sock = connectTo(load_balancer, PORT_LB);
			write(sock, &counter, sizeof(int));
			read(sock, &reply, sizeof(int));
			close(sock);
			duration = time_millis() - st;
			printf("%ld %lu %ld %d\n", st, delay, duration, reply);
			return 0;
		}
	}
	for(i=0; i<counter; i++)
		wait(NULL);
}
