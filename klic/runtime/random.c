/* ---------------------------------------------------------- 
%   (C)1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
/*
  random number generator
*/
#include <klic/basic.h>

#ifdef NRAND48
#include <klic/struct.h>
#include <klic/g_methtab.h>
#include <klic/g_basic.h>
#include <klic/gg_macro.h>
#include <klic/susp.h>

#include <stdio.h>

#define GG_CLASS_NAME()		random__numbers
#define GG_OBJ_TYPE		struct random_number_generator

static q *unify();
static q generate();
static long print();
static q *gc();
q *resume_goals();
q *do_unify();

GG_OBJ_TYPE {
  struct generator_object_method_table *method_table;
  long range;
  long max;
  unsigned short state[3];
};

/*
  We don't define body unification method here, as body unification
  with a random generator should be exceptional and the value
  generation method defined below would suffice.
*/

GGDEF_GENERATE()
{
  G_STD_DECL;
  q cons;
  q res;
  q var;
  long one_random;
  struct generator_susp *s;

  GG_TRY_TO_ALLOC(cons, makecons, 2, gc_request);
  GG_TRY_TO_ALLOC(var, makeref, 1, gc_request);
  GG_TRY_TO_ALLOC(s, (struct generator_susp *),
		  sizeof(struct generator_susp)/sizeof(q), gc_request);

  do {
    one_random = nrand48(GG_SELF->state);
  } while (one_random >= GG_SELF->max);

  car_of(cons) = makeint(one_random % GG_SELF->range);
  derefone(var) = makeref(s);
  cdr_of(cons) = var;
  s->backpt = makeref(var);
  s->u.o = tag_generator_susp(GG_SELF);
  heapp = g_allocp;
  return cons;
 gc_request:
  heapp = g_allocp;
  return 0;
}

GGDEF_PRINT()
{
  G_STD_DECL;
  fprintf(g_fp, "RANDOM@%X", (q*)GG_SELF-new_space_top);
  return 0;
}

GGDEF_GC()
{
  G_STD_DECL;
  q *newself = g_allocp;
  
  BCOPY(GG_SELF, g_allocp, sizeof(GG_OBJ_TYPE));
  g_allocp += sizeof(GG_OBJ_TYPE)/sizeof(q);
  gcsp = g_sp;
  heapp = g_allocp;
  return newself;
}

GGDEF_UNIFY()
{
  return 0;
}

#define GGUSE_MY_GENERATE
#define GGUSE_MY_GC
#define GGUSE_MY_UNIFY
#define GGUSE_MY_PRINT

#include <klic/gg_methtab.h>

GGDEF_NEW()
{
  GG_STD_DECL_FOR_NEW;
  q res;
  q var;
  GG_OBJ_TYPE *obj;
  long seed;
  long range;

  if (GG_ARGC < 1 || GG_ARGC > 2) {
    fatalf("Wrong number of arguments (%d) in creation of random number generator",
	   GG_ARGC);
  }

  GGSET_INTARG_FOR_NEW(range, GG_ARGV[0]);
  if (GG_ARGC > 1) {
    GGSET_INTARG_FOR_NEW(seed, GG_ARGV[1]);
  } else {
    seed = 0;
  }
  if (range <1) {
    fatalf("Illegal first argument (%d) in creation of random number generator",
	   range);
  }
  GGSET_NEWOBJ_FOR_NEW(obj, (GG_OBJ_TYPE *));

  obj->state[0] = seed >> (sizeof(seed)*4);
  obj->state[1] = seed >> (sizeof(seed)*2);
  obj->state[2] = seed >> (sizeof(seed)*0);
  obj->range = range;
  obj->max = ((((unsigned long)(~0))<<1)>>1)/range*range;
  GG_RETURN_FROM_NEW(GG_MAKE_HOOK_VAR(obj));
}
#endif
