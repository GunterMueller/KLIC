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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/ini.c,v 1.7 2011/05/16 16:21:58 william Exp $
 */

#define _INCLUDE_FROM_INI_C_

#include "tgifdefs.h"

#include "dialog.e"
#include "ini.e"
#include "msg.e"
#include "strtbl.e"
#include "util.e"

typedef struct TgIniEntryInfoStruct {
   char *pszEntry;
   char *pszValue;
   int bScanned;
   struct TgIniEntryInfoStruct *pNextEntry, *pPrevEntry;
} TGINIENTRYINFO;

typedef struct TgIniSectionInfoStruct {
   char *pszSection;
   int bAllowDupKey;
   int bValuelessKey;
   TGINIENTRYINFO *pFirstEntry, *pLastEntry;
   struct TgIniSectionInfoStruct *pNextSection, *pPrevSection;
} TGINISECTIONINFO;

typedef struct TgIniFileInfoStruct {
   char *pszFile;
   int bModified;
   int bStripQuotes;
   int bCheckFileTime;
   time_t stFileTime;
   TGINISECTIONINFO *pFirstSection, *pLastSection;
   struct TgIniFileInfoStruct *pNextInfo, *pPrevInfo;
} TGINIFILEINFO;

typedef struct TgIniStruct {
   TGINIFILEINFO *pFirstInfo, *pLastInfo;
} TGINI;

static TGINI tgIni;

/* -------------------- Utility Functions -------------------- */

static
int InvalidPath(pszFile)
   char *pszFile;
{
   sprintf(gszMsgBox, TgLoadString(STID_INVALID_GIVEN_PATH_MUST_FULL), pszFile);
   fprintf(stderr, "%s\n", gszMsgBox);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   return FALSE;
}

static
int GetLastModifiedTime(pszFile, pFileTime)
   char *pszFile;
   time_t *pFileTime;
{
   struct stat stat_buf;

   if (stat(pszFile, &stat_buf) == 0) {
      memcpy(pFileTime, &stat_buf.st_mtime, sizeof(time_t));
      return TRUE;
   }
   return FALSE;
}

static
int CompareFileTime(pNewFileTime, pOldFileTime)
   time_t *pNewFileTime, *pOldFileTime;
{
   long lval=((long)(*pNewFileTime)) - ((long)(*pOldFileTime));
   int ival=(int)lval;

   if (lval > 0) {
      if (ival < 0) return (-ival);
   } else if (lval < 0) {
      if (ival > 0) return (-ival);
   } else {
      ival = 0;
   }
   return ival;
}

/* -------------------- Discard Functions -------------------- */

static
void UnlinkFileInfo(ptifi)
   TGINIFILEINFO *ptifi;
{
   if (ptifi->pPrevInfo != NULL) {
      ptifi->pPrevInfo->pNextInfo = ptifi->pNextInfo;
   } else {
      tgIni.pFirstInfo = ptifi->pNextInfo;
   }
   if (ptifi->pNextInfo != NULL) {
      ptifi->pNextInfo->pPrevInfo = ptifi->pPrevInfo;
   } else {
      tgIni.pLastInfo = ptifi->pPrevInfo;
   }
}

static
void DiscardEntryInfo(ptiei)
   TGINIENTRYINFO *ptiei;
{
   UtilFree(ptiei->pszEntry);
   UtilFree(ptiei->pszValue);
}

static
void DiscardSectionInfo(ptisi)
   TGINISECTIONINFO *ptisi;
{
   TGINIENTRYINFO *ptiei=NULL, *ptiei_next;
   
   for (ptiei=ptisi->pFirstEntry; ptiei != NULL; ptiei=ptiei_next) {
      ptiei_next = ptiei->pNextEntry;
      DiscardEntryInfo(ptiei);
      free(ptiei);
   }
   UtilFree(ptisi->pszSection);
}

