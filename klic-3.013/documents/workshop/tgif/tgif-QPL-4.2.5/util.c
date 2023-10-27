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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/util.c,v 1.13 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_UTIL_C_

#include "tgifdefs.h"

#include "dialog.e"
#include "file.e"
#include "remote.e"
#include "strtbl.e"
#include "util.e"

void UtilFree(pszStr)
   char *pszStr;
{
   if (pszStr != NULL) free(pszStr);
}

char *UtilFreeStringArray(ppsz, nItems)
   char **ppsz;
   int nItems;
   /* ppsz is also freed */
{
   if (ppsz != NULL) {
      int i;

      for (i=0; i < nItems; i++) UtilFree(ppsz[i]);
      free(ppsz);
   }
   return NULL;
}

char *UtilStrDup(pszStr)
   char *pszStr;
   /* must eventually free the returned string with free() */
{
   int len=(pszStr == NULL ? 0 : strlen(pszStr));
   char *c_ptr=(char*)malloc((len+1)*sizeof(char));

   if (c_ptr == NULL) return NULL;
   strcpy(c_ptr, (pszStr == NULL ? "" : pszStr));
   return c_ptr;
}

int UtilStrCpyN(pszDest, nMaxDestSz, pszSrc)
   char *pszDest, *pszSrc;
   int nMaxDestSz;
{
   int len=0;

   if (pszDest != NULL && nMaxDestSz > 0) *pszDest = '\0';
   if (pszDest == NULL || pszSrc == NULL || nMaxDestSz <= 0) return 0;

   len = strlen(pszSrc);
   if (len > nMaxDestSz-1) len = nMaxDestSz-1;
   strncpy(pszDest, pszSrc, len);
   pszDest[len] = '\0';
   return len;
}

int UtilStrCpy(pszDest, pszSrc)
   char *pszDest, *pszSrc;
   /* safe copy byte by byte from pszSrc to pszDest */
{
   int len=0;

   if (pszDest == NULL || pszSrc == NULL) return 0;

   while (((*pszDest++)=(*pszSrc++)) != '\0') {
      len++;
   }
   return len;
}

void UtilLeftTrim(pszStr)
   char *pszStr;
   /* pszStr must be terminated by '\0' */
{
   register char *c_ptr;

   for (c_ptr=pszStr; *c_ptr != '\0'; c_ptr++) {
      char ch=(*c_ptr);
      
      if (!(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')) {
         break;
      }
   }
   if (*c_ptr != '\0' && c_ptr != pszStr) {
      while ((*pszStr++ = *c_ptr++) != '\0') ;
   }
}

void UtilRightTrim(pszStr)
   char *pszStr;
   /* pszStr must be terminated by '\0' */
{
   register int len;

   for (len=strlen(pszStr)-1; len >= 0; len--) {
      char ch=pszStr[len];
      
      if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
         pszStr[len] = '\0';
      } else {
         break;
      }
   }
}

void UtilTrimBlanks(pszStr)
   char *pszStr;
   /* pszStr must be terminated by '\0' */
{
   register int len;
   register char *c_ptr;

   for (len=strlen(pszStr)-1; len >= 0; len--) {
      char ch=pszStr[len];
      
      if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
         pszStr[len] = '\0';
      } else {
         break;
      }
   }
   for (c_ptr=pszStr; *c_ptr != '\0'; c_ptr++) {
      char ch=(*c_ptr);
      
      if (!(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')) {
         break;
      }
   }
   if (*c_ptr != '\0' && c_ptr != pszStr) {
      while ((*pszStr++ = *c_ptr++) != '\0') ;
   }
}

void UtilStrUpper(psz)
   char *psz;
{
   for ( ; *psz != '\0'; psz++) {
      if (*psz >= 'a' && *psz <= 'z') {
         *psz = *psz-'a'+'A';
      }
   }
}

void UtilStrLower(psz)
   char *psz;
{
   for ( ; *psz != '\0'; psz++) {
      if (*psz >= 'A' && *psz <= 'Z') {
         *psz = *psz-'A'+'a';
      }
   }
}

