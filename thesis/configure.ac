#Description:
#	 buildsystem using autotools. Please run
#	 '$autoreconf -i --force' to generate
#	 build files (configure script).
#
#Author: Adenilson Cavalcanti savagobr@yahoo.com
#
#TODO:
# support linking with efence
# support 'check' unit tests

AC_PREREQ(2.58)
AC_INIT(, 0.01, [],)
AC_CONFIG_SRCDIR([src/beta.cpp])

AM_INIT_AUTOMAKE([1.7.9 foreign])
#dnl AC_ISC_POSIX
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC

export PKG_CONFIG_PATH=/usr/lib/pkgconfig

#Libraries
PKG_CHECK_MODULES(OCV, opencv)
AC_SUBST(OCV_CFLAGS)
AC_SUBST(OCV_LIBS)

PKG_CHECK_MODULES(FFTW, fftw3)
AC_SUBST(FFTW_CFLAGS)
AC_SUBST(FFTW_LIBS)

#XXX: check does not suport pkg-config!
#AC_CHECK_LIB(function, library, [CHECK_LIBS="-lcheck"],
#            AC_MSG_ERROR([have not found check!]), [])
#if test -n "$CHECK_LIBS"; then
#        AC_SUBST(CHECK_LIBS)
#fi


#Directories
AC_SUBST(csourcedir, $srcdir/src)
AC_SUBST(utestdir, $srcdir/utest)

AC_HEADER_STDC

AC_OUTPUT([
Makefile
])