static
int WriteOutIniFile(ptifi, pszAltFile)
   TGINIFILEINFO *ptifi;
   char *pszAltFile;
{
   int ok=TRUE;
   char *pszFile=(pszAltFile==NULL ? ptifi->pszFile : pszAltFile);
   char *c_ptr=strchr(pszFile, DIR_SEP);
   FILE *pFile=NULL;
   TGINISECTIONINFO *ptisi=NULL;
   
   if (c_ptr == NULL) {
      return InvalidPath(pszFile);
   }
   if ((pFile=fopen(pszFile, "w")) == NULL) {
      return FailToWriteFileMessage(pszFile);
   }
   for (ptisi=ptifi->pFirstSection; ok && ptisi != NULL;
         ptisi=ptisi->pNextSection) {
      int bValuelessKey=ptisi->bValuelessKey;
      TGINIENTRYINFO *ptiei=NULL;

      /* do not translate -- program constants */
      if (fprintf(pFile, "[%s]\n", ptisi->pszSection) <= 0) {
         ok = FALSE;
         break;
      }
      for (ptiei=ptisi->pFirstEntry; ok && ptiei != NULL;
            ptiei=ptiei->pNextEntry) {
         /* remember that the key is ";" for a comment line */
         if (strcmp(ptiei->pszEntry, ";") == 0) {
            if (fprintf(pFile, "%s\n", ptiei->pszValue) <= 0) {
               ok = FailToWriteFileMessage(pszFile);
               break;
            }
         } else if (bValuelessKey) {
            if (fprintf(pFile, "%s\n", ptiei->pszEntry) <= 0) {
               ok = FailToWriteFileMessage(pszFile);
               break;
            }
         } else {
            if (fprintf(pFile, "%s=%s\n", ptiei->pszEntry,
                  ptiei->pszValue) <= 0) {
               ok = FailToWriteFileMessage(pszFile);
               break;
            }
         }
      }
      if (ok && ptisi->pNextSection != NULL) {
         if (fprintf(pFile, "\n") <= 0) {
            ok = FailToWriteFileMessage(pszFile);
            break;
         }
      }
   }
   if (fclose(pFile) == EOF) {
      return FailToWriteFileMessage(pszFile);
   }
   ptifi->bModified = FALSE;
   return ok;
}

static
int DiscardFileInfo(ptifi)
   TGINIFILEINFO *ptifi;
{
   TGINISECTIONINFO *ptisi=NULL, *ptisi_next;
   int ok=TRUE;
   
   if (ptifi->bModified) {
      ok = WriteOutIniFile(ptifi, NULL);
   }
   for (ptisi=ptifi->pFirstSection; ptisi != NULL; ptisi=ptisi_next) {
      ptisi_next = ptisi->pNextSection;
      DiscardSectionInfo(ptisi);
      free(ptisi);
   }
   UtilFree(ptifi->pszFile);
   return ok;
}

/* -------------------- Allocation Functions -------------------- */

static
TGINIENTRYINFO *AllocEntryInfo(ptisi, pszEntry, pszValue)
   TGINISECTIONINFO *ptisi;
   char *pszEntry, *pszValue;
{
   /* for a comment line, make the key to be ";" */
   char *buf1=UtilStrDup(pszEntry==NULL ? ";" : pszEntry);
   char *buf2=UtilStrDup(pszValue);
   TGINIENTRYINFO *ptiei=
         (TGINIENTRYINFO*)malloc(sizeof(TGINIENTRYINFO));

   if (buf1 == NULL || buf2 == NULL || ptiei == NULL) {
      UtilFree(buf1);
      UtilFree(buf2);
      free(ptiei);
      FailAllocMessage();
      return NULL;
   }
   memset(ptiei, 0, sizeof(TGINIENTRYINFO));
   
   /* first-in-first-out */
   ptiei->pNextEntry = NULL;
   ptiei->pPrevEntry = ptisi->pLastEntry;
   if (ptisi->pLastEntry == NULL) {
      ptisi->pFirstEntry = ptiei;
   } else {
      ptisi->pLastEntry->pNextEntry = ptiei;
   }
   ptisi->pLastEntry = ptiei;
   
   ptiei->pszEntry = buf1;
   ptiei->pszValue = buf2;
   ptiei->bScanned = FALSE;
   return ptiei;
}

