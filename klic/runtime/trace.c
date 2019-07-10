/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <ctype.h>
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/predinfo.h>
#include <klic/generic.h>
#include <klic/functorstuffs.h>
#include <klic/susp.h>
#ifdef STRINGH
#include <string.h>
#else
#include <strings.h>
#endif
#include <stdio.h>
#include <setjmp.h>

#include <klic/trace.h>
#include <klic/traceio.h>
#ifdef DIST
#include <klic/distio.h>
#endif
#ifdef SHM
#include <klic/shm.h>
#endif

extern module wait_penum_routine();

extern char *realloc_check();
extern char *malloc_check();
extern char *calloc();
extern void free();

static struct trace_info_rec *trace_info_recs;
static long num_info_recs, info_rec_size;

#define DefaultTraceInfo     (&trace_info_recs[0])
#define TraceInfo(g) \
  (&trace_info_recs[intval((g)->args[(g)->pred->arity-1])])

#define ExtraArgs	3
#define TraceSeq(g) \
(intval((g)->args[(g)->pred->arity-2]))

/* For remembering goals resumed during reduction */

static int n_resumed;
static struct goalrec **different_prio_resume;
static int max_different_prio_resume = 0;

#ifndef DIST
#ifdef XTERM

/****************************************
  Multi-window debugging
****************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

static void open_window(in, out, id)
     FILE **in, **out;
     long id;
{
  char sockname[17];
  char *ioprogram;
  int sock;
  int infd, outfd;
  struct sockaddr *addr;
  int addr_size;
  fd_set readfds;
  char *child_args[7];
  void *malloc();
  char title[64];

  strcpy(sockname,"/tmp/klic_XXXXXX");
  ioprogram = malloc_check(strlen(KLICLIB)+strlen("/klic/pipedio")+1);
  strcpy(ioprogram, KLICLIB);
  strcat(ioprogram, "/klic/pipedio");

  sprintf(title, "KLIC tracer (%ld)", id);
  (void) mktemp(sockname);
  sock = socket(AF_UNIX, SOCK_STREAM, 0);
  addr_size = sizeof(struct sockaddr) + strlen(sockname) + 1;
  addr = (struct sockaddr *)malloc(addr_size);
  addr->sa_family = PF_UNIX;
  strcpy(addr->sa_data, sockname);
  bind(sock, addr, addr_size);
  free(addr);
  listen(sock, 5);
  child_args[0] = XTERM;
  child_args[1] = "-T";
  child_args[2] = title;
  child_args[3] = "-e";
  child_args[4] = ioprogram;
  child_args[5] = sockname;
  child_args[6] = 0;
  if (fork() == 0) {
    execv(XTERM, child_args);
  } else {
    outfd = accept(sock, 0, 0);
    infd = accept(sock, 0, 0);
    unlink(sockname);
    *in = fdopen(infd, "r");
    *out = fdopen(outfd, "w");
  }
}
#endif
#endif

/****************************************
  Predicate and Module Tables
****************************************/

struct mod_table_entry *mod_table = 0, **mod_index;
static struct pred_table_entry *pred_table = 0;

static unsigned int n_mod, n_pred;

static mod_compare_names(x, y)
     struct mod_table_entry *x, *y;
{
  return strcmp((char *)x->name, (char *)y->name);
}

static mod_compare_addr(x, y)
     struct mod_table_entry **x, **y;
{
  return (long)(*x)->func - (long)(*y)->func;
}

static pred_compare_names(x, y)
     struct pred_table_entry *x, *y;
{
  int result;
  if (!(result = strcmp((char *)x->mte->name, (char *)y->mte->name)))
    if (!(result = strcmp((char *)x->name, (char *)y->name)))
      result = x->pred->arity - y->pred->arity;
  return result;
}  

static struct mod_table_entry *get_mte(pred)
     struct predicate *pred;
{
  module mod = (module)pred->func;
  unsigned int low = 0, high = n_mod, mid;
  for (mid = low; mid < high; mid++) {
    if ((module)mod_index[mid]->func == mod) break;
  }
  if (mid == high) {
    fprintf(stderr, "get_mte(): not found(bug).\n");  /* for debug */
    return 0;
  } else return mod_index[mid];
}

static struct pred_hash_entry {
  struct predicate *pred;
  struct pred_table_entry *pte;
} *pred_hash;

static unsigned long pred_hash_mask;

static enter_pred_hash(pred, pte)
     struct predicate *pred;
     struct pred_table_entry *pte;
{
  long h = ((((unsigned long) pred) >> 2) & pred_hash_mask);
  while (pred_hash[h].pred != 0) {
    h = (h + 1) & pred_hash_mask;
  }
  pred_hash[h].pred = pred;
  pred_hash[h].pte = pte;
}  

static struct pred_table_entry *get_pte(pred)
     struct predicate *pred;
{
  long h = ((((unsigned long) pred) >> 2) & pred_hash_mask);
  while (pred_hash[h].pred != pred) {
    if (pred_hash[h].pred == 0) {
      /* somehow we couldn't find the predicate */
      static struct pred_table_entry dummy;
      dummy.pred = pred;
      dummy.mte = get_mte(pred);
      dummy.name = (unsigned char *)"???";
      dummy.spied = 0;
      return &dummy;
    }
    h = (h + 1) & pred_hash_mask;
  }
  return pred_hash[h].pte;
}  

extern Const struct modinfo defined_modules[];

static make_name_tables()
{
  declare_globals;
  int k;
  struct mod_table_entry *mte, **mtxe;
  struct pred_table_entry *pte;

  /* Let's first count the numbers of modules and predicates in this program */
  n_pred = 0;
  n_mod = 0;
  {
    Const struct modinfo *dm;
    for (dm = &defined_modules[0]; dm->name != 0; dm++) {
      Const struct predinfo * Const *dp;
      n_mod++;
      for (dp = &dm->preds[0]; *dp != 0; dp++) {
	n_pred++;
      }
    }
  }

  /* Allocate predicate table and its hash table */
  pred_table =
    (struct pred_table_entry *)
      calloc(n_pred, sizeof(struct pred_table_entry));
  {
    unsigned long hash_size = 1;
    while (hash_size <= (3*n_pred/2)) hash_size *= 2;
    pred_hash =
      (struct pred_hash_entry *)
	calloc(hash_size, sizeof(struct pred_hash_entry));
    pred_hash_mask = hash_size - 1;
    for (k = 0; k < hash_size; k++) {
      pred_hash[k].pred = 0;
      pred_hash[k].pte = 0;
    }
  }

  /* fill predicate table */
  pte = pred_table;
  {
    Const struct modinfo *dm;
    for (dm = &defined_modules[0]; dm->name != 0; dm++) {
      Const struct predinfo * Const *dp;
      for (dp = &dm->preds[0]; *dp != 0; dp++) {
	pte->name = (*dp)->name;
	pte->pred = (*dp)->predstruct;
	pte->spied = 0;
	pte->default_trace = 1;
	pte++;
      }
    }
  }

  /* allocate module table and its index */
  mod_table =
    (struct mod_table_entry *)
      calloc(n_mod, sizeof(struct mod_table_entry));
  mod_index =
    (struct mod_table_entry **)
      calloc(n_mod, sizeof(struct mod_table_entry *));

  mte = mod_table; mtxe = mod_index;

  /* fill module table */
  {
    Const struct modinfo *dm;
    for (dm = &defined_modules[0]; dm->name != 0; dm++) {
      mte->name = dm->name;
      mte->func = (*dm->preds)->predstruct->func;
      *mtxe++ = mte;
      mte++;
    }
  }

  qsort(mod_table, n_mod, sizeof(struct mod_table_entry),
	mod_compare_names);
  qsort(mod_index, n_mod, sizeof(struct mod_table_entry *),
	mod_compare_addr);

  for (k = 0; k < n_pred; k++) {
    pred_table[k].mte = get_mte(pred_table[k].pred);
  }

  qsort(pred_table, n_pred, sizeof(struct pred_table_entry),
	pred_compare_names);
  for (k = 0; k < n_pred; k++) {
    enter_pred_hash(pred_table[k].pred, &pred_table[k]);
  }
}

