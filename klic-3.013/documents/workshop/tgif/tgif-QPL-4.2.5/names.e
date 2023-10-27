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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/names.e,v 1.6 2011/05/16 16:21:58 william Exp $
 */

#ifndef _NAMES_E_
#define _NAMES_E_

extern char	curDomainName[];
extern char	curDomainPath[];
extern char	curDir[];
extern char	curLocalDir[];
extern char	curSymDir[];

extern int	doubleClickInterval;
extern int	importFromLibrary;
extern int	importingFromExec;
extern int	curDirIsLocal;

extern int	ignoreDirectoryFlag;
		/* use to be ignoreDirectoryFlagInMakeNameDspItemArray; */

#ifdef _INCLUDE_FROM_NAMES_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_NAMES_C_*/

extern void	ParseSymPath ARGS_DECL((char*));
extern void	InitNames ARGS_DECL((void));
extern void	UpdateDirInfo ARGS_DECL((void));
extern void	UpdateSymInfo ARGS_DECL((void));
extern void	CleanUpNames ARGS_DECL((void));

extern int	DirInSymPath ARGS_DECL((char *DirName));
extern char	**MakeNameDspItemArray ARGS_DECL((int Entries, DspList*));

extern int	SelectFileName ARGS_DECL((char *MsgStr, char *SelStr));
extern int	SelectFileNameToPaste ARGS_DECL((char *MsgStr, char *SelStr));
extern int	SelectFileNameToImport ARGS_DECL((char *MsgStr, char *ExtStr,
		                                  char *SelStr));
extern int	GetSymbolPath ARGS_DECL((char *SymName, int Pin,
		                         char *PathName));
extern int	NameInCurDir ARGS_DECL((char *FileName));
extern void	ChangeDomain ARGS_DECL((void));
extern int	SelectSymDir ARGS_DECL((char *SelStr));
extern int	SelectFromLibrary ARGS_DECL((char *MsgStr, char *ExtStr,
		                             char *SelStr, char *PathStr));
extern void	SetCurDir ARGS_DECL((char *FileName));
extern void	SetCurSymDir ARGS_DECL((char *FileName));
extern void	SetCurImportDir ARGS_DECL((char *FileName));

extern void	CopyDomainInfoToIni ARGS_DECL((void));
extern void	EditDomainPaths ARGS_DECL((void));
extern void	SelectDefaultDomain ARGS_DECL((void));
extern void	AddADomain ARGS_DECL((void));
extern void	DeleteADomain ARGS_DECL((void));
extern void	ReloadDomainInfoFromX ARGS_DECL((void));

extern int	RefreshDomainMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateDomainMenu ARGS_DECL((TgMenu*, int X, int Y,
		                             TgMenuInfo*,
		                             int status_str_xlated));

#ifdef _INCLUDE_FROM_NAMES_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_NAMES_C_*/

#endif /*_NAMES_E_*/
