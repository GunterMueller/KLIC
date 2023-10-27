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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/cursor.e,v 1.6 2011/05/16 16:21:57 william Exp $
 */

#ifndef _CURSOR_E_
#define _CURSOR_E_

extern Cursor	nullCursor;
extern Cursor	cornerCursor;
extern Cursor	handCursor;
extern Cursor	defaultCursor;
extern Cursor	watchCursor;
extern Cursor	drawCursor;
extern Cursor	freehandCursor;
extern Cursor	vertexCursor;
extern Cursor	rotateCursor;
extern Cursor	rotatingCursor;
extern Cursor	horiShearCursor;
extern Cursor	vertShearCursor;
extern Cursor	moveCursor;
extern Cursor	textCursor;
extern Cursor	hyperSpaceCursor;
extern Cursor	magCursor;
extern Cursor	floodCursor;
extern Cursor	dripCursor;

extern int	watchCursorOnMainWindow;

#ifdef _INCLUDE_FROM_CURSOR_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_CURSOR_C_*/

extern void	SetTextCursor ARGS_DECL((Window));
extern void	SetNullCursor ARGS_DECL((Window));
extern void	SetWatchCursor ARGS_DECL((Window));
extern void	SetDrawCursor ARGS_DECL((Window));
extern void	SetFreeHandCursor ARGS_DECL((Window));
extern void	SetVertexCursor ARGS_DECL((Window));
extern void	SetRotateCursor ARGS_DECL((Window));
extern void	SetRotatingCursor ARGS_DECL((Window));
extern void	SetHoriShearCursor ARGS_DECL((Window));
extern void	SetVertShearCursor ARGS_DECL((Window));
extern void	SetMoveCursor ARGS_DECL((Window));
extern void	SetHyperSpaceCursor ARGS_DECL((Window));
extern void	SetFloodFillCursor ARGS_DECL((Window));
extern void	SetDripCursor ARGS_DECL((Window));
extern void	SetHandCursor ARGS_DECL((Window));
extern void	SetDefaultCursor ARGS_DECL((Window));
extern void	ShowCursor ARGS_DECL((void));
extern void	CreateCursor ARGS_DECL((void));
extern void	PutCursor ARGS_DECL((Window, int X, int Y, int Foreground));
extern void	CleanUpCursors ARGS_DECL((void));
extern Cursor	NewFontCursor ARGS_DECL((char*));
extern void	SetWindowCursor ARGS_DECL((Window, Cursor));
extern void	DeleteFontCursor ARGS_DECL((Cursor));

#ifdef _INCLUDE_FROM_CURSOR_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_CURSOR_C_*/

#endif /*_CURSOR_E_*/
