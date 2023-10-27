/*
 * Author:      William Chia-Wei Cheng (bill.cheng@acm.org)
 *
 * Copyright (C) 2001-2009, William Chia-Wei Cheng.
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE.QPL included in the packaging of this file.
 *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgifdefs.h,v 1.10 2011/05/16 16:22:00 william Exp $
 */

#ifndef _TGIFDEFS_H_
#define _TGIFDEFS_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifdef HAVE_GETTIMEOFDAY
#undef _NO_GETTIMEOFDAY
#endif /* HAVE_GETTIMEOFDAY */

#ifdef _NO_GETTIMEOFDAY
#include <sys/timeb.h>
#endif /* _NO_GETTIMEOFDAY */

#ifdef isc /* SunSoft/Interactive UNIX */
#include <sys/errno.h>
#include <sys/bsdtypes.h>
#endif /* isc */

#if !defined(FD_SET)
#include <sys/select.h>
#endif /* !defined(FD_SET) */

#ifdef HAVE_CONFIG_H
/*
 * If HAVE_CONFIG_H is defined, it means that "configure" (and not imake)
 *       should be used to build tgif.
 */
#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#define DIR_ENTRY struct direct
#else /* ~HAVE_SYS_DIR_H */
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#define DIR_ENTRY struct dirent
#else /* ~HAVE_DIRENT_H */
#include "vms_comp.h"
#define DIR_ENTRY struct dirent
#endif /* HAVE_DIRENT_H */
#endif /* HAVE_SYS_DIR_H */
#else /* ~HAVE_CONFIG_H */
/*
 * If HAVE_CONFIG_H is not defined, it means that tgif should
 *       be built with imake (and not with "configure").
 */
#ifdef VMS
#include "vms_comp.h"
#define DIR_ENTRY struct dirent
#else /* ~VMS */
#ifdef ibm
#include <sys/dir.h>
#define DIR_ENTRY struct direct
#else /* ~ibm */
#ifdef apollo
#include <sys/dir.h>
#define DIR_ENTRY struct direct
#else /* ~apollo */
#ifdef NeXT
#include <sys/dir.h>
#define DIR_ENTRY struct direct
#else /* ~NeXT */
#ifdef luna88k
#include <sys/dir.h>
#define DIR_ENTRY struct direct
#else /* ~luna88k */
#ifdef sequent
#include <sys/dir.h>
#define DIR_ENTRY struct direct
#else /* ~sequent */
#include <dirent.h>
#define DIR_ENTRY struct dirent
#endif /* sequent */
#endif /* luna88k */
#endif /* NeXT */
#endif /* apollo */
#endif /* ibm */
#endif /* VMS */
#endif /* HAVE_CONFIG_H */

#ifdef __NetBSD__
#if !defined(__ELF__) && !defined(mips)
#define _DL_EXPORT_UNDERSCORE
#else /* ~(!defined(__ELF__) && !defined(mips)) */
#ifdef _DL_EXPORT_UNDERSCORE
#undef _DL_EXPORT_UNDERSCORE
#endif /* _DL_EXPORT_UNDERSCORE */
#endif /* !defined(__ELF__) && !defined(mips) */
#endif /* __NetBSD__ */

#ifdef NOT_DEFINED
/*
 * On some Mac OS X systems, libdl.dylib exists but there is no <dlcfn.h>.
 *     Not sure if someone deleted <dlfcn.h> or it's a problem with the
 *     system.
 */
#ifdef HAVE_LIBDL
#undef _NO_DL_SUPPORT
#endif /* HAVE_LIBDL */
#endif /* NOT_DEFINED */

#ifndef _NO_DL_SUPPORT
#include <dlfcn.h>
#ifndef RTLD_GLOBAL
#define RTLD_GLOBAL 0
#endif /* ~RTLD_GLOBAL */
#endif /* ~_NO_DL_SUPPORT */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */

#ifdef _ENABLE_NLS
#ifndef ENABLE_NLS
#define ENABLE_NLS
#endif /* ~ENABLE_NLS */
#endif /* _ENABLE_NLS */

#ifndef _NO_LOCALE_SUPPORT
#ifdef ENABLE_NLS
#include <libintl.h>
#endif /* ENABLE_NLS */
#endif /* ~_NO_LOCALE_SUPPORT */

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

#ifndef _NO_LOCALE_SUPPORT
#include <X11/Xlocale.h>
#endif /* ~_NO_LOCALE_SUPPORT */

#include "const.h"
#include "types.h"

#endif /*_TGIFDEFS_H_*/
