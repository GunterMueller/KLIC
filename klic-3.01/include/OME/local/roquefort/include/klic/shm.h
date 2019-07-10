/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

/* Machine depending Dec */
#include <klic/shm_machine.h>

#define currid (glbl->par.aux.shm.currid0)
#define oldid  (glbl->par.aux.shm.oldid0)

#define SHM_HEAP_TOP (glbl->par.aux.shm.shm_htop0)
#define SHP_SIZE     (glbl->par.aux.shm.shm_hbyte0)

  /* number of Planes */
extern long N_PLNE;
  /* size of 1 Shared-memory plane */
extern long SHM_SIZE;
  /* last PE for Profile */
extern long LAST_PE;
extern long Disp_child;

Extern int shm_trace_lock Init(0);

#define MASTER 0

#define MAX_N_PE  32
#define MAX_N_PLNE 8

#define is_shmconnect(X) (1)

#define is_locala(X) \
    ( (unsigned long)((long)(X)-(long)heaptop) < real_heapbytesize )

  /* Shared-memory address or no */
#define is_shma(X) \
    ( (unsigned long)((long)(X)-(long)SHM_HEAP_TOP ) < SHP_SIZE )

/* calculates a hash key for lock
*/
#define HTBL_base 0
  /* allocating shared-memory */
#define g_key()      HTBL_base
  /* updating the shared-memory management table */
#define m_key()      (HTBL_base+1)
  /* for Sequential GC */
#define gc_key()     (HTBL_base+2)
  /* updating atom table */
#define a_key()     (HTBL_base+3)
  /* updating functor table */
#define f_key()     (HTBL_base+4)
#define tr_key()    (HTBL_base+5)
  /* queuing a goal to external process */
#define p_key(penum) ( HTBL_base + 6 + penum )

  /* global generator hook */
#define is_genhook(X) ((long)(X) & 0x01)
#define tag_genhook(X) ((Sinfo*)((long)(X)+1))
#define untag_genhook(X) ((Sinfo*)((long)(X)-1))
  /* local consumer or generator */
#define tag_local(x)	((q)((char *)(x) + ATOMIC))
#define untag_local(x)	((q)((char *)(x) - ATOMIC))
#define ext_queued (glbl->par.aux.shm.queued0)
#define ex_qp (glbl->par.aux.shm.ex_qp0)

  /* adjusts the memory boundary */
#define reckon(sz) ((long)((((sz)+sizeof(long)-1)/sizeof(long))*sizeof(long)))
#define get_adjust_addr(type,top,inc) ((type*)(((char*)(top)) \
     + (reckon(sizeof(type))*inc)))
#define get_otherPE_glbl(n) \
     get_adjust_addr(struct global_variables,top_shm_glbl,n)

  /* shared memory alloc */
#define galloc(Siz) ( { q* addr = gallocp; \
  ((gallocp+=(Siz)) <= glimit)?addr:shm_galloc(Siz);})


#define free_local_tbl(ptr) \
    { TADDRtbl *sl= (TADDRtbl*)ptr; \
      (sl->prev)->next = sl->next; \
      (sl->next)->prev = sl->prev; \
      sl->next = ADDRtbl_free;    \
      ADDRtbl_free = sl;          \
    }

#define makeshvar(svar) {\
   struct generator_susp* vcell;\
      svar = (q)galloc((sizeof(struct generator_susp)/sizeof(q))+1);\
      vcell = (struct generator_susp*)(((q*)svar)+1);\
      vcell->backpt = (q)svar;\
      vcell->u.o = tag_generator_susp(pSHM_VAR);\
      *((q*)svar) = (q)vcell;\
}


#define get_space(X) ( {\
    int cid = currid;\
    int cnt = 1;\
    int no = 0;\
    do { \
      if ( (unsigned long)((long)(X)-(long)(PLNE_ptr[cid]->top_addr)) < SHM_SIZE ) \
        { no = cnt; break; } \
      cnt++; \
    } while( (cid = PLNE_ptr[cid]->direct) != (-1) ); \
    no; } )

#define cmp_forward_ptr(x,y) ( {\
    int xv;                 \
    int yv;                 \
    int ret;                \
    xv = get_space(x);      \
    yv = get_space(y);      \
    if ( xv == yv ) {       \
      if ( x < y ) ret = 1; \
      else         ret = 0; \
    } else if ( xv < yv ) { \
       ret = 1;             \
    } else {                \
       ret = 0;             \
    }                       \
    ret;} )

