.PHONY: all 
BIN = client server p2pclient p2pserver client_test server1 server2 \
			server3 server4 server5 server6 server7
all: client_test server7

# p2pclient p2pserver
CC = gcc
CFLAGS = -Wall -std=gnu99 -c -g 
ERRORDIR = ../error
QUEUEDIR = ../queue
OBJS = commsocket.o \
		$(ERRORDIR)/error.o \
		$(QUEUEDIR)/linklist.o \
		$(QUEUEDIR)/queuelist.o \

INCS = -I ../error -I ../queue
LDFLAGS = -pthread

client: $(OBJS) client.o
	$(CC) $^ -o $@

client_test: $(OBJS) client_test.o
	$(CC) $^ -o $@

server1: $(OBJS) server1.o
	$(CC) $^ -o $@

server2: $(OBJS) server2.o
	$(CC) $(LDFLAGS) $^ -o $@

server3: $(OBJS) server3.o
	$(CC) $(LDFLAGS) $^ -o $@

server4: $(OBJS) server4.o
	$(CC) $(LDFLAGS) $^ -o $@

server5: $(OBJS) server5.o
	$(CC) $(LDFLAGS) $^ -o $@

server6: $(OBJS) server6.o
	$(CC) $(LDFLAGS) $^ -o $@

server7: $(OBJS) server7.o
	$(CC) $(LDFLAGS) $^ -o $@

p2pclient: $(OBJS) p2pclient.o
	$(CC) $^ -o $@

p2pserver: $(OBJS) p2pserver.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INCS) $< -o $@

clean:
	rm -f $(OBJS) *.o  $(BIN) 