# ----------------------------------------------------------
#   (C) 1994, 1995 Institute for New Generation Computer Technology
#       (Read COPYRIGHT for detailed information.)
#   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
#       (Read COPYRIGHT-JIPDEC for detailed information.)
# ----------------------------------------------------------

.SUFFIXES: $(SUFFIXES) .kl1 .ext

PARALLEL = 0

PROGRAMS = bb gb normalize version \
	generate obj write \
	insert macro extern kl1cmp cmain \
	util

KL1SRC = bb.kl1 gb.kl1 normalize.kl1 version.kl1 \
	generate.kl1 obj.kl1 write.kl1 \
	insert.kl1 macro.kl1 extern.kl1 kl1cmp.kl1 cmain.kl1 \
	util.kl1 msg.kl1 cmacro.kl1

KL1PPSRC = kl1pp.kl1 cmacro.kl1 macropp.kl1 \
	msg.kl1 macro.kl1 extern.kl1 obj.kl1 util.kl1 write.kl1

CSRC = bb.c gb.c normalize.c version.c \
	generate.c obj.c write.c \
	insert.c macro.c extern.c kl1cmp.c cmain.c \
	util.c msg.c cmacro.c

KL1PPCSRC = kl1pp.c cmacro.c macropp.c \
	msg.c macro.c extern.c obj.c util.c write.c

EXT = bb.ext gb.ext normalize.ext version.ext \
	generate.ext obj.ext write.ext \
	insert.ext macro.ext extern.ext kl1cmp.ext cmain.ext \
	util.ext msg.ext cmacro.ext

KL1PPEXT = kl1pp.ext cmacro.ext macropp.ext \
	msg.ext macro.ext extern.ext obj.ext util.ext write.ext

OBJECTS = bb.o gb.o normalize.o version.o \
	generate.o obj.o write.o \
	insert.o macro.o extern.o kl1cmp.o cmain.o \
	util.o msg.o cmacro.o atom.o funct.o predicates.o

KL1PPOBJS = kl1pp.o cmacro.o macropp.o \
	msg.o macro.o extern.o obj.o util.o write.o

KLIC = ../compiler/klic -P$(PARALLEL) $(KLICOPT) \
	-R -I../include -X. -D../compiler/klicdb -v 

all: klicdb klic $(KL1SELFCOMP)

install: all
	- $(INSTDIR) $(KLICBIN)
	- $(INSTDIR) $(KLICLIB)/klic
	if (test `pwd` != $(KLICBIN)) then ($(INSTALL) klic$(CYGWIN) $(KLICBIN)) fi
	if (test `pwd` != $(KLICLIB)/klic) then \
		($(INSTALL) $(KL1SELFCOMP)$(CYGWIN) klicdb$(CYGWIN) $(KLICLIB)/klic) fi

dist: klic klicdb klicdb.init main.o $(CSRC) $(KL1PPCSRC) klicdb.c klic.c \
	options.c klic.h version.c \
	atom.h funct.h atom.c funct.c predicates.c

version.kl1: ../version.sed version.kl1.tmplt
	sed -f ../version.sed <version.kl1.tmplt >version.kl1
version.h: ../version.sed version.h.tmplt
	sed -f ../version.sed <version.h.tmplt >version.h

../version.sed:
	cd ..; make version.sed

klicdb: klicdb.c klic.h
	$(CC) -I../include $(CFLAGS) \
	-o klicdb klicdb.c options.c $(LDFLAGS)
#klic:	klic.c options.c klic.h version.h
#	$(CC) -DPVMINC=\"$(PVMINC)\" -DPVMLIBDIR=\"$(PVMLIBDIR)\" \
#	-DPVMLIB=\"$(PVMLIB)\" -I../include $(CFLAGS) \
#	-o klic klic.c options.c $(LDFLAGS)

klic:	klic.c options.c klic.h version.h
	$(CC) -I../include $(CFLAGS) \
	-o klic klic.c options.c $(LDFLAGS)

kl1pp: $(KL1PPSRC) ../runtime/libklict.a
	./klic -P$(PARALLEL) -K $(KL1CMP) -D./klicdb -v \
		-X. -I../include -L../runtime \
		$(KLICOPT) -o kl1pp $(KL1PPSRC)

.kl1.c:
	./klic -K $(KL1CMP) -D./klicdb -X. -C -v $<

.kl1.ext:
	./klic -K $(KL1CMP) -D./klicdb -C -v $<

atom.h: klicdb $(EXT) klicdb.init libklic.db
	./klicdb -X. $(EXT)
funct.h: klicdb $(EXT) klicdb.init libklic.db
	./klicdb -X. $(EXT)
atom.c: klicdb $(EXT) klicdb.init libklic.db
	./klicdb -X. $(EXT)
funct.c: klicdb $(EXT) klicdb.init libklic.db
	./klicdb -X. $(EXT)
predicates.c: klicdb $(EXT) libklic.db
	./klicdb -X. $(EXT)

kl1cmp: klic klicdb klicdb.init libklic.db $(KL1SRC) \
	../runtime/libklict.a
	./klic -P$(PARALLEL) -K $(KL1CMP) -D./klicdb -v \
		-X. -I../include \
		$(KLICOPT) -o kl1cmp $(KL1SRC)

klicdb.init: ../runtime/klic.db
	( test -f klicdb.init && cmp -s klicdb.init ../runtime/klic.db ) || \
	/bin/cp -p ../runtime/klic.db klicdb.init
libklic.db: ../runtime/libklic.db
	cmp -s libklic.db ../runtime/libklic.db || \
	/bin/cp -p ../runtime/libklic.db libklic.db
main.o: ../runtime/main.o
	cp -p ../runtime/main.o .

# ========================================
# Verifying a newly built compiler
# ========================================

verifydir:
	- mkdir verifydir
	- for file in $(KL1SRC); do \
		ln -s ../$$file verifydir; \
	  done

verify: verifydir kl1cmp $(CSRC)
	$(KLIC) -C -K ../compiler/kl1cmp$(CYGWIN) verifydir/*.kl1
	- for file in $(PROGRAMS); do \
	  echo $$file; \
	  tail +2 $$file.c >/tmp/$$file.old; \
	  tail +2 verifydir/$$file.c >/tmp/$$file.new; \
	  diff /tmp/$$file.old /tmp/$$file.new; \
	  /bin/rm -f /tmp/$$file.old /tmp/$$file.new; \
	done

depend:

clean:
	/bin/rm -f -r verifydir *.o *~ \#*\# core
	/bin/rm -f kl1cmp$(CYGWIN) klic$(CYGWIN) \
		 kl1pp$(CYGWIN) klicdb$(CYGWIN) \
		 libklic.db klic.db klicdb.init

distclean: clean

realclean: distclean
	/bin/rm -f $(CSRC) $(KL1PPCSRC) *.ext atom.h funct.h \
		version.h version.kl1 version.sed \
		atom.c funct.c predicates.c

klicdb.o: klic.h ../include/klic/config.h
klic.o: klic.h ../include/klic/config.h

bb.o: ../include/klic/config.h
gb.o: ../include/klic/config.h
normalize.o: ../include/klic/config.h
version.o: ../include/klic/config.h
generate.o: ../include/klic/config.h
obj.o: ../include/klic/config.h
write.o: ../include/klic/config.h
insert.o: ../include/klic/config.h
macro.o: ../include/klic/config.h
extern.o: ../include/klic/config.h
kl1cmp.o: ../include/klic/config.h
cmain.o: ../include/klic/config.h
util.o: ../include/klic/config.h
