##---------------------------------------------------------- 
##   (C)1995 Institute for New Generation Computer Technology 
##       (Read COPYRIGHT for detailed information.) 
##----------------------------------------------------------
# runtime option for test programs

RUNOPT=		# none for shared memory version

# Top directory of KLIC
TOP=..
# command prefix to run
CMD=./

# Maximum Nodes
NODES = 16

PARALLEL = 10

# KLIC compiler to compile test progs.
KLIC=$(TOP)/compiler/klic -K$(TOP)/compiler/kl1cmp \
	-D$(TOP)/compiler/klicdb -X. -I$(TOP)/include -L$(TOP)/runtime

# options to compiler KLIC programs
KLICSOPT=-v -shm -P$(PARALLEL) -g #$(KLICOPT)

# system dependency
SYSTEM=$(TOP)/runtime/libklics.a $(TOP)/compiler/kl1cmp $(TOP)/compiler/klic

# Result checking command. You may want to rewrite this on some machines.
# If you want to see progress of the executino, comment out this macro.
T= `basename $@ .tst`
R = | cmp - cmpdir/`basename $@ .tst`.out &&echo ''

TEST = forward_goal.tst general_unify_1.tst general_unify_2.tst \
	multiple_wait.tst t0thr1.tst t0thr2.tst t0thr3.tst unify_exrefs.tst \
	vector_built.tst vector_built_2.tst wec_exhaust_1.tst \
	wec_exhaust_2.tst wec_exhaust_3.tst wec_indirect.tst
TEST_PROGS = forward_goal general_unify_1 general_unify_2 \
	multiple_wait t0thr1 t0thr2 t0thr3  unify_exrefs vector_built \
	vector_built_2 wec_exhaust_1 wec_exhaust_2 wec_exhaust_3 \
	wec_indirect
TEST_SRCS = forward_goal.kl1 general_unify_1.kl1 general_unify_2.kl1 \
	multiple_wait.kl1 t0thr1.kl1 t0thr2.kl1 t0thr3.kl1 unify_exrefs.kl1 \
	vector_built.kl1 vector_built_2.kl1 wec_exhaust_1.kl1 \
	wec_exhaust_2.kl1 wec_exhaust_3.kl1 wec_indirect.kl1
TEST_CSRCS = forward_goal.c general_unify_1.c general_unify_2.c \
	multiple_wait.c t0thr1.c t0thr2.c t0thr3.c unify_exrefs.c \
	vector_built.c vector_built_2.c wec_exhaust_1.c \
	wec_exhaust_2.c wec_exhaust_3.c wec_indirect.c
TEST_OBJS = forward_goal.o general_unify_1.o general_unify_2.o \
	multiple_wait.o t0thr1.o t0thr2.o t0thr3.o unify_exrefs.o \
	vector_built.o vector_built_2.o wec_exhaust_1.o \
	wec_exhaust_2.o wec_exhaust_3.o wec_indirect.o

MEASURE= nqueen12.tst nqueen13.tst read.tst throw.tst unify.tst slife.tst
MEASURE_PROGS= nqueen nqueen12 nqueen13 nqueen14  read throw unify slife
MEASURE_SRCS= nqueen.kl1 read.kl1 throw.kl1 unify.kl1 slife.kl1
MEASURE_CSRCS= nqueen.c read.c throw.c unify.c slife.c
MEASURE_OBJS= nqueen.o read.o throw.o unify.o slife.o

SHELL=/bin/sh

dummy:
	@echo "If you want to measure the performance, run 'make all-measure'";
	make all-test
all: all-test all-measure
forward_goal.tst: forward_goal
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 68 +-7 msec; \
		echo "$(CMD)forward_goal -p1 $(RUNOPT) $R"; \
		$(CMD)forward_goal -p1 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 6035 +-165 msec; \
		echo "$(CMD)forward_goal -p2 $(RUNOPT) $R"; \
		$(CMD)forward_goal -p2 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 5 ] ; then \
		echo Reference time is 5463 +-320 msec; \
		echo "$(CMD)forward_goal -p5 $(RUNOPT) $R"; \
		$(CMD)forward_goal -p5 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 8 ] ; then \
		echo Reference time is 5473 +-249 msec; \
		echo "$(CMD)forward_goal -p8 $(RUNOPT) $R"; \
		$(CMD)forward_goal -p8 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 9 ] ; then \
		echo Reference time is 5619 +-233 msec; \
		echo "$(CMD)forward_goal -p9 $(RUNOPT) $R"; \
		$(CMD)forward_goal -p9 $(RUNOPT) $R; \
		sleep 5; \
	fi
