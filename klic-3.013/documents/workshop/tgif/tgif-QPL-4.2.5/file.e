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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/file.e,v 1.17 2011/05/16 16:21:57 william Exp $
 */

#ifndef _FILE_E_
#define _FILE_E_

extern int	PRTGIF;
extern int	prTgifFoundColorInfo;
extern char	curFileName[MAXPATHLENGTH+1];
extern int	curFileDefined;
extern int	fileVersion;
extern int	curFileWriteVersion;
extern int	importingFile;
extern int	mergingFile;
extern int	serializingFile;
extern int	deserializingFile;
extern int	psDotsPerInch;
extern float	printMag;
extern int	saveTmpOnReturn;
extern int	warpToWinCenter;
extern float	tiledPageScaling;

extern char	* psXOffStr[];
extern float	psXOff[];
extern char	* * psYOffStr;
extern float	* psYOff;
extern float	* psPageWidthInInch;
extern float	* psPageHeightInInch;
extern char	printCommand[];
extern char	outputDir[];
extern char	* fileMenuStr[];

extern char	* savedComments;
extern int	savedCommentsLen;
extern int	saveCommentsInSaveNew;
extern int	usePsAdobeString;
extern char	adobeString[];
extern char	epsfString[];

extern int	readingPageNum;
extern int	loadedCurPageNum;

extern int	writeFileFailed;
extern int	foundGoodStateObject;

extern int	cmdLineHasPageNum;
extern int	cmdLinePageNum;
extern char	cmdLinePageNumStr[80];

extern int	cmdLineOneFilePerPage;
extern int	cmdLineA4;
extern int	cmdLineLetter;
extern int	cmdLineNoShowPageInEPS;
extern int	cmdLineColor;
extern int	cmdLineRequestedColor;

extern int	cmdLineDumpURL;
extern int	cmdLineDumpURLWithHeader;
extern int	cmdLineDumpURLHeaderOnly;
extern int	cmdLineDumpURLShowStatus;

extern int	cmdLineDosEpsFilter;
extern int	cmdLinePreviewOnly;

extern int	tmpFileMode;
extern char	cmdLineTmpFileModeStr[MAXSTRING];

extern int	cmdLineOpenDisplay;
extern int	cmdLineStdOut;
extern int	cmdLineWhereToPrint;
extern int	cmdLineTiffEPSI;
extern int	cmdLinePdfSetPageDevice;
extern int	cmdLinePsSetup;

extern char	cmdLineFileToExec[MAXSTRING];
extern char	cmdLineProducedBy[MAXSTRING];
extern char	cmdLineOpenFile[MAXSTRING];
extern char	cmdLineJpegToPpm6Cmd[MAXSTRING];

extern int	cmdLineQuiet;

extern char	cmdLineCustomPatternDir[MAXSTRING];
extern char	customPatDir[MAXSTRING];

extern int	generateTiffEPSI;
extern int	showPageInEPS;

extern int	gnQuit;

extern int	myFileBgPixel;
extern int	myFileFgPixel;
extern char	*myFileBgColorStr;
extern char	*myFileFgColorStr;
extern XColor	myFileBgColor;
extern int	myFileBgPixmapW;
extern int	myFileBgPixmapH;
extern Pixmap	myFileBgPixmap;
extern char	*myFileBgGifURL;
extern char	*gpszProducedBy;

extern int	dumpOneFilePerPage;

extern int	bufferAsFileForWhiteBoard;

extern int	flushColormapOnOpen;

extern struct BBRec	*gpExportClipBBox;

extern int		gnNumFilePSFontAliases;
extern KeyValInfo	*gaFilePSFontAliases;

extern SpecifyPagesInfo	gPagesToPrintSpec;

extern GenerateByInfo	gGenerateByInfo;

#ifdef _INCLUDE_FROM_FILE_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_FILE_C_*/

extern char	* MkTempFile ARGS_DECL((char *buf, int buf_sz, char *psz_dir,
		                        char *psz_prefix));
extern int	IsFiletUnSavable ARGS_DECL((void));
extern void	SetUnSavableFile ARGS_DECL((int unsavable));
extern void	UpdateDocumentFonts ARGS_DECL((char*));
extern void	UpdatePropagatedEPSInfo ARGS_DECL((char *line, int *status));
extern void	SetCmdLineDontCondense ARGS_DECL((void));
extern void	SetCmdLineCondensedPS ARGS_DECL((void));

extern int	FindProgramInPath ARGS_DECL((char *cmd, char *msg, int no_msg));
extern int	ExecuteCmd ARGS_DECL((char*, int done_msg));
extern void	CleanUpComments ARGS_DECL((void));
extern void	ClearFileInfo ARGS_DECL((int clean_up_comments));
extern int	OkayToCreateFile ARGS_DECL((char*));
extern void	SaveObj ARGS_DECL((FILE*, struct ObjRec *ObjPtr, int Level));
extern int	FileNameHasExtension ARGS_DECL((char *file_name, int file_type,
		                                int *pn_gzipped,
		                                int *pn_no_name));
