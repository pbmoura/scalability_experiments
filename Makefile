all: client worker lb

client:
	gcc src/client.c -o bin/client -lrt -lpthread

worker:
	gcc src/worker.c -o bin/worker -lrt -lpthread

lb:
	gcc src/loadbalancer.c -o bin/loadbalancer -lrt -lpthread

clean:
	rm bin/*
