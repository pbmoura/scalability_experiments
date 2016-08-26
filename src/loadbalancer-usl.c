#include "loadbalancer-elastic.c"

double contention, coherency, s1, x1, max_throughput, max_workers;
sem_t *sem_arrival;
int arrivals, worker_capacity, monitoring_fraction;
long last_cycle;

int usl_peak() {
	return lrint(sqrt((1 - contention) / coherency));
}

double usl(int n) {
	return n / (1 + contention * (n - 1) + n * coherency * (n - 1));
}

double arrival_rate() {
	return (float)arrivals / ((float)monitoring_interval/monitoring_fraction/1000.0);
}

double estimate_workers(double x) { //x is required throughput
	double w, delta, c = (x / x1);
	if (x >= max_throughput) {
		fprintf(stderr, "load exceeds limit\n");
		w = max_workers;
	} else {
		delta = pow(contention - coherency - 1 / c, 2.0)
				- 4 * coherency * (1 - contention);
		w = (-contention + coherency + 1 / c - sqrt(delta)) / (2 * coherency);
	}
	fprintf(stderr, "estimating %f workers with delta %f\n", w, delta);
	return floor(w * 10) / 10;
}

//int estimate_num_workers(int current_num_workers, int current_load) {
int estimate_num_workers() {
	int n;
	if (load == 0)
		n = 1;
	else {
		n = ceil(
				estimate_workers(
						arrival_rate()));
		if (n < 1)
			n = 1;
	}
	return n;
}

long workers_available(Linked_list *iterator) {
	double available = 0;
	int queue_size;
	while (iterator != NULL) {
		queue_size = ((Node*)iterator->value)->queue_size;
		fprintf(stderr, "%ld queue size %d\n", time_millis(), queue_size);
		if (queue_size < worker_capacity)
			available += ((worker_capacity - queue_size)/(double)worker_capacity);
		iterate(&iterator);
	}
	fprintf(stderr, "%ld workers available %f\n", time_millis(), available);
	return lrint(available); 
}

void verify_num_workers() {
	int n, diff;

	//n = estimate_num_workers(num_workers, load);
	n = estimate_num_workers();
	diff = n - workers_available(workers);
	fprintf(stderr, "%ld estimate %d diff %d\n", time_millis(), n, diff);
	/*if (load > usl(num_workers) * x1)
		request_workers(pool_manager, n);
	else*/
		if (diff > 0) {
				request_workers(pool_manager, diff);
		} else if (diff < 0) {
			if (load > n * worker_capacity) {
				diff = load / worker_capacity - num_workers;
			}
			if (diff < 0)
				release_workers(pool_manager, diff);
		}
	sem_wait(sem_arrival);
	arrivals = 0;
	last_cycle = time_millis();
	sem_post(sem_arrival);
}

void onarrival() {
	long now = time_millis();
	sem_wait(sem_arrival);
	if ( (now > last_cycle) && ( (now - last_cycle)) >= (monitoring_interval / monitoring_fraction) * (monitoring_fraction-1) ) 
		arrivals++;
	sem_post(sem_arrival);
}

void init(int argc, char *argv[]) {
	contention = atof(argv[3]);
	coherency = atof(argv[4]);
	s1 = atof(argv[5]);
	x1 = atof(argv[6]);
	worker_capacity = s1 * x1;//atoi(argv[7]);
	monitoring_fraction = atoi(argv[8]);
	max_workers = usl_peak();
	max_throughput = usl(max_workers) * x1;
	last_cycle = time_millis();
	sem_arrival = createsemaphore("/sem_arrival", 1);
	fprintf(stderr, "limit of %f workers at %f req/s\n", max_workers, max_throughput);
}
