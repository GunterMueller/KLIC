/* ---------------------------------------------------------- 
%   (C)1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <klic/basic.h>
#include <klic/rmon.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>

/*#define SHOEN_START_TAG         0
#define SHOEN_OUTPUT_PROF_TAG 100
#define SHOEN_TERMINATE_TAG   200*/

#define MAXNUM   256

static int sock;
static char *filename;

static void finish()
{
  close(sock);
  shutdown(sock,2);
  unlink(filename);
  exit(1);
}

static int readbuf(rf, bufp, req_size)
     FILE *rf;
     char *bufp;  int req_size;
{
  int n;

  while ((n = fread((char *)bufp, 1, req_size, rf)) < req_size) {
    if ( n < 0 || (n == 0 && !feof(rf))) {
      fclose(rf);
      fprintf(stderr,"Read Error\n");
      return -1;
    } else if (n == 0) {
      fclose(rf);
      return -1;
    }
    req_size -= n;  bufp += n;
  }
  return 0;
}

void last_column(disp_nodes)
     int disp_nodes;
{
  int i,j;
  for(i = 0; i < 2 ; i++) {
    printf("CPU minf mjf xcal  intr ithr  csw icsw migr smtx  srw syscl  usr sys  wt idl\n");	  
    for (j = 0; j < disp_nodes; j++) {
      printf(" %d    0   0    0    0    0    0    0    0    0    0     0   0   0  0  100\n",j);
    }	  
    fflush(stdout);
    sleep(2);
  }
}

int main(argc,argv)
     int argc; char **argv;
{
  long s_times, i_times, gc_times, sm_times, rm_times;
  int i,j,n,disp_nodes;
  int family = PF_UNIX, sock2/*, sock2len*/, max, req_size;
  socklen_t sock2len;
  struct sockaddr *addr, addr2;
  char *path;
  FILE *rf;
  union  {
    long v; char b[sizeof(long)];
  } prof_tag, nodes;

  union  {
    long v[MAXNUM*5];
    char b[MAXNUM*5*sizeof(long)];
  } ibuf;


/* init */
  switch (argc) {
  case 3:
    disp_nodes = atoi(argv[2]);
    break;
  case 2:
    disp_nodes = 20;
    break;
  default:
    fprintf(stderr,"usage: rmon socket_name [nodes]\n");
    exit(1);
  }

  path = argv[1];

  addr = (struct sockaddr *)malloc(sizeof(struct sockaddr)+strlen(path)+5);
  addr->sa_family = family;
  strcpy(addr->sa_data,"/tmp/");
  strcat(addr->sa_data,path);

  sock = socket(family,SOCK_STREAM,0);
  if (sock < 0) {
    perror("socket");
    exit(1);
  }
  if (bind(sock, addr, sizeof(struct sockaddr)) < 0) {
    perror("bind");
    exit(1);
  }
  if (listen(sock, 5) < 0) {
    perror("listen");
    exit(1);
  }

  filename = addr->sa_data;

  signal(SIGINT,finish);

  sock2len = sizeof(addr2);
  addr2.sa_family = family;

/* server loop */
 top:
  if ((sock2 = accept(sock, &addr2, &sock2len)) < 0) {
    perror("accept");
    close(sock);
    exit(1);
  }

  rf = fdopen(sock2,"r");
 
 /* client loop */
  for (;;) {
    if ( readbuf(rf, prof_tag.b, sizeof(long)) < 0 ) {
      last_column(disp_nodes); fclose(rf); goto top;
    }

    switch(prof_tag.v){
    case SHOEN_START_TAG:
      for (j = 0; j < disp_nodes; j++) {
	printf(" %d    0   0    0    0    0    0    0    0    0    0     0   0   0  0  100\n",j);
      }	  
      fflush(stdout);
      break;

    case SHOEN_OUTPUT_PROF_TAG:
      
      if ( readbuf(rf, nodes.b, sizeof(long)) < 0 ) {
	last_column(disp_nodes);  fclose(rf);  goto top;
      }
      
      if(nodes.v > disp_nodes){
	fprintf(stderr, "Please restart : rmon_server %ld\n", nodes.v);
	finish();
      }

      if (readbuf(rf, ibuf.b, nodes.v * 5 *sizeof(long)) < 0 )  {
	last_column(disp_nodes); fclose(rf); goto top;
      }
      
      printf("CPU minf mjf xcal  intr ithr  csw icsw migr smtx  srw syscl  usr sys  wt idl\n");
      n = 0;

      for (j = 0; j<nodes.v; j++) {
	s_times = ibuf.v[n];
	i_times = ibuf.v[n+1];
	gc_times = ibuf.v[n+2];
	sm_times = ibuf.v[n+3];
	rm_times = ibuf.v[n+4];
	      
	printf(" %d    0   0    0    0    0    0    0    0    0    0     0 ",j);
	if (s_times) { 
	  printf(" %ld 0 0 %ld\n",
		 s_times - (i_times + sm_times + rm_times),
		 i_times + sm_times + rm_times);
	} else {
	  printf(" 0 0 0 100\n");
	}
	n+=5;
      }

      for (j = nodes.v ; j < disp_nodes ;j++){
	printf(" %d    0   0    0    0    0    0    0    0    0    0     0   0   0  0  100\n",j);
      }
      fflush(stdout);
      break;

    case SHOEN_TERMINATE_TAG:
      last_column(disp_nodes);
      fclose(rf);
      goto top;
     
    default:
      fprintf(stderr, "rmon_server invalid msg from shoen\n");
      finish();
    }
  }
}
