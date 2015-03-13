.PHONY: all p2p
all: client server p2pclient p2pserver
p2p: p2pclient p2pserver
# p2pclient p2pserver
CC = gcc
CFLAGS = -Wall -std=gnu99 -c -g 

OBJS = commsocket.o 


client: $(OBJS) client.o
	$(CC) $^ -o $@

server: $(OBJS) server.o
	$(CC) $^ -o $@

p2pclient: $(OBJS) p2pclient.o
	$(CC) $^ -o $@

p2pserver: $(OBJS) p2pserver.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@


clean:
	rm -f *.o  server client p2pclient p2pserver
