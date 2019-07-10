/* ---------------------------------------------------------- 
%   (C)1993,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>

extern char *malloc(), *realloc();

char *malloc_check(size)
     unsigned long size;
{
  char *res = malloc(size);
  if (res == 0) {
    fatalf("No more memory available from Unix\nMalloc failed for %d bytes",
	   size);
  }
  return res;
}

char *realloc_check(original, newsize)
     char *original;
     unsigned long newsize;
{
  char *res = realloc(original, newsize);
  if (res == 0) {
    fatalf("No more memory available from Unix\nRealloc failed for %d bytes",
	   newsize);
  }
  return res;
}

void reinit_alloc()
{
  declare_globals;
  heaplimit = real_heaplimit = new_space_top+heapsize;
  heapbottom = real_heaplimit+incrementsize;
  heaptop = heapp = new_space_top;
  real_heapbytesize = (unsigned long)heaptop - (unsigned long)heapbottom;
  this_more_space = 0;
  gc_hooktab_size = 32;
  num_gc_hooks = 0;
  gc_hook_table = (q*(**)())malloc_check(gc_hooktab_size*sizeof(q*(**)()));
  after_gc_hooktab_size = 32;
  num_after_gc_hooks = 0;
  after_gc_hook_table =
    (q*(**)())malloc_check(gc_hooktab_size*sizeof(q*(**)()));
}

void initalloc()
{
  declare_globals;
  unsigned long bytesize;

  if(heapsize != 0  && incrementsize != 0) {
    bytesize = (heapsize + incrementsize)*sizeof(q);
  } else {
    fatalf("Illegal memory size specification: heap = %d, incremental = %d",
	    heapsize, incrementsize);
  }
  old_space_size = new_space_size = bytesize;
  new_space_top = (q *)malloc_check(bytesize);
  old_space_top = (q *)malloc_check(bytesize);
  reinit_alloc();
}

void register_gc_hook(routine)
     q*(*routine)();
{
  declare_globals;
  if (num_gc_hooks >= gc_hooktab_size) {
    gc_hooktab_size *= 2;
    gc_hook_table = (q*(**)())
      realloc_check(gc_hook_table,
		    gc_hooktab_size*sizeof(q*(**)()));
  }
  gc_hook_table[num_gc_hooks++] = routine;
}
void register_before_pcheck_hook(routine)
     q*(*routine)();
{
  declare_globals;
  if (num_before_pcheck_hooks >= before_pcheck_hooktab_size) {
    before_pcheck_hooktab_size *= 2;
    before_pcheck_hook_table = (q*(**)())
      realloc_check(before_pcheck_hook_table,
		    before_pcheck_hooktab_size*sizeof(q*(**)()));
  }
  before_pcheck_hook_table[num_before_pcheck_hooks++] = routine;
}
void register_after_gc_hook(routine)
     q*(*routine)();
{
  declare_globals;
  if (num_after_gc_hooks >= after_gc_hooktab_size) {
    after_gc_hooktab_size *= 2;
    after_gc_hook_table = (q*(**)())
      realloc_check(after_gc_hook_table,
		    after_gc_hooktab_size*sizeof(q*(**)()));
  }
  after_gc_hook_table[num_after_gc_hooks++] = routine;
}
