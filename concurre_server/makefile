.PHONY: all 
BIN = client_test server server2
all: client_test server 

# p2pclient p2pserver
CC = g++
CFLAGS = -Wall  -c -g 
ERRORDIR = ../error

OBJS = $(ERRORDIR)/error.o \
		 

INCS = -I ../error -I ../queue
LDFLAGS =  -pthread

client_test: $(OBJS) commsocket.o client_test.o
	$(CC) $(LDFLAGS) $^ -o $@

server: $(OBJS) commsocket.o server.o
	$(CC) $(LDFLAGS) $^ -o $@

server2: $(OBJS) commsocket2.o server2.o
	$(CC) $(LDFLAGS) $^ -o $@

server6: $(OBJS) commsocket.o server6.o
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCS) $< -o $@

clean:
	rm -f $(OBJS) *.o  $(BIN)