general_unify_1.tst: general_unify_1
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 47 +-10 msec; \
		echo "$(CMD)general_unify_1 -p1 $(RUNOPT) $R"; \
		$(CMD)general_unify_1 -p1 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 122 +-11 msec; \
		echo "$(CMD)general_unify_1 -p2 $(RUNOPT) $R"; \
		$(CMD)general_unify_1 -p2 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 5 ] ; then \
		echo Reference time is 150 +-16 msec; \
		echo "$(CMD)general_unify_1 -p5 $(RUNOPT) $R"; \
		$(CMD)general_unify_1 -p5 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 8 ] ; then \
		echo Reference time is 173 +-17 msec; \
		echo "$(CMD)general_unify_1 -p8 $(RUNOPT) $R"; \
		$(CMD)general_unify_1 -p8 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 9 ] ; then \
		echo Reference time is 155 +-11 msec; \
		echo "$(CMD)general_unify_1 -p9 $(RUNOPT) $R"; \
		$(CMD)general_unify_1 -p9 $(RUNOPT) $R; \
		sleep 5; \
	fi
general_unify_2.tst: general_unify_2
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 38 +-7 msec; \
		echo "$(CMD)general_unify_2 -p1 $(RUNOPT) $R"; \
		$(CMD)general_unify_2 -p1 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 90 +-11 msec; \
		echo "$(CMD)general_unify_2 -p2 $(RUNOPT) $R"; \
		$(CMD)general_unify_2 -p2 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 5 ] ; then \
		echo Reference time is 92 +-8 msec; \
		echo "$(CMD)general_unify_2 -p5 $(RUNOPT) $R"; \
		$(CMD)general_unify_2 -p5 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 8 ] ; then \
		echo Reference time is 114 +-31 msec; \
		echo "$(CMD)general_unify_2 -p8 $(RUNOPT) $R"; \
		$(CMD)general_unify_2 -p8 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 9 ] ; then \
		echo Reference time is 111 +-10 msec; \
		echo "$(CMD)general_unify_2 -p9 $(RUNOPT) $R"; \
		$(CMD)general_unify_2 -p9 $(RUNOPT) $R; \
		sleep 5; \
	fi
multiple_wait.tst: multiple_wait
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 57 +-5 msec; \
		echo "$(CMD)multiple_wait -p1 $(RUNOPT) $R"; \
		$(CMD)multiple_wait -p1 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 4593 +-54 msec; \
		echo "$(CMD)multiple_wait -p2 $(RUNOPT) $R"; \
		$(CMD)multiple_wait -p2 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 5 ] ; then \
		echo Reference time is 5175 +-176 msec; \
		echo "$(CMD)multiple_wait -p5 $(RUNOPT) $R"; \
		$(CMD)multiple_wait -p5 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 8 ] ; then \
		echo Reference time is 5584 +-195 msec; \
		echo "$(CMD)multiple_wait -p8 $(RUNOPT) $R"; \
		$(CMD)multiple_wait -p8 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 9 ] ; then \
		echo Reference time is 5738 +-163 msec; \
		echo "$(CMD)multiple_wait -p9 $(RUNOPT) $R"; \
		$(CMD)multiple_wait -p9 $(RUNOPT) $R; \
		sleep 5; \
	fi
