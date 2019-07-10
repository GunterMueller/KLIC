/*
  Copyright 1994, 1995 Institute for New Generation Computer Technology
  Read COPYRIGHT for detailed information

  Copyright 1996, 1997, 1998 Japan Information Processing Development Center
  Read COPYRIGHT-JIPDEC for detailed information

  KLIC System Configuration Setting
  This file was created by KLIC configuration script.
  Date of Confuguration: Sun May 15 00:22:03 JST 2016
  Configured by: ueda
*/

/* CPU name for locking shared-memory parallel */

#define Unknown_cpu

/* Directories for installation */

#define KLICBIN "/home/ueda/local/roquefort/bin"
#define KLICLIB "/home/ueda/local/roquefort/lib"
#define KLICINCLUDE "$HOME/local/roquefort/include"
#define KLICINCLUDE0 "-I$HOME/local/roquefort/include"
#define KLIC_COMPILER "/home/ueda/local/roquefort/lib/klic/kl1cmp"
#define KLIC_DBMAKER "/home/ueda/local/roquefort/lib/klic/klicdb"

/* Laguage and program processing systems */

#define CC "gcc -m32"
#define LD "gcc -m32"
#define RANLIB "ranlib"

/* Additional CC flags for optimized compilation of KL1 programs */

#define UOPTFLAGS ""

/* Additional flags for LD */

#define LIBRARIES "-lklic -lklicm -L/usr/lib   -lm -lnsl"
#define LIBRARIES_T "-lklict -lklicm -L/usr/lib   -lm -lnsl"
#define LIBRARIES_D "-lklicd -lklicm -L/usr/lib   -lm -lnsl"
#define LIBRARIES_S "-lklics -lklicm -L/usr/lib   -lm -lnsl"
#define KLIC_CC_OPTIONS "-O2 -fPIC"
#define KLIC_LD_OPTIONS " "

/* Usual C macros depending on availability */

#define ASYNCIO
#define STRINGH
#define STDDEFH
#define SETLINEBUF
#define USELOCKF
#define USESIG
#define GETRUSAGE
#undef USEBCMP
#undef USEBCOPY
#undef USEBZERO
#define USESTRCHR

#define USEUSLEEP
#define USEULIMIT
#define USEGETDTABLESIZE
#define USETIMER
#define NRAND48
#define ISASTREAM
#define USESTREAMINCLUDEDIR
#undef USEPROCBIND
#define USESELECT
#undef XTERM
#undef DECL_SYS_ERRLIST
#undef DECL_FPRINTF

#ifdef USEBCMP
#define BCMP(x,y,len)		bcmp(x,y,len)
#else
#define BCMP(x,y,len)		memcmp(x,y,len)
#endif

#ifdef USEBCOPY
#define BCOPY(from,to,len)	bcopy(from,to,len)
#else
#define BCOPY(from,to,len)	memcpy(to,from,len)
#endif

#ifdef USEBZERO
#define BZERO(from,len)		bzero(from,len)
#else
#define BZERO(from,len)		memset(from,0,len)
#endif

#ifdef USESTRCHR
#define STRCHR			strchr
#else
#define STRCHR			index
#endif

#undef CROSS


#undef CPU_TYPE
#define CPU_TYPE INTEL
#define INTEL

#define DIST_COMPILER_FLAG(name, ext)     {	(void)sprintf(bufp, " -DDIST -DSHM_DIST %s -I. %s%s", 		                   klic_incdir, (name), (ext)); }
#define DIST_LINKAGE_FLAG() 

