/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

extern q eq_terms_body();
extern q *GD_MAKE_GENERIC_GOAL(), *G_MAKE_THE_NEW_GOAL();
extern q *do_shallow_unify(), *do_shallow_unify_value();
extern q *G_SUSPEND();

#define GD_rappend(pref) G_rappend0(GD_CLASS_NAME(),pref)
#define GD_guard(fa) G_guard0(GD_CLASS_NAME(),fa)
#define GD_body(fa) G_body0(GD_CLASS_NAME(),fa)
#define GD_body_no_arity(fa) GD_body0(GD_CLASS_NAME(),fa)

#define GD_OBJ(x) G_MAKEFUNCTOR(x)

#ifdef __STDC__
#define GD_method_table1(class) class##_g_data_method_table
#else
#define GD_method_table1(class) class/**/_g_data_method_table
#endif

#define GD_method_table0(class)  GD_method_table1(class)
#define GD_method_table GD_method_table0(GD_CLASS_NAME())

#define GD_USE_CLASS(class) \
extern struct data_object_method_table GD_method_table0(class)

#ifdef __STDC__
#define GD_CLASS_NAME_STRING G_stringify(GD_CLASS_NAME())
#else
#define GD_CLASS_NAME_STRING 0
#endif

#define GD_FAIL(errmsg)  fatal(errmsg)
#define GD_UNIFY_FAIL \
  G_error("Failure", "active unification", "data", GD_CLASS_NAME_STRING)
#define GD_ERROR_IN_NEW(errmsg) \
  G_error(errmsg, "creation", "data", GD_CLASS_NAME_STRING)
#define GD_ERROR_IN_UNIFY(errmsg) \
  G_error(errmsg, "unification", "data", GD_CLASS_NAME_STRING)
#define GD_ERROR_IN_METHOD(errmsg, methodname) \
  G_error(errmsg, methodname, "data", GD_CLASS_NAME_STRING)

#define GD_RETURN return(g_allocp) 
#define GD_GCREQUEST GENERIC_GCREQUEST

#define GD_GSUCCESS (long)GENERIC_SUCCEEDED
#define GD_GFAILURE (long)GENERIC_FAILED

#define GD_GRETURN(x) return((q) x)

#define GD_GUNIFY_FAIL GD_GFAIL
#define GD_GFAIL return((q) GD_GFAILURE)
#define GD_GSUCCEED return((q) GD_GSUCCESS)

/**************************************************/
#define GD_ALLOC_AREA(new,type,size) \
{ \
  q res; \
  G_HEAPALLOC_WITH_CHECK(new,size,type,g_allocp,res); \
  if(GD_GCREQUEST == res){ \
    struct goalrec *goal; \
    g_allocp = \
      GD_MAKE_GENERIC_GOAL(&goal,makefunctor(GD_SELF),g_method_functor, \
			   GD_ARGV,g_allocp); \
    G_PUSH_GOAL(goal); \
    GD_RETURN; \
  } \
}

#define GD_ALLOC_AREA_FOR_NEW(new,type,size) \
{ \
  q res; \
  G_HEAPALLOC_WITH_CHECK(new,size,type,g_allocp,res); \
  if(GD_GCREQUEST == res){ \
    q var; \
    struct goalrec *goal; \
    g_allocp = \
      G_MAKE_THE_NEW_GOAL(&var,&goal,g_myself,GD_ARGC,GD_ARGV,g_allocp); \
    G_PUSH_GOAL_TO_SPECIAL_QUEUE(goal); \
    GD_SUSPEND_NEW(var); \
  } \
}

/***************************************************************************/

#define GDSET_NEWOBJ_FOR_NEW(newgobj,size) \
{ \
  q res; \
  G_HEAPALLOC_WITH_CHECK((newgobj),(size),(GD_OBJ_TYPE *),g_allocp,res); \
  if(GENERIC_GCREQUEST == res){ \
    q var; \
    struct goalrec *goal; \
    g_allocp = \
      G_MAKE_THE_NEW_GOAL(&var,&goal,g_myself,GD_ARGC,GD_ARGV,g_allocp); \
    G_PUSH_GOAL_TO_SPECIAL_QUEUE(goal); \
    GD_SUSPEND_NEW(var); \
  } \
  newgobj->method_table = &GD_method_table; \
}

#define GDSET_NEWOBJ(newgobj) \
{ \
  q res; \
  G_HEAPALLOC_WITH_CHECK(newgobj,(GD_OBJ_SIZE(GD_SELF)), \
			 (GD_OBJ_TYPE *),g_allocp,res); \
  if (GD_GCREQUEST == res) { \
    struct goalrec *goal; \
    g_allocp = \
      GD_MAKE_GENERIC_GOAL(&goal,makefunctor(GD_SELF), \
			   g_method_functor,GD_ARGV, g_allocp); \
    G_PUSH_GOAL(goal); \
    GD_RETURN; \
  } \
  newgobj->method_table = GD_SELF->method_table; \
}

