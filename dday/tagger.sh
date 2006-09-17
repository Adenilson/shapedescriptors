#ETAGS=/cygdrive/c/emacs-21.3/bin/etags.exe
ETAGS=etags	# Exuberant ctags
rm TAGS
find . -name '*.cpp' -o -name '*.h' -o -name '*.c' | xargs etags --append

		    
