all: seqlist

BIN = seqlist
CC = gcc
CFLAGS = -g -Wall -std=gnu99 -c

INCS = ../error
OBJS = seqlist.o \
	 	$(INCS)/error.o

%.o: %.c
	$(CC) $(CFLAGS) -I $(INCS) $< -o $@

seqlist: $(OBJS) test.o
	$(CC) -I $(INCS) $^ -o $@

clean:
	rm -f $(BIN) *.o