# ---------------------------------------------------------- 
#   (C)1995 Institute for New Generation Computer Technology 
#       (Read COPYRIGHT for detailed information.) 
#   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
#       (Read COPYRIGHT-JIPDEC for detailed information.)
#-----------------------------------------------------------
TOP=../../..

clean:
	rm -f *~ *.orig *.rej \#* \# *.mk *.sh

distclean: clean

setup:
	@cd $(TOP)/runtime; ./setupcomm -install pvm
	@cd $(TOP)/runtime; ./setupcomm -install pvm-tcp
