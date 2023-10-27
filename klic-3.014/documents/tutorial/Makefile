all: KL1TUT.info tutorial.dvi

dist: all

KL1TUT.info: tutorial.tex
	makeinfo --no-split tutorial.tex

tutorial.dvi: tutorial.tex
	tex tutorial.tex
	texindex tutorial.??
	tex tutorial.tex

clean:
	- /bin/rm -f tutorial.?? tutorial.log tutorial.aux \
		tutorial.cps tutorial.fns tutorial.kys \
		tutorial.mds tutorial.pgs tutorial.toc \
		tutorial.tps tutorial.vrs *~ \#*\#

distclean: clean

realclean: clean
	- /bin/rm -f tutorial.dvi KL1TUT.info
