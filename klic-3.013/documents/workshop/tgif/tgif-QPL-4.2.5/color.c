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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/color.c,v 1.25 2011/05/16 16:21:56 william Exp $
 */

#define _INCLUDE_FROM_COLOR_C_

#include "tgifdefs.h"
#include "cmdids.h"

#include "choice.e"
#include "cmd.e"
#include "color.e"
#include "dialog.e"
#include "drawing.e"
#include "file.e"
#include "font.e"
#include "imgproc.e"
#include "mainloop.e"
#include "mainmenu.e"
#include "mark.e"
#include "menu.e"
#include "menuinfo.e"
#include "miniline.e"
#include "msg.e"
#include "obj.e"
#include "page.e"
#include "pattern.e"
#include "ps.e"
#include "raster.e"
#include "rect.e"
#include "select.e"
#include "setup.e"
#include "stk.e"
#include "strtbl.e"
#include "text.e"
#include "tgcwheel.e"
#include "util.e"
#include "xpixmap.e"

#include "xbm/upend.xbm"

#define MAX_VERTICAL_BTNS 6

int	maxColors=0;
int	defaultColorIndex=9;
int	colorIndex=0;
int	*colorPixels=NULL;
int	*xorColorPixels=NULL;
int	*colorLayerOn=NULL;
char	defaultColorStr[COLORSTRLEN];
char	myFgColorStr[COLORSTRLEN];
char	myBgColorStr[COLORSTRLEN];
char	myRubberBandColorStr[COLORSTRLEN];
char	**colorMenuItems=NULL;
XColor	*tgifColors=NULL;
XColor	*tgifRequestedColors=NULL;
XColor	myBgColor;
int	maxRGB=0;
int	colorDump=FALSE;
int	useLocalRGBTxt=FALSE;
int	printUsingRequestedColor=FALSE;
int	colorLayers=FALSE;
int	needToRedrawColorWindow=FALSE;
int	initColorDontReload=FALSE;
int	gnUpdatePixelObjCount=0;

char	defaultBgColorStr[COLORSTRLEN];
int	defaultBgColorIndex=INVALID;

char	altEditTextBgColorStr[COLORSTRLEN];
int	altEditTextBgIndex=INVALID;
char	altEditTextHighlightColorStr[COLORSTRLEN];
int	altEditTextHighlightIndex=INVALID;
int	useAltEditTextBgColor=FALSE;

char	pngExportTransparentColor[MAXSTRING];

static int colorWindowFirstIndex=0;
static int colorItemH=12;

static int	canChangeAttrColor=FALSE;
static int	defaultColorIndexInXDefaults=FALSE;
static int	defaultColorInXDefaults=FALSE;
static int	allocatedMaxColors=MAXCOLORS;

static char *defaultColorMenuItems[MAXCOLORS]={
   /* do not translate -- these strings are color names */
   "magenta", "red", "green", "blue", "yellow", "pink", "cyan", "CadetBlue",
   "white", "black", "DarkSlateGray"
};

void DefaultColorArrays(Entries, ForePixels, Valid, InitRV, StatusStr)
   int Entries, **ForePixels, **Valid, **InitRV;
   char ***StatusStr;
{
   register int i, *fore_pixels, pixel, *valid, *init_rv;

   pixel = myFgPixel;
   *ForePixels = fore_pixels = (int*)malloc(Entries*sizeof(int));
   if (*ForePixels == NULL) FailAllocMessage();
   *Valid = valid = (int*)malloc(Entries*sizeof(int));
   if (*Valid == NULL) FailAllocMessage();
   *InitRV = init_rv = (int*)malloc(Entries*sizeof(int));
   if (*InitRV == NULL) FailAllocMessage();
   for (i=0; i < Entries; i++) {
      *fore_pixels++ = pixel;
      *valid++ = TRUE;
      *init_rv++ = FALSE;
   }
   if (StatusStr != NULL) {
      *StatusStr = (char**)malloc(Entries*sizeof(char*));
      if (*StatusStr == NULL) FailAllocMessage();
      for (i=0; i < Entries; i++) {
         (*StatusStr)[i] = (char*)malloc((MAXSTRING+1)*sizeof(char));
         if ((*StatusStr)[i] == NULL) FailAllocMessage();
         *(*StatusStr)[i] = '\0';
      }
   }
}

int GetDrawingBgPixel(index, pixel)
   int index, pixel;
{
   if (index != INVALID && index != BACKPAT) {
      return pixel;
   } else if (myFileBgColorStr == NULL) {
      return myBgPixel;
   } else {
      return myFileBgPixel;
   }
}

int GetDrawingFgPixel(index, pixel)
   int index, pixel;
{
   if (index != INVALID && index != BACKPAT) {
      return pixel;
   } else if (myFileBgColorStr == NULL || myFileFgColorStr == NULL) {
      return myFgPixel;
   } else {
      return myFileFgPixel;
   }
}

void GetDrawingFgColorStr(index, pixel, color_buf, buf_sz)
   int index, pixel, buf_sz;
   char *color_buf;
{
   if (index != INVALID && index != BACKPAT) {
      UtilStrCpyN(color_buf, buf_sz, colorMenuItems[index]);
   } else if (myFileBgColorStr == NULL || myFileFgColorStr == NULL) {
      UtilStrCpyN(color_buf, buf_sz, myFgColorStr);
   } else {
      UtilStrCpyN(color_buf, buf_sz, myFileFgColorStr);
   }
}

struct LocalColorRec {
   char *name, *spec;
   int len;
};

static struct LocalColorRec *privateColorInfo=NULL;

int TgifParseColor(psz_color, p_color)
   char *psz_color;
   XColor *p_color;
{
   struct LocalColorRec *ptr;
   int len;

   if (p_color != NULL) memset(p_color, 0, sizeof(XColor));
   if (!useLocalRGBTxt || *psz_color == '#') {
      return (int)XParseColor(mainDisplay, mainColormap, psz_color, p_color);
   }
   len = strlen(psz_color);
   for (ptr=privateColorInfo; ptr->name != NULL; ptr++) {
      if (len == ptr->len && strcmp(psz_color, ptr->name) == 0) {
         if (ptr->spec != NULL) {
            return (int)XParseColor(mainDisplay, mainColormap, ptr->spec,
                  p_color);
         }
         break;
      }
   }
   return (int)XParseColor(mainDisplay, mainColormap, psz_color, p_color);
}

int ParseAndAllocColorByName(colorname, color, red_req, green_req, blue_req)
   char *colorname;
   XColor *color;
   unsigned short *red_req, *green_req, *blue_req;
{
   Colormap colormap;

   if (!TgifParseColor(colorname, color)) {
      fprintf(stderr, TgLoadString(STID_WARN_CANNOT_PARSE_NAMED_COLOR),
            colorname);
      fprintf(stderr, "\n");
      return FALSE;
   }
   if (red_req != NULL) *red_req = color->red;
   if (green_req != NULL) *green_req = color->green;
   if (blue_req != NULL) *blue_req = color->blue;
   if (!XAllocColor(mainDisplay, mainColormap, color)) {
      if (newColormapUsed) {
         fprintf(stderr, TgLoadString(STID_CANNOT_ALLOC_NAMED_COLOR),
               colorname);
         fprintf(stderr, "\n");
         return FALSE;
      }
      colormap = XCopyColormapAndFree(mainDisplay, mainColormap);
      mainColormap = colormap;
      newColormapUsed = TRUE;
      if (mainWindow != None) {
         XSetWindowColormap(mainDisplay, mainWindow, mainColormap);
      }
      if (!XAllocColor(mainDisplay, mainColormap, color)) {
         fprintf(stderr, TgLoadString(STID_CANNOT_ALLOC_NAMED_COLOR),
               colorname);
         fprintf(stderr, "\n");
         return FALSE;
      }
   }
   return TRUE;
}

void RecalcXorPixels()
{
   int i, bg_pixel;

   bg_pixel = GetDrawingBgPixel(INVALID, INVALID);
   if (xorColorPixels != NULL) {
      for (i=0; i < maxColors; i++) {
         xorColorPixels[i] = colorPixels[i] ^ bg_pixel;
      }
   }
}

static
void InitLocalRGBTxt()
{
   char *c_ptr;

   useLocalRGBTxt = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"LocalRGBTxt")) != NULL) {
      char *fname=UtilStrDup(c_ptr);
      FILE *fp;

      if (fname == NULL) { FailAllocMessage(); return; }
      if ((fp=fopen(fname, "r")) != NULL) {
         int num_lines=0, ok=TRUE, line_num=0, info_index=0;
         char *buf;

         while (fgets(gszMsgBox, sizeof(gszMsgBox), fp) != NULL) {
            int len=strlen(gszMsgBox);

            if (len > 0 && gszMsgBox[len-1] != '\n') {
               while (fgets(gszMsgBox, sizeof(gszMsgBox), fp) != NULL) {
                  len = strlen(gszMsgBox);
                  if (len > 0 && gszMsgBox[len-1] == '\n') break;
               }
            }
            num_lines++;
         }
         rewind(fp);
         if ((privateColorInfo=(struct LocalColorRec *)malloc(
               (num_lines+1)*sizeof(struct LocalColorRec))) == NULL) {
            FailAllocMessage();
            ok = FALSE;
         }
         while ((buf=UtilGetALine(fp)) != NULL) {
            char *red_str, *green_str, *blue_str, *color_name;

            line_num++;
            if ((red_str=strtok(buf, " \t\n\r")) != NULL &&
                  (green_str=strtok(NULL, " \t\n\r")) != NULL &&
                  (blue_str=strtok(NULL, " \t\n\r")) != NULL &&
                  (color_name=strtok(NULL, "\t\n\r")) != NULL) {
               struct LocalColorRec *color_info_ptr;
               int red, green, blue;

               color_info_ptr = (&privateColorInfo[info_index]);
               while (*color_name == ' ' || *color_name == '\t') color_name++;
               if (sscanf(red_str, "%d", &red) == 1 &&
                     sscanf(green_str, "%d", &green) == 1 &&
                     sscanf(blue_str, "%d", &blue) == 1 &&
                     red >= 0 && red <= 0xff && green >= 0 && green <= 0xff &&
                     blue >= 0 && blue <= 0xff && *color_name != '\0') {
                  color_info_ptr->name = UtilStrDup(color_name);
                  if (color_info_ptr->name == NULL) FailAllocMessage();
                  color_info_ptr->len = strlen(color_info_ptr->name);
                  sprintf(gszMsgBox, "#%02x%02x%02x", red, green, blue);
                  color_info_ptr->spec = UtilStrDup(gszMsgBox);
                  if (color_info_ptr->spec == NULL) FailAllocMessage();
                  info_index++;
               } else {
                  fprintf(stderr,
                        TgLoadCachedString(CSTID_MALFORMED_LINE_NUM_IN_FILE),
                        line_num, fname);
                  fprintf(stderr, "\n");
               }
            } else {
               fprintf(stderr,
                     TgLoadCachedString(CSTID_MALFORMED_LINE_NUM_IN_FILE),
                     line_num, fname);
               fprintf(stderr, "\n");
            }
            privateColorInfo[info_index].name = NULL;
            privateColorInfo[info_index].len = 0;
            free(buf);
         }
         fclose(fp);
         if (ok) useLocalRGBTxt = TRUE;
      } else {
         fprintf(stderr, TgLoadString(STID_INVALID_FILE_IN_X_DEFAULT),
               fname, TOOL_NAME, "LocalRGBTxt");
         fprintf(stderr, "\n");
      }
      free(fname);
   }
   printUsingRequestedColor = cmdLineRequestedColor;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PrintUsingRequestedColor")) !=
         NULL && (strcmp(c_ptr, "true") == 0 || strcmp(c_ptr, "True") == 0)) {
      printUsingRequestedColor = TRUE;
   }
}

static
int InitColorFromXPixmap(pn_num_colors, pppsz_color_str)
   int *pn_num_colors;
   char ***pppsz_color_str;
   /* return TRUE mean maxColors and initial colors are setup */
{
   char *c_ptr, xpm_fname[MAXPATHLENGTH], *color_char=NULL, **color_str=NULL;
   int i, ncolors=0, rc;

   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ColorFromXPixmap")) == NULL) {
      return FALSE;
   }
   strncpy(xpm_fname, c_ptr, sizeof(xpm_fname));
   rc = MyReadPixmapFile(xpm_fname, NULL, NULL, NULL, NULL, NULL,
         NULL, NULL, NULL, &ncolors, NULL, NULL, &color_char, &color_str,
         NULL, NULL);
   if (rc != BitmapSuccess) {
      fprintf(stderr, TgLoadString(STID_INVALID_XDEF_COLORXPM_GET),
            TOOL_NAME, "ColorFromXPixmap", xpm_fname);
      fprintf(stderr, "\n");
      return FALSE;
   }
   if (myReadTransparentPixmap) {
      fprintf(stderr, TgLoadString(STID_INVALID_XDEF_COLORXPM_TRANS),
            TOOL_NAME, "ColorFromXPixmap", xpm_fname);
      fprintf(stderr, "\n");
      if (color_str != NULL) {
         for (i=0; i < ncolors; i++) {
            if (color_str[i] != NULL) {
               free(color_str[i]);
            }
         }
         free(color_str);
      }
      if (color_char != NULL) free(color_char);
      return FALSE;
   }
   *pn_num_colors = ncolors;
   *pppsz_color_str = color_str;
   if (color_char != NULL) free(color_char);
   return TRUE;
}

