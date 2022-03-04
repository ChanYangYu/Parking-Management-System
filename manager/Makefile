GCC = g++
CFLAGS = -Wall -c -I"include"

manager: manager.o
	$(CC) -o $@ $^
.c.o: manager.c
	$(CC) $(CFLAGS) $^
	
clean:
	rm -r *.o manager .paswd