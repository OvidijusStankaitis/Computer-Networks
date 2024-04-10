CFLAGS = -Wall -Wextra

all: client1 client2 server1 server2

client1: client1.c
	gcc $(CFLAGS) -o client1 client1.c -lpthread

client2: client2.c
	gcc $(CFLAGS) -o client2 client2.c -lpthread

server1: server1.c
	gcc $(CFLAGS) -o server1 server1.c -lpthread

server2: server2.c
	gcc $(CFLAGS) -o server2 server2.c -lpthread

clean:
	rm -f client1 client2 server1 server2
	clear