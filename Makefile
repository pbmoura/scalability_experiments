all: client worker lb play pool elastic load

client:
	gcc src/client.c -o bin/client -lrt -lpthread -dH
	
play:
	gcc src/trace-player.c -o bin/play -lrt -lpthread -g -dH

worker:
	gcc src/worker-nosync.c -o bin/worker -lrt -lpthread -lm -g -dH -rdynamic

lb:
	gcc src/loadbalancer.c -o bin/loadbalancer -lrt -lpthread -dH
	
elastic:
	gcc src/loadbalancer-usl.c -o bin/elastic -lrt -lpthread -lm -g -dH

load:
	gcc src/loadbalancer-single.c -o bin/load -lrt -lpthread -dH
	
pool:
	gcc src/pool-manager.c -o bin/pool -lrt -lpthread -dH

clean:
	rm bin/*
