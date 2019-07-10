/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"
extern unsigned long enter_functor();

module module_functor__table();
Const struct predicate predicate_functor__table_xnew__functor_3 =
   { module_functor__table, 0, 3 };
Const struct predicate predicate_functor__table_x_3D_2E_2E_2 =
   { module_functor__table, 1, 2 };
Const struct predicate predicate_functor__table_xlength_3 =
   { module_functor__table, 2, 3 };
Const struct predicate predicate_functor__table_xfill__args_4 =
   { module_functor__table, 3, 4 };
Const struct predicate predicate_functor__table_xfunctor_3 =
   { module_functor__table, 4, 3 };
Const struct predicate predicate_functor__table_xfunctor_4 =
   { module_functor__table, 5, 4 };
Const struct predicate predicate_functor__table_xarg_3 =
   { module_functor__table, 6, 3 };
Const struct predicate predicate_functor__table_xarg_4 =
   { module_functor__table, 7, 4 };
Const struct predicate predicate_functor__table_xsetarg_5 =
   { module_functor__table, 8, 5 };
Const struct predicate predicate_functor__table_xsetarg_4 =
   { module_functor__table, 9, 4 };
extern Const struct predicate predicate_functor__table_xsetarg_4;
extern Const struct predicate predicate_functor__table_xnew__functor_3;

