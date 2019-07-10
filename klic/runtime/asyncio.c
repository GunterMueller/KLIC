/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/unify.h>
#include <klic/asyncio.h>
#ifdef USESIG
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <netinet/in.h>
#include <netdb.h>
#ifdef USESELECT
#include <sys/select.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
/*
  void sigio_handler(allocp, sig)
  void add_sigio_handler(fd, func, sigio_type, rfd, wfd)
  void init_sigio_handler()

  static int send_ready_message(allocp, fd, sigio_type)
  static q* gc_asyncio_streams(allocp, ntop, otop, nsize, osize)
  void init_asynchronous_io()
  void close_asynchronous_io_stream(fd)
  void register_asynchronous_io_stream(fd, stream)
  int poll_read_available(fd)
*/
#ifndef SIGIO
#define SIGIO SIGPOLL
#endif
#endif /* USESIG */

#ifdef __CYGWIN__
#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46
#endif

extern q convert_c_string_to_klic_string();
extern void add_signal_handler();
extern void register_gc_hook();

static q *asyncio_streams = 0;
static long fd_setsize;
static fd_set sigio_infds;
static fd_set sigio_outfds;
static enum sigiotype *sigio_types;
static int(**sigio_handlers)();
static struct timeval zerotime = { 0, 0 };

/* This function may be called from itimer interrupt handler */

#define Call_sigio_handler(again, allocp, fd, type) \
  ((again = (sigio_handlers[fd])(allocp, fd, type, &fdsr, &fdsw)),\
   (allocp = heapp))

#define C2KLIC convert_c_string_to_klic_string

int debugger_flag;
int
sigio_handler(allocp, sig)
q* allocp;
int sig;
{
    declare_globals;
    fd_set fdsr, fdsw;
    int r;
    static int retry_fd;
    static enum sigiotype retry_sigio_type = KLIC_SIGIO_NONE;

    if (retry_sigio_type != KLIC_SIGIO_NONE) {
      int fd = retry_fd;
      int again;
      again = (sigio_handlers[fd])(allocp, fd, retry_sigio_type, &fdsr, &fdsw);
      if (again)
	return 1;
      allocp = heapp;
      retry_sigio_type = KLIC_SIGIO_NONE;
    }
    fdsr = sigio_infds;
    fdsw = sigio_outfds;
    r = select(fd_setsize, &fdsr, &fdsw, 0, &zerotime);
    {
	if (debugger_flag) {
	    int fd;
	    size_t strlen();
	    char buffer[100];
	    fd = open("/dev/pts/2", 1);
	    sprintf(buffer, "%d:r=%d %08x\n", my_node, r, *(int *)&fdsr);
	    write(fd, buffer, strlen(buffer));
	    close(fd);
	}
    }

    if (r > 0) {
        int fd;
	int again;
	for (fd = 0; fd < fd_setsize; ++fd) {
	    enum sigiotype sigio_type = sigio_types[fd];
	    enum sigiotype call_type;
	    
	    switch (sigio_type) {
	    case KLIC_SIGIO_IN:
	      if (FD_ISSET(fd, &fdsr))
		call_type = KLIC_SIGIO_IN;
	      else
		continue;
	      break;
	    case KLIC_SIGIO_OUT:
	      if (FD_ISSET(fd, &fdsw))
		call_type = KLIC_SIGIO_OUT;
	      else
		continue;
	      break;
	    case KLIC_SIGIO_INOUT:
	      if (FD_ISSET(fd, &fdsr))
		if (FD_ISSET(fd, &fdsw))
		  call_type = KLIC_SIGIO_INOUT;
		else
		  call_type = KLIC_SIGIO_IN;
	      else if (FD_ISSET(fd, &fdsw))
		call_type = KLIC_SIGIO_OUT;
	      else
		continue;
	      break;
	    default:
	      continue;
	    }
	    again = (sigio_handlers[fd])(allocp, fd, call_type, &fdsr, &fdsw);
	    if (again) {
	      retry_fd = fd;
	      retry_sigio_type = call_type;
	      return 1;
	    }
	    allocp = heapp;
	}
    }
    heapp = allocp;
    return 0;
}

void
add_sigio_handler(fd, func, sigio_type)
long fd;
int (*func)();
enum sigiotype sigio_type;
{
    sigio_handlers[fd] = func;
    sigio_types[fd] = sigio_type;
    switch (sigio_type) {
     case KLIC_SIGIO_NONE:
	FD_CLR(fd, &sigio_infds);
	FD_CLR(fd, &sigio_outfds);
	break;
     case KLIC_SIGIO_IN:
	FD_SET(fd, &sigio_infds);
	FD_CLR(fd, &sigio_outfds);
	break;
     case KLIC_SIGIO_OUT:
	FD_CLR(fd, &sigio_infds);
	FD_SET(fd, &sigio_outfds);
	break;
     case KLIC_SIGIO_INOUT:
	FD_SET(fd, &sigio_infds);
	FD_SET(fd, &sigio_outfds);
	break;
    }
}

void
init_sigio_handler()
{
  static sigio_initiated = 0;
  extern char *malloc();
  int k;
  if (sigio_initiated)
    return;
#ifdef USEGETDTABLESIZE
  fd_setsize = getdtablesize();
#else
#ifdef USEULIMIT
  fd_setsize = ulimit(4, 0);
  if (fd_setsize < 0) {
    fatal("Can't obtain file descriptor table size");
  }
#else
  fatal("Don't know how to obtaine file descriptor table size");
#endif /* USEULIMIT */
#endif /* USEGETDTABLESIZE */
#ifdef __CYGWIN__
  fd_setsize = FD_SETSIZE;
#endif /* CYGWIN */
  sigio_types = (enum sigiotype *)malloc(sizeof(enum sigiotype) * fd_setsize);
  sigio_handlers = (int(**)())malloc(sizeof(int (*)()) * fd_setsize);
  for (k = 0; k < fd_setsize; ++k) {
      sigio_handlers[k] = (int (*)())0;
      sigio_types[k] = KLIC_SIGIO_NONE;
  }
  FD_ZERO(&sigio_infds);
  FD_ZERO(&sigio_outfds);
#ifdef USESIG
  add_signal_handler(SIGIO, sigio_handler);
#endif /* USESIG */
  sigio_initiated = 1;
}

#ifdef ASYNCIO
static int send_ready_message(allocp, fd, sigio_type, rfd, wfd)
     q* allocp;
     long fd;
     enum sigiotype sigio_type;
     fd_set *rfd, *wfd;
{
  declare_globals;
  q* array = asyncio_streams;

  if (array[fd] == 0) {
      klic_fprintf(stderr, "Unexpected IO interrupt for fd %d ignored\n", fd);
  } else {
#ifdef __CYGWIN__
    q newcons = makecons(allocp);
    q newvar = allocp[0] = makeref(&allocp[0]);
    allocp[1] = makeint(fd);
    allocp += 2;
    allocp = do_unify_value(allocp, array[fd], newcons);
    array[fd] = newvar;
  }
#else
    char c[16];
    int i,type,len;
    struct sockaddr_in addr;

    len = sizeof(type);


    if(!getsockopt(fd,SOL_SOCKET,SO_TYPE,&type,&len)){
      if(SOCK_DGRAM==type){
	q address, newvar, newcons;
	char str[INET_ADDRSTRLEN];

	len = sizeof(addr);
	i = recvfrom(fd,c,16,MSG_PEEK,(struct sockaddr *)&addr,&len);

	if(i<0){
  	  heapp = allocp;
	  return 0;
	}

  	inet_ntop(AF_INET,&addr.sin_addr.s_addr,str,INET_ADDRSTRLEN);
  	address = C2KLIC(str,allocp);
  	allocp = heapp;

  	newcons = makecons(allocp);
  	newvar = allocp[0] = makeref(&allocp[0]);
  	allocp[1] = address;
  	allocp += 2;
  	allocp = do_unify_value(allocp, array[fd], newcons);
  	array[fd] = newvar;
	heapp = allocp;
	return 0;
      }

      {
      q newcons = makecons(allocp);
      q newvar = allocp[0] = makeref(&allocp[0]);
      allocp[1] = makeint(fd);
      allocp += 2;
      allocp = do_unify_value(allocp, array[fd], newcons);
      array[fd] = newvar;
      heapp = allocp;
      return 0;

      }
    }
  }
#endif /* CYGWIN */
  heapp = allocp;
  return 0;
}

#define CopyIfNeeded(fd, array) \
{ \
  if ((array)[fd] != 0) { \
    allocp = copy_one_term(&array[fd], allocp, ntop, otop, nsize, osize); \
  } \
}

static q* gc_asyncio_streams(allocp, ntop, otop, nsize, osize)
     q *allocp, *ntop, *otop;
     unsigned long nsize, osize;
{
  extern q *copy_one_term();
  long fd;
  for (fd=0; fd<fd_setsize; fd++) {
    CopyIfNeeded(fd, asyncio_streams);
  }
  return allocp;
}

void init_asynchronous_io()
{
#ifdef USESIG
  static asyncio_initiated = 0;
  if (!asyncio_initiated) {
    extern char *malloc();
    int k;

    init_sigio_handler();
    asyncio_streams = (q*)malloc(sizeof(q)*fd_setsize);
    for (k=0; k<fd_setsize; k++)
      asyncio_streams[k] = 0;
    register_gc_hook(gc_asyncio_streams);
    asyncio_initiated = 1;
  }
#endif
}
void close_asynchronous_io_stream(fd)
     long fd;
{
#ifdef USESIG
  add_sigio_handler(fd, 0, KLIC_SIGIO_NONE);
  asyncio_streams[fd] = 0;
#endif
}
void register_asynchronous_io_stream(fd, stream)
     long fd;
     q stream;
{
#ifdef USESIG
  add_sigio_handler(fd, send_ready_message, KLIC_SIGIO_INOUT);
  asyncio_streams[fd] = stream;
#endif
}
#endif /* ASYNCIO*/

int poll_read_available(fd)
     int fd;
{
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  return select(fd_setsize, &fds, 0, 0, &zerotime);
}
