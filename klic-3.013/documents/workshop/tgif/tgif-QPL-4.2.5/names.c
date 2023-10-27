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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/names.c,v 1.10 2011/05/16 16:21:58 william Exp $
 */

#define _INCLUDE_FROM_NAMES_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "auxtext.e"
#include "box.e"
#include "button.e"
#include "choose.e"
#include "cutpaste.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "file.e"
#include "font.e"
#include "import.e"
#include "ini.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "menu.e"
#include "msg.e"
#include "names.e"
#include "navigate.e"
#include "raster.e"
#include "rect.e"
#include "remote.e"
#include "setup.e"
#include "strtbl.e"
#include "util.e"
#include "xpixmap.e"

char curDomainName[MAXPATHLENGTH+1];
char curDomainPath[MAXPATHLENGTH+1];
char curDir[MAXPATHLENGTH+1];
char curLocalDir[MAXPATHLENGTH+1];
char curSymDir[MAXPATHLENGTH+1];

int doubleClickInterval=300;
int importFromLibrary=FALSE;
int importingFromExec=FALSE;
int curDirIsLocal=TRUE;

int ignoreDirectoryFlag=FALSE;

static int domainInIni=FALSE;
static int domainInResource=TRUE;

static char curImportDir[MAXPATHLENGTH+1];

static DspList *symbolList=NULL;
static int numSymbols=0;
static DspList *dirList=NULL;
static int numDirEntries=0;

static DspList *topOfSymLinkList=NULL, *topOfDirLinkList=NULL;

static
void CleanUpDirList()
{
   DspList *dsp_ptr=NULL, *next_dsp=NULL;

   for (dsp_ptr=topOfDirLinkList; dsp_ptr != NULL; dsp_ptr=next_dsp) {
      next_dsp = dsp_ptr->next;
      free(dsp_ptr);
   }
   topOfDirLinkList = NULL;
}

static
void CleanUpSymList()
{
   DspList *dsp_ptr=NULL, *next_dsp=NULL;

   for (dsp_ptr=topOfSymLinkList; dsp_ptr != NULL; dsp_ptr=next_dsp) {
      next_dsp = dsp_ptr->next;
      free(dsp_ptr);
   }
   topOfSymLinkList = NULL;
}

static
char *ReadPath(path_str, dir_str)
   char *path_str, *dir_str;
{
   register char *s1, *s2;

   s1 = path_str;
   while (*s1==' ' || *s1=='\t' || *s1=='\n') s1++;
   if (*s1 == '~') {
      strcpy(dir_str, homeDir);
      s2 = &dir_str[strlen(dir_str)];
      s1++;
   } else {
      s2 = dir_str;
   }
   for ( ; *s1 != '\0' && *s1 != ':'; s1++) {
      if (*s1 == '\\') {
         UtilStrCpy(s1, s1+1);
      } else {
         *s2++ = *s1;
      }
   }
   *s2 = '\0';
   if (*s1 == ':') s1++;
   return s1;
}

static
void CleanUpSymPath()
{
   if (symPath != NULL) {
      int i;

      for (i=0; i < symPathNumEntries; i++) {
         if (symPath[i] != NULL) {
            free(symPath[i]);
         }
      }
      if (symPath != NULL) free(symPath);
      symPath = NULL;
   }
}

void ParseSymPath(path_str)
   char *path_str;
{
   int i, path_len=0;
   char *s, dir_str[MAXPATHLENGTH+1];

   CleanUpSymPath();

   for (i=0, s=path_str; *s != '\0'; ) {
      s = ReadPath(s, dir_str);
      UtilTrimBlanks(dir_str);
      if (*dir_str != '\0') i++;
   }
   symPath = (char**)malloc(i*sizeof(char*));
   if (symPath == NULL) FailAllocMessage();
   symPathNumEntries = i;
   *curDomainPath = '\0';
   for (i=0, s=path_str; *s != '\0'; ) {
      s = ReadPath(s, dir_str);
      UtilTrimBlanks(dir_str);
      if (*dir_str != '\0') {
         symPath[i] = (char*)malloc((MAXPATHLENGTH+1)*sizeof(char));
         if (symPath[i] == NULL) FailAllocMessage();
         strcpy(symPath[i], dir_str);
         if (path_len == 0) {
            sprintf(&curDomainPath[path_len], "%s", dir_str);
            path_len += strlen(dir_str);
         } else {
            sprintf(&curDomainPath[path_len], ":%s", dir_str);
            path_len += strlen(dir_str)+1;
         }
         i++;
      }
   }
}

static
int LargerStr(S1, S2)
   register char *S1, *S2;
   /* returns TRUE if S1 > S2 */
{
   while (*S1 == *S2 && *S1 != '\0' && *S2 != '\0') { S1++; S2++; }

   return (*S1 > *S2);
}

static
DspList *SymbolListing()
{
   int len, path_index, count = 0, reject, sym_ext_len, pin_ext_len;
   char path[MAXPATHLENGTH], sym_ext_str[MAXSTRING], pin_ext_str[MAXSTRING];
   DspList *dsp_ptr, *head_ptr, *tail_ptr, *p, *p1;
   DIR *dirp;
   DIR_ENTRY *d;

   head_ptr = tail_ptr = NULL;

   sprintf(sym_ext_str, ".%s", SYM_FILE_EXT);
   sym_ext_len = strlen(sym_ext_str);
   sprintf(pin_ext_str, ".%s", PIN_FILE_EXT);
   pin_ext_len = strlen(pin_ext_str);
   for (path_index = 0; path_index < symPathNumEntries; path_index++) {
      strcpy(path, symPath[path_index]);
      if (strcmp(".", path) == 0) {
         if (curDirIsLocal) {
            strcpy(path, curDir);
         } else {
            strcpy(path, curLocalDir);
         }
      }
      if ((dirp=opendir(path)) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_DIR_FOR_READING),
               path);
         Msg(gszMsgBox);
         continue;
      }
      while ((d=readdir(dirp)) != NULL) {
         int symbol_is_pin=FALSE;

         len = strlen(d->d_name);
         if (len > sym_ext_len &&
               (strcmp(sym_ext_str, &d->d_name[len-sym_ext_len]) == 0)) {
            d->d_name[len-sym_ext_len] = '\0';
         } else if (len > pin_ext_len &&
               (strcmp(pin_ext_str, &d->d_name[len-pin_ext_len]) == 0)) {
            d->d_name[len-pin_ext_len] = '\0';
            symbol_is_pin = TRUE;
         } else {
            continue;
         }
         if (head_ptr == NULL) {
            head_ptr = tail_ptr = (DspList*)malloc(sizeof(DspList));
            if (head_ptr == NULL) FailAllocMessage();
            memset(head_ptr, 0, sizeof(DspList));
            UtilStrCpyN(head_ptr->itemstr, sizeof(head_ptr->itemstr),
                  d->d_name);
            UtilStrCpyN(head_ptr->pathstr, sizeof(head_ptr->pathstr), path);
            /* The directory field is TRUE if the entry has PIN_FILE_EXT. */
            head_ptr->directory = symbol_is_pin;
         } else {
            p1 = NULL;
            reject = FALSE;
            for (p=head_ptr; p != NULL; p = p->next) {
               if (strcmp(d->d_name, p->itemstr) == 0) {
                  reject = TRUE;
                  break;
               } else if (LargerStr(d->d_name, p->itemstr)) {
                  p1 = p;
               } else {
                  break;
               }
            }
            if (reject) continue;
   
            dsp_ptr = (DspList*)malloc(sizeof(DspList));
            if (dsp_ptr == NULL) FailAllocMessage();
            memset(dsp_ptr, 0, sizeof(DspList));
            dsp_ptr->next = p;
            UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), d->d_name);
            UtilStrCpyN(dsp_ptr->pathstr, sizeof(dsp_ptr->pathstr), path);
            /* The directory field is TRUE if the entry has PIN_FILE_EXT. */
            dsp_ptr->directory = symbol_is_pin;

            if (p == NULL) {
               /* dsp_ptr has the largest element */
               tail_ptr->next = dsp_ptr;
               tail_ptr = dsp_ptr;
            } else if (p1 == NULL) {
               head_ptr = dsp_ptr;
            } else {
               p1->next = dsp_ptr;
            }
         }
         count++;
      }
      closedir(dirp);
   }
   numSymbols = count;
   return head_ptr;
}

static
void BuildSymbolList()
{
   int i=0, watch_cursor=watchCursorOnMainWindow;
   DspList *dsp_ptr=NULL, *next_dsp=NULL;

   if (topOfSymLinkList == NULL) {
      if (!watch_cursor) {
         SetWatchCursor(drawWindow);
         SetWatchCursor(mainWindow);
      }
      if ((topOfSymLinkList = SymbolListing()) != NULL) BuildSymbolList();
      if (!watch_cursor) {
         SetDefaultCursor(mainWindow);
         ShowCursor();
      }
   }
   if (topOfSymLinkList != NULL) {
      if (symbolList != NULL) free(symbolList);

      symbolList = (DspList*)malloc(numSymbols*sizeof(DspList));
      if (symbolList == NULL) FailAllocMessage();
      memset(symbolList, 0, numSymbols*sizeof(DspList));
      dsp_ptr = topOfSymLinkList;
      for (i=0; i < numSymbols; i++, dsp_ptr = next_dsp) {
         next_dsp = dsp_ptr->next;
         UtilStrCpyN(symbolList[i].itemstr, sizeof(symbolList[i].itemstr),
               dsp_ptr->itemstr);
         UtilStrCpyN(symbolList[i].pathstr, sizeof(symbolList[i].pathstr),
               dsp_ptr->pathstr);
         symbolList[i].directory = dsp_ptr->directory;
         symbolList[i].next = &symbolList[i+1];
         free(dsp_ptr);
      }
      symbolList[numSymbols-1].next = NULL;
      topOfSymLinkList = NULL;
   }
}

