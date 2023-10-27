/*
 * Author:	George Carrette, <GJC@MITECH.COM> in November, 1990.
 *
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/vms_comp.c,v 1.1 2004/06/18 23:19:10 william Exp $
 */

/* vms_comp routines, generally useful functions
   to aid in porting/running various common Unix style code under VAX/VMS.

   This code has been sufficient to run programs from sources
   such as COMP.SOURCES.X, in particular XV and TGIF.
  
   Some routines are for direct unix compatibility and have the same
   exact calling sequence and names as in various versions of unix.
   Some are vms specific and are intended to be called from generally 
   unix-style programs inside a minimal amount of {#ifdef VMS #endif}
   conditionalized code.

   **  Written by George Carrette, <GJC@MITECH.COM> in November, 1990.
   **  Includes modified VMS readdir() routines.
   **  Written by Rich $alz, <rsalz@bbn.com> in August, 1990.
   **  some additions by Rolf Niepraschk, niepraschk@ptb.de, 1997/11/24

To use:

#include "vms_comp.h"

Used generally where you see #include <sys/dir.h>.

Contains:
 bcopy    ... full functionality
 rindex   ... full functionality
 lstat    ... full functionality, just a name change
 fork     ... dummy, returns 1.
 getwd    ... full functionality, returns unix style pathname.
 unlink   ... full functionality(?), just a name change
 popen    ... ?.
 pclose   ... ?.
 opendir  ... almost full functionality.
 closedir ... almost full functionality.
 readdir  ... almost full functionality.
 telldir  ... almost full functionality.
 seekdir  ... almost full functionality.
 ioctl    ... dummy, returns 0. -- RN --
 fcntl    ... dummy, returns 0. -- RN --

 do_vms_wildcard ... non-unix. but put this into your MAIN program to
                     get limited wildcard processing on the command line.
                     Currently returns vms style pathnames.

*/

/* some of these unix compat functions are just dummy
   versions to get the code running. some are fully functional.
   the use of fork and popen/pclose in particular
   assume unix command shell as used in the main code.
   And since this sort of thing is generally done for the purposes
   such as printing, spooling mail, etc, which are best done
   in considerably different ways under VMS, it is better to
   dummy this stuff up now and then restructure the code that
   calls this sort of thing to be more abstract/subroutinized
   and then conditionalize it specifically for VMS
*/

#include "vms_comp.h"

#ifdef VMS

#if 0                    /* RN */
bcopy(x,y,n)
     char *x,*y;
     long n;
{memmove(x,y,n);}

char *rindex(p,c)
     char *p;
     int c;
{return(strrchr(p,c));}

int lstat(x,y)
{return(stat(x,y));}
#endif                   /* RN */

fork()
{return(1);}

static char *getwd_tmp = NULL;

char *getwd(p)
     char *p;
{int c;
 char *l1,*l2;
 if (getwd_tmp == NULL)
   getwd_tmp = (char *) malloc(512);
 l1 = getwd_tmp;
 getcwd(l1,512);
 l2 = p;
 *l2++ = '/';
 while((c = *l1++))
   switch(c)
     {case '.':
      case '[':
	*l2++ = '/';
	break;
      case ']':
      case ':':
	break;
      default:
	*l2++ = c;}
 *l2 = 0;
 return(p);}

#if 0                    /* RN */
unlink(p)
     char *p;
{delete(p);}
#endif                   /* RN */

static vms_wild_putargs(),vms_wild_put_one(),vms_wild_put_wild();
static struct dsc$descriptor *set_dsc();
static struct dsc$descriptor *set_dsc_cst();

do_vms_wildcard(pargc,pargv)
     int *pargc;
     char ***pargv;
{int j,vsize;
 int argc; char **argv;
 argc = *pargc;
 argv = *pargv;
 *pargc = 0;
 vsize = 3;
 *pargv = (char **) malloc(sizeof (char *) * vsize);
 for(j=0;j<argc;++j)
   vms_wild_putargs(argv[j],pargc,pargv,&vsize);}

static vms_wild_putargs(s,pargc,pargv,pvsize)
     char *s; int *pargc; char ***pargv; int *pvsize;
{if (!strchr(s,'*'))
   vms_wild_put_one(s,pargc,pargv,pvsize);
 else
   vms_wild_put_wild(s,pargc,pargv,pvsize);}

