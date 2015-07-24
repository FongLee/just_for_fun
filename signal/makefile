.PHONY: all
all: sigwaitinfo_test handler_sig_test

# p2pclient p2pserver
CC = gcc
CFLAGS = -Wall -std=gnu99 -c -g 

OBJS = test_sigwaitinfo.o sync_handle_signal.o 

LDFLAGS = -pthread

sigwaitinfo_test: test_sigwaitinfo.o
	$(CC) $^ -o $@

handler_sig_test: sync_handle_signal.o 
	$(CC) $^ -o $@
	
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o  test_sigwaitinfo