/****************************************
  Initiation
****************************************/

static int trace_initiated = 0;
static module trace_trigger_routine();
static struct predicate trace_trigger_preds[MAXSTDARGS+1];
static unsigned int trace_seq;
struct enqueue_trace_rec *trace_enqueued_goals;

void initiate_trace()
{
  int k;
  make_name_tables();
  for (k=0; k<=MAXSTDARGS; k++) {
    trace_trigger_preds[k].func = trace_trigger_routine;
    trace_trigger_preds[k].pred = k;
    trace_trigger_preds[k].arity = k+ExtraArgs;
  }
  n_resumed = 0;
  max_different_prio_resume = 16;
  different_prio_resume = (struct goalrec **)
    malloc_check(max_different_prio_resume*sizeof(struct goalrec *));

  trace_seq = 2;
  trace_enqueued_goals = 0;

  trace_info_recs =
    (struct trace_info_rec *)
      malloc_check(sizeof(struct trace_info_rec));
  info_rec_size = 1;
  num_info_recs = 1;
  EnabledPorts(DefaultTraceInfo) = -1;	/* all one */
  LeashedPorts(DefaultTraceInfo) = -1;	/* all one */
  Leaping(DefaultTraceInfo) = 0;
  Spying(DefaultTraceInfo) = 0;
  Verbose(DefaultTraceInfo) = 0;
  Depth(DefaultTraceInfo) = 3;
  Length(DefaultTraceInfo) = 7;
#ifndef DIST
#ifdef SETLINEBUF
  setlinebuf(stderr);
#endif
  if ((InStream(DefaultTraceInfo) = fopen("/dev/tty", "r")) == NULL) {
    fatal("Can't open /dev/tty for trace command input");
  }
#endif
  OutStream(DefaultTraceInfo) = stderr;

  trace_initiated = 1;
}

#define InitTrace() { if (trace_initiated==0) initiate_trace(); }

/****************************************
  Main Tracing Routines
****************************************/

int trace_flag;
static struct goalrec *qp_before;
static struct goalrec *parent;
static struct trace_info_rec *parent_info;
static unsigned long parent_seq;

#define SUBGOAL_MAX 100
static char subgoal_trace[SUBGOAL_MAX];
static char is_resumed[SUBGOAL_MAX];
static int subgoal_seq[SUBGOAL_MAX];

#define SUBTERM_MAX 100
static unsigned long subterm_stack[SUBTERM_MAX];
int subterm_sp;

#define InitPort()	{ subterm_sp = 0; }

#define Traced(g)				\
  ((g)->pred->func == trace_trigger_routine)

struct goalrec *trace_goal(g, seq, info)
     struct goalrec *g;
     unsigned int seq;
     struct trace_info_rec *info;
{
  InitTrace();
  if (info==0) {
    info = DefaultTraceInfo;
  }
  if (!Traced(g)) {
    declare_globals;
    struct goalrec *ng;
    Const struct predicate *pred = g->pred;
    unsigned int k, arity;
    arity = pred->arity;
    heapalloc(ng, arity+5, (struct goalrec *));
    ng->next = g->next;
    ng->pred = &trace_trigger_preds[arity];
    for (k=0; k!=arity; k++) {
      ng->args[k] = g->args[k];
    }
    ng->args[k] = makecons(pred);
    ng->args[k+1] = makeint(seq);
    ng->args[k+2] = makeint(info-trace_info_recs);
    return ng;
  } else {
    return g;
  }
}

unsigned int untrace_goal(g)
     struct goalrec *g;
{
  if (Traced(g)) {
    unsigned int real_arity = g->pred->arity-ExtraArgs;
    g->pred = (struct predicate *)consp(g->args[real_arity]);
    return intval(g->args[real_arity+1]);
  } else {
    return 0;
  }
}

/*
static struct goalrec *
copy_traced_goal(g)
     struct goalrec *g;
{
  int k;
  struct goalrec *cg =
    (struct goalrec *)malloc_check((g->pred->arity+2)*sizeof(q*));
  cg->pred = g->pred;
  for (k=0; k!=g->pred->arity; k++) {
    cg->args[k] = g->args[k];
  }
  return cg;
}
*/

static int
  call_port_command(), reduce_port_command(),
  susp_port_command(), fail_port_command();
static void
  print_ready_queue(),
  print_suspended_goals();
static int
  print_debug_status(), print_port_names(),
  print_modules(), print_predicates();

static module trace_trigger_routine(glbl, qp, allocp, toppred)
  struct global_variables *glbl;
  struct goalrec *qp;
  q *allocp;
  Const struct predicate *toppred;
{
  extern module wait_prio_routine();
  Const struct predicate *traced_pred;
  struct pred_table_entry *pte;
  struct trace_info_rec *info;

  InitPort();
  info = TraceInfo(qp);

  /* Rewrite the queue top goal record for traced goal */
  parent_seq = untrace_goal(qp);

 retry:
  traced_pred = qp->pred;
  if (traced_pred->func == wait_prio_routine) {
    declare_globals;
    struct goalrec *ng;
    int narity = qp->pred->arity - ExtraArgs;
    int k;
    heapalloc(ng, narity+2, (struct goalrec *));
    ng->pred = (struct predicate *)(consp(qp->args[narity]));
    for (k=0; k!=narity; k++) {
      ng->args[k] = qp->args[k];
    }
    ng->next = qp->next;
    qp = ng;
    goto retry;
  }
  else if (traced_pred->func == wait_penum_routine) {
    declare_globals;
    struct goalrec *ng;
    int narity = qp->pred->arity - 2;
    int k;
    heapalloc(ng, narity+2, (struct goalrec *));
    ng->pred = (struct predicate *)(consp(qp->args[narity]));
    for (k=0; k!=narity; k++) {
      ng->args[k] = qp->args[k];
    }
    ng->next = qp->next;
    qp = ng;
    goto retry;
  } 
  pte = get_pte(traced_pred);
  Spying(info) = pte->spied;
  qp_before = qp->next;
  trace_flag = 1;

  if (!Leaping(info) || Spying(info)) {
    parent = qp;
    if (Enabled(info,Call)) {
      TRACE_ATTACHIO();
      for (;;) {
#ifdef PARALLEL
	trace_printf(info, "%d:%9d CALL: ", my_node, parent_seq);
#else
	trace_printf(info, "%4d CALL: ", parent_seq);
#endif
	print_goal(qp, 0, info);
	if (Leashed(info,Call) || Spying(info)) {
	  if (!call_port_command(qp, get_mte(qp->pred), &info)) break;
	} else {
	  trace_printf(info, "\n");
	  break;
	}
      }
      trace_fflush(info);
    }
  }

  parent_info = info;
  if (trace_flag) heaplimit = 0;
  return traced_pred->func(glbl, qp, allocp, traced_pred);
}

