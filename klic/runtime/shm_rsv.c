/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/shm.h>
#include <signal.h>

/*
#define SYNC_GC    /* Synchronous GC */
/**/
#define PAGE   4096
#define HPAGE  2048
#define N_PAGE  64

/* Number of plains (copying GC) */
long N_PLNE = 3;

long LAST_PE = 0;

long Disp_child = 0;

int MON_SIZE;
int ATOM_TABLE_SIZE = 256*PAGE; /* 512KB */
/* Heap size on shared memory */
long SHM_SIZE = PAGE*N_PAGE;
long SHM_SIZEq;

long TOTAL_SIZE;

/* threshold value (favourable)THV1 %, (compel)THV2 % */
#define THV1 90
#define THV2 98

/* table size for locking */
#define LOCK_size  (6+total_node)

int* lockp[6+MAX_N_PE];


Volatile PLNE* PLNE_ptr[MAX_N_PLNE];

  /* Management variables on shared memory */
struct global_variables* shm_glblp = &globals;
char* shm_start_addr;
struct global_variables* top_shm_glbl;
int* pe_status;
int* cur_status;
struct ex_goalrec **top_shm_qp;
struct ex_goalrec **shm_qp;
int* top_shm_pid;
struct ex_goalrec **top_shm_next;

  /* for Profile */
long My_measure;
long* Prof_measure = &My_measure;

  /* current used plane number */

  /* current shared memory top address of myself */
q* gallocp = 0;
  /* current shared memory limit of myself */
q* glimit = 0;

long* get_page();
int* init_lock();
void abend();

/* adjusts the memory boundary */
#define adjust(x) ((long*)((((long)(x)+sizeof(long)-1)/sizeof(long))*sizeof(long)))

/* Initialize shared memory.
  allocaltes shared memory and intializes shared variables.
*/
struct global_variables* shm_init(glbl)
struct global_variables* glbl;
{
  long NUM_PE = total_node;
  long MY_PENO;
  int fds,i,j;
  long* addr;
  char bf[32];
  long GLOBALS, EXT_INFO, LOCK_AREA, PLANE_INF, PROFILE_B;
  long BBUF[PAGE/sizeof(long)];

  if ( NUM_PE < 1 || NUM_PE >= MAX_N_PE )  abend(" Number of processes ???");
  if ( N_PLNE < 2 || N_PLNE >= MAX_N_PLNE )  abend(" Number of spacees ???");
/* Management size on shared memory */
     GLOBALS = reckon(sizeof(struct global_variables))*NUM_PE;
     EXT_INFO = reckon(sizeof(long)*NUM_PE +
                       sizeof(long)*NUM_PE +
                       sizeof(struct goalrec*)*NUM_PE +
                       sizeof(q*)*NUM_PE);
     LOCK_AREA = reckon(sizeof(int)*LOCK_size);
     PLANE_INF = reckon(sizeof(PLNE))*N_PLNE;
     PROFILE_B = sizeof(long)*2;

     MON_SIZE  =     ATOM_TABLE_SIZE + 
                     GLOBALS +
                     EXT_INFO +
                     LOCK_AREA +
                     PLANE_INF +
                     PROFILE_B;

/* Shared Heap size */
    SHM_SIZE *= sizeof(q); /* byte count */
    SHM_SIZE = ((SHM_SIZE+PAGE-1)/PAGE)*PAGE;
    SHM_SIZEq = SHM_SIZE/sizeof(q);
    SHP_SIZE = SHM_SIZE * N_PLNE;

    if ( NUM_PE == 1 ) SHP_SIZE = 0; /* only 1 process */

    LAST_PE = NUM_PE - 1;

    TOTAL_SIZE = MON_SIZE + SHP_SIZE;

  /* management table */
     sprintf(bf,"/tmp/SHM%d",getpid());
     fds = open(bf,(O_RDWR|O_CREAT),0644);
     for(j=0;j<(PAGE/sizeof(long));j++) BBUF[j] = -1;
     lseek(fds,TOTAL_SIZE,SEEK_SET);
     write(fds,BBUF,PAGE);
/*     for(j=0;j<((TOTAL_SIZE+PAGE-1)/PAGE);j++) write(fds,BBUF,PAGE);*/

