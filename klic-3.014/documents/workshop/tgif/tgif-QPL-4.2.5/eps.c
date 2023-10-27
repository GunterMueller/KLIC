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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/eps.c,v 1.9 2011/05/16 16:21:57 william Exp $
 */

#define _INCLUDE_FROM_EPS_C_

#include "tgifdefs.h"

#include "cmd.e"
#include "color.e"
#include "cursor.e"
#include "dialog.e"
#include "drawing.e"
#include "dup.e"
#include "eps.e"
#include "file.e"
#include "grid.e"
#include "mark.e"
#include "msg.e"
#include "names.e"
#include "obj.e"
#include "pattern.e"
#include "ps.e"
#include "rect.e"
#include "select.e"
#include "setup.e"
#include "special.e"
#include "strtbl.e"
#include "util.e"
#include "xbitmap.e"

#ifndef SEEK_SET
#define SEEK_SET 0
#endif /* ~SEEK_SET */

typedef struct EPSLineRec {
   char *s;
   struct EPSLineRec *next, *prev;
} *EPSLineRecPtr;

float	defaultEPSScaling=1.0;
char	defaultEPSScalingStr[80];

int	autoEPSPreviewBitmap=FALSE;

static struct EPSLineRec *topLine=NULL, *botLine=NULL;
static int numLines=0;

static char tiffToXbmCmd[MAXSTRING+1];
static int tiffToXbmCmdInitialized=FALSE;
static char psToXbmCmd[MAXSTRING+1];

/* do not translate -- program constants */
static char hexValue[]="0123456789abcdef";
static char flippedHexValue[]="084c2a6e195d3b7f";

static char defTiffToXbmCmd[]="tifftopnm %s | pgmtopbm | pbmtoxbm";
static char defPsToXbmCmd[] =
      "gs -q -dNOPAUSE -sDEVICE=pbm -sOutputFile=- -- \"%s\" | pbmtoxbm";

static float bitmapThresholdFor8bpsPreviewBitmap=(float)0.5;

void InitEPS()
{
   char *c_ptr=NULL;

   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ForceClearAfterEPS")) !=
         NULL) {
      fprintf(stderr, TgLoadString(STID_NAMED_XDEF_IS_OBSOLETE), TOOL_NAME,
            "ForceClearAfterEPS");
      fprintf(stderr, "\n");
   }
   defaultEPSScaling = 1.0;
   strcpy(defaultEPSScalingStr, "1");
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DefaultEPSScaling")) != NULL) {
      strcpy(defaultEPSScalingStr, c_ptr);
      defaultEPSScaling = (float)atof(c_ptr);
      if (defaultEPSScaling <= 0.0) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
               TOOL_NAME, "DefaultEPSScaling", c_ptr, 1);
         defaultEPSScaling = 1.0;
         strcpy(defaultEPSScalingStr, "1");
      } else if (strcmp(defaultEPSScalingStr,"1")==0 ||
            strcmp(defaultEPSScalingStr,"1.0")==0) {
         defaultEPSScaling = 1.0;
         strcpy(defaultEPSScalingStr, "1");
      }
   }
   tiffToXbmCmdInitialized = TRUE;
   /* do not translate -- program constants */
   strcpy(tiffToXbmCmd, defTiffToXbmCmd);
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"TiffToXbm")) != NULL) {
      int count=0;

      strcpy(tiffToXbmCmd, c_ptr);
      for (c_ptr=strstr(tiffToXbmCmd,"%s"); c_ptr!=NULL;
            c_ptr=strstr(++c_ptr,"%s")) {
         count++;
      }
      if (count != 1) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "TiffToXbm", tiffToXbmCmd, defTiffToXbmCmd);
         strcpy(tiffToXbmCmd, defTiffToXbmCmd);
      }
   }
   /* do not translate -- program constants */
   strcpy(psToXbmCmd, defPsToXbmCmd);
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PsToXbm")) != NULL) {
      int count=0;

      strcpy(psToXbmCmd, c_ptr);
      for (c_ptr=strstr(psToXbmCmd,"%s"); c_ptr!=NULL;
            c_ptr=strstr(++c_ptr,"%s")) {
         count++;
      }
      if (count != 1) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "PsToXbm", psToXbmCmd, defPsToXbmCmd);
         strcpy(psToXbmCmd, defPsToXbmCmd);
      }
   }
   autoEPSPreviewBitmap = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"AutoEPSPreviewBitmap")) !=
         NULL && UtilStrICmp(c_ptr, "true") == 0) {
      autoEPSPreviewBitmap = TRUE;
   }
   bitmapThresholdFor8bpsPreviewBitmap = (float)0.5;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"BitmapThreshold"))!=NULL) {
      bitmapThresholdFor8bpsPreviewBitmap = (float) atof(c_ptr);
      if (bitmapThresholdFor8bpsPreviewBitmap < ((float)0) ||
            bitmapThresholdFor8bpsPreviewBitmap > (((float)1)+INT_TOL)) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_STR),
               TOOL_NAME, "BitmapThreshold", c_ptr, "0.5");
         fprintf(stderr, "\n");
         bitmapThresholdFor8bpsPreviewBitmap = (float)0.5;
      }
   }
}

static
void CleanUpLines()
{
   struct EPSLineRec *line_ptr, *next_line;

   for (line_ptr=topLine; line_ptr != NULL; line_ptr=next_line) {
      next_line = line_ptr->next;
      if (line_ptr->s != NULL) free(line_ptr->s);
      free(line_ptr);
   }
   topLine = botLine = NULL;
   numLines = 0;
}

void CleanUpEPS()
{
   CleanUpLines();
   stripEPSComments = FALSE;
}

static
void DumpXBmByte(fp, num_nibbles, image_h, nibble_index, row_index,
      byte_data, pn_out_byte_count)
   FILE *fp;
   int num_nibbles, image_h, nibble_index, row_index;
   int byte_data, *pn_out_byte_count;
{
   if ((*pn_out_byte_count)++ == 12) {
      (*pn_out_byte_count) = 1;
      if (fprintf(fp, "\n   ") == EOF) writeFileFailed = TRUE;
   }
   if (fprintf(fp, "0x%c", flippedHexValue[(byte_data>>4) & 0xf]) == EOF) {
      writeFileFailed = TRUE;
   }
   if (row_index == image_h-1 && nibble_index == num_nibbles-1) {
      if (fprintf(fp, "%c};\n", flippedHexValue[byte_data & 0xf]) == EOF) {
         writeFileFailed = TRUE;
      }
   } else {
      if (fprintf(fp, "%c, ", flippedHexValue[byte_data & 0xf]) == EOF) {
         writeFileFailed = TRUE;
      }
   }
}

static
int UpdateEndDetectState(c, p_end_detect_state, p_line_count)
   char c;
   int *p_end_detect_state;
   int *p_line_count;
{
   int end_detect_state=(*p_end_detect_state);

   if (c == '\n') {
      (*p_line_count)++;
      *p_end_detect_state = 0;

      return FALSE;
   }
   switch (end_detect_state) {
   case 0: *p_end_detect_state = ((c == '%') ? 1 : 0); break;
   case 1: *p_end_detect_state = ((c == '%') ? 2 : 0); break;
   case 2: *p_end_detect_state = ((c == 'E' || c == 'e') ? 3 : 0); break;
   case 3: *p_end_detect_state = ((c == 'n') ? 4 : 0); break;
   case 4: *p_end_detect_state = ((c == 'd') ? 5 : 0); break;
   default: *p_end_detect_state = 0; break;
   }
   return (*p_end_detect_state == 5);
}