struct goalrec *trace_after(qp, allocp)
     struct goalrec *qp;
     q *allocp;
{
  declare_globals;
  struct goalrec *qt, *before = qp_before;
  unsigned int subgoals, k;
  struct goalrec **bsearch();
  struct enqueue_trace_rec *tr;
  struct trace_info_rec *info;

  InitPort();

  info = parent_info;
  for (qt = qp, k = 0;
       qt != before && k < SUBGOAL_MAX;
       qt = qt->next, k++) {
    int j;
    different_prio_resume[n_resumed] = qt;
    for (j = 0; different_prio_resume[j] != qt; j++)
      ;
    if (j == n_resumed) {
      subgoal_trace[k] = get_pte(qt->pred)->default_trace;
      is_resumed[k] = 0;
      subgoal_seq[k] = trace_seq++;
    } else {
      if (subgoal_trace[k] = Traced(qt)) {
	subgoal_seq[k] = TraceSeq(qt);
      } else {
	subgoal_seq[k] = trace_seq++;
      }
      is_resumed[k] = 1;
    }
  }
  for (tr = trace_enqueued_goals;
       tr != 0 && k < SUBGOAL_MAX;
       tr = tr->next, k++) {
    int j;
    different_prio_resume[n_resumed] = tr->g;
    for (j = 0; different_prio_resume[j] != tr->g; j++)
      ;
    if (j == n_resumed) {
      subgoal_seq[k] = trace_seq++;
      subgoal_trace[k] = 1;
      is_resumed[k] = 0;
    } else {
      if (subgoal_trace[k] = Traced(tr->g)) {
	subgoal_seq[k] = TraceSeq(tr->g);
      } else {
	subgoal_seq[k] = trace_seq++;
      }
      is_resumed[k] = 1;
    }
  }
       
  subgoals = k;
  n_resumed = 0;

  if (Enabled(info, Reduce) &&
      (!Leaping(info) || Spying(info))) {
    TRACE_ATTACHIO();
    for (;;) {
#ifdef PARALLEL
      trace_printf(info, "%d:%9d REDU: ", my_node, parent_seq);
#else
      trace_printf(info, "%4d REDU: ", parent_seq);
#endif
      print_goal(parent, 0, info);
      if (subgoals != 0) {
        trace_printf(info, " :-");
	k = 0;
	for (qt = qp;
	     qt != before;
	     qt = qt->next, k++) {
	  trace_printf(info, "\n%4d%4d%c%c",
		  subgoal_seq[k],
		  k,
		  (k < SUBGOAL_MAX && is_resumed[k] ? '!' : ':'),
		  (k < SUBGOAL_MAX && subgoal_trace[k] ? '+' : '-' ));
	  print_goal(qt, parent->pred->func, info);
	}
	for (tr = trace_enqueued_goals;
	     tr != 0 && k < SUBGOAL_MAX;
	     tr = tr->next, k++) {
          trace_printf(info, "\n%4d%4d%c%c",
		  subgoal_seq[k],
		  k,
		  (k < SUBGOAL_MAX && is_resumed[k] ? '#' : '*'),
		  (k < SUBGOAL_MAX && subgoal_trace[k] ? '+' : '-' ));
	  print_goal(tr->g, parent->pred->func, info);
	  print_priority(tr->prio, info);
	}
      }
      if (Leashed(info,Reduce) || Spying(info)) {
	if (!reduce_port_command(subgoals, qp, get_mte(parent->pred), &info)) {
	  break;
	}
      } else {
        trace_printf(info, "\n");
	trace_fflush(info);
	break;
      }
    }
  }
  TRACE_DETACHIO();
  {
    struct goalrec **qpp;
    for (qpp = &qp, k = 0;
	 *qpp != before;
	 qpp = &(*qpp)->next, k++) {
      if (trace_flag && subgoal_trace[k]) {
	*qpp = trace_goal(*qpp, subgoal_seq[k], info);
      } else {
	(void) untrace_goal(*qpp);
      }
    }
  }
  for (tr = trace_enqueued_goals;
       tr != 0 && k < SUBGOAL_MAX;
       k++) {
    int trace_save = trace_flag;
    struct enqueue_trace_rec *next;
    if (trace_flag && subgoal_trace[k]) {
      tr->g = trace_goal(tr->g, subgoal_seq[k], info);
    } else {
      (void) untrace_goal(tr->g);
    }
    trace_flag = 0;
    enqueue_goal(qp, tr->prio, tr->g, glbl);
    trace_flag = trace_save;
    next = tr->next;
    free(tr);
    tr = next;
  }
  trace_enqueued_goals = 0;
  trace_flag = 0;

  parent = 0;
  heapp = allocp;
  if (interrupt_off) heaplimit = real_heaplimit;
  else heaplimit = 0;
  return qp;
}

struct goalrec *trace_susp(qp, reasonp, spontaneous)
     struct goalrec *qp;
     q *reasonp;
     int spontaneous;
{
  declare_globals;
  unsigned int seq = ( spontaneous ? trace_seq++ : parent_seq );
  struct trace_info_rec *info;

  InitPort();

  info = parent_info;
  if (Enabled(info,Susp) && (!Leaping(info) || Spying(info))) {
    TRACE_ATTACHIO();
    for (;;) {
#ifdef PARALLEL
      trace_printf(info, "%d:%9d SUSP: ", my_node, seq);
#else
      trace_printf(info, "%4d SUSP: ", seq);
#endif
      print_goal(qp, 0, info);
      if (Leashed(info,Susp) || Spying(info)) {
	if (!susp_port_command(qp, get_mte(qp->pred), &info)) break;
      } else {
	trace_printf(info, "\n");
	trace_fflush(info);
	break;
      }
    }
  }
  TRACE_DETACHIO();
  if (trace_flag) {
    qp = trace_goal(qp, seq, info);
  }
  trace_flag = spontaneous;
  return qp;
}

trace_resumption(qp)
     struct goalrec *qp;
{
  /*
    This routine only registers the resumed goal in a table
    rather than actually tracing it.
    Real tracing will be done at the commmit port.
    */
  if (++n_resumed == max_different_prio_resume) {
    max_different_prio_resume *= 2;
    different_prio_resume = (struct goalrec **)
      realloc_check(different_prio_resume,
		    max_different_prio_resume*sizeof(struct goalrec *));
  }
  different_prio_resume[n_resumed-1] = qp;
}

trace_failure(qp)
     struct goalrec *qp;
{
  declare_globals;
  unsigned int seq;
  struct trace_info_rec *info;

  InitTrace();
  InitPort();
  info = DefaultTraceInfo;