static
int DoSetAltEditTextBgColor()
{
   int new_alloc=FALSE;

   if (*altEditTextBgColorStr == '\0') return FALSE;

   altEditTextBgIndex = QuickFindColorIndex(NULL, altEditTextBgColorStr,
         &new_alloc, FALSE);
   if (altEditTextBgIndex == INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_ALLOC_EDIT_TEXT_BGCOLOR),
            altEditTextBgColorStr);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      *altEditTextBgColorStr = '\0';
      return FALSE;
   }
   if (colorLayers) RedrawColorWindow();

   return TRUE;
}

static
int DoSetAltEditTextHighlightColor()
{
   int new_alloc=FALSE;

   if (*altEditTextHighlightColorStr == '\0') return FALSE;

   altEditTextHighlightIndex = QuickFindColorIndex(NULL,
         altEditTextHighlightColorStr, &new_alloc, FALSE);
   if (altEditTextHighlightIndex == INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_ALLOC_EDIT_TEXT_HICOLOR),
            altEditTextHighlightColorStr);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      *altEditTextHighlightColorStr = '\0';
      return FALSE;
   }
   if (colorLayers) RedrawColorWindow();

   return TRUE;
}

static
int DoSetDefaultBgColor()
{
   int new_alloc=FALSE;

   if (*defaultBgColorStr == '\0') return FALSE;

   defaultBgColorIndex = QuickFindColorIndex(NULL, defaultBgColorStr,
         &new_alloc, FALSE);
   if (defaultBgColorIndex == INVALID) {
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_ALLOC_DEF_BGCOLOR),
            defaultBgColorStr);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      *defaultBgColorStr = '\0';
      return FALSE;
   }
   return TRUE;
}

static int std_palette8[] = { 0x00, 0xff };
static int std_palette27[] = { 0x00, 0x80, 0xff };
static int std_palette64[] = { 0x00, 0x55, 0xaa, 0xff };
static int std_palette216[] = { 0x00, 0x33, 0x66, 0x99, 0xcc, 0xff };
static char *openoffice_galaxy_palette[] = {
      /* black */
      "#000000",
      "#151a1f",
      "#29333c",
      "#4b5a66",
      "#8b97a0",
      "#bec5c9",
      "#d8dcdf",
      "#edf1f5",
      "#ffffff",
      /* blue-gray */
      "#123349",
      "#35556b",
      "#5382a1",
      "#a3b8cb",
      "#dae3ea",
      /* more-blue-gray */
      "#003e6d",
      "#005a9b",
      "#0877bc",
      "#71c0eb",
      "#a4cef0",
      "#ddedfa",
      /* blue */
      "#063d80",
      "#0646a2",
      "#0852d4",
      "#085dff",
      "#0fa0ff",
      "#b8f1ff",
      /* green */
      "#063e00",
      "#096000",
      "#128902",
      "#1cbb04",
      "#8dff1a",
      "#cfff9e",
      /* yellow */
      "#b2bc00",
      "#7f7800",
      "#46580b",
      "#314005",
      "#634900",
      "#c69200",
      /* orange */
      "#f26a00",
      "#faa217",
      "#ffc726",
      "#ffdf24",
      "#fff67b",
      "#ffff9e",
      /* red */
      "#6d0000",
      "#8e0000",
      "#c70909",
      "#ff0000",
      "#fc5b5b",
      "#ffa199",
      /* purple */
      "#2e008f",
      "#8866cc",
      "#c2b0e5",
      NULL
};
static char *openoffice_google_palette[] = {
      /* black */
      "#000000",
      "#262626",
      "#404040",
      "#575757",
      "#747474",
      "#909090",
      "#a8a8a8",
      "#c5c5c5",
      "#e6e6e6",
      "#ffffff",
      /* saturated */
      "#ff0000",
      "#ff9900",
      "#ffff00",
      "#98fe00",
      "#00ff00",
      "#00ffff",
      "#0035ff",
      "#9900ff",
      "#ff00ff",
      "#ff0187",
      /* red */
      "#660000",
      "#990000",
      "#cc0000",
      "#f03939",
      "#ea9999",
      "#f4cccc",
      /* orange */
      "#783f04",
      "#b45f06",
      "#e69138",
      "#f6b26b",
      "#f9cb9c",
      "#fce5cd",
      /* yellow */
      "#7f6b00",
      "#bfa000",
      "#f1d932",
      "#ffe766",
      "#ffec99",
      "#fff6cc",
      /* light green */
      "#2c4a00",
      "#4c7f00",
      "#5f9f00",
      "#84be2d",
      "#acdb65",
      "#d1e7b1",
      /* green */
      "#1c4e13",
      "#26761d",
      "#5ba84f",
      "#84c47d",
      "#b0d7a8",
      "#d6ead3",
      /* cyan */
      "#0c343d",
      "#134f5c",
      "#45818e",
      "#76a5af",
      "#a2c4c9",
      "#d0e0e3",
      /* blue */
      "#073763",
      "#0b5394",
      "#3d85c6",
      "#6fa8dc",
      "#9fc5e8",
      "#cfe2f3",
      /* purple */
      "#20124d",
      "#351c75",
      "#674ea7",
      "#8e7cc3",
      "#b4a7d6",
      "#d9d2e9",
      /* magenta */
      "#4c114a",
      "#741b68",
      "#a64d9b",
      "#c27bbd",
      "#d5a6d4",
      "#ead1ea",
      /* redish */
      "#5a002f",
      "#94004e",
      "#c10066",
      "#e53b94",
      "#e895c1",
      "#f6d1e5",
      NULL
};