static
DspList *DirListing(Path, ExtStr, OtherExtStr)
   char *Path, *ExtStr, *OtherExtStr;
{
   DspList *dsp_ptr, *head_ptr, *tail_ptr, *p, *p1;
   DIR *dirp;
   DIR_ENTRY *d;
   int len, ext_len, count = 0;
   char path[MAXPATHLENGTH], s[MAXPATHLENGTH], *ext_str=NULL;
   struct stat stat_buf;

   if (ExtStr != NULL && *ExtStr != '\0') {
      if (OtherExtStr != NULL && *OtherExtStr != '\0') {
         ext_len = (strlen(ExtStr)<<1)+3+strlen(OtherExtStr);
         if ((ext_str=SetUpExtStr(ext_len, ExtStr, OtherExtStr)) == NULL) {
            return NULL;
         }
      } else {
         ext_len = (strlen(ExtStr)<<1)+3;
         if ((ext_str=SetUpExtStr(ext_len, ExtStr, "")) == NULL) {
            return NULL;
         }
      }
   } else if (OtherExtStr != NULL && *OtherExtStr != '\0') {
      ext_len = strlen(OtherExtStr)+3;
      if ((ext_str=SetUpExtStr(ext_len, "", OtherExtStr)) == NULL) {
         return NULL;
      }
   }
   ext_len = (ext_str == NULL ? 0 : strlen(ext_str));
   if (*Path == '\0') {
      strcpy(path, DIR_SEP_STR);
      if ((dirp=opendir(path)) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_DIR_FOR_READING),
               path);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         if (ext_str != NULL) free(ext_str);
         return NULL;
      }
   } else {
      strcpy(path, Path);
      if ((dirp=opendir(path)) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_DIR_FOR_READING),
               path);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         if (ext_str != NULL) free(ext_str);
         return NULL;
      }
#ifdef __CYGWIN__
      if (path[strlen(path)-1] != DIR_SEP) {
         strcat(path, DIR_SEP_STR);
      }
#else /* ~__CYGWIN__ */
      strcat(path, DIR_SEP_STR);
#endif /* __CYGWIN__ */
   }

#ifdef VMS
   /* Fake the .. entry for VMS */
   head_ptr = tail_ptr = (DspList*)malloc(sizeof(DspList));
   if (head_ptr == NULL) FailAllocMessage();
   memset(head_ptr, 0, sizeof(DspList));
   head_ptr->directory = TRUE;
   UtilStrCpyN(head_ptr->itemstr, sizeof(head_ptr->itemstr), "../");
   head_ptr->next = NULL;
   count = 1;
#else
   head_ptr = tail_ptr = NULL;
#endif /* VMS */

   while ((d=readdir(dirp)) != NULL) {
      len = strlen(d->d_name);
      if (ext_len == 0) {
#ifdef VMS
         if (len > 4 && (strcmp(".dir", &d->d_name[len-4]) == 0)) {
            d->d_name[len-4] = DIR_SEP;
            d->d_name[len-4+1] = '\0';
            dsp_ptr = (DspList*)malloc(sizeof(DspList));
            if (dsp_ptr == NULL) FailAllocMessage();
            memset(dsp_ptr, 0, sizeof(DspList));
            dsp_ptr->directory = TRUE;
            UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), d->d_name);
         } else if (strcmp(d->d_name, ".") == 0) {
#else
         if (strcmp(d->d_name, ".") == 0) {
#endif /* VMS */
            continue;
         } else {
            sprintf(s, "%s%s", path, d->d_name);
            if (stat(s, &stat_buf) != 0) {
               int show_msg=TRUE;
#ifndef _NO_LSTAT
               if (lstat(s, &stat_buf) == 0 &&
                     ((stat_buf.st_mode & S_IFLNK) == S_IFLNK)) {
                  show_msg = FALSE;
               }
#endif /* _NO_LSTAT */
               if (show_msg) {
                  sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_STAT_GIVEN_FILE),
                        s);
                  Msg(gszMsgBox);
               }
               continue;
            } else {
               dsp_ptr = (DspList*)malloc(sizeof(DspList));
               if (dsp_ptr == NULL) FailAllocMessage();
               memset(dsp_ptr, 0, sizeof(DspList));
               if (stat_buf.st_mode & S_IFDIR) {
                  dsp_ptr->directory = TRUE;
                  strcat(d->d_name, DIR_SEP_STR);
               } else {
                  dsp_ptr->directory = FALSE;
               }
               UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr),
                     d->d_name);
            }
         }
      } else if (ExtensionMatch(ext_str, d->d_name)) {
         dsp_ptr = (DspList*)malloc(sizeof(DspList));
         if (dsp_ptr == NULL) FailAllocMessage();
         memset(dsp_ptr, 0, sizeof(DspList));
         dsp_ptr->directory = FALSE;
         UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), d->d_name);
#ifdef VMS
      } else if (len > 4 && (strcmp(".dir", &d->d_name[len-4]) == 0)) {
         d->d_name[len-4] = DIR_SEP;
         d->d_name[len-4+1] = '\0';
         dsp_ptr = (DspList*)malloc(sizeof(DspList));
         if (dsp_ptr == NULL) FailAllocMessage();
         memset(dsp_ptr, 0, sizeof(DspList));
         dsp_ptr->directory = TRUE;
         UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), d->d_name);
#endif /* VMS */
      } else if (strcmp(d->d_name, ".") == 0) {
         continue;
      } else {
         sprintf(s, "%s%s", path, d->d_name);
         if (stat(s, &stat_buf) != 0) {
            int show_msg=TRUE;
#ifndef _NO_LSTAT
            if (lstat(s, &stat_buf) == 0 &&
                  ((stat_buf.st_mode & S_IFLNK) == S_IFLNK)) {
               show_msg = FALSE;
            }
#endif /* _NO_LSTAT */
            if (show_msg) {
               sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_STAT_GIVEN_FILE),
                     s);
               Msg(gszMsgBox);
            }
            continue;
         } else if (stat_buf.st_mode & S_IFDIR) {
            dsp_ptr = (DspList*)malloc(sizeof(DspList));
            if (dsp_ptr == NULL) FailAllocMessage();
            memset(dsp_ptr, 0, sizeof(DspList));
            dsp_ptr->directory = TRUE;
            strcat(d->d_name, DIR_SEP_STR);
            UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), d->d_name);
         } else {
            continue;
         }
      }
      if (head_ptr == NULL) {
         head_ptr = tail_ptr = dsp_ptr;
      } else {
         p1 = NULL;
         for (p = head_ptr; p != NULL; p = p->next) {
            if (LargerStr(d->d_name, p->itemstr)) {
               p1 = p;
            } else {
               break;
            }
         }
         dsp_ptr->next = p;
         if (p == NULL) {
            /* dsp_ptr has the largest element */
            tail_ptr->next = dsp_ptr;
            tail_ptr = dsp_ptr;
         } else if (p1 == NULL) {
            head_ptr = dsp_ptr;
         } else {
            p1->next = dsp_ptr;
         }
      }
      count++;
   }
   closedir(dirp);
   numDirEntries = count;
   if (ext_str != NULL) free(ext_str);
   return head_ptr;
}

static
void BuildDirList()
{
   int i=0, watch_cursor=watchCursorOnMainWindow;
   DspList *dsp_ptr=NULL, *next_dsp=NULL;

   if (topOfDirLinkList == NULL) {
      if (!watch_cursor) {
         SetWatchCursor(drawWindow);
         SetWatchCursor(mainWindow);
      }
      if (curDirIsLocal) {
         if ((topOfDirLinkList=DirListing(curDir, OBJ_FILE_EXT, "")) != NULL) {
            BuildDirList();
         }
      } else if ((topOfDirLinkList=DirListing(curLocalDir, OBJ_FILE_EXT, "")) !=
            NULL) {
         BuildDirList();
      }
      if (!watch_cursor) {
         SetDefaultCursor(mainWindow);
         ShowCursor();
      }
   }
   if (topOfDirLinkList != NULL) {
      if (dirList != NULL) free(dirList);

      dirList = (DspList*)malloc(numDirEntries*sizeof(DspList));
      if (dirList == NULL) FailAllocMessage();
      memset(dirList, 0, numDirEntries*sizeof(DspList));
      dsp_ptr = topOfDirLinkList;
      for (i=0; i < numDirEntries; i++, dsp_ptr = next_dsp) {
         next_dsp = dsp_ptr->next;
         UtilStrCpyN(dirList[i].itemstr, sizeof(dirList[i].itemstr),
               dsp_ptr->itemstr);
         UtilStrCpyN(dirList[i].pathstr, sizeof(dirList[i].pathstr),
               dsp_ptr->pathstr);
         dirList[i].directory = dsp_ptr->directory;
         dirList[i].next = &dirList[i+1];
         free(dsp_ptr);
      }
      dirList[numDirEntries-1].next = NULL;
      topOfDirLinkList = NULL;
   }
}

static char gszDefaultDomainSec[]="DefaultDomain";
static char gszDomainPathsSec[]="DomainPaths";
static char gszDomainIniFile[MAXPATHLENGTH];

