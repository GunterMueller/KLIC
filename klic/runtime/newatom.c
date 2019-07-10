/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/basic.h>
#include <klic/newatom.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/unify.h>
#include <klic/index.h>
#include <klic/atomstuffs.h>
#ifdef SHM
#include <klic/shm.h>
#endif

#ifdef STRINGH
#include <string.h>
#else
#include <strings.h>
#endif

#ifdef SHM
extern char* shm_start_addr;
extern int ATOM_TABLE_SIZE;
static char* Buff_S;
static char* Buff_E;

#define malloc_check(sz) ( (Buff_S += (sz)) > Buff_E ? \
                           (char *) abend("Atom Table overflow!"):\
                           (char *)(Buff_S - (sz)) )

init_shm_atom() {
  struct atomhashtable* tmpahp;
  struct functorhashtable* tmpfhp;
  Buff_S = shm_start_addr;
  Buff_E = Buff_S + ATOM_TABLE_SIZE;
  shm_namep = (ShmNameBuff*)malloc_check(sizeof(ShmNameBuff));
  tmpahp = (struct atomhashtable*)malloc_check(sizeof(struct atomhashtable));
  *tmpahp = *atomhp;
  atomhp = tmpahp;
  tmpfhp = (struct functorhashtable*)malloc_check(sizeof(struct functorhashtable));
  *tmpfhp = *functhp;
  functhp = tmpfhp;
}

#else
extern char *malloc_check();
#endif

static long hash_name(name)
     unsigned char *name;
{
  long value = 0L;
  unsigned char c;
  for (value = 0L;
       c = *name++;
       value = 3L*value + c)
    ;
  return value;
}

static long list2name(name,list)
     unsigned char *name;
     q list;
{
  int i = 0;
  q x;
  while (isref(list)) { list = derefone(list); }
  while(list != NILATOM) {
    if (i >= 1024 ) {
      fatal("atom length is too long");
    }
    x = car_of(list);
    while (isref(x)) {
      x = derefone(x);
    }
    name[i++] = (unsigned char)(intval(x));
    list = cdr_of(list);
    while (isref(list)) {
      list = derefone(list);
    }
  }
  name[i] = 0;
  return i;
}

static void enter_old_atom(i,name)
     unsigned int i;
     unsigned char *name;
{
  long index, index0;
  long hashvalue;

  hashvalue = hash_name(name) % (atomhp->hashtablesize);
  
  if ((index = atomhp->table[hashvalue]-1) >= 0) {
    while ((strcmp((char *)atomname[index],(char *)name))) {
      if ((index0 = nextatom[index]) == -1) {
	nextatom[index] =   i;
	nextatom[i] = -1;
	goto finish;
      } else 
	index = index0;}
  } else {
      atomhp->table[hashvalue] = i + 1;
      nextatom[i] = -1;
    }
 finish:;
}

static void enter_old_functor(i, a_no, arity)
unsigned int i;
unsigned long a_no, arity;
{
  long index, index0, hashvalue;

  hashvalue = ((long)(a_no - ATOMNUMBERBASE + arity)) % 
                                     functhp->hashtablesize;
  
  if ((index = functhp->table[hashvalue]-1) >= 0) {
    while ((functors[index] != a_no) || (arities[index] != arity)) {
      if ((index0 = nextfunctor[index]) == -1) {
	nextfunctor[index] =   i;
	nextfunctor[i] = -1;
	goto finish;
      } else index = index0;
    }} else {
      functhp->table[hashvalue] = i + 1;
      nextfunctor[i] = -1;
    }
 finish:;
}

static void make_atom_table()
{
  unsigned int i;

  init_atom = DONE;
  
  atomhp->atomid = numberOfAtoms-1;
  atomhp->hashtablesize = Hashsize;
  atomhp->table =
    (long *) malloc_check((unsigned)(sizeof(long) * Hashsize));
  namearea = (unsigned char *) malloc_check((unsigned ) Namesize);
  nameareap = namearea;
  if (numberOfAtoms) {
    atomhp->nametablesize = numberOfAtoms;
    nextatom =
      (long *) malloc_check((unsigned )(sizeof(long) * numberOfAtoms));
  } else {
    atomhp->nametablesize = 1;
    atomname = (unsigned char **) malloc_check(sizeof(char *));
    nextatom = (long *) malloc_check(sizeof(long));
  }

  for (i = 0; i < Hashsize; i++) { atomhp->table[i] = 0; }
  for (i = 0; i < atomhp->nametablesize; i++) { nextatom[i] = -1; }
  for (i = 0; i < numberOfAtoms; i++) {enter_old_atom(i,atomname[i]);}

}