#define GDSET_NEWOBJ_IN_NEWGEN(newobj) \
  { \
    unsigned long size = GD_OBJ_SIZE(GD_SELF); \
    newobj = (GD_OBJ_TYPE *) g_allocp; \
    newobj->method_table = GD_SELF->method_table; \
    g_allocp += size; \
    if((g_allocp) > real_heaplimit) fatal("not enough space collected"); \
  }

/***************************************************************************/
#define GD_GDEREF(x) \
{ \
  while (1) { \
    if (!isstruct(x)) { \
      if (atomicnotref(x)) { \
	break; \
      } else { \
	q temp0 = derefone(x); \
	if(isref(temp0) && (x) == derefone(temp0)) { \
          return(x); \
	} else { \
	  (x) = temp0; \
	} \
      } \
    } else { \
      break; \
    } \
  } \
}

#define GD_DEREF_AND_HOOK(obj,method_functor,argv) \
{ \
  while (1) { \
    if (!isstruct(obj)) { \
      if (atomicnotref(obj)) { \
	break; \
      } else { \
	q temp0 = derefone(obj); \
	if(isref(temp0) && (obj) == derefone(temp0)) { \
          struct goalrec *goal; \
          g_allocp = \
	    GD_MAKE_GENERIC_GOAL(&goal,(obj),method_functor,argv,g_allocp); \
	  g_allocp = G_SUSPEND((obj),goal,g_allocp); \
          GD_RETURN; \
	} else { \
	  (obj) = temp0; \
	} \
      } \
    } else { \
      break; \
    } \
  } \
}

#define GD_DEREF(x) \
{ \
  while (1) { \
    if (!isstruct(x)) { \
      if (atomicnotref(x)) { \
	break; \
      } else { \
	q temp0 = derefone(x); \
	if(isref(temp0) && (x) == derefone(temp0)) { \
          struct goalrec *goal; \
          g_allocp = \
	    GD_MAKE_GENERIC_GOAL(&goal,makefunctor(GD_SELF), \
				 g_method_functor,GD_ARGV,g_allocp); \
          g_allocp = G_SUSPEND((x),goal,g_allocp); \
          GD_RETURN; \
	} else { \
	  (x) = temp0; \
	} \
      } \
    } else { \
      break; \
    } \
  } \
}



#define GD_DEREF_FOR_NEW(x) \
{ \
  while (1) { \
    if (!isstruct(x)) { \
      if (atomicnotref(x)) { \
	break; \
      } else { \
	q temp0 = derefone(x); \
	if(isref(temp0) && (x) == derefone(temp0)) { \
          q var; \
          struct goalrec *goal; \
       	  g_allocp = \
	    G_MAKE_THE_NEW_GOAL(&var,&goal,g_myself,GD_ARGC,\
				GD_ARGV,g_allocp); \
          g_allocp = G_SUSPEND((x),goal,g_allocp); \
          GD_SUSPEND_NEW(var); \
	} else { \
	  (x) = temp0; \
	} \
      } \
    } else { \
      break; \
    } \
  } \
}


/**************************************************************************/
#define GDDEF_NEW() \
  q \
  GD_rappend(new) (GD_ARGC,GD_ARGV,g_allocp) \
    long GD_ARGC; \
    q GD_ARGV[]; \
    q *g_allocp;

#define GD_SUSPEND_NEW(v) \
{ heapp = g_allocp; return(v); }

#define GD_RETURN_FROM_NEW(x) \
{ heapp = g_allocp; return(makefunctor(x)); }

#define GD_STD_DECL_FOR_NEW \
  G_STD_DECL; \
  GDset_myself_for_new

#define GDset_myself_for_new \
  q (*g_myself)() = GD_rappend(new)

/**************************************************************************/
#define GD_GUNIFY(x,y) eq_terms_body(x,y)

#define GD_UNIFY(x,y)  { g_allocp = do_shallow_unify(g_allocp,x,y); }
#define GD_UNIFY_VALUE(x,y) \
{								\
  if (!isref(x) || derefone(x) != (x)) {			\
    g_allocp = do_shallow_unify_value(g_allocp, (x), (y));	\
  } else {							\
    derefone(x) = (y);						\
  }								\
}

/****************************************************/
#define GD_SWITCH_ON_METHOD switch(g_method_functor)
#define GD_SWITCH_ON_ARITY switch (arities[g_method_functor])

#define GD_METHOD_CASE_DIRECT(fa) \
 case (long) G_functor(fa)

