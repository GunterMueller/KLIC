/* ---------------------------------------------------------- 
%   (C)1994 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

/******************************
  Suspension Structures
*******************************/

/*
  Variables _with_ suspended goals are represented as
    a two-word pointer loop,
    the second word being the suspension record structure.
*/

struct hook {
  struct hook *next;		/* next suspension structure */
  union goal_or_consumer {
    long l;			/* long integer for tag manipulation */
    struct goalrec *g;		/* pointer to a goal record */
    struct consumer_object *o;	/* pointer to a consumer object */
  } u;
};

#define CONSUMER_HOOK_TAG 1

#define is_consumer_hook(u) \
  ((u.l&CONSUMER_HOOK_TAG) == 1)

#define tag_consumer_hook(obj) \
  ((struct consumer_object *)((long)(obj)+CONSUMER_HOOK_TAG))

#define untag_consumer_hook(obj) \
  ((struct consumer_object *)((long)(obj)-CONSUMER_HOOK_TAG))

struct susprec {
  q backpt;
  union {
    struct hook first_hook;
    long l;
  } u;
};

#define suspp(x)	((struct susprec *)((unsigned long)(x) - VARREF))

#define allocnewsusp(var,srec)						\
{									\
  q *temp;								\
  heapalloc(temp, (sizeof(struct susprec))/sizeof(q)+1, (q *));		\
  initnewsusp(temp,var,srec);						\
}

#define makenewsusp(var,srec,allocp)					\
{									\
  initnewsusp(allocp,var,srec);						\
  allocp += sizeof(struct susprec)/sizeof(q)+1;				\
}

#define initnewsusp(temp,var,srec)					\
{									\
  var = makeref(temp);							\
  srec = (struct susprec *) (temp+1);					\
  derefone(var) = (q)srec;						\
  srec->backpt = var;							\
  srec->u.first_hook.next = &srec->u.first_hook;			\
}

#define allochook(oldhook,newhook)					\
{									\
  heapalloc(newhook, sizeof(struct hook)/sizeof(q), (struct hook *));	\
  initnewhook(oldhook,newhook);						\
}

#define addhook(oldhook,newhook,allocp)					\
{									\
  newhook = (struct hook *) allocp;					\
  initnewhook(oldhook,newhook);						\
  allocp += sizeof(struct hook)/sizeof(q);				\
}

#define initnewhook(oldhook,newhook)					\
{									\
  (newhook)->next= (oldhook)->next;					\
  (oldhook)->next = (newhook);						\
}

/*
  generator object support
*/

struct generator_susp {
  q backpt;
  union {
    struct generator_object *o;
    long l;
  } u;
};

#define generator_suspp(susp) ((struct generator_susp *)(susp))

#define GENERATOR_OBJECT_TAG 1

#define is_generator_susp(u) \
  ((u.l)&GENERATOR_OBJECT_TAG == 1)

#define tag_generator_susp(obj) \
  ((struct generator_object *)((long)(obj) + GENERATOR_OBJECT_TAG))

#define untag_generator_susp(obj) \
  ((struct generator_object *)((long)(obj) - GENERATOR_OBJECT_TAG))

#define allocnewgensusp(var,gsusp) \
{ \
  q *tmp; \
  heapalloc(tmp, (sizeof(struct generator_susp))/sizeof(q)+1 , (q*)); \
  initnewgensusp(tmp, var, gsusp); \
}

#define initnewgensusp(tmp, var, gsusp) \
{ \
  var = makeref(tmp); \
  gsusp = (struct generator_susp *)(tmp+1); \
  derefone(var) = (q)gsusp; \
  gsusp->backpt = var; \
}

/* Perpetual Suspension Detection */

struct suspended_goal_rec {
  struct goalrec *goal;
  struct suspended_goal_rec *next;
};

