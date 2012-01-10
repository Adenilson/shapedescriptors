blob_analysis: blob_demo.o blob_filter.o window.o post_process.o
	g++ -o blob_analysis blob_demo.o blob_filter.o window.o \
	post_process.o -lml -lcvaux -lhighgui -lcv -lcxcore -lstdc++

clean:
	rm -f *.o *.out *~ blob_analysis sample_image.jpg TAGS *.bmp *.png

tags:
	find . -name '*.cpp' -o -name '*.h' -o -name '*.c' | xargs etags --append
