/* ---------------------------------------------------------- 
%   (C)1993, 1994, 1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
extern q * GC_STD_UNIFY();
extern long GC_STD_PRINT();
extern q *GC_STD_GC();
extern long GD_STD_REGIST();
extern long GD_STD_DEALLOCATE();
extern q *GD_STD_CLOSE();
extern q GD_STD_ENCODE();

#define GC_STD_REGIST GD_STD_REGIST
#define GC_STD_DEALLOCATE GD_STD_DEALLOCATE
#define GC_STD_CLOSE GD_STD_CLOSE
#define GC_STD_ENCODE GD_STD_ENCODE

struct consumer_object_method_table GC_method_table
  = {
#ifndef GCUSE_MY_UNIFY
  GC_STD_UNIFY ,
#else
  GC_rappend(active_unify) ,
#endif

#ifndef GCUSE_MY_PRINT
 GC_STD_PRINT ,
#else
 GC_rappend(print) ,
#endif

#ifndef GCUSE_MY_GC
 GC_STD_GC ,
#else
 GC_rappend(gc) ,
#endif

#ifndef GCUSE_MY_REGIST
 GC_STD_REGIST ,
#else
 GC_rappend(regist) ,
#endif

#ifndef GCUSE_MY_DEALLOCATE
 GC_STD_DEALLOCATE ,
#else
 GC_rappend(deallocate) ,
#endif

#ifndef GCUSE_MY_CLOSE
 GC_STD_CLOSE ,
#else
 GC_rappend(close) , 
#endif

#ifndef GCUSE_MY_ENCODE
 GC_STD_ENCODE 
#else
 GC_rappend(encode) 
#endif
};
