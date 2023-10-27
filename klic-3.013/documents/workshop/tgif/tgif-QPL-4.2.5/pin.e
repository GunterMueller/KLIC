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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/pin.e,v 1.6 2011/05/16 16:21:58 william Exp $
 */

#ifndef _PIN_E_
#define _PIN_E_

extern int	drawPolyToConnectPins;

extern struct ObjRec *gpStartPin, *gpEndPin;

#ifdef _INCLUDE_FROM_PIN_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_PIN_C_*/

extern int	FindExtraPoly ARGS_DECL((struct SelRec *pTopSel,
		                   struct SelRec *pBotSel,
		                   struct SelRec **ppTopSel,
		                   struct SelRec **ppBotSel, int *pnCount,
		                   struct BBRec *pBBox));
extern void	HighLightAPin ARGS_DECL((int nForward));
extern void	HandlePinHighlights ARGS_DECL((int MouseX, int MouseY));
extern void	ConnectPins ARGS_DECL((void));
extern void	FreeConn ARGS_DECL((struct ConnRec *pConn));
extern void	DelConnFromPoly ARGS_DECL((struct ConnRec *pConn));
extern void	DelConnFromPin ARGS_DECL((struct ConnRec *pConn));
extern void	CleanUpPins ARGS_DECL((void));
extern int	InitPins ARGS_DECL((void));

#ifdef _INCLUDE_FROM_PIN_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_PIN_C_*/

#endif /*_PIN_E_*/
