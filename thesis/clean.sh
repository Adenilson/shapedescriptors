rm -fr aclocal.m4* config.* configure install-sh Makefile Makefile.in missing mkinstalldirs autom4te.cache/ COPYING INSTALL depcomp
rm -fr *~
rm -fr data/*txt data/*.png
rm -fr build

#Automake-1.9 doesn't require this
#echo "Oh my" > NEWS
#echo "blessed" > README
#echo "and holy" > AUTHORS
#echo "God!" > ChangeLog

mkdir build