static
int ReadPreviewBitmap(fp, file_name, image_w, image_h, bps, bitmap, image)
   FILE *fp;
   char *file_name;
   int image_w, image_h, bps;
   Pixmap *bitmap;
   XImage **image;
{
   int k=0, j=0, i=0, num_nibbles=0, bit_count=0, out_byte_count=0, line_sz=0;
   int line_count=0, end_detect_state=0; /* detecting "%%End..." */
   char *line=NULL, *c_ptr=NULL;

   switch (bps) {
   case 1:
      num_nibbles = ((image_w & 0x3)==0) ? (int)(image_w>>2) :
            (int)(image_w>>2)+1;
      if (num_nibbles & 0x1) num_nibbles++;
      break;
   case 8:
      num_nibbles = (image_w<<1);
      break;
   default:
      sprintf(gszMsgBox, TgLoadString(STID_GIVEN_BPS_PREVIEW_NOT_SUP), bps);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   line_sz = (num_nibbles+10)*sizeof(char);
   line = (char*)malloc(line_sz);
   if (line == NULL) FailAllocMessage();
   if (cmdLineDosEpsFilter && cmdLinePreviewOnly) {
      *bitmap = None;
      *image = NULL;
      /* do not translate -- program constants */
      printf("#define noname_width %1d\n", image_w);
      printf("#define noname_height %1d\n", image_h);
      printf("#define noname_x_hot 0\n");
      printf("#define noname_y_hot 0\n");
      printf("static char noname_bits[] = {\n   ");
   } else {
      *bitmap = XCreatePixmap(mainDisplay, dummyBitmap, image_w, image_h, 1);
      XFillRectangle(mainDisplay, *bitmap, xbmGC, 0, 0, image_w, image_h);
      *image = XGetImage(mainDisplay,*bitmap,0,0,image_w,image_h,1,ZPixmap);
   }
   for (i=0; i < image_h; i++) {
      int byte_data=0;

      for (j=0, c_ptr=line; j < num_nibbles; j++, c_ptr++) {
         while (TRUE) {
            int c=getc(fp);

            if (c == EOF) {
               sprintf(gszMsgBox, TgLoadString(STID_INVALID_PREVIEW_BMP_IN_EPS),
                     file_name);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               free(line);
               if (*bitmap != None) {
                  XFreePixmap(mainDisplay, *bitmap);
                  *bitmap = None;
               }
               if (*image != NULL) {
                  XDestroyImage(*image);
                  *image = NULL;
               }
               return FALSE;
            }
            *c_ptr = (char)c;
            if (UpdateEndDetectState(*c_ptr, &end_detect_state, &line_count)) {
               /* %%End... detected */
               sprintf(gszMsgBox, TgLoadString(STID_INVALID_PREVIEW_BMP_IN_EPS),
                     file_name);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               free(line);
               if (*bitmap != None) {
                  XFreePixmap(mainDisplay, *bitmap);
                  *bitmap = None;
               }
               if (*image != NULL) {
                  XDestroyImage(*image);
                  *image = NULL;
               }
               return FALSE;
            }
            if ((*c_ptr >= '0' && *c_ptr <= '9') ||
                  (*c_ptr >= 'a' && *c_ptr <= 'f') ||
                  (*c_ptr >= 'A' && *c_ptr <= 'F')) {
               break;
            }
         }
      }
      *c_ptr = '\0';
      bit_count = 0;
      c_ptr = line;
      for (j=0; j<num_nibbles && *c_ptr!='\0'; j++, c_ptr++) {
         int data=0;

         if (*c_ptr >= '0' && *c_ptr <= '9') {
            data = (int)(*c_ptr) - (int)('0');
         } else if (*c_ptr >= 'a' && *c_ptr <= 'f') {
            data = (int)(*c_ptr) - (int)('a') + 10;
         } else if (*c_ptr >= 'A' && *c_ptr <= 'F') {
            data = (int)(*c_ptr) - (int)('A') + 10;
         } else {
            break;
         }
         if (cmdLineDosEpsFilter && cmdLinePreviewOnly) {
            if (j & 0x1) {
               byte_data |= (data<<4);
               DumpXBmByte(stdout, num_nibbles, image_h, j, i,
                     byte_data, &out_byte_count);
            } else {
               byte_data = data;
            }
         } else {
            switch (bps) {
            case 1:
               for (k = 0; k < 4; k++) {
                  if (bit_count++ == image_w) break;

                  if (data & (1<<(3-k))) {
                     XPutPixel(*image, j*4+k, i, 1);
                  }
               }
               break;
            case 8:
               if (j & 0x1) {
                  double dval=(double)0;

                  byte_data |= (data<<4);
                  dval = (((double)byte_data) / ((double)255));
                  if (dval > ((double)bitmapThresholdFor8bpsPreviewBitmap)) {
                     XPutPixel(*image, (j>>1), i, 1);
                  }
               } else {
                  byte_data = data;
               }
               break;
            default: /* should not get here */ break;
            }
         }
      }
      if (cmdLineDosEpsFilter && cmdLinePreviewOnly) {
         if (num_nibbles & 0x1) {
            DumpXBmByte(stdout, num_nibbles, image_h, j, i,
                  byte_data, &out_byte_count);
         }
      }
   }
   if (fgets(line, line_sz, fp) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_INVALID_PREVIEW_BMP_IN_EPS),
            file_name);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(line);
      if (*bitmap != None) {
         XFreePixmap(mainDisplay, *bitmap);
         *bitmap = None;
      }
      if (*image != NULL) {
         XDestroyImage(*image);
         *image = NULL;
      }
      return FALSE;
   }
   if (cmdLineDosEpsFilter && cmdLinePreviewOnly) {
   } else {
      XPutImage(mainDisplay,*bitmap,xbmGC,*image,0,0,0,0,image_w,image_h);
   }
   free(line);
   return TRUE;
}

static
void AddLine(s)
   char *s;
{
   struct EPSLineRec *line_ptr;

   line_ptr = (struct EPSLineRec *)malloc(sizeof(struct EPSLineRec));
   if (line_ptr == NULL) FailAllocMessage();
   line_ptr->s = s;

   line_ptr->next = NULL;
   line_ptr->prev = botLine;
   if (botLine == NULL) {
      topLine = line_ptr;
   } else {
      botLine->next = line_ptr;
   }
   botLine = line_ptr;

   numLines++;
}

static
unsigned long ReadDoubleWord(buf)
   char *buf;
{
   unsigned long lval, total=0L;
   unsigned char *c_ptr=(unsigned char *)buf;

   lval = (unsigned long)(*c_ptr++);
   total += lval;
   lval = (unsigned long)(*c_ptr++);
   total += (lval<<8);
   lval = (unsigned long)(*c_ptr++);
   total += (lval<<16);
   lval = (unsigned long)(*c_ptr++);
   total += (lval<<24);
   return total;
}

static
int XbmToPreviewBitmap(FP, xbm_fname)
   FILE *FP;
   char *xbm_fname;
{
   unsigned int image_w, image_h;
   int rc, x_hot, y_hot, **data, num_image_bytes_per_row, num_lines;
   int row, col;
   Pixmap bitmap;
   XImage *image;

   if ((rc=XReadBitmapFile(mainDisplay, mainWindow, xbm_fname, &image_w,
         &image_h, &bitmap, &x_hot, &y_hot)) != BitmapSuccess) {
      return FALSE;
   }
   image = XGetImage(mainDisplay, bitmap, 0, 0, image_w, image_h, 1, ZPixmap);
   if (image == NULL) {
      XFreePixmap(mainDisplay, bitmap);
      return FALSE;
   }
   num_image_bytes_per_row = ((image_w & 0x7) ? (image_w>>3)+1 : (image_w>>3));
   num_lines = ((image_w & 0x7) ? (((image_w>>3)+1)<<1) : ((image_w>>3)<<1));
   num_lines = ((num_lines & 0x3f) ? (num_lines>>6)+1 : (num_lines>>6));

   if ((data=(int**)malloc(image_h*sizeof(int*))) == NULL) {
      XFreePixmap(mainDisplay, bitmap);
      XDestroyImage(image);
      return FailAllocMessage();
   }
   for (row=0; row < image_h; row++) {
      if ((data[row]=(int*)malloc(num_image_bytes_per_row*sizeof(int))) ==
            NULL) {
         int i;

         for (i=0; i < row; i++) free(data[i]);
         free(data);
         XFreePixmap(mainDisplay, bitmap);
         XDestroyImage(image);
         return FailAllocMessage();
      } else {
         for (col=0; col<num_image_bytes_per_row; col++) {
            data[row][col] = 0;
         }
      }
   }
   /* do not translate -- program constants */
   fprintf(FP, "%%!\n");
   fprintf(FP, "%%%%BeginPreview: %1d %1d 1 %1d\n", image_w, image_h,
         num_lines*image_h);
   for (row=0; row < image_h; row++) {
      for (col=0; col < image_w; col++) {
         if (XGetPixel(image, col, row) != 0) {
            data[row][col>>3] |= (1<<(7 - (col & 0x7)));
         }
      }
   }
   for (row=0; row < image_h; row++) {
      int byte_count=0;

      fprintf(FP, "%% ");
      for (col=0; col < num_image_bytes_per_row; col++) {
         if (byte_count++ == 32) {
            byte_count = 1;
            fprintf(FP, "\n%% ");
         }
         fprintf(FP, "%c", hexValue[(data[row][col]>>4) & 0xf]);
         fprintf(FP, "%c", hexValue[data[row][col] & 0xf]);
      }
      fprintf(FP, "\n");
   }
   fprintf(FP, "%%%%EndImage\n");
   fprintf(FP, "%%%%EndPreview\n");
   
   for (row=0; row < image_h; row++) free(data[row]);
   free(data);
   XDestroyImage(image);
   XFreePixmap(mainDisplay, bitmap);
   return TRUE;
}

static
int ErrorInConvertTiffToXbm(fp, tiff_fp, xbm_fp, fname)
   FILE *fp, *tiff_fp, *xbm_fp;
   char *fname;
{
   if (fp != NULL) fclose(fp);
   if (tiff_fp != NULL) fclose(tiff_fp);
   if (xbm_fp != NULL) fclose(xbm_fp);
   if (fname != NULL) {
      if (writeFileFailed) {
         FailToWriteFileMessage(fname);
      }
      unlink(fname);
   }
   return FALSE;
}

