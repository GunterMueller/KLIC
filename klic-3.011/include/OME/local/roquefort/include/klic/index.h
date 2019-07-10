/* ---------------------------------------------------------- 
%   (C)1993 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#define deref_and_switch(x, susp, atomic, cons, funct)	\
{ \
  while (1) { \
    if (!isstruct(x)) { \
      if (atomicnotref(x)) { \
	goto atomic; \
      } else { \
	q temp0 = derefone(x); \
	if(isref(temp0) && (x) == derefone(temp0)) { \
	  goto susp; \
	} else { \
	  (x) = temp0; \
	} \
      } \
    } else { \
      if(functnotcons(x)) \
	goto funct; \
      else goto cons; \
    } \
  } \
}

#define deref_and_test(x, susp) \
{ \
  q temp0 = derefone(x); \
  if(!isref(temp0) || (x) != derefone(temp0)) { \
    (x) = temp0; \
  } else { \
    *reasonp++ = (x); \
    goto susp; \
  } \
}

#define deref_and_jump(x, loop) \
{ \
  q temp0 = derefone(x); \
  if(!isref(temp0)) { \
    (x) = temp0; \
    goto loop; \
  } \
}

#define not_cons(x)		((tag = ptagof(x)) != CONS)
#define not_functor()		(tag <= CONS)
#define not_atomic()		(tag == 0)