static
void InitDomain()
{
   char *c_ptr=NULL, domain_str[20], sym_path[80], cap_tool_name[MAXSTRING];
   int default_found=FALSE, need_to_copy_domain_into_ini=FALSE;
   char *pszDomainNames=NULL, *psz_default_path=NULL;

   sprintf(gszDomainIniFile, "%s%cdomain.ini", tgifDir, DIR_SEP);

   domainInIni = FALSE;
   domainInResource = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "DomainInIni")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      if ((pszDomainNames=tgGetProfileString(gszDomainPathsSec, NULL,
            gszDomainIniFile)) != NULL) {
         domainInIni = TRUE;
         domainInResource = FALSE;
         tgFreeProfileString(pszDomainNames);
      } else {
         need_to_copy_domain_into_ini = TRUE;
      }
   }
   if (domainInIni && (pszDomainNames=tgGetProfileString(gszDomainPathsSec,
         NULL, gszDomainIniFile)) != NULL) {
      char *pszKeys=NULL;

      if ((pszKeys=tgGetProfileString(gszDefaultDomainSec, NULL,
            gszDomainIniFile)) != NULL) {
         char *pszKey=pszKeys;
         char *pszPath=tgGetProfileString(gszDomainPathsSec, pszKey,
               gszDomainIniFile);

         if (pszPath != NULL || strcmp(pszKey, "Examples") == 0) {
            psz_default_path = UtilStrDup(pszPath==NULL ? TGIF_PATH : pszPath);
            if (psz_default_path == NULL) FailAllocMessage();

            UtilStrCpyN(curDomainName, sizeof(curDomainName), pszKey);
            default_found = TRUE;
            tgFreeProfileString(pszPath);
         }
         tgFreeProfileString(pszKeys);
      }
      if (!default_found) {
         /* just pick the first one */
         char *pszKey=pszDomainNames;
         char *pszPath=tgGetProfileString(gszDomainPathsSec, pszKey,
               gszDomainIniFile);

         if (pszPath != NULL) {
            psz_default_path = UtilStrDup(pszPath);
            if (psz_default_path == NULL) FailAllocMessage();

            UtilStrCpyN(curDomainName, sizeof(curDomainName), pszKey);
            default_found = TRUE;
            tgFreeProfileString(pszPath);
         }
         tgFreeProfileString(pszKeys);
      }
      tgFreeProfileString(pszDomainNames);
   }
   if (domainInResource && (c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "DefaultDomain")) != NULL) {
      int default_domain=atoi(c_ptr);

      sprintf(domain_str, "DomainPath%1d", default_domain);
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, domain_str)) != NULL) {
         char *c_ptr1;

         while (*c_ptr==' ' || *c_ptr=='\t' || *c_ptr=='\n') c_ptr++;
         if (*c_ptr != '\0' && (c_ptr1=strchr(c_ptr, ':')) != NULL) {
            int len=c_ptr1-c_ptr;

            strncpy(curDomainName, c_ptr, len);
            curDomainName[len] = '\0';
            psz_default_path = UtilStrDup(++c_ptr1);
            if (psz_default_path == NULL) FailAllocMessage();
            default_found = TRUE;
         }
      } else {
         sprintf(domain_str, "Domain%1d", default_domain);
         if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, domain_str)) != NULL) {
            if (*c_ptr != '\0') {
               strcpy(cap_tool_name, TOOL_NAME);
               UtilStrUpper(cap_tool_name);
               strcpy(curDomainName, c_ptr);
               sprintf(sym_path, "%s_%s", cap_tool_name, c_ptr);
               default_found = TRUE;
               domainInResource = FALSE;
            }
         }
      }
   }
   if (default_found) {
      if (domainInIni || domainInResource) {
         ParseSymPath(psz_default_path);
      } else if ((c_ptr=getenv(sym_path)) == NULL ||
            ((int)strlen(c_ptr)) >= MAXPATHLENGTH-1) {
         ParseSymPath(".");
      } else {
         ParseSymPath(c_ptr);
      }
   } else {
      char buf[MAXPATHLENGTH];

      strcpy(buf, ".:");
      UtilStrCpyN(&buf[2], sizeof(buf)-2, TGIF_PATH);
      ParseSymPath(buf);
   }
   if (need_to_copy_domain_into_ini) {
      CopyDomainInfoToIni();
      tgWriteProfileString(gszDefaultDomainSec, curDomainName, "",
            gszDomainIniFile);
      tgWriteProfileString(NULL, NULL, NULL, gszDomainIniFile);

      if ((pszDomainNames=tgGetProfileString(gszDomainPathsSec, NULL,
            gszDomainIniFile)) != NULL) {
         domainInIni = TRUE;
         domainInResource = FALSE;
         tgFreeProfileString(pszDomainNames);
      }
   }
   UtilFree(psz_default_path);
}

void InitNames()
{
   char *c_ptr=NULL;

   if (mainDisplay != NULL) InitNamesInfo();

   ignoreDirectoryFlag = FALSE;

   symbolList = dirList = NULL;
   numSymbols = numDirEntries = 0;
   topOfSymLinkList = topOfDirLinkList = NULL;

   *curDomainName = '\0';
   *curDomainPath = '\0';
   *curSymDir = '\0';
   strcpy(curDir, bootDir);
   strcpy(curLocalDir, bootDir);
   strcpy(curImportDir, bootDir);

   if (mainDisplay == NULL) return;

   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "DoubleClickInterval")) !=
         NULL) {
      doubleClickInterval = atoi(c_ptr);
   } else {
      doubleClickInterval = 300;
   }
   warpToWinCenter = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"WarpToWinCenter")) != NULL) {
      if (UtilStrICmp(c_ptr, "false") == 0) {
         warpToWinCenter = FALSE;
      }
   }
   importFromLibrary = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ImportFromLibrary")) != NULL) {
      if (UtilStrICmp(c_ptr, "true") == 0) {
         importFromLibrary = TRUE;
      }
   }
   InitDomain();
}

void UpdateDirInfo()
{
   CleanUpDirList();
}

void UpdateSymInfo()
{
   CleanUpSymList();
}

void CleanUpNames()
{
   CleanUpNamesInfo();

   CleanUpSymPath();
   if (symbolList != NULL) free(symbolList);
   if (dirList != NULL) free(dirList);
   symbolList = dirList = NULL;
   ignoreDirectoryFlag = FALSE;
}

char **MakeNameDspItemArray(Entries, DLPtr)
   int Entries;
   DspList *DLPtr;
{
   int i, j, len;
   char **dsp_ptr, *c_ptr;

   if (Entries == 0) return NULL;

   dsp_ptr = (char**)malloc(Entries*sizeof(char*));
   if (dsp_ptr == NULL) FailAllocMessage();
   c_ptr = (char*)malloc(Entries*(MAXPATHLENGTH+1)*sizeof(char));
   if (c_ptr == NULL) FailAllocMessage();
   for (i=0; i < Entries; i++, DLPtr=DLPtr->next) {
      dsp_ptr[i] = c_ptr;
      len = strlen(DLPtr->itemstr);
      if (!ignoreDirectoryFlag && !DLPtr->directory) {
         for (j = len; j >= 0 && DLPtr->itemstr[j] != DIR_SEP; j--) ;
         if (j >= 0) {
            strcpy(c_ptr, (&(DLPtr->itemstr[j]))+1);
         } else {
            strcpy(c_ptr, DLPtr->itemstr);
         }
      } else {
         strcpy(c_ptr, DLPtr->itemstr);
      }
      c_ptr += MAXPATHLENGTH;
   }
   return dsp_ptr;
}

int DirInSymPath(DirName)
   char *DirName;
{
   register int i;

   if (symPath == NULL) return FALSE;

   for (i = 0; i < symPathNumEntries; i++) {
      if (strcmp(DirName, symPath[i]) == 0) {
         return TRUE;
      }
   }
   return FALSE;
}

static int lengthOfLongestItem=INVALID;

static
char **MakeLongNameDspItemArray(Entries, DLPtr)
   int Entries;
   DspList *DLPtr;
{
   int i, len;
   char **dsp_ptr, *c_ptr;

   lengthOfLongestItem = INVALID;

   if (Entries == 0) return NULL;

   dsp_ptr = (char**)malloc(Entries*sizeof(char*));
   if (dsp_ptr == NULL) FailAllocMessage();
   c_ptr = (char*)malloc(Entries*(MAXPATHLENGTH+1)*sizeof(char));
   if (c_ptr == NULL) FailAllocMessage();
   for (i=0; i < Entries; i++, DLPtr=DLPtr->next) {
      dsp_ptr[i] = c_ptr;
      len = strlen(DLPtr->itemstr);
      strcpy(c_ptr, DLPtr->itemstr);
      if (len > lengthOfLongestItem) lengthOfLongestItem = len;

      c_ptr += MAXPATHLENGTH;
   }
   return dsp_ptr;
}

struct DirNamesInfoRec {
   char ext_str[81], other_ext_str[81];
} dirNamesInfo;

static
int GetNamesEntries(p_dsp_ptr, p_entries, pn_num_entries, pn_marked_index,
      pp_check_array, cur_buf, p_void)
   DspList **p_dsp_ptr;
   char ***p_entries, *cur_buf;
   int *pn_num_entries, *pn_marked_index;
   struct CheckArrayRec **pp_check_array;
   void *p_void;
{
   struct DirNamesInfoRec *p_dninfo=(struct DirNamesInfoRec*)p_void;
   DspList *dsp_ptr, *next_dsp;

   for (dsp_ptr=topOfDirLinkList; dsp_ptr != NULL; dsp_ptr=next_dsp) {
      next_dsp = dsp_ptr->next;
      free(dsp_ptr);
   }
   topOfDirLinkList = NULL;

   *p_dsp_ptr = NULL;
   *p_entries = NULL;
   *pn_num_entries = 0;
   *pn_marked_index = INVALID;

   if (*cur_buf == '\0') {
      /* this can only happen if the change_to_root */
      return TRUE;
   } else {
      char status_buf[MAX_STATUS_BTNS+1][MAXSTRING+1];
      int one_line_status=FALSE;

      SaveStatusStringsIntoBuf(status_buf, &one_line_status);
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_GET_DIR_LIST_PLEASE_WAIT),
            cur_buf);
      SetStringStatus(gszMsgBox);
      XSync(mainDisplay, False);

      if ((topOfDirLinkList=DirListing(cur_buf,
            p_dninfo->ext_str, p_dninfo->other_ext_str)) == NULL) {
         RestoreStatusStringsFromBuf(status_buf, one_line_status);
         return FALSE;
      }
      RestoreStatusStringsFromBuf(status_buf, one_line_status);
      *pn_num_entries = numDirEntries;
      if (topOfDirLinkList == NULL) {
         *p_entries = MakeNameDspItemArray(*pn_num_entries, dirList);
      } else {
         *p_entries = MakeNameDspItemArray(*pn_num_entries, topOfDirLinkList);
      }
   }
   return TRUE;
}

static
int NamesAfterLoop(p_dsp_ptr, p_entries, pn_num_entries, pn_marked_index,
      pp_check_array, cur_buf, btn_id, selected_index, p_void)
   DspList **p_dsp_ptr;
   char ***p_entries, *cur_buf;
   int *pn_num_entries, *pn_marked_index, btn_id, selected_index;
   struct CheckArrayRec **pp_check_array;
   void *p_void;
{
   if (*p_dsp_ptr != NULL) free(*p_dsp_ptr);
   if (*p_entries != NULL) {
      free(**p_entries);
      free(*p_entries);
   }
   *p_dsp_ptr = NULL;
   *p_entries = NULL;
   *pn_num_entries = 0;

   switch (btn_id) {
   case BUTTON_OK: break;
   case BUTTON_SETDIR:
      if (cur_buf != NULL) {
         int len=strlen(cur_buf);

         if (len > 4 && cur_buf[len-4] == DIR_SEP && cur_buf[len-3] == '.' &&
               cur_buf[len-2] == '.' && cur_buf[len-1] == DIR_SEP) {
            break;
         }
      }
      return FALSE;
   case BUTTON_CANCEL: break;
   }
   return TRUE;
}

