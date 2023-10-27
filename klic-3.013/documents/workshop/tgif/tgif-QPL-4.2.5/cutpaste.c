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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/cutpaste.c,v 1.63 2011/06/25 22:36:42 cvsps Exp $
 */

#define _INCLUDE_FROM_CUTPASTE_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "arc.e"
#include "auxtext.e"
#include "attr.e"
#include "box.e"
#include "choice.e"
#include "choose.e"
#include "cmd.e"
#include "color.e"
#ifndef _NO_KINPUT
#include "convkinput.e"
#endif /* ~_NO_KINPUT */
#include "cutpaste.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "file.e"
#include "font.e"
#include "grid.e"
#include "import.e"
#include "ini.e"
#include "mainloop.e"
#include "mark.e"
#include "menu.e"
#include "miniline.e"
#include "move.e"
#include "msg.e"
#include "names.e"
#ifndef _NO_NKF
#include "nkf.e"
#endif /* ~_NO_NKF */
#include "obj.e"
#include "oval.e"
#include "page.e"
#include "pattern.e"
#include "poly.e"
#include "polygon.e"
#include "raster.e"
#include "rcbox.e"
#include "remote.e"
#include "select.e"
#include "setup.e"
#include "special.e"
#include "stretch.e"
#include "strtbl.e"
#include "text.e"
#include "util.e"
#include "xbitmap.e"
#include "xpixmap.e"

int	copyingToCutBuffer=FALSE;
int	pastingFile=FALSE;

char	gszFilesIniFile[MAXPATHLENGTH];
/* do not translate -- program constants */
char	*gpszRecentFilesSec="Recent Files";
char	*gpszRecentFilesCountKey="Count";

int	 cutBufferIsTgifObj=FALSE;
int	 cutBufferIsUTF8=FALSE;
struct DynStrRec dsCutBuffer;

Time	startSelectionOwnershipTime=(Time)0;
Time	endSelectionOwnershipTime=(Time)0;
int	startSelectionOwnershipTimeValid=FALSE;
int	endSelectionOwnershipTimeValid=FALSE;

SetCutBufferInfo gSetCutBufferInfo;

static int pasteFromXSelectionOnly=TRUE;
static int pasteFromSelectionTimeout=10;

static Atom gaCutBufferAtom[] = { XA_CUT_BUFFER0, XA_CUT_BUFFER1,
   XA_CUT_BUFFER2, XA_CUT_BUFFER3, XA_CUT_BUFFER4, XA_CUT_BUFFER5,
   XA_CUT_BUFFER6, XA_CUT_BUFFER7, (Atom)0 };

static Atom gaAllSelectionAtom[] = { 
   XA_PRIMARY,
   /*
    * It does not appear that you need to try anything other than XA_PRIMARY.
    * 
    * XA_CUT_BUFFER0, XA_CUT_BUFFER1, XA_CUT_BUFFER2, XA_CUT_BUFFER3,
    * XA_CUT_BUFFER4, XA_CUT_BUFFER5, XA_CUT_BUFFER6, XA_CUT_BUFFER7,
    */
   (Atom)0 };

static
void ResetCutBufferInfo()
{
   UtilFree(gSetCutBufferInfo.tgif_dyn_str.s);
   UtilFree(gSetCutBufferInfo.utf8_dyn_str.s);
   UtilFree(gSetCutBufferInfo.text_dyn_str.s);
   memset(&gSetCutBufferInfo, 0, sizeof(SetCutBufferInfo));
}

static
void SetCutBuffer(buf, bytes_to_write, buf_is_simple_string, buf_is_utf8_string)
   char *buf;
   int bytes_to_write, buf_is_simple_string, buf_is_utf8_string;
   /*
    * if buf_is_simple_string is TRUE, this is for text
    * otherwise, it's an object
    */
{
   FreeDynStrBuf(&dsCutBuffer);
   memset(&dsCutBuffer, 0, sizeof(dsCutBuffer));

   dsCutBuffer.s = (char*)malloc((bytes_to_write+1)*sizeof(char));
   if (dsCutBuffer.s == NULL) FailAllocMessage();
   memcpy(dsCutBuffer.s, buf, bytes_to_write);
   dsCutBuffer.s[bytes_to_write] = '\0';
   dsCutBuffer.sz = bytes_to_write+1;

   cutBufferIsTgifObj = (buf_is_simple_string ? FALSE : TRUE);
   cutBufferIsUTF8 = buf_is_utf8_string;

   if (buf_is_simple_string) {
      if (buf_is_utf8_string) {
         DynStrSet(&gSetCutBufferInfo.utf8_dyn_str, dsCutBuffer.s);
         gSetCutBufferInfo.utf8_valid = TRUE;
      } else {
         DynStrSet(&gSetCutBufferInfo.text_dyn_str, dsCutBuffer.s);
         gSetCutBufferInfo.text_valid = TRUE;
      }
   } else {
      DynStrSet(&gSetCutBufferInfo.tgif_dyn_str, dsCutBuffer.s);
      gSetCutBufferInfo.tgif_valid = TRUE;
   }
}

#ifdef _CALL_XSTOREBYTES_ALSO
static
int WriteOldStyleCutBuffer()
{
   int xstorebytes_failed=FALSE;

   copyingToCutBuffer = TRUE;
   XRotateBuffers(mainDisplay, 1);
   XStoreBytes(mainDisplay, dsCutBuffer.s, dsCutBuffer.sz-1);

   XSync(mainDisplay, False);
   if (copyingToCutBuffer == INVALID) {
      xstorebytes_failed = TRUE;
   }
   copyingToCutBuffer = FALSE;
   return (!xstorebytes_failed);
}
#endif /* _CALL_XSTOREBYTES_ALSO */

int WriteBufToCutBuffer(buf, bytes_to_write, buf_is_simple_string,
      buf_is_utf8_string, pscbi)
   char *buf;
   int bytes_to_write, buf_is_simple_string, buf_is_utf8_string;
   SetCutBufferInfo *pscbi;
   /*
    * if buf_is_simple_string is TRUE, this is for text
    * otherwise, it's an object
    */
{
   int copy_failed=FALSE, setselowner_failed=FALSE;

   ClearSelection();
   SetCutBuffer(buf, bytes_to_write, buf_is_simple_string, buf_is_utf8_string);
#ifdef _CALL_XSTOREBYTES_ALSO
   if (!WriteOldStyleCutBuffer()) {
      ClearSelection();
   }
#endif /* _CALL_XSTOREBYTES_ALSO */
   /*
    * If use gSelectionMainAtom here, on Fedora 11, pasting anything copied
    *       from other tools ends up pasting from tgif!
    * So, XA_PRIMARY is hard coded and gSelectionMainAtom is not used.
    */
   XSetSelectionOwner(mainDisplay, XA_PRIMARY, mainWindow,
         lastKeyOrBtnEvInfo.time);
   if (XGetSelectionOwner(mainDisplay, XA_PRIMARY) != mainWindow) {
      setselowner_failed = TRUE;
      sprintf(gszMsgBox, TgLoadString(STID_CANT_ACQUIRE_X_SELECTION));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else {
      startSelectionOwnershipTimeValid = TRUE;
      endSelectionOwnershipTimeValid = FALSE;
      startSelectionOwnershipTime = lastKeyOrBtnEvInfo.time;
      endSelectionOwnershipTime = (Time)0;
   }
   return (!copy_failed);
}

void ClearSelection()
{
   /*
    * XStoreBytes(mainDisplay, NULL, 0);
    * XSetSelectionOwner(mainDisplay, XA_PRIMARY, None,
    *       lastKeyOrBtnEvInfo.time);
    */

   /*
    * Does not appear to need to do this:
    * 
    * int i=0;
    * 
    * for (i=0; gaAllSelectionAtom[i] != (Atom)0; i++) {
    *    XSetSelectionOwner(mainDisplay, gaAllSelectionAtom[i], None,
    *          lastKeyOrBtnEvInfo.time);
    * }
    */
}

static
int CopyObjectToCutBuffer(force)
   int force;
   /* if force == TRUE, copy the TGIF object no matter what */
   /* if force == FALSE, do not copy if the object is a simple text object, */
   /*    i.e., all with same font, size, color, etc */
   /* returns FALSE if copying in text mode -- this is */
   /*    interpreted as an attempt to copy highlighted text */
{
   FILE *fp=NULL;
   char tmpfile[MAXSTRING], *cut_buffer=NULL;
   struct stat stat;
   unsigned char header=TGIF_HEADER;
   struct ObjRec *partial_text_obj_ptr=NULL;

   if (curChoice == DRAWTEXT) {
      if (!textHighlight) {
         return FALSE;
      } else {
         /* create a partial text object with only the minilines */
         partial_text_obj_ptr = CreateTextObjFromHighLightedText();
         if (partial_text_obj_ptr == NULL) {
            return FALSE;
         }
         if (!force) {
            struct TextRec *text_ptr=partial_text_obj_ptr->detail.t;
            MiniLinesInfo *minilines=(&text_ptr->minilines);
            MiniLineInfo *pMiniLine=minilines->first;
            StrBlockInfo *pStrBlock=pMiniLine->first_block;
            StrSegInfo *pStrSeg=pStrBlock->seg;
            int sz_unit=pStrSeg->sz_unit, double_byte=pStrSeg->double_byte, font=pStrSeg->font, style=pStrSeg->style;
            int underline_on=pStrSeg->underline_on, overline_on=pStrSeg->overline_on;
            int color_index=INVALID;

            if (SingleFontText(text_ptr, &sz_unit, &double_byte, &font, &style,
                  &underline_on, &overline_on) &&
                  SingleColorText(text_ptr, &color_index)) {
               /* since it's a simple text object, don't copy it as a TGIF object */
               return FALSE;
            }
         }
      }
   } else if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return TRUE;
   }
   if (MkTempFile(tmpfile, sizeof(tmpfile), tmpDir, TOOL_NAME) == NULL) {
      return TRUE;
   }
   // Naehring: added b to the mode string.
   if ((fp=fopen(tmpfile, "wb+")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            tmpfile);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return TRUE;
   }
   writeFileFailed = FALSE;
   if (write(fileno(fp), (char*)(&header), 1) < 1) writeFileFailed = TRUE;

   if (curChoice == DRAWTEXT) {
      /* create a partial text object with only the minilines */
      copyInDrawTextMode = TRUE;
      Save(fp, partial_text_obj_ptr, 0, 1);
      copyInDrawTextMode = FALSE;

      FreeObj(partial_text_obj_ptr);
   } else {
      struct SelRec *sel_ptr=NULL;
      struct ObjRec *top_obj=NULL, *bot_obj=NULL;

      for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
         struct ObjRec *obj_ptr=DupObj(sel_ptr->obj);

         obj_ptr->prev = NULL;
         obj_ptr->next = top_obj;

         if (top_obj == NULL) {
            bot_obj = obj_ptr;
         } else {
            top_obj->prev = obj_ptr;
         }
         top_obj = obj_ptr;
      }
      Save(fp, bot_obj, 0, 1);
      while (top_obj != NULL) {
         struct ObjRec *obj_ptr=top_obj->next;

         FreeObj(top_obj);
         top_obj = obj_ptr;
      }
   }
   if (writeFileFailed) {
      FailToWriteFileMessage(tmpfile);
      fclose(fp);
      unlink(tmpfile);
      writeFileFailed = FALSE;
      return TRUE;
   }
   fflush(fp);
   if (fstat(fileno(fp), &stat) < 0) {
      fclose(fp);
      unlink(tmpfile);
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_FSTAT_ABORT_COPY), tmpfile);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return TRUE;
   }

   if (cut_buffer != NULL) free(cut_buffer);
   cut_buffer = (char*)malloc((stat.st_size+1)*sizeof(char));
   if (cut_buffer == NULL) FailAllocMessage();

   rewind(fp);
   if (read(fileno(fp), cut_buffer, stat.st_size) < stat.st_size) {
      sprintf(gszMsgBox, TgLoadString(STID_ERR_READING_FILE_COPY_ABORT),
            tmpfile);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else {
      if (!WriteBufToCutBuffer(cut_buffer, stat.st_size, FALSE, FALSE, NULL)) {
         sprintf(gszMsgBox, TgLoadString(STID_COPY_FAILED_OBJ_MAYBE_TOO_BIG));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_COPY_BUFFER_UPDATED));
         Msg(gszMsgBox);
      }
   }
   fclose(fp);
   unlink(tmpfile);
   UtilFree(cut_buffer);

   return (curChoice != DRAWTEXT);
}

int CopyToCutBuffer()
   /* returns FALSE if copying in text mode -- this is */
   /*    interpreted as an attempt to copy highlighted text */
{
   int rc=TRUE, handle_edit_text_size=FALSE;

   ResetCutBufferInfo();
   if (curChoice == DRAWTEXT) {
      if (escPressed) {
         escPressed = FALSE;
         Msg(TgLoadString(STID_ESC_KEY_PRESS_IGNORED));
      }
      if (!textHighlight) {
         MsgBox(TgLoadString(STID_NO_TEXT_SELECTED_FOR_COPY), TOOL_NAME,
               INFO_MB);
         return FALSE;
      } else {
         if (editTextSize != 0) {
            handle_edit_text_size = TRUE;
            if (RestoreEditTextSize(curTextObj, TRUE)) {
               UpdTextBBox(curTextObj);
            }
         }
         if (CanCopyHighLightedTextAsUTF8Strings(NULL)) {
            XEvent ev;

            if (handle_edit_text_size && editTextSize != 0) {
               if (RestoreEditTextSize(curTextObj, FALSE)) {
                  UpdTextBBox(curTextObj);
               }
            }
            copyInDrawTextMode = TRUE;
            copyDoubleByteStringInDrawTextMode = TRUE;
            ev.type = KeyPress;
            DrawText(&ev);
            copyDoubleByteStringInDrawTextMode = FALSE;
            copyInDrawTextMode = FALSE;
         } else if (CanCopyHighLightedTextAsStrings()) {
            XEvent ev;

            if (handle_edit_text_size && editTextSize != 0) {
               if (RestoreEditTextSize(curTextObj, FALSE)) {
                  UpdTextBBox(curTextObj);
               }
            }
            copyInDrawTextMode = TRUE;
            ev.type = KeyPress;
            DrawText(&ev);
            copyInDrawTextMode = FALSE;
         } else {
#ifdef NOT_DEFINED
            char *cut_buffer=NULL;
            int cut_buffer_size=0;

            GatherHighLightedTextAsStrings(&cut_buffer, &cut_buffer_size);
            if (cut_buffer == NULL) {
               ClearSelection();
            } else {
               if (WriteBufToCutBuffer(cut_buffer, cut_buffer_size-1, TRUE,
                     FALSE, NULL)) {
                  sprintf(gszMsgBox, TgLoadString(STID_COPY_BUFFER_UPDATED));
               } else {
                  sprintf(gszMsgBox, TgLoadString(STID_COPY_FAIL_SEL_STR_MAY_TOO_LNG));
               }
               Msg(gszMsgBox);
               free(cut_buffer);
            }
#endif /* NOT_DEFINED */
            /*
             * create a partial text object with only the minilines, this
             *       will be done in CopyObjectToCutBuffer() below
             */
         }
      }
   } else if (topSel == NULL) {
      MsgBox(TgLoadString(STID_NO_OBJ_SELECTED_FOR_COPY), TOOL_NAME, INFO_MB);
      return TRUE;
   }
   rc = CopyObjectToCutBuffer(FALSE);
   if (handle_edit_text_size && editTextSize != 0) {
      if (RestoreEditTextSize(curTextObj, FALSE)) {
         UpdTextBBox(curTextObj);
      }
   }
   return rc;
}

int CopyPlainTextAsObject()
   /* returns FALSE if copying in text mode -- this is */
   /*    interpreted as an attempt to copy highlighted text */
{
   if (!(curChoice == DRAWTEXT && textHighlight)) {
      return (curChoice != DRAWTEXT);
   }
   return CopyObjectToCutBuffer(TRUE);
}

void CutToCutBuffer()
{
   if (curChoice == DRAWTEXT && textCursorShown) {
      CopyToCutBuffer();
      DelSelText();
   }
   if (curChoice == NOTHING && topSel != NULL) {
      CopyToCutBuffer();
      DelAllSelObj();
   }
}

unsigned int PasteString(CutBuffer, highlight, record_cmd)
   char *CutBuffer;
   int highlight, record_cmd;
{
   char *c_ptr=NULL, *dest_c_ptr=NULL;
   int x, y, num_lines=0, char_count, root_x, root_y, grid_x, grid_y;
   unsigned int status=0, button_pressed=0;
   struct ObjRec *obj_ptr=NULL;
   struct TextRec *text_ptr=NULL;
   Window root_win=None, child_win=None;
   MiniLineInfo *pFirstMiniLine=NULL, *pLastMiniLine=NULL;

   if (*CutBuffer == '\0') {
      MsgBox(TgLoadString(STID_CUT_BUFFER_EMPTY), TOOL_NAME, INFO_MB);
      return 0;
   }
   TieLooseEnds();
   SetCurChoice(NOTHING);
   if (topSel!=NULL) { HighLightReverse(); RemoveAllSel(); }

   XQueryPointer(mainDisplay, drawWindow, &root_win, &child_win,
         &root_x, &root_y, &x, &y, &status);
   GridXY(x, y, &grid_x, &grid_y);

   obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (obj_ptr == NULL) FailAllocMessage();
   memset(obj_ptr, 0, sizeof(struct ObjRec));

   text_ptr = (struct TextRec *)malloc(sizeof(struct TextRec));
   if (text_ptr == NULL) FailAllocMessage();
   memset(text_ptr, 0, sizeof(struct TextRec));

   for (c_ptr=CutBuffer, num_lines = 0; *c_ptr != '\0'; num_lines++) {
      MiniLineInfo *pMiniLine=NULL;
      if (lengthLimit256InInsertChar) {
         char s[MAXSTRING+1];

         char_count = 0;
         dest_c_ptr = s;
         while (*c_ptr != '\0' && *c_ptr != '\n' && *c_ptr != '\r') {
            *dest_c_ptr++ = *c_ptr++;
            if (++char_count == MAXSTRING) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_STRING_LEN_EXCEEDS_AND_TRUNC),
                     MAXSTRING);
               Msg(gszMsgBox);
               while (*c_ptr != '\0' && *c_ptr != '\n' && *c_ptr != '\r') {
                  c_ptr++;
               }
               break;
            }
         }
         *dest_c_ptr = '\0';
         pMiniLine = CreateMiniLineFromString(s, &pFirstMiniLine,
               &pLastMiniLine);
      } else {
         unsigned char *psz_CR=(unsigned char *)strchr(c_ptr, '\r');
         unsigned char *psz_LF=(unsigned char *)strchr(c_ptr, '\n');
         int use_CR=FALSE, use_LF=FALSE;

         if (psz_CR == NULL) {
            if (psz_LF != NULL) {
               use_LF = TRUE;
            }
         } else if (psz_CR == NULL) {
            use_CR = TRUE;
         } else if (psz_CR > psz_LF) {
            use_LF = TRUE;
         } else {
            use_CR = TRUE;
         }
         if (use_CR) {
            *psz_CR = '\0';
            pMiniLine = CreateMiniLineFromString(c_ptr, &pFirstMiniLine,
                  &pLastMiniLine);
            *psz_CR = '\n';
            c_ptr = (char*)psz_CR;
         } else if (use_LF) {
            *psz_LF = '\0';
            pMiniLine = CreateMiniLineFromString(c_ptr, &pFirstMiniLine,
                  &pLastMiniLine);
            *psz_LF = '\r';
            c_ptr = (char*)psz_LF;
         } else {
            pMiniLine = CreateMiniLineFromString(c_ptr, &pFirstMiniLine,
                  &pLastMiniLine);
            c_ptr += strlen(c_ptr);
         }
      }
      if (*c_ptr == '\n') {
         c_ptr++;
         if (c_ptr[1] == '\r') c_ptr++;
      } else if (*c_ptr == '\r') {
         c_ptr++;
         if (c_ptr[1] == '\n') c_ptr++;
      }
   }
   text_ptr->lines = num_lines;
   text_ptr->minilines.first = pFirstMiniLine;
   text_ptr->minilines.last = pLastMiniLine;
   text_ptr->baseline_y = grid_y+pFirstMiniLine->asc;

   CopyCurInfoIntoTextPtr(obj_ptr, text_ptr);

   obj_ptr->x = grid_x;
   obj_ptr->y = grid_y;
   obj_ptr->type = OBJ_TEXT;
   obj_ptr->color = colorIndex;
   if (mainDisplay != NULL) {
      UtilStrCpyN(obj_ptr->color_str, sizeof(obj_ptr->color_str),
            colorMenuItems[colorIndex]);
   }
   obj_ptr->id = objId++;;
   obj_ptr->dirty = FALSE;
   obj_ptr->rotation = 0;
   obj_ptr->detail.t = text_ptr;
   obj_ptr->fattr = obj_ptr->lattr = NULL;
   obj_ptr->ctm = NULL;

   RecalcTextMetrics(text_ptr, grid_x, text_ptr->baseline_y);
   UpdTextBBox(obj_ptr);
   AddObj(NULL, topObj, obj_ptr);
   button_pressed = PlaceTopObj(obj_ptr, NULL, NULL);

   if (highlight) SelectTopObj();
   if (record_cmd) RecordNewObjCmd();
   SetFileModified(TRUE);
   justDupped = FALSE;

   return button_pressed;
}

