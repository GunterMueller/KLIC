/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
----------------------------------------------------------- */

/*
  struct.h

  Definition of data structures for KL1/C system

  fix for DEC alpha
*/

#include <klic/param.h>

/****************************************
  UNIVERSAL ONE-WORD DATA

  All the one-word data is represented as type "q".
  This pointer type will always be used after type conversion.
  Using "struct gazonk *" type is for reducing confusion.
****************************************/

typedef struct gazonk *q;

/****************************************
  POINTER TAGS

  [Pointer Tags]
  Lower 2 bits of pointers are used as tags in the following way.

     ...00: Variable Reference
     ...01: Pointer to a Cons Cell
     ...10: Atomic (integer, symbolic atom, etc)
     ...11: Pointer to a Functor Structure

  Variables without suspended goals are represented as
    self-referencing variables.
  Variables _with_ suspended goals are represented as
    a two-word pointer loop,
    the second word being the suspension record structure.

  [Tags for Atomic Data]
     ..0010: Integer (integers have 28 bits including sign)
     ..0110: Symbolic Atom
     ..1010: Reserved
     ..1110: Reserved
****************************************/

#define VALUEMASK	(~3L)
#define ADDRESSMASK	(~3L)

#define PTAGBITS	2L
#define PTAGMASK	0x3
#define	VARREF		0x0
#define CONS		0x1
#define ATOMIC		0x2
#define FUNCTOR		0x3

#define STRUCT		0x1
#define FUNCTNOTCONS	0x2
#define ATOMNOTREF	0x2

#define ptagof(x)	((unsigned long)(x) & PTAGMASK)
#define addressof(x)	((unsigned long)(x) & ADDRESSMASK)

#define isatomic(x)	(ptagof(x) == ATOMIC)
#define iscons(x)	(ptagof(x) == CONS)
#define isfunctor(x)	(ptagof(x) == FUNCTOR)
#define isref(x)	(ptagof(x) == VARREF)

#define isstruct(x)	((unsigned long)(x) & STRUCT)
#define functnotcons(x)	((unsigned long)(x) & FUNCTNOTCONS)
#define atomicnotref(x)	((unsigned long)(x) & ATOMNOTREF)

#define ATAGBITS	4
#define ATAGMASK	0xFL
#define INT		(ATOMIC+0x0)
#define SYM		(ATOMIC+0x4)

#define atagof(x)	((unsigned long)(x) & ATAGMASK)

#define isint(x)	(atagof(x) == INT)
#define issym(x)	(atagof(x) == SYM)

/****************************************
  DATA STRUCTURES

  All the data structures are to be allocated at 4 byte boundaries.
****************************************/

struct functor {
  q functor;			/* principal functor as atomic q object */
  q args[1];			/* arguments */
};

#define functor_of(s)		(functorp(s)->functor)
#define arg(s, k)		(functorp(s)->args[k])

struct cons {
  q cdr, car;
};

#define car_of(x)		(consp(x)->car)
#define cdr_of(x)		(consp(x)->cdr)

#define pcar_of(x)              (makeref(consp(x)+1))
#define pcdr_of(x)              (makeref(consp(x)))

/****************************************
  GOAL MANAGEMENT STRUCTURES
****************************************/

/* Predicate structures are constant structures outside of heap */
/* They are pointed to by atomic tags and treated as atomic in GC */

typedef char * (*module)();

struct predicate {
  module (*func)();		/* pointer to function */
  unsigned short int pred;	/* predicate number */
  unsigned short int arity;	/* number of args */
  Const struct arginfo *info;	/* argument mode info */
};

/* Argument mode infomation */

struct arginfo {
  int info;
  /* 0: unknown; 1: input; 2: output; -1: stop mark */
};

/* Goal records */

struct goalrec {
  struct goalrec *next;		/* pointer to next goal */
  Const struct predicate *pred;	/* predicate descriptor of the goal */
  q args[24];			/* arguments; number 24 is meaningless */
};

