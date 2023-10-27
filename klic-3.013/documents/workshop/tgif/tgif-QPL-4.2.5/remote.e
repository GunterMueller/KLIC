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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/remote.e,v 1.6 2011/05/16 16:21:59 william Exp $
 */

#ifndef _TGIF_REMOTE_E_
#define _TGIF_REMOTE_E_

extern int	postingCGIQuery;
extern int	gettingHttpHeaderOnly;
extern char	*fnameForPostingCGIQuery;
extern char	gszLocalPID[];
extern char	gzipCmd[];
extern char	gunzipCmd[];

#ifdef _INCLUDE_FROM_REMOTE_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_REMOTE_C_*/

extern void	CleanUpDownloadStats ARGS_DECL((void));
extern int	UpdateDownloadStats ARGS_DECL((int bytes, char *psz_buf));

extern int	GetClientID ARGS_DECL((char *psz_buf, int buf_sz));
extern void	GetUserAgent ARGS_DECL((char *buf, int buf_sz));
extern void	SetUserAgent ARGS_DECL((char*));
extern int	GetUserID ARGS_DECL((char *psz_buf, int buf_sz));
extern int	UserAbortComm ARGS_DECL((void));
extern int	OtherAbortComm ARGS_DECL((void));
extern int	GetPageNumFromPageSpec ARGS_DECL((char *psz_spec,
		                                  int *pn_page_num));
extern void	FreeRemoteBuf ARGS_DECL((char*));
extern int	DirIsRemote ARGS_DECL((char*));
extern int	FileIsRemote ARGS_DECL((char*));
extern int	UrlIsHtml ARGS_DECL((char*));
extern int	FormRemoteName ARGS_DECL((char *psz_file, char *psz_def_ext,
		                          char *psz_return));
extern int	FormNewFileName ARGS_DECL((char *psz_dir, char *psz_file,
		                           char *psz_def_ext, char *psz_return,
		                           char **ppsz_page_spec));
extern void	ShowRemoteStatus ARGS_DECL((char*));
extern int	LoadRemoteFileInMem ARGS_DECL((char *url, char **ppsz_buf,
		                               char **ppsz_content_type,
		                               int *pn_buf_sz, int *pn_html,
		                               int force_load, char *psz_final,
		                               int cb_final));
extern char	*WriteRemoteFileIntoTemp ARGS_DECL((char *psz_buf, int buf_sz,
		                                    char *psz_ext));
extern int	UseExternalViewer ARGS_DECL((int is_html, char *psz_url,
		                             char *psz_content_type,
		                             char *tmp_fname));
extern int	LoadRemoteFileFromMem ARGS_DECL((char *psz_url, char *psz_buf,
		                                 char *psz_content_type,
		                                 int buf_sz, int is_html));
extern void	InitLocalPID ARGS_DECL((void));
extern void	InitRemote ARGS_DECL((void));
extern void	CleanUpRemote ARGS_DECL((void));

#ifdef _INCLUDE_FROM_REMOTE_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_REMOTE_C_*/

#endif /*_TGIF_REMOTE_E_*/
