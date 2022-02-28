all: server client manager
GCC=gcc
CFLAGS= -Wall -c -g

server: server.o serverFunc.o _parson.o _linkedList.o
	$(CC) -o $@ $^

client: client.o
	$(CC) -o $@ $^

manager: manager.o
	$(CC) -o $@ $^
.c.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -r *.o server client manager