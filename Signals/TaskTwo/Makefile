CC := gcc
CFLAGS := -Wall

all: receiver sender

receiver: receiver.o
	$(CC) $^ -o $@

sender: sender.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf *.o receiver sender