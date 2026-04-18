BIN := server
SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
CC := gcc
LDFLAGS := -lrt

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	rm -rf *.o $(BIN)