static
int DirNames(TopStr, ExtStr, SelStr, JustSetDir)
   char *TopStr, *ExtStr, *SelStr;
   int *JustSetDir;
{
   char win_name[128], selected_str[MAXSTRING+1];
   int selected_index=INVALID, selected_btn_id;
   struct DirNamesInfoRec dninfo;

   *SelStr = '\0';
   *JustSetDir = FALSE;

   memset(&dninfo, 0, sizeof(struct DirNamesInfoRec));

   UtilStrCpyN(dirNamesInfo.ext_str, sizeof(dirNamesInfo.ext_str), ExtStr);
   UtilStrCpyN(dirNamesInfo.other_ext_str, sizeof(dirNamesInfo.other_ext_str),
         ExtStr);

   if (ExtStr != NULL && strcmp(ExtStr, EPSF_FILE_EXT) == 0) {
      sprintf(dirNamesInfo.other_ext_str, ".%s;.epsi", PS_FILE_EXT);
   } else if (ExtStr != NULL && strcmp(ExtStr, OBJ_FILE_EXT) == 0) {
      sprintf(dirNamesInfo.other_ext_str, ".%s;.%s", SYM_FILE_EXT,
            PIN_FILE_EXT);
      if (strstr(dirNamesInfo.other_ext_str, ".obj") == NULL) {
         strcat(dirNamesInfo.other_ext_str, ";.obj");
      }
      if (strstr(dirNamesInfo.other_ext_str, ".sym") == NULL) {
         strcat(dirNamesInfo.other_ext_str, ";.sym");
      }
      if (strstr(dirNamesInfo.other_ext_str, ".pin") == NULL) {
         strcat(dirNamesInfo.other_ext_str, ";.pin");
      }
      if (strstr(dirNamesInfo.other_ext_str, ".tgo") == NULL) {
         strcat(dirNamesInfo.other_ext_str, ";.tgo");
      }
      if (strstr(dirNamesInfo.other_ext_str, ".tgs") == NULL) {
         strcat(dirNamesInfo.other_ext_str, ";.tgs");
      }
      if (strstr(dirNamesInfo.other_ext_str, ".tgp") == NULL) {
         strcat(dirNamesInfo.other_ext_str, ";.tgp");
      }
      sprintf(selected_str, ";.obj.gz;.tgo.gz;.%s.gz", OBJ_FILE_EXT);
      strcat(dirNamesInfo.other_ext_str, selected_str);
   } else if (ExtStr != NULL && strcmp(ExtStr, "gif") == 0) {
      strcpy(dirNamesInfo.other_ext_str, ".GIF");
   } else if (ExtStr != NULL && strcmp(ExtStr, "png") == 0) {
      strcpy(dirNamesInfo.other_ext_str, ".PNG");
   } else if (ExtStr != NULL && strcmp(ExtStr, "jpeg") == 0) {
      strcpy(dirNamesInfo.other_ext_str, ".JPEG;.jpg;.JPG");
   } else {
      *dirNamesInfo.other_ext_str = '\0';
   }
   ResetNamesInfo();
   NamesSetTitle(TopStr);
   NamesAddButton(TgLoadCachedString(CSTID_OK), BUTTON_OK);
   if (!(importingFile || (curFileDefined && curDirIsLocal))) {
      NamesAddButton(TgLoadString(STID_SET_WORKING_DIR), BUTTON_SETDIR);
   }
   NamesAddButton(TgLoadCachedString(CSTID_CANCEL), BUTTON_CANCEL);
   NamesSetEntries(NULL, NULL, 0, NULL, TRUE, INVALID, 0);
   NamesSetStyle(NAMES_SELECT_FILE, NAMES_LOOP_MANY);
   NamesSetCallback((GetEntriesFunc*)GetNamesEntries,
         (AfterLoopFunc*)NamesAfterLoop, NULL);
   if (importingFile) {
      NamesSetDir(curImportDir);
   } else if (curDirIsLocal) {
      NamesSetDir(curDir);
   } else {
      NamesSetDir(curLocalDir);
   }
   if (!(importingFile || (curFileDefined && curDirIsLocal))) {
      Msg(TgLoadString(STID_CLICK_SETWORKDIR_TO_SET_DIR));
   }
   *selected_str = '\0';

   sprintf(win_name, "%s - %s", TOOL_NAME, TopStr);
   selected_btn_id = Names(win_name, &selected_index,
         selected_str, sizeof(selected_str), &dirNamesInfo);
   if (selected_btn_id == BUTTON_OK) {
      *JustSetDir = FALSE;
      strcpy(SelStr, selected_str);
      if (FileIsRemote(selected_str)) {
         /* index != INVALID means SelStr is valid */
         return 0;
      }
      return selected_index;
   } else if (selected_btn_id == BUTTON_SETDIR) {
      if (DirIsRemote(selected_str)) {
         *SelStr = '\0';
         *JustSetDir = FALSE;
         sprintf(gszMsgBox, TgLoadString(STID_SETWORKDIR_NOT_SUP_FOR_STR),
               selected_str);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else {
         int len=strlen(selected_str);

         if (len > 0 && selected_str[len-1] == DIR_SEP) {
            UtilShrinkName(selected_str);
            len = strlen(selected_str);
         }
         if (len > 0) {
            if (selected_str[len-1] == DIR_SEP) {
               selected_str[--len] = '\0';
            } else {
               while (len > 0 && selected_str[len-1] != DIR_SEP) {
                  selected_str[--len] = '\0';
               }
               if (len > 0) selected_str[--len] = '\0';
            }
         }
         strcpy(SelStr, selected_str);
         *JustSetDir = TRUE;
      }
   }
   return INVALID;
}

static
int ChooseAName(TopStr, entries, num_entries, marked_index)
   char *TopStr, **entries;
   int num_entries, marked_index;
{
   char win_name[128];
   int selected_index=INVALID;

   ResetNamesInfo();
   NamesSetTitle(TopStr);
   NamesAddButton(TgLoadCachedString(CSTID_OK), BUTTON_OK);
   NamesAddButton(TgLoadCachedString(CSTID_CANCEL), BUTTON_CANCEL);
   NamesSetEntries(NULL, entries, num_entries, NULL, TRUE, marked_index, 0);
   NamesSetStyle(NAMES_COMPLEX_SELECT_NAME, NAMES_LOOP_ONCE);
   sprintf(win_name, "%s - %s", TOOL_NAME, TopStr);
   if (Names(win_name, &selected_index, NULL, 0, NULL) == BUTTON_OK) {
      return selected_index;
   }
   return INVALID;
}

int SelectFileName(MsgStr, SelStr)
   char *MsgStr, *SelStr;
{
   int index, saved_num_dir_entries, just_set_dir;
   DspList *dsp_ptr, *next_dsp;
   char saved_cur_dir[MAXPATHLENGTH];

   if (curDirIsLocal) {
      strcpy(saved_cur_dir, curDir);
   } else {
      strcpy(saved_cur_dir, curLocalDir);
   }
   saved_num_dir_entries = numDirEntries;
   if ((index=DirNames(MsgStr,OBJ_FILE_EXT,SelStr,&just_set_dir)) == INVALID) {
      if (just_set_dir) {
         if (curDirIsLocal) {
            strcpy(curDir, SelStr);
         } else {
            strcpy(curLocalDir, SelStr);
         }
         BuildDirList();
         if (strcmp(saved_cur_dir, curDir) != 0 && DirInSymPath(".")) {
            UpdateSymInfo();
         }
         RedrawTitleWindow();
         sprintf(gszMsgBox, TgLoadString(STID_OPENSAVE_DIR_CHANGE_TO_REMAIN),
               curDir, curImportDir);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         Msg("");
      } else {
         numDirEntries = saved_num_dir_entries;
         for (dsp_ptr=topOfDirLinkList; dsp_ptr != NULL; dsp_ptr=next_dsp) {
            next_dsp = dsp_ptr->next;
            free(dsp_ptr);
         }
         topOfDirLinkList = NULL;
      }
      *SelStr = '\0';
      return INVALID;
   }
   BuildDirList();
   Msg("");
   return index;
}

int SelectFileNameToPaste(MsgStr, SelStr)
   char *MsgStr, *SelStr;
{
   int index, saved_num_dir_entries, just_set_dir;
   DspList *dsp_ptr, *next_dsp;
   char saved_cur_dir[MAXPATHLENGTH];

   if (curDirIsLocal) {
      strcpy(saved_cur_dir, curDir);
   } else {
      strcpy(saved_cur_dir, curLocalDir);
   }
   saved_num_dir_entries = numDirEntries;
   if ((index=DirNames(MsgStr,NULL,SelStr,&just_set_dir)) == INVALID) {
      if (just_set_dir) {
         if (curDirIsLocal) {
            strcpy(curDir, SelStr);
         } else {
            strcpy(curLocalDir, SelStr);
         }
         BuildDirList();
         if (strcmp(saved_cur_dir, curDir) != 0 && DirInSymPath(".")) {
            UpdateSymInfo();
         }
         RedrawTitleWindow();
         sprintf(gszMsgBox, TgLoadString(STID_OPENSAVE_DIR_CHANGE_TO_REMAIN),
               curDir, curImportDir);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         Msg("");
      } else {
         numDirEntries = saved_num_dir_entries;
         for (dsp_ptr=topOfDirLinkList; dsp_ptr != NULL; dsp_ptr=next_dsp) {
            next_dsp = dsp_ptr->next;
            free(dsp_ptr);
         }
         topOfDirLinkList = NULL;
      }
      *SelStr = '\0';
      return INVALID;
   }
   BuildDirList();
   Msg("");
   return index;
}

int SelectFileNameToImport(MsgStr, ExtStr, SelStr)
   char *MsgStr, *ExtStr, *SelStr;
{
   int index, saved_num_dir_entries, just_set_dir;
   DspList *dsp_ptr, *next_dsp;

   saved_num_dir_entries = numDirEntries;
   if ((index=DirNames(MsgStr, ExtStr, SelStr, &just_set_dir)) == INVALID) {
      if (just_set_dir) {
         /* This should not happen since SetDir is removed during importing. */
         strcpy(curImportDir, SelStr);
         sprintf(gszMsgBox, TgLoadString(STID_IMPORT_DIR_CHANGED_TO_REMAIN),
               curImportDir, curDir);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      *SelStr = '\0';
   } else {
      Msg("");
   }
   numDirEntries = saved_num_dir_entries;
   for (dsp_ptr=topOfDirLinkList; dsp_ptr != NULL; dsp_ptr=next_dsp) {
      next_dsp = dsp_ptr->next;
      free(dsp_ptr);
   }
   topOfDirLinkList = NULL;

   return index;
}

int GetSymbolPath(SymName, Pin, PathName)
   char *SymName, *PathName;
   int Pin;
{
   int i;

   if (topOfSymLinkList == NULL) {
      if ((topOfSymLinkList=SymbolListing()) != NULL) BuildSymbolList();
   }
   if (symbolList == NULL) return FALSE;

   for (i = 0; i < numSymbols; i++) {
      /* The directory field is TRUE if the entry has PIN_FILE_EXT. */
      if (symbolList[i].directory == Pin &&
            strcmp(SymName, symbolList[i].itemstr) == 0) {
         strcpy(PathName, symbolList[i].pathstr);
         return TRUE;
      }
   }
   return FALSE;
}

int NameInCurDir(FileName)
   char *FileName;
{
   int i;

   if (dirList == NULL) return FALSE;

   for (i = 0; i < numDirEntries; i++) {
      if (dirList[i].directory && strcmp(FileName, dirList[i].itemstr) == 0) {
         return TRUE;
      }
   }
   return FALSE;
}

static char **tmpDomainName=NULL;
static char **tmpDomainPath=NULL;
static int numTmpDomainNames=0;

static
void CleanUpTmpDomainName()
{
   if (tmpDomainName != NULL) {
      int i=0;

      for (i=0; i < numTmpDomainNames; i++) {
         UtilFree(tmpDomainName[i]);
         UtilFree(tmpDomainPath[i]);
      }
      free(tmpDomainName);
      free(tmpDomainPath);
      tmpDomainName = tmpDomainPath = NULL;
   }
}

static
DspList *DomainListing(pn_num_entries, name_only)
   int *pn_num_entries, name_only;
   /*
    * If (name_only == TRUE) returned DspList only has names.
    * If (name_only == FALSE) returned DspList has name=path.
    */
{
   int i=0, seen_examples=FALSE;
   char s[MAXSTRING], *c_ptr=NULL, **ppsz_keys=NULL;
   DspList *dsp_ptr=NULL, *head_ptr=NULL, *tail_ptr=NULL, *p=NULL, *p1=NULL;
   DspList *next_dsp=NULL;
   int one_line_status=FALSE;
   char status_buf[MAX_STATUS_BTNS+1][MAXSTRING+1];

   SaveStatusStringsIntoBuf(status_buf, &one_line_status);
   SetStringStatus(TgLoadString(STID_GEN_LIST_OF_DOMAIN_NAMES_WAIT));
   *pn_num_entries = 0;
   if (domainInIni) {
      char *pszKeys=tgGetProfileString(gszDomainPathsSec, NULL,
            gszDomainIniFile);

      if (pszKeys != NULL) {
         int count=1;
         char *pszKey=NULL;

         ppsz_keys = (char**)malloc(sizeof(char*));
         if (ppsz_keys == NULL) FailAllocMessage();
         ppsz_keys[count-1] = NULL;
         for (pszKey=pszKeys; *pszKey != '\0'; pszKey++) {
            ppsz_keys = (char**)realloc(ppsz_keys, (++count)*sizeof(char*));
            if (ppsz_keys == NULL) FailAllocMessage();
            ppsz_keys[count-1] = NULL;

            ppsz_keys[count-2] = UtilStrDup(pszKey);
            if (ppsz_keys[count-2] == NULL) FailAllocMessage();

            pszKey += strlen(pszKey);
         }
         *pn_num_entries = count;
         tgFreeProfileString(pszKeys);
      }
   } else {
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "MaxDomains")) == NULL) {
         *pn_num_entries = 1;
      } else {
         *pn_num_entries = atoi(c_ptr)+1;
      }
   }
   if (*pn_num_entries == 0) {
      RestoreStatusStringsFromBuf(status_buf, one_line_status);
      return NULL;
   }
   numTmpDomainNames = (*pn_num_entries);
   tmpDomainName = (char**)malloc((*pn_num_entries)*sizeof(char*));
   tmpDomainPath = (char**)malloc((*pn_num_entries)*sizeof(char*));
   if (tmpDomainName == NULL || tmpDomainPath == NULL) FailAllocMessage();
   memset(tmpDomainName, 0, (*pn_num_entries)*sizeof(char*));
   memset(tmpDomainPath, 0, (*pn_num_entries)*sizeof(char*));

   head_ptr = tail_ptr = NULL;
   for (i = 0; i < (*pn_num_entries); i++) {
      if (i != (*pn_num_entries)-1) {
         char *c_ptr1=NULL, *psz_path_in_ini=NULL;

         if (domainInIni) {
            if ((psz_path_in_ini=tgGetProfileString(gszDomainPathsSec,
                  ppsz_keys[i], gszDomainIniFile)) != NULL) {
               c_ptr = psz_path_in_ini;
               while (*c_ptr==' ' || *c_ptr=='\t' || *c_ptr=='\n') c_ptr++;
            }
         } else if (domainInResource) {
            sprintf(s, "DomainPath%1d", i);
            if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, s)) != NULL) {
               while (*c_ptr==' ' || *c_ptr=='\t' || *c_ptr=='\n') c_ptr++;
            }
         } else {
            sprintf(s, "Domain%1d", i);
            c_ptr = XGetDefault(mainDisplay, TOOL_NAME, s);
         }
         if (c_ptr == NULL || *c_ptr == '\0') {
            if (domainInIni) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_CANT_GEN_DOM_NAMES_INI_ENTRY),
                     ppsz_keys[i], gszDomainPathsSec, gszDomainIniFile);
            } else if (domainInResource) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_CANT_GEN_DOM_NAMES_XDEF), TOOL_NAME,
                     "DomainPath", i);
            } else {
               sprintf(gszMsgBox,
                     TgLoadString(STID_CANT_GEN_DOM_NAMES_XDEF), TOOL_NAME,
                     "Domain", i);
            }
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            for ( ; head_ptr != NULL; head_ptr = next_dsp) {
               next_dsp = head_ptr->next;
               free(head_ptr);
            }
            if (ppsz_keys != NULL) {
               for (i=0; i < *pn_num_entries; i++) {
                  tgFreeProfileString(ppsz_keys[i]);
               }
               free(ppsz_keys);
            }
            tgFreeProfileString(psz_path_in_ini);
            RestoreStatusStringsFromBuf(status_buf, one_line_status);
            return NULL;
         }
         dsp_ptr = (DspList*)malloc(sizeof(DspList));
         if (dsp_ptr == NULL) FailAllocMessage();
         memset(dsp_ptr, 0, sizeof(DspList));
         if (domainInIni) {
            if (name_only) {
               UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr),
                     ppsz_keys[i]);
            } else {
               sprintf(gszMsgBox, "%s=%s", ppsz_keys[i],
                     psz_path_in_ini==NULL ? "" : psz_path_in_ini);
               UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr),
                     gszMsgBox);
            }
            tmpDomainName[i] = UtilStrDup(ppsz_keys[i]);
            tmpDomainPath[i] = UtilStrDup(psz_path_in_ini==NULL ? "" :
                  psz_path_in_ini);
            if (tmpDomainName[i] == NULL || tmpDomainPath[i] == NULL) {
               FailAllocMessage();
            }
         } else if ((c_ptr1=strchr(c_ptr, ':')) != NULL) {
            *c_ptr1 = '\0';
            if (name_only) {
               UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), c_ptr);
            } else {
               sprintf(gszMsgBox, "%s=%s", c_ptr, &c_ptr1[1]);
               UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr),
                     gszMsgBox);
            }
            tmpDomainName[i] = UtilStrDup(c_ptr);
            tmpDomainPath[i] = UtilStrDup(&c_ptr1[1]);
            if (tmpDomainName[i] == NULL || tmpDomainPath[i] == NULL) {
               FailAllocMessage();
            }
            *c_ptr1 = ':';
         } else {
            if (name_only) {
               UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), c_ptr);
            } else {
               sprintf(gszMsgBox, "%s=", c_ptr);
               UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr),
                     gszMsgBox);
            }
            tmpDomainName[i] = UtilStrDup(c_ptr);
            tmpDomainPath[i] = UtilStrDup("");
            if (tmpDomainName[i] == NULL || tmpDomainPath[i] == NULL) {
               FailAllocMessage();
            }
         }
         /* do not translate -- program constants */
         if (strcmp(tmpDomainName[i], "Examples") == 0) {
            seen_examples = TRUE;
         }
         tgFreeProfileString(psz_path_in_ini);
      } else if (seen_examples) {
         (*pn_num_entries)--;
         numTmpDomainNames--;
         break;
      } else {
         dsp_ptr = (DspList*)malloc(sizeof(DspList));
         if (dsp_ptr == NULL) FailAllocMessage();
         memset(dsp_ptr, 0, sizeof(DspList));
         /* do not translate -- program constants */
         if (name_only) {
            UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), "Examples");
         } else {
            sprintf(gszMsgBox, "Examples=%s", TGIF_PATH);
            UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), gszMsgBox);
         }
         tmpDomainName[i] = UtilStrDup("Examples");
         tmpDomainPath[i] = UtilStrDup(TGIF_PATH);
         if (tmpDomainName[i] == NULL || tmpDomainPath[i] == NULL) {
            FailAllocMessage();
         }
      }
      if (head_ptr == NULL) {
         head_ptr = tail_ptr = dsp_ptr;
      } else {
         p1 = NULL;
         for (p = head_ptr; p != NULL; p = p->next) {
            if (LargerStr(dsp_ptr->itemstr, p->itemstr)) {
               p1 = p;
            } else {
               break;
            }
         }
         dsp_ptr->next = p;
         if (p == NULL) {  /* dsp_ptr has the largest element */
            tail_ptr->next = dsp_ptr;
            tail_ptr = dsp_ptr;
         } else if (p1 == NULL) {
            head_ptr = dsp_ptr;
         } else {
            p1->next = dsp_ptr;
         }
      }
   }
   if (ppsz_keys != NULL) {
      for (i=0; i < *pn_num_entries; i++) {
         tgFreeProfileString(ppsz_keys[i]);
      }
      free(ppsz_keys);
   }
   RestoreStatusStringsFromBuf(status_buf, one_line_status);

   return head_ptr;
}