static vms_wild_put_one(s,pargc,pargv,pvsize)
     char *s; int *pargc; char ***pargv; int *pvsize;
{int nvsize,i;
 char ** nargv;
 if (*pargc == *pvsize)
   {nvsize = 2 * *pvsize;
    nargv = (char **) malloc(sizeof (char *) * nvsize);
    for(i=0;i < *pargc; ++i) nargv[i] = (*pargv)[i];
    free(*pargv);
    *pargv = nargv;
    *pvsize = nvsize;}
 (*pargv)[(*pargc)++] = s;}


static struct dsc$descriptor *set_dsc(x,buff,len)
     struct dsc$descriptor *x;
     char *buff;
     int len;
{(*x).dsc$w_length = len;
 (*x).dsc$a_pointer = buff;
 (*x).dsc$b_class = DSC$K_CLASS_S;
 (*x).dsc$b_dtype = DSC$K_DTYPE_T;
 return(x);}

static struct dsc$descriptor *set_dsc_cst(x,buff)
     struct dsc$descriptor *x;
     char *buff;
{(*x).dsc$w_length = strlen(buff);
 (*x).dsc$a_pointer = buff;
 (*x).dsc$b_class = DSC$K_CLASS_S;
 (*x).dsc$b_dtype = DSC$K_DTYPE_T;
 return(x);}

static vms_wild_put_wild(s,pargc,pargv,pvsize)
     char *s; int *pargc; char ***pargv; int *pvsize;
{struct dsc$descriptor fnamed,foutd,rfnamed;
 char *ns,*p;
 int rval;
 long context;
 set_dsc_cst(&rfnamed,";");
 set_dsc_cst(&fnamed,s);
 set_dsc(&foutd,0,0);
 foutd.dsc$b_class = DSC$K_CLASS_D;
 context = 0;
 while(1)
  {rval = lib$find_file(&fnamed,&foutd,&context,0,&rfnamed,0,0);
   if (rval == RMS$_NMF) break;
   if (rval == RMS$_FNF) break;
   if (rval != RMS$_NORMAL) exit(rval);
   ns = (char *) malloc(foutd.dsc$w_length + 1);
   ns[foutd.dsc$w_length] = 0;
   memcpy(ns,foutd.dsc$a_pointer,foutd.dsc$w_length);
   if (p = strchr(ns,']')) ns = p+1;
   if (p = strchr(ns,';')) *p = 0;
   vms_wild_put_one(ns,pargc,pargv,pvsize);}
 if (foutd.dsc$a_pointer) lib$sfree1_dd(&foutd);
 if (context)
   {rval = lib$find_file_end(&context);
    if (rval != SS$_NORMAL) exit(rval);}}

/* globalvalue CLI$M_NOWAIT; -- RN */

static int create_mbx();

#define mailbox_size (512)
#define mailbox_byte_quota (3*mailbox_size)
#define mailbox_protection_mask (0x0000F000)

struct popen_cell
{FILE *fp;
 char *mbx_name;
 short mbx_chan;
 long pid;
 long completed;
 long comp_status;
 struct popen_cell *next;
 struct popen_cell *prev;};

#if __VMS_VER < 70000000 /* RN */
static struct popen_cell *popen_list = NULL;

static struct popen_cell *find_popen_cell(fp)
     FILE *fp;
{struct popen_cell *l;
 for(l=popen_list;l != NULL; l = l->next)
   if (l->fp == fp) return(l);
 return(NULL);}

void p_describe(fp)
     FILE *fp;
{struct popen_cell *cell;
 if (!(cell = find_popen_cell(fp)))
   {printf("File pointer is not from popen, or it has been closed\n");
    return;}
 printf("FILE *fp                = %08X\n",cell->fp);
 printf("char *mbx_name          = %s\n",cell->mbx_name);
 printf("short mbx_chan          = %d\n",cell->mbx_chan);
 printf("long pid                = %08X\n",cell->pid);
 printf("long completed          = %d\n",cell->completed);
 printf("long comp_status        = %d\n",cell->comp_status);
 printf("struct popen_cell *next = %08X\n",cell->next);
 printf("struct popen_cell *prev = %08X\n",cell->prev);}

