/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <stdio.h>
#include <klic/gdobject.h>
#include <klic/interpe.h>
#include <klic/distio.h>

#include "atom.h"
#include "funct.h"

struct read_hook_object{
  struct consumer_object_method_table *method_table;
  long node;
  long index;
  long wec;
  q to_read_hook;
  long return_index;
};

#define GC_CLASS_NAME() read_hook
#define GC_OBJ_TYPE struct read_hook_object
#define GC_OBJ_SIZE(obj)  G_SIZE_IN_Q(GC_OBJ_TYPE)

#include <klic/gc_macro.h>
#include <klic/gd_macro.h>

GCDEF_UNIFY()
{
  G_STD_DECL;

  GC_SWITCH_ON_TERM(list0,atomic0,functor0,generic_data0,susp0);

 list0:
 atomic0:
 functor0:
 generic_data0:
  if(receive_answer_flag){
    if((GC_SELF->return_index) == answer_return_exp_index){
      INT_CL_DEBUG_X(klic_fprintf(stdout,
				  "rdhok receive answer value -> send_release \n"));
      send_release(GC_SELF->node, GC_SELF->index, GC_SELF->wec);
    } else {
      INT_CL_DEBUG_X(klic_fprintf(stdout,
				  "rdhok receive answer value -> send_unify \n"));
      send_unify(GC_SELF->node, GC_SELF->index, GC_SELF->wec, GC_TERM);
    }      
  } else {
    INT_CL_DEBUG_X(klic_fprintf(stdout,
				"rdhok active unify -> send_unify \n"));

    send_unify(GC_SELF->node, GC_SELF->index, GC_SELF->wec, GC_TERM);
/*    (int)GC_SELF->method_table = 0;*/

  }
  GC_SELF->node = 0;
  GC_SELF->index = 0;
  GC_SELF->wec = 0;

  GC_TERMINATE;
 susp0:
  fatal("illegal data for unify rdhok");
}

GCDEF_GC()
{
  G_STD_DECL;
  GC_OBJ_TYPE *newself;

  INT_CL_DEBUG_X(klic_fprintf(stdout,"readhook GC invoked\n"));

  GCSET_NEWOBJ_IN_NEWGEN(newself);

  newself->node = GC_SELF->node;
  newself->index = GC_SELF->index;
  newself->wec = GC_SELF->wec;
  newself->return_index = GC_SELF->return_index;

  G_COPY_KL1_TERM_TO_NEWGEN(GC_SELF->to_read_hook, newself->to_read_hook);

  GC_RETURN_FROM_GC(newself);
}

GCDEF_PRINT()
{}

#define GCUSE_MY_UNIFY
#define GCUSE_MY_GC
#define GCUSE_MY_PRINT

#include <klic/gc_methtab.h>

GCDEF_NEW() /* node, index, wec */
{
  GC_STD_DECL_FOR_NEW;
  GC_OBJ_TYPE *new_read_hook;

  GCSET_NEWOBJ_FOR_NEW(new_read_hook, GC_OBJ_SIZE(new_read_hook));
  new_read_hook->node   = (long)intval(GC_ARGV[0]);
  new_read_hook->index  = (long)intval(GC_ARGV[1]);
  new_read_hook->wec    = (long)intval(GC_ARGV[2]);

  new_read_hook->to_read_hook = GC_MAKE_HOOK_VAR(new_read_hook);

  new_read_hook->return_index =  send_read(new_read_hook->node, 
					   new_read_hook->index, 
					   new_read_hook->to_read_hook,
					   TRANSFER_CNT);
  new_read_hook->wec--;     /* WEC of EXREF in %read is 1 */
  GC_RETURN_FROM_NEW(new_read_hook->to_read_hook);
}
 