static
int SelectDomain(SelStr)
   char *SelStr;
{
   int i, index=INVALID, num_entries=0;
   char **entries=NULL;
   DspList *dsp_ptr=NULL, *next_dsp=NULL;

   if ((dsp_ptr=DomainListing(&num_entries, TRUE)) == NULL) {
      CleanUpTmpDomainName();
      MsgBox(TgLoadString(STID_CANT_GET_LIST_OF_DOM_NAMES), TOOL_NAME, INFO_MB);
      *SelStr = '\0';
      return INVALID;
   }
   entries = MakeNameDspItemArray(num_entries, dsp_ptr);
   if ((index=ChooseAName(TgLoadString(STID_PLEASE_SELECT_A_NEW_DOMAIN),
         entries, num_entries, INVALID)) == INVALID) {
      *SelStr = '\0';
   } else {
      strcpy(SelStr, entries[index]);
   }
   for ( ; dsp_ptr != NULL; dsp_ptr = next_dsp) {
      next_dsp = dsp_ptr->next;
      free(dsp_ptr);
   }
   free(*entries);
   free(entries);

   Msg("");

   index = INVALID;
   if (*SelStr != '\0' && num_entries > 0 && tmpDomainName != NULL) {
      for (i=0; i < num_entries; i++) {
         if (tmpDomainName[i] != NULL && strcmp(tmpDomainName[i],SelStr)==0) {
            index = i;
            break;
         }
      }
   }
   CleanUpTmpDomainName();
   return index;
}

