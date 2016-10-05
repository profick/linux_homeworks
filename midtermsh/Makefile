all: midtermsh
midtermsh.o: midtermsh.cpp
	g++ midtermsh.cpp -o midtermsh.o -c

midtermsh: midtermsh.o
	g++ -s midtermsh.o -o midtermsh

clean:
	rm -f *.o midtermsh
