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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/prtgif.c,v 1.6 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_PRTGIF_C_

#define PRTGIF_NO_TGIF_DBG 1

#include "tgifdefs.h"

#include "prtgif.e"

int main(argc, argv)
   int	argc;
   char	* argv[];
{
   register int	i;
   int		total_len;
   char		tgif_path[MAXSTRING+1], print_cmd[MAXSTRING+1], * cmd, * c_ptr;
   char		tmp_str[MAXSTRING+1], **s_ptr;
   FILE		* fp;

   if ((c_ptr=getenv("TGIFPATH")) == NULL) {
      strcpy(tgif_path, TGIF_PATH);
   } else {
      if (strlen(c_ptr) >= MAXSTRING) {
         /* must be an error */
         strcpy(tgif_path, TGIF_PATH);
      } else {
         strcpy(tgif_path, c_ptr);
      }
   }

#ifdef PRINT_CMD
   strcpy(print_cmd, PRINT_CMD);
#else
#ifdef VMS
   strcpy(print_cmd, "print");
#else
#ifdef SYSV
   strcpy(print_cmd, "lp -dpostscript");
#else
   strcpy(print_cmd, "lpr");
#endif /* SYSV */
#endif /* VMS */
#endif /* PRINT_CMD */

   total_len = strlen("tgif ")+strlen("-prtgif ");
   total_len += strlen(tgif_path); total_len += strlen("-tgif_path")+4;
   total_len += strlen(print_cmd); total_len += strlen("-print_cmd")+4;
   for (i=argc-1, s_ptr=(&argv[1]); i > 0; i--, s_ptr++) {
      total_len += strlen(*s_ptr)+1;
   }
   cmd = (char*)malloc((total_len+2)*sizeof(char));
   if (cmd == NULL) {
      fprintf(stderr, "Out of virtual memory and cannot malloc().\n");
   }
   c_ptr = cmd;
   sprintf(c_ptr, "tgif "); c_ptr += strlen("tgif")+1;
   sprintf(c_ptr, "-prtgif "); c_ptr += strlen("-prtgif")+1;
   sprintf(c_ptr, "-tgif_path \"%s\" ", tgif_path);
   c_ptr += strlen(tgif_path)+strlen("-tgif_path")+4;
   sprintf(c_ptr, "-print_cmd \"%s\" ", print_cmd);
   c_ptr += strlen(print_cmd)+strlen("-print_cmd")+4;
   for (argc--, argv++; argc > 0; argc--, argv++) {
      sprintf(c_ptr, "%s ", *argv);
      c_ptr += strlen(*argv)+1;
   }
   *(--c_ptr) = '\0';

   if ((fp=(FILE*)popen(cmd, "r")) == NULL) {
      fprintf(stderr, "Cannot popen() to tgif.  Abort!\n");
      exit (-1);
   }
   while (fgets(tmp_str, MAXSTRING, fp) != NULL) {
      fprintf(stderr, "%s", tmp_str);
   }
   pclose(fp);
   free(cmd);
   return 0;
}