     if ( (long)(shm_start_addr = (char*)
         mmap(0,TOTAL_SIZE,(PROT_READ|PROT_WRITE),MAP_SHARED,fds,0))  == -1 ) {
        perror(0); exit(1);
     }
     close(fds);
     unlink(bf);

     SHM_HEAP_TOP = MON_SIZE + (long)shm_start_addr;
     SHP_SIZE += 0xff; /* allowance */
     addr = adjust(shm_start_addr+ATOM_TABLE_SIZE);
     top_shm_glbl = (struct global_variables*)addr;
     for(i=0;i<NUM_PE;i++) {
        *((struct global_variables*)addr)++ = *glbl;
        addr = adjust(addr);
     }
     pe_status = (int*)addr;
     addr += NUM_PE;
     top_shm_pid = (int*)addr;
     addr += NUM_PE;
     top_shm_qp = (struct ex_goalrec **)addr;
     addr += NUM_PE;
     top_shm_next = (struct ex_goalrec**)addr;
     addr += NUM_PE;
     addr = adjust(addr);

     for(i=0;i<NUM_PE;i++) {
        *(pe_status+i) = 0;
        *(top_shm_qp+i) = (struct ex_goalrec*)(top_shm_next+i);
        *(top_shm_next+i) = 0;
      }

     addr = adjust(init_lock(addr));

  /* local address table initialize */
     d_shm_init();        /* inits external ADDRTBL */

     for(i=0;i<N_PLNE;i++)  {
       PLNE_ptr[i] = ((PLNE*)addr)++;
       addr = adjust(addr);
     }

     MY_PENO = 0;  /* I am MASTER */
     if ( NUM_PE == 1 ) { /* only one process */
       glbl = shm_glblp = top_shm_glbl;
       my_node = MY_PENO;
       total_node = NUM_PE;
       cur_status = pe_status + MY_PENO;
       shm_qp = top_shm_qp + MY_PENO;
       shm_glblp->par.aux.shm.ex_qp0 = (struct ex_goalrec*)top_shm_next + MY_PENO;
       PLNE_ptr[0]->caddr = 0;
       PLNE_ptr[1]->limit3 = (long*)MAXKLICINT;
       /*
	 This fix is temporary. (from Chik)
	 */
       init_shm_atom();		/* manages shared atom-id & funct-id */
       /* end fix */
       return( glbl );
     }

  /* Each Heap plains */
   SHM_HEAP_TOP = (long)addr;
   for(i=0;i<N_PLNE;i++) {

      PLNE_ptr[i]->top_addr = addr;

      PLNE_ptr[i]->caddr = addr;
      PLNE_ptr[i]->direct = -1;
      PLNE_ptr[i]->demand = 0;
      for(j=0;j<=MAX_N_PE;j++) PLNE_ptr[i]->proc[j] = 0;
      PLNE_ptr[i]->limit1 = addr+(SHM_SIZEq*THV1/100);
      PLNE_ptr[i]->limit2 = addr+(SHM_SIZEq*THV2/100);
      addr += SHM_SIZEq;
      PLNE_ptr[i]->limit3 = addr;
  }
  
  /* for Profile */
  Prof_measure = addr;
  *Prof_measure = 0;

  init_shm_atom();     /* manages shared atom-id & funct-id */

  currid = oldid = 0;

  /* invoke parallel process */
  {
     int child;
     *(top_shm_pid) = getpid();
     for (i=1;i<NUM_PE;i++) {
         *(pe_status+i) = 1;
         if ( child = fork() ) {
            *(top_shm_pid+i) = child;
	    if ( Disp_child ) 
	      printf("The %dth-%d Child process start !\n",i,child);
         } else {
            MY_PENO = i;
            break;
         }
     }

  }
  glbl = shm_glblp = get_otherPE_glbl(MY_PENO);
  my_node = MY_PENO;
  total_node = NUM_PE;
  cur_status = pe_status + MY_PENO;
  shm_qp = top_shm_qp + MY_PENO;
  shm_glblp->par.aux.shm.ex_qp0 = (struct ex_goalrec*)(top_shm_next + MY_PENO);

