/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

#include <sys/time.h>
#include <klic/timer.h>
#define Twotime2time(X,Y) (((X) * 86400 + (Y)))

extern q *reserve_klic_timer_interrupt();


module module_timer();
Const struct predicate predicate_timer_xadd_3 =
   { module_timer, 0, 3 };
Const struct predicate predicate_timer_xsub_3 =
   { module_timer, 1, 3 };
Const struct predicate predicate_timer_xcompare_3 =
   { module_timer, 2, 3 };
Const struct predicate predicate_timer_xget__time__of__day_1 =
   { module_timer, 3, 1 };
Const struct predicate predicate_timer_xinstantiate__at_2 =
   { module_timer, 4, 2 };
Const struct predicate predicate_timer_xinstantiate__after_3 =
   { module_timer, 5, 3 };
Const struct predicate predicate_timer_xinstantiate__after_2 =
   { module_timer, 6, 2 };
Const struct predicate predicate_timer_xinstantiate__every_3 =
   { module_timer, 7, 3 };
Const struct predicate predicate_timer_xtimer__loop_5 =
   { module_timer, 8, 5 };
extern Const struct predicate predicate_timer_xadd_3;

module module_timer(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  q a0, a1, a2, a3, a4;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  switch_on_pred() {
    case_pred(0, add_3_top);
    case_pred(1, sub_3_top);
    case_pred(2, compare_3_top);
    case_pred(3, get__time__of__day_1_top);
    case_pred(4, instantiate__at_2_top);
    case_pred(5, instantiate__after_3_top);
    case_pred(6, instantiate__after_2_top);
    case_pred(7, instantiate__every_3_top);
    last_case_pred(8, timer__loop_5_top);
  }

 add_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 add_3_clear_reason:
  reasonp = reasons;
 add_3_0:
 add_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_time_3)) goto add_3_interrupt;
 add_3_2:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_time_3)) goto add_3_interrupt;
  x0 = arg(a0, 0);
 add_3_3:
  if (!isint(x0)) goto add_3_14;
  x1 = arg(a0, 1);
 add_3_4:
  if (!isint(x1)) goto add_3_13;
  x2 = arg(a0, 2);
 add_3_5:
  if (!isint(x2)) goto add_3_12;
  x3 = arg(a1, 0);
 add_3_6:
  if (!isint(x3)) goto add_3_11;
  x4 = arg(a1, 1);
 add_3_7:
  if (!isint(x4)) goto add_3_10;
  x5 = arg(a1, 2);
 add_3_8:
  if (!isint(x5)) goto add_3_9;
  
{
  struct timeval time0, time1;
  TimeSet(time0,Twotime2time(intval(x0),intval(x1)), intval(x2));
  TimeSet(time1,Twotime2time(intval(x3),intval(x4)), intval(x5));
  TimeAdd(time0,time1,time0);
  x6=makeint((unsigned long)time0.tv_sec / 86400L);
  x7=makeint((unsigned long)time0.tv_sec % 86400L);
  x8=makeint(time0.tv_usec);
}
  allocp[0] = makesym(functor_time_3);
  allocp[1] = x6;
  allocp[2] = x7;
  allocp[3] = x8;
  x9 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a2, x9);
  proceed();
 add_3_9:
  if (!isref(x5)) goto add_3_interrupt;
  deref_and_jump(x5,add_3_8);
  *reasonp++ =  x5;
  goto add_3_interrupt;
 add_3_10:
  if (!isref(x4)) goto add_3_interrupt;
  deref_and_jump(x4,add_3_7);
  *reasonp++ =  x4;
  goto add_3_interrupt;
 add_3_11:
  if (!isref(x3)) goto add_3_interrupt;
  deref_and_jump(x3,add_3_6);
  *reasonp++ =  x3;
  goto add_3_interrupt;
 add_3_12:
  if (!isref(x2)) goto add_3_interrupt;
  deref_and_jump(x2,add_3_5);
  *reasonp++ =  x2;
  goto add_3_interrupt;
 add_3_13:
  if (!isref(x1)) goto add_3_interrupt;
  deref_and_jump(x1,add_3_4);
  *reasonp++ =  x1;
  goto add_3_interrupt;
 add_3_14:
  if (!isref(x0)) goto add_3_interrupt;
  deref_and_jump(x0,add_3_3);
  *reasonp++ =  x0;
  goto add_3_interrupt;
 case VARREF:
  deref_and_jump(a1,add_3_2);
  *reasonp++ =  a1;
  goto add_3_interrupt;
  };
  goto add_3_interrupt;
 case VARREF:
  deref_and_jump(a0,add_3_1);
  *reasonp++ =  a0;
  goto add_3_interrupt;
  };
  goto add_3_interrupt;
 add_3_ext_interrupt:
  reasonp = 0l;
 add_3_interrupt:
  goto interrupt_3;
 }

 sub_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 sub_3_clear_reason:
  reasonp = reasons;
 sub_3_0:
 sub_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_time_3)) goto sub_3_interrupt;
 sub_3_2:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_time_3)) goto sub_3_interrupt;
  x0 = arg(a0, 0);
 sub_3_3:
  if (!isint(x0)) goto sub_3_14;
  x1 = arg(a0, 1);
 sub_3_4:
  if (!isint(x1)) goto sub_3_13;
  x2 = arg(a0, 2);
 sub_3_5:
  if (!isint(x2)) goto sub_3_12;
  x3 = arg(a1, 0);
 sub_3_6:
  if (!isint(x3)) goto sub_3_11;
  x4 = arg(a1, 1);
 sub_3_7:
  if (!isint(x4)) goto sub_3_10;
  x5 = arg(a1, 2);
 sub_3_8:
  if (!isint(x5)) goto sub_3_9;
  
