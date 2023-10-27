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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/choice.e,v 1.6 2011/05/16 16:21:56 william Exp $
 */

#ifndef _CHOICE_E_
#define _CHOICE_E_

extern int	curChoice;
extern int	cycleThroughChoice;

extern int	setCurChoiceDontUpdateUI;

#ifdef _INCLUDE_FROM_CHOICE_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_CHOICE_C_*/

extern void	InitChoice ARGS_DECL((void));
extern void	CleanUpChoices ARGS_DECL((void));

extern void	ShowColor ARGS_DECL((int PropagateWhereToPrint));
extern void	ShowHoriAlign ARGS_DECL((void));
extern void	ShowVertAlign ARGS_DECL((void));
extern void	ShowJust ARGS_DECL((void));
extern void	ShowCurFont ARGS_DECL((void));
extern void	ShowTextVSpace ARGS_DECL((void));
extern void	ShowZoom ARGS_DECL((void));
extern void	ShowTextSize ARGS_DECL((void));
extern void	ShowRotate ARGS_DECL((void));
extern void	ShowLineWidth ARGS_DECL((void));
extern void	ShowLineStyle ARGS_DECL((void));
extern void	ShowLineType ARGS_DECL((void));
extern void	ShowDash ARGS_DECL((void));
extern void	ShowWhereToPrint ARGS_DECL((void));
extern void	ShowFile ARGS_DECL((void));
extern void	ShowRCBRadius ARGS_DECL((void));
extern void	ShowMoveMode ARGS_DECL((void));
extern void	ShowShape ARGS_DECL((void));
extern void	ShowStretchableTextMode ARGS_DECL((void));
extern void	ShowFill ARGS_DECL((void));
extern void	ShowTransPatMode ARGS_DECL((void));
extern void	ShowPen ARGS_DECL((void));
extern void	ShowPage ARGS_DECL((void));
extern void	ShowPageLayout ARGS_DECL((void));

extern void	ShowCurChoiceMouseStatus ARGS_DECL((int choice,
		                                    unsigned int state,
		                                    int cur_text_under_mouse));
extern void	SetCurChoice ARGS_DECL((int NewChoice));
extern void	PushCurChoice ARGS_DECL((void));
extern void	FormatFloat ARGS_DECL((float *pfval, char *buf));
extern void	FormatAngle ARGS_DECL((int DegreeTimes64, char *buf));
extern int	ChoiceEventHandler ARGS_DECL((XEvent *));
extern void	RedrawModeWindow ARGS_DECL((void));
extern int	ModeEventHandler ARGS_DECL((XEvent *));

#ifdef _INCLUDE_FROM_CHOICE_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_CHOICE_C_*/

#endif /*_CHOICE_E_*/