static
struct ObjRec *CreateTmpBoxObj(LtX, LtY, RbX, RbY)
   int LtX, LtY, RbX, RbY;
{
   register struct BoxRec *box_ptr;
   register struct ObjRec *obj_ptr;

   box_ptr = (struct BoxRec *)malloc(sizeof(struct BoxRec));
   if (box_ptr == NULL) FailAllocMessage();
   memset(box_ptr, 0, sizeof(struct BoxRec));
   box_ptr->fill = NONEPAT;
   box_ptr->width = 0;
   box_ptr->pen = NONEPAT;
   box_ptr->dash = 0;

   obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (obj_ptr == NULL) FailAllocMessage();
   memset(obj_ptr, 0, sizeof(struct ObjRec));

   obj_ptr->bbox.ltx = obj_ptr->obbox.ltx = obj_ptr->x = LtX;
   obj_ptr->bbox.lty = obj_ptr->obbox.lty = obj_ptr->y = LtY;
   obj_ptr->bbox.rbx = obj_ptr->obbox.rbx = RbX;
   obj_ptr->bbox.rby = obj_ptr->obbox.rby = RbY;
   obj_ptr->type = OBJ_BOX;
   obj_ptr->color = colorIndex;
   if (mainDisplay != NULL) {
      UtilStrCpyN(obj_ptr->color_str, sizeof(obj_ptr->color_str),
            colorMenuItems[colorIndex]);
   }
   obj_ptr->id = 0;
   obj_ptr->dirty = FALSE;
   obj_ptr->rotation = 0;
   obj_ptr->detail.b = box_ptr;
   obj_ptr->fattr = obj_ptr->lattr = NULL;
   obj_ptr->ctm = NULL;
   obj_ptr->invisible = FALSE;
   obj_ptr->trans_pat = FALSE;

   return obj_ptr;
}

void AssignNewObjIds(ObjPtr)
   struct ObjRec *ObjPtr;
{
   register struct ObjRec *obj_ptr;
   register struct AttrRec *attr_ptr;

   ObjPtr->id = objId++;
   switch (ObjPtr->type) {
   case OBJ_GROUP:
   case OBJ_SYM:
   case OBJ_ICON:
   case OBJ_PIN:
      for (obj_ptr=ObjPtr->detail.r->first; obj_ptr != NULL;
            obj_ptr=obj_ptr->next) {
         AssignNewObjIds(obj_ptr);
      }
      break;
   default: break;
   }
   for (attr_ptr=ObjPtr->fattr; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
      AssignNewObjIds(attr_ptr->obj);
   }
}

#define CBF_NON_TGIF 0
#define CBF_TGIF_STRING 1
#define CBF_TGIF_OBJ 2

static
int CutBufferType(cut_buffer)
   char *cut_buffer;
{
   unsigned char header=TGIF_HEADER;

   if (((unsigned char)(*cut_buffer)) == header) {
      if (strncmp(&cut_buffer[1], "%TGIF", 5) == 0) {
         return CBF_TGIF_OBJ;
      } else if (strncmp(&cut_buffer[1], "state(", 6) == 0) {
         /* very old tgif format (even before tgif-2.12) */
         return CBF_TGIF_OBJ;
      }
      return CBF_TGIF_STRING;
   }
   return CBF_NON_TGIF;
}

