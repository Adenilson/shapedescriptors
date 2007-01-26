#Description:
#	 buildsystem using autotools. Please run
#	 '$autoreconf -i --force' to generate
#	 build files (configure script).
#
#Author: Adenilson Cavalcanti savagobr@yahoo.com
#
#TODO:
# debug build target
# support linking with efence

AM_CPPFLAGS = -Wall -O2 -Weffc++
bin_PROGRAMS = contour_extractor utester

contour_extractor_SOURCES = $(csourcedir)/base.h $(csourcedir)/beta.cpp \
	$(csourcedir)/contour.cpp $(csourcedir)/contour.h \
	$(csourcedir)/descriptors.cpp $(csourcedir)/descriptors.h \
	$(csourcedir)/output.cpp $(csourcedir)/output.h \
	$(csourcedir)/seq.h $(csourcedir)/vision.h \
	$(csourcedir)/ window.h
contour_extractor_LDADD = $(OCV_LIBS)
contour_extractor_CPPFLAGS = $(AM_CPPFLAGS) $(OCV_CFLAGS)


utester_SOURCES = $(csourcedir)/fourier.h $(utestdir)/fft_test.cpp \
	$(csourcedir)/mcomplex.h $(utestdir)/square.h $(utestdir)/circle.h

utester_LDADD = $(FFTW_LIBS) -lcheck -lpthread
utester_CPPFLAGS = $(AM_CPPFLAGS) $(FFTW_CFLAGS)