static
int ConvertTiffToXbm(fp, tiff_offset, tiff_sz, xbm_fname, xbm_sz)
   FILE *fp;
   int tiff_offset, tiff_sz, xbm_sz;
   char *xbm_fname;
{
   FILE *tiff_fp=NULL, *xbm_fp=NULL, *pfp;
   int bytes_to_read, bytes_read;
   char tiff_fname[MAXPATHLENGTH+1];
   char cmd[(MAXSTRING<<1)+1];

   *tiff_fname = *xbm_fname = '\0';
   /* extract TIFF into a file */
   if (fseek(fp, tiff_offset, SEEK_SET) != 0) {
      return ErrorInConvertTiffToXbm(fp, NULL, NULL, NULL);
   }
   if (MkTempFile(tiff_fname, sizeof(tiff_fname), tmpDir, TOOL_NAME) == NULL) {
      return ErrorInConvertTiffToXbm(fp, NULL, NULL, NULL);
   }
   /* do not translate -- program constants */
   if ((tiff_fp=fopen(tiff_fname, "w")) == NULL) {
      return ErrorInConvertTiffToXbm(fp, NULL, NULL, NULL);
   }
   while (tiff_sz > 0) {
      bytes_to_read = min(tiff_sz, sizeof(gszMsgBox));
      if (bytes_to_read !=
            (int)fread(gszMsgBox, sizeof(char), bytes_to_read, fp)) {
         return ErrorInConvertTiffToXbm(fp, tiff_fp, NULL, tiff_fname);
      }
      if ((int)fwrite(gszMsgBox, sizeof(char), bytes_to_read, tiff_fp) <= 0) {
         writeFileFailed = TRUE;
         return ErrorInConvertTiffToXbm(fp, tiff_fp, NULL, tiff_fname);
      }
      tiff_sz -= bytes_to_read;
   }
   fclose(tiff_fp);
   tiff_fp = NULL;

   /* convert TIFF to XBM */
   if (MkTempFile(xbm_fname, xbm_sz, tmpDir, TOOL_NAME) == NULL) {
      return ErrorInConvertTiffToXbm(fp, NULL, NULL, NULL);
   }
   if (!tiffToXbmCmdInitialized) {
      strcpy(tiffToXbmCmd, "tifftopnm %s | pgmtopbm | pbmtoxbm");
   }
   sprintf(cmd, tiffToXbmCmd, tiff_fname);
   if (!FindProgramInPath(cmd, NULL, FALSE)) {
      return ErrorInConvertTiffToXbm(fp, NULL, NULL, NULL);
   }
   if ((xbm_fp=fopen(xbm_fname, "w")) == NULL) {
      return ErrorInConvertTiffToXbm(fp, NULL, NULL, NULL);
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM), cmd);
   SetStringStatus(gszMsgBox);
   if (mainDisplay != NULL) XSync(mainDisplay, False);
   if ((pfp=(FILE*)popen(cmd,"r")) == NULL) {
      unlink(tiff_fname);
      return ErrorInConvertTiffToXbm(fp, NULL, xbm_fp, xbm_fname);
   }
   while ((bytes_read=(int)fread(gszMsgBox, sizeof(char), sizeof(gszMsgBox),
         pfp)) > 0) {
      if ((int)fwrite(gszMsgBox, sizeof(char), bytes_read, xbm_fp) <= 0) {
         writeFileFailed = TRUE;
         break;
      }
   }
   pclose(pfp);
   SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
   fclose(xbm_fp);
   xbm_fp = NULL;
   unlink(tiff_fname);
   *tiff_fname = '\0';
   if (writeFileFailed) {
      return ErrorInConvertTiffToXbm(fp, NULL, xbm_fp, xbm_fname);
   }
   return TRUE;
}

static
FILE *ErrorInGetTiffEPSIInfo(fp, eps_fp, fname)
   FILE *fp, *eps_fp;
   char *fname;
{
   if (fp != NULL) fclose(fp);
   if (eps_fp != NULL) fclose(eps_fp);
   if (fname != NULL) {
      if (writeFileFailed) {
         FailToWriteFileMessage(fname);
      }
      unlink(fname);
   }
   return NULL;
}

static
FILE *GetTiffEPSIInfo(fp, pszEPS, file_buf_sz, pnPreviewOK)
   FILE *fp;
   char *pszEPS;
   int file_buf_sz, *pnPreviewOK;
{
   int bytes_to_read=0x1e, tiff_sz, tiff_offset, eps_sz;
   int need_to_check_ps=TRUE;
   char buf[0x20], xbm_fname[MAXPATHLENGTH+1];
   FILE *eps_fp=NULL;

   writeFileFailed = FALSE;
   *xbm_fname = '\0';
   *pszEPS = '\0';
   rewind(fp);
   if (fread(buf, sizeof(char), bytes_to_read, fp) != bytes_to_read) {
      return ErrorInGetTiffEPSIInfo(fp, eps_fp, NULL);
   }
   eps_sz = ReadDoubleWord(&buf[0x08]);
   tiff_offset = ReadDoubleWord(&buf[0x14]);
   tiff_sz = ReadDoubleWord(&buf[0x18]);

   if (cmdLineDosEpsFilter) {
      if (cmdLinePreviewOnly) {
         FILE *xbm_fp=NULL;
         int bytes_read=0;

         *pnPreviewOK = ConvertTiffToXbm(fp, tiff_offset, tiff_sz, xbm_fname,
               sizeof(xbm_fname));
         if (fseek(fp, 0x1e, SEEK_SET) != 0) {
            return ErrorInGetTiffEPSIInfo(fp, NULL, NULL);
         }
         if ((xbm_fp=fopen(xbm_fname,"r")) == NULL) {
            unlink(xbm_fname);
            *xbm_fname = '\0';
            return ErrorInGetTiffEPSIInfo(fp, NULL, NULL);
         }
         while ((bytes_read=(int)fread(gszMsgBox, sizeof(char),
               sizeof(gszMsgBox), xbm_fp)) > 0) {
            if ((int)fwrite(gszMsgBox, sizeof(char), bytes_read, stdout) <= 0) {
               writeFileFailed = TRUE;
               break;
            }
         }
         fclose(xbm_fp);
         if (writeFileFailed) {
            fprintf(stderr, TgLoadString(STID_FAIL_TO_WRITE_TO_STDOUT));
         }
         unlink(xbm_fname);
         *xbm_fname = '\0';
         return ((FILE*)TRUE);
      } else {
         *pnPreviewOK = TRUE;
         if (fseek(fp, 0x1e, SEEK_SET) != 0) {
            return ErrorInGetTiffEPSIInfo(fp, NULL, NULL);
         }
      }
   } else {
      *pnPreviewOK = ConvertTiffToXbm(fp, tiff_offset, tiff_sz, xbm_fname,
            sizeof(xbm_fname));
      if (fseek(fp, 0x1e, SEEK_SET) != 0) {
         return ErrorInGetTiffEPSIInfo(fp, NULL, NULL);
      }
      if (MkTempFile(pszEPS, file_buf_sz, tmpDir, TOOL_NAME) == NULL) {
         return ErrorInGetTiffEPSIInfo(fp, NULL, NULL);
      }
      if ((eps_fp=fopen(pszEPS, "w")) == NULL) {
         return ErrorInGetTiffEPSIInfo(fp, NULL, NULL);
      }
      if (*pnPreviewOK && !XbmToPreviewBitmap(eps_fp, xbm_fname)) {
         *pnPreviewOK = FALSE;
      }
      unlink(xbm_fname);
      *xbm_fname = '\0';
   }

   /* copy the EPS content from the original file to the new EPS file */
   while (eps_sz > 0) {
      bytes_to_read = min(eps_sz, sizeof(gszMsgBox));
      if (bytes_to_read !=
            (int)fread(gszMsgBox, sizeof(char), bytes_to_read, fp)) {
         return ErrorInGetTiffEPSIInfo(fp, eps_fp, pszEPS);
      }
      if (need_to_check_ps) {
         need_to_check_ps = FALSE;
         if (!(bytes_to_read>=2 && gszMsgBox[0]=='%' && gszMsgBox[1]=='!')) {
            fclose(fp);
            fclose(eps_fp);
            unlink(pszEPS);
            return NULL;
         }
      }
      if (cmdLineDosEpsFilter) {
         if ((int)fwrite(gszMsgBox, sizeof(char), bytes_to_read, stdout) <= 0) {
            writeFileFailed = TRUE;
            return ErrorInGetTiffEPSIInfo(fp, eps_fp, pszEPS);
         }
      } else {
         if ((int)fwrite(gszMsgBox, sizeof(char), bytes_to_read, eps_fp) <= 0) {
            writeFileFailed = TRUE;
            return ErrorInGetTiffEPSIInfo(fp, eps_fp, pszEPS);
         }
      }
      eps_sz -= bytes_to_read;
   }
   if (fp != NULL) fclose(fp);
   if (eps_fp != NULL) fclose(eps_fp);
   if (cmdLineDosEpsFilter) {
      return ((FILE*)TRUE);
   }
   return fopen(pszEPS, "r");
}

static
void GetEPSFullPath(file_name, full_path, buf_sz)
   char *file_name, *full_path;
   int buf_sz;
{
   if (*file_name == DIR_SEP) {
      UtilStrCpyN(full_path, buf_sz, file_name);
   } else {
      char *c_ptr=NULL;

      if (PRTGIF && !cmdLineOpenDisplay && *cmdLineOpenFile != '\0') {
         UtilStrCpyN(full_path, buf_sz, cmdLineOpenFile);
      } else if (*scanFileFullPath == DIR_SEP) {
         UtilStrCpyN(full_path, buf_sz, scanFileFullPath);
      } else {
         sprintf(full_path, "%s%c%s", curDir, DIR_SEP, scanFileFullPath);
      }
      if ((c_ptr=UtilStrRChr(full_path, (int)DIR_SEP)) != NULL) {
         strcpy(++c_ptr, file_name);
      } else {
         /* Umm... this should not happen */
         UtilStrCpyN(full_path, buf_sz, file_name);
      }
   }
}