static
int GetObjsFromCutBuffer(cut_buffer, len, pp_top_obj, pp_bot_obj)
   char *cut_buffer;
   int len;
   struct ObjRec **pp_top_obj, **pp_bot_obj;
   /* the cut_buffer is the original cut buffer shifted one byte */
{
   FILE *fp=NULL;
   int read_status=0;
   char tmpfile[MAXSTRING];
   struct ObjRec *obj_ptr=NULL, *saved_top_obj=NULL, *saved_bot_obj=NULL;

   (*pp_top_obj) = (*pp_bot_obj) = NULL;

   if (MkTempFile(tmpfile, sizeof(tmpfile), tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   // Naehring: added b to the mode string.
   if ((fp=fopen(tmpfile, "wb+")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            tmpfile);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   writeFileFailed = FALSE;
   if (write(fileno(fp), cut_buffer, len) < len) {
      fclose(fp);
      unlink(tmpfile);
      sprintf(gszMsgBox, TgLoadString(STID_ERR_WRITING_FILE_PASTE_ABORT),
            tmpfile);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   fflush(fp);
   rewind(fp);

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);

   saved_top_obj = topObj;
   saved_bot_obj = botObj;
   curPage->top = topObj = NULL;
   curPage->bot = botObj = NULL;
    
   importingFile = TRUE;
   pastingFile = TRUE;
   readingPageNum = loadedCurPageNum = 0;
   foundGoodStateObject = FALSE;
   while ((read_status=ReadObj(fp, &obj_ptr)) == TRUE) {
      if (obj_ptr != NULL) {
         AdjForOldVersion(obj_ptr);
         UnlockAnObj(obj_ptr);
         AssignNewObjIds(obj_ptr);
         AddObj(NULL, topObj, obj_ptr);
      }
   }
   fclose(fp);
   if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
      RedrawColorWindow();
   }
   importingFile = FALSE;
   pastingFile = FALSE;
   unlink(tmpfile);
   SetDefaultCursor(mainWindow);
   ShowCursor();

   if (read_status == INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_FILEVER_TOO_LARGE_PASTE_ABORT),
            fileVersion);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   (*pp_top_obj) = topObj;
   (*pp_bot_obj) = botObj;

   curPage->top = topObj = saved_top_obj;
   curPage->bot = botObj = saved_bot_obj;
    
   return TRUE;
}

void FreeSelectionOrCutBuffer(cut_buffer, from_selection)
   char *cut_buffer;
   int from_selection;
{
   if (from_selection) {
      UtilFree(cut_buffer);
   } else {
      if (cut_buffer != NULL) XFree(cut_buffer);
   }
}

char *FetchSelectionOrCutBuffer(pnLen, pnFromSelection)
   int *pnLen, *pnFromSelection;
{
   int len=0;
   unsigned long uLen=0L;
   char *cut_buffer=GetTextBytesFromSelection(FALSE, &uLen);

   *pnLen = 0;
   *pnFromSelection = FALSE;
   if (cut_buffer == NULL || uLen == 0L) {
      if (pasteFromXSelectionOnly) return NULL;

      cut_buffer = (char*)XFetchBytes(mainDisplay, &len);
      if (len == 0) {
         return NULL;
      }
   } else {
      *pnFromSelection = TRUE;
      len = (int)uLen;
   }
   *pnLen = len;

   return cut_buffer;
}

static int pastingUTF8String=FALSE;

int PasteFromCutBuffer()
   /* returns FALSE if pasting in text mode and non-tgif bytes are */
   /*    in the cut buffer -- this is interpreted as an attempt to */
   /*    paste into the current text */
{
   int len=0, ltx=0, lty=0, rbx=0, rby=0, dx=0, dy=0, from_selection=FALSE;
   char *cut_buffer=NULL, *orig_cut_buffer=NULL;
   struct ObjRec *obj_ptr=NULL, *tmp_obj=NULL;
   struct ObjRec *saved_top_obj=NULL, *saved_bot_obj=NULL;
   struct ObjRec *pasted_top_obj=NULL, *pasted_bot_obj=NULL;
   struct ObjRec *tmp_top_obj=NULL, *tmp_bot_obj=NULL;
   StrSegInfo ssi;

   memset(&ssi, 0, sizeof(StrSegInfo));

   if (curChoice == DRAWTEXT && textCursorShown &&
         CanPasteUTF8StringIntoText(&ssi)) {
      pastingUTF8String = TRUE;
   }
   cut_buffer = FetchSelectionOrCutBuffer(&len, &from_selection);
   if (cut_buffer == NULL) {
      MsgBox(TgLoadString(STID_CUT_BUFFER_EMPTY), TOOL_NAME, INFO_MB);
      pastingUTF8String = FALSE;
      return TRUE;
   }
   orig_cut_buffer = cut_buffer;

   if (CutBufferType(cut_buffer) != CBF_TGIF_OBJ) {
      if (curChoice == DRAWTEXT) {
         XEvent ev;

         FreeSelectionOrCutBuffer(cut_buffer, from_selection);
         pasteInDrawTextMode = TRUE;
         ev.type = KeyPress;
         DrawText(&ev);
         pastingUTF8String = FALSE;
         return FALSE;
      }
#ifndef _NO_KINPUT
      if (copyAndPasteJIS) {
         CvtJisToEuc(cut_buffer, cut_buffer);
      }
#endif /* ~_NO_KINPUT */
      Msg(TgLoadString(STID_PASTE_FROM_NON_TGIF));
      PasteString(cut_buffer, TRUE, TRUE);
      FreeSelectionOrCutBuffer(cut_buffer, from_selection);
      pastingUTF8String = FALSE;

      return TRUE;
   }
   if (curChoice == DRAWTEXT) {
      SaveCursorPositionInCurText();
   } else {
      MakeQuiescent();
   }
   if (!GetObjsFromCutBuffer(&cut_buffer[1], len-1, &pasted_top_obj,
         &pasted_bot_obj)) {
      FreeSelectionOrCutBuffer(orig_cut_buffer, from_selection);
      pastingUTF8String = FALSE;
      return TRUE;
   }
   FreeSelectionOrCutBuffer(orig_cut_buffer, from_selection);

   saved_top_obj = topObj;
   saved_bot_obj = botObj;
   curPage->top = topObj = pasted_top_obj;
   curPage->bot = botObj = pasted_bot_obj;
    
   if (curChoice == DRAWTEXT && textCursorShown && topObj != NULL &&
         topObj == botObj && topObj->type == OBJ_TEXT) {
      /* we are pasting minilines and not object */
      struct ObjRec *partial_text_obj_ptr=topObj;

      RestoreCursorPositionInCurText();
      curPage->top = topObj = saved_top_obj;
      curPage->bot = botObj = saved_bot_obj;

      if (curStrBlock->type == SB_SUPSUB_CENTER) {
         strcpy(gszMsgBox, TgLoadString(STID_CANNOT_PASTE_MIXED_TEXT));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else {
         Msg(TgLoadString(STID_STR_BLOCKS_PASTED_FROM_TGIF));
         PasteMiniLinesAtCursor(partial_text_obj_ptr);
      }
      FreeObj(partial_text_obj_ptr);
      pastingUTF8String = FALSE;

      return FALSE;
   }
   if (curChoice == DRAWTEXT) {
      /* we've made a mistake earlier on, should have MakeQuiescent() */
      struct ObjRec *pasted_top_obj=topObj, *pasted_bot_obj=botObj;

      curPage->top = topObj = saved_top_obj;
      curPage->bot = botObj = saved_bot_obj;
      RestoreCursorPositionInCurText();

      MakeQuiescent();

      saved_top_obj = topObj;
      saved_bot_obj = botObj;
      curPage->top = topObj = pasted_top_obj;
      curPage->bot = botObj = pasted_bot_obj;
   }
   if (topObj != NULL) SetFileModified(TRUE);

   ltx = topObj->obbox.ltx;
   lty = topObj->obbox.lty;
   rbx = topObj->obbox.rbx;
   rby = topObj->obbox.rby;
   for (obj_ptr = topObj->next; obj_ptr != NULL; obj_ptr = obj_ptr->next) {
      if (obj_ptr->obbox.ltx < ltx) ltx = obj_ptr->obbox.ltx;
      if (obj_ptr->obbox.lty < lty) lty = obj_ptr->obbox.lty;
      if (obj_ptr->obbox.rbx > rbx) rbx = obj_ptr->obbox.rbx;
      if (obj_ptr->obbox.rby > rby) rby = obj_ptr->obbox.rby;
   }
   tmp_obj = CreateTmpBoxObj(ltx, lty, rbx, rby);

   tmp_top_obj = topObj;
   tmp_bot_obj = botObj;
   curPage->top = topObj = NULL;
   curPage->bot = botObj = NULL;
   PlaceTopObj(tmp_obj, saved_top_obj, saved_bot_obj);
   curPage->top = topObj = tmp_top_obj;
   curPage->bot = botObj = tmp_bot_obj;

   dx = tmp_obj->obbox.ltx - ltx;
   dy = tmp_obj->obbox.lty - lty;
   FreeBoxObj(tmp_obj);

   for (obj_ptr=topObj; obj_ptr != NULL; obj_ptr=obj_ptr->next) {
      MoveObj(obj_ptr, dx, dy);
   }
    
   SelAllObj(FALSE, FALSE);
    
   if (botObj != NULL) {
      botObj->next = saved_top_obj;
   } else {
      curPage->top = topObj = saved_top_obj;
   }

   if (saved_top_obj != NULL) {
      saved_top_obj->prev = botObj;
      curPage->bot = botObj = saved_bot_obj;
   }
   RedrawDrawWindow(botObj);
   PrepareToRecord(CMD_NEW, NULL, NULL, 0);
   RecordCmd(CMD_NEW, NULL, topSel, botSel, numObjSelected);
   HighLightForward();

   Msg(TgLoadString(STID_OBJS_PASTED_FROM_TGIF));
   pastingUTF8String = FALSE;
   return TRUE;
}        

int PasteFromFile()
   /* returns FALSE if pasting in text mode and non-tgif bytes are */
   /*    in the cut buffer -- this is interpreted as an attempt to */
   /*    paste into the current text */
{
   char file_name[MAXPATHLENGTH+1];
   FILE *fp=NULL;
   char inbuf[MAXSTRING+1], *cut_buffer=NULL;
   int size=0, pos=0;

   if (SelectFileNameToPaste(TgLoadString(STID_SEL_A_FILE_TO_PASTE_DOTS),
         file_name) == INVALID) {
      return TRUE;
   } else if (FileIsRemote(file_name)) {
      MsgBox(TgLoadString(STID_PASTING_REMOTE_FILE_NOT_SUP), TOOL_NAME,
            INFO_MB);
      return TRUE;
   }
   if (curChoice == DRAWTEXT) {
      XEvent ev;

      pasteInDrawTextMode = TRUE;
      pasteFromFileInDrawTextMode = TRUE;
      strcpy(pasteFromFileName, file_name);
      ev.type = KeyPress;
      DrawText(&ev);
      return FALSE;
   }
   if ((fp=fopen(file_name, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
            file_name);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return TRUE;
   }
   while (fgets(inbuf, MAXSTRING, fp) != NULL) size += strlen(inbuf);
   fclose(fp);
   if (size == 0) {
      sprintf(gszMsgBox, TgLoadString(STID_NAMED_FILE_IS_EMPTY), file_name);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return TRUE;
   }
   cut_buffer = (char*)malloc((size+2)*sizeof(char));
   if (cut_buffer == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_MALLOC_NUM_BYTES), size+2);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return TRUE;
   }
   if ((fp=fopen(file_name, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
            file_name);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return TRUE;
   }
   while (fgets(&cut_buffer[pos], MAXSTRING, fp) != NULL) {
      pos += strlen(&cut_buffer[pos]);
   }
   fclose(fp);
#ifndef _NO_NKF
   if (Tg_useNKF()) {
      char *out_buffer=(char*)malloc(size+2);

      if (out_buffer == NULL) FailAllocMessage();
      Tg_do_kconv(cut_buffer, out_buffer, size+2);
      PasteString(out_buffer, TRUE, TRUE);
      free(out_buffer);
   } else {
#ifndef _NO_KINPUT
      if (copyAndPasteJIS) {
         CvtJisToEuc(cut_buffer, cut_buffer);
      }
#endif /* ~_NO_KINPUT */
      PasteString(cut_buffer, TRUE, TRUE);
   }
#else /* _NO_NKF */
   PasteString(cut_buffer, TRUE, TRUE);
#endif /* ~_NO_NKF */

   return TRUE;
}

#ifdef NOT_DEFINED
static
int WindowPropertyExists(dpy, win, property)
   Display *dpy;
   Window win;
   Atom property;
{
   Atom type_atom=(Atom)0;
   unsigned long ulBytesLeft=0L, ulLen=0L;
   char *psz=NULL;
   int nFormat=0, rc=FALSE;
   unsigned long ulCurSize=0L, ulTotalLeft=0x10000L;
   int nStatus=XGetWindowProperty(dpy, win, property,
         ulCurSize, ulTotalLeft, False, AnyPropertyType,
         &type_atom, &nFormat, &ulLen, &ulBytesLeft,
         (unsigned char **)(&psz));

   rc = (nStatus != None);

   if (psz != NULL) XFree(psz);

   return rc;
}
#endif /* NOT_DEFINED */

static
char *GetTextBytesFromWindowProperty(win, property, compound_text, pul_len,
      delete_prop)
   Window win;
   Atom property;
   int compound_text, delete_prop;
   unsigned long *pul_len;
{
   int actual_format=0;
   unsigned long long_offset=0L, bytes_after=0L, nitems=0L, total=0L;
   char *cut_buffer=NULL, *psz=NULL;
   Atom actual_type=(Atom)0;

   /*
    * Accorind to the documentation for XGetWindowProperty(), if it is called
    *         with AnyPropertyType, the following is true:
    *     N = actual length of the stored property in bytes
    *         (even if the format is 16 or 32)
    *     I = 4 * long_offset
    *     T = N - I
    *     L = MINIMUM(T, 4 * long_length)
    *     A = N - (I + L)
    * We use a fixed long_legnth of 0x4000L
    * L is nitems
    * A is bytes_after
    */
   if (debugCopyPaste > 1) {
      char *atom_name=NULL;

      atom_name = XGetAtomName(mainDisplay, property);
      fprintf(stderr, "Property name is '%s'.\n", atom_name);
      XFree(atom_name);
   }
   while (XGetWindowProperty(mainDisplay, win, property,
            long_offset, 0x4000L, delete_prop, AnyPropertyType,
            &actual_type, &actual_format, &nitems, &bytes_after,
            (unsigned char **)(&psz)) == Success) {
      if (debugCopyPaste) {
         static int n=0;
         fprintf(stderr, "[%1d] actual_format = %1d, nitems = %1d\n", ++n,
               (int)actual_format, (int)nitems);
      }
      if (psz != NULL && nitems > 0) {
         unsigned int bytes_per_item=(actual_format>>3);
         unsigned int bytes_got=(bytes_per_item==1 ? nitems :
               (bytes_per_item*nitems));

         total += bytes_got;
         if (cut_buffer == NULL) {
            cut_buffer = (char*)malloc(total+1);
         } else {
            cut_buffer = (char*)realloc(cut_buffer, total+1);
         }
         if (cut_buffer == NULL) FailAllocMessage();
         memcpy(&cut_buffer[long_offset<<2], psz, bytes_got);
         cut_buffer[total] = '\0';
         long_offset += (bytes_got>>2);
      }
      if (psz != NULL) XFree(psz);

      if (nitems == 0 || bytes_after == 0L) {
         break;
      }
   }
   *pul_len = total;

   return cut_buffer;
}

static
char *GetTextBytesFromGivenSelection(compound_text, pulLen, which_selection,
      paste_utf8_string, pssi, pn_timeout_msg_displayed)
   int compound_text, which_selection, paste_utf8_string;
   int *pn_timeout_msg_displayed;
   unsigned long *pulLen;
   StrSegInfo *pssi;
   /* the returned string should be freed by calling UtilFree() */
{
   int done=FALSE, need_to_try_text=FALSE, need_to_try_utf8=FALSE;
   char *cut_buffer=NULL;
   time_t tloc, endtime;
   Window selection_owner_win=None;
   XEvent ev;

   selection_owner_win = XGetSelectionOwner(mainDisplay, which_selection);
   if (selection_owner_win == None) {
      return NULL;
   }
   if (compound_text) {
      XConvertSelection(mainDisplay, which_selection, compoundTextAtom,
            tmpSelectionAtom, mainWindow, lastKeyOrBtnEvInfo.time);
   } else {
      if (paste_utf8_string) {
         /* try tgif object first, then try utf8, then try text */
         need_to_try_utf8 = TRUE;
      } else {
         /* try tgif object then try text */
         need_to_try_text = TRUE;
      }
      XConvertSelection(mainDisplay, which_selection, tgifProtocolAtom,
            tmpSelectionAtom, mainWindow, lastKeyOrBtnEvInfo.time);
   }
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev) ||
         XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
   }
   XFlush(mainDisplay);

   time(&tloc);
   endtime = tloc + pasteFromSelectionTimeout;

   SaveStatusStrings();
   sprintf(gszMsgBox, TgLoadString(STID_CONVERTING_GIVEN_DOTS),
         (compound_text ? "COMPOUND_TEXT" : "TEXT"));
   SetStringStatus(gszMsgBox);
   while (!done) {
      if (XPending(mainDisplay)) {
         XNextEvent(mainDisplay, &ev);
         if (ev.type == SelectionNotify) {
            XSelectionEvent *xselectionev=(&ev.xselection);

            if (xselectionev->property == None) {
               /* conversion has been refused */
               if (paste_utf8_string) {
                  if (need_to_try_utf8) {
                     need_to_try_utf8 = FALSE;
                     need_to_try_text = TRUE;
                     XConvertSelection(mainDisplay, which_selection,
                           utf8StringAtom, tmpSelectionAtom, mainWindow,
                           lastKeyOrBtnEvInfo.time);
                     continue;
                  } else if (need_to_try_text) {
                     need_to_try_text = FALSE;
                     XConvertSelection(mainDisplay, which_selection,
                           textAtom, tmpSelectionAtom, mainWindow,
                           lastKeyOrBtnEvInfo.time);
                     continue;
                  }
               } else {
                  if (need_to_try_text) {
                     need_to_try_text = FALSE;
                     need_to_try_utf8 = TRUE;
                     XConvertSelection(mainDisplay, which_selection,
                           textAtom, tmpSelectionAtom, mainWindow,
                           lastKeyOrBtnEvInfo.time);
                     continue;
                  } else if (need_to_try_utf8) {
                     need_to_try_utf8 = FALSE;
                     XConvertSelection(mainDisplay, which_selection,
                           utf8StringAtom, tmpSelectionAtom, mainWindow,
                           lastKeyOrBtnEvInfo.time);
                     continue;
                  }
               }
               if (pasteFromXSelectionOnly) {
                  Msg(TgLoadString(STID_SELECTION_CONV_REFUSED));
               } else {
                  TwoLineMsg(TgLoadString(STID_SELECTION_CONV_REFUSED),
                        TgLoadString(STID_TRY_PASTE_WITH_OLD_X_MECH));
               }
            } else {
               cut_buffer = GetTextBytesFromWindowProperty(
                     ev.xselection.requestor,
                     ev.xselection.property, compound_text, pulLen, True);
               if (cut_buffer == NULL || (pulLen != NULL && *pulLen == 0L)) {
                  if (paste_utf8_string) {
                     if (need_to_try_utf8) {
                        need_to_try_utf8 = FALSE;
                        need_to_try_text = TRUE;
                        XConvertSelection(mainDisplay, which_selection,
                              utf8StringAtom, tmpSelectionAtom, mainWindow,
                              lastKeyOrBtnEvInfo.time);
                        continue;
                     } else if (need_to_try_text) {
                        need_to_try_text = FALSE;
                        XConvertSelection(mainDisplay, which_selection,
                              textAtom, tmpSelectionAtom, mainWindow,
                              lastKeyOrBtnEvInfo.time);
                        continue;
                     }
                  } else {
                     if (need_to_try_text) {
                        need_to_try_text = FALSE;
                        need_to_try_utf8 = TRUE;
                        XConvertSelection(mainDisplay, which_selection,
                              textAtom, tmpSelectionAtom, mainWindow,
                              lastKeyOrBtnEvInfo.time);
                        continue;
                     } else if (need_to_try_utf8) {
                        need_to_try_utf8 = FALSE;
                        XConvertSelection(mainDisplay, which_selection,
                              utf8StringAtom, tmpSelectionAtom, mainWindow,
                              lastKeyOrBtnEvInfo.time);
                        continue;
                     }
                  }
               }
            }
            done = TRUE;
         } else if (ev.type == SelectionRequest) {
            HandleSelectionRequest(&ev);
         }
      } else {
         MillisecondSleep(100);
      }
      time(&tloc);
      if (tloc >= endtime) break;
   }
   RestoreStatusStrings();

   if (!done) {
      if (*pn_timeout_msg_displayed) {
         /* don't display the timeout message again */
      } else {
         *pn_timeout_msg_displayed = TRUE;
         sprintf(gszMsgBox, TgLoadString(STID_TIMEOUT_CONVERT_GIVEN),
               (compound_text ? "COMPOUND_TEXT" : "TEXT"));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      UtilFree(cut_buffer);

      return NULL;
   }
   if (cut_buffer != NULL) {
      if (paste_utf8_string && CutBufferType(cut_buffer) != CBF_TGIF_OBJ) {
         if (!ConvertToOrFromUTF8(CONVERT_FROM_UTF8,
               (canvasFontDoubleByte ? curFont : pssi->font), &cut_buffer)) {
            UtilFree(cut_buffer);
            return NULL;
         }
      } else {
#ifndef _NO_KINPUT
         if (cut_buffer) CvtCompoundTextToEuc(cut_buffer, cut_buffer);
#endif /* ~_NO_KINPUT */
      }
   }
   return cut_buffer;
}

char *GetTextBytesFromSelection(compound_text, pulLen)
   int compound_text;
   unsigned long *pulLen;
   /* the returned string should be freed by calling UtilFree() */
{
   int i=0, paste_utf8_string=FALSE, timeout_msg_displayed=FALSE;
   char *cut_buffer=NULL;
   StrSegInfo ssi;

   if (pulLen != NULL) *pulLen = 0L;

   memset(&ssi, 0, sizeof(StrSegInfo));
   if (curChoice == DRAWTEXT && textCursorShown && pastingUTF8String) {
      if (!textHighlight) {
         if (canvasFontDoubleByte && CanPasteUTF8StringIntoText(&ssi)) {
            if (ssi.double_byte) {
               SetCopyUTF8FontInfo(&ssi, TRUE);
            }
            paste_utf8_string = TRUE;
         } else if (CanFindDoubleByteFontAtCursor(&ssi)) {
            if (ssi.double_byte) {
               SetCopyUTF8FontInfo(&ssi, TRUE);
            }
            paste_utf8_string = TRUE;
         }
      } else if (CanPasteUTF8StringIntoText(&ssi)) {
         if (ssi.double_byte) {
            SetCopyUTF8FontInfo(&ssi, TRUE);
         }
         paste_utf8_string = TRUE;
      }
   }
   SaveStatusStrings();
   sprintf(gszMsgBox, TgLoadString(STID_CONVERTING_GIVEN_DOTS),
         (compound_text ? "COMPOUND_TEXT" : "TEXT"));
   SetStringStatus(gszMsgBox);
   for (i=0; gaAllSelectionAtom[i] != (Atom)0; i++) {
      cut_buffer = GetTextBytesFromGivenSelection(compound_text, pulLen,
            gaAllSelectionAtom[i], paste_utf8_string, &ssi,
            &timeout_msg_displayed);
      if (cut_buffer == NULL || (pulLen != NULL && *pulLen == 0L)) {
         continue;
      } else {
         return cut_buffer;
      }
   }
   RestoreStatusStrings();
   /*
    * If nothing else worked, go back to the the old way of getting data from
    *       the root window.
    */
   if (!timeout_msg_displayed) {
      cut_buffer = GetTextBytesFromWindowProperty(rootWindow, XA_CUT_BUFFER0,
            compound_text, pulLen, False);
      if (cut_buffer != NULL) {
         if (paste_utf8_string && CutBufferType(cut_buffer) != CBF_TGIF_OBJ) {
            if (!ConvertToOrFromUTF8(CONVERT_FROM_UTF8,
                  (canvasFontDoubleByte ? curFont: ssi.font), &cut_buffer)) {
               UtilFree(cut_buffer);
               return NULL;
            }
         } else {
#ifndef _NO_KINPUT
            if (cut_buffer) CvtCompoundTextToEuc(cut_buffer, cut_buffer);
#endif /* ~_NO_KINPUT */
         }
      }
   }
   return cut_buffer;
}

int PasteCompoundText()
{
   unsigned long uLen=0L;
   char *cut_buffer=NULL;

   if (curChoice == DRAWTEXT) {
      XEvent ev;

      pasteInDrawTextMode = TRUE;
      pasteCompoundTextInDrawTextMode = TRUE;
      ev.type = KeyPress;
      DrawText(&ev);
      return FALSE;
   }
   cut_buffer = GetTextBytesFromSelection(TRUE, &uLen);
   if (cut_buffer == NULL || uLen == 0L) {
      return TRUE;
   }
   Msg(TgLoadString(STID_PASTE_COMP_TEXT_FROM_NON_TGIF));
   PasteString(cut_buffer, TRUE, TRUE);
   UtilFree(cut_buffer);

   return TRUE;
}

int CopyDoubleByteString()
   /* returns FALSE if copying in text mode -- this is */
   /*    interpreted as an attempt to copy highlighted text */
{
   int handle_edit_text_size=FALSE;

   if (curChoice == DRAWTEXT) {
      if (escPressed) {
         escPressed = FALSE;
         Msg(TgLoadString(STID_ESC_KEY_PRESS_IGNORED));
      }
      if (!textHighlight) {
         MsgBox(TgLoadString(STID_NO_TEXT_SELECTED_FOR_COPY), TOOL_NAME,
               INFO_MB);
         return FALSE;
      } else {
         if (editTextSize != 0) {
            handle_edit_text_size = TRUE;
            if (RestoreEditTextSize(curTextObj, TRUE)) {
               UpdTextBBox(curTextObj);
            }
         }
         if (CanCopyHighLightedTextAsUTF8Strings(NULL)) {
            XEvent ev;

            if (handle_edit_text_size && editTextSize != 0) {
               if (RestoreEditTextSize(curTextObj, FALSE)) {
                  UpdTextBBox(curTextObj);
               }
            }
            copyInDrawTextMode = TRUE;
            copyDoubleByteStringInDrawTextMode = TRUE;
            ev.type = KeyPress;
            DrawText(&ev);
            copyDoubleByteStringInDrawTextMode = FALSE;
            copyInDrawTextMode = FALSE;

            return FALSE;
         }
      }
   }
   return TRUE;
}

int PasteDoubleByteString()
{
   int rc=0;

   pastingUTF8String = TRUE;
   rc = PasteFromCutBuffer();
   pastingUTF8String = FALSE;

   return rc;
}

void CleanUpCutBuffer()
{
   copyingToCutBuffer = FALSE;
}

/* ----------------------- Properties ----------------------- */

static
void SkipIntProp(lWhich, plSkip)
   long lWhich, *plSkip;
{
   (*plSkip) |= lWhich;
}

static
void TestStringProp(n1, n2, psz1, psz2, lWhich, plSkip)
   int n1, n2;
   char *psz1, *psz2; /* psz2 cannot be NULL */
   long lWhich, *plSkip;
{
   if (n1 != n2 ||
         ((psz1 == NULL || *psz1 == '\0') && *psz2 != '\0') ||
         ((psz1 != NULL && *psz1 != '\0') && strcmp(psz1, psz2) != 0)) {
      SkipIntProp(lWhich, plSkip);
   }
}

static
void SetStringProp(n1, pn2, psz1, psz2)
   int n1, *pn2;
   char *psz1, *psz2; /* psz2 cannot be NULL */
{
   *pn2 = n1;
   if (psz1 == NULL) {
      *psz2 = '\0';
   } else {
      strcpy(psz2, psz1);
   }
}

static
void TestIntProp(n1, n2, lWhich, plSkip)
   int n1, n2;
   long lWhich, *plSkip;
{
   if (n1 != n2) SkipIntProp(lWhich, plSkip);
}

static
void TestFontProp(nDoubleByte1, nFont1, nStyle1, nDoubleByte2, nFont2,
      nStyle2, lWhich, plSkip)
   int nDoubleByte1, nFont1, nStyle1;
   int nDoubleByte2, nFont2, nStyle2;
   long lWhich, *plSkip;
{
   if (nStyle1 != nStyle2) {
      SkipIntProp(lWhich, plSkip);
   } else if (nDoubleByte1 != nDoubleByte2) {
      SkipIntProp(lWhich, plSkip);
   } else if (nDoubleByte1) {
      if (nFont1 != nFont2) {
         SkipIntProp(lWhich, plSkip);
      }
   } else {
      if (nFont1 != nFont2) {
         SkipIntProp(lWhich, plSkip);
      }
   }
}

static
void TestCTMProp(ctm1, transformed, ctm2, lWhich, plSkip)
   struct XfrmMtrxRec *ctm1, *ctm2;
   int transformed;
   long lWhich, *plSkip;
{
   if ((ctm1 == NULL && transformed) ||
         (ctm1 != NULL && (!transformed ||
         ctm1->m[0] != ctm2->m[0] ||
         ctm1->m[1] != ctm2->m[1] ||
         ctm1->m[2] != ctm2->m[2] ||
         ctm1->m[3] != ctm2->m[3] ||
         ctm1->t[0] != ctm2->t[0] ||
         ctm1->t[1] != ctm2->t[1]))) {
      SkipIntProp(lWhich, plSkip);
   }
}

void SetIntPropertyMask(lWhich, nValue, pszValue, plMask, plSkip, pProp)
   long lWhich, *plMask, *plSkip;
   int nValue;
   char *pszValue;
   struct PropertiesRec *pProp;
{
   if (((*plSkip) & lWhich) == lWhich) {
      /*
       * this property is inconsistent, continue skipping it
       */
   } else if (((*plMask) & lWhich) == lWhich) {
      /*
       * this property was set before, if it's not the same, skip/invalidate it
       */
      switch (lWhich) {
      case PROP_MASK_COLOR:
         TestStringProp(nValue, pProp->color, pszValue, pProp->color_str,
               lWhich, plSkip);
         break;
      case PROP_MASK_WIDTH:
         TestStringProp(nValue, pProp->width, pszValue, pProp->width_spec,
               lWhich, plSkip);
         break;
      case PROP_MASK_AW:
         TestStringProp(nValue, pProp->aw, pszValue, pProp->aw_spec,
               lWhich, plSkip);
         break;
      case PROP_MASK_AH:
         TestStringProp(nValue, pProp->ah, pszValue, pProp->ah_spec,
               lWhich, plSkip);
         break;
      case PROP_MASK_TRANSPAT:
         TestIntProp(nValue, pProp->trans_pat, lWhich, plSkip);
         break;
      case PROP_MASK_FILL:
         TestIntProp(nValue, pProp->fill, lWhich, plSkip);
         break;
      case PROP_MASK_PEN:
         TestIntProp(nValue, pProp->pen, lWhich, plSkip);
         break;
      case PROP_MASK_DASH:
         TestIntProp(nValue, pProp->dash, lWhich, plSkip);
         break;
      case PROP_MASK_ARROW_STYLE:
         TestIntProp(nValue, pProp->arrow_style, lWhich, plSkip);
         break;
      case PROP_MASK_CURVED:
         TestIntProp(nValue, pProp->curved, lWhich, plSkip);
         break;
      case PROP_MASK_RCB_RADIUS:
         TestIntProp(nValue, pProp->rcb_radius, lWhich, plSkip);
         break;
      case PROP_MASK_TEXT_JUST:
         TestIntProp(nValue, pProp->text_just, lWhich, plSkip);
         break;
      case PROP_MASK_TEXT_SZ_UNIT:
         TestIntProp(nValue, pProp->text_sz_unit, lWhich, plSkip);
         break;
      case PROP_MASK_UNDERLINE_ON:
         TestIntProp(nValue, pProp->underline_on, lWhich, plSkip);
         break;
      case PROP_MASK_OVERLINE_ON:
         TestIntProp(nValue, pProp->overline_on, lWhich, plSkip);
         break;
      }
   } else {
      /* this property is never set, go set it */
      (*plMask) |= lWhich;
      switch (lWhich) {
      case PROP_MASK_COLOR:
         SetStringProp(nValue, &pProp->color, pszValue, pProp->color_str);
         break;
      case PROP_MASK_WIDTH:
         SetStringProp(nValue, &pProp->width, pszValue, pProp->width_spec);
         break;
      case PROP_MASK_AW:
         SetStringProp(nValue, &pProp->aw, pszValue, pProp->aw_spec);
         break;
      case PROP_MASK_AH:
         SetStringProp(nValue, &pProp->ah, pszValue, pProp->ah_spec);
         break;
      case PROP_MASK_TRANSPAT: pProp->trans_pat = nValue; break;
      case PROP_MASK_FILL: pProp->fill = nValue; break;
      case PROP_MASK_PEN: pProp->pen = nValue; break;
      case PROP_MASK_DASH: pProp->dash = nValue; break;
      case PROP_MASK_ARROW_STYLE: pProp->arrow_style = nValue; break;
      case PROP_MASK_CURVED: pProp->curved = nValue; break;
      case PROP_MASK_RCB_RADIUS: pProp->rcb_radius = nValue; break;
      case PROP_MASK_TEXT_JUST: pProp->text_just = nValue; break;
      case PROP_MASK_TEXT_SZ_UNIT: pProp->text_sz_unit = nValue; break;
      case PROP_MASK_VSPACE: pProp->v_space = nValue; break;
      case PROP_MASK_UNDERLINE_ON: pProp->underline_on = nValue; break;
      case PROP_MASK_OVERLINE_ON: pProp->overline_on = nValue; break;
      }
   }
}

void SetFontPropertyMask(nDoubleByte, nFont, nStyle, plMask, plSkip, pProp)
   int nDoubleByte, nFont, nStyle;
   long *plMask, *plSkip;
   struct PropertiesRec *pProp;
{
   if (((*plSkip) & PROP_MASK_TEXT_FONT) == PROP_MASK_TEXT_FONT) {
      /*
       * this property is inconsistent, continue skipping it
       */
   } else if (((*plMask) & PROP_MASK_TEXT_FONT) == PROP_MASK_TEXT_FONT) {
      /*
       * this property was set before, if it's not the same, skip/invalidate it
       */
      TestFontProp(nDoubleByte, nFont, nStyle, pProp->double_byte,
            pProp->text_font, pProp->text_style, PROP_MASK_TEXT_FONT, plSkip);
   } else {
      /* this property is never set, go set it */
      (*plMask) |= PROP_MASK_TEXT_FONT;
      pProp->double_byte = nDoubleByte;
      pProp->text_font = nFont;
      pProp->text_style = nStyle;
   }
}

void SetCTMPropertyMask(ctm, plMask, plSkip, pProp)
   struct XfrmMtrxRec *ctm;
   long *plMask, *plSkip;
   struct PropertiesRec *pProp;
{
   if (((*plSkip) & PROP_MASK_CTM) == PROP_MASK_CTM) {
      /*
       * this property is inconsistent, continue skipping it
       */
   } else if (((*plMask) & PROP_MASK_CTM) == PROP_MASK_CTM) {
      /*
       * this property was set before, if it's not the same, skip/invalidate it
       */
      TestCTMProp(ctm, pProp->transformed, &pProp->ctm, PROP_MASK_CTM, plSkip);
   } else {
      /* this property is never set, go set it */
      (*plMask) |= PROP_MASK_CTM;
      if (ctm == NULL) {
         pProp->transformed = FALSE;
      } else {
         pProp->transformed = TRUE;
         memcpy(&pProp->ctm, ctm, sizeof(struct XfrmMtrxRec));
      }
   }
}

void SetTextPropMask(ObjPtr, plMask, plSkip, pProp)
   struct ObjRec *ObjPtr;
   long *plMask, *plSkip;
   struct PropertiesRec *pProp;
{
   struct TextRec *text_ptr=ObjPtr->detail.t;
   int sz_unit=INVALID, double_byte=INVALID, font=INVALID, style=INVALID;
   int underline_on=FALSE, overline_on=FALSE;

   SetCTMPropertyMask(ObjPtr->ctm, plMask, plSkip, pProp);

   SetIntPropertyMask(PROP_MASK_COLOR, ObjPtr->color,
         colorMenuItems[ObjPtr->color], plMask, plSkip, pProp);

   SetIntPropertyMask(PROP_MASK_TRANSPAT, ObjPtr->trans_pat, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_FILL, text_ptr->fill, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_PEN, text_ptr->pen, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_TEXT_JUST, text_ptr->minilines.just, NULL,
         plMask, plSkip, pProp);
   SetIntPropertyMask(PROP_MASK_VSPACE, text_ptr->minilines.v_space, NULL,
         plMask, plSkip, pProp);

   if (SingleFontText(text_ptr, &sz_unit, &double_byte, &font, &style,
         &underline_on, &overline_on)) {
      SetIntPropertyMask(PROP_MASK_TEXT_SZ_UNIT, sz_unit, NULL,
            plMask, plSkip, pProp);
      SetIntPropertyMask(PROP_MASK_UNDERLINE_ON, underline_on, NULL,
            plMask, plSkip, pProp);
      SetIntPropertyMask(PROP_MASK_OVERLINE_ON, overline_on, NULL,
            plMask, plSkip, pProp);
      SetFontPropertyMask(double_byte, font, style, plMask, plSkip, pProp);
   }
}

static
void SetPropMask(ObjPtr, plMask, plSkip, pProp)
   struct ObjRec *ObjPtr;
   long *plMask, *plSkip;
   struct PropertiesRec *pProp;
{
   if (curChoice == DRAWTEXT) {
      if (textCursorShown && textHighlight) {
         SetIntPropertyMask(PROP_MASK_TRANSPAT, transPat, NULL,
               plMask, plSkip, pProp);
         SetIntPropertyMask(PROP_MASK_FILL, objFill, NULL,
               plMask, plSkip, pProp);
         SetIntPropertyMask(PROP_MASK_PEN, penPat, NULL,
               plMask, plSkip, pProp);

         SetIntPropertyMask(PROP_MASK_TEXT_JUST, textJust, NULL,
               plMask, plSkip, pProp);
         SetIntPropertyMask(PROP_MASK_VSPACE, textVSpace, NULL,
               plMask, plSkip, pProp);

         if (HighlightedTextHasSameProperty(PROP_MASK_COLOR,
               curStrBlock->seg->color, TRUE)) {
            SetIntPropertyMask(PROP_MASK_COLOR, colorIndex,
                  colorMenuItems[colorIndex], plMask, plSkip, pProp);
         }
         if (HighlightedTextHasSameProperty(PROP_MASK_TEXT_SZ_UNIT,
               curStrBlock->seg->sz_unit, TRUE)) {
            SetIntPropertyMask(PROP_MASK_TEXT_SZ_UNIT, GetCurSzUnit(), NULL,
                  plMask, plSkip, pProp);
         }
         if (HighlightedTextHasSameProperty(PROP_MASK_UNDERLINE_ON,
               curStrBlock->seg->underline_on, TRUE)) {
            SetIntPropertyMask(PROP_MASK_UNDERLINE_ON, curUnderlineOn, NULL,
                  plMask, plSkip, pProp);
         }
         if (HighlightedTextHasSameProperty(PROP_MASK_OVERLINE_ON,
               curStrBlock->seg->overline_on, TRUE)) {
            SetIntPropertyMask(PROP_MASK_OVERLINE_ON, curOverlineOn, NULL,
                  plMask, plSkip, pProp);
         }
         if (HighlightedTextHasSameProperty(PROP_MASK_TEXT_FONT,
               curStrBlock->seg->font, TRUE) &&
               HighlightedTextHasSameProperty(PROP_MASK_TEXT_STYLE,
               curStrBlock->seg->style, TRUE)) {
            SetFontPropertyMask(canvasFontDoubleByte, curFont, curStyle, plMask,
                  plSkip, pProp);
         }
      } else {
         SetIntPropertyMask(PROP_MASK_TRANSPAT, transPat, NULL,
               plMask, plSkip, pProp);
         SetIntPropertyMask(PROP_MASK_FILL, objFill, NULL,
               plMask, plSkip, pProp);
         SetIntPropertyMask(PROP_MASK_PEN, penPat, NULL,
               plMask, plSkip, pProp);

         SetIntPropertyMask(PROP_MASK_TEXT_JUST, textJust, NULL,
               plMask, plSkip, pProp);
         SetIntPropertyMask(PROP_MASK_VSPACE, textVSpace, NULL,
               plMask, plSkip, pProp);

         SetIntPropertyMask(PROP_MASK_COLOR, colorIndex,
               colorMenuItems[colorIndex], plMask, plSkip, pProp);
         SetIntPropertyMask(PROP_MASK_TEXT_SZ_UNIT, GetCurSzUnit(), NULL,
               plMask, plSkip, pProp);
         SetIntPropertyMask(PROP_MASK_UNDERLINE_ON, curUnderlineOn, NULL,
               plMask, plSkip, pProp);
         SetIntPropertyMask(PROP_MASK_OVERLINE_ON, curOverlineOn, NULL,
               plMask, plSkip, pProp);
         SetFontPropertyMask(canvasFontDoubleByte, curFont, curStyle, plMask,
               plSkip, pProp);
      }
   } else if (ObjPtr == NULL) {
      double radian=((double)(textRotation))*M_PI/180.0/64.0;
      double sin_val=sin(radian);
      double cos_val=cos(radian);
      struct XfrmMtrxRec ctm;

      ctm.m[CTM_SX] = ctm.m[CTM_SY] = ((double)1000)*cos_val;
      ctm.m[CTM_SIN] = ((double)1000)*sin_val;
      ctm.m[CTM_MSIN] = (-ctm.m[CTM_SIN]);
      ctm.t[CTM_TX] = ctm.t[CTM_TY] = 0;
      SetCTMPropertyMask(&ctm, plMask, plSkip, pProp);

      SetIntPropertyMask(PROP_MASK_COLOR, colorIndex,
            colorMenuItems[colorIndex], plMask, plSkip, pProp);
      SetIntPropertyMask(PROP_MASK_WIDTH, curWidthOfLine[lineWidth],
            curWidthOfLineSpec[lineWidth],
            plMask, plSkip, pProp);
      SetIntPropertyMask(PROP_MASK_AW, curArrowHeadW[lineWidth],
            curArrowHeadWSpec[lineWidth],
            plMask, plSkip, pProp);
      SetIntPropertyMask(PROP_MASK_AH, curArrowHeadH[lineWidth],
            curArrowHeadHSpec[lineWidth],
            plMask, plSkip, pProp);

      SetIntPropertyMask(PROP_MASK_TRANSPAT, transPat, NULL,
            plMask, plSkip, pProp);
      SetIntPropertyMask(PROP_MASK_FILL, objFill, NULL,
            plMask, plSkip, pProp);
      SetIntPropertyMask(PROP_MASK_PEN, penPat, NULL,
            plMask, plSkip, pProp);
      SetIntPropertyMask(PROP_MASK_DASH, curDash, NULL,
            plMask, plSkip, pProp);
      SetIntPropertyMask(PROP_MASK_CURVED, curSpline, NULL,
            plMask, plSkip, pProp);
      SetIntPropertyMask(PROP_MASK_ARROW_STYLE, lineStyle, NULL,
            plMask, plSkip, pProp);
      SetIntPropertyMask(PROP_MASK_RCB_RADIUS, rcbRadius, NULL,
            plMask, plSkip, pProp);

      SetIntPropertyMask(PROP_MASK_TEXT_JUST, textJust, NULL,
            plMask, plSkip, pProp);
      SetIntPropertyMask(PROP_MASK_TEXT_SZ_UNIT, GetCurSzUnit(), NULL,
            plMask, plSkip, pProp);
      SetIntPropertyMask(PROP_MASK_VSPACE, textVSpace, NULL,
            plMask, plSkip, pProp);
      SetIntPropertyMask(PROP_MASK_UNDERLINE_ON, curUnderlineOn, NULL,
            plMask, plSkip, pProp);
      SetIntPropertyMask(PROP_MASK_OVERLINE_ON, curOverlineOn, NULL,
            plMask, plSkip, pProp);

      SetFontPropertyMask(canvasFontDoubleByte, curFont, curStyle, plMask,
            plSkip, pProp);
   } else {
      struct ObjRec *obj_ptr=NULL;

      switch (ObjPtr->type) {
      case OBJ_POLY: SetPolyPropMask(ObjPtr, plMask, plSkip, pProp); break;
      case OBJ_BOX: SetBoxPropMask(ObjPtr, plMask, plSkip, pProp); break;
      case OBJ_OVAL: SetOvalPropMask(ObjPtr, plMask, plSkip, pProp); break;
      case OBJ_TEXT: SetTextPropMask(ObjPtr, plMask, plSkip, pProp); break;
      case OBJ_POLYGON:
         SetPolygonPropMask(ObjPtr, plMask, plSkip, pProp);
         break;
      case OBJ_ARC: SetArcPropMask(ObjPtr, plMask, plSkip, pProp); break;
      case OBJ_RCBOX: SetRCBoxPropMask(ObjPtr, plMask, plSkip, pProp); break;
      case OBJ_XBM: SetXBmPropMask(ObjPtr, plMask, plSkip, pProp); break;
      case OBJ_XPM: SetXPmPropMask(ObjPtr, plMask, plSkip, pProp); break;

      case OBJ_GROUP:
      case OBJ_ICON:
      case OBJ_SYM:
      case OBJ_PIN:
         for (obj_ptr=ObjPtr->detail.r->first; obj_ptr != NULL;
               obj_ptr=obj_ptr->next) {
            SetPropMask(obj_ptr, plMask, plSkip, pProp);
         }
         break;
      }
   }
}

static
int GetAngleFromCTM(ctm)
   struct XfrmMtrxRec *ctm;
   /* returned angle is degree*64 */
{
   double sin_val=(double)(ctm->m[CTM_SIN]/((double)1000));
   double cos_val=(double)(ctm->m[CTM_SX]/((double)1000));
   double radian=(double)0;
   int angle=0;

   if (fabs(sin_val) < EQ_TOL) {
      if (cos_val > 0) {
         radian = (double)0;
      } else {
         radian = (double)M_PI;
      }
   } else if (fabs(cos_val) < EQ_TOL) {
      if (sin_val> 0) {
         radian = (double)(M_PI/2.0);
      } else {
         radian = (double)(M_PI*3.0/2.0);
      }
   } else {
      radian = (double)atan(sin_val/cos_val);
      if (radian >= 0) {
         if (sin_val < 0) {
            radian += (double)M_PI;
         }
      } else {
         if (cos_val > 0) {
            radian += (double)(M_PI*2.0);
         } else {
            radian += (double)M_PI;
         }
      }
   }
   angle = (int)(radian*((double)180)*((double)64)/M_PI);
   return angle;
}

static
void DoPasteAProperty(lWhich, pProp)
   long lWhich;
   struct PropertiesRec *pProp;
{
   char szBuf[MAXSTRING];
   int index=0, new_alloc=FALSE;

   switch (lWhich) {
   case PROP_MASK_COLOR:
      index = QuickFindColorIndex(NULL, pProp->color_str, &new_alloc, FALSE);
      if (index == INVALID) {
         sprintf(gszMsgBox, TgLoadString(STID_FAIL_ALLOC_NAMED_COLOR),
               pProp->color_str);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      } else {
         ChangeAllSelColor(index, TRUE);
      }
      break;
   case PROP_MASK_WIDTH:
      ChangeAllSelRealLineWidth(CHANGE_WIDTH, pProp->width, (-1), (-1),
            pProp->width_spec, NULL, NULL, TRUE);
      break;
   case PROP_MASK_AW:
      ChangeAllSelRealLineWidth(CHANGE_AW, (-1), pProp->aw, (-1),
            NULL, pProp->aw_spec, NULL, TRUE);
      break;
   case PROP_MASK_AH:
      ChangeAllSelRealLineWidth(CHANGE_AH, (-1), (-1), pProp->ah,
            NULL, NULL, pProp->ah_spec, TRUE);
      break;

   case PROP_MASK_TRANSPAT: ChangeAllSelTransPat(pProp->trans_pat, TRUE); break;
   case PROP_MASK_FILL: ChangeAllSelFill(pProp->fill, TRUE); break;
   case PROP_MASK_PEN: ChangeAllSelPen(pProp->pen, TRUE); break;
   case PROP_MASK_DASH: ChangeAllSelLineDash(pProp->dash, TRUE); break;
   case PROP_MASK_ARROW_STYLE:
      ChangeAllSelLineStyle(pProp->arrow_style, TRUE);
      break;
   case PROP_MASK_CURVED: ChangeAllSelLineType(pProp->curved, TRUE); break;
   case PROP_MASK_RCB_RADIUS: ChangeAllSelRCBRadius(pProp->rcb_radius); break;
   case PROP_MASK_TEXT_JUST: ChangeFontJust(pProp->text_just); break;
   case PROP_MASK_TEXT_SZ_UNIT:
      sprintf(szBuf, "%1d", SzUnitToFontSize(pProp->text_sz_unit));
      SetSelFontSize(szBuf);
      break;
   case PROP_MASK_VSPACE: ChangeVSpace(pProp->v_space); break;
   case PROP_MASK_UNDERLINE_ON: ChangeFontUnderline(pProp->underline_on); break;
   case PROP_MASK_OVERLINE_ON: ChangeFontOverline(pProp->overline_on); break;

   case PROP_MASK_TEXT_FONT:
      ChangeFont(pProp->text_font, TRUE);
      ChangeFontStyle(pProp->text_style);
      break;

   case PROP_MASK_CTM:
      if (topSel == NULL) {
         if (pProp->transformed) {
            int angle=GetAngleFromCTM(&pProp->ctm);

            FormatAngle(angle, szBuf);
         } else {
            strcpy(szBuf, "0");
         }
         SetTextRotation(szBuf);
      } else {
         SetSelCTM(pProp->transformed, &pProp->ctm);
      }
      break;
   }
}

static
void DoGetAProperty(lWhich, pProp)
   long lWhich;
   struct PropertiesRec *pProp;
{
   char szBuf[MAXSTRING];
   int index=0, new_alloc=FALSE;

   switch (lWhich) {
   case PROP_MASK_COLOR:
      index = QuickFindColorIndex(NULL, pProp->color_str, &new_alloc, FALSE);
      if (index == INVALID) {
         sprintf(gszMsgBox, TgLoadString(STID_FAIL_ALLOC_NAMED_COLOR),
               pProp->color_str);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      } else {
         ChangeAllSelColor(index, TRUE);
      }
      break;
   case PROP_MASK_WIDTH_INDEX:
      ChangeAllSelLineWidth(pProp->width_index, TRUE);
      break;
   case PROP_MASK_WIDTH:
      ChangeAllSelRealLineWidth(CHANGE_WIDTH, pProp->width, (-1), (-1),
            pProp->width_spec, NULL, NULL, TRUE);
      break;
   case PROP_MASK_AW:
      ChangeAllSelRealLineWidth(CHANGE_AW, (-1), pProp->aw, (-1),
            NULL, pProp->aw_spec, NULL, TRUE);
      break;
   case PROP_MASK_AH:
      ChangeAllSelRealLineWidth(CHANGE_AH, (-1), (-1), pProp->ah,
            NULL, NULL, pProp->ah_spec, TRUE);
      break;

   case PROP_MASK_TRANSPAT: ChangeAllSelTransPat(pProp->trans_pat, TRUE); break;
   case PROP_MASK_FILL: ChangeAllSelFill(pProp->fill, TRUE); break;
   case PROP_MASK_PEN: ChangeAllSelPen(pProp->pen, TRUE); break;
   case PROP_MASK_DASH: ChangeAllSelLineDash(pProp->dash, TRUE); break;
   case PROP_MASK_ARROW_STYLE:
      ChangeAllSelLineStyle(pProp->arrow_style, TRUE);
      break;
   case PROP_MASK_CURVED: ChangeAllSelLineType(pProp->curved, TRUE); break;
   case PROP_MASK_RCB_RADIUS: ChangeAllSelRCBRadius(pProp->rcb_radius); break;
   case PROP_MASK_TEXT_JUST: ChangeFontJust(pProp->text_just); break;
   case PROP_MASK_TEXT_SZ_UNIT:
      sprintf(szBuf, "%1d", SzUnitToFontSize(pProp->text_sz_unit));
      SetSelFontSize(szBuf);
      break;
   case PROP_MASK_VSPACE: ChangeVSpace(pProp->v_space); break;
   case PROP_MASK_UNDERLINE_ON: ChangeFontUnderline(pProp->underline_on); break;
   case PROP_MASK_OVERLINE_ON: ChangeFontOverline(pProp->overline_on); break;

   case PROP_MASK_TEXT_FONT:
      ChangeFont(pProp->text_font, TRUE);
      ChangeFontStyle(pProp->text_style);
      break;

   case PROP_MASK_CTM:
      if (topSel == NULL) {
         if (pProp->transformed) {
            int angle=GetAngleFromCTM(&pProp->ctm);

            FormatAngle(angle, szBuf);
         } else {
            strcpy(szBuf, "0");
         }
         SetTextRotation(szBuf);
      } else {
         SetSelCTM(pProp->transformed, &pProp->ctm);
      }
      break;
   }
}

static char gszPropIniFile[MAXPATHLENGTH];

static char gszCopyPasteSec[MAXSTRING];
static char gszCopyPasteBackupSec[MAXSTRING];
static char gszPropSetSec[MAXSTRING];
static char gszPropProfilePrefix[MAXSTRING];

struct PropInfoRec {
   long bit;
   int checked;
   char *key;
   char *desc;
};

static struct PropInfoRec gstPropInfo[] = {
   { PROP_MASK_AH,           FALSE, NULL, NULL },
   { PROP_MASK_AW,           FALSE, NULL, NULL },
   { PROP_MASK_ARROW_STYLE,  FALSE, NULL, NULL },
   { PROP_MASK_COLOR,        FALSE, NULL, NULL },
   { PROP_MASK_DASH,         FALSE, NULL, NULL },
   { PROP_MASK_WIDTH,        FALSE, NULL, NULL },
   { PROP_MASK_TRANSPAT,     FALSE, NULL, NULL },
   { PROP_MASK_FILL,         FALSE, NULL, NULL },
   { PROP_MASK_PEN,          FALSE, NULL, NULL },
   { PROP_MASK_RCB_RADIUS,   FALSE, NULL, NULL },
   { PROP_MASK_CURVED,       FALSE, NULL, NULL },
   { PROP_MASK_TEXT_FONT,    FALSE, NULL, NULL },
   { PROP_MASK_TEXT_STYLE,   FALSE, NULL, NULL },
   { PROP_MASK_TEXT_JUST,    FALSE, NULL, NULL },
   { PROP_MASK_TEXT_SZ_UNIT, FALSE, NULL, NULL },
   { PROP_MASK_VSPACE,       FALSE, NULL, NULL },
   { PROP_MASK_UNDERLINE_ON, FALSE, NULL, NULL },
   { PROP_MASK_OVERLINE_ON,  FALSE, NULL, NULL },
   { PROP_MASK_CTM,          FALSE, NULL, NULL },
   { PROP_MASK_WIDTH_INDEX,  FALSE, NULL, NULL },
   { 0L, FALSE, NULL, NULL }
};

static struct PropInfoRec gstCompatPropInfo[] = {
   /*
    * do not translate -- program constants
    *
    * These are used for compatibility reasons.  In an earlier version,
    *       the keys in "cutpaste.ini" are these strings.  In the current
    *       version, hex numbers are used.  gstCompatPropInfo are only used
    *       by UpdateSavedPropKeys() to update the old keys to new ones.
    */
   { PROP_MASK_AH,           FALSE, NULL, "arrow height" },
   { PROP_MASK_AW,           FALSE, NULL, "arrow width" },
   { PROP_MASK_ARROW_STYLE,  FALSE, NULL, "arrow style" },
   { PROP_MASK_COLOR,        FALSE, NULL, "color" },
   { PROP_MASK_DASH,         FALSE, NULL, "dash" },
   { PROP_MASK_WIDTH,        FALSE, NULL, "line width" },
   { PROP_MASK_TRANSPAT,     FALSE, NULL, "pattern transparency" },
   { PROP_MASK_FILL,         FALSE, NULL, "fill" },
   { PROP_MASK_PEN,          FALSE, NULL, "pen" },
   { PROP_MASK_RCB_RADIUS,   FALSE, NULL, "rcbox radius" },
   { PROP_MASK_CURVED,       FALSE, NULL, "spline" },
   { PROP_MASK_TEXT_FONT,    FALSE, NULL, "text font" },
   { PROP_MASK_TEXT_STYLE,   FALSE, NULL, "text style (no use, part of text font)" },
   { PROP_MASK_TEXT_JUST,    FALSE, NULL, "text justification" },
   { PROP_MASK_TEXT_SZ_UNIT, FALSE, NULL, "text size" },
   { PROP_MASK_VSPACE,       FALSE, NULL, "text vertical spacing" },
   { PROP_MASK_UNDERLINE_ON, FALSE, NULL, "text underline" },
   { PROP_MASK_CTM,          FALSE, NULL, "transformation matrix" },
   { 0L, FALSE, NULL, NULL }
};

static
void WriteIntProp(pszSec, pszKey, nValue)
   char *pszSec, *pszKey;
   int nValue;
{
   sprintf(gszMsgBox, "%1d", nValue);
   tgWriteProfileString(pszSec, pszKey, gszMsgBox, gszPropIniFile);
}

static
void WriteStringProp(pszSec, pszKey, nValue, pszValue)
   char *pszSec, *pszKey;
   int nValue;
   char *pszValue;
{
   sprintf(gszMsgBox, "%1d,%s", nValue, (pszValue==NULL ? "" : pszValue));
   tgWriteProfileString(pszSec, pszKey, gszMsgBox, gszPropIniFile);
}

static
void WriteFontProp(pszSec, pszKey, nFont, nStyle)
   char *pszSec, *pszKey;
   int nFont, nStyle;
{
   char font_str[MAXSTRING];

   *font_str = '\0';
   GetPSFontStr(nFont, nStyle, font_str);
   /* font_str starts with the '/' character */
   sprintf(gszMsgBox, "%1d,%s", nStyle, &font_str[1]);
   tgWriteProfileString(pszSec, pszKey, gszMsgBox, gszPropIniFile);
}

static
void WriteCTMProp(pszSec, pszKey, nTransformed, ctm)
   char *pszSec, *pszKey;
   int nTransformed;
   struct XfrmMtrxRec *ctm;
{
   if (nTransformed) {
      sprintf(gszMsgBox, "%1d,%g,%g,%g,%g,%1d,%1d", nTransformed,
            ctm->m[CTM_SX], ctm->m[CTM_SIN], ctm->m[CTM_MSIN], ctm->m[CTM_SY],
            ctm->t[CTM_TX], ctm->t[CTM_TY]);
   } else {
      sprintf(gszMsgBox, "%1d,%g,%g,%g,%g,%1d,%1d", nTransformed,
            (double)1000, (double)0, (double)0, (double)1000, 0, 0);
   }
   tgWriteProfileString(pszSec, pszKey, gszMsgBox, gszPropIniFile);
}

static
int ParseIntProp(pszBuf, pnValue)
   char *pszBuf;
   int *pnValue;
{
   return (sscanf(pszBuf, "%d", pnValue) == 1);
}

static
int ParseStringProp(pszBuf, pnValue, pszValue)
   char *pszBuf;
   int *pnValue;
   char *pszValue;
{
   char *psz1=strtok(pszBuf, ","), *psz2=NULL;

   if (psz1 == NULL) return FALSE;
   psz2 = strtok(NULL, ",");
   if (psz2 == NULL) return FALSE;

   if (sscanf(psz1, "%d", pnValue) != 1) return FALSE;
   strcpy(pszValue, psz2);

   return TRUE;
}

static
int ParseFontSzUnitProp(pszBuf, pnSzUnit)
   char *pszBuf;
   int *pnSzUnit;
{
   int i, sz_unit=0;

   if (sscanf(pszBuf, "%d", &sz_unit) != 1) return FALSE;
   for (i=0; i < numFontSizes; i++) {
      if (sz_unit == fontSzUnits[i]) {
         *pnSzUnit = sz_unit;
         return TRUE;
      }
   }
   if (topSel == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_FIND_SIZE_USE_ALT),
            SzUnitToFontSize(sz_unit), defaultFontSize);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      *pnSzUnit = FontSizeToSzUnit(defaultFontSize);
   } else {
      *pnSzUnit = sz_unit;
   }
   return TRUE;
}

