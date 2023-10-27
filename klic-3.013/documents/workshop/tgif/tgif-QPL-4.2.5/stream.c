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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/stream.c,v 1.6 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_STREAM_C_

#include "tgifdefs.h"

#include "dialog.e"
#include "drawing.e"
#include "file.e"
#include "http.e"
#include "import.e"
#include "menu.e"
#include "move.e"
#include "msg.e"
#include "navigate.e"
#include "obj.e"
#include "remote.e"
#include "setup.e"
#include "stream.e"
#include "strtbl.e"
#include "util.e"

static int gnMultipartReplace=FALSE;
static int gnPossibleMultipartReplace=TRUE;

static int gnJustLF=0;
static int gnHeaderLen=0;
static int gnStartIndex=0;
static int gnBoundaryLen=0;

static char *gpszBoundary=NULL;

static struct ObjRec *gpVideoObj=NULL;

static char **gaszFilterForContentType=NULL;
static int gnMaxStreamFilters=0;

static
void CleanUpFilter()
{
   if (gaszFilterForContentType != NULL) {
      int i=0;

      for (i=0; i < (gnMaxStreamFilters<<1); i++) {
         UtilFree(gaszFilterForContentType[i]);
         UtilFree(gaszFilterForContentType[++i]);
      }
      free(gaszFilterForContentType);
   }
   gaszFilterForContentType = NULL;
   gnMaxStreamFilters = 0;
}

static
void InitFilter()
{
   char *c_ptr=NULL;
   int max_filters=0;

   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"MaxStreamFilters")) != NULL) {
      max_filters = atoi(c_ptr);
      if (max_filters <= 0) {
         sprintf(gszMsgBox, TgLoadString(STID_BAD_XDEF_MUST_BE_GT_0),
               TOOL_NAME, "MaxStreamFilters", c_ptr);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
   }
   gnMaxStreamFilters = 0;
   if (max_filters > 0) {
      int i=0;

      gaszFilterForContentType =
            (char**)malloc(((max_filters<<1)+1)*sizeof(char*));
      if (gaszFilterForContentType == NULL) FailAllocMessage();
      memset(gaszFilterForContentType, 0, ((max_filters<<1)+1)*sizeof(char*));
      for (i=0; i < max_filters; i++) {
         char buf[80];

         sprintf(buf, "StreamFilter%1d", i);
         if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,buf)) == NULL) {
            sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_XDEF), TOOL_NAME,
                  buf);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            max_filters = i;
            break;
         } else {
            char *psz=strchr(c_ptr, ':');
            int index=(i<<1);

            if (psz == NULL) {
               sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF), TOOL_NAME,
                     buf, c_ptr);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               max_filters = i;
               break;
            }
            *psz++ = '\0';
            gaszFilterForContentType[index] = UtilStrDup(c_ptr);
            gaszFilterForContentType[index+1] = UtilStrDup(psz);
            if (gaszFilterForContentType[index] == NULL ||
                  gaszFilterForContentType[index+1] == NULL) {
               FailAllocMessage();
            }
            UtilTrimBlanks(gaszFilterForContentType[index]);
            UtilTrimBlanks(gaszFilterForContentType[index+1]);
         }
      }
      if (max_filters == 0) {
         free(gaszFilterForContentType);
         gaszFilterForContentType = NULL;
      }
   }
   gnMaxStreamFilters = max_filters;
}

void CleanUpMime()
{
   gnMultipartReplace = FALSE;
   gnPossibleMultipartReplace = TRUE;
   gnJustLF = gnHeaderLen = gnStartIndex = gnBoundaryLen = 0;
   UtilFree(gpszBoundary);
   gpszBoundary = NULL;
   if (gpVideoObj != NULL) {
      UnlinkObj(gpVideoObj);
      FreeObj(gpVideoObj);
   }
   gpVideoObj = NULL;
   CleanUpFilter();
}

void InitMime()
{
   gnMultipartReplace = FALSE;
   gnPossibleMultipartReplace = TRUE;
   gnJustLF = gnHeaderLen = gnStartIndex = gnBoundaryLen = 0;
   gpszBoundary = NULL;
   gpVideoObj = NULL;
   InitFilter();
}

