/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

extern q * GG_STD_UNIFY();
extern q GG_STD_SUSPEND();
extern long GG_STD_PRINT();
extern q *GG_STD_GC();
extern long GD_STD_REGIST();
extern long GD_STD_DEALLOCATE();
extern q *GD_STD_CLOSE();
extern q GD_STD_ENCODE();

#define GG_STD_REGIST GD_STD_REGIST
#define GG_STD_DEALLOCATE GD_STD_DEALLOCATE
#define GG_STD_CLOSE GD_STD_CLOSE
#define GG_STD_ENCODE GD_STD_ENCODE

struct generator_object_method_table GG_method_table
  = {
#ifndef GGUSE_MY_UNIFY
  GG_STD_UNIFY ,
#else
  GG_rappend(active_unify) ,
#endif

#ifndef GGUSE_MY_GENERATE
  GG_STD_GENERATE ,
#else
  GG_rappend(generate) ,
#endif

#ifndef GGUSE_MY_SUSPEND
  GG_STD_SUSPEND,
#else
  GG_rappend(suspend) ,
#endif

#ifndef GGUSE_MY_PRINT
 GG_STD_PRINT ,
#else
 GG_rappend(print) ,
#endif

#ifndef GGUSE_MY_GC
 GG_STD_GC ,
#else
 GG_rappend(gc) ,
#endif

#ifndef GGUSE_MY_REGIST
 GG_STD_REGIST ,
#else
 GG_rappend(regist) ,
#endif

#ifndef GGUSE_MY_DEALLOCATE
 GG_STD_DEALLOCATE ,
#else
 GG_rappend(deallocate) ,
#endif

#ifndef GGUSE_MY_CLOSE
 GG_STD_CLOSE ,
#else
 GG_rappend(close) ,
#endif

#ifndef GGUSE_MY_ENCODE
 GG_STD_ENCODE 
#else
 GG_rappend(encode)
#endif

};
