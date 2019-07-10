/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

extern q GC_wakeup_g_new();
extern q *G_MAKE_THE_NEW_GOAL();
extern q *do_shallow_unify(), *do_shallow_unify_value();
extern q *G_SUSPEND();

#define GG_rappend(pref) G_rappend0(GG_CLASS_NAME(),pref)
#define GG_body(fa) G_body0(GG_CLASS_NAME(),fa)

extern q GG_MAKE_HOOK_VAR();

#define GG_OBJ_SIZE G_SIZE_IN_Q(GG_OBJ_TYPE)

#ifdef __STDC__
#define GG_method_table1(class) class##_g_generator_method_table
#else
#define GG_method_table1(class) class/**/_g_generator_method_table
#endif

#define GG_method_table0(class) GG_method_table1(class)

#define GG_method_table GG_method_table0(GG_CLASS_NAME())

#define GG_USE_CLASS(class) \
extern struct generator_object_method_table GG_method_table0(class)

#ifdef __STDC__
#define GG_CLASS_NAME_STRING G_stringify(GG_CLASS_NAME())
#else
#define GG_CLASS_NAME_STRING 0
#endif

#define GG_FAIL(errmsg) fatal(errmsg)
#define GG_ERROR_IN_NEW(errmsg) \
  G_error(errmsg, "creation", "generator", GG_CLASS_NAME_STRING)
#define GG_SUCCEEDED GENERIC_SUCCEEDED
#define GG_GCREQUEST GENERIC_GCREQUEST
#define GG_RETURN { return(g_allocp); }
/* GG_RETUTN_FROM_GENERIC ? */

#define GG_DEREF_AND_TEST(x, susplab) \
{ \
  q temp0 = derefone(x); \
  if(!isref(temp0) || (x) != derefone(temp0)) { \
    (x) = temp0; \
  } else { \
    goto susp; \
  } \
}

#define GG_TERMINATE return(g_allocp)

#define GG_SUSPEND(var) \
{ \
  q newvar, tmp; \
  q argv[2]; \
  G_MAKE_VAR(newvar); \
  argv[0] = GG_TERM; \
  argv[1] = newvar; \
  tmp = GC_wakeup_g_new(2,argv,g_allocp); \
  GG_KL1_UNIFY(var,tmp); \
  return(g_allocp); \
}

#define GG_SWITCH_ON_TERM(cons,atomic,funct,dobj,susp) \
{ \
  while (1) { \
    if (!isstruct(GG_TERM)) { \
      if (atomicnotref(GG_TERM)) { \
	goto atomic; \
      } else { \
	q temp0 = derefone(GG_TERM); \
	if(isref(temp0) && (GG_TERM) == derefone(temp0)) { \
	  goto susp; \
	} else { \
	  (GG_TERM) = temp0; \
	} \
      } \
    } else if(functnotcons(GG_TERM)) {\
	if(isref(functor_of(GG_TERM))) { \
	  goto dobj; \
	} else {\
           goto funct; \
        } \
    } else goto cons; \
  } \
}

/***************************************************************************/

#define GG_DEREF(x) \
{ \
  while (1) { \
    if (!isstruct(x)) { \
      if (atomicnotref(x)) { \
	break; \
      } else { \
	q temp0 = derefone(x); \
	if(isref(temp0) && (x) == derefone(temp0)) { \
	  GG_SUSPEND(x); \
	  return g_allocp; \
	} else { \
	  (x) = temp0; \
	} \
      } \
    } else { \
      break; \
    } \
  } \
}

#define GG_TRY_TO_ALLOC(new,type,size,label) \
{ \
  G_HEAPALLOC(new, size, type); \
  if((long)real_heaplimit <= (long)(g_allocp)) { \
    goto label; \
  } \
}

#define GG_UNIFY(x,y) g_allocp = do_shallow_unify(g_allocp, x, y)

/**************************************************************************/

#define GGset_myself_for_new \
  q (*g_myself)() = GG_rappend(new)

#define GG_STD_DECL_FOR_NEW \
  G_STD_DECL; GGset_myself_for_new

#define GGDEF_NEW() \
  q GG_rappend(new) (GG_ARGC, GG_ARGV, g_allocp) \
    long GG_ARGC; \
    q    GG_ARGV[]; \
    q   *g_allocp;

#define GG_DEREF_FOR_NEW(x) \
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
	    G_MAKE_THE_NEW_GOAL(&var,&goal,g_myself, \
				GG_ARGC,GG_ARGV,g_allocp); \
          g_allocp = G_SUSPEND((x),goal,g_allocp); \
          GG_RETURN_FROM_NEW(var); \
	} else { \
	  (x) = temp0; \
	} \
      } \
    } else { \
      break; \
    } \
  } \
}

#define GGSET_NEWOBJ_FOR_NEW(newgobj, type) \
{ \
  q res; \
  G_HEAPALLOC_WITH_CHECK(newgobj,GG_OBJ_SIZE, \
			 type, g_allocp, res); \
  if(GG_GCREQUEST == res) { \
    q var; \
    struct goalrec *goal; \
    g_allocp = G_MAKE_THE_NEW_GOAL(&var,&goal,g_myself, \
				   GG_ARGC,GG_ARGV,g_allocp); \
    G_PUSH_GOAL_TO_SPECIAL_QUEUE(goal); \
    GG_RETURN_FROM_NEW(var); \
  } \
  newgobj->method_table = &GG_method_table; \
}

#define GG_RETURN_FROM_NEW(var) { heapp = g_allocp; return(var); }

/****************************************************/
#define GGSET_NEWOBJ(newgobj) \
{ \
  GD_ALLOC_AREA(newgobj, (GG_OBJ_TYPE *),GG_OBJ_SIZE); \
  newgobj->method_able = GG_SELF->method_table; \
}

