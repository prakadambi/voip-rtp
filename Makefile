BIN		= rtpclient
OBJECTS 	= rtpclient.o RTP.o Hdr_Builder.o Network.o Utils.o 
BIN2		= rtpserv
OBJECTS2	= rtpserv.o RTP.o Hdr_Builder.o Network.o Utils.o
CC = gcc
LIBS = -lpulse -lpulse-simple

all: send receive

send: $(OBJECTS)
	$(CC) $(OBJECTS)  $(LIBS) -o $(BIN)

rtpclient: rtpclient.c 
	$(CC) -c rtpclient.c -w

RTP.o: RTP.c
	$(CC) -c RTP.c -w

Hdr_Builder.o: Hdr_Builder.c
	$(CC) -c Hdr_Builder.c

Network.o: Network.c
	$(CC) -c Network.c

Utils.o: Utils.c
	$(CC) -c Utils.c

receive: $(OBJECTS2)
	$(CC) $(OBJECTS2) $(LIBS) -o $(BIN2)

rtpserv.o: rtpserv.c
	$(CC) -c rtpserv.c -w

clean:
	rm -f $(OBJECTS) $(OBJECTS2) $(BIN) $(BIN2) *~ *.core