int UtilStrNCaseCmp(pszStr1, pszStr2, nCount)
   char *pszStr1, *pszStr2;
   int nCount;
{
   char *c_ptr1, *c_ptr2, ch1, ch2;
   int i=0;

   for (c_ptr1=pszStr1, c_ptr2=pszStr2; i < nCount && (*c_ptr1 != '\0' ||
         *c_ptr2 != '\0'); c_ptr1++, c_ptr2++, i++) {
      ch1 = *c_ptr1;
      ch2 = *c_ptr2;
      if (ch1 >= 'A' && ch1 <= 'Z') ch1 = ch1-'A'+'a';
      if (ch2 >= 'A' && ch2 <= 'Z') ch2 = ch2-'A'+'a';
      if (ch1 != ch2) break;
   }
   if (i == nCount) return 0;
   return ((*c_ptr1)-(*c_ptr2));
}

int UtilStrICmp(pszStr1, pszStr2)
   char *pszStr1, *pszStr2;
{
   int diff=FALSE;
   char *c_ptr1, *c_ptr2, ch1, ch2;

   for (c_ptr1=pszStr1, c_ptr2=pszStr2; *c_ptr1 != '\0' || *c_ptr2 != '\0';
         c_ptr1++, c_ptr2++) {
      ch1 = *c_ptr1;
      ch2 = *c_ptr2;
      if (ch1 >= 'A' && ch1 <= 'Z') ch1 = ch1-'A'+'a';
      if (ch2 >= 'A' && ch2 <= 'Z') ch2 = ch2-'A'+'a';
      if (ch1 != ch2) {
         diff = TRUE;
         break;
      }
   }
   return (diff ? ((*c_ptr1)-(*c_ptr2)) : 0);
}

char *UtilStrRChr(pszStr, int_ch)
   char *pszStr;
   int int_ch;
{
   int len=strlen(pszStr);
   char ch=(char)int_ch;

   for (len--; len >= 0; len--) {
      if (pszStr[len] == ch) {
         return (&pszStr[len]);
      }
   }
   return NULL;
}

char *UtilStrTok(pszStr, pszDelim, ppszState)
   char *pszStr, *pszDelim, **ppszState;
{
   char *psz=NULL, *psz_start=NULL;

   if (pszStr != NULL) {
      *ppszState = NULL;
      psz_start = pszStr;
      while (*psz_start != '\0' && strchr(pszDelim, *psz_start) != NULL) {
         psz_start++;
      }
      if (*psz_start == '\0') return NULL;

      for (psz=psz_start; *psz != '\0'; psz++) {
         if (strchr(pszDelim, *psz) != NULL) {
            *psz = '\0';
            *ppszState = psz;

            return psz_start;
         }
      }
      if (*psz == '\0' && psz_start != psz) {
         return psz_start;
      }
   } else {
      if (*ppszState == NULL) return NULL;

      psz_start = &(*ppszState)[1];
      *ppszState = NULL;
      while (*psz_start != '\0' && strchr(pszDelim, *psz_start) != NULL) {
         psz_start++;
      }
      if (*psz_start == '\0') return NULL;

      for (psz=psz_start; *psz != '\0'; psz++) {
         if (strchr(pszDelim, *psz) != NULL) {
            *psz = '\0';
            *ppszState = psz;

            return psz_start;
         }
      }
      if (*psz == '\0' && psz_start != psz) {
         return psz_start;
      }
   }
   return NULL;
}

#ifdef NO_STRSTR
char *strstr (pszStr, pszSubStr)
   char *pszStr, *pszSubStr;
{
   int len;

   for (len=strlen(pszSubStr); *pszStr != '\0'; pszStr++) {
      if (strncmp(pszStr, pszSubStr, len) == 0) {
         return(pszStr);
      }
   }
   return NULL;
}
#endif /* NO_STRSTR */

