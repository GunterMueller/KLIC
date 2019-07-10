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
#include <klic/functorstuffs.h> 
#include <klic/timing.h>

#include <klic/gb.h>
#include <klic/gobj.h>
#include <klic/susp.h>
#include <klic/functorstuffs.h>
#include <klic/atomstuffs.h>

#include <stdio.h>
#include <setjmp.h>

#include <klic/interpe.h>
#include <klic/rmon.h>

#include <klic/generic.h> 
#include <klic/index.h> 
#include <klic/alloc.h>

/*
  Distributed unification
  ANSWER VALUE		void send_answer_value(data, node, index, wec)
  READ			long send_read(node, index, var, transc)
  RELEASE		void send_release(node, index, wec)
  UNIFY			void send_unify(node, index, wec, data)

  Process migration
  THROW GOAL		struct goalrec * send_throw_goal(node, goal, oldqp)
			

  Termination for the reduction nodes
  TERMINATE MSG		void terminate_all_node()

  */

extern q search_exptbl();
extern q reply_hook_g_new();
struct goalrec *trace_goal();
extern void encode_data();
extern void send_message();
extern void send_message_without_wtc_chk();
extern void terminate_profiling();

q pop_decode_stack();

/*
  ANSWER VALUE
  */
static q*
decode_answer_value(inbuf, allocp)
combuf *inbuf;
q *allocp;
{
    q exref;
    module decoder;  
    long return_index, wec;
    declare_globals;

    INT_CL_DEBUG_X(iosprintf("%d:decode_answer_value\n", my_node));
    INT_CL_PROBE_X(netstat.recv_answer_cnt++);

    add_node_wtc(GET_BUFFER(inbuf));

    return_index = GET_BUFFER(inbuf);
    wec          = GET_BUFFER(inbuf);

    exref = (q)search_exptbl(return_index, wec);  
    
    deref_and_switch(exref, susp_label, atomic_label, cons_label, funct_label);  
    
 atomic_label:
 cons_label:
 funct_label:
    /* decode_data is not used, and collected by local GC */
    decode_data = pop_decode_stack();

    return(allocp);
 susp_label:
    
    decode_data = pop_decode_stack();
    receive_answer_flag = 1;

    /* Hack to avoid sending %unify when a ReadHook receives answer_value. */
    answer_return_exp_index = return_index;
    
    allocp = do_unify(allocp, exref, decode_data);
    
    answer_return_exp_index = -1;
    receive_answer_flag = 0;
    /* Hack to avoid sending %unify when a ReadHook receives answer_value. */
    return(allocp);
}


static long
encode_answer_value(data, buffer, index, wec)
q data;
combuf *buffer;
long index, wec;
{
    long wtc, send_wtc;

    INT_CL_PROBE_X(netstat.send_answer_cnt++);
    
    wtc = get_wtc(ANSWER_WTC_UNIT, get_wtc_normal_request);
    send_wtc = wtc;
    if (send_wtc == 0)
      send_wtc = ANSWER_WTC_UNIT;
    encode_data(buffer, data, eager_transfer_level);
    PUT4_BUFFER(buffer, decode_answer_value, send_wtc, index, wec);  
    
    return(wtc);
}
void
send_answer_value(data, node, index, wec)
long node;
q data;
long index, wec;
{
    declare_globals;
    combuf *buffer = NODE_TO_BUFFER(node);
    INT_CL_DEBUG_X(iosprintf("%d:send_answer_value\n", my_node));
    
    if(!encode_answer_value(data, buffer, index, wec)){
	message_suspend(ANSWER_WTC_UNIT, node, buffer);
    } else {
	send_message(node, buffer);
    }
}

struct generator_object_method_table* get_exref_methtab();

/*struct exref_object{
  struct generator_object_method_table *method_table;
  long node;
  long index;
  long wec;
  q    to_exref;
  long gc_flag;
};*/

#include<klic/ge_exref.h>

static int transfer_read();

/*
  READ
  */
