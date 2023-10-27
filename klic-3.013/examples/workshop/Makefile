all:	bst.out \
	compact.out \
	cont.out \
	fibSqrCube.out \
	fibSqrCubeDD.out \
	gencon.out \
	genconfail.out \
	genconsusp.out \
	genconwp.out \
	hamm.out \
	naive.out \
	not.out \
	nots.out \
	nrev0.out \
	nrev1.out \
	primes.out \
	primesDD.out \
	primes_prio.out \
	primes_prio2.out \
	prio.out \
	qsort.out \
	remove.out \
	request.out \
	search.out \
	seq.out

bst.out: bst.kl1
	klic -o bst.out bst.kl1

compact.out: compact.kl1
	klic -o compact.out compact.kl1

cont.out: cont.kl1
	klic -o cont.out cont.kl1

fibSqrCube.out: fibSqrCube.kl1
	klic -o fibSqrCube.out fibSqrCube.kl1

fibSqrCubeDD.out: fibSqrCubeDD.kl1
	klic -o fibSqrCubeDD.out fibSqrCubeDD.kl1

gencon.out: gencon.kl1
	klic -o gencon.out gencon.kl1

genconfail.out: genconfail.kl1
	klic -o genconfail.out genconfail.kl1

genconsusp.out: genconsusp.kl1
	klic -o genconsusp.out genconsusp.kl1

genconwp.out: genconwp.kl1
	klic -o genconwp.out genconwp.kl1

hamm.out: hamm.kl1
	klic -o hamm.out hamm.kl1

naive.out: naive.kl1
	klic -o naive.out naive.kl1

not.out: not.kl1
	klic -o not.out not.kl1

nots.out: nots.kl1
	klic -o nots.out nots.kl1

nrev0.out: nrev0.kl1
	klic -o nrev0.out nrev0.kl1

nrev1.out: nrev1.kl1 append.kl1
	klic -o nrev1.out nrev1.kl1 append.kl1

primes.out: primes.kl1
	klic -o primes.out primes.kl1

primesDD.out: primesDD.kl1
	klic -o primesDD.out primesDD.kl1

primes_prio.out: primes_prio.kl1
	klic -o primes_prio.out primes_prio.kl1

primes_prio2.out: primes_prio2.kl1
	klic -o primes_prio2.out primes_prio2.kl1

prio.out: prio.kl1
	klic -o prio.out prio.kl1

qsort.out: qsort.kl1 data.kl1
	klic -o qsort.out qsort.kl1 data.kl1

remove.out: remove.kl1
	klic -o remove.out remove.kl1

request.out: request.kl1
	klic -o request.out request.kl1

search.out: search.kl1
	klic -o search.out search.kl1

seq.out: seq.kl1
	klic -o seq.out seq.kl1

clean:
	rm -f *.c *.o *.ext klic.db atom.* funct.* predicates.* *.out
