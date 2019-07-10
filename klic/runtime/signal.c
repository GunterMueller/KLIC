/* ---------------------------------------------------------- 
%   (C)1994 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#define SIGMAIN
#include <klic/basic.h>
#ifdef USESIG
#include <klic/struct.h>
#include <klic/sighndl.h>

/****************************************
  GENERAL SIGNAL HANDLING UTILITY
****************************************/

/* Asynchronous signal handler */

static void handle_signal(sig, code, scp, addr)
     int sig, code;
     struct sigcontext *scp;
     char *addr;
{
  declare_globals;
  interrupt_off = 0;
  heaplimit = 0;
  signal_done = 0;
  signal_flags[sig] = 1;
}

/* Synchronized signal handler called between reductions */

q* klic_signal_handler(allocp)
     q *allocp;
{
  declare_globals;
  int sig;

  do {
    signal_done = 1;
    for (sig = 0; sig < NSIG; sig++) {
      if (signal_flags[sig]) {
	int again;
	signal_flags[sig] = 0;
	again = signal_handlers[sig](allocp, sig);
	allocp = heapp;
	if (again) {
	  signal_done = 0;
	  signal_flags[sig] = 1;
	  interrupt_off = 0;
	  return allocp;
	}
      }
    }
    interrupt_off = -1;
    heaplimit = real_heaplimit;
    for (sig = 0; sig < NSIG; sig++) {
      if (signal_flags[sig]) {
	signal_done = 0;
	break;
      }
    }
  } while (!signal_done);
  return allocp;
}

/* Utility subroutine to add signal handlers */

void add_slit_check_handler(sig, func)
     int sig;
     int (*func)();
{
  if (sig >= NSIG) {
    fatalf("Illegal signal number %d", sig);
  }
  if (signal_handlers[sig] != 0) {
    fatalf("Trying to redefine a signal handler for signal %d", sig);
  }
  signal_handlers[sig] = func;
}

add_signal_handler(sig, func)
     int sig;
     int (*func)();
{
  struct sigaction new;
  add_slit_check_handler(sig, func);
  new.sa_handler = handle_signal;
  sigemptyset(&new.sa_mask);
#ifdef SA_RESTART
  new.sa_flags = SA_RESTART;
#else
  new.sa_flags = 0;
#endif
  if ((long)sigaction(sig, &new, 0) == -1L) {
    fatalf("Can't set up signal handler for signal %d", sig);
  }
  return sig;
}

/* Initiation */

static void init_general_signal_handling()
{
  int sig;
  extern q *gc_for_signal_handler();
  for (sig = 0; sig < NSIG; sig++) {
    signal_flags[sig] = 0;
    signal_handlers[sig] = 0;
  }
}

/****************************************
  DEFAULT SIGINT (CONTROL-C) HANDLER
****************************************/

static int default_sigint_handler(allocp, sig)
     q *allocp;
     int sig;
{
  declare_globals;
  extern int sigint_interrupt;
  sigint_interrupt = 1;
  return 0;
}

/****************************************
  STREAMED SIGNAL HANDLING UTILITY
****************************************/

static q signal_streams[NSIG];

/* Garbage collection hook */

static q* gc_signal_streams(allocp, ntop, otop, nsize, osize)
     q *allocp, *ntop, *otop;
     unsigned int nsize, osize;
{
  int sig;
  extern q *copy_one_term();
  for (sig = 0; sig < NSIG; sig++) {
    if (signal_streams[sig] != 0) {
      allocp =
	copy_one_term(&signal_streams[sig],
		      allocp, ntop, otop, nsize, osize);
    }
  }
  return allocp;
}

int streamed_signal_handler(allocp, sig)
     q *allocp;
     int sig;
{
  declare_globals;
  extern q *do_unify_value();
  q newcons = makecons(allocp);
  q newvar = allocp[0] = makeref(&allocp[0]);
  allocp[1] = makeint(sig);
  allocp += 2;
  allocp = do_unify_value(allocp, signal_streams[sig], newcons);
  signal_streams[sig] = newvar;
  heapp = allocp;
  return 0;
}

q *register_streamed_signal(allocp, sig, stream)
     q *allocp;
     int sig;
     q stream;
{
  extern q *do_unify();
  if (signal_streams[sig] == 0) {
    signal_streams[sig] = stream;
    add_signal_handler(sig, streamed_signal_handler);
  } else {
    allocp = do_unify(allocp, stream, signal_streams[sig]);
  }
  return allocp;
}

/* Initiation */

static void init_streamed_signal_handling()
{
  int sig;
  for (sig = 0; sig < NSIG; sig++) {
    signal_streams[sig] = 0;
  }
  register_gc_hook(gc_signal_streams);
}

/****************************************
  DEFAULT SIGPIPE HANDLER
****************************************/

static int default_sigpipe_handler(allocp, sig)
     q *allocp;
     int sig;
{
  declare_globals;
  return 0;
}

/*
  GENERAL INITIATION
*/

void init_klic_signal_handling()
{
  init_general_signal_handling();
  init_streamed_signal_handling();
  add_signal_handler(SIGINT, default_sigint_handler);
  add_signal_handler(SIGPIPE, default_sigpipe_handler);
}

#endif
