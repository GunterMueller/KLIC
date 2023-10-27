/* ---------------------------------------------------------- 
%   (C)1993 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

extern q
  *do_unify(), *do_unify2(), *do_unify3(), *do_unify4(),
  *do_unify_value(), *do_unify_value2(),
  *do_unify_value3(), *do_unify_value4();

#define unify(x, y) \
{ allocp = do_unify(allocp, (x), (y)); }

#define unify2(x, y, z, w) \
{ allocp = do_unify2(allocp, (x), (y), (z), (w)); }

#define unify3(x, y, z, w, s, t) \
{ allocp = do_unify3(allocp, (x), (y), (z), (w), (s), (t)); }

#define unify4(x, y, z, w, s, t, u, v) \
{ allocp = do_unify4(allocp, (x), (y), (z), (w), (s), (t), (u), (v)); }

#ifdef COMPACTCODE
#define unify_value(x, y) \
{ allocp = do_unify_value(allocp, (x), (y)); }
#define unify_value2(x, y, z, w) \
{ allocp = do_unify_value2(allocp, (x), (y), (z), (w) }
#define unify_value3(x, y, z, w, s, t) \
{ allocp = do_unify_value3(allocp, (x), (y), (z), (w), (s), (t)) }
#define unify_value4(x, y, z, w, s, t, u, v) \
{ allocp = do_unify_value4(allocp, (x), (y), (z), (w), (s), (t), (u), (v)) }
#else
#define unify_value(x, y)\
{								\
  if (!isref(x) || derefone(x) != (x)) {			\
    allocp = do_unify_value(allocp, (x), (y));			\
  } else {							\
    derefone(x) = (y);						\
  }								\
}
#define unify_value2(x, y, z, w)\
{								\
  unify_value((x), (y));					\
  unify_value((z), (w));					\
}
#define unify_value3(x, y, z, w, s, t)\
{								\
  unify_value((x), (y));					\
  unify_value((z), (w));					\
  unify_value((s), (t));					\
}
#define unify_value4(x, y, z, w, s, t, u, v)\
{								\
  unify_value((x), (y));					\
  unify_value((z), (w));					\
  unify_value((s), (t));					\
  unify_value((u), (v));					\
}
#endif
