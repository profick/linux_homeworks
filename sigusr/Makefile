all: sigusr
sigusr.o: sigusr.c
	gcc sigusr.c -o sigusr.o -c

sigusr: sigusr.o
	gcc -s sigusr.o -o sigusr

clean:
	rm -f *.o sigusr
