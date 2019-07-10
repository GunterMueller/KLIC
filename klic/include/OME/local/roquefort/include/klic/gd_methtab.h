/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

extern q GD_STD_GUNIFY();
extern q * GD_STD_UNIFY();
extern long GD_STD_PRINT();
extern q *GD_STD_GC();
extern long GD_STD_REGIST();
extern long GD_STD_DEALLOCATE();
extern q *GD_STD_CLOSE();
extern q *GD_STD_GENERIC();
extern q GD_STD_GGENERIC();
extern q GD_STD_COMPARE();
extern q GD_STD_HASH();
extern q GD_STD_ENCODE();
extern q GD_STD_SHMCOPY();

struct data_object_method_table GD_method_table
  = {
#ifndef GDUSE_MY_GUNIFY
  GD_STD_GUNIFY ,
#else
  GD_rappend(passive_unify) ,
#endif

#ifndef GDUSE_MY_UNIFY
  GD_STD_UNIFY ,
#else
  GD_rappend(active_unify) ,
#endif

#ifndef GDUSE_MY_PRINT
 GD_STD_PRINT ,
#else
 GD_rappend(print) ,
#endif

#ifndef GDUSE_MY_GC
 GD_STD_GC ,
#else
 GD_rappend(gc) ,
#endif

#ifndef GDUSE_MY_REGIST
 GD_STD_REGIST ,
#else
 GD_rappend(regist) ,
#endif

#ifndef GDUSE_MY_DEALLOCATE
 GD_STD_DEALLOCATE ,
#else
 GD_rappend(deallocate) ,
#endif

#ifndef GDUSE_MY_CLOSE
 GD_STD_CLOSE ,
#else
 GD_rappend(close) ,
#endif

#ifndef GDUSE_MY_GENERIC
 GD_STD_GENERIC ,
#else
 GD_rappend(generic) ,
#endif

#ifndef GDUSE_MY_GGENERIC
 GD_STD_GGENERIC ,
#else
 GD_rappend(guard_generic) ,
#endif

#ifndef GDUSE_MY_COMPARE
 GD_STD_COMPARE ,
#else
 GD_rappend(compare) ,
#endif

#ifndef GDUSE_MY_HASH
 GD_STD_HASH ,
#else
 GD_rappend(hash) ,
#endif

#ifndef GDUSE_MY_ENCODE
 GD_STD_ENCODE ,
#else
 GD_rappend(encode) ,
#endif

#ifndef GDUSE_MY_SHMCOPY
 GD_STD_SHMCOPY
#else
 GD_rappend(encode)
#endif
};