static
int ParseFontProp(pszBuf, pnDoubleByte, pnFont, pnStyle)
   char *pszBuf;
   int *pnDoubleByte, *pnFont, *pnStyle;
{
   char font_str[MAXSTRING], *psz=NULL;

   *font_str = '\0';
   psz = strchr(pszBuf, ',');
   if (psz == NULL) return FALSE;

   *psz = '\0';
   if (sscanf(pszBuf, "%d", pnStyle) != 1) {
      *psz = ',';
      return FALSE;
   }
   *psz++ = ',';
   strcpy(font_str, psz);
   *pnFont = GetFontIndex(font_str, *pnStyle, TRUE);
   *pnDoubleByte = FALSE;
   if (*pnFont == INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_FIND_FONT_USE_ALT), font_str,
            "Times");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      *pnDoubleByte = FALSE;
      *pnFont = FONT_TIM;
   }
   return TRUE;
}

static
int ParseCTMProp(pszBuf, pnTransformed, ctm)
   char *pszBuf;
   int *pnTransformed;
   struct XfrmMtrxRec *ctm;
{
   struct PropInfoRec *ppir=NULL;
   char *psz=strchr(pszBuf, ',');

   if (psz == NULL) return FALSE;
   *psz = '\0';
   if (sscanf(pszBuf, "%d", pnTransformed) != 1) {
      *psz = ',';
      return FALSE;
   }
   *psz++ = ',';
   if (sscanf(psz, "%lg , %lg , %lg, %lg , %d , %d",
         &ctm->m[CTM_SX], &ctm->m[CTM_SIN], &ctm->m[CTM_MSIN], &ctm->m[CTM_SY],
         &ctm->t[CTM_TX], &ctm->t[CTM_TY]) != 6) {
      return FALSE;
   }
   if (topSel != NULL) {
      struct SelRec *sel_ptr=NULL;

      for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
         switch (sel_ptr->obj->type) {
         case OBJ_GROUP:
         case OBJ_ICON:
         case OBJ_SYM:
         case OBJ_PIN:
            for (ppir=gstPropInfo; ppir->bit != 0L; ppir++) {
               if (ppir->bit == PROP_MASK_CTM) {
                  sprintf(gszMsgBox,
                        TgLoadString(STID_CANNOT_PASTE_COMPOUND_OBJ),
                        ppir->desc, ppir->desc);
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                  break;
               }
            }
            return FALSE;
         }
      }
   } else if (topSel == NULL && (*pnTransformed)) {
      if ((fabs(ctm->m[CTM_SX]-ctm->m[CTM_SY]) < EQ_TOL) &&
            (fabs(ctm->m[CTM_SIN]+ctm->m[CTM_MSIN]) < EQ_TOL)) {
         return TRUE;
      }
      for (ppir=gstPropInfo; ppir->bit != 0L; ppir++) {
         if (ppir->bit == PROP_MASK_CTM) {
            sprintf(gszMsgBox, TgLoadString(STID_CUT_BUF_HAS_NON_ROT_COMP),
                  ppir->desc);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            break;
         }
      }
      *pnTransformed = FALSE;
   }
   return TRUE;
}

