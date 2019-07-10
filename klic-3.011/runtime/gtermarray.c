/* ---------------------------------------------------------- 
%   (C)1993,1994 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <klic/gdobject.h>
#include <klic/g_termarray.h>

#define GD_CLASS_NAME() termarray
#define GD_OBJ_TYPE struct termarray_object
#define GD_OBJ_SIZE(obj) (G_SIZE_IN_Q(GD_OBJ_TYPE)+obj->nterm-1)

#include <klic/gd_macro.h>

GDDEF_GUNIFY()
{
  G_STD_DECL;

  if (GD_SELF->method_table != GD_OTHER->method_table) GD_GUNIFY_FAIL;
  if (GD_SELF->terms != GD_OTHER->terms) GD_GUNIFY_FAIL;
  if (GD_SELF->nterm != GD_OTHER->nterm) GD_GUNIFY_FAIL;
  GD_GSUCCEED;
}

GDDEF_UNIFY()
{
  G_STD_DECL;

  if (GD_SELF->method_table != GD_OTHER->method_table) GD_UNIFY_FAIL;
  if (GD_SELF->terms != GD_OTHER->terms) GD_UNIFY_FAIL;
  if (GD_SELF->nterm != GD_OTHER->nterm) GD_UNIFY_FAIL;
  GD_RETURN;
}

GDDEF_GC()
{
  G_STD_DECL;
  unsigned long size = GD_SELF->nterm;
  GD_OBJ_TYPE *newself;
  unsigned long i;

  GDSET_NEWOBJ_IN_NEWGEN(newself);
  newself->nterm = size;
  for (i=0; i < size; i++) {
    G_COPY_KL1_TERM_TO_NEWGEN(GD_SELF->terms[i],newself->terms[i]);
  }
  GD_RETURN_FROM_GC(newself);
}

#define GDUSE_MY_GUNIFY
#define GDUSE_MY_UNIFY
#define GDUSE_MY_GC

#include <klic/gd_methtab.h>

GDDEF_NEW()
{
  GD_STD_DECL_FOR_NEW;
  unsigned long size = GD_ARGC;
  GD_OBJ_TYPE *newobj;
  int i;

  G_HEAPALLOC(newobj,G_SIZE_IN_Q(GD_OBJ_TYPE)+size-1,(GD_OBJ_TYPE *));
  newobj->method_table = &GD_method_table;
  newobj->nterm = size;
  for (i=0; i<size; i++) {
    newobj->terms[i] = GD_ARGV[i];
  }
  GD_RETURN_FROM_NEW(newobj);
}

q gd_new_termarray(argc,argv,g_allocp)
     unsigned long argc;
     q argv[];
     q *g_allocp;
{
 return termarray_g_new(argc,argv,g_allocp);
}
