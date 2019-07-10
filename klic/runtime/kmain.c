/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#define MAIN
#ifdef DIST
/*#define exit(X) myexit(__FILE__, __LINE__, X)*/
#endif
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/unify.h>
#include <klic/timing.h>
#include <klic/functorstuffs.h>

#include <string.h>
#include <stdio.h>
#include <setjmp.h>
#include <errno.h>
#ifdef USESIG
#include <signal.h>
#endif

#ifdef SHM
#include <klic/shm.h>
#endif

#ifdef DIST
/*#include <klic/trace.h>*/
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <klic/sighndl.h>
#include <klic/distio.h>
#include <klic/interpe.h>
#include <klic/rmon.h>

int receive_message();
q* gc_exp_table();
q* gc_decode_stack();
q* scan_imp_table();

#endif
#include <klic/options.h>


extern char *runtime_version, *runtime_date;

jmp_buf klic_topmost;
int start_tracing = 0;
#ifdef DEBUGLIB
int count_suspension = 0;
#endif
#ifdef USETIMER
static Volatile int dummy_alarm_set;
#endif /*USETIMER*/
module module_main();

extern Const struct predicate predicate_main_xmain_0;
#ifdef DIST
extern Const struct predicate topsucceed_pred;
#endif

char *optarg;

#ifdef DIST
static int
kill_tasks (allocp, sig)
q *allocp;
int sig;
{
    klic_fprintf(stdout, "kill tasks\n");
    kill_child();
    exit(1);
}
#endif

static void toploop()
{
  declare_globals;
  struct goalrec *qp = current_queue;
  Const struct predicate *toppred = qp->pred;
  module func = (module)toppred->func;
  while (1) {
    func = ((module (*)())func)(glbl, qp, heapp, toppred);
    qp = current_queue;
    toppred = qp->pred;
  }
}

