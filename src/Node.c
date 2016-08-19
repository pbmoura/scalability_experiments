#include <pthread.h>
#include <semaphore.h>

typedef struct Node {
	char* name;
	int socket;
	pthread_t thread;
	int queue_size, active;
	pthread_mutex_t mutex;
	sem_t *sem_read;
}Node;

Node* createNode(char* name, int socket, pthread_t thread) {
	char label[20];
	Node* node = malloc(sizeof(Node));
	node->name = name;
	node->socket = socket;
	node->thread = thread;
	node->queue_size = 0;
	node->active = 1;
	pthread_mutex_init(&(node->mutex), NULL );
	sprintf(label, "/sem_read_%s", name);
	node->sem_read = createsemaphore(label, 0);
	return node;
}

void arrivalNode(Node* node) {
	pthread_mutex_lock(&(node->mutex));
	(node->queue_size)++;
	pthread_mutex_unlock(&(node->mutex));
}

void departureNode(Node* node) {
	pthread_mutex_lock(&(node->mutex));
	(node->queue_size)--;
	pthread_mutex_unlock(&(node->mutex));
}