void InitColor()
{
   int i, index=0, looking_for_default_color=FALSE, tmp_max, color_from_pixmap=FALSE;
   XColor color;
   char buf[80], *c_ptr, fg_color[80], bg_color[80], brdr_color[80];
   long bg_gray=(long)0;
   int bg_allocated=FALSE, fg_allocated=FALSE, brdr_allocated=FALSE, ncolors=0;
   int std_palette=0, need_to_handle_additional_colors=FALSE;
   char **color_str=NULL, *additional_colors_buf=NULL;

   InitLocalRGBTxt();

   strcpy(fg_color, "Black");
   strcpy(bg_color, "White");
   strcpy(brdr_color, "Black");
   if (!initColorDontReload) {
      reverseVideo = FALSE;
      if (cmdLineRV != INVALID) {
         reverseVideo = cmdLineRV;
      } else if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"ReverseVideo")) !=
            NULL) {
         reverseVideo = FALSE;
         if (UtilStrICmp(c_ptr, "on") == 0 || UtilStrICmp(c_ptr, "true") == 0) {
            reverseVideo = TRUE;
         }
      }
      colorDump = (!PRTGIF);
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"InitialPrintInColor")) !=
            NULL && UtilStrICmp(c_ptr, "false") == 0) {
         colorDump = FALSE;
      }
   }
   defaultColorInXDefaults = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "DefaultColor"))
         != NULL) {
      defaultColorInXDefaults = TRUE;
      UtilStrCpyN(defaultColorStr, sizeof(defaultColorStr), c_ptr);
   }
   defaultColorIndexInXDefaults = FALSE;
   defaultColorIndex = 9;
   if (!defaultColorInXDefaults) {
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "DefaultColorIndex"))
            != NULL) {
         defaultColorIndexInXDefaults = TRUE;
         defaultColorIndex = atoi(c_ptr);
      }
   }
   color_from_pixmap = InitColorFromXPixmap(&ncolors, &color_str);
   if (color_from_pixmap && ncolors > 0 && color_str != NULL) {
      maxColors = ncolors;
      need_to_handle_additional_colors = TRUE;
   } else {
      if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
            "UseMobileWebSafePalette")) != NULL) {
         maxColors = std_palette = 216;
      } else if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
            "UseStdPalette216")) != NULL) {
         maxColors = std_palette = 216;
      } else if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
            "UseStdPalette64")) != NULL) {
         maxColors = std_palette = 64;
      } else if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
            "UseStdPalette27")) != NULL) {
         maxColors = std_palette = 27;
      } else if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
            "UseStdPalette8")) != NULL) {
         maxColors = std_palette = 8;
      } else if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
            "UseOpenOfficeGalaxyPalette")) != NULL) {
         maxColors = std_palette = 53;
      } else if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
            "UseOpenOfficeGooglePalette")) != NULL) {
         maxColors = std_palette = 80;
      } else {
         maxColors = MAXCOLORS;
         if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "MaxColors")) != NULL) {
            maxColors = atoi(c_ptr);
            if (maxColors <= 0) {
               fprintf(stderr, TgLoadString(STID_INVALID_XDEF_USE_ALT_VALUE),
                     TOOL_NAME, "MaxColors", c_ptr, MAXCOLORS);
               fprintf(stderr, "\n");
               maxColors = MAXCOLORS;
            }
         }
      }
      if (std_palette > 0) {
         need_to_handle_additional_colors = TRUE;
      }
   }
   if (need_to_handle_additional_colors) {
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"AdditionalColors")) !=
            NULL) {
         char *buf_copy=UtilStrDup(c_ptr), *psz_state=NULL;

         if (buf_copy == NULL) FailAllocMessage();
         UtilTrimBlanks(buf_copy);
         additional_colors_buf = UtilStrDup(buf_copy);
         if (additional_colors_buf == NULL) FailAllocMessage();
         for (c_ptr=UtilStrTok(buf_copy, ",\t\n\r", &psz_state); c_ptr != NULL;
               c_ptr=UtilStrTok(NULL, ",\t\n\r", &psz_state)) {
            maxColors++;
         }
         UtilFree(buf_copy);
      }
   }
   if (colorDisplay) {
      if (((cmdLineForeground != NULL && cmdLineBackground == NULL) ||
            (cmdLineForeground == NULL && cmdLineBackground != NULL)) &&
            reverseVideo) {
         fprintf(stderr, TgLoadString(STID_NORM_VIDEO_MODE_ASSUMED),
               (cmdLineForeground == NULL ? "-bg" : "-fg"));
         fprintf(stderr, "\n");
         reverseVideo = FALSE;
      }
      if (cmdLineForeground != NULL) {
         strcpy(fg_color, cmdLineForeground);
      } else if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"Foreground")) !=
            NULL) {
         if (reverseVideo) {
            strcpy(bg_color, c_ptr);
         } else {
            strcpy(fg_color, c_ptr);
         }
      } else if (reverseVideo) {
         strcpy(fg_color, "white");
      } else {
         strcpy(fg_color, "black");
      }
      if (cmdLineBackground != NULL) {
         strcpy(bg_color, cmdLineBackground);
      } else if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"Background")) !=
            NULL) {
         if (reverseVideo) {
            strcpy(fg_color, c_ptr);
         } else {
            strcpy(bg_color, c_ptr);
         }
      } else if (reverseVideo) {
         strcpy(bg_color, "black");
      } else {
         strcpy(bg_color, "white");
      }
      if (cmdLineBorder != NULL) {
         strcpy(brdr_color, cmdLineBorder);
      } else if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
            "BorderColor")) != NULL) {
         strcpy(brdr_color, c_ptr);
      } else {
         strcpy(brdr_color, fg_color);
      }
   } else {
      if (reverseVideo) {
         strcpy(fg_color, "white");
         strcpy(bg_color, "black");
      } else {
         strcpy(fg_color, "black");
         strcpy(bg_color, "white");
      }
      strcpy(brdr_color, fg_color);
   }

   if (maxColors <= 0) {
      fprintf(stderr, "%s\n", TgLoadString(STID_ZERO_COLORS_SPECIFIED));
      exit (-1);
   }

   colorPixels = (int*)malloc(maxColors*sizeof(int));
   xorColorPixels = (int*)malloc(maxColors*sizeof(int));
   colorLayerOn = (int*)malloc(maxColors*sizeof(int));
   colorMenuItems = (char**)malloc(maxColors*sizeof(char*));
   if (colorPixels == NULL || xorColorPixels == NULL || colorLayerOn == NULL ||
         colorMenuItems == NULL) {
      FailAllocMessage();
   }

   tgifColors = (XColor*)malloc(maxColors*sizeof(XColor));
   tgifRequestedColors = (XColor*)malloc(maxColors*sizeof(XColor));
   if (tgifColors == NULL || tgifRequestedColors == NULL) FailAllocMessage();

   allocatedMaxColors = maxColors;
   for (i=0; i < allocatedMaxColors; i++) {
      colorMenuItems[i] = (char*)malloc(COLORSTRLEN*sizeof(char));
      if (colorMenuItems[i] == NULL) FailAllocMessage();
   }
   if (color_from_pixmap && ncolors > 0 && color_str != NULL) {
      for (i=0; i < maxColors; i++) {
         strcpy(colorMenuItems[i], color_str[i]);
         free(color_str[i]);
         colorLayerOn[i] = TRUE;
      }
      free(color_str);
      color_str = NULL;
   } else {
      int r=0, g=0, b=0;

      switch (std_palette) {
      case 0:
         for (i=0; i < maxColors; i++) {
            sprintf(buf, "Color%1d", i);
            if ((c_ptr=XGetDefault(mainDisplay, TOOL_NAME, buf)) != NULL) {
               UtilStrCpyN(colorMenuItems[i], COLORSTRLEN, c_ptr);
            } else if (i < MAXCOLORS) {
               UtilStrCpyN(colorMenuItems[i], COLORSTRLEN,
                     defaultColorMenuItems[i]);
            } else {
               fprintf(stderr, TgLoadString(STID_CANNOT_GET_X_DEFAULT),
                     TOOL_NAME, buf);
               fprintf(stderr, "\n");
               exit(-1);
            }
            UtilTrimBlanks(colorMenuItems[i]);
            colorLayerOn[i] = TRUE;
         }
         break;
      case 8:
         i = 0;
         for (r=0; r < 2; r++) {
            for (g=0; g < 2; g++) {
               for (b=0; b < 2; b++) {
                  sprintf(colorMenuItems[i], "#%02x%02x%02x",
                        std_palette8[r], std_palette8[g], std_palette8[b]);
                  colorLayerOn[i] = TRUE;
                  i++;
               }
            }
         }
         if (!defaultColorIndexInXDefaults) {
            if (reverseVideo) {
               defaultColorIndex = 7;
            } else {
               defaultColorIndex = 0;
            }
         }
         break;
      case 27:
         i = 0;
         for (r=0; r < 3; r++) {
            for (g=0; g < 3; g++) {
               for (b=0; b < 3; b++) {
                  sprintf(colorMenuItems[i], "#%02x%02x%02x",
                        std_palette27[r], std_palette27[g], std_palette27[b]);
                  colorLayerOn[i] = TRUE;
                  i++;
               }
            }
         }
         if (!defaultColorIndexInXDefaults) {
            if (reverseVideo) {
               defaultColorIndex = 26;
            } else {
               defaultColorIndex = 0;
            }
         }
         break;
      case 64:
         i = 0;
         for (r=0; r < 4; r++) {
            for (g=0; g < 4; g++) {
               for (b=0; b < 4; b++) {
                  sprintf(colorMenuItems[i], "#%02x%02x%02x",
                        std_palette64[r], std_palette64[g], std_palette64[b]);
                  colorLayerOn[i] = TRUE;
                  i++;
               }
            }
         }
         if (!defaultColorIndexInXDefaults) {
            if (reverseVideo) {
               defaultColorIndex = 63;
            } else {
               defaultColorIndex = 0;
            }
         }
         break;
      case 216:
         i = 0;
         for (r=0; r < 6; r++) {
            for (g=0; g < 6; g++) {
               for (b=0; b < 6; b++) {
                  sprintf(colorMenuItems[i], "#%02x%02x%02x",
                        std_palette216[r], std_palette216[g], std_palette216[b]);
                  colorLayerOn[i] = TRUE;
                  i++;
               }
            }
         }
         if (!defaultColorIndexInXDefaults) {
            if (reverseVideo) {
               defaultColorIndex = 215;
            } else {
               defaultColorIndex = 0;
            }
         }
         break;
      case 53: /* OpenOffice Galaxy Palette */
         for (i=0; i < std_palette; i++) {
            strcpy(colorMenuItems[i], openoffice_galaxy_palette[i]);
            colorLayerOn[i] = TRUE;
         }
         if (!defaultColorIndexInXDefaults) {
            if (reverseVideo) {
               defaultColorIndex = 8;
            } else {
               defaultColorIndex = 0;
            }
         }
         break;
      case 80: /* OpenOffice Google Palette */
         for (i=0; i < std_palette; i++) {
            strcpy(colorMenuItems[i], openoffice_google_palette[i]);
            colorLayerOn[i] = TRUE;
         }
         if (!defaultColorIndexInXDefaults) {
            if (reverseVideo) {
               defaultColorIndex = 9;
            } else {
               defaultColorIndex = 0;
            }
         }
         break;
      }
   }
   if (need_to_handle_additional_colors && additional_colors_buf != NULL) {
      char *psz_state=NULL;

      for (c_ptr=UtilStrTok(additional_colors_buf, ",\t\n\r", &psz_state); c_ptr != NULL;
            c_ptr=UtilStrTok(NULL, ",\t\n\r", &psz_state)) {
         UtilStrCpyN(colorMenuItems[i], COLORSTRLEN, c_ptr);
         colorLayerOn[i] = TRUE;
         i++;
      }
      UtilFree(additional_colors_buf);
   }
   if (threeDLook) {
      static int initVryLtPixel=FALSE;

      myBlackPixel = myWhitePixel = myLtGryPixel = myDkGryPixel = INVALID;
      myVryLtPixel = myYellowPixel = INVALID;
      memset(&color, 0, sizeof(XColor));
      if (ParseAndAllocColorByName("black", &color, NULL, NULL, NULL)) {
         myBlackPixel = color.pixel;
      }
      if (ParseAndAllocColorByName("white", &color, NULL, NULL, NULL)) {
         myWhitePixel = color.pixel;
      }
      if (ParseAndAllocColorByName("#ffff00", &color, NULL, NULL, NULL)) {
         myYellowPixel = color.pixel;
      }
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"LightGrayColor")) != NULL) {
         if (ParseAndAllocColorByName(c_ptr, &color, NULL, NULL, NULL)) {
            myLtGryPixel = color.pixel;
         } else {
            fprintf(stderr, TgLoadString(STID_INVALID_XDEF_WILL_TRY_STR),
                  TOOL_NAME, "LightGrayColor", c_ptr, "gray75");
         }
      }
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DarkGrayColor")) != NULL) {
         if (ParseAndAllocColorByName(c_ptr, &color, NULL, NULL, NULL)) {
            myDkGryPixel = color.pixel;
         } else {
            fprintf(stderr, TgLoadString(STID_INVALID_XDEF_WILL_TRY_STR),
                  TOOL_NAME, "DarkGrayColor", c_ptr, "gray50");
         }
      }
      if (initVryLtPixel && (c_ptr=XGetDefault(mainDisplay,TOOL_NAME,
            "VeryLightGrayColor")) != NULL) {
         if (ParseAndAllocColorByName(c_ptr, &color, NULL, NULL, NULL)) {
            myVryLtPixel = color.pixel;
         } else {
            fprintf(stderr, TgLoadString(STID_INVALID_XDEF_WILL_TRY_STR),
                  TOOL_NAME, "VeryLightGrayColor", c_ptr, "gray95");
         }
      }
      if (myLtGryPixel == INVALID && ParseAndAllocColorByName("gray75", &color,             NULL, NULL, NULL)) {
         myLtGryPixel = color.pixel;
      }
      if (myDkGryPixel == INVALID && ParseAndAllocColorByName("gray50", &color,
            NULL, NULL, NULL)) {
         myDkGryPixel = color.pixel;
      }
      if (initVryLtPixel && myVryLtPixel == INVALID &&
            ParseAndAllocColorByName("gray95", &color, NULL, NULL, NULL)) {
         myVryLtPixel = color.pixel;
      }
      if (myBlackPixel == INVALID || myWhitePixel == INVALID ||
            myLtGryPixel == INVALID || myDkGryPixel == INVALID ||
            (initVryLtPixel && myVryLtPixel == INVALID) ||
            myYellowPixel == INVALID) {
         fprintf(stderr, "%s\n", TgLoadString(STID_FAIL_TO_ALLOC_3D_COLORS));
         threeDLook = FALSE;
      }
   }
   if (colorDisplay) {
      index = 0;
      for (i=0; i < maxColors; i++) {
         unsigned short red_req, green_req, blue_req;

         if (!ParseAndAllocColorByName(colorMenuItems[i], &color,
               &red_req, &green_req, &blue_req)) {
            fprintf(stderr, TgLoadString(STID_FAIL_TO_ALLOC_COLOR_NUM),
                  i, colorMenuItems[i], TOOL_NAME);
            fprintf(stderr, "\n");
            exit (-1);
         }

         if (i != index) strcpy(colorMenuItems[index], colorMenuItems[i]);

         colorPixels[index] = color.pixel;

         tgifColors[index].red = color.red;
         tgifColors[index].green = color.green;
         tgifColors[index].blue = color.blue;

         tgifRequestedColors[index].red = red_req;
         tgifRequestedColors[index].green = green_req;
         tgifRequestedColors[index].blue = blue_req;

         if (UtilStrICmp(colorMenuItems[i], fg_color) == 0) {
            myFgPixel = color.pixel;
            strcpy(myFgColorStr, fg_color);

            fg_allocated = TRUE;
         }
         if (UtilStrICmp(colorMenuItems[i], bg_color) == 0) {
            myBgPixel = color.pixel;
            myBgColor.pixel = color.pixel;
            myBgColor.red = color.red;
            myBgColor.green = color.green;
            myBgColor.blue = color.blue;
            strcpy(myBgColorStr, bg_color);

            bg_allocated = TRUE;
            bg_gray = (((long)tgifColors[i].red)<<2) +
                  (((long)tgifColors[i].green)<<3) + ((long)tgifColors[i].blue);
         }
         if (UtilStrICmp(colorMenuItems[i], brdr_color) == 0) {
            myBorderPixel = color.pixel;
            brdr_allocated = TRUE;
         }
         index++;
      }
      maxColors = index;
      if (maxColors <= 0) {
         fprintf(stderr, "%s\n", TgLoadString(STID_ZERO_COLORS_SPECIFIED));
         exit (-1);
      }
      looking_for_default_color = FALSE;
      if (defaultColorInXDefaults) {
         looking_for_default_color = TRUE;
         for (i=0; i < maxColors; i++) {
            if (UtilStrICmp(defaultColorStr, colorMenuItems[i]) == 0) {
               defaultColorIndex = i;
               looking_for_default_color = FALSE;
               break;
            }
         }
      } else if (defaultColorIndexInXDefaults) {
         if (defaultColorIndexInXDefaults && defaultColorIndex >= maxColors) {
            fprintf(stderr, TgLoadString(STID_DEFCOLORINDEX_GE_MAXCOLORS),
                  TOOL_NAME, "DefaultColorIndex", TOOL_NAME, "MaxColors",
                  TOOL_NAME, "DefaultColorIndex");
            fprintf(stderr, "\n");
            defaultColorIndex = 0;
         }
      } else {
         looking_for_default_color = TRUE;
         for (i=0; i < maxColors; i++) {
            if (UtilStrICmp(fg_color, colorMenuItems[i]) == 0) {
               defaultColorIndex = i;
               looking_for_default_color = FALSE;
               break;
            }
         }
      }
      if (!fg_allocated) {
         if (!ParseAndAllocColorByName(fg_color, &color, NULL, NULL, NULL)) {
            fprintf(stderr, TgLoadString(STID_FAIL_ALLOC_NAMED_COLOR_ABORT),
                  fg_color);
            fprintf(stderr, "\n");
            exit (-1);
         }
         myFgPixel = color.pixel;
         strcpy(myFgColorStr, fg_color);
      }
      if (!bg_allocated) {
         if (!ParseAndAllocColorByName(bg_color, &color, NULL, NULL, NULL)) {
            fprintf(stderr, TgLoadString(STID_FAIL_ALLOC_NAMED_COLOR_ABORT),
                  bg_color);
            fprintf(stderr, "\n");
            exit (-1);
         }
         myBgPixel = color.pixel;
         myBgColor.pixel = color.pixel;
         myBgColor.red = color.red;
         myBgColor.green = color.green;
         myBgColor.blue = color.blue;
         strcpy(myBgColorStr, bg_color);
         if (looking_for_default_color) {
            bg_gray = (((long)color.red)<<2) + (((long)color.green)<<3) +
                  ((long)color.blue);
         }
      }
      if (defaultColorInXDefaults && looking_for_default_color) {
         if (!ParseAndAllocColorByName(defaultColorStr, &color, NULL, NULL, NULL)) {
            fprintf(stderr, TgLoadString(STID_INVALID_XDEF_COLORXPM_GET),
                  TOOL_NAME, "DefaultColor", defaultColorStr);
            fprintf(stderr, "\n");
            defaultColorInXDefaults = FALSE;
         }
         for (i=0; i < maxColors; i++) {
            if (color.pixel == colorPixels[i]) {
               defaultColorIndex = i;
               looking_for_default_color = FALSE;
               break;
            }
         }
         if (looking_for_default_color) {
            int new_alloc=FALSE;

            index = QuickFindColorIndex(NULL, defaultColorStr, &new_alloc, FALSE);
            if (index == INVALID) {
               fprintf(stderr, TgLoadString(STID_INVALID_XDEF_COLORXPM_GET),
                     TOOL_NAME, "DefaultColor", defaultColorStr);
               fprintf(stderr, "\n");
               defaultColorInXDefaults = FALSE;
            } else {
               defaultColorIndex = index;
               looking_for_default_color = FALSE;
            }
         }
      }
      if (looking_for_default_color) {
         long val, val1;

         defaultColorIndex = 0;
         val = (((long)tgifColors[0].red)<<2) +
               (((long)tgifColors[0].green)<<3) + ((long)tgifColors[0].blue);
         if (bg_gray >= 0x67ff9) { /* (0xffff<<1) + (0xffff<<2) + (0x7fff) */
            for (i=1; i < maxColors; i++) {
               val1 = (((long)tgifColors[i].red)<<2) +
                     (((long)tgifColors[i].green)<<3) +
                     ((long)tgifColors[i].blue);
               if (val > val1) {
                  val = val1;
                  defaultColorIndex = i;
               }
            }
         } else {
            for (i=1; i < maxColors; i++) {
               val1 = (((long)tgifColors[i].red)<<2) +
                     (((long)tgifColors[i].green)<<3) +
                     ((long)tgifColors[i].blue);
               if (val < val1) {
                  val = val1;
                  defaultColorIndex = i;
               }
            }
         }
      }
      if (!brdr_allocated) {
         if (!ParseAndAllocColorByName(brdr_color, &color, NULL, NULL, NULL)) {
            fprintf(stderr, TgLoadString(STID_FAIL_ALLOC_NAMED_COLOR_ABORT),
                  brdr_color);
            fprintf(stderr, "\n");
            exit (-1);
         }
         myBorderPixel = color.pixel;
      }
      for (i=0; i < maxColors; i++) {
         xorColorPixels[i] = colorPixels[i] ^ myBgPixel;
      }
      colorIndex = defaultColorIndex;
   } else {
      /* !colorDisplay */
      if (!ParseAndAllocColorByName(fg_color, &color, NULL, NULL, NULL)) {
         fprintf(stderr, TgLoadString(STID_FAIL_ALLOC_NAMED_COLOR_ABORT),
               fg_color);
         fprintf(stderr, "\n");
         exit (-1);
      }
      myFgPixel = color.pixel;
      strcpy(myFgColorStr, fg_color);

      if (!ParseAndAllocColorByName(bg_color, &color, NULL, NULL, NULL)) {
         fprintf(stderr, TgLoadString(STID_FAIL_ALLOC_NAMED_COLOR_ABORT),
               bg_color);
         fprintf(stderr, "\n");
         exit (-1);
      }
      myBgPixel = color.pixel;
      strcpy(myBgColorStr, bg_color);

      if (!ParseAndAllocColorByName(brdr_color, &color, NULL, NULL, NULL)) {
         fprintf(stderr, TgLoadString(STID_FAIL_ALLOC_NAMED_COLOR_ABORT),
               brdr_color);
         fprintf(stderr, "\n");
         exit (-1);
      }
      myBorderPixel = color.pixel;

      for (i=0; i < maxColors; i++) {
         unsigned short red_req, green_req, blue_req;

         if (!ParseAndAllocColorByName(colorMenuItems[i], &color,
               &red_req, &green_req, &blue_req)) {
            fprintf(stderr, TgLoadString(STID_FAIL_ALLOC_COLOR_NUM_ABORT),
                  i, colorMenuItems[i]);
            fprintf(stderr, "\n");
            exit (-1);
         }

         tgifColors[i].red = color.red;
         tgifColors[i].green = color.green;
         tgifColors[i].blue = color.blue;

         tgifRequestedColors[i].red = red_req;
         tgifRequestedColors[i].green = green_req;
         tgifRequestedColors[i].blue = blue_req;

         if (UtilStrICmp(colorMenuItems[i], bg_color) == 0) {
            colorPixels[i] = myBgPixel;
         } else {
            colorPixels[i] = myFgPixel;
         }
         xorColorPixels[i] = myFgPixel ^ myBgPixel;
      }
      colorIndex = defaultColorIndex;
   }
   if (!ParseAndAllocColorByName("black", &color, NULL, NULL, NULL)) {
      fprintf(stderr, TgLoadString(STID_FAIL_ALLOC_NAMED_COLOR_ABORT),
            "black");
      fprintf(stderr, "\n");
      exit (-1);
   }
   tmp_max = max(((int)color.red),max(((int)color.green),((int)color.blue)));
