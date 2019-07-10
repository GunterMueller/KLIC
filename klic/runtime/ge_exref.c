/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <stdio.h>

#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/unify.h>
#include <klic/index.h>
#include <klic/gb.h>
#include <klic/bb.h>
#include <klic/g_basic.h>
#include <klic/g_extern.h>
#include <klic/g_extinl.h>
#include <klic/susp.h>
#include <klic/gg_macro.h>

#include <klic/interpe.h>
#include <klic/distio.h>

#include "atom.h"
#include "funct.h"

#include <klic/ge_exref.h>
/*struct exref_object{
  struct generator_object_method_table *method_table;
  long node;
  long index;
  long wec;
  q    to_exref;
  long gc_flag;
};*/

#define GG_CLASS_NAME() exref
#define GG_OBJ_TYPE struct exref_object

#include <klic/gc_macro.h>
#include <klic/gd_macro.h>
#include <klic/generic.h>

q read_hook_g_new();

GGDEF_GENERATE()
{
  G_STD_DECL;
  q rdhok;
  q* allocp;

  generic_arg[0] = makeint(GG_SELF->node);
  generic_arg[1] = makeint(GG_SELF->index);
  generic_arg[2] = makeint(GG_SELF->wec);

  allocp = g_allocp;                    /* Using allocp in 'new_generic' */
  new_generic(read_hook_g_new, 3, rdhok, 0); 
  g_allocp = allocp;                    /* Using allocp in 'new_generic' */

  GG_SELF->method_table = 0;

  GG_SELF->node = 0;
  GG_SELF->index = 0;
  GG_SELF->wec = 0;

  GG_SELF->gc_flag = IMPREC_RELEASED;

  heapp = g_allocp;
  return(rdhok);
}

struct generator_object_method_table* get_exref_methtab();

GGDEF_UNIFY()
{
  G_STD_DECL;
  struct generator_susp *gsusp = generator_suspp(derefone(GG_SELF));

  struct exref_object *GG_OBJ = (struct exref_object *)untag_generator_susp(gsusp->u.o);

  GG_SWITCH_ON_TERM(list0,atomic0,functor0,generic_data0,susp0);

 list0:
 atomic0:
 functor0:
 generic_data0:
  send_unify(GG_OBJ->node, GG_OBJ->index, GG_OBJ->wec, GG_TERM);

  derefone(GG_SELF) = GG_TERM;

  GG_OBJ->method_table = 0;
  GG_OBJ->gc_flag = IMPREC_RELEASED;

  GG_TERMINATE;
 susp0:
  {
      struct susprec *gterm = (struct susprec *)(derefone(GG_TERM));

      if(is_generator_susp(gterm->u)){
	  struct generator_susp *gsusp1 = generator_suspp(derefone(GG_TERM));
	  struct exref_object *dummy_obj = (struct exref_object *)untag_generator_susp(gsusp1->u.o);
/*	  fprintf(stderr, "Node:%d unify EXREF-GENERA",my_node);*/

	  if(dummy_obj-> method_table == get_exref_methtab()){
/*	      fprintf(stderr, "Node:%d unify EXREF-EXREF",my_node);*/
	  } else {
	      fatal("unify EXREF-UnknownGEN occur");
	  }

	  goto list0;
      } else {
/*	  fprintf(stderr, "Node:%d unify EXREF-non-EXREF",my_node);*/
      }
  }
  return(NULL);  /* Cannot unify now */
}

Extern void *decode_exref();

GGDEF_ENCODE()
{
  long send_wec;

  if (GG_SELF->wec < MIN_WEC){
    fatal("illegal WEC in exref object");      
  }

  if (GG_SELF->wec < MIN_WEC*2){
    return(GENERIC_FAILED);
  }
  send_wec = (GG_SELF->wec)>>1;

  GG_SELF->wec -= send_wec;

  PUT_BUFFER(buffer, decode_exref);  
  PUT_BUFFER(buffer, GG_SELF->node);
  PUT_BUFFER(buffer, GG_SELF->index);
  PUT_BUFFER(buffer, send_wec);

  return(GENERIC_SUCCEEDED);
}

GGDEF_GC()
{
  G_STD_DECL;
  GG_OBJ_TYPE *newself;

  INT_CL_DEBUG_X(klic_fprintf(stdout,"exref GC invoked\n"));

  if(GG_SELF->gc_flag != IMPREC_NOT_COPIED){
    ioeprintf("illegal exref in GC\n");
    ERROR_STOP;
  }

  GGSET_NEWOBJ_IN_NEWGEN(newself);

  newself->node = GG_SELF->node;
  newself->index = GG_SELF->index;
  newself->wec = GG_SELF->wec;
  G_COPY_KL1_TERM_TO_NEWGEN(GG_SELF->to_exref, newself->to_exref);

  GG_SELF->gc_flag = IMPREC_COPIED;
  newself->gc_flag = IMPREC_NOT_COPIED;

  GG_RETURN_FROM_GC(newself);
}

GGDEF_PRINT()
{}

#define GGUSE_MY_GENERATE
#define GGUSE_MY_UNIFY
#define GGUSE_MY_GC
#define GGUSE_MY_PRINT
#define GGUSE_MY_ENCODE

#include <klic/gg_methtab.h>

GGDEF_NEW() /* node, index, wec */
{
  GG_STD_DECL_FOR_NEW;
  GG_OBJ_TYPE *new_exref;

  GGSET_NEWOBJ_FOR_NEW(new_exref, (struct exref_object *));

  new_exref->node   = (long)intval(GG_ARGV[0]);
  new_exref->index  = (long)intval(GG_ARGV[1]);
  new_exref->wec    = (long)intval(GG_ARGV[2]);

  new_exref->to_exref = GG_MAKE_HOOK_VAR(new_exref);

  new_exref->gc_flag  = IMPREC_NOT_COPIED;

  regist_imp_entry(new_exref);

  GG_RETURN_FROM_NEW(new_exref->to_exref);
}

struct generator_object_method_table*
get_exref_methtab()
{
    return(&GG_method_table);
}
