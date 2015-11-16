all: client worker lb play

client:
	gcc src/client.c -o bin/client -lrt -lpthread
	
play:
	gcc src/trace-player.c -o bin/play -lrt -lpthread

worker:
	gcc src/worker.c -o bin/worker -lrt -lpthread

lb:
	gcc src/loadbalancer-elastic.c -o bin/loadbalancer -lrt -lpthread -lm

clean:
	rm bin/*