t0thr1.tst: t0thr1
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 45+-4 msec; \
		echo "$(CMD)t0thr1 -p1 $(RUNOPT) $R"; \
		$(CMD)t0thr1 -p1 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 319 +-15 msec; \
		echo "$(CMD)t0thr1 -p2 $(RUNOPT) $R"; \
		$(CMD)t0thr1 -p2 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 5 ] ; then \
		echo Reference time is 490 +-14 msec; \
		echo "$(CMD)t0thr1 -p5 $(RUNOPT) $R"; \
		$(CMD)t0thr1 -p5 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 8 ] ; then \
		echo Reference time is 531 +-12 msec; \
		echo "$(CMD)t0thr1 -p8 $(RUNOPT) $R"; \
		$(CMD)t0thr1 -p8 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 9 ] ; then \
		echo Reference time is 561 +-34 msec; \
		echo "$(CMD)t0thr1 -p9 $(RUNOPT) $R"; \
		$(CMD)t0thr1 -p9 $(RUNOPT) $R; \
		sleep 5; \
	fi
t0thr2.tst: t0thr2
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 37 +-6 msec; \
		echo "$(CMD)t0thr2 -p1 $(RUNOPT) $R"; \
		$(CMD)t0thr2 -p1 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 289 +-7 msec; \
		echo "$(CMD)t0thr2 -p2 $(RUNOPT) $R"; \
		$(CMD)t0thr2 -p2 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 5 ] ; then \
		echo Reference time is 345 +-26 msec; \
		echo "$(CMD)t0thr2 -p5 $(RUNOPT) $R"; \
		$(CMD)t0thr2 -p5 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 8 ] ; then \
		echo Reference time is 342 +-23 msec; \
		echo "$(CMD)t0thr2 -p8 $(RUNOPT) $R"; \
		$(CMD)t0thr2 -p8 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 9 ] ; then \
		echo Reference time is 342 +-15 msec; \
		echo "$(CMD)t0thr2 -p9 $(RUNOPT) $R"; \
		$(CMD)t0thr2 -p9 $(RUNOPT) $R; \
		sleep 5; \
	fi
t0thr3.tst: t0thr3
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 35 +-3 msec; \
		echo "$(CMD)t0thr3 -p1 $(RUNOPT) $R"; \
		$(CMD)t0thr3 -p1 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 347 +-5 msec; \
		echo "$(CMD)t0thr3 -p2 $(RUNOPT) $R"; \
		$(CMD)t0thr3 -p2 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 5 ] ; then \
		echo Reference time is 405 +-12 msec; \
		echo "$(CMD)t0thr3 -p5 $(RUNOPT) $R"; \
		$(CMD)t0thr3 -p5 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 8 ] ; then \
		echo Reference time is 457 +-151 msec; \
		echo "$(CMD)t0thr3 -p8 $(RUNOPT) $R"; \
		$(CMD)t0thr3 -p8 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 9 ] ; then \
		echo Reference time is 419 +-12 msec; \
		echo "$(CMD)t0thr3 -p9 $(RUNOPT) $R"; \
		$(CMD)t0thr3 -p9 $(RUNOPT) $R; \
		sleep 5; \
	fi
unify_exrefs.tst: unify_exrefs
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 55 +-9 msec; \
		echo "$(CMD)unify_exrefs -p1 $(RUNOPT) $R"; \
		$(CMD)unify_exrefs -p1 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 588 +-61 msec; \
		echo "$(CMD)unify_exrefs -p2 $(RUNOPT) $R"; \
		$(CMD)unify_exrefs -p2 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 5 ] ; then \
		echo Reference time is 993 +-49 msec; \
		echo "$(CMD)unify_exrefs -p5 $(RUNOPT) $R"; \
		$(CMD)unify_exrefs -p5 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 8 ] ; then \
		echo Reference time is 1106 +-97 msec; \
		echo "$(CMD)unify_exrefs -p8 $(RUNOPT) $R"; \
		$(CMD)unify_exrefs -p8 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 9 ] ; then \
		echo Reference time is 1066 +-68 msec; \
		echo "$(CMD)unify_exrefs -p9 $(RUNOPT) $R"; \
		$(CMD)unify_exrefs -p9 $(RUNOPT) $R; \
		sleep 5; \
	fi
