/* ---------------------------------------------------------- 
%   (C)1993,1994 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <klic/gdobject.h>
#include <klic/g_pointer.h>

#define GD_CLASS_NAME() pointer
#define GD_OBJ_TYPE struct pointer_object
#define GD_OBJ_SIZE(obj) (G_SIZE_IN_Q(GD_OBJ_TYPE))

#include <klic/gd_macro.h>


GDDEF_GUNIFY()
{
  G_STD_DECL;

  if (GD_SELF->method_table != GD_OTHER->method_table) GD_GUNIFY_FAIL;
  if (GD_SELF->pointer != GD_OTHER->pointer) GD_GUNIFY_FAIL;
  GD_GSUCCEED;
}

GDDEF_UNIFY()
{
  G_STD_DECL;

  if (GD_SELF->method_table != GD_OTHER->method_table) GD_UNIFY_FAIL;
  if (GD_SELF->pointer != GD_OTHER->pointer) GD_UNIFY_FAIL;
  GD_RETURN;
}

GDDEF_GC()
{
  G_STD_DECL;
  GD_OBJ_TYPE *newself;
  GDSET_NEWOBJ_IN_NEWGEN(newself);
  newself->pointer = GD_SELF->pointer;
  GD_RETURN_FROM_GC(newself);
}

#define GDUSE_MY_GUNIFY
#define GDUSE_MY_UNIFY
#define GDUSE_MY_GC

#include <klic/gd_methtab.h>

GDDEF_NEW()
{
  GD_STD_DECL_FOR_NEW;
  GD_OBJ_TYPE *newobj;
  if (GD_ARGC != 1) GD_FAIL("Argument mismatch in pointer:new");
  G_HEAPALLOC(newobj,GD_OBJ_SIZE(newobj),(GD_OBJ_TYPE *));
  newobj->method_table = &GD_method_table;
  newobj->pointer = (char *)GD_ARGV[0];
  GD_RETURN_FROM_NEW(newobj);
}

q gd_new_pointer(arg0,g_allocp)
     q arg0;
     q *g_allocp;
{
  q argv[1];
  argv[0] = arg0;
  return pointer_g_new(1,argv,g_allocp);
}
