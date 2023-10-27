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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/cutpaste.e,v 1.21 2011/05/19 16:46:22 william Exp $
 */

#ifndef _CUTPASTE_E_
#define _CUTPASTE_E_

#define CBI_TGIF 0
#define CBI_UTF8 1
#define CBI_TEXT 2

typedef struct tagSetCutBufferInfo {
   int tgif_valid; 
   int utf8_valid;
   int text_valid;
   struct DynStrRec tgif_dyn_str;
   struct DynStrRec utf8_dyn_str;
   struct DynStrRec text_dyn_str;
} SetCutBufferInfo;

extern int	copyingToCutBuffer;
extern int	pastingFile;

extern char	gszFilesIniFile[MAXPATHLENGTH];
extern char	*gpszRecentFilesSec;
extern char	*gpszRecentFilesCountKey;

extern int	cutBufferIsTgifObj;
extern int	cutBufferIsUTF8;
extern struct DynStrRec	dsCutBuffer;

extern Time	startSelectionOwnershipTime;
extern Time	endSelectionOwnershipTime;
extern int	startSelectionOwnershipTimeValid;
extern int	endSelectionOwnershipTimeValid;

extern SetCutBufferInfo	gSetCutBufferInfo;

#ifdef _INCLUDE_FROM_CUTPASTE_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_CUTPASTE_C_*/

extern int	WriteBufToCutBuffer ARGS_DECL((char *buf, int buf_sz,
		                               int buf_is_simple_string,
		                               int buf_is_utf8_string,
		                               SetCutBufferInfo*));
extern void	ClearSelection ARGS_DECL((void));
extern int	CopyToCutBuffer ARGS_DECL((void));
extern int	CopyPlainTextAsObject ARGS_DECL((void));
extern void	CutToCutBuffer ARGS_DECL((void));
extern unsigned int	PasteString ARGS_DECL((char*, int highlight,
			                       int record_cmd));
extern void	AssignNewObjIds ARGS_DECL((struct ObjRec *));
extern void	FreeSelectionOrCutBuffer ARGS_DECL((char*, int nFromSelection));
extern char	* FetchSelectionOrCutBuffer ARGS_DECL((int *pnLen,
		                                       int *pnFromSelection));
extern int	PasteFromCutBuffer ARGS_DECL((void));
extern int	PasteFromFile ARGS_DECL((void));
extern char	* GetTextBytesFromSelection ARGS_DECL((int compound_text,
		                                       unsigned long*));
extern int	PasteCompoundText ARGS_DECL((void));
extern int	CopyDoubleByteString ARGS_DECL((void));
extern int	PasteDoubleByteString ARGS_DECL((void));
extern void	CleanUpCutBuffer ARGS_DECL((void));

extern void	SetIntPropertyMask ARGS_DECL((long lWhich, int nValue,
		                              char *pszValue, long *plMask,
		                              long *plSkip,
		                              struct PropertiesRec *pProp));
extern void	SetFontPropertyMask ARGS_DECL((int nDoubleByte, int nFont,
		                               int nStyle, long *plMask,
		                               long *plSkip,
		                               struct PropertiesRec *pProp));
extern void	SetCTMPropertyMask ARGS_DECL((struct XfrmMtrxRec *ctm,
		                              long *plMask, long *plSkip,
		                              struct PropertiesRec *pProp));
extern void	SetTextPropMask ARGS_DECL((struct ObjRec *, long *plMask,
		                           long *plSkip,
		                           struct PropertiesRec *pProp));
extern void	CleanUpProperties ARGS_DECL((void));
extern void	InitProperties ARGS_DECL((void));
extern void	DoGetProperty ARGS_DECL((int index));
extern EditAttrInfo	*CreateGetPropertyInfo ARGS_DECL((void));
extern void	CopyProperties ARGS_DECL((int nPrompt));
extern void	SaveProperties ARGS_DECL((void));
extern void	PasteProperties ARGS_DECL((int nPrompt));
extern void	RestoreProperties ARGS_DECL((void));

extern void	BackupCopiedProperties ARGS_DECL((void));
extern void	RestoreCopiedProperties ARGS_DECL((void));
extern int	EditIniSection ARGS_DECL((char *pszTitle, char *pszSection,
		                          char *pszIniFile, AfterLoopFunc*));
extern char	*SelectFromIniSection ARGS_DECL((char *pszTitle,
		                                 char *pszSection,
		                                 char *pszIniFile));

extern void	FreeRecentFilesListing ARGS_DECL((KeyValInfo*, int));
extern KeyValInfo	*RecentFilesListing ARGS_DECL((int *pnEntries));
extern void	OpenARecentlyUsedFile ARGS_DECL((int file_index));
extern void	AddARecentlyUsedFile ARGS_DECL((char *path));
extern int	InitRecentFiles ARGS_DECL((void));
extern void	CleanUpRecentFiles ARGS_DECL((void));

extern int	CreateSimpleStringObj ARGS_DECL((char*));
extern void	SaveSimpleStringObj ARGS_DECL((FILE*, struct ObjRec *));
extern void	ReadSimpleStringObj ARGS_DECL((FILE*, char*, struct ObjRec **));
extern void	FreeSimpleStringObj ARGS_DECL((struct ObjRec *));

extern void	ReplaceGraphic ARGS_DECL((void));

extern void	ImportMultipageTextFile ARGS_DECL((void));
extern void	SetMarginsForImportMultipageTextFile ARGS_DECL((void));
extern void	ToggleWordWrapDuringImportMultipageTextFile ARGS_DECL((void));
extern int	RefreshImportMutipageTextFileMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateImportMutipageTextFileMenu ARGS_DECL((TgMenu*, int X,
				int Y, TgMenuInfo*, int status_str_xlated));

extern void	CleanUpCutPaste ARGS_DECL((void));
extern int	InitCutPaste ARGS_DECL((void));

#ifdef _INCLUDE_FROM_CUTPASTE_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_CUTPASTE_C_*/

#endif /*_CUTPASTE_E_*/
