FLAGS=-std=c++1y -ltbb -g
all: addvec

clean:
	rm addvec.o addvec

addvec: addvec.o
	g++ ${FLAGS} -o addvec addvec.o

addvec.o: addvec.cpp
	g++ ${FLAGS} -c addvec.cpp

