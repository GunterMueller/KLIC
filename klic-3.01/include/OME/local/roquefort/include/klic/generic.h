/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/g_methtab.h>

extern q *gd_generic();

#define new_generic(name,argc,out,offset)  \
  { out = name(argc,generic_arg,allocp+offset); allocp = heapp; }

#define call_generic(obj,funct,offset) \
  allocp = (q *)gd_generic(allocp+(offset),(obj),(funct),generic_arg)

#define guard_generic(object,mf,inargc,label) \
{ \
  q *obj = (q *)functorp(object); \
  q retval = ((struct data_object_method_table *) \
                  (*obj))->g_generic( obj, (mf), generic_arg); \
\
  if( retval != GENERIC_SUCCEEDED ){ \
    if( retval == GENERIC_FAILED ) { goto label ; } \
    else{     *reasonp++ = retval;   goto label ; } \
  } \
}

#define isgobj(x)  (isref(functor_of(x)))

#ifdef __STDC__
#define data_object_method_table_of(class) class##_g_data_method_table
#else
#define data_object_method_table_of(class) class/**/_g_data_method_table
#endif

#define isclass(x, class) \
  ((struct data_object_method_table *)(functor_of(x)) == \
   &data_object_method_table_of(class))

#define declare_method_table_of(class) \
  extern struct data_object_method_table data_object_method_table_of(class)
