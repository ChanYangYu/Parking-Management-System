GCC = g++
CFLAGS = -Wall -c -I"include"
SRCS = server.c serverFunc.c _parson.c _linkedList.c
OBJS = $(SRCS:.c=.o)

server: server.o serverFunc.o _parson.o _linkedList.o
	$(CC) -o $@ $^
.c.o: $(SRCS)
	$(CC) $(CFLAGS) $^
	
clean:
	rm -r *.o server