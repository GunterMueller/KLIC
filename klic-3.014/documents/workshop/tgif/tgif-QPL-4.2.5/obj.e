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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/obj.e,v 1.6 2011/05/16 16:21:58 william Exp $
 */

#ifndef _OBJ_E_
#define _OBJ_E_

extern struct ObjRec	* topObj, * botObj;

#ifdef _INCLUDE_FROM_OBJ_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_OBJ_C_*/

extern void	AddObj ARGS_DECL((struct ObjRec *Prev, struct ObjRec *Next,
		                  struct ObjRec *));
extern void	UnlinkObj ARGS_DECL((struct ObjRec *));
extern void	ReplaceObj ARGS_DECL((struct ObjRec *old_obj,
		                      struct ObjRec *new_obj));
extern void	FreeObj ARGS_DECL((struct ObjRec *));
extern void	DelObj ARGS_DECL((struct ObjRec *));
extern void	DelAllObj ARGS_DECL((void));
extern void	AdjObjOBBox ARGS_DECL((struct ObjRec *));
extern void	AdjObjBBox ARGS_DECL((struct ObjRec *));
extern void	RecursivelyAdjObjBBox ARGS_DECL((struct ObjRec *sub_obj,
		                                 struct ObjRec *owner_obj,
		                                 struct ObjRec *top_owner));
extern void	AdjObjCache ARGS_DECL((struct ObjRec *));
extern void	AdjCaches ARGS_DECL((void));
extern void	AdjObjSplineVs ARGS_DECL((struct ObjRec *));
extern void	AdjSplineVs ARGS_DECL((void));
extern void	ClearObjCachesInAllPages ARGS_DECL((void));
extern void	MoveRotatedObjCache ARGS_DECL((struct ObjRec *, int, int));
extern void	AdjObjHotSpot ARGS_DECL((void));
extern void	AdjHotSpots ARGS_DECL((void));
extern int	IsTopLevelObject ARGS_DECL((struct ObjRec *));

extern struct ObjRec	*GetTopOwner ARGS_DECL((struct ObjRec *));

#ifdef _INCLUDE_FROM_OBJ_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_OBJ_C_*/

#endif /*_OBJ_E_*/
