/* ---------------------------------------------------------- 
%   (C)1993, 1994 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <stdio.h>
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/unify.h>
#include <klic/index.h>
#include <klic/gb.h>
#include <klic/functorstuffs.h>
#include <klic/gobj.h>

/* 
  General Guard Unification

  return value:
  0 : failed
  1 : succeeded
  other : suspended
*/

#define SUCCEEDED ((q)0)
#define FAILED ((q)1)

q eq_terms_body(x,y)
     q x, y;
{
 loop_entry:
  deref_and_switch(x, susp_x, x_atomic, x_cons, x_functor);
 x_functor:
  deref_and_switch(y, susp_y, fail, fail, funct_ok);
  funct_ok: {
    q funcx = functor_of(x);
    q funcy = functor_of(y);
    if (funcx == funcy && isatomic(funcx)) {
      long arity = arityof(funcx);
      long i;
      long size = arity - 1;
      for(i=0; i<size; ++i){
	q retval;
	if ((retval = eq_terms_body(arg(x, i), arg(y, i))) != SUCCEEDED) {
	  return retval;
	}
      }
      x = arg(x, size);
      y = arg(y, size);
      goto loop_entry;
    } else if (isref(funcx) && isref(funcy)) {
      /* both arguments are generic objects */
      return generic_passive_unify(data_objectp(x), data_objectp(y));
    } else {
      return FAILED;
    }
  }
 x_cons:
  deref_and_switch(y, susp_y, fail, cons_ok, fail);
  cons_ok: {
    q retval;
    if ((retval = eq_terms_body(car_of(x), car_of(y))) == SUCCEEDED) {
      x = cdr_of(x);
      y = cdr_of(y);
      goto loop_entry;
    } else {
      return retval;
    }
  }
 x_atomic:
  deref_and_switch(y, susp_y, atomic_ok, fail, fail);
 atomic_ok:
  return ((x == y) ? SUCCEEDED : FAILED);
 fail:
  return FAILED;
 susp_x:
  return x;
 susp_y:
  return y;
}
