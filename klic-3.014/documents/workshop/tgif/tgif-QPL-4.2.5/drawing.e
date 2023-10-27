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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/drawing.e,v 1.11 2011/05/16 16:21:57 william Exp $
 */

#ifndef _DRAWING_E_
#define _DRAWING_E_

extern int	disableRedraw;
extern int	intrCheckInterval;
extern int	pasteInDrawTextMode;
extern int	pasteFromFileInDrawTextMode;
extern int	pasteCompoundTextInDrawTextMode;
extern char	pasteFromFileName[];
extern int	copyInDrawTextMode;
extern int	copyDoubleByteStringInDrawTextMode;
extern int	numRedrawBBox;
extern int	numClipRecs;
extern int	clipOrdering;
extern XRectangle	clipRecs[];
extern int	checkBBox;

extern int	btn1Warp;

extern int	userDisableRedraw;
extern int	executingCommands;
extern int	escPressedWhileExecutingCommands;
extern int	gnDisableShortcuts; /* used by ExecCmdsFromFile() */
extern int	enableMouseWheel;
extern int	btn2PopupMainMenu;

#ifdef _INCLUDE_FROM_DRAWING_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_DRAWING_C_*/

extern void	SetXorDrawGC ARGS_DECL((int xor_pixel));
extern void	SetDefaultDrawWinClipRecs ARGS_DECL((void));
extern void	SetDefaultIconWinClipRecs ARGS_DECL((void));
extern void	ShowInterrupt ARGS_DECL((int));
extern int	HideInterrupt ARGS_DECL((void));
extern void	RedrawDummyWindow1 ARGS_DECL((void));
extern void	RedrawDummyWindow2 ARGS_DECL((void));
extern void	DummiesEventHandler ARGS_DECL((XEvent*));
extern int	KeyPressEventIsEscape ARGS_DECL((XKeyEvent*));
extern int	ESCPressed ARGS_DECL((void));
extern int	CheckInterrupt ARGS_DECL((int check_esc));
extern void	DrawClippedPixmap ARGS_DECL((Pixmap, Window, GC, int PixmapW,
		                             int PixmapH, int LtX, int LtY));
extern void	FillClippedRectangle ARGS_DECL((Window, GC, int LtX, int LtY,
		                                int OrigW, int OrigH));
extern int	ObjInVisibleLayer ARGS_DECL((struct ObjRec *));
extern int	DrawObj ARGS_DECL((Window, struct ObjRec *));
extern void	DrawPaperBoundary ARGS_DECL((Window));
extern void	RedrawAnArea ARGS_DECL((struct ObjRec *BotObj, int LtX,
		                        int LtY, int RbX, int RbY));
extern void	RedrawAreas ARGS_DECL((struct ObjRec *BotObj, int LtX1,
		                        int LtY1, int RbX1, int RbY1,
		                        int LtX2, int LtY2, int RbX2,
		                        int RbY2));
extern int	RedrawDuringScrolling ARGS_DECL((void));
extern void	RedrawAreaFromCache ARGS_DECL((double start_frac,
		                               int scroll_all_the_way));
extern void	RedrawDrawWindow ARGS_DECL((struct ObjRec *BotObj));
extern Pixmap	DrawAllOnPixmap ARGS_DECL((int *LtX, int *LtY, int *W, int *H,
		                           int nRedraw));
extern void	ClearAndRedrawDrawWindow ARGS_DECL((void));
extern void	ClearAndRedrawDrawWindowNoCurT ARGS_DECL((void));
                /* use to be ClearAndRedrawDrawWindowDontDrawCurText */
extern int	BeginExecAnimate ARGS_DECL((void));
extern void	EndExecAnimate ARGS_DECL((void));
extern void	CleanUpDrawingWindow ARGS_DECL((void));
extern int	ShortHand ARGS_DECL((XEvent *));
extern int	ExecuteCmdById ARGS_DECL((int nCmdId, int nIndex));
extern int	CallShortCut ARGS_DECL((char *Name, int argc, char *argv[],
		                        char *Code, unsigned int State));
extern int	SomethingDirty ARGS_DECL((void));
extern void	EditIndexedAttrInEditor ARGS_DECL((int index));
extern void	EditIndexedAttrGroupInEditor ARGS_DECL((int index));
extern TgMenu	*CreateEditAttrInEditorMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                       TgMenuInfo*,
		                                       int status_str_xlated));
extern TgMenu	*CreateEditAttrGroupInEditorMenu ARGS_DECL((TgMenu*, int X,
				int Y, TgMenuInfo*, int status_str_xlated));
extern void	GetProperty ARGS_DECL((int index));
extern TgMenu	*CreateGetPropertyMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                  TgMenuInfo*,
		                                  int status_str_xlated));
extern void	PeekDimension ARGS_DECL((int index));
extern TgMenu	*CreatePeekDimensionMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                    TgMenuInfo*,
		                                    int status_str_xlated));
extern int	RefreshContextMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateContextMenu ARGS_DECL((TgMenu*, int X, int Y,
		                              TgMenuInfo*,
		                              int status_str_xlated));
extern int	RefreshEditTextContextMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateEditTextContextMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                      TgMenuInfo*,
		                                      int status_str_xlated));
extern int	ContextMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern void	HandleMotionForPortInDrawWindow ARGS_DECL((int mouse_x,
		                                           int mouse_y));
extern int	HandlePressForPortInDrawWindow ARGS_DECL((int cancel));
extern int	DrawingEventHandler ARGS_DECL((XEvent *));

#ifdef _INCLUDE_FROM_DRAWING_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_DRAWING_C_*/

#endif /*_DRAWING_E_*/
