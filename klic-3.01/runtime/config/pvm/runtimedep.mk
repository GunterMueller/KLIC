# ---------------------------------------------------------- 
#   (C) 1995 Institute for New Generation Computer Technology 
#	(Read COPYRIGHT for detailed information.) 
#   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
#       (Read COPYRIGHT-JIPDEC for detailed information.)
# ----------------------------------------------------------
distpkt.o: distpkt.c \
    ../include/klic/basic.h ../include/klic/config.h \
    ../include/klic/struct.h ../include/klic/param.h \
    ../include/klic/primitives.h ../include/klic/alloc.h \
    ../include/klic/control.h ../include/klic/unify.h \
    ../include/klic/timing.h ../include/klic/distio.h \
    ../include/klic/distproc.h ../include/klic/interpe.h \
    ../include/klic/distpkt.h ../include/klic/rmon.h
distio.o: distio.c \
    ../include/klic/basic.h ../include/klic/config.h \
    ../include/klic/struct.h ../include/klic/param.h \
    ../include/klic/distio.h ../include/klic/distproc.h 
distproc.o: distproc.c \
    ../include/klic/basic.h ../include/klic/config.h \
    ../include/klic/struct.h ../include/klic/param.h \
    ../include/klic/primitives.h ../include/klic/alloc.h \
    ../include/klic/control.h ../include/klic/distio.h \
    ../include/klic/distproc.h ../include/klic/interpe.h \
    ../include/klic/distpkt.h ../include/klic/rmon.h \
    ../include/klic/options.h