{
  struct timeval time0, time1;
  TimeSet(time0,Twotime2time(intval(x0),intval(x1)), intval(x2));
  TimeSet(time1,Twotime2time(intval(x3),intval(x4)), intval(x5));
  TimeSub(time0,time1,time0);
  x6=makeint((unsigned long)time0.tv_sec / 86400);
  x7=makeint((unsigned long)time0.tv_sec % 86400);
  x8=makeint(time0.tv_usec);
}
  allocp[0] = makesym(functor_time_3);
  allocp[1] = x6;
  allocp[2] = x7;
  allocp[3] = x8;
  x9 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a2, x9);
  proceed();
 sub_3_9:
  if (!isref(x5)) goto sub_3_interrupt;
  deref_and_jump(x5,sub_3_8);
  *reasonp++ =  x5;
  goto sub_3_interrupt;
 sub_3_10:
  if (!isref(x4)) goto sub_3_interrupt;
  deref_and_jump(x4,sub_3_7);
  *reasonp++ =  x4;
  goto sub_3_interrupt;
 sub_3_11:
  if (!isref(x3)) goto sub_3_interrupt;
  deref_and_jump(x3,sub_3_6);
  *reasonp++ =  x3;
  goto sub_3_interrupt;
 sub_3_12:
  if (!isref(x2)) goto sub_3_interrupt;
  deref_and_jump(x2,sub_3_5);
  *reasonp++ =  x2;
  goto sub_3_interrupt;
 sub_3_13:
  if (!isref(x1)) goto sub_3_interrupt;
  deref_and_jump(x1,sub_3_4);
  *reasonp++ =  x1;
  goto sub_3_interrupt;
 sub_3_14:
  if (!isref(x0)) goto sub_3_interrupt;
  deref_and_jump(x0,sub_3_3);
  *reasonp++ =  x0;
  goto sub_3_interrupt;
 case VARREF:
  deref_and_jump(a1,sub_3_2);
  *reasonp++ =  a1;
  goto sub_3_interrupt;
  };
  goto sub_3_interrupt;
 case VARREF:
  deref_and_jump(a0,sub_3_1);
  *reasonp++ =  a0;
  goto sub_3_interrupt;
  };
  goto sub_3_interrupt;
 sub_3_ext_interrupt:
  reasonp = 0l;
 sub_3_interrupt:
  goto interrupt_3;
 }

 compare_3_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 compare_3_clear_reason:
  reasonp = reasons;
 compare_3_0:
 compare_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_time_3)) goto compare_3_interrupt;
 compare_3_2:
  switch (ptagof(a1)) {
 case FUNCTOR:
  if (functor_of(a1) != makesym(functor_time_3)) goto compare_3_interrupt;
  x0 = arg(a0, 0);
 compare_3_3:
  if (!isint(x0)) goto compare_3_20;
  x1 = arg(a1, 0);
 compare_3_4:
  if (!isint(x1)) goto compare_3_19;
  gblt_greater(x0,x1,compare_3_5);
  x2 = makesym(atom__3E);
  unify_value(a2, x2);
  proceed();
 compare_3_5:
  gblt_less(x0,x1,compare_3_6);
  x2 = makesym(atom__3C);
  unify_value(a2, x2);
  proceed();
 compare_3_6:
  gblt_eq(x0,x1,compare_3_interrupt);
  x2 = arg(a0, 1);
 compare_3_7:
  if (!isint(x2)) goto compare_3_18;
  x3 = arg(a1, 1);
 compare_3_8:
  if (!isint(x3)) goto compare_3_17;
  gblt_greater(x2,x3,compare_3_9);
  x4 = makesym(atom__3E);
  unify_value(a2, x4);
  proceed();
 compare_3_9:
  gblt_less(x2,x3,compare_3_10);
  x4 = makesym(atom__3C);
  unify_value(a2, x4);
  proceed();
 compare_3_10:
  gblt_eq(x2,x3,compare_3_interrupt);
  x4 = arg(a0, 2);
 compare_3_11:
  if (!isint(x4)) goto compare_3_16;
  x5 = arg(a1, 2);
 compare_3_12:
  if (!isint(x5)) goto compare_3_15;
  gblt_less(x4,x5,compare_3_13);
  x6 = makesym(atom__3C);
  unify_value(a2, x6);
  proceed();
 compare_3_13:
  gblt_eq(x4,x5,compare_3_14);
  x6 = makesym(atom__3D);
  unify_value(a2, x6);
  proceed();
 compare_3_14:
  gblt_greater(x4,x5,compare_3_interrupt);
  x6 = makesym(atom__3E);
  unify_value(a2, x6);
  proceed();
 compare_3_15:
  if (!isref(x5)) goto compare_3_interrupt;
  deref_and_jump(x5,compare_3_12);
  *reasonp++ =  x5;
  goto compare_3_interrupt;
 compare_3_16:
  if (!isref(x4)) goto compare_3_interrupt;
  deref_and_jump(x4,compare_3_11);
  *reasonp++ =  x4;
  goto compare_3_interrupt;
 compare_3_17:
  if (!isref(x3)) goto compare_3_interrupt;
  deref_and_jump(x3,compare_3_8);
  *reasonp++ =  x3;
  goto compare_3_interrupt;
 compare_3_18:
  if (!isref(x2)) goto compare_3_interrupt;
  deref_and_jump(x2,compare_3_7);
  *reasonp++ =  x2;
  goto compare_3_interrupt;
 compare_3_19:
  if (!isref(x1)) goto compare_3_interrupt;
  deref_and_jump(x1,compare_3_4);
  *reasonp++ =  x1;
  goto compare_3_interrupt;
 compare_3_20:
  if (!isref(x0)) goto compare_3_interrupt;
  deref_and_jump(x0,compare_3_3);
  *reasonp++ =  x0;
  goto compare_3_interrupt;
 case VARREF:
  deref_and_jump(a1,compare_3_2);
  *reasonp++ =  a1;
  goto compare_3_interrupt;
  };
  goto compare_3_interrupt;
 case VARREF:
  deref_and_jump(a0,compare_3_1);
  *reasonp++ =  a0;
  goto compare_3_interrupt;
  };
  goto compare_3_interrupt;
 compare_3_ext_interrupt:
  reasonp = 0l;
 compare_3_interrupt:
  goto interrupt_3;
 }

 get__time__of__day_1_top: {
  q x0, x1, x2, x3;
  qp = qp->next;
 get__time__of__day_1_clear_reason:
  reasonp = reasons;
 get__time__of__day_1_0:
  
{
  struct timeval now;
#ifdef USETIMER
  klic_gettod(&now);
  x0 = makeint((unsigned)now.tv_sec / 86400);
  x1 = makeint((unsigned)now.tv_sec % 86400);
  x2 = makeint(now.tv_usec);
#else
  x0 = 0;
  x1 = 0;
  x2 = 0;
#endif
}
  allocp[0] = makesym(functor_time_3);
  allocp[1] = x0;
  allocp[2] = x1;
  allocp[3] = x2;
  x3 = makefunctor(&allocp[0]);
  allocp += 4;
  unify_value(a0, x3);
  proceed();
 get__time__of__day_1_ext_interrupt:
  reasonp = 0l;
 get__time__of__day_1_interrupt:
  toppred = &predicate_timer_xget__time__of__day_1;
  goto interrupt_1;
 }

 instantiate__at_2_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a1 = qp->args[1];
  qp = qp->next;
 instantiate__at_2_clear_reason:
  reasonp = reasons;
 instantiate__at_2_0:
 instantiate__at_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_time_3)) goto instantiate__at_2_interrupt;
  x0 = arg(a0, 0);
 instantiate__at_2_2:
  if (!isint(x0)) goto instantiate__at_2_7;
  x1 = makeint(0L);
  gblt_greater_or_eq(x0,x1,instantiate__at_2_interrupt);
  x2 = arg(a0, 1);
 instantiate__at_2_3:
  if (!isint(x2)) goto instantiate__at_2_6;
  x3 = makeint(0L);
  gblt_greater_or_eq(x2,x3,instantiate__at_2_interrupt);
  x4 = makeint(86400L);
  gblt_less(x2,x4,instantiate__at_2_interrupt);
  x5 = arg(a0, 2);
 instantiate__at_2_4:
  if (!isint(x5)) goto instantiate__at_2_5;
  x6 = makeint(0L);
  gblt_greater_or_eq(x5,x6,instantiate__at_2_interrupt);
  x7 = makeint(1000000L);
  gblt_less(x5,x7,instantiate__at_2_interrupt);
  