void ResetMultipartReplace(redraw)
   int redraw;
{
   gnMultipartReplace = FALSE;
   gnPossibleMultipartReplace = TRUE;
   gnJustLF = gnHeaderLen = gnStartIndex = gnBoundaryLen = 0;
   UtilFree(gpszBoundary);
   gpszBoundary = NULL;
   if (gpVideoObj != NULL) {
      UnlinkObj(gpVideoObj);
      FreeObj(gpVideoObj);
      if (redraw) {
         ClearAndRedrawDrawWindow();
      }
   }
   gpVideoObj = NULL;
}

int IsJustLF(buf)
   char *buf;
{
   char *first_lf=strchr(buf, '\n');

   if (first_lf == NULL) return TRUE;
   if (first_lf == buf) return TRUE;
   --first_lf;
   return (*first_lf != '\r');
}

char *GetHeaderLine(psz_start, just_lf)
   char *psz_start;
   int just_lf;
{
   if (just_lf) {
      return strchr(psz_start, '\n');
   } else {
      return strstr(psz_start, "\r\n");
   }
}

static char *gppszValidMultipartReplace[] = {
    "multipart/x-mixed-replace",  /* only knows about JPEG movie */
    NULL
};

static
int ValidMultipartReplace(buf, pn_multipart)
   char *buf;
   int *pn_multipart;
{
   char *psz=NULL, *psz_copy=UtilStrDup(buf);
   char **ppsz=NULL;

   *pn_multipart = FALSE;

   if (psz_copy == NULL) FailAllocMessage();
   UtilTrimBlanks(psz_copy);
   psz = strchr(psz_copy, '/');
   if (psz != NULL) {
      *psz = '\0';
      if (UtilStrICmp(psz_copy, "multipart") == 0) {
         *pn_multipart = TRUE;
      }
      *psz = '/';
   } else {
      UtilFree(psz_copy);
      return FALSE;
   }
   for (ppsz=gppszValidMultipartReplace; *ppsz != NULL; ppsz++) {
      if (UtilStrICmp(*ppsz, psz_copy) == 0) {
         UtilFree(psz_copy);
         return TRUE;
      }
   }
   UtilFree(psz_copy);
   return FALSE;
}

static char *gppszValidImageContentType[] = {
    "image/jpeg", "JPEG-dither", /* JPEG movie or Motion JPEG */
    "image/jpg", "JPEG-dither", /* JPEG movie or Motion JPEG */
    "image/png", "PNG-dither", /* Netscape Server Push */
    NULL
};

static
int ValidImageContentType(buf, ppsz_filter)
   char *buf, **ppsz_filter;
{
   char *psz_copy=UtilStrDup(buf);
   char **ppsz=NULL;

   if (psz_copy == NULL) FailAllocMessage();
   UtilTrimBlanks(psz_copy);
   ppsz = (gaszFilterForContentType==NULL ? gppszValidImageContentType:
         gaszFilterForContentType);
   for ( ; *ppsz != NULL; ppsz++) {
      if (UtilStrICmp(*ppsz, psz_copy) == 0) {
         if (ppsz_filter != NULL) *ppsz_filter = ppsz[1];
         UtilFree(psz_copy);
         return TRUE;
      }
      ppsz++;
   }
   if (UtilStrICmp(psz_copy, "text/html") == 0) {
      if (ppsz_filter != NULL) *ppsz_filter = NULL;
      UtilFree(psz_copy);
      return TRUE;
   }
   UtilFree(psz_copy);
   return FALSE;
}

static
void UpdateBoundary(buf)
   char *buf;
{
   UtilFree(gpszBoundary);
   gpszBoundary = (char*)malloc(strlen(buf)+3);
   if (gpszBoundary == NULL) FailAllocMessage();
   sprintf(gpszBoundary, "--%s", buf);
   UtilTrimBlanks(gpszBoundary);
   if (*gpszBoundary == '"') {
      int len=strlen(gpszBoundary);

      if (len > 2 && gpszBoundary[len-1] == '"') {
         char *psz=gpszBoundary, *psz1=(&gpszBoundary[1]);

         gpszBoundary[--len] = '\0';
         while (*psz1 != '\0') {
            *psz++ = *psz1++;
         }
         *psz = '\0';
      }
   }
   gnBoundaryLen = strlen(gpszBoundary);
}

