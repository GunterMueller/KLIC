/* ---------------------------------------------------------- 
%   (C)1994 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/predinfo.h>
extern Const struct predicate predicate_main_xmain_0;

extern Const unsigned long initial_atoms_atom_c;
extern Unsigned char **atomname_atom_c;

extern Const unsigned long initial_functors_funct_c;
extern unsigned long *functors_funct_c;
extern unsigned long *arities_funct_c;

extern Const struct modinfo defined_modules_predicates_c[];

extern int klic_main();

int main(argc, argv)
     int argc;
     char **argv;
{
  /* Globals referenced from the shared library are now put in kmain.c
     and copied from here */ /* ueda */
  return klic_main(argc, argv,
		   &predicate_main_xmain_0,
		   initial_atoms_atom_c,
		   atomname_atom_c,
		   initial_functors_funct_c,
		   functors_funct_c,
		   arities_funct_c,
		   defined_modules_predicates_c
		   );
}
