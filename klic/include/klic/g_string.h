/* ----------------------------------------------------------
%   (C)1993,1994,1995 Institute for New Generation Computer Technology
%       (Read COPYRIGHT for detailed information.)
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

struct byte_string_object {
  struct data_object_method_table *method_table;
  q next;
  unsigned int index:(8*sizeof(int)-2); /* size or index */
  int iscnst:1;
  int ismbdiff:1;		/* reserved for future use */
  /*
    The following field "body" is actually used as a union:
    when shallowed, as actually the body of the string;
    when deep, i.e., for difference records, the element that differs.
    */
  unsigned char *body;
};

#define STRING_SHALLOW_MARK	(makeint(0))
#define IS_SHALLOW_STRING(s)	((s)->next == STRING_SHALLOW_MARK)

#define string_structure_type_8 struct byte_string_object

#define declare_string_constant_8(body, length)		\
{      							\
  &byte__string_g_data_method_table,			\
  STRING_SHALLOW_MARK,		/* next */		\
  length,			/* index */		\
  1,				/* iscnst */		\
  0,				/* ismbdiff */		\
  (unsigned char *)body		/* body */		\
}
