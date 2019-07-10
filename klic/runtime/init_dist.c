/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/unify.h>
#include <klic/timing.h>
#include <klic/index.h>

#include <stdio.h>

#include <klic/interpe.h>
#include <klic/distio.h>
#include <klic/rmon.h>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

combuf *alloc_combuf();
void init_comm();
void init_rmonitor();

timerstruct before_exec, after_exec;

void init_rusage()
{
  measure(before_exec);
}


static void initiate_global_data()
{
/* Hack to avoid sending %unify when a ReadHook receives answer_value. */
  receive_answer_flag = 0;
  answer_return_exp_index = -1;
/* Hack to avoid sending %unify when a ReadHook receives answer_value. */

  active_exp_entry = 0;

  /* for suspending message */
  susp_msg_list = (struct susp_msg_rec *)0;

  /* Measurement Initialization */
  BZERO((char *)&netstat, sizeof(netstat));

  /* For runtime monitor */
}

void init_dist()
{
  initiate_exp_table();
  initiate_global_data();
  initiate_imp_table();
  init_comm();
  init_rmonitor();
  initiate_decode_stack();
}