extern void	RemoveFileNameExtension ARGS_DECL((char*));
extern int	FixDecimalPoint ARGS_DECL((char*));
extern void	Save ARGS_DECL((FILE *, struct ObjRec *BotObj, int Level,
		                int PageNumber));
extern int	SaveTmpFile ARGS_DECL((char *));
extern void	SaveNewFile ARGS_DECL((int SaveSelectedOnly, char *pszNewFile));
extern void	SaveSymInLibrary ARGS_DECL((void));
extern void	SaveFile ARGS_DECL((void));
extern void	SavePages ARGS_DECL((SpecifyPagesInfo*));
extern char	* ParseStr ARGS_DECL((char *Str, int C, char *Left,
		                      int LeftSz));
extern char	* FindChar ARGS_DECL((int C, char *Str));
extern void	SaveCreatorID ARGS_DECL((FILE *FP, struct ObjRec *ObjPtr,
		                         char *pszPrefix));
extern int	ReadCreatorID ARGS_DECL((FILE *FP, struct ObjRec **ObjPtr));
extern int	ReadObj ARGS_DECL((FILE *, struct ObjRec **));
extern void	AdjForOldVersion ARGS_DECL((struct ObjRec *));
extern int	DownloadRemoteFile ARGS_DECL((char *file_name,
		                              char **ppsz_content_type,
		                              char **ppsz_page_spec,
		                              int *pn_is_html,
		                              char *return_tmp_fname,
		                              char *final_url,
		                              int cb_final_url));
extern int	ImportGivenFile ARGS_DECL((char*, int group_and_lock,
		                           int highlight));
extern void	ImportFile ARGS_DECL((void));
extern int	ClearBgColorInfo ARGS_DECL((int nRedraw));
extern int	ClearBgPixmapInfo ARGS_DECL((int nRedraw));
extern int	SetFileBgColor ARGS_DECL((void));
extern int	SetFileFgColor ARGS_DECL((void));
extern int	SetFileBgPixmap ARGS_DECL((void));
extern void	CheckFileAttrsInLoad ARGS_DECL((void));
extern int	LoadFile ARGS_DECL((char *FileName, int ObjFile,
		                    int GzippedObjFile));
extern void	DumpPatFill ARGS_DECL((FILE *, int Fill, struct BBRec,
		                       int Indent, int EOL));
extern void	DumpSymOutline ARGS_DECL((FILE *, struct ObjRec *));
extern int	DumpBBox ARGS_DECL((FILE *, int PageOnly, struct BBRec *));
extern void	ModifyOutputFileName ARGS_DECL((char *));
extern void	SetPsSetup ARGS_DECL((char *));
extern void	SetBopHook ARGS_DECL((char *));
extern void	SetEopHook ARGS_DECL((char *));
extern int	GetCmdUsingDefAndXDef ARGS_DECL((char *buf, int buf_sz,
		                                 char *def_cmd, char *option,
		                                 int verify_count,
		                                 int msg_box_if_error));
extern void	SetOutputFileName ARGS_DECL((char *psz_file_return,
		                             char *psz_ext, int *pn_short_name,
		                             char **ppsz_rest));
extern void	Dump ARGS_DECL((char *));
extern void	DumpPages ARGS_DECL((void));
extern void	DumpOnePageInTileMode ARGS_DECL((int Row, int Col));
extern void	DumpOnePageInStackMode ARGS_DECL((void));
extern void	DumpOneFilePerPage ARGS_DECL((void));
extern void	PrintWithCommand ARGS_DECL((char *));
extern void	PrintSelectedObjs ARGS_DECL((void));
extern void	SetPrintReduction ARGS_DECL((void));
extern void	NewProc ARGS_DECL((void));
extern int	BeforeOpenURL ARGS_DECL((int *pn_do_not_save));
extern void	OpenURL ARGS_DECL((char *psz_url, int do_not_save,
		                   int *pn_need_to_check_auto_exec));
extern void	AfterOpenURL ARGS_DECL((int need_to_check_auto_exec));
extern void	OpenProc ARGS_DECL((char*));
extern void	SetTemplate ARGS_DECL((void));
extern int	QuitProc ARGS_DECL((void));
extern int	SolveProc ARGS_DECL((void));
extern int	SimulateProc ARGS_DECL((void));
extern int	ProbeProc ARGS_DECL((void));
extern int	AnimateProc ARGS_DECL((void));
extern int	EscapeProc ARGS_DECL((void));
extern int	RefreshFileMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateFileMenu ARGS_DECL((TgMenu*, int X, int Y, TgMenuInfo*,
		                           int status_str_xlated));
extern int	FileMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern TgMenu	*CreateRecentFilesMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                  TgMenuInfo*,
		                                  int status_str_xlated));
extern char	* ReadFileIntoBuf ARGS_DECL((char *fname,
		                             unsigned int *pn_buf_sz));
extern int	WriteBufIntoTmpFile ARGS_DECL((char *buf, int buf_sz,
		                               char *tmp_fname,
		                               int tmp_fname_sz));
extern void	CleanUpFiles ARGS_DECL((void));

#ifdef _INCLUDE_FROM_FILE_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_FILE_C_*/

#endif /*_FILE_E_*/