static
TGINISECTIONINFO *AllocSectionInfo(ptifi, pszSection, bValuelessKey)
   TGINIFILEINFO *ptifi;
   char *pszSection;
   int bValuelessKey;
{
   char *buf=UtilStrDup(pszSection);
   TGINISECTIONINFO *ptisi=
         (TGINISECTIONINFO*)malloc(sizeof(TGINISECTIONINFO));

   if (buf == NULL || ptisi == NULL) {
      UtilFree(buf);
      free(ptisi);
      FailAllocMessage();
      return NULL;
   }
   memset(ptisi, 0, sizeof(TGINISECTIONINFO));
   
   /* first-in-first-out */
   ptisi->pNextSection = NULL;
   ptisi->pPrevSection = ptifi->pLastSection;
   if (ptifi->pLastSection == NULL) {
      ptifi->pFirstSection = ptisi;
   } else {
      ptifi->pLastSection->pNextSection = ptisi;
   }
   ptifi->pLastSection = ptisi;
   
   ptisi->pszSection = buf;
   ptisi->bAllowDupKey = FALSE;
   ptisi->bValuelessKey = bValuelessKey;
   return ptisi;
}

static
TGINIFILEINFO *AllocFileInfo(pszFile)
   char *pszFile;
{
   char *buf=UtilStrDup(pszFile);
   TGINIFILEINFO *ptifi=(TGINIFILEINFO*)malloc(sizeof(TGINIFILEINFO));
   if (buf == NULL || ptifi == NULL) {
      UtilFree(buf);
      free(ptifi);
      FailAllocMessage();
      return NULL;
   }
   memset(ptifi, 0, sizeof(TGINIFILEINFO));
   
   /* last-in-first-out */
   ptifi->pNextInfo = tgIni.pFirstInfo;
   ptifi->pPrevInfo = NULL;
   if (tgIni.pFirstInfo == NULL) {
      tgIni.pLastInfo = ptifi;
   } else {
      tgIni.pFirstInfo->pPrevInfo = ptifi;
   }
   tgIni.pFirstInfo = ptifi;
   
   ptifi->pszFile = buf;
   ptifi->bModified = FALSE;
   ptifi->bStripQuotes = TRUE;
   ptifi->bCheckFileTime = TRUE;
   memset(&ptifi->stFileTime, 0, sizeof(time_t));

   if (!GetLastModifiedTime(pszFile, &ptifi->stFileTime)) {
      memset(&ptifi->stFileTime, 0, sizeof(time_t));
   }
   return ptifi;
}

/* -------------------- Parse Functions -------------------- */

static
int ParseFile(ptifi)
   TGINIFILEINFO *ptifi;
{
   char *buf;
   TGINISECTIONINFO *ptisi=NULL;
   char *pszFile=ptifi->pszFile;
   char *c_ptr=strchr(pszFile, DIR_SEP);
   FILE *pFile=NULL;
   
   if (c_ptr == NULL) {
      return InvalidPath(pszFile);
   }
   if ((pFile=fopen(pszFile, "r")) == NULL) return TRUE;
   
   while ((buf=UtilGetALine(pFile)) != NULL) {
      char *c_ptr;
      
      UtilTrimBlanks(buf);
      if (*buf == ';') {
         /* handle comments, remember that the key is ";" for a comment line */
         if (ptisi != NULL) {
            if (AllocEntryInfo(ptisi, NULL, buf) == NULL) return FALSE;
         }
      } else if (*buf == '[') {
         if ((c_ptr=strchr(&buf[1], ']')) == NULL) {
            ptisi = NULL;
         } else {
            *c_ptr = '\0';
            /*
             * Don't need to call UtilTrimBlanks(&buf[1]) because that's how
             *      GetProfileString() works!
             */
            if ((ptisi=AllocSectionInfo(ptifi, &buf[1], FALSE)) == NULL) {
               return FALSE;
            }
         }
      } else if (ptisi != NULL) {
         if ((c_ptr=strchr(buf, '=')) != NULL) {
            char *psz=(&c_ptr[1]);
                
            *c_ptr = '\0';
            c_ptr = psz;
            UtilTrimBlanks(buf);
            UtilTrimBlanks(c_ptr);
            if (AllocEntryInfo(ptisi, buf, c_ptr) == NULL) return FALSE;
            if (ptisi->bValuelessKey) {
#ifdef _DEBUG /* debug, do not translate */
               fprintf(stderr, "%s [%s] of '%s'.\n",
                     "WARNING: Ambiguous valueless key section",
                     ptisi->pszSection, pszFile);
#endif /* _DEBUG */
            }
         } else if (*buf != '\0') {
            char cNull='\0';

            if (AllocEntryInfo(ptisi, buf, &cNull) == NULL) return FALSE;
            ptisi->bValuelessKey = TRUE;
         }
      }
      UtilFree(buf);
   }
   fclose(pFile);
   return TRUE;
}

