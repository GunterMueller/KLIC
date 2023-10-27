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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/msg.c,v 1.49 2011/05/16 16:21:58 william Exp $
 */

#define _INCLUDE_FROM_MSG_C_

#include "tgifdefs.h"
#include "patchlvl.h"

#include "auxtext.e"
#include "button.e"
#include "chat.e"
#include "cutpaste.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "inmethod.e"
#include "mainloop.e"
#include "markup.e"
#include "menu.e"
#include "msg.e"
#include "navigate.e"
#include "pattern.e"
#include "ps.e"
#include "raster.e"
#include "rect.e"
#include "remote.e"
#include "rm_intrf.e"
#include "setup.e"
#include "strtbl.e"
#include "tangram2.e"
#include "text.e"
#include "util.e"
#include "version.e"
#include "wb.e"
#include "xbitmap.e"

#include "xbm/redraw.xbm"

char	scanFileFullPath[MAXPATHLENGTH+1];
char	scanFileName[MAXPATHLENGTH+1];
int	scanLineNum=0;

char	progName[MAXPATHLENGTH+1];
char	fatalMsgBuf1[MAXSTRING<<1];
char	fatalMsgBuf2[MAXSTRING<<1];

int	cmdLineParentWindowID=0;
int	cmdLineTgrm2=FALSE;
int	cmdLineWb2=FALSE;

struct MsgRec {
   char			* s;
   struct MsgRec	* next, * prev;
};

static struct MsgRec	* topMsg = NULL, * botMsg = NULL;
static struct MsgRec	* mostRecentTopMsgPtr = NULL;
static int		msgCount = 0;
static int		topMsgNumber = 0, mostRecentTopMsgNumber = INVALID;
static int		firstCharPos = 0;

static int		reverseMouseStatusButtons=FALSE;
static int		gnMsgRows=2;

int ShowOrPrintMsg(buf, use_msgbox)
   char *buf;
   int use_msgbox;
{
   if (PRTGIF && !cmdLineOpenDisplay) {
      fprintf(stderr, "%s\n", buf);
   } else if (use_msgbox) {
      MsgBox(buf, TOOL_NAME, INFO_MB);
   } else {
      Msg(buf);
   }
   return FALSE;
}

int FailAllocMessage()
{
   fprintf(stderr, "%s\n", TgLoadString(STID_OUT_OF_VMEM_CANNOT_MALLOC));
   fflush(stderr);
   return FALSE;
}

int FailAllocBitmapMessage(w, h)
   int w, h;
{
   fprintf(stderr, TgLoadString(STID_CANT_ALLOC_BITMAP_OF_SIZE), w, h);
   fprintf(stderr, "\n");
   fflush(stderr);
   return FALSE;
}

int FailAllocPixmapMessage(w, h)
   int w, h;
{
   fprintf(stderr, TgLoadString(STID_CANNOT_ALLOC_PIXMAP_OF_SIZE), w, h);
   fprintf(stderr, "\n");
   fflush(stderr);
   return FALSE;
}

int FailToWriteFileMessage(file_name)
   char *file_name;
{
   if (PRTGIF) {
      fprintf(stderr, TgLoadString(STID_FAIL_TO_WRITE_TO_FILE), file_name);
      fprintf(stderr, "\n");
   } else {
      char msg[MAXPATHLENGTH<<1];

      sprintf(msg, TgLoadString(STID_FAIL_TO_WRITE_TO_FILE), file_name);
      MsgBox(msg, TOOL_NAME, INFO_MB);
   }
   return FALSE;
}

int FailToOpenMessage(file_name, rwa, extra_msg)
   char *file_name, *rwa, *extra_msg;
   /* rwa is either "r", "w", or "a" */
{
   char msg[MAXPATHLENGTH<<1];
   int msgid=0;

   switch (*rwa) {
   case 'r': msgid = STID_CANNOT_OPEN_FILE_FOR_READING; break;
   case 'w': msgid = STID_CANNOT_OPEN_FILE_FOR_WRITING; break;
   case 'a': msgid = STID_CANNOT_OPEN_FILE_FOR_APPEND; break;
   default: msgid = INVALID; break;
   }
   if (extra_msg == NULL) {
      sprintf(msg, TgLoadString(msgid), file_name);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", msg);
      } else {
         MsgBox(msg, TOOL_NAME, INFO_MB);
      }
   } else {
      sprintf(msg, TgLoadString(msgid), file_name);
      strcat(msg, "  ");
      strcat(msg, extra_msg);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", msg);
      } else {
         MsgBox(msg, TOOL_NAME, INFO_MB);
      }
   }
   return FALSE;
}

int FailToCreateWindowMessage(where, win_name, do_exit)
   char *where, *win_name;
   int do_exit;
{
   char msg[MAXSTRING<<1];

   if (win_name == NULL) {
      strcpy(msg, TgLoadString(STID_FAIL_TO_CREATE_WINDOW));
   } else {
      sprintf(msg, TgLoadString(STID_FAIL_TO_CREATE_NAMED_WINDOW), win_name);
   }
   if (do_exit) {
      Error(where, msg);
   }
   fprintf(stderr, "%s\n", msg);

   return FALSE;
}

int FatalUnexpectedError(msg1, msg2)
   char *msg1, *msg2;
{
   char buf[2048];

   if (msg2 == NULL) {
      fprintf(stderr, "%s\n", msg1);
   } else {
      fprintf(stderr, "%s\n%s\n", msg1, msg2);
   }
   fprintf(stderr, TgLoadString(STID_SAFEST_SAVE_N_EXIT_REPRODUCE),
         authorEmailString);
   fprintf(stderr, "\n");
   fflush(stderr);

   sprintf(buf, TgLoadString(STID_FATAL_UNEXPECTED_ERROR),
         msg1, (msg2==NULL ? "" : "\n"), (msg2==NULL ? "" : msg2),
         authorEmailString);
   MsgBox(buf, TOOL_NAME, STOP_MB);
   XFlush(mainDisplay);
   XSync(mainDisplay, False);
   return FALSE;
}

int TgAssert(must_be_true, msg1, msg2)
   int must_be_true;
   char *msg1, *msg2;
{
   if (must_be_true) return TRUE;

   FatalUnexpectedError(msg1, msg2);
   return FALSE;
}

int BoldMsgCharBytes(pszStr)
   char *pszStr;
{
#ifdef ENABLE_NLS
   if (boldMsgFontDoubleByte) {
      if ((*pszStr) & 0x80) {
         return 2;
      }
   }
#endif /* ENABLE_NLS */
   return 1;
}

char *BoldMsgNextChar(pszStr)
   char *pszStr;
{
#ifdef ENABLE_NLS
   if (boldMsgFontDoubleByte) {
      if ((*pszStr) & 0x80) {
         return (&pszStr[2]);
      }
   }
#endif /* ENABLE_NLS */
   return (&pszStr[1]);
}

char *BoldMsgLastChar(pszStr)
   char *pszStr;
{
   if (*pszStr == '\0') return NULL;

#ifdef ENABLE_NLS
   if (boldMsgFontDoubleByte) {
      int nLen=strlen(pszStr), nIndex=0;
      char *pszLast=NULL;

      while (nIndex < nLen) {
         int nSubStrIndex=0, nDoubleByte=FALSE;
         char *psz=NULL, *pszStart=NULL;

         pszStart = &pszStr[nIndex];
         nDoubleByte = ((*pszStart) & 0x80);
         if (nDoubleByte) {
            for (psz=pszStart, nSubStrIndex=0;
                  nIndex+nSubStrIndex < nLen && ((*psz)&0x80) == nDoubleByte;
                  psz=(&psz[2]), nSubStrIndex+=2) {
               pszLast = psz;
            }
         } else {
            for (psz=pszStart, nSubStrIndex=0;
                  nIndex+nSubStrIndex < nLen && ((*psz)&0x80) == 0;
                  psz++, nSubStrIndex++) {
               pszLast = psz;
            }
         }
         nIndex += nSubStrIndex;
      }
      return pszLast;
   }
#endif /* ENABLE_NLS */
   return (&pszStr[strlen(pszStr)-1]);
}

char *BoldMsgStrChr(pszStr, c)
   char *pszStr;
   int c;
   /* c can only be a single-byte char */
{
#ifdef ENABLE_NLS
   if (boldMsgFontDoubleByte) {
      int nLen=strlen(pszStr), nIndex=0;

      while (nIndex < nLen) {
         int nSubStrIndex=0, nDoubleByte=FALSE;
         char *psz=NULL, *pszStart=NULL;

         pszStart = &pszStr[nIndex];
         nDoubleByte = ((*pszStart) & 0x80);
         if (nDoubleByte) {
            for (psz=pszStart, nSubStrIndex=0;
                  nIndex+nSubStrIndex < nLen && ((*psz)&0x80) == nDoubleByte;
                  psz=(&psz[2]), nSubStrIndex+=2) {
            }
         } else {
            for (psz=pszStart, nSubStrIndex=0;
                  nIndex+nSubStrIndex < nLen && ((*psz)&0x80) == 0;
                  psz++, nSubStrIndex++) {
               if ((int)(*psz) == c) {
                  return psz;
               }
            }
         }
         nIndex += nSubStrIndex;
      }
      return NULL;
   }
#endif /* ENABLE_NLS */
   return strchr(pszStr, c);
}

int MenuTextWidth(font_ptr, psz_str, len)
   XFontStruct *font_ptr;
   char *psz_str;
   int len;
{
#ifdef ENABLE_NLS
   if (menuFontSet != NULL) {
      return XmbTextEscapement(menuFontSet, psz_str, len);
   }
#endif /* ENABLE_NLS */
   return XTextWidth(font_ptr, psz_str, len);
}

int MsgTextWidth(font_ptr, psz_str, len)
   XFontStruct *font_ptr;
   char *psz_str;
   int len;
{
#ifdef ENABLE_NLS
   if (msgFontSet != NULL) {
      return XmbTextEscapement(msgFontSet, psz_str, len);
   }
#endif /* ENABLE_NLS */
   return XTextWidth(font_ptr, psz_str, len);
}

int BoldMsgTextWidth(font_ptr, psz_str, len)
   XFontStruct *font_ptr;
   char *psz_str;
   int len;
{
#ifdef ENABLE_NLS
   if (boldMsgFontSet != NULL) {
      return XmbTextEscapement(boldMsgFontSet, psz_str, len);
   }
#endif /* ENABLE_NLS */
   return XTextWidth(font_ptr, psz_str, len);
}

int ItalicMsgTextWidth(font_ptr, psz_str, len)
   XFontStruct *font_ptr;
   char *psz_str;
   int len;
{
#ifdef ENABLE_NLS
   if (italicMsgFontSet != NULL) {
      return XmbTextEscapement(italicMsgFontSet, psz_str, len);
   }
#endif /* ENABLE_NLS */
   return XTextWidth(font_ptr, psz_str, len);
}

int BoldItalicMsgTextWidth(font_ptr, psz_str, len)
   XFontStruct *font_ptr;
   char *psz_str;
   int len;
{
#ifdef ENABLE_NLS
   if (boldItalicMsgFontSet != NULL) {
      return XmbTextEscapement(boldItalicMsgFontSet, psz_str, len);
   }
#endif /* ENABLE_NLS */
   return XTextWidth(font_ptr, psz_str, len);
}

