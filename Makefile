all: client worker lb play pool elastic load

client:
	gcc src/client.c -o bin/client -lrt -lpthread
	
play:
	gcc src/trace-player-pool.c -o bin/play -lrt -lpthread -g

worker:
	gcc src/worker-nosync.c -o bin/worker -lrt -lpthread -lm

lb:
	gcc src/loadbalancer.c -o bin/loadbalancer -lrt -lpthread
	
elastic:
	gcc src/loadbalancer-usl.c -o bin/elastic -lrt -lpthread -lm

load:
	gcc src/loadbalancer-single.c -o bin/load -lrt -lpthread
	
pool:
	gcc src/pool-manager.c -o bin/pool -lrt -lpthread

clean:
	rm bin/*
