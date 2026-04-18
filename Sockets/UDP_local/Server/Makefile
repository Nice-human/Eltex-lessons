BIN := server
SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
CC := gcc
CFLAGS := -Wall -Wextra

$(BIN): $(OBJ)
	$(CC) $^ -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf *.o $(BIN)