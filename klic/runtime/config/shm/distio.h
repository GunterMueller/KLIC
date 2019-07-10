/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#ifndef _KLIC_DISTIO_H_
#define _KLIC_DISTIO_H_

#ifdef __STDC__
#define USE_STDARG
#include <stdarg.h>
#define VA_START(A, L)	va_start(A, L)
#define VA_ARGS(A)	A
#else
#include <varargs.h>
#define VA_START(A, L)	va_start(A)
#define VA_ARGS(A)	()
#endif

#include <klic/distpkt.h>

enum io_tag {
    FinishTag,
    AttachTag,
    DetachTag,
    System_putbTag,
    System_getsTag,
    System_putcTag,
    System_getcTag,
    System_writeTag,
    System_fflushTag,
    PutbTag,
    EPutbTag,
    GetsTag,
    User_putcTag,
    User_eputcTag,
    User_getcTag,
    User_ungetcTag,
    User_writeTag,
    User_ewriteTag,
    User_fflushTag,
    User_efflushTag,
    Emergency_stopTag,
    System_IsEOFTag
};


Extern FILE *io_tty Init(0);
#define IO_Lock                 (s_lock  (&io_lockwds->lockwd))
#define IO_Unlock               (s_unlock(&io_lockwds->lockwd))
#define SystemReadSize          1024
#define CloseIO()
#define AttachIO()		{ if (!my_io_lock_flag) {IO_Lock; my_io_lock_flag=1;} }
#define DetachIO()		{ IO_Unlock; my_io_lock_flag =0; }

#define SystemPrint(Str)        (int)klic_fwrite(Str,strlen(Str),1,stderr)
Extern int SystemRead();
#define SystemPutc(Char)	(int)klic_putc(Char,stderr)
Extern int SystemGetc();
#define SystemFflush()		(int)klic_fflush(stderr)
#define Print(Str)		(int)klic_fwrite(Str,strlen(Str),1,stdout)
#define EPrint(Str)		(int)klic_fwrite(Str,strlen(Str),1,stderr)
#define Read(Str)		(int)klic_gets(Str)
#define UserPutc(Char)		(int)klic_putc(Char,stdout)
#define UserEPutc(Char)		(int)klic_putc(Char,stderr)
#define UserGetc()		(int)klic_getc(stdin)
#define UserUngetc(Char)        (int)ungetc(Char, stdin)
#define UserWrite(Str,Len)      (int)klic_fwrite(Str,Len,1,stdout)
#define UserEWrite(Str,Len)     (int)klic_fwrite(Str,Len,1,stderr)
#define UserFflush()		(int)klic_fflush(stdout)
#define UserEFflush()		(int)klic_fflush(stderr)
#define EmergencyStop()
#define SystemIsEOF()

extern void ioprintf VA_ARGS((char *format, ...));
extern void ioeprintf VA_ARGS((char *format, ...));
extern void iosprintf VA_ARGS((char *format, ...));

#define io_server(argv,argc)

#include <klic/distproc.h>
#include <klic/distpkt.h>

#endif /* _KLIC_DISTIO_H_ */
