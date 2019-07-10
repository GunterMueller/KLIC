/* ----------------------------------------------------------
%   (C)1995 Institute for New Generation Computer Technology
%       (Read COPYRIGHT for detailed information.)
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

struct exref_object{
  struct generator_object_method_table *method_table;
  long node;
  long index;
  long wec;
  q    to_exref;
  long gc_flag;
};