#ifndef DONTFREECOLORS
   XFreeColors(mainDisplay, mainColormap, &(color.pixel), 1, 0);
#endif
   if (tmp_max > maxRGB) maxRGB = tmp_max;
   if (!ParseAndAllocColorByName("white", &color, NULL, NULL, NULL)) {
      fprintf(stderr, TgLoadString(STID_FAIL_ALLOC_NAMED_COLOR_ABORT),
            "white");
      fprintf(stderr, "\n");
      exit (-1);
   }
   tmp_max = max(((int)color.red),max(((int)color.green),((int)color.blue)));
#ifndef DONTFREECOLORS
   XFreeColors(mainDisplay, mainColormap, &(color.pixel), 1, 0);
#endif
   if (tmp_max > maxRGB) maxRGB = tmp_max;
   if (tmp_max == 0) {
      fprintf(stderr, "%s\n", TgLoadString(STID_UNEXPECTED_MAX_RGB_ZERO));
   }
   *myRubberBandColorStr = '\0';
   myRubberBandPixel = INVALID;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"RubberBandColor")) != NULL &&
            ParseAndAllocColorByName(c_ptr, &color, NULL, NULL, NULL)) {
      strcpy(myRubberBandColorStr, c_ptr);
      myRubberBandPixel = color.pixel;
      xorOne = myRubberBandPixel^myBgPixel;
   } else {
      xorOne = myFgPixel^myBgPixel;
   }

   xorZero = myBgPixel;
   if (myFgPixel == myBgPixel) {
      fprintf(stderr, "%s\n", TgLoadString(STID_FG_BG_SAME_COLOR_WARNING));
   }
   if (!initColorDontReload) {
      canChangeAttrColor = FALSE;
      if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"CanChangeAttrColor")) !=
            NULL && UtilStrICmp(c_ptr, "true") == 0) {
         canChangeAttrColor = TRUE;
      }
      colorLayers = FALSE;
      if (colorDisplay && (c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
            "ColorLayers")) != NULL && UtilStrICmp(c_ptr, "true") == 0) {
         colorLayers = TRUE;
      }
      *defaultBgColorStr = '\0';
      defaultBgColorIndex = INVALID;
      if (colorDisplay && (c_ptr=XGetDefault(mainDisplay, TOOL_NAME,
            "DefaultObjectBackground")) != NULL) {
         strcpy(defaultBgColorStr, c_ptr);
      } else {
         strcpy(defaultBgColorStr, myBgColorStr);
      }
      DoSetDefaultBgColor();

      *altEditTextBgColorStr = *altEditTextHighlightColorStr = '\0';
      altEditTextBgIndex = altEditTextHighlightIndex = INVALID;
      useAltEditTextBgColor = FALSE;
   }
   if (!colorDisplay) {
      int new_alloc=FALSE;
      int color_index=QuickFindColorIndex(NULL, myFgColorStr, &new_alloc, TRUE);

      if (color_index == INVALID) {
         fprintf(stderr, TgLoadString(STID_CANNOT_SET_FG_COLOR), myFgColorStr,
               colorMenuItems[colorIndex]);
         fprintf(stderr, "\n");
      } else {
         colorIndex = color_index;
      }
   }
   UtilStrCpyN(pngExportTransparentColor, sizeof(pngExportTransparentColor),
         myBgColorStr);
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"PNGExportTransparentColor")) != NULL) {
      int new_alloc=0;
      char spec[MAXSTRING];

      UtilStrCpyN(spec, sizeof(spec), c_ptr);
      UtilTrimBlanks(spec);

      if (QuickFindColorIndex(NULL, spec, &new_alloc, FALSE) == INVALID) {
         fprintf(stderr, TgLoadString(STID_INVALID_XDEF_COLORXPM_GET),
               TOOL_NAME, "PNGExportTransparentColor", spec);
         fprintf(stderr, "\n");
         return;
      }
      UtilStrCpyN(pngExportTransparentColor, sizeof(pngExportTransparentColor), spec);
   }
}

static
int DrawAVerticalTab(color_index, x, y, skip)
   int color_index, x, y, skip;
{
   XPoint v[5];
   int offset;

   if (skip) return colorItemH;

   offset = ((colorItemH-7)>>1);
   v[0].x = v[4].x = x;   v[0].y = v[4].y = y;
   v[1].x = x+scrollBarW; v[1].y = y;
   v[2].x = x+scrollBarW; v[2].y = y+colorItemH;
   v[3].x = x;            v[3].y = y+colorItemH;

   XSetForeground(mainDisplay, defaultGC, colorPixels[color_index]);
   XFillPolygon(mainDisplay, colorWindow, defaultGC, v, 5, Convex,
         CoordModeOrigin);
   XSetForeground(mainDisplay, defaultGC, myBgPixel);
   XFillRectangle(mainDisplay, colorWindow, defaultGC, x+offset, y+offset,
         7, 7);
   XSetForeground(mainDisplay, defaultGC, myFgPixel);
   XDrawRectangle(mainDisplay, colorWindow, defaultGC, x+offset, y+offset,
         7, 7);
   if (colorLayerOn[color_index]) {
      XDrawLine(mainDisplay, colorWindow, defaultGC, x+offset, y+offset,
            x+offset+7, y+offset+7);
      XDrawLine(mainDisplay, colorWindow, defaultGC, x+offset,
            y+offset+7, x+offset+7, y+offset);
   }
   if (threeDLook) {
      struct BBRec bbox;

      SetBBRec(&bbox, x, y, x+scrollBarW-windowPadding, y+colorItemH);
      TgDrawThreeDButton(mainDisplay, colorWindow, textMenuGC, &bbox,
            TGBS_RAISED, 1, TRUE);
   } else {
      XDrawRectangle(mainDisplay, colorWindow, defaultGC, x, y,
            scrollBarW, colorItemH);
   }
   return colorItemH;
}

void RedrawColorWindow()
{
   int i, x=(windowPadding>>1), y=0, y_start;

   needToRedrawColorWindow = FALSE;
   if (!colorLayers || colorWindow == None) return;

   XClearWindow(mainDisplay, colorWindow);

   y_start = y = (upend_height*MAX_VERTICAL_BTNS)+(windowPadding>>1);
   for (i=0; i < maxColors; i++) {
      if (i >= colorWindowFirstIndex) {
         int h=0;

         h = DrawAVerticalTab(i, x, y, FALSE);
         y += h;
         if (y > colorWindowH) break;
      }
   }
   for (i=0, y=(windowPadding>>1); i < MAX_VERTICAL_BTNS; i++,
         y += upend_height) {
      XSetTSOrigin(mainDisplay, rasterGC, x, y);
      XSetStipple(mainDisplay, rasterGC, scrollPixmap[i+4]);
      XFillRectangle(mainDisplay, colorWindow, rasterGC,
            x, y, upend_width, upend_height);
      if (threeDLook) {
         struct BBRec bbox;

         SetBBRec(&bbox, x, y, x+upend_height, y+upend_height);
         TgDrawThreeDButton(mainDisplay, colorWindow, textMenuGC, &bbox,
               TGBS_RAISED, 1, FALSE);
      }
   }
   XSetTSOrigin(mainDisplay, rasterGC, 0, 0);
   if (threeDLook) {
      XSetForeground(mainDisplay, defaultGC, myBorderPixel);
      XDrawRectangle(mainDisplay, colorWindow, defaultGC, 0, 0, scrollBarW-1,
            colorWindowH-1);
      XSetForeground(mainDisplay, defaultGC, myFgPixel);
   }
}

