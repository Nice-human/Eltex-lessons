BIN := server
SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
CC := gcc
CFLAGS := -Wall -Wextra -g
LDFLAGS := -lrt

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf *.o $(BIN)