static void proc_exit_ast(cell)
     struct popen_cell *cell;
{cell->completed = 1;}

static void pclose_cleanup(cell)
     struct popen_cell *cell;
{sys$dassgn(cell->mbx_chan);
 free(cell->mbx_name);
 if (!cell->completed)
   sys$delprc(&cell->pid,0);
 memset(cell,0,sizeof(struct popen_cell));
 free(cell);}

static void pclose_delq(cell)
     struct popen_cell *cell;
{if (cell->prev)
   {cell->prev->next = cell->next;
    if (cell->next)
      cell->next->prev = cell->prev;}
 else
   {popen_list = cell->next;
    if (cell->next)
      cell->next->prev = NULL;}}

static void popen_push(cell)
     struct popen_cell *cell;
{if (popen_list)
   popen_list->prev = cell;
 cell->prev = NULL;
 cell->next = popen_list;
 popen_list = cell;}

/* 17-APR-1991 -GJC@MITECH.COM  version 1.0
               Implement unix popen and pclose in vms by using mailboxes.
 */

/* popen starts a subprocess and opens a pipe to its stdout or stdin
   (mode = "r" its stdout, = "w" its stdin)
   */

FILE *popen(command,mode)
     char *command,*mode;
{char *temp;
 struct popen_cell *cell;
 int readp,n,mask,ret;
 char *name,*prompt,*in,*out;
 struct dsc$descriptor comm_d,in_d,out_d,name_d,prompt_d;
 
 if (strcmp(mode,"r") == 0)
   readp = 1;
 else if (strcmp(mode,"w") == 0)
   readp = 0;
 else
   return(NULL);

 temp = mktemp("POPEN_MB_XXXXXXXXXX");
 n = strlen(temp);
  cell =  (struct popen_cell *) malloc(sizeof(struct popen_cell));
 cell->mbx_name = (char *) malloc(n+1);
 strcpy(cell->mbx_name,temp);
 if ((cell->mbx_chan = create_mbx(cell->mbx_name)) < 0)
   {cell->completed = 1;
    pclose_cleanup(cell);
    return(NULL);}

 if (readp)
   {in = "NL:";
    out = cell->mbx_name;}
 else
   {in = cell->mbx_name;
    out = "NL:";}

 name = 0;
 prompt = 0;
 mask = CLI$M_NOWAIT;

 cell->completed = 0;

 ret = lib$spawn((command) ? set_dsc_cst(&comm_d,command) : 0,
                 (in) ? set_dsc_cst(&in_d,in) : 0,
                 (out) ? set_dsc_cst(&out_d,out) : 0,
                 &mask,
                 (name)  ? set_dsc_cst(&name_d,name) : 0,
                 &cell->pid,
                 &cell->comp_status,
                 0, /* event flag */
		 proc_exit_ast,
		 cell,
                 (prompt) ? set_dsc_cst(&prompt_d,prompt) : 0,
                 0 /* cli */
                 );
 
 if (ret != SS$_NORMAL)
   {cell->completed = 1;
    pclose_cleanup(cell);
    return(NULL);}

 if (!(cell->fp = fopen(cell->mbx_name,mode)))
   {pclose_cleanup(cell);
    return(NULL);}

 popen_push(cell);

 return(cell->fp);}

pclose(fp)
     FILE *fp;
{int i;
 struct popen_cell *cell;
 i = fclose(fp);
 if (cell = find_popen_cell(fp))
   {pclose_delq(cell);
    pclose_cleanup(cell);}
 return(i);}

static int create_mbx(name)
  char *name;
{short chan;
 int prmflg,maxmsg,bufquo,promsk,acmode,iflag,retval;
 struct dsc$descriptor lognam;
 prmflg = 0;
 maxmsg = mailbox_size;
 bufquo = mailbox_byte_quota;
 promsk = mailbox_protection_mask;
 acmode = 0;
 set_dsc_cst(&lognam,name);
 retval = sys$crembx(prmflg,&chan,maxmsg,bufquo,promsk,acmode,&lognam);
 if (retval != SS$_NORMAL) return(-1);
 return(chan);}

#endif /* __VMS_VER < 70000000 */ /* RN */