void DrawMenuString(dpy, win, gc, x, baseline_y, menu_str, len)
   Display *dpy;
   Window win;
   GC gc;
   int x, baseline_y, len;
   char *menu_str;
{
#ifdef ENABLE_NLS
   if (menuFontSet != NULL) {
      XmbDrawString(dpy, win, menuFontSet, gc, x, baseline_y, menu_str, len);
      return;
   }
#endif /* ENABLE_NLS */
   XDrawString(dpy, win, gc, x, baseline_y, menu_str, len);
}

void DrawMsgString(dpy, win, gc, x, baseline_y, msg_str, len)
   Display *dpy;
   Window win;
   GC gc;
   int x, baseline_y, len;
   char *msg_str;
{
#ifdef ENABLE_NLS
   if (msgFontSet != NULL) {
      XmbDrawString(dpy, win, msgFontSet, gc, x, baseline_y, msg_str, len);
      return;
   }
#endif /* ENABLE_NLS */
   XDrawString(dpy, win, gc, x, baseline_y, msg_str, len);
}

void DrawBoldMsgString(dpy, win, gc, x, baseline_y, msg_str, len)
   Display *dpy;
   Window win;
   GC gc;
   int x, baseline_y, len;
   char *msg_str;
{
#ifdef ENABLE_NLS
   if (boldMsgFontSet != NULL) {
      XmbDrawString(dpy, win, boldMsgFontSet, gc, x, baseline_y, msg_str, len);
      return;
   }
#endif /* ENABLE_NLS */
   XDrawString(dpy, win, gc, x, baseline_y, msg_str, len);
}

void DrawItalicMsgString(dpy, win, gc, x, baseline_y, msg_str, len)
   Display *dpy;
   Window win;
   GC gc;
   int x, baseline_y, len;
   char *msg_str;
{
#ifdef ENABLE_NLS
   if (italicMsgFontSet != NULL) {
      XmbDrawString(dpy, win, italicMsgFontSet, gc, x, baseline_y, msg_str,
            len);
      return;
   }
#endif /* ENABLE_NLS */
   XDrawString(dpy, win, gc, x, baseline_y, msg_str, len);
}

void DrawBoldItalicMsgString(dpy, win, gc, x, baseline_y, msg_str, len)
   Display *dpy;
   Window win;
   GC gc;
   int x, baseline_y, len;
   char *msg_str;
{
#ifdef ENABLE_NLS
   if (boldItalicMsgFontSet != NULL) {
      XmbDrawString(dpy, win, boldItalicMsgFontSet, gc, x, baseline_y, msg_str,
            len);
      return;
   }
#endif /* ENABLE_NLS */
   XDrawString(dpy, win, gc, x, baseline_y, msg_str, len);
}

static
void AddMsg(Msg)
   char *Msg;
{
   char *s;
   struct MsgRec *msg_ptr;

   firstCharPos = 0;
   if (*Msg == '\0') { topMsgNumber = msgCount; return; }

   s = (char*)malloc((strlen(Msg)+1)*sizeof(char));
   if (s == NULL) FailAllocMessage();
   msg_ptr = (struct MsgRec *)malloc(sizeof(struct MsgRec));
   if (msg_ptr == NULL) FailAllocMessage();

   strcpy(s, Msg);
   msg_ptr->s = s;

   ++msgCount;
   if (msgCount > topMsgNumber+gnMsgRows) topMsgNumber = msgCount-gnMsgRows;

   msg_ptr->prev = botMsg;
   msg_ptr->next = NULL;

   if (botMsg == NULL) {
      topMsg = msg_ptr;
   } else {
      botMsg->next = msg_ptr;
   }
   botMsg = msg_ptr;
}

void CleanUpMsg()
{
   register struct MsgRec *msg_ptr, *prev_msg;

   for (msg_ptr=botMsg; msg_ptr != NULL; msg_ptr=prev_msg) {
      prev_msg = msg_ptr->prev;
      free(msg_ptr->s);
      free(msg_ptr);
   }
   topMsg = botMsg = mostRecentTopMsgPtr = NULL;
   msgCount = topMsgNumber = firstCharPos = 0;
   mostRecentTopMsgNumber = INVALID;
}

static
struct MsgRec *FindMsg(Number)
   int Number;
{
   register int i;
   register struct MsgRec *ptr;

   if (Number >= msgCount) {
      return botMsg;
   } else if (Number < 0) {
      return topMsg;
   } else if (Number > (int)(msgCount/2)) {
      for (i=msgCount-1, ptr=botMsg; i != Number; i--, ptr=ptr->prev) {
      }
   } else {
      for (i=0, ptr=topMsg; i != Number; i++, ptr=ptr->next) {
      }
   }
   return ptr;
}

void RedrawMsg(erase_background)
   int erase_background;
{
   int i, x, y, len;
   XEvent ev;
   struct MsgRec *msg_ptr;

   if (msgWindow == None) return;

   if (erase_background) {
      XClearWindow(mainDisplay, msgWindow);
      XSync(mainDisplay, False);
   }
   while (XCheckWindowEvent(mainDisplay, msgWindow, ExposureMask, &ev)) ;

   if (topMsgNumber == msgCount) {
      if (threeDLook) {
         struct BBRec bbox;

         SetBBRec(&bbox, 0, 0, msgWindowW, msgWindowH);
         TgDrawThreeDButton(mainDisplay, msgWindow, textMenuGC, &bbox,
               TGBS_LOWRED, 2, TRUE);
      }
      return;
   }
   x = 2 + windowPadding;
   y = 2 + ((msgFontSet==NULL && msgFontPtr==NULL) ? defaultFontAsc :
         msgFontAsc) + windowPadding;

   mostRecentTopMsgPtr = msg_ptr = (topMsgNumber == mostRecentTopMsgNumber) ?
         mostRecentTopMsgPtr : FindMsg(topMsgNumber);
   mostRecentTopMsgNumber = topMsgNumber;

   if (msgFontPtr != NULL) {
      XSetFont(mainDisplay, defaultGC, msgFontPtr->fid);
   }
   for (i=topMsgNumber; i < min(msgCount,topMsgNumber+gnMsgRows); i++) {
      int w=0;

      len = strlen(msg_ptr->s);
      if (msgFontSet == NULL && msgFontPtr == NULL) {
         w = defaultFontWidth*strlen(msg_ptr->s);
      } else {
         w = MsgTextWidth(msgFontPtr, msg_ptr->s, len);
      }
      if (w > firstCharPos) {
         DrawMsgString(mainDisplay, msgWindow, defaultGC, x-firstCharPos, y,
               msg_ptr->s, len);
      }
      msg_ptr = msg_ptr->next;
      y += ((msgFontSet==NULL && msgFontPtr==NULL) ? defaultFontHeight :
            msgFontHeight);
   }
   if (msgFontSet != NULL || msgFontPtr != NULL) {
      XSetFont(mainDisplay, defaultGC, defaultFontPtr->fid);
   }
   if (erase_background && threeDLook) {
      struct BBRec bbox;

      SetBBRec(&bbox, 0, 0, msgWindowW, msgWindowH);
      TgDrawThreeDButton(mainDisplay, msgWindow, textMenuGC, &bbox,
            TGBS_LOWRED, 2, TRUE);
   }
   XSync(mainDisplay, False);
}

void Msg(Message)
   char *Message;
{
   AddMsg(Message);
   RedrawMsg(TRUE);
}

void TwoLineMsg(Msg1, Msg2)
   char *Msg1, *Msg2;
{
   AddMsg(Msg1);
   AddMsg(Msg2);
   RedrawMsg(TRUE);
}

void PrintMsgBuffer()
{
   char file_name[MAXPATHLENGTH], *rest, msg[MAXSTRING];
   FILE *fp;
   struct MsgRec *msg_ptr;

   *file_name = '\0';
   Dialog(TgLoadString(STID_ENTER_FNAME_TO_WRITE_MSG_BUF),
         TgLoadString(STID_STDOUT_STDERR_ESC_CANCEL), file_name);
   UtilTrimBlanks(file_name);
   if (*file_name == '\0') return;

   /* do not translate -- program constants */
   if (strcmp(file_name, "stdout") == 0) {
      for (msg_ptr=topMsg; msg_ptr != NULL; msg_ptr=msg_ptr->next) {
         printf("%s\n", msg_ptr->s);
      }
   } else if (strcmp(file_name, "stderr") == 0) {
      for (msg_ptr=topMsg; msg_ptr != NULL; msg_ptr=msg_ptr->next) {
         fprintf(stderr, "%s\n", msg_ptr->s);
      }
   } else {
      int short_name;

      if (!OkayToCreateFile(file_name)) return;

      if ((short_name=IsPrefix(bootDir, file_name, &rest))) ++rest;
      if ((fp=fopen(file_name, "w")) == NULL) {
         sprintf(msg, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
               (short_name ? rest : file_name));
         MsgBox(msg, TOOL_NAME, INFO_MB);
         return;
      }
      writeFileFailed = FALSE;
      for (msg_ptr=topMsg; msg_ptr != NULL; msg_ptr=msg_ptr->next) {
         if (fprintf(fp, "%s\n", msg_ptr->s) == EOF) {
            writeFileFailed = TRUE;
         }
      }
      fclose(fp);

      if (writeFileFailed) {
         writeFileFailed = FALSE;
         FailToWriteFileMessage(file_name);
      } else {
         sprintf(msg, TgLoadString(STID_MSG_BUF_SAVED_INTO_GIVEN),
               (short_name ? rest : file_name));
         MsgBox(msg, TOOL_NAME, INFO_MB);
      }
   }
}

#include "xbm/btn1.xbm"

static int	oneLineStatus=FALSE;
static char	btnStatusStr[MAX_STATUS_BTNS][MAXSTRING+1];
static char	oneLineStatusStr[MAXSTRING+1];

