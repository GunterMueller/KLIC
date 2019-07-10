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

/*
  datas for general utility with consumer object
*/

/****************
  suspension_reason

  shows result of the processing for consumer object.

  0: normal;
  1: GC request;
  other: the process is suspended. pointer of the reason of the prosess.
******************/
extern q suspension_reason;

/************
  rest_of_stream

  indicates the next hooked term. This variable is meaningful
  just `suspension_reason' is 0.

  0: normal;
  other: the next hooked term
**************/
extern q rest_of_stream;

/*
  this function has replaced by G_suspend
  make new hook for object for gmethod.c
q make_new_hook_for_gobj();
*/

/*
  this function has not already used
struct goalrec *call_generic_new();
*/

extern q method_result;

#define GC_rappend(pref) G_rappend0(GC_CLASS_NAME(),pref)
#define GC_body(fa) G_body0(GC_CLASS_NAME(),fa)

extern q GC_MAKE_HOOK_VAR();

#ifdef __STDC__
#define GC_method_table1(class) class##_g_data_method_table
#else
#define GC_method_table1(class) class/**/_g_data_method_table
#endif

#define GC_method_table0(class) GC_method_table1(class)

#define GC_method_table GC_method_table0(GC_CLASS_NAME())

#define GC_USE_CLASS(class) \
extern struct consumer_object_method_table GC_method_table0(class)

#define GC_CLASS_NAME_STRING G_stringify(GC_CLASS_NAME())

#define    GC_FAIL(errmsg)    fatal(errmsg)
#define	   GC_ERROR_IN_NEW(errmsg) \
  G_error(errmsg, "creation", "consumer", GC_CLASS_NAME_STRING)
#define    GC_SUCCEEDED       GENERIC_SUCCEEDED
#define    GC_GCREQUEST       GENERIC_GCREQUEST
#define    GC_RETURN      { method_result = 0; return(g_allocp); }
#define    GC_RETURN_WITH_HOOK(x) \
     { \
       method_result = GC_SUCCEEDED; \
       rest_of_stream = x; \
       return(g_allocp); \
      }
#define    GC_RETURN_FROM_GENERIC \
     { if (G_ISREF(GC_TERM)) {GC_RETURN_WITH_HOOK(GC_TERM);} \
       else if (G_ISREF(G_CDR_OF(GC_TERM))) \
         {GC_RETURN_WITH_HOOK(G_CDR_OF(GC_TERM));} \
       else if (G_CDR_OF(GC_TERM) == NILATOM) { GC_TERMINATE;} \
       else { debug_printf("### %k ###", GC_TERM); fatal("Illegal data"); } \
      }

#define GC_TERMINATE \
     { \
       method_result = GC_SUCCEEDED;\
       rest_of_stream = 0; \
       return(g_allocp); \
      }

#define GC_SUSPEND(var) \
     { \
       q newvar, tmp; \
       q argv[2]; \
       method_result = GC_SUCCEEDED; \
       G_MAKE_VAR(newvar); \
       rest_of_stream = newvar; \
       argv[0] = GC_TERM; \
       argv[1] = newvar; \
       tmp = GC_wakeup_g_new(2,argv,g_allocp); \
       g_allocp = heapp; \
       GC_KL1_UNIFY(var,tmp); \
       return(g_allocp); \
     }

/**************************************************/

#define GC_DEREF_AND_TEST(x, susp) \
{ \
  q temp0 = derefone(x); \
  if(!isref(temp0) || (x) != derefone(temp0)) { \
    (x) = temp0; \
  } else { \
    goto susp; \
  } \
}


#define GC_SWITCH_ON_TERM(cons,atomic,funct,dobj,susp) \
{ \
  while (1) { \
    if (!isstruct(GC_TERM)) { \
      if (atomicnotref(GC_TERM)) { \
	goto atomic; \
      } else { \
	q temp0 = derefone(GC_TERM); \
	if(isref(temp0) && (GC_TERM) == derefone(temp0)) { \
	  goto susp; \
	} else { \
	  (GC_TERM) = temp0; \
	} \
      } \
    } else if(functnotcons(GC_TERM)) {\
	if(isref(functor_of(GC_TERM))) { \
	  goto dobj; \
	} else {\
           goto funct; \
        } \
    } else goto cons; \
  } \
}

