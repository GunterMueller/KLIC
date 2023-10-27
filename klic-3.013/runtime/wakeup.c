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

struct GC_wakeup_object {
  struct consumer_object_method_table *method_table;
  q x;
  q y;
};

#define GC_CLASS_NAME() GC_wakeup
#define GC_OBJ_TYPE struct GC_wakeup_object
#define GC_OBJ_SIZE(obj)  sizeof(GC_OBJ_TYPE)

#include <klic/gc_macro.h>

/* basic method definitions */

GCDEF_UNIFY()
{
  G_STD_DECL;

  GC_KL1_UNIFY(GC_SELF->x,GC_SELF->y);
  GC_TERMINATE;
}

GCDEF_GC()
{
  G_STD_DECL;
  GC_OBJ_TYPE *newself;

  GCSET_NEWOBJ_IN_NEWGEN(newself);
  G_COPY_KL1_TERM_TO_NEWGEN(GC_SELF->x,newself->x);
  G_COPY_KL1_TERM_TO_NEWGEN(GC_SELF->y,newself->y);
  GC_RETURN_FROM_GC(newself);
}


GCDEF_PRINT()
{
  G_STD_DECL;
  GC_PRINT("$$WAKEUP$");
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
  GC_OBJ_TYPE *wakeup;
  q var;

  if (GC_ARGC != 2) GC_FAIL("arity mismatch in creation of a merger.");

  GCSET_NEWOBJ_FOR_NEW(wakeup,GC_OBJ_SIZE(wakeup));
  wakeup->x = GC_ARGV[0];
  wakeup->y = GC_ARGV[1];
  var = GC_MAKE_HOOK_VAR(wakeup);
  GC_RETURN_FROM_NEW(var);
}
