/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

/* #include <klic/functorstuffs.h> */
/* functor staffs  */
#define FUNCTORNUMBERBASE 0
#define functoratom(f) functors[symval(f) - FUNCTORNUMBERBASE]
#define arityof(f) arities[symval(f) - FUNCTORNUMBERBASE]
/* #include <klic/functorstuff.h> end*/

#define Hashsize 1024
#define Namesize  1024
#define Atomsize 1024
#define Functorsize 1024

#define YET  -1
#define DONE 0

#define AtomError -1

static int init_atom = YET;
static int init_functor = YET;

extern unsigned long  numberOfAtoms;
extern unsigned long  numberOfFunctors;

extern unsigned long *functors;
extern unsigned long *arities;
extern unsigned char **atomname;

/*
unsigned long *functors;
long          *arities;
char          **atomname;
*/

struct atomhashtable{
  long atomid;
  unsigned long nametablesize;
  unsigned long hashtablesize;
  long *table;
} atomhtable;
struct atomhashtable* atomhp = &atomhtable;

struct functorhashtable{
  long functorid;
  unsigned long functortablesize;
  unsigned long hashtablesize;
  long *table;
} functhtable;
struct functorhashtable *functhp = &functhtable;

static long *nextatom;
static long *nextfunctor;

#ifdef SHM
typedef struct atomnamebuff {
  unsigned char *namea0;
  unsigned char *namep0;
} ShmNameBuff;
ShmNameBuff* shm_namep;
#define namearea  (shm_namep->namea0)
#define nameareap (shm_namep->namep0)
#else
static unsigned char *namearea;
static unsigned char *nameareap;
#endif
  
extern long initial_atoms;
extern long initial_functors;

#define numberOfAtoms initial_atoms

#define numberOfFunctors initial_functors
