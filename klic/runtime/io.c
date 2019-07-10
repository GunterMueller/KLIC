/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

#include <stdio.h>
#include <klic/gobj.h>
#include <klic/g_pointer.h>
#ifdef DIST
#include <klic/distio.h>
#endif
extern q gd_new_pointer();
#define file_pointer(x) \
  ((FILE*)((struct pointer_object *)(data_objectp(x)))->pointer)

struct byte_string_object;
static FILE *open_named_file(str, mode)
     struct byte_string_object *str;
     char *mode;
{
  extern unsigned char *generic_string_body();
  extern unsigned long generic_string_size();
  char buf[1024];
  (void)strncpy(buf,generic_string_body(str),generic_string_size(str));
  buf[generic_string_size(str)] = '\0';
  return fopen(buf,mode);
}

module module_io();
Const struct predicate predicate_io_xoutstreamq_2 =
   { module_io, 0, 2 };
Const struct predicate predicate_io_xoutstream_2 =
   { module_io, 1, 2 };
Const struct predicate predicate_io_xoutstream_1 =
   { module_io, 2, 1 };
Const struct predicate predicate_io_xout_2 =
   { module_io, 3, 2 };
Const struct predicate predicate_io_xwait__and__print_4 =
   { module_io, 4, 4 };
Const struct predicate predicate_io_xwaitAll_5 =
   { module_io, 5, 5 };
Const struct predicate predicate_io_xinstreamq_2 =
   { module_io, 6, 2 };
Const struct predicate predicate_io_xinstream_2 =
   { module_io, 7, 2 };
Const struct predicate predicate_io_xinstream_1 =
   { module_io, 8, 1 };
Const struct predicate predicate_io_xin_2 =
   { module_io, 9, 2 };
Const struct predicate predicate_io_xtestok_2 =
   { module_io, 10, 2 };
Const struct predicate predicate_io_xputs_5 =
   { module_io, 11, 5 };
Const struct predicate predicate_io_xerrstream_1 =
   { module_io, 12, 1 };

