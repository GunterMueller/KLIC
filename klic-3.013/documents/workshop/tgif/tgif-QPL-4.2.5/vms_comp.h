/*
 * Author:	George Carrette, <GJC@MITECH.COM>
 *
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/vms_comp.h,v 1.1 2004/06/18 23:19:10 william Exp $
 */

/* Header file for vms_comp routines, generally useful functions
   to aid in porting/running Unix and BSD code under VAX/VMS.

   **  Written by George Carrette, <GJC@MITECH.COM>.
   **  Includes modified VMS readdir() routines.
   **  Written by Rich $alz, <rsalz@bbn.com> in August, 1990.

To use:

#include "vms_comp.h"

*/
#ifndef _VMS_COMP_H_
#define _VMS_COMP_H_

#include <stdio.h>

#ifdef VMS

#include <ctype.h>
#include <errno.h>
#include <descrip.h>
#include <string.h>
#include <stdlib.h>       /* RN */
#include <stat.h>         /* RN */

#include <unixio.h>       /* RN */
#include <unistd.h>       /* RN */
#include <rmsdef.h>
#include <ssdef.h>
#include <clidef.h>       /* RN */
#include <lib$routines.h> /* RN */
#include <starlet.h>      /* RN */

#if __VMS_VER < 70000000 /* RN */
#  define bcopy memmove
#  define rindex strrchr
#endif

#define lstat stat
#define unlink delete

#ifndef DESCRIP_H_DEFINED
#include <descrip.h>
#endif

int ioctl(int fildes, int request,... /* arg */); /* RN */

/* 12-NOV-1990 added d_namlen field -GJC@MITECH.COM */

    /* Data structure returned by READDIR(). */
struct dirent {
    char	d_name[100];		/* File name		*/
    int         d_namlen;
    int		vms_verscount;		/* Number of versions	*/
    int		vms_versions[20];	/* Version numbers	*/
};

    /* Handle returned by opendir(), used by the other routines.  You
     * are not supposed to care what's inside this structure. */
typedef struct _dirdesc {
    long			context;
    int				vms_wantversions;
    char			*pattern;
    struct dirent		entry;
    struct dsc$descriptor_s	pat;
} DIR;

/* Another name for this? Used by TGIF */

struct direct {
    char	d_name[100];		/* File name		*/
    int         d_namlen;
    int		vms_verscount;		/* Number of versions	*/
    int		vms_versions[20];	/* Version numbers	*/
};


#define rewinddir(dirp)		seekdir((dirp), 0L)

extern DIR		*opendir();
extern struct dirent	*readdir();
extern long		telldir();
extern void		seekdir();
extern void		closedir();
extern void		vmsreaddirversions();

#endif /* VMS */

#endif /*_VMS_COMP_H_*/