int UtilShrinkName(pszFile)
   char *pszFile;
{
   char *c_ptr=NULL, *real_ptr=NULL, *real_name=NULL;

   if (pszFile[0] == '.' && pszFile[1] == DIR_SEP) return TRUE;
   if (*pszFile != DIR_SEP) return FALSE;

   real_name = UtilStrDup(pszFile);
   if (real_name == NULL) return FALSE;
   real_ptr = real_name;
   c_ptr = &real_name[1];

   while (*c_ptr != '\0') {
      if (*c_ptr == '.') {
         if (c_ptr[1] == '.') {
            if (c_ptr[2] == DIR_SEP) {
               /* "../" */
               if (real_ptr != real_name) {
                  while (*(--real_ptr) != DIR_SEP) ;
               }
               c_ptr = (&c_ptr[3]);
            } else {
               while (*c_ptr != '\0' && *c_ptr != DIR_SEP) {
                  *(++real_ptr) = *c_ptr++;
               }
               if (*c_ptr == DIR_SEP) {
                  c_ptr++;
                  *(++real_ptr) = DIR_SEP;
               }
            }
         } else if (c_ptr[1] == DIR_SEP) {
            /* "./" */
            c_ptr = (&c_ptr[2]);
         } else {
            while (*c_ptr != '\0' && *c_ptr != DIR_SEP) {
               *(++real_ptr) = *c_ptr++;
            }
            if (*c_ptr == DIR_SEP) {
               c_ptr++;
               *(++real_ptr) = DIR_SEP;
            }
         }
#ifndef apollo
      } else if (*c_ptr == DIR_SEP) {
         c_ptr++;
#endif
      } else {
         while (*c_ptr != '\0' && *c_ptr != DIR_SEP) {
            *(++real_ptr) = *c_ptr++;
         }
         if (*c_ptr == DIR_SEP) {
            c_ptr++;
            *(++real_ptr) = DIR_SEP;
         }
      }
   }
   *(++real_ptr) = '\0';
   strcpy(pszFile, real_name);
   free(real_name);
   return TRUE;
}

void UtilRemoveQuotes(pszStr)
   char *pszStr;
{
   int nStart=0, nEnd=strlen(pszStr)-1, still_going=TRUE;

   while (still_going) {
      if (nStart >= nEnd) break;
      switch (pszStr[nStart]) {
      case '"': if (pszStr[nEnd] != '"') still_going = FALSE; break;
      case '\'': if (pszStr[nEnd] != '\'') still_going = FALSE; break;
      default: still_going = FALSE; break;
      }
      if (still_going) {
         nStart++;
         pszStr[nEnd--] = '\0';
      }
   }
   if (nStart != 0) {
      char *c_ptr=&pszStr[nStart];

      while (*pszStr != '\0') *pszStr++ = (*c_ptr++);
   }
}

static char gszMsg[512];

char *UtilGetALine(pFile)
   FILE *pFile;
{
   unsigned long len;
   
   if (fgets(gszMsg, sizeof(gszMsg), pFile) == NULL) return NULL;
   
   len = strlen(gszMsg);
   if (len == sizeof(gszMsg)-1 && gszMsg[len-1] != '\n') {
      char *buf=UtilStrDup(gszMsg);
      unsigned long cur_len;
      int still_going=TRUE;
      
      if (buf == NULL) return NULL;
      
      cur_len = strlen(buf);
      while (still_going && fgets(gszMsg, sizeof(gszMsg), pFile) != NULL) {
         len = strlen(gszMsg);
         if (len != sizeof(gszMsg)-1) {
            still_going = FALSE;
         } else if (gszMsg[len-1] == '\n') {
            gszMsg[--len] = '\0';
            still_going = FALSE;
         }
         if (buf != NULL) {
            char *new_buf=(char*)realloc(buf, (size_t)(cur_len+len+1));
            
            if (new_buf == NULL) {
               free(buf);
               return NULL;
            }
            buf = new_buf;
            strcat(&buf[cur_len], gszMsg);
            cur_len += len;
         }
      }
      return buf;
   } else {
      if (gszMsg[len-1] == '\n') gszMsg[--len] = '\0';
      return UtilStrDup(gszMsg);
   }
}

