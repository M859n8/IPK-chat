CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g 
BIN=ipk24chat-client

all:
	$(CC) $(CFLAGS) -o $(BIN) tcp.c main.c
clean: 
	rm -f $(BIN)
run:
	$(CC) $(CFLAGS) -o $(BIN) tcp.c main.c
	./ipk24chat-client -s "127.0.0.1" -t tcp