module module_functor__table(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const q cons_const_0[] = {
    makeint(0),
    makeint(0),
  };
  q a0, a1, a2, a3, a4;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  a1 = qp->args[1];
  switch_on_pred() {
    case_pred(0, new__functor_3_top);
    case_pred(1, _3D_2E_2E_2_top);
    case_pred(2, length_3_top);
    case_pred(3, fill__args_4_top);
    case_pred(4, functor_3_top);
    case_pred(5, functor_4_top);
    case_pred(6, arg_3_top);
    case_pred(7, arg_4_top);
    case_pred(8, setarg_5_top);
    last_case_pred(9, setarg_4_top);
  }

 new__functor_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 new__functor_3_clear_reason:
  reasonp = reasons;
 new__functor_3_0:
 new__functor_3_1:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (a1 != PERIODATOM) goto new__functor_3_4;
 new__functor_3_2:
  switch (ptagof(a2)) {
 case ATOMIC:
 new__functor_3_3:
  if (a2 != makeint(2L)) goto new__functor_3_4;
  x0 = makecons(cons_const_0);
  unify_value(a0, x0);
  proceed();
 case VARREF:
  deref_and_jump(a2,new__functor_3_2);
  *reasonp++ =  a2;
  goto new__functor_3_4;
  };
  goto new__functor_3_4;
 case VARREF:
  deref_and_jump(a1,new__functor_3_1);
  *reasonp++ =  a1;
  goto new__functor_3_4;
  };
  goto new__functor_3_4;
 new__functor_3_4:
  otherwise(new__functor_3_interrupt);
 new__functor_3_5:
  if (!isint(a2)) goto new__functor_3_11;
  x0 = makeint(0L);
  gblt_greater(a2,x0,new__functor_3_8);
 new__functor_3_6:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (isint(a1)) goto new__functor_3_8;
 new__functor_3_7:
  {
  long i, arity = intval(a2);
  allocp[0] = makesym(enter_functor(symval(a1),arity));
  for (i=1; i<=arity; i++) allocp[i] = makeint(0);
  x1 = makefunctor(allocp);
  allocp += arity+1;
}
  unify_value(a0, x1);
  proceed();
 case VARREF:
  deref_and_jump(a1,new__functor_3_6);
  *reasonp++ =  a1;
  goto new__functor_3_8;
  };
  goto new__functor_3_8;
 new__functor_3_8:
  if (a2 != makeint(0L)) goto new__functor_3_interrupt;
 new__functor_3_9:
  switch (ptagof(a1)) {
 case ATOMIC:
  if (isint(a1)) goto new__functor_3_interrupt;
 new__functor_3_10:
  gblt_atom(a1,new__functor_3_interrupt);
  unify_value(a0, a1);
  proceed();
 case VARREF:
  deref_and_jump(a1,new__functor_3_9);
  *reasonp++ =  a1;
  goto new__functor_3_interrupt;
  };
  goto new__functor_3_interrupt;
 new__functor_3_11:
  if (!isref(a2)) goto new__functor_3_interrupt;
  deref_and_jump(a2,new__functor_3_5);
  *reasonp++ =  a2;
  goto new__functor_3_interrupt;
 new__functor_3_ext_interrupt:
  reasonp = 0l;
 new__functor_3_interrupt:
  goto interrupt_3;
 }

 _3D_2E_2E_2_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  qp = qp->next;
 _3D_2E_2E_2_clear_reason:
  reasonp = reasons;
 _3D_2E_2E_2_0:
 _3D_2E_2E_2_1:
  switch (ptagof(a1)) {
 case CONS:
  x0 = cdr_of(a1);
 _3D_2E_2E_2_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (x0 != NILATOM) goto _3D_2E_2E_2_3;
  x1 = car_of(a1);
  unify(a0, x1);
  proceed();
 case VARREF:
  deref_and_jump(x0,_3D_2E_2E_2_2);
  *reasonp++ =  x0;
  goto _3D_2E_2E_2_3;
  };
  goto _3D_2E_2E_2_3;
 _3D_2E_2E_2_3:
  x1 = car_of(a1);
 _3D_2E_2E_2_4:
  switch (ptagof(x1)) {
 case ATOMIC:
  if (x1 != PERIODATOM) goto _3D_2E_2E_2_8;
 _3D_2E_2E_2_5:
  switch (ptagof(x0)) {
 case CONS:
  x2 = cdr_of(x0);
 _3D_2E_2E_2_6:
  switch (ptagof(x2)) {
 case CONS:
  x3 = cdr_of(x2);
 _3D_2E_2E_2_7:
  switch (ptagof(x3)) {
 case ATOMIC:
  if (x3 != NILATOM) goto _3D_2E_2E_2_8;
  x5 = car_of(x2);
  allocp[0] = x5;
  x6 = car_of(x0);
  allocp[1] = x6;
  x4 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a0, x4);
  proceed();
 case VARREF:
  deref_and_jump(x3,_3D_2E_2E_2_7);
  *reasonp++ =  x3;
  goto _3D_2E_2E_2_8;
  };
  goto _3D_2E_2E_2_8;
 case VARREF:
  deref_and_jump(x2,_3D_2E_2E_2_6);
  *reasonp++ =  x2;
  goto _3D_2E_2E_2_8;
  };
  goto _3D_2E_2E_2_8;
 case VARREF:
  deref_and_jump(x0,_3D_2E_2E_2_5);
  *reasonp++ =  x0;
  goto _3D_2E_2E_2_8;
  };
  goto _3D_2E_2E_2_8;
 case VARREF:
  deref_and_jump(x1,_3D_2E_2E_2_4);
  *reasonp++ =  x1;
  goto _3D_2E_2E_2_8;
  };
  goto _3D_2E_2E_2_8;
 case VARREF:
  deref_and_jump(a1,_3D_2E_2E_2_1);
  *reasonp++ =  a1;
  goto _3D_2E_2E_2_8;
  };
  goto _3D_2E_2E_2_8;
 _3D_2E_2E_2_8:
  otherwise(_3D_2E_2E_2_interrupt);
 _3D_2E_2E_2_9:
  switch (ptagof(a1)) {
 case CONS:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_functor__table_xfill__args_4);
  x0 = cdr_of(a1);
  allocp[2] = x0;
  allocp[3] = makeint(0L);
  allocp[4] = x1 = makeref(&allocp[4]);
  allocp[5] = a0;
  allocp[6] = (q)(struct goalrec*)&allocp[0];
  allocp[7] = (q)(&predicate_functor__table_xnew__functor_3);
  allocp[8] = x1;
  x2 = car_of(a1);
  allocp[9] = x2;
  allocp[10] = x3 = makeref(&allocp[10]);
  a0 = x0;
  a1 = makeint(0L);
  a2 = x3;
  qp = (struct goalrec*)&allocp[6];
  allocp += 11;
  execute(length_3_0);
  goto length_3_ext_interrupt;
 case VARREF:
  deref_and_jump(a1,_3D_2E_2E_2_9);
  *reasonp++ =  a1;
  goto _3D_2E_2E_2_interrupt;
  };
  goto _3D_2E_2E_2_interrupt;
 _3D_2E_2E_2_ext_interrupt:
  reasonp = 0l;
 _3D_2E_2E_2_interrupt:
  goto interrupt_2;
 }

 length_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 length_3_clear_reason:
  reasonp = reasons;
 length_3_0:
 length_3_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = makeint(1L);
  bblt_add_no_check_y(a1,x0,x1);
  a0 = cdr_of(a0);
  a1 = x1;
  execute(length_3_0);
  goto length_3_ext_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto length_3_interrupt;
  unify(a2, a1);
  proceed();
 case VARREF:
  deref_and_jump(a0,length_3_1);
  *reasonp++ =  a0;
  goto length_3_interrupt;
  };
  goto length_3_interrupt;
 length_3_ext_interrupt:
  reasonp = 0l;
 length_3_interrupt:
  toppred = &predicate_functor__table_xlength_3;
  goto interrupt_3;
 }

 fill__args_4_top: {
  q x0, x1, x2, x3;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 fill__args_4_clear_reason:
  reasonp = reasons;
 fill__args_4_0:
 fill__args_4_1:
  switch (ptagof(a0)) {
 case CONS:
 fill__args_4_2:
  switch (ptagof(a2)) {
 case FUNCTOR:
 fill__args_4_3:
 fill__args_4_4:
  if (!isint(a1)) goto fill__args_4_5;
  x0 = car_of(a0);
  
{
  struct functor *f = functorp(a2);
  f->args[intval(a1)] = x0;
  x1 = makefunctor(f);
}
  x2 = makeint(1L);
  bblt_add_no_check(a1,x2,x3);
  a0 = cdr_of(a0);
  a1 = x3;
  a2 = x1;
  execute(fill__args_4_clear_reason);
  goto fill__args_4_ext_interrupt;
 fill__args_4_5:
  if (!isref(a1)) goto fill__args_4_interrupt;
  deref_and_jump(a1,fill__args_4_4);
  *reasonp++ =  a1;
  goto fill__args_4_interrupt;
 case VARREF:
  deref_and_jump(a2,fill__args_4_2);
  *reasonp++ =  a2;
  goto fill__args_4_interrupt;
  };
  goto fill__args_4_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto fill__args_4_interrupt;
  unify(a3, a2);
  proceed();
 case VARREF:
  deref_and_jump(a0,fill__args_4_1);
  *reasonp++ =  a0;
  goto fill__args_4_interrupt;
  };
  goto fill__args_4_interrupt;
 fill__args_4_ext_interrupt:
  reasonp = 0l;
 fill__args_4_interrupt:
  goto interrupt_4;
 }

 functor_3_top: {
  q x0, x1;
  a2 = qp->args[2];
  qp = qp->next;
 functor_3_clear_reason:
  reasonp = reasons;
 functor_3_0:
 functor_3_1:
  if (isref(a0)) goto functor_3_2;
  gblt_pfunctor(a0,x0,functor_3_interrupt);
  gblt_arity(a0,x1,functor_3_interrupt);
  unify_value(a1, x0);
  unify_value(a2, x1);
  proceed();
 functor_3_2:
  deref_and_jump(a0,functor_3_1);
  *reasonp++ =  a0;
  goto functor_3_interrupt;
 functor_3_ext_interrupt:
  reasonp = 0l;
 functor_3_interrupt:
  goto interrupt_3;
 }

 functor_4_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 functor_4_clear_reason:
  reasonp = reasons;
 functor_4_0:
 functor_4_1:
  if (isref(a0)) goto functor_4_2;
  gblt_pfunctor(a0,x0,functor_4_interrupt);
  gblt_arity(a0,x1,functor_4_interrupt);
  unify_value(a1, x0);
  unify_value(a2, x1);
  unify_value(a3, a0);
  proceed();
 functor_4_2:
  deref_and_jump(a0,functor_4_1);
  *reasonp++ =  a0;
  goto functor_4_interrupt;
 functor_4_ext_interrupt:
  reasonp = 0l;
 functor_4_interrupt:
  goto interrupt_4;
 }

 arg_3_top: {
  q x0;
  a2 = qp->args[2];
  qp = qp->next;
 arg_3_clear_reason:
  reasonp = reasons;
 arg_3_0:
 arg_3_1:
  if (!isint(a0)) goto arg_3_4;
 arg_3_2:
  if (isref(a1)) goto arg_3_3;
  gblt_arg(a0,a1,x0,arg_3_interrupt);
  unify(a2, x0);
  proceed();
 arg_3_3:
  deref_and_jump(a1,arg_3_2);
  *reasonp++ =  a1;
  goto arg_3_interrupt;
 arg_3_4:
  if (!isref(a0)) goto arg_3_interrupt;
  deref_and_jump(a0,arg_3_1);
  *reasonp++ =  a0;
  goto arg_3_interrupt;
 arg_3_ext_interrupt:
  reasonp = 0l;
 arg_3_interrupt:
  goto interrupt_3;
 }

 arg_4_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 arg_4_clear_reason:
  reasonp = reasons;
 arg_4_0:
 arg_4_1:
  if (!isint(a0)) goto arg_4_4;
 arg_4_2:
  if (isref(a1)) goto arg_4_3;
  gblt_arg(a0,a1,x0,arg_4_interrupt);
  unify(a2, x0);
  unify_value(a3, a1);
  proceed();
 arg_4_3:
  deref_and_jump(a1,arg_4_2);
  *reasonp++ =  a1;
  goto arg_4_interrupt;
 arg_4_4:
  if (!isref(a0)) goto arg_4_interrupt;
  deref_and_jump(a0,arg_4_1);
  *reasonp++ =  a0;
  goto arg_4_interrupt;
 arg_4_ext_interrupt:
  reasonp = 0l;
 arg_4_interrupt:
  goto interrupt_4;
 }

 setarg_5_top: {
  q x0;
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 setarg_5_clear_reason:
  reasonp = reasons;
 setarg_5_0:
 setarg_5_1:
  if (!isint(a0)) goto setarg_5_4;
 setarg_5_2:
  if (isref(a1)) goto setarg_5_3;
  gblt_arg(a0,a1,x0,setarg_5_interrupt);
  unify(a2, x0);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_functor__table_xsetarg_4);
  allocp[2] = a0;
  allocp[3] = a1;
  allocp[4] = a3;
  allocp[5] = a4;
  qp = (struct goalrec*)&allocp[0];
  allocp += 6;
  proceed();
 setarg_5_3:
  deref_and_jump(a1,setarg_5_2);
  *reasonp++ =  a1;
  goto setarg_5_interrupt;
 setarg_5_4:
  if (!isref(a0)) goto setarg_5_interrupt;
  deref_and_jump(a0,setarg_5_1);
  *reasonp++ =  a0;
  goto setarg_5_interrupt;
 setarg_5_ext_interrupt:
  reasonp = 0l;
 setarg_5_interrupt:
  goto interrupt_5;
 }

 setarg_4_top: {
  q x0, x1;
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 setarg_4_clear_reason:
  reasonp = reasons;
 setarg_4_0:
 setarg_4_1:
  switch (ptagof(a1)) {
 case CONS:
 setarg_4_2:
  switch (ptagof(a0)) {
 case ATOMIC:
 setarg_4_3:
  switch ((unsigned long)a0) {
 case (unsigned long) makeint(1L):
  x1 = cdr_of(a1);
  allocp[0] = x1;
  allocp[1] = a2;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  proceed();
 case (unsigned long) makeint(2L):
  allocp[0] = a2;
  x1 = car_of(a1);
  allocp[1] = x1;
  x0 = makecons(&allocp[0]);
  allocp += 2;
  unify_value(a3, x0);
  proceed();
  };
  goto setarg_4_interrupt;
 case VARREF:
  deref_and_jump(a0,setarg_4_2);
  *reasonp++ =  a0;
  goto setarg_4_interrupt;
  };
  goto setarg_4_interrupt;
 case FUNCTOR:
 setarg_4_4:
 setarg_4_5:
  if (!isint(a0)) goto setarg_4_6;
  
{
  if (isfunctor(a1)) {
    struct functor *f = functorp(a1);
    int index = intval(a0);
    q pf = f->functor;
    int size = arityof(pf);
    int k;
    allocp[0] = pf;
    for (k=1; k<=size; k++) {
      allocp[k] = (k == index ? a2 :  f->args[k-1]);
    }
    x0 = makefunctor(allocp);
    allocp += size+1;
  } else goto setarg_4_interrupt;
}
  unify_value(a3, x0);
  proceed();
 setarg_4_6:
  if (!isref(a0)) goto setarg_4_interrupt;
  deref_and_jump(a0,setarg_4_5);
  *reasonp++ =  a0;
  goto setarg_4_interrupt;
 case VARREF:
  deref_and_jump(a1,setarg_4_1);
  *reasonp++ =  a1;
  goto setarg_4_interrupt;
  };
  goto setarg_4_interrupt;
 setarg_4_ext_interrupt:
  reasonp = 0l;
 setarg_4_interrupt:
  goto interrupt_4;
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
  loop_within_module(module_functor__table);
}
