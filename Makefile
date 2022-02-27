all: server client manager
GCC=gcc
CFLAGS= -Wall -c -g

server: server.o
	$(CC) -o $@ $^

client: client.o
	$(CC) -o $@ $^

manager: manager.o
	$(CC) -o $@ $^
.c.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -r *.o server client manager