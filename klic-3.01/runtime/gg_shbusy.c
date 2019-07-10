/* ---------------------------------------------------------- 
%   (C)1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
/*
  sample program for generator object
*/
#include <stdio.h>

#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/g_methtab.h>
#include <klic/g_basic.h>
#include <klic/gg_macro.h>
#include <klic/susp.h>
#include <klic/shm.h>

#define GG_CLASS_NAME() shbusy
#define GG_OBJ_TYPE struct Shbusy

struct Shbusy {
  struct generator_object_method_table *method_table;
};

extern struct generator_object* pSHM_BUSY;
extern q* do_unify();

#define One_more do_unify(g_allocp,GG_SELF,GG_TERM)

GGDEF_UNIFY()
{
  G_STD_DECL;

  struct generator_object* gobj;
  q pair = derefone(GG_SELF);
  if ( !isref(pair) || GG_SELF != derefone(pair) ) { return One_more;}
  gobj = n_lock(GG_SELF,pair);
  if ( derefone(GG_SELF) == pair ) {
    n_unlock(pair,gobj);
  }
  return One_more;
}

GGDEF_GENERATE()
{
   return( makecons(0) );
}

GGDEF_SUSPEND()
{
  return( makecons(0) );
}

GGDEF_PRINT()
{
  fprintf(g_fp, "<BUSY Generator %x>", GG_SELF);
  return(0);
}

GGDEF_GC()
{
  fatal("bug!!!");
}

#define GGUSE_MY_UNIFY
#define GGUSE_MY_GENERATE
#define GGUSE_MY_SUSPEND
#define GGUSE_MY_GC
#define GGUSE_MY_PRINT

#include <klic/gg_methtab.h>

GGDEF_NEW()
{
}