void ChangeDomain()
{
   char *c_ptr=NULL, domain_name[MAXPATHLENGTH+1], env_str[MAXPATHLENGTH+1];
   char s[MAXSTRING+1], s1[MAXSTRING+1], cap_tool_name[MAXSTRING+1];
   int index=0;
   XEvent ev;

   *domain_name = '\0';
   index = SelectDomain(domain_name);
   if (index == INVALID || *domain_name == '\0') return;

   XSync(mainDisplay, False);
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   if (domainInIni) {
      char *pszPath=tgGetProfileString(gszDomainPathsSec, domain_name,
            gszDomainIniFile);

      if (pszPath != NULL) {
         ParseSymPath(pszPath);
         tgFreeProfileString(pszPath);
      } else if (strcmp(domain_name, "Examples") == 0) {
         ParseSymPath(TGIF_PATH);
      } else {
         ParseSymPath(".");
      }
   } else if (domainInResource) {
      sprintf(s, "DomainPath%1d", index);
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,s)) != NULL) {
         char *c_ptr1;

         while (*c_ptr==' ' || *c_ptr=='\t' || *c_ptr=='\n') c_ptr++;
         if (*c_ptr != '\0' && (c_ptr1=strchr(c_ptr, ':')) != NULL) {
            c_ptr = &c_ptr1[1];
         }
         ParseSymPath(c_ptr);
      } else if (strcmp(domain_name, "Examples") == 0) {
         ParseSymPath(TGIF_PATH);
      } else {
         ParseSymPath(".");
      }
   } else {
      strcpy(cap_tool_name, TOOL_NAME);
      UtilStrUpper(cap_tool_name);
      sprintf(env_str, "%s_%s", cap_tool_name, domain_name);
      if ((c_ptr=getenv(env_str)) == NULL) {
         if (strcmp(domain_name, "Examples") == 0) {
            ParseSymPath(TGIF_PATH);
         } else {
            ParseSymPath(".");
         }
      } else {
         ParseSymPath(c_ptr);
      }
   }
   UpdateSymInfo();

   strcpy(curDomainName, domain_name);
   sprintf(s, TgLoadString(STID_CURRENT_DOMAIN_IS), curDomainName);
   sprintf(s1, TgLoadString(STID_SYMBOL_PATH_SET_TO), curDomainPath);
   TwoLineMsg(s, s1);
   RedrawTitleWindow();
}

static char *oldDomain=NULL, *oldDir=NULL;

static
DspList *SymDirListing(pn_marked_index, pn_num_entries)
   int *pn_marked_index, *pn_num_entries;
{
   int i, checking=FALSE;
   DspList *dsp_ptr, *head_ptr, *tail_ptr;

   *pn_marked_index = INVALID;
   if (oldDomain!=NULL && oldDir!=NULL && strcmp(oldDomain,curDomainName)==0) {
      checking = TRUE;
   }
   head_ptr = tail_ptr = NULL;
   for (i = 0; i < symPathNumEntries; i++) {
      dsp_ptr = (DspList*)malloc(sizeof(DspList));
      if (dsp_ptr == NULL) FailAllocMessage();
      memset(dsp_ptr, 0, sizeof(DspList));
      dsp_ptr->next = NULL;
      UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), symPath[i]);

      if (head_ptr == NULL) {
         head_ptr = tail_ptr = dsp_ptr;
      } else {
         tail_ptr->next = dsp_ptr;
         tail_ptr = dsp_ptr;
      }
      if (checking && strcmp(oldDir, symPath[i]) == 0) {
         checking = FALSE;
         *pn_marked_index = i;
      }
   }
   *pn_num_entries = symPathNumEntries;
   return head_ptr;
}

int SelectSymDir(SelStr)
   char *SelStr;
{
   int index, num_entries=0, marked_index=INVALID;
   char **entries=NULL, msg[MAXSTRING+1];
   DspList *dsp_ptr, *next_dsp;

   *SelStr = '\0';
   if ((dsp_ptr=SymDirListing(&marked_index, &num_entries)) == NULL) {
      return INVALID;
   }
   entries = MakeLongNameDspItemArray(num_entries, dsp_ptr);

   sprintf(msg, TgLoadString(STID_PLS_SEL_A_DIR_IN_GIVEN_DOMAIN),
         curDomainName);
   if ((index=ChooseAName(msg, entries, num_entries, marked_index)) !=
         INVALID) {
      strcpy(SelStr, entries[index]);
      if (oldDomain != NULL) free(oldDomain);
      if (oldDir != NULL) free(oldDir);
      oldDomain = (char*)malloc((strlen(curDomainName)+1)*sizeof(char));
      if (oldDomain == NULL) FailAllocMessage();
      oldDir = (char*)malloc((strlen(SelStr)+1)*sizeof(char));
      if (oldDir == NULL) FailAllocMessage();
      strcpy(oldDomain, curDomainName);
      strcpy(oldDir, SelStr);
   }
   for ( ; dsp_ptr != NULL; dsp_ptr = next_dsp) {
      next_dsp = dsp_ptr->next;
      free(dsp_ptr);
   }
   free(*entries);
   free(entries);

   Msg("");
   return index;
}

static
DspList *BitmapListing(ExtStr, OtherExtStr)
   char *ExtStr, *OtherExtStr;
{
   int len, path_index, count = 0, reject, ext_len;
   char path[MAXPATHLENGTH], ext_str[MAXSTRING];
   DspList *head_ptr=NULL, *tail_ptr=NULL;
   DIR *dirp=NULL;
   DIR_ENTRY *d=NULL;

   head_ptr = tail_ptr = NULL;

   sprintf(ext_str, ".%s", ExtStr);
   ext_len = strlen(ext_str);
   for (path_index=0; path_index < symPathNumEntries; path_index++) {
      strcpy(path, symPath[path_index]);
      if (strcmp(".", path) == 0) {
         if (curDirIsLocal) {
            strcpy(path, curDir);
         } else {
            strcpy(path, curLocalDir);
         }
      }
      if ((dirp=opendir(path)) == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_DIR_FOR_READING),
               path);
         Msg(gszMsgBox);
         continue;
      }
      while ((d=readdir(dirp)) != NULL) {
         len = strlen(d->d_name);
         if (!((len > ext_len &&
               UtilStrICmp(ext_str,&d->d_name[len-ext_len]) == 0) ||
               ExtensionMatch(OtherExtStr, d->d_name))) {
            continue;
         }
         if (head_ptr == NULL) {
            head_ptr = tail_ptr = (DspList*)malloc(sizeof(DspList));
            if (head_ptr == NULL) FailAllocMessage();
            memset(head_ptr, 0, sizeof(DspList));
            UtilStrCpyN(head_ptr->itemstr, sizeof(head_ptr->itemstr),
                  d->d_name);
            UtilStrCpyN(head_ptr->pathstr, sizeof(head_ptr->pathstr), path);
         } else {
            DspList *dsp_ptr=NULL, *p=NULL, *p1=NULL;

            p1 = NULL;
            reject = FALSE;
            for (p=head_ptr; p != NULL; p = p->next) {
               if (strcmp(d->d_name, p->itemstr) == 0) {
                  reject = TRUE;
                  break;
               } else if (LargerStr(d->d_name, p->itemstr)) {
                  p1 = p;
               } else {
                  break;
               }
            }
            if (reject) continue;
   
            dsp_ptr = (DspList*)malloc(sizeof(DspList));
            if (dsp_ptr == NULL) FailAllocMessage();
            memset(dsp_ptr, 0, sizeof(DspList));
            dsp_ptr->next = p;
            UtilStrCpyN(dsp_ptr->itemstr, sizeof(dsp_ptr->itemstr), d->d_name);
            UtilStrCpyN(dsp_ptr->pathstr, sizeof(dsp_ptr->pathstr), path);

            if (p == NULL) {
               /* dsp_ptr has the largest element */
               tail_ptr->next = dsp_ptr;
               tail_ptr = dsp_ptr;
            } else if (p1 == NULL) {
               head_ptr = dsp_ptr;
            } else {
               p1->next = dsp_ptr;
            }
         }
         count++;
      }
      closedir(dirp);
   }
   numSymbols = count;
   return head_ptr;
}