static
int FindBoundary(buf)
   char *buf;
{
   char *semi_colon=NULL;
   int found=FALSE;

   while (*buf == ' ' || *buf == '\t') buf++;
   semi_colon = strchr(buf, ';');
   while (!found) {
      char *equal_ptr=NULL;

      if (semi_colon != NULL) *semi_colon = '\0';
      equal_ptr = strchr(buf, '=');
      if (equal_ptr != NULL) {
         *equal_ptr = '\0';
         if (UtilStrICmp(buf, "boundary") == 0) {
            UpdateBoundary(&equal_ptr[1]);
            found = TRUE;
         }
         *equal_ptr = '=';
      }
      if (semi_colon == NULL) break;
      *semi_colon++ = ';';
      buf = semi_colon;
      while (*buf == ' ' || *buf == '\t') buf++;
      semi_colon = strchr(buf, ';');
   }
   return found;
}

static
int GetContentLength(buf)
   char *buf;
{
   char *c_ptr=NULL, *psz=NULL, *line_ptr=buf;
   int content_len=(-1), just_lf=IsJustLF(buf);
   int good_content_type=FALSE, last_line_is_content_type=FALSE;
   int inc=(just_lf?1:2);

   for (c_ptr=GetHeaderLine(line_ptr, just_lf); c_ptr != NULL;
         c_ptr=GetHeaderLine(line_ptr, just_lf)) {
      char *colon_ptr=NULL;

      if (c_ptr == line_ptr) {
         /* reach the end of header, now decide if the header is good */
         gnJustLF = just_lf;
         gnHeaderLen = gnStartIndex = ((&c_ptr[inc]) - buf);
         if (good_content_type && gpszBoundary != NULL) {
            gnMultipartReplace = TRUE;
            if ((debugHttp % 100) == 99 && cmdLineDumpURL &&
                  cmdLineDumpURLWithHeader) {
               fprintf(stdout, "%s", buf);
            }
         } else {
            ResetMultipartReplace(FALSE);
            gnPossibleMultipartReplace = FALSE;
         }
         return (content_len==(-1) ? 0 : content_len);
      }
      *c_ptr = '\0';
      if (*line_ptr == ' ' || *line_ptr == '\t') {
         if (last_line_is_content_type && good_content_type) {
            for (psz=line_ptr; *psz == ' ' || *psz == '\t'; psz++) ;
            FindBoundary(psz);
         }
      } else {
         last_line_is_content_type = FALSE;
         colon_ptr = strchr(line_ptr, ':');
         if (colon_ptr != NULL) {
            *colon_ptr = '\0';
            if (UtilStrICmp(line_ptr, "Content-Length") == 0) {
               char length_str[MAXSTRING];
               int len=0;

               UtilStrCpyN(length_str, MAXSTRING-1, &colon_ptr[1]);
               UtilTrimBlanks(length_str);
               if (sscanf(length_str, "%d", &len) == 1) {
                  content_len = len;
               }
            } else if (UtilStrICmp(line_ptr, "Content-Type") == 0) {
               int multipart=FALSE;

               last_line_is_content_type = TRUE;
               psz = strchr(&colon_ptr[1], ';');
               if (psz != NULL) *psz = '\0';
               good_content_type = ValidMultipartReplace(&colon_ptr[1],
                     &multipart);
               if (!good_content_type && multipart) {
#ifdef _TGIF_DBG /* debug, do not translate */
                  fprintf(stderr, "%s Content-Type: %s\n",
                        "Does not know how to handle", &colon_ptr[1]);
#endif /* _TGIF_DBG */
               }
               if (psz != NULL) {
                  *psz++ = ';';
                  if (good_content_type) {
                     FindBoundary(psz);
                  }
               }
            }
            *colon_ptr = ':';
         }
      }
      *c_ptr = (just_lf ? '\n' : '\r');
      line_ptr = (&c_ptr[inc]);
   }
   return (-1);
}

