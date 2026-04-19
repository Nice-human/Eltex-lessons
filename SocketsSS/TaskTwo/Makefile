BIN := server client1 client2 client3
SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
CC := gcc
CFLAGS := -Wall -pthread

all: $(BIN)

server: server.o
	$(CC) $^ -o $@ $(CFLAGS)

client1: client1.o
	$(CC) $^ -o $@

client2: client2.o
	$(CC) $^ -o $@

client3: client3.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run-server: server
	./server

run-clients: client1 client2 client3
	./client1
	./client2
	./client3

.PHONY: clean run-server run-clients
clean:
	rm -rf *.o server client1 client2 client3