static q*
decode_read(inbuf, allocp)
combuf *inbuf;
q *allocp;
{
    q data, rhook, readhook;
    long node, index, wec;
    declare_globals;
    long recved_wtc, trans_cnt;
    
    INT_CL_DEBUG_X(iosprintf("%d:decode_read\n", my_node));
    INT_CL_PROBE_X(netstat.recv_read_cnt++);

    /*  node_wtc = node_wtc + GET_BUFFER(inbuf);*/
    recved_wtc = GET_BUFFER(inbuf);

    data = (q)search_exptbl(GET_BUFFER(inbuf), 1L);
    trans_cnt = GET_BUFFER(inbuf);
    trans_cnt--;

    node  = GET_BUFFER(inbuf);
    index = GET_BUFFER(inbuf);
    wec   = GET_BUFFER(inbuf);

    if(node == my_node){
	add_node_wtc(recved_wtc);
	readhook = (q)search_exptbl(index, wec);

	receive_answer_flag = 1;
	/* Hack to avoid sending %unify when
	   a ReadHook receives answer_value. */
	answer_return_exp_index = index;
    
	allocp = do_unify(allocp, readhook, data);
    
	answer_return_exp_index = -1;
	receive_answer_flag = 0;
	/* Hack to avoid sending %unify
	   when a ReadHook receives answer_value. */
	return(allocp);
    }
    
    deref_and_switch(data, susp_label, atomic_label, cons_label, funct_label);
    
 susp_label:
    if(derefone(data)!=data){
	
    if(trans_cnt > 0) {
	struct susprec *term = (struct susprec *)(derefone(data));
	if(is_generator_susp(term->u)){
	    struct generator_susp *susp1 = generator_suspp(derefone(data));
	    struct exref_object *dummy_obj = 
	      (struct exref_object *)untag_generator_susp(susp1->u.o);	
	    if(dummy_obj-> method_table == get_exref_methtab()){
	      INT_CL_DEBUG_X(ioprintf("Node:%d exref read transfer\n", my_node));
	      if(transfer_read(trans_cnt,recved_wtc,node,index,wec,dummy_obj))
		return(allocp);
#if 0
	    } else {
		fatal("Decode read invalid generator");
#endif
	    }
	}
    }

    }
  
    add_node_wtc(recved_wtc);
    generic_arg[0] = makeint(node);  /* Node Number */
    generic_arg[1] = makeint(index); /* exp_table (for answer) index */
    generic_arg[2] = makeint(wec);   /* WEC */
  
    new_generic(reply_hook_g_new, 3, rhook, 0);
    
    allocp = do_unify(allocp ,rhook, data);
    return(allocp);
    
 atomic_label:
 cons_label:
 funct_label:  /* should be rewritten if some data_obj cannot be send */
    
    {
        declare_globals;
	combuf *buffer = NODE_TO_BUFFER(node);
	INT_CL_DEBUG_X
	  (iosprintf("%d:send_reply_answer_value current_wtc=%d, retwtc = %d\n",
		     my_node, node_wtc, recved_wtc));
        INT_CL_PROBE_X(netstat.send_answer_cnt++);
        encode_data(buffer, data, eager_transfer_level);
        PUT4_BUFFER(buffer, decode_answer_value, recved_wtc, index, wec);  
	send_message_without_wtc_chk(node, buffer);
    }
    return(allocp);
}

static int
transfer_read(transc,recved_wtc,node,index,wec,exref)
long transc, recved_wtc, node, index, wec;
struct exref_object* exref;
{
    declare_globals;
    if(exref->wec > MIN_WEC){
	combuf *buffer = NODE_TO_BUFFER(exref->node);
	exref->wec--;
	PUT4_BUFFER(buffer, decode_read, recved_wtc, exref->index, transc);
	PUT3_BUFFER(buffer, node, index, (long)EXPORT_UNIT_WEC);
	send_message(exref->node, buffer);
	INT_CL_DEBUG_X(ioprintf("Node %d, read transfered to node %d\n", 
		my_node, exref->node));
	return(1);
    } else {
	INT_CL_DEBUG_X(ioprintf("Node %d, tried to read transfered but failed\n", my_node));
	return(0);
    }
}

static long
encode_read(buffer, index, var, exptbl_indexp, transc)
combuf *buffer;
long index;
q var;
long * exptbl_indexp;
long transc;
{
    declare_globals;
    int ret_index,wtc, send_wtc;
    
    INT_CL_PROBE_X(netstat.send_read_cnt++);
    
    wtc = get_wtc(READ_WTC_UNIT, get_wtc_normal_request);
    send_wtc = wtc;
    if (send_wtc == 0)
      send_wtc = READ_WTC_UNIT;
/*    PUT3_BUFFER(buffer, decode_read, send_wtc, index);*/
    PUT4_BUFFER(buffer, decode_read, send_wtc, index, transc);
    
    ret_index = regist_exptbl(var);
    PUT3_BUFFER(buffer, my_node, ret_index, (long)EXPORT_UNIT_WEC);

    *exptbl_indexp = ret_index;
    return(wtc);
}

