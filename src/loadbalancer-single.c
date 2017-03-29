#include "loadbalancer-elastic.c"

double limit_max, limit_min;
int pool_step;

void init(int argc, char *argv[]) {
	fprintf(stderr, "init %s, %s\n", argv[3], argv[4]);
	limit_min = atof(argv[3]);
	limit_max = atof(argv[4]);
	pool_step = atoi(argv[5]);
	fprintf(stderr, "load limits %f - %f	\n", limit_min, limit_max);
}

void verify_num_workers() {

	double requests_per_worker = (double)load/num_workers;
	fprintf(stderr, "%ld requests per worker %f\n", time_millis(), requests_per_worker);
	if (requests_per_worker >= limit_max)
		request_workers(pool_manager, pool_step);
	else if (requests_per_worker <= limit_min && num_workers > 1)
		release_workers(pool_manager, -pool_step);

}

void onarrival() {}
