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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/special.e,v 1.8 2011/05/16 16:21:59 william Exp $
 */

#ifndef _SPECIAL_E_
#define _SPECIAL_E_

extern int	placingTopObj;
extern int	connectingPortsByWire;
extern int	connectingPortsFromInternalCommand;

extern char	gszAttrSeparator[];

#ifdef _INCLUDE_FROM_SPECIAL_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_SPECIAL_C_*/

extern struct ObjRec	* ReadSymbol ARGS_DECL((FILE *, int FileType));
extern struct ObjRec	* GetObjRepresentation ARGS_DECL((char *PathName,
			                                  char *SymName,
			                                  int FileType));
extern unsigned int	PlaceTopObj ARGS_DECL((struct ObjRec *,
		                               struct ObjRec *savedTopObj,
		                               struct ObjRec *savedBotObj));
extern void	Instantiate ARGS_DECL((void));
extern int	ObjIsAPort ARGS_DECL((struct ObjRec *));
extern void	ConnectTwoPortsByAWire ARGS_DECL((void));
extern void	RepeatConnectTwoPortsByAWire ARGS_DECL((void));
extern void	ConnectPortsToBroadcastWire ARGS_DECL((void));
extern void	RenameSignalNameForAPort ARGS_DECL((void));
extern void	ClearSignalNameForAPort ARGS_DECL((void));
extern void	MergePortsWithAnObject ARGS_DECL((void));
extern void	RenumberObjectIds ARGS_DECL((void));
extern void	MakeSymbolic ARGS_DECL((void));
extern void	UnMakeSymbolic ARGS_DECL((void));
extern int	MakeIconic ARGS_DECL((char *SymPath, int record_cmd));
extern void	UnMakeIconic ARGS_DECL((void));
extern void	ImportAttrs ARGS_DECL((void));
extern int	ImportNamedAttrs ARGS_DECL((FILE*, struct ObjRec *,
				int num_restricted, char **ppsz_restricted,
				char *fname));
extern void	ExportAttrs ARGS_DECL((void));
extern void	MergeWithTable ARGS_DECL((void));
extern void	ExportToTable ARGS_DECL((void));
extern void     ToggleShowWireSignalName ARGS_DECL((void));
extern int      RefreshPortMenu ARGS_DECL((TgMenu*));
extern TgMenu   *CreatePortMenu ARGS_DECL((TgMenu*, int X, int Y, TgMenuInfo*,
		                           int status_str_xlated));
extern void	RefreshSpecialMenu ARGS_DECL((TgMenu*));
extern int	SpecialMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern void	CleanUpSpecial ARGS_DECL((void));
extern int	InitSpecial ARGS_DECL((void));

#ifdef _INCLUDE_FROM_SPECIAL_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_SPECIAL_C_*/

#endif /*_SPECIAL_E_*/