int OneColorObject(ObjPtr, ColorIndex)
   struct ObjRec *ObjPtr;
   int *ColorIndex;
{
   struct ObjRec *obj_ptr=NULL;
   struct TextRec *text_ptr=NULL;

   for (obj_ptr=ObjPtr->detail.r->last; obj_ptr!=NULL; obj_ptr=obj_ptr->prev) {
      if (ObjPtr->type == OBJ_PIN && obj_ptr != GetPinObj(ObjPtr)) {
         continue;
      }
      switch (obj_ptr->type) {
      case OBJ_POLY:
         if (*ColorIndex == INVALID) {
            if ((obj_ptr->detail.p->fill != NONEPAT &&
                  obj_ptr->detail.p->fill != BACKPAT) ||
                  (obj_ptr->detail.p->pen != NONEPAT &&
                  obj_ptr->detail.p->pen != BACKPAT)) {
               *ColorIndex = obj_ptr->color;
            }
         } else if (obj_ptr->color != *ColorIndex) {
            if ((obj_ptr->detail.p->fill != NONEPAT &&
                  obj_ptr->detail.p->fill != BACKPAT) ||
                  (obj_ptr->detail.p->pen != NONEPAT &&
                  obj_ptr->detail.p->pen != BACKPAT)) {
               return FALSE;
            }
         }
         break;
      case OBJ_BOX:
         if (*ColorIndex == INVALID) {
            if ((obj_ptr->detail.b->fill != NONEPAT &&
                  obj_ptr->detail.b->fill != BACKPAT) ||
                  (obj_ptr->detail.b->pen != NONEPAT &&
                  obj_ptr->detail.b->pen != BACKPAT)) {
               *ColorIndex = obj_ptr->color;
            }
         } else if (obj_ptr->color != *ColorIndex) {
            if ((obj_ptr->detail.b->fill != NONEPAT &&
                  obj_ptr->detail.b->fill != BACKPAT) ||
                  (obj_ptr->detail.b->pen != NONEPAT &&
                  obj_ptr->detail.b->pen != BACKPAT)) {
               return FALSE;
            }
         }
         break;
      case OBJ_OVAL:
         if (*ColorIndex == INVALID) {
            if ((obj_ptr->detail.o->fill != NONEPAT &&
                  obj_ptr->detail.o->fill != BACKPAT) ||
                  (obj_ptr->detail.o->pen != NONEPAT &&
                  obj_ptr->detail.o->pen != BACKPAT)) {
               *ColorIndex = obj_ptr->color;
            }
         } else if (obj_ptr->color != *ColorIndex) {
            if ((obj_ptr->detail.o->fill != NONEPAT &&
                  obj_ptr->detail.o->fill != BACKPAT) ||
                  (obj_ptr->detail.o->pen != NONEPAT &&
                  obj_ptr->detail.o->pen != BACKPAT)) {
               return FALSE;
            }
         }
         break;
      case OBJ_TEXT:
         text_ptr = obj_ptr->detail.t;

         if (*ColorIndex == INVALID) {
            if (text_ptr->fill != NONEPAT && text_ptr->fill != BACKPAT) {
               *ColorIndex = obj_ptr->color;
            }
         }
         if (*ColorIndex == INVALID) {
            if (text_ptr->pen != NONEPAT && text_ptr->pen != BACKPAT) {
               MiniLinesInfo *minilines=(&text_ptr->minilines);

               *ColorIndex = minilines->first->first_block->seg->color;
               if (!OnePropertyMiniLines(PROP_MASK_COLOR,
                     minilines->first->first_block->seg->color, minilines,
                     TRUE)) {
                  return FALSE;
               }
            }
         } else {
            if (text_ptr->fill != NONEPAT && text_ptr->fill != BACKPAT) {
               if (*ColorIndex != obj_ptr->color) {
                  return FALSE;
               }
            }
            if (text_ptr->pen != NONEPAT && text_ptr->pen != BACKPAT) {
               if (!OnePropertyMiniLines(PROP_MASK_COLOR, *ColorIndex,
                     &text_ptr->minilines, TRUE)) {
                  return FALSE;
               }
            }
         }
         break;
      case OBJ_POLYGON:
         if (*ColorIndex == INVALID) {
            if ((obj_ptr->detail.g->fill != NONEPAT &&
                  obj_ptr->detail.g->fill != BACKPAT) ||
                  (obj_ptr->detail.g->pen != NONEPAT &&
                  obj_ptr->detail.g->pen != BACKPAT)) {
               *ColorIndex = obj_ptr->color;
            }
         } else if (obj_ptr->color != *ColorIndex) {
            if ((obj_ptr->detail.g->fill != NONEPAT &&
                  obj_ptr->detail.g->fill != BACKPAT) ||
                  (obj_ptr->detail.g->pen != NONEPAT &&
                  obj_ptr->detail.g->pen != BACKPAT)) {
               return FALSE;
            }
         }
         break;
      case OBJ_ARC:
         if (*ColorIndex == INVALID) {
            if ((obj_ptr->detail.a->fill != NONEPAT &&
                  obj_ptr->detail.a->fill != BACKPAT) ||
                  (obj_ptr->detail.a->pen != NONEPAT &&
                  obj_ptr->detail.a->pen != BACKPAT)) {
               *ColorIndex = obj_ptr->color;
            }
         } else if (obj_ptr->color != *ColorIndex) {
            if ((obj_ptr->detail.a->fill != NONEPAT &&
                  obj_ptr->detail.a->fill != BACKPAT) ||
                  (obj_ptr->detail.a->pen != NONEPAT &&
                  obj_ptr->detail.a->pen != BACKPAT)) {
               return FALSE;
            }
         }
         break;
      case OBJ_RCBOX:
         if (*ColorIndex == INVALID) {
            if ((obj_ptr->detail.rcb->fill != NONEPAT &&
                  obj_ptr->detail.rcb->fill != BACKPAT) ||
                  (obj_ptr->detail.rcb->pen != NONEPAT &&
                  obj_ptr->detail.rcb->pen != BACKPAT)) {
               *ColorIndex = obj_ptr->color;
            }
         } else if (obj_ptr->color != *ColorIndex) {
            if ((obj_ptr->detail.rcb->fill != NONEPAT &&
                  obj_ptr->detail.rcb->fill != BACKPAT) ||
                  (obj_ptr->detail.rcb->pen != NONEPAT &&
                  obj_ptr->detail.rcb->pen != BACKPAT)) {
               return FALSE;
            }
         }
         break;
      case OBJ_XBM:
         if (*ColorIndex == INVALID) {
            if (obj_ptr->detail.xbm->fill != NONEPAT &&
                  obj_ptr->detail.xbm->fill != BACKPAT) {
               *ColorIndex = obj_ptr->color;
            }
         } else if (obj_ptr->color != *ColorIndex) {
            if (obj_ptr->detail.xbm->fill != NONEPAT &&
                  obj_ptr->detail.xbm->fill != BACKPAT) {
               return FALSE;
            }
         }
         break;
      case OBJ_XPM:
         if (*ColorIndex == INVALID) {
            *ColorIndex = obj_ptr->color;
         } else if (obj_ptr->color != *ColorIndex) {
            return FALSE;
         }
         break;
      case OBJ_GROUP:
      case OBJ_SYM:
      case OBJ_ICON:
      case OBJ_PIN:
         if (!OneColorObject(obj_ptr, ColorIndex)) {
            return FALSE;
         }
         break;
      }
   }
   return TRUE;
}

int ChangeObjColor(ObjPtr, ColorIndex)
   struct ObjRec *ObjPtr;
   int ColorIndex;
{
   register struct ObjRec *obj_ptr;
   register struct AttrRec *attr_ptr;
   int changed=FALSE, icon_color_index;

   switch (ObjPtr->type) {
   case OBJ_POLY:
   case OBJ_BOX:
   case OBJ_OVAL:
   case OBJ_TEXT:
   case OBJ_POLYGON:
   case OBJ_ARC:
   case OBJ_RCBOX:
   case OBJ_XBM:
   case OBJ_XPM:
      if (ObjPtr->type == OBJ_TEXT) {
         if (SetMiniLinesProperty(PROP_MASK_COLOR, ColorIndex,
               &ObjPtr->detail.t->minilines)) {
            InvalidateTextCache(ObjPtr->detail.t);
            changed = TRUE;
         }
      } else if (ObjPtr->color != ColorIndex) {
         ObjPtr->color = ColorIndex;
         if (mainDisplay != NULL) {
            UtilStrCpyN(ObjPtr->color_str, sizeof(ObjPtr->color_str),
                  colorMenuItems[ObjPtr->color]);
         }
         if (ObjPtr->type == OBJ_XPM) {
            /* RecolorXPmObj(ObjPtr, ColorIndex); */
         }
         changed = TRUE;
      }
      break;

   case OBJ_GROUP:
   case OBJ_SYM:
      for (obj_ptr = ObjPtr->detail.r->last; obj_ptr != NULL;
            obj_ptr = obj_ptr->prev) {
         if (ChangeObjColor(obj_ptr, ColorIndex)) {
            changed = TRUE;
         }
      }
      break;

   case OBJ_ICON:
      icon_color_index = INVALID;
      if (OneColorObject(ObjPtr, &icon_color_index) &&
            icon_color_index != ColorIndex) {
         for (obj_ptr = ObjPtr->detail.r->last; obj_ptr != NULL;
               obj_ptr = obj_ptr->prev) {
            if (ChangeObjColor(obj_ptr, ColorIndex)) {
               changed = TRUE;
            }
         }
      }
      break;

   case OBJ_PIN:
      icon_color_index = INVALID;
      if (OneColorObject(GetPinObj(ObjPtr), &icon_color_index) &&
            icon_color_index != ColorIndex) {
         if (ChangeObjColor(GetPinObj(ObjPtr), ColorIndex)) {
            changed = TRUE;
         }
      }
      break;
   }
   if ((canChangeAttrColor || changePropertiesOfAttrs) &&
         ObjPtr->type != OBJ_TEXT) {
      for (attr_ptr=ObjPtr->fattr; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
         if (SetMiniLinesProperty(PROP_MASK_COLOR, ColorIndex,
               &attr_ptr->obj->detail.t->minilines)) {
            InvalidateTextCache(attr_ptr->obj->detail.t);
            changed = TRUE;
         }
      }
   }
   return changed;
}

void ChangeAllSelColor(ColorIndex, HighLight)
   int ColorIndex, HighLight;
{
   register struct SelRec *sel_ptr;
   int changed=FALSE, dont_do_obj=FALSE;
   int saved_sticky_menu_selection=stickyMenuSelection;
   int dont_check_text=FALSE;
   XGCValues values;

   if (topSel != NULL && topSel == botSel && topSel->obj->type == OBJ_XPM) {
      /* if the only selected object is an OBJ_XPM, chang the current color */
      stickyMenuSelection = TRUE;
      if (gnInImageProc) dont_check_text = TRUE;
      dont_do_obj = TRUE;
   }
   if (topSel == NULL || stickyMenuSelection) {
      int text_obj_created=FALSE, text_cursor_shown=FALSE;

      if (!dont_check_text) {
         if (!(curChoice == DRAWTEXT && textCursorShown)) {
            text_cursor_shown = textCursorShown;
            text_obj_created = TieLooseEnds();
         }
      }
      colorIndex = ColorIndex;
      if (colorLayers && !colorLayerOn[colorIndex]) {
         if (topSel == NULL) {
            sprintf(gszMsgBox, TgLoadString(STID_INVISIBLE_COLOR_SELECTED),
                  colorIndex, colorMenuItems[colorIndex]);
            Msg(gszMsgBox);
         }
      }
      ShowColor(TRUE);
      UpdatePinnedMenu(MENU_COLOR);
      if (!PRTGIF && colorLayers && needToRedrawColorWindow) {
         RedrawColorWindow();
      }
      if (!dont_check_text) {
         if (curChoice == DRAWTEXT && textCursorShown) {
            text_cursor_shown = TRUE;
            if (ChangeEditTextProperty(PROP_MASK_COLOR, colorIndex)) {
               curTextModified = TRUE;
               RedrawCurText();
               SetFileModified(TRUE);
               if (cycleThroughChoice) {
                  SetPushedFontValue(PUSH_COLORINDEX, colorIndex);
               }
            }
         } else {
            textCursorShown = FALSE;
         }
      }
      if (topSel == NULL) dont_do_obj = TRUE;
   }
   stickyMenuSelection = saved_sticky_menu_selection;
   sprintf(gszMsgBox, TgLoadString(STID_COLOR_SET_TO),
         colorMenuItems[ColorIndex]);
   Msg(gszMsgBox);
   if (dont_do_obj) return;

   values.foreground = colorPixels[ColorIndex];
   values.function = GXcopy;
   values.fill_style = FillSolid;
   XChangeGC(mainDisplay, drawGC,
         GCForeground | GCFunction | GCFillStyle, &values);

   if (HighLight) HighLightReverse();
   StartCompositeCmd();
   for (sel_ptr=botSel; sel_ptr != NULL; sel_ptr=sel_ptr->prev) {
      PrepareToReplaceAnObj(sel_ptr->obj);
      if (ChangeObjColor(sel_ptr->obj, ColorIndex)) {
         changed = TRUE;
         RecordReplaceAnObj(sel_ptr->obj);
      } else {
         AbortPrepareCmd(CMD_REPLACE);
      }
   }
   EndCompositeCmd();

   if (changed) {
      int need_to_update_sel_bbox=FALSE;

      if (!PRTGIF && colorLayers) {
         struct SelRec *next_sel;

         for (sel_ptr=topSel; sel_ptr != NULL; sel_ptr=next_sel) {
            struct ObjRec *obj_ptr=sel_ptr->obj;

            next_sel = sel_ptr->next;
            obj_ptr->tmp_parent = NULL;
            if (!ObjInVisibleLayer(obj_ptr)) {
               need_to_update_sel_bbox = TRUE;
               if (sel_ptr->prev == NULL) {
                  topSel = sel_ptr->next;
               } else {
                  sel_ptr->prev->next = sel_ptr->next;
               }
               if (sel_ptr->next == NULL) {
                  botSel = sel_ptr->prev;
               } else {
                  sel_ptr->next->prev = sel_ptr->prev;
               }
               free(sel_ptr);
            }
         }
         if (needToRedrawColorWindow) {
            RedrawColorWindow();
         }
      }
      SetFileModified(TRUE);
      RedrawAnArea(botObj, selLtX-GRID_ABS_SIZE(1),
            selLtY-GRID_ABS_SIZE(1), selRbX+GRID_ABS_SIZE(1),
            selRbY+GRID_ABS_SIZE(1));
      if (need_to_update_sel_bbox) UpdSelBBox();
   }
   if (HighLight) HighLightForward();
}