static
void RedrawStatusStrings()
{
   register int i;
   struct BBRec bbox;

   if (PRTGIF || noStatusWindow || statusWindow==None) return;

   if (oneLineStatus) {
      int y;

      XClearWindow(mainDisplay, statusWindow);
      if (threeDLook) {
         SetBBRec(&bbox, 0, 0, statusWindowW, statusWindowH);
         TgDrawThreeDButton(mainDisplay, statusWindow, textMenuGC, &bbox,
               TGBS_RAISED, 1, FALSE);
      }
      if (msgFontSet == NULL && msgFontPtr == NULL) {
         if (defaultFontHeight+windowPadding+(brdrW<<1)+2 > btn1_height) {
            y = 2 + brdrW + defaultFontAsc + (windowPadding>>1);
         } else {
            y = ((statusWindowH-defaultFontHeight-windowPadding)>>1) +
                  defaultFontAsc;
         }
         DrawMsgString(mainDisplay, statusWindow, defaultGC,
               (windowPadding>>1)+2, y, oneLineStatusStr,
               strlen(oneLineStatusStr));
      } else {
         if (msgFontHeight+windowPadding+(brdrW<<1)+2 > btn1_height) {
            y = 2 + brdrW + msgFontAsc + (windowPadding>>1);
         } else {
            y = ((statusWindowH-msgFontHeight-windowPadding)>>1) + msgFontAsc;
         }
         if (msgFontPtr != NULL) {
            XSetFont(mainDisplay, defaultGC, msgFontPtr->fid);
         }
         DrawMsgString(mainDisplay, statusWindow, defaultGC,
               (windowPadding>>1)+2, y, oneLineStatusStr,
               strlen(oneLineStatusStr));
         XSetFont(mainDisplay, defaultGC, defaultFontPtr->fid);
      }
   } else {
      int left=0, w=(int)(statusWindowW/3), right=0;

      if (msgFontPtr != NULL) {
         XSetFont(mainDisplay, defaultGC, msgFontPtr->fid);
      }
      for (i=0; i < MAX_STATUS_BTNS; i++) {
         right += w;
         if (right >= statusWindowW) right = statusWindowW-1;
         XClearWindow(mainDisplay, statusSubWindow[i]);
         if (msgFontSet == NULL && msgFontPtr == NULL) {
            DrawMsgString(mainDisplay, statusSubWindow[i], defaultGC, 2,
                  1+defaultFontAsc, btnStatusStr[i], strlen(btnStatusStr[i]));
         } else {
            DrawMsgString(mainDisplay, statusSubWindow[i], defaultGC, 2,
                  1+msgFontAsc, btnStatusStr[i], strlen(btnStatusStr[i]));
         }
         if (threeDLook) {
            SetBBRec(&bbox, 0, 0, statusSubWindowW[i], statusSubWindowH[i]);
            TgDrawThreeDButton(mainDisplay, statusSubWindow[i], textMenuGC,
                  &bbox, TGBS_LOWRED, 1, FALSE);
         }
         left += w;
      }
      if (msgFontSet != NULL || msgFontPtr != NULL) {
         XSetFont(mainDisplay, defaultGC, defaultFontPtr->fid);
      }
   }
}

void RedrawStatusWindow()
{
   register int i;
   XEvent ev;

   if (PRTGIF || noStatusWindow || statusWindow==None) return;

   if (!oneLineStatus) {
      int left=windowPadding, w=(int)(statusWindowW/3), y=0, right=0;

      if (msgFontSet == NULL && msgFontPtr == NULL) {
         if (defaultFontHeight+(brdrW<<1)+2 > btn1_height) {
            y = (statusWindowH-btn1_height)>>1;
         } else {
            y = 1;
         }
      } else {
         if (msgFontHeight+(brdrW<<1)+2 > btn1_height) {
            y = (statusWindowH-btn1_height)>>1;
         } else {
            y = 1;
         }
      }
      for (i=0; i < MAX_STATUS_BTNS; i++) {
         int x=left+(brdrW<<2);

         right += w;
         if (right >= statusWindowW) right = statusWindowW-1;
         XSetTSOrigin(mainDisplay, rasterGC, x, y);
         XSetStipple(mainDisplay, rasterGC, statusBtnPixmap[i]);
         XFillRectangle(mainDisplay, statusWindow, rasterGC,
               x, y, btn1_width, btn1_height);
         XSetTSOrigin(mainDisplay, rasterGC, 0, 0);
         left += w;
      }
   }
   RedrawStatusStrings();
   if (threeDLook) {
      struct BBRec bbox;

      SetBBRec(&bbox, 0, 0, statusWindowW, statusWindowH);
      TgDrawThreeDButton(mainDisplay, statusWindow, textMenuGC, &bbox,
            TGBS_RAISED, 1, FALSE);
   }
   XSync(mainDisplay, False);
   while (XCheckWindowEvent(mainDisplay, statusWindow, ExposureMask, &ev)) ;
}

void SetMouseStatus(Left, Middle, Right)
   char *Left, *Middle, *Right;
   /* If Left, Middle, and Right are all NULL, just refresh the status */
{
   register int i;
   int force_redraw=FALSE, left_index, right_index;

   if (reverseMouseStatusButtons) {
      left_index = 2;
      right_index = 0;
   } else {
      left_index = 0;
      right_index = 2;
   }

   if (PRTGIF || noStatusWindow) return;

   if (oneLineStatus) {
      oneLineStatus = FALSE;
      if (statusWindow != None) {
         XClearWindow(mainDisplay, statusWindow);
         for (i=0; i < MAX_STATUS_BTNS; i++) {
            XMapWindow(mainDisplay, statusSubWindow[i]);
         }
         XSync(mainDisplay, False);
      }
      force_redraw = TRUE;
   } else if (Left != NULL || Middle != NULL || Right != NULL) {
      if (((Left == NULL && *btnStatusStr[left_index] == '\0') ||
            (Left != NULL && strcmp(Left, btnStatusStr[left_index]) == 0)) &&
            ((Middle == NULL && *btnStatusStr[1] == '\0') ||
            (Middle != NULL && strcmp(Middle, btnStatusStr[1]) == 0)) &&
            ((Right == NULL && *btnStatusStr[right_index] == '\0') ||
            (Right != NULL && strcmp(Right, btnStatusStr[right_index]) == 0))) {
         return;
      }
   }
   if (Left != NULL || Middle != NULL || Right != NULL) {
      if (Left != NULL) {
         if (Left != btnStatusStr[left_index] &&
               strcmp(Left, btnStatusStr[left_index]) != 0) {
            UtilStrCpyN(btnStatusStr[left_index], MAXSTRING+1, Left);
         }
      } else {
         *btnStatusStr[left_index] = '\0';
      }
      if (Middle != NULL) {
         if (Middle != btnStatusStr[1] &&
               strcmp(Middle, btnStatusStr[1]) != 0) {
            UtilStrCpyN(btnStatusStr[1], MAXSTRING+1, Middle);
         }
      } else {
         *btnStatusStr[1] = '\0';
      }
      if (Right != NULL) {
         if (Right != btnStatusStr[right_index] &&
               strcmp(Right, btnStatusStr[right_index]) != 0) {
            UtilStrCpyN(btnStatusStr[right_index], MAXSTRING+1, Right);
         }
      } else {
         *btnStatusStr[right_index] = '\0';
      }
   }
   if (force_redraw) {
      RedrawStatusWindow();
   } else {
      RedrawStatusStrings();
   }
}

void SetStringStatus(StatusStr)
   char *StatusStr;
   /* If StatusStr is NULL, just refresh the status */
{
   register int i;
   int force_redraw=FALSE;

   if (PRTGIF || noStatusWindow) return;

   if (!oneLineStatus) {
      oneLineStatus = TRUE;
      if (statusWindow != None) {
         for (i=0; i < MAX_STATUS_BTNS; i++) {
            XUnmapWindow(mainDisplay, statusSubWindow[i]);
         }
         XSync(mainDisplay, False);
      }
      force_redraw = TRUE;
   } else if (StatusStr != NULL && strcmp(StatusStr, oneLineStatusStr) == 0) {
      return;
   }
   /* if StatusStr and oneLineStatusStr are the same string, don't copy */
   if (StatusStr != NULL && StatusStr != oneLineStatusStr) {
      UtilStrCpyN(oneLineStatusStr, MAXSTRING+1, StatusStr);
   }
   if (force_redraw) {
      RedrawStatusWindow();
   } else {
      RedrawStatusStrings();
   }
}

static MouseOverStatusInfo *topStatusInfo=NULL;

void SaveStatusStrings()
{
   MouseOverStatusInfo *pmosi=NULL;
   int i=0;

   if (PRTGIF || noStatusWindow) return;

   pmosi = (MouseOverStatusInfo *)malloc(sizeof(MouseOverStatusInfo));
   if (pmosi == NULL) FailAllocMessage();
   pmosi->next = topStatusInfo;
   for (i=0; i < MAX_STATUS_BTNS; i++) {
      UtilStrCpyN(pmosi->btn_str[i], MAXSTRING+1, btnStatusStr[i]);
   }
   UtilStrCpyN(pmosi->one_line_str, MAXSTRING+1, oneLineStatusStr);
   pmosi->one_line_status = oneLineStatus;
   topStatusInfo = pmosi;
}

void RestoreStatusStrings()
{
   MouseOverStatusInfo *pmosi=NULL;
   int i=0;

   if (PRTGIF || noStatusWindow || topStatusInfo == NULL) return;

   for (i=0; i < MAX_STATUS_BTNS; i++) {
      UtilStrCpyN(btnStatusStr[i], MAXSTRING+1, topStatusInfo->btn_str[i]);
   }
   UtilStrCpyN(oneLineStatusStr, MAXSTRING+1, topStatusInfo->one_line_str);
   if (topStatusInfo->one_line_status != oneLineStatus) {
      if (topStatusInfo->one_line_status) {
         SetStringStatus(oneLineStatusStr);
      } else {
         SetMouseStatus(btnStatusStr[0], btnStatusStr[1], btnStatusStr[2]);
      }
   } else {
      RedrawStatusWindow();
   }
   pmosi = topStatusInfo->next;
   free(topStatusInfo);
   topStatusInfo = pmosi;
}

void SaveStatusStringsIntoBuf(ppsz_buf, pn_one_line)
   char ppsz_buf[MAX_STATUS_BTNS+1][MAXSTRING+1];
   int *pn_one_line;
   /* dimension of ppsz_buf must be [MAX_STATUS_BTNS+1][MAXSTRING+1] */
{
   register int i;

   if (PRTGIF || noStatusWindow) return;
   for (i=0; i < MAX_STATUS_BTNS; i++) {
      UtilStrCpyN(ppsz_buf[i], MAXSTRING+1, btnStatusStr[i]);
   }
   UtilStrCpyN(ppsz_buf[i], MAXSTRING+1, oneLineStatusStr);
   *pn_one_line = oneLineStatus;
}

void RestoreStatusStringsFromBuf(ppsz_buf, one_line)
   char ppsz_buf[MAX_STATUS_BTNS+1][MAXSTRING+1];
   int one_line;
   /* dimension of ppsz_buf must be [MAX_STATUS_BTNS+1][MAXSTRING+1] */
{
   register int i;

   if (PRTGIF || noStatusWindow) return;
   for (i=0; i < MAX_STATUS_BTNS; i++) {
      UtilStrCpyN(btnStatusStr[i], MAXSTRING+1, ppsz_buf[i]);
   }
   UtilStrCpyN(oneLineStatusStr, MAXSTRING+1, ppsz_buf[i]);
   if (one_line != oneLineStatus) {
      if (one_line) {
         SetStringStatus(oneLineStatusStr);
      } else {
         SetMouseStatus(btnStatusStr[0], btnStatusStr[1], btnStatusStr[2]);
      }
   } else {
      RedrawStatusWindow();
   }
}

void BeginProgress(ppi, total_size)
   ProgressInfo *ppi;
   int total_size;
{
   memset(ppi, 0, sizeof(ProgressInfo));
   ppi->total_size = total_size;
   SaveStatusStringsIntoBuf(ppi->status_buf, &ppi->one_line_status);
}

void UpdateProgress(ppi, cur_size)
   ProgressInfo *ppi;
   int cur_size;
{
   int percent=(int)(((double)cur_size) / ((double)ppi->total_size) *
            ((double)100));

   if (percent >= ppi->target_percent) {
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_PROGRESS_PERCENT),
            percent);
      SetStringStatus(gszMsgBox);
      XSync(mainDisplay, False);
      while (ppi->target_percent <= percent) ppi->target_percent += 5;
   }
}