/* Priority queue is currently implemented as a linear list of */
/* priority queue records, in descending order of precedence */

struct prioqrec {
  struct prioqrec *next;
  long prio;			/* signed, to use -1 as sentinel */
  struct goalrec *q;
};

/****************************************
  DATA MANIPULATION MACROS
****************************************/

/* Type conversion */

#define refp(x)		((q *)((unsigned long)(x) - VARREF))
#define consp(x)	((struct cons *)((unsigned long)(x) - CONS))
#define functorp(x)	((struct functor *)((unsigned long)(x) - FUNCTOR))
#define goalp(x)	((struct goalrec *)((unsigned long)(x) - ATOMIC))
#define predp(x)	((struct predicate *)((unsigned long)(x) - ATOMIC))

/* Variables */

#define makeref(x)	(q) ((unsigned long)(x) + VARREF)
#define derefone(x)	(*refp(x))

/* Atomic Values */

#define intval(x)	((long)(x) >> ATAGBITS)
#define symval(x)	((unsigned long)(x) >> ATAGBITS)

#define makeint(n)	((q)(((long)(n) << ATAGBITS) + INT))
#define makecint(n)	((long)makeint(n))
#define makesym(n)	((q)(((unsigned long)(n) << ATAGBITS) + SYM))

#define eqatom(x, y)	((x) == (y))

/* for non-heap addr */
#define makeatomic(adr) ((q)((unsigned long)(adr) + ATOMIC))

/* for principal functors */
#define makepf(x)	makesym(x)

/* Conses */
#define makecons(x)	((q)((unsigned long)(x) + CONS))

/* Functors */

#define makefunctor(x) 	((q)((unsigned long)(x) + FUNCTOR))

Extern struct funcdesc {
  q name;			/* atom */
  unsigned long arity;		/* number of args */
} functab[1];

/* Structures for linkage */

struct ext_atom {
  char *name;			/* name string */
  q value;			/* its value with symbol tag */
};

struct ext_funct {
  char *name;			/* name string */
  long arity;			/* arity */
  q value;			/* functor id with atomic tag */
};

struct ext_pred {
  char *module;			/* module name string */
  char *name;			/* predicate name */
  long arity;			/* arity */
  q value;			/* pred id with atomic tag */
};

/* Special Atoms */

#define NILATOM		makesym(0)
#define PERIODATOM	makesym(1)

/* Global Variables */

#define current_queue	(glbl->current_queue0)
#define resumed_goals	(glbl->resumed_goals0)
#define prioq		(glbl->prioq0)
#define current_prio	(glbl->current_prio0)
#define top_prio	(glbl->top_prio0)
#define heapp		(glbl->heapp0)
#define heaptop		(glbl->heaptop0)
#define heaplimit	(glbl->heaplimit0)
#define real_heaplimit	(glbl->real_heaplimit0)
#define heapbottom	(glbl->heapbottom0)
#define heapsize	(glbl->heapsize0)
#define maxheapsize	(glbl->maxheapsize0)
#define real_heapbytesize (glbl->real_heapbytesize0)
#define incrementsize	(glbl->incrementsize0)
#define maxactiveratio	(glbl->maxactiveratio0)
#define new_space_top	(glbl->new_space_top0)
#define old_space_top	(glbl->old_space_top0)
#define new_space_size	(glbl->new_space_size0)
#define old_space_size	(glbl->old_space_size0)
#define this_more_space	(glbl->this_more_space0)
#define gcstack		(glbl->gcstack0)
#define gcsp		(glbl->gcsp0)
#define gcmax		(glbl->gcmax0)
#define gcstack_size	(glbl->gcstack_size0)
#define interrupt_off   (glbl->interrupt_off0)
#define interrupt_qp    (glbl->interrupt_qp0)
#define program_name	(glbl->program_name0)
#define command_argc	(glbl->command_argc0)
#define command_argv	(glbl->command_argv0)
#define gc_hook_table	(glbl->gc_hook_table0)
#define gc_hooktab_size (glbl->gc_hooktab_size0)
#define num_gc_hooks	(glbl->num_gc_hooks0)
#define after_gc_hook_table (glbl->after_gc_hook_table0)
#define after_gc_hooktab_size (glbl->after_gc_hooktab_size0)
#define num_after_gc_hooks (glbl->num_after_gc_hooks0)
#define suspensions	(glbl->suspensions0)
#define	resumes		(glbl->resumes0)
#define copied_susp	(glbl->copied_susp0)
#define cum_susps	(glbl->cum_susps0)
#define	cum_resumps	(glbl->cum_resumps0)
#define suspended_goal_list (glbl->suspended_goal_list0)
#define postmortem_pred (glbl->postmortem_pred0)
#define postmortem_args (glbl->postmortem_args0)
#define generic_argc	(glbl->generic_argc0)
#define generic_arg	(glbl->generic_arg0)
#define reasons		(glbl->reasons0)
#if ( defined(DIST) && defined(USE_REDUCTION_COUNT) )
#define reduction_count (glbl->reduction_count0)
#endif
#define my_node		(glbl->par.my_num0)
#define total_node	(glbl->par.num_pes0)

