/* ---------------------------------------------------------- 
%   (C)1993,1994 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <stdio.h>
#include <klic/gdobject.h>
#include "atom.h"
#include "funct.h"

struct merge_object {
  struct consumer_object_method_table *method_table;
  unsigned long count;
  q        outstream;
};

#define GC_CLASS_NAME() merge
#define GC_OBJ_TYPE struct merge_object
#define GC_OBJ_SIZE(obj)  G_SIZE_IN_Q(GC_OBJ_TYPE)

#include <klic/gc_macro.h>
#include <klic/gd_macro.h>
extern Const struct goalrec *module_gcmerger();
G_USE_PREDICATE(predicate_gcmerge_xin_4);

#define GC_MERGE_IN_GOAL &predicate_gcmerge_xin_4

#define GC_MAKE_MERGE_IN_GOAL(newvar,vec,index,size) \
{ \
  struct goalrec *goalp1; \
  G_HEAPALLOC(goalp1,G_SIZE_IN_Q(struct goalrec)-2,(struct goalrec *)); \
  goalp1->pred = GC_MERGE_IN_GOAL; \
  goalp1->args[0] = newvar; \
  goalp1->args[1] = vec; \
  goalp1->args[2] = G_MAKEINT(index); \
  goalp1->args[3] = G_MAKEINT(size); \
  G_PUSH_GOAL(goalp1); \
}

GD_USE_CLASS(vector);

/* basic method definitions */

GCDEF_UNIFY()
{
  G_STD_DECL;
  q newvar;

 top:
  GC_SWITCH_ON_TERM(list0,atomic0,functor0,generic_data0,susp0);

  list0:
    {
      struct cons *newout;
      GC_TRY_TO_ALLOC(newout,(struct cons *),2,gc_request);
      newout->car = G_CAR_OF(GC_TERM);
      GCSET_VAR(newout->cdr);
/**/
      GC_UNIFY(GC_SELF->outstream,G_MAKECONS(newout));
/**/
/*
      GC_KL1_UNIFY(GC_SELF->outstream,G_MAKECONS(newout));
*/
      GC_SELF->outstream = newout->cdr;
      GC_TERM = G_CDR_OF(GC_TERM);
      goto top;
    }
  atomic0:
    if (GC_TERM == NILATOM) {
      if (--(GC_SELF->count) == 0) {
	GC_UNIFY(GC_SELF->outstream,NILATOM);
/*
	GC_KL1_UNIFY(GC_SELF->outstream,NILATOM);
*/
      }
      GC_TERMINATE;
    } else goto illegal_data;
  functor0:
    goto illegal_data;
  generic_data0:
    if (GD_IS_CLASS(vector,GC_TERM)) {
      unsigned long size;
      unsigned long i;
      {
	q argv[1];
	GD_CALL_GMETHOD(GC_TERM,vector_1, argv);
	size = G_INTVAL(argv[0]);
      }
      GC_SELF->count += size - 1;
      if (GC_SELF->count == 0) {
	GC_UNIFY(GC_SELF->outstream,NILATOM);
/*
	GC_KL1_UNIFY(GC_SELF->outstream,NILATOM);
*/
      } else {
	q argv[2];
	q hook_var;
	for (i=0; i<size; i++) {
	  argv[0] = G_MAKEINT(i);
	  GD_CALL_GMETHOD(GC_TERM,element_2,argv);
	  GC_TRY_TO_ALLOC(hook_var,(q),1,gc_request2);
	  derefone(hook_var) = GC_MAKE_HOOK_VAR(GC_SELF);
	  GC_KL1_UNIFY(argv[1],hook_var);
	}
      }
      GC_TERMINATE;
    gc_request2:
      GC_SELF->count -= size-i-1;
      G_MAKE_VAR(newvar);
      GC_MAKE_MERGE_IN_GOAL(newvar,GC_TERM,i,size);
      GC_RETURN_WITH_HOOK(newvar);
    } else goto illegal_data;
 susp0: 
  GC_RETURN_WITH_HOOK(GC_TERM);
 illegal_data:
  debug_printf("### %k ###\n", GC_TERM);
  GC_FAIL("Illegal data unified with merger");
 gc_request:
  G_MAKE_VAR(newvar);
  GC_KL1_UNIFY(GC_TERM,newvar);
  GC_RETURN_WITH_HOOK(newvar);
}

GCDEF_GC()
{
  G_STD_DECL;
  GC_OBJ_TYPE *newself;

  GCSET_NEWOBJ_IN_NEWGEN(newself);
  newself->count = GC_SELF->count;
  G_COPY_KL1_TERM_TO_NEWGEN(GC_SELF->outstream,newself->outstream);
  GC_RETURN_FROM_GC(newself);
}


GCDEF_PRINT()
{
  G_STD_DECL;
  GC_PRINT("$$MERGER$");
  GC_RETURN_FROM_PRINT;
}

#define GCUSE_MY_UNIFY
#define GCUSE_MY_PRINT
#define GCUSE_MY_GC

/* define the method table structure of the merger */
#include <klic/gc_methtab.h>

/*  new_vector function */
GCDEF_NEW()
{
  GC_STD_DECL_FOR_NEW;
  GC_OBJ_TYPE *newmerger;
  q var;

  if (GC_ARGC != 1) GC_ERROR_IN_NEW("Arity mismatch");

  GCSET_NEWOBJ_FOR_NEW(newmerger,GC_OBJ_SIZE(newmerger));
  newmerger->count = 1;
  newmerger->outstream = GC_ARGV[0];
  var = GC_MAKE_HOOK_VAR(newmerger);
  GC_RETURN_FROM_NEW(var);
}