klic_main(argc, argv)
     int argc;
     char **argv;
{
  declare_globals;
  void initalloc();
  q *initiate_prioq();
  q *reinitiate_prioq();
#ifdef USESIG
  void init_klic_signal_handling();
#endif
#ifdef USETIMER
  void init_virtualized_timer();
  void init_klic_timer_handling();
#endif
#ifdef DEBUGLIB
  extern struct goalrec *trace_goal();
#endif
#ifdef SHM
  struct timeval time_before;
  struct timezone tzp;
  gettimeofday(&time_before, NULL);
#endif /*SHM*/
  program_name = argv[0];
  total_node = num_pes = 1;
  my_node = 0;
  h_size = HEAPSIZE;
  max_h_size = (unsigned long)(-1); /* largest possible */
  max_a_ratio = 0.5;
  inc_size = INCREMENTSIZE;
#ifdef USETIMER
  dl_det_interval = 500000;	/* default deadlock detect interval is 500ms */
#endif
  {
      int targc = argc;
      char **targv = argv;
      char *errp = parse_opts(opttable, &targc, &targv);
      if (errp) {
	  int i;
	  klic_local_fprintf(stderr,
		       "Invalid or missusage option or value '%s'\n", errp);
	  klic_local_fprintf(stderr,
		     "Usage: %s [options, ...] [program arguments, ...]\n",
		       program_name);
	  for (i = 0; opttable[i].opt_str; ++i) {
	      if (opttable[i].opt_usage)
		klic_local_fprintf(stderr, "\t%s\n", opttable[i].opt_usage);
	  }
	  exit(1);
      }
      command_argc = targc;
      command_argv = targv;
      if (verbose) 
	klic_local_fprintf(stderr, "KLIC runtime version %s (%s)\n",
			   runtime_version, runtime_date);
      
  }
  total_node = num_pes;
#ifdef DIST
#ifdef USE_REDUCTION_COUNT
  reduction_count = 0;
#endif
  if (total_node == 0)
    total_node = 1;

  if (eager_transfer_all_flag) {
      eager_transfer_level = -1;
  } else if (!eager_transfer_level) {
      eager_transfer_level = 1;
  }
  {
      int child_id;
      /* Spawn total_node+1 nodes.
	 1 node for SHOEN node, 'total_node' for reduction nodes.
	 the spawn_children() should return:
	 parent process:	0
	 child processes:	1 <= nodeid <=  total_node+1 */
      child_id = spawn_children(total_node + 1, opttable,
				    program_name, command_argc, command_argv);
      if (child_id == 0) {
	  io_node_flag = 1;  /* for klic_fprintf */
	  io_init_comm();
	  io_server(command_argc, command_argv);
	  /* never return from io_server() */
	  exit(0);
      }
      my_node = child_id - 1;
      total_node = num_pes;
      /* Master node:		my_node == 0		IS_MASTER_NODE()
	 SHOEN node:		my_node == total_node	IS_SHOEN_NODE()
	 Reduction nodes:	0 <= my_node < total_node
	 */
      init_dist();
  }
#endif /* DIST */
  /* All processing depending on the argument variables must be done
     after spawn_children() or it fails to read the arguments. */
  maxactiveratio = max_a_ratio;
  heapsize = h_size;
  maxheapsize = max_h_size;
  incrementsize = inc_size;
  if (heapsize > maxheapsize)
    maxheapsize = heapsize;
  initalloc();

#ifdef SHM
  My_glbl = glbl = shm_init(glbl);
#if 0 /* for Debugging */
  getchar()
#endif
#endif

  {
    extern struct goalrec *get_top_priority_queue();
    q *allocp;
    struct goalrec *qp;

    allocp = initiate_prioq(heapp);
    qp = (struct goalrec *) allocp;

#ifdef USESIG
    init_klic_signal_handling();
#endif	/*USESIG */
#ifdef USETIMER
    init_virtualized_timer();
    init_klic_timer_handling();
#endif
#ifdef DIST
    register_gc_hook(gc_exp_table);
    register_gc_hook(gc_decode_stack);
    register_after_gc_hook(scan_imp_table);

    if (IS_MASTER_NODE(my_node)) {	/* master node */
	qp->pred = &predicate_main_xmain_0;
	heapp = allocp + 2;
	node_wtc = SUPPLY_WTC_UNIT;
    } else if (IS_SHOEN_NODE(my_node)) {	/* shoen node */
	add_signal_handler(SIGTERM, kill_tasks);
	shoen_wtc = -SUPPLY_WTC_UNIT;
	{
	    struct timeval tp;
	    if(!gettimeofday(&tp, NULL)){
		netstat.start_sec = tp.tv_sec;
		netstat.start_usec= tp.tv_usec;
	    }
	}
    }
    init_rusage(); /***** for inter NODE *****/
#else /* !DIST */
    qp->pred = &predicate_main_xmain_0;
    heapp = allocp + 2;
#endif /* DIST */
#ifdef DEBUGLIB
    cum_susps = 0;
    cum_resumps = 0;
    suspensions = 0;
    resumes = 0;
#endif /* DEBUGLIB */
    while (1) {
#ifdef DEBUGLIB
      cum_susps += suspensions;
      suspensions = 0;
      cum_resumps += resumes;
      resumes = 0;
      current_prio = -1;
      suspended_goal_list = 0;
      if (start_tracing) {
	  initiate_trace();
#ifdef DIST
	  if (IS_MASTER_NODE(my_node))
#endif
	    qp = trace_goal(qp, 1, 0);
	  start_tracing = 0;
      }
      initialize_suspension_count_table();
#endif	/*DEBUGLIB*/
#ifdef DIST
      if (IS_MASTER_NODE(my_node))
#endif
#ifdef SHM
      if ( my_node == MASTER )
#endif
	(void) enqueue_goal(0, HIGHESTPRIO-1, qp, glbl);

      current_queue = get_top_priority_queue();
      resumed_goals = 0;

#ifdef DIST
      init_dist_signal_setup();
#else /* !DIST */
      interrupt_off = -1;
      postmortem_pred = 0;
      postmortem_args = 0;
#endif /* DIST */

      if (setjmp(klic_topmost) == 0) toploop();

#ifdef DIST
      DetachIO();
      CloseIO();
      Close_net();
      break;
#else /*!DIST*/
      if (postmortem_pred == 0 || postmortem_args == 0) break;
      klic_fprintf(stderr, "Starting postmortem processing...\n");
      allocp = initiate_prioq(heapp+incrementsize/2); /* dirty patch */
      qp = (struct goalrec *) allocp;
      qp->pred = postmortem_pred;
      allocp += 2;
      if (isfunctor(postmortem_args)) {
	int arity = arityof(functor_of(postmortem_args));
	int k;
	for (k=0; k<arity; k++) {
	  *allocp++ = arg(postmortem_args, k);
	}
      }
      heapp = allocp;
#endif /*DIST*/
    }
  }
#ifdef SHM
  if ( my_node == MASTER ) {
    wait(0);
    {
      struct timeval time_after;
      struct timezone tzp;
      if (!gettimeofday(&time_after, NULL)) {
	fprintf(stderr,
		"Response time is %d msec\n",
		(time_after.tv_sec - time_before.tv_sec)*1000 +
		(time_after.tv_usec - time_before.tv_usec)/1000);
      }
    }
  }
#endif /*SHM*/
  return 0;
}

