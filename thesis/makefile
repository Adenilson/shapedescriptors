CC=g++
#CFLAGS=-c -O3
CFLAGS=-O3
all: extractor

extractor: beta.o contour.o output.o
	$(CC) $(CFLAGS) beta.o contour.o output.o -lcv0.9 -lhighgui0.9 -o extractor.out

gextractor: beta.o contour.o output.o
	$(CC) beta.o contour.o output.o -lcv0.9 -lhighgui0.9 -o gextractor.out
 
clean:
	rm -fr *.o *.*~ extractor.out gextractor.out *.sci *.txt