{
#ifdef USETIMER
  allocp =
    reserve_klic_timer_interrupt(allocp,
				 Twotime2time(intval(x0), intval(x2)),
				 intval(x5), a1);
#endif
}
  proceed();
 instantiate__at_2_5:
  if (!isref(x5)) goto instantiate__at_2_interrupt;
  deref_and_jump(x5,instantiate__at_2_4);
  *reasonp++ =  x5;
  goto instantiate__at_2_interrupt;
 instantiate__at_2_6:
  if (!isref(x2)) goto instantiate__at_2_interrupt;
  deref_and_jump(x2,instantiate__at_2_3);
  *reasonp++ =  x2;
  goto instantiate__at_2_interrupt;
 instantiate__at_2_7:
  if (!isref(x0)) goto instantiate__at_2_interrupt;
  deref_and_jump(x0,instantiate__at_2_2);
  *reasonp++ =  x0;
  goto instantiate__at_2_interrupt;
 case VARREF:
  deref_and_jump(a0,instantiate__at_2_1);
  *reasonp++ =  a0;
  goto instantiate__at_2_interrupt;
  };
  goto instantiate__at_2_interrupt;
 instantiate__at_2_ext_interrupt:
  reasonp = 0l;
 instantiate__at_2_interrupt:
  goto interrupt_2;
 }

 instantiate__after_3_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 instantiate__after_3_clear_reason:
  reasonp = reasons;
 instantiate__after_3_0:
 instantiate__after_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_time_2)) goto instantiate__after_3_interrupt;
  x0 = arg(a0, 0);
  x1 = makeint(86400L);
  bblt_div_no_check_y(x0,x1,x2);
  x3 = makeint(86400L);
  bblt_mod_no_check_y(x0,x3,x4);
  unify(x0, x4);
  allocp[0] = makesym(functor_time_3);
  allocp[1] = x2;
  allocp[2] = x0;
  x6 = arg(a0, 1);
  allocp[3] = x6;
  x5 = makefunctor(&allocp[0]);
  a0 = x5;
  allocp += 4;
  execute(instantiate__after_3_0);
  goto instantiate__after_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,instantiate__after_3_1);
  *reasonp++ =  a0;
  goto instantiate__after_3_interrupt;
  };
  goto instantiate__after_3_interrupt;
 instantiate__after_3_ext_interrupt:
  reasonp = 0l;
 instantiate__after_3_interrupt:
  goto interrupt_3;
 }

 instantiate__after_2_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7;
  a1 = qp->args[1];
  qp = qp->next;
 instantiate__after_2_clear_reason:
  reasonp = reasons;
 instantiate__after_2_0:
 instantiate__after_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_time_3)) goto instantiate__after_2_interrupt;
  x0 = arg(a0, 0);
 instantiate__after_2_2:
  if (!isint(x0)) goto instantiate__after_2_7;
  x1 = makeint(0L);
  gblt_greater_or_eq(x0,x1,instantiate__after_2_interrupt);
  x2 = arg(a0, 1);
 instantiate__after_2_3:
  if (!isint(x2)) goto instantiate__after_2_6;
  x3 = makeint(0L);
  gblt_greater_or_eq(x2,x3,instantiate__after_2_interrupt);
  x4 = makeint(86400L);
  gblt_less(x2,x4,instantiate__after_2_interrupt);
  x5 = arg(a0, 2);
 instantiate__after_2_4:
  if (!isint(x5)) goto instantiate__after_2_5;
  x6 = makeint(0L);
  gblt_greater_or_eq(x5,x6,instantiate__after_2_interrupt);
  x7 = makeint(1000000L);
  gblt_less(x5,x7,instantiate__after_2_interrupt);
  
