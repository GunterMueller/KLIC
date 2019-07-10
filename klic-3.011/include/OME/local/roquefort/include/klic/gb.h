/* ---------------------------------------------------------- 
%   (C)1993,1994 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

extern q builtin_compare_terms();
extern q builtin_hash_term();
extern void printl(q x);

#define gblt_compare(x, y, r, fail) \
  r = builtin_compare_terms((x), (y)); \
  if (isref(r)) { \
    *reasonp++ = r; \
    goto fail; \
  }

#define gblt_hash(x, r, fail) \
  r = builtin_hash_term(x, 0)

#define gblt_deep_hash(x, l, r, fail) \
  r = builtin_hash_term(x, intval(l)); \
  if (isref(r)) { \
    *reasonp++ = r; \
    goto fail; \
  }

#define gblt_current_prio(p, fail) \
  p = makeint(current_prio)

#define gblt_wait( x, susp )

#define gblt_list( x, fail )

#define gblt_atom( x, fail )

#define gblt_integer( x, fail )

#define gblt_atomic( x, fail )

#define gblt_float( x, fail )

/* Multitude Comparison */

#define gblt_greater( x, y, fail ) \
    if ((long)(x) <= (long)(y)) goto fail;

#define gblt_greater_or_eq( x, y, fail ) \
    if ((long)(x) < (long)(y)) goto fail;

#define gblt_less( x, y, fail ) \
    if ((long)(x) >= (long)(y)) goto fail;

#define gblt_less_or_eq( x, y, fail ) \
    if ((long)(x) > (long)(y)) goto fail;

#define gblt_eq( x, y,  fail ) \
    if ((long)(x) != (long)(y)) goto fail;

/* Arithmetical/Logical Operations on Integers */

#define gblt_and( x, y, z, fail ) \
    z = (q)((unsigned long)(x) & (unsigned long)(y));

#define gblt_or( x, y, z, fail ) \
    z = (q)((unsigned long)(x) | (unsigned long)(y));

#define gblt_exclusive_or( x, y, z, fail ) \
    z = (q)(((unsigned long)(x) ^ (unsigned long)(y)) + INT);

#define gblt_rshift( x, y, z, fail ) \
    z =  makeint((unsigned long)(intval(x)) >> intval(y));

#define gblt_lshift( x, y, z, fail ) \
    z =  (q)((((long)(x) - INT) << intval(y)) + INT);

#define gblt_add( x, y, z, fail ) \
    z = (q)((unsigned long)(x) + ((unsigned long)(y) - INT));

#define gblt_subtract( x, y, z, fail ) \
    z = (q)((unsigned long)(x) - ((unsigned long)(y) - INT)); 

#define gblt_multiply( x, y, z, fail ) \
    z = (q)(((long)(x) - INT) * (intval(y)) + INT);

#define gblt_divide( x, y, z, fail ) \
{ \
  long macrotemp = (long)(y) - INT; \
  if (macrotemp == 0) goto fail; \
  z = makeint(((long)(x) - INT)/macrotemp); \
}

#define gblt_modulo( x, y, z, fail ) \
{ \
  long macrotemp = (long)(y) - INT; \
  if (macrotemp == 0) goto fail; \
  z = (q)(((long)(x) - INT) % macrotemp + INT); \
}

#define gblt_complement( x, z, fail ) \
    z = makeint((long)(x) ^ ((-1l) << ATAGBITS));

#define gblt_plus( x, z, fail ) \
    z = x;

#define gblt_minus( x, z, fail ) \
    z = (q)(2*INT-(long)(x));

#define gblt_fix( x, z, fail ) \
    z = makeint((long)float_value(x));

/* Functor Manipulation */

#define gblt_pfunctor( x, f, fail) \
    if (isatomic(x)) { \
      f = x; \
    } else if (iscons(x)) { \
      f = PERIODATOM; \
    } else if (!isref(functorp(x)->functor)) { \
      f = makesym(functoratom(functorp(x)->functor)); \
    } else  { \
      f = x; \
    }

#define gblt_arity( x, a, fail) \
    if (isatomic(x)) { \
      a = makeint(0); \
    } else if (iscons(x)) { \
      a = makeint(2); \
    } else if (!isref(functorp(x)->functor)) { \
      a = makeint(arityof(functorp(x)->functor)); \
    } else { \
      a = makeint(0); \
    }

#define gblt_arg( n, x, a, fail) \
    if (isatomic(x)) { \
      goto fail; \
    } else if (iscons(x)) { \
      if (n == makeint(1)) { \
	a = car_of(x); \
      } else if (n == makeint(2)) { \
	a = cdr_of(x); \
      } else { \
	goto fail; \
      } \
    } else if (!isref(functorp(x)->functor)) { \
      long temp = intval(n); \
      if (temp <= 0 || temp > arityof(functorp(x)->functor)) goto fail; \
      a = arg(x, temp-1); \
    } else { \
      goto fail; \
    }

/* Generic Object Manipulation */

#ifdef __STDC__
#define TableName(x)	(x##_g_data_method_table)
#else
#define TableName(x)	(x/**/_g_data_method_table)
#endif

#define jump_if_not_in_class(x, class, lab)				\
{									\
  extern struct data_object_method_table TableName(class);		\
  if (!isgobj(x) || functor_of(x) != (q)&TableName(class))		\
    goto lab;								\
}

#define gblt_size_of_vector(v, s, faillab)				\
{									\
  extern q size_of_vector();						\
  s = size_of_vector(v);						\
}

#define gblt_element_of_vector(v, k, e, faillab)			\
{									\
  extern q element_of_vector();						\
  e = element_of_vector(v,k);						\
  if ((long)e == 0) goto faillab;					\
}

#define gblt_is_string(s, faillab)					\
{ jump_if_not_in_class((s), byte__string, faillab); }

#define gblt_size_of_string(v, s, faillab)				\
{									\
  extern q size_of_string();						\
  s = size_of_string(v);						\
}

#define gblt_element_size_of_string(v, es, faillab)			\
  es = makeint(8);

#define gblt_element_of_string(v, k, e, faillab)			\
{									\
  extern q element_of_string();						\
  e = element_of_string(v,k);						\
  if ((long)e == 0) goto faillab;					\
}

/* Guard General Unification */

q eq_terms_body();

#define if_not_equal(x, y, faillab) \
if ((x)!=(y) || isref(x)) { \
  q retval; \
  switch((long)(retval = eq_terms_body((x), (y)))){ \
  case 0: break; \
  default: *reasonp++ = retval; /* fall thru */ \
  case 1: goto faillab; \
  } \
}

/* Clause Grouping */

#define otherwise(lab) \
{ if (reasonp != reasons) goto lab; }

#define alternative(lab) \
{ \
  if (reasonp != reasons) { \
    int macrotmp = check_stack_for_alternatively(allocp, reasonp); \
    allocp = heapp; \
    if (macrotmp != 0) goto lab; \
  } \
}

/* Misc */

#define gblt_assign( x, y, fail )  x =  y; 

#define gblt_not_eq( x, y, fail ) \
    if ((long)(x) == (long)(y)) goto fail;

#define gblt_diff( x, y, fail ) 				\
    if ((isatomic(y) || isatomic(x) ? (x) == (y) :		\
	 (iscons(y) ? iscons(x) :				\
	  (isfunctor(x) && functor_of(x) == functor_of(y)))))	\
	goto fail;

#define gblt_display_console(w,fail)  printl(w);

#define gblt_tag(x, y, faillab)   y = makeint(ptagof(x))
#define gblt_value(x, y, faillab) y = makeint(((unsigned long)x)>>ATAGBITS)
