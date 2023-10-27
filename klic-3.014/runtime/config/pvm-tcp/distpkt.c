/* #define DEBUG */
/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
/* PVM-TCP dependent inter-pe communication library */
/* distpkt.c,v 1.3 1997/06/06 10:41:13 sekita Exp */
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/unify.h>
#include <klic/timing.h>

#include <stdio.h>
#include <signal.h>

#include <klic/asyncio.h>
#include <klic/distio.h>
#include <klic/interpe.h>

#include <fcntl.h>
#ifdef ISASTREAM
#include <stropts.h>
#endif

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>

int receive_message();
int add_signal_handler();
void init_sigio_handler();

#define SYNCID_CONN 0x3141
#define SYNCID_CONN2 0x5926
#define SYNCID_CONN3 0x5358
#define SYNCID_CONN4 0x9793

#ifdef DEBUG
#define DEBUG_X(X) X
#else
#define DEBUG_X(X)
#endif
#include <errno.h>
#ifdef DECL_SYS_ERRLIST
extern char *sys_errlist[];
#endif
extern int errno;
/*
  Communication buffer management
  ALLOCATE COMBUF		combuf *alloc_combuf(bufsize)
  FREE COMBUF			void *free_combuf(buf)
  BACKUP COMBUF			combuf *backup_combuf(buf)
  realloc combuf		static combuf *realloc_combuf(inbuf, bufsize)
  EXPAND OUTBUF			void expand_outbuf(outbuf)

  Communication initialize
  INIT COMMUNICATION		void init_comm()
  IO INIT COMMUNICATION		void io_init_comm()
  IO INIT AFTER INTERRUPT SETUP	void init_dist_signal_setup() 
  Raw packet send/receive
  RECEIVE PACKET		combuf *receive_packet(heap_neededp)
  SEND_PACKET			void send_packet(node, sendbuf)
*/

#define INITIAL_COMBUF_SIZE	0x10000	/* for inbuf and outbuf */

/* Communication buffer management functions */
combuf *
alloc_combuf(bufsize)
long bufsize;
{
    combuf *buf = (combuf *)malloc(sizeof(combuf));
    long *newptr;
    if (!buf)
      fatal("malloc failed in alloc_combuf (1)");
    buf->buf_size = bufsize;
    newptr =  (long *)malloc((bufsize+1) * sizeof(long));
    buf->rd_index = 0;		/* pointer for read */
    buf->wt_index = 0;		/* pointer for write */
    if (newptr == NULL)
      fatal("malloc failed in alloc_combuf (2)");
    buf->buffer = newptr + 1;
    return buf;
}

static combuf *
realloc_combuf(inbuf, bufsize)
combuf *inbuf;
long bufsize;
{
    long *bufp = inbuf->buffer - 1;
    bufp = (long *)realloc((char *)bufp, (bufsize+1) * sizeof(long));
    if (bufp == NULL)
      return 0;
    inbuf->buffer = bufp+1;
    inbuf->buf_size = bufsize;
    return inbuf;
}

void *
free_combuf(buf)
combuf *buf;
{
    free(buf->buffer - 1);
    free(buf);
}
/* backup_combuf() is called only within message_suspend() */
combuf *
backup_combuf(buf)
combuf *buf;
{
    combuf *retbuf;

    /* backup the contents of BUF into newly created combuf  */
    retbuf = alloc_combuf(buf->wt_index);
    BCOPY((char *)(buf->buffer-1), (char *)(retbuf->buffer-1),
	  sizeof(long)* (buf->wt_index + 1));
    retbuf->wt_index = buf->wt_index;
    retbuf->rd_index = buf->rd_index;
    buf->wt_index = 0;		/* Clear the write buffer */
    return retbuf;
}

