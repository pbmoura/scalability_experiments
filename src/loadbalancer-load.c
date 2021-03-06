#include "loadbalancer-elastic.c"

int limit_max, limit_min, pool_step;

void init(int argc, char *argv[]) {
	fprintf(stderr, "init %s - %s\n", argv[3], argv[4]);
	limit_min = atoi(argv[3]);
	limit_max = atoi(argv[4]);
	pool_step = atoi(argv[5]);
	fprintf(stderr, "load limits %i - %i\n", limit_min, limit_max);
}

void verify_num_workers() {
	Linked_list* iterator;
	int max=0, min=limit_max+1;
	Node* node;
	fprintf(stderr, "verifying number of workers\n");
	iterator = workers;
	do {
		node = ((Node*)iterator->value);
		fprintf(stderr, "%s has load %d\n", node->name, node->queue_size);
		if (node->queue_size > max)
			max = node->queue_size;
		if (node->queue_size < min)
			min = node->queue_size;
		iterate(&iterator);
	} while (iterator != NULL);
	fprintf(stderr, "num of workers between %i and %i\n", min, max);
	if (min >= limit_max)
		request_workers(pool_manager, pool_step);
	if (max <= limit_min && num_workers > 1)
		release_workers(pool_manager, -pool_step);
}

void onarrival() {}