/* -------------------- External Functions -------------------- */

static
TGINIENTRYINFO *FindEntryInfo(ptisi, pszEntry)
   TGINISECTIONINFO *ptisi;
   char *pszEntry;
{
   if (ptisi == NULL) return NULL;

    /* finding a comment line is not allowed */
   if (pszEntry == NULL || strcmp(pszEntry, ";") == 0) return NULL;

   if (ptisi->bAllowDupKey) {
      TGINIENTRYINFO *ptiei=NULL;

      for (ptiei=ptisi->pFirstEntry; ptiei != NULL;
            ptiei=ptiei->pNextEntry) {
         if (!ptiei->bScanned && UtilStrICmp(pszEntry, ptiei->pszEntry) == 0) {
            ptiei->bScanned = TRUE;
            return ptiei;
         }
      }
   } else {
      TGINIENTRYINFO *ptiei=NULL;

      for (ptiei=ptisi->pFirstEntry; ptiei != NULL;
            ptiei=ptiei->pNextEntry) {
         if (UtilStrICmp(pszEntry, ptiei->pszEntry) == 0) {
            return ptiei;
         }
      }
   }
   return NULL;
}

static
TGINISECTIONINFO *FindSectionInfo(ptifi, pszSection)
   TGINIFILEINFO *ptifi;
   char *pszSection;
{
   TGINISECTIONINFO *ptisi=NULL;

   if (ptifi == NULL) return NULL;

   for (ptisi=ptifi->pFirstSection; ptisi != NULL;
         ptisi=ptisi->pNextSection) {
      if (UtilStrICmp(pszSection, ptisi->pszSection) == 0) {
         return ptisi;
      }
   }
   return NULL;
}

static
TGINIFILEINFO *TgIniFindFileInfo(pszFile, bAutoLoad)
   char *pszFile;
   int bAutoLoad;
{
   TGINIFILEINFO *ptifi=NULL;

   if (pszFile == NULL) return NULL;

   for (ptifi=tgIni.pFirstInfo; ptifi != NULL; ptifi=ptifi->pNextInfo) {
      if (UtilStrICmp(pszFile, ptifi->pszFile) == 0) {
         if (ptifi->bCheckFileTime) {
            time_t stFileTime;

            memset(&stFileTime, 0, sizeof(time_t));
            if (!GetLastModifiedTime(pszFile, &stFileTime) ||
                  CompareFileTime(&stFileTime, &ptifi->stFileTime) > 0) {
               DiscardFileInfo(ptifi);
               UnlinkFileInfo(ptifi);
               free(ptifi);
               ptifi = NULL;
               break;
            }
         }
         if (ptifi != NULL) return ptifi;
      }
   }
   if (!bAutoLoad) return NULL;
   if ((ptifi=AllocFileInfo(pszFile)) == NULL) return NULL;
   if (!ParseFile(ptifi)) {
      DiscardFileInfo(ptifi);
      UnlinkFileInfo(ptifi);
      free(ptifi);
      return NULL;
   }
   return ptifi;
}

static
int TgIniDiscardFileInfo(pszFile)
   char *pszFile;
{
   TGINIFILEINFO *ptifi=TgIniFindFileInfo(pszFile, FALSE);
   int ok=TRUE;
   
   if (ptifi == NULL) return TRUE;
   
   ok = DiscardFileInfo(ptifi);
   
   UnlinkFileInfo(ptifi);
   free(ptifi);
   return ok;
}

static
void TgIniBeginFastProfile(ptifi)
   TGINIFILEINFO *ptifi;
{
   if (ptifi != NULL) ptifi->bCheckFileTime = FALSE;
}

static
void TgIniEndFastProfile(ptifi)
   TGINIFILEINFO *ptifi;
{
   if (ptifi != NULL) ptifi->bCheckFileTime = TRUE;
}