static
int ConvertPsToXbm(pszPsPath, pszXbmPath, file_buf_sz)
   char *pszPsPath, *pszXbmPath;
   int file_buf_sz;
{
   FILE *pFile=NULL, *pPipe=NULL;
   char *pszCmd=NULL, szBuf[MAXSTRING];
   int bytes_read=0, watch_cursor=watchCursorOnMainWindow;

   if (MkTempFile(pszXbmPath, file_buf_sz, tmpDir, TOOL_NAME) == NULL) {
      return FALSE;
   }
   pszCmd = (char*)malloc(
         (strlen(psToXbmCmd)+strlen(pszPsPath)+10)*sizeof(char));
   if (pszCmd == NULL) {
      FailAllocMessage();
      return FALSE;
   }
   sprintf(pszCmd, psToXbmCmd, pszPsPath);
   if (!FindProgramInPath(pszCmd, NULL, FALSE)) {
      free(pszCmd);
      return FALSE;
   }
   if ((pFile=fopen(pszXbmPath,"w")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
            pszXbmPath);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      return FALSE;
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_EXECUTING_GIVEN_PROGRAM),
         pszCmd);
   SetStringStatus(gszMsgBox);
   XSync(mainDisplay, False);
   if ((pPipe=(FILE*)popen(pszCmd,"r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_EXECUTE_CMD), pszCmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(pszCmd);
      fclose(pFile);
      unlink(pszXbmPath);
      return FALSE;
   }
   if (!watch_cursor) {
      SetWatchCursor(drawWindow);
      SetWatchCursor(mainWindow);
   }
   writeFileFailed = FALSE;
   while ((bytes_read=fread(szBuf, sizeof(char), sizeof(szBuf), pPipe)) > 0) {
      if ((int)fwrite(szBuf, sizeof(char), bytes_read, pFile) <= 0) {
         writeFileFailed = TRUE;
         break;
      }
   }
   pclose(pPipe);
   if (!watch_cursor) {
      SetDefaultCursor(mainWindow);
      ShowCursor();
   }
   SetStringStatus(TgLoadCachedString(CSTID_DOTS_DONE));
   free(pszCmd);
   fclose(pFile);
   if (writeFileFailed) {
      FailToWriteFileMessage(pszXbmPath);
      unlink(pszXbmPath);
      return FALSE;
   }
   return TRUE;
}

static
int AutoGeneratePreviewBitmap(pszPsPath, pBitmap, pImage, pnImageW, pnImageH)
   char *pszPsPath;
   Pixmap *pBitmap;
   XImage **pImage;
   int *pnImageW, *pnImageH;
{
   char xbm_fname[MAXPATHLENGTH+1];
   unsigned int tmp_w=0, tmp_h=0;
   int rc=0, x_hot=0, y_hot=0, empty_image=FALSE, dont_free_orig_bitmap=FALSE;
   Pixmap orig_bitmap=None;

   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   SaveStatusStrings();
   rc = ConvertPsToXbm(pszPsPath, xbm_fname, sizeof(xbm_fname));
   RestoreStatusStrings();
   SetDefaultCursor(mainWindow);
   ShowCursor();
   if (!rc) {
      return FALSE;
   }
   SetWatchCursor(drawWindow);
   SetWatchCursor(mainWindow);
   rc = XReadBitmapFile(mainDisplay, mainWindow, xbm_fname, &tmp_w, &tmp_h,
         &orig_bitmap, &x_hot, &y_hot);
   SetDefaultCursor(mainWindow);
   ShowCursor();
   unlink(xbm_fname);
   if (rc != BitmapSuccess) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_GEN_PREVIEW_FOR_PS),
            pszPsPath);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   *pnImageW = (int)tmp_w;
   *pnImageH = (int)tmp_h;
   rc = AutoTrimBitmap(orig_bitmap, pnImageW, pnImageH, pBitmap, pImage,
         &empty_image, &dont_free_orig_bitmap);
   if (!dont_free_orig_bitmap) {
      XFreePixmap(mainDisplay, orig_bitmap);
   }
   if (!rc) {
      if (empty_image) {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_GEN_PREVIEW_FOR_EMPTY_PS),
               pszPsPath);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return FALSE;
   }
   return rc;
}

