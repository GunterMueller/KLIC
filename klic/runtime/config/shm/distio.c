/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
/*
  For io-node
  IO NODE SERVER PROGRAM		void io_server(argc,argv)

  For shoen and reduction node
  WORKER NODE CLIENT ROUTINE	int io_client(enum io_tag msg_tag, ...)
*/

#include <stdio.h>
#include <errno.h>
#include <klic/basic.h>
#include <klic/sighndl.h>
#include <klic/struct.h>
#include <klic/distio.h>

int klic_gets(str,len,fp)
char *str;
int len;
FILE *fp;
{
  while (1) {
    int r = (int)fgets(str,len,fp);
    if ((r != EOF && r != 0) || errno != EINTR) {
      return r;
    }
  } 
}


SystemRead(Str)
char *Str;
{
  if (!io_tty) {
    io_tty = fopen("/dev/tty","r");
    if (io_tty==NULL) {
      io_tty = stdin;
    }
  }
  klic_gets(Str,SystemReadSize,io_tty);
  return 0;
}

SystemGetc()
{
  if (!io_tty) {
    io_tty = fopen("/dev/tty","r");
    if (io_tty==NULL) {
      io_tty = stdin;
    }
  }
  klic_getc(io_tty);
  return 0;
}