#define GD_METHOD_CASE(fa) \
   case (long) G_functor(fa): \
  g_allocp = GD_body(fa) (GD_SELF,g_method_functor,GD_ARGV,g_allocp); \
  break

#define GD_METHOD_CASE_DEFAULT \
  default: fatal("undefined method")

#define GD_SWITCH_ON_GMETHOD_NAME \
  switch(functors[g_method_functor - FUNCTORNUMBERBASE])

#define GD_METHOD_NAME_CASE(sym) \
 case ((long)(G_atom(sym))) : \
  g_allocp = GD_body(sym)(GD_SELF, g_method_functor, \
					GD_ARGV, g_allocp); \
  break

#define GD_METHOD_NAME_CASE_DEFAULT default: fatal("undefined method")

#define GDDEF_METHOD(fa) \
  static q * \
  GD_body(fa) (GD_SELF,g_method_functor,GD_ARGV,g_allocp) \
    GD_OBJ_TYPE * GD_SELF; \
    long g_method_functor; \
    q GD_ARGV[]; \
    q *g_allocp;

#define GDDEF_METHOD_NO_ARITY(fn) \
  q * \
  GD_body_no_arity(fn)(GD_SELF,g_method_functor,GD_ARGV,g_allocp) \
  GD_OBJ_TYPE *GD_SELF; \
  long g_method_functor; \
  q GD_ARGV[]; \
  q *g_allocp;

/**********************************************************************/
#define GDDEF_GUNIFY() \
static q \
  GD_rappend(passive_unify) (GD_SELF, GD_OTHER) \
    GD_OBJ_TYPE * GD_SELF; \
    GD_OBJ_TYPE * GD_OTHER;

#define GDDEF_UNIFY() \
  static q *\
  GD_rappend(active_unify) (GD_SELF,GD_OTHER,g_allocp) \
    GD_OBJ_TYPE * GD_SELF; \
    GD_OBJ_TYPE * GD_OTHER; \
    q *g_allocp;

#define GD_PUTC(x) klic_putc(x,g_fp)

#define GD_RETURN_FROM_PRINT return(1L)

#define GDDEF_PRINT() \
  static long \
  GD_rappend(print) (GD_SELF,g_fp,g_depth,g_length) \
    GD_OBJ_TYPE * GD_SELF; \
    FILE *g_fp; \
    unsigned long g_depth; \
    unsigned long g_length;

#define GD_PRINT(x) klic_fprintf(g_fp, (x))

#define GD_PRINTF(s, arg) \
{ klic_fprintf(g_fp, s, arg); }

#define GD_PRINT_KL1_TERM(x) \
{ general_print(x, g_fp, g_depth, g_length); }

#define GD_PRINT_KL1_TERMS(x,b,s) \
{ \
  int i; \
  general_print((((q *)(x))+(b)),g_fp,g_depth,g_length); \
  for(i=(b)+1; i<(b)+(s); i++){ \
    klic_fprintf(g_fp,","); \
    general_print((((q *)(x))+i),g_fp,g_depth,g_length); \
  } \
}

/******************************************************************/

#define GDDEF_COMPARE() \
  static q \
  GD_rappend(compare) (GD_SELF,GD_OTHER) \
    GD_OBJ_TYPE * GD_SELF, * GD_OTHER;

#define GDDEF_HASH() \
  static q \
  GD_rappend(hash) (GD_SELF,GD_LEVEL) \
    GD_OBJ_TYPE * GD_SELF; \
    long GD_LEVEL;

#define GDDEF_GC() \
 static q * \
  GD_rappend(gc) (GD_SELF,g_allocp,g_sp) \
    GD_OBJ_TYPE * GD_SELF; \
    q *g_allocp; \
    q **g_sp;

#define GD_RETURN_FROM_GC(newgobj) \
{ \
  gcsp = g_sp; \
  heapp = g_allocp; \
  return(((q *)(newgobj))); \
}

#define GD_COPY_KL1_TERM_TO_NEWGEN(from,to) G_COPY_KL1_TERM_TO_NEWGEN(from,to) 

#define GDDEF_REGIST() \
  static long \
  GD_rappend(regist) (GD_SELF,g_allocp) \
    GD_OBJ_TYPE * GD_SELF; \
    q *g_allocp;

#define GDDEF_DEALLOCATE() \
  static long \
  GD_rappend(deallocate) (GD_SELF) \
    GD_OBJ_TYPE * GD_SELF;

#define GDDEF_CLOSE() \
  static q * \
  GD_rappend(close) (GD_SELF) \
    GD_OBJ_TYPE * GD_SELF; \

#define GDDEF_GENERIC() \
static q * \
 GD_rappend(generic) (GD_SELF,g_method_functor,GD_ARGV,g_allocp) \
    GD_OBJ_TYPE * GD_SELF; \
    long g_method_functor; \
    q GD_ARGV[]; \
    q *g_allocp;

