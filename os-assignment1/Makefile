CC=gcc
CFLAGS=-Wall -pthread

all: client server

client: client.c
	$(CC) $(CFLAGS) client.c -o client -lm

server: server.c
	$(CC) $(CFLAGS) server.c -o server -lm

clean:
	rm -f client server

