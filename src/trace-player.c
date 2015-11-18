#include "common.c"

int main(int argc, char* argv[]) {
	int units = atoi(argv[1]);
	char* load_balancer = argv[2];
	char* file_name = argv[3];
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
		counter++;
		usleep(1000 * delay);
		if (!fork()) {
			st = time_millis();
			sock = connectTo(load_balancer, PORT_LB);
			write(sock, &units, sizeof(units));
			read(sock, &reply, sizeof(reply));
			duration = time_millis() - st;
			printf("%ld %lu %ld %d\n", st, delay, duration, reply);
			return 0;
		}
	}
	for(i=0; i<counter; i++)
		wait(NULL);
}