  seq = (qp==parent ? parent_seq : trace_seq++);
  if (Enabled(info,Fail)) {
    TRACE_ATTACHIO();
    for (;;) {
#ifdef PARALLEL
      trace_printf(info, "%d:%9d FAIL: ", my_node, seq);
#else
      trace_printf(info, "%4d FAIL: ", seq);
#endif
      print_goal(qp, 0, info);
      if (Leashed(info,Fail) || Spying(info)) {
	if (!fail_port_command(qp, get_mte(qp->pred), &info)) break;
      } else {
	trace_printf(info, "\n");
	trace_fflush(info);
	break;
      }
    }
  }
  TRACE_DETACHIO();
  trace_flag = 0;
}

trace_deadlock(qp)
     struct goalrec *qp;
{
  declare_globals;
  unsigned int seq;
  struct trace_info_rec *info;

  InitTrace();
  InitPort();

  if (!Traced(qp)) {
    qp = trace_goal(qp, trace_seq++, DefaultTraceInfo);
  }

  info = TraceInfo(qp);
  seq = TraceSeq(qp);
  TRACE_ATTACHIO();
  trace_printf(info, "!!! Perpetual Suspention Detected !!!\n");
  if (Enabled(info,Fail)) {
    for (;;) {
#ifdef PARALLEL
      trace_printf(info, "%d:%9d PSUS: ", my_node, seq);
      print_goal(qp, 0, info);
#else
      trace_printf(info, "%4d PSUS: ", seq);
      print_goal(qp, 0, info);
#endif
      if (!fail_port_command(qp, get_mte(qp->pred), &info)) break;
    }
  }
  TRACE_DETACHIO();
}

/****************************************
  Tracer Command Interpreter
****************************************/

enum tracer_command {
  Continue, Leap,		/* Execution Control */
  Skip, QuasiSkip,
  Abort,
  ToggleTrace,
  Unify,			/* PseudoExecution */
  EnablePort, DisablePort, LeashPort, UnLeashPort,
  Trace, NoTrace, Spy, NoSpy, SetDefaultTrace, UnsetDefaultTrace,
  TraceQueuedGoal, NoTraceQueuedGoal, SubTerm,
  PrintDepth, PrintLength,	/* Print Out Control */
  PrintVerbose,
  Window,			/* Trace Window Control */
  ListMod, ListPred, ListSpied,	/* Listing */
  Debugging,			/* Status Query */
  DumpSusp,			/* Dump Suspended Goals */
  DumpQueue,			/* Dump Ready Queue */
  Help, Unknown			/* Miscellaneous */
  };

static char
  command_line[256],
  *clp;

static tracer_error();
static print_help();

#define skip_blanks()  { while (isspace(*clp)) clp++; }

static enum tracer_command trace_parse_command(info)
     struct trace_info_rec *info;
{
  Const struct command_table_entry *cte;
  static char command_name[256];
  static Const struct command_table_entry {
    char *name;
    enum tracer_command command;
  } command_table[] = {
    /* Execution Control */
    { "", Continue },
    { "c", Continue }, { "continue", Continue },
    { "l", Leap }, { "leap", Leap },
    { "s", Skip }, { "skip", Skip },
    { "q", QuasiSkip }, { "quasiskip", QuasiSkip },
    { "a", Abort }, { "abort", Abort },
    /* Pseudo Execution */
    { "u", Unify }, { "unify", Unify },
    /* Port Control */
    { "E", EnablePort }, { "enable", EnablePort },
    { "D", DisablePort }, { "disable", DisablePort },
    { "L", LeashPort }, { "leash", LeashPort },
    { "U", UnLeashPort }, { "unleash", UnLeashPort },
    /* Trace Control */
    { "t", SetDefaultTrace }, { "defaulttrace", SetDefaultTrace },
    { "n", UnsetDefaultTrace }, { "defaultnotrace", UnsetDefaultTrace },
    { "+", Trace }, { "trace", Trace },
    { "-", NoTrace }, { "notrace", NoTrace },
    { "tq", TraceQueuedGoal }, { "tracequeued", TraceQueuedGoal },
    { "nq", NoTraceQueuedGoal }, { "notracequeued", NoTraceQueuedGoal },
    { "^", SubTerm },
    /* Spying */
    { "S", Spy }, { "spy", Spy },
    { "N", NoSpy }, { "nospy", NoSpy },
    /* Print Out Control */
    { "pd", PrintDepth }, { "depth", PrintDepth },
    { "pl", PrintLength }, { "length", PrintLength },
    { "pv", PrintVerbose }, { "verbose", PrintVerbose },
    /* I/O Redirection */
    { "w", Window }, { "window", Window },
    /* Listing */
    { "lm", ListMod }, { "modules", ListMod },
    { "lp", ListPred }, { "predicates", ListPred },
    { "ls", ListSpied }, { "spied", ListSpied },
    /* Status Query */
    { "=", Debugging }, { "debugging", Debugging },
    /* Dump Ready Queue */
    { "Q", DumpQueue }, { "queue", DumpQueue },
    /* Dump Suspended Goals */
    { "W", DumpSusp }, { "waiting", DumpSusp }, { "suspended", DumpSusp },
    /* Miscellaneous */
    { "h", Help }, { "?", Help }, { "help", Help },
    { command_name, Unknown }
  };
  char *cnp = command_name;
#ifdef DIST
/*
  { char eofflag;
    SystemIsEOF(eofflag);
    if (eofflag) return Abort;
  }
*/
  trace_printf(info, "? ");
  SystemFflush();
  SystemRead(command_line);
#else
  if (feof(InStream(info))) return Abort;
  trace_printf(info, "? ");
  trace_fflush(info);
  trace_fgets(command_line, 255, info);
#endif
  clp = command_line;

  skip_blanks();

  if (isdigit(*clp)) {
    return ToggleTrace;
  }
  if (isalpha(*clp)) {
    while (isalpha(*clp)) *cnp++ = *clp++;
  } else if (ispunct(*clp)) {
    while (ispunct(*clp)) *cnp++ = *clp++;
  } else {
    while (*clp != 0 && !isspace(*clp)) *cnp++ = *clp++;
  }
  *cnp = '\0';
  cte = command_table;
  while (strcmp(command_name, cte->name)) cte++;
  return cte->command;
}

static enum trace_port trace_parse_port()
{
  static char port_name[256];
  static struct port_table_entry {
    enum trace_port port;
    char *name;
  } port_table[] = {
    { Call, "c" }, { Call, "call" },
    { Susp, "s" }, { Susp, "susp" }, { Susp, "suspend" },
    { Reduce, "r" }, { Reduce, "redu" }, { Reduce, "reduce" },
    { Fail, "f" }, { Fail, "fail"},
    { AllPorts, "a" }, { AllPorts, "all" },
    { UnknownPort, port_name }
  };
  struct port_table_entry *pte;
  char *pnp = port_name;
  skip_blanks();
  if (*clp == '\0') return NoMorePort;
  while (!isspace(*clp)) *pnp++ = tolower(*clp++);
  *pnp = '\0';
  pte = port_table;
  while (strcmp(port_name, pte->name)) pte++;
  return pte->port;
}

static long trace_parse_int(addr)
     long *addr;
/*
  Returns:
  0: nothing read
  1: integer value read
  -1: error found while parsing
*/
{
  long n;
  if (*clp == '\0') return 0;
  if (!isdigit(*clp)) return -1;
  n = 0;
  do {
    n = 10*n + *clp++ - '0';
  } while (isdigit(*clp));
  *addr = n;
  return 1;
}

