/* ---------------------------------------------------------- 
%   (C) 1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/index.h>
#include <klic/generic.h>
#include <klic/functorstuffs.h>
#include <klic/atomstuffs.h>

/* Bases (prime numbers added) for hashing various types */
#define SYM_HASH_BASE	     67148443L
#define CONS_HASH_BASE	     73118131L
#define FUNCT_HASH_BASE	     43147297L

q hash_kl1_term();

q builtin_hash_term(x, l)
     q x;
     long l;
{
  q tmp = hash_kl1_term(x, l);
  if (!isint(tmp)) return tmp;
  /* mask out the sign bit to return only positive integer value*/
  return makeint(intval(tmp) & ((1L << (sizeof(long)*8-ATAGBITS-1))-1));
}

q hash_kl1_term(x, l)
     q x;
     long l;
{
 again:
  deref_and_switch(x, susp, atomic, cons, funct);
 atomic:
  if (isint(x)) return x;
  return makeint(0x5b*symval(x)+SYM_HASH_BASE);
 cons:
  if (l > 0) {
    q carh, cdrh;
    l--;
    carh = hash_kl1_term(car_of(x), l);
    if (!isint(carh)) return carh;
    cdrh = hash_kl1_term(cdr_of(x), l);
    if (!isint(cdrh)) return cdrh;
    return makeint(0x25*intval(carh) + 0x59*intval(cdrh) + CONS_HASH_BASE);
  } else {
    return makeint(CONS_HASH_BASE);
  }
 funct:
  if (!isgobj(x)) {
    if (l > 0) {
      long k, arity = arityof(functor_of(x));
      long acc = FUNCT_HASH_BASE + 0x3d * symval(functor_of(x));
      l--;
      for (k=0; k<arity; k++) {
	q tmp = hash_kl1_term(arg(x, k), l);
	if (!isint(tmp)) return tmp;
	acc = acc*3 + intval(tmp);
      }
      return makeint(acc);
    } else {
      return makeint(FUNCT_HASH_BASE + 0x3d * symval(functor_of(x)));
    }
  } else {
    return
      ((struct data_object *)functorp(x))->method_table->
	hash((struct data_object *)functorp(x), l);
  }
 susp:
  return x;
}

q builtin_compare_terms(x, y)
     q x, y;
{
 again:
  while (isref(y)) {
    q temp = derefone(y);
    if(!isref(temp) || (y) != derefone(temp)) {
      y = temp;
    } else {
      return y;
    }
  }
  deref_and_switch(x, x_susp, x_atomic, x_cons, x_funct);
 x_susp:
  return x;
 x_atomic:
  if (isint(x)) {
    if (!isint(y)) goto lt;
    if (intval(x) < intval(y)) goto lt;
    if (intval(x) > intval(y)) goto gt;
    goto eq;
  } else {
    if (isint(y)) goto gt;
    if (symval(x)<symval(y)) goto lt;
    if (symval(x)>symval(y)) goto gt;
    goto eq;
  }

 x_cons:
  if (isatomic(y)) goto gt;
  if (!iscons(y)) goto lt;
  {
    q tmp = builtin_compare_terms(car_of(x), car_of(y));
    if (tmp != makeint(0)) return tmp;
  }
  x = cdr_of(x);
  y = cdr_of(y);
  goto again;

 x_funct:
  if (!isfunctor(y)) goto gt;
  if (!isgobj(x)) {
    if (isgobj(y)) goto lt;
    if (symval(functor_of(x)) < symval(functor_of(y))) goto lt;
    if (symval(functor_of(x)) > symval(functor_of(y))) goto gt;
    {
      int arity = arityof(functor_of(x));
      int k;
      for (k=0; k<arity-1; k++) {
	q tmp = builtin_compare_terms(arg(x, k), arg(y, k));
	if (tmp != makeint(0)) return tmp;
      }
      x = arg(x, k);
      y = arg(y, k);
    }
    goto again;
  } else {
    if (!isgobj(y)) goto gt;
    if (functor_of(x) < functor_of(y)) goto lt;
    if (functor_of(x) > functor_of(y)) goto gt;
    return
      ((struct data_object *)functorp(x))->
	method_table->
	  compare((struct data_object *)functorp(x),
		  (struct data_object *)functorp(y));
  }

 lt: return makeint(-1);
 eq: return makeint(0);
 gt: return makeint(1);
}