static void make_functor_table()
{ unsigned int i;

  init_functor = DONE;
  
  functhp->functorid = numberOfFunctors-1;
  functhp->hashtablesize = Hashsize;
  functhp->table =
    (long *) malloc_check((unsigned)(sizeof(long) * Hashsize));
  if (numberOfFunctors) {
    functhp->functortablesize = numberOfFunctors;
    nextfunctor =
      (long *) malloc_check((unsigned )(sizeof(long)*numberOfFunctors));
  } else {
      functhp->functortablesize = 1;
      functors = (unsigned long *)malloc_check(sizeof(q));
      arities  = (unsigned long *)malloc_check(sizeof(q));
      nextfunctor = (long *)malloc_check(sizeof(q));
    }
  for (i = 0; i < Hashsize; i++) { functhp->table[i] = 0; }
  for (i = 0; i < functhp->functortablesize; i++) {nextfunctor[i] = -1;}
  for (i = 0; i < numberOfFunctors; i++) 
    { enter_old_functor(i,functors[i],arities[i]); }
}

extern unsigned char *generic_string_body();
extern unsigned long generic_string_size();

unsigned long enter_atom_body(name, namelen)
     char name[];
     unsigned long namelen;
{
  declare_globals;
  long index, index0, hashvalue;

  if (init_atom) make_atom_table();

  if (strcmp((char *)name,"[]")==0) return(0L);
  if (strcmp((char *)name,".")==0) return(1L);
  hashvalue = ((unsigned long) hash_name(name)) % atomhp->hashtablesize;

#ifdef SHM
  s_lock(a_key());
#endif
 again:  
  if ((index = (atomhp->table[hashvalue])-1) >= 0) {
    while ((strcmp((char *)atomname[index],(char *)name))) {
      if ((index0 = nextatom[index]) == -1) {
	if (atomhp->atomid +1 == atomhp->nametablesize ) goto expand;
	nextatom[index] =  (long)(++(atomhp->atomid));
	if ((long)(nameareap-namearea) + namelen + 1 >= Namesize)
	  {  namearea = (unsigned char *) malloc_check((unsigned ) Namesize);
	     nameareap = namearea; }
        strcpy((char *)nameareap,(char *)name);
	atomname[atomhp->atomid] = nameareap;
        nameareap += namelen;
	*nameareap++ = '\0';
	nextatom[atomhp->atomid] = -1;
#ifdef SHM
	s_unlock(a_key());
#endif
	return(atomhp->atomid + ATOMNUMBERBASE);
      } else 
	index = index0;}
#ifdef SHM
    s_unlock(a_key());
#endif
    return(index + ATOMNUMBERBASE);
  }
  else {
    if (atomhp->atomid +1 == atomhp->nametablesize) goto expand;
    else {
      if ((long)(nameareap-namearea) + namelen >= Namesize) {
	namearea = (unsigned char *) malloc_check((unsigned ) Namesize);
	nameareap = namearea;
      }
      atomhp->table[hashvalue] = ++(atomhp->atomid) + 1;
      strcpy((char *)nameareap,(char *)name);
      atomname[atomhp->atomid] = nameareap;
      nameareap += strlen((char *)name)+1;
      nextatom[atomhp->atomid] = -1;
    }
#ifdef SHM
    s_unlock(a_key());
#endif
    return(atomhp->atomid + ATOMNUMBERBASE);
  }

 expand:
  {
    declare_globals;
    unsigned char **newatomname; long *newnextatom; int i;
#ifdef SHM
    if ( is_shma(atomname) ) abend("Can't expand on this version.");
#endif
    newatomname = (unsigned char **)
      malloc_check((atomhp->nametablesize) * sizeof(char *)*2);
    newnextatom =
      (long *) malloc_check((atomhp->nametablesize) * sizeof(long)*2);
    for (i = 0; i < atomhp->nametablesize; i++) {
      newatomname[i] = atomname[i];
      newnextatom[i] = nextatom[i];
    }
    for (i = atomhp->nametablesize; i < 2*atomhp->nametablesize; i++) {
      newnextatom[i] = -1;
    }
    atomhp->nametablesize = 2 *atomhp->nametablesize;
    atomname = newatomname;
    nextatom = newnextatom;
  }
    goto again;
}

