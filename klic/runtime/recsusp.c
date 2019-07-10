/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/basic.h>
#include <klic/struct.h>
#include <stdio.h>
#define SUSPTABLESIZE 256

extern char *calloc(), *malloc();

struct susp_count_ent {
  Const struct predicate *pred;
  long counter;
  struct susp_count_ent *next;
};

static struct susp_count_ent *suspension_count_table[SUSPTABLESIZE];
static struct susp_count_ent *free_ent = 0;
static int susp_count_ent_num = 0;

static Inline struct susp_count_ent *
allocate_count_ent_bulk() {
  struct susp_count_ent *ent = 
    (struct susp_count_ent *)
      calloc(SUSPTABLESIZE, sizeof(struct susp_count_ent));
  struct susp_count_ent *top = ent;
  int i;

  for (i = 0; i < SUSPTABLESIZE-1; ++i) {
    struct susp_count_ent *next = ent+1;
    ent->next = next;
    ent = next;
  }
  ent->next = 0;
  return top;
}

static Inline struct susp_count_ent *
alloc_ent() {
  struct susp_count_ent *tmp;
  ++susp_count_ent_num;
  if (!free_ent) {
    free_ent = allocate_count_ent_bulk();
  }
  tmp = free_ent;
  free_ent = free_ent->next;
  tmp->counter = 1;
  return tmp;
}

static Inline unsigned long
hash_pred(pred)
     struct predicate *pred;
{
  return ((unsigned long)pred >> 3) % SUSPTABLESIZE;
}

void
record_suspended_pred(goal)
     struct goalrec *goal;
{
  Const struct predicate *pred = goal->pred;
  unsigned int key = hash_pred(pred);
  
  if (suspension_count_table[key]) {
    struct susp_count_ent *ent;
    for (ent = suspension_count_table[key];
	 ent != 0;
	 ent = ent->next) {
      if (ent->pred == pred) {
	ent->counter++;
	return;
      }
    }
  }
  /* the predicate has not been registered yet */
  {	
    struct susp_count_ent *newent = alloc_ent();
    newent->pred = pred;
    newent->next = suspension_count_table[key];
    suspension_count_table[key] = newent;
  }
}

static int
entcomp(a, b)
     struct susp_count_ent **a;
     struct susp_count_ent **b;
{
  return (*b)->counter - (*a)->counter;
}

void scan_suspended_pred() {
  struct susp_count_ent **sort_table;
  struct susp_count_ent **sptr;

  if(susp_count_ent_num) {
    sort_table = (struct susp_count_ent **)
      malloc(sizeof(struct susp_count_ent *) * susp_count_ent_num);
    sptr = sort_table;
    {
      int i;
      struct susp_count_ent *ent;

      for (i=0; i<SUSPTABLESIZE; ++i) {
	ent = suspension_count_table[i];
	while(ent) {
	  *sptr++ = ent;
	  ent = ent->next;
	}
      }

      qsort(sort_table, susp_count_ent_num,
	    sizeof(struct susp_count_ent *), entcomp);

      {
	int i;
	struct susp_count_ent **ent = sort_table;
	for (i=0; i<susp_count_ent_num; ++i, ++ent) {
	  klic_fprintf(stderr, "%8d: ", (*ent)->counter);
	  fprint_pred(stderr, (*ent)->pred);
	  klic_putc('\n', stderr);
	}
      }
    }
  }
}

void
initialize_suspension_count_table() {
  int i;
  for (i=0; i<SUSPTABLESIZE; ++i) {
    suspension_count_table[i] = 0;
  }
}
