/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
/* PVM dependent inter-pe communication library */
/* $Id: distproc.h,v 1.1.1.1 2000/07/28 02:01:42 igarashi Exp $ */
#include <pvm3.h>

/* Definition for command options for the distributed system */

#ifdef MAIN
/* following variables are set by parse_arg(),
   and are used only in klic_main() */
static int eager_transfer_all_flag;
#endif

Extern int useconfig_flag;

#if defined(USETIMER) && !defined(USE_REDUCTION_COUNT)
Extern unsigned long interval_usec;
Extern unsigned int noitimer;
#define TIMER_OPTIONS							\
    "noitimer",	(union all_type *)&noitimer, ARG_SET,			\
    "-noitimer: disable itimer interrupt",				\
									\
    "I",	(union all_type *)&interval_usec, ARG_SIZE,		\
    "-I <timer value>: specify interrupt timer interval",

#else
/* If USETIMER is not defined USE_REDUCTION_COUNT is forcedly defined */
#if !defined(USE_REDUCTION_COUNT)
#define USE_REDUCTION_COUNT
#endif  /* defined(DIST) && !defined(USE_REDUCTION_COUNT) */
#endif	/* USETIMER */
Extern int no_sendsig;

/* Solaris. Bind a process to a specific processor */
#ifdef USEPROCBIND
Extern long no_procbind Init(0); 	/* flag for disabling procbind() */
#define PROCBIND_OPTION							\
    "noprocbind",(union all_type *)&no_procbind, ARG_SET,		\
    "-noprocbind: disable procbind",
#endif /* USEPROCBIND */

  /* The option name must be mached to the argument of child invocation.
     So, we define it as a macro */
#define OPT_PARENT_TID   "=parent_tid="
Extern unsigned long parent_tid;/* the value used in child_spawn() */
Extern int rel_spawn_path; 	/* relative spawn path flag */

#ifdef USE_REDUCTION_COUNT
#define DEFAULT_POLLING_INTERVAL 300
#define SHOEN_POLLING_INTERVAL 300
Extern int polling_interval Init(DEFAULT_POLLING_INTERVAL);
#endif

#ifndef PROCBIND_OPTION
#define PROCBIND_OPTION
#endif
#ifndef TIMER_OPTIONS
#define TIMER_OPTIONS
#endif

#define USER_OPTIONS							\
    OPT_PARENT_TID,	(union all_type *)&parent_tid, ARG_SIZE, 0,	\
									\
    "rmon",	(union all_type *)&rmonnode,  ARG_STRING, 		\
    "-rmon <rmonitor node>: runtime monitor node name", 		\
									\
    "relsp",	(union all_type *)&rel_spawn_path, ARG_SET,		\
    "-relsp: spawn using relative path",				\
                                                                        \
    "msgbw",	(union all_type *)&msg_busywait_flag, ARG_SET,		\
    "-msgbw: busy waiting message arrival in idle loop",        	\
                                                                        \
    "e",	(union all_type *)&eager_transfer_all_flag, ARG_SET,	\
    "-e: transfer all level",						\
									\
    "E",	(union all_type *)&eager_transfer_level, ARG_SIZE,	\
    "-E <level>: specify transfer level",				\
                                                                        \
    "useconfig", (union all_type *)&useconfig_flag, ARG_SET,            \
    "-useconfig:  preliminary option, don't use!",                      \
									\
    PROCBIND_OPTION							\
									\
    TIMER_OPTIONS							\
									\
    "n",	(union all_type *)&network_statistics_flag, ARG_SET,	\
    "-n: gather network statistics",					\
									\
    "S",	(union all_type *)&no_sendsig, ARG_SET,			\
    "-S: disable sendsig",						\

#ifdef USE_REDUCTION_COUNT
#define POLLING_OPTIONS \
    "I",   (union all_type *)&polling_interval, ARG_SIZE,           \
    "-I <usec>: polling interval for checking arrival messages",    \

#endif

struct tid_table {
    int child_count;
    int io;
    int *child;
};

Extern struct tid_table tid_table;
Extern int io_node_flag;

#define Tid_of(X)	(tid_table.child[X])
#define Tid_of_io()	(tid_table.io)

#define Close_net()	close_network()

#define Node_type()     ( io_node_flag == 0 )