static
int TgIniSetProfileStripQuotes(ptifi, bStripQuotes)
   TGINIFILEINFO *ptifi;
   int bStripQuotes;
{
   if (ptifi != NULL) {
      int bReturn=ptifi->bStripQuotes;

      ptifi->bStripQuotes = bStripQuotes;
      return bReturn;
   }
   return FALSE;
}

static
char *TgIniGetProfileString(pszSection, pszEntry, ptifi)
   char *pszSection, *pszEntry;
   TGINIFILEINFO *ptifi;
{
   char *buf=NULL;
   int buf_sz=0, cur_len=0;
   
   if (pszSection == NULL) {
      TGINISECTIONINFO *ptisi=NULL;

      for (ptisi=ptifi->pFirstSection; ptisi != NULL;
            ptisi=ptisi->pNextSection) {
         int len=strlen(ptisi->pszSection);
         
         while (len+cur_len+6 >= buf_sz) {
            buf_sz += 0x100;
            if (buf == NULL) {
               buf = (char*)malloc(buf_sz);
            } else {
               buf = (char*)realloc(buf, (size_t)buf_sz);
            }
            if (buf == NULL) {
               FailAllocMessage();
               return NULL;
            }
         }
         strcpy(&buf[cur_len], ptisi->pszSection);
         cur_len = (int)((&buf[len+cur_len+1]) - buf);
      }
      if (buf != NULL) buf[cur_len] = '\0';
   } else if (pszEntry == NULL) {
      TGINISECTIONINFO *ptisi=FindSectionInfo(ptifi, pszSection);
      
      if (ptisi != NULL) {
         TGINIENTRYINFO *ptiei=NULL;

         for (ptiei=ptisi->pFirstEntry; ptiei != NULL;
               ptiei=ptiei->pNextEntry) {
            int len=0;

            if (strcmp(ptiei->pszEntry, ";") == 0) {
               /*
                * skip comments, remember that the key is ";"
                * for a comment line
                */
               continue;
            }
            len = strlen(ptiei->pszEntry);
            
            while (len+cur_len+6 >= buf_sz) {
               buf_sz += 0x100;
               if (buf == NULL) {
                  buf = (char*)malloc(buf_sz);
               } else {
                  buf = (char*)realloc(buf, (size_t)buf_sz);
               }
               if (buf == NULL) {
                  FailAllocMessage();
                  return NULL;
               }
            }
            strcpy(&buf[cur_len], ptiei->pszEntry);
            cur_len = (int)((&buf[len+cur_len+1]) - buf);
         }
      }
      if (buf != NULL) buf[cur_len] = '\0';
   } else {
      TGINISECTIONINFO *ptisi=FindSectionInfo(ptifi, pszSection);
      
      if (ptisi != NULL) {
         TGINIENTRYINFO *ptiei=FindEntryInfo(ptisi, pszEntry);
         
         if (ptiei != NULL) {
            buf = UtilStrDup(ptiei->pszValue);
            if (buf != NULL && ptifi->bStripQuotes) {
               int nNumChars=strlen(buf);

               if (nNumChars > 1 && buf[0] == buf[nNumChars-1]) {
                  char *pszLastChar=(&buf[nNumChars-1]);

                  if (*buf == '\'' || *buf == '"') {
                     char *pby=buf, *pby1=(&buf[1]);

                     while (pby1 != pszLastChar) {
                        *pby++ = *pby1++;
                     }
                     *((char*)pby) = '\0';
                  }
               }
            }
         }
      }
   }
   return buf;
}

