/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
/* PVM dependent I/O communication library */
/* $Id: distio.h,v 1.1.1.1 2000/07/28 02:01:42 igarashi Exp $ */
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


int io_client VA_ARGS((enum io_tag msg_tag, ...));

Extern int has_attached Init(0);
#define CloseIO()		io_client(FinishTag)
#define AttachIO()		if (!has_attached) io_client(AttachTag)
#define DetachIO()		if(has_attached) io_client(DetachTag)
#define SystemPrint(Str)	io_client(System_putbTag, Str)
#define SystemRead(Str)		io_client(System_getsTag, Str)
#define SystemPutc(Char)	io_client(System_putcTag, Char)
#define SystemGetc()		io_client(System_getcTag)
#define SystemWrite(Str,Len)	io_client(System_writeTag, Str, (long)(Len))
#define SystemFflush()		io_client(System_fflushTag)
#define Print(Str)		io_client(PutbTag, Str)
#define EPrint(Str)		io_client(EPutbTag, Str)
#define Read(Str)		io_client(GetsTag, Str)
#define UserPutc(Char)		io_client(User_putcTag, Char)
#define UserEPutc(Char)		io_client(User_eputcTag, Char)
#define UserGetc()		io_client(User_getcTag)
#define UserUngetc(Char)	io_client(User_ungetcTag, Char)
#define UserWrite(Str,Len)	io_client(User_writeTag, Str, (long)(Len))
#define UserEWrite(Str,Len)	io_client(User_ewriteTag, Str, (long)(Len))
#define UserFflush()		io_client(User_fflushTag)
#define UserEFflush()		io_client(User_efflushTag)
#define EmergencyStop()		io_client(Emergency_stopTag)
#define SystemIsEOF()		io_client(System_IsEOFTag)

extern void ioprintf VA_ARGS((char *format, ...));
extern void ioeprintf VA_ARGS((char *format, ...));
extern void iosprintf VA_ARGS((char *format, ...));

#include <klic/distproc.h>