#define GC_SWITCH_ON_METHOD_BEFORE(atomic,list,functor,generic,susp,illegal_data) \
 while (1) {\
 switch(ptagof(GC_TERM)) { \
 case CONS: \
  { \
   q g_car = G_CAR_OF(GC_TERM); \
   switch(ptagof(g_car)) { \
   case ATOMIC: goto atomic; \
   case CONS: goto list; \
   case FUNCTOR: \
     if (isref(G_FUNCTOR_OF(g_car))) goto generic; \
     else goto functor; \
   default: \
     GC_SUSPEND(g_car); \
   } \
  } \
 case ATOMIC: \
  if (GC_TERM == NILATOM) { \
      GC_TERMINATE; \
  } else goto illegal_data; \
 case FUNCTOR: goto functor; \
 case VARREF: \
    GC_DEREF_AND_TEST(GC_TERM, susp); \
 default: goto illegal_data; \
 } \
}

#define GC_SWITCH_ON_ATOM_METHOD switch((unsigned long)(G_CAR_OF(GC_TERM)))

#define GC_ATOM_METHOD_CASE(atom) \
   case G_MAKESYM(atom): \
     { q g_car = G_CAR_OF(G_atom(GC_TERM)); \
         g_allocp = GC_body(g_car) (GC_SELF,g_car,g_allocp); \
       if (method_result) GC_SUSPEND(method_result); \
       GC_RETURN; \
     }

#define GC_SWITCH_ON_FUNCTOR_METHOD \
          switch((long)G_FUNCTOR_OF(G_CAR_OF(GC_TERM)))

#define GC_FUNCTOR_METHOD_CASE(fa) \
   case (long)G_MAKESYM(G_functor(fa)): \
     { q g_car = G_CAR_OF(GC_TERM); \
       g_allocp = GC_body(fa) (GC_SELF,g_car,g_allocp); \
       if (method_result) GC_SUSPEND(method_result); \
       break; \
     }

/***************************************************************************/

#define GC_DEREF(x) \
{ \
  while (1) { \
    if (!isstruct(x)) { \
      if (atomicnotref(x)) { \
	break; \
      } else { \
	q temp0 = derefone(x); \
	if(isref(temp0) && (x) == derefone(temp0)) { \
	  GC_SUSPEND(x); \
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


#define GC_TRY_TO_ALLOC(new,type,size,gc_request) \
{ \
  q res; \
  G_HEAPALLOC_WITH_CHECK(new,size,type,g_allocp,res); \
  if (GC_GCREQUEST == res) goto gc_request; \
}


#define GC_UNIFY(x,y)  g_allocp = do_shallow_unify(g_allocp,x,y)
#define GC_UNIFY_VALUE(x,y)					\
{								\
  if (!isref(x) || derefone(x) != (x)) {			\
    g_allocp = do_shallow_unify_value(g_allocp, (x), (y));		\
  } else {							\
    derefone(x) = (y);						\
  }								\
}

/**************************************************************************/

#define GCset_myself_for_new \
  q (*g_myself)() = GC_rappend(new)

#define GC_STD_DECL_FOR_NEW \
  G_STD_DECL; \
  GCset_myself_for_new


#define GCDEF_NEW() \
  q \
  GC_rappend(new) (GC_ARGC,GC_ARGV,g_allocp) \
    long GC_ARGC; \
    q GC_ARGV[]; \
    q *g_allocp;

#define GC_DEREF_FOR_NEW(x) \
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
				GC_ARGC,GC_ARGV,g_allocp); \
          g_allocp = G_SUSPEND((x),goal,g_allocp); \
          GC_RETURN_FROM_NEW(var); \
	} else { \
	  (x) = temp0; \
	} \
      } \
    } else { \
      break; \
    } \
  } \
}


#define GCSET_NEWOBJ_FOR_NEW(newgobj,size) \
  { \
    q res; \
    G_HEAPALLOC_WITH_CHECK(newgobj,size,(GC_OBJ_TYPE *),g_allocp,res); \
    if(GC_GCREQUEST == res){ \
      q var; \
      struct goalrec *goal; \
      g_allocp = \
	G_MAKE_THE_NEW_GOAL(&var,&goal,g_myself,GC_ARGC,GC_ARGV,g_allocp); \
      G_PUSH_GOAL_TO_SPECIAL_QUEUE(goal); \
      GC_RETURN_FROM_NEW(var); \
    } \
  newgobj->method_table = &GC_method_table; \
 }

