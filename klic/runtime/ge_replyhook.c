/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <stdio.h>
#include <klic/gdobject.h>
#include "atom.h"
#include "funct.h"

struct reply_hook_object{
  struct consumer_object_method_table *method_table;
  long node;
  long index;
  long wec;
};

#define GC_CLASS_NAME() reply_hook
#define GC_OBJ_TYPE struct reply_hook_object
#define GC_OBJ_SIZE(obj)  G_SIZE_IN_Q(GC_OBJ_TYPE)

#include <klic/gc_macro.h>
#include <klic/gd_macro.h>

GCDEF_UNIFY()
{
  G_STD_DECL;

/*  INT_CL_DEBUG_X(printf("replyhook active unify \n"));*/

  GC_SWITCH_ON_TERM(list0,atomic0,functor0,generic_data0,susp0);

 list0:
 atomic0:
 functor0:
 generic_data0:
  send_answer_value(GC_TERM, 
		    GC_SELF->node,
		    GC_SELF->index,
		    GC_SELF->wec);
		    
  GC_TERMINATE;
 susp0:
  fatal("illegal data for unify rhook");
}

GCDEF_GC()
{
  G_STD_DECL;
  GC_OBJ_TYPE *newself;

  GCSET_NEWOBJ_IN_NEWGEN(newself);
  newself->node = GC_SELF->node;
  newself->index = GC_SELF->index;
  newself->wec = GC_SELF->wec;

  GC_RETURN_FROM_GC(newself);
}

GCDEF_PRINT()
{}



#define GCUSE_MY_UNIFY
#define GCUSE_MY_GC
#define GCUSE_MY_PRINT


#include <klic/gc_methtab.h>

GCDEF_NEW() /*  node, index,  wec */
{
  GC_STD_DECL_FOR_NEW;
  GC_OBJ_TYPE *new_reply_hook;
  q var;


  GCSET_NEWOBJ_FOR_NEW(new_reply_hook, GC_OBJ_SIZE(new_reply_hook));
  new_reply_hook->node = intval(GC_ARGV[0]);
  new_reply_hook->index  = intval(GC_ARGV[1]);
  new_reply_hook->wec    = intval(GC_ARGV[2]);

  var = GC_MAKE_HOOK_VAR(new_reply_hook);
/*  GC_RETURN_WITH_HOOK_FROM_NEW(var);*/
  GC_RETURN_FROM_NEW(var);
}

