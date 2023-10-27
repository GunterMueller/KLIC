# ---------------------------------------------------------- 
#   (C)1993,1994 Institute for New Generation Computer Technology 
#       (Read COPYRIGHT for detailed information.) 
#   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
#       (Read COPYRIGHT-JIPDEC for detailed information.)
# ----------------------------------------------------------
SLEEP = sleep 1
PARALLEL = 0
KLIC = ../compiler/klic -P$(PARALLEL) \
	-v -K../compiler/kl1cmp -D../compiler/klicdb \
	-X. -I../include -L../runtime -g
KLICSYS = ../compiler/klic ../compiler/kl1cmp ../compiler/klicdb \
	../include/klic/* ../runtime/libklic.a

.SUFFIXES: $(.SUFFIXES) .tst .res

TESTS = deriv.tst fact.tst hanoi.tst iotest.tst kkqueen.tst \
	life.tst mastermind.tst merge.tst parsetest.tst \
	pascal.tst pp.tst primes.tst primesp.tst prio.tst \
	puzzle.tst qlay.tst qsort.tst turtles.tst \
	vecstr.tst wave.tst atomfunc.tst cmac.tst

TESTPROGS = \
	deriv fact hanoi iotest kkqueen \
	life mastermind merge parsetest \
	pascal pp primes primesp prio \
	puzzle qlay qsort turtles \
	vecstr wave atomfunc cmac

TESTSRCS = \
	deriv.kl1 fact.kl1 hanoi.kl1 iotest.kl1 kkqueen.kl1 \
	life.kl1 mastermind.kl1 merge.kl1 parsetest.kl1 \
	pascal.kl1 pp.kl1 primes.kl1 primesp.kl1 prio.kl1 \
	puzzle.kl1 qlay.kl1 qsort.kl1 turtles.kl1 \
	vecstr.kl1 wave.kl1 atomfunc.kl1 cmac.kl1

tests:	klicdb.init libklic.db objects $(TESTS)

klicdb.init:
	cp -p ../runtime/klic.db klicdb.init

libklic.db:
	cp -p ../runtime/libklic.db .

objects:
	$(KLIC) -c $(TESTSRCS)

.res.tst:
	cmp $*.res $*.res.cmp

deriv.res: deriv
	./deriv >deriv.res
fact.res: fact
	./fact >fact.res
hanoi.res: hanoi
	./hanoi >hanoi.res
kkqueen.res: kkqueen
	./kkqueen >kkqueen.res
iotest.res: iotest
	./iotest >iotest.res
life.res: life
	./life >life.res
mastermind.res: mastermind
	./mastermind >mastermind.res
merge.res: merge
	./merge >merge.res
parsetest.res: parsetest
	./parsetest >parsetest.res
pascal.res: pascal
	./pascal >pascal.res
pp.res:	pp
	./pp >pp.res
primes.res: primes
	./primes >primes.res
primesp.res: primesp
	./primesp >primesp.res
prio.res: prio
	./prio >prio.res
puzzle.res: puzzle
	./puzzle >puzzle.res
qlay.res: qlay
	./qlay >qlay.res
qsort.res:	qsort
	./qsort >qsort.res
turtles.res: turtles
	./turtles >turtles.res
vecstr.res: vecstr
	./vecstr >vecstr.res
wave.res: wave
	./wave >wave.res
atomfunc.res: atomfunc
	./atomfunc >atomfunc.res
server.res: server
	./server >server.res
client.res: client
	./client >client.res
cmac.res: cmac
	./cmac >cmac.res

deriv:	deriv.kl1 $(KLICSYS)
	$(KLIC) -o deriv deriv.kl1; $(SLEEP)
fact:	fact.kl1 $(KLICSYS)
	$(KLIC) -o fact fact.kl1; $(SLEEP)
hanoi:	hanoi.kl1 $(KLICSYS)
	$(KLIC) -o hanoi hanoi.kl1; $(SLEEP)
kkqueen: kkqueen.kl1 $(KLICSYS)
	$(KLIC) -o kkqueen kkqueen.kl1; $(SLEEP)
iotest:	iotest.kl1 $(KLICSYS)
	$(KLIC) -o iotest iotest.kl1; $(SLEEP)
life:	life.kl1 $(KLICSYS)
	$(KLIC) -o life life.kl1; $(SLEEP)
mastermind: mastermind.kl1 $(KLICSYS)
	$(KLIC) -o mastermind mastermind.kl1; $(SLEEP)
merge: merge.kl1 $(KLICSYS)
	$(KLIC) -o merge merge.kl1; $(SLEEP)
parsetest: parsetest.kl1 $(KLICSYS)
	$(KLIC) -o parsetest parsetest.kl1; $(SLEEP)
pascal:	pascal.kl1 $(KLICSYS)
	$(KLIC) -o pascal pascal.kl1; $(SLEEP)
pp:	pp.kl1 $(KLICSYS)
	$(KLIC) -o pp pp.kl1; $(SLEEP)
primes:	primes.kl1 $(KLICSYS)
	$(KLIC) -o primes primes.kl1; $(SLEEP)
primesp: primesp.kl1 $(KLICSYS)
	$(KLIC) -o primesp primesp.kl1; $(SLEEP)
prio:	prio.kl1 $(KLICSYS)
	$(KLIC) -o prio prio.kl1; $(SLEEP)
puzzle:	puzzle.kl1 $(KLICSYS)
	$(KLIC) -o puzzle puzzle.kl1; $(SLEEP)
qlay:	qlay.kl1 $(KLICSYS)
	$(KLIC) -o qlay qlay.kl1; $(SLEEP)
qsort:	qsort.kl1 $(KLICSYS)
	$(KLIC) -o qsort qsort.kl1; $(SLEEP)
turtles:	turtles.kl1 $(KLICSYS)
	$(KLIC) -o turtles turtles.kl1; $(SLEEP)
vecstr:	vecstr.kl1 $(KLICSYS)
	$(KLIC) -o vecstr vecstr.kl1; $(SLEEP)
wave:	wave.kl1 $(KLICSYS)
	$(KLIC) -o wave wave.kl1; $(SLEEP)
atomfunc:	atomfunc.kl1 $(KLICSYS)
	$(KLIC) -o atomfunc atomfunc.kl1; $(SLEEP)
server:	server.kl1 $(KLICSYS)
	$(KLIC) -o server server.kl1; $(SLEEP)
client:	client.kl1 $(KLICSYS)
	$(KLIC) -o client client.kl1; $(SLEEP)
cmac: cmac.kl1 $(KLICSYS)
	$(KLIC) -o cmac cmac.kl1; $(SLEEP)
iotest.res.cmp: iotest.kl1 $(KLICSYS)
	cp iotest.kl1 iotest.res.cmp

atomfunc.kl1:	#atomfunc.perl ../runtime/klic.db
	perl atomfunc.perl ../runtime/klic.db >work; mv work atomfunc.kl1

clean:
	/bin/rm -f *.c *.ext *.h *.o klic.db klicdb.init libklic.db \
		work* *~ core $(TESTPROGS)

distclean: clean
	/bin/rm -f $(TESTPROGS) server client *.res

realclean: distclean