/* temporary */
#define GG_SUSPEND_GOAL(var,goal) \
{ g_allocp = GC_suspend_goal(var,goal,g_allocp); }

/* temporary
#define GGSET_UNIFY_GOAL(goal,x,y) \
{ g_allocp = GC_make_unify_goal(goal,x,y,g_allocp); }

#define GG_GSUCCESS GENERIC_SUCCEEDED
#define GG_GFAILURE GENERIC_FAILED

#define GG_GRETURN(x) return((q)(x))

#define GG_GFAIL return((q)GG_GFAILURE)
#define GG_GSUCCEED return((q)GG_GSUCCESS)

/**********************************************************************/

#define GGDEF_UNIFY() \
static q* \
 GG_rappend(active_unify) (GG_SELF, GG_TERM, g_allocp) \
  q GG_SELF; \
  q GG_TERM; \
  q *g_allocp;

#define GGDEF_SUSPEND() \
static q \
GG_rappend(suspend) (GG_SELF, GG_GOAL) \
  q GG_SELF; \
  struct goalrec* GG_GOAL;

#define GGDEF_GENERATE() \
static q \
GG_rappend(generate) (GG_SELF, g_allocp) \
  GG_OBJ_TYPE *GG_SELF; \
  q *g_allocp;

#define GG_PRINT(x) klic_fprintf(g_fp, (x))

#define GG_PUTC(x) klic_fputc(x,g_fp)

#define GG_RETURN_FROM_PRINT return(1L)

#define GGDEF_PRINT() \
  static \
  long \
  GG_rappend(print) (GG_SELF,g_fp,g_depth,g_length) \
    GG_OBJ_TYPE *GG_SELF; \
    FILE *g_fp; \
    unsigned long g_depth; \
    unsigned long g_length;

#define GG_PRINT_KL1_TERMS(x,b,s) \
{ \
  int i; \
  general_print((((q *)(x))+(b)),g_fp,g_depth,g_length); \
  for(i=(b)+1; i<(b)+(s); i++){ \
    klic_fprintf(g_fp,","); \
    general_print((((q *)(x))+i),g_fp,g_depth,g_length); \
  } \
}

/******************************************************************/


#define GGDEF_GC() \
static \
q * GG_rappend(gc) (GG_SELF, g_allocp, g_sp) \
  GG_OBJ_TYPE *GG_SELF; \
  q *g_allocp; \
  q **g_sp;

#define GGSET_NEWOBJ_IN_NEWGEN(newobj) \
  { \
    unsigned long size = GG_OBJ_SIZE; \
    *g_allocp = (q) GG_SELF->method_table; \
    newobj = (GG_OBJ_TYPE *) g_allocp; \
    g_allocp += size; \
    if(g_allocp > real_heaplimit) fatal("not enough space collected"); \
  }

#define GG_RETURN_FROM_GC(newgobj) \
{ \
  gcsp  = g_sp; \
  heapp = g_allocp; \
  return(((q *)(newgobj))); \
}

#define GGDEF_REGIST() \
  static \
  long \
  GG_rappend(regist) (GG_SELF,g_allocp) \
    GG_OBJ_TYPE *GG_SELF; \
    q *g_allocp;

#define GGDEF_DEALLOCATE() \
  static \
  long \
  GG_rappend(deallocate) (GG_SELF) \
    GG_OBJ_TYPE *GG_SELF;

#define GGDEF_CLOSE() \
  static q * \
  GG_rappend(close) (GG_SELF) \
    GG_OBJ_TYPE *GG_SELF;

#define GGDEF_GENERIC() \
static q *\
 GG_rappend(active_unify) (GG_SELF,GG_TERM,g_allocp) \
    GG_OBJ_TYPE *GG_SELF; \
    q GG_TERM; \
    q *g_allocp;

/*******************************************/

#define GGSET_INTARG_FOR_NEW(var,argv_i) \
{ \
  GG_DEREF_FOR_NEW(argv_i); \
  if(!G_ISINT(argv_i)) GG_FAIL("not integer"); \
  (var) = G_INTVAL(argv_i); \
}

#define GGSET_INTARG(var,argv_i) \
{ \
  GG_DEREF(argv_i); \
  if(!G_ISINT(argv_i)) GG_FAIL("not integer"); \
  (var) = G_INTVAL(argv_i); \
}

#define GGSET_INTARG_WITHIN_RANGE(var,argv_i,llim,ulim) \
{ \
  GG_DEREF(argv_i); \
  if(!G_ISINT(argv_i)) GG_FAIL("not integer"); \
  (var) = G_INTVAL(argv_i); \
  if( (llim>(var)) || ((var)>=ulim) ) GG_FAIL("out of range"); \
}

#define GGSET_NEW_CONS(var)  G_HEAPALLOC(var,2,makecons)

#define GGSET_VAR(self) self = G_MAKEREF(&self)

extern struct predicate predicate_unify__term__dcode_xunify__goal_2;
#define GG_KL1_UNIFY(x, y) \
{ \
  struct goalrec *gp; \
  G_HEAPALLOC(gp, 4, (struct goalrec *)); \
  gp->pred = &predicate_unify__term__dcode_xunify__goal_2; \
  gp->args[0] = x; \
  gp->args[1] = y; \
  resume_same_prio(gp); \
}

/*** for Distribute KLIC system ***/

#define GGDEF_ENCODE() \
  static q \
  GG_rappend(encode) (GG_SELF, buffer, depth)\
    GG_OBJ_TYPE *GG_SELF;\
    combuf *buffer;\
    long depth;