/**********************************************************************/
/*  guard utility */
#define GDDEF_GGENERIC() \
static q \
 GD_rappend(guard_generic) (GD_SELF,g_method_functor,GD_ARGV) \
    GD_OBJ_TYPE * GD_SELF; \
    long g_method_functor; \
    q GD_ARGV[];

#define GD_SWITCH_ON_GMETHOD switch(g_method_functor)
#define GD_GMETHOD_CASE_DIRECT \
  case (long) G_functor(fa)
#define GD_GMETHOD_CASE(fa) \
  case (long) G_functor(fa): \
  { \
    q retval = (q) GD_guard(fa) (GD_SELF,g_method_functor,GD_ARGV); \
    GD_GRETURN(retval); \
  } \
  break

#define GDDEF_GMETHOD(fa) \
  static q \
  GD_guard(fa) (GD_SELF,g_method_functor,GD_ARGV) \
    GD_OBJ_TYPE * GD_SELF; \
    long g_method_functor; \
    q GD_ARGV[];

#define GD_GMETHOD_CASE_DEFAULT \
  default: \
  GD_GFAIL

/*******************************************/
/* utility */
#define GDSET_INTARG_FOR_NEW(var,argv_i) \
{ \
  while (!G_ISINT(argv_i)) { \
    if (!G_ISREF(argv_i)) { \
      GD_ERROR_IN_NEW("Non-integer parameter"); \
    } \
    GD_DEREF_FOR_NEW(argv_i); \
  } \
  (var) = G_INTVAL(argv_i); \
}

#define GDSET_INTARG(var,argv_i) \
{ \
  while (!G_ISINT(argv_i)) { \
    if (!G_ISREF(argv_i)) { \
      GD_ERROR_IN_METHOD("Non-integer paramter", 0); \
    } \
    GD_DEREF(argv_i); \
  } \
  (var) = G_INTVAL(argv_i); \
}

#define GDSET_SYMARG_FOR_NEW(var, arg) \
{ \
  while (!G_ISSYM(arg)) { \
    if (!G_ISREF(arg)) GD_ERROR_IN_NEW("Non-atom paramter"); \
    GD_DEREF_FOR_NEW(arg); \
  } \
  (var) = (arg); \
}

#define GDSET_SYMARG(var, arg) \
{ \
  while (!G_ISSYM(arg)) { \
    if (!G_ISREF(arg)) GD_ERROR_IN_METHOD("Non-atom paramter", 0); \
    GD_DEREF(arg); \
  } \
  (var) = (arg); \
}

#define GDSET_INTARG_WITHIN_RANGE(var,argv_i,llim,ulim) \
{ \
  while (!G_ISINT(argv_i)) { \
    if (!G_ISREF(argv_i)) GD_ERROR_IN_METHOD("Non-integer paramter", 0); \
    GD_DEREF(argv_i); \
  } \
  (var) = G_INTVAL(argv_i); \
  if ((llim>(var)) || ((var)>=ulim)) \
    GD_ERROR_IN_METHOD("out of range", 0); \
}

#define GDSET_GINTARG_WITHIN_RANGE(var,x,from,to) \
{ \
  while (!G_ISINT(x)) { \
    if (!G_ISREF(x)) GD_GFAIL; \
    GD_GDEREF(x); \
  } \
  var = G_INTVAL(x); \
  if( ((from)>var) || (var>=(to)) ) GD_GFAIL; \
}

/*******************************************/
#define GD_IS_CLASS(class,obj) \
   ((struct data_object_method_table *)(&(GD_method_table0(class))) == ((struct data_object_method_table *)functor_of(obj)))

#define GD_CALL_GMETHOD(obj,method,argv) \
     (q)((((struct data_object *)functorp(obj))->method_table)->g_generic) \
             ((struct data_object *)functorp(obj), G_functor(method),argv)
	 
#define GD_CALL_METHOD(obj,method,argv) \
     g_allocp = ((((struct data_object *)functorp(obj))-> \
		  method_table)->body_generic) \
		  ((struct data_object *)functorp(obj), \
                        G_functor(method),argv,g_allocp)

#define GD_NEW(class,argc,argv) \
     (q) GD_rappend(class,new) (argc,argv,g_allocp)


#define GDDEF_ENCODE() \
 q \
  GD_rappend(encode) (GD_SELF,buffer,depth) \
    GD_OBJ_TYPE * GD_SELF; \
    combuf *buffer; \
    long depth;

#define GDDEF_SHMCOPY() \
 q \
  GD_rappend(encode) (GD_SELF) \
    GD_OBJ_TYPE * GD_SELF;
