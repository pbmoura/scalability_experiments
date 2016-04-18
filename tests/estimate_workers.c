#include <stdio.h>
#include <math.h>

double contention, coherency, x1, max_throughput, max_workers;

double estimate_workers(double x) { //x is required throughput
	double p, w, delta, c = (x / x1);
	fprintf(stderr, "workers estimate for %f\n", x);
	printf("capacity %f\n", c);
	if (x >= max_throughput) {
		fprintf(stderr, "load exceeds limit\n");
		w = max_workers;
	} else {
		p = pow(contention - coherency - 1 / c, 2.0);
		delta = p - 4 * coherency * (1 - contention);
		w = (-contention + coherency + 1 / c - sqrt(delta))
			/ (2 * coherency);
	}
	fprintf(stderr, "estimating %f workers with delta %f and p %f\n", w, delta, p);
	return floor(w * 10) / 10;
}

int main(int argc, char* argv[]) {
	double x;
	contention = 0.04;
	coherency = 0.01;
	x1 = 0.8;
	max_throughput = 4.424779;
	max_workers = 10;

	printf("%f\n", 4 * coherency * (1 - contention));

	for(x=3.5; x<4.3; x+=0.1) {
		estimate_workers(x);
	}

}