static
int DoMyReadEPSFile(file_name, pn_image_w, pn_image_h, p_bitmap, p_image,
      pn_num_lines, ppsz_epsflines, pn_epsf_level, pf_llx, pf_lly,
      pf_urx, pf_ury, psz_write_date)
   char *file_name;
   int *pn_image_w, *pn_image_h, *pn_num_lines, *pn_epsf_level;
   Pixmap *p_bitmap;
   XImage **p_image;
   char ***ppsz_epsflines, *psz_write_date;
   float *pf_llx, *pf_lly, *pf_urx, *pf_ury;
{
   struct EPSLineRec *line_ptr, *next_line;
   int i;
   char *line=NULL, *c_ptr, loc_time[MAXSTRING+1];
   char tiff_eps_fname[MAXPATHLENGTH+1];
   char real_fname[MAXPATHLENGTH+1];
   int first_line=TRUE, image_w=0, image_h=0, epsf_level=0;
   int boundingbox_found=FALSE, preview_found=FALSE, found;
   int boundingbox_atend=FALSE, tiff_preview_ok=FALSE;
   int boundingbox_image_w=0, boundingbox_image_h=0;
   FILE *fp=NULL;
   struct stat stat_buf;
   Pixmap bitmap=None;
   XImage *image=NULL;
   float llx=0.0, lly=0.0, urx=0.0, ury=0.0;

   if (!cmdLineDosEpsFilter) {
      *pn_image_w = *pn_image_h = *pn_epsf_level = 0;
      *p_bitmap = None;
      *p_image = NULL;
      if (pn_num_lines != NULL) *pn_num_lines = 0;
      if (ppsz_epsflines != NULL) *ppsz_epsflines = NULL;
      *pn_epsf_level = 0;
      *pf_llx = llx; *pf_lly = lly; *pf_urx = urx; *pf_ury = ury;
      *psz_write_date = '\0';
   }
   *real_fname = '\0';
   GetEPSFullPath(file_name, real_fname, sizeof(real_fname));

   if ((fp=fopen(real_fname, "r")) == NULL) return BitmapOpenFailed;

   CleanUpLines();

   *tiff_eps_fname = '\0';
   while ((line=UtilGetALine(fp)) != NULL) {
      int need_to_free_line=TRUE;

      if (first_line) {
         if (line[0] == ((char)0xc5) && line[1] == ((char)0xd0) &&
               line[2] == ((char)0xd3) && line[3] == ((char)0xc6) &&
               line[4] == ((char)0x1e) && line[5] == '\0') {
            free(line);
            if (cmdLineDosEpsFilter) {
               if ((fp=GetTiffEPSIInfo(fp, tiff_eps_fname,
                     sizeof(tiff_eps_fname), &tiff_preview_ok)) == NULL) {
                  sprintf(gszMsgBox,
                        TgLoadString(STID_FAIL_TO_PARSE_WINEPS_FILE),
                        real_fname);
                  if (PRTGIF || mainWindow==None) {
                     fprintf(stderr, "%s\n", gszMsgBox);
                  } else {
                     MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
                  }
                  if (fp != NULL) fclose(fp);
                  if (*tiff_eps_fname != '\0') unlink(tiff_eps_fname);
                  CleanUpLines();
                  return BitmapFileInvalid;
               }
               fp = NULL;
               return BitmapSuccess;
            } else if ((fp=GetTiffEPSIInfo(fp, tiff_eps_fname,
                  sizeof(tiff_eps_fname), &tiff_preview_ok)) == NULL ||
                  (line=UtilGetALine(fp)) == NULL) {
               sprintf(gszMsgBox,
                     TgLoadString(STID_FAIL_TO_PARSE_WINEPS_FILE), real_fname);
               if (PRTGIF || mainWindow==None) {
                  fprintf(stderr, "%s\n", gszMsgBox);
               } else {
                  MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               }
               if (fp != NULL) fclose(fp);
               if (*tiff_eps_fname != '\0') unlink(tiff_eps_fname);
               CleanUpLines();
               return BitmapFileInvalid;
            }
         } else if (cmdLineDosEpsFilter && !cmdLinePreviewOnly) {
            printf("%s\n", line);
            free(line);
            while ((line=UtilGetALine(fp)) != NULL) {
               printf("%s\n", line);
               free(line);
            }
            fclose(fp);
            return BitmapSuccess;
         }
         first_line = FALSE;
         if (line[0] != '%' || line[1] != '!') {
            free(line);
            fclose(fp);
            if (*tiff_eps_fname != '\0') unlink(tiff_eps_fname);
            CleanUpLines();
            return BitmapFileInvalid;
         }
         if (cmdLineDosEpsFilter) {
            if (!cmdLinePreviewOnly) {
               printf("%s\n", line);
            }
         } else {
            AddLine(line);
         }
         need_to_free_line = FALSE;
      } else if (!cmdLineDosEpsFilter && (!boundingbox_found ||
            boundingbox_atend) && strncmp(line, "%%BoundingBox:", 14) == 0) {
         if (sscanf(&(line[14]), "%f %f %f %f", &llx, &lly, &urx, &ury) == 4) {
            boundingbox_found = TRUE;
            boundingbox_image_w = abs(round(urx-llx));
            boundingbox_image_h = abs(round(ury-lly));
         } else if (!boundingbox_found) {
            c_ptr = FindChar((int)'(', &(line[14]));
            if (strncmp(c_ptr, "atend)", 6) == 0) {
               boundingbox_atend = TRUE;
            }
         }
      } else if (!(cmdLineDosEpsFilter && !cmdLinePreviewOnly) &&
            !preview_found && strncmp(line, "%%BeginPreview:", 15) == 0) {
         int bps=1;
         char *preview_line=NULL;

         /*
          * The 4th argument to %%BeginPreview is the number of lines
          *         in the preview image.  It's probably a good idea
          *         to handle it or have a way not to read beyond
          *         %%EndImage or %%EndPreview.
          */
         if (sscanf(&(line[15]), "%d %d %d", &image_w, &image_h, &bps) != 3) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_PREVIEW_BOX_IN_EPS),
                  real_fname);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            if (need_to_free_line) free(line);
            fclose(fp);
            if (*tiff_eps_fname != '\0') unlink(tiff_eps_fname);
            CleanUpLines();
            return BitmapFileInvalid;
         } else {
            switch (bps) {
            case 1: break;
            case 8: break;
            default:
               sprintf(gszMsgBox, TgLoadString(STID_GIVEN_BPS_PREVIEW_NOT_SUP),
                     bps);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               if (need_to_free_line) free(line);
               fclose(fp);
               if (*tiff_eps_fname != '\0') unlink(tiff_eps_fname);
               CleanUpLines();
               return BitmapFileInvalid;
            }
         }
         if (boundingbox_found) {
            /*
             * If the width and height information in the boundingbox and
             *     the preview bitmap are differ only by one, we consider
             *     this a round-off error and adjust the boundingbox so
             *     that they become identical!
             */
            int diff=0;

            diff = image_w - boundingbox_image_w;
            if (diff == 1 || diff == (-1)) {
               if (urx >= llx) {
                  urx += (float)diff;
               } else {
                  urx -= (float)diff;
               }
            }
            diff = image_h - boundingbox_image_h;
            if (diff == 1 || diff == (-1)) {
               if (ury >= lly) {
                  ury += (float)diff;
               } else {
                  ury -= (float)diff;
               }
            }
         }
         preview_found = TRUE;
         if (!ReadPreviewBitmap(fp, real_fname, image_w, image_h, bps, &bitmap,
               &image)) {
            /* error message already displayed in ReadPreviewBitmap() */
            if (need_to_free_line) free(line);
            fclose(fp);
            if (*tiff_eps_fname != '\0') unlink(tiff_eps_fname);
            CleanUpLines();
            return BitmapFileInvalid;
         }
         found = FALSE;
         while ((preview_line=UtilGetALine(fp)) != NULL) {
            if (strncmp(preview_line, "%%EndPreview", 12) == 0) {
               found = TRUE;
               free(preview_line);
               break;
            }
            free(preview_line);
         }
         if (!found) {
            sprintf(gszMsgBox, TgLoadString(STID_INVALID_PREVIEW_BOX_IN_EPS),
                  real_fname);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            if (need_to_free_line) free(line);
            if (bitmap != None) {
               XFreePixmap(mainDisplay, bitmap);
               bitmap = None;
            }
            if (image != NULL) {
               XDestroyImage(image);
               image = NULL;
            }
            fclose(fp);
            if (*tiff_eps_fname != '\0') unlink(tiff_eps_fname);
            CleanUpLines();
            return BitmapFileInvalid;
         }
      } else {
         if (line[0] == '%' && line[1] == '!') epsf_level++;
         if (line[0] == '%' && line[1] == '%' && line[2] != '%') {
            /* Actaully, need to check DCS conformance! */
         } else if (!stripEPSComments || line[0] != '%') {
            if (cmdLineDosEpsFilter) {
               if (!cmdLinePreviewOnly) {
                  printf("%s\n", line);
               }
            } else {
               AddLine(line);
            }
            need_to_free_line = FALSE;
         }
      }
      if (need_to_free_line) free(line);
   }
   if (fp != NULL) fclose(fp);
   if (*tiff_eps_fname != '\0') unlink(tiff_eps_fname);
   if (!cmdLineDosEpsFilter) {
      if (!boundingbox_found) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_BBOX_IN_GIVEN_FILE),
               real_fname);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         if (bitmap != None) {
            XFreePixmap(mainDisplay, bitmap);
            bitmap = None;
         }
         if (image != NULL) {
            XDestroyImage(image);
            image = NULL;
         }
         CleanUpLines();
         return BitmapFileInvalid;
      }
      if (*tiff_eps_fname != '\0' && !tiff_preview_ok) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_CONVERT_WINEPS_PREVIEW),
               real_fname);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      if (autoEPSPreviewBitmap && image == NULL && bitmap == None &&
            !preview_found && !(*tiff_eps_fname != '\0' && tiff_preview_ok)) {
         if (AutoGeneratePreviewBitmap(real_fname, &bitmap, &image,
               &image_w, &image_h)) {
         }
      }
      if (stat(real_fname, &stat_buf) == 0) {
         strcpy(loc_time, ctime (&(stat_buf.st_mtime)));
         loc_time[24] = '\0';
      } else {
         /* do not translate -- program constants */
         strcpy(loc_time, "(unknown)");
      }
      *pn_image_w = image_w;
      *pn_image_h = image_h;
      *p_bitmap = bitmap;
      *p_image = image;
      if (pn_num_lines != NULL) *pn_num_lines = numLines;
      if (ppsz_epsflines != NULL) {
         *ppsz_epsflines = (char**)malloc(numLines*sizeof(char*));
         if (*ppsz_epsflines == NULL) FailAllocMessage();
      }
      for (i=0, line_ptr=topLine; line_ptr != NULL; line_ptr = next_line, i++) {
         next_line = line_ptr->next;
         if (ppsz_epsflines != NULL) {
            (*ppsz_epsflines)[i] = UtilStrDup(line_ptr->s);
            if ((*ppsz_epsflines)[i] == NULL) FailAllocMessage();
         }
         free(line_ptr->s);
         free(line_ptr);
      }
      *pn_epsf_level = epsf_level;
      *pf_llx = llx; *pf_lly = lly; *pf_urx = urx; *pf_ury = ury;
      strcpy(psz_write_date, loc_time);
   } else {
      for (i=0, line_ptr=topLine; line_ptr != NULL; line_ptr = next_line, i++) {
         next_line = line_ptr->next;
         free(line_ptr->s);
         free(line_ptr);
      }
   }
   topLine = botLine = NULL;
   numLines = 0;
   return BitmapSuccess;
}

int MyReadEPSFile(file_name, image_w, image_h, bitmap, image, num_lines,
      epsflines, epsf_level, llx, lly, urx, ury, write_date)
   char *file_name;
   int *image_w, *image_h, *num_lines, *epsf_level;
   Pixmap *bitmap;
   XImage **image;
   char ***epsflines, *write_date;
   float *llx, *lly, *urx, *ury;
{
   int rc;

   SaveStatusStrings();
   rc = DoMyReadEPSFile(file_name, image_w, image_h, bitmap, image,
         num_lines, epsflines, epsf_level, llx, lly, urx, ury, write_date);
   RestoreStatusStrings();
   return rc;
}

static
FILE *JustGetTiffEPSIInfo(fp, pszEPS, file_buf_sz)
   FILE *fp;
   char *pszEPS;
   int file_buf_sz;
{
   int bytes_to_read=0x1e, eps_sz, need_to_check_ps=TRUE;
   char buf[0x20];
   FILE *eps_fp=NULL;

   writeFileFailed = FALSE;
   *pszEPS = '\0';
   rewind(fp);
   if (fread(buf, sizeof(char), bytes_to_read, fp) != bytes_to_read) {
      fclose(fp);
      fclose(eps_fp);
      return NULL;
   }
   eps_sz = ReadDoubleWord(&buf[0x08]);

   if (MkTempFile(pszEPS, file_buf_sz, tmpDir, TOOL_NAME) == NULL) {
      fclose(fp);
      unlink(pszEPS);
      return NULL;
   }
   if ((eps_fp=fopen(pszEPS, "w")) == NULL) {
      fclose(fp);
      unlink(pszEPS);
      return NULL;
   }
   /* copy the EPS content from the original file to the new EPS file */
   while (eps_sz > 0) {
      bytes_to_read = min(eps_sz, sizeof(gszMsgBox));
      if (bytes_to_read !=
            (int)fread(gszMsgBox, sizeof(char), bytes_to_read, fp)) {
         fclose(fp);
         fclose(eps_fp);
         unlink(pszEPS);
         return NULL;
      }
      if (need_to_check_ps) {
         need_to_check_ps = FALSE;
         if (!(bytes_to_read>=2 && gszMsgBox[0]=='%' && gszMsgBox[1]=='!')) {
            fclose(fp);
            fclose(eps_fp);
            unlink(pszEPS);
            return NULL;
         }
      }
      if ((int)fwrite(gszMsgBox, sizeof(char), bytes_to_read, eps_fp) <= 0) {
         writeFileFailed = TRUE;
         fclose(fp);
         fclose(eps_fp);
         unlink(pszEPS);
         return NULL;
      }
      eps_sz -= bytes_to_read;
   }
   fclose(fp);
   fclose(eps_fp);
   return fopen(pszEPS, "r");
}

