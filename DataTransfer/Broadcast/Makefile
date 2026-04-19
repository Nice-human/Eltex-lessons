BIN := server client
SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
CC := gcc
CFLAGS := -Wall

all: $(BIN)

server: server.o
	$(CC) $^ -o $@

udp_client: udp_client.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run-server: server
	./server

run-udp: client
	./client

.PHONY: clean run-server run-udp
clean:
	rm -rf *.o server client