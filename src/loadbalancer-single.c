#include "loadbalancer-elastic.c"

double limit_max, limit_min;

void init(int argc, char *argv[]) {
	limit_min = atof(argv[3]);
	limit_max = atof(argv[4]);
	fprintf(stderr, "load limits %i - %i\n", limit_min, limit_max);
}

void verify_num_workers() {

	double requests_per_worker = (double)load/num_workers;
	fprintf(stderr, "%ld requests per worker %f\n", time_millis(), requests_per_worker);
	if (requests_per_worker >= limit_max)
		request_workers(pool_manager, 1);
	else if (requests_per_worker <= limit_min && num_workers > 1)
		release_workers(pool_manager, -1);

}

void onarrival() {}