static
int JustReadEPSLines(xbm_ptr)
   struct XBmRec *xbm_ptr;
{
   struct EPSLineRec *line_ptr, *next_line;
   int i=0;
   char *c_ptr, *line, loc_time[MAXSTRING+1];
   char tiff_eps_fname[MAXPATHLENGTH+1];
   char real_fname[MAXPATHLENGTH+1];
   int first_line=TRUE, propagated_eps_status=INVALID;
   int boundingbox_found=FALSE, preview_found=FALSE, found;
   int boundingbox_atend=FALSE;
   float llx, lly, urx, ury;
   char *file_name=xbm_ptr->filename;
   FILE *fp=NULL;
   struct stat stat_buf;

   *real_fname = '\0';
   GetEPSFullPath(file_name, real_fname, sizeof(real_fname));

   if ((fp=fopen(real_fname, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_EPS_FILE_FOR_READ),
            real_fname);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
         fprintf(stderr, "   %s\n",
               TgLoadString(STID_EPS_OBJ_SKIPPED_FOR_PRINT));
      } else {
         char msg1[MAXSTRING];

         sprintf(msg1, "   %s", TgLoadString(STID_EPS_OBJ_SKIPPED_FOR_PRINT));
         TwoLineMsg(gszMsgBox, msg1);
      }
      return FALSE;
   }

   if (xbm_ptr->epsflines != NULL) {
      for (i = 0; i < xbm_ptr->num_epsf_lines; i++) {
         if (xbm_ptr->epsflines[i] != NULL) {
            free(xbm_ptr->epsflines[i]);
         }
      }
      free(xbm_ptr->epsflines);
      xbm_ptr->epsflines = NULL;
   }
   CleanUpLines();

   *tiff_eps_fname = '\0';
   while ((line=UtilGetALine(fp)) != NULL) {
      int need_to_free_line=TRUE;

      if (first_line) {
         if (line[0] == ((char)0xc5) && line[1] == ((char)0xd0) &&
               line[2] == ((char)0xd3) && line[3] == ((char)0xc6) &&
               line[4] == ((char)0x1e)) {
            free(line);
            if ((fp=JustGetTiffEPSIInfo(fp, tiff_eps_fname,
                  sizeof(tiff_eps_fname))) == NULL ||
                  (line=UtilGetALine(fp)) == NULL) {
               if (fp != NULL) fclose(fp);
               if (*tiff_eps_fname != '\0') unlink(tiff_eps_fname);
               CleanUpLines();
               return FALSE;
            }
         }
         first_line = FALSE;
         if (line[0] != '%' || line[1] != '!') {
            if (need_to_free_line) free(line);
            fclose (fp);
            CleanUpLines();
            return FALSE;
         }
         AddLine(line);
         need_to_free_line = FALSE;
         propagated_eps_status = INVALID;
      } else if ((!boundingbox_found || boundingbox_atend) &&
            strncmp(line, "%%BoundingBox:", 14) == 0) {
         if (sscanf(&(line[14]), "%f %f %f %f", &llx, &lly, &urx, &ury) == 4) {
            boundingbox_found = TRUE;
         } else if (!boundingbox_found) {
            c_ptr = FindChar((int)'(', &(line[14]));
            if (strncmp(c_ptr, "atend)", 6) == 0) {
               boundingbox_atend = TRUE;
            }
         }
         propagated_eps_status = INVALID;
      } else if (!preview_found && strncmp(line, "%%BeginPreview:", 15) == 0) {
         char *preview_line=NULL;

         preview_found = TRUE;
         found = FALSE;
         while ((preview_line=UtilGetALine(fp)) != NULL) {
            if (strncmp(preview_line, "%%EndPreview", 12) == 0) {
               found = TRUE;
               free(preview_line);
               break;
            }
            free(preview_line);
         }
         if (!found) {
            if (PRTGIF) {
               fprintf(stderr, TgLoadString(STID_INVALID_PREVIEW_BMP_IN_EPS),
                     real_fname);
            } else {
               sprintf(gszMsgBox, TgLoadString(STID_INVALID_PREVIEW_BMP_IN_EPS),
                     real_fname);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            }
            free(line);
            if (need_to_free_line) free(line);
            fclose(fp);
            CleanUpLines();
            return FALSE;
         }
         propagated_eps_status = INVALID;
      } else if (line[0] == '%' && line[1] == '%' && line[2] != '%') {
         /* Actaully, need to check DCS conformance! */
         if (propagated_eps_status != INVALID && line[2] != '+') {
            propagated_eps_status = INVALID;
         }
         UpdatePropagatedEPSInfo(line, &propagated_eps_status);
      } else {
         if (line[0] == '%' && line[1] == '!') {
            (xbm_ptr->epsf_level)++;
         } else if (!stripEPSComments || line[0] != '%') {
            AddLine(line);
            need_to_free_line = FALSE;
         }
         propagated_eps_status = INVALID;
      }
      if (need_to_free_line) free(line);
   }
   fclose(fp);
   if (*tiff_eps_fname != '\0') unlink(tiff_eps_fname);
   if (!boundingbox_found) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_BBOX_IN_GIVEN_FILE),
            real_fname);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      CleanUpLines();
      return FALSE;
   }
   if (stat(real_fname, &stat_buf) == 0) {
      strcpy(loc_time, ctime(&(stat_buf.st_mtime)));
      loc_time[24] = '\0';
   } else {
      /* do not translate -- program constants */
      strcpy(loc_time, "(unknown)");
   }
   if (strcmp(xbm_ptr->write_date, loc_time) != 0) {
      sprintf(gszMsgBox, TgLoadString(STID_EPS_FILE_NEWER_THAN_EPS_OBJ),
            real_fname);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         Msg(gszMsgBox);
      }
   }
   xbm_ptr->epsflines = (char**)malloc(numLines*sizeof(char*));
   if (xbm_ptr->epsflines == NULL) FailAllocMessage();
   xbm_ptr->num_epsf_lines = numLines;

   for (i=0, line_ptr=topLine; line_ptr != NULL; line_ptr = next_line, i++) {
      next_line = line_ptr->next;
      (xbm_ptr->epsflines)[i] = UtilStrDup(line_ptr->s);
      free(line_ptr->s);
      free(line_ptr);
   }
   topLine = botLine = NULL;
   numLines = 0;

   return TRUE;
}

void DumpEPSObj(FP, ObjPtr)
   FILE *FP;
   struct ObjRec *ObjPtr;
{
   int i;
   float scale=0, llx_psu, lly_psu, urx_psu, ury_psu, x_psu=0, y_psu=0;
   float w_psu, h_psu, bbox_w_psu, bbox_h_psu, dx_psu, dy_psu, ltx_psu, rby_psu;
   struct XBmRec *xbm_ptr=ObjPtr->detail.xbm;
   struct MtrxRec mtrx;
   int no_rotate=(ObjPtr->ctm==NULL), need_to_free_epsflines=FALSE;
         /* (ObjPtr->ctm->m[CTM_SIN]== 0 && ObjPtr->ctm->m[CTM_MSIN] == 0); */

   if (!xbm_ptr->save_epsf) {
      if (xbm_ptr->bitmap == None || xbm_ptr->epsflines == NULL) {
         if (!JustReadEPSLines(xbm_ptr)) {
            return;
         }
         need_to_free_epsflines = TRUE;
      } else {
         char loc_time[MAXSTRING+1], real_fname[MAXPATHLENGTH+1];
         struct stat stat_buf;

         *real_fname = '\0';
         GetEPSFullPath(xbm_ptr->filename, real_fname, sizeof(real_fname));

         if (stat(real_fname, &stat_buf) == 0) {
            strcpy(loc_time, ctime(&(stat_buf.st_mtime)));
            loc_time[24] = '\0';
         } else {
            /* do not translate -- program constants */
            strcpy(loc_time, "(unknown)");
         }
         if (strcmp(xbm_ptr->write_date, loc_time) != 0) {
            sprintf(gszMsgBox, TgLoadString(STID_EPS_FILE_NEWER_THAN_EPS_OBJ),
                  xbm_ptr->filename);
            if (PRTGIF) {
               fprintf(stderr, "%s\n", gszMsgBox);
            } else {
               Msg(gszMsgBox);
            }
         }
      }
   }
   /*
    * psu is PostScript Unit (72 psu/inch) and px be pixel.
    * scale's unit is psu/px.
    */
   scale = ((float)psDotsPerInch)/((float)PIX_PER_INCH*100.0)*printMag;

   llx_psu = (float)(((float)xbm_ptr->llx) / 1000.0);
   lly_psu = (float)(((float)xbm_ptr->lly) / 1000.0);
   urx_psu = (float)(((float)xbm_ptr->urx) / 1000.0);
   ury_psu = (float)(((float)xbm_ptr->ury) / 1000.0);

   if (no_rotate) {
      w_psu = (float)((float)(ObjPtr->obbox.rbx-ObjPtr->obbox.ltx) * scale);
      h_psu = (float)((float)(ObjPtr->obbox.rby-ObjPtr->obbox.lty) * scale);
   } else {
      w_psu = (float)((float)(ObjPtr->orig_obbox.rbx-ObjPtr->orig_obbox.ltx) *
            scale);
      h_psu = (float)((float)(ObjPtr->orig_obbox.rby-ObjPtr->orig_obbox.lty) *
            scale);
   }
   bbox_w_psu = urx_psu - llx_psu;
   bbox_h_psu = ury_psu - lly_psu;

