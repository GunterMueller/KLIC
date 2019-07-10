/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

union all_type {
    int i;
    long l;
    double d;
    char *s;
};
enum argtype {
    ARG_DOUBLE,
    ARG_SIZE,
    ARG_RESET,
    ARG_SET,
    ARG_STRING,
    ARG_NOT_USED
};

char *parse_opts();

/* not copied when spawned */
#define ARG_SHOULD_NOT_COPY(X)	((int)(X) >= (int)ARG_NOT_USED)
#define ARG_NOT_COPIED(X) \
  (enum argtype)((int)(X) + (int)ARG_NOT_USED)

#define ARG_ORIGINAL_TYPE(X) \
  (enum argtype)(ARG_SHOULD_NOT_COPY(X)?(int)(X)-(int)ARG_NOT_USED:(int)(X))

struct opttable {
    char *opt_str;
    union all_type *opt_dest;
    enum argtype opt_type;
    char *opt_usage;
};

#ifndef MAIN
extern Const struct opttable opttable[];
#else  /* MAIN */
static long num_pes;
static double max_a_ratio;
static unsigned long h_size;
static unsigned long max_h_size;
static unsigned long inc_size;
static int verbose;
extern int start_tracing;
extern int count_suspension;
#ifdef USETIMER
static unsigned long dl_det_interval;
#endif /*USETIMER*/
Const struct opttable opttable[] = {
#ifdef USER_OPTIONS
    USER_OPTIONS
#endif
#ifdef POLLING_OPTIONS
    POLLING_OPTIONS
#endif
#ifdef PARALLEL
    "p",        (union all_type *)&num_pes, ARG_SIZE,
    "-p <# of workers>: number of worker processes ",
#endif
#ifdef DIST_OPTIONS
    DIST_OPTIONS
#endif
#ifdef DIST
#ifdef SHM_DIST
    "B",	(union all_type *)&shared_buffer_size, ARG_SIZE,
    "-B <communication buffer size>",
#endif
#endif
#ifdef SHM
    "S",        (union all_type *)&SHM_SIZE, ARG_SIZE,
    "-S <shared-memory heap size>: shared-memory heap size, never expand",

    "D",        (union all_type *)&Disp_child, ARG_SET,
    "-D: display child process number on UNIX",

#endif
    "a",	(union all_type *)&max_a_ratio, ARG_DOUBLE,
    "-a <ratio>: active cell ratio to triger heap extension\n\t  <ratio> should be a floating point number >0 and <1",
    "h",	(union all_type *)&h_size, ARG_SIZE,
    "-h <initial heap size>: initial size of heap",

    "H",	(union all_type *)&max_h_size, ARG_SIZE,
    "-H <max heap size>: maximum possible heap size",

    "i",	(union all_type *)&inc_size, ARG_SIZE,
    "-i <increment>: gap between heap top and bottom\n\t  for -h, -H and -i, 32k means 32KW and 2m means 2MW",

    "g",	(union all_type *)&measure_gc, ARG_SET,
    "-g: set garbage collection measurement on",

    "v",	(union all_type *)&verbose, ARG_SET,
    "-v: display KLIC runtime version",
#ifdef DEBUGLIB
    "t",	(union all_type *)&start_tracing, ARG_SET,
    "-t: start execution with tracing",

    "s",	(union all_type *)&count_suspension, ARG_SET,
    "-s: count suspension",
#endif /* DEBUGLIB */
#ifdef USETIMER
    "deadlock-detection-interval",
		(union all_type *)&dl_det_interval, ARG_SIZE,
    "-deadlock-detection-interval <interval>:\n\t  specify deadlock detect interval in micro sec.\n\t  Set <interval> to 0 to supress early deadlock detection completely.",
#endif /*USETIMER*/
    0, 0,
};
#endif /* MAIN */