void EndProgress(ppi)
   ProgressInfo *ppi;
{
   RestoreStatusStringsFromBuf(ppi->status_buf, ppi->one_line_status);
}

void MsgEventHandler(input)
   XEvent *input;
{
   XButtonEvent *button_ev;

   if (input->type == Expose) {
      RedrawMsg(TRUE);
   } else if (input->type == EnterNotify) {
      SetMouseStatus(TgLoadCachedString(CSTID_SCROLL_DOWN_1_LINE),
            TgLoadCachedString(CSTID_SCROLL_AROUND),
            TgLoadCachedString(CSTID_SCROLL_UP_1_LINE));
   } else if (input->type == ButtonPress) {
      button_ev = &(input->xbutton);
      if (button_ev->button == Button1) {
         if (button_ev->state & (ShiftMask | ControlMask)) {
            firstCharPos += (((msgFontSet==NULL && msgFontPtr==NULL) ?
                  defaultFontWidth : msgFontWidth)<<2);
            RedrawMsg(TRUE);
         } else {
            if (topMsgNumber+1 >= msgCount) return;

            topMsgNumber++;
            RedrawMsg(TRUE);
         }
      } else if (button_ev->button == Button2) {
         int done=FALSE, saved_x=button_ev->x, saved_y=button_ev->y, x, y;
         int saved_pos=firstCharPos, cur_top_msg_number=topMsgNumber, cur_dx=0;
         XEvent ev;

         RedrawMsg(TRUE);

         XGrabPointer(mainDisplay, msgWindow, False,
            PointerMotionMask | ButtonReleaseMask, GrabModeAsync,
            GrabModeAsync, None, handCursor, CurrentTime);

         while (!done) {
            XNextEvent(mainDisplay, &ev);

            if (ev.type == Expose || ev.type == VisibilityNotify) {
               ExposeEventHandler(&ev, TRUE);
            } else if (ev.type == ButtonRelease) {
               XUngrabPointer(mainDisplay, CurrentTime);
               done = TRUE;
            } else if (ev.type == MotionNotify) {
               double dx=(double)0, dy=(double)0;

               x = ev.xmotion.x;
               y = ev.xmotion.y;
               dx = ((double)(x-saved_x)) /
                     ((double)((msgFontSet==NULL && msgFontPtr==NULL) ?
                     defaultFontWidth : msgFontWidth));
               dy = ((double)(y-saved_y)) /
                     ((double)((msgFontSet==NULL && msgFontPtr==NULL) ?
                     defaultFontHeight : msgFontHeight));
               topMsgNumber += round(dy);
               if (topMsgNumber > msgCount) topMsgNumber = msgCount;
               if (topMsgNumber < 0) topMsgNumber = 0;
               if (topMsgNumber != cur_top_msg_number || round(dx) != cur_dx) {
                  cur_top_msg_number = topMsgNumber;
                  cur_dx = round(dx);
                  firstCharPos = cur_dx *
                        ((msgFontSet==NULL && msgFontPtr==NULL) ?
                        defaultFontWidth : msgFontWidth) + saved_pos;
                  if (firstCharPos < 0) firstCharPos = 0;
                  RedrawMsg(TRUE);
               }
               while (XCheckMaskEvent(mainDisplay, PointerMotionMask, &ev)) ;
            }
         }
      } else if (button_ev->button == Button3) {
         if (button_ev->state & (ShiftMask | ControlMask)) {
            if (firstCharPos <= 0) return;

            firstCharPos -= (((msgFontSet==NULL && msgFontPtr==NULL) ?
                  defaultFontWidth : msgFontWidth)<<2);
            if (firstCharPos < 0) firstCharPos = 0;
            RedrawMsg(TRUE);
         } else {
            if (topMsgNumber == 0) return;

            topMsgNumber--;
            RedrawMsg(TRUE);
         }
      }
   }
}

void StatusEventHandler(input)
   XEvent *input;
{
   if (PRTGIF || noStatusWindow) return;

   if (input->type == Expose) {
      RedrawStatusWindow();
   }
}

void RedrawUserRedrawWindow()
{
   XEvent ev;
   int x, y;
   XGCValues values;

   while (XCheckWindowEvent(mainDisplay,userRedrawWindow,ExposureMask,&ev)) ;
   while (XCheckWindowEvent(mainDisplay,userRedrawWindow,ButtonPressMask,&ev)) ;

   x = ((userRedrawWindowW-redraw_width)>>1);
   y = ((userRedrawWindowH-redraw_height)>>1);
   XClearWindow(mainDisplay, userRedrawWindow);
   values.stipple = userRedrawBitmap;
   values.ts_x_origin = x;
   values.ts_y_origin = y;
   XChangeGC(mainDisplay, rasterGC,
         GCStipple | GCTileStipXOrigin | GCTileStipYOrigin, &values);
   XFillRectangle(mainDisplay, userRedrawWindow, rasterGC, x, y,
         redraw_width, redraw_height);
   XSetTSOrigin(mainDisplay, rasterGC, 0, 0);
   if (userDisableRedraw) {
      GC gc;
      int shift=windowPadding;

      values.foreground = myFgPixel;
      values.background = (threeDLook ? myLtGryPixel : myBgPixel);
      gc = XCreateGC(mainDisplay, userRedrawWindow, GCForeground | GCBackground,
            &values);
      XDrawLine(mainDisplay, userRedrawWindow, gc, shift,
            userRedrawWindowH-1-shift, userRedrawWindowW-1-shift, shift);
      XFreeGC(mainDisplay, gc);
   }
   if (threeDLook) {
      struct BBRec bbox;

      SetBBRec(&bbox, 0, 0, userRedrawWindowW, userRedrawWindowH);
      TgDrawThreeDButton(mainDisplay, userRedrawWindow, textMenuGC, &bbox,
            (userDisableRedraw ? TGBS_LOWRED : TGBS_RAISED), 2, TRUE);
   }
}

static
void ToggleUserRedraw()
{
   userDisableRedraw = !userDisableRedraw;
   RedrawUserRedrawWindow();
   if (!userDisableRedraw) {
      ClearAndRedrawDrawWindow();
   }
}

void UserRedrawEventHandler(input)
   XEvent *input;
{
   if (PRTGIF || noStatusWindow) return;

   if (input->type == Expose) {
      RedrawUserRedrawWindow();
   } else if (input->type == EnterNotify) {
      SetMouseStatus(TgLoadCachedString(userDisableRedraw ?
            CSTID_ENABLE_REDRAW : CSTID_DISABLE_REDRAW),
            TgLoadCachedString(CSTID_PARANED_NONE),
            TgLoadCachedString(CSTID_PARANED_NONE));
   } else if (input->type == ButtonPress) {
      ToggleUserRedraw();
      SetMouseStatus(TgLoadCachedString(userDisableRedraw ?
            CSTID_ENABLE_REDRAW : CSTID_DISABLE_REDRAW),
            TgLoadCachedString(CSTID_PARANED_NONE),
            TgLoadCachedString(CSTID_PARANED_NONE));
   }
}

void InitStatus()
{
   register int i;
   char *c_ptr;

   oneLineStatus = FALSE;
   *oneLineStatusStr = '\0';
   for (i=0; i < MAX_STATUS_BTNS; i++) *btnStatusStr[i] = '\0';

   reverseMouseStatusButtons = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
         "ReverseMouseStatusButtons")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      reverseMouseStatusButtons = TRUE;
   }
   gnMsgRows = (int)((msgWindowH-2-(windowPadding<<1)) /
         ((msgFontSet==NULL && msgFontPtr==NULL) ? defaultFontHeight :
         msgFontHeight));
}

void CleanUpStatus()
{
   while (topStatusInfo != NULL) {
      MouseOverStatusInfo *next_mosi=topStatusInfo->next;

      free(topStatusInfo);
      topStatusInfo = next_mosi;
   }
   if (redraw_bits == NULL) { }
   if (btn1_bits == NULL) { }
}

static char	*scanVal=NULL;
static char	*scanSep=NULL;

void InitScan(s, pat)
   char *s, *pat;
{
   scanVal = s;
   scanSep = pat;
}

static
char *GetString()
{
   char *c_ptr;

   if (scanVal == NULL || scanSep == NULL) return NULL;
   while (*scanVal!='\0' && strchr(scanSep,*scanVal)!=NULL) *scanVal++ = '\0';
   if (*scanVal == '\0') return NULL;

   c_ptr=scanVal;
   for ( ; *scanVal!='\0' && strchr(scanSep,*scanVal)==NULL; scanVal++) ;
   if (*scanVal != '\0') *scanVal++ = '\0';

   return c_ptr;
}

int ScanValue(fmt, v, item, stype)
   char *fmt, *item, *stype;
   void *v;
{
   char *c_ptr, msg[MAXPATHLENGTH];

   if (scanVal == NULL) return INVALID;
#ifdef DEBUG /* debug, do not translate */
   printf("get %s for %s from %s:  ", item, stype, scanVal);
#endif

   if ((c_ptr=GetString()) == NULL) {
#ifdef DEBUG /* debug, do not translate */
      printf("nothing\n");
#endif
      (void)sprintf(msg, TgLoadString(STID_FILE_LINE_MISS_FIELD_IN_OBJ),
            scanFileName, scanLineNum, item, stype);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", msg);
      } else {
         Msg(msg);
      }
      return INVALID;
   }
   if (sscanf(c_ptr, fmt, v) != 1) {
#ifdef DEBUG /* debug, do not translate */
      printf("error in %s\n", c_ptr);
#endif
      (void)sprintf(msg, TgLoadString(STID_FILE_LINE_BAD_FIELD_IN_OBJ),
            scanFileName, scanLineNum, item, stype, c_ptr);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", msg);
      } else {
         Msg(msg);
      }
      return INVALID;
   }

#ifdef DEBUG /* debug, do not translate */
   printf("got %d\n", (*(int *)v));
#endif

   return 0;
}

int ScanDynStrValue(v, item, stype)
   struct DynStrRec *v;
   char *item, *stype;
   /*
    * 
    */
{
   char *c_ptr=NULL, *psz=NULL, msg[MAXPATHLENGTH];

   if (scanVal == NULL) return INVALID;
#ifdef DEBUG /* debug, do not translate */
   printf("get %s for %s from %s:  ", item, stype, scanVal);
#endif

   if ((c_ptr=GetString()) == NULL) {
#ifdef DEBUG /* debug, do not translate */
      printf("nothing\n");
#endif
      (void)sprintf(msg, TgLoadString(STID_FILE_LINE_MISS_FIELD_IN_OBJ),
            scanFileName, scanLineNum, item, stype);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", msg);
      } else {
         Msg(msg);
      }
      return INVALID;
   }
   while (*c_ptr != '"' && *c_ptr != '\0') c_ptr++;
   if (c_ptr != NULL) {
      psz = ReadString(++c_ptr);
   }
   if (psz == NULL) {
#ifdef DEBUG /* debug, do not translate */
      printf("error in %s\n", c_ptr);
#endif
      (void)sprintf(msg, TgLoadString(STID_FILE_LINE_BAD_FIELD_IN_OBJ),
            scanFileName, scanLineNum, item, stype, c_ptr);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", msg);
      } else {
         Msg(msg);
      }
      return INVALID;
   }
   *(--psz) = '\0';
   DynStrSet(v, c_ptr);