/* expand_outbuf() is called only within the macro PUT_BUFFER() */
void
expand_outbuf(outbuf)
combuf *outbuf;
{
    /* always double the size */
    /* expand_outbuf() must expand the buffer at least 4-words.
       Otherwise, PUT4_BUFFER() in distpkt.h will crash.        */ 
    declare_globals;   
    long bufsize = outbuf->buf_size * 2;
    outbuf = realloc_combuf(outbuf, bufsize);
    if (!outbuf)
      fatal("realloc failed in expanding outbuf");
    ioprintf("PE %d outbuf expanded to %d\n", my_node, bufsize);
}

static int
makesocket()
{
    declare_globals;
    struct sockaddr_in recvsin;
    int s;
    DEBUG_X((void)debug_fprintf(stderr, "%d:try to make port\n", my_node));
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	(void)klic_fprintf(stderr,
			   "%d:Socket failed:%s", my_node, sys_errlist[errno]);
	exit(2);
    }
    DEBUG_X((void)debug_fprintf(stderr, "%d:done.\n", my_node));
    BZERO((char *)&recvsin, sizeof(recvsin));
    DEBUG_X((void)debug_fprintf(stderr, "%d:try to bind\n", my_node));
    if (bind(s, (struct sockaddr *)&recvsin, sizeof(recvsin))) {
	(void)klic_fprintf(stderr,
			   "%d:bind failed:%s", my_node, sys_errlist[errno]);
	exit(2);
    }
    DEBUG_X((void)debug_fprintf(stderr, "%d:done.\n", my_node));
    return s;
}
static
infocheck(info)
int info;
{
    declare_globals;
    if (info >= 0)
      return;
    (void)klic_fprintf(stderr, "%d:info returns error=%d\n", my_node, info);
    for(;;);
}

/* Raw packet send/receive functions */
static void
forceread(fd, buffer, size)
int fd;
char *buffer;
long size;
{
    int r;
    declare_globals;
    DEBUG_X((void)debug_fprintf(stderr,
			       "%d:Enter forceread(%d, %d)\n",
			       my_node, fd, size));
    for (;;) {
	r = read(fd, buffer, size);
	if (r == size)
	  return;
	DEBUG_X(debug_fprintf(stderr, "%d:multiread %d/%d\n",
			     my_node, r, size));
	if (r > 0) {
	    buffer += r;
	    size -= r;
	    continue;
	} else if (r == 0) {
	    int i;
	    klic_fprintf(stderr, "%d: fd:%d suddenly closed\n", my_node, fd);
	    for (i = 0; i < tid_table.child_count; ++i) {
		if (i == my_node)
		  continue;
		if (port_tbl[i].fd == fd) {	/* Very Bad Manner.... */
		    klic_fprintf(stderr, "%d: the fd:%d come from (pe:%d)?\n",
				 my_node, fd, i);
		    break;
		}
	    }
	    kill_child();
	    exit(1);
	} else if (errno == EINTR)
	  continue;
	(void)klic_fprintf(stderr, "%d: read error %d: %s\n",
			   my_node, r, sys_errlist[errno]);
	exit(4);
    }
}