module module_io(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  static Const q funct_const_0[] = {
    makesym(functor_putc_1),
    makeint(10),
  };
  q a0, a1, a2, a3, a4;

  q *reasonp;
 module_top:
  a0 = qp->args[0];
  switch_on_pred() {
    case_pred(0, outstreamq_2_top);
    case_pred(1, outstream_2_top);
    case_pred(2, outstream_1_top);
    case_pred(3, out_2_top);
    case_pred(4, wait__and__print_4_top);
    case_pred(5, waitAll_5_top);
    case_pred(6, instreamq_2_top);
    case_pred(7, instream_2_top);
    case_pred(8, instream_1_top);
    case_pred(9, in_2_top);
    case_pred(10, testok_2_top);
    case_pred(11, puts_5_top);
    last_case_pred(12, errstream_1_top);
  }

 outstreamq_2_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  qp = qp->next;
 outstreamq_2_clear_reason:
  reasonp = reasons;
 outstreamq_2_0:
 outstreamq_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
 outstreamq_2_2:
  if (!isgobj(a0)) goto outstreamq_2_5;
 outstreamq_2_3:
  guard_generic(a0, functor_string_2, 0, outstreamq_2_5);
  x0 = generic_arg[0];
  x1 = generic_arg[1];
 outstreamq_2_4:
  
{
  FILE *file =
    open_named_file((struct byte_string_object *)functorp(a0), "w");
  if (file==NULL) goto outstreamq_2_5;
  x2 = gd_new_pointer((q)file,allocp);
  allocp = heapp;
}
  allocp[0] = makesym(functor_normal_1);
  allocp[1] = x4 = makeref(&allocp[1]);
  x3 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x3);
  a0 = x4;
  a1 = x2;
  execute(out_2_clear_reason);
  goto out_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,outstreamq_2_1);
  *reasonp++ =  a0;
  goto outstreamq_2_5;
  };
  goto outstreamq_2_5;
 outstreamq_2_5:
  otherwise(outstreamq_2_interrupt);
  x0 = makesym(atom_abnormal);
  unify_value(a1, x0);
  proceed();
 outstreamq_2_ext_interrupt:
  reasonp = 0l;
 outstreamq_2_interrupt:
  toppred = &predicate_io_xoutstreamq_2;
  goto interrupt_2;
 }

 outstream_2_top: {
  q x0;
  a1 = qp->args[1];
  qp = qp->next;
 outstream_2_clear_reason:
  reasonp = reasons;
 outstream_2_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_io_xtestok_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  a1 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(outstreamq_2_0);
  goto outstreamq_2_ext_interrupt;
 outstream_2_ext_interrupt:
  reasonp = 0l;
 outstream_2_interrupt:
  goto interrupt_2;
 }

 outstream_1_top: {
  q x0;
  qp = qp->next;
 outstream_1_clear_reason:
  reasonp = reasons;
 outstream_1_0:
  
{
  x0 = gd_new_pointer((q)stdout,allocp);
  allocp = heapp;
}
  a1 = x0;
  execute(out_2_clear_reason);
  goto out_2_ext_interrupt;
 outstream_1_ext_interrupt:
  reasonp = 0l;
 outstream_1_interrupt:
  goto interrupt_1;
 }

 out_2_top: {
  q x0, x1, x2, x3, x4, x5, x6;
  a1 = qp->args[1];
  qp = qp->next;
 out_2_clear_reason:
  reasonp = reasons;
 out_2_0:
 out_2_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 out_2_2:
  switch (ptagof(x0)) {
 case ATOMIC:
  if (isint(x0)) goto out_2_interrupt;
  switch (symval(x0)) {
 case symval(makesym(atom_nl)):
  x2 = cdr_of(a0);
  allocp[0] = x2;
  allocp[1] = makefunctor(funct_const_0);
  x1 = makecons(&allocp[0]);
  a0 = x1;
  allocp += 2;
  execute(out_2_0);
  goto out_2_ext_interrupt;
 case symval(makesym(atom_flush)):
 out_2_3:
  if (isref(a1)) goto out_2_4;
  (void)fflush(file_pointer(a1));
  a0 = cdr_of(a0);
  execute(out_2_clear_reason);
  goto out_2_ext_interrupt;
 out_2_4:
  deref_and_jump(a1,out_2_3);
  *reasonp++ =  a1;
  goto out_2_interrupt;
  };
  goto out_2_interrupt;
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor_putc_1:
 out_2_5:
  if (isref(a1)) goto out_2_8;
  x1 = arg(x0, 0);
 out_2_6:
  if (!isint(x1)) goto out_2_7;
  klic_putc(intval(x1), file_pointer(a1));
  a0 = cdr_of(a0);
  execute(out_2_clear_reason);
  goto out_2_ext_interrupt;
 out_2_7:
  if (!isref(x1)) goto out_2_interrupt;
  deref_and_jump(x1,out_2_6);
  *reasonp++ =  x1;
  goto out_2_interrupt;
 out_2_8:
  deref_and_jump(a1,out_2_5);
  *reasonp++ =  a1;
  goto out_2_interrupt;
 case functor_print_1:
  x1 = arg(x0, 0);
 out_2_9:
  if (isref(x1)) goto out_2_10;
  gblt_pfunctor(x1,x2,out_2_interrupt);
  gblt_arity(x1,x3,out_2_interrupt);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_io_xout_2);
  allocp[2] = x4 = makeref(&allocp[2]);
  allocp[3] = a1;
  allocp[4] = (q)(struct goalrec*)&allocp[0];
  allocp[5] = (q)(&predicate_io_xwait__and__print_4);
  allocp[6] = x5 = makeref(&allocp[6]);
  allocp[7] = a1;
  x6 = cdr_of(a0);
  allocp[8] = x6;
  allocp[9] = x4;
  a0 = makeint(1L);
  a1 = x3;
  a2 = x1;
  a3 = x1;
  a4 = x5;
  qp = (struct goalrec*)&allocp[4];
  allocp += 10;
  execute(waitAll_5_0);
  goto waitAll_5_ext_interrupt;
 out_2_10:
  deref_and_jump(x1,out_2_9);
  *reasonp++ =  x1;
  goto out_2_interrupt;
 case functor_puts_1:
  x1 = arg(x0, 0);
 out_2_11:
  switch (ptagof(x1)) {
 case FUNCTOR:
 out_2_12:
  if (!isgobj(x1)) goto out_2_interrupt;
 out_2_13:
  gblt_is_string(x1,out_2_interrupt);
  gblt_size_of_string(x1,x2,out_2_interrupt);
  gblt_element_size_of_string(x1,x3,out_2_interrupt);
 out_2_14:
  if (x3 != makeint(8L)) goto out_2_interrupt;
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_io_xout_2);
  allocp[2] = x4 = makeref(&allocp[2]);
  allocp[3] = a1;
  a1 = makeint(0L);
  a2 = x2;
  a3 = x4;
  a4 = cdr_of(a0);
  a0 = x1;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(puts_5_0);
  goto puts_5_ext_interrupt;
 case VARREF:
  deref_and_jump(x1,out_2_11);
  *reasonp++ =  x1;
  goto out_2_interrupt;
  };
  goto out_2_interrupt;
 case functor_sync_1:
  x1 = arg(x0, 0);
  x2 = makeint(0L);
  unify_value(x1, x2);
  a0 = cdr_of(a0);
  execute(out_2_0);
  goto out_2_ext_interrupt;
  };
  goto out_2_interrupt;
 case VARREF:
  deref_and_jump(x0,out_2_2);
  *reasonp++ =  x0;
  goto out_2_interrupt;
  };
  goto out_2_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto out_2_interrupt;
 out_2_15:
  if (isref(a1)) goto out_2_16;
  