#define GC_RETURN_FROM_NEW(x) \
{ heapp = g_allocp; return(x); }


/****************************************************/
#define GC_SWITCH_ON_METHOD switch(g_method_functor)

#define GC_METHOD_CASE(fa) \
  case (long) G_functor(fa): \
  g_allocp = GC_body(fa) (GC_SELF,g_method_functor,GC_ARGV,g_allocp); \
  break

#define GC_METHOD_CASE_DEFAULT \
  default: fatal("undefined method")

#define GCDEF_FUNCTOR_METHOD(fa) \
  static q * \
  GC_body(fa) (GC_SELF,GC_TERM,g_allocp) \
    GC_OBJ_TYPE *GC_SELF; \
    q GC_TERM; \
    q *g_allocp;

#define GCSET_NEWOBJ(newgobj) \
  { \
    GD_ALLOC_AREA(newgobj,(GC_OBJ_TYPE *),GC_OBJ_SIZE(GC_SELF)); \
    newgobj->method_table = GC_SELF->method_table; \
  }

#define GC_SUSPEND_GOAL(var,goal) \
{ g_allocp = GC_suspend_goal(var,goal,g_allocp); }
#define GCSET_UNIFY_GOAL(goalp,x,y) \
{ g_allocp = GC_make_unify_goal(goalp,x,y,g_allocp); }

#define GC_GSUCCESS GENERIC_SUCCEEDED
#define GC_GFAILURE    GENERIC_FAILED

#define GC_GRETURN(x) return((q) x)

#define GC_GFAIL return((q) GC_GFAILURE)
#define GC_GSUCCEED return((q) GC_GSUCCESS)

/**********************************************************************/

#define GCDEF_UNIFY() \
  static q * \
  GC_rappend(active_unify) (GC_SELF,GC_TERM,g_allocp) \
    GC_OBJ_TYPE *GC_SELF; \
    q GC_TERM; \
    q *g_allocp;

#define GCDEF_SEND() \
  static q *\
  GC_rappend(active_unify) (GC_SELF,GC_TERM,g_allocp) \
    GC_OBJ_TYPE *GC_SELF; \
    q GC_TERM; \
    q *g_allocp;

#define GC_PRINT(x) klic_fprintf(g_fp, (x))

#define GC_PUTC(x) klic_putc(x,g_fp)

#define GC_RETURN_FROM_PRINT return(1L)

#define GCDEF_PRINT() \
  static \
  long \
  GC_rappend(print) (GC_SELF,g_fp,g_depth,g_length) \
    GC_OBJ_TYPE *GC_SELF; \
    FILE *g_fp; \
    unsigned long g_depth; \
    unsigned long g_length;

#define GC_PRINT_KL1_TERMS(x,b,s) \
{ \
  int i; \
  general_print((((q *)(x))+(b)),g_fp,g_depth,g_length); \
  for(i=(b)+1; i<(b)+(s); i++){ \
    klic_fprintf(g_fp,","); \
    general_print((((q *)(x))+i),g_fp,g_depth,g_length); \
  } \
}

/******************************************************************/

#define GCDEF_GC() \
 static \
  q * \
  GC_rappend(gc) (GC_SELF,g_allocp,g_sp) \
    GC_OBJ_TYPE *GC_SELF; \
    q *g_allocp; \
    q **g_sp;

#define GCSET_NEWOBJ_IN_NEWGEN(newobj) \
  { \
    unsigned long size = GC_OBJ_SIZE(GC_SELF); \
    *g_allocp = (q) GC_SELF->method_table; \
    newobj = (GC_OBJ_TYPE *) g_allocp; \
    g_allocp += size; \
    if(g_allocp > real_heaplimit) fatal("not enough space collected"); \
  }

#define GC_RETURN_FROM_GC(newgobj) \
{ \
  gcsp  = g_sp; \
  heapp = g_allocp; \
  return(((q *)(newgobj))); \
}

