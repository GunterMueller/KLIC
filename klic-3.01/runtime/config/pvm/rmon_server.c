/* ---------------------------------------------------------- 
%   (C)1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
#if defined(VPP) || defined(AP3000)
#define VPPAP
#endif
#ifndef VPPAP
#include <pvm3.h>
#endif
#include <stdio.h>
#include <signal.h>
#include <klic/config.h>
#include <klic/basic.h>
#include <klic/rmon.h>
#ifdef VPPAP
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#define MAXNUM    256

#ifdef VPPAP
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

last_column(disp_nodes)
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

main(argc,argv)
     int argc; char **argv;
{
  long s_times, i_times, gc_times, sm_times, rm_times;
  int i,j,n,disp_nodes;
  int family = PF_UNIX, sock2, sock2len, max, req_size;
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
	fprintf(stderr, "Please restart : rmon_server %d\n", nodes);
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
	  int idle_times = i_times + sm_times + rm_times;
	  printf(" %d 0 0 %d\n",
		 100 * (s_times - idle_times)/ s_times,
		 100 * idle_times / s_times);
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
#else
main(argc,argv)
     int argc; char **argv;
{
  int i,j,mytid,histid,bufsize,msgtag,info,buf_id,n,disp_nodes;
  long nodes, ibuf[MAXNUM*5], s_times, i_times, gc_times, sm_times, rm_times;
  long prof_tag;

  if (argc == 1) {
      disp_nodes = 20;
  } else {
      disp_nodes = atoi(argv[1]);
  }

  mytid = pvm_mytid();
  fprintf(stderr, "Rmonitor Tid is %x\n", mytid);

  for (;;) {
      buf_id = pvm_recv(-1,-1);
      if (buf_id < 0 ){
	  fprintf(stderr, "Error in pvm_recv\n");
      }
      info = pvm_bufinfo(buf_id,&bufsize,&msgtag,&histid);
      if (info < 0 ){
	  fprintf(stderr, "Error in pvm_bufinfo\n");
      }

      pvm_upklong(&prof_tag, 1, 1);
      switch(prof_tag){
	case SHOEN_START_TAG:
	  for (j = 0; j < disp_nodes; j++) {
	      fprintf(stdout, " %d    0   0    0    0    0    0    0    0    0    0     0   0   0  0  100\n",j);
	  }	  
	  fflush(stdout);
	  break;

	case SHOEN_OUTPUT_PROF_TAG:

	  pvm_upklong(&nodes, 1, 1);
	  if(nodes > disp_nodes){
	      fprintf(stderr, "Please restart : rmon_server %d\n", nodes);
	      exit();
	  }

	  pvm_upklong(ibuf, nodes*5, 1);

	  fprintf(stdout, "CPU minf mjf xcal  intr ithr  csw icsw migr smtx  srw syscl  usr sys  wt idl\n");
	  n = 0;

	  for (j = 0; j<nodes; j++) {
	      s_times = ibuf[n];
	      i_times = ibuf[n+1];
	      gc_times = ibuf[n+2];
	      sm_times = ibuf[n+3];
	      rm_times = ibuf[n+4];
	      
	      fprintf(stdout, " %d    0   0    0    0    0    0    0    0    0    0     0 ",j);
	      if (s_times) { 
		  fprintf(stdout, " %d 0 0 %d\n",
			  s_times - (i_times + sm_times + rm_times),
			  i_times + sm_times + rm_times);
	      } else {
		  fprintf(stdout, " 0 0 0 100\n");
	      }
	      n+=5;
	  }

	  for(j = nodes ; j < disp_nodes ;j++){
	      fprintf(stdout, " %d    0   0    0    0    0    0    0    0    0    0     0   0   0  0  100\n",j);
	  }
	  fflush(stdout);
	  break;

	case SHOEN_TERMINATE_TAG:
	  for(i = 0; i < 2 ; i++){
	      fprintf(stdout, "CPU minf mjf xcal  intr ithr  csw icsw migr smtx  srw syscl  usr sys  wt idl\n");	  
	      for (j = 0; j < disp_nodes; j++) {
		  fprintf(stdout, " %d    0   0    0    0    0    0    0    0    0    0     0   0   0  0  100\n",j);
	      }	  
	      fflush(stdout);
	      sleep(2);
	    }
	  break;

	default:
	  fprintf(stderr, "rmon_server invalid msg from shoen\n");
	  exit();
	  break;
      }
  }
}
#endif