static trace_parse_name(np)
     char *np;
{
  if (*clp == '\0') return 0;
  while (isalpha(*clp) || isdigit(*clp) || *clp == '_') {
    *np++ = *clp++;
  }
  *np = '\0';
  return 1;
}

static trace_parse_predicate_name(mod, pred, arity, curmod, info)
     struct mod_table_entry **mod;
     struct pred_table_entry **pred;
     int *arity;
     struct mod_table_entry *curmod;
     struct trace_info_rec *info;
/*
  Returns
  0: nothing read
  1: only module name read ("Module")
  2: module and predicate name read ("Module:Pred")
  3: module and predicate name with arity read ("Module:Pred/Arity")
  -1: error found while parsing
*/
{
  char name[256];
  int k;
  struct mod_table_entry *mte;
  struct pred_table_entry *pte;
  *mod = 0;
  *pred = 0;
  skip_blanks();
  if (!trace_parse_name(name)) {
    return 0;
  }
  if (*clp == ':') {
    for (k = 0, mte = mod_table;
	 k < n_mod;
	 k++, mte++) {
      if (!strcmp((char *)mte->name, (char *)name)) {
	*mod = mte;
	goto mod_found;
      }      
    }
    tracer_error(info, "Unknown module: %s", name);
    return -1;
  mod_found:
    clp++;
    if (isspace(*clp)) return 1;
    if (!trace_parse_name(name)) {
      tracer_error(info, "Predicate name expected after colon");
      return -1;
    }
  } else {
    *mod = mte = curmod;
  }
  for (k = 0, pte = pred_table;
       k < n_pred;
       k++, pte++) {
    if (pte->pred->func == mte->func &&
	!strcmp((char *)pte->name, (char *)name)) {
      *pred = pte;
      goto pred_found;
    }
  }
  tracer_error(info, "Unknown predicate: %s:%s", mte->name, name);
  return -1;
 pred_found:
  if (isspace(*clp)) return 2;
  if (*clp++ != '/') {
    tracer_error(info, "Arity should be specified after slash");
    return -1;
  }
  if (trace_parse_int(arity) != 1) {
    tracer_error(info, "Decimal arity expected after slash");
    return -1;
  }
  for (;
       k < n_pred;
       k++, pte++) {
    if (!strcmp((char *)pte->name, (char *)name) &&
	pte->pred->arity == *arity) {
      *pred = pte;
      goto arity_found;
    }
  }
  tracer_error(info, "Unknown predicate: %s:%s/%d", mte->name, name, *arity);
  return -1;
 arity_found:
  return 3;
}

static q subterm_of_goal(g)
     struct goalrec *g;
{
  Const struct predicate *pred;
  int k;
  q term;

  pred = (Traced(g)
	  ? (struct predicate *)consp(g->args[g->pred->arity - ExtraArgs])
	  : g->pred);
  if (subterm_stack[0] > pred->arity) return 0;
  term = g->args[subterm_stack[0]-1];
  for (k=1; k<subterm_sp; k++) {
    long ind = subterm_stack[k];
  deref:
    if (isref(term)) {
      if (derefone(term) != term &&
	  (!isref(derefone(term)) || derefone(derefone(term)) != term)) {
	term = derefone(term);
	goto deref;
      } else {
	return 0;
      }
    } else if (isfunctor(term)) {
      if (isgobj(term)) return 0;
      if (ind > arityof(functor_of(term))) return 0;
      term = arg(term, ind-1);
    } else if (iscons(term)) {
      if (ind == 1) {
	term = car_of(term);
      } else if (ind == 2) {
	term = cdr_of(term);
      } else return 0;
    } else return 0;
  }
  return term;
}

#define COMMON_COMMANDS\
 Abort: \
 case EnablePort: case DisablePort: \
 case LeashPort: case UnLeashPort: \
 case Spy: case NoSpy: \
 case SetDefaultTrace: case UnsetDefaultTrace: \
 case PrintDepth: case PrintLength: case PrintVerbose: \
 case SubTerm: \
 case ListMod: case ListPred: case ListSpied: \
 case DumpSusp: \
 case Debugging: \
 case Window: \
 case Unknown

static common_port_command(command, pred, curmod, g, info)
     enum tracer_command command;
     struct predicate *pred;
     struct mod_table_entry *curmod;
     struct goalrec *g;
     struct trace_info_rec **info;
{
  enum trace_port port;
  Const struct mod_table_entry *mte;
  struct pred_table_entry *pte;
  int arity;
  int is_spy, set_flag, only_spied;

