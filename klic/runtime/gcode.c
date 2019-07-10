/* ---------------------------------------------------------- 
%   (C)1993,1994 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

/*
  predicate

  method table
  ref to module
  predicate desc (atom)
*/

#include <stdio.h>
#include <klic/gdobject.h>
#include <klic/predinfo.h>
#include "atom.h"
#include "funct.h"

#ifdef STRINGH
#include <string.h>
#else
#include <strings.h>
#endif

#define GD_CLASS_NAME() predicate
#define GD_OBJ_TYPE struct predicate_object
#define GD_OBJ_SIZE(obj) G_SIZE_IN_Q(GD_OBJ_TYPE)

#include <klic/gd_macro.h>

#include <klic/gmodule.h>

GD_USE_CLASS(vector);

GDDEF_GUNIFY()
{
  G_STD_DECL;

  if(GD_SELF->method_table != GD_OTHER->method_table ||
     GD_SELF->pdesc != GD_OTHER->pdesc)
    GD_GFAIL;
  else 
    GD_GSUCCEED;
}

GDDEF_UNIFY()
{
  G_STD_DECL;

  if(GD_SELF->method_table != GD_OTHER->method_table ||
     GD_SELF->pdesc != GD_OTHER->pdesc)
    GD_UNIFY_FAIL;
  else
    GD_RETURN;
}

GDDEF_GC()
{
  G_STD_DECL;
  GD_OBJ_TYPE *newself;

  GDSET_NEWOBJ_IN_NEWGEN(newself);
  newself->pdesc = GD_SELF->pdesc;
  GD_COPY_KL1_TERM_TO_NEWGEN(GD_SELF->module_obj,newself->module_obj);
  newself->predicate_name = GD_SELF->predicate_name;
  GD_RETURN_FROM_GC(newself);
}

/* Generic method */

GDDEF_METHOD(module_1)
{
  G_STD_DECL;

  GD_UNIFY(GD_ARGV[0],GD_SELF->module_obj);
  GD_RETURN;
}

GDDEF_METHOD(name_1)
{
  G_STD_DECL;

  GD_UNIFY(GD_ARGV[0],GD_SELF->predicate_name);
  GD_RETURN;
}

GDDEF_METHOD(arity_1)
{
  G_STD_DECL;
  Const struct predicate *pred = GD_SELF->pdesc;

  GD_UNIFY(GD_ARGV[0], makeint(pred->arity));
  GD_RETURN;
}

#define GD_ALLOC_GOAL(goal, pdesc, arity) \
{ \
  G_HEAPALLOC(goal,(size)+2,(struct goalrec *)); \
  (goal)->pred = (pdesc); \
}

#define GD_GOAL_ARG(i, goal) ((goal)->args[i])

#define G_ARITY_OF(fn) arityof(fn)

GDDEF_METHOD(apply_1)
{
  G_STD_DECL;
  
  q func;
  int size;

  GD_DEREF(GD_ARGV[0]);
  func = GD_ARGV[0];
  if (!G_ISFUNCTOR(func) || !(GD_IS_CLASS(vector, func))) {
    GD_ERROR_IN_METHOD("Illegal argument specification", "apply");
  }

  {
    struct goalrec *goal;
    q argv[2];
    int i;

    GD_CALL_GMETHOD(func,vector_1,argv);
    size = G_INTVAL(argv[0]);
    if (GD_SELF->pdesc->arity != size) {
      GD_ERROR_IN_METHOD("Arity mismatch", "apply");
    }
    GD_ALLOC_GOAL(goal, GD_SELF->pdesc, size);
    for(i=0; i<size; ++i){
      argv[0] = G_MAKEINT(i);
      GD_CALL_GMETHOD(func,element_2,argv);
      GD_GOAL_ARG(i, goal) = argv[1];
    }
    G_PUSH_GOAL(goal);
  }
  GD_RETURN;
}

#define GD_METHOD_ARITY arities[g_method_functor - FUNCTORNUMBERBASE]

/* GDDEF_METHOD_NO_ARITY(call) */
GDDEF_METHOD(call)
{
  G_STD_DECL;
  
  int size = GD_METHOD_ARITY;

  {
    struct goalrec *goal;
    int i;

    GD_ALLOC_GOAL(goal, GD_SELF->pdesc, size);
    for(i=0; i<size; ++i){
      GD_GOAL_ARG(i, goal) = GD_ARGV[i];
    }
    G_PUSH_GOAL(goal);
  }
  GD_RETURN;
}

