BIN := server tcp_client udp_client
SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
CC := gcc
CFLAGS := -Wall

all: $(BIN)

server: server.o
	$(CC) $^ -o $@

tcp_client: tcp_client.o
	$(CC) $^ -o $@

udp_client: udp_client.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run-server: server
	./server

run-tcp: tcp_client
	./tcp_client

run-udp: udp_client
	./udp_client

.PHONY: clean run-server run-tcp run-udp
clean:
	rm -rf *.o server tcp_client udp_client