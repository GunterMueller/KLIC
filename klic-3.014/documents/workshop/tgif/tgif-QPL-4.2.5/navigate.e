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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/navigate.e,v 1.7 2011/05/16 16:21:58 william Exp $
 */

#ifndef _NAVIGATE_E_
#define _NAVIGATE_E_

extern int	navigatingBackAndForth;
extern int	inHyperSpace;
extern int	inSlideShow;
extern int	visibleGridInSlideShow;
extern int	slideShowInfoValid;
extern char	* slideShowBorderColor;
extern int	slideShowXOffset;
extern int	slideShowYOffset;
extern int	autoHyperSpaceOnRemote;
extern int	allowLaunchInHyperSpace;
extern int	navigateRefresh;
extern int	goHyperSpaceInSlideShow;

#ifdef _INCLUDE_FROM_NAVIGATE_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_NAVIGATE_C_*/

extern int	ParseXYSpec ARGS_DECL((char *spec, int *pnx, int *pny));
extern int	ParseWHSpec ARGS_DECL((char *spec, int *pnw, int *pnh));
extern void	CleanUpSlideShowInfo ARGS_DECL((void));
extern void	CleanUpNavigate ARGS_DECL((void));
extern void	InitNavigate ARGS_DECL((void));

extern void	UpdateLRU ARGS_DECL((struct URLCacheRec *url_cache));
extern struct URLCacheRec	*FindURLCache ARGS_DECL((char *psz_url,
				                         int update_lru));
extern void	UpdateURLCache ARGS_DECL((char *psz_url, char *psz_remote_buf,
		                          char *psz_content_type,
		                          int remote_buf_sz, int is_html));

extern void	BeforeNavigate ARGS_DECL((void));
extern void	CommitNavigate ARGS_DECL((void));

extern void	NavigateBack ARGS_DECL((void));
extern void	NavigateForward ARGS_DECL((void));
extern void	AdjustNavigate ARGS_DECL((void));
extern void	NavigateRefresh ARGS_DECL((void));
extern void	NavigateHotList ARGS_DECL((void));
extern void	NavigateAddToHotList ARGS_DECL((void));
extern void	NavigateSessionHistory ARGS_DECL((void));
extern void	ToggleHyperSpace ARGS_DECL((int KeepSelected));
extern void	NextSlide ARGS_DECL((void));
extern void	PrevSlide ARGS_DECL((void));
extern void	EnterSlideShow ARGS_DECL((void));
extern void	LeaveSlideShow ARGS_DECL((void));
extern void	SetSlideShowBorderColor ARGS_DECL((void));
extern void	SetSlideShowWindowOffsets ARGS_DECL((void));
extern void	ToggleVisibleGridInSlideShow ARGS_DECL((void));
extern void	GoHyperSpaceInSlideShow ARGS_DECL((void));
extern void	FreehandModeInSlideShow ARGS_DECL((void));
extern int	RefreshSlideShowModeMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateSlideShowModeMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                    TgMenuInfo*,
		                                    int status_str_xlated));
extern int	SlideShowModeMenu ARGS_DECL((int X, int Y, int TrackMenubar));
extern int	RefreshSlideShowMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateSlideShowMenu ARGS_DECL((TgMenu*, int X, int Y,
		                                TgMenuInfo*,
		                                int status_str_xlated));
extern int	ReadSlideShowInfo ARGS_DECL((char*));

extern void	RefreshNavigateMenu ARGS_DECL((TgMenu*));
extern TgMenu	*CreateNavigateMenu ARGS_DECL((TgMenu*, int X, int Y,
		                               TgMenuInfo*,
		                               int status_str_xlated));
extern int	NavigateMenu ARGS_DECL((int X, int Y, int TrackMenubar));

#ifdef _INCLUDE_FROM_NAVIGATE_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_NAVIGATE_C_*/

#endif /*_NAVIGATE_E_*/