  only_spied = 0;
  switch (command) {
  case Abort:
    fatal("Execution aborted");
  case EnablePort:
    if ((port = trace_parse_port()) == NoMorePort) {
      print_port_names(*info);
      break;
    }
    do {
      if (port == UnknownPort) {
	print_port_names(*info);
	break;
      } else if (port == AllPorts) {
	EnabledPorts(*info) = -1;
      } else {
	EnabledPorts(*info) |= (1 << (int)port);
      }
    } while ((port = trace_parse_port()) != NoMorePort);
    break;
  case DisablePort:
    if ((port = trace_parse_port()) == NoMorePort) {
      print_port_names(*info);
      break;
    }
    do {
      if (port == UnknownPort) {
	print_port_names(*info);
	break;
      } else if (port == AllPorts) {
	EnabledPorts(*info) = 0;
      } else {
	EnabledPorts(*info) ^= (1 << (int)port);
      }
    } while ((port = trace_parse_port()) != NoMorePort);
    break;
  case LeashPort:
    if ((port = trace_parse_port()) == NoMorePort) {
      print_port_names(*info);
      break;
    }
    do {
      if (port == UnknownPort) {
	print_port_names(*info);
	break;
      } else if (port == AllPorts) {
	LeashedPorts(*info) = -1;
      } else {
	LeashedPorts(*info) |= (1 << (int)port);
      }
    } while ((port = trace_parse_port()) != NoMorePort);
    break;
  case UnLeashPort:
    if ((port = trace_parse_port()) == NoMorePort) {
      print_port_names(*info);
      break;
    }
    do {
      if (port == UnknownPort) {
	print_port_names(*info);
	break;
      } else if (port == AllPorts) {
	LeashedPorts(*info) = 0;
      } else {
	LeashedPorts(*info) ^= (1 << (int)port);
      }
    } while ((port = trace_parse_port()) != NoMorePort);
    break;
#define SpyOrTrace(x) (is_spy ? (x)->spied : (x)->default_trace)    
#define SetSpyOrTrace(x, y) \
    { if (is_spy) (x)->spied = (y); else (x)->default_trace = (y); }
  case SetDefaultTrace:
    is_spy = 0;
    set_flag = 1;
    goto set_reset;
  case UnsetDefaultTrace:
    is_spy = 0;
    set_flag = 0;
    goto set_reset;
  case Spy:
    is_spy = 1;
    set_flag = 1;
    goto set_reset;
  case NoSpy:
    is_spy = 1;
    set_flag = 0;
  set_reset:
    {
      static Const char *set_reset[] = { "reset", "set" };
      static Const char *spy_trace[] = { "Default trace", "Spy point" };
      switch (trace_parse_predicate_name(&mte, &pte, &arity, curmod, *info)) {
      case -1:
	break;
      case 1:
	for (pte = pred_table;
	     pte < pred_table + n_pred;
	     pte++) {
	  if (pte->pred->func == mte->func) {
	    SetSpyOrTrace(pte, set_flag);
	  }
	}
	trace_printf(*info, "\t%s %s on all predicates in module %s\n",
		     spy_trace[is_spy], set_reset[set_flag], mte->name);
	break;
      case 2:
	do {
	  SetSpyOrTrace(pte, set_flag);
	  trace_printf(*info, "\t%s %s on predicate %s:%s/%d\n",
		       spy_trace[is_spy], set_reset[set_flag],
		       mte->name, pte->name, pte->pred->arity);
	  pte++;
	} while (pte < pred_table + n_pred &&
		 !strcmp((char *)(pte-1)->name, (char *)pte->name));
	break;
      case 0:
	pte = get_pte(pred);
	mte = pte->mte;
	arity = pred->arity;
	/* fall through */
      case 3:
	SetSpyOrTrace(pte, set_flag);
	trace_printf(*info, "\t%s %s on predicate %s:%s/%d\n",
		     spy_trace[is_spy], set_reset[set_flag],
		     mte->name, pte->name, pte->pred->arity);
	break;
      }
      break;
    }
  case PrintDepth:
    skip_blanks();
    if (trace_parse_int(&Depth(*info)) != 1) {
      trace_printf(*info, "\tCurrent print depth = %ld\n", Depth(*info));
    }
    break;
  case PrintLength:
    skip_blanks();
    if (trace_parse_int(&Length(*info)) != 1) {
      trace_printf(*info, "\tCurrent print length = %ld\n", Length(*info));
    }
    break;
  case PrintVerbose:
    Verbose(*info) = !Verbose(*info);
    trace_printf(*info,
		 "\tVerbose printing switched %s\n",
		 (Verbose(*info) ? "ON" : "OFF" ));
    break;
  case SubTerm:
    skip_blanks();
    {
      long subterm;
      switch (trace_parse_int(&subterm)) {
      case 1:
	if (subterm_sp == SUBTERM_MAX) {
	  trace_printf(*info, "\tGoing too deep in subterm inspection\n");
	} else if (subterm == 0) {
	  if (subterm_sp > 0) subterm_sp--;
	} else {
	  subterm_stack[subterm_sp++] = subterm;
	  if (subterm_of_goal(g) == 0) {
	    trace_printf(*info, "\tNo element #%ld in the current subterm\n",
			 subterm);
	    subterm_sp--;
	  }
	}
	break;
      case -1:
	subterm_sp = 0;
	break;
      case 0:
	if (subterm_sp != 0) subterm_sp--;
	break;
      }
    }
    break;
  case ListMod:
    print_modules(*info);
    break;
  case ListSpied:
    only_spied = 1;
  case ListPred:
    skip_blanks();
    switch (trace_parse_predicate_name(&mte, &pte, &arity, curmod, *info)) {
    case 0: print_predicates(0, 0, -1, only_spied, *info); break;
    case 1: print_predicates(mte, 0, -1, only_spied, *info); break;
    case 2: print_predicates(mte, pte, -1, only_spied, *info); break;
    case 3: print_predicates(mte, pte, arity, only_spied, *info); break;
    }
    break;
  case DumpSusp:
    print_suspended_goals(*info);
    break;
  case Debugging:
    print_debug_status(*info);
    break;
#ifndef DIST
#ifdef XTERM
  case Window:
    skip_blanks();
    {
      long window_id;
      long old_id = *info-trace_info_recs;
      struct trace_info_rec *newinfo;

      switch (trace_parse_int(&window_id)) {
      case -1:
	tracer_error(*info, "Invalid argument");
	goto error;
      case 0: {
	window_id = num_info_recs;
	if (info_rec_size == num_info_recs) {
	  int k;
	  struct trace_info_rec *new_info_recs =
	    (struct trace_info_rec *)
	      malloc_check(sizeof(struct trace_info_rec)*info_rec_size*2);
	  for (k=0; k<info_rec_size; k++) {
	    new_info_recs[k] = trace_info_recs[k];
	  }
	  info_rec_size *= 2;
	  trace_info_recs = new_info_recs;
	}
	num_info_recs++;
	newinfo = &trace_info_recs[window_id];
	*newinfo = **info;
	open_window(&newinfo->in, &newinfo->out, window_id);
	break;
      }
      case 1:
	if (window_id >= num_info_recs) {
	  tracer_error(*info, "Non-existent window id");
	  trace_printf(*info,
		       "\tThere are only %ld windows\n",
		       num_info_recs);
	  goto error;
	} else {
	  newinfo = &trace_info_recs[window_id];
	}
	break;
      }
      trace_printf(*info, "\t===> Window %ld\n", window_id);
      trace_fflush(*info);
      *info = newinfo;
      trace_printf(*info, "\tWindow %ld ===>\n", old_id);
    }
  error:
    break;
#endif
#endif
  case Unknown:
    tracer_error(*info, "Unknown command");
    trace_printf(*info, "\tTry ? for command list\n");
    break;
  }
}

#define CONTROL_COMMANDS\
 Continue: case Leap: case Skip

static control_command(command, info)
     enum tracer_command command;
     struct trace_info_rec *info;
{
  switch(command) {
  case Continue:
    Leaping(info) = 0;
    break;
  case Leap:
    Leaping(info) = 1;
    TRACE_DETACHIO();
    break;
  case Skip:
    trace_flag = 0;
    TRACE_DETACHIO();
    break;
  }
}

static Const char *common_help[] = {
 "E: enable port;   D: disable port;  L: leash port;    U: unleash port;",
 "S <pred>: spy;    N <pred>: nospy;  t <pred>: trace;  n <pred>: notrace;",
 "pd: print depth;  pl: print length; pv: toggle verbose print;",
 "^ <n>: subterm;   ^ 0: superterm;   ^: no subterm;",
 "Q: ready queue;   W: suspended (waiting) goals;",
 "=: debug status;  ls: list spied;   lm: list modules; lp: list predicates;",
 "a: abort execution",
 0 };

static call_port_command(qp, curmod, info)
     struct goalrec *qp;
     struct mod_table_entry *curmod;
     struct trace_info_rec **info;
{
  int again = 0;
  int err = 0;
  enum tracer_command command;

  switch (command = trace_parse_command(*info)) {
  case COMMON_COMMANDS:
    common_port_command(command, parent->pred, curmod, parent, info);
    again = 1;
    break;
  case CONTROL_COMMANDS:
    control_command(command, *info);
    break;
  case DumpQueue:
    print_ready_queue(qp, 0, *info);
    again = 1;
    break;
  default:
    err = 1;
  }    
  if (err) {
    static Const char *call_help[] = {
      "\t*** Commands available at CALL ports ***",
      "<cr>: continue;   c: continue;      s: skip;          l: leap;",
      0
    };
    print_help(call_help, *info);
    print_help(common_help, *info);
    again = 1;
  }
  return again;
}

