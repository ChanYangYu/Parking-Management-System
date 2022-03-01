all: server client manager

GCC=g++
CFLAGS= -Wall -c -g

server: server.o serverFunc.o _parson.o _linkedList.o
	$(CC) -o $@ $^

client: client.o userclient.o 
	$(CC) -o $@ $^

manager: manager.o
	$(CC) -o $@ $^
.c.o:
	$(CC) $(CFLAGS) $<
.cpp.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -r *.o server client manager
