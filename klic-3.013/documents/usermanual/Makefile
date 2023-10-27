# ---------------------------------------------------------- 
#   (C)1993, 1994, 1995 Institute for New Generation Computer Technology 
#	(Read COPYRIGHT for detailed information.) 
#   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
#       (Read COPYRIGHT-JIPDEC for detailed information.)
# ----------------------------------------------------------

MULE2 = emacs
JTEXINDEX = texindex

all: KLIC.info klic.dvi

allj: KLICj.info klicj.dvi

dist: all allj

KLIC.info: klic.tex
	makeinfo --no-split klic.tex

KLICj.info: klicj.tex
	$(MULE2) -batch -l klicj-info.el

klic.dvi: klic.tex
	tex klic.tex
	texindex klic.??
	tex klic.tex

klicj.dvi: klicj.tex
	ptex klicj.tex
#	$(JTEXINDEX) klicj.cp
#	jtex klicj.tex
#	nkf -e klicj.cp > klicj.cpe
#	/bin/mv -f klicj.cpe klicj.cp
# 	$(JTEXINDEX) klicj.cp
	$(JTEXINDEX) klicj.??
	ptex klicj.tex

clean:
	- /bin/rm -f klic.?? klic.log klic.aux \
		klic.cps klic.fns klic.kys klic.mds klic.pgs klic.toc \
		klic.tps klic.vrs  *~ \#*\# \
		klicj.?? klicj.log klicj.aux \
		klicj.cps klicj.fns klicj.kys klicj.mds klicj.pgs klicj.toc \
		klicj.tps klicj.vrs

distclean: clean

realclean: clean
	- /bin/rm -f klic.dvi KLIC.info klicj.dvi KLICj.info