static
void WritePropToIni(lWhich, pszSec, pszKey, pProp)
   long lWhich;
   char *pszSec, *pszKey;
   struct PropertiesRec *pProp;
{
   switch (lWhich) {
   case PROP_MASK_AH:
      WriteStringProp(pszSec, pszKey, pProp->ah, pProp->ah_spec);
      break;
   case PROP_MASK_AW:
      WriteStringProp(pszSec, pszKey, pProp->aw, pProp->aw_spec);
      break;
   case PROP_MASK_ARROW_STYLE:
      WriteIntProp(pszSec, pszKey, pProp->arrow_style);
      break;
   case PROP_MASK_COLOR:
      WriteStringProp(pszSec, pszKey, pProp->color, pProp->color_str);
      break;

   case PROP_MASK_DASH: WriteIntProp(pszSec, pszKey, pProp->dash); break;
   case PROP_MASK_WIDTH:
      WriteStringProp(pszSec, pszKey, pProp->width, pProp->width_spec);
      break;
   case PROP_MASK_TRANSPAT:
      WriteIntProp(pszSec, pszKey, pProp->trans_pat);
      break;
   case PROP_MASK_FILL: WriteIntProp(pszSec, pszKey, pProp->fill); break;
   case PROP_MASK_PEN: WriteIntProp(pszSec, pszKey, pProp->pen); break;
   case PROP_MASK_RCB_RADIUS:
      WriteIntProp(pszSec, pszKey, pProp->rcb_radius);
      break;
   case PROP_MASK_CURVED: WriteIntProp(pszSec, pszKey, pProp->curved); break;
   case PROP_MASK_TEXT_FONT:
      WriteFontProp(pszSec, pszKey, pProp->text_font, pProp->text_style);
      break;
   case PROP_MASK_TEXT_JUST:
      WriteIntProp(pszSec, pszKey, pProp->text_just);
      break;
   case PROP_MASK_TEXT_SZ_UNIT:
      WriteIntProp(pszSec, pszKey, pProp->text_sz_unit);
      break;
   case PROP_MASK_UNDERLINE_ON:
      WriteIntProp(pszSec, pszKey, pProp->underline_on);
      break;
   case PROP_MASK_OVERLINE_ON:
      WriteIntProp(pszSec, pszKey, pProp->overline_on);
      break;
   case PROP_MASK_VSPACE: WriteIntProp(pszSec, pszKey, pProp->v_space); break;
   case PROP_MASK_CTM:
      WriteCTMProp(pszSec, pszKey, pProp->transformed, &pProp->ctm);
      break;
   }
}

static
int ParseAProp(lWhich, pszBuf, pProp)
   long lWhich;
   char *pszBuf;
   struct PropertiesRec *pProp;
{
   switch (lWhich) {
   case PROP_MASK_AH:
      return ParseStringProp(pszBuf, &pProp->ah, pProp->ah_spec);
   case PROP_MASK_AW:
      return ParseStringProp(pszBuf, &pProp->aw, pProp->aw_spec);
   case PROP_MASK_ARROW_STYLE: return ParseIntProp(pszBuf, &pProp->arrow_style);
   case PROP_MASK_COLOR:
      return ParseStringProp(pszBuf, &pProp->color, pProp->color_str);
   case PROP_MASK_DASH: return ParseIntProp(pszBuf, &pProp->dash);
   case PROP_MASK_WIDTH:
      return ParseStringProp(pszBuf, &pProp->width, pProp->width_spec);
   case PROP_MASK_TRANSPAT: return ParseIntProp(pszBuf, &pProp->trans_pat);
   case PROP_MASK_FILL: return ParseIntProp(pszBuf, &pProp->fill);
   case PROP_MASK_PEN: return ParseIntProp(pszBuf, &pProp->pen);
   case PROP_MASK_RCB_RADIUS: return ParseIntProp(pszBuf, &pProp->rcb_radius);
   case PROP_MASK_CURVED: return ParseIntProp(pszBuf, &pProp->curved);
   case PROP_MASK_TEXT_FONT:
      return ParseFontProp(pszBuf, &pProp->double_byte, &pProp->text_font,
            &pProp->text_style);
   case PROP_MASK_TEXT_JUST: return ParseIntProp(pszBuf, &pProp->text_just);
   case PROP_MASK_TEXT_SZ_UNIT:
      return ParseFontSzUnitProp(pszBuf, &pProp->text_sz_unit);
   case PROP_MASK_UNDERLINE_ON:
      return ParseIntProp(pszBuf, &pProp->underline_on);
   case PROP_MASK_OVERLINE_ON: return ParseIntProp(pszBuf, &pProp->overline_on);
   case PROP_MASK_VSPACE: return ParseIntProp(pszBuf, &pProp->v_space);
   case PROP_MASK_CTM:
      return ParseCTMProp(pszBuf, &pProp->transformed, &pProp->ctm);
   }
   return FALSE;
}

static
int ReadPropFromIni(pszSec, plMask, pCheckArray, pProp)
   /*
    * This routine sets *plMask according to what's in the ini file.
    *     All associated fields in pProp are filled in.
    * This routine sets pCheckArray->num_cols and pCheckArray->num_rows;
    *     pCheckArray->value is set to NULL.
    */
   char *pszSec;
   long *plMask;
   struct CheckArrayRec *pCheckArray;
   struct PropertiesRec *pProp;
{
   struct PropInfoRec *ppir=NULL;
   char *pszKey=NULL;
   char *pszKeys=tgGetProfileString(pszSec, NULL, gszPropIniFile);

   *plMask = 0L;
   for (ppir=gstPropInfo; ppir->bit != 0L; ppir++) ppir->checked = FALSE;
   pCheckArray->num_cols = 1;

   if (pszKeys == NULL) {
      strcpy(gszMsgBox, TgLoadString(STID_NO_PROPERTY_TO_PASTE));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   for (pszKey=pszKeys; *pszKey != '\0'; pszKey++) {
      for (ppir=gstPropInfo; ppir->bit != 0L; ppir++) {
         if (strcmp(pszKey, ppir->key) == 0) {
            break;
         }
      }
      if (ppir != NULL) {
         char *pszValue=tgGetProfileString(pszSec, pszKey, gszPropIniFile);

         if (topSel == NULL && (ppir->bit == PROP_MASK_WIDTH ||
               ppir->bit == PROP_MASK_AW || ppir->bit == PROP_MASK_AH)) {
         } else {
            if (ParseAProp(ppir->bit, ((pszValue==NULL)?"":pszValue), pProp)) {
               ppir->checked = TRUE;
               pCheckArray->num_rows++;
               *plMask |= ppir->bit;
            }
         }
         if (pszValue != NULL) tgFreeProfileString(pszValue);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_KEY_IN_SEC_OF_INIFILE),
               pszKey, pszSec, gszPropIniFile);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      pszKey += strlen(pszKey);
   }
   tgFreeProfileString(pszKeys);
   return TRUE;
}

#define COPY_BUTTON    101
#define PASTE_BUTTON   102
#define SAVE_BUTTON    103
#define RESTORE_BUTTON 104

void CleanUpProperties()
{
   struct PropInfoRec *ppir=NULL;

   for (ppir=gstPropInfo; ppir->bit != 0L; ppir++) {
      UtilFree(ppir->desc);
      UtilFree(ppir->key);
      ppir->desc = ppir->key = NULL;
   }
}

static
void UpdateSavedPropKeys()
{
   char *pszName=NULL;
   char *pszNames=tgGetProfileString(gszPropSetSec, NULL, gszPropIniFile);
   int already_updated=FALSE;

   if (pszNames == NULL) return;

   for (pszName=pszNames; !already_updated && *pszName != '\0'; pszName++) {
      char szPropSetSec[MAXSTRING], *pszKeys=NULL;

      sprintf(szPropSetSec, "%s: %s", gszPropProfilePrefix, pszName);

      pszKeys = tgGetProfileString(szPropSetSec, NULL, gszPropIniFile);
      if (pszKeys != NULL) {
         char *pszKey=NULL;

         for (pszKey=pszKeys; *pszKey != '\0'; pszKey++) {
            if (pszKey[0] == '0' && pszKey[1] == 'x') {
               /* already updated */
               already_updated = TRUE;
               break;
            } else {
               char *pszValue=tgGetProfileString(szPropSetSec, pszKey,
                     gszPropIniFile);

               if (pszValue != NULL) {
                  struct PropInfoRec *ppir_compat=NULL, *ppir=NULL;

                  for (ppir_compat=gstCompatPropInfo, ppir=gstPropInfo;
                        ppir_compat->bit != 0L;
                        ppir_compat++, ppir++) {
                     if (strcmp(ppir_compat->desc, pszKey) == 0) {
                        tgWriteProfileString(szPropSetSec, ppir->key, pszValue,
                              gszPropIniFile);
                        tgWriteProfileString(szPropSetSec, pszKey, NULL,
                              gszPropIniFile);
                        break;
                     }
                  }
                  tgFreeProfileString(pszValue);
               }
            }
            pszKey += strlen(pszKey);
         }
         tgFreeProfileString(pszKeys);
      }
      pszName += strlen(pszName);
   }
   tgFreeProfileString(pszNames);

   if (!already_updated) {
      tgWriteProfileString(NULL, NULL, NULL, gszPropIniFile);
   }
}

void InitProperties()
{
   struct PropInfoRec *ppir=NULL;

   /* do not translate -- program constants */
   sprintf(gszPropIniFile, "%s%ccutpaste.ini", tgifDir, DIR_SEP);

   strcpy(gszCopyPasteSec, "Copy/Paste Properties");
   strcpy(gszCopyPasteBackupSec, "Copy/Paste Properties - Backup");
   strcpy(gszPropSetSec, "Property Set Names");
   strcpy(gszPropProfilePrefix, "Property Profile");

   for (ppir=gstPropInfo; ppir->bit != 0L; ppir++) {
      if (ppir->key != NULL) return;

      ppir->desc = UtilStrDup(PropLoadString(ppir->bit));
      if (ppir->desc == NULL) FailAllocMessage();

      sprintf(gszMsgBox, "0x%08lx", ppir->bit);
      ppir->key = UtilStrDup(gszMsgBox);
      if (ppir->key == NULL) FailAllocMessage();
   }
   UpdateSavedPropKeys();
}

static
void FormatPropForDisplay(lWhich, pProp, ppir, pszBuf)
   long lWhich;
   struct PropertiesRec *pProp;
   struct PropInfoRec *ppir;
   char *pszBuf;
{
   char font_str[MAXSTRING];
   int nLen=0;

   sprintf(pszBuf, "%s: ", ppir->desc);
   nLen = strlen(pszBuf);

   switch (lWhich) {
   case PROP_MASK_CTM: strcat(pszBuf, "..."); break;

   case PROP_MASK_COLOR: strcat(pszBuf, pProp->color_str); break;
   case PROP_MASK_WIDTH_INDEX:
      sprintf(&pszBuf[nLen], "%1d", pProp->width_index);
      break;
   case PROP_MASK_WIDTH: strcat(pszBuf, pProp->width_spec); break;
   case PROP_MASK_AW: strcat(pszBuf, pProp->aw_spec); break;
   case PROP_MASK_AH: strcat(pszBuf, pProp->ah_spec); break;

   case PROP_MASK_TRANSPAT:
      sprintf(&pszBuf[nLen], "%1d", pProp->trans_pat);
      break;
   case PROP_MASK_FILL: sprintf(&pszBuf[nLen], "%1d", pProp->fill); break;
   case PROP_MASK_PEN: sprintf(&pszBuf[nLen], "%1d", pProp->pen); break;
   case PROP_MASK_DASH: sprintf(&pszBuf[nLen], "%1d", pProp->dash); break;
   case PROP_MASK_ARROW_STYLE:
      sprintf(&pszBuf[nLen], "%1d", pProp->arrow_style);
      break;
   case PROP_MASK_CURVED: sprintf(&pszBuf[nLen], "%1d", pProp->curved); break;
   case PROP_MASK_RCB_RADIUS:
      sprintf(&pszBuf[nLen], "%1d", pProp->rcb_radius);
      break;
   case PROP_MASK_TEXT_JUST:
      sprintf(&pszBuf[nLen], "%1d", pProp->text_just);
      break;
   case PROP_MASK_TEXT_SZ_UNIT:
      if (showFontSizeInPoints) {
         sprintf(&pszBuf[nLen], "%1dpt",
               SzUnitToPointSize(pProp->text_sz_unit));
      } else {
         sprintf(&pszBuf[nLen], "%1d", SzUnitToFontSize(pProp->text_sz_unit));
      }
      break;
   case PROP_MASK_VSPACE: sprintf(&pszBuf[nLen], "%1d", pProp->v_space); break;
   case PROP_MASK_UNDERLINE_ON:
      sprintf(&pszBuf[nLen], "%1d", pProp->underline_on);
      break;
   case PROP_MASK_OVERLINE_ON:
      sprintf(&pszBuf[nLen], "%1d", pProp->overline_on);
      break;

   case PROP_MASK_TEXT_FONT:
      *font_str = '\0';
      GetPSFontStr(pProp->text_font, pProp->text_style, font_str);
      /* font_str starts with the '/' character */
      strcat(pszBuf, &font_str[1]);
      break;
   }
}

static
int GetPropSelection(pp_dsp_ptr, ppsz_entries, pn_num_entries, pn_marked_index,
      pp_check_array, cur_buf, p_void)
   DspList **pp_dsp_ptr;
   char ***ppsz_entries, *cur_buf;
   int *pn_num_entries, *pn_marked_index;
   struct CheckArrayRec **pp_check_array;
   void *p_void;
{
   struct PropertiesRec *pProp=(struct PropertiesRec *)p_void;
   struct PropInfoRec *ppir=NULL;
   int num_rows=0;
   char **ppsz=NULL;

   for (ppir=gstPropInfo; ppir->bit != 0L; ppir++) {
      if (ppir->checked) {
         num_rows++;
      }
   }
   *ppsz_entries = (char**)malloc(num_rows*sizeof(char*));
   if (*ppsz_entries == NULL) FailAllocMessage();
   memset(*ppsz_entries, 0, num_rows*sizeof(char*));

   for (ppir=gstPropInfo, ppsz=(*ppsz_entries); ppir->bit != 0L; ppir++) {
      if (ppir->checked) {
         char szBuf[MAXSTRING];

         *szBuf = '\0';
         FormatPropForDisplay(ppir->bit, pProp, ppir, szBuf);
         *ppsz = UtilStrDup(szBuf);
         if ((*ppsz) == NULL) FailAllocMessage();
         ppsz++;
      }
   }
   *pn_num_entries = num_rows;
   return TRUE;
}

static
int DoCopyProperties(pCheckArray, pProp)
   /*
    * Returns FALSE if nothing copied.
    */
   struct CheckArrayRec *pCheckArray;
   struct PropertiesRec *pProp;
{
   struct PropInfoRec *ppir=NULL;
   int index=0, count=0;
   char *pszKeys=NULL;

