/* ---------------------------------------------------------- 
%   Copyright (C) 1995 Takashi Chikayama
%       (Read COPYRIGHT for detailed information.) 
----------------------------------------------------------- */

/*
  Goal Object
*/

#include <klic/gdobject.h>
#include <klic/goalobj.h>
#include "atom.h"
#include "funct.h"

#ifdef STRINGH
#include <string.h>
#else
#include <strings.h>
#endif

#define GD_CLASS_NAME() goal
#define GD_OBJ_TYPE struct goal_object
#define GD_OBJ_SIZE(obj) G_SIZE_IN_Q(GD_OBJ_TYPE)

#include <klic/gd_macro.h>
#include <klic/gmodule.h>
#include <klic/predinfo.h>
#include <klic/g_vector.h>

extern struct data_object_method_table
  predicate_g_data_method_table,
  vector_g_data_method_table;

extern Const struct predicate step_trigger_pred;

/* Predicate Table */

struct pred_tab_ent {
  Const struct predicate *pred;
  q modname;			/* module name atom */
  q name;			/* predicate name atom */
} *pred_tab;

static unsigned long hash_mask;

#define HashFunc(pred)	((((unsigned long) (pred)) >> 2) & hash_mask)

static pred_tab_made = 0;
static make_pred_tab()
{
  declare_globals;
  int k, n_pred;
  Const struct modinfo *dm;
  extern Const struct modinfo defined_modules[];

  /* Let's first count the number predicates in this program */
  n_pred = 0;
  for (dm = &defined_modules[0]; dm->name != 0; dm++) {
    Const struct predinfo * Const *dp;
    for (dp = &dm->preds[0]; *dp != 0; dp++) {
      n_pred++;
    }
  }

  /* Allocate predicate table and its hash table */
  {
    unsigned long hash_size = 1;
    while (hash_size <= (3*n_pred/2)) hash_size *= 2;
    pred_tab =
      (struct pred_tab_ent *)
	calloc(hash_size, sizeof(struct pred_tab_ent));
    hash_mask = hash_size - 1;
    for (k = 0; k < hash_size; k++) {
      pred_tab[k].pred = 0;
    }
  }

  /* fill predicate table */
  for (dm = &defined_modules[0]; dm->name != 0; dm++) {
    extern unsigned long enter_atom_body();
    q modname = makesym(enter_atom_body(dm->name, strlen(dm->name)));
    Const struct predinfo * Const *dp;
    for (dp = &dm->preds[0]; *dp != 0; dp++) {
      Const struct predicate *pred = (*dp)->predstruct;
      long h = HashFunc(pred);
      while (pred_tab[h].pred != pred) {
	if (pred_tab[h].pred == 0) {
	  pred_tab[h].pred = pred;
	  pred_tab[h].modname = modname;
	  pred_tab[h].name =
	    makesym(enter_atom_body((*dp)->name, strlen((*dp)->name)));
	  break;
	}
	h = (h + 1) & hash_mask;
      }
    }
  }
}

static struct pred_tab_ent *find_pred_ent(pred)
     struct predicate *pred;
{
  long h;
  if (!pred_tab_made) {
    make_pred_tab();
    pred_tab_made = 1;
  }
  for (h = HashFunc(pred);
       pred_tab[h].pred != pred;
       h = (h + 1) & hash_mask) {
    if (pred_tab[h].pred == 0) {
      /* somehow we couldn't find the predicate */
      fatal("Predicate not found in the table");
    }
  }
  return &pred_tab[h];
}

/* Copy for Garbage Colleciton */

GDDEF_GC()
{
  G_STD_DECL;
  GD_OBJ_TYPE *newself;
  struct goalrec *goal = GD_SELF->goal;
  struct goalrec *newgoal = (struct goalrec *)g_allocp;
  long arity, k;

  arity = goal->pred->arity;
  g_allocp += arity + 2;
  newgoal->next = 0;
  newgoal->pred = goal->pred;
  for (k = 0; k < arity; k++) {
    GD_COPY_KL1_TERM_TO_NEWGEN(goal->args[k], newgoal->args[k]);
  }
  GDSET_NEWOBJ_IN_NEWGEN(newself);
  newself->goal = newgoal;
  newself->id = GD_SELF->id;
  GD_RETURN_FROM_GC(newself);
}