int SelectFromLibrary(MsgStr, ExtStr, SelStr, PathStr)
   char *MsgStr, *ExtStr, *SelStr, *PathStr;
{
   int index, num_entries=0;
   char **entries=NULL;
   char other_ext_str[MAXSTRING];
   DspList *dsp_ptr, *next_dsp;

   *other_ext_str = '\0';

   if (strcmp(ExtStr, XBM_FILE_EXT) == 0) {
      Msg(TgLoadString(STID_GEN_LIST_XBM_NAMES_WAIT));
   } else if (strcmp(ExtStr, XPM_FILE_EXT) == 0) {
      Msg(TgLoadString(STID_GEN_LIST_XPM_NAMES_WAIT));
   } else if (strcmp(ExtStr, OBJ_FILE_EXT) == 0) {
      Msg(TgLoadString(STID_GEN_LIST_OBJ_NAMES_WAIT));
      sprintf(other_ext_str, ".%s;.%s", SYM_FILE_EXT, PIN_FILE_EXT);
   } else if (strcmp(ExtStr, SYM_FILE_EXT) == 0) {
      Msg(TgLoadString(STID_GEN_LIST_SYM_NAMES_WAIT));
      sprintf(other_ext_str, ".%s", PIN_FILE_EXT);
   } else if (strcmp(ExtStr, EPSF_FILE_EXT) == 0) {
      sprintf(gszMsgBox, TgLoadString(STID_GEN_LIST_TYPE_FILE_NAMES_WAIT),
            "EPS");
      sprintf(other_ext_str, ".%s;.epsi", PS_FILE_EXT);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_GEN_LIST_TYPE_FILE_NAMES_WAIT),
            ExtStr);
      Msg(gszMsgBox);
      sprintf(other_ext_str, ".GIF");
   }
   if ((topOfSymLinkList=BitmapListing(ExtStr,other_ext_str)) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_NO_GIVEN_TYPE_FILE_FOUND), ExtStr);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      *SelStr = *PathStr = '\0';
      return INVALID;
   }
   BuildSymbolList();

   num_entries = numSymbols;
   entries = MakeNameDspItemArray(num_entries, symbolList);

   if ((index=ChooseAName(MsgStr, entries, num_entries, INVALID)) == INVALID) {
      *SelStr = *PathStr = '\0';
   } else {
      strcpy(SelStr, entries[index]);
      strcpy(PathStr, symbolList[index].pathstr);
   }

   for (dsp_ptr=topOfSymLinkList; dsp_ptr != NULL; dsp_ptr=next_dsp) {
      next_dsp = dsp_ptr->next;
      free(dsp_ptr);
   }
   free(*entries);
   free(entries);

   Msg("");
   UpdateSymInfo();
   return index;
}

void SetCurDir(FileName)
   char *FileName;
{
   char file_name[MAXPATHLENGTH+1], *psz=NULL;

   strcpy(file_name, FileName);
   UtilShrinkName(file_name);

   if (curDirIsLocal && FileIsRemote(FileName)) {
      strcpy(curLocalDir, curDir);
      if (autoHyperSpaceOnRemote) {
         inHyperSpace = TRUE;
         Msg(TgLoadCachedString(CSTID_ENTERING_HYPERSPACE));
      }
   } else if (!curDirIsLocal && !FileIsRemote(FileName)) {
      *curLocalDir = '\0';
   }
   if ((psz=UtilStrRChr(file_name, DIR_SEP)) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_NO_DIR_SEP_IN_FUNC_WORK_DIR),
            "SetDurDir()", bootDir);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      strcpy(curDir, bootDir);
      strcpy(curFileName, FileName);
   } else {
      *psz++ = '\0';
      strcpy(curFileName, psz);
      strcpy(curDir, file_name);
   }
   curDirIsLocal = (!DirIsRemote(curDir));
   RedrawDummyWindow1();
}

void SetCurSymDir(FileName)
   char *FileName;
{
   char file_name[MAXPATHLENGTH+1], *psz=NULL;

   strcpy(file_name, FileName);
   UtilShrinkName(file_name);

   if ((psz=UtilStrRChr(file_name, DIR_SEP)) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_NO_DIR_SEP_IN_FUNC_SYM_DIR),
            "SetDurSymDir()", bootDir);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      strcpy(curSymDir, bootDir);
      strcpy(curFileName, FileName);
   } else {
      *psz++ = '\0';
      strcpy(curFileName, psz);
      strcpy(curSymDir, file_name);
   }
}

void SetCurImportDir(FileName)
   char *FileName;
{
   char file_name[MAXPATHLENGTH+1], *psz=NULL;

   strcpy(file_name, FileName);
   UtilShrinkName(file_name);

   if ((psz=UtilStrRChr(file_name, DIR_SEP)) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_NO_DIR_SEP_IN_FUNC_IMPORT_DIR),
            "SetDurImportir()", bootDir);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      strcpy(curImportDir, bootDir);
   } else {
      *psz = '\0';
      strcpy(curImportDir, file_name);
   }
}

/* ---------------------- Additional Domain Functions ---------------------- */

static
DspList *FreeDspLinkedList(dsp_ptr)
   DspList *dsp_ptr;
{
   DspList *next_dsp=NULL;

   for ( ; dsp_ptr != NULL; dsp_ptr=next_dsp) {
      next_dsp = dsp_ptr->next;
      free(dsp_ptr);
   }
   return NULL;
}

void CopyDomainInfoToIni()
{
   int i=0, num_entries=0;
   DspList *dsp_ptr=NULL;

   tgWriteProfileString(gszDomainPathsSec, NULL, NULL, gszDomainIniFile);
   if ((dsp_ptr=DomainListing(&num_entries, TRUE)) == NULL) {
      CleanUpTmpDomainName();
      return;
   }
   dsp_ptr = FreeDspLinkedList(dsp_ptr);

   for (i=0; i < num_entries; i++) {
      /* do not translate -- program constants */
      if (strcmp(tmpDomainName[i], "Examples") != 0) {
         tgWriteProfileString(gszDomainPathsSec, tmpDomainName[i],
               tmpDomainPath[i], gszDomainIniFile);
      }
   }
   tgWriteProfileString(NULL, NULL, NULL, gszDomainIniFile);
   CleanUpTmpDomainName();
}

static
DspList *DomainListToDomainArray(dsp_ptr, num_entries, set_directory)
   DspList *dsp_ptr;
   int num_entries, set_directory;
{
   int i=0;
   DspList *dsp_ptr1=NULL, *new_dsp=NULL, *pdl=NULL;

   new_dsp = (DspList*)malloc(num_entries*sizeof(DspList));
   if (new_dsp == NULL) FailAllocMessage();
   memset(new_dsp, 0, num_entries*sizeof(DspList));
   for (i=0, pdl=new_dsp, dsp_ptr1=dsp_ptr; i < num_entries;
         i++, pdl++, dsp_ptr1=dsp_ptr1->next) {
      UtilStrCpyN(pdl->itemstr, sizeof(pdl->itemstr), dsp_ptr1->itemstr);
      /* use the directory field for inherited */
      if (set_directory) pdl->directory = TRUE;
      pdl->next = (&pdl[1]);
   }
   new_dsp[num_entries-1].next = NULL;
   FreeDspLinkedList(dsp_ptr);

   return new_dsp;
}

static
int EditDomainPathsAfterLoop(pp_dsp_ptr, ppsz_entries, pn_num_entries,
      pn_marked_index, pp_check_array, cur_buf, btn_id, selected_index, p_void)
   DspList **pp_dsp_ptr;
   char ***ppsz_entries, *cur_buf;
   int *pn_num_entries, *pn_marked_index, btn_id, selected_index;
   struct CheckArrayRec **pp_check_array;
   void *p_void;
   /*
    * Returns FALSE if the content of the dialogbox is acceptable and
    *       the dialogbox will be closed.
    * Returns TRUE to tell the dialogbox to continue to loop.  In this case,
    *       this function should call MsgBox() to let the user know why
    *       the dialogbox is not closed.
    */
{
   int i=0, num_entries=(*pn_num_entries);

   if (btn_id == BUTTON_CANCEL) {
      return FALSE;
   }
   for (i=0; i < num_entries; i++) {
      char *psz_key=(*ppsz_entries)[i], *psz_value=NULL;
      char *psz=strchr(psz_key, '=');

      *psz = '\0';
      psz_value = UtilStrDup(&psz[1]);
      if (psz_value == NULL) FailAllocMessage();
      *psz = '=';
      UtilTrimBlanks(psz_value);
      if (*psz_value == '\0') {
         int len=strlen(psz_key);

         if (len > 0 && psz_key[len-1] == '=') psz_key[len-1] = '\0';
         sprintf(gszMsgBox, TgLoadString(STID_EMPTY_PATH_DISALLOW_FOR_DOM),
               psz_key);
         if (len > 0 && psz_key[len-1] == '\0') psz_key[len-1] = '=';
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         UtilFree(psz_value);
         return TRUE;
      }
      UtilFree(psz_value);
   }
   /* everything is fine */
   return FALSE;
}

static
int DoEditDomainPaths(psz_title, dsp_ptr, entries, num_entries,
      pf_after_loop)
   char *psz_title, **entries;
   DspList *dsp_ptr;
   int num_entries;
   AfterLoopFunc *pf_after_loop;
{
   char win_name[128];

   sprintf(win_name, TgLoadString(STID_TOOL_EDIT_DOM_PATHS), TOOL_NAME);
   ResetNamesInfo();
   NamesSetTitle(psz_title);
   NamesAddButton(TgLoadCachedString(CSTID_OK), BUTTON_OK);
   NamesAddButton(TgLoadCachedString(CSTID_CANCEL), BUTTON_CANCEL);
   /* ignore double-click and <CR> */
   NamesSetDefaultBtnId(BUTTON_OK, INVALID);
   NamesSetStyle(NAMES_EDIT_ATTR, NAMES_LOOP_MANY);
   NamesSetCallback(NULL, pf_after_loop, NULL);
   NamesSetEntries(dsp_ptr, entries, num_entries, NULL, TRUE, INVALID, 0);
   return (Names(win_name, NULL, NULL, 0, NULL)==BUTTON_OK);
}

