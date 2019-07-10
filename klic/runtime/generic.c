/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#define DEFINE_GLOBAL
#include <klic/gdobject.h>
#include <klic/functorstuffs.h>
#include <klic/atomstuffs.h>
#include <stdio.h>
#include <klic/gd_macro.h>
#include <klic/susp.h>

extern q *suspend_goal();
extern q gd_new_pointer();
extern q gd_new_termarray();


module module_generic();

Extern Const struct predicate predicate_generic_xgeneric_2;

#define G_NEW_GOAL &predicate_generic_xnew_3
#define GD_GENERIC_GOAL &predicate_generic_xgeneric_2

/*************************** inlines *******************************/

Inline q *G_SUSPEND(x,goal,g_allocp)
     q x;
     struct goalrec * goal;
     q *g_allocp;
{
  G_STD_DECL;
  return suspend_goal(g_allocp, goal, x, 1);
}

Inline q *GD_MAKE_GENERIC_GOAL(goalp1,var,method_functor,argv,g_allocp)
     struct goalrec **goalp1;
     q var;
     unsigned long method_functor;
     q argv[];
     q *g_allocp;
{
  G_STD_DECL;
  unsigned long argc = arities[method_functor - FUNCTORNUMBERBASE];
  q objp;
  int i;

  G_HEAPALLOC(objp,G_SIZE_IN_Q(struct functor)+argc-1,
	      makefunctor);
  functor_of(objp) = makesym(method_functor);
  for (i=0; i<argc; i++) {
    arg(objp,i) = argv[i];
  }

  G_HEAPALLOC(*goalp1,G_SIZE_IN_Q(struct goalrec)-4,(struct goalrec *));
  (*goalp1)->pred    = GD_GENERIC_GOAL;
  (*goalp1)->args[0] = objp;
  (*goalp1)->args[1] = var;
  return g_allocp;
}

Inline q *G_MAKE_THE_NEW_GOAL(var,goalp1,myself,argc,argv,g_allocp)
     q *var;
     struct goalrec **goalp1;
     q (*myself)();
     unsigned long argc;
     q argv[];
     q *g_allocp;
{
  G_STD_DECL;
  q newobj,newobj2;
  q newvar;
  newobj = gd_new_pointer(myself,g_allocp);
  newobj2  = gd_new_termarray(argc,argv,heapp);
  g_allocp = heapp;  

  G_HEAPALLOC(*goalp1,(G_SIZE_IN_Q(struct goalrec)-3),(struct goalrec *));
  G_MAKE_VAR(newvar);
  (*goalp1)->pred    = G_NEW_GOAL;
  (*goalp1)->args[0] = newvar;
  (*goalp1)->args[1] = newobj;
  (*goalp1)->args[2] = newobj2;
  *var = newvar;
  return g_allocp;
}

/**********************************************************************/
#define GD_DEREF_FOR_GENERIC(obj) \
{ \
  while (isref(obj)) { \
    q temp0 = derefone(obj); \
    if(isref(temp0) && (obj) == derefone(temp0)) { \
      struct goalrec *goal; \
      g_allocp = \
      GD_MAKE_GENERIC_GOAL(&goal,(obj),g_method_functor,g_argv,g_allocp); \
      g_allocp = G_SUSPEND((obj),goal,g_allocp); \
      GD_RETURN; \
    } else { \
      (obj) = temp0; \
    } \
  } \
}



/**********************************************************************/
q *
gd_generic(g_allocp,g_object,g_method_functor,g_argv)
    q *g_allocp;
    q g_object;
    long g_method_functor;
    q *g_argv;
{
  G_STD_DECL;

  GD_DEREF_FOR_GENERIC(g_object);
  if (!isfunctor(g_object) || !isref(functor_of(g_object))) {
    klic_printf("### method %F issued to %k ###\n",
		g_method_functor, g_object);
    fatal("Generic method issued to a non-object");
  }
  {
    struct data_object *g_obj = data_objectp(g_object);
    g_allocp = (g_obj->method_table)
                ->body_generic(g_obj,g_method_functor,g_argv,g_allocp);
  }
  GD_RETURN;

}