   tgWriteProfileString(gszCopyPasteSec, NULL, NULL, gszPropIniFile);
   for (ppir=gstPropInfo; ppir->bit != 0L; ppir++) {
      if (ppir->checked) {
         if (pCheckArray->value[0][index]) {
            WritePropToIni(ppir->bit, gszCopyPasteSec, ppir->key, pProp);
            count++;
         }
         index++;
      }
   }
   tgWriteProfileString(NULL, NULL, NULL, gszPropIniFile);
   if (count == 0) {
      strcpy(gszMsgBox, TgLoadString(STID_NO_PROPERTY_TO_COPY));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   pszKeys = tgGetProfileString(gszCopyPasteSec, NULL, gszPropIniFile);
   if (pszKeys != NULL) {
      int total=(strlen(gszCopyPasteSec)+4), offset=0;
      char *pszKey=NULL;
      char *buf=(char*)malloc(total+1); /* sizeof buf is always total+1 */

      if (buf == NULL) FailAllocMessage();
      memset(buf, 0, total+1);
      *buf = TGIF_HEADER;
      sprintf(&buf[1], "[%s]", gszCopyPasteSec);
      offset = total;

      for (pszKey=pszKeys; *pszKey != '\0'; pszKey++) {
         int key_len=strlen(pszKey);
         char *pszValue=tgGetProfileString(gszCopyPasteSec, pszKey,
               gszPropIniFile);

         if (pszValue != NULL) {
            int value_len=strlen(pszValue);

            buf = (char*)realloc(buf, total+key_len+value_len+3);
            if (buf == NULL) FailAllocMessage();
            sprintf(&buf[offset], "%s=%s", pszKey, pszValue);

            offset += key_len+value_len+2;
            total += key_len+value_len+2;

            tgFreeProfileString(pszValue);
         } else {
            buf = (char*)realloc(buf, total+key_len+3);
            if (buf == NULL) FailAllocMessage();
            sprintf(&buf[offset], "%s=", pszKey);

            offset += key_len+2;
            total += key_len+2;
         }
         pszKey += key_len;
      }
      buf[total] = '\0';
      tgFreeProfileString(pszKeys);

      if (!WriteBufToCutBuffer(buf, total+1, TRUE, FALSE, NULL)) {
         strcpy(gszMsgBox, TgLoadString(STID_COPY_FAILED_OBJ_MAYBE_TOO_BIG));
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else {
         strcpy(gszMsgBox, TgLoadString(STID_COPY_BUFFER_UPDATED));
         Msg(gszMsgBox);
      }
      free(buf);
   }
   return TRUE;
}

static
int PropSetExists(pszPropSet)
   char *pszPropSet;
{
   int nReturn=FALSE;
   char *pszKeys=tgGetProfileString(gszPropSetSec, NULL, gszPropIniFile);

   if (pszKeys != NULL) {
      char *pszKey=NULL;

      for (pszKey=pszKeys; *pszKey != '\0'; pszKey++) {
         if (UtilStrICmp(pszPropSet, pszKey) == 0) {
            nReturn = TRUE;
            break;
         }
         pszKey += strlen(pszKey);
      }
      tgFreeProfileString(pszKeys);
   }
   return nReturn;
}

static
int DoSaveProperties(pCheckArray, pProp)
   /*
    * Returns FALSE if nothing saved.
    */
   struct CheckArrayRec *pCheckArray;
   struct PropertiesRec *pProp;
{
   char szPropSet[MAXSTRING], szPropSetSec[MAXSTRING];
   struct PropInfoRec *ppir=NULL;
   int index=0, count=0;

   *szPropSet = *szPropSetSec = '\0';
   strcpy(gszMsgBox, TgLoadString(STID_ENTER_NAME_FOR_PROP_SET));
   if (Dialog(gszMsgBox, NULL, szPropSet) == INVALID) return FALSE;
   UtilTrimBlanks(szPropSet);
   if (*szPropSet == '\0') return FALSE;
   if (PropSetExists(szPropSet)) {
      sprintf(gszMsgBox, TgLoadString(STID_PROP_SET_EXISTS_OVERWRITE_YNC),
            szPropSet);
      if (MsgBox(gszMsgBox, TOOL_NAME, YNC_MB) != MB_ID_YES) {
         return FALSE;
      }
   }
   sprintf(szPropSetSec, "%s: %s", gszPropProfilePrefix, szPropSet);
   tgWriteProfileString(gszPropSetSec, szPropSet, "", gszPropIniFile);
   tgWriteProfileString(szPropSetSec, NULL, NULL, gszPropIniFile);
   for (ppir=gstPropInfo; ppir->bit != 0L; ppir++) {
      if (ppir->checked) {
         if (pCheckArray->value[0][index]) {
            WritePropToIni(ppir->bit, szPropSetSec, ppir->key, pProp);
            count++;
         }
         index++;
      }
   }
   tgWriteProfileString(NULL, NULL, NULL, gszPropIniFile);
   if (count == 0) {
      strcpy(gszMsgBox, TgLoadString(STID_NO_PROP_TO_SAVE));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   return TRUE;
}

static
int DoPasteProperties(pCheckArray, pProp)
   /*
    * Returns FALSE if nothing pasted.
    */
   struct CheckArrayRec *pCheckArray;
   struct PropertiesRec *pProp;
{
   struct PropInfoRec *ppir=NULL;
   int index=0, count=0;
   int paste_sel=FALSE;

   if ((curChoice == NOTHING || curChoice == VERTEXMODE ||
         curChoice == ROTATEMODE)) {
      paste_sel = (topSel != NULL);
   }
   for (ppir=gstPropInfo; ppir->bit != 0L; ppir++) {
      if (ppir->checked) {
         if (pCheckArray->value[0][index]) {
            count++;
         }
         index++;
      }
   }
   if (count == 0) {
      strcpy(gszMsgBox, TgLoadString(STID_NO_PROPERTY_TO_PASTE));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   if (paste_sel) {
      HighLightReverse();
      StartCompositeCmd();
   }
   for (ppir=gstPropInfo, index=0; ppir->bit != 0L; ppir++) {
      if (ppir->checked) {
         if (pCheckArray->value[0][index]) {
            DoPasteAProperty(ppir->bit, pProp);
         }
         index++;
      }
   }
   if (paste_sel) {
      EndCompositeCmd();
      HighLightForward();
   }
   return TRUE;
}

#define COPY_PROP    0
#define PASTE_PROP   1
#define SAVE_PROP    2
#define RESTORE_PROP 3

static
int FreePropSelection(pp_dsp_ptr, ppsz_entries, pn_num_entries, pn_marked_index,
      pp_check_array, cur_buf, btn_id, selected_index, p_void)
   DspList **pp_dsp_ptr;
   char ***ppsz_entries, *cur_buf;
   int *pn_num_entries, *pn_marked_index, btn_id, selected_index;
   struct CheckArrayRec **pp_check_array;
   void *p_void;
{
   struct PropertiesRec *pProp=(struct PropertiesRec *)p_void;
   struct PropInfoRec *ppir=NULL;
   int nReturn=TRUE;

   switch ((int)(long)(pProp->userdata)) {
   case COPY_PROP:
      if (btn_id == COPY_BUTTON) {
         nReturn = !DoCopyProperties(*pp_check_array, pProp);
      }
      break;
   case SAVE_PROP:
      if (btn_id == SAVE_BUTTON) {
         nReturn = !DoSaveProperties(*pp_check_array, pProp);
      }
      break;
   case PASTE_PROP:
      if (btn_id == PASTE_BUTTON) {
         nReturn = !DoPasteProperties(*pp_check_array, pProp);
      }
      break;
   case RESTORE_PROP:
      if (btn_id == RESTORE_BUTTON) {
         nReturn = !DoPasteProperties(*pp_check_array, pProp);
      }
      break;
   }
   if (*ppsz_entries != NULL) {
      char **ppsz=(*ppsz_entries);

      for (ppir=gstPropInfo; ppir->bit != 0L; ppir++) {
         if (ppir->checked) {
            UtilFree(*ppsz);
            ppsz++;
         }
      }
      free(*ppsz_entries);
      *ppsz_entries = NULL;
   }
   return nReturn;
}

static
int PropSelectionCheckUpdate(pp_dsp_ptr, ppsz_entries, pn_num_entries,
      pn_marked_index, pp_check_array, cur_buf, col, row, p_void)
   DspList **pp_dsp_ptr;
   char ***ppsz_entries, *cur_buf;
   int *pn_num_entries, *pn_marked_index, col, row;
   struct CheckArrayRec **pp_check_array;
   void *p_void;
{
   struct PropertiesRec *pProp=(struct PropertiesRec *)p_void;
   char *psz=NULL, saved_ch='\0', *buf=NULL;
   int checked=FALSE;

   if (ppsz_entries == NULL || *ppsz_entries == NULL ||
         (*ppsz_entries)[row] == NULL || pp_check_array == NULL ||
         (*pp_check_array) == NULL || (*pp_check_array)->value == NULL) {
      return FALSE;
   }
   buf = (*ppsz_entries)[row];
   checked = (*pp_check_array)->value[0][row];

   psz = strchr(buf, ':');
   if (psz != NULL) {
      saved_ch = *psz;
      *psz = '\0';
      switch ((int)(long)(pProp->userdata)) {
      case COPY_PROP:
         sprintf(gszMsgBox, TgLoadString(checked ? STID_WILL_COPY_NAMED_PROP :
               STID_WILL_NOT_COPY_NAMED_PROP), buf);
         break;
      case SAVE_PROP:
         sprintf(gszMsgBox, TgLoadString(checked ? STID_WILL_SAVE_NAMED_PROP :
               STID_WILL_NOT_SAVE_NAMED_PROP), buf);
         break;
      case PASTE_PROP:
         sprintf(gszMsgBox, TgLoadString(checked ? STID_WILL_PASTE_NAMED_PROP :
               STID_WILL_NOT_PASTE_NAMED_PROP), buf);
         break;
      case RESTORE_PROP:
         sprintf(gszMsgBox, TgLoadString(checked ?
               STID_WILL_RESTORE_NAMED_PROP : STID_WILL_NOT_RESTORE_NAMED_PROP),
               buf);
         break;
      }
      *psz = saved_ch;
   }
   SetStringStatus(gszMsgBox);

   return TRUE;
}

static
void SelectProperties(pszTitle, nWhich, pCheckArray, pProp)
   char *pszTitle;
   int nWhich;
   struct CheckArrayRec *pCheckArray;
   struct PropertiesRec *pProp;
{
   char win_name[128];

   ResetNamesInfo();

   NamesSetTitle(pszTitle);
   switch (nWhich) {
   case COPY_PROP:
      NamesAddButton(TgLoadCachedString(CSTID_COPY), COPY_BUTTON);
      NamesAddButton(TgLoadCachedString(CSTID_CANCEL), BUTTON_CANCEL);
      /* ignore double-click and <CR> */
      NamesSetDefaultBtnId(COPY_BUTTON, INVALID);
      NamesSetEntries(NULL, NULL, 0, pCheckArray, TRUE, INVALID, 0);
      NamesSetStyle(NAMES_SIMPLE_SELECT_NAME, NAMES_LOOP_MANY);
      NamesSetCallback((GetEntriesFunc*)GetPropSelection,
            (AfterLoopFunc*)FreePropSelection,
            (CheckUpdateFunc*)PropSelectionCheckUpdate);
      sprintf(win_name, TgLoadString(STID_TOOL_COPY_PROP), TOOL_NAME);
      break;
   case SAVE_PROP:
      NamesAddButton(TgLoadCachedString(CSTID_SAVE), SAVE_BUTTON);
      NamesAddButton(TgLoadCachedString(CSTID_CANCEL), BUTTON_CANCEL);
      /* ignore double-click and <CR> */
      NamesSetDefaultBtnId(SAVE_BUTTON, INVALID);
      NamesSetEntries(NULL, NULL, 0, pCheckArray, TRUE, INVALID, 0);
      NamesSetStyle(NAMES_SIMPLE_SELECT_NAME, NAMES_LOOP_MANY);
      NamesSetCallback((GetEntriesFunc*)GetPropSelection,
            (AfterLoopFunc*)FreePropSelection,
            (CheckUpdateFunc*)PropSelectionCheckUpdate);
      sprintf(win_name, TgLoadString(STID_TOOL_SAVE_PROP), TOOL_NAME);
      break;
   case PASTE_PROP:
      NamesAddButton(TgLoadCachedString(CSTID_PASTE), PASTE_BUTTON);
      NamesAddButton(TgLoadCachedString(CSTID_CANCEL), BUTTON_CANCEL);
      /* ignore double-click and <CR> */
      NamesSetDefaultBtnId(PASTE_BUTTON, INVALID);
      NamesSetEntries(NULL, NULL, 0, pCheckArray, TRUE, INVALID, 0);
      NamesSetStyle(NAMES_SIMPLE_SELECT_NAME, NAMES_LOOP_MANY);
      NamesSetCallback((GetEntriesFunc*)GetPropSelection,
            (AfterLoopFunc*)FreePropSelection,
            (CheckUpdateFunc*)PropSelectionCheckUpdate);
      sprintf(win_name, TgLoadString(STID_TOOL_PASTE_PROP), TOOL_NAME);
      break;
   case RESTORE_PROP:
      NamesAddButton(TgLoadCachedString(CSTID_RESTORE), RESTORE_BUTTON);
      NamesAddButton(TgLoadCachedString(CSTID_CANCEL), BUTTON_CANCEL);
      /* ignore double-click and <CR> */
      NamesSetDefaultBtnId(RESTORE_BUTTON, INVALID);
      NamesSetEntries(NULL, NULL, 0, pCheckArray, TRUE, INVALID, 0);
      NamesSetStyle(NAMES_SIMPLE_SELECT_NAME, NAMES_LOOP_MANY);
      NamesSetCallback((GetEntriesFunc*)GetPropSelection,
            (AfterLoopFunc*)FreePropSelection,
            (CheckUpdateFunc*)PropSelectionCheckUpdate);
      sprintf(win_name, TgLoadString(STID_TOOL_RESTORE_PROP), TOOL_NAME);
      break;
   }
   Names(win_name, NULL, NULL, 0, pProp);
}

static
int PrepareToCopyProperties(pProp, lMask, lSkip, pCheckArray)
   struct PropertiesRec *pProp;
   long lMask, lSkip;
   struct CheckArrayRec *pCheckArray;
{
   struct PropInfoRec *ppir=NULL;
   int index=0, num_rows=0;

   for (ppir=gstPropInfo; ppir->bit != 0L; ppir++) {
      ppir->checked = FALSE;
      if ((lMask & ppir->bit) != 0L && (lSkip & ppir->bit) == 0) {
         num_rows++;
      }
   }
   pCheckArray->num_cols = 1;
   pCheckArray->num_rows = num_rows;

   pCheckArray->value = (int**)malloc(sizeof(int*));
   if (pCheckArray->value == NULL) FailAllocMessage();
   memset(pCheckArray->value, 0, sizeof(int*));

   pCheckArray->value[0] = (int*)malloc(num_rows*sizeof(int));
   if (pCheckArray->value[0] == NULL) FailAllocMessage();
   memset(pCheckArray->value[0], 0, num_rows*sizeof(int));

   for (ppir=gstPropInfo, index=0; ppir->bit != 0L; ppir++) {
      long bit=ppir->bit;

      if ((lMask & bit) != 0L && (lSkip & bit) == 0) {
         ppir->checked = TRUE;
         pCheckArray->value[0][index++] = TRUE;
      }
   }
   return TRUE;
}

static
int PrepareToPasteProperties(pszSec, plMask, pCheckArray, pProp)
   char *pszSec;
   struct PropertiesRec *pProp;
   long *plMask;
   struct CheckArrayRec *pCheckArray;
{
   struct PropInfoRec *ppir=NULL;
   int index=0, num_rows=0;

   for (ppir=gstPropInfo; ppir->bit != 0L; ppir++) {
      ppir->checked = FALSE;
   }
   if (!ReadPropFromIni(pszSec, plMask, pCheckArray, pProp)) {
      return FALSE;
   }
   num_rows = pCheckArray->num_rows;

   pCheckArray->value = (int**)malloc(sizeof(int*));
   if (pCheckArray->value == NULL) FailAllocMessage();
   memset(pCheckArray->value, 0, sizeof(int*));

   pCheckArray->value[0] = (int*)malloc(num_rows*sizeof(int));
   if (pCheckArray->value[0] == NULL) FailAllocMessage();
   memset(pCheckArray->value[0], 0, num_rows*sizeof(int));

   for (ppir=gstPropInfo, index=0; ppir->bit != 0L; ppir++) {
      long bit=ppir->bit;

      if (((*plMask) & bit) != 0L) {
         ppir->checked = TRUE;
         pCheckArray->value[0][index++] = TRUE;
      }
   }
   return TRUE;
}

static
int SetupProperties(pProp, plMask, plSkip, pCheckArray, nWhich)
   struct PropertiesRec *pProp;
   long *plMask, *plSkip;
   struct CheckArrayRec *pCheckArray;
   int nWhich;
{
   int copy_sel=FALSE;

   pProp->userdata = (void*)(long)nWhich;
   if ((curChoice == NOTHING || curChoice == VERTEXMODE ||
         curChoice == ROTATEMODE)) {
      if (topSel == NULL) {
         copy_sel = FALSE;
      } else if (topSel == botSel) {
         copy_sel = TRUE;
      } else {
         switch (nWhich) {
         case COPY_PROP:
            strcpy(gszMsgBox, TgLoadString(STID_SEL_ONLY_ONE_FOR_COPY_PROP));
            break;
         case SAVE_PROP:
            strcpy(gszMsgBox, TgLoadString(STID_SEL_ONLY_ONE_FOR_SAVE_PROP));
            break;
         }
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return FALSE;
      }
   }
   if (copy_sel) {
      SetPropMask(topSel->obj, plMask, plSkip, pProp);
   } else {
      SetPropMask(NULL, plMask, plSkip, pProp);
   }
   /* shouldn't do this if we support multiple objects */
   *plSkip = ~((*plMask) | PROP_MASK_WIDTH_INDEX);

   if (pCheckArray != NULL) {
      return PrepareToCopyProperties(pProp, *plMask, *plSkip, pCheckArray);
   }
   return TRUE;
}

static
void FixMasksForGetProperty(pProp, plMask, plSkip)
   struct PropertiesRec *pProp;
   long *plMask, *plSkip;
{
   long lMask=(*plMask);

   *plSkip |= PROP_MASK_CTM;

   if ((lMask & PROP_MASK_WIDTH) == PROP_MASK_WIDTH) {
      int i=0;
      char *width_spec=pProp->width_spec;

      if (((lMask & PROP_MASK_AW) == PROP_MASK_AW) &&
            ((lMask & PROP_MASK_AH) == PROP_MASK_AH)) {
         char *aw_spec=pProp->aw_spec;
         char *ah_spec=pProp->ah_spec;

         *plSkip |= (PROP_MASK_WIDTH | PROP_MASK_AW | PROP_MASK_AH);
         for (i=0; i < maxLineWidths; i++) {
            if (strcmp(width_spec, curWidthOfLineSpec[i]) == 0 &&
                  strcmp(aw_spec, curArrowHeadWSpec[i]) == 0 &&
                  strcmp(ah_spec, curArrowHeadHSpec[i]) == 0) {
               pProp->width_index = i;
               *plMask |= PROP_MASK_WIDTH_INDEX;
               break;
            }
         }
      } else {
         *plSkip |= (PROP_MASK_WIDTH);
         for (i=0; i < maxLineWidths; i++) {
            if (strcmp(width_spec, curWidthOfLineSpec[i]) == 0) {
               pProp->width_index = i;
               *plMask |= PROP_MASK_WIDTH_INDEX;
               break;
            }
         }
      }
   }
}

void DoGetProperty(target_index)
   int target_index;
{
   struct PropertiesRec properties;
   struct PropInfoRec *ppir=NULL;
   long lMask=0L, lSkip=0L;
   int index=0;
   struct SelRec *saved_top_sel=NULL, *saved_bot_sel=NULL;

   memset(&properties, 0, sizeof(struct PropertiesRec));
   if (!SetupProperties(&properties, &lMask, &lSkip, NULL, COPY_PROP)) {
      return;
   }
   FixMasksForGetProperty(&properties, &lMask, &lSkip);

   HighLightReverse();
   saved_top_sel = topSel;
   saved_bot_sel = botSel;
   topSel = botSel = NULL;
   UpdSelBBox();
   for (ppir=gstPropInfo; ppir->bit != 0L; ppir++) {
      if ((lMask & ppir->bit) != 0L && (lSkip & ppir->bit) == 0) {
         if (index == target_index) {
            /* do something */
            DoGetAProperty(ppir->bit, &properties);
            break;
         }
         index++;
      }
   }
   topSel = saved_top_sel;
   botSel = saved_bot_sel;
   UpdSelBBox();
   HighLightReverse();
}

EditAttrInfo *CreateGetPropertyInfo()
{
   struct PropertiesRec properties;
   struct PropInfoRec *ppir=NULL;
   EditAttrInfo *pEditAttrInfo=NULL;
   long lMask=0L, lSkip=0L;
   int index=0, num_attrs=0;
   char **attr_strings=NULL, **status_strings=NULL;

   pEditAttrInfo = (EditAttrInfo*)malloc(sizeof(EditAttrInfo));
   if (pEditAttrInfo == NULL) FailAllocMessage();
   memset(pEditAttrInfo, 0, sizeof(EditAttrInfo));

   memset(&properties, 0, sizeof(struct PropertiesRec));
   if (!SetupProperties(&properties, &lMask, &lSkip, NULL, COPY_PROP)) {
      return NULL;
   }
   FixMasksForGetProperty(&properties, &lMask, &lSkip);

   for (ppir=gstPropInfo; ppir->bit != 0L; ppir++) {
      ppir->checked = FALSE;
      if ((lMask & ppir->bit) != 0L && (lSkip & ppir->bit) == 0) {
         num_attrs++;
      }
   }
   attr_strings = (char**)malloc(num_attrs*sizeof(char*));
   status_strings = (char**)malloc(num_attrs*sizeof(char*));
   if (attr_strings == NULL || status_strings == NULL) FailAllocMessage();
   memset(attr_strings, 0, num_attrs*sizeof(char*));
   memset(status_strings, 0, num_attrs*sizeof(char*));

   for (ppir=gstPropInfo, index=0; ppir->bit != 0L; ppir++) {
      long bit=ppir->bit;

      if ((lMask & bit) != 0L && (lSkip & bit) == 0) {
         char *psz=NULL, szBuf[MAXSTRING];

         *szBuf = '\0';
         FormatPropForDisplay(ppir->bit, &properties, ppir, szBuf);
         attr_strings[index] = UtilStrDup(szBuf);
         if (attr_strings[index] == NULL) FailAllocMessage();
         if ((psz=strchr(szBuf, ':')) != NULL) {
            *psz++ = '\0';
            UtilTrimBlanks(psz);
            sprintf(gszMsgBox,
                  TgLoadCachedString(CSTID_GET_NAMED_PROP_FROM_SEL_OBJ), szBuf);
            status_strings[index] = UtilStrDup(gszMsgBox);
            if (status_strings[index] == NULL) FailAllocMessage();
         }
         ppir->checked = TRUE;
         index++;
      }
   }
   pEditAttrInfo->num_attrs = num_attrs;
   pEditAttrInfo->fore_colors = NULL;
   pEditAttrInfo->attr_indices = NULL;
   pEditAttrInfo->attr_names = NULL;
   pEditAttrInfo->attr_values = NULL;
   pEditAttrInfo->attr_strings = attr_strings;
   pEditAttrInfo->status_strings = status_strings;

   return pEditAttrInfo;
}

void CopyProperties(nPrompt)
   int nPrompt;
{
   struct PropertiesRec properties;
   long lMask=0L, lSkip=0L;
   struct CheckArrayRec check_array;

   memset(&properties, 0, sizeof(struct PropertiesRec));
   memset(&check_array, 0, sizeof(struct CheckArrayRec));
   if (!SetupProperties(&properties, &lMask, &lSkip, &check_array, COPY_PROP)) {
      return;
   }
   tgWriteProfileString(gszCopyPasteSec, NULL, NULL, gszPropIniFile);
   tgWriteProfileString(NULL, NULL, NULL, gszPropIniFile);
   if (nPrompt) {
      SelectProperties(TgLoadString(STID_UNCHECK_PROP_FOR_COPY_DOTS),
            COPY_PROP, &check_array, &properties);
   } else {
      DoCopyProperties(&check_array, &properties);
   }
   CleanUpCheckArray(&check_array);
}

void SaveProperties()
{
   struct PropertiesRec properties;
   long lMask=0L, lSkip=0L;
   struct CheckArrayRec check_array;

   memset(&properties, 0, sizeof(struct PropertiesRec));
   memset(&check_array, 0, sizeof(struct CheckArrayRec));
   if (!SetupProperties(&properties, &lMask, &lSkip, &check_array, SAVE_PROP)) {
      return;
   }
   SelectProperties(TgLoadString(STID_UNCHECK_PROP_FOR_SAVE_DOTS),
         SAVE_PROP, &check_array, &properties);
   CleanUpCheckArray(&check_array);
}

static
int WriteBufToIni(buf, ini_fname)
   char *buf, *ini_fname;
{
   char *pszSec=buf, *pszKey=NULL, *pszKeyStart=NULL;
   int len=strlen(pszSec);

   pszKeyStart = (&buf[len+1]);
   if (*pszSec == '[' && pszSec[len-1] == ']') {
      pszSec[len-1] = '\0';
      pszSec++;
   }
   tgWriteProfileString(pszSec, NULL, NULL, ini_fname);

   for (pszKey=pszKeyStart; *pszKey != '\0'; pszKey++) {
      char *psz=strchr(pszKey, '=');

      if (psz == NULL) {
         tgWriteProfileString(pszSec, NULL, NULL, ini_fname);
         tgWriteProfileString(NULL, NULL, NULL, ini_fname);
         return FALSE;
      }
      *psz = '\0';
      tgWriteProfileString(pszSec, pszKey, &psz[1], ini_fname);
      *psz = '=';

      pszKey += strlen(pszKey);
   }
   tgWriteProfileString(NULL, NULL, NULL, ini_fname);

   return TRUE;
}

void PasteProperties(nPrompt)
   int nPrompt;
{
   struct PropertiesRec properties;
   long lMask=0L, lSkip=0L;
   struct CheckArrayRec check_array;
   int len=0, from_selection=FALSE;
   char *cut_buffer=NULL;

   cut_buffer = FetchSelectionOrCutBuffer(&len, &from_selection);
   if (cut_buffer == NULL) {
      MsgBox(TgLoadString(STID_CUT_BUFFER_EMPTY), TOOL_NAME, INFO_MB);
      return;
   }
   sprintf(gszMsgBox, "[%s]", gszCopyPasteSec);
   if (((unsigned char)(*cut_buffer)) != TGIF_HEADER ||
         strcmp(&cut_buffer[1], gszMsgBox) != 0) {
      FreeSelectionOrCutBuffer(cut_buffer, from_selection);
      MsgBox(TgLoadString(STID_COPY_PROP_BEFORE_PASTE), TOOL_NAME, INFO_MB);
      return;
   }
   if (!WriteBufToIni(&cut_buffer[1], gszPropIniFile)) {
      strcpy(gszMsgBox, TgLoadString(STID_MALFORMED_CUT_BUF_WHILE_PASTE));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);

      FreeSelectionOrCutBuffer(cut_buffer, from_selection);
      return;
   }
   FreeSelectionOrCutBuffer(cut_buffer, from_selection);

   memset(&check_array, 0, sizeof(struct CheckArrayRec));
   memset(&properties, 0, sizeof(struct PropertiesRec));
   properties.userdata = (void*)PASTE_PROP;
   if (!PrepareToPasteProperties(gszCopyPasteSec, &lMask, &check_array,
         &properties)) {
      return;
   }
   lSkip = (~lMask);

   if (nPrompt) {
      SelectProperties(TgLoadString(STID_UNCHECK_PROP_FOR_PASTE_DOTS),
            PASTE_PROP, &check_array, &properties);
   } else {
      DoPasteProperties(&check_array, &properties);
   }
   CleanUpCheckArray(&check_array);
}

static
int GetPropSetNames(pp_dsp_ptr, ppsz_entries, pn_num_entries, pn_marked_index,
      pp_check_array, cur_buf, p_void)
   DspList **pp_dsp_ptr;
   char ***ppsz_entries, *cur_buf;
   int *pn_num_entries, *pn_marked_index;
   struct CheckArrayRec **pp_check_array;
   void *p_void;
{
   int num_rows=0;
   char **ppsz=NULL, *pszName=NULL;
   char *pszNames=tgGetProfileString(gszPropSetSec, NULL, gszPropIniFile);

   if (pszNames == NULL) {
      strcpy(gszMsgBox, TgLoadString(STID_NO_PROP_SETS_TO_RESTORE));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   for (pszName=pszNames; *pszName != '\0'; pszName++, num_rows++) {
      pszName += strlen(pszName);
   }
   if (num_rows == 0) {
      tgWriteProfileString(gszPropSetSec, NULL, NULL, gszPropIniFile);
      tgWriteProfileString(NULL, NULL, NULL, gszPropIniFile);
      strcpy(gszMsgBox, TgLoadString(STID_NO_PROP_SETS_TO_RESTORE));
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   *ppsz_entries = (char**)malloc(num_rows*sizeof(char*));
   if (*ppsz_entries == NULL) FailAllocMessage();
   memset(*ppsz_entries, 0, num_rows*sizeof(char*));

   for (pszName=pszNames, ppsz=(*ppsz_entries); *pszName != '\0'; pszName++) {
      *ppsz = UtilStrDup(pszName);
      if ((*ppsz) == NULL) FailAllocMessage();
      ppsz++;

      pszName += strlen(pszName);
   }
   tgFreeProfileString(pszNames);
   *pn_num_entries = num_rows;
   return TRUE;
}

static
int FreePropSetNames(pp_dsp_ptr, ppsz_entries, pn_num_entries, pn_marked_index,
      pp_check_array, cur_buf, btn_id, selected_index, p_void)
   DspList **pp_dsp_ptr;
   char ***ppsz_entries, *cur_buf;
   int *pn_num_entries, *pn_marked_index, btn_id, selected_index;
   struct CheckArrayRec **pp_check_array;
   void *p_void;
{
   int nReturn=TRUE;

   if (btn_id == BUTTON_OK) {
      nReturn = FALSE;
   }
   if (*ppsz_entries != NULL) {
      char **ppsz=(*ppsz_entries);
      int i=0;

      for (i=0; i < *pn_num_entries; i++) {
         UtilFree(*ppsz);
         ppsz++;
      }
      free(*ppsz_entries);
      *ppsz_entries = NULL;
   }
   return nReturn;
}

static
int SelectPropSetForRestore(pszTitle, pszPropSet, nPropSetSize)
   char *pszTitle, *pszPropSet;
   int nPropSetSize;
{
   char win_name[128];

   ResetNamesInfo();

   NamesSetTitle(pszTitle);
   NamesAddButton(TgLoadCachedString(CSTID_OK), BUTTON_OK);
   NamesAddButton(TgLoadCachedString(CSTID_CANCEL), BUTTON_CANCEL);
   NamesSetDefaultBtnId(BUTTON_OK, BUTTON_OK);
   NamesSetEntries(NULL, NULL, 0, NULL, TRUE, INVALID, 0);
   NamesSetStyle(NAMES_SIMPLE_SELECT_NAME, NAMES_LOOP_MANY);
   NamesSetCallback((GetEntriesFunc*)GetPropSetNames,
         (AfterLoopFunc*)FreePropSetNames, NULL);
   sprintf(win_name, TgLoadString(STID_TOOL_RESTORE_PROP), TOOL_NAME);

   Names(win_name, NULL, pszPropSet, nPropSetSize, pszPropSet);
   return (*pszPropSet != '\0');
}

void RestoreProperties()
{
   struct PropertiesRec properties;
   long lMask=0L, lSkip=0L;
   struct CheckArrayRec check_array;
   char szPropSet[MAXSTRING], szPropSetSec[MAXSTRING];

   memset(&check_array, 0, sizeof(struct CheckArrayRec));
   memset(&properties, 0, sizeof(struct PropertiesRec));
   properties.userdata = (void*)RESTORE_PROP;
   *szPropSet = '\0';
   if (!SelectPropSetForRestore(TgLoadString(STID_SEL_A_PROP_SET_TO_RESTORE),
         szPropSet, sizeof(szPropSet))) { 
      return;
   }
   sprintf(szPropSetSec, "%s: %s", gszPropProfilePrefix, szPropSet);
   if (!PrepareToPasteProperties(szPropSetSec, &lMask, &check_array,
         &properties)) {
      return;
   }
   lSkip = (~lMask);

   SelectProperties(TgLoadString(STID_UNCHECK_PROP_FOR_RESTORE_DOTS),
         RESTORE_PROP, &check_array, &properties);
   CleanUpCheckArray(&check_array);
}

void BackupCopiedProperties()
{
   char *pszKeys=tgGetProfileString(gszCopyPasteSec, NULL, gszPropIniFile);

   tgWriteProfileString(gszCopyPasteBackupSec, NULL, NULL, gszPropIniFile);
   if (pszKeys != NULL) {
      char *pszKey=NULL;

      for (pszKey=pszKeys; *pszKey != '\0'; pszKey++) {
         char *pszValue=tgGetProfileString(gszCopyPasteSec, pszKey,
               gszPropIniFile);

         if (pszValue != NULL) {
            tgWriteProfileString(gszCopyPasteBackupSec, pszKey, pszValue,
                  gszPropIniFile);
            tgFreeProfileString(pszValue);
         }
         pszKey += strlen(pszKey);
      }
      tgFreeProfileString(pszKeys);
   }
   tgWriteProfileString(NULL, NULL, NULL, gszPropIniFile);
}

void RestoreCopiedProperties()
{
   char *pszKeys=tgGetProfileString(gszCopyPasteBackupSec, NULL,
         gszPropIniFile);

   tgWriteProfileString(gszCopyPasteSec, NULL, NULL, gszPropIniFile);
   if (pszKeys != NULL) {
      char *pszKey=NULL;

      for (pszKey=pszKeys; *pszKey != '\0'; pszKey++) {
         char *pszValue=tgGetProfileString(gszCopyPasteBackupSec, pszKey,
               gszPropIniFile);

         if (pszValue != NULL) {
            tgWriteProfileString(gszCopyPasteSec, pszKey, pszValue,
                  gszPropIniFile);
            tgFreeProfileString(pszValue);
         }
         pszKey += strlen(pszKey);
      }
      tgFreeProfileString(pszKeys);
   }
   tgWriteProfileString(NULL, NULL, NULL, gszPropIniFile);
}

static
DspList *IniSectionListing(pszSection, pszIniFile, pnEntries)
   char *pszSection, *pszIniFile;
   int *pnEntries;
{
   DspList *dsp_ptr=NULL;
   char *pszKeys=NULL;
   int num_entries=0;

   *pnEntries = 0;
   if ((pszKeys=tgGetProfileString(pszSection, NULL, pszIniFile)) != NULL) {
      DspList *pdl=NULL;
      char *pszKey=NULL;

      for (pszKey=pszKeys; *pszKey != '\0'; pszKey++) {
         num_entries++;
         pszKey += strlen(pszKey);
      }
      dsp_ptr = (DspList*)malloc(num_entries*sizeof(DspList));
      if (dsp_ptr == NULL) FailAllocMessage();
      memset(dsp_ptr, 0, num_entries*sizeof(DspList));
      for (pszKey=pszKeys, pdl=dsp_ptr; *pszKey != '\0'; pszKey++, pdl++) {
         char *pszValue=tgGetProfileString(pszSection, pszKey, pszIniFile);

         sprintf(gszMsgBox, "%s=%s", pszKey, (pszValue==NULL ? "" : pszValue));
         UtilStrCpyN(pdl->itemstr, sizeof(pdl->itemstr), gszMsgBox);
         /* use the directory field for inherited */
         pdl->directory = TRUE;
         pdl->next = (&pdl[1]);

         if (pszValue != NULL) tgFreeProfileString(pszValue);
         pszKey += strlen(pszKey);
      }
      tgFreeProfileString(pszKeys);
   }
   if (num_entries == 0) return NULL;

   dsp_ptr[num_entries-1].next = NULL;
   *pnEntries = num_entries;

   return dsp_ptr;
}

static
int EditIniSectionStrings(psz_title, dsp_ptr, entries, num_entries,
      pf_after_loop)
   char *psz_title, **entries;
   DspList *dsp_ptr;
   int num_entries;
   AfterLoopFunc *pf_after_loop;
{
   char win_name[128];

   sprintf(win_name, TgLoadString(STID_TOOL_EDIT_INI_SECTION), TOOL_NAME);
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

static
int DefEditIniSectionCheck(psz_key, psz_value)
   char *psz_key, *psz_value;
{
   return TRUE;
}

static
int EditIniSectionDefAfterLoop(pp_dsp_ptr, ppsz_entries, pn_num_entries,
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
      psz_value = (&psz[1]);
      if (!DefEditIniSectionCheck(psz_key, psz_value)) {
         /* psz_value is not an acceptable value for psz_key */
         sprintf(gszMsgBox, TgLoadString(STID_BAD_VALUE_FOR_KEY), psz_key,
               psz_value);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         *psz = '=';
         return TRUE;
      }
      *psz = '=';
   }
   /* everything is fine */
   return FALSE;
}

int EditIniSection(pszTitle, pszSection, pszIniFile, pf_after_loop)
   char *pszTitle, *pszSection, *pszIniFile;
   AfterLoopFunc *pf_after_loop;
{
   int num_entries=0, ok_pressed=FALSE;
   DspList *dsp_ptr=NULL;
   char sz_title[MAXSTRING], **entries=NULL;

   dsp_ptr = IniSectionListing(pszSection, pszIniFile, &num_entries);
   if (dsp_ptr == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_NOTHING_TO_EDIT_SEC_EMPTY),
            pszSection, pszIniFile);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   MakeQuiescent();

   ignoreDirectoryFlag = TRUE;
   entries = MakeNameDspItemArray(num_entries, dsp_ptr);
   ignoreDirectoryFlag = FALSE;
   if (pszTitle == NULL) {
      sprintf(sz_title, TgLoadString(STID_EDIT_VALUES_FOR_DOTS), pszSection);
   } else {
      UtilStrCpyN(sz_title, sizeof(sz_title), pszTitle);
   }
   if (pf_after_loop == NULL) {
      pf_after_loop = (AfterLoopFunc*)EditIniSectionDefAfterLoop;
   }
   if (EditIniSectionStrings(sz_title, dsp_ptr, entries, num_entries,
         pf_after_loop)) {
      int i=0;

      tgWriteProfileString(pszSection, NULL, NULL, pszIniFile);
      for (i=0; i < num_entries; i++) {
         char *psz=strchr(entries[i], '=');

         *psz++ = '\0';
         tgWriteProfileString(pszSection, entries[i], psz, pszIniFile);
      }
      tgWriteProfileString(NULL, NULL, NULL, pszIniFile);
      ok_pressed = TRUE;
   }
   free(dsp_ptr);
   free(*entries);
   free(entries);

   SetCurChoice(curChoiceBeforeMakeQuiescent);

   return ok_pressed;
}

static
DspList *IniSectionKeysListing(pszSection, pszIniFile, pnEntries)
   char *pszSection, *pszIniFile;
   int *pnEntries;
{
   DspList *dsp_ptr=NULL;
   char *pszKeys=NULL;
   int num_entries=0;

   *pnEntries = 0;
   if ((pszKeys=tgGetProfileString(pszSection, NULL, pszIniFile)) != NULL) {
      DspList *pdl=NULL;
      char *pszKey=NULL;

      for (pszKey=pszKeys; *pszKey != '\0'; pszKey++) {
         num_entries++;
         pszKey += strlen(pszKey);
      }
      dsp_ptr = (DspList*)malloc(num_entries*sizeof(DspList));
      if (dsp_ptr == NULL) FailAllocMessage();
      memset(dsp_ptr, 0, num_entries*sizeof(DspList));
      for (pszKey=pszKeys, pdl=dsp_ptr; *pszKey != '\0'; pszKey++, pdl++) {
         UtilStrCpyN(pdl->itemstr, sizeof(pdl->itemstr), pszKey);
         /* use the directory field for inherited */
         pdl->next = (&pdl[1]);

         pszKey += strlen(pszKey);
      }
      tgFreeProfileString(pszKeys);
   }
   if (num_entries == 0) return NULL;

   dsp_ptr[num_entries-1].next = NULL;
   *pnEntries = num_entries;

   return dsp_ptr;
}

static
int SelectFromIniSectionStrings(psz_title, dsp_ptr, entries, num_entries,
      selected_str, selected_str_sz)
   char *psz_title, **entries, *selected_str;
   DspList *dsp_ptr;
   int num_entries, selected_str_sz;
{
   char win_name[128];

   sprintf(win_name, TgLoadString(STID_TOOL_SEL_FROM_INI_SECTION), TOOL_NAME);
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

char *SelectFromIniSection(pszTitle, pszSection, pszIniFile)
   char *pszTitle, *pszSection, *pszIniFile;
{
   int num_entries=0;
   DspList *dsp_ptr=NULL;
   char sz_title[MAXSTRING], **entries=NULL, selected_str[MAXSTRING];

   MakeQuiescent();

   dsp_ptr = IniSectionKeysListing(pszSection, pszIniFile, &num_entries);
   if (dsp_ptr == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_NOTHING_TO_SEL_SEC_EMPTY),
            pszSection, pszIniFile);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   ignoreDirectoryFlag = TRUE;
   entries = MakeNameDspItemArray(num_entries, dsp_ptr);
   ignoreDirectoryFlag = FALSE;
   if (pszTitle == NULL) {
      sprintf(sz_title, TgLoadString(STID_SELECT_A_VALUE_FROM_SEC_DOTS),
            pszSection);
   } else {
      UtilStrCpyN(sz_title, sizeof(sz_title), pszTitle);
   }
   *selected_str = '\0';
   if (!SelectFromIniSectionStrings(sz_title, dsp_ptr, entries, num_entries,
         selected_str, sizeof(selected_str))) {
      *selected_str = '\0';
   }
   free(dsp_ptr);
   free(*entries);
   free(entries);

   return (*selected_str=='\0' ? NULL : UtilStrDup(selected_str));
}

/* ----------------------- Recently Used Files ----------------------- */

static int maxRecentFiles=10;

void FreeRecentFilesListing(pkvi, max_count)
   KeyValInfo *pkvi;
   int max_count;
{
   if (pkvi != NULL) {
      int i=0;

      for (i=0; i < max_count; i++) {
         UtilFree(pkvi[i].key);
         UtilFree(pkvi[i].value);
      }
      free(pkvi);
   }
}

KeyValInfo *RecentFilesListing(pnEntries)
   int *pnEntries;
   /*
    * key will store the file name
    * value will store the full path
    */
{
   int i=0, count=0, real_count=0, fix_up=FALSE;
   char *buf=NULL;
   KeyValInfo *pkvi=NULL;

   *pnEntries = 0;
   if ((buf=tgGetProfileString(gpszRecentFilesSec, gpszRecentFilesCountKey,
         gszFilesIniFile)) == NULL) {
      return NULL;
   }
   count = atoi(buf);
   tgFreeProfileString(buf);
   pkvi = (KeyValInfo*)malloc(count*sizeof(KeyValInfo));
   if (pkvi == NULL) FailAllocMessage();
   memset(pkvi, 0, count*sizeof(KeyValInfo));
   for (i=0; i < count; i++) {
      char sz_key[40], *psz=NULL;

      sprintf(sz_key, "%1d", i);
      buf = tgGetProfileString(gpszRecentFilesSec, sz_key, gszFilesIniFile);
      /* just in case the ini file is corrupted */
      if (buf == NULL) {
         fix_up = TRUE;
         continue;
      }
      psz = UtilStrRChr(buf, DIR_SEP);
      if (psz != NULL) {
         *psz = '\0';
         pkvi[real_count].key = UtilStrDup(&psz[1]);
         if (pkvi[real_count].key == NULL) FailAllocMessage();
         *psz = DIR_SEP;
      } else {
         pkvi[real_count].key = UtilStrDup(buf);
         if (pkvi[real_count].key == NULL) FailAllocMessage();
      }
      pkvi[real_count].value = UtilStrDup(buf);
      if (pkvi[real_count].value == NULL) FailAllocMessage();
      if (fix_up) {
         tgWriteProfileString(gpszRecentFilesSec, sz_key, buf, gszFilesIniFile);
      }
      tgFreeProfileString(buf);
      real_count++;
   }
   if (fix_up) {
      sprintf(gszMsgBox, "%1d", real_count);
      tgWriteProfileString(gpszRecentFilesSec, gpszRecentFilesCountKey,
            gszMsgBox, gszFilesIniFile);
      tgWriteProfileString(NULL, NULL, NULL, gszFilesIniFile);
   }
   if (real_count == 0) {
      FreeRecentFilesListing(pkvi, count);
      pkvi = NULL;
   }
   *pnEntries = real_count;

   return pkvi;
}

void OpenARecentlyUsedFile(file_index)
   int file_index;
{
   int count=0, do_not_save=FALSE, need_to_check_auto_exec=FALSE;
   KeyValInfo *pkvi=RecentFilesListing(&count);
   char *psz_url=NULL;

   if (pkvi == NULL || count == 0 || file_index >= count) return;

   psz_url = pkvi[file_index].value;

   if (!BeforeOpenURL(&do_not_save)) {
      return;
   }
   OpenURL(psz_url, do_not_save, &need_to_check_auto_exec);
   FreeRecentFilesListing(pkvi, count);
   AfterOpenURL(need_to_check_auto_exec);
}

void AddARecentlyUsedFile(path)
   char *path;
{
   int i=0, count=0, move_index=INVALID;
   KeyValInfo *pkvi=RecentFilesListing(&count);

   if (count == 0) {
      /* do not translate -- program constants */
      tgWriteProfileString(gpszRecentFilesSec, NULL, NULL, gszFilesIniFile);
      tgWriteProfileString(gpszRecentFilesSec, "0", path, gszFilesIniFile);
   } else {
      char sz_key[40];

      for (i=0; i < count; i++) {
         if (strcmp(path, pkvi[i].value) == 0) {
            if (i == 0) {
               FreeRecentFilesListing(pkvi, count);
               return;
            }
            move_index = i;
            break;
         }
      }
      tgWriteProfileString(gpszRecentFilesSec, NULL, NULL, gszFilesIniFile);
      if (move_index == INVALID) {
         if (count+1 > maxRecentFiles) {
            count = maxRecentFiles-1;
         }
         for (i=0; i < count+1; i++) {
            sprintf(sz_key, "%1d", i);
            if (i == 0) {
               tgWriteProfileString(gpszRecentFilesSec, sz_key, path,
                     gszFilesIniFile);
            } else {
               tgWriteProfileString(gpszRecentFilesSec, sz_key, pkvi[i-1].value,
                     gszFilesIniFile);
            }
         }
      } else {
         if (count > maxRecentFiles) {
            count = maxRecentFiles;
         }
         for (i=0; i < count; i++) {
            sprintf(sz_key, "%1d", i);
            if (i == 0) {
               tgWriteProfileString(gpszRecentFilesSec, sz_key,
                     pkvi[move_index].value, gszFilesIniFile);
            } else if (i <= move_index) {
               tgWriteProfileString(gpszRecentFilesSec, sz_key, pkvi[i-1].value,
                     gszFilesIniFile);
            } else {
               tgWriteProfileString(gpszRecentFilesSec, sz_key, pkvi[i].value,
                     gszFilesIniFile);
            }
         }
      }
   }
   sprintf(gszMsgBox, "%1d", (move_index==INVALID ? count+1 : count));
   tgWriteProfileString(gpszRecentFilesSec, gpszRecentFilesCountKey,
         gszMsgBox, gszFilesIniFile);
   tgWriteProfileString(NULL, NULL, NULL, gszFilesIniFile);
   FreeRecentFilesListing(pkvi, count);
}

int InitRecentFiles()
{
   char *c_ptr=NULL;

   if (PRTGIF) return TRUE;

   /* do not translate -- program constants */
   sprintf(gszFilesIniFile, "%s%cfiles.ini", tgifDir, DIR_SEP);

   maxRecentFiles = 10;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"MaxRecentFiles")) != NULL) {
      maxRecentFiles = atoi(c_ptr);
      if (maxRecentFiles <= 0 || maxRecentFiles > 99) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF_RNG_USE_ALT_VAL),
               TOOL_NAME, "MaxRecentFiles", c_ptr, 1, 99, 10);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         maxRecentFiles = 10;
      }
   }
   if ((c_ptr=tgGetProfileString(gpszRecentFilesSec, gpszRecentFilesCountKey,
         gszFilesIniFile)) != NULL) {
      int count = atoi(c_ptr);

      if (count > maxRecentFiles) {
         sprintf(gszMsgBox, "%1d", maxRecentFiles);
         tgWriteProfileString(gpszRecentFilesSec, gpszRecentFilesCountKey,
               gszMsgBox, gszFilesIniFile);
         tgWriteProfileString(NULL, NULL, NULL, gszFilesIniFile);
      }
      tgFreeProfileString(c_ptr);
   }
   return TRUE;
}