char *UtilGetAContinuedLine(pFile)
   FILE *pFile;
{
   unsigned long len;
   char *buf;

   if ((buf=UtilGetALine(pFile)) == NULL) return NULL;

   len = strlen(buf);
   while (len > 0 && buf[len-1] == '\\') {
      char *tmp_buf;
      int tmp_len;

      buf[--len] = '\0';
      tmp_buf = UtilGetALine(pFile);
      if (tmp_buf == NULL) return buf;
      tmp_len = strlen(tmp_buf);
      if (tmp_len == 0) {
         free(tmp_buf);
         return buf;
      } else if ((buf=(char*)realloc(buf, len+tmp_len+1)) == NULL) {
         free(tmp_buf);
         return NULL;
      }
      strcpy(&buf[len], tmp_buf);
      len += tmp_len;
      free(tmp_buf);
   }
   return buf;
}

int UtilCopyFile(pszFromFile, pszToFile)
   char *pszFromFile, *pszToFile;
{
   char buf[0x1000];
   int bytes_read=0;
   int fd1=(-1), fd2=(-1);
   mode_t mode=((tmpFileMode == 0) ? 0700 : tmpFileMode);

   if ((fd1=open(pszFromFile, O_RDONLY)) == (-1)) return TG_REMOTE_STATUS_READ;
   if (UtilPathExists(pszToFile)) {
      if ((fd2=open(pszToFile, O_WRONLY|O_TRUNC)) == (-1)) {
         close(fd1);
         return TG_REMOTE_STATUS_WRITE;
      }
   } else {
      if ((fd2=open(pszToFile, O_WRONLY|O_CREAT|O_TRUNC, mode)) == (-1)) {
         close(fd1);
         return TG_REMOTE_STATUS_WRITE;
      }
   }
   while ((bytes_read=read(fd1, buf, sizeof(buf))) > 0) {
      if (write(fd2, buf, bytes_read) <= 0) {
         close(fd1);
         close(fd2);
         return TG_REMOTE_STATUS_FILE;
      }
   }
   close(fd1);
   close(fd2);
   if (tmpFileMode != 0 && chmod(pszToFile, tmpFileMode)) {
      char msg[MAXSTRING<<1];

      sprintf(msg, TgLoadString(STID_CANNOT_CHMOD), pszToFile, tmpFileMode);
      MsgBox(msg, TOOL_NAME, INFO_MB);
   }
   return TG_REMOTE_STATUS_OK;
}

int UtilConcatFile(pszFromFile1, pszFromFile2, pszToFile)
   char *pszFromFile1, *pszFromFile2, *pszToFile;
{
   char buf[0x1000];
   int bytes_read=0;
   int fd1=(-1), fd2=(-1), fd3=(-1);
   mode_t mode=((tmpFileMode == 0) ? 0700 : tmpFileMode);

   if ((fd1=open(pszFromFile1, O_RDONLY)) == (-1)) return TG_REMOTE_STATUS_READ;
   if ((fd2=open(pszFromFile2, O_RDONLY)) == (-1)) {
      close(fd1);
      return TG_REMOTE_STATUS_READ;
   }
   if (UtilPathExists(pszToFile)) {
      if ((fd3=open(pszToFile, O_WRONLY|O_TRUNC)) == (-1)) {
         close(fd1);
         close(fd2);
         return TG_REMOTE_STATUS_WRITE;
      }
   } else {
      if ((fd3=open(pszToFile, O_WRONLY|O_CREAT|O_TRUNC, mode)) == (-1)) {
         close(fd1);
         close(fd2);
         return TG_REMOTE_STATUS_WRITE;
      }
   }
   while ((bytes_read=read(fd1, buf, sizeof(buf))) > 0) {
      if (write(fd3, buf, bytes_read) <= 0) {
         close(fd1);
         close(fd2);
         close(fd3);
         return TG_REMOTE_STATUS_FILE;
      }
   }
   close(fd1);
   while ((bytes_read=read(fd2, buf, sizeof(buf))) > 0) {
      if (write(fd3, buf, bytes_read) <= 0) {
         close(fd2);
         close(fd3);
         return TG_REMOTE_STATUS_FILE;
      }
   }
   close(fd2);
   close(fd3);
   if (tmpFileMode != 0 && chmod(pszToFile, tmpFileMode)) {
      char msg[MAXSTRING<<1];

      sprintf(msg, TgLoadString(STID_CANNOT_CHMOD), pszToFile, tmpFileMode);
      MsgBox(msg, TOOL_NAME, INFO_MB);
   }
   return TG_REMOTE_STATUS_OK;
}