/* Body Methods */

GDDEF_METHOD(reduce_0)
{
  G_STD_DECL;
  resume_same_prio(GD_SELF->goal);
  GD_RETURN;
}

GDDEF_METHOD(step_4)
{
  G_STD_DECL;
  struct goalrec *newgoal;
  GD_ALLOC_AREA(newgoal, (struct goalrec *), 2+5);
  newgoal->pred = &step_trigger_pred;
  newgoal->args[0] = makefunctor(GD_SELF);
  newgoal->args[1] = GD_ARGV[0]; /* children info */
  newgoal->args[2] = GD_ARGV[1]; /* woken-up info */
  newgoal->args[3] = GD_ARGV[2]; /* failure info */
  newgoal->args[4] = GD_ARGV[3]; /* suspension reason info */
  resume_same_prio(newgoal);
  GD_RETURN;
}

GDDEF_METHOD(arg_2)
{
  G_STD_DECL;
  long size, position;
  struct goalrec *goal;
  q the_arg;

  goal = GD_SELF->goal;
  size = goal->pred->arity;
  GDSET_INTARG_WITHIN_RANGE(position, GD_ARGV[0], 1, size+1);
  the_arg = goal->args[position-1];

  GD_UNIFY(GD_ARGV[1], the_arg);
  GD_RETURN;
}

GDDEF_METHOD(setarg_4)
{
  G_STD_DECL;
  long size, position;
  GD_OBJ_TYPE *newobj;
  struct goalrec *oldgoal, *newgoal;
  q oldarg;
  long k;

  size = GD_SELF->goal->pred->arity;
  GDSET_INTARG_WITHIN_RANGE(position, GD_ARGV[0], 1, size+1);
  oldarg = GD_SELF->goal->args[position-1];

  /* Allocate and set up new goal record */
  GD_ALLOC_AREA(newgoal, (struct goalrec *), size+2);
  oldgoal = GD_SELF->goal;
  newgoal->pred = oldgoal->pred;
  for (k = 0; k < size; k++) {
    newgoal->args[k] = oldgoal->args[k];
  }
  newgoal->args[position-1] = GD_ARGV[2];

  /* Allocate and set up new goal object */
  GDSET_NEWOBJ(newobj);
  newobj->goal = newgoal;

  GD_UNIFY(GD_ARGV[1], oldarg);
  GD_UNIFY_VALUE(GD_ARGV[3], GD_OBJ(newobj));
  GD_RETURN;
}

GDDEF_METHOD(id_1)
{
  G_STD_DECL;
  unsigned long id;

  id = GD_SELF->id;
  GD_UNIFY_VALUE(GD_ARGV[0], makeint(id));
  GD_RETURN;
}

GDDEF_METHOD(predicate_4)
{
  G_STD_DECL;
  struct pred_tab_ent *pte;

  pte = find_pred_ent(GD_SELF->goal->pred);
  GD_UNIFY_VALUE(GD_ARGV[0], pte->modname);
  GD_UNIFY_VALUE(GD_ARGV[1], pte->name);
  GD_UNIFY_VALUE(GD_ARGV[2], makeint(pte->pred->arity));
  GD_UNIFY_VALUE(GD_ARGV[3], makeint((unsigned long)(pte->pred)>>2));
  GD_RETURN;
}

GDDEF_METHOD(set__predicate_2)
{
  G_STD_DECL;
  long arity;
  GD_OBJ_TYPE *newobj;
  struct goalrec *oldgoal, *newgoal;
  q predq;
  Const struct predicate_object *pred;
  long k;

  oldgoal = GD_SELF->goal;
  arity = oldgoal->pred->arity;

  predq = GD_ARGV[0];
  GD_DEREF(predq);
  if (!G_ISGOBJ(predq) || !GD_IS_CLASS(predicate, predq)) {
    GD_ERROR_IN_METHOD("First argument is not a predicate object",
		       "set_predicate");
  }
  pred = (Const struct predicate_object *)functorp(predq);
  if (pred->pdesc->arity != arity) {
    GD_ERROR_IN_METHOD("Arity of given predicate object doesn't agree with the goal",
		       "set_predicate");
  }

  /* Allocate and set up new goal record */
  GD_ALLOC_AREA(newgoal, (struct goalrec *), arity+2);
  newgoal->pred = pred->pdesc;
  for (k = 0; k < arity; k++) {
    newgoal->args[k] = oldgoal->args[k];
  }

  /* Allocate and set up new goal object */
  GDSET_NEWOBJ(newobj);
  newobj->goal = newgoal;

  GD_UNIFY_VALUE(GD_ARGV[1], GD_OBJ(newobj));
  GD_RETURN;
}