static
int TgIniWriteProfileString(pszSection, pszEntry, pszValue, ptifi)
   char *pszSection, *pszEntry, *pszValue;
   TGINIFILEINFO *ptifi;
{
   TGINISECTIONINFO *ptisi=FindSectionInfo(ptifi, pszSection);
   
   if (ptisi == NULL) {
      /* append */
      if ((ptisi=AllocSectionInfo(ptifi, pszSection, FALSE)) == NULL) {
         return FALSE;
      }
   }
   if (pszEntry == NULL) {
      TGINIENTRYINFO *ptiei=NULL, *ptiei_next;
   
      for (ptiei=ptisi->pFirstEntry; ptiei != NULL; ptiei=ptiei_next) {
         ptiei_next = ptiei->pNextEntry;
         DiscardEntryInfo(ptiei);
         free(ptiei);
      }
      ptisi->pFirstEntry = ptisi->pLastEntry = NULL;
   } else if (strcmp(pszEntry, ";") == 0) {
      /* can't write a comment line into an INI file */
      return FALSE;
   } else {
      TGINIENTRYINFO *ptiei=NULL;
      
      if (!ptisi->bAllowDupKey) {
         ptiei = FindEntryInfo(ptisi, pszEntry);
      }
      if (ptiei == NULL) {
         /* append */
         if (AllocEntryInfo(ptisi, pszEntry,
               (pszValue==NULL ? "" : pszValue)) == NULL) {
            return FALSE;
         }
      } else if (pszValue == NULL) {
         if (ptiei->pPrevEntry == NULL) {
            ptisi->pFirstEntry = ptiei->pNextEntry;
         } else {
            ptiei->pPrevEntry->pNextEntry = ptiei->pNextEntry;
         }
         if (ptiei->pNextEntry == NULL) {
            ptisi->pLastEntry = ptiei->pPrevEntry;
         } else {
            ptiei->pNextEntry->pPrevEntry = ptiei->pPrevEntry;
         }
         DiscardEntryInfo(ptiei);
         free(ptiei);
      } else {
         if (ptisi->bValuelessKey) {
#ifdef _DEBUG /* debug, do not translate */
            fprintf(stderr, "%s %s section [%s] of '%s'.\n",
                  "WARNING: tgWriteProfileString() is called with a",
                  "non-NULL pszValue for a valueless key",
                  ptisi->pszSection, ptifi->pszFile);
#endif /* _DEBUG */
         }
         /* replace */
         UtilFree(ptiei->pszValue);
         if ((ptiei->pszValue=UtilStrDup(pszValue)) ==
               NULL) {
            return FALSE;
         }
      }
   }
   ptifi->bModified = TRUE;
   return TRUE;
}

static
void TgIniBeginDupKeySection(pszSection, ptifi)
   char *pszSection;
   TGINIFILEINFO *ptifi;
{
   TGINISECTIONINFO *ptisi=FindSectionInfo(ptifi, pszSection);
   TGINIENTRYINFO *ptiei=NULL;
      
   if (ptisi == NULL) return;

   for (ptiei=ptisi->pFirstEntry; ptiei != NULL; ptiei=ptiei->pNextEntry) {
      ptiei->bScanned = FALSE;
   }
   ptisi->bAllowDupKey = TRUE;
}

static
void TgIniEndDupKeySection(pszSection, ptifi)
   char *pszSection;
   TGINIFILEINFO *ptifi;
{
   TGINISECTIONINFO *ptisi=FindSectionInfo(ptifi, pszSection);
      
   if (ptisi == NULL) return;

   ptisi->bAllowDupKey = FALSE;
}

static
int TgIniBeginValuelessKeySection(pszSection, ptifi)
   char *pszSection;
   TGINIFILEINFO *ptifi;
{
   TGINISECTIONINFO *ptisi=FindSectionInfo(ptifi, pszSection);
   int bValuelessKey=FALSE;
      
   if (ptisi == NULL) {
      if ((ptisi=AllocSectionInfo(ptifi, pszSection, TRUE)) == NULL) {
         return FALSE;
      }
   }
   if (ptisi->pFirstEntry == NULL) {
      if (!ptisi->bValuelessKey) {
         ptisi->bValuelessKey = TRUE;
      }
   } else {
      if (!ptisi->bValuelessKey) {
#ifdef _DEBUG /* debug, do not translate */
         fprintf(stderr, "%s non-valueless section [%s] of '%s'.\n",
               "WARNING: tgBeginValuelessKeySection() is called for a",
               pszSection, ptifi->pszFile);
#endif /* _DEBUG */
      }
   }
   bValuelessKey = ptisi->bValuelessKey;

   ptisi->bValuelessKey = TRUE;
   return bValuelessKey;
}