Extern struct global_variables {
  /* First four items are accessed frequently */
  q *heapp0;			/* top of free area (when decached) */
  q * Volatile heaplimit0;	/* copy of real_heaplimit or 0 */
  struct goalrec *current_queue0; /* queue for current priority */
  struct goalrec *resumed_goals0; /* spontaneous goals during reduction */
  unsigned long current_prio0;	/* current priority */
  unsigned long top_prio0;	/* highest priority with ready goal */
  /* The rest are not accessed as often */
  struct prioqrec prioq0;	/* priority queue head */
  q *heaptop0;			/* top of the current heap */
  q *heapbottom0;		/* bottom of free area */
  q *real_heaplimit0;		/* limit of normal heap use */
  unsigned long heapsize0, maxheapsize0, incrementsize0;
  unsigned long real_heapbytesize0;
  double maxactiveratio0;
  unsigned long this_more_space0; /* at least this # of words needed after GC */
  q *new_space_top0, *old_space_top0;
  unsigned long new_space_size0, old_space_size0;
  q **gcstack0;
  q **gcsp0;
  q **gcmax0;
  unsigned long gcstack_size0;
  Volatile long interrupt_off0;
  struct goalrec *interrupt_qp0;

  struct {
   /* parallel comm Imp. */
    long my_num0;
    long num_pes0;
    union {
      /* shared-memory Imp. */
      struct {
	long queued0;
	struct ex_goalrec* ex_qp0;
	long currid0;
	long oldid0;
	long shm_htop0;
	long shm_hbyte0;
	long dummy[10];
      } shm;
      /* dist-memory Imp. */

    } aux;
  } par;

  char *program_name0;
  int command_argc0;
  char **command_argv0;
  q* (**gc_hook_table0)();
  int gc_hooktab_size0;
  int num_gc_hooks0;
  q* (**after_gc_hook_table0)();
  int after_gc_hooktab_size0;
  int num_after_gc_hooks0;
  unsigned long
    suspensions0, resumes0, copied_susp0, cum_susps0, cum_resumps0;
  struct suspended_goal_rec *suspended_goal_list0;
  Const struct predicate *postmortem_pred0;
  q postmortem_args0;
  long generic_argc0;		/* number of arguments to generic methods */
  q generic_arg0[MAXGENERICARGS]; /* arguments of generic methods */
  q reasons0[MAXSUSPENSION];	/* suspension reasons */
#if ( defined(DIST) && defined(USE_REDUCTION_COUNT) )
  int reduction_count0;
#endif
} globals;

Extern struct global_variables* My_glbl  Init(&globals);

#define declare_globals struct global_variables *glbl = My_glbl

/*
  memory block outside of heap.
*/

Extern long higher_priority_goal;

/*
  variable for mutable objects
*/

Extern q suspension_reason;
Extern q rest_of_stream;
