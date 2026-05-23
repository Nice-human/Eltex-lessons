BIN := server client1 client2
SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
CC := gcc
CFLAGS := -Wall

all: $(BIN)

server: server.o
	$(CC) $^ -o $@ $(CFLAGS)

client1: client1.o
	$(CC) $^ -o $@

client2: client2.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: 
clean:
	rm -rf *.o server client1 client2