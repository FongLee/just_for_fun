.PHONY: all 
BIN = client1 server1 server2 server3 client3 server4
all: client1 server1 

# p2pclient p2pserver
CC = g++
CFLAGS = -Wall -g -c
ERRORDIR = ../error

OBJS = $(ERRORDIR)/error.o \
		 
INCS = -I ../error -I ../queue
LDFLAGS =  -pthread

client1: $(OBJS) commsocket.o client_test.o
	$(CC) $(LDFLAGS) $^ -o $@

server1: $(OBJS) commsocket.o server1.o
	$(CC) $(LDFLAGS) $^ -o $@

server2: $(OBJS) commsocket2.o server2.o
	$(CC) $(LDFLAGS) $^ -o $@

client3: $(OBJS) commsocket2.o client_test3.o
	$(CC) $(LDFLAGS) $^ -o $@

server3: $(OBJS) commsocket2.o server3.o
	$(CC) $(LDFLAGS) $^ -o $@

server4: $(OBJS) commsocket2.o server4.o
	$(CC) $(LDFLAGS) $^ -o $@

server6: $(OBJS) commsocket.o server6.o
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(INCS) $(CFLAGS)   $< -o $@

clean:
	rm -f $(OBJS) *.o  $(BIN) *.txt