vector_built.tst: vector_built
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 130 +-10 msec; \
		echo "$(CMD)vector_built -p1 $(RUNOPT) $R"; \
		$(CMD)vector_built -p1 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 1644 +-81 msec; \
		echo "$(CMD)vector_built -p2 $(RUNOPT) $R"; \
		$(CMD)vector_built -p2 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 3 ] ; then \
		echo Reference time is 1451 +-103 msec; \
		echo "$(CMD)vector_built -p3 $(RUNOPT) $R"; \
		$(CMD)vector_built -p3 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 5 ] ; then \
		echo Reference time is 1413 +-78 msec; \
		echo "$(CMD)vector_built -p5 $(RUNOPT) $R"; \
		$(CMD)vector_built -p5 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 7 ] ; then \
		echo Reference time is 1533 +-97 msec; \
		echo "$(CMD)vector_built -p7 $(RUNOPT) $R"; \
		$(CMD)vector_built -p7 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 10 ] ; then \
		echo Reference time is 1635 +-76 msec; \
		echo "$(CMD)vector_built -p10 $(RUNOPT) $R"; \
		$(CMD)vector_built -p10 $(RUNOPT) $R; \
		sleep 5; \
	fi
vector_built_2.tst: vector_built_2
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 100 +-7 msec; \
		echo "$(CMD)vector_built_2 -p1 $(RUNOPT) $R"; \
		$(CMD)vector_built_2 -p1 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 649 +-66 msec; \
		echo "$(CMD)vector_built_2 -p2 $(RUNOPT) $R"; \
		$(CMD)vector_built_2 -p2 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 3 ] ; then \
		echo Reference time is 1570 +-129 msec; \
		echo "$(CMD)vector_built_2 -p3 $(RUNOPT) $R"; \
		$(CMD)vector_built_2 -p3 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 5 ] ; then \
		echo Reference time is 1165 +-81 msec; \
		echo "$(CMD)vector_built_2 -p5 $(RUNOPT) $R"; \
		$(CMD)vector_built_2 -p5 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 7 ] ; then \
		echo Reference time is 1159 +-59 msec; \
		echo "$(CMD)vector_built_2 -p7 $(RUNOPT) $R"; \
		$(CMD)vector_built_2 -p7 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 10 ] ; then \
		echo Reference time is 1205 +-66 msec; \
		echo "$(CMD)vector_built_2 -p10 $(RUNOPT) $R"; \
		$(CMD)vector_built_2 -p10 $(RUNOPT) $R; \
		sleep 5; \
	fi
wec_exhaust_1.tst: wec_exhaust_1
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 44 +-6 msec; \
		echo "$(CMD)wec_exhaust_1 -p1 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_1 -p1 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 899 +-97 msec; \
		echo "$(CMD)wec_exhaust_1 -p2 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_1 -p2 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 3 ] ; then \
		echo Reference time is 1152 +-67 msec; \
		echo "$(CMD)wec_exhaust_1 -p3 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_1 -p3 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 5 ] ; then \
		echo Reference time is 1104 +-92 msec; \
		echo "$(CMD)wec_exhaust_1 -p5 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_1 -p5 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 7 ] ; then \
		echo Reference time is 1077 +-73 msec; \
		echo "$(CMD)wec_exhaust_1 -p7 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_1 -p7 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 10 ] ; then \
		echo Reference time is 1274 +-71 msec; \
		echo "$(CMD)wec_exhaust_1 -p10 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_1 -p10 $(RUNOPT) $R; \
		sleep 5; \
	fi

wec_exhaust_2.tst: wec_exhaust_2
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 77 +-3 msec; \
		echo "$(CMD)wec_exhaust_2 -p1 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_2 -p1 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 10382 +-515 msec; \
		echo "$(CMD)wec_exhaust_2 -p2 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_2 -p2 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 3 ] ; then \
		echo Reference time is 9816 +-461 msec; \
		echo "$(CMD)wec_exhaust_2 -p3 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_2 -p3 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 5 ] ; then \
		echo Reference time is 8241 +-688 msec; \
		echo "$(CMD)wec_exhaust_2 -p5 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_2 -p5 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 7 ] ; then \
		echo Reference time is 7460 +-325 msec; \
		echo "$(CMD)wec_exhaust_2 -p7 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_2 -p7 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 10 ] ; then \
		echo Reference time is 7506 +-420 msec; \
		echo "$(CMD)wec_exhaust_2 -p10 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_2 -p10 $(RUNOPT) $R; \
		sleep 5; \
	fi