int UtilPathExists(pszPath)
   char *pszPath;
{
   struct stat stat_buf;

   return (stat(pszPath, &stat_buf) == 0);
}

int UtilPathIsLink(pszPath)
   char *pszPath;
   /*
    * if pszPath is not valid, return FALSE
    * if pszPath is valid but not a link, return FALSE
    * if pszPath is valid and is a link, return TRUE
    */
{
#ifndef _NO_LSTAT
    struct stat stat_buf;

    if (lstat(pszPath, &stat_buf) == 0 &&
            ((stat_buf.st_mode & S_IFLNK) == S_IFLNK)) {
        return TRUE;
    }
#endif /* _NO_LSTAT */
    return FALSE;
}

int UtilIsDirectory(pszPath)
   char *pszPath;
{
   struct stat stat_buf;

   return (stat(pszPath, &stat_buf) == 0 &&
      (stat_buf.st_mode & S_IFDIR) == S_IFDIR);
}

int UtilCreateDirHier(pszDir)
   char *pszDir;
{
    char *psz=NULL;
    int ok=TRUE;

    if (*pszDir == '\0') return FALSE;

    if (UtilPathExists(pszDir)) {
        if (UtilPathIsLink(pszDir)) {
            return TRUE;
        }
        if (UtilIsDirectory(pszDir)) {
            return TRUE;
        }
        return FALSE;
    }
    if (*pszDir == DIR_SEP && pszDir[1] == '\0') {
        return FALSE;
    }
    if ((psz=strrchr(pszDir, DIR_SEP)) == NULL) {
        return FALSE;
    }
    *psz = '\0';
    if (!UtilCreateDirHier(pszDir)) {
        ok = FALSE;
    }
    *psz = DIR_SEP;

    if (ok) {
        if (mkdir(pszDir, 0755) != 0) {
            return FALSE;
        }
    }
    return ok;
}

#ifndef _NO_GETTIMEOFDAY
static
long ms_time(tv)
   struct timeval *tv;
{
   return ((long)(tv->tv_usec / 1000.0)) + ((long)(tv->tv_sec * 1000));
}
#endif /* ~_NO_GETTIMEOFDAY */

long UtilGetMilliSecTime(pl_sec, pl_msec)
   long *pl_sec, *pl_msec;
{
#ifdef _NO_GETTIMEOFDAY
   struct timeb start;
#else /* ~_NO_GETTIMEOFDAY */
   struct timeval start;
   struct timezone zone;
#endif /* _NO_GETTIMEOFDAY */
   long ms_start_time=0L;

#ifdef _NO_GETTIMEOFDAY
   ftime(&start);
   ms_start_time = ((long)start.time * 1000) + ((long)start.millitm);
   if (pl_sec != NULL) *pl_sec = ((long)(start.time));
   if (pl_msec != NULL) *pl_msec = ((long)(start.millitm));
#else /* ~_NO_GETTIMEOFDAY */
   gettimeofday(&start, &zone);
   ms_start_time = ms_time(&start);
   if (pl_sec != NULL) *pl_sec = ((long)(start.tv_sec));
   if (pl_msec != NULL) *pl_msec = ((long)(start.tv_usec/1000));
#endif /* _NO_GETTIMEOFDAY */

   return ms_start_time;
}  

