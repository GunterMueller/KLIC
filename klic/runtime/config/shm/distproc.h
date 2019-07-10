/* ----------------------------------------------------------
%   (C)1994,1995 Institute for New Generation Computer Technology 
%   (C)1997      Advanced Information Technology Research Center
%       (Read COPYRIGHT for detailed information.) 
----------------------------------------------------------- */

#ifndef _KLIC_DISTPROC_H_
#define _KLIC_DISTPROC_H_

/* Definition for command options for the distributed system */

#ifdef MAIN
/* following variables are set by parse_arg(),
   and are used only in klic_main() */
static int eager_transfer_all_flag;
#endif

#ifdef USETIMER
Extern unsigned long interval_usec;
Extern unsigned int noitimer;
#define TIMER_OPTIONS							\
    "noitimer",	(union all_type *)&noitimer, ARG_SET,			\
    "-noitimer: disable itimer interrupt",				\
									\
    "I",	(union all_type *)&interval_usec, ARG_SIZE,		\
    "-I <timer value>: specify interrupt timer interval",
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
    PROCBIND_OPTION							\
									\
    TIMER_OPTIONS							\
									\
    "n",	(union all_type *)&network_statistics_flag, ARG_SET,	\
    "-n: gather network statistics",					\
									\
    "S",	(union all_type *)&no_sendsig, ARG_SET,			\
    "-S: disable sendsig",						\

struct tid_table {
    int child_count;
    int io;
    int *child;
};

#define Tid_of(X)	(tid_table.child[X])
#define Tid_of_io()	(tid_table.io)

#define Close_net()	close_network()

Extern int io_node_flag;

#define Node_type()     ( io_node_flag == 0 )

#define spawn_children(n, t, p, c, v)  r_spawn_children(n,t,p,c,v); glbl = My_glbl;

#ifndef MAP_FAILED
#define MAP_FAILED (-1)
#endif
#ifndef MAP_FILE
#define MAP_FILE 0
#endif
#ifndef MAP_VARIABLE
#define MAP_VARIABLE 0
#endif

#endif /* _KLIC_DISTPROC_H_ */
