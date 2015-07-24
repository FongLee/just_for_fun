BIN = test
all: test

CC = gcc
CFLAGS = -c -g -Wall -std=gnu99
LDFLAGS = 
OBJS = linklist.o queuelist.o
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

test: test.o $(OBJS)
	$(CC)  $^ -o $@

clean:
	rm -f *.o $(BIN)


