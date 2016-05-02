#include <stdio.h>

int main(int argc, char* argv[]) {
	int i, n, t;

	while(fscanf(stdin, "%d\n", &n) != EOF) {
		t = 1000000 / n;
		for(i = 0; i < n; i++)
			printf("%d\n", t);
	}
}
