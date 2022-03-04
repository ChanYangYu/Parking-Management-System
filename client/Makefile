CPP = g++
FLAGS = $(OFLAGS) -L"/usr/lib" $(HOME)/linux-gcc-32/libIrrKlang.so -lglut -lGL -lGLU
FLAGS2 = $(OFLAGS) -L"/usr/lib" $(HOME)/linux-gcc-64/libIrrKlang.so -lglut -lGL -lGLU -pthread
OFLAGS = -I"include"

32: userclient.o
	cp -r bin/linux-gcc-32 $(HOME)
	$(CPP) client.cpp userclient.o -o client $(FLAGS)
	$(CPP) checkkey.cpp userclient.o -o checkkey $(FLAGS)
	$(CPP) calculatetime.cpp userclient.o -o calculatetime $(FLAGS)

64: userclient.o
	cp -r bin/linux-gcc-64 $(HOME)
	$(CPP) client.cpp userclient.o -o client $(FLAGS2)
	$(CPP) checkkey.cpp userclient.o -o checkkey $(FLAGS2)
	$(CPP) calculatetime.cpp userclient.o -o calculatetime $(FLAGS2)

.cpp.o:
	$(CPP) -c userclient.cpp $(OFLAGS)

clean:
	rm -r *.o client checkkey calculatetime *.txt