void EditDomainPaths()
{
   int num_entries=0;
   DspList *dsp_ptr=NULL;
   char *pszKeys=NULL, **entries=NULL;

   if ((pszKeys=tgGetProfileString(gszDomainPathsSec, NULL,
         gszDomainIniFile)) == NULL) {
      CopyDomainInfoToIni();
   } else {
      tgFreeProfileString(pszKeys);
   }
   if ((dsp_ptr=DomainListing(&num_entries, FALSE)) == NULL) {
      MsgBox(TgLoadString(STID_CANT_GET_LIST_OF_DOM_NAMES), TOOL_NAME, INFO_MB);
      return;
   }
   CleanUpTmpDomainName();

   dsp_ptr = DomainListToDomainArray(dsp_ptr, num_entries, TRUE);

   ignoreDirectoryFlag = TRUE;
   entries = MakeNameDspItemArray(num_entries, dsp_ptr);
   ignoreDirectoryFlag = FALSE;

   if (DoEditDomainPaths(TgLoadString(STID_EDIT_DOMAIN_PATHS_DOTS), dsp_ptr,
         entries, num_entries,
         (AfterLoopFunc*)EditDomainPathsAfterLoop)) {
      int i=0;

      tgWriteProfileString(gszDomainPathsSec, NULL, NULL, gszDomainIniFile);
      for (i=0; i < num_entries; i++) {
         char *psz=strchr(entries[i], '=');

         *psz++ = '\0';
         tgWriteProfileString(gszDomainPathsSec, entries[i], psz,
               gszDomainIniFile);
      }
      tgWriteProfileString(NULL, NULL, NULL, gszDomainIniFile);
   }
   free(dsp_ptr);
   free(*entries);
   free(entries);
}

static
int DoSelectDefaultDomain(psz_title, dsp_ptr, entries, num_entries,
      selected_str, selected_str_sz)
   char *psz_title, **entries, *selected_str;
   DspList *dsp_ptr;
   int num_entries, selected_str_sz;
{
   char win_name[128];

   sprintf(win_name, TgLoadString(STID_TOOL_SEL_DEFAULT_DOMAIN), TOOL_NAME);
   ResetNamesInfo();
   NamesSetTitle(psz_title);
   NamesAddButton(TgLoadCachedString(CSTID_OK), BUTTON_OK);
   NamesAddButton(TgLoadCachedString(CSTID_CANCEL), BUTTON_CANCEL);
   /* ignore double-click and <CR> */
   NamesSetDefaultBtnId(BUTTON_OK, BUTTON_OK);
   NamesSetStyle(NAMES_SIMPLE_SELECT_NAME, NAMES_LOOP_ONCE);
   NamesSetEntries(dsp_ptr, entries, num_entries, NULL, TRUE, INVALID, 0);
   return (Names(win_name, NULL, selected_str, selected_str_sz,
         NULL)==BUTTON_OK);
}

void SelectDefaultDomain()
{
   int num_entries=0;
   DspList *dsp_ptr=NULL;
   char *pszKeys=NULL, **entries=NULL, selected_domain[MAXSTRING];

   if ((pszKeys=tgGetProfileString(gszDomainPathsSec, NULL,
         gszDomainIniFile)) == NULL) {
      CopyDomainInfoToIni();
   } else {
      tgFreeProfileString(pszKeys);
   }
   if ((dsp_ptr=DomainListing(&num_entries, TRUE)) == NULL) {
      MsgBox(TgLoadString(STID_CANT_GET_LIST_OF_DOM_NAMES), TOOL_NAME, INFO_MB);
      return;
   }
   CleanUpTmpDomainName();

   dsp_ptr = DomainListToDomainArray(dsp_ptr, num_entries, FALSE);

   ignoreDirectoryFlag = TRUE;
   entries = MakeNameDspItemArray(num_entries, dsp_ptr);
   ignoreDirectoryFlag = FALSE;

   *selected_domain = '\0';
   if (!DoSelectDefaultDomain(TgLoadString(STID_SEL_DEFAULT_DOMAIN_DOTS),
         dsp_ptr, entries, num_entries, selected_domain,
         sizeof(selected_domain))) {
      *selected_domain = '\0';
   }
   free(dsp_ptr);
   free(*entries);
   free(entries);
   if (*selected_domain != '\0') {
      tgWriteProfileString(gszDefaultDomainSec, NULL, NULL, gszDomainIniFile);
      tgWriteProfileString(gszDefaultDomainSec, selected_domain, "",
            gszDomainIniFile);
      tgWriteProfileString(NULL, NULL, NULL, gszDomainIniFile);
   }
}

void AddADomain()
{
   int num_entries=0;
   DspList *dsp_ptr=NULL;
   char *pszKeys=NULL, **entries=NULL, spec[MAXSTRING];

   if ((pszKeys=tgGetProfileString(gszDomainPathsSec, NULL,
         gszDomainIniFile)) == NULL) {
      CopyDomainInfoToIni();
   } else {
      tgFreeProfileString(pszKeys);
   }
   if ((dsp_ptr=DomainListing(&num_entries, TRUE)) == NULL) {
      MsgBox(TgLoadString(STID_CANT_GET_LIST_OF_DOM_NAMES), TOOL_NAME, INFO_MB);
      return;
   }
   CleanUpTmpDomainName();

   dsp_ptr = DomainListToDomainArray(dsp_ptr, num_entries, FALSE);

   ignoreDirectoryFlag = TRUE;
   entries = MakeNameDspItemArray(num_entries, dsp_ptr);
   ignoreDirectoryFlag = FALSE;

   *spec = '\0';
   if (Dialog(TgLoadString(STID_PLS_SPEC_A_NEW_DOMAIN_NAME), NULL, spec) !=
         INVALID) {
      UtilTrimBlanks(spec);
      if (*spec != '\0') {
         int i=0, dup_found=FALSE;

         for (i=0; i < num_entries; i++) {
            if (UtilStrICmp(dsp_ptr[i].itemstr, spec) == 0) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_GIVEN_DOMAIN_ALREADY_EXISTS), spec);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               dup_found = TRUE;
               break;
            }
         }
         if (!dup_found) {
            tgWriteProfileString(gszDomainPathsSec, spec, ".",
                  gszDomainIniFile);
            tgWriteProfileString(NULL, NULL, NULL, gszDomainIniFile);
            sprintf(gszMsgBox, TgLoadString(STID_GIVEN_DOM_ADDED_PATH_SET_TO),
                  spec, ".");
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
      }
   }
   free(dsp_ptr);
   free(*entries);
   free(entries);
}

void DeleteADomain()
{
   int num_entries=0;
   DspList *dsp_ptr=NULL;
   char *pszKeys=NULL, **entries=NULL, selected_domain[MAXSTRING];

   if ((pszKeys=tgGetProfileString(gszDomainPathsSec, NULL,
         gszDomainIniFile)) == NULL) {
      CopyDomainInfoToIni();
   } else {
      tgFreeProfileString(pszKeys);
   }
   if ((dsp_ptr=DomainListing(&num_entries, TRUE)) == NULL) {
      MsgBox(TgLoadString(STID_CANT_GET_LIST_OF_DOM_NAMES), TOOL_NAME, INFO_MB);
      return;
   }
   CleanUpTmpDomainName();

   dsp_ptr = DomainListToDomainArray(dsp_ptr, num_entries, FALSE);

   ignoreDirectoryFlag = TRUE;
   entries = MakeNameDspItemArray(num_entries, dsp_ptr);
   ignoreDirectoryFlag = FALSE;

   *selected_domain = '\0';
   if (!DoSelectDefaultDomain(TgLoadString(STID_SEL_DOMAIN_TO_DELETE_DOTS),
         dsp_ptr, entries, num_entries, selected_domain,
         sizeof(selected_domain))) {
      *selected_domain = '\0';
   }
   free(dsp_ptr);
   free(*entries);
   free(entries);
   if (*selected_domain != '\0') {
      sprintf(gszMsgBox, TgLoadString(STID_OK_TO_DELETE_NAMED_DOMAIN_YNC),
            selected_domain);
      if (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB) == MB_ID_YES) {
         tgWriteProfileString(gszDomainPathsSec, selected_domain, NULL,
               gszDomainIniFile);
         tgWriteProfileString(NULL, NULL, NULL, gszDomainIniFile);
      }
   }
}

void ReloadDomainInfoFromX()
{
   if (MsgBox(TgLoadString(STID_OK_TO_RELOAD_DOM_INFO_FROM_X), TOOL_NAME,
         YNC_MB) == MB_ID_YES) {
      tgWriteProfileString(gszDefaultDomainSec, NULL, NULL, gszDomainIniFile);
      tgWriteProfileString(gszDomainPathsSec, NULL, NULL, gszDomainIniFile);
      tgWriteProfileString(NULL, NULL, NULL, gszDomainIniFile);
      InitDomain();
      CopyDomainInfoToIni();
      tgWriteProfileString(gszDefaultDomainSec, curDomainName, "",
            gszDomainIniFile);
      tgWriteProfileString(NULL, NULL, NULL, gszDomainIniFile);
   }
}

int RefreshDomainMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* EditDomainPaths */
   ok &= TgEnableMenuItemById(menu, CMDID_EDITDOMAINPATHS, domainInIni);
   /* SelectDefaultDomain */
   ok &= TgEnableMenuItemById(menu, CMDID_SELECTDEFAULTDOMAIN, domainInIni);
   /* AddADomain */
   ok &= TgEnableMenuItemById(menu, CMDID_ADDADOMAIN, domainInIni);
   /* DeleteADomain */
   ok &= TgEnableMenuItemById(menu, CMDID_DELETEADOMAIN, domainInIni);
   /* ReloadDomainInfoFromX */
   ok &= TgEnableMenuItemById(menu, CMDID_RELOADDOMAININFOFROMX, domainInIni);

   return ok;
}

TgMenu *CreateDomainMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      menu->track_menubar = TRUE;
      if (!RefreshDomainMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc = ((RefreshMenuFunc*)RefreshDomainMenu);
   }
   return menu;
}