void SetUpColorMenuPixmap(fore_colors, init_rv, pixmap, rows, cols)
   int **fore_colors, **init_rv, *rows, *cols;
   Pixmap **pixmap;
{
   register int i;

   *pixmap = (Pixmap*)malloc(maxColors*sizeof(Pixmap));
   *fore_colors = (int*)malloc(maxColors*sizeof(int));
   *init_rv = (int*)malloc(maxColors*sizeof(int));
   if (*pixmap == NULL || *fore_colors == NULL || *init_rv == NULL) {
      FailAllocMessage();
   }
   for (i=0; i < maxColors; i++) {
      (*pixmap)[i] = patPixmap[SOLIDPAT];
      (*fore_colors)[i] = colorPixels[i];
      (*init_rv)[i] = FALSE;
   }
   *cols = ((maxColors % 10)==0) ? (int)(maxColors/10) : (int)(maxColors/10)+1;
   *rows = (maxColors <= 10) ? maxColors : 10;
}

void RefreshColorMenu(menu)
   TgMenu *menu;
{
   int i, num_items=menu->num_items;
   TgMenuItem *menuitems=menu->menuitems;

   for (i=0; i < num_items; i++) {
      TgMenuItem *menu_item=(&menuitems[i]);
      TgMenuItem stMenuItem;

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.state = TGBS_NORMAL;
      stMenuItem.checked = (i == colorIndex);
      TgSetMenuItemInfo(menu_item, TGMU_MASK_STATE|TGMU_MASK_CHECK,
            &stMenuItem);
   }
}

TgMenu *CreateColorMenu(parent_menu, x, y, menu_info, status_str_xlated)
   TgMenu *parent_menu;
   int x, y;
   TgMenuInfo *menu_info;
   int status_str_xlated; /* ignored, always 0 */
{
   int i=0;
   TgMenu *menu=NULL;
   TgMenuInfo stMenuInfo;
   TgMenuItemInfo *item_info=NULL;

   memcpy(&stMenuInfo, menu_info, sizeof(TgMenuInfo));
   stMenuInfo.items = (TgMenuItemInfo*)malloc(
         (maxColors+1)*sizeof(TgMenuItemInfo));
   if (stMenuInfo.items == NULL) FailAllocMessage();
   memset(stMenuInfo.items, 0, (maxColors+1)*sizeof(TgMenuItemInfo));
   for (item_info=stMenuInfo.items, i=0; i < maxColors; item_info++, i++) {
      item_info->menu_str = (char*)(int*)(&colorPixels[i]);
      item_info->shortcut_str = NULL;
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_SET_COLOR_TO),
            colorMenuItems[i]);
      item_info->status_str = UtilStrDup(gszMsgBox);
      if (item_info->status_str == NULL) FailAllocMessage();
      item_info->submenu_info = NULL;
      item_info->cmdid = CMDID_CHANGEALLSELCOLOR;
   }
   stMenuInfo.items[maxColors].cmdid = INVALID;
   stMenuInfo.type |= TGMUTYPE_CANSCROLL;

   /* the status_str has been translated in TgLoadCachedString() */
   menu = TgCreateMenuFromMenuInfo(parent_menu, x, y, &stMenuInfo, TRUE);
   for (item_info=stMenuInfo.items, i=0; i < maxColors; item_info++, i++) {
      UtilFree(item_info->status_str);
   }
   memset(stMenuInfo.items, 0, (maxColors+1)*sizeof(TgMenuItemInfo));
   free(stMenuInfo.items);
   stMenuInfo.items = NULL;
   if (menu != NULL) {
      TgMenuItem *menu_item=NULL;
      TgMenuItem stMenuItem;

      menu->track_menubar = TRUE;
      TgAdjustMenuGeometry(menu, 16, 16, 10);
      menu_item = (&menu->menuitems[colorIndex]);

      memset(&stMenuItem, 0, sizeof(TgMenuItem));
      stMenuItem.checked = TRUE;
      if (!TgSetMenuItemInfo(menu_item, TGMU_MASK_CHECK, &stMenuItem)) {
         return TgDestroyMenu(menu, TRUE);
      }
      SetScrollableMenuFirstIndex(menu, colorIndex);
   }
   return menu;
}

int ColorMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(colorMenuInfo.create_proc)(NULL, X, Y, &colorMenuInfo,
         INVALID);

   activeMenu = MENU_COLOR;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}

void CleanUpColors()
{
   register int i;

   if (privateColorInfo != NULL) {
      struct LocalColorRec *ptr;

      for (ptr=privateColorInfo; ptr->name != NULL; ptr++) {
         free(ptr->name);
         if (ptr->spec != NULL) free(ptr->spec);
      }
      free(privateColorInfo);
      privateColorInfo = NULL;
   }
   if (colorLayerOn != NULL) free(colorLayerOn);
   if (colorPixels != NULL) free(colorPixels);
   if (xorColorPixels != NULL) free(xorColorPixels);
   colorLayerOn = colorPixels = xorColorPixels = NULL;

   if (tgifColors != NULL) free(tgifColors);
   if (tgifRequestedColors != NULL) free(tgifRequestedColors);
   tgifColors = tgifRequestedColors = NULL;

   if (colorMenuItems != NULL) {
      for (i=0; i < maxColors; i++) {
         if (colorMenuItems[i] != NULL) free(colorMenuItems[i]);
      }
      free(colorMenuItems);
   }
   colorMenuItems = NULL;

   maxColors = 0;
   defaultColorIndex = 9;
   colorIndex = 0;

   if (upend_bits == NULL) { }
}

static
void SetAllColorLayersState(on_state)
   int on_state;
{
   int i, changed=FALSE;

   for (i=0; i < maxColors; i++) {
      if (colorLayerOn[i] != on_state) {
         changed = TRUE;
         colorLayerOn[i] = on_state;
      }
   }
   strcpy(gszMsgBox, TgLoadString(on_state ? STID_ALL_COLOR_LAYERS_VISIBLE :
         STID_ALL_COLOR_LAYERS_INVISIBLE));
   Msg(gszMsgBox);
   if (changed) {
      MakeQuiescent();
      RedrawColorWindow();
      ClearAndRedrawDrawWindow();
      SetCurChoice(curChoiceBeforeMakeQuiescent);
   }
}

static
void HandleClickInColorWindow(button_ev)
   XButtonEvent *button_ev;
{
   int i, index, total, offset;

   if (threeDLook) {
      index = (int)((button_ev->y-1-(windowPadding>>1)) / upend_height);
   } else {
      index = (int)((button_ev->y-1) / scrollBarW);
   }
   if (index < 0) index = 0;

   switch (index+4) {
   case SCROLL_UPEND:
   case SCROLL_UP:
   case SCROLL_CHECKALL:
   case SCROLL_UNCHECKALL:
   case SCROLL_DOWN:
   case SCROLL_DOWNEND:
      if (button_ev->button != Button1) return;
      break;
   default: break;
   }

   switch (index+4) {
   case SCROLL_UPEND:
      colorWindowFirstIndex = 0;
      RedrawColorWindow();
      break;
   case SCROLL_UP:
      if (colorWindowFirstIndex > 0) {
         if (button_ev->state & (ShiftMask | ControlMask)) {
            colorWindowFirstIndex -= 10;
            if (colorWindowFirstIndex < 0) colorWindowFirstIndex = 0;
         } else {
            colorWindowFirstIndex--;
         }
         RedrawColorWindow();
      }
      break;
   case SCROLL_CHECKALL: SetAllColorLayersState(TRUE); break;
   case SCROLL_UNCHECKALL: SetAllColorLayersState(FALSE); break;
   case SCROLL_DOWN:
      if (colorWindowFirstIndex < maxColors-1) {
         if (button_ev->state & (ShiftMask | ControlMask)) {
            colorWindowFirstIndex += 10;
            if (colorWindowFirstIndex > maxColors-1) {
               colorWindowFirstIndex = maxColors-1;
            }
         } else {
            colorWindowFirstIndex++;
         }
         RedrawColorWindow();
      }
      break;
   case SCROLL_DOWNEND:
      total = colorWindowH-(upend_height*MAX_VERTICAL_BTNS);
      colorWindowFirstIndex = maxColors-1;
      for (i=maxColors-1; i >= 0; i--) {
         int w=DrawAVerticalTab(i, 0, 0, TRUE);

         total -= w;
         if (total > 0) {
            colorWindowFirstIndex = i;
         } else {
            break;
         }
      }
      RedrawColorWindow();
      break;
   default:
      offset = button_ev->y-(upend_height*MAX_VERTICAL_BTNS);
      for (i=0; i < maxColors; i++) {
         if (i >= colorWindowFirstIndex) {
            int h=DrawAVerticalTab(i, 0, 0, TRUE);

            if (h >= offset) {
               if (button_ev->button == Button1) {
                  colorLayerOn[i] = !colorLayerOn[i];
                  sprintf(gszMsgBox, TgLoadString(colorLayerOn[i] ? 
                        STID_A_COLOR_LAYER_TURNED_ON :
                        STID_A_COLOR_LAYER_TURNED_OFF),
                        colorMenuItems[i], i);
                  SetStringStatus(gszMsgBox);
                  if (i == colorIndex) {
                     sprintf(gszMsgBox,
                           TgLoadString(STID_INVISIBLE_COLOR_SELECTED),
                           colorIndex, colorMenuItems[colorIndex]);
                     Msg(gszMsgBox);
                  }
                  MakeQuiescent();
                  RedrawColorWindow();
                  ClearAndRedrawDrawWindow();
                  SetCurChoice(curChoiceBeforeMakeQuiescent);
               } else if (button_ev->button == Button3) {
                  int save_sticky_menu_selection=stickyMenuSelection;

                  stickyMenuSelection = TRUE;
                  ChangeAllSelColor(i, TRUE);
                  stickyMenuSelection = save_sticky_menu_selection;
               }
               break;
            }
            offset -= h;
         }
      }
      break;
   }
}

