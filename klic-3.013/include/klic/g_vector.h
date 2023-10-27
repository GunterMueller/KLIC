/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

struct vector_object {
  struct data_object_method_table *method_table;
  q next;
  int index:(8*sizeof(int)-1); /* size or index */
  int iscnst:1;
  /*
    The following field "body" is actually used as a union:
    when shallowed, as actually the body of the vector;
    when deep, i.e., for difference records, the element that differs.
    */
  q *body;
};

#define VECTOR_SHALLOW_MARK	(makeint(0))
#define IS_SHALLOW_VECTOR(v)	((v)->next == VECTOR_SHALLOW_MARK)

#define vector_structure_type struct vector_object

#define declare_vector_constant(body, length)		\
{      							\
  &vector_g_data_method_table,				\
  VECTOR_SHALLOW_MARK,		/* next */		\
  length,			/* index */		\
  1,				/* iscnst */		\
  (q *) body			/* body */		\
}

extern q create_vector();
