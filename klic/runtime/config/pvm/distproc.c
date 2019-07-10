/* ---------------------------------------------------------- 
%   (C)1994,1995 Institute for New Generation Computer Technology 
%       (Read COPYRIGHT for detailed information.) 
%   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
%       (Read COPYRIGHT-JIPDEC for detailed information.)
----------------------------------------------------------- */
/* PVM dependent process spawn library */
/* $Id: distproc.c,v 1.1.1.1 2000/07/28 02:01:42 igarashi Exp $ */
#include <stdio.h>
#include <klic/basic.h>
#include <klic/struct.h>
#include <klic/primitives.h>
#include <klic/distio.h>
#include <klic/interpe.h>
#include <klic/rmon.h>
#include <signal.h>
#ifdef USEPROCBIND
#include <unistd.h>
#include <sys/processor.h>
#include <sys/procset.h>
#endif
#ifdef STRINGH
#include <string.h>
#else
#include <strings.h>
#endif

#include <klic/options.h>

#include <stdlib.h>

#define SYNCID_SPAWN 0x3193
/*
  char *expand_prog_name(buffer, progname)
  static void setup_send_args(opttable)
  static void recv_extract_args(opttable)
  static int io_spawn_children(num_child, opttable, prog_name, argc, argv)
  static int child_spawn_children(par_tid, opttable, prog_name, argc, argv)
  int spawn_children(num_child, opttable, prog_name, argc, argv)
  */

#define OPT_DEBUG(X) 

/*  for useconfig option */

#define CONFIGFILENAME "/.klic-config"
#define NHOSTNAMEBUF  10
#define HOSTNAMETABLESIZE 2

static int nhostnames, hostnamebufsize;
static char *hostnamebuf;
static char **hostnametable;
static char **gethostnametable()
{
 FILE *cfile;
 char *cfilename, *dirname;

 if (NULL == (dirname = getenv("HOME"))) return 0;
 cfilename = (char *) malloc(strlen(dirname)+strlen(CONFIGFILENAME)+1);
 strcpy(cfilename, dirname);
 strcat(cfilename, CONFIGFILENAME);

 cfile = fopen(cfilename,"r");

 if (cfile) {
  int tablesize = NHOSTNAMEBUF,
      realtablesize = NHOSTNAMEBUF,
      readsize, hosttablesize, p0;
  char *tmphostnamebuf;
  tmphostnamebuf = hostnamebuf = (char *)malloc(realtablesize);
  readsize = fread(tmphostnamebuf, 1, tablesize, cfile);
  while (!feof(cfile)) {
    if (readsize < tablesize) tablesize -= readsize;
    else {
      int tmprealtablesize = 2*realtablesize;
      if (hostnamebuf = (char *)realloc(hostnamebuf, tmprealtablesize)) {
        tmphostnamebuf = hostnamebuf+realtablesize;
        realtablesize = tmprealtablesize;
      } else { klic_fprintf(stderr, "can't get memory\n"); exit(1); }
    }
    readsize = fread(tmphostnamebuf, 1, tablesize, cfile);
  }
  fclose(cfile);
  hostnamebufsize = realtablesize - (tablesize - readsize);

  hosttablesize = HOSTNAMETABLESIZE;
  hostnametable = (char **)malloc(hosttablesize*sizeof(char *));
  nhostnames = 0;
  p0 = 0;
  for (;;) {
    for (; (hostnamebuf[p0] == '\n' && p0 < hostnamebufsize); p0++);
    if (p0 >= hostnamebufsize) break;
    if (hosttablesize <= nhostnames) {
      hosttablesize *= 2;
      if (!(hostnametable = (char **)realloc(hostnametable, 
					     sizeof(char *)*hosttablesize))){
      	  klic_fprintf(stderr, "can't get memory\n"); exit(1); }
    }
    hostnametable [nhostnames] = &(hostnamebuf[p0]);
    for (p0++; (hostnamebuf[p0] != '\n' && p0 < hostnamebufsize); p0++);
    if (p0 >= hostnamebufsize) break;
    nhostnames++;
    hostnamebuf[p0++] = 0;
  }
  if (!nhostnames) {
    free(hostnametable);
    free(hostnamebuf);
    return 0;
  } else return hostnametable; 
 } else return 0;
}

/* distprocs */   