{
  FILE *f = file_pointer(a1);
  if (f!=stdout && f!=stderr) (void)fclose(f);
}
  proceed();
 out_2_16:
  deref_and_jump(a1,out_2_15);
  *reasonp++ =  a1;
  goto out_2_interrupt;
 case VARREF:
  deref_and_jump(a0,out_2_1);
  *reasonp++ =  a0;
  goto out_2_interrupt;
  };
  goto out_2_interrupt;
 out_2_ext_interrupt:
  reasonp = 0l;
 out_2_interrupt:
  toppred = &predicate_io_xout_2;
  goto interrupt_2;
 }

 wait__and__print_4_top: {
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  qp = qp->next;
 wait__and__print_4_clear_reason:
  reasonp = reasons;
 wait__and__print_4_0:
 wait__and__print_4_1:
  if (isref(a1)) goto wait__and__print_4_4;
 wait__and__print_4_2:
  if (isref(a0)) goto wait__and__print_4_3;
  fprint(file_pointer(a1),a0);
  unify(a3, a2);
  proceed();
 wait__and__print_4_3:
  deref_and_jump(a0,wait__and__print_4_2);
  *reasonp++ =  a0;
  goto wait__and__print_4_interrupt;
 wait__and__print_4_4:
  deref_and_jump(a1,wait__and__print_4_1);
  *reasonp++ =  a1;
  goto wait__and__print_4_interrupt;
 wait__and__print_4_ext_interrupt:
  reasonp = 0l;
 wait__and__print_4_interrupt:
  goto interrupt_4;
 }

 waitAll_5_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 waitAll_5_clear_reason:
  reasonp = reasons;
 waitAll_5_0:
 waitAll_5_1:
  if (!isint(a0)) goto waitAll_5_9;
 waitAll_5_2:
  if (!isint(a1)) goto waitAll_5_8;
  gblt_greater(a0,a1,waitAll_5_3);
  unify(a4, a3);
  proceed();
 waitAll_5_3:
  gblt_less_or_eq(a0,a1,waitAll_5_interrupt);
 waitAll_5_4:
  if (isref(a2)) goto waitAll_5_7;
  gblt_arg(a0,a2,x0,waitAll_5_interrupt);
 waitAll_5_5:
  if (isref(x0)) goto waitAll_5_6;
  gblt_pfunctor(x0,x1,waitAll_5_interrupt);
  gblt_arity(x0,x2,waitAll_5_interrupt);
  x3 = makeint(1L);
  bblt_add_no_check(a0,x3,x4);
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_io_xwaitAll_5);
  allocp[2] = x4;
  allocp[3] = a1;
  allocp[4] = a2;
  allocp[5] = x5 = makeref(&allocp[5]);
  allocp[6] = a4;
  a0 = makeint(1L);
  a1 = x2;
  a2 = x0;
  a4 = x5;
  qp = (struct goalrec*)&allocp[0];
  allocp += 7;
  execute(waitAll_5_0);
  goto waitAll_5_ext_interrupt;
 waitAll_5_6:
  deref_and_jump(x0,waitAll_5_5);
  *reasonp++ =  x0;
  goto waitAll_5_interrupt;
 waitAll_5_7:
  deref_and_jump(a2,waitAll_5_4);
  *reasonp++ =  a2;
  goto waitAll_5_interrupt;
 waitAll_5_8:
  if (!isref(a1)) goto waitAll_5_interrupt;
  deref_and_jump(a1,waitAll_5_2);
  *reasonp++ =  a1;
  goto waitAll_5_interrupt;
 waitAll_5_9:
  if (!isref(a0)) goto waitAll_5_interrupt;
  deref_and_jump(a0,waitAll_5_1);
  *reasonp++ =  a0;
  goto waitAll_5_interrupt;
 waitAll_5_ext_interrupt:
  reasonp = 0l;
 waitAll_5_interrupt:
  toppred = &predicate_io_xwaitAll_5;
  goto interrupt_5;
 }

 instreamq_2_top: {
  q x0, x1, x2, x3, x4;
  a1 = qp->args[1];
  qp = qp->next;
 instreamq_2_clear_reason:
  reasonp = reasons;
 instreamq_2_0:
 instreamq_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
 instreamq_2_2:
  if (!isgobj(a0)) goto instreamq_2_5;
 instreamq_2_3:
  guard_generic(a0, functor_string_2, 0, instreamq_2_5);
  x0 = generic_arg[0];
  x1 = generic_arg[1];
 instreamq_2_4:
  
{
  FILE *file =
    open_named_file((struct byte_string_object *)functorp(a0), "r");
  if (file==NULL) goto instreamq_2_5;
  x2 = gd_new_pointer((q)file,allocp);
  allocp = heapp;
}
  allocp[0] = makesym(functor_normal_1);
  allocp[1] = x4 = makeref(&allocp[1]);
  x3 = makefunctor(&allocp[0]);
  allocp += 2;
  unify_value(a1, x3);
  a0 = x4;
  a1 = x2;
  execute(in_2_clear_reason);
  goto in_2_ext_interrupt;
 case VARREF:
  deref_and_jump(a0,instreamq_2_1);
  *reasonp++ =  a0;
  goto instreamq_2_5;
  };
  goto instreamq_2_5;
 instreamq_2_5:
  otherwise(instreamq_2_interrupt);
  x0 = makesym(atom_abnormal);
  unify_value(a1, x0);
  proceed();
 instreamq_2_ext_interrupt:
  reasonp = 0l;
 instreamq_2_interrupt:
  toppred = &predicate_io_xinstreamq_2;
  goto interrupt_2;
 }

 instream_2_top: {
  q x0;
  a1 = qp->args[1];
  qp = qp->next;
 instream_2_clear_reason:
  reasonp = reasons;
 instream_2_0:
  allocp[0] = (q)qp;
  allocp[1] = (q)(&predicate_io_xtestok_2);
  allocp[2] = x0 = makeref(&allocp[2]);
  allocp[3] = a1;
  a1 = x0;
  qp = (struct goalrec*)&allocp[0];
  allocp += 4;
  execute(instreamq_2_0);
  goto instreamq_2_ext_interrupt;
 instream_2_ext_interrupt:
  reasonp = 0l;
 instream_2_interrupt:
  goto interrupt_2;
 }

 instream_1_top: {
  q x0;
  qp = qp->next;
 instream_1_clear_reason:
  reasonp = reasons;
 instream_1_0:
  
{
  x0 = gd_new_pointer((q)stdin,allocp);
  allocp = heapp;
}

  a1 = x0;
  execute(in_2_clear_reason);
  goto in_2_ext_interrupt;
 instream_1_ext_interrupt:
  reasonp = 0l;
 instream_1_interrupt:
  goto interrupt_1;
 }

 in_2_top: {
  q x0, x1, x2;
  a1 = qp->args[1];
  qp = qp->next;
 in_2_clear_reason:
  reasonp = reasons;
 in_2_0:
 in_2_1:
  switch (ptagof(a0)) {
 case CONS:
  x0 = car_of(a0);
 in_2_2:
  switch (ptagof(x0)) {
 case FUNCTOR:
  switch (symval(functor_of(x0))) {
 case functor_getc_1:
 in_2_3:
  if (isref(a1)) goto in_2_4;
  x1 = makeint(getc(file_pointer(a1)));
  x2 = arg(x0, 0);
  unify_value(x2, x1);
  a0 = cdr_of(a0);
  execute(in_2_clear_reason);
  goto in_2_ext_interrupt;
 in_2_4:
  deref_and_jump(a1,in_2_3);
  *reasonp++ =  a1;
  goto in_2_interrupt;
 case functor_sync_1:
  x1 = arg(x0, 0);
  x2 = makeint(0L);
  unify_value(x1, x2);
  a0 = cdr_of(a0);
  execute(in_2_0);
  goto in_2_ext_interrupt;
  };
  goto in_2_interrupt;
 case VARREF:
  deref_and_jump(x0,in_2_2);
  *reasonp++ =  x0;
  goto in_2_interrupt;
  };
  goto in_2_interrupt;
 case ATOMIC:
  if (a0 != NILATOM) goto in_2_interrupt;
 in_2_5:
  if (isref(a1)) goto in_2_6;
  
{
  FILE *f = file_pointer(a1);
  if (f!=stdin) (void)fclose(f);
}
  proceed();
 in_2_6:
  deref_and_jump(a1,in_2_5);
  *reasonp++ =  a1;
  goto in_2_interrupt;
 case VARREF:
  deref_and_jump(a0,in_2_1);
  *reasonp++ =  a0;
  goto in_2_interrupt;
  };
  goto in_2_interrupt;
 in_2_ext_interrupt:
  reasonp = 0l;
 in_2_interrupt:
  toppred = &predicate_io_xin_2;
  goto interrupt_2;
 }

 testok_2_top: {
  q x0;
  a1 = qp->args[1];
  qp = qp->next;
 testok_2_clear_reason:
  reasonp = reasons;
 testok_2_0:
 testok_2_1:
  switch (ptagof(a0)) {
 case FUNCTOR:
  if (functor_of(a0) != makesym(functor_normal_1)) goto testok_2_interrupt;
  x0 = arg(a0, 0);
  unify(a1, x0);
  proceed();
 case VARREF:
  deref_and_jump(a0,testok_2_1);
  *reasonp++ =  a0;
  goto testok_2_interrupt;
  };
  goto testok_2_interrupt;
 testok_2_ext_interrupt:
  reasonp = 0l;
 testok_2_interrupt:
  goto interrupt_2;
 }

 puts_5_top: {
  q x0, x1, x2, x3, x4, x5;
  a1 = qp->args[1];
  a2 = qp->args[2];
  a3 = qp->args[3];
  a4 = qp->args[4];
  qp = qp->next;
 puts_5_clear_reason:
  reasonp = reasons;
 puts_5_0:
 puts_5_1:
  if (isref(a1)) goto puts_5_4;
 puts_5_2:
  if (isref(a1)) goto puts_5_3;
  if_not_equal(a1, a2, puts_5_5);
  unify(a3, a4);
  proceed();
 puts_5_3:
  deref_and_jump(a1,puts_5_2);
  *reasonp++ =  a1;
  goto puts_5_5;
 puts_5_4:
  deref_and_jump(a1,puts_5_1);
  *reasonp++ =  a1;
  goto puts_5_5;
 puts_5_5:
  otherwise(puts_5_interrupt);
 puts_5_6:
  switch (ptagof(a0)) {
 case FUNCTOR:
 puts_5_7:
  if (!isgobj(a0)) goto puts_5_interrupt;
 puts_5_8:
  gblt_is_string(a0,puts_5_interrupt);
 puts_5_9:
  if (!isint(a1)) goto puts_5_10;
  gblt_element_of_string(a0,a1,x0,puts_5_interrupt);
  x1 = makeint(1L);
  bblt_add_no_check(a1,x1,x2);
  allocp[0] = makesym(functor_putc_1);
  allocp[1] = x0;
  x3 = makefunctor(&allocp[0]);
  allocp[2] = x5 = makeref(&allocp[2]);
  allocp[3] = x3;
  x4 = makecons(&allocp[2]);
  allocp += 4;
  unify_value(a3, x4);
  a1 = x2;
  a3 = x5;
  execute(puts_5_0);
  goto puts_5_ext_interrupt;
 puts_5_10:
  if (!isref(a1)) goto puts_5_interrupt;
  deref_and_jump(a1,puts_5_9);
  *reasonp++ =  a1;
  goto puts_5_interrupt;
 case VARREF:
  deref_and_jump(a0,puts_5_6);
  *reasonp++ =  a0;
  goto puts_5_interrupt;
  };
  goto puts_5_interrupt;
 puts_5_ext_interrupt:
  reasonp = 0l;
 puts_5_interrupt:
  toppred = &predicate_io_xputs_5;
  goto interrupt_5;
 }

 errstream_1_top: {
  q x0;
  qp = qp->next;
 errstream_1_clear_reason:
  reasonp = reasons;
 errstream_1_0:
  
{
  x0 = gd_new_pointer((q)stderr,allocp);
  allocp = heapp;
}
  a1 = x0;
  execute(out_2_clear_reason);
  goto out_2_ext_interrupt;
 errstream_1_ext_interrupt:
  reasonp = 0l;
 errstream_1_interrupt:
  goto interrupt_1;
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
  loop_within_module(module_io);
}