#define ck_new_galloc(X) {\
    int cid = currid;\
    while ( (cid = PLNE_ptr[cid]->direct) != (-1) ) { \
       if ( (unsigned long)((long)(X)-(long)(PLNE_ptr[cid]->top_addr) ) < SHM_SIZE ) { new_galloc(cid); break; }\
    }\
  }


#define is_cur_space_ptr(X) ( \
    (unsigned long)((long)(X)-(long)(PLNE_ptr[currid]->top_addr) ) < SHM_SIZE )

#define is_forward_space_ptr(X) ( {\
    int ok = 0;\
    int cid = currid;\
    while ( (cid = PLNE_ptr[cid]->direct) != (-1) ) {\
       if ( (unsigned long)((long)(X)-(long)(PLNE_ptr[cid]->top_addr) ) < SHM_SIZE ) {\
	 ok = 1; break;\
       }\
    }\
    ok; } )

#define is_cur_or_forward_ptr(X) ( {\
    int ok = 0;\
    int cid = currid;\
    do {\
       if ( (unsigned long)((long)(X)-(long)(PLNE_ptr[cid]->top_addr) ) < SHM_SIZE ) {\
	 ok = 1; break;\
       }\
    } while ( (cid = PLNE_ptr[cid]->direct) != (-1) ) ;\
    ok; } )

#define ck_throw_new_galloc(num) {\
    int cid = currid;\
    while( (cid=PLNE_ptr[cid]->direct) != -1 ) { \
       if ( PLNE_ptr[cid]->proc[num] ) {new_galloc(cid);} \
    }\
  }




  /* Structures */
typedef struct addr_tbl {
   struct addr_tbl *next;
   struct addr_tbl *prev;
   q *localA;
   q *globalA;
} TADDRtbl;

struct ex_goalrec {
   struct ex_goalrec *next;
   long prio;
   struct goalrec goal;
} ;

typedef struct PE_shook {
   struct PE_shook    *next;
   long               PE_num;
   long               prio;
   TADDRtbl          *indp;
} Sinfo;

typedef struct shvar {
   struct generator_object_method_table *method;
   Sinfo *chain;
} Shvar;

/* Management table of a shared memory plain */
typedef struct {
   long* top_addr;
   long* caddr;
   long  direct;
   long  demand;
   long  proc[MAX_N_PE];
   long* limit1;
   long* limit2;
   long* limit3;
} PLNE;

/* External declarations */

/* lock table address */
extern int* lockp[];

/* common variables */
extern int F_shm_gc;

extern Volatile PLNE* PLNE_ptr[];

extern int* Volatile pe_status;
extern int* Volatile cur_status;

extern long* Volatile Prof_measure;

extern struct global_variables* top_shm_glbl;

extern int* top_shm_pid;

extern struct ex_goalrec ** Volatile top_shm_qp;

/* memorized the last generated variable on shm */
extern q last_shm_var;

extern struct generator_object* qSHM_BUSY;
extern struct generator_object* pSHM_VAR;
extern struct generator_object_method_table* SHM_BUSY;
extern struct generator_object_method_table* SHM_VAR;

/* current shared memory top address, limit of myself */
extern q* gallocp;
extern q* glimit;

extern TADDRtbl ADDRtbl;
extern TADDRtbl *ADDRtbl_free;

/* Declarations of functions */
/* convert real PE number from Logical PE number */
int phisc_pe();

/* common routines */
struct global_variables* shm_init();

/* copy into shared memory */
struct ex_goalrec* shm_goal_copy();

/* copy argument into shared memory */
q shm_copy();

/* stack a goal */
int shm_goal_stack();

Shvar* create_genhook();

Shvar* shm_add_consumer();

q* shm_ck_request();
q* shm_resume_goals();


struct goalrec *get_invoked_goals();

/* allocate shared memory routines */
q* shm_galloc();

/* GC routines */
int ck_shm_gc();
struct goalrec* shm_gc();

TADDRtbl* create_local_tbl();

/* for GC */

extern q** shm_gcstack;
extern q** shm_gcmax;
extern q** shm_sp;
extern int shm_gcstack_size;

/* for debugging */
#define debug_wait() { while(!*pe_status) {} }

