/* ----------------------------------------------------------
%   (C)1993,1994 Institute for New Generation Computer Technology
%       (Read COPYRIGHT for detailed information.)
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
/*
  Object table
*/
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/g_basic.h>
#include <klic/g_methtab.h>

#define generic_deallocate(obj) (obj)->method_table->deallocate(obj)

struct object_table {
  unsigned int type;
  void *obj;
  struct object_table *next;
};

static int gc_hook_initialized = 0;

#define OTBLBLKSIZE 100

static struct object_table *tbl_root = (struct object_table *)0;

static struct object_table *free_oent = (struct object_table *)0;

static Inline struct object_table *
allocOtblBulk() {
  int i;
  extern char *calloc();
  struct object_table *ptr =
    (struct object_table *)calloc(sizeof(struct object_table), OTBLBLKSIZE);
  struct object_table *top = ptr;

  if(!ptr)
    fatal("Memory exhosted allocating object table");
  
  for(i=0; i<OTBLBLKSIZE; i++, ptr++) {
    ptr->next = (ptr+1);
  }

  (ptr-1)->next = (struct object_table *)0;

  return(top);
}

static Inline struct object_table *
allocOtblEnt() {
  struct object_table *ent = free_oent;
  if(!ent)
    ent = allocOtblBulk();

  free_oent = ent->next;
  return(ent);
}

static q *deallocate_garbage(allocp)
     q *allocp;
{
  struct object_table *tbl = tbl_root;
  struct object_table *newtbl = (struct object_table *)0;
  struct object_table **ptr = &newtbl;
  struct object_table *free = free_oent;

  while(tbl) {
    struct object_table *next = tbl->next;
    struct data_object *obj = tbl->obj;
    q newplace = (q)(obj->method_table);

    if(!isstruct(newplace)) { /* the object is garbage */
      int retval;
      switch(tbl->type) {
      case G_CONSUMER:
	retval = generic_deallocate((struct consumer_object *)(obj));
	break;
      case G_GENERATOR:
	retval = generic_deallocate((struct generator_object *)(obj));
	break;
      default: /* data object */
	retval = generic_deallocate(obj);
      }
      if(retval) {
	fatal("Close method has not been supported\n");
      }
      tbl->next = free;
      free = tbl;
    } else {
      tbl->obj = functorp(newplace);
      *ptr = tbl;
      ptr = &(tbl->next);
    }
    tbl = next;
  }
  *ptr = (struct object_table *)0;
  tbl_root = newtbl;
  free_oent = free;
  return(allocp);
}

void register_for_deallocation(obj, type)
     void *obj;
     int type;
{
  struct object_table *newent = allocOtblEnt();
  struct object_table *tbl = tbl_root;
  void register_after_gc_hook();

  if(!gc_hook_initialized) {
    register_after_gc_hook(deallocate_garbage);
    gc_hook_initialized = 1;
  }

  newent->obj = obj;
  newent->type = type;
  newent->next = tbl;
  tbl_root = newent;
}
