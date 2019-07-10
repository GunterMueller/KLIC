/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
/* PVM dependent I/O communication library */
/* $Id: distio.c,v 1.1.1.1 2000/07/28 02:01:42 igarashi Exp $ */
#include <stdio.h>
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/distio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
/*
  For io-node
  IO NODE SERVER PROGRAM		void io_server(argc,argv)

  For shoen and reduction node
  WORKER NODE CLIENT ROUTINE	int io_client(enum io_tag msg_tag, ...)
  */

static void
sigint_handler()
{
    klic_fprintf(stdout, "kill tasks from io_server\n");
    kill_child();
    exit(-1);
}
static FILE *
open_tty()
{
    FILE *f;
    f = fopen("/dev/tty", "r");
    if (f)
      return f;
    klic_fprintf(stderr, "Can't open /dev/tty for trace command input\n");
    return stdin;
}    

void
io_server(argc,argv)
int argc;
char **argv;
{
  int his_tid;
  int dest_tid;
  int info;
  int bufid;
  enum io_tag kind;
  FILE *tracer_input = 0;
  char buf[1024];
  int pe_count = tid_table.child_count;

  signal(SIGINT,sigint_handler);

  his_tid = -1;

  while (1) {
    FILE *tmpstream;
    {
	int len;
	int msgtag;
	int tkind;
	bufid = pvm_recv(his_tid,-1);
	info = pvm_bufinfo(bufid,&len,&msgtag,&dest_tid);
	info = pvm_upkint(&tkind,1,1);
	kind = tkind;
    }
    switch(kind) {
    case FinishTag:
	if (--pe_count == 0) {
	    pvm_exit();
	    exit(0);
	}
	break;
     case AttachTag:
	his_tid = dest_tid;
	break;
     case DetachTag:
	his_tid = -1;
	break;
     case Emergency_stopTag:
	kill_child();
	pvm_exit();
	exit(0);
     case System_IsEOFTag:
	{
	    int result;
	    if (!tracer_input)
	      tracer_input = open_tty();
	    result = feof(tracer_input);
	    bufid = pvm_initsend(PvmDataRaw);
	    info = pvm_pkint(&result,1,1);
	    info = pvm_send(dest_tid,3);
	    break;
	}
	
     case System_putcTag:
     case User_eputcTag:
	tmpstream = stderr; goto putc_common;
     case User_putcTag:
	tmpstream = stdout;
     putc_common:
	info  = pvm_upkbyte(buf,1,1);
	debug_fputc(buf[0],tmpstream);
	break;

     case User_getcTag:
	tmpstream = stdin; goto getc_common;
     case System_getcTag:
	if (!tracer_input)
	  tracer_input = open_tty();
	tmpstream = tracer_input;
     getc_common:
	{
	    int ch;
	    ch = klic_getc(tmpstream);
	    bufid =pvm_initsend(PvmDataRaw);
	    info = pvm_pkint(&ch,1,1);
	    info = pvm_send(dest_tid,3);
	}
	break;
     case User_ungetcTag:
	info = pvm_upkbyte(buf,1,1);
	ungetc(buf[0], stdin);
	break;
     case System_writeTag:
     case User_ewriteTag:
	tmpstream = stderr; goto write_common;
     case User_writeTag:
	tmpstream = stdout; goto write_common;
     write_common:
	{
	    long len;
	    char *write_buf;
	    long written = 0;
	    info = pvm_upklong(&len, 1, 1);
	    write_buf = (char *) malloc(len);
	    info = pvm_upkbyte(write_buf,len,1);
	    while (written < len) {
		written += debug_fwrite(write_buf+written, 1, len, tmpstream);
	    }
	    free(write_buf);
	}
	break;
     case System_fflushTag:
     case User_efflushTag:
	tmpstream = stderr; goto flush_common;
     case User_fflushTag:
	tmpstream = stdout; goto flush_common;
     flush_common:
	{
	    int result = debug_fflush(tmpstream);
	    bufid =pvm_initsend(PvmDataRaw);
	    info = pvm_pkint(&result,1,1);
	    info = pvm_send(dest_tid,3);
	}
	break;

     case System_putbTag:
     case EPutbTag:
	tmpstream = stderr; goto putb_common;
     case PutbTag:
	tmpstream = stdout;
     putb_common:
	info  = pvm_upkstr(buf);
	debug_fprintf(tmpstream, "%s", buf);
	break;

     case GetsTag:
	tmpstream = stdin; goto gets_common;
     case System_getsTag:
	if (!tracer_input)
	  tracer_input = open_tty();
	tmpstream = tracer_input;
     gets_common:
	{
	    char *ret;
	    ret = fgets(buf, 255, tmpstream);
	    if (ret == NULL)
	      buf[0] = 0;
	    bufid = pvm_initsend(PvmDataRaw);
	    info = pvm_pkstr(buf);
	    info = pvm_send(dest_tid,3);
	    break;
	}
   default:
      debug_fprintf(stderr,"Unknown message %d\n",kind);
/*
      pvm_exit();
      exit(1);
*/
    }
    /*mm = pvm_probe(-1,-1);*/
  }
}
int
#ifdef USE_STDARG
io_client(enum io_tag msg_tag, ...)
#else
io_client(msg_tag, va_alist)
enum io_tag msg_tag;
va_dcl
#endif
{
    int bufid;
    int info;
    int msg = (int)msg_tag;
    va_list args;
    VA_START(args, msg_tag);

    bufid = pvm_initsend(PvmDataRaw);

    switch (msg_tag) {
     case AttachTag:
	has_attached = 1; goto simple_common;
     case DetachTag:
	has_attached = 0; goto simple_common;
     case FinishTag:
     case Emergency_stopTag:
     simple_common:
	info = pvm_pkint(&msg,1,1);
	info = pvm_send(Tid_of_io(), 3);
	break;
     case System_putbTag:
     case PutbTag:
     case EPutbTag:
	{
	    char *str = va_arg(args, char *);
	    info = pvm_pkint(&msg,1,1);
	    info = pvm_pkstr(str);
	    info = pvm_send(Tid_of_io(),3);
	}
	break;
     case System_getsTag:
     case GetsTag:
	{
	    char *str = va_arg(args, char *);
	    info = pvm_pkint(&msg,1,1);
	    info = pvm_send(Tid_of_io(),3);
	    bufid = pvm_recv(Tid_of_io(),-1);
	    info = pvm_upkstr(str);
	}
	break;
     case System_putcTag:
     case User_putcTag:
     case User_eputcTag:
	{
	    char ch = va_arg(args, int);
	    info = pvm_pkint(&msg,1,1);
	    info = pvm_pkbyte(&ch,1,1);
	    info = pvm_send(Tid_of_io(),3);
	}
	break;
     case User_ungetcTag:
        {
	    char ch = va_arg(args, int);
	    info = pvm_pkint(&msg,1,1);
	    info = pvm_pkbyte(&ch,1,1);
	    info = pvm_send(Tid_of_io(),3);
	 }
	break;
     case System_writeTag:
     case User_ewriteTag:
     case User_writeTag:
     write_common:
	{
	    char *write_buf = va_arg(args, char *);
	    long len = va_arg(args, long);
	    info = pvm_pkint(&msg,1,1);
	    info = pvm_pklong(&len,1,1);
	    info = pvm_pkbyte(write_buf, len, 1);
	    info = pvm_send(Tid_of_io(), 3);
	    break;
	}
     case System_fflushTag:
     case User_fflushTag:
     case User_efflushTag:
     case User_getcTag:
     case System_getcTag:
     case System_IsEOFTag:
	{
	    int result;
	    info = pvm_pkint(&msg,1,1);
	    info = pvm_send(Tid_of_io(),3);
	    bufid = pvm_recv(Tid_of_io(),-1);
	    info = pvm_upkint(&result,1,1);
	    va_end(args);
	    return result;
	}
	break;
    default:			/* must be program bug */
	abort();
    }
    va_end(args);
    return 0;
}
