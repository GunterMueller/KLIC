/* ---------------------------------------------------------- 
%   (C)1994 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
/*
  Signal handling
*/

#ifndef _KLIC_SIGHNDL_H_
#define _KLIC_SIGHNDL_H_

#ifdef USESIG
#include <signal.h>

#ifndef NSIG
#define NSIG 32
#endif

#define signal_flags		(klic_sgnl_flags->flags)
#define signal_handlers		(klic_sgnl_flags->sgnl_handlers)
#define signal_done		(klic_sgnl_flags->done)

struct klic_sgnl_flags {
  Volatile long flags[NSIG];
  int ((* Volatile sgnl_handlers[NSIG])());
  Volatile long done;
} my_klic_sgnl_flags;

#ifdef SIGMAIN
Volatile struct klic_sgnl_flags *klic_sgnl_flags = &my_klic_sgnl_flags;
#else
extern Volatile struct klic_sgnl_flags *klic_sgnl_flags;
#endif

#endif
#endif