{
#ifdef USETIMER
  struct timeval now, intvl, at;
  klic_gettod(&now);
  TimeSet(intvl, Twotime2time(intval(x0), intval(x2)), intval(x5));
  TimeAdd(now, intvl, at);
  allocp = reserve_klic_timer_interrupt(allocp, at.tv_sec, at.tv_usec, a1);
#endif
}
  proceed();
 instantiate__after_2_5:
  if (!isref(x5)) goto instantiate__after_2_interrupt;
  deref_and_jump(x5,instantiate__after_2_4);
  *reasonp++ =  x5;
  goto instantiate__after_2_interrupt;
 instantiate__after_2_6:
  if (!isref(x2)) goto instantiate__after_2_interrupt;
  deref_and_jump(x2,instantiate__after_2_3);
  *reasonp++ =  x2;
  goto instantiate__after_2_interrupt;
 instantiate__after_2_7:
  if (!isref(x0)) goto instantiate__after_2_interrupt;
  deref_and_jump(x0,instantiate__after_2_2);
  *reasonp++ =  x0;
  goto instantiate__after_2_interrupt;
 case VARREF:
  deref_and_jump(a0,instantiate__after_2_1);
  *reasonp++ =  a0;
  goto instantiate__after_2_interrupt;
  };
  goto instantiate__after_2_interrupt;
 instantiate__after_2_ext_interrupt:
  reasonp = 0l;
 instantiate__after_2_interrupt:
  goto interrupt_2;
 }

 instantiate__every_3_top: {
  q x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  a1 = qp->args[1];
  a2 = qp->args[2];
  qp = qp->next;
 instantiate__every_3_clear_reason:
  reasonp = reasons;
 instantiate__every_3_0:
 instantiate__every_3_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  switch (symval(functor_of(a0))) {
 case functor_time_2:
  x0 = arg(a0, 0);
  x1 = makeint(86400L);
  bblt_div_no_check_y(x0,x1,x2);
  x3 = makeint(86400L);
  bblt_mod_no_check_y(x0,x3,x4);
  unify(x0, x4);
  allocp[0] = makesym(functor_time_3);
  allocp[1] = x2;
  allocp[2] = x0;
  x6 = arg(a0, 1);
  allocp[3] = x6;
  x5 = makefunctor(&allocp[0]);
  a0 = x5;
  allocp += 4;
  execute(instantiate__every_3_0);
  goto instantiate__every_3_ext_interrupt;
 case functor_time_3:
  x0 = arg(a0, 0);
 instantiate__every_3_2:
  if (!isint(x0)) goto instantiate__every_3_7;
  x1 = makeint(0L);
  gblt_greater_or_eq(x0,x1,instantiate__every_3_interrupt);
  x2 = arg(a0, 1);
 instantiate__every_3_3:
  if (!isint(x2)) goto instantiate__every_3_6;
  x3 = makeint(0L);
  gblt_greater_or_eq(x2,x3,instantiate__every_3_interrupt);
  x4 = makeint(86400L);
  gblt_less(x2,x4,instantiate__every_3_interrupt);
  x5 = arg(a0, 2);
 instantiate__every_3_4:
  if (!isint(x5)) goto instantiate__every_3_5;
  x6 = makeint(0L);
  gblt_greater_or_eq(x5,x6,instantiate__every_3_interrupt);
  x7 = makeint(1000000L);
  gblt_less(x5,x7,instantiate__every_3_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_timer_xtimer__loop_5);
  allocp[2] = x8 = makeref(&allocp[2]);
  allocp[3] = x9 = makeref(&allocp[3]);
  allocp[4] = a0;
  allocp[5] = a2;
  allocp[6] = a1;
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_timer_xinstantiate__at_2);
  allocp[9] = x9;
  allocp[10] = x8;
  allocp[11] = (q)(struct goalrec*)&allocp[7];
  allocp[12] = (q)(&predicate_timer_xadd_3);
  allocp[13] = x10 = makeref(&allocp[13]);
  allocp[14] = a0;
  allocp[15] = x9;
  a0 = x10;
  qp = (struct goalrec*)&allocp[11];
  allocp += 16;
  execute(get__time__of__day_1_0);
  goto get__time__of__day_1_ext_interrupt;
 instantiate__every_3_5:
  if (!isref(x5)) goto instantiate__every_3_interrupt;
  deref_and_jump(x5,instantiate__every_3_4);
  *reasonp++ =  x5;
  goto instantiate__every_3_interrupt;
 instantiate__every_3_6:
  if (!isref(x2)) goto instantiate__every_3_interrupt;
  deref_and_jump(x2,instantiate__every_3_3);
  *reasonp++ =  x2;
  goto instantiate__every_3_interrupt;
 instantiate__every_3_7:
  if (!isref(x0)) goto instantiate__every_3_interrupt;
  deref_and_jump(x0,instantiate__every_3_2);
  *reasonp++ =  x0;
  goto instantiate__every_3_interrupt;
  };
  goto instantiate__every_3_interrupt;
 case VARREF:
  deref_and_jump(a0,instantiate__every_3_1);
  *reasonp++ =  a0;
  goto instantiate__every_3_interrupt;
  };
  goto instantiate__every_3_interrupt;
 instantiate__every_3_ext_interrupt:
  reasonp = 0l;
 instantiate__every_3_interrupt:
  goto interrupt_3;
 }

 timer__loop_5_top: {
  q x0, x1, x2, x3;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 timer__loop_5_clear_reason:
  reasonp = reasons;
 timer__loop_5_0:
 timer__loop_5_1:
  if (isref(a4)) goto timer__loop_5_2;
  gblt_wait(a4,timer__loop_5_3);
  x0 = NILATOM;
  unify_value(a3, x0);
  proceed();
 timer__loop_5_2:
  deref_and_jump(a4,timer__loop_5_1);
  *reasonp++ =  a4;
  goto timer__loop_5_3;
 timer__loop_5_3:
  alternative(timer__loop_5_clear_reason);
 timer__loop_5_4:
  if (isref(a0)) goto timer__loop_5_5;
  gblt_wait(a0,timer__loop_5_interrupt);
  allocp[0] = x1 = makeref(&allocp[0]);
  allocp[1] = a0;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_timer_xtimer__loop_5);
  allocp[2] = x2 = makeref(&allocp[2]);
  allocp[3] = x3 = makeref(&allocp[3]);
  allocp[4] = a2;
  allocp[5] = x1;
  allocp[6] = a4;
  allocp[7] = (q)(struct goalrec*)&allocp[0];
  allocp[8] = (q)(&predicate_timer_xinstantiate__at_2);
  allocp[9] = x3;
  allocp[10] = x2;
  allocp[11] = (q)(struct goalrec*)&allocp[7];
  allocp[12] = (q)(&predicate_timer_xadd_3);
  allocp[13] = a1;
  allocp[14] = a2;
  allocp[15] = x3;
  qp = (struct goalrec*)&allocp[11];
  allocp += 16;
  proceed();
 timer__loop_5_5:
  deref_and_jump(a0,timer__loop_5_4);
  *reasonp++ =  a0;
  goto timer__loop_5_interrupt;
 timer__loop_5_ext_interrupt:
  reasonp = 0l;
 timer__loop_5_interrupt:
  goto interrupt_5;
 }
 interrupt_5:
  allocp[6] = a4;
 interrupt_4:
  allocp[5] = a3;
 interrupt_3:
  allocp[4] = a2;
 interrupt_2:
  allocp[3] = a1;
 interrupt_1:
  allocp[2] = a0;
 interrupt_0:
  allocp = interrupt_goal(allocp, toppred, reasonp);
 proceed_label:
  loop_within_module(module_timer);
}