static reduce_port_command(subgoals, qp, curmod, info)
     unsigned long subgoals;
     struct goalrec *qp;
     struct mod_table_entry *curmod;
     struct trace_info_rec **info;
{
  int k, flag;
  enum tracer_command command;
  int again = 0;
  int err = 0;
  switch (command = trace_parse_command(*info)) {
  case ToggleTrace:
    flag = -1; goto trace_notrace;
  case Trace:
    flag = 1; goto trace_notrace;
  case NoTrace:
    flag = 0;
  trace_notrace:
    if (subgoals == 0) {
      tracer_error(*info, "No subgoals to trace");
    } else if (subgoals >= SUBGOAL_MAX) {
      tracer_error(*info, "Too many subgoals to control");
    } else {
      skip_blanks();
      if (*clp == '\0') {
	for (k = 0; k < subgoals; k++) {
	  subgoal_trace[k] = flag;
	}
      } else {
	long traced;
	while (1) {
	  switch (trace_parse_int(&traced)) {
	  case 0:
	    goto exit_loop;
	  case 1:
	    if (0 <= traced && traced < subgoals) {
	      subgoal_trace[traced] =
		( flag >= 0 ? flag : !subgoal_trace[traced] );
	    } else {
	      tracer_error(*info, "Subgoal %ld doesn't exist", traced);
	    }
	    skip_blanks();
	    break;
	  default:
	    tracer_error(*info, "Subgoal number expected");
	    goto exit_loop;
	  }
	}
      }
    }
  exit_loop:
    again = 1;
    break;
  case CONTROL_COMMANDS:
    control_command(command, *info);
    break;
  case COMMON_COMMANDS:
    common_port_command(command, parent->pred, curmod, parent, info);
    again = 1;
    break;
  case DumpQueue:
    print_ready_queue(qp, 0, *info);
    again = 1;
    break;
  default:
    err = 1;
    break;
  }    
  if (err) {
    static Const char *reduce_help[] = {
      "\t*** Commands available at REDUCE ports ***",
      "<cr>: continue;   c: continue;      s: skip;          l: leap;",
      "+ <n>: trace;     - <n>: no trace;  <n>: toggle trace",
      0
    };
    print_help(reduce_help, *info);
    print_help(common_help, *info);
    again = 1;
  }
  return again;
}

static susp_port_command(qp, curmod, info)
     struct goalrec *qp;
     struct mod_table_entry *curmod;
     struct trace_info_rec **info;
{
  enum tracer_command command;
  int again = 0;
  int err = 0;
  switch (command = trace_parse_command(*info)) {
  case CONTROL_COMMANDS:
    control_command(command, *info);
    break;
  case COMMON_COMMANDS:
    common_port_command(command, qp->pred, curmod, qp, info);
    again = 1;
    break;
  default:
    err = 1;
    break;
  }    
  if (err) {
    static Const char *susp_help[] = {
      "\t*** Commands available at SUSPENSION ports ***",
      "<cr>: continue;   c: continue;      s: skip;          l: leap;",
      0
    };
    print_help(susp_help, *info);
    print_help(common_help, *info);
    again = 1;
  }
  return again;
}

static fail_port_command(qp, curmod, info)
     struct goalrec *qp;
     struct mod_table_entry *curmod;
     struct trace_info_rec **info;
{
  enum tracer_command command;
  int again = 0;
  int err = 0;

  switch (command = trace_parse_command(*info)) {
  case COMMON_COMMANDS:
    common_port_command(command, qp->pred, curmod, qp, info);
    again = 1;
    break;
  case DumpQueue:
    print_ready_queue(qp, 0, *info);
    again = 1;
    break;
  default:
    err = 1;
    break;
  }    
  if (err) {
    static Const char *fail_help[] = {
      "\t*** Commands available at FAIL ports ***",
      0
    };
    print_help(fail_help, *info);
    print_help(common_help, *info);
    again = 1;
  }
  return again;
}

static print_port_status(info)
     struct trace_info_rec *info;
{
  static char plusminus[] = { '-', '+' };
  trace_printf(info, "\t   port: Call Susp Redu Fail\n");
  trace_printf(info, "\tenabled:  %c    %c    %c    %c\n",
	       plusminus[Enabled(info,Call)],
	       plusminus[Enabled(info,Susp)],
	       plusminus[Enabled(info,Reduce)],
	       plusminus[Enabled(info,Fail)]);
  trace_printf(info, "\tleashed:  %c    %c    %c    %c\n",
	       plusminus[Leashed(info,Call)],
	       plusminus[Leashed(info,Susp)],
	       plusminus[Leashed(info,Reduce)],
	       plusminus[Leashed(info,Fail)]);
}

static print_debug_status(info)
     struct trace_info_rec *info;
{
  print_port_status(info);
  trace_printf(info,
	       "\tprint %s; depth = %u; length = %u\n",
	       (Verbose(info) ? "verbose" : "terse"),
	       Depth(info), Length(info));
}

static print_port_names(info)
     struct trace_info_rec *info;
{
  static Const char *port_help[] = {
    "Available ports are \"call\", \"susp\", \"redu\" and \"fail\".",
    "You can also specify \"all\".",
    0 };
  print_help(port_help, info);
}

/****************************************
  Printing Out
****************************************/

static tracer_error(info, s, a0, a1, a2, a3, a4, a5, a6, a7, a8)
     struct trace_info_rec *info;
     char *s;
     long a0, a1, a2, a3, a4, a5, a6, a7, a8;
{
  trace_printf(info, "\t!!! ");
  trace_printf(info, s, a0, a1, a2, a3, a4, a5, a6, a7, a8);
  trace_printf(info, " !!!\n");
}

static print_help(messages, info)
     Const char **messages;
     struct trace_info_rec *info;
{
  while (*messages) {
    trace_printf(info, "    %s\n", *messages++);
  }
}

static trace_print(out, x, depth, length)
     FILE *out;
     q x;
     unsigned long depth, length;
{
  fprint_partially(out, x, depth, length);
}

/* #ifdef DIST */
#if 0
io_print_goal(g, mod, depth, length, verbose)
     struct goalrec *g;
     module mod;
     unsigned long depth, length;
     int verbose;
{
  char *tracetmpname = tmpnam((char *)NULL);
  FILE *tracetmp = fopen(tracetmpname,"w");
  char io_buf[1024];
  extern int verbose_print;

  fprint_goal(tracetmp, g, mod, depth, length, verbose);
  freopen(tracetmpname,"r",tracetmp);
  {
    long i;
    do {
      for (i = 0; i < sizeof(io_buf)-1; i++) {
	int ch = klic_getc(tracetmp);
	if (ch == EOF)
	  break;
	io_buf[i] = ch;
      }
      io_buf[i] = '\0';
      trace_printf(tracetmp, "%s", io_buf, depth, length);
    } while (i == sizeof(io_buf)-1);
  }
  fclose(tracetmp);
  unlink(tracetmpname);
}
#endif

trace_fflush(info)
     struct trace_info_rec *info;
{
  klic_fflush(OutStream(info));
}

trace_printf(info, format, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab)
     struct trace_info_rec *info;
     char *format;
     long a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab;
{
  klic_fprintf(OutStream(info), format,
		a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab);
}

trace_fgets(buf, n, info)
     char *buf;
     int n;
     struct trace_info_rec *info;
{
  klic_fgets(buf, n, InStream(info));
}

print_priority(prio, info)
     long prio;
     struct trace_info_rec *info;
{
  declare_globals;
  if (prio < current_prio) {
    trace_printf(info, "@prio(-%ld)", current_prio-prio);
  } else if (prio > current_prio) {
    trace_printf(info, "@prio(+%ld)", prio-current_prio);
  }
}

