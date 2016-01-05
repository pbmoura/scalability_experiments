#include "loadbalancer-elastic.c"

double contention, coherency, s1, x1, max_throughput, max_workers;

int usl_peak() {
	return lrint(sqrt((1 - contention) / coherency));
}

double usl(int n) {
	return n / (1 + contention * (n - 1) + n * coherency * (n - 1));
}

double service_time(int w) { //w is the number of workers
	double n = (double) w;
	fprintf(stderr, "service time for %d\n", w);
	return s1 + s1 * contention * (n - 1) + s1 * coherency * n * (n - 1);
}

double arrival_rate(int current_load, double service_time) { //t is the service time
	fprintf(stderr, "arrival rate for %d %f\n", current_load, service_time);
	return current_load / service_time;
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
						arrival_rate(current_load,
								service_time(current_num_workers))));
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
}

void init(int argc, char *argv[]) {
	contention = atof(argv[3]);
	coherency = atof(argv[4]);
	s1 = atof(argv[5]);
	x1 = atof(argv[6]);
	max_workers = usl_peak();
	max_throughput = usl(max_workers);
	fprintf(stderr, "limit of %f workers at %f req/s\n", max_workers, max_throughput);
}
