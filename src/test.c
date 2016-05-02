#include <stdio.h>
#include <pthread.h>

void *readfile(void *arg) {
	FILE* fd = (FILE*)arg;
	unsigned long value;
	int matches;
	while((matches = fscanf(fd, "%lu\n", &value)) != EOF)
		printf("thread read %lu\n", value);
}

int main(int argc, char *argv[]) {
	FILE* fd;
	pthread_t t1, t2;

	fd = fopen(argv[1],"r");
	pthread_create(&t1, NULL, readfile, (void*)fd);
	pthread_create(&t2, NULL, readfile, (void*)fd);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	fclose(fd);
}
