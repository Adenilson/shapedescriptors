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

AC_PREREQ(2.58)
AC_INIT(, 0.01, [],)
AC_CONFIG_SRCDIR(.)

AM_INIT_AUTOMAKE([foreign])

#dnl AC_ISC_POSIX
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC

export PKG_CONFIG_PATH=/usr/lib/pkgconfig

PKG_CHECK_MODULES(OCV, opencv)
AC_SUBST(OCV_CFLAGS)
AC_SUBST(OCV_LIBS)

AC_HEADER_STDC

AC_OUTPUT([
Makefile
])