/*
**  VMS readdir() routines.
**  Written by Rich $alz, <rsalz@bbn.com> in August, 1990.
**  This code has no copyright.
*/

/* 12-NOV-1990 added d_namlen field and special case "." name -GJC@MITECH.COM 
               added unlink and getwd, also typically needed to port
               unix style code that does directory manipulation.
   26-MAR-1991 added lowercasing of d_name. -GJC@MITECH.COM typically
               strcmp and other operations may be done on the names.
               added a lot more hair to fixunixname.
 */

    /* Uncomment the next line to get a test routine. */
/*#define TEST*/

    /* Number of elements in vms_versions array */
#define VERSIZE(e)	(sizeof e->vms_versions / sizeof e->vms_versions[0])

static char *fixunixname(name)
     char *name;
     /* This handles special cases such as "." and ".."
	and also undoes some of the stuff that getwd does.
	*/
{FILE *f;
 char *p;
 static char *tmp = NULL;
 if (strcmp(".",name) == 0) return("");
 if (strcmp("..",name) == 0) return("[-]");
 if (strchr(name,':') ||
     strchr(name,'[') ||
     strchr(name,']'))
   return(name);
 if (tmp == NULL)
   tmp = (char *) malloc(128);
 if (name[0] != '/')
   /* foo/bar/baz => [.foo.bar.baz] */
   {strcpy(tmp,"[.");
    strcat(tmp,name);
    p = &tmp[strlen(tmp) - 1];
    if (*p == '/') *p = 0;
    strcat(tmp,"]");
    for(p=tmp;*p;++p) if (*p == '/') *p = '.';
    return(tmp);}
 if ((name[0] == '/') && !strchr(name+1,'/'))
   /* /foo => foo:[000000] */
   {strcpy(tmp,name+1);
    strcat(tmp,":[000000]");
    return(tmp);}
 /* /foo/bar/baz => foo:[bar.baz] */
 p = strchr(name+1,'/');
 *p = 0;
 strcpy(tmp,name+1);
 strcat(tmp,":[");
 strcat(tmp,p+1);
 p = &tmp[strlen(tmp) - 1];
 if (*p == '/') *p = 0;
 strcat(tmp,"]");
 for(p=tmp;*p;++p) if (*p == '/') *p = '.';
 return(tmp);}


/*
**  Open a directory, return a handle for later use.
*/

static char *opendir_pat = "*.*";

DIR *opendir(name)
    char	*name;
{DIR *dd;
 /* Get memory for the handle, and the pattern. */
 if ((dd = (DIR *)malloc(sizeof *dd)) == NULL)
   {errno = ENOMEM;
    return NULL;}

 name = fixunixname(name);
    
 dd->pattern = (char *) malloc((unsigned int)(strlen(name) +
					      strlen(opendir_pat) +
					      1));
 if (dd->pattern == NULL)
   {free((char *)dd);
    errno = ENOMEM;
    return NULL;}

 /* Fill in the fields; mainly playing with the descriptor. */
 strcpy(dd->pattern,name);
 strcat(dd->pattern,opendir_pat);
 dd->context = 0;
 dd->vms_wantversions = 0;
 set_dsc_cst(&dd->pat,dd->pattern);
 return dd;}


/*
**  Set the flag to indicate we want versions or not.
*/

void vmsreaddirversions(dd, flag)
     DIR		*dd;
     int		flag;
{dd->vms_wantversions = flag;}


/*
**  Free up an opened directory.
*/

void closedir(dd)
    DIR		*dd;
{if (dd->context)
   lib$find_file_end(&dd->context);
 free(dd->pattern);
 free((char *)dd);}

/*
**  Collect all the version numbers for the current file.
*/

