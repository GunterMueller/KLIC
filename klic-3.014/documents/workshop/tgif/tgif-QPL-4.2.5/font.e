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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/font.e,v 1.20 2011/05/16 16:21:57 william Exp $
 */

#ifndef _FONT_E_
#define _FONT_E_

#define CONVERT_TO_UTF8		(TRUE)
#define CONVERT_FROM_UTF8	(FALSE)

extern XFontStruct	* canvasFontPtr;
extern int	canvasFontHeight;
extern int	canvasFontAsc;
extern int	canvasFontDes;
extern int	canvasFontDirection;
extern int	canvasFontDontReencode;
extern int	canvasFontDoubleByte;
extern int	canvasFontDoubleByteModBytes;
extern int	canvasFontDoubleByteVertical;
extern int	canvasFontSize;
extern int	canvasFontIsFaked;
extern int	canvasLineAsc;
extern int	canvasLineDes;

extern XFontStruct	* rulerFontPtr;
extern int	rulerFontWidth;
extern int	rulerFontHeight;
extern int	rulerFontAsc;
extern int	rulerFontDes;

extern XFontStruct	* defaultFontPtr;
extern int	defaultFontWidth;
extern int	defaultFontHeight;
extern int	defaultFontAsc;
extern int	defaultFontDes;

extern XFontStruct	* menuFontPtr;
extern int	menuFontWidth;
extern int	menuFontHeight;
extern int	menuFontAsc;
extern int	menuFontDes;

#ifdef ENABLE_NLS
extern XFontSet	menuFontSet;
extern XFontSet	msgFontSet;
extern XFontSet	boldMsgFontSet;
extern XFontSet	italicMsgFontSet;
extern XFontSet	boldItalicMsgFontSet;
#else /* ~ENABLE_NLS */
extern char	*menuFontSet;
extern char	*msgFontSet;
extern char	*boldMsgFontSet;
extern char	*italicMsgFontSet;
extern char	*boldItalicMsgFontSet;
#endif /* ENABLE_NLS */
extern int	boldMsgFontDoubleByte;

extern XFontStruct	* boldMsgFontPtr;
extern int	boldMsgFontWidth;
extern int	boldMsgFontHeight;
extern int	boldMsgFontAsc;
extern int	boldMsgFontDes;

extern XFontStruct	* italicMsgFontPtr;
extern int	italicMsgFontWidth;
extern int	italicMsgFontHeight;
extern int	italicMsgFontAsc;
extern int	italicMsgFontDes;

extern XFontStruct	* boldItalicMsgFontPtr;
extern int	boldItalicMsgFontWidth;
extern int	boldItalicMsgFontHeight;
extern int	boldItalicMsgFontAsc;
extern int	boldItalicMsgFontDes;

extern XFontStruct	* msgFontPtr;
extern int	msgFontWidth;
extern int	msgFontHeight;
extern int	msgFontAsc;
extern int	msgFontDes;

extern int	curFont;
extern int	curSzUnit, curRealSzUnit;
extern int	curStyle;
extern int	textRotation;
extern int	curUnderlineOn;
extern int	curOverlineOn;

extern int	actualCurFont;
extern int	actualCurSzUnit;
extern int	actualCurStyle;

extern int	curUnderlineYOffset;
extern int	curOverlineYOffset;

extern int	changePropertiesOfAttrs;

extern char	* * fontMenuStr;
extern char	* defFontMenuStr[];
extern char	* * sizeMenuStr;
extern int	* fontSzUnits;
extern int	numFonts;
extern int	numFontSizes;

extern int	changingFontSizeFromRead;
extern int	allowFontFaceSubstitution;

extern char	cmdLineDontReencode[];

extern int	defaultFontSize;

extern int	showFontSizeInPoints;

extern int	substitutedFont;
extern int	substitutedFontSzUnit;

extern int	defaultSingleByteFont;

extern int		gnNumPSFontAliases;
extern KeyValInfo	*gaPSFontAliases;

#ifdef _INCLUDE_FROM_FONT_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_FONT_C_*/

