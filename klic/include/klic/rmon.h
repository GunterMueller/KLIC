/* ---------------------------------------------------------- 
%   (C) 1995 Institute for New Generation Computer Technology 
%	(Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
---------------------------------------------------------- */

/************* For profiling ************************/

#define PROF_GATHER_INTERVAL_SEC 2
#define PROF_SAMPLE_INTERVAL_uSEC 10111

#define SHOEN_START_TAG       0
#define SHOEN_OUTPUT_PROF_TAG 100
#define SHOEN_TERMINATE_TAG   200

struct perfmon_state {
    int idle_on;
    int gc_on;
    int ms_on;
    int mr_on;
};
Extern Volatile struct perfmon_state perfmon_state;
struct perfmon_counter {
    long intr_count;
    long idle_count;
    long gc_count;
    long ms_count;
    long mr_count;
};
Extern Volatile struct perfmon_counter perfmon_counter;

Extern char *rmonnode;
Extern int rmon_tid;

#define IDLE_ON()  perfmon_state.idle_on = 1
#define IDLE_OFF() perfmon_state.idle_on = 0
#define GC_ON()    perfmon_state.gc_on = 1
#define GC_OFF()   perfmon_state.gc_on = 0
#define SEND_ON()  perfmon_state.ms_on = 1
#define SEND_OFF() perfmon_state.ms_on = 0
#define RECV_ON()  perfmon_state.mr_on = 1
#define RECV_OFF() perfmon_state.mr_on = 0

Extern int gather_prof_ready Init(1);
struct profile_rec {
    long stimes;  /* sampling times */
    long itimes;  /* idle times */
    long gctimes; /* GC times */
    long mstimes; /* message sending times */
    long mrtimes; /* message receiving times */
};

Extern struct profile_rec *shoen_profile;
Extern int gathered_count Init(0);

Extern long *send_pbuf;
