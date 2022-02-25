all: server client
GCC=gcc
CFLAGS= -Wall -c -g

server: server.o _ipc.o
	$(CC) -o $@ $^

client: client.o _ipc.o
	$(CC) -o $@ $^

.c.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -r *.o server client