   memset(&mtrx, 0, sizeof(struct MtrxRec));
   mtrx.image_w = (float)bbox_w_psu; mtrx.image_h = (float)bbox_h_psu;
   mtrx.w = w_psu; mtrx.h = h_psu;
   mtrx.rotate = ROTATE0; mtrx.flip = xbm_ptr->flip;

   CalcTransform(&mtrx);

   dx_psu = llx_psu*mtrx.m[0][0] + lly_psu*mtrx.m[1][0];
   dy_psu = llx_psu*mtrx.m[0][1] + lly_psu*mtrx.m[1][1];

   if (no_rotate) {
      ltx_psu = (float)((float)(ObjPtr->obbox.ltx) * scale);
      rby_psu = (float)(((float)psDotsPerInch)*psYOff[pageStyle] -
            ((float)(ObjPtr->obbox.rby) * scale));
      x_psu = (mtrx.transformed_w >= 0.0) ?
            (ltx_psu)-dx_psu : (ltx_psu+w_psu)-dx_psu;
      y_psu = (mtrx.transformed_h >= 0.0) ?
            (rby_psu)-dy_psu : (rby_psu+h_psu)-dy_psu;
   } else {
      float tmp_dx, tmp_dy;
      int dx, dy, ltx, rby, x, y, tmp_x, tmp_y;

      if (llx_psu == 0 && lly_psu == 0) {
         tmp_dx = dx_psu / scale;
         tmp_dy = dy_psu / scale;
      } else {
         tmp_dx = dx_psu / mtrx.dump_h_scale; /* tmp_dx = llx_psu */
         tmp_dy = dy_psu / mtrx.dump_v_scale; /* tmp_dy = lly_psu */
      }
      dx = round(tmp_dx);
      dy = round(tmp_dy);
      ltx = ObjPtr->orig_obbox.ltx;
      rby = ObjPtr->orig_obbox.rby;
      x = ltx - dx;
      y = rby + dy;
      TransformPointThroughCTM(x-ObjPtr->x, y-ObjPtr->y, ObjPtr->ctm,
            &tmp_x, &tmp_y);
      tmp_x += ObjPtr->x;
      tmp_y += ObjPtr->y;
      if (llx_psu == 0 && lly_psu == 0) {
         x_psu = (float)(((float)tmp_x) * scale);
         y_psu = (float)(((float)psDotsPerInch)*psYOff[pageStyle] -
               (((float)tmp_y) * scale));
      } else {
         x_psu = (float)(((float)tmp_x) * mtrx.dump_h_scale);
         y_psu = (float)(((float)psDotsPerInch)*psYOff[pageStyle] -
               (((float)tmp_y) * mtrx.dump_v_scale));
      }
   }
   /* do not translate -- program constants */
   fprintf(FP, "%% EPS\n");
   fprintf(FP, "end\n");
   fprintf(FP, "/tgiflevel%1d save def\n", xbm_ptr->epsf_level);
   fprintf(FP, "/tgifdictcount%1d countdictstack def\n", xbm_ptr->epsf_level);
   fprintf(FP, "/tgifopcount%1d count 1 sub def\n", xbm_ptr->epsf_level);
   fprintf(FP, "userdict begin\n");
   fprintf(FP, "/showpage {} def\n");
   fprintf(FP, "/letter {} def\n");
   fprintf(FP, "/legal {} def\n");
   fprintf(FP, "/a4 {} def\n");
   fprintf(FP, "/setpagedevice {pop} def\n");
   fprintf(FP, "/erasepage {} def\n");
   fprintf(FP, "/bop-hook {} def\n");
   fprintf(FP, "/eop-hook {} def\n");
   fprintf(FP, "/start-hook {} def\n");
   fprintf(FP, "/end-hook {} def\n");
   fprintf(FP, "0 setgray 0 setlinecap 1 setlinewidth\n");
   fprintf(FP, "0 setlinejoin 10 setmiterlimit [] 0 setdash newpath\n");
   fprintf(FP, "1 %1d %1d div %.3f mul 100 div div dup neg scale\n",
         psDotsPerInch, PIX_PER_INCH, printMag);
   fprintf(FP, "%1d %s mul neg %1d %s mul neg translate\n", psDotsPerInch,
         psXOffStr[pageStyle], psDotsPerInch, psYOffStr[pageStyle]);
   fprintf(FP, "\n");
   if (no_rotate) {
      fprintf(FP, "%.3f %.3f translate %.3f %.3f scale %1d rotate\n",
            x_psu, y_psu, mtrx.dump_h_scale, mtrx.dump_v_scale, (-mtrx.degree));
   } else {
      float m[6], ftmp_x, ftmp_y;
      int abs_ltx=ABS_X(ObjPtr->rotated_obbox[0].x);
      int abs_lty=ABS_Y(ObjPtr->rotated_obbox[0].y);
      int tmp_x, tmp_y;

      ReverseTransformPointThroughCTM(abs_ltx-ObjPtr->x, abs_lty-ObjPtr->y,
            ObjPtr->ctm, &tmp_x, &tmp_y);
      tmp_x += ObjPtr->x;
      tmp_y += ObjPtr->y;
      ftmp_x = ((float)tmp_x) - llx_psu;
      ftmp_y = ((float)tmp_y) - ury_psu;
      abs_ltx = round(ftmp_x);
      abs_lty = round(ftmp_y);
      TransformPointThroughCTM(abs_ltx-ObjPtr->x, abs_lty-ObjPtr->y,
            ObjPtr->ctm, &tmp_x, &tmp_y);
      ftmp_x = ((float)tmp_x) + ObjPtr->x;
      ftmp_y = ((float)tmp_y) + ObjPtr->y;

      fprintf(FP, "%.3f %.3f translate %.3f %.3f scale %1d rotate\n",
            x_psu, y_psu, mtrx.dump_h_scale, mtrx.dump_v_scale, (-mtrx.degree));

      m[CTM_SX] = ((float)ObjPtr->ctm->m[CTM_SX])/((float)1000.0);
      m[CTM_SY] = ((float)ObjPtr->ctm->m[CTM_SY])/((float)1000.0);
      m[CTM_SIN] = ((float)ObjPtr->ctm->m[CTM_SIN])/((float)1000.0);
      m[CTM_MSIN] = ((float)ObjPtr->ctm->m[CTM_MSIN])/((float)1000.0);
      fprintf(FP, "[%.3f %.3f %.3f %.3f 0 0] concat\n",
            m[CTM_SX], -m[CTM_SIN], -m[CTM_MSIN], m[CTM_SY]);
   }
   fprintf(FP, "\n");
   fprintf(FP, "%%%%BeginDocument: %s\n", xbm_ptr->filename);

   for (i = 0; i < xbm_ptr->num_epsf_lines; i++) {
      fprintf(FP, "%s\n", xbm_ptr->epsflines[i]);
   }
   fprintf(FP, "%%%%EndDocument\n");
   fprintf(FP, "count tgifopcount%1d sub dup 0 gt %s\n",
         xbm_ptr->epsf_level, "{{pop} repeat} {pop} ifelse");
   fprintf(FP, "countdictstack tgifdictcount%1d sub dup 0 gt %s\n",
         xbm_ptr->epsf_level, "{{end} repeat} {pop} ifelse");
   fprintf(FP, "tgiflevel%1d restore\n", xbm_ptr->epsf_level);
   fprintf(FP, "tgifdict begin\n");
   fprintf(FP, "\n");

   if (need_to_free_epsflines && xbm_ptr->epsflines != NULL) {
      for (i=0; i < xbm_ptr->num_epsf_lines; i++) {
         if (xbm_ptr->epsflines[i] != NULL) {
            free(xbm_ptr->epsflines[i]);
         }
      }
      if (xbm_ptr->epsflines != NULL) free(xbm_ptr->epsflines);
      xbm_ptr->num_epsf_lines = 0;
      xbm_ptr->epsflines = NULL;
   }
}

void SetEPSObjCTM(obj_ptr, p_orig_obbox)
   struct ObjRec *obj_ptr;
   struct BBRec *p_orig_obbox;
{
   struct XBmRec *xbm_ptr=obj_ptr->detail.xbm;
   int w=(p_orig_obbox->rbx-p_orig_obbox->ltx);
   int h=(p_orig_obbox->rby-p_orig_obbox->lty), image_w=0, image_h=0;

   if (obj_ptr->ctm != NULL) return;

   if (xbm_ptr->real_type == XBM_XBM) {
      image_w = xbm_ptr->image_w;
      image_h = xbm_ptr->image_h;
   } else {
      image_w = xbm_ptr->eps_w;
      image_h = xbm_ptr->eps_h;
   }
   if (image_w != w || image_h != h) {
      float fval=(float)0;
      struct XfrmMtrxRec ctm;

      memset(&ctm, 0, sizeof(struct XfrmMtrxRec));
      obj_ptr->orig_obbox.ltx = p_orig_obbox->ltx;
      obj_ptr->orig_obbox.lty = p_orig_obbox->lty;
      obj_ptr->orig_obbox.rbx = p_orig_obbox->rbx;
      obj_ptr->orig_obbox.rby = p_orig_obbox->rby;
      fval = ((float)w)/((float)image_w)*((float)1000.0);
      ctm.m[CTM_SX] = (double)fval;
      fval = ((float)h)/((float)image_h)*((float)1000.0);
      ctm.m[CTM_SY] = (double)fval;
      obj_ptr->obbox.rbx = obj_ptr->bbox.rbx = p_orig_obbox->ltx+image_w;
      obj_ptr->obbox.rby = obj_ptr->bbox.rby = p_orig_obbox->lty+image_h;
      SetCTM(obj_ptr, &ctm);
      AdjObjBBox(obj_ptr);
   }
}

