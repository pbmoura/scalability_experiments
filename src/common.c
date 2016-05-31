#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <time.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>

#define PORT_PAR "8890"
#define PORT_SER "8889"
#define PORT "8888"
#define PORT_LB "8887"
#define PORT_PM "8886"
#define PORT_MN "8885"

void work(int units, int usage, int delay) {
	int i, j;
	for (i = 0; i < units; i++) {
		for (j = 0; j < usage; j++)
			;
		usleep(delay);
	}
}

long time_millis() {
	long ms; // Milliseconds
	time_t s;  // Seconds
	struct timespec spec;

	clock_gettime(CLOCK_MONOTONIC, &spec);

	s = spec.tv_sec;
	ms = spec.tv_nsec / 1e6;

	return s * 1000 + ms;
}

struct addrinfo* getConnection(char *hostname, char *port) {
	struct addrinfo hints, *result;
	char *addr, *node;

	//HARDCODED: use infiniband on DAS-4

	addr = malloc(16);
	node = malloc(3);
	sprintf(node, "%d", atoi(hostname + 4));
	strcpy(addr, "10.149.0.");
	strcat(addr, node);

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
	hints.ai_flags = AI_CANONNAME;
	hints.ai_protocol = 0; /* Any protocol */
	if (getaddrinfo(addr, port, &hints, &result) != 0) {
		fprintf(stderr, "ERROR getaddrinfo\n");  //: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}
	return result;
}

int connectAddr(struct addrinfo *conn, char* tag) {
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//fprintf(stderr, "%ld connectingTo %s %i\n", time_millis(), tag, sockfd);
	if (connect(sockfd, conn->ai_addr, conn->ai_addrlen) < 0)
		fprintf(stderr, "ERROR connecting to %s (%s): %d\n", conn->ai_canonname, tag, errno);
	return sockfd;
}

int connectTo(char *hostname, char *port, char *tag) {
	struct addrinfo hints, *conn;
	int sockfd;

	conn = getConnection(hostname, port);
	if (conn == NULL ) {
		fprintf(stderr, "No connection info\n");
		exit(EXIT_FAILURE);
	}

	return connectAddr(conn, tag);
}

void socketlisten(int *listenfd, int port) {
	struct sockaddr_in serv_addr;

	*listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY );
	serv_addr.sin_port = htons(port);

	bind(*listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));

	listen(*listenfd, 30);
}

sem_t* createsemaphore(char *sem_name, int value) {
	sem_t *sem = sem_open(sem_name, O_CREAT | O_EXCL, 0644, value);
	if (sem == SEM_FAILED ) {
		fprintf(stderr, "Error creating semaphore %s: %d\n", sem_name, errno);
		exit(1);
	}
	sem_unlink(sem_name);
	return sem;
}