void CleanUpRecentFiles()
{
}

/* ----------------------- SimpleString Object ----------------------- */

int CreateSimpleStringObj(buf)
   char *buf;
{
   SimpleString *ss_ptr=NULL;
   struct ObjRec *obj_ptr=NULL;

   ss_ptr = (SimpleString *)malloc(sizeof(SimpleString));
   if (ss_ptr == NULL) FailAllocMessage();
   memset(ss_ptr, 0, sizeof(SimpleString));

   DynStrSet(&ss_ptr->dyn_str, buf);

   obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (obj_ptr == NULL) FailAllocMessage();
   memset(obj_ptr, 0, sizeof(struct ObjRec));

   obj_ptr->type = OBJ_SS;
   obj_ptr->color = colorIndex;
   if (mainDisplay != NULL) {
      UtilStrCpyN(obj_ptr->color_str, sizeof(obj_ptr->color_str),
            colorMenuItems[colorIndex]);
   }
   obj_ptr->id = objId++;
   obj_ptr->detail.ss = ss_ptr;
   /*
    * the double-byte stuff is left alone, always single byte for now
    */

   AddObj(NULL, topObj, obj_ptr);

   return TRUE;
}

void SaveSimpleStringObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   SimpleString *ss_ptr=ObjPtr->detail.ss;

   if (fprintf(FP, "simple_string('%s',%1d,%1d,%1d,%1d,\n\t\"",
         colorMenuItems[ObjPtr->color], ObjPtr->id, ss_ptr->double_byte,
         ss_ptr->double_byte_mod_bytes, ss_ptr->double_byte_vertical) == EOF) {
      writeFileFailed = TRUE;
   }
   if (ss_ptr->double_byte) {
      SaveDoubleByteString(FP, ss_ptr->dyn_str.s);
   } else {
      SaveString(FP, ss_ptr->dyn_str.s);
   }
   if (fprintf(FP, "\")") == EOF) writeFileFailed = TRUE;
}

