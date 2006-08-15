CC=g++
#CFLAGS=-c -O3
CFLAGS=-O3
all: extractor

extractor: beta.o contour.o output.o descriptors.o
	$(CC) $(CFLAGS) beta.o contour.o output.o descriptors.o -lcv0.9 -lhighgui0.9 -o extractor.out

gextractor: beta.o contour.o output.o descriptors.o
	$(CC) beta.o contour.o output.o descriptors.o -lcv0.9 -lhighgui0.9 -o gextractor.out
 
clean:
	rm -fr *.o *.*~ extractor.out gextractor.out *.sci *.txt *.png TAGS

tags: clean
	find . -name '*.cpp' -o -name '*.h' -o -name '*.c' | xargs etags --append