static
void ScanHeader(buf)
   char *buf;
{
   if (GetContentLength(buf) > 0) {
      /* the header contains Content-Length, no good! */
      ResetMultipartReplace(FALSE);
      gnPossibleMultipartReplace = FALSE;
   }
}

typedef struct tagImageContentInfo {
   char sz_content_type[MAXSTRING];
   char *psz_filter;
   int content_len;
   int header_len;
} ImageContentInfo;

/*
 * The data in the buffer is expected to have the following structure:
 *
 *     HTTP/1.0 200
 *     Content-Type: multipart/x-mixed-replace: boundary=BOUNDARYPATTERN
 *
 *     --BOUNDARYPATTERN
 *     Content-Type: image/jpeg
 *
 *     ~~~~..JFIF.....H.H..~~..Photoshop 3.0.8BIM.~....
 *     3~O~~~~~~.~~~~?~G~~~~~~~~,~~~a~ \~~~
 *
 *     --BOUNDARYPATTERN
 *     Content-Type: image/jpeg
 *
 *     ~~~~..JFIF..........~~.C...........#!.$.M2.**.^C
 *     ..................................~~
 *
 * Notices that the first 3 bytes in a JPEG file is ff d8 ff and
 * the last 2 bytes are ff d9.  This is how the code can figure
 * out the end of the JPEG file without Content-Length.
 */

static
int GetContent(buf, buf_len, start_index, pici)
   char *buf;
   int buf_len, start_index;
   ImageContentInfo *pici;
{
   char *c_ptr=NULL, *line_ptr=(&buf[start_index]);
   int content_len=(-1), just_lf=IsJustLF(line_ptr);
   int found_boundary=FALSE, inc=(just_lf?1:2), first_line=TRUE;
   int faked_found_boundary=FALSE, seen_none_blank_line=FALSE;

   memset(pici, 0, sizeof(ImageContentInfo));
   for (c_ptr=GetHeaderLine(line_ptr, just_lf); c_ptr != NULL;
         c_ptr=GetHeaderLine(line_ptr, just_lf)) {
      char *colon_ptr=NULL;

      if (!found_boundary) {
         *c_ptr = '\0';
         if (*line_ptr == ' ' || *line_ptr == '\t') {
            /* ignore */
         } else if (first_line && *line_ptr == '\0') {
            /*
             * it may not have the boundary line, just pretend that the
             *       boundary line is found
             */
            found_boundary = TRUE;
            faked_found_boundary = TRUE;
         } else {
            if (strcmp(line_ptr, gpszBoundary) == 0) {
               found_boundary = TRUE;
            } else if (strncmp(gpszBoundary, "----", 4) == 0 &&
                  strcmp(line_ptr, &gpszBoundary[2]) == 0) {
               found_boundary = TRUE;
               UpdateBoundary(&line_ptr[2]);
            }
            seen_none_blank_line = TRUE;
         }
         first_line = FALSE;
         *c_ptr = (just_lf ? '\n' : '\r');
         line_ptr = (&c_ptr[inc]);
         continue;
      }
      if (c_ptr == line_ptr && seen_none_blank_line) {
         /* reach the end of header, now decide if the header is good */

         pici->header_len = ((&c_ptr[inc])-(&buf[start_index]));
         pici->content_len = (content_len==(-1) ? 0 : content_len);
         if (pici->content_len == 0) {
            /*
             * no content length information, see if it's really a JPEG file,
             *       and if it is, just look for the EOI
             */
            if (start_index+pici->header_len+3 <= buf_len) {
               char *psz=(&buf[start_index+pici->header_len]);

               if (psz[0] == ((char)0xff) && psz[1] == ((char)0xd8) &&
                     psz[2] == ((char)0xff)) {
                  int index=0;

                  /* now look for EOI, which is 0xffd9 */
                  for (index=start_index+pici->header_len; index < buf_len-1;
                        index++, psz++) {
                     if (*psz == ((char)0xff) && psz[1] == ((char)0xd9)) {
                        /* got EOI, now we know the content length */
                        pici->content_len = content_len =
                              index+2-(start_index+pici->header_len);
                        break;
                     }
                  }
               } else if (psz[0] == ((char)0x89) && psz[1] == 'P' &&
                     psz[2] == 'N' && psz[3] == 'G') {
                  /*
                   * Well, Content-Length better be there!
                   */
               } else {
                  /* this stream should be abandoned */
               }
            }
         }
         if (start_index+pici->header_len+pici->content_len >= buf_len) {
            /* image is not in the buffer yet */
            return (-1);
         }
         return (content_len==(-1) ? 0 : content_len);
      }
      *c_ptr = '\0';
      if (*line_ptr == ' ' || *line_ptr == '\t' || *line_ptr == '\0') {
         /* ignore */
      } else {
         seen_none_blank_line = TRUE;

         colon_ptr = strchr(line_ptr, ':');
         if (colon_ptr != NULL) {
            *colon_ptr = '\0';
            if (UtilStrICmp(line_ptr, "Content-Length") == 0) {
               char length_str[MAXSTRING];
               int len;

               UtilStrCpyN(length_str, MAXSTRING-1, &colon_ptr[1]);
               UtilTrimBlanks(length_str);
               if (sscanf(length_str, "%d", &len) == 1) {
                  content_len = len;
               }
            } else if (UtilStrICmp(line_ptr, "Content-Type") == 0) {
               char *psz=strchr(&colon_ptr[1], ';');

               if (psz != NULL) *psz = '\0';
               UtilStrCpyN(pici->sz_content_type,
                     sizeof(pici->sz_content_type),
                     &colon_ptr[1]);
               UtilTrimBlanks(pici->sz_content_type);
               if (!ValidImageContentType(&colon_ptr[1], &pici->psz_filter)) {
                  fprintf(stderr,
                        TgLoadCachedString(CSTID_DONT_KNOW_HOW_HANDLE_FRAME_CT),
                        &colon_ptr[1]);
                  fprintf(stderr, "\n");
               } else if (pici->psz_filter == NULL) {
                  /* content-type is text/html, pretend it's okay */
               }
               if (psz != NULL) *psz++ = ';';
            }
            *colon_ptr = ':';
         } else if (faked_found_boundary) {
            if (strcmp(line_ptr, gpszBoundary) == 0) {
               faked_found_boundary = FALSE;
            }
         }
      }
      *c_ptr = (just_lf ? '\n' : '\r');
      line_ptr = (&c_ptr[inc]);
   }
   return (-1);
}

