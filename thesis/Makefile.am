#Description:
#	 buildsystem using autotools. Please run
#	 '$autoreconf -i --force' to generate
#	 build files (configure script).
#
#Author: Adenilson Cavalcanti savagobr@yahoo.com
#
#TODO:
# source files in a directory (curse you autoreconf2.5!)
# support linking with efence
# support 'check' unit tests

AM_CPPFLAGS = $(OCV_CFLAGS) -Wall

bin_PROGRAMS = contour_extractor

contour_extractor_SOURCES = $(csourcedir)/base.h $(csourcedir)/beta.cpp \
	$(csourcedir)/contour.cpp $(csourcedir)/contour.h \
	$(csourcedir)/descriptors.cpp $(csourcedir)/descriptors.h \
	$(csourcedir)/output.cpp $(csourcedir)/output.h \
	$(csourcedir)/seq.h $(csourcedir)/vision.h \
	$(csourcedir)/ window.h
contour_extractor_LDADD = $(OCV_LIBS)
contour_extractor_CPPFLAGS = $(AM_CPPFLAGS)