#define GCDEF_REGIST() \
  static \
  long \
  GC_rappend(regist) (GC_SELF,g_allocp) \
    GC_OBJ_TYPE *GC_SELF; \
    q *g_allocp;

#define GCDEF_DEALLOCATE() \
  static \
  long \
  GC_rappend(deallocate) (GC_SELF) \
    GC_OBJ_TYPE *GC_SELF;

#define GCDEF_CLOSE() \
  static q * \
  GC_rappend(close) (GC_SELF) \
    GC_OBJ_TYPE *GC_SELF;

#define GCDEF_GENERIC() \
static q *\
 GC_rappend(active_unify) (GC_SELF,GC_TERM,g_allocp) \
    GC_OBJ_TYPE *GC_SELF; \
    q GC_TERM; \
    q *g_allocp;

/*******************************************/

#define GCSET_INTARG_FOR_NEW(var,argv_i) \
{ \
  GC_DEREF_FOR_NEW(argv_i); \
  if(!G_ISINT(argv_i)) GC_FAIL("not integer"); \
  (var) = G_INTVAL(argv_i); \
}

#define GCSET_INTARG(var,argv_i) \
{ \
  GC_DEREF(argv_i); \
  if(!G_ISINT(argv_i)) GC_FAIL("not integer"); \
  (var) = G_INTVAL(argv_i); \
}

#define GCSET_INTARG_WITHIN_RANGE(var,argv_i,llim,ulim) \
{ \
  GC_DEREF(argv_i); \
  if(!G_ISINT(argv_i)) GC_FAIL("not integer"); \
  (var) = G_INTVAL(argv_i); \
  if( (llim>(var)) || ((var)>=ulim) ) GC_FAIL("out of range"); \
}

#define GCSET_MESSAGE(message)					\
{								\
  message = G_CAR_OF(GC_TERM);					\
  while (G_ISREF(message)) {					\
    q temp = G_DEREFONE(message);				\
    if (G_ISREF(temp) &&					\
	(temp == message || G_DEREFONE(temp) == message)) {	\
      reason = message;						\
      GC_SELF->stream = GC_TERM;				\
      goto suspend;						\
    } else {							\
      message = temp;						\
    }								\
  }								\
}

#define GCSET_MESSAGE_INT_ARG(arg, message, pos)		\
{								\
  q temp = G_ARG(message, pos);					\
  while (!G_ISINT(temp)) {					\
    if (G_ISREF(temp)) {					\
      q temp1 = G_DEREFONE(temp);				\
      if (G_ISREF(temp1) && G_DEREFONE(temp1) == temp) {	\
	reason = temp1;						\
	GC_SELF->stream = GC_TERM;				\
	goto suspend;						\
      }								\
      temp = temp1;						\
    } else {							\
      goto message_error;					\
    }								\
  }								\
  arg = G_INTVAL(temp);						\
}

#define GCSET_MESSAGE_STR_ARG(arg, message, pos)		\
{								\
  q temp = G_ARG(message, pos);					\
  while (!G_ISGOBJ(temp) ||					\
	 (struct data_object_method_table *)			\
	 G_FUNCTOR_OF(temp) !=					\
	 &byte__string_g_data_method_table) {			\
    if (G_ISREF(temp)) {					\
      q temp1 = G_DEREFONE(temp);				\
      if (G_ISREF(temp1) && G_DEREFONE(temp1) == temp) {	\
	reason = temp1;						\
	GC_SELF->stream = GC_TERM;				\
	goto suspend;						\
      }								\
      temp = temp1;						\
    } else {							\
      goto message_error;					\
    }								\
  }								\
  arg = (struct byte_string_object *)G_FUNCTORP(temp);		\
}

#define GCSET_NEW_CONS(var)  G_HEAPALLOC(var,2,makecons)

#define GCSET_VAR(self) self = G_MAKEREF(&self)

extern struct predicate predicate_unify__term__dcode_xunify__goal_2;
#define GC_KL1_UNIFY(x, y) \
{ \
  struct goalrec *gp; \
  G_HEAPALLOC(gp, 4, (struct goalrec *)); \
  gp->pred = &predicate_unify__term__dcode_xunify__goal_2; \
  gp->args[0] = x; \
  gp->args[1] = y; \
  resume_same_prio(gp); \
}
