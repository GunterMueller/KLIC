/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

struct module_object {
  struct data_object_method_table *method_table;
  q name;
};

#define module_structure_type		struct module_object
#define declare_module_constant(mod,name)	\
{						\
  &module_g_data_method_table,			\
  name						\
}
#define makemodule(x)		makefunctor(&x)

struct predicate_object {
  struct data_object_method_table *method_table;
  Const struct predicate *pdesc; /* atom tag */
  q module_obj;
  q predicate_name;
};

#define predicate_structure_type	struct predicate_object
#define declare_pred_constant(pred,mod,name)	\
{						\
  &predicate_g_data_method_table,		\
  (struct predicate *)&pred,			\
  makemodule(mod),				\
  name						\
}

#define makepred(x)		makefunctor(&x)