#ifdef DEBUG /* debug, do not translate */
   printf("got \"%s\"\n", ((struct DynStrRec *)v)->s);
#endif

   return 0;
}

#define MAXEMERGENCYCOUNT 5

static int	emergencyCount = 0;

static
void SaveEmergencyTmpFile()
{
   switch (SaveTmpFile("EmergencySave")) {
   case OBJ_FILE_TYPE:
      fprintf(stderr, TgLoadString(STID_WORKING_FILE_SAVED_TO),
            "EmergencySave", OBJ_FILE_EXT);
      fprintf(stderr, "\n");
      break;
   case SYM_FILE_TYPE:
      fprintf(stderr, TgLoadString(STID_WORKING_FILE_SAVED_TO),
            "EmergencySave", SYM_FILE_EXT);
      fprintf(stderr, "\n");
      break;
   case PIN_FILE_TYPE:
      fprintf(stderr, TgLoadString(STID_WORKING_FILE_SAVED_TO),
            "EmergencySave", PIN_FILE_EXT);
      fprintf(stderr, "\n");
      break;
   case INVALID:
      fprintf(stderr, "%s\n",
            TgLoadString(STID_UNABLE_TO_SAVE_WORKING_FILE));
      break;
   }
}

void EmergencySave(sig)
   int sig;
{
   switch (sig) {
   case SIGHUP:
      fprintf(stderr, TgLoadString(STID_NAMED_SIG_RECV), "SIGHUP");
      fprintf(stderr, "\n");
      break;
   case SIGFPE:
      fprintf(stderr, TgLoadString(STID_NAMED_SIG_RECV), "SIGFPE");
      fprintf(stderr, "\n");
      break;
#ifndef linux
   case SIGBUS:
      fprintf(stderr, TgLoadString(STID_NAMED_SIG_RECV), "SIGBUS");
      fprintf(stderr, "\n");
      break;
#endif
   case SIGSEGV:
      fprintf(stderr, TgLoadString(STID_NAMED_SIG_RECV), "SIGSEGV");
      fprintf(stderr, "\n");
      break;
   }
   if (++emergencyCount > MAXEMERGENCYCOUNT) {
      fprintf(stderr, TgLoadString(STID_ERR_COUNT_EXCEED_TOOL_ABORTED),
            MAXEMERGENCYCOUNT, "EmergencySave()", TOOL_NAME);
      fprintf(stderr, "\n");
      exit(-1);
   }
   if (exitNormally) return;

   signal(SIGHUP, SIG_DFL);
   signal(SIGFPE, SIG_DFL);
#ifndef linux
   signal(SIGBUS, SIG_DFL);
#endif
   signal(SIGSEGV, SIG_DFL);

   if (fileModified) {
      SaveEmergencyTmpFile();
   }
   exitNormally = TRUE;
   exit(0);
}

static
int EmergencySaveForXCont(s)
   char *s;
{
   if (++emergencyCount > MAXEMERGENCYCOUNT) {
      fprintf(stderr, TgLoadString(STID_ERR_COUNT_EXCEED_TOOL_ABORTED),
            MAXEMERGENCYCOUNT, s, TOOL_NAME);
      fprintf(stderr, "\n");
      exit(-1);
   }
   if (exitNormally) return 0;

   if (copyingToCutBuffer) {
      emergencyCount--;
      copyingToCutBuffer = INVALID;
      return 0;
   }
   signal(SIGHUP, SIG_DFL);
   signal(SIGFPE, SIG_DFL);
#ifndef linux
   signal(SIGBUS, SIG_DFL);
#endif
   signal(SIGSEGV, SIG_DFL);

   if (fileModified) {
      SaveEmergencyTmpFile();
   }
   exitNormally = TRUE;
   return (-1);
}

int EmergencySaveForX(dsp, ev)
   Display *dsp;
   XErrorEvent *ev;
{
   char msg[MAXSTRING+1];

   XGetErrorText(mainDisplay, (int)(ev->error_code), msg, MAXSTRING);
   fprintf(stderr, TgLoadString(STID_X_ERROR_MSG), msg);
   fprintf(stderr, "\n");

   return EmergencySaveForXCont("EmergencySaveForX()");
}

int IOEmergencySaveForX(dsp)
   Display *dsp;
{
   return EmergencySaveForXCont("IOEmergencySaveForX()");
}

void Error(where, msg)
   char *where, *msg;
{
   fprintf(stderr, TgLoadString(STID_FATAL_ERROR_IN_FUNC_PLUS_DESC),
         where, msg);
   fprintf(stderr, "\n");
   fprintf(stderr, TgLoadString(STID_TOOL_ABORTED), TOOL_NAME);
   fprintf(stderr, "\n");
   if (fileModified) EmergencySave(0);
   exit(-1);
}

void GetTgifVersionAndPatchLevel(buf, buf_sz)
   char *buf;
   int buf_sz;
{
   char buf1[MAXSTRING], buf2[MAXSTRING];

   *buf1 = *buf2 = '\0';
   if (TGIF_PATCHLEVEL == 0) {
      if (*specialBuild == '\0') {
         sprintf(buf1, TgLoadString(STID_TOOL_VERSION),
               TOOL_NAME, versionString);
      } else {
         sprintf(buf1, TgLoadString(STID_TOOL_VERSION_SPC_BUILD),
               TOOL_NAME, versionString, specialBuild);
      }
      strcpy(buf2, versionString);
   } else {
      if (*specialBuild == '\0') {
         sprintf(buf1, TgLoadString(STID_TOOL_VERSION_PATCH),
               TOOL_NAME, versionString, TGIF_PATCHLEVEL);
      } else {
         sprintf(buf1, TgLoadString(STID_TOOL_VERSION_PATCH_SPC_BUILD),
               TOOL_NAME, versionString, TGIF_PATCHLEVEL, specialBuild);
      }
      sprintf(buf2, "%s.%1d", versionString, TGIF_PATCHLEVEL);
   }
#ifdef TGIF_DISTR_VER
   if (strcmp(TGIF_DISTR_VER, buf2) != 0) {
      sprintf(&buf1[strlen(buf1)], TgLoadString(STID_TGIF_DISTRUBITION),
            TGIF_DISTR_VER);
   }
#endif /* TGIF_DISTR_VER */
#ifdef NOT_DEFINED
   sprintf(&buf1[strlen(buf1)], TgLoadString(STID_TGIF_FLAGS), "_TGIF_WB2");
#endif /* NOT_DEFINED */
   UtilStrCpyN(buf, buf_sz, buf1);
}

void Usage(tool_name)
   char *tool_name;
{
   char buf[MAXSTRING];

   GetTgifVersionAndPatchLevel(buf, sizeof(buf));
   fprintf(stdout, "%s\n", buf);

   fprintf(stdout, "%s\n\n", copyrightString);
   fprintf(stdout, "Usage:\t%s \\\n", tool_name);
   fprintf(stdout, "\t[-display displayname] \\\n");
   fprintf(stdout, "\t[-fg <color>] \\\n");
   fprintf(stdout, "\t[-bg <color>] \\\n");
   fprintf(stdout, "\t[-bd <color>] \\\n");
   fprintf(stdout, "\t[-rv] [-nv] \\\n");
   fprintf(stdout, "\t[-bw] \\\n");
   fprintf(stdout, "\t[-tgwb2 [-rmcastlibdir <directory> | -rmcastlibpath <path>] [-parent <parent_window_id>]] \\\n");
   fprintf(stdout, "\t[-reqcolor] \\\n");
   fprintf(stdout, "\t[-cwo[+sbwarp]] \\\n");
   fprintf(stdout, "\t[-hyper] \\\n");
   fprintf(stdout, "\t[-a4 | -letter] \\\n");
   fprintf(stdout, "\t[-geometry <geom>] [=<geom>] \\\n");
   fprintf(stdout, "\t[-exec <file>] \\\n");
   fprintf(stdout, "\t[-dbim {xcin|chinput|xim|kinput2}] \\\n");
   fprintf(stdout, "\t[-sbim xim] \\\n");
   fprintf(stdout, "\t[-usexlib] \\\n");
   fprintf(stdout, "\t[-listdontreencode] \\\n");
   fprintf(stdout, "\t[-version | --version] \\\n");
   fprintf(stdout, "\t[-pdfspd | -pdfspd=true | -pdfspd=false] \\\n");
   fprintf(stdout, "\t[-pssetup \"<string>\"] \\\n");
   fprintf(stdout, "\t[-nomode] \\\n");
   fprintf(stdout, "\t[-tg2genparse=\"<string>\"] \\\n");
   fprintf(stdout, "\t[{<file>[.%s] | -merge <file1>[.%s] <file2>[.%s] ...}]\n",
         OBJ_FILE_EXT, OBJ_FILE_EXT, OBJ_FILE_EXT);
   fprintf(stdout, "or:\n");
   fprintf(stdout, "\t%s -print \\\n", tool_name);
   fprintf(stdout, "\t[-display displayname] \\\n");
   fprintf(stdout, "\t[-eps | -p | -epsi | -tiffepsi] \\\n");
   fprintf(stdout, "\t[-ps | -f] \\\n");
   fprintf(stdout, "\t[-text] \\\n");
   fprintf(stdout, "\t[-gif | -png | -jpeg | -ppm | -pbm | -xpm | -xbm | -html] \\\n");
   fprintf(stdout, "\t[-pdf] \\\n");
   fprintf(stdout, "\t[-netlist] \\\n");
   fprintf(stdout, "\t[-stdout] \\\n");
   fprintf(stdout, "\t[-raw[+h[eaderonly]]] \\\n");
   fprintf(stdout, "\t[-dosepsfilter [-previewonly]] \\\n");
   fprintf(stdout, "\t[-status] \\\n");
   fprintf(stdout, "\t[-gray] \\\n");
   fprintf(stdout, "\t[-color | -reqcolor] \\\n");
   fprintf(stdout, "\t[-adobe | -adobe=<number>/<number>] \\\n");
   fprintf(stdout,
         "\t[-dontreencode=\"<%s>:<%s>:...\" | -listdontreencode] \\\n",
         "fontfamily", "fontfamily");
   fprintf(stdout, "\t[-version | --version | -justversion] \\\n");
   fprintf(stdout, "\t[-producedby=\"<string>\"] \\\n");
   fprintf(stdout, "\t[-page <number>] \\\n");
   fprintf(stdout, "\t[-print_cmd \"<command>\"] \\\n");
   fprintf(stdout, "\t[-one_file_per_page] \\\n");
   fprintf(stdout, "\t[-pepsc] \\\n");
   fprintf(stdout, "\t[-pdfspd | -pdfspd=true | -pdfspd=false] \\\n");
   fprintf(stdout, "\t[-pssetup \"<string>\"] \\\n");
   fprintf(stdout, "\t[-j2p6_cmd \"<command>\"] \\\n");
   fprintf(stdout, "\t[-dontcondense | -condensed] \\\n");
   fprintf(stdout, "\t[-a4 | -letter] \\\n");
   fprintf(stdout, "\t[-noshowpageineps] \\\n");
   fprintf(stdout, "\t[-quiet] \\\n");
   fprintf(stdout, "\t[-bop_hook \"<string>\"] \\\n");
   fprintf(stdout, "\t[-eop_hook \"<string>\"] \\\n");
   fprintf(stdout, "\t[-tmp_file_mode \"<octal number>\"] \\\n");
   fprintf(stdout, "\t[-odir] \\\n");
   fprintf(stdout, "\t[-exec <file>] \\\n");
   fprintf(stdout, "\t[<file1>[.%s] <file2>[.%s] ...]\n",
         OBJ_FILE_EXT, OBJ_FILE_EXT);
   exit(-1);
}