print_goal(g, mod, info)
     struct goalrec *g;
     module mod;
     struct trace_info_rec *info;
{
/* #ifdef DIST */
#if 0
  io_print_goal(g, mod, info, Depth(info), Length(info), Verbose(info));
#else
  fprint_goal(OutStream(info), g, mod,
	      Depth(info), Length(info), Verbose(info));
#endif
}

fprint_goal(stream, g, mod, depth, length, verbose)
     FILE *stream;
     struct goalrec *g;
     module mod;
     unsigned long depth, length;
     int verbose;
{
  extern module wait_prio_routine();
  extern int verbose_print;
  q term;

  verbose_print = verbose;
  if (subterm_sp != 0 && (term = subterm_of_goal(g)) != 0) {
    int k;
    for (k=0; k<subterm_sp; k++) {
      klic_fprintf(stream, "^%ld", subterm_stack[k]);
    }
    klic_fprintf(stream, " ");
    trace_print(stream, term, depth, length);
  } else {
    Const struct predicate *pred;
    struct pred_table_entry *pte;
    int traced;

  retry:
    traced = Traced(g);
    pred = (traced
	    ? (struct predicate *) consp(g->args[g->pred->arity-ExtraArgs])
	    : g->pred);
    if (pred->func == wait_prio_routine) {
      declare_globals;
      struct goalrec *ng;
      int narity = g->pred->arity - ExtraArgs, k;
      heapalloc(ng, narity+2, (struct goalrec *));
      ng->pred = (struct predicate *)(consp(g->args[narity]));
      for (k=0; k!=narity; k++) {
	ng->args[k] = g->args[k];
      }
      g = ng;
      goto retry;
    } 
    /* 
       Node() pragma is valid in the sequentiol implementation.
       Thus, these processing must be done in the seq. impl..
       */
    else if (pred->func == wait_penum_routine) {
      declare_globals;
      struct goalrec *ng;
      int narity = g->pred->arity - 2, k;
      heapalloc(ng, narity+2, (struct goalrec *));
      ng->pred = (struct predicate *)(consp(g->args[narity]));
      for (k=0; k!=narity; k++) {
	ng->args[k] = g->args[k];
      }
      g = ng;
      goto retry;
    }
    pte = get_pte(pred);
    if ((module)pred->func != mod) {
      klic_fprintf(stream, "%s:", (pte && pte->mte && pte->mte->name ? 
				   (char *)pte->mte->name : "???"));
    }
    klic_fprintf(stream, "%s", (pte ? (char *)pte->name : "???"));
    if (pred->arity != 0) {
      unsigned int k;
      klic_fprintf(stream, "(");
      trace_print(stream, g->args[0], depth, length);
      for (k = 1;
	   k < pred->arity;
	   k++) {
	klic_fprintf(stream, ",");
	trace_print(stream, g->args[k], depth, length);
      }
      klic_fprintf(stream, ")");
    }
  }
}

static print_modules(info)
     struct trace_info_rec *info;
{
  int k;

  trace_printf(info, "\t*** Module List ***\n");
  for (k = 0; k < n_mod; k++) {
    struct mod_table_entry *mte = &mod_table[k];
    trace_printf(info,
		 "\t%8X %s\n",
		 (unsigned long)mte->func,
		 mte->name);
  }
  trace_printf(info, "\t*******************\n");
}

static print_predicates(mte, pte, arity, spied_only, info)
     struct mod_table_entry *mte;
     struct pred_table_entry *pte;
     int arity;
     int spied_only;
     struct trace_info_rec *info;
{
  int k;
  int none = 1;
  trace_printf(info,
	       (spied_only ? "\t*** Spied Predicates ***\n"
		: "\t**** Predicate List ****\n"));
  for (k = 0; k < n_pred; k++) {
    struct pred_table_entry *x = &pred_table[k];
    if ((!spied_only || x->spied) &&
	(!mte || x->mte == mte) &&
	(!pte || !strcmp((char *)x->name, (char *)pte->name)) &&
	(arity < 0 || x->pred->arity == arity)) {
      none = 0;
      trace_printf(info,
		   "\t%8X %c%c%s:%s/%d\n",
		   (unsigned long)x->pred,
		   (x->spied && !spied_only ? '*' : ' '),
		   (x->default_trace ? '+' : '-'),
		   x->mte->name,
		   x->name, x->pred->arity);
    }
  }
  trace_printf(info,
	       (none ? "\t********* NONE *********\n" :
		"\t************************\n"));
}

static long dump_one_goal(qp, prio, dump_seq, info)
     struct goalrec *qp;
     long prio, dump_seq;
     struct trace_info_rec *info;
{
  if (Traced(qp)) {
    trace_printf(info, "%6d* ", TraceSeq(qp));
  } else {
    trace_printf(info, "%6d  ", dump_seq++);
  }
  print_goal(qp, 0, info);
  print_priority(prio, info);
  trace_printf(info, "\n");
  return dump_seq;
}

static long dump_one_queue(prio, qp, dump_seq, info)
     long prio;
     struct goalrec *qp;
     long dump_seq;
     struct trace_info_rec *info;
{
  extern Const struct predicate queue_empty_pred;
  extern Const struct predicate topsucceed_pred;
  if (qp->pred != &queue_empty_pred &&
      qp->pred != &topsucceed_pred) {
    do {
      dump_seq = dump_one_goal(qp, prio, dump_seq, info);
      qp = qp->next;
      if (qp==0) {
	trace_printf(info,
		     "!!! The rest of the queue of this priority unknown !!!\n");
	break;
      }
    } while (qp->pred != &queue_empty_pred &&
	     qp->pred != &topsucceed_pred);
  }
  return dump_seq;
}

static void
print_ready_queue(qp, dump_seq, info)
     struct goalrec *qp;
     long dump_seq;
     struct trace_info_rec *info;
{
  declare_globals;
  struct prioqrec *pq = prioq.next;
  dump_seq = dump_one_queue(current_prio, qp, dump_seq, info);
  while (pq->prio >= 0) {
    dump_seq = dump_one_queue(pq->prio, pq->q, dump_seq, info);
    pq = pq->next;
  }
}

static void
print_suspended_goals(info)
     struct trace_info_rec *info;
{
  declare_globals;
  struct suspended_goal_rec *sm = suspended_goal_list;
  long dump_seq = 0;
  if (sm == 0) {
    trace_printf(info, "\t*** No suspended goals ***\n");
  } else {
    do {
      if (isint(sm->goal->next)) {
	dump_seq =
	  dump_one_goal(sm->goal, intval(sm->goal->next), dump_seq, info);
      }
      sm = sm->next;
    } while (sm != 0);
  }
}

void
fprint_pred(fp, pred)
     FILE *fp;
     struct predicate *pred;
{
  static int name_table_init = 0;
  if(!name_table_init) {
    make_name_tables();
    name_table_init = 1;
  }
  {
    struct pred_table_entry *pte = get_pte(pred);
    klic_fprintf(fp, "%s:%s/%d",(pte ? (char *)pte->mte->name : "???"),
		 (pte ? (char*)pte->name : "???"), pred->pred);
  }
}