struct ObjRec *CreateEPSObj(FileName, ImageW, ImageH, bitmap, image,
      num_lines, lines, epsf_level, llx, lly, urx, ury, write_date)
   char *FileName, **lines, *write_date;
   int ImageW, ImageH, num_lines, epsf_level;
   Pixmap bitmap;
   XImage *image;
   float *llx, *lly, *urx, *ury;
{
   struct XBmRec *xbm_ptr=NULL;
   struct ObjRec *obj_ptr=NULL;
   struct BBRec orig_obbox;
   int len=strlen(FileName), w=0, h=0;
   char *name=NULL;

   name = (char*)malloc((len+1)*sizeof(char));
   if (name == NULL) FailAllocMessage();
   strcpy(name, FileName);

   xbm_ptr = (struct XBmRec *)malloc(sizeof(struct XBmRec));
   if (xbm_ptr == NULL) FailAllocMessage();
   memset(xbm_ptr, 0, sizeof(struct XBmRec));

   xbm_ptr->image = image;
   xbm_ptr->image_w = ImageW;
   xbm_ptr->image_h = ImageH;
   xbm_ptr->bitmap = bitmap;
   xbm_ptr->data = NULL;

   if (bitmap == None) {
      xbm_ptr->eps_w = w = ((*urx) >= (*llx)) ? (int)((*urx)-(*llx)) :
            (int)((*llx)-(*urx));
      xbm_ptr->eps_h = h = ((*ury) >= (*lly)) ? (int)((*ury)-(*lly)) :
            (int)((*lly)-(*ury));
   } else {
      /* same as above! */
      xbm_ptr->eps_w = w = ((*urx) >= (*llx)) ? (int)((*urx)-(*llx)) :
            (int)((*llx)-(*urx));
      xbm_ptr->eps_h = h = ((*ury) >= (*lly)) ? (int)((*ury)-(*lly)) :
            (int)((*lly)-(*ury));
      w = ImageW;
      h = ImageH;
   }
   xbm_ptr->fill = objFill;
   xbm_ptr->flip = xbm_ptr->cached_flip = 0;
   xbm_ptr->llx = (int)((*llx)*1000.0);
   xbm_ptr->lly = (int)((*lly)*1000.0);
   xbm_ptr->urx = (int)((*urx)*1000.0);
   xbm_ptr->ury = (int)((*ury)*1000.0);
   xbm_ptr->cached_zoom = 0;
   xbm_ptr->cached_bitmap = None;
   xbm_ptr->cached_w = xbm_ptr->cached_h = 0;

   xbm_ptr->real_type = XBM_EPS;
   xbm_ptr->filename = name;
   strcpy(xbm_ptr->write_date, write_date);
   xbm_ptr->epsflines = lines;
   xbm_ptr->num_epsf_lines = num_lines;
   xbm_ptr->epsf_level = epsf_level;
   xbm_ptr->save_epsf = saveEPSLines;

   obj_ptr = (struct ObjRec *)malloc(sizeof(struct ObjRec));
   if (obj_ptr == NULL) FailAllocMessage();
   memset(obj_ptr, 0, sizeof(struct ObjRec));

   obj_ptr->bbox.ltx = obj_ptr->obbox.ltx = obj_ptr->x = drawOrigX;
   obj_ptr->bbox.lty = obj_ptr->obbox.lty = obj_ptr->y = drawOrigY;
   obj_ptr->bbox.rbx = obj_ptr->obbox.rbx = w + drawOrigX;
   obj_ptr->bbox.rby = obj_ptr->obbox.rby = h + drawOrigY;
   obj_ptr->type = OBJ_XBM;
   obj_ptr->color = colorIndex;
   if (mainDisplay != NULL) {
      UtilStrCpyN(obj_ptr->color_str, sizeof(obj_ptr->color_str),
            colorMenuItems[colorIndex]);
   }
   obj_ptr->id = objId++;
   obj_ptr->dirty = FALSE;
   obj_ptr->rotation = 0;
   obj_ptr->locked = FALSE;
   obj_ptr->detail.xbm = xbm_ptr;
   obj_ptr->fattr = obj_ptr->lattr = NULL;
   obj_ptr->ctm = NULL;

   memcpy(&orig_obbox, &obj_ptr->obbox, sizeof(struct BBRec));
   SetEPSObjCTM(obj_ptr, &orig_obbox);

   return obj_ptr;
}

void UpdateEPS()
{
   struct SelRec *sel_ptr;
   int count=0;

   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      if (sel_ptr->obj->type == OBJ_XBM &&
            sel_ptr->obj->detail.xbm->real_type == XBM_EPS) {
         count++;
      }
   }
   if (count == 0) {
      MsgBox(TgLoadString(STID_NO_TOP_LEVEL_EPS_SELECTED), TOOL_NAME, INFO_MB);
      return;
   }
   HighLightReverse();

   StartCompositeCmd();
   count = 0;
   for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=sel_ptr->next) {
      if (sel_ptr->obj->type == OBJ_XBM &&
            sel_ptr->obj->detail.xbm->real_type == XBM_EPS) {
         struct XBmRec *xbm_ptr, *new_xbm_ptr;
         struct ObjRec *obj_ptr, *new_obj_ptr;
         char write_date[32], **lines=NULL;
         int rc, num_lines=0, epsf_level, image_w, image_h, save_epsf;
         int transformed;
         float llx, lly, urx, ury;
         Pixmap bitmap;
         XImage *image=NULL;

         obj_ptr = sel_ptr->obj;
         xbm_ptr = obj_ptr->detail.xbm;
         transformed = (obj_ptr->ctm!=NULL);

         save_epsf = xbm_ptr->save_epsf;

         importingFile = TRUE;
         SetWatchCursor(drawWindow);
         SetWatchCursor(mainWindow);
         rc = MyReadEPSFile(xbm_ptr->filename, &image_w, &image_h, &bitmap,
               &image, (save_epsf ? &num_lines : NULL),
               (save_epsf ? &lines : NULL), &epsf_level, &llx, &lly, &urx, &ury,
               write_date);
         SetDefaultCursor(mainWindow);
         ShowCursor(); 

         if (rc != BitmapSuccess) {
            sprintf(gszMsgBox, TgLoadString(STID_CANNOT_IMPORT_GIVEN_EPS),
                  xbm_ptr->filename);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            importingFile = FALSE;
            return;
         }
         importingFile = FALSE;
         PrepareToReplaceAnObj(obj_ptr);
         if (save_epsf) saveEPSLines = TRUE;
         new_obj_ptr = CreateEPSObj(xbm_ptr->filename, image_w, image_h, bitmap,
               image, num_lines, lines, epsf_level, &llx, &lly, &urx, &ury,
               write_date);
         saveEPSLines = FALSE;

         if (!save_epsf && lines != NULL) {
            int i=0;

            for (i=0; i < num_lines; i++) {
               if (lines[i] != NULL) free(lines[i]);
            }
            free(lines);
         }
         new_obj_ptr->x = obj_ptr->x;
         new_obj_ptr->y = obj_ptr->y;
         if (transformed) {
            new_obj_ptr->obbox.ltx = obj_ptr->orig_obbox.ltx;
            new_obj_ptr->obbox.lty = obj_ptr->orig_obbox.lty;
            new_obj_ptr->obbox.rbx = obj_ptr->orig_obbox.rbx;
            new_obj_ptr->obbox.rby = obj_ptr->orig_obbox.rby;
            new_obj_ptr->obbox.rby = obj_ptr->orig_obbox.rby;
            SetCTM(new_obj_ptr, obj_ptr->ctm);
            AdjObjBBox(new_obj_ptr);
         } else {
            new_obj_ptr->bbox.ltx = obj_ptr->bbox.ltx;
            new_obj_ptr->bbox.lty = obj_ptr->bbox.lty;
            new_obj_ptr->bbox.rbx = obj_ptr->bbox.rbx;
            new_obj_ptr->bbox.rby = obj_ptr->bbox.rby;
            new_obj_ptr->obbox.ltx = obj_ptr->obbox.ltx;
            new_obj_ptr->obbox.lty = obj_ptr->obbox.lty;
            new_obj_ptr->obbox.rbx = obj_ptr->obbox.rbx;
            new_obj_ptr->obbox.rby = obj_ptr->obbox.rby;
         }
         new_xbm_ptr = new_obj_ptr->detail.xbm;
         new_xbm_ptr->flip = xbm_ptr->flip;

         ReplaceObj(obj_ptr, new_obj_ptr);
         sel_ptr->obj = new_obj_ptr;
         RecordReplaceAnObj(new_obj_ptr);
         FreeObj(obj_ptr);

         count++;
      }
   }
   EndCompositeCmd();

   if (count > 0) {
      UpdSelBBox();
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1), selLtY-GRID_ABS_SIZE(1),
            selRbX+GRID_ABS_SIZE(1), selRbY+GRID_ABS_SIZE(1));
      SetFileModified(TRUE);
      justDupped = FALSE;
      Msg(TgLoadString(STID_EPS_OBJECT_UPDATED));
   }
   HighLightForward();
}