extern int	IsFontDoubleByte ARGS_DECL((int font_index));
extern int	IsDoubleByteFontVertical ARGS_DECL((int font_index, int style));
extern int	CanConvertUTF8ToFont ARGS_DECL((int font_index));
extern int	CanConvertFontToUTF8 ARGS_DECL((int font_index));
extern int	ConvertToOrFromUTF8 ARGS_DECL((int which, int font_index,
                                         char **ppsz_cut_buffer));
extern int	GetInitialFont ARGS_DECL((void));
extern int	SzUnitToFontSize ARGS_DECL((int sz_unit));
extern int	SzUnitToPointSize ARGS_DECL((int sz_unit));

extern int	BadDoubleByteString ARGS_DECL((char *pszStr, char *pszZero));
extern char	* GetPrevChar ARGS_DECL((int nDoubleByte, char *pszStr,
		                         char *pszCur));
extern char	* GetShowFontChar ARGS_DECL((int FontIndex));
extern void	GetCurFontInfoStr ARGS_DECL((char *buf, int buf_sz));
extern XFontStruct	*LoadAFontByName ARGS_DECL((char *font_name,
		                                    int double_byte,
		                                    int can_debug));
extern void	GetPSFontStr ARGS_DECL((int FontIndex, int Style, char *));
extern XFontStruct	*LoadASymbolFont ARGS_DECL((int font_size));
extern void	MapAliasedPSFontName ARGS_DECL((char *buf, int buf_sz));
extern void	SetTextExtentsInfo ARGS_DECL((TextExtentsInfo*));
extern struct StrRec	*SegmentDoubleByteString ARGS_DECL((char*));
extern void	MyTextExtents ARGS_DECL((XFontStruct *xfs, char *pszStr,
		                         int nLen, XCharStruct *pxcs));
extern int	MyTextWidth ARGS_DECL((XFontStruct *xfs, char *pszStr,
		                       int nLen));
extern void	MyDrawString ARGS_DECL((Display*, Drawable, GC gc, int nDepth,
		                        int x, int y, char *pszStr, int nLen));
extern Pixmap	PaintVertChar ARGS_DECL((TextExtentsInfo*, int nRotate));
extern int	ValidCharCode ARGS_DECL((char *));
extern char	* CharCodeTranslate ARGS_DECL((char *));
extern void	SaveDontReencode ARGS_DECL((FILE*));
extern int	ReadDontReencode ARGS_DECL((char*));
extern void	SavePSFontAliases ARGS_DECL((FILE*));
extern void	SaveDoubleByteModBytes ARGS_DECL((FILE*));
extern void	SavePSFontNeedCharSubs ARGS_DECL((FILE*));
extern void	SavePSCharSubs ARGS_DECL((FILE*));
extern int	ReadPSFontAliases ARGS_DECL((FILE*, char*));
extern int	ReadPSFontNeedCharSubs ARGS_DECL((FILE*, char*));
extern int	ReadPSCharSubs ARGS_DECL((FILE*, char*));
extern void	PrepareMiniLines ARGS_DECL((MiniLinesInfo*));
extern void	PrepareEightBitFontInfo ARGS_DECL((void));
extern int	NeedEncode ARGS_DECL((char *FontName, int FontIndex,
		                      int Style));
extern int	GetFontIndex ARGS_DECL((char *FontStr, int Style,
		                        int MustFind));
extern int	GetFontAndStyle ARGS_DECL((char *FontStr, int *pnFont,
		                           int *pnStyle, int MustFind));
extern int	GetFontIndexFromPSFontAndStyle ARGS_DECL((char *FontStr,
                                                          int Style,
                                                          int *pnDoubleByte,
                                                          int *pnModified));
extern void	CleanUpDontReencode ARGS_DECL((void));
extern void	InitDontReencode ARGS_DECL((char*));
extern void	InitPSFontCharSubs ARGS_DECL((void));
extern void	PrTgifInitBaseFonts ARGS_DECL((void));
extern int	GetCurSzUnit ARGS_DECL((void));
extern void	PutCurSzUnit ARGS_DECL((int sz_unit));
extern void	GetCurFontMsg ARGS_DECL((char*, StrSegInfo*));
extern void	CurFontMsg ARGS_DECL((int nShowInMsgWindow,
		                      int nShowInStatusWindow, StrSegInfo*));
