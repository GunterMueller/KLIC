# ---------------------------------------------------------- 
#   (C)1995 Institute for New Generation Computer Technology 
#       (Read COPYRIGHT for detailed information.) 
#   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
#       (Read COPYRIGHT-JIPDEC for detailed information.)
#-----------------------------------------------------------
TOP=../../..
all:
clean:
	/bin/rm -f *~ *.rej *.orig \#
distclean: clean
	/bin/rm -f $(TOP)/runtime/lock.s
	/bin/rm -f lock.s

setup:
	@cd $(TOP)/runtime; ./setupcomm -install shm
