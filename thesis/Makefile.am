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

contour_extractor_SOURCES = base.h beta.cpp contour.cpp contour.h \
 descriptors.cpp descriptors.h output.cpp output.h seq.h vision.h \
 window.h
contour_extractor_LDADD = $(OCV_LIBS)
contour_extractor_CPPFLAGS = $(AM_CPPFLAGS)