extern void	DumpReEncodeVector ARGS_DECL((FILE*, char *vec_prefix,
		                              char *vec_suffix,
		                              short *flag_ptr));
extern void	DumpEightBitFontInfo ARGS_DECL((FILE*));

extern int	GetCompatibleSize ARGS_DECL((int FontDPI, int FontSize));
extern int	SetCanvasFont ARGS_DECL((void));
extern int	CurFontCheck ARGS_DECL((void));
extern int	TrySetCanvasFont ARGS_DECL((int font, int style, int size,
		                            int must_find_requested,
		                            int *pnGotRequested));
extern void	ObjFontInfoToCurFontInfo ARGS_DECL((struct TextRec *));
extern void	StrSegInfoToCurFontInfo ARGS_DECL((StrSegInfo*));
extern void	InitFonts ARGS_DECL((void));
extern void	SaveCurFont ARGS_DECL((void));
extern void	RestoreCurFont ARGS_DECL((void));
extern struct PushedFontRec	*GetTopFontStk ARGS_DECL((void));
extern struct PushedFontRec	*PushCurFont ARGS_DECL((void));
extern void	PopCurFont ARGS_DECL((void));
extern void	SetPushedFontValue ARGS_DECL((int Which, int Value));
extern int	ChangeMiniLinesFontProp ARGS_DECL((MiniLinesInfo*, int which,
		                                   int value));
extern int	ChangeObjTextStyle ARGS_DECL((struct ObjRec *, int Style));
extern void	ChangeFontStyle ARGS_DECL((int Style));
extern int	ChangeObjTextJust ARGS_DECL((struct ObjRec *, int Just));
extern void	ChangeFontJust ARGS_DECL((int Just));
extern int	ChangeObjTextUnderline ARGS_DECL((struct ObjRec *,
		                                  int Underline));
extern void	ChangeFontUnderline ARGS_DECL((int Underline));
extern int	ChangeObjTextOverline ARGS_DECL((struct ObjRec *,
		                                  int Overline));
extern void	ChangeFontOverline ARGS_DECL((int Overline));
extern int	RefreshFontStyleMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateFontStyleMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                TgMenuInfo*,
		                                int status_str_xlated));
extern int	StyleMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern int	GetSizeMenuIndex ARGS_DECL((void));
extern int	ChangeObjTextSzUnit ARGS_DECL((struct ObjRec *, int sz_unit));
extern void	ChangeFontSize ARGS_DECL((int SizeIndex));
extern int	RefreshFontSizeMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateFontSizeMenu ARGS_DECL((TgMenu*, int X, int Y,
		                               TgMenuInfo*,
		                               int status_str_xlated));
extern int	SizeMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern int	ChangeObjTextFont ARGS_DECL((struct ObjRec *, int FontIndex));
extern int	ChangeFont ARGS_DECL((int FontIndex, int ForceNewChoice));
extern int	RefreshFontMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateFontMenu ARGS_DECL((TgMenu*, int X, int Y, TgMenuInfo*,
		                           int status_str_xlated));
extern int	FontMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern int	ChangeObjVSpace ARGS_DECL((struct ObjRec *, int VSpace));
extern void	ChangeVSpace ARGS_DECL((int VSpace));
extern void	ChangeAllSelRealSzUnit ARGS_DECL((int sz_unit, int HighLight));
extern void	SetSelFontSize ARGS_DECL((char*));
extern void	CleanUpFonts ARGS_DECL((void));

extern void	VerifyCompatibleFontIndex ARGS_DECL((int*));
extern void	GetCompatibleFontName ARGS_DECL((int old_font_index, int style,
		                                 char *font_str));

#ifdef _INCLUDE_FROM_FONT_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_FONT_C_*/

#endif /*_FONT_E_*/