wec_exhaust_3.tst: wec_exhaust_3
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 89 +-46 msec; \
		echo "$(CMD)wec_exhaust_3 -p1 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_3 -p1 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 12242 +-340 msec; \
		echo "$(CMD)wec_exhaust_3 -p2 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_3 -p2 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 3 ] ; then \
		echo Reference time is 10941 +-260 msec; \
		echo "$(CMD)wec_exhaust_3 -p3 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_3 -p3 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 5 ] ; then \
		echo Reference time is 8824 +-321 msec; \
		echo "$(CMD)wec_exhaust_3 -p5 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_3 -p5 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 7 ] ; then \
		echo Reference time is 8672 +-235 msec; \
		echo "$(CMD)wec_exhaust_3 -p7 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_3 -p7 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 10 ] ; then \
		echo Reference time is 9042 +-240 msec; \
		echo "$(CMD)wec_exhaust_3 -p10 $(RUNOPT) $R"; \
		$(CMD)wec_exhaust_3 -p10 $(RUNOPT) $R; \
		sleep 5; \
	fi
wec_indirect.tst: wec_indirect
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 38 +-9 msec; \
		echo "$(CMD)wec_indirect -p1 $(RUNOPT) $R"; \
		$(CMD)wec_indirect -p1 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 4108 +-38 msec; \
		echo "$(CMD)wec_indirect -p2 $(RUNOPT) $R"; \
		$(CMD)wec_indirect -p2 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 3 ] ; then \
		echo Reference time is 4189 +-47 msec; \
		echo "$(CMD)wec_indirect -p3 $(RUNOPT) $R"; \
		$(CMD)wec_indirect -p3 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 5 ] ; then \
		echo Reference time is 4310 +-210 msec; \
		echo "$(CMD)wec_indirect -p5 $(RUNOPT) $R"; \
		$(CMD)wec_indirect -p5 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 7 ] ; then \
		echo Reference time is 4262 +-27 msec; \
		echo "$(CMD)wec_indirect -p7 $(RUNOPT) $R"; \
		$(CMD)wec_indirect -p7 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 10 ] ; then \
		echo Reference time is 4286 +-41 msec; \
		echo "$(CMD)wec_indirect -p10 $(RUNOPT) $R"; \
		$(CMD)wec_indirect -p10 $(RUNOPT) $R; \
		sleep 5; \
	fi

nqueen.tst: nqueen12.tst nqueen13.tst
nqueen12.tst: nqueen12
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 28344 +-556 msec; \
		echo "$(CMD)nqueen12 -p1 $(RUNOPT) 12 $R"; \
		$(CMD)nqueen12 -p1 $(RUNOPT) 12 $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 14361 +-487 msec; \
		echo "$(CMD)nqueen12 -p2 $(RUNOPT) 12 $R"; \
		$(CMD)nqueen12 -p2 $(RUNOPT) 12 $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 4 ] ; then \
		echo Reference time is 8509 +-205 msec; \
		echo "$(CMD)nqueen12 -p4 $(RUNOPT) 12 $R"; \
		$(CMD)nqueen12 -p4 $(RUNOPT) 12 $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 8 ] ; then \
		echo Reference time is 5848 +-421 msec; \
		echo "$(CMD)nqueen12 -p8 $(RUNOPT) 12 $R"; \
		$(CMD)nqueen12 -p8 $(RUNOPT) 12 $R; \
		sleep 5; \
	fi