static
void HandleMotionInColorWindow(motion_ev)
   XMotionEvent *motion_ev;
{
   int i, index, offset;

   if (threeDLook) {
      index = (int)((motion_ev->y-1-(windowPadding>>1)) / upend_height);
   } else {
      index = (int)((motion_ev->y-1) / scrollBarW);
   }
   if (index < 0) index = 0;

   switch (index+4) {
   case SCROLL_UPEND:
   case SCROLL_UP:
   case SCROLL_CHECKALL:
   case SCROLL_UNCHECKALL:
   case SCROLL_DOWN:
   case SCROLL_DOWNEND:
      if (motion_ev->state & (ShiftMask | ControlMask)) {
         SetColorMouseStatusStrings(TRUE, index);
      } else {
         SetColorMouseStatusStrings(FALSE, index);
      }
      break;
   default:
      offset = motion_ev->y-(upend_height*MAX_VERTICAL_BTNS);
      for (i=0; i < maxColors; i++) {
         if (i >= colorWindowFirstIndex) {
            int h=DrawAVerticalTab(i, 0, 0, TRUE);

            if (h >= offset) {
               char left_str[80], right_str[80];

               sprintf(left_str, TgLoadString(colorLayerOn[i] ?
                     STID_TURN_COLOR_LAYER_NUM_OFF :
                     STID_TURN_COLOR_LAYER_NUM_ON),
                     colorMenuItems[i], i);
               sprintf(right_str, TgLoadCachedString(CSTID_SET_COLOR_TO),
                     colorMenuItems[i]);
               SetMouseStatus(left_str, TgLoadCachedString(CSTID_PARANED_NONE),
                     right_str);
               break;
            }
            offset -= h;
         }
      }
      break;
   }
}

void ColorEventHandler(input)
   XEvent *input;
{
   XEvent ev;

   if (!colorLayers) return;

   if (input->type == Expose) {
      while (XCheckWindowEvent(mainDisplay, colorWindow, ExposureMask, &ev)) ;
      RedrawColorWindow();
   } else if (input->type == EnterNotify) {
      SetMouseStatus("", "", "");
   } else if (input->type == MotionNotify) {
      while (XCheckWindowEvent(mainDisplay,colorWindow,PointerMotionMask,&ev)) ;
      HandleMotionInColorWindow(&input->xmotion);
   } else if (input->type == ButtonPress) {
      XButtonEvent *button_ev=(&(input->xbutton));

      if (button_ev->button == Button1 || button_ev->button == Button3) {
         HandleClickInColorWindow(button_ev);
      }
   }
}

void RedrawColorDummyWindow()
{
   XClearWindow(mainDisplay, colorDummyWindow);
   if (threeDLook) {
      struct BBRec bbox;

      SetBBRec(&bbox, 0, 0, scrollBarW, colorDummyWindowH);
      TgDrawThreeDButton(mainDisplay, colorDummyWindow, textMenuGC, &bbox,
            TGBS_RAISED, 2, FALSE);
   }
}

void ColorDummyEventHandler(input)
   XEvent *input;
{
   if (!colorLayers) return;

   if (input->type == Expose) {
      XEvent ev;

      while (XCheckWindowEvent(mainDisplay,colorDummyWindow,ExposureMask,&ev)) ;
      RedrawColorDummyWindow();
   } else if (input->type == EnterNotify) {
      SetMouseStatusToAllNone();
   }
}

typedef struct tagUpdatePixelRec {
   int picture_changed;
} UpdatePixelInfo;

static
void UpdatePixelForStrSeg(pStrSeg, pUserData)
   StrSegInfo *pStrSeg;
   void *pUserData;
{
   UpdatePixelInfo *pupi=((UpdatePixelInfo*)pUserData);
   int new_alloc=FALSE;
   int index=QuickFindColorIndex(NULL, pStrSeg->color_str, &new_alloc, TRUE);

   if (index != pStrSeg->color) pupi->picture_changed = TRUE;
   if (index == INVALID) {
      sprintf(gszMsgBox,
            TgLoadCachedString(CSTID_CANNOT_ALLOC_COLOR_USE_ALT),
            pStrSeg->color_str, colorMenuItems[colorIndex]);
      Msg(gszMsgBox);
      allocColorFailed = TRUE;
      pStrSeg->color = colorIndex;
   } else {
      pStrSeg->color = index;
   }
   if (mainDisplay != NULL) {
      UtilStrCpyN(pStrSeg->color_str, sizeof(pStrSeg->color_str),
            colorMenuItems[pStrSeg->color]);
   }
}

int UpdatePixel(ObjPtr)
   struct ObjRec *ObjPtr;
{
   int c, i, r;
   int ncolors=0, new_alloc=FALSE, index=0, changed=FALSE, len=0;
   int picture_changed=FALSE;
   unsigned long pixel=0L, *from_pixels=NULL, *to_pixels=NULL;
   struct ObjRec *obj_ptr=NULL;
   struct XPmRec *xpm_ptr=NULL;
   struct AttrRec *attr_ptr=NULL;

   switch (ObjPtr->type) {
   case OBJ_GROUP:
   case OBJ_SYM:
   case OBJ_ICON:
   case OBJ_PIN:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr!=NULL;
            obj_ptr=obj_ptr->prev) {
         if (UpdatePixel(obj_ptr)) {
            picture_changed = TRUE;
         }
      }
      break;
   case OBJ_XPM:
      index = QuickFindColorIndex(NULL, ObjPtr->color_str,
            &new_alloc, TRUE);
      if (index != ObjPtr->color) picture_changed = TRUE;
      if (index == INVALID) {
         sprintf(gszMsgBox,
               TgLoadCachedString(CSTID_CANNOT_ALLOC_COLOR_USE_ALT),
               ObjPtr->color_str, colorMenuItems[colorIndex]);
         Msg(gszMsgBox);
         allocColorFailed = TRUE;
         ObjPtr->color = colorIndex;
      } else {
         ObjPtr->color = index;
      }
      if (mainDisplay != NULL) {
         UtilStrCpyN(ObjPtr->color_str, sizeof(ObjPtr->color_str),
               colorMenuItems[ObjPtr->color]);
      }
      changed = FALSE;
      xpm_ptr = ObjPtr->detail.xpm;
      ncolors = xpm_ptr->ncolors;
      from_pixels = (unsigned long *)malloc(ncolors*sizeof(unsigned long));
      to_pixels = (unsigned long *)malloc(ncolors*sizeof(unsigned long));
      if (from_pixels == NULL || to_pixels == NULL) FailAllocMessage();
      for (i=0; i < ncolors; i++) {
         from_pixels[i] = xpm_ptr->pixels[i];
         if (xpm_ptr->pixels[i] == (-1)) {
            /* transparent pixel */
         } else {
            index = QuickFindColorIndex(NULL, xpm_ptr->color_str[i],
                  &new_alloc, TRUE);
            if (index == INVALID) {
               sprintf(gszMsgBox,
                     TgLoadCachedString(CSTID_CANNOT_ALLOC_COLOR_USE_ALT),
                     xpm_ptr->color_str[i], colorMenuItems[colorIndex]);
               Msg(gszMsgBox);
               allocColorFailed = TRUE;

               len = strlen(colorMenuItems[colorIndex]);
               free(xpm_ptr->color_str[i]);
               xpm_ptr->color_str[i] = (char*)malloc((len+1)*sizeof(char));
               if (xpm_ptr->color_str[i] == NULL) FailAllocMessage();
               strcpy(xpm_ptr->color_str[i], colorMenuItems[colorIndex]);

               xpm_ptr->pixels[i] = colorPixels[colorIndex];
            } else {
               if (xpm_ptr->pixels[i] != colorPixels[index]) {
                  changed = TRUE;
               }
               xpm_ptr->pixels[i] = colorPixels[index];
            }
         }
         to_pixels[i] = xpm_ptr->pixels[i];
      }
      if (changed) {
         int image_w=xpm_ptr->image_w, image_h=xpm_ptr->image_h;
         XImage *image=xpm_ptr->image;
         Pixmap pixmap=xpm_ptr->pixmap;

         if (xpm_ptr->cached_pixmap != None) {
            XFreePixmap(mainDisplay, xpm_ptr->cached_pixmap);
         }
         xpm_ptr->cached_pixmap = None;
         if (xpm_ptr->cached_bitmap != None) {
            XFreePixmap(mainDisplay, xpm_ptr->cached_bitmap);
         }
         xpm_ptr->cached_bitmap = None;
         xpm_ptr->cached_color = (-1);
         xpm_ptr->cached_zoom = 0;

         sprintf(gszMsgBox, TgLoadCachedString(CSTID_UPDATING_PIXELS),
               ++gnUpdatePixelObjCount, image_w, image_h);
         SetStringStatus(gszMsgBox);
         XSync(mainDisplay, False);
         for (r=0; r<image_h; r++) {
            for (c=0; c<image_w; c++) {
               pixel = XGetPixel(image, c, r);
               for (i=0; i < ncolors; i++) {
                  if (from_pixels[i] == pixel) {
                     XPutPixel(image, c, r, to_pixels[i]);
                     break;
                  }
               }
            }
         }
         XPutImage(mainDisplay,pixmap,xpmGC,image,0,0,0,0,image_w,image_h);
         picture_changed = TRUE;
      }
      free(from_pixels);
      free(to_pixels);
      break;
   default:
      index = QuickFindColorIndex(NULL, ObjPtr->color_str,
            &new_alloc, TRUE);
      if (index != ObjPtr->color) picture_changed = TRUE;
      if (index == INVALID) {
         sprintf(gszMsgBox,
               TgLoadCachedString(CSTID_CANNOT_ALLOC_COLOR_USE_ALT),
               ObjPtr->color_str, colorMenuItems[colorIndex]);
         Msg(gszMsgBox);
         allocColorFailed = TRUE;
         ObjPtr->color = colorIndex;
      } else {
         ObjPtr->color = index;
      }
      if (mainDisplay != NULL) {
         UtilStrCpyN(ObjPtr->color_str, sizeof(ObjPtr->color_str),
               colorMenuItems[ObjPtr->color]);
      }
      if (ObjPtr->type == OBJ_TEXT) {
         UpdatePixelInfo upi;

         memset(&upi, 0, sizeof(UpdatePixelInfo));
         upi.picture_changed = FALSE;
         DoFuncOnStrSegForMiniLines(&ObjPtr->detail.t->minilines,
               UpdatePixelForStrSeg, &upi);
         if (upi.picture_changed) {
            picture_changed = TRUE;
         }
      }
      break;
   }
   for (attr_ptr=ObjPtr->fattr; attr_ptr != NULL; attr_ptr=attr_ptr->next) {
      if (UpdatePixel(attr_ptr->obj)) {
         picture_changed = TRUE;
      }
   }
   return picture_changed;
}

void UpdateXPmObjects(ObjPtr)
   struct ObjRec *ObjPtr;
{
   struct ObjRec *obj_ptr;
   struct XPmRec *xpm_ptr;

   switch (ObjPtr->type) {
   case OBJ_GROUP:
   case OBJ_SYM:
   case OBJ_ICON:
   case OBJ_PIN:
      for (obj_ptr=ObjPtr->detail.r->last; obj_ptr!=NULL;
            obj_ptr=obj_ptr->prev) {
         UpdateXPmObjects(obj_ptr);
      }
      break;
   case OBJ_XPM:
      xpm_ptr = ObjPtr->detail.xpm;
      if (xpm_ptr->image == NULL) {
         xpm_ptr->image = XGetImage(mainDisplay, xpm_ptr->pixmap, 0, 0,
               xpm_ptr->image_w, xpm_ptr->image_h, AllPlanes, ZPixmap);
      }
      break;
   }
}