static void
forcewrite(fd, buffer, size)
int fd;
char *buffer;
long size;
{
    int r;
    declare_globals;
    DEBUG_X((void)debug_fprintf(stderr,
			       "%d:Enter forcewrite(%d, %d)\n",
			       my_node, fd, size));
    for (;;) {
	r = write(fd, buffer, size);
	if (r == size)
	  return;
	DEBUG_X(debug_fprintf(stderr, "%d:multiwrite %d/%d\n",
			     my_node, r, size));
	if (r > 0) {
	    buffer += r;
	    size -= r;
	    continue;
	} else if (errno == EINTR)
	  continue;
	(void)klic_fprintf(stderr, "%d: write error %d: %s\n",
			   my_node, r, sys_errlist[errno]);
	exit(4);
    }
}
static void
make_connection(tbl)
struct port_tbl *tbl;
{
    int my_portnum;
    int info;
    int i;
    char hostname[MAXHOSTNAMELEN];
    int s_sock;
    declare_globals;
    if (gethostname(hostname, sizeof(hostname)) < 0) {
	(void)klic_fprintf(stderr,
			   "%d: gethostname:%s\n",
			   my_node, sys_errlist[errno]);
	exit(3);
    }
    s_sock = -1;		/* init to invalid fd */
    if (my_node > 0) {
	struct sockaddr_in recvsin;
	int len = sizeof(recvsin);
	s_sock = makesocket();
	if (getsockname(s_sock, (struct sockaddr *)&recvsin, &len) < 0) {
	    (void)klic_fprintf(stderr, "%d:getsockname:%s",
			       my_node, sys_errlist[errno]);
	    exit(0);
	}
	my_portnum = htons(recvsin.sin_port);
	listen(s_sock, 5);
    } else
      my_portnum = 0;
    info = pvm_initsend(PvmDataRaw); infocheck(info);
    info = pvm_pkint(&my_portnum, 1, 1);infocheck(info);
    info = pvm_pkstr(hostname);infocheck(info);
    info = pvm_send(Tid_of_io(), 3);infocheck(info);
    DEBUG_X((void)debug_fprintf(stderr,
			       "%d:send to io(%d)\n", my_node, Tid_of_io()));
    info = pvm_recv(Tid_of_io(), -1);infocheck(info);
    for (i = 0; i < tid_table.child_count; ++i) {
      char tmp_node_name[MAXHOSTNAMELEN];
      char *tmp_str;
      info = pvm_upkint(&(port_tbl[i].port), 1, 1);infocheck(info);
      info = pvm_upkstr(tmp_node_name);infocheck(info);
      tmp_str = (char *)malloc(strlen(tmp_node_name) + 1);
      strcpy(tmp_str, tmp_node_name);
      port_tbl[i].node_name = tmp_str;
    }
    if (port_tbl[my_node].port != my_portnum
	|| strcmp(port_tbl[my_node].node_name, hostname)) {
	(void)klic_fprintf(stderr, "%d:error %d != %d or %s != %s\n",
			   my_node, 
			   port_tbl[my_node].port, my_portnum,
			   port_tbl[my_node].node_name, hostname);
	exit(3);
    }
    for (i = 0; i < my_node; ++i) {
	struct sockaddr_in from;
	int r;
	int fromlen = sizeof(struct sockaddr);
	long his_node;
	int recvs;
	recvs = accept(s_sock, (struct sockaddr *)&from, &fromlen);
	if (recvs < 0) {
	    (void)klic_fprintf(stderr, "%d: accept:%s\n",  my_node,
			       sys_errlist[errno]);
	    exit(0);
	}
	forceread(recvs, &his_node, sizeof(his_node));
	DEBUG_X((void)debug_fprintf(stderr,
				   "%d:<pe(%d) fd=%d\n",
				   my_node, his_node, recvs));
	if (his_node < 0 || his_node >= tid_table.child_count) {
	    DEBUG_X((void)debug_fprintf(stderr,
				       "%d: invalid node id received %d\n",
				       my_node, his_node));
	    exit(0);
	}
	port_tbl[his_node].fd = recvs;
    }
    close(s_sock);
    port_tbl[my_node].fd = -1;	/* Self-communication not allowed */
    for (i = my_node+1; i < tid_table.child_count; ++i) {
	struct sockaddr_in sendsin;
	struct hostent *hp, *gethostbyname();
	char *hisname = port_tbl[i].node_name;
	int csocket;
	struct in_addr serverip;
	
	hp = gethostbyname(hisname);
	if (!hp) {
	    (void)klic_fprintf(stderr, "%d: %s notfound\n", my_node, hisname);
	    exit(3);
	}
	BZERO((char *)&sendsin, sizeof(sendsin));
	BCOPY((char *)(hp->h_addr_list[0]), (char *)&sendsin.sin_addr,
	      sizeof(struct in_addr));
	sendsin.sin_family = AF_INET;
	sendsin.sin_port = htons(port_tbl[i].port);
	DEBUG_X((void)debug_fprintf(stderr, "%d:connect to %s:%d\n", my_node,
				   inet_ntoa(sendsin. sin_addr),
				   ntohs(sendsin.sin_port)));
	csocket = socket(AF_INET, SOCK_STREAM, 0);
	if (csocket < 0) {
	    (void)klic_fprintf(stderr, "%d: cannot create slave socket %d: %s",
			       my_node, csocket, sys_errlist[errno]);
	    exit(3);
	}
	if (connect(csocket,
		    (struct sockaddr *)&sendsin, sizeof(sendsin))){
	    (void)klic_fprintf(stderr,
			       "%d: cannot connect to node(%d)server %s:0x%x: %s",
			       my_node, i, hisname, ntohl(serverip.s_addr),
			       sys_errlist[errno]);
	    exit(1);
	}
	DEBUG_X((void)debug_fprintf(stderr,
				   "%d:>pe(%d) fd=%d\n", my_node, i, csocket));
	port_tbl[i].fd = csocket;
	forcewrite(csocket, &my_node, sizeof(my_node));
    }
}
void
io_init_comm()
{
    int i;
    int info;
    port_tbl = (struct port_table *)calloc(tid_table.child_count,
					   sizeof(*port_tbl));
    for (i = 0; i < tid_table.child_count; ++i) {
      char tmp_node_name[MAXHOSTNAMELEN];
      char *tmp_str;
      info = pvm_recv(Tid_of(i), -1);infocheck(info);
      info = pvm_upkint(&(port_tbl[i].port), 1, 1);infocheck(info);
      info = pvm_upkstr(tmp_node_name); infocheck(info);
      tmp_str = (char *)malloc(strlen(tmp_node_name) + 1);
      strcpy(tmp_str, tmp_node_name);
      port_tbl[i].node_name = tmp_str;
      DEBUG_X(debug_fprintf(stdout, "%d: %s(%d)\n",
			     i, port_tbl[i].node_name, port_tbl[i].port));
    }
    info = pvm_initsend(PvmDataRaw);infocheck(info);
    for (i = 0; i < tid_table.child_count; ++i) {
	info = pvm_pkint(&(port_tbl[i].port), 1, 1);infocheck(info);
	info = pvm_pkstr(port_tbl[i].node_name);infocheck(info);
    }
    for (i = 0; i < tid_table.child_count; ++i) {
	info = pvm_send(Tid_of(i), 3);infocheck(info);
    }
    io_wait_sync(SYNCID_CONN);
    io_sync(SYNCID_CONN2);
    io_wait_sync(SYNCID_CONN3);
    io_sync(SYNCID_CONN4);
    {
	int dummy;
	info = pvm_initsend(PvmDataRaw);infocheck(info);
	info = pvm_pkint(&dummy, 1, 1);infocheck(info);
	info = pvm_send(Tid_of(MASTER_NODE), 3);infocheck(info);
	DEBUG_X((void)debug_fprintf(stderr, "io:semaphore send\n"));
    }
}
static fd_set port_fds;
static int max_port_fds;
init_comm()
{
    int i;
    static char mybuffer[BUFSIZ];
    declare_globals;
/*    freopen("/dev/pts/3", "w", stderr);*/
    setvbuf(stderr, mybuffer, _IOLBF, BUFSIZ);
    
    /* initialize out buffer */
    kumon_outbuf = alloc_combuf(INITIAL_COMBUF_SIZE);
    
    /* initialize in buffer */
    kumon_inbuf = alloc_combuf(INITIAL_COMBUF_SIZE);
    port_tbl = (struct port_table *)calloc(tid_table.child_count,
					   sizeof(*port_tbl));
    make_connection(port_tbl);
    FD_ZERO(&port_fds);
    max_port_fds = -1;
    for (i = 0; i < tid_table.child_count; ++i) {
	if (i != my_node) {
	    int the_fd = port_tbl[i].fd;
	    FD_SET(the_fd, &port_fds);
	    if (max_port_fds < the_fd)
	      max_port_fds = the_fd;
	}
    }
}
static int
dist_sigio_stream_handler(allocp, fd, sigio_type, rfd, wfd)
q *allocp;
long fd;
enum sigiotype sigio_type;
fd_set *rfd, *wfd;
{
    int i;
    int again;
    declare_globals;
    for (i = 0; i < tid_table.child_count; ++i)
      if (i != my_node)
	FD_CLR(port_tbl[i].fd, rfd);
    return receive_message(allocp, SIGIO);
}