static
void TgIniEndValuelessKeySection(pszSection, ptifi, bValuelessKey)
   char *pszSection;
   TGINIFILEINFO *ptifi;
   int bValuelessKey;
{
   TGINISECTIONINFO *ptisi=FindSectionInfo(ptifi, pszSection);
      
   if (ptisi == NULL) return;

   if (ptisi->bValuelessKey != bValuelessKey) {
#ifdef _DEBUG /* debug, do not translate */
      fprintf(stderr, "%s valuess-ness for section [%s] of '%s'.\n",
            "WARNING: tgEndValuelessKeySection() changes the",
            pszSection, ptifi->pszFile);
#endif /* _DEBUG */
   }
   ptisi->bValuelessKey = bValuelessKey;
}

static
int TgIniIsValuelessKeySection(pszSection, ptifi)
   char *pszSection;
   TGINIFILEINFO *ptifi;
{
   TGINISECTIONINFO *ptisi=FindSectionInfo(ptifi, pszSection);
      
   if (ptisi == NULL) {
#ifdef _DEBUG /* debug, do not translate */
      fprintf(stderr, "%s section [%s] of '%s'.\n",
            "WARNING: tgIsValuelessKeySection() is called with a non-existant",
            pszSection, ptifi->pszFile);
#endif /* _DEBUG */
      return FALSE;
   }
   return ptisi->bValuelessKey;
}

static
int TgIniDeleteDupKeyValue(pszSection, pszEntry, pszValue, ptifi)
   char *pszSection, *pszEntry, *pszValue;
   TGINIFILEINFO *ptifi;
{
   TGINISECTIONINFO *ptisi=FindSectionInfo(ptifi, pszSection);
   TGINIENTRYINFO *ptiei=NULL;
   
   if (ptisi == NULL) {
      return TRUE;
   }
   TgIniBeginDupKeySection(pszSection, ptifi);
   for (ptiei=FindEntryInfo(ptisi, pszEntry); ptiei != NULL;
         ptiei=FindEntryInfo(ptisi, pszEntry)) {
      if (UtilStrICmp(pszValue, ptiei->pszValue) == 0) {
         if (ptiei->pPrevEntry == NULL) {
            ptisi->pFirstEntry = ptiei->pNextEntry;
         } else {
            ptiei->pPrevEntry->pNextEntry = ptiei->pNextEntry;
         }
         if (ptiei->pNextEntry == NULL) {
            ptisi->pLastEntry = ptiei->pPrevEntry;
         } else {
            ptiei->pNextEntry->pPrevEntry = ptiei->pPrevEntry;
         }
         DiscardEntryInfo(ptiei);
         free(ptiei);
         ptifi->bModified = TRUE;
         break;
      }
   }
   TgIniEndDupKeySection(pszSection, ptifi);
   return TRUE;
}

static
void TgIniCleanUp()
{
   TGINIFILEINFO *ptifi=NULL, *ptifi_next;

   for (ptifi=tgIni.pFirstInfo; ptifi != NULL; ptifi=ptifi_next) {
      ptifi_next = ptifi->pNextInfo;
      DiscardFileInfo(ptifi);
      free(ptifi);
   }
   tgIni.pFirstInfo = tgIni.pLastInfo = NULL;
}

/* -------------------- Exported Functions -------------------- */

void tgCleanUpProfile()
{
   TgIniCleanUp();
}

char *tgFreeProfileString(pszStr)
   char *pszStr;
{
   UtilFree(pszStr);
   return NULL;
}

void tgBeginFastProfile(pszFile)
   char *pszFile;
{
   TGINIFILEINFO *ptifi=TgIniFindFileInfo(pszFile, TRUE);
   
   if (ptifi == NULL) return;
   
   TgIniBeginFastProfile(ptifi);
}

void tgEndFastProfile(pszFile)
   char *pszFile;
{
   TGINIFILEINFO *ptifi=TgIniFindFileInfo(pszFile, TRUE);
   
   if (ptifi == NULL) return;
   
   TgIniEndFastProfile(ptifi);
}

int tgSetProfileStripQuotes(pszFile, bStripQuotes)
   char *pszFile;
   int bStripQuotes;
{
   TGINIFILEINFO *ptifi=TgIniFindFileInfo(pszFile, TRUE);
   
   if (ptifi == NULL) return FALSE;
   
   return TgIniSetProfileStripQuotes(ptifi, bStripQuotes);
}

