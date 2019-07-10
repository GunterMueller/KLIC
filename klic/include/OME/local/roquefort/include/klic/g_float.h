/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

struct float_object {
  struct data_object_method_table *method_table;
  double value;
};

#define float_structure_type	struct float_object
#define declare_float_constant(value) \
{ &float_g_data_method_table, value }
#define makefloat(x)		makefunctor(&x)
#define float_value(x) (((float_structure_type *)functorp(x))->value)