void
kill_child()
{
    int i;
    int mytid;
    mytid = pvm_mytid();
    for (i = 0; i < tid_table.child_count; i++) {
	if (Tid_of(i) > 0 && mytid != Tid_of(i))
	  pvm_kill(Tid_of(i));
    }
    pvm_exit();
}

void
child_sync(syncid)
int syncid;
{
    int info;
    info = pvm_initsend(PvmDataRaw);
    info = pvm_pkint(&syncid, 1, 1);
    info = pvm_send(Tid_of_io(), 3);
}
void
io_wait_sync(syncid)
int syncid;
{
    int i;
    for (i = 0; i < tid_table.child_count; ++i) {
	int syncid_recv;
	int info;
	info = pvm_recv(Tid_of(i), -1);
	info = pvm_upkint(&syncid_recv, 1, 1);
	if (syncid != syncid_recv) {
	    klic_fprintf(stderr,
			 "IOnode sync-error expected %d but %d at pe%d\n",
			 syncid, syncid_recv, i);
	    exit(1);
	}
    }
}
void
child_wait_sync(syncid)
int syncid;
{
    declare_globals;
    int info;
    int syncid_recv;
    info = pvm_recv(Tid_of_io(), -1);
    info = pvm_upkint(&syncid_recv, 1, 1);
    if (syncid != syncid_recv) {
	klic_fprintf(stderr, "%d: sync-error expected %d but %d\n",
		     my_node, syncid, syncid_recv);
	exit(1);
    }
}
void
io_sync(syncid)
int syncid;
{
    int i;
    int info;
    info = pvm_initsend(PvmDataRaw);
    info = pvm_pkint(&syncid, 1, 1);
    for (i = 0; i < tid_table.child_count; ++i) {
	int info;
	info = pvm_send(Tid_of(i), 1);
    }
}

static void
sigint_handler()
{
    klic_fprintf(stdout, "kill tasks in children_spawn()\n");
    kill_child();
    exit(1);
}

static char *
expand_prog_name(buffer, progname)
char *buffer;
char *progname;
{
    char dir[256];
    if (rel_spawn_path || progname[0] == '/') {
	strcpy(buffer, progname);
    } else {
	getcwd(dir, 255);
	sprintf(buffer, "%s/%s", dir, progname);
    }
    if (network_statistics_flag)
      klic_fprintf(stderr, "%s\n", buffer);
    return buffer;
}

static void
setup_send_args(opttable)
Const struct opttable *opttable;
{
    int i;
    for (i = 0; opttable[i].opt_str; ++i) {
	if (ARG_SHOULD_NOT_COPY(opttable[i].opt_type))
	  continue;
	switch (ARG_ORIGINAL_TYPE(opttable[i].opt_type)) {
	 case ARG_DOUBLE:
	    pvm_pkdouble(&(opttable[i].opt_dest->d), 1, 1);
	    break;
	 case ARG_SIZE:
	    pvm_pklong(&(opttable[i].opt_dest->l), 1, 1);
	    break;
	 case ARG_RESET:
	 case ARG_SET:
	    pvm_pkint(&(opttable[i].opt_dest->i), 1, 1);
	    break;
	 case ARG_STRING:
	    {
		int len;
		if (!opttable[i].opt_dest->s) {
		    /* When the option is not set */
		    len = -1;
		    pvm_pkint(&len, 1, 1);
		} else {
		    len = strlen(opttable[i].opt_dest->s);
		    pvm_pkint(&len, 1, 1);
		    pvm_pkstr(opttable[i].opt_dest->s);
		}
	    }
	    break;
	}
    }
}

