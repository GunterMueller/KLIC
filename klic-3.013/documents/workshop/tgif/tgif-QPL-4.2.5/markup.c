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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/markup.c,v 1.6 2011/05/16 16:21:58 william Exp $
 */

#define _INCLUDE_FROM_MARKUP_C_

#include "tgifdefs.h"

#include "file.e"
#include "http.e"
#include "markup.e"
#include "msg.e"
#include "setup.e"
#include "util.e"

int cmdLineParseHtml=FALSE;

static int parseHtml=FALSE;

/* ===================== Parse Routines ===================== */

static
int DumpHtmlBuf(in_tag, psz_start)
   int in_tag;
   char *psz_start;
{
   int last_ch_is_lf=FALSE;
   char *psz=NULL;

   if (in_tag) {
      for (psz=psz_start; *psz != '\0'; psz++) {
         switch (*psz) {
         case '\r':
            if (psz[1] == '\n') {
               psz++;
            }
            printf("\n");
            last_ch_is_lf = TRUE;
            break;
         case '\n':
            printf("\n");
            last_ch_is_lf = TRUE;
            break;
         default:
            fputc(*psz, stdout);
            last_ch_is_lf = FALSE;
            break;
         }
      }
   } else {
      for (psz=psz_start; *psz != '\0'; psz++) {
         switch (*psz) {
         case '\r':
            if (psz[1] == '\n') {
               psz++;
            }
            printf("\n");
            last_ch_is_lf = TRUE;
            break;
         case '\n':
            printf("\n");
            last_ch_is_lf = TRUE;
            break;
         default:
            fputc(*psz, stdout);
            last_ch_is_lf = FALSE;
            break;
         }
      }
   }
   if (!last_ch_is_lf) {
      printf("\n");
   }
   return TRUE;
}

static
int DoParseMarkUpFile(buf)
   char *buf;
{
   int in_tag=FALSE; /* TRUE if inside a tag (and looking for '>') */
   char *psz_start=buf;

   in_tag = (*psz_start == '<');
   while (psz_start != NULL && *psz_start != '\0') {
      char *psz=NULL, saved_ch='\0';

      if (in_tag) {
         if ((psz=strchr(psz_start, '>')) == NULL) break;

         saved_ch = (*(++psz));
         *psz = '\0';
         DumpHtmlBuf(in_tag, psz_start);
         *psz = saved_ch;
         in_tag = FALSE;
      } else {
         if ((psz=strchr(psz_start, '<')) == NULL) break;

         *psz = '\0';
         DumpHtmlBuf(in_tag, psz_start);
         *psz = '<';
         in_tag = TRUE;
      }
      psz_start = psz;
   }
   if (psz_start != NULL) {
      DumpHtmlBuf(in_tag, psz_start);
   }
   printf("\n");
   fflush(stdout);

   return TRUE;
}

int ParseMarkUpFile(buf, buf_sz, pn_html, psz_content_type)
   char *buf, *psz_content_type;
   int buf_sz, *pn_html;
{
   if (parseHtml) {
      if (*pn_html || *buf == '>') {
         if (!DoParseMarkUpFile(buf)) {
         }
      }
   }
   return TRUE;
}

/* ===================== Init & CleanUp Routines ===================== */

void CleanUpHtml()
{
}

int InitHtml()
{
   parseHtml = FALSE;
   if (!PRTGIF || cmdLineOpenDisplay) {
      char *c_ptr=FALSE;

      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "ParseHtml")) !=
            NULL && UtilStrICmp(c_ptr, "true") == 0) {
         parseHtml = TRUE;
      }
   }
   if (PRTGIF && cmdLineParseHtml) {
      parseHtml = TRUE;
   }
   return TRUE;
}

