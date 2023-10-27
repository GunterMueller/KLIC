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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/chat.e,v 1.6 2011/05/16 16:21:56 william Exp $
 */

#ifndef _CHAT_E_
#define _CHAT_E_

#include "tidget.h"

extern char	*cmdLineChatNickName;

#ifdef _INCLUDE_FROM_CHAT_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_CHAT_C_*/

extern void	RedrawChatWindow ARGS_DECL((void));
extern void	MoveResizeChatWindow ARGS_DECL((int x, int y, int w, int h));
extern void	ChatEventHandler ARGS_DECL((XEvent*, TidgetInfo*));
extern int	IsChatWindowEvent ARGS_DECL((XEvent*, TidgetInfo**));

extern int	GetInitialChatWindowHeight ARGS_DECL((void));
extern void	SetChatWindowGeom ARGS_DECL((void));
extern void	MapChatSubWindows ARGS_DECL((void));
extern int	CreateChatSubWindows ARGS_DECL((void));

extern void	ChatAppendChatLine ARGS_DECL((TextFormatInfo*, char *nick_name,
				char *encoding, char *buf));
extern void	ChatAddUser ARGS_DECL((char*));

extern int	InitChat ARGS_DECL((void));
extern void	CleanUpChat ARGS_DECL((void));

#ifdef _INCLUDE_FROM_CHAT_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_CHAT_C_*/

#endif /*_CHAT_E_*/