nqueen13.tst: nqueen13
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 181149 +-4545 msec; \
		echo "$(CMD)nqueen13 -p1 $(RUNOPT) 13 $R"; \
		$(CMD)nqueen13 -p1 $(RUNOPT) 13 $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 89356 +-2060 msec; \
		echo "$(CMD)nqueen13 -p2 $(RUNOPT) 13 $R"; \
		$(CMD)nqueen13 -p2 $(RUNOPT) 13 $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 4 ] ; then \
		echo Reference time is 45676 +-1451 msec; \
		echo "$(CMD)nqueen13 -p4 $(RUNOPT) 13 $R"; \
		$(CMD)nqueen13 -p4 $(RUNOPT) 13 $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 8 ] ; then \
		echo Reference time is 24863 +-379 msec; \
		echo "$(CMD)nqueen13 -p8 $(RUNOPT) 13 $R"; \
		$(CMD)nqueen13 -p8 $(RUNOPT) 13 $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 12 ] ; then \
		echo Reference time is 18085 +-427 msec; \
		echo "$(CMD)nqueen13 -p12 $(RUNOPT) 13 $R"; \
		$(CMD)nqueen13 -p12 $(RUNOPT) 13 $R; \
		sleep 5; \
	fi
nqueen14.tst: nqueen14
	if [ $(NODES) -ge 1 ] ; then \
		echo "(CMD)nqueen14 -p1 $(RUNOPT) 14 $R"; \
		(CMD)nqueen14 -p1 $(RUNOPT) 14 $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Response time is 601414 msec; \
		echo "(CMD)nqueen14 -p2 $(RUNOPT) 14 $R"; \
		(CMD)nqueen14 -p2 $(RUNOPT) 14 $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 4 ] ; then \
		echo Response time is 303094 msec; \
		echo "(CMD)nqueen14 -p4 $(RUNOPT) 14 $R"; \
		(CMD)nqueen14 -p4 $(RUNOPT) 14 $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 8 ] ; then \
		echo Response time is 163584 msec; \
		echo "(CMD)nqueen14 -p8 $(RUNOPT) 14 $R"; \
		(CMD)nqueen14 -p8 $(RUNOPT) 14 $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 12 ] ; then \
		echo Response time is 128024 msec; \
		echo "(CMD)nqueen14 -p12 $(RUNOPT) 14 $R"; \
		(CMD)nqueen14 -p12 $(RUNOPT) 14 $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 16 ]
		echo Response time is 97256 msec; \
		echo "$(CMD)nqueen14 -p16 $(RUNOPT) 14 $R"; \
		$(CMD)nqueen14 -p16 $(RUNOPT) 14 $R; \
		sleep 5; \
	fi
read.tst: read
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 20162 +-224 msec; \
		echo "$(CMD)read -p2 $(RUNOPT) 1000 $R"; \
		$(CMD)read -p2 $(RUNOPT) 1000 $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 40344 +-607 msec; \
		echo "$(CMD)read -p2 $(RUNOPT) 2000 $R"; \
		$(CMD)read -p2 $(RUNOPT) 2000 $R; \
		sleep 5; \
	fi
slife.tst: slife
	if [ $(NODES) -ge 1 ] ; then \
		echo Reference time is 2629 +-132 msec; \
		echo "$(CMD)slife -p1 $(RUNOPT) $R"; \
		$(CMD)slife -p1 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 55100 +-2255 msec; \
		echo "$(CMD)slife -p2 $(RUNOPT) $R"; \
		$(CMD)slife -p2 $(RUNOPT) $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 4 ] ; then \
		echo Reference time is 35497 +-1371 msec; \
		echo "$(CMD)slife -p4 $(RUNOPT) $R"; \
		$(CMD)slife -p4 $(RUNOPT) $R; \
		sleep 5; \
	fi
throw.tst: throw
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 10110 +-80 msec; \
		echo "./throw -p2 1000 $R"; \
		./throw -p2 1000 $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 20276 +-469 msec; \
		echo "./throw -p2 2000 $R"; \
		./throw -p2 2000 $R; \
		sleep 5; \
	fi
unify.tst: unify
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 10315 +-703 msec; \
		echo "./unify -p2 1000 $R"; \
		./unify -p2 1000 $R; \
		sleep 5; \
	fi
	if [ $(NODES) -ge 2 ] ; then \
		echo Reference time is 20234 +-271 msec; \
		echo "./unify -p2 2000 $R"; \
		./unify -p2 2000 $R; \
		sleep 5; \
	fi

