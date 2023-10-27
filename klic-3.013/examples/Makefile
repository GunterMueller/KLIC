all:	append.out \
	chcount.out \
	counter.out \
	fiblazy.out \
	hamm.out \
	hamm2.out \
	inverter.out \
	primes.out \
	smallset.out \
	squash.out \
	twins.out

append.out: append.kl1 tty.kl1
	klic -o append.out append.kl1 tty.kl1

chcount.out: chcount.kl1
	klic -o chcount.out chcount.kl1

counter.out: counter.kl1 tty.kl1
	klic -o counter.out counter.kl1 tty.kl1

fiblazy.out: fiblazy.kl1 tty.kl1
	klic -o fiblazy.out fiblazy.kl1 tty.kl1

hamm.out: hamm.kl1 tty.kl1
	klic -o hamm.out hamm.kl1 tty.kl1

hamm2.out: hamm2.kl1 tty.kl1
	klic -o hamm2.out hamm2.kl1 tty.kl1

inverter.out: inverter.kl1
	klic -o inverter.out inverter.kl1

primes.out: primes.kl1
	klic -o primes.out primes.kl1

smallset.out: smallset.kl1 tty.kl1
	klic -o smallset.out smallset.kl1 tty.kl1

squash.out: squash.kl1
	klic -o squash.out squash.kl1

twins.out:	twins.kl1 tty.kl1
	klic -o twins.out twins.kl1 tty.kl1

clean:
	rm -f *.c *.o *.ext klic.db atom.* funct.* predicates.* *.out
