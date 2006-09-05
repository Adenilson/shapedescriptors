CC=g++ -g

all: complexer 1dfft function
	echo "Compiling everything..."

debug: 1debug

function: function.o
	g++ -o function function.o

complexer: complex.o
	g++ -o complexer complex.o

1dfft: 1dfft.o
	g++ -o 1dfft 1dfft.o -lfftw3

1debug: 1debug.o
	$(CC) -o debugdfft 1dfft.o -lfftw3

1debug.o: 1dfft.cpp
	$(CC) -o 1dfft.o -c 1dfft.cpp
clean:
	rm -fr *~ *.o complexer 1dfft debugdfft function
