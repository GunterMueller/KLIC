/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
/* external module/class reference */

#define G_USE_PREDICATE(name) \
extern Const struct predicate name

/* type judgement. */
#define G_ISATOMIC(x) isatomic(x)
#define G_ISCONS(x) iscons(x)
#define G_ISFUNCTOR(x) isfunctor(x)
#define G_ISREF(x) isref(x)
#define G_ISSTRUCT(x) isstruct(x)
#define G_FUNCTNOTCONS(x) functnotcons(x)
#define G_ATOMICNOTREF(x) atomicnotref(x)
#define G_ISINT(x) isint(x)
#define G_ISSYM(x) issym(x)
#define G_ISGOBJ(x) (isfunctor(x) && isref(functor_of(x)))
#define G_EQATOM(x, y) eqatom(x, y)

/* data manipulation macro. */
#define G_FUNCTOR_OF(s) functor_of(s)
#define G_ARG(s, k) arg(s, k)
#define G_CAR_OF(x) car_of(x)
#define G_CDR_OF(x) cdr_of(x)
#define G_PCAR_OF(x) pcar_of(x)
#define G_PCDR_OF(x) pcdr_of(x)
#define G_DEREFONE(x) derefone(x)

/* convertion. C->KLIC. */
#define G_MAKEINT(n) makeint(n)
#define G_MAKECINT(n) makecint(n)
#define G_MAKESYM(n) makesym(n)
#define G_MAKEREF(x) makeref(x)
#define G_MAKEATOMIC(adr) makeatomic(adr) /* for non-heap addr */
#define G_MAKEPF(x) makepf(x) /* for principal functors */
#define G_MAKECONS(x) makecons(x)
#define G_MAKEFUNCTOR(x) makefunctor(x)

/* convertion. KLIC->C. */
#define G_INTVAL(x) intval(x)
#define G_SYMVAL(x) symval(x)
#define G_REFP(x) refp(x)
#define G_CONSP(x) consp(x)
#define G_FUNCTORP(x) functorp(x)
#define G_GOALP(x) goalp(x)
#define G_SUSPP(x) suspp(x)
#define G_PREDP(x) predp(x)

#ifdef __STDC__
#define G_rappend1(class,post) class##_g_##post
#define G_functor(fa) functor_##fa
#define G_atom(fa) atom_##fa
#define G_guard1(class,fa) class##_g_guard_##fa
#define G_body1(class,fa) class##_g_body_##fa
#define G_stringify1(s) #s
#define G_stringify(s) G_stringify1(s)
#else
#define G_rappend1(class,post) class/**/_g_/**/post
#define G_functor(fa) functor_/**/fa
#define G_atom(fa) atom_/**/fa
#define G_guard1(class,fa) class/**/_g_guard_/**/fa
#define G_body1(class,fa) class/**/_g_body_/**/fa
#define G_stringify(s) "s"
#endif

#define G_rappend0(class,post) G_rappend1(class,post)
#define G_guard0(class,fa) G_guard1(class,fa)
#define G_body0(class,fa) G_body1(class,fa)

#define G_HEAPALLOC(from,size,type) \
{ \
  from = type(g_allocp); \
  g_allocp += size; \
}

#define G_HEAPALLOC_WITH_CHECK(from,size,type,allocp,res) \
{ \
  if ((long)real_heaplimit <= (long)((allocp)+(size))) { \
    this_more_space += (size); \
    heaplimit = 0; \
    (res) = GENERIC_GCREQUEST; \
  } else { \
    G_HEAPALLOC((from),(size),type); \
    (res) = GENERIC_SUCCEEDED; \
  } \
}

#define G_MAKE_VAR(x) \
{ G_HEAPALLOC(x,1,(q)); derefone(x) = x; }

#define G_STD_DECL struct global_variables *glbl = My_glbl

#define G_SIZE_IN_Q(type) ((sizeof(type)+sizeof(q)-1)/sizeof(q))

extern q** make_larger_stack();

#define G_COPY_KL1_TERM_TO_NEWGEN(from,to) \
{ \
  if ((g_sp) == gcmax) { \
    (g_sp) = make_larger_stack(g_sp); \
  } \
  if(isref(from)) { \
    q *newword = g_allocp++; \
    derefone(newword) = from; \
    to = makeref(newword); \
    *(g_sp++) = newword; \
  } else { \
    to = from; \
    *(g_sp++) = &(to); \
  } \
}

#define G_PUSH_GOAL(goal) \
{ resume_same_prio(goal); }

#define G_PUSH_GOAL_TO_SPECIAL_QUEUE(goal) \
  G_PUSH_GOAL(goal)

/*
  deta flag for Object Table
*/

#define G_DATA 0
#define G_CONSUMER 1
#define G_GENERATOR 2

/*
  external functions
*/

#ifndef DEFINE_GLOBAL
#ifdef __GNUC__
extern Volatile voidfn G_error;
#else
extern voidfn G_error;
#endif
#endif