static void collectversions(dd)
     DIR *dd;
{struct dsc$descriptor_s pat;
 struct dsc$descriptor_s res;
 struct dirent *e;
 char *p;
 char buff[sizeof dd->entry.d_name];
 int i;
 char *text;
 long  context;

 /* Convenient shorthand. */
 e = &dd->entry;

 /* Add the version wildcard, ignoring the "*.*" put on before */
 i = strlen(dd->pattern);
 text = (char *) malloc((unsigned int)(i + strlen(e->d_name)+ 2 + 1));
 if (text == NULL)
   return;
 (void)strcpy(text, dd->pattern);
 text[i - strlen(opendir_pat)] = 0;
 strcat(text,e->d_name);
 strcat(text,";*");

 /* Set up the pattern descriptor. */
 set_dsc_cst(&pat,text);

 /* Set up result descriptor. */
 set_dsc(&res,buff,sizeof buff - 2);

 /* Read files, collecting versions. */
 for (context = 0; e->vms_verscount < VERSIZE(e); e->vms_verscount++)
   {if (lib$find_file(&pat, &res, &context) == RMS$_NMF || context == 0)
      break;
    buff[sizeof buff - 1] = '\0';
    if (p = strchr(buff, ';'))
      e->vms_versions[e->vms_verscount] = atoi(p + 1);
    else
      e->vms_versions[e->vms_verscount] = -1;}

 lib$find_file_end(&context);

 free(text);}


static strtolower(s)
     char *s;
{int c;
 char *p;
 p = s;
 while(c = *p)
   {if ((c > 'Z') || (c < 'A'))
      ++p;
    else
      *p++ = c + ('a' - 'A');}}


/*
**  Read the next entry from the directory.
*/
struct dirent *readdir(dd)
    DIR				*dd;
{struct dsc$descriptor_s	res;
 char			*p;
 char			buff[sizeof dd->entry.d_name];
 int				i;
 int status;

 /* Set up result descriptor, and get next file. */
 set_dsc(&res,buff,sizeof buff - 2);
 status = lib$find_file(&dd->pat, &res, &dd->context);
 if (status == RMS$_NMF || dd->context == 0L)
   /* None left... */
   return NULL;

 if ((status != SS$_NORMAL) && (status != RMS$_NORMAL))
   /* May be something like RMS$_SYN for bad syntax patterns */
   return NULL;

 /* Force the buffer to end with a NUL. */
 buff[sizeof buff - 1] = '\0';
 for (p = buff; !isspace(*p); p++);
 *p = '\0';

 /* Skip any directory component and just copy the name. */
 if (p = strchr(buff, ']'))
   (void)strcpy(dd->entry.d_name, p + 1);
 else
   (void)strcpy(dd->entry.d_name, buff);

 /* Clobber the version. */
 if (p = strchr(dd->entry.d_name, ';'))
   *p = '\0';

 dd->entry.d_namlen = strlen(dd->entry.d_name);

 dd->entry.vms_verscount = 0;
 if (dd->vms_wantversions)
   collectversions(dd);

 /* force to lowercase, since many unix programs look for
    specific file extensions, etc, which are almost always in
    lowercase */

 strtolower(dd->entry.d_name);
    
 return &dd->entry;}


/*
**  Return something that can be used in a seekdir later.
*/

long telldir(dd)
     DIR		*dd;
{return dd->context;}


/*
**  Return to a spot where we used to be.
*/

void seekdir(dd, pos)
     DIR		*dd;
     long	pos;
{dd->context = pos;}

int ioctl(int fildes, int request,... /* arg */)
{
  fprintf(stderr, "\"ioctl\" not implemented (VMS)\n");
  return 0;
}

int fcntl (int filedes, int request , int argument)
{
  fprintf(stderr, "\"fcntl\" not implemented (VMS)\n");
  return 0;
}



#ifdef	TEST
main()
{
    char		buff[256];
    DIR			*dd;
    struct dirent	*dp;
    int			i;
    int			j;

    for ( ; ; ) {
	printf("\n\nEnter dir:  ");
	(void)fflush(stdout);
	(void)gets(buff);
	if (buff[0] == '\0')
	    break;
	if ((dd = opendir(buff)) == NULL) {
	    perror(buff);
	    continue;
	}

	/* Print the directory contents twice, the second time print
	 * the versions. */
	for (i = 0; i < 2; i++) {
	    while (dp = readdir(dd)) {
		printf("%s%s", i ? "\t" : "    ", dp->d_name);
		for (j = 0; j < dp->vms_verscount; j++)
		    printf("  %d", dp->vms_versions[j]);
		printf("\n");
	    }
	    rewinddir(dd);
	    vmsreaddirversions(dd, 1);
	}
	closedir(dd);
    }
    exit(0);
}
#endif	/* TEST */

#endif  /* VMS */