long
send_read(node, index, var, transc)
long node;	/* Node No.	of Imported Data */
long index;	/* ExpTbl index of Imported Data */
q var;		/* To Readhook			 */
long transc;
{
    declare_globals;
    long exptbl_index;
    combuf *buffer = NODE_TO_BUFFER(node);

    INT_CL_DEBUG_X(iosprintf("%d:send_read\n", my_node));
    
    if(!encode_read(buffer, index, var, &exptbl_index, transc)){
	message_suspend(READ_WTC_UNIT, node, buffer);
    } else {
	send_message(node, buffer);
    }
    return exptbl_index;
}


/*
  RELEASE
  */
static q*
decode_release(inbuf, allocp)
combuf *inbuf;
q *allocp;
{
    q exp_data;
    declare_globals;
    long exp_index, wec;
    
    INT_CL_DEBUG_X(iosprintf("%d:decode_release\n", my_node));
    INT_CL_PROBE_X(netstat.recv_release_cnt++);
    
    exp_index = GET_BUFFER(inbuf);
    wec       = GET_BUFFER(inbuf);
    
    exp_data = (q)search_exptbl(exp_index, wec);
    return(allocp);
}
static void encode_release(buffer, index, wec)
combuf *buffer;
long index;
long wec;
{
    declare_globals;
    INT_CL_PROBE_X(netstat.send_release_cnt++);
    
    PUT3_BUFFER(buffer, decode_release, index, wec);
}

void
send_release(node, index, wec)
long node, index, wec;
{
    declare_globals;
    combuf *buffer = NODE_TO_BUFFER(node);

    INT_CL_DEBUG_X(iosprintf("%d:send_release\n", my_node));

    encode_release(buffer, index, wec);
    send_message_without_wtc_chk(node, buffer);
}

/*
  UNIFY
  */
static q*
decode_unify(inbuf, allocp)
combuf *inbuf;
q *allocp;
{
    q exp_data;
    long unify_index, unify_wec;
    module decoder;    
    declare_globals;

    INT_CL_DEBUG_X(iosprintf("%d:decode_unify\n", my_node));
    INT_CL_PROBE_X(netstat.recv_unify_cnt++);

    /*  node_wtc = node_wtc + GET_BUFFER(inbuf);*/
    add_node_wtc(GET_BUFFER(inbuf));
   
    unify_index = GET_BUFFER(inbuf);
    unify_wec   = GET_BUFFER(inbuf);
    exp_data = (q)search_exptbl(unify_index, unify_wec);
/*    decoder = (module)GET_BUFFER(inbuf);
    allocp = (q *)decoder(inbuf, allocp);*/
    decode_data = pop_decode_stack();
    
    allocp = do_unify(allocp, exp_data, decode_data);
    return(allocp);
}

static long
encode_unify(buffer, index, wec, data)
combuf *buffer;
q data;
long index, wec;
{
    declare_globals;
    long wtc, send_wtc;

    INT_CL_PROBE_X(netstat.send_unify_cnt++);

    wtc = get_wtc(UNIFY_WTC_UNIT, get_wtc_normal_request);
    send_wtc = wtc;
    if (send_wtc == 0)
      send_wtc = UNIFY_WTC_UNIT;
    encode_data(buffer, data, eager_transfer_level);
    PUT4_BUFFER(buffer, decode_unify, send_wtc, index, wec);
    
    return(wtc);
}

void
send_unify(node, index, wec, data)
long node,index, wec;
q data;
{
    declare_globals;
    combuf *buffer = NODE_TO_BUFFER(node);

    INT_CL_DEBUG_X(iosprintf("%d:send_unify\n", my_node));
    
    if(!encode_unify(buffer, index, wec, data)){
	message_suspend(UNIFY_WTC_UNIT, node, buffer);
    } else {
	send_message(node, buffer);
    }
}

/*
  THROW GOAL
  */
