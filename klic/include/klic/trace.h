/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */

/*
  Tracer Status Information
*/

struct trace_info_rec {
  FILE *in;
  FILE *out;
  q original_arg;
  int enabled, leashed, leaping, spying;
  int verbose;
  unsigned long print_depth, print_length;
};

#define Leaping(info)		((info)->leaping)
#define Spying(info)		((info)->spying)
#define EnabledPorts(info)	((info)->enabled)
#define LeashedPorts(info)	((info)->leashed)
#define Enabled(info,port)	((EnabledPorts(info) >> (int)(port)) & 1)
#define Leashed(info,port)	((LeashedPorts(info) >> (int)(port)) & 1)
#define Verbose(info)		((info)->verbose)
#define Depth(info)		((info)->print_depth)
#define Length(info)		((info)->print_length)

#define InStream(info)		((info)->in)
#define OutStream(info)		((info)->out)

/*
  Info on Enqueued Goals
*/

struct enqueue_trace_rec {
  struct enqueue_trace_rec *next;
  struct goalrec *g;
  long prio;
};

/*
  Name Tables
*/

struct mod_table_entry {
  module (*func)();
  Const unsigned char *name;
};

struct pred_table_entry {
  Const struct predicate *pred;
  Const struct mod_table_entry *mte;
  Const unsigned char *name;
  char spied;
  char default_trace;
};

/*
  Trace Control
*/

enum trace_port {
  Call, Susp, Reduce, Fail, AllPorts, NoMorePort, UnknownPort };