GDDEF_GENERIC()
{
  G_STD_DECL;

  GD_SWITCH_ON_GMETHOD {
    GD_METHOD_CASE(module_1);
    GD_METHOD_CASE(name_1);
    GD_METHOD_CASE(arity_1);
    GD_METHOD_CASE(apply_1);
  default:
    GD_SWITCH_ON_GMETHOD_NAME {
      GD_METHOD_NAME_CASE(call);
      GD_METHOD_NAME_CASE_DEFAULT;
    }
  }
  GD_RETURN;
}

GDDEF_PRINT()
{
  G_STD_DECL;
  GD_PRINT("predicate#(");
  GD_PRINT_KL1_TERMS(((struct module_object *)
		      (G_FUNCTORP(GD_SELF->module_obj)))->name,
		     0, 1);
  GD_PRINT(":");
  GD_PRINT_KL1_TERMS(GD_SELF->predicate_name, 0, 1);
  GD_PRINTF("/%d)", GD_SELF->pdesc->arity);
  GD_RETURN_FROM_PRINT;
}

GDDEF_GMETHOD(arity_1)
{
  G_STD_DECL;
  Const struct predicate *pred = GD_SELF->pdesc;

  GD_ARGV[0] =  makeint(pred->arity);
  GD_GSUCCEED;
}

GDDEF_GMETHOD(predicate_0)
{
  G_STD_DECL;
  if (GD_SELF->pdesc) {  GD_GSUCCEED; }
  else { GD_GFAIL; }
}

GDDEF_GGENERIC()
{
  GD_SWITCH_ON_GMETHOD {
    GD_GMETHOD_CASE(predicate_0);
    GD_GMETHOD_CASE(arity_1);
    GD_GMETHOD_CASE_DEFAULT;
  }
}

#define GDUSE_MY_GUNIFY
#define GDUSE_MY_UNIFY
#define GDUSE_MY_PRINT
#define GDUSE_MY_GC
#define GDUSE_MY_GENERIC
#define GDUSE_MY_GGENERIC

#include <klic/gd_methtab.h>

Const struct predicate *locate_predicate_in_module(mname, pname, arity)
     q mname, pname;
     long arity;
{
  unsigned char *modname = namestringof(mname);
  unsigned char *predname = namestringof(pname);
  extern Const struct modinfo defined_modules[];
  Const struct modinfo *mp;
  for (mp = defined_modules; mp->name != 0; mp++) {
    if (strcmp((char *)mp->name, (char *)modname) == 0) {
      Const struct predinfo * Const *pp;
      for (pp = mp->preds; *pp != 0; pp++) {
	if (strcmp((char *)((*pp)->name), (char *)predname) == 0 &&
	    (*pp)->predstruct->arity == arity) {
	  return (*pp)->predstruct;
	}
      }
      return 0;
    }
  }
  return 0;
}

/*
  new predicate
*/

GDDEF_NEW()
{
  GD_STD_DECL_FOR_NEW;
  struct module_object *module_obj;
  Const struct predicate *predaddr;
  q predname;
  int arity;

  GD_USE_CLASS(module);

  if (GD_ARGC!=3) GD_ERROR_IN_NEW("Too few or too many arguments");
  GD_DEREF_FOR_NEW(GD_ARGV[0]);
  if(!G_ISFUNCTOR(GD_ARGV[0]) ||
     (((struct module_object *)(G_FUNCTORP(GD_ARGV[0])))->method_table
      != &(GD_method_table1(module)))) {
    GD_ERROR_IN_NEW("First parameter is not a module");
  }
  module_obj = (struct module_object *)(G_FUNCTORP(GD_ARGV[0]));

  GDSET_SYMARG_FOR_NEW(predname, GD_ARGV[1]);
  GDSET_INTARG_FOR_NEW(arity, GD_ARGV[2]);

  predaddr = locate_predicate_in_module(module_obj->name, predname, arity);
  if (predaddr == 0) {
    char message[1024];
    sprintf(message,
	    "Nonexistent predicate %s:%s/%d",
	    namestringof(module_obj->name),
	    namestringof(predname),
	    arity);
    GD_ERROR_IN_NEW(message);
  }

  {
    GD_OBJ_TYPE *newpred;
    
    GDSET_NEWOBJ_FOR_NEW(newpred, sizeof(struct predicate_object));
    newpred->pdesc = predaddr;
    newpred->module_obj = G_MAKEFUNCTOR(module_obj);
    newpred->predicate_name = predname;
    GD_RETURN_FROM_NEW(newpred);
  }
}