static q*
decode_throw_goal(inbuf, allocp)
     combuf *inbuf;
     q *allocp;
{
  declare_globals;
  struct goalrec *newgoal;
  struct predicate *pred_of_goal;
  unsigned short int arity;
  int i;
  module decoder;
  long prio;

  INT_CL_DEBUG_X(iosprintf("%d:decode_throw_goal\n", my_node));
  INT_CL_PROBE_X(netstat.recv_throw_cnt++);

  /*  node_wtc = node_wtc + GET_BUFFER(inbuf);*/
  add_node_wtc(GET_BUFFER(inbuf));
  prio = GET_BUFFER(inbuf);

  pred_of_goal = (struct predicate *)(GET_BUFFER(inbuf));
  arity = (unsigned short int)(pred_of_goal->arity); 
  newgoal = (struct goalrec *)allocp;
  allocp = allocp+arity+2;
/*  if(allocp >= real_heaplimit){
    fatal("Bad estimation of decodeing, heap exausted");
  }*/

  newgoal->pred = pred_of_goal;

  for ( i=arity; i > 0 ; i--){
    newgoal->args[i-1] = pop_decode_stack();
  }  

  if(prio == current_prio){
    resume_same_prio(newgoal); 
  } else {
    (void) enqueue_goal(0, prio, newgoal, glbl);    
  }
  return(allocp);
}

static long
encode_throw_goal(goal, buffer)
struct goalrec *goal;
combuf *buffer;
{
  int arity,i;
  long wtc;
  long send_wtc;
  declare_globals;

  INT_CL_PROBE_X(netstat.send_throw_cnt++);

  wtc = get_wtc(THROW_WTC_UNIT, get_wtc_normal_request);
  send_wtc = wtc;
  if (send_wtc== 0)
    send_wtc = THROW_WTC_UNIT;

  arity = (int) (goal->pred->arity);
  for(i=0;i<arity;i++){
      encode_data(buffer, goal->args[i], eager_transfer_level);
  }

  PUT4_BUFFER(buffer, decode_throw_goal, send_wtc, (long)current_prio,
	      goal->pred);
  return(wtc);
}

struct goalrec *
send_throw_goal(node, goal, oldqp)
long node;
struct goalrec *goal;
struct goalrec *oldqp;
{
  declare_globals;
  extern int trace_flag;
  combuf *buffer;

  INT_CL_DEBUG_X(iosprintf("%d:send_throw_goal\n", my_node));
  if(node >= total_node){
    ioeprintf("Try to send node %d from node %d\n",node, my_node);
    fatal("send_throw_goal, invalid destination node");
  }    

  if(node == my_node){ /* prio of goal == prio of oldqp ?? */
    goal->next = oldqp;
    return(goal);
  } else {
    buffer = NODE_TO_BUFFER(node);
#ifdef DEBUGLIB
    if (trace_flag) {
      static long throw_trace_count = 0;
      goal = trace_goal(goal, (node+1)*100000+throw_trace_count, 0);
      throw_trace_count++;
    }
#endif
    if(!encode_throw_goal(goal, buffer)){
      message_suspend(THROW_WTC_UNIT, node, buffer);
    } else {
      send_message(node, buffer);
    }
    return(oldqp);
  }
}


/*
  TERMINATE MSG decoding/encoding
  */

static char *
put_network_statistics(io_buf)
char *io_buf;
{
  declare_globals;
  char *ptr = io_buf;
  sprintf(ptr,"NODE %d Net Stat:\n", my_node);
  ptr += strlen(ptr);
  if (IS_SHOEN_NODE(my_node)) {
    sprintf(ptr, " rcv_req_wtc %d, snd_sup_wtc %d, rcv_ret_wtc %d\n",
	    netstat.recv_request_wtc_cnt, netstat.send_supply_wtc_cnt,
	    netstat.recv_return_wtc_cnt);
    ptr += strlen(ptr);
  } else {
    sprintf(ptr,
	    " snd_thrw %d, snd_read %d, snd_ufy %d, snd_ans %d, snd_rel %d\n",
	    netstat.send_throw_cnt, netstat.send_read_cnt,
	    netstat.send_unify_cnt, netstat.send_answer_cnt,
	    netstat.send_release_cnt);
    ptr += strlen(ptr);
    sprintf(ptr,
	    " rcv_thrw %d, rcv_read %d, rcv_ufy %d, rcv_ans %d, rcv_rel %d\n",
	    netstat.recv_throw_cnt,
	    netstat.recv_read_cnt, netstat.recv_unify_cnt,
	    netstat.recv_answer_cnt, netstat.recv_release_cnt);
    ptr += strlen(ptr);
    sprintf(ptr, " snd_req_wtc %d, rcv_sup_wtc %d, snd_ret_wtc %d\n",
	    netstat.send_request_wtc_cnt, netstat.recv_supply_wtc_cnt,
	    netstat.send_return_wtc_cnt);
    ptr += strlen(ptr);
    sprintf(ptr, " msg_susp %d, msg_resu %d\n",
	    netstat.msg_suspend_cnt, netstat.msg_resume_cnt);
    ptr += strlen(ptr);
  }
  sprintf(ptr, " total_msg %d, total_send_bytes %d, total_recv_bytes %d\n\n",
	  netstat.send_throw_cnt + netstat.recv_throw_cnt
	  + netstat.send_read_cnt + netstat.recv_read_cnt
	  + netstat.send_unify_cnt + netstat.recv_unify_cnt
	  + netstat.send_answer_cnt + netstat.recv_answer_cnt
	  + netstat.send_release_cnt + netstat.recv_release_cnt
	  + netstat.send_request_wtc_cnt + netstat.recv_request_wtc_cnt
	  + netstat.send_supply_wtc_cnt + netstat.recv_supply_wtc_cnt
	  + netstat.send_return_wtc_cnt + netstat.recv_return_wtc_cnt,
	  netstat.send_data_siz, netstat.recv_data_siz);
  ptr += strlen(ptr);
  return ptr;
}

