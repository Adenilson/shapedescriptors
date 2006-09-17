blob_analysis: blob_demo.o blob_filter.o window.o
	g++ -o blob_analysis blob_demo.o blob_filter.o window.o -lcv0.9 -lhighgui0.9 -lstdc++

clean:
	rm -f *.o *.out *~ blob_analysis sample_image.jpg TAGS

tags:
	sh tagger.sh
