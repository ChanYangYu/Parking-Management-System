all:
	cd server; make
	cd manager; make
	cd client; make;

clean:
	cd server; make clean
	cd manager; make clean
	cd client; make clean