char *tgGetProfileString(pszSection, pszEntry, pszFile)
   char *pszSection, *pszEntry, *pszFile;
{
   TGINIFILEINFO *ptifi=TgIniFindFileInfo(pszFile, TRUE);
   
   if (ptifi == NULL) return NULL;
   
   return TgIniGetProfileString(pszSection, pszEntry, ptifi);
}

int tgGetProfileInt(pszSection, pszEntry, nDefault, pszFile)
   char *pszSection, *pszEntry, *pszFile;
   int nDefault;
{
   TGINIFILEINFO *ptifi=NULL;
   char *c_ptr=NULL;
   int rc=nDefault;

   if (pszSection == NULL) {
      int rc=(TgIniDiscardFileInfo(pszFile) ? nDefault : (int)(nDefault==0));
      
      return rc;
   }
   ptifi = TgIniFindFileInfo(pszFile, TRUE);
   
   if (ptifi == NULL) return nDefault;
   
   c_ptr = TgIniGetProfileString(pszSection, pszEntry, ptifi);
   rc = (c_ptr == NULL ? nDefault : (int)atoi(c_ptr));
   
   UtilFree(c_ptr);
   return rc;
}

int tgWriteProfileString(pszSection, pszEntry, pszString, pszFile)
   char *pszSection, *pszEntry, *pszString, *pszFile;
{
   TGINIFILEINFO *ptifi=TgIniFindFileInfo(pszFile, TRUE);

   if (pszSection == NULL) {
      if (ptifi == NULL || !ptifi->bModified) return TRUE;
      return WriteOutIniFile(ptifi, NULL);
   }
   return TgIniWriteProfileString(pszSection, pszEntry, pszString, ptifi);
}

void tgBeginDupKeySection(pszSection, pszFile)
   char *pszSection, *pszFile;
{
   TGINIFILEINFO *ptifi=NULL;

   if (pszSection == NULL) return;

   ptifi = TgIniFindFileInfo(pszFile, TRUE);
   
   if (ptifi == NULL) return;
   
   TgIniBeginDupKeySection(pszSection, ptifi);
}

void tgEndDupKeySection(pszSection, pszFile)
   char *pszSection, *pszFile;
{
   TGINIFILEINFO *ptifi=NULL;

   if (pszSection == NULL) return;

   ptifi = TgIniFindFileInfo(pszFile, TRUE);
   
   if (ptifi == NULL) return;
   
   TgIniEndDupKeySection(pszSection, ptifi);
}

int tgBeginValuelessKeySection(pszSection, pszFile)
   char *pszSection, *pszFile;
{
   TGINIFILEINFO *ptifi=NULL;

   if (pszSection == NULL) return FALSE;

   ptifi = TgIniFindFileInfo(pszFile, TRUE);
   
   if (ptifi == NULL) return FALSE;
   
   return TgIniBeginValuelessKeySection(pszSection, ptifi);
}

void tgEndValuelessKeySection(pszSection, pszFile, bValuelessKey)
   char *pszSection, *pszFile;
   int bValuelessKey;
{
   TGINIFILEINFO *ptifi=NULL;
   
   if (pszSection == NULL) return;

   ptifi = TgIniFindFileInfo(pszFile, TRUE);
   
   if (ptifi == NULL) return;
   
   TgIniEndValuelessKeySection(pszSection, ptifi, bValuelessKey);
}

int tgIsValuelessKeySection(pszSection, pszFile)
   char *pszSection, *pszFile;
{
   TGINIFILEINFO *ptifi=NULL;
   
   if (pszSection == NULL) return FALSE;

   ptifi = TgIniFindFileInfo(pszFile, TRUE);
   
   if (ptifi == NULL) return FALSE;
   
   return TgIniIsValuelessKeySection(pszSection, ptifi);
}

int tgDeleteDupKeyValue(pszSection, pszEntry, pszValue, pszFile)
   char *pszSection, *pszEntry, *pszValue, *pszFile;
{
   TGINIFILEINFO *ptifi=NULL;

   if (pszFile == NULL || pszSection == NULL || pszEntry == NULL ||
         pszValue == NULL) {
      return FALSE;
   }
   ptifi = TgIniFindFileInfo(pszFile, TRUE);
   
   if (ptifi == NULL) return TRUE;
   
   return TgIniDeleteDupKeyValue(pszSection, pszEntry, pszValue, ptifi);
}