int ProcessPrTgifOptions(argc, argv, from_prtgif)
   int argc, from_prtgif;
   char *argv[];
{
   char *c_ptr=NULL;
   int saved_argc=argc, show_copyright=FALSE;
   char **saved_argv=argv;

   *cmdLineTmpFileModeStr = '\0';
   *cmdLineGenParser = '\0';
   *cmdLineFileToExec = '\0';
   *cmdLineJpegToPpm6Cmd = '\0';
   *cmdLineCustomPatternDir = '\0';
   cmdLineOneFilePerPage = FALSE;
   *outputDir = '\0';
   for (argc--, argv++; argc > 0; argc--, argv++) {
      if (**argv == '-') {
         if (strcmp(*argv, "-version") == 0 ||
               strcmp(*argv, "--version") == 0) {
            show_copyright = TRUE;
         } else if (strcmp(*argv, "-tgrm2") == 0) {
            cmdLineTgrm2 = TRUE;
         } else if (strcmp(*argv, "-justversion") == 0) {
            char buf[MAXSTRING];

            GetTgifVersionAndPatchLevel(buf, sizeof(buf));
            fprintf(stdout, "%s\n", buf);
            fprintf(stdout, "%s (%s)\n\n", copyrightString, authorEmailString);
            fflush(stdout);

            return INVALID;
         }
      }
   }
   argc = saved_argc;
   argv = saved_argv;
   if (show_copyright) {
      char buf[MAXSTRING];

      GetTgifVersionAndPatchLevel(buf, sizeof(buf));
      fprintf(stderr, "%s\n", buf);
      fprintf(stderr, "%s (%s)\n\n", copyrightString, authorEmailString);
      fflush(stderr);
   }
   if (!from_prtgif) {
      if ((c_ptr=getenv("TGIFPATH")) == NULL) {
         strcpy(drawPath, TGIF_PATH);
      } else {
         if (((int)strlen(c_ptr)) >= MAXSTRING) {
            /* must be an error */
            strcpy(drawPath, TGIF_PATH);
         } else {
            strcpy(drawPath, c_ptr);
         }
      }
#ifdef PRINT_CMD
      strcpy(printCommand, PRINT_CMD);
#else
#ifdef VMS
      strcpy(printCommand, "print");
#else
#ifdef SYSV
      strcpy(printCommand, "lp -dpostscript");
#else
      strcpy(printCommand, "lpr");
#endif /* SYSV */
#endif /* VMS */
#endif /* PRINT_CMD */
   }
   cmdLineWhereToPrint = INVALID;
   whereToPrint = PRINTER;

   usePsAdobeString = FALSE;
   *adobeString = *epsfString = '\0';
   *cmdLineDontReencode = *cmdLineProducedBy = '\0';
   *cmdLineOpenFile = '\0';
#ifdef _USE_PS_ADOBE_STRING
   {
      char buf[MAXSTRING];

      UtilStrCpyN(buf, sizeof(buf), _USE_PS_ADOBE_STRING);
      if (!ParsePsAdobeString(buf,&usePsAdobeString,adobeString,epsfString)) {
         fprintf(stderr, TgLoadString(STID_BAD_COMPILE_OPT_USE_ALT_STR),
               "_USE_PS_ADOBE_STRING", _USE_PS_ADOBE_STRING, "false");
         fprintf(stderr, "\n");
         usePsAdobeString = FALSE;
         *adobeString = *epsfString = '\0';
      }
   }
#endif /* _USE_PS_ADOBE_STRING */
   c_ptr = printCommand;
   c_ptr += strlen(printCommand);
   for (argc--, argv++; argc > 0; argc--, argv++) {
      if (**argv == '-') {
         int len=0;

         if (strcmp(*argv, "-display") == 0) {
            argc--; argv++;
            if (argc <= 0) {
               fprintf(stderr, "%s\n\n",
                     TgLoadString(STID_MISSING_DISPLAY_NAME));
               Usage(progName);
            }
            if ((displayName=UtilStrDup(*argv)) == NULL) FailAllocMessage();
            cmdLineOpenDisplay = TRUE;
         } else if (strcmp(*argv, "-p") == 0 || strcmp(*argv, "-eps") == 0) {
            whereToPrint = cmdLineWhereToPrint = LATEX_FIG;
         } else if (strcmp(*argv,"-f") == 0 || strcmp(*argv,"-ps") == 0) {
            whereToPrint = cmdLineWhereToPrint = PS_FILE;
         } else if (strcmp(*argv,"-stdout") == 0) {
            cmdLineStdOut = TRUE;
         } else if (strcmp(*argv,"-epsi") == 0) {
            cmdLineWhereToPrint = EPSI_FILE;
            cmdLineTiffEPSI = FALSE;
            cmdLineOpenDisplay = TRUE;
         } else if (strcmp(*argv,"-tiffepsi") == 0) {
            cmdLineWhereToPrint = EPSI_FILE;
            cmdLineTiffEPSI = TRUE;
            cmdLineOpenDisplay = TRUE;
         } else if (strcmp(*argv,"-gif") == 0) {
            cmdLineWhereToPrint = GIF_FILE;
            cmdLineColor = TRUE;
            cmdLineOpenDisplay = TRUE;
         } else if (strcmp(*argv,"-png") == 0) {
            cmdLineWhereToPrint = PNG_FILE;
            cmdLineColor = TRUE;
            cmdLineOpenDisplay = TRUE;
         } else if (strcmp(*argv,"-jpeg") == 0) {
            cmdLineWhereToPrint = JPEG_FILE;
            cmdLineColor = TRUE;
            cmdLineOpenDisplay = TRUE;
         } else if (strcmp(*argv,"-ppm") == 0) {
            cmdLineWhereToPrint = PPM_FILE;
            cmdLineColor = TRUE;
            cmdLineOpenDisplay = TRUE;
         } else if (strcmp(*argv,"-pbm") == 0) {
            cmdLineWhereToPrint = PPM_FILE;
            cmdLineColor = FALSE;
            cmdLineOpenDisplay = TRUE;
         } else if (strcmp(*argv,"-xpm") == 0) {
            cmdLineWhereToPrint = XBM_FILE;
            cmdLineColor = TRUE;
            cmdLineOpenDisplay = TRUE;
         } else if (strcmp(*argv,"-xbm") == 0) {
            cmdLineWhereToPrint = XBM_FILE;
            cmdLineColor = FALSE;
            cmdLineOpenDisplay = TRUE;
         } else if (strcmp(*argv,"-html") == 0) {
            cmdLineWhereToPrint = HTML_FILE;
            cmdLineColor = TRUE;
            cmdLineOpenDisplay = TRUE;
         } else if (strcmp(*argv,"-pdf") == 0) {
            whereToPrint = cmdLineWhereToPrint = PDF_FILE;
         } else if (strcmp(*argv, "-text") == 0) {
            whereToPrint = cmdLineWhereToPrint = TEXT_FILE;
         } else if (strcmp(*argv, "-netlist") == 0) {
            whereToPrint = cmdLineWhereToPrint = NETLIST_FILE;
         } else if (strcmp(*argv,"-svg") == 0) {
            whereToPrint = cmdLineWhereToPrint = SVG_FILE;
         } else if (strcmp(*argv, "-raw") == 0) {
            cmdLineDumpURL = TRUE;
            cmdLineDumpURLWithHeader = FALSE;
            cmdLineDumpURLHeaderOnly = FALSE;
         } else if (strcmp(*argv, "-raw+h") == 0) {
            cmdLineDumpURL = TRUE;
            cmdLineDumpURLWithHeader = TRUE;
            cmdLineDumpURLHeaderOnly = FALSE;
         } else if (strcmp(*argv, "-raw+headeronly") == 0) {
            cmdLineDumpURL = TRUE;
            cmdLineDumpURLWithHeader = TRUE;
            cmdLineDumpURLHeaderOnly = TRUE;
         } else if (strcmp(*argv, "-dosepsfilter") == 0) {
            cmdLineDosEpsFilter = TRUE;
         } else if (strcmp(*argv, "-previewonly") == 0) {
            cmdLinePreviewOnly = TRUE;
         } else if (strcmp(*argv, "-status") == 0) {
            cmdLineDumpURLShowStatus = TRUE;
         } else if (strcmp(*argv, "-page") == 0) {
            argc--; argv++;
            if (argc <= 0) {
               fprintf(stderr, "%s\n\n",
                     TgLoadString(STID_MISSING_PAGE_NUMBER));
               Usage(progName);
            }
            cmdLineHasPageNum = TRUE;
            UtilStrCpyN(cmdLinePageNumStr, sizeof(cmdLinePageNumStr), *argv);
            if (sscanf(cmdLinePageNumStr, "%d", &cmdLinePageNum) != 1 ||
                  cmdLinePageNum <= 0) {
               cmdLineHasPageNum = FALSE;
               fprintf(stderr, TgLoadString(STID_INVALID_GIVEN_PAGE_NUMBER_STR),
                     cmdLinePageNumStr);
               fprintf(stderr, "\n\n");
               Usage(progName);
            }
         } else if (strcmp(*argv, "-one_file_per_page") == 0) {
            cmdLineOneFilePerPage = TRUE;
         } else if (strcmp(*argv, "-pepsc") == 0) {
            stripEPSComments = FALSE;
         } else if (strcmp(*argv, "-nolandpdfspd") == 0) {
            cmdLinePdfSetPageDevice = FALSE;
            fprintf(stderr, TgLoadString(STID_OBSOLETE_CMDLINE_ASSUME),
                  "-nolandpdfspd", "-pdfspd=false");
            fprintf(stderr, "\n");
         } else if (strcmp(*argv, "-pdfspd") == 0) {
            cmdLinePdfSetPageDevice = TRUE;
         } else if (strncmp(*argv, "-pdfspd=", 8) == 0) {
            char tmp_str[80];

            UtilStrCpyN(tmp_str, sizeof(tmp_str), &(*argv)[8]);
            UtilTrimBlanks(tmp_str);
            if (UtilStrICmp(tmp_str, "true") == 0) {
               cmdLinePdfSetPageDevice = TRUE;
            } else if (UtilStrICmp(tmp_str, "false") == 0) {
               cmdLinePdfSetPageDevice = FALSE;
            } else {
               fprintf(stderr, TgLoadString(STID_BAD_CMDLINE_OPT_IGNORED),
                     "-pdfspd", tmp_str);
               fprintf(stderr, "\n");
            }
         } else if (strcmp(*argv, "-pssetup") == 0) {
            argc--; argv++;
            if (argc <= 0) {
               fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                     "-pssetup");
               fprintf(stderr, "\n\n");
               Usage(progName);
            }
            cmdLinePsSetup = TRUE;
            SetPsSetup(*argv);
         } else if (strcmp(*argv, "-dontcondense") == 0) {
            SetCmdLineDontCondense();
         } else if (strcmp(*argv, "-condensed") == 0) {
            SetCmdLineCondensedPS();
         } else if (strcmp(*argv, "-a4") == 0) {
            cmdLineA4 = TRUE;
         } else if (strcmp(*argv, "-letter") == 0) {
            cmdLineLetter = TRUE;
         } else if (strcmp(*argv, "-noshowpageineps") == 0) {
            cmdLineNoShowPageInEPS = TRUE;
         } else if (strcmp(*argv, "-quiet") == 0) {
            cmdLineQuiet = TRUE;
         } else if (strcmp(*argv, "-bop_hook") == 0) {
            argc--; argv++;
            if (argc <= 0) {
               fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                     "-bop_hook");
               fprintf(stderr, "\n\n");
               Usage(progName);
            }
            SetBopHook(*argv);
         } else if (strcmp(*argv, "-eop_hook") == 0) {
            argc--; argv++;
            if (argc <= 0) {
               fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                     "-eop_hook");
               fprintf(stderr, "\n\n");
               Usage(progName);
            }
            SetEopHook(*argv);
         } else if (strcmp(*argv, "-tmp_file_mode") == 0) {
            int nFileMode=0;

            argc--; argv++;
            if (argc <= 0) {
               fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                     "-tmp_file_mode");
               fprintf(stderr, "\n\n");
               Usage(progName);
            }
            UtilStrCpyN(cmdLineTmpFileModeStr, sizeof(cmdLineTmpFileModeStr),
                  *argv);
            UtilTrimBlanks(cmdLineTmpFileModeStr);
            if (*cmdLineTmpFileModeStr != '\0' && sscanf(cmdLineTmpFileModeStr,
                  "%o", &nFileMode) == 1) {
               if (nFileMode >= 0) {
                  tmpFileMode = nFileMode;
               } else {
                  fprintf(stderr,
                        TgLoadString(STID_INVALID_ARG_FOR_CMDLINE_OPT),
                        "-tmp_file_mode", cmdLineTmpFileModeStr);
                  fprintf(stderr, "\n\n");
                  Usage(progName);
               }
            }
         } else if (strcmp(*argv, "-gray") == 0) {
            useGray = TRUE;
         } else if (strcmp(*argv, "-bw") == 0) {
         } else if (strcmp(*argv, "-color") == 0) {
            cmdLineRequestedColor = FALSE;
            cmdLineColor = TRUE;
         } else if (strcmp(*argv, "-reqcolor") == 0) {
            cmdLineRequestedColor = TRUE;
            cmdLineColor = TRUE;
         } else if (strcmp(*argv, "-adobe") == 0) {
            usePsAdobeString = TRUE;
         } else if (strncmp(*argv, "-adobe=", 7) == 0) {
            char tmp_str[80];

            UtilStrCpyN(tmp_str, sizeof(tmp_str), &(*argv)[7]);
            UtilTrimBlanks(tmp_str);
            if (!ParsePsAdobeString(tmp_str, &usePsAdobeString, adobeString,
                  epsfString)) {
               fprintf(stderr, TgLoadString(STID_BAD_CMDLINE_OPT_USE_ALT_STR),
                     "-adobe", tmp_str, "false");
               fprintf(stderr, "\n");
               usePsAdobeString = FALSE;
               *adobeString = *epsfString = '\0';
            }
         } else if (strcmp(*argv, "-listdontreencode") == 0) {
#ifdef _DONT_REENCODE
            fprintf(stderr, "-D_DONT_REENCODE=\"%s\"\n", _DONT_REENCODE);
#endif /* _DONT_REENCODE */
         } else if (strcmp(*argv, "-version") == 0 ||
               strcmp(*argv, "--version") == 0) {
            cmdLineVersion = TRUE;
         } else if (strncmp(*argv, "-dontreencode=", 14) == 0) {
            strcpy(cmdLineDontReencode, &(*argv)[14]);
            UtilTrimBlanks(cmdLineDontReencode);
            if (*cmdLineDontReencode != '\0') {
               UtilRemoveQuotes(cmdLineDontReencode);
               UtilTrimBlanks(cmdLineDontReencode);
            }
         } else if (strncmp(*argv, "-producedby=", 12) == 0) {
            UtilStrCpyN(cmdLineProducedBy, sizeof(cmdLineProducedBy),
                  &(*argv)[12]);
            UtilTrimBlanks(cmdLineProducedBy);
            if (*cmdLineProducedBy != '\0') {
               UtilRemoveQuotes(cmdLineProducedBy);
            }
         } else if (strncmp(*argv, "-tg2genparser=", 14) == 0) {
            UtilStrCpyN(cmdLineGenParser, sizeof(cmdLineGenParser),
                  &(*argv)[14]);
            UtilTrimBlanks(cmdLineGenParser);
            if (*cmdLineGenParser != '\0') {
               UtilRemoveQuotes(cmdLineGenParser);
            }
         } else if (strcmp(*argv, "-parse_html") == 0) {
            cmdLineParseHtml = TRUE;
            /* cmdLineDumpURL will be reset in UrlDump() */
            cmdLineDumpURL = TRUE;
            cmdLineDumpURLWithHeader = FALSE;
            cmdLineDumpURLHeaderOnly = FALSE;
         } else if (strcmp(*argv, "-exec") == 0) {
            argc--; argv++;
            if (argc <= 0) {
               fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                     "-exec");
               fprintf(stderr, "\n\n");
               Usage(progName);
            }
            UtilStrCpyN(cmdLineFileToExec, sizeof(cmdLineFileToExec), *argv);
            UtilTrimBlanks(cmdLineFileToExec);
            if (*cmdLineFileToExec == '-' && cmdLineFileToExec[1] != '\0') {
               fprintf(stderr, "%s\n\n",
                     TgLoadString(STID_BAD_EXEC_FNAME_DASH));
               Usage(progName);
            }
            cmdLineOpenDisplay = TRUE;
         } else if (strcmp(*argv, "-tgif_path") == 0) {
            argc--; argv++;
            if (argc <= 0) {
               fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                     "-tgif_path");
               fprintf(stderr, "\n\n");
               Usage(progName);
            }
            len = strlen(*argv);
            if (**argv == '"' && (*argv)[len-1] == '"') {
               strcpy(drawPath, &(*argv)[1]);
               drawPath[strlen(drawPath)-1] = '\0';
            } else {
               strcpy(drawPath, *argv);
            }
            if (!from_prtgif) {
               fprintf(stderr, "%s\n",
                     TgLoadString(STID_TGIF_PATH_SHOULD_NOT_BE_USED));
            }
         } else if (strcmp(*argv, "-print_cmd") == 0) {
            argc--; argv++;
            if (argc <= 0) {
               fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                     "-print_cmd");
               fprintf(stderr, "\n\n");
               Usage(progName);
            }
            len = strlen(*argv);
            if (**argv == '"' && (*argv)[len-1] == '"') {
               strcpy(printCommand, &(*argv)[1]);
               printCommand[strlen(printCommand)-1] = '\0';
            } else {
               strcpy(printCommand, *argv);
            }
         } else if (strcmp(*argv, "-j2p6_cmd") == 0) {
            argc--; argv++;
            if (argc <= 0) {
               fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                     "-j2p6_cmd");
               fprintf(stderr, "\n\n");
               Usage(progName);
            }
            len = strlen(*argv);
            if (**argv == '"' && (*argv)[len-1] == '"') {
               if (len-3 >= MAXSTRING) {
                  fprintf(stderr, TgLoadString(STID_CMDLINE_ARG_GIVEN_TOO_LONG),
                        "-j2p6_cmd");
                  fprintf(stderr, "\n");
               } else {
                  strcpy(cmdLineJpegToPpm6Cmd, &(*argv)[1]);
                  cmdLineJpegToPpm6Cmd[strlen(cmdLineJpegToPpm6Cmd)-1] = '\0';
               }
            } else {
               if (len-1 >= MAXSTRING) {
                  fprintf(stderr, TgLoadString(STID_CMDLINE_ARG_GIVEN_TOO_LONG),
                        "-j2p6_cmd");
                  fprintf(stderr, "\n");
               } else {
                  strcpy(cmdLineJpegToPpm6Cmd, *argv);
                  cmdLineJpegToPpm6Cmd[strlen(cmdLineJpegToPpm6Cmd)-1] = '\0';
               }
            }
         } else if (strcmp(*argv, "-patterndir") == 0) {
            argc--; argv++;
            if (argc <= 0) {
               fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                     "-patterndir");
               fprintf(stderr, "\n\n");
               Usage(progName);
            }
            UtilStrCpyN(cmdLineCustomPatternDir,
                  sizeof(cmdLineCustomPatternDir), *argv);
            UtilTrimBlanks(cmdLineCustomPatternDir);
            len = strlen(cmdLineCustomPatternDir);
            if (len > 0 && cmdLineCustomPatternDir[len-1] == DIR_SEP) {
               cmdLineCustomPatternDir[len-1] = '\0';
            }
            if (*cmdLineCustomPatternDir == '\0' ||
                  !UtilIsDirectory(cmdLineCustomPatternDir)) {
               fprintf(stderr, TgLoadString(STID_PATTERNDIR_NOT_EXIST),
                     cmdLineCustomPatternDir);
               fprintf(stderr, "\n");
               *cmdLineCustomPatternDir = '\0';
            }
         } else if (strncmp(*argv, "-o", 2) == 0) {
            strcpy(outputDir, &(*argv)[2]);
         } else if (strcmp(*argv, "-prtgif") != 0 &&
               strcmp(*argv, "-print") != 0 &&
               strcmp(*argv, "-PRINT") != 0) {
            strcat(c_ptr++, " ");
            strcat(c_ptr, *argv);
            c_ptr += strlen(*argv);
         }
      } else {
         break;
      }
   }
   if (cmdLineOneFilePerPage) {
      if ((!cmdLineOpenDisplay && whereToPrint == PRINTER) ||
            (cmdLineOpenDisplay && cmdLineWhereToPrint == INVALID)) {
         fprintf(stderr, TgLoadString(STID_PRINT_TO_PRINTER_INCOMPAT),
               "-one_file_page_page");
         fprintf(stderr, "\n\n");
         Usage(progName);
      }
   }
   return argc;
}