unsigned long intern(name0)
q name0;
{
  unsigned char name[1024];
  unsigned long length;
  if (name0 == NILATOM) return AtomError;
  length = list2name(name, name0);
  return enter_atom_body(name, length);
}

unsigned long enter_atom_string(name0)
q name0;
{
  extern unsigned char *convert_klic_string_to_c_string();
  unsigned char *name = convert_klic_string_to_c_string(name0);
  unsigned long retval =
    enter_atom_body(name, generic_string_size(functorp(name0)));
  free(name);
  return retval;
}

unsigned long enter_functor(a_no,arity)
unsigned long a_no, arity;
{
  declare_globals;
  long index, index0, hashvalue;

  if (init_functor) make_functor_table();

  hashvalue = ((long)(a_no - ATOMNUMBERBASE + arity))  %
                                       functhp->hashtablesize;
#ifdef SHM
  s_lock(f_key());
#endif
 again:  
  if ((index = functhp->table[hashvalue]-1)>= 0) {
    while ((functors[index] != a_no) || (arities[index] != arity)) {
      if ((index0 = nextfunctor[index]) == -1) {
	if (functhp->functorid +1 == functhp->functortablesize ) goto expand;
	nextfunctor[index] =   (long) (++(functhp->functorid));
	functors[functhp->functorid] = a_no;
        arities[functhp->functorid] = arity;
	nextfunctor[functhp->functorid] = -1;
#ifdef SHM
	s_unlock(f_key());
#endif
	return(functhp->functorid + FUNCTORNUMBERBASE);
      } else index = index0;}
#ifdef SHM
    s_unlock(f_key());
#endif
    return(index + FUNCTORNUMBERBASE);
  }
  else {
    if ( functhp->functorid +1 == functhp->functortablesize) goto expand;
    else {
      functhp->table[hashvalue] = ++(functhp->functorid) + 1;
      functors[functhp->functorid] = a_no;
      arities[functhp->functorid] = arity;
      nextfunctor[functhp->functorid] = -1;
    }
#ifdef SHM
    s_unlock(f_key());
#endif
    return(functhp->functorid + FUNCTORNUMBERBASE);
  }

 expand:
  { unsigned long *newfunctors;
    unsigned long *newarities;
    unsigned long *newnextfunctor;
    int i;
#ifdef SHM
    if ( is_shma(functors) ) abend("Can't expand on this version.");
#endif
    newfunctors =
      (unsigned long *)
	malloc_check((functhp->functortablesize) * sizeof(unsigned long)*2);
    newarities =
      (unsigned long *)
	malloc_check((functhp->functortablesize) * sizeof(unsigned long)*2);
    newnextfunctor =
      (unsigned long *)
	malloc_check((functhp->functortablesize) * sizeof(unsigned long)*2);
    for (i = 0; i < functhp->functortablesize; i++) {
      newfunctors[i] = functors[i];
      newarities[i] = arities[i];
      newnextfunctor[i] = nextfunctor[i];
    }
    for (i = functhp->functortablesize;
                i < 2*functhp->functortablesize; i++) {
      newnextfunctor[i] = -1;
    }
    functhp->functortablesize = 2 *functhp->functortablesize;
    functors = newfunctors;
    arities  = newarities;
    nextfunctor = (long *)newnextfunctor;
  }
    goto again;
}

unsigned long new_atom()
{ atomname[atomhp->atomid] = 0;
  return((atomhp->atomid)++ + ATOMNUMBERBASE);}
