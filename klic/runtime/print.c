/* ---------------------------------------------------------- 
%   (C)1993,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/index.h>
#include <klic/gb.h>
#include <klic/functorstuffs.h>
#include <klic/atomstuffs.h>
#include <stdio.h>
#include <klic/gobj.h>
#include <klic/susp.h>

#ifdef DEBUGLIB
int verbose_print;
#endif

void fprint_partially(stream, x, depth, length)
     FILE *stream;
     q x;
     unsigned long depth, length;
{
  declare_globals;
  int leng = length;
#ifdef DEBUGLIB
  extern int subterm_sp;
#endif

  deref_and_switch(x, var, atomic, cons, composite);

 atomic:
  switch (atagof(x)) {
  case INT:
    klic_fprintf(stream, "%ld", intval(x));
    return;
  case SYM:
    if (x == NILATOM) {
      klic_fprintf(stream, "[]");
    } else if (x == PERIODATOM) {
      klic_fprintf(stream, ".");
    } else {
      klic_fprintf(stream, "%s", namestringof(x));
    }
    return;
  }

 var:
  klic_fprintf(stream, "_%X", (q*)x-new_space_top);
#ifdef DEBUGLIB
 if (verbose_print) {
   if (derefone(x) != x) {
     struct susprec *susp = suspp(derefone(x));
     if (is_generator_susp(susp->u)) {
       struct generator_object *gobj =
	 untag_generator_susp(generator_suspp(susp)->u.o);
       klic_putc('(', stream);
       verbose_print = 0;
       subterm_sp = 0;
       method_table_of(gobj)->print(gobj, stream, depth, length);
       verbose_print = 1;
       klic_putc(')', stream);
     } else {
       struct hook *second_hook = susp->u.first_hook.next;
       struct hook *loophook = second_hook;
      
       do {
	 union goal_or_consumer u;
	 u = loophook->u;
	 if (u.l != 0 && !is_consumer_hook(u) && isint(u.g->next)) {
	   klic_putc('(', stream);
	   verbose_print = 0;
	   subterm_sp = 0;
	   fprint_goal(stream, u.g, 0);
	   verbose_print = 1;
	   klic_putc(')', stream);
	 }
	 loophook = loophook->next;
       } while (loophook != second_hook);
     }
   }
 }
#endif
  return;
  
  cons:
#ifdef GCDEBUG
    klic_fprintf(stream, " %x", x);
#endif
    klic_fprintf(stream, "[");
    if(depth ==0){
      klic_fprintf(stream, "..]");
      return;
    }
    while (1) {
      fprint_partially(stream, car_of(x), depth-1, length);
      x = cdr_of(x);
      deref_and_switch(x, othercdr, atomiccdr, conscdr, othercdr);
  conscdr:
      klic_fprintf(stream, ",");
      if(--leng) continue;
      klic_fprintf(stream, "..");
      goto listtail;
  }
 atomiccdr:
  if (issym(x) && symval(x) == 0) goto listtail;
 othercdr:
  klic_fprintf(stream, "|");
  fprint_partially(stream, x, depth-1, length);
 listtail:
  klic_fprintf(stream, "]");
  return;

 composite:
  {
    int i;
    q f = functor_of(x);
#ifdef GCDEBUG
    klic_fprintf(stream, " %x", x);
#endif
    if (isatomic(f)) {
      klic_fprintf(stream, "%s(",functoratomname(f));
      if(depth == 0) {
	klic_fprintf(stream, "..)");
	return;
      }
      for (i = 0; i < arityof(f)-1; i++) {
	fprint_partially(stream, arg(x,i), depth-1, length);
	klic_fprintf(stream, ",");
	if(i>length){
	  klic_fprintf(stream, "..");
	  goto funct_tail;
	}
      }
      fprint_partially(stream, arg(x,arityof(f)-1), depth-1, length);
    funct_tail:
      klic_fprintf(stream, ")");
    }else if(isref(f)){
      generic_print(((struct data_object *)(functorp(x))),
		    stream, depth, length);
    } else {
      klic_fprintf(stderr, "Invalid functor : %x\n", f);
    }
  }

  return;
}

print_partially(x, depth, length)
     q x;
     unsigned long depth, length;
{
  fprint_partially(stdout, x, depth, length);
}

fprint(stream, x)
     FILE *stream;
     q x;
{
  fprint_partially(stream, x, (unsigned long)-1, (unsigned long)-1);
}
     
print(x)
  q x;
{
  fprint_partially(stdout, x, (unsigned long)-1, (unsigned long)-1);
}

void printl(x)
q x;
{
  print(x);
  klic_putc('\n',stdout);
}

general_print(a, stream, depth, length)
     q *a;
     FILE *stream;
     unsigned long depth, length;
{
  fprint_partially(stream, a, depth, length);
}