int ProcessTgifOptions(argc, argv, file_name, name_sz)
   int argc, name_sz;
   char *argv[], *file_name;
   /* All these strangeness with strings are related to */
   /*	Prolog's foreign function interface. */
{
   int i=0, len=0;

   *file_name = '\0';
   *cmdLineTmpFileModeStr = '\0';
   *cmdLineGenParser = '\0';
   *cmdLineFileToExec = '\0';
   *cmdLineRMCastLibDir = *cmdLineRMCastLibPath = '\0';
   *cmdLineCustomPatternDir = '\0';
   cmdLineMergeArgv = NULL;
   for (i=1; i < argc; i++) {
      if (strcmp(argv[i], "-display") == 0) {
         if (++i == argc) {
            fprintf(stderr, "%s\n\n", TgLoadString(STID_MISSING_DISPLAY_NAME));
            Usage(progName);
         }
         if ((displayName=UtilStrDup(argv[i])) == NULL) FailAllocMessage();
      } else if (strcmp(argv[i], "-tgrm2") == 0) {
         cmdLineTgrm2 = TRUE;
      } else if (strcmp(argv[i], "-rmcastlibdir") == 0) {
         if (++i == argc) {
            fprintf(stderr, "%s\n\n", TgLoadString(STID_MISSING_RMCASTDIR));
            Usage(progName);
         }
         UtilStrCpyN(cmdLineRMCastLibDir, sizeof(cmdLineRMCastLibDir), argv[i]);
      } else if (strcmp(argv[i], "-rmcastlibpath") == 0) {
         if (++i == argc) {
            fprintf(stderr, "%s\n\n", TgLoadString(STID_MISSING_RMCASTLIB));
            Usage(progName);
         }
         UtilStrCpyN(cmdLineRMCastLibPath, sizeof(cmdLineRMCastLibPath),
               argv[i]);
      } else if (strcmp(argv[i], "-tgwb2") == 0) {
         cmdLineWb2 = TRUE;
      } else if (strcmp(argv[i], "-bw") == 0) {
         cmdLineBW = TRUE;
      } else if (strcmp(argv[i], "-parent") == 0) {
         if (++i == argc) {
            fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                  "-parent");
            fprintf(stderr, "\n\n");
            Usage(progName);
         }
         if (strncmp(argv[i], "0x", 2) == 0) {
            if (sscanf(&(argv[i])[2], "%x", &cmdLineParentWindowID) != 1) {
               fprintf(stderr, TgLoadString(STID_FAIL_TO_PARSE_FOR_A_VAL),
                     argv[i]);
               fprintf(stderr, "\n\n");
               Usage(progName);
            }
         } else {
            if (sscanf(argv[i], "%d", &cmdLineParentWindowID) != 1) {
               fprintf(stderr, TgLoadString(STID_FAIL_TO_PARSE_FOR_A_VAL),
                     argv[i]);
               fprintf(stderr, "\n\n");
               Usage(progName);
            }
         }
      } else if (strcmp(argv[i], "-reqcolor") == 0) {
         cmdLineRequestedColor = TRUE;
      } else if (strcmp(argv[i], "-rv") == 0) {
         cmdLineRV = TRUE;
      } else if (strcmp(argv[i], "-nv") == 0) {
         cmdLineRV = FALSE;
      } else if (strcmp(argv[i], "-cwo") == 0) {
         cmdLineCWO = TRUE;
         cmdLineBtn1Warp = FALSE;
      } else if (strcmp(argv[i], "-cwo+sbwarp") == 0) {
         cmdLineCWO = TRUE;
         cmdLineBtn1Warp = TRUE;
      } else if (strcmp(argv[i], "-hyper") == 0) {
         inHyperSpace = TRUE;
      } else if (strcmp(argv[i], "-exec") == 0) {
         if (++i == argc) {
            fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                  "-exec");
            fprintf(stderr, "\n\n");
            Usage(progName);
         }
         UtilStrCpyN(cmdLineFileToExec, sizeof(cmdLineFileToExec), argv[i]);
         UtilTrimBlanks(cmdLineFileToExec);
         if (*cmdLineFileToExec == '-' && cmdLineFileToExec[1] != '\0') {
            fprintf(stderr, "%s\n\n", TgLoadString(STID_BAD_EXEC_FNAME_DASH));
            Usage(progName);
         }
      } else if (strcmp(argv[i], "-dbim") == 0) {
         if (++i == argc) {
            fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                  "-dbim");
            fprintf(stderr, "\n\n");
            Usage(progName);
         }
         if (cmdLineHasInputMethod) {
            fprintf(stderr, "%s\n\n", TgLoadString(STID_ONLYONE_DBIM_OR_SBIM));
            Usage(progName);
         }
         strcpy(cmdLineInputMethod, argv[i]);
         cmdLineHasInputMethod = TRUE;
         cmdLineDoubleByteInputMethod = TRUE;
      } else if (strcmp(argv[i], "-sbim") == 0) {
         if (++i == argc) {
            fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                  "-sbim");
            fprintf(stderr, "\n\n");
            Usage(progName);
         }
         if (cmdLineHasInputMethod) {
            fprintf(stderr, "%s\n\n", TgLoadString(STID_ONLYONE_DBIM_OR_SBIM));
            Usage(progName);
         }
         strcpy(cmdLineInputMethod, argv[i]);
         cmdLineHasInputMethod = TRUE;
         cmdLineDoubleByteInputMethod = FALSE;
      } else if (strcmp(argv[i], "-usexlib") == 0) {
         cmdLineUseXlib = TRUE;
      } else if (strcmp(argv[i], "-merge") == 0) {
         cmdLineMerge = TRUE;
      } else if (strcmp(argv[i], "-a4") == 0) {
         cmdLineA4 = TRUE;
      } else if (strcmp(argv[i], "-letter") == 0) {
         cmdLineLetter = TRUE;
#ifdef NOT_DEFINED
      } else if (strcmp(argv[i], "-listen") == 0) {
         cmdLineWhiteBoardListenOnly = TRUE;
#endif /* NOT_DEFINED */
      } else if (strcmp(argv[i], "-no-wb") == 0) {
         cmdLineNoWhiteBoard = TRUE;
      } else if (strcmp(argv[i], "-no-chat") == 0) {
         cmdLineNoChatWindow = TRUE;
      } else if (strcmp(argv[i], "-chatnick") == 0) {
         if (++i == argc) {
            fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                  "-chatnick");
            fprintf(stderr, "\n\n");
            Usage(progName);
         }
         cmdLineChatNickName = UtilStrDup(argv[i]);
         if (cmdLineChatNickName == NULL) FailAllocMessage();
      } else if (strcmp(argv[i], "-fg") == 0) {
         if (++i == argc) {
            fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                  "-fg");
            fprintf(stderr, "\n\n");
            Usage(progName);
         }
         cmdLineForeground = (char*)malloc(80*sizeof(char));
         if (cmdLineForeground == NULL) FailAllocMessage();
         strcpy(cmdLineForeground, argv[i]);
      } else if (strcmp(argv[i], "-bg") == 0) {
         if (++i == argc) {
            fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                  "-bg");
            fprintf(stderr, "\n\n");
            Usage(progName);
         }
         cmdLineBackground = (char*)malloc(80*sizeof(char));
         if (cmdLineBackground == NULL) FailAllocMessage();
         strcpy(cmdLineBackground, argv[i]);
      } else if (strcmp(argv[i], "-bd") == 0) {
         if (++i == argc) {
            fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                  "-bd");
            fprintf(stderr, "\n\n");
            Usage(progName);
         }
         cmdLineBorder = (char*)malloc(80*sizeof(char));
         if (cmdLineBorder == NULL) FailAllocMessage();
         strcpy(cmdLineBorder, argv[i]);
      } else if (strcmp(argv[i], "-geometry") == 0) {
         if (++i == argc) {
            fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                  "-geometry");
            fprintf(stderr, "\n\n");
            Usage(progName);
         }
         strcat(geometrySpec, argv[i]);
         geometrySpecified = TRUE;
      } else if (strcmp(argv[i], "-listdontreencode") == 0) {
#ifdef _DONT_REENCODE
         fprintf(stderr, "-D_DONT_REENCODE=\"%s\"\n", _DONT_REENCODE);
#endif /* _DONT_REENCODE */
      } else if (strcmp(argv[i], "-version") == 0 ||
            strcmp(argv[i], "--version") == 0) {
         cmdLineVersion = TRUE;
      } else if (strcmp(argv[i], "-nomode") == 0) {
         cmdLineNoModeWindow = TRUE;
      } else if (strcmp(argv[i], "-pdfspd") == 0) {
         cmdLinePdfSetPageDevice = TRUE;
      } else if (strncmp(argv[i], "-pdfspd=", 8) == 0) {
         char tmp_str[80];

         UtilStrCpyN(tmp_str, sizeof(tmp_str), &(argv[i])[8]);
         UtilTrimBlanks(tmp_str);
         if (UtilStrICmp(tmp_str, "true") == 0) {
            cmdLinePdfSetPageDevice = TRUE;
         } else if (UtilStrICmp(tmp_str, "false") == 0) {
            cmdLinePdfSetPageDevice = FALSE;
         } else {
            fprintf(stderr, TgLoadString(STID_BAD_CMDLINE_OPT_IGNORED),
                  "-pdfspd", tmp_str);
            fprintf(stderr, "\n");
         }
      } else if (strcmp(argv[i], "-pssetup") == 0) {
         i++;
         if (i >= argc) {
            fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                  "-pssetup");
            fprintf(stderr, "\n\n");
            Usage(progName);
         }
         cmdLinePsSetup = TRUE;
         SetPsSetup(argv[i]);
      } else if (strcmp(*argv, "-patterndir") == 0) {
         argc--; argv++;
         if (argc <= 0) {
            fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                  "-patterndir");
            fprintf(stderr, "\n\n");
            Usage(progName);
         }
         UtilStrCpyN(cmdLineCustomPatternDir,
               sizeof(cmdLineCustomPatternDir), *argv);
         UtilTrimBlanks(cmdLineCustomPatternDir);
         len = strlen(cmdLineCustomPatternDir);
         if (len > 0 && cmdLineCustomPatternDir[len-1] == DIR_SEP) {
            cmdLineCustomPatternDir[len-1] = '\0';
         }
         if (*cmdLineCustomPatternDir == '\0' ||
               !UtilIsDirectory(cmdLineCustomPatternDir)) {
            fprintf(stderr, TgLoadString(STID_PATTERNDIR_NOT_EXIST),
                  cmdLineCustomPatternDir);
            fprintf(stderr, "\n");
            *cmdLineCustomPatternDir = '\0';
         }
      } else if (argv[i][0] == '=') {
         strcpy(geometrySpec, argv[i]);
         geometrySpecified = TRUE;
      } else if (argv[i][0] != '-') {
         if (cmdLineMerge) {
            int j=0;

            cmdLineMergeArgc = argc-i;
            cmdLineMergeArgv = (char**)malloc(cmdLineMergeArgc*sizeof(char*));
            if (cmdLineMergeArgv == NULL) FailAllocMessage();
            for (j=i; j < argc; j++) {
               if (*argv[j] == '-') {
                  Usage(progName);
               }
               cmdLineMergeArgv[j-i] = UtilStrDup(argv[j]);
               if (cmdLineMergeArgv[j-i] == NULL) FailAllocMessage();
            }
            break;
         } else {
            UtilStrCpyN(file_name, name_sz, argv[i]);
         }
      } else {
#ifndef USE_XT_INITIALIZE
         Usage(progName);
#else /* USE_XT_INITIALIZE */
         if (strcmp(argv[i], "-xrm") == 0) {
            if (++i == argc) {
               fprintf(stderr, TgLoadString(STID_MISSING_ARG_FOR_CMDLINE_OPT),
                     "-xrm");
               fprintf(stderr, "\n");
            }
         }
#endif /* ~USE_XT_INITIALIZE */
      }
   }
   return TRUE;
}
