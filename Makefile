32:
	cd server; make
	cd manager; make
	cd client; make 32;

64:
	cd server; make
	cd manager; make
	cd client; make 64;

clean:
	cd server; make clean
	cd manager; make clean
	cd client; make clean