static void
recv_extract_args(opttable)
Const struct opttable *opttable;
{
    int i;
    for (i = 0; opttable[i].opt_str; ++i) {
	if (ARG_SHOULD_NOT_COPY(opttable[i].opt_type))
	  continue;
	switch (ARG_ORIGINAL_TYPE(opttable[i].opt_type)) {
	 case ARG_DOUBLE:
	    pvm_upkdouble(&(opttable[i].opt_dest->d), 1, 1);
	    OPT_DEBUG(klic_fprintf(stderr, "%s=%f\n", opttable[i].opt_str,
				   opttable[i].opt_dest->d));
	    break;
	 case ARG_SIZE:
	    pvm_upklong(&(opttable[i].opt_dest->l), 1, 1);
	    OPT_DEBUG(klic_fprintf(stderr, "%s=%ld\n",
				   opttable[i].opt_str,
				   opttable[i].opt_dest->l));
	    break;
	 case ARG_RESET:
	 case ARG_SET:
	    pvm_upkint(&(opttable[i].opt_dest->i), 1, 1);
	    OPT_DEBUG(klic_fprintf(stderr, "%s=%d\n",
				   opttable[i].opt_str,
				   opttable[i].opt_dest->i));
	    break;
	 case ARG_STRING:
	    {
		int len;
		char *recv_str;
		pvm_upkint(&len, 1, 1);
		if (len < 0)
		  /* When the option is not set */
		  opttable[i].opt_dest->s = NULL;
		else {
		    recv_str = malloc(len+1);
		    pvm_upkstr(recv_str);
		    opttable[i].opt_dest->s = recv_str;
		    OPT_DEBUG(klic_fprintf(stderr,
					   "%s=%s\n", opttable[i].opt_str,
					   opttable[i].opt_dest->s));
		}
	    }
	    break;
	}
    }
}

static int
io_spawn_children(num_child, opttable, prog_name, argc, argv)
int num_child;
Const struct opttable *opttable;
char *prog_name;
int argc;
char **argv;
{
    char binname[256];
    char arg_buf[20];
    int ntask;
    int *pid_tbl;
    int i;

    pid_tbl = (int *)calloc(num_child, sizeof(int));

    Tid_of_io() = pvm_mytid();
    tid_table.child = (int *)calloc(num_child, sizeof(int));
    tid_table.child_count = num_child;
    if (Tid_of_io() < 0) {
	klic_fprintf(stderr, "PVM has not been started!!\n");
	exit(1);
    }
    expand_prog_name(binname, prog_name);
    sprintf(arg_buf, "-%s%d", OPT_PARENT_TID, Tid_of_io());
    {
      char **sargv = (char **)malloc(sizeof(char *) * (argc + 3));
      sargv[0] = arg_buf;
      sargv[1] = "--";
      for (i = 0; i < argc; ++i)
	sargv[2+i] = argv[i];
      sargv[2+argc] = 0;
      /*  preliminary patch for host configuration */
      {
	char **htable;
	if (useconfig_flag && (htable = gethostnametable())) {
	  if (!pvm_spawn(binname, sargv, PvmTaskHost, htable[0], 1,
			 &(tid_table.child[0])) ) {
	    klic_fprintf(stderr, "Can't execute tasks!!\n");
	    exit(1);
	  }
	  switch (nhostnames) {
	  case 1:
	    ntask = pvm_spawn(binname, sargv, PvmTaskDefault, 0,
			      num_child - 1, &(tid_table.child[1]));
	    if (ntask != num_child - 1) {
	      klic_fprintf(stderr, "Can't execute tasks!!\n");
	      kill_child();
	      exit(1);
	    }
	    break;
	  default:
	    for (i = 0; i < num_child - 1; i++) {
	      if (0 == pvm_spawn(binname, sargv, PvmTaskHost,
				 htable[i % (num_child - 1) + 1 ], 1,
				 &(tid_table.child[i+1]))) {
		klic_fprintf(stderr, "Can't execute tasks!!\n");
		kill_child();
		exit(1);
	      }
	    }
	  }
	  free(htable);
	} else {
	  ntask = pvm_spawn(binname, sargv, PvmTaskDefault, 0,
			    num_child , tid_table.child);
	  if (ntask != num_child) {
	    klic_fprintf(stderr, "Can't execute tasks!!\n");
	    kill_child();
	    exit(1);
	  }
	}
      }
      signal(SIGINT, sigint_handler);
      free(sargv);
    }

    for (i = 0; i < num_child; i++) {
	int his_tid;
	/* accept initial greetings of child */
	pvm_recv(Tid_of(i), -1);
	pvm_upkint(&his_tid,1,1);
	pvm_upkint(&pid_tbl[i],1,1);
	if (his_tid != Tid_of(i)) {
	    klic_fprintf(stderr, "%dth child tid mismatch(%d != %d)\n",
			 i, his_tid, Tid_of(i));
	    kill_child();
	    exit(1);
	}

	pvm_initsend(PvmDataRaw);
	pvm_pkint(&num_child, 1, 1);
	pvm_pkint(&Tid_of(0), num_child, 1);
	setup_send_args(opttable);
	pvm_send(Tid_of(i), 3);

    }
    /* receive ack from all children */
    io_wait_sync(SYNCID_SPAWN);
    if (network_statistics_flag) {
	klic_fprintf(stderr, "reduction_node:\n");
	for (i = 0; i < num_child; ++i) {
	    if (i == num_child - 1)
	      klic_fprintf(stderr, "shoen_node:\n");
	    klic_fprintf(stderr,"\tPE = %d,\tTID = %x,\tPID = %d\n",
			 i, Tid_of(i), pid_tbl[i]);
	}
    }
    free(pid_tbl);
    { /* send start-semaphore to the master node */
	int start_semaphore = 0;
	pvm_initsend(PvmDataRaw);
	pvm_pkint(&start_semaphore, 1, 1);
	pvm_send(Tid_of(MASTER_NODE), 3);
    }
}

