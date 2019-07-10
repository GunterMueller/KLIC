/* ---------------------------------------------------------- 
%   (C)1993,1994 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

/*
  module data type

+---------+
| methd   |
+---------+
| symbol  |
+---------+
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

#define GD_CLASS_NAME() module
#define GD_OBJ_TYPE struct module_object
#define GD_OBJ_SIZE(obj) (sizeof(struct module_object))/sizeof(q)

#include <klic/gd_macro.h>
#include <klic/gmodule.h>

/* basic method definitions */

GDDEF_GUNIFY()
{
  G_STD_DECL;

  if (GD_SELF->method_table != GD_OTHER->method_table ||
      GD_SELF->name != GD_OTHER->name)
    GD_GUNIFY_FAIL;
  else
    GD_GSUCCEED;
}


GDDEF_UNIFY()
{
  G_STD_DECL;

  if (GD_SELF->method_table != GD_OTHER->method_table ||
      GD_SELF->name != GD_OTHER->name)
    GD_UNIFY_FAIL;
  GD_RETURN;
}

GDDEF_GC()
{
  G_STD_DECL;
  GD_OBJ_TYPE *newself;

  GDSET_NEWOBJ_IN_NEWGEN(newself);
  newself->name = GD_SELF->name;
  GD_RETURN_FROM_GC(newself);
}

/* Generic method */

GDDEF_METHOD(name_1)
{
  G_STD_DECL;
  GD_UNIFY( GD_ARGV[0],GD_SELF->name);
  GD_RETURN;
}


GDDEF_GENERIC()
{
  G_STD_DECL;

  GD_SWITCH_ON_GMETHOD {
    GD_METHOD_CASE(name_1);
    GD_METHOD_CASE_DEFAULT;
  }
  GD_RETURN;
}

GDDEF_PRINT()
{
  G_STD_DECL;
  GD_PRINT("<MODULE#");
  GD_PRINT_KL1_TERMS(GD_SELF->name, 0, 1);
  GD_PRINT(">");
  GD_RETURN_FROM_PRINT;
}

GDDEF_GMETHOD(module_0)
{
 G_STD_DECL;
 GD_GSUCCEED;
}

GDDEF_GMETHOD(defined_2)
{
 G_STD_DECL;
 extern struct predicate *locate_predicate_in_module();
 q predname = GD_ARGV[0];
 q arity = GD_ARGV[1];

 GD_GDEREF(predname);
 if (!G_ISSYM(predname)) GD_GFAIL;
 GD_GDEREF(arity);
 if (!G_ISINT(arity) || G_INTVAL(arity) < 0) GD_GFAIL;
 if (locate_predicate_in_module(GD_SELF->name, predname,
				G_INTVAL(arity)) != 0) {
   GD_GSUCCEED;
 } else {
   GD_GFAIL;
 }
}

GDDEF_GGENERIC()
{
  G_STD_DECL;

  GD_SWITCH_ON_GMETHOD {
    GD_GMETHOD_CASE(module_0);
    GD_GMETHOD_CASE(defined_2);
    GD_GMETHOD_CASE_DEFAULT;
  }
}

#define GDUSE_MY_GUNIFY
#define GDUSE_MY_UNIFY
#define GDUSE_MY_PRINT
#define GDUSE_MY_GC
#define GDUSE_MY_GENERIC
#define GDUSE_MY_GGENERIC

/* define the method table structure of the vector */
#include <klic/gd_methtab.h>

/*  new_module function */
/*
  The argument is only one and it's the name of the module
*/

GDDEF_NEW()
{
  GD_STD_DECL_FOR_NEW;
  q atom;
  extern Const struct modinfo defined_modules[];
  Const struct modinfo *mp;
  unsigned char *name;

  GDSET_SYMARG_FOR_NEW(atom,GD_ARGV[0]);
  name = namestringof(atom);
  for (mp = defined_modules; mp->name != 0; mp++) {
    if (strcmp((char *)mp->name, (char *)name) == 0) {
      GD_OBJ_TYPE *newmodule;
      GDSET_NEWOBJ_FOR_NEW(newmodule,sizeof(struct module_object));
      newmodule->name = atom;
      GD_RETURN_FROM_NEW(newmodule);
    }
  }
  /* not found: return the name atom itself */
  GD_RETURN_FROM_NEW(atom);
}