/*  Generic Method Table */
GDDEF_GENERIC()
{
  G_STD_DECL;

  GD_SWITCH_ON_METHOD{
    GD_METHOD_CASE(reduce_0);
    GD_METHOD_CASE(step_4);
    GD_METHOD_CASE(arg_2);
    GD_METHOD_CASE(setarg_4);
    GD_METHOD_CASE(predicate_4);
    GD_METHOD_CASE(id_1);
    GD_METHOD_CASE(set__predicate_2);
    GD_METHOD_CASE_DEFAULT;
  }
  GD_RETURN;
}

/* Guard Methods */

GDDEF_GGENERIC()
{
  G_STD_DECL;
  GD_SWITCH_ON_GMETHOD {
    GD_GMETHOD_CASE_DEFAULT;
  }
}

/* Printer is not specialized for now */

/* Comparison is not defined */

/* Hashing is not defined */

#define GDUSE_MY_GC
#define GDUSE_MY_GENERIC
#define GDUSE_MY_GGENERIC

/* Define the method table structure */

#include <klic/gd_methtab.h>

/* Object Creation */

static long goal_obj_id = 0;

q make_goal_object(goal, allocp)
     struct goalrec *goal;
     q *allocp;
{
  declare_globals;
  struct goal_object *obj = (struct goal_object *) allocp;
  extern int untrace_goal();
  (void) untrace_goal(goal);
  obj->method_table = &goal_g_data_method_table;
  obj->goal = goal;
  obj->id = goal_obj_id++;
  allocp += (sizeof(struct goal_object) + sizeof(long) - 1) / sizeof(long);
  heapp = allocp;
  return makefunctor(obj);
}

GDDEF_NEW()
{
  GD_STD_DECL_FOR_NEW;
  GD_OBJ_TYPE *newobj;
  struct goalrec *newgoal;
  q predq, argvq;
  struct predicate_object *pred;
  long pred_arity, argv_arity;
  long k;
  extern q size_of_vector();

  if (GD_ARGC != 2) {
    GD_ERROR_IN_NEW("Wrong number of arguments");
  }
  predq = GD_ARGV[0];
  GD_DEREF_FOR_NEW(predq);
  if (!G_ISGOBJ(predq) || !GD_IS_CLASS(predicate, predq)) {
    GD_ERROR_IN_NEW("First argument is not a predicate object");
  }
  pred = (struct predicate_object *)functorp(predq);
  pred_arity = pred->pdesc->arity;

  argvq = GD_ARGV[1];
  GD_DEREF_FOR_NEW(argvq);
  if (!G_ISGOBJ(argvq) || !GD_IS_CLASS(vector, argvq)) {
    GD_ERROR_IN_NEW("Second argument is not a vector object");
  }
  argv_arity = intval(size_of_vector(argvq));

  if (pred_arity != argv_arity) {
    GD_ERROR_IN_NEW("Number of args mismatch with the predicate");
  }

  GD_ALLOC_AREA_FOR_NEW(newgoal, (struct goalrec *), pred_arity+2);
  newgoal->pred = pred->pdesc;
  for (k = 0; k < pred_arity; k++) {
    extern q element_of_vector();
    newgoal->args[k] = element_of_vector(argvq, makeint(k));
  }
  GDSET_NEWOBJ_FOR_NEW(newobj, G_SIZE_IN_Q(GD_OBJ_TYPE));
  newobj->goal = newgoal;
  newobj->id = goal_obj_id++;
  GD_RETURN_FROM_NEW(newobj);
}