static int
child_spawn_children(par_tid, opttable, prog_name, argc, argv)
int par_tid;
Const struct opttable *opttable;
char *prog_name;
int argc;
char **argv;
{
    int mytid;
    int num_child;
    int pid;
    declare_globals;

    Tid_of_io() = par_tid;

    mytid = pvm_mytid();
    /* initial greetings */
    pvm_initsend(PvmDataRaw);
    pvm_pkint(&mytid, 1, 1);
    pid = getpid();
    pvm_pkint(&pid, 1, 1);
    pvm_send(Tid_of_io(), 3);

    pvm_recv(Tid_of_io(), -1);
    pvm_upkint(&num_child, 1, 1);
    tid_table.child_count = num_child;
    tid_table.child = (int *)calloc(num_child, sizeof(int));
    pvm_upkint(&Tid_of(0), num_child, 1);

    recv_extract_args(opttable);

    /* send ack to io node */
    child_sync(SYNCID_SPAWN);
    {
	int i;
	/* find my tid from the table sent */
	for (i = 0; i < num_child; ++i) 
	  if (Tid_of(i) == mytid) {
	      if (IS_MASTER_NODE(i)) {
		  /* master node must receive the start-semaphore from
		     io_node */
		  int start_semaphore;
		  pvm_recv(Tid_of_io(), -1);
		  pvm_upkint(&start_semaphore, 1, 1);
	      }
	      return i;
	  }
	/* I cannot find my tid in the child-tid table??? */
	kill_child();
	abort();
	return -1;
    }
}

int
spawn_children(num_child, opttable, prog_name, argc, argv)
int num_child;
Const struct opttable *opttable;
char *prog_name;
int argc;
char **argv;
{
    declare_globals;
#if defined(VPP) || defined(AP3000)
    int mytid = pvm_mytid();
    if ( pvm_parent() == PvmNoParent )
#else
    if ( parent_tid == 0 ) 
#endif
      {	/* original process */
#ifdef AP3000
        extern int main();
	parent_tid = 0;
        __pvm_set_node_name(main);
#endif
	io_spawn_children(num_child, opttable, prog_name, argc, argv);
	return 0;
    } else {
	/* at this poine, don't rely on the value num_child for child.
	   the value is transfered by send_args/recv_args in the function */
	int my_penum =
	  child_spawn_children(parent_tid, opttable, prog_name, argc, argv);
#ifdef USEPROCBIND
/*	if (!no_procbind && !IS_SHOEN_NODE(my_penum)) {*/
	if (!no_procbind){
	    /* fix process to the pe */
	    processorid_t obind;
	    long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
	    processor_bind(P_PID, P_MYID,
			   (processorid_t)(my_penum % nprocs), &obind);
	}
#endif
	return my_penum + 1;
    }
}

void
count_profile()
{
    perfmon_counter.intr_count++;
    if (perfmon_state.idle_on) {
	perfmon_counter.idle_count++;
	return;
    }
    if (perfmon_state.ms_on) {
	perfmon_counter.ms_count++;
	return;
    }
    if (perfmon_state.mr_on) {
	perfmon_counter.mr_count++;
	return;
    }
    if (perfmon_state.gc_on) {
	perfmon_counter.gc_count++;
	return;
    }
}

#if 0
myexit(char *s, int *l, int *c)
{
    FILE *f;
    char buf[10];
    sprintf(buf,"/tmp/%d",getpid());
    f=fopen(buf,"w");
    klic_fprintf(f, "exit(pid=%d,my_node=%d) %s:%d\n",
		 getpid(), my_node,  s, l);
    fclose(f);
    exit(c);
}
#endif