static char *
put_node_statistics(io_buf)
char *io_buf;
{
  declare_globals;
  long utime, stime;
  extern timerstruct before_exec, after_exec;

  measure(after_exec);
#define  field_diff_dist(field) (after_exec.field - before_exec.field)

#ifdef GETRUSAGE
#define  diff_usec_dist(field)\
  (field_diff_dist(field.tv_sec) * 1000000 + field_diff_dist(field.tv_usec))
  utime = (long)(diff_usec_dist(ru_utime)/1000);
  stime = (long)(diff_usec_dist(ru_stime)/1000);

#else /* !GETRUSAGE */
  utime = ((long)tick2msec(field_diff_dist(tms_utime)));
  stime = ((long)tick2msec(field_diff_dist(tms_stime)));

#endif/* !GETRUSAGE */

  {
      char *ptr = io_buf;
      sprintf(ptr, "Node %ld Local Stat:\n", my_node);
      ptr += strlen(ptr);
  
      if (!IS_SHOEN_NODE(my_node)) {
	  sprintf(ptr, " heap size = %d words\n", heapsize);
	  ptr += strlen(ptr);
      }
      sprintf(ptr, " %ld ms total; %ld user; %ld system\n",
	      utime + stime, utime, stime);
      ptr += strlen(ptr);
      if (!IS_SHOEN_NODE(my_node)) {
	  if (measure_gc) {
	      sprintf(ptr, " %ld ms utime & %ld ms stime in ",
		      gcums, gcsms);
	      ptr += strlen(ptr);
	  }
	  sprintf(ptr, " %ld GC %ld suspensions; %ld resumptions\n",
		  gctimes, suspensions, resumes);
	  ptr += strlen(ptr);
      }
      return ptr;
  }
}

static void
print_statistics()
{
    declare_globals;
    char charbuf[4096];
    char *ptr = charbuf;
    ptr = put_node_statistics(ptr);
#ifdef MACDEP_DIST_STATISTICS
    MACDEP_DIST_STATISTICS();
#endif
    ptr = put_network_statistics(ptr);
    iosprintf("%s", charbuf);
}

static q*
decode_terminate_msg(inbuf, allocp)
combuf *inbuf;
q *allocp;
{
  declare_globals;
  extern jmp_buf klic_topmost;
  if (network_statistics_flag)
    print_statistics();
  longjmp(klic_topmost, 1);
#if 0
  CloseIO();
  Close_net();
				/* usually all reduction nodes finish their
				   task here. */
  exit(0);
#endif
}

static void
encode_terminate_msg(buffer)
combuf *buffer;
{
    declare_globals;
    PUT_BUFFER(buffer, decode_terminate_msg);
}

static void
send_terminate_msg(node)
long node;
{
  declare_globals;
  combuf *buffer = NODE_TO_BUFFER(node);

  INT_CL_DEBUG_X(iosprintf("%d:send_terminate\n", my_node));

  encode_terminate_msg(buffer);
  send_message_without_wtc_chk(node, buffer);
}

/*
  Currently terminate_all_node() is called from topsucceed() on shoen node,
  therefore, this message is not delivered to the shoen.
  The child nodes should not call this function.
  */
void
terminate_all_node()
{
    declare_globals;
    long node;
    for (node = 0; node < total_node; ++node)
      send_terminate_msg(node);
    if (network_statistics_flag)
      print_statistics();
    if (rmonnode)
      terminate_profiling();
}
