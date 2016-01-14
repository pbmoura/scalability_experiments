#include "loadbalancer-elastic.c"

double contention, coherency, s1, x1, max_throughput, max_workers;
sem_t *sem_arrival;
int arrivals;

int usl_peak() {
	return lrint(sqrt((1 - contention) / coherency));
}

double usl(int n) {
	return n / (1 + contention * (n - 1) + n * coherency * (n - 1));
}

double arrival_rate() {
	//fprintf(stderr, "arrival rate for %d %f\n", current_load, service_time);
	//return current_load / service_time;
	fprintf(stderr, "arrival rate for %u %d\n", monitoring_interval, arrivals);
	return (float)arrivals / ((float)monitoring_interval/1000.0);
}

double estimate_workers(double x) { //x is required throughput
	double w, delta, c = (x / x1);
	fprintf(stderr, "workers estimate for %f\n", x);
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

int estimate_num_workers(int current_num_workers, int current_load) {
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

void verify_num_workers() {
	int n, diff;

	n = estimate_num_workers(num_workers, load);
	diff = n - num_workers;
	if (diff > 0) {
		request_workers(pool_manager, diff);
	} else if (diff < 0) {
		release_workers(pool_manager, diff);
	}
	sem_wait(sem_arrival);
	arrivals = 0;
	sem_post(sem_arrival);
}

void onarrival() {
	sem_wait(sem_arrival);
	arrivals++;
	sem_post(sem_arrival);
}

void init(int argc, char *argv[]) {
	contention = atof(argv[3]);
	coherency = atof(argv[4]);
	s1 = atof(argv[5]);
	x1 = atof(argv[6]);
	max_workers = usl_peak();
	max_throughput = usl(max_workers) * x1;
	sem_arrival = createsemaphore("/sem_arrival", 1);
	fprintf(stderr, "limit of %f workers at %f req/s\n", max_workers, max_throughput);
}