int FlushColormap()
{
   int changed=FALSE;
   struct ObjRec *obj_ptr=NULL;

#ifdef DONT_FREE_COLORMAP
   Msg(TgLoadString(STID_COLORMAP_NOT_RESETED));
   return FALSE;
#endif
   if (!newColormapUsed) {
      Colormap colormap=XCopyColormapAndFree(mainDisplay, mainColormap);

      mainColormap = colormap;
      newColormapUsed = TRUE;
      XSetWindowColormap(mainDisplay, mainWindow, mainColormap);
   }
   if (newColormapUsed) {
      struct PageRec *page_ptr=NULL;
      int saved_color_layers=FALSE;
      struct AttrRec *attr_ptr=NULL;

      for (page_ptr=firstPage; page_ptr != NULL; page_ptr=page_ptr->next) {
         for (obj_ptr=page_ptr->bot; obj_ptr!=NULL; obj_ptr=obj_ptr->prev) {
            UpdateXPmObjects(obj_ptr);
         }
      }
      initColorDontReload = TRUE;
      CleanUpColors();
      XFreeColormap(mainDisplay, mainColormap);
      mainColormap = DefaultColormap(mainDisplay, mainScreen);
      XSetWindowColormap(mainDisplay, mainWindow, mainColormap);
      newColormapUsed = FALSE;

      saved_color_layers = colorLayers;
      InitColor();
      initColorDontReload = FALSE;
      colorLayers = saved_color_layers;

      DoSetDefaultBgColor();

      ShowColor(TRUE);
      if (myFileBgColorStr != NULL) {
         int saved_my_file_bg_pixel=myFileBgPixel;
         int saved_my_file_fg_pixel=myFileFgPixel;
         int ok=TRUE;

         if (myFileFgColorStr != NULL && !SetFileFgColor()) {
            if (SetFileFgColor()) {
               if (saved_my_file_fg_pixel != myFileFgPixel) {
                  changed = TRUE;
               }
            } else {
               ok = FALSE;
            }
         }
         if (ok) {
            if (SetFileBgColor()) {
               if (saved_my_file_bg_pixel != myFileBgPixel) {
                  changed = TRUE;
               }
            } else {
               ok = FALSE;
            }
         }
         if (!ok) {
            ClearBgColorInfo(FALSE);
            changed = TRUE;
         }
      }
      DoSetAltEditTextBgColor();
      DoSetAltEditTextHighlightColor();

      RecalcXorPixels();

      SaveStatusStrings();
      gnUpdatePixelObjCount = 0;
      for (page_ptr=firstPage; page_ptr != NULL; page_ptr=page_ptr->next) {
         for (obj_ptr=page_ptr->bot; obj_ptr!=NULL; obj_ptr=obj_ptr->prev) {
            if (UpdatePixel(obj_ptr)) {
               changed = TRUE;
            }
         }
      }
      if (tgifObj->fattr != NULL) {
         for (attr_ptr=tgifObj->fattr; attr_ptr != NULL;
               attr_ptr=attr_ptr->next) {
            if (UpdatePixel(attr_ptr->obj)) {
               changed = TRUE;
            }
         }
      }
      RestoreStatusStrings();

      DestroyPinnedMenu(MENU_COLOR);
      if (colorLayers) {
         colorWindowFirstIndex = 0;
         RedrawColorWindow();
      }
      if (changed) ClearAndRedrawDrawWindow();
      return TRUE;
   }
   return FALSE;
}

void AddColor()
{
   char spec[MAXSTRING+1], *c_ptr=NULL;

   if (!colorDisplay) {
      MsgBox(TgLoadString(STID_ADDCOLOR_FOR_BW_DPY), TOOL_NAME, INFO_MB);
      return;
   }
   *spec = '\0';
   if (Dialog(TgLoadString(STID_ENTER_COLORS_TO_ADD), NULL, spec) == INVALID) {
      return;
   }
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;
   for (c_ptr=strtok(spec, ", \t\n\r"); c_ptr != NULL && *c_ptr != '\0';
         c_ptr=strtok(NULL, ", \t\n\r")) {
      int new_alloc;
      int index=QuickFindColorIndex(NULL, c_ptr, &new_alloc, FALSE);

      if (index == INVALID) {
         sprintf(gszMsgBox, TgLoadString(STID_FAIL_ALLOC_NAMED_COLOR), c_ptr);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         return;
      } else {
         ChangeAllSelColor(index, FALSE);
      }
   }
}

void ChooseColor()
{
   TgColorWheel_HandleCntrlSpace(mainDisplay, drawWindow);
}

void SetAltEditTextBgColor()
{
   char spec[MAXSTRING+1];

   *spec = '\0';
   sprintf(gszMsgBox, TgLoadString(STID_ENTER_EDIT_BG_COLOR),
         (altEditTextBgIndex==INVALID ?
         (useAltEditTextBgColor ? myFgColorStr : myBgColorStr) :
         altEditTextBgColorStr));
   if (Dialog(gszMsgBox, NULL, spec) == INVALID) return;
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   if (UtilStrICmp(spec, TgLoadCachedString(CSTID_NONE)) == 0) {
      *altEditTextBgColorStr = '\0';
      altEditTextBgIndex = INVALID;
      if (curChoice == DRAWTEXT && textCursorShown) {
         RedrawCurText();
      }
      return;
   }
   UtilStrCpyN(altEditTextBgColorStr, sizeof(altEditTextBgColorStr), spec);
   if (DoSetAltEditTextBgColor()) {
      if (curChoice == DRAWTEXT && textCursorShown) {
         if (!useAltEditTextBgColor) {
            useAltEditTextBgColor = TRUE;
            UpdatePinnedMenu(MENU_PROPERTIES);
         }
         RedrawCurText();
      }
   }
}

void SetAltEditTextHighlightColor()
{
   char spec[MAXSTRING+1];

   *spec = '\0';
   sprintf(gszMsgBox, TgLoadString(STID_ENTER_EDIT_HILIGHT_COLOR),
         (altEditTextHighlightIndex==INVALID ?
         (useAltEditTextBgColor ? myBgColorStr : myFgColorStr) :
         altEditTextHighlightColorStr));
   if (Dialog(gszMsgBox, NULL, spec) == INVALID) return;
   UtilTrimBlanks(spec);
   if (*spec == '\0') return;

   if (UtilStrICmp(spec, TgLoadCachedString(CSTID_NONE)) == 0) {
      *altEditTextHighlightColorStr = '\0';
      altEditTextHighlightIndex = INVALID;
      if (curChoice == DRAWTEXT && textCursorShown) {
         RedrawCurText();
      }
      return;
   }
   UtilStrCpyN(altEditTextHighlightColorStr,
         sizeof(altEditTextHighlightColorStr), spec);
   if (DoSetAltEditTextHighlightColor()) {
      RedrawCurText();
   }
}

void ToggleAltEditTextBgColor()
{
   useAltEditTextBgColor = (!useAltEditTextBgColor);
   RedrawCurText();
   UpdatePinnedMenu(MENU_PROPERTIES);
}

void SaveColors(FP)
   FILE *FP;
{
   int i;

   if (fprintf(FP, "color_info(%1d,%1d,%1d,[\n", maxColors, maxRGB,
         colorLayers) == EOF) {
      writeFileFailed = TRUE;
   }
   for (i=0; i < maxColors; i++) {
      if (i == maxColors-1) {
         if (fprintf(FP, "\t\"%s\", %1d, %1d, %1d, %1d, %1d, %1d, %1d\n",
               colorMenuItems[i],
               (int)(tgifColors[i].red), (int)(tgifColors[i].green),
               (int)(tgifColors[i].blue), (int)(tgifRequestedColors[i].red),
               (int)(tgifRequestedColors[i].green),
               (int)(tgifRequestedColors[i].blue),
               colorLayerOn==NULL ? 1 : colorLayerOn[i]) == EOF) {
            writeFileFailed = TRUE;
         }
      } else {
         if (fprintf(FP, "\t\"%s\", %1d, %1d, %1d, %1d, %1d, %1d, %1d,\n",
               colorMenuItems[i],
               (int)(tgifColors[i].red), (int)(tgifColors[i].green),
               (int)(tgifColors[i].blue), (int)(tgifRequestedColors[i].red),
               (int)(tgifRequestedColors[i].green),
               (int)(tgifRequestedColors[i].blue),
               colorLayerOn==NULL ? 1 : colorLayerOn[i]) == EOF) {
            writeFileFailed = TRUE;
         }
      }
   }
   if (fprintf(FP, "]).\n") == EOF) {
      writeFileFailed = TRUE;
   }
}

int ReadColors(FP, Inbuf)
   FILE *FP;
   char *Inbuf;
{
   char *c_ptr, *line;
   int index=0, max_colors=0, max_rgb=0, color_layers=FALSE;

   c_ptr = FindChar((int)'(', Inbuf);
   InitScan(c_ptr, "\t\n, ");
   if (GETINT("color_info", max_colors,   "maxColors") == INVALID ||
       GETINT("color_info", max_rgb,      "maxRGB") == INVALID ||
       GETINT("color_info", color_layers, "colorLayers") == INVALID) {
      return FALSE;
   }
   if (PRTGIF && !cmdLineOpenDisplay && cmdLineColor) {
      CleanUpColors();
      maxColors = max_colors;
      maxRGB = max_rgb;
      /* colorLayers = color_layers; */
      colorMenuItems = (char**)malloc(maxColors*sizeof(char*));
      tgifColors = (XColor*)malloc(maxColors*sizeof(XColor));
      if (colorMenuItems == NULL || tgifColors == NULL) FailAllocMessage();
      memset(colorMenuItems, 0, maxColors*sizeof(char*));
      memset(tgifColors, 0, maxColors*sizeof(XColor));
      for (index=0; index < maxColors; index++) {
         colorMenuItems[index] = (char*)malloc(COLORSTRLEN*sizeof(char));
         if (colorMenuItems[index] == NULL) FailAllocMessage();
         *colorMenuItems[index] = '\0';
      }
   }
   if (colorDisplay && (!(PRTGIF && !cmdLineOpenDisplay)) && !importingFile) {
      int saved_color_layers=colorLayers;

      colorLayers = color_layers;
      if (colorLayers != saved_color_layers) {
         if (colorLayers) {
            XMapWindow(mainDisplay, colorWindow);
            XMapWindow(mainDisplay, colorDummyWindow);
            Msg(TgLoadString(STID_COLOR_LAYERS_ENABLED));
         } else {
            XUnmapWindow(mainDisplay, colorWindow);
            XUnmapWindow(mainDisplay, colorDummyWindow);
            Msg(TgLoadString(STID_COLOR_LAYERS_DISABLED));
         }
         Reconfigure(TRUE);
      }
   }
   index = 0;
   while ((line=UtilGetALine(FP)) != NULL) {
      char color_s[40];
      int red=0, green=0, blue=0;
      int requested_red=0, requested_green=0, requested_blue=0, layer_on=FALSE;

      scanLineNum++;
      if (*line == ']') {
         free(line);
         break;
      }
      if (PRTGIF && !cmdLineOpenDisplay && cmdLineColor && index >= maxColors) {
         fprintf(stderr, "%s\n",
               TgLoadString(STID_TOO_MANY_LINES_IN_COLORINFO));
         free(line);
         break;
      }
      c_ptr = FindChar((int)'"', line);
      c_ptr = ParseStr(c_ptr, (int)'"', color_s, sizeof(color_s));
      InitScan(c_ptr, "\t\n, ");
      if (GETINT("color_info", red,             "red") == INVALID ||
          GETINT("color_info", green,           "green") == INVALID ||
          GETINT("color_info", blue,            "blue") == INVALID ||
          GETINT("color_info", requested_red,   "requested_red") == INVALID ||
          GETINT("color_info", requested_green, "requested_green") == INVALID ||
          GETINT("color_info", requested_blue,  "requested_blue") == INVALID ||
          GETINT("color_info", layer_on,        "layer_on") == INVALID) {
         return FALSE;
      }
      if (PRTGIF && !cmdLineOpenDisplay && cmdLineColor) {
         UtilStrCpyN(colorMenuItems[index], COLORSTRLEN*sizeof(char), color_s);
         if (cmdLineRequestedColor) {
            tgifColors[index].red = red;
            tgifColors[index].green = green;
            tgifColors[index].blue = blue;
         } else {
            tgifColors[index].red = requested_red;
            tgifColors[index].green = requested_green;
            tgifColors[index].blue = requested_blue;
         }
         index++;
      }
      if ((!(PRTGIF && !cmdLineOpenDisplay)) && !importingFile) {
         int new_alloc=FALSE;
         int cur_index=QuickFindColorIndex(NULL, color_s, &new_alloc, TRUE);

         if (cur_index != INVALID && cur_index < maxColors &&
               colorLayerOn != NULL) {
            colorLayerOn[cur_index] = layer_on;
         }
      }
      free(line);
   }
   prTgifFoundColorInfo = TRUE;
   return TRUE;
}

void DumpRGBColorLine(FP, ColorIndex, Indent, EndOfLine)
   FILE *FP;
   int ColorIndex, Indent, EndOfLine;
{
   register int i;

   if (colorDump) {
      for (i=0; i < Indent; i++) fprintf(FP, " ");
      if (tgifColors[ColorIndex].red == 0 &&
            tgifColors[ColorIndex].green == 0 &&
            tgifColors[ColorIndex].blue == 0) {
         fprintf(FP, "0 %s", gPsCmd[PS_SETGRAY]);
      } else if (tgifColors[ColorIndex].red == maxRGB &&
            tgifColors[ColorIndex].green == maxRGB &&
            tgifColors[ColorIndex].blue == maxRGB) {
         fprintf(FP, "1 %s", gPsCmd[PS_SETGRAY]);
      } else {
         fprintf(FP, "%.3f %.3f %.3f %s",
               ((float)tgifColors[ColorIndex].red/maxRGB),
               ((float)tgifColors[ColorIndex].green/maxRGB),
               ((float)tgifColors[ColorIndex].blue/maxRGB),
               gPsCmd[PS_SETRGBCOLOR]);
      }
      fprintf(FP, "%c", EndOfLine ? '\n' : ' ');
   } else {
      for (i=0; i < Indent; i++) fprintf(FP, " ");
      fprintf(FP, "0 %s\n", gPsCmd[PS_SETGRAY]);
   }
}

