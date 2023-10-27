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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/dup.e,v 1.6 2011/05/16 16:21:57 william Exp $
 */

#ifndef _DUP_E_
#define _DUP_E_

extern int	justDupped;
extern int	useRecentDupDistance;
extern int	dupDx;
extern int	dupDy;

#ifdef _INCLUDE_FROM_DUP_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_DUP_C_*/

extern void	CopyObjId ARGS_DECL((struct ObjRec *From, struct ObjRec *To));
extern void	CopyObjLocks ARGS_DECL((struct ObjRec *From,
		                        struct ObjRec *To));
extern void	UnlockAnObj ARGS_DECL((struct ObjRec *From));
extern void	DupObjXfrmMtrx ARGS_DECL((struct ObjRec *From,
		                          struct ObjRec *To));
extern void	DupObjBasics ARGS_DECL((struct ObjRec *From,
		                        struct ObjRec *To));
extern void	DupPolyObj ARGS_DECL((struct PolyRec *, struct ObjRec *));
extern void	DupPolygonObj ARGS_DECL((struct PolygonRec *, struct ObjRec *));
extern void	DupOvalObj ARGS_DECL((struct OvalRec *, struct ObjRec *));
extern void	DupBoxObj ARGS_DECL((struct BoxRec *, struct ObjRec *));
extern void	DupRCBoxObj ARGS_DECL((struct RCBoxRec *, struct ObjRec *));
extern void	DupArcObj ARGS_DECL((struct ArcRec *, struct ObjRec *));
extern void	DupXBmObj ARGS_DECL((struct XBmRec *, struct ObjRec *));
extern void	DupXPmObj ARGS_DECL((struct XPmRec *, struct ObjRec *));
extern void	DupStrSeg ARGS_DECL((StrBlockInfo*, StrSegInfo*));
extern void	DupStrBlock ARGS_DECL((StrBlockInfo*, MiniLineInfo*,
		                       StrBlockInfo**, StrBlockInfo**));
extern void	DupMiniLine ARGS_DECL((MiniLineInfo *pMiniLine,
		                       MiniLinesInfo *pOwnerMiniLines,
		                       StrBlockInfo *pOwnerBlock,
		                       MiniLineInfo **ppFirstMiniLine,
		                       MiniLineInfo **ppLastMiniLine));
extern void	DupMiniLines ARGS_DECL((MiniLinesInfo *minilines,
		                        StrBlockInfo *pOwnerBlock,
		                        MiniLinesInfo **ppMiniLines));
extern void	DupTextObj ARGS_DECL((struct TextRec *, struct ObjRec *From,
		                      struct ObjRec *To));
extern void	DupConnection ARGS_DECL((struct ConnRec *From,
		                         struct ConnRec *To));
extern void	DupGroupObj ARGS_DECL((struct GroupRec *, struct ObjRec *));
extern struct ObjRec	* DupObj ARGS_DECL((struct ObjRec *));
extern void	DupSelObj ARGS_DECL((void));
extern void	DupTheseObjects ARGS_DECL((struct SelRec *Top,
		                           struct SelRec *Bot,
		                           struct SelRec **NewTop,
		                           struct SelRec **NewBot));
extern void	JustDupSelObj ARGS_DECL((struct SelRec **NewTop,
		                         struct SelRec **NewBot));

#ifdef _INCLUDE_FROM_DUP_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_DUP_C_*/

#endif /*_DUP_E_*/
