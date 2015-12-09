#include <pthread.h>

typedef struct Node {
	char* name;
	int queue_size;
	pthread_mutex_t mutex;
}Node;

Node* createNode(char* name) {
	Node* node = malloc(sizeof(Node));
	node->name = name;
	node->queue_size = 0;
	pthread_mutex_init(&(node->mutex), NULL );
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
