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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/select.e,v 1.7 2011/05/16 16:21:59 william Exp $
 */

#ifndef _SELECT_E_
#define _SELECT_E_

extern int		selLtX, selLtY, selRbX, selRbY;
extern int		selObjLtX, selObjLtY, selObjRbX, selObjRbY;
extern int		selNoLockLtX, selNoLockLtY, selNoLockRbX, selNoLockRbY;
extern int		selNoLockObjLtX, selNoLockObjLtY;
extern int		selNoLockObjRbX, selNoLockObjRbY;
extern int		numObjSelected;
extern int		numObjLocked;
extern struct SelRec	* topSel, * botSel;
extern struct VSelRec	* topVSel, * botVSel;

#ifdef _INCLUDE_FROM_SELECT_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_SELECT_C_*/

extern int	GetObjCurved ARGS_DECL((struct ObjRec *));

extern struct SelRec	*SelectThisObject ARGS_DECL((struct ObjRec *));

extern int	CountSelectedVertices ARGS_DECL((void));
extern void	CalcBBox ARGS_DECL((int X1, int Y1, int X2, int Y2,
		                    int *LtX, int *LtY, int *RbX, int *RbY));
extern void	CalcVertexBBox ARGS_DECL((int *LtX, int *LtY, int *RbX,
		                          int *RbY));
extern void	UnSelNonVertexObjs ARGS_DECL((int HighLight,
		                              int NoLockedObjOnly));
extern void	FreeTopSel ARGS_DECL((void));
extern void	JustFreeSel ARGS_DECL((struct SelRec *pTopSel,
		                       struct SelRec *pBotSel));
extern void	UnlinkSel ARGS_DECL((struct SelRec *pSel,
		                     struct SelRec **ppTopSel,
		                     struct SelRec **ppBotSel));
extern struct SelRec	*FindObjInSel ARGS_DECL((struct ObjRec *pObj,
		                                 struct SelRec *pTopSel,
		                                 struct SelRec *pBotSel));
extern int	PrependObjToSel ARGS_DECL((struct ObjRec *pObj,
		                           struct SelRec **ppTopSel,
		                           struct SelRec **ppBotSel));
extern struct SelRec	*AddObjIntoSel ARGS_DECL((struct ObjRec *pObj,
			                          struct SelRec *pPrevSel,
			                          struct SelRec *pNextSel,
			                          struct SelRec **ppTopSel,
			                          struct SelRec **ppBotSel));
extern void	JustRemoveAllVSel ARGS_DECL((void));
extern void	RemoveAllSel ARGS_DECL((void));
extern struct AttrRec	* FindObjAttrWithName ARGS_DECL((struct ObjRec *ObjPtr,
			                                 char *AttrName));
extern struct ObjRec	* FindAnObj ARGS_DECL((int XOff, int YOff,
			                       struct ObjRec **OwnerObj,
			                       struct ObjRec **ConnectObj,
			                       char *ObjName));
extern struct SelRec	* AlreadySelected ARGS_DECL((struct ObjRec *));
extern void	AddSel ARGS_DECL((struct SelRec *Prev, struct SelRec *Next,
		                  struct SelRec *));
extern void	AddNewSelObj ARGS_DECL((struct ObjRec *));
extern void	ExpandCurSelBBoxes ARGS_DECL((struct ObjRec *));
extern void	UpdSelBBox ARGS_DECL((void));
extern void	SelBox ARGS_DECL((Window, GC, int X1, int Y1, int X2, int Y2));
extern void	Select ARGS_DECL((XEvent *));
extern struct AttrRec	* FindFileAttrWithName ARGS_DECL((char *AttrName));
extern struct AttrRec	* FindAttrWithName ARGS_DECL((struct ObjRec *,
			                              char *AttrName,
			                              struct ObjRec **));
extern struct AttrRec	* ValidAttrArg ARGS_DECL((char *CPtr, struct ObjRec *,
			                          char **NewCPtr));
extern int	DoTeleport ARGS_DECL((struct AttrRec *));
extern int	DoPageTeleport ARGS_DECL((struct AttrRec *, int ByPageName));
extern void	DoExecLoop ARGS_DECL((struct ObjRec *, struct AttrRec *));
extern void	Teleport ARGS_DECL((XButtonEvent *));
extern void	SelAllObj ARGS_DECL((int HighLight, int ignore_slideshow));
extern void	JustMoveSelToTop ARGS_DECL((void));
extern void	MoveSelToTop ARGS_DECL((void));
extern void	MoveSelToBot ARGS_DECL((void));
extern void	DelAllSelObj ARGS_DECL((void));
extern void	GroupSingleObj ARGS_DECL((int ForcePromoteAttrs));
extern void	GroupSelObj ARGS_DECL((int highlight, int record_cmd,
		                       int redraw));
extern void	SelectTopObj ARGS_DECL((void));

#ifdef _INCLUDE_FROM_SELECT_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_SELECT_C_*/

#endif /*_SELECT_E_*/