all-measure: $(MEASURE)
all-measure-src: $(MEASURE_PROGS)
all-test: $(TEST)
all-test-src: $(TEST_PROGS)

forward_goal: forward_goal.o lib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o lib.o
general_unify_1: general_unify_1.o lib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o lib.o
general_unify_2: general_unify_2.o lib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o lib.o
multiple_wait: multiple_wait.o lib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o lib.o
t0thr1: t0thr1.o lib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o lib.o
t0thr2: t0thr2.o lib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o lib.o
t0thr3: t0thr3.o lib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o lib.o
unify_exrefs: unify_exrefs.o lib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o lib.o
vector_built: vector_built.o lib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o lib.o
vector_built_2: vector_built_2.o lib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o lib.o
wec_exhaust_1: wec_exhaust_1.o lib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o lib.o
wec_exhaust_2: wec_exhaust_2.o lib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o lib.o
wec_exhaust_3: wec_exhaust_3.o lib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o lib.o
wec_indirect: wec_indirect.o lib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o lib.o


slife: slife.o
	${KLIC} ${KLICSOPT} -o $@ $@.o
nqueen: nqueen.o rlib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o rlib.o
nqueen12: nqueen
	rm -f nqueen12; ln nqueen nqueen12
nqueen13: nqueen
	rm -f nqueen13; ln nqueen nqueen13
nqueen14: nqueen
	rm -f nqueen14; ln nqueen nqueen14
read: read.o rlib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o rlib.o
throw: throw.o rlib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o rlib.o
unify: unify.o rlib.o
	${KLIC} ${KLICSOPT} -o $@ $@.o rlib.o

$(MEASURE_OBJS) rlib.o $(TEST_OBJS) lib.o: sysupdate.time klicdb.init libklic.db \
		$(MEASURE_SRCS) rlib.kl1 $(TEST_SRCS) lib.kl1
	$(KLIC) $(KLICSOPT) -c $(MEASURE_SRCS) rlib.kl1 $(TEST_SRCS) lib.kl1

lib.o: $(SYSTEM) klicdb.init libklic.db
forward_goal: $(SYSTEM) klicdb.init libklic.db
general_unify_1: $(SYSTEM) klicdb.init libklic.db
general_unify_2: $(SYSTEM) klicdb.init libklic.db
multiple_wait: $(SYSTEM) klicdb.init libklic.db
t0thr1: $(SYSTEM) klicdb.init libklic.db
t0thr2: $(SYSTEM) klicdb.init libklic.db
t0thr3: $(SYSTEM) klicdb.init libklic.db
unify_exrefs: $(SYSTEM) klicdb.init libklic.db
vector_built: $(SYSTEM) klicdb.init libklic.db
vector_built_2: $(SYSTEM) klicdb.init libklic.db
wec_exhaust_1: $(SYSTEM) klicdb.init libklic.db
wec_exhaust_2: $(SYSTEM) klicdb.init libklic.db
wec_exhaust_3: $(SYSTEM) klicdb.init libklic.db
wec_indirect: $(SYSTEM) klicdb.init libklic.db

klicdb.init: $(TOP)/runtime/klic.db
	cp -p $(TOP)/runtime/klic.db klicdb.init

libklic.db: $(TOP)/runtime/libklic.db
	cp -p $(TOP)/runtime/libklic.db .
sysupdate.time:  $(SYSTEM)
	make clean;
	touch sysupdate.time
depend:
realclean: distclean
distclean: clean
	rm -f \#* *~ *.orig *.rej
clean:
	rm -f *.ext *.c *.h libklic.db klicdb.init
	rm -f *.o core a.out klic.db *.out
	rm -f $(TEST_PROGS) rm -f $(MEASURE_PROGS)
	rm -f sysupdate.time
backups:
	mv -f *.ext *.c *.h *.o libklic.db klicdb.init \
	core a.out klic.db \
	$(TEST_PROGS) $(MEASURE_PROGS) backup
