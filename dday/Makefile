blob_analysis: blob_demo.o
	g++ -o blob_analysis blob_demo.o -lcv0.9 -lhighgui0.9

clean:
	rm -f *.o *.out *~ blob_analysis sample_image.jpg TAGS

tags:
	sh tagger.sh
