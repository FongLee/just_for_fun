all: tcp concurre queue seqlist


TCPDIR = ./tcp
CONSERVERDIR = ./concurre_server
QUEUEDIR = ./queue
SEQLISTDIR = ./seqlist

tcp:
	cd $(TCPDIR) && make

concurre:
	cd $(CONSERVERDIR) && make

queue:
	cd $(QUEUEDIR) && make

seqlist:
	cd $(SEQLISTDIR) && make

clean:

	cd $(TCPDIR) && make clean
	cd $(CONSERVERDIR) && make clean
	cd $(QUEUEDIR) && make clean
	cd $(SEQLISTDIR) && make clean
