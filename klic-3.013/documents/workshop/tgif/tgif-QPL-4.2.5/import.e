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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/import.e,v 1.15 2011/05/16 16:21:58 william Exp $
 */

#ifndef _IMPORT_E_
#define _IMPORT_E_

#ifdef _INCLUDE_FROM_IMPORT_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_IMPORT_C_*/

extern void	MillisecondSleep ARGS_DECL((int ms));
extern int	ExtensionMatch ARGS_DECL((char *Spec, char *DirName));
extern char	*SetUpExtStr ARGS_DECL((int cbBuf, char *szExt,
		char *szOtherExt));
extern void	CleanUpImport ARGS_DECL((void));
extern void	InitImport ARGS_DECL((void));
extern void	ImportXBitmapFile ARGS_DECL((void));
extern void	ImportXPixmapFile ARGS_DECL((void));
extern void	ImportEPSFile ARGS_DECL((int Embed, char *psz_fname_spec));
extern int	ConvertGifToXpm ARGS_DECL((char *pszGifPath, char *pszXpmPath,
		                           int xpm_path_sz));
extern int	ConvertPngToXpm ARGS_DECL((char *pszPngPath, char *pszXpmPath,
		                           int xpm_path_sz));
extern int	ConvertJpegToXpm ARGS_DECL((char *pszJpegPath, char *pszXpmPath,
		                            int xpm_path_sz));
extern int	ConvertImageFileToPpm6 ARGS_DECL((char *pszImageFilePath,
		                                  char *pszPpm6Path,
		                                  int ppm6_path_sz,
		                                  int format));
extern int	ConvertJpegToPpm6 ARGS_DECL((char *pszJpegPath,
		                             char *pszPpm6Path,
		                             int ppm6_path_sz));
extern int	ConvertPngToPpm6 ARGS_DECL((char *pszJpegPath,
		                            char *pszPpm6Path,
		                            int ppm6_path_sz));
extern void	ImportPNGFile ARGS_DECL((void));
extern void	ImportJPEGFile ARGS_DECL((int Embed, char *psz_fname_spec));
extern void	ImportGIFFile ARGS_DECL((void));
extern int	ConvertPbmToXbm ARGS_DECL((char *pszPbmPath, char *pszXbmPath,
		                           int xbm_path_sz));
extern void	ImportPBMFile ARGS_DECL((void));
extern int	ConvertPgmToXpm ARGS_DECL((char *pszPgmPath, char *pszXpmPath,
		                           int xpm_path_sz));
extern void	ImportPGMFile ARGS_DECL((void));
extern int	ConvertPpmToXpm ARGS_DECL((char *pszPpmPath, char *pszXpmPath,
		                           int xpm_path_sz));
extern void	ImportPPMFile ARGS_DECL((void));
extern DspList	*ImportFilterListing ARGS_DECL((int *pnEntries));
extern int	GetEnableFailedImportMsgBox ARGS_DECL((void));
extern int	SetEnableFailedImportMsgBox ARGS_DECL((int));
extern void	ImportOtherFile ARGS_DECL((void));
extern void	ImportOtherFileType ARGS_DECL((char*));
extern int	ImportSpecifiedFileType ARGS_DECL((char *psz_path,
		                                   char *psz_filter));
extern int	ConvertOtherToXpm ARGS_DECL((char *pszFile, char *pszFormat,
		                             char *pszXpmPath,
		                             int xpm_path_sz));
extern void	ImportGIFToXPixmapDeck ARGS_DECL((void));
extern void	CreateThumbnails ARGS_DECL((void));
extern void	BrowseXBitmap ARGS_DECL((void));
extern void	BrowseXPixmap ARGS_DECL((void));
extern void	BrowseOther ARGS_DECL((void));
extern void	BrowseOtherType ARGS_DECL((char*));

extern struct ObjRec	* CreatePpmTrueObjFromImage ARGS_DECL((XImage*,
					int image_w, int image_h,
					char *ppm_data,
					unsigned int data_size));
extern void	ScreenCapture ARGS_DECL((void));
extern void	FullScreenCapture ARGS_DECL((void));
extern void	DelayedFullScreenCapture ARGS_DECL((void));
extern void	ToggleHideDuringCapture ARGS_DECL((void));
extern int	RefreshScreenCaptureMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateScreenCaptureMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                    TgMenuInfo*,
		                                    int status_str_xlated));
extern void	ToggleAutoEPSPreviewBitmap ARGS_DECL((void));
extern int	RefreshImportMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateImportMenu ARGS_DECL((TgMenu*, int X, int Y, TgMenuInfo*,
		                             int status_str_xlated));

#ifdef _INCLUDE_FROM_IMPORT_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_IMPORT_C_*/

#endif /*_IMPORT_E_*/
