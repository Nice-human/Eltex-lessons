CC := gcc
CFLAGS := -Wall -Wextra

all: server client

server: server.c
	$(CC) $(CFLAGS) server.c -o server

client: client.c
	$(CC) $(CFLAGS) client.c -o client

.PHONY: clean
clean:
	rm -rf server client