#ifdef USETIMER
static void dummy_alarm_handler()
{
    dummy_alarm_set = 0;
}
#endif

module topsucceed(glbl, qp, allocp, toppred)
     struct global_variables *glbl;
     struct goalrec *qp;
     q *allocp;
     Const struct predicate *toppred;
#ifdef DIST
{
  extern q *klic_interrupt();
  extern void send_return_wtc();
  extern Const struct predicate topsucceed_pred;

#ifdef USE_REDUCTION_COUNT
  fd_set rfds;
  struct timeval tv;
  /* Watch stdin (fd 0) to see when it has input. */
  FD_ZERO(&rfds);
  FD_SET(0, &rfds);
#endif

 idle_loop:
      
  if (IS_SHOEN_NODE(my_node)) {	/* if shoen node */
      if (shoen_wtc > 0) { 
	  ioeprintf("illegal wtc in shoen node\n");
	  fatal("illegal WTC");
      } else if(shoen_wtc == 0){
	  struct timeval tp;
	  if(!gettimeofday(&tp, NULL)){
	      iosprintf("Response time is %d msec\n",
		      (tp.tv_sec - netstat.start_sec)*1000 +
		      (tp.tv_usec - netstat.start_usec)/1000);
	  }
	  terminate_all_node();
	  CloseIO();
	  Close_net();
	  exit(0);
      }
  } else {			/* if normal node */
      if ( suspensions == resumes && !susp_msg_list) {
	  if (node_wtc)
	    send_return_wtc(node_wtc);
      }
  }

  IDLE_ON();
#ifdef SHM_DIST
  if (!(IS_SHOEN_NODE(my_node))) {
    if (interrupt_off) while ( (allocp <= heaplimit) && (signal_done));
    if (!signal_done)  {
      heaplimit = 0;
      interrupt_off = 0;
    }
  } else 
#endif
    {
#ifdef USE_REDUCTION_COUNT
	/* Wait up to five seconds. */
	tv.tv_sec = 0;
  if (!(IS_SHOEN_NODE(my_node))) {
	tv.tv_usec = polling_interval;
  } else {
	tv.tv_usec = SHOEN_POLLING_INTERVAL;
  }
	(void) select(1, &rfds, NULL, NULL, &tv);
#else
	sigset_t new_mask;
	sigset_t prev_mask;
	if (sigfillset(&new_mask) < 0)
	  fatalp("sigfillset", "");
	if (sigprocmask(SIG_BLOCK, &new_mask, &prev_mask) < 0)
	  fatalp("sigprocmask", "mask set error");
	if (interrupt_off)
	  if (sigsuspend(&prev_mask) < 0 && errno != EINTR)
	    fatalp("sigsuspend", "");
	if (sigprocmask(SIG_SETMASK, &prev_mask, (sigset_t *)0) < 0)
	  fatalp("sigprocmask", "mask reset error");
#endif
      }

  allocp[0] = (q)qp->next;
  allocp[1] = (q)&topsucceed_pred;
  qp = (struct goalrec *)allocp;
  allocp += 2;

  IDLE_OFF();
  heapp = klic_interrupt(allocp, qp);
  qp = current_queue;

  if ((qp->pred == &topsucceed_pred) && (heapp == allocp)) {
      allocp -= 2;
      goto idle_loop;
  }
  
  return (module) current_queue->pred->func;
}

