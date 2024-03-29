CC=gcc
CFLAGS=-Wall -Wextra -pedantic
BIN=ipk24chat-client

all:
	$(CC) $(CFLAGS) -o $(BIN) main.c
clean: 
	rm -f $(BIN)
run:
	$(CC) $(CFLAGS) -o $(BIN) main.c
	./ipk24chat-client -s 0 -t udp