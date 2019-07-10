/* ---------------------------------------------------------- 
%   (C)1993,1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/config.h>
#ifdef XTERM
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

loop(in, out)
     int in, out;
{
  char buf[BUFFER_SIZE];
  long bytes;
 again:
  while ((bytes = read(in, buf, BUFFER_SIZE)) != 0) {
    long written = 0;
    if (bytes == -1) {
      if (errno == EINTR) {
	goto again;
      } else {
	exit(1);
      }
    }
    do {
      written += write(out, &buf[written], bytes-written);
    } while (written < bytes);
  }
}

#define DELETE		0x7f
#define CTRL_A		('A'-'@')
#define CTRL_B		('B'-'@')
#define CTRL_C		('C'-'@')
#define CTRL_D		('D'-'@')
#define CTRL_E		('E'-'@')
#define CTRL_F		('F'-'@')
#define CTRL_G		('G'-'@')
#define CTRL_H		('H'-'@')
#define CTRL_I		('I'-'@')
#define CTRL_J		('J'-'@')
#define CTRL_K		('K'-'@')
#define CTRL_L		('L'-'@')
#define CTRL_M		('M'-'@')
#define CTRL_N		('N'-'@')
#define CTRL_O		('O'-'@')
#define CTRL_P		('P'-'@')
#define CTRL_Q		('Q'-'@')
#define CTRL_R		('R'-'@')
#define CTRL_S		('S'-'@')
#define CTRL_T		('T'-'@')
#define CTRL_U		('U'-'@')
#define CTRL_V		('V'-'@')
#define CTRL_W		('W'-'@')
#define CTRL_X		('X'-'@')
#define CTRL_Y		('Y'-'@')
#define CTRL_Z		('Z'-'@')

set_terminal_parameters()
{
  struct termios params;

  tcgetattr(fileno(stdin), &params);
  params.c_iflag |= (ICRNL);
  params.c_oflag |= (OPOST | ONLCR);
  params.c_cflag &= ~CSIZE;
  params.c_cflag |= (CS8 | CREAD | CLOCAL);
  params.c_lflag |= (ICANON | ECHOE | ECHO | ISIG);
  params.c_cc[VINTR]	= CTRL_C;
  params.c_cc[VERASE]	= DELETE;
  params.c_cc[VKILL]	= CTRL_U;
  params.c_cc[VEOF]	= CTRL_D;
  params.c_cc[VREPRINT]	= CTRL_L;
  tcsetattr(fileno(stdin), TCSANOW, &params);
}

main(argc, argv)
     int argc;
     char **argv;
{
  char *sockname;
  int insock, outsock;
  int infd, outfd;
  struct sockaddr *addr;
  int addr_size;
  fd_set readfds;
  void *malloc();

  if (argc != 2) exit(1);

  set_terminal_parameters();

  sockname = argv[1];
  insock = socket(AF_UNIX, SOCK_STREAM, 0);
  outsock = socket(AF_UNIX, SOCK_STREAM, 0);
  addr_size = sizeof(struct sockaddr) + strlen(sockname) + 1;
  addr = (struct sockaddr *)malloc(addr_size);
  addr->sa_family = PF_UNIX;
  strcpy(addr->sa_data, sockname);
  connect(insock, addr, addr_size);
  connect(outsock, addr, addr_size);
  free(addr);
  if (fork() == 0) {
    close(outsock);
    loop(insock, fileno(stdout));
  } else {
    close(insock);
    loop(fileno(stdin), outsock);
    wait(0);
  }
  exit(0);
}

#else
main() { return 1; }
#endif