/**********************************************************************/
q
GC_MAKE_HOOK_VAR(obj)
     struct consumer_object *obj;
{
  declare_globals;
  q newvar;
  struct susprec *susp;

  allocnewsusp(newvar,susp);
  susp->u.first_hook.u.o = tag_consumer_hook(obj);
  return(newvar);
}

/************************* standard methods *************************/

q GD_STD_GUNIFY(GD_SELF, GD_OTHER) 
    struct data_object *GD_SELF; 
    struct data_object *GD_OTHER; 
{
  GD_GSUCCEED;
}

q *GD_STD_UNIFY(GD_SELF, GD_OTHER, g_allocp) 
    struct data_object *GD_SELF; 
    struct data_object *GD_OTHER; 
    q *g_allocp;
{
  return(g_allocp);
}

long GD_STD_PRINT(GD_SELF,g_fp,g_depth,g_length) 
    struct data_object *GD_SELF; 
    FILE *g_fp; 
    unsigned long g_depth; 
    unsigned long g_length;
{
  klic_fprintf(g_fp, "<OBJ@%x>", (long)GD_SELF);
  return(1L);
}

q *GD_STD_GC(GD_SELF,g_allocp, g_sp) 
    struct data_object *GD_SELF; 
    q *g_allocp;
    q **g_sp;
{
  declare_globals;
  gcsp = g_sp;
  return(g_allocp);
}

long GD_STD_REGIST(GD_SELF,g_allocp) 
    struct data_object *GD_SELF; 
    q *g_allocp;
{
  return(1L);
}

long GD_STD_DEALLOCATE(GD_SELF) 
    struct data_object *GD_SELF;
{
  return(1L);
}

q *GD_STD_CLOSE(GD_SELF, g_allocp) 
    q *g_allocp;
    struct data_object *GD_SELF; 
{
  return(g_allocp);
}


q *GD_STD_GENERIC(GD_SELF, g_method_functor, GD_ARGV, g_allocp)
    q *GD_SELF;
    long g_method_functor;
    q GD_ARGV[];
    q * g_allocp;
{
  return(g_allocp);
}

q GD_STD_GGENERIC(GD_SELF, g_method_functor, GD_ARGV)
    struct data_object *GD_SELF;
    long g_method_functor;
    q GD_ARGV[];
{
  return(GENERIC_SUCCEEDED);
}

q GD_STD_COMPARE(GD_SELF, GD_OTHER)
     struct data_object *GD_SELF, *GD_OTHER;
{
  debug_printf("### %k compared with %k ###\n",
	       makefunctor(GD_SELF), makefunctor(GD_OTHER));
  fatal("Comparision of objects of an inappropriate class made");
}

q GD_STD_HASH(GD_SELF, level)
     struct data_object *GD_SELF;
     long level;
{
  return makeint((long)GD_SELF->method_table);
}

#ifdef __GNUC__
Volatile void G_error(errmsg, where, object_kind, class)
#else
void G_error(errmsg, where, object_kind, class)
#endif
     char *errmsg, *where, *object_kind, *class;
{
  fatalf("%s\n\t(in %s of %s object of class \"%s\")",
	 errmsg, (where==0 ? "unknown method" : where),
	 object_kind, (class==0 ? "" : class));
}

q GD_STD_ENCODE(self, buffer)
     struct data_object *self;
 /*     combuf *buffer; */
     void *buffer;		/* To avoid useless file include */
{
  return(GENERIC_FAILED);
}

q GD_STD_SHMCOPY(self)
     struct data_object *self;
{
   fatal("Undefined Data object copied into shared-memory");
}

/*
  next variable for new method has not already used
struct generic *new_term;
*/
q method_result;

/*
  Generator object
*/

q
GG_MAKE_HOOK_VAR(obj)
     struct generator_object *obj;
{
  declare_globals;
  q newvar;
  struct generator_susp *gsusp;

  allocnewgensusp(newvar,gsusp);
  gsusp->u.o = tag_generator_susp(obj);
  return(newvar);
}

q GG_STD_SUSPEND()
{
  return(makecons(0));
}