 /* at first 0 plain */
/*
  gallocp = glimit = 0;
  PLNE_ptr[currid]->proc[MY_PENO] = 1;
*/
  /* this allocation will be disappeared */
  galloc(0);

  return( glbl );
}

/* finalize shared memory
  detach shared memory
*/
shm_fin()
{
}

/* galloc
  allocates shared memory specifyed size
  if no current space, get from shared memory pool
*/
q* shm_galloc(siz)
int siz;
{
  declare_globals;
   q* temp = gallocp;
 GET_L:
   if((gallocp+=siz) <= glimit)  return(temp);
   if ( glimit == 0 ) { /* this means initial state */
      currid = 0;
      PLNE_ptr[currid]->proc[my_node] = 1;
   }
   if( siz > HPAGE ) {
      int npsize = (1+(siz/HPAGE))*HPAGE;
      temp = gallocp = (q*)get_page(npsize);
      glimit = gallocp + npsize;
   } else {
      temp = gallocp = (q*)get_page(HPAGE);
      glimit = gallocp+HPAGE;
   }
   goto  GET_L;
}

/* get_page(siz)
  get shared memory from shared memory pool
*/
long* get_page(siz)
int siz;
{
   declare_globals;
   long *temp,*pos;
   int key = g_key();

 ReTry:
   s_lock(key);
   pos = PLNE_ptr[currid]->caddr;
   temp = pos + siz;
   if (PLNE_ptr[currid]->limit1>=temp) {
     PLNE_ptr[currid]->caddr = temp;
     s_unlock(key);
     return(pos);
   }
#ifdef SYNC_GC
   heaplimit = 0;
#else
   if ( currid != oldid ) {
     if ( *cur_status == 4 ) { /* inside GC */
        abend(" shared memory overflow !!!");
     }
     heaplimit = 0;
   }
#endif
   if (PLNE_ptr[currid]->limit3>=temp) {
     PLNE_ptr[currid]->caddr = temp;
     s_unlock(key);
     return(pos);
   }
   s_unlock(key);

#ifdef SYNC_GC
   abend(" Shared memory overflow !!!");
#else
   /* secondary try */
   {
      int i,j;
      if ( PLNE_ptr[currid]->direct == -1 ) {
        int key = m_key();
        s_lock(key);
        if ( PLNE_ptr[currid]->direct == -1 ) {
          j = (currid+1) % N_PLNE;
          for(i=0;i<total_node;i++) {
            if (PLNE_ptr[j]->proc[i]) {
              struct global_variables* tglbl = get_otherPE_glbl(i);
              tglbl->interrupt_off0 = 0;
              klic_barrier();
              tglbl->heaplimit0 = 0;
              sleep(1);
              if (PLNE_ptr[j]->proc[i]) {
                abend(" Shared memory overflow !!!");
              }
            }
          }
          /* initialize table */
          PLNE_ptr[j]->caddr = PLNE_ptr[j]->top_addr;
          PLNE_ptr[j]->demand = 0;
          PLNE_ptr[j]->direct = -1;
          PLNE_ptr[j]->proc[my_node] = 1;
          PLNE_ptr[currid]->direct = j;
          currid = j;
          s_unlock(key);
          goto ReTry;
        }
        s_unlock(key);
      }
      currid = PLNE_ptr[currid]->direct;
      PLNE_ptr[currid]->proc[my_node] = 1;
      goto ReTry;
    }
#endif
}

/* new_galloc --- change to the new plain from the old plain
*/
new_galloc(cid)
int cid;
{
  declare_globals;
  currid = cid;
  PLNE_ptr[currid]->proc[my_node] = 1;
  gallocp = glimit = (q*)1;
}

/* initialize a lock table
*/
int* init_lock(addr)
int* addr;
{
  declare_globals;
  int i,j;
  for(i=0;i<LOCK_size;i++)  {
      *addr = 0;
      lockp[i] = (addr)++;
  }
  return((int*)addr);
}


/* abend with a message
  make sence at debug time
*/
void
abend(mess)
char* mess;
{
  declare_globals;
  fprintf(stderr,"The %dth process - %s\n",my_node,mess);
  kill(0, SIGTERM);
}
