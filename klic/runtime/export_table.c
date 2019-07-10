/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/unify.h>
#include <klic/timing.h>
#include <klic/index.h>

#include <stdio.h>

#include <klic/interpe.h>
#include <klic/distio.h>

static void initiate_allocated_exp_table()
{
  int i;

  top_of_exp_freelist = &exp_table[current_exp_size];

  for(i = current_exp_size ; i < (current_exp_size+EXP_TABLE_INC_SIZE); i++){
    exp_table[i].index = i;            /* for getting index from address */
    exp_table[i].wec   = 0;            /* for getting index from address */
    exp_table[i].next = &exp_table[i+1];
    exp_table[i].data = (q)UNUSED_EXPREC;
  }
  exp_table[current_exp_size+EXP_TABLE_INC_SIZE-1].next = 0;

  current_exp_size += EXP_TABLE_INC_SIZE;
}

static struct exp_entry *pop_exp_table_entry()
{
  struct exp_entry *poped_exp_entry;

  if (!top_of_exp_freelist){
    /*** for Debug ***
    for(i=0; i<current_exp_size-1; i++){
      if(exp_table[i].wec == 0){
	ioeprintf("Export table reclaim error in entry %d\n",i);
      }
    }
    *** for Debug ***/

    {
      char *exp_alloc;

      exp_alloc = (char *) realloc((char *)exp_table, 
	  (current_exp_size+EXP_TABLE_INC_SIZE)*sizeof(struct exp_entry));
      if(exp_alloc == NULL){
	fatal("realloc failed in pop_exp_table_entry");
      }
      
      exp_table = (struct exp_entry*)exp_alloc;

      initiate_allocated_exp_table();
    }
  }

  poped_exp_entry = top_of_exp_freelist;
  top_of_exp_freelist = top_of_exp_freelist->next;

  if(poped_exp_entry->data != (q)UNUSED_EXPREC){
    ioeprintf("export_entry doubly used\n");
    ERROR_STOP;
    fatal("export_entry doubly used");
  }
  return(poped_exp_entry);
}

static void push_exp_table_entry(push_exp_entry)
     struct exp_entry *push_exp_entry;
{
  push_exp_entry->data = (q)UNUSED_EXPREC;
  push_exp_entry->next = top_of_exp_freelist;
  top_of_exp_freelist = push_exp_entry;
}

#ifdef I_think_this_is_for_debug /* kumon */
static void check_exp_table_data(data)
     q data;
{
 TOP:
  switch(ptagof(data)){
  case ATOMIC:
    return;
  case CONS:
    check_exp_table_data(car_of(data));
    check_exp_table_data(cdr_of(data));    
    return;
  case FUNCTOR:
    fatal("not appear yet\n");
    return;
  case VARREF:
    deref_and_switch(data, VAR, TOP, TOP, TOP);
  VAR:
    return;
  default:
    fatal("invalid data\n");    
  }
}
void check_exp_table()
{
  int i;
  for(i=0;i<current_exp_size;i++){
    if(exp_table[i].data != (q)UNUSED_EXPREC){
      check_exp_table_data(exp_table[i].data);
    }
  }
}
#endif /* I_think_this_is_for_debug kumon */

long regist_exptbl(data)
     q data;
{
  struct exp_entry *exp_table_entry;

  exp_table_entry = pop_exp_table_entry();
  active_exp_entry++;

  exp_table_entry->wec = (long)EXPORT_UNIT_WEC;

  exp_table_entry->data = data;
  return(exp_table_entry->index);
}

q search_exptbl(index, wec)
     long index;
     long wec;
{
  q data;

  data = exp_table[index].data;
  if((long)data == UNUSED_EXPREC){
    ioeprintf("illegal exp record %d\n", index);
    ERROR_STOP;
  }

  exp_table[index].wec -= wec;
  if (exp_table[index].wec ==0){
    push_exp_table_entry(&exp_table[index]);
    active_exp_entry--;

  }
  return(data);
}


void initiate_exp_table()
{
  char *exp_alloc;

  current_exp_size = 0;

  exp_alloc = (char *) malloc(EXP_TABLE_INC_SIZE*sizeof(struct exp_entry));
  if(exp_alloc == NULL){
    fatal("malloc failed in initiate_exp_table");
  }
  exp_table = (struct exp_entry *)exp_alloc;

  initiate_allocated_exp_table();

}

q general_gc();

q* gc_exp_table(allocp, ntop, otop, nsize, osize)
     q *allocp, *ntop, *otop;
     unsigned long nsize, osize;
{
  declare_globals;

  int i, j;
  q newdata;

  j = 0;
  for(i=0;i<current_exp_size;i++){
    if(exp_table[i].data != (q)UNUSED_EXPREC){
      newdata = general_gc(&(exp_table[i].data), allocp, gcsp);
      exp_table[i].data = newdata;
      allocp = heapp;
      j++;
    }
    if(j == active_exp_entry){
      break;
    }
    
  }
  return(allocp);
}