static
void ScanBody(buf, pn_buf_len)
   char *buf;
   int *pn_buf_len;
{
   ImageContentInfo ici;

   memset(&ici, 0, sizeof(ImageContentInfo));
   while (GetContent(buf, *pn_buf_len, gnStartIndex, &ici) > 0) {
      if (*ici.sz_content_type != '\0' && ici.psz_filter != NULL) {
         char *tmp_fname=WriteRemoteFileIntoTemp(
               &buf[gnStartIndex+ici.header_len], ici.content_len, NULL);

         if (tmp_fname != NULL) {
            int left_over=0;
            int saved_enable_failed_import_msgbox=GetEnableFailedImportMsgBox();
            char *dest=NULL, *src=NULL;

            if (gpVideoObj != NULL) {
               UnlinkObj(gpVideoObj);
               FreeObj(gpVideoObj);
            }
            gpVideoObj = NULL;
            if (strcmp(ici.sz_content_type, "image/png") == 0) {
               SetEnableFailedImportMsgBox(FALSE);
            }
            if (ImportSpecifiedFileType(tmp_fname, ici.psz_filter)) {
               gpVideoObj = topObj;
               MoveObj(gpVideoObj, drawOrigX-gpVideoObj->obbox.ltx,
                     drawOrigY-gpVideoObj->obbox.lty);
               DrawObj(drawWindow, gpVideoObj);
            }
            SetEnableFailedImportMsgBox(saved_enable_failed_import_msgbox);
            unlink(tmp_fname);
            free(tmp_fname);
            /* now we need to shrink the buffer */
            left_over = *pn_buf_len -
                  gnStartIndex - ici.header_len - ici.content_len;
            dest = (&buf[gnStartIndex]);
            src = (&buf[gnStartIndex+ici.header_len+ici.content_len]);
            if (left_over >= 5 && UtilStrNCaseCmp(src, "HTTP/", 5) == 0) {
               dest = buf;
               memcpy(dest, src, left_over*sizeof(char));
               *pn_buf_len = left_over;
               ResetMultipartReplace(FALSE);
               ScanHeader(buf);
               if (!gnMultipartReplace) {
                  return;
               }
            } else {
               memcpy(dest, src, left_over*sizeof(char));
               *pn_buf_len = gnStartIndex + left_over;
            }
         }
      }
   }
}

