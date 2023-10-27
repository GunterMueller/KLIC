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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/group.e,v 1.7 2011/05/16 16:21:57 william Exp $
 */

#ifndef _GROUP_E_
#define _GROUP_E_

extern int	gnDeleteAttrsWhileUngrouping;

#ifdef _INCLUDE_FROM_GROUP_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_GROUP_C_*/

extern struct ObjRec	*JustCreateGroupObj ARGS_DECL((void));
extern void	CreateGroupObj ARGS_DECL((struct ObjRec *Top,
		                          struct ObjRec *Bot));
extern void	SaveGroupObj ARGS_DECL((FILE *, struct ObjRec *, int Level));
extern void	SaveCompObj ARGS_DECL((FILE *, struct ObjRec *, int Level));
extern void	SaveIconObj ARGS_DECL((FILE *, struct ObjRec *, int Level));
extern void	SavePinObj ARGS_DECL((FILE *, struct ObjRec *, int Level));
extern void	ReadGroupObj ARGS_DECL((FILE *, int Type, struct ObjRec **));
extern void	FreeGroupObj ARGS_DECL((struct ObjRec *));
extern void	UngroupObj ARGS_DECL((struct ObjRec *ObjPtr,
		                      struct SelRec **TopSelPtr,
		                      struct SelRec **BotSelPtr));
extern void	UngroupSelObj ARGS_DECL((int highlight, int record_cmd));
extern void	LockSelObj ARGS_DECL((void));
extern void	UnlockSelObj ARGS_DECL((void));

#ifdef _INCLUDE_FROM_GROUP_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_GROUP_C_*/

#endif /*_GROUP_E_*/
