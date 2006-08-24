all: complexer 1dfft
	echo "Compiling everything..."

complexer: complex.o
	g++ -o complexer complex.o

1dfft: 1dfft.o
	g++ -o 1dfft 1dfft.o -lfftw3

clean:
	rm -fr *~ *.o complexer 1dfft