void HandleMultipartReplace(buf, pn_buf_len)
   char *buf;
   int *pn_buf_len;
{
   if (!gnPossibleMultipartReplace) return;

   if (gnMultipartReplace) {
      ScanBody(buf, pn_buf_len);
   } else {
      ScanHeader(buf);
      if (gnMultipartReplace) {
         ScanBody(buf, pn_buf_len);
      }
   }
}

void FakeUserAgent(buf)
   char *buf;
{
   char prev_agent[MAXSTRING<<1], spec[MAXSTRING<<1];

   if (buf != NULL && strcmp(buf, "-1") != 0) {
      int len=0;

      UtilStrCpyN(spec, sizeof(spec), buf);
      UtilTrimBlanks(spec);
      len = strlen(spec);
      if (len > 0 && spec[len-1] == ')') spec[len-1] = '\0';
   } else {
      *prev_agent = '\0';
      GetUserAgent(prev_agent, sizeof(prev_agent));
      if (*prev_agent == '\0') {
         sprintf(gszMsgBox, TgLoadString(STID_ENTER_USERAGENT_FOR_HTTP));
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_ENTER_USERAGENT_FOR_HTTP_CUR),
               prev_agent);
      }
      *spec = '\0';
      if (Dialog(gszMsgBox, TgLoadString(STID_PRESS_ENTER_FOR_DEF_USERAGENT),
            spec) == INVALID) {
         return;
      }
   }
   UtilTrimBlanks(spec);
   SetUserAgent(spec);
   *prev_agent = '\0';
   GetUserAgent(prev_agent, sizeof(prev_agent));
   if (*prev_agent == '\0') {
      sprintf(gszMsgBox, TgLoadString(STID_WILL_USE_DEF_USERAGENT_HTTP));
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_WILL_USE_NAMED_USERAGENT_HTTP),
            prev_agent);
   }
   Msg(gszMsgBox);
}

void FakeReferer(buf)
   char *buf;
{
   char spec[MAXSTRING<<1];

   *spec = '\0';
   if (buf != NULL && strcmp(buf, "-1") != 0) {
      int len=0;

      UtilStrCpyN(spec, sizeof(spec), buf);
      UtilTrimBlanks(spec);
      len = strlen(spec);
      if (len > 0 && spec[len-1] == ')') spec[len-1] = '\0';
   } else {
      if (gpszFakedReferer == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_ENTER_REFERRER_FOR_HTTP));
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_ENTER_REFERRER_FOR_HTTP_CUR),
               gpszFakedReferer);
      }
      *spec = '\0';
      if (Dialog(gszMsgBox, TgLoadString(STID_PRESS_ENTER_FOR_NO_REFERRER),
            spec) == INVALID) {
         return;
      }
   }
   if (*spec != '\0') {
      if (!navigatingBackAndForth) BeforeNavigate();
      ClearFileInfo(FALSE);
      RedrawTitleWindow();
   }
   UtilTrimBlanks(spec);
   HttpFakeReferer(spec);
   if (gpszFakedReferer == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_WILL_NOT_USE_REFERRER_HTTP));
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_WILL_USE_NAMED_REFERRER_HTTP),
            gpszFakedReferer);
   }
   Msg(gszMsgBox);
}

void ToggleKeepAlive()
{
   gnHttpKeepAlive = (!gnHttpKeepAlive);
   sprintf(gszMsgBox, TgLoadString(gnHttpKeepAlive ?
         STID_WILL_USE_KEEP_ALIVE_HTTP : STID_NOT_WILL_USE_KEEP_ALIVE_HTTP));
   Msg(gszMsgBox);
}

