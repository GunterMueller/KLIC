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

extern int exref_g_generator_method_table;
extern int read_hook_g_data_method_table;

struct exref_object{
  struct generator_object_method_table *method_table;
  long pe_num;
  long index;
  long wec;
  q to_exref;
  long gc_flag;
};

static void initiate_allocated_imp_table()
{
  int i;

  for(i = current_imp_size ; i < (current_imp_size+IMP_TABLE_INC_SIZE); i++){
    imp_table[i].next = i+1;
    imp_table[i].object = (q)UNUSED_IMPREC;
    free_imp_rec++;
  }
  imp_table[current_imp_size+IMP_TABLE_INC_SIZE-1].next = END_IMP_FREELIST;

  if(current_imp_size == 0){
    top_of_imp_freelist.next = 1;
    imp_table[0].next = END_IMP_TABLE;
    free_imp_rec--;
  } else {
    top_of_imp_freelist.next = current_imp_size;
  }
  current_imp_size += IMP_TABLE_INC_SIZE;
}

void regist_imp_entry(obj)
     q obj;
{
  long import;

  if(top_of_imp_freelist.next == END_IMP_FREELIST){
    char *imp_alloc;

    imp_alloc = (char *) realloc((char *)imp_table,
       (current_imp_size+IMP_TABLE_INC_SIZE)*sizeof(struct imp_entry));

    if(imp_alloc == NULL){
      fatal("realloc failed in regist_imp_entry");
    }
    imp_table = (struct imp_entry*)imp_alloc;
    initiate_allocated_imp_table();
  }

  import = top_of_imp_freelist.next;
  top_of_imp_freelist.next = imp_table[import].next;

  imp_table[import].object = obj;

  imp_table[import].next = imp_table[0].next;

  imp_table[0].next = import;

  active_imp_rec++;
  free_imp_rec--;
}

static void send_release_after_gc(next_import)
     long next_import;
{
  declare_globals;
  struct exref_object *exref;
  
  exref = (struct exref_object *)(imp_table[next_import].object);

  if(exref->pe_num>total_node){
    ioeprintf("next_import is %d\n",next_import);
    ioeprintf("PE %d invalid pe_num in send_release_after_gc\n",my_node);
    ERROR_STOP;

    fatal("invalid pe_num in send_release_after_gc");
  }

  send_release(exref->pe_num, exref->index, exref->wec);
}


static void check_imp_before_scan()
{
  declare_globals;
  struct exref_object *ex_ref;
  int next_impo = imp_table[0].next;

  while(next_impo != END_IMP_TABLE){
    ex_ref = (struct exref_object *)imp_table[next_impo].object;
    if(intval(ex_ref->pe_num)>total_node){
      ioeprintf("next_impo is %d\n",next_impo);
      fatal("invalid pe_num in checi_imp_before_scan");
    }
    next_impo = imp_table[next_impo].next;
  }
}

static void check_imp_after_scan()
{
  declare_globals;
  struct exref_object *ex_ref;
  int next_impor = imp_table[0].next;

  while(next_impor != END_IMP_TABLE){
    ex_ref = (struct exref_object *)imp_table[next_impor].object;
    if(intval(ex_ref->pe_num)>total_node){
      ioeprintf("next_impor is %d\n",next_impor);
      fatal("invalid pe_num in check_imp_after_scan");
    }

    next_impor = imp_table[next_impor].next;
  }
}

q* scan_imp_table(allocp)
     q* allocp;
{
  declare_globals;
  long next_imp, next_imp_d, prev_imp;

  struct exref_object *ex_obj;

  /*check_imp_before_scan();*/

  prev_imp = 0;
  next_imp = imp_table[prev_imp].next;

  while(next_imp != END_IMP_TABLE){
    ex_obj = (struct exref_object *)imp_table[next_imp].object;
    
    switch(ex_obj->gc_flag){
    case IMPREC_COPIED:
      imp_table[next_imp].object = (q)derefone(ex_obj);

      ex_obj = (struct exref_object *)((long)imp_table[next_imp].object & ~0x3L);
      imp_table[next_imp].object = (q)ex_obj;
      ex_obj->gc_flag = IMPREC_NOT_COPIED;

      prev_imp = next_imp;
      next_imp = imp_table[next_imp].next;
      break;

    case IMPREC_NOT_COPIED:

      imp_table[prev_imp].next = imp_table[next_imp].next;
      next_imp_d = imp_table[next_imp].next;

      send_release_after_gc(next_imp);

      imp_table[next_imp].next = top_of_imp_freelist.next;
      top_of_imp_freelist.next = next_imp;

      next_imp = next_imp_d;

      active_imp_rec--;
      free_imp_rec++;
      break;

    case IMPREC_RELEASED:
      imp_table[prev_imp].next = imp_table[next_imp].next;
      next_imp_d = imp_table[next_imp].next;

      imp_table[next_imp].next = top_of_imp_freelist.next;
      top_of_imp_freelist.next = next_imp;

      next_imp = next_imp_d;

      active_imp_rec--;
      free_imp_rec++;
      break;
    default:
      ioeprintf("invalid gc_flag in imprec\n");
      ERROR_STOP;
      break;
    }
  }

  /*check_imp_after_scan();*/

  return(allocp);
}

void initiate_imp_table()
{
  char *imp_alloc;

  active_imp_rec = 0;
  free_imp_rec = 0;

  current_imp_size = 0;

  imp_alloc = (char *) malloc(IMP_TABLE_INC_SIZE*sizeof(struct imp_entry));
  if(imp_alloc == NULL){
    fatal("malloc failed in initiate_imp_table");    
  }

  imp_table = (struct imp_entry *)imp_alloc;

  initiate_allocated_imp_table();

}