void ReadSimpleStringObj(FP, Inbuf, ObjPtr)
   FILE *FP;
   char *Inbuf;
   struct ObjRec **ObjPtr;
{
   SimpleString *ss_ptr=NULL;
   char *psz=NULL, *s=NULL, color_str[40], *line=NULL;
   int id=0, double_byte=FALSE, db_mod_bytes=FALSE, db_vertical=FALSE;
   int new_alloc=FALSE;

   *ObjPtr = NULL;

   s = FindChar((int)'(', Inbuf);
   s = ParseStr(s, (int)',', color_str, sizeof(color_str));
   InitScan(s, "\t\n, ");

   if (GETINT("simple_string", id, "id") == INVALID ||
       GETINT("simple_string", double_byte, "double_byte") == INVALID ||
       GETINT("simple_string", db_mod_bytes, "db_mod_bytes") == INVALID ||
       GETINT("simple_string", db_vertical, "db_vertical") == INVALID) {
      return;
   }
   if (id >= objId) objId = id+1;

   if ((line=UtilGetALine(FP)) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_UNEXPECTED_EOF_IN_ABORT_READ),
            scanFileName, scanLineNum, "ReadSimpleStringObj()");
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return;
   }
   scanLineNum++;

   psz = FindChar((int)'"', line);
   s = ReadString(psz);
   *(--s) = '\0';

   *ObjPtr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (*ObjPtr == NULL) FailAllocMessage();
   memset(*ObjPtr, 0, sizeof(struct ObjRec));
   ss_ptr = (SimpleString *)malloc(sizeof(SimpleString));
   if (ss_ptr == NULL) FailAllocMessage();
   memset(ss_ptr, 0, sizeof(SimpleString));

   DynStrSet(&ss_ptr->dyn_str, psz);
   free(line);

   ss_ptr->double_byte = double_byte;
   ss_ptr->double_byte_mod_bytes = db_mod_bytes;
   ss_ptr->double_byte_vertical = db_vertical;

   (*ObjPtr)->color = QuickFindColorIndex(*ObjPtr, color_str, &new_alloc, TRUE);
   UtilStrCpyN((*ObjPtr)->color_str, sizeof((*ObjPtr)->color_str), color_str);
   (*ObjPtr)->id = id;
}

void FreeSimpleStringObj(ObjPtr)
   struct ObjRec *ObjPtr;
{
   UtilFree(ObjPtr->detail.ss->dyn_str.s);
   free(ObjPtr->detail.ss);
   free(ObjPtr);
}

/* ----------------------- ReplaceGraphic() ----------------------- */

static
void UnInheritAllAttrs(obj_ptr)
   struct ObjRec *obj_ptr;
{
   struct AttrRec *attr_ptr;

   for (attr_ptr=obj_ptr->fattr; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
      if (attr_ptr->inherited) {
         attr_ptr->inherited = FALSE;
      }
   }
}

void ReplaceGraphic()
{
   struct SelRec *saved_top_sel=NULL, *saved_bot_sel=NULL, *sel_ptr=NULL;
   struct SelRec *prev_sel=NULL;
   struct ObjRec *pasted_top_obj=NULL, *pasted_bot_obj=NULL;
   struct ObjRec *icon_obj_to_replace=NULL;
   char *orig_cut_buffer=NULL;
   int sel_ltx=selLtX, sel_lty=selLtY, sel_rbx=selRbX, sel_rby=selRbY;
   int len=0, changed=FALSE, icons_converted_to_groups=FALSE, icon_count=0;
   int from_selection=FALSE;

   if (topSel == NULL) {
      MsgBox(TgLoadCachedString(CSTID_NO_OBJ_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      if (sel_ptr->obj->type == OBJ_ICON) {
         icon_obj_to_replace = sel_ptr->obj;
         icon_count++;
      }
   }
   if (icon_count > 1) {
      MsgBox(TgLoadString(STID_TOO_MANY_ICON_REPLACE_GRAPHIC), TOOL_NAME,
            INFO_MB);
      return;
   }
   orig_cut_buffer = FetchSelectionOrCutBuffer(&len, &from_selection);
   if (orig_cut_buffer == NULL) {
      MsgBox(TgLoadString(STID_CUT_BUFFER_EMPTY), TOOL_NAME, INFO_MB);
      return;
   }
   if (CutBufferType(orig_cut_buffer) != CBF_TGIF_OBJ) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_REPLACE_GRAPHIC_NOT_TGIF),
            TOOL_NAME);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();
   saved_top_sel = topSel;
   saved_bot_sel = botSel;
   topSel = botSel = NULL;
   UpdSelBBox();

   if (!GetObjsFromCutBuffer(&orig_cut_buffer[1], len-1, &pasted_top_obj,
         &pasted_bot_obj)) {
      FreeSelectionOrCutBuffer(orig_cut_buffer, from_selection);
      topSel = saved_top_sel;
      botSel = saved_bot_sel;
      UpdSelBBox();
      HighLightForward();
      return;
   }
   FreeSelectionOrCutBuffer(orig_cut_buffer, from_selection);

   if (pasted_top_obj == NULL || pasted_top_obj != pasted_bot_obj) {
      MsgBox(TgLoadString(STID_CANT_REPLACE_GRAPHIC_TOO_MANY), TOOL_NAME,
            INFO_MB);
      topSel = saved_top_sel;
      botSel = saved_bot_sel;
      UpdSelBBox();
      HighLightForward();
      return;
   }
   if (pasted_top_obj->type == OBJ_SYM || pasted_top_obj->type == OBJ_ICON ||
         pasted_top_obj->type == OBJ_PIN) {
      pasted_top_obj->type = OBJ_GROUP;
   }
   DelAllAttrs(pasted_top_obj->fattr);

   topSel = saved_top_sel;
   botSel = saved_bot_sel;
   UpdSelBBox();

   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=prev_sel) {
      struct ObjRec *obj_ptr=sel_ptr->obj, *new_obj_ptr=NULL;
      int convert_to_group=TRUE;

      prev_sel = sel_ptr->prev;
      if (obj_ptr->type == OBJ_TEXT) continue;

      if (obj_ptr->type == OBJ_ICON && obj_ptr == icon_obj_to_replace) {
         switch (MsgBox(TgLoadString(STID_Q_CVT_ICON_TO_GRP_IN_REPLACE),
               TOOL_NAME, YNC_MB)) {
         case MB_ID_YES: break;
         case MB_ID_NO: convert_to_group = FALSE; break;
         case MB_ID_CANCEL: continue;
         }
      }
      changed = TRUE;

      PrepareToReplaceAnObj(obj_ptr);

      new_obj_ptr = DupObj(pasted_top_obj);
      MoveObj(new_obj_ptr, obj_ptr->obbox.ltx-new_obj_ptr->obbox.ltx,
            obj_ptr->obbox.lty-new_obj_ptr->obbox.lty);

      new_obj_ptr->prev = obj_ptr->prev;
      new_obj_ptr->next = obj_ptr->next;
      new_obj_ptr->fattr = obj_ptr->fattr;
      new_obj_ptr->lattr = obj_ptr->lattr;

      obj_ptr->fattr = obj_ptr->lattr = NULL;
      UnlinkObj(obj_ptr);

      if (obj_ptr->type == OBJ_SYM) new_obj_ptr->type = OBJ_SYM;

      UpdAttrOwner(new_obj_ptr->fattr, new_obj_ptr);
      if (new_obj_ptr->prev == NULL) {
         curPage->top = topObj = new_obj_ptr;
      } else {
         new_obj_ptr->prev->next = new_obj_ptr;
      }
      if (new_obj_ptr->next == NULL) {
         curPage->bot = botObj = new_obj_ptr;
      } else {
         new_obj_ptr->next->prev = new_obj_ptr;
      }
      AdjObjBBox(new_obj_ptr);
      ExpandCurSelBBoxes(new_obj_ptr);

      sel_ptr->obj = new_obj_ptr;
      if (convert_to_group) {
         icons_converted_to_groups = TRUE;
         UnInheritAllAttrs(new_obj_ptr);
      } else {
         struct SelRec *tmp_sel=NULL;

         saved_top_sel = topSel;
         saved_bot_sel = botSel;
         topSel = botSel = tmp_sel = SelectThisObject(new_obj_ptr);
         UpdSelBBox();
         MakeIconic(NULL, FALSE);
         topSel = saved_top_sel;
         botSel = saved_bot_sel;
         UpdSelBBox();
         free(tmp_sel);
      }
      RecordReplaceAnObj(new_obj_ptr);
      FreeObj(obj_ptr);
   }
   EndCompositeCmd();

   if (changed) {
      UpdSelBBox();
      RedrawAreas(botObj, sel_ltx-GRID_ABS_SIZE(1), sel_lty-GRID_ABS_SIZE(1),
            sel_rbx+GRID_ABS_SIZE(1), sel_rby+GRID_ABS_SIZE(1),
            selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
      justDupped = FALSE;
      if (icons_converted_to_groups) {
         MsgBox(TgLoadString(STID_REPLACE_GRAPHIC_ICON_TO_GROUP), TOOL_NAME,
               INFO_MB);
      } else {
         Msg(TgLoadString(STID_REPLACE_GRAPHIC_DONE));
      }
   }
   HighLightForward();
}

/* ----------------------- ReplaceGraphic() ----------------------- */

static int wordWrapDuringImportTextFile=FALSE;

#ifndef A4PAPER
static int topMarginForImportTextFile=HALF_INCH;
static int leftMarginForImportTextFile=HALF_INCH;
static int rightMarginForImportTextFile=HALF_INCH;
static int bottomMarginForImportTextFile=HALF_INCH;
#else /* A4PAPER */
static int topMarginForImportTextFile=ONE_CM;
static int leftMarginForImportTextFile=ONE_CM;
static int rightMarginForImportTextFile=ONE_CM;
static int bottomMarginForImportTextFile=ONE_CM;
#endif /* A4PAPER */

static
int DoSetMarginsForImportMultipageTextFile(spec)
   char *spec;
{
   int top=0, bottom=0, left=0, right=0;
   char *top_str=NULL, *bottom_str=NULL, *left_str=NULL, *right_str=NULL;

   UtilTrimBlanks(spec);
   top_str = strtok(spec, ",\t\n\r");
   if (top_str == NULL) return FALSE;
   bottom_str = strtok(NULL, ",\t\n\r");
   if (bottom_str == NULL) return FALSE;
   left_str = strtok(NULL, ",\t\n\r");
   if (left_str == NULL) return FALSE;
   right_str = strtok(NULL, ",\t\n\r");
   if (right_str == NULL) return FALSE;

   if (GetDimension(top_str, FALSE, &top) &&
         GetDimension(bottom_str, FALSE, &bottom) &&
         GetDimension(left_str, FALSE, &left) &&
         GetDimension(right_str, FALSE, &right)) {
      topMarginForImportTextFile = top;
      bottomMarginForImportTextFile = bottom;
      leftMarginForImportTextFile = left;
      rightMarginForImportTextFile = right;

      return TRUE;
   }
   return FALSE;
}

static
void GetCurMarginsForImportTextFile(buf)
   char *buf;
{
   char n_str[MAXSTRING], s_str[MAXSTRING], w_str[MAXSTRING], e_str[MAXSTRING];
   float n=((float)topMarginForImportTextFile)*printMag/((float)100.0);
   float s=((float)bottomMarginForImportTextFile)*printMag/((float)100.0);
   float w=((float)leftMarginForImportTextFile)*printMag/((float)100.0);
   float e=((float)rightMarginForImportTextFile)*printMag/((float)100.0);
   float n_val=(float)0.0, s_val=(float)0.0, w_val=(float)0.0, e_val=(float)0.0;
   float unit=(float)0.0;

   switch (gridSystem) {
   case ENGLISH_GRID:
      unit = (float)PIX_PER_INCH;

      n_val = n / unit;
      s_val = s / unit;
      w_val = w / unit;
      e_val = e / unit;
      FormatFloat(&n_val, n_str);
      FormatFloat(&s_val, s_str);
      FormatFloat(&w_val, w_str);
      FormatFloat(&e_val, e_str);
      /* do not translate -- program constants */
      sprintf(buf, "[ %s in, %s in, %s in, %s in ]",
            n_str, s_str, w_str, e_str);
      break;
   case METRIC_GRID:
      unit = (float)ONE_CM;

      n_val = n / unit;
      s_val = s / unit;
      w_val = w / unit;
      e_val = e / unit;
      FormatFloat(&n_val, n_str);
      FormatFloat(&s_val, s_str);
      FormatFloat(&w_val, w_str);
      FormatFloat(&e_val, e_str);
      /* do not translate -- program constants */
      sprintf(buf, "[ %s cm, %s cm, %s cm, %s cm ]",
            n_str, s_str, w_str, e_str);
      break;
   }
}

void ImportMultipageTextFile()
{
}

void SetMarginsForImportMultipageTextFile()
{
   char buf[MAXSTRING<<1], spec[MAXSTRING+1], spec_copy[MAXSTRING+1];

   GetCurMarginsForImportTextFile(gszMsgBox);
   sprintf(buf, TgLoadString(STID_CUR_MARGINS_ARE_GIVEN), gszMsgBox);
   *spec = '\0';
   switch (gridSystem) {
   case ENGLISH_GRID:
      sprintf(gszMsgBox, TgLoadString(STID_ENTER_MARGINS),
            "0.5in,0.5in,0.5in,0.5in");
      break;
   case METRIC_GRID:
      sprintf(gszMsgBox, TgLoadString(STID_ENTER_MARGINS),
            "1cm,1cm,1cm,1cm");
      break;
   }
   if (Dialog(gszMsgBox, buf, spec) == INVALID) {
      return;
   }
   UtilStrCpyN(spec_copy, sizeof(spec_copy), spec);

   if (!DoSetMarginsForImportMultipageTextFile(spec_copy)) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_FOR_4_VAL), spec);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else {
      char buf[MAXSTRING<<1];

      strcpy(buf, "    ");
      GetCurMarginsForImportTextFile(&buf[4]);
      TwoLineMsg(TgLoadString(STID_NEW_MARGINS_ARE_GIVEN), buf);
   }
}

void ToggleWordWrapDuringImportMultipageTextFile()
{
   wordWrapDuringImportTextFile = !wordWrapDuringImportTextFile;
   sprintf(gszMsgBox, TgLoadString(wordWrapDuringImportTextFile ?
         STID_WILL_USE_WORDWRAP_IMPORT_TEXT : STID_NO_WORDWRAP_IMPORT_TEXT),
         TOOL_NAME);
   Msg(gszMsgBox);
}

int RefreshImportMutipageTextFileMenu(menu)
   TgMenu *menu;
{
   int ok=TRUE;

   /* Word Wrap */
   ok &= TgSetMenuItemCheckById(menu, CMDID_TOGGLEWORDWRAPONIMPORTTEXT,
         wordWrapDuringImportTextFile);

   /*
    * Well, don't know how to do all these yet.  Just disable them for now.
    */
   ok &= TgEnableMenuItemById(menu, CMDID_IMPORTMULTIPAGETEXTFILE, FALSE);
   ok &= TgEnableMenuItemById(menu, CMDID_SETMARGINSONIMPORTTEXT, FALSE);
   ok &= TgEnableMenuItemById(menu, CMDID_TOGGLEWORDWRAPONIMPORTTEXT, FALSE);

   return ok;
}

TgMenu *CreateImportMutipageTextFileMenu(parent_menu, x, y, menu_info,
      status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   TgMenu *menu=TgCreateMenuFromMenuInfo(parent_menu, x, y, menu_info, FALSE);

   if (menu != NULL) {
      if (!RefreshImportMutipageTextFileMenu(menu)) {
         return TgDestroyMenu(menu, TRUE);
      }
      menu->refresh_proc =
            ((RefreshMenuFunc*)RefreshImportMutipageTextFileMenu);
   }
   return menu;
}

/* ----------------------- Init and CleanUp ----------------------- */

static
void CleanUpCutBufferInfo()
{
   ResetCutBufferInfo();
}

void CleanUpCutPaste()
{
   cutBufferIsTgifObj = FALSE;
   cutBufferIsUTF8 = FALSE;
   FreeDynStrBuf(&dsCutBuffer);
   CleanUpCutBufferInfo();
}

static
void ResetXCutBuffers()
{
   int i=0;

   for (i=0; gaCutBufferAtom[i] != (Atom)0; i++) {
      XChangeProperty(mainDisplay, rootWindow, gaCutBufferAtom[i], XA_STRING,
            8, PropModeAppend, (unsigned char *)"", 0);
   }
}

static
void InitCutBufferInfo()
{
   memset(&gSetCutBufferInfo, 0, sizeof(SetCutBufferInfo));
}

int InitCutPaste()
{
   char *c_ptr=NULL;
   int ival=0;

   InitCutBufferInfo();

   pasteFromXSelectionOnly = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PasteFromXSelectionOnly")) !=
         NULL && UtilStrICmp(c_ptr, "false") == 0) {
      pasteFromXSelectionOnly = FALSE;
   }
   pasteFromSelectionTimeout = 10;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PasteFromSelectionTimeout")) !=
         NULL) {
      if (sscanf(c_ptr, "%d", &ival) != 1 || ival < 0) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF), TOOL_NAME,
               "PasteFromSelectionTimeout", c_ptr);
         fprintf(stderr, "\n");
      } else {
         pasteFromSelectionTimeout = ival;
      }
   }
   debugCopyPaste = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DebugCopyPaste")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      debugCopyPaste = TRUE;
   }
   memset(&dsCutBuffer, 0, sizeof(dsCutBuffer));
   ResetXCutBuffers();

   return TRUE;
}
