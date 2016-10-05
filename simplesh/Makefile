all: simplesh
simplesh.o: simplesh.cpp
	g++ simplesh.cpp -o simplesh.o -c

simplesh: simplesh.o
	g++ -s simplesh.o -o simplesh

clean:
	rm -f *.o simplesh