#else /* #ifndef DIST */
{
  extern q *klic_interrupt();
  extern Const struct predicate topsucceed_pred;
  if (suspensions == resumes) {
#ifdef DEBUGLIB
    if(count_suspension) {
      if(suspensions) {
	klic_fprintf(stderr, "Suspension counts:\n");
	scan_suspended_pred();
      } else {
	klic_fprintf(stderr, "No suspension in this run.\n");
      }
    }
#endif
    longjmp(klic_topmost, 1);
  }

#ifdef USESIG
  /* perpetual suspension suspected, but let's wait a while for interrupts */

  /* enqueue topsucceed goal again */
  allocp[0] = (q)qp;
  allocp[1] = (q)&topsucceed_pred;
  qp = (struct goalrec *)allocp;
  allocp += 2;

  /* Let's not hastily check perpetual suspension */
#ifdef USETIMER
  if (dl_det_interval) {
    extern void call_after_specified_interval();
    sigset_t new_mask;
    sigset_t prev_mask;
    sigfillset(&new_mask);
    sigprocmask(SIG_BLOCK, &new_mask, &prev_mask);
    while (interrupt_off && dummy_alarm_set)
      sigsuspend(&prev_mask);
    if (interrupt_off) {
	call_after_specified_interval(dl_det_interval/(unsigned long)1000000,
				      dl_det_interval%(unsigned long)1000000,
				      dummy_alarm_handler);
	dummy_alarm_set = 1;
	/* Use while-do rather than do-while to avoid suspected OS bugs */
	while (interrupt_off && dummy_alarm_set)
	    sigsuspend(&prev_mask);
    }
    sigprocmask(SIG_SETMASK, &prev_mask, 0);
  }
#else
#ifdef USEUSLEEP
  usleep(1000);
#else
  (void) sleep(1);
#endif /*USEUSLEEP*/
#endif

  /* If no interrupts during the pause, run GC */
  if (interrupt_off)
#ifdef USETIMER
    if (dl_det_interval != 0)
#endif
      allocp = real_heaplimit;
  allocp = klic_interrupt(allocp, qp);
  qp = current_queue;

  if (qp->pred == &topsucceed_pred) {
    /* no interrupt after topsucceed was called */
      sigset_t new_mask;
      sigset_t prev_mask;
      if (sigfillset(&new_mask) < 0)
	fatalp("sigfillset", "");
      if (sigprocmask(SIG_BLOCK, &new_mask, &prev_mask) < 0)
	fatalp("sigprocmask", "mask set error");
      if (interrupt_off)
	if (sigsuspend(&prev_mask) < 0 && errno != EINTR)
	  fatalp("sigsuspend", "");
      if (sigprocmask(SIG_SETMASK, &prev_mask, (sigset_t *)0) < 0)
	fatalp("sigprocmask", "mask reset error");

  }
  /* check resumpsions */
  heapp = klic_interrupt(allocp, qp);
  return (module) current_queue->pred->func;
#else /*!USESIG*/
  fatalf("%d perpetually suspending goals found",
	 suspensions-resumes);
#endif /*USESIG*/
}
#endif /*DIST*/
