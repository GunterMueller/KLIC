/* Compiled by KLIC compiler version 3.003 (Mon Mar 29 19:42:59 JST 1999) */
#include <klic/klichdr.h>
#include "atom.h"
#include "funct.h"

#include <stdio.h>
#include <stdlib.h>
#include <klic/timing.h>

extern print();
timerstruct before, after;
static long gctimes_before;


module module_builtin();
Const struct predicate predicate_builtin_xprint_1 =
   { module_builtin, 0, 1 };
Const struct predicate predicate_builtin_xprint_2 =
   { module_builtin, 1, 2 };
Const struct predicate predicate_builtin_xputc_2 =
   { module_builtin, 2, 2 };
Const struct predicate predicate_builtin_xreadint_1 =
   { module_builtin, 3, 1 };
Const struct predicate predicate_builtin_xstart__measure_0 =
   { module_builtin, 4, 0 };
Const struct predicate predicate_builtin_xreport__measure_1 =
   { module_builtin, 5, 1 };
Const struct predicate predicate_builtin_xcurrent__node_2 =
   { module_builtin, 6, 2 };
Const struct predicate predicate_builtin_xidle_0 =
   { module_builtin, 7, 0 };

module module_builtin(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  register q *allocp;
  Const struct predicate *toppred;
{
  q a0, a1;

  q *reasonp;
 module_top:
  switch_on_pred() {
    case_pred(0, print_1_top);
    case_pred(1, print_2_top);
    case_pred(2, putc_2_top);
    case_pred(3, readint_1_top);
    case_pred(4, start__measure_0_top);
    case_pred(5, report__measure_1_top);
    case_pred(6, current__node_2_top);
    last_case_pred(7, idle_0_top);
  }

 print_1_top: {
  a0 = qp->args[0];
  qp = qp->next;
 print_1_clear_reason:
  reasonp = reasons;
 print_1_0:
  print(a0); klic_putc('\n', stdout);
  proceed();
 print_1_ext_interrupt:
  reasonp = 0l;
 print_1_interrupt:
  goto interrupt_1;
 }

 print_2_top: {
  q x0;
  a0 = qp->args[0];
  a1 = qp->args[1];
  qp = qp->next;
 print_2_clear_reason:
  reasonp = reasons;
 print_2_0:
  print(a0);
  x0 = NILATOM;
  unify_value(a1, x0);
  proceed();
 print_2_ext_interrupt:
  reasonp = 0l;
 print_2_interrupt:
  goto interrupt_2;
 }

 putc_2_top: {
  q x0;
  a0 = qp->args[0];
  a1 = qp->args[1];
  qp = qp->next;
 putc_2_clear_reason:
  reasonp = reasons;
 putc_2_0:
 putc_2_1:
  if (!isint(a0)) goto putc_2_2;
  klic_putc(intval(a0),stdout);
  x0 = NILATOM;
  unify_value(a1, x0);
  proceed();
 putc_2_2:
  if (!isref(a0)) goto putc_2_interrupt;
  deref_and_jump(a0,putc_2_1);
  *reasonp++ =  a0;
  goto putc_2_interrupt;
 putc_2_ext_interrupt:
  reasonp = 0l;
 putc_2_interrupt:
  goto interrupt_2;
 }

 readint_1_top: {
  q x0;
  a0 = qp->args[0];
  qp = qp->next;
 readint_1_clear_reason:
  reasonp = reasons;
 readint_1_0:
  {
    int n;
    int c;
    char buf[1024];
    char *bufp = buf;
    klic_fprintf(stderr,"> ");
    do {
      c = klic_getc(stdin);
      *bufp++ = c;
    } while (c != '\n' && c != -1);
    x0 = makeint(atol(buf));
  }
  unify_value(a0, x0);
  proceed();
 readint_1_ext_interrupt:
  reasonp = 0l;
 readint_1_interrupt:
  goto interrupt_1;
 }

 start__measure_0_top: {
  qp = qp->next;
 start__measure_0_clear_reason:
  reasonp = reasons;
 start__measure_0_0:
  {
    measure(before);
    gctimes_before = gctimes;
  }
  proceed();
 start__measure_0_ext_interrupt:
  reasonp = 0l;
 start__measure_0_interrupt:
  goto interrupt_0;
 }

 report__measure_1_top: {
  q x0;
  a0 = qp->args[0];
  qp = qp->next;
 report__measure_1_clear_reason:
  reasonp = reasons;
 report__measure_1_0:
  
    measure(after);
    klic_fprintf(stdout, "heap size = %d words\n", heapsize);
    {
#ifdef GETRUSAGE
      long u_usec = diff_usec(ru_utime);
      long s_usec = diff_usec(ru_stime);
      long t_usec = u_usec + s_usec;
      long swaps = field_diff(ru_nswap);
      long minflt = field_diff(ru_minflt);
      long majflt = field_diff(ru_majflt);
      long inblock = field_diff(ru_inblock);
      long outblock = field_diff(ru_oublock);
      long nvcsw = field_diff(ru_nvcsw);
      long nivcsw = field_diff(ru_nivcsw);
      x0 = makeint(t_usec/1000);
      klic_fprintf(stdout,
      		   "%ld ms total; %ld user; %ld system\n",
	   	   t_usec/1000, u_usec/1000, s_usec/1000);
      klic_fprintf(stdout,
      		   "  %ld swaps; %ld minor page faults; %ld major page faults\n",
		   swaps, minflt, majflt);
      klic_fprintf(stdout,
      		   "  %ld block inputs; %ld block outputs\n",
		  inblock, outblock);
      klic_fprintf(stdout,
      		   "  %ld context switches (%ld voluntary)\n",
		   nvcsw+nivcsw, nvcsw);
#else
      long u_msec = (int) tick2msec(field_diff(tms_utime));
      long s_msec = (int) tick2msec(field_diff(tms_stime));
      long t_msec =
	(int) (tick2msec(field_diff(tms_utime)) +
	       tick2msec(field_diff(tms_stime)));
      x0 = makeint(t_msec);
      klic_fprintf(stdout,
		   "%ld ms total; %ld user; %ld system\n",
		   t_msec, u_msec, s_msec);
#endif
      klic_fprintf(stdout, "  ");
      if (measure_gc) {
	klic_fprintf(stdout,
		     "%ld ms utime & %ld ms stime in ",
		     gcums, gcsms);
      }
      klic_fprintf(stdout, "%ld GC\n", gctimes-gctimes_before);
      klic_fprintf(stdout, "  %ld suspensions; %ld resumptions\n",
		   suspensions, resumes);
      if (cum_susps != 0 || cum_resumps != 0) {
	klic_fprintf(stdout,
		     "  %ld total suspensions; %ld total resumptions\n",
		     cum_susps+suspensions, cum_resumps+resumes);
      }
  }
  unify_value(a0, x0);
  proceed();
 report__measure_1_ext_interrupt:
  reasonp = 0l;
 report__measure_1_interrupt:
  goto interrupt_1;
 }

 current__node_2_top: {
  q x0, x1;
  a0 = qp->args[0];
  a1 = qp->args[1];
  qp = qp->next;
 current__node_2_clear_reason:
  reasonp = reasons;
 current__node_2_0:
  bblt_current_node_no_check(x0,x1);
  unify_value(a0, x0);
  unify_value(a1, x1);
  proceed();
 current__node_2_ext_interrupt:
  reasonp = 0l;
 current__node_2_interrupt:
  goto interrupt_2;
 }

 idle_0_top: {
  qp = qp->next;
 idle_0_clear_reason:
  reasonp = reasons;
 idle_0_0:
  proceed();
 idle_0_ext_interrupt:
  reasonp = 0l;
 idle_0_interrupt:
  goto interrupt_0;
 }
 interrupt_2:
  allocp[3] = a1;
 interrupt_1:
  allocp[2] = a0;
 interrupt_0:
  allocp = interrupt_goal(allocp, toppred, reasonp);
 proceed_label:
  loop_within_module(module_builtin);
}
