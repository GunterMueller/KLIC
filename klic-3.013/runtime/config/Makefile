# ---------------------------------------------------------- 
#   (C)1995 Institute for New Generation Computer Technology 
#       (Read COPYRIGHT for detailed information.) 
#   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
#       (Read COPYRIGHT-JIPDEC for detailed information.)
#-----------------------------------------------------------
SUBDIR=pvm pvm-tcp shm

all:
clean:
	for d in $(SUBDIR); do (cd $$d; [ ! -f Makefile ] || make clean); done
	rm -f *~ \#* *.orig *.rej

distclean:
	for d in $(SUBDIR); \
	do (cd $$d; [ -f Makefile ] && make distclean)|| true;\
	done
	rm -f *~ *.orig *.rej
