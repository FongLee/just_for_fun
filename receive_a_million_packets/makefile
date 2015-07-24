all: recv send

BIN = recv send

CC = gcc
#CC = arm-linux-gcc
#CFLAGS = -g -Wall -c -std=gnu99  -march=i686
CFLAGS = -g -Wall -c  

OBJS = net.o
INCS = 
LDFLAGS =  -pthread

%.o: %.c
	$(CC) $(CFLAGS) $(INCS) $< -o $@

recv: $(OBJS) udpreceive.o
	$(CC) $(INCS) $(LDFLAGS) $^ -o $@

send: $(OBJS) udpsender.o
	$(CC) $(INCS) $(LDFLAGS) $^ -o $@

clean: 
	rm -f $(BIN) $(OBJS) *.o