int debugger_flag;
static void
dummy() {
    debugger_flag = ~debugger_flag;
    signal(SIGUSR2, dummy);
}
void
init_dist_signal_setup()
{
    declare_globals;
    int i;
    q *set_simple_interval_timer_handler();

    init_sigio_handler();

    child_sync(SYNCID_CONN);
    child_wait_sync(SYNCID_CONN2);
    for (i = 0; i < tid_table.child_count; ++i) {
	if (i != my_node) {
	    int the_fd = port_tbl[i].fd;
	    add_sigio_handler(the_fd,
			      dist_sigio_stream_handler, KLIC_SIGIO_IN);
	    if (!no_sendsig) {
		extern int errno;
#ifdef ISASTREAM
		if (!isastream(the_fd))
		  fatalf("%ld: fd(%d) <- pe(%d) is not a stream\n",
			 my_node, the_fd, i);
#endif
		if (fcntl(the_fd, F_SETOWN, getpid()) < 0)
		  fatal("fcntl(setown) errno=%d\n", errno);
#ifdef FASYNC
		if (fcntl(the_fd, F_SETFL, FASYNC) < 0)
		  fatal("fcntl(setfl) errno=%d\n", errno);
#else
		if (ioctl(the_fd, I_SETSIG, S_INPUT) < 0)
		  fatal("ioctl(setsig) errno=%d\n", errno);
#endif
	    }
	}
    }
#ifdef USETIMER
    if (!noitimer) {
	struct itimerval interval;
	long isec, usec;
	if (interval_usec == 0)
	  interval_usec = 10000;
	isec = interval_usec / 1000000;
	usec = interval_usec % 1000000;
	heapp = set_simple_interval_timer_handler(heapp, isec, usec,
						  receive_message);
    }
#endif
    signal(SIGUSR2, dummy);
    child_sync(SYNCID_CONN3);
    child_wait_sync(SYNCID_CONN4);
    if (IS_MASTER_NODE(my_node)) {
	int info;
	int dummy;
	info = pvm_recv(Tid_of_io(), -1);infocheck(info);
	info = pvm_upkint(&dummy, 1, 1);infocheck(info);
	DEBUG_X((void)debug_fprintf(stderr, "%d:semaphore receive\n", my_node));
    }
}
static struct timeval immediate = {0, 0};
combuf *
receive_packet(heap_neededp)
long *heap_neededp;	/* this variable will be obsoleted in the future. */
{
    int i;
    int r;
    fd_set port_ready;
    combuf *inbuf = kumon_inbuf;
    long wtindex;
    declare_globals;
    port_ready = port_fds;
    r = select(max_port_fds + 1, &port_ready, 0, 0, &immediate);
    if (r == 0)
      return 0;
    if (r < 0) {
	if (errno == EINTR)
	  return 0;
	(void)klic_fprintf(stderr,
			   "%d:select failed:%s", my_node, sys_errlist[errno]);
	exit(0);
    }
    wtindex = 0;
    for (i = 0; i < tid_table.child_count; ++i) {
	long pktsize;
	int the_fd;
	if (i == my_node)
	  continue;
	the_fd = port_tbl[i].fd;
	if (FD_ISSET(the_fd, &port_ready)) {
	    forceread(the_fd, (char *)&pktsize, sizeof(pktsize));
	    INT_CL_PROBE_X(netstat.recv_data_siz +=
			   pktsize*sizeof(long) + sizeof(long));
	    if (pktsize == 0) { /* receive null packet: port politely closed */
		FD_CLR(the_fd, &port_fds);
#ifdef FASYNC
		fcntl(the_fd, F_SETFL, 0);
#else
		ioctl(the_fd, I_SETSIG, 0); /* remove SIGIO */
#endif
		DEBUG_X(debug_printf("%d: port to pe(%d) closed\n", my_node, i));
		/* Dont close this port now */
	    } else {
		DEBUG_X((void)debug_fprintf(stderr, "%ld:readpktsize=%ld\n",
					   my_node, pktsize));
		if (pktsize + wtindex > inbuf->buf_size) {
		    inbuf = realloc_combuf(inbuf, pktsize + wtindex);
		    if (!inbuf)
		      fatal("realloc failed in expanding inbuf");
		}
		forceread(the_fd, (char *)&(inbuf->buffer[wtindex]),
			  pktsize * sizeof(long));
		wtindex += pktsize;
		/*break;*/
	    }
	    if (--r <= 0)
	      break;
	}
    }
    inbuf->rd_index = 0;
    inbuf->wt_index = wtindex;
    *heap_neededp = wtindex * DECODE_MAX_OBJ_RATIO;
    return inbuf;
}

void
send_packet(node, sendbuf)
long node;
combuf *sendbuf;
{
    sendbuf->buffer[-1] = sendbuf->wt_index;
    forcewrite(port_tbl[node].fd,
	       sendbuf->buffer - 1, (sendbuf->wt_index+1) * sizeof(long));
    INT_CL_PROBE_X(netstat.send_data_siz += sendbuf->wt_index * sizeof(long));
    sendbuf->wt_index = 0;
}

close_network()
{
    combuf *buffer;
    int i;
    declare_globals;
    for (i = 0; i < tid_table.child_count; ++i) {
	if (i == my_node)
	  continue;
	buffer = NODE_TO_BUFFER(i);
	buffer->wt_index = 0;
	send_packet(i, buffer);	/* empty buffer... */
    }
    pvm_exit();
}
struct recv_int_stat recv_int_stat[NSIG];

char *
print_interrupt_statistics(ptr)
char *ptr;
{
    sprintf(ptr, " sigalrm=(%d+%d)/%d, sigio=(%d+%d)/%d, sigusr1=(%d+%d)/%d\n",
	    recv_int_stat[SIGALRM].usefull,
	    recv_int_stat[SIGALRM].useless,
	    recv_int_stat[SIGALRM].enter,
	    recv_int_stat[SIGIO].usefull,
	    recv_int_stat[SIGIO].useless,
	    recv_int_stat[SIGIO].enter,
	    recv_int_stat[SIGUSR1].usefull,
	    recv_int_stat[SIGUSR1].useless,
	    recv_int_stat[SIGUSR1].enter);
    return strlen(ptr)+ptr;
}

static void send_useless_message();
static q *decode_useless_message(inbuf, allocp)
combuf *inbuf;
q *allocp;
{
    
    declare_globals;
    int the_fd;
    long from = GET_BUFFER(inbuf);
    ioprintf("useless:%d -> %d\n", from, my_node);
    the_fd = port_tbl[from].fd;
    if (fcntl(the_fd, F_SETOWN, getpid()) < 0)
      fatal("fcntl(setown) errno=%d\n", errno);
#ifdef FASYNC
    if (fcntl(the_fd, F_SETFL, FASYNC) < 0)
      fatal("fcntl(setfl) errno=%d\n", errno);
#else
    if (ioctl(the_fd, I_SETSIG, S_INPUT) < 0)
      fatal("ioctl(setsig) errno=%d\n", errno);
#endif
    if (from < my_node)
      send_useless_message();
    return allocp;
}
static void send_useless_message()
{
    declare_globals;
    int i;
    for (i = 0; i < tid_table.child_count; ++i) {
	if (i != my_node) {
	    combuf *to_buffer = NODE_TO_BUFFER(i);
	    PUT_BUFFER(to_buffer, decode_useless_message);
	    PUT_BUFFER(to_buffer, my_node);
	    send_message_without_wtc_chk(i, to_buffer);
	}
    }
}
