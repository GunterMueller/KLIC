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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/http.e,v 1.8 2011/05/16 16:21:57 william Exp $
 */

#ifndef _TGIF_HTTP_E_
#define _TGIF_HTTP_E_

extern int	debugHttp;
extern int	gnHttpKeepAlive;
extern char	*gpszFakedReferer;

#ifdef _INCLUDE_FROM_HTTP_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_HTTP_C_*/

extern void	HttpFreeBuf ARGS_DECL((char *buf));
extern void	HttpDebug ARGS_DECL((int));
extern void	DoBase64Encode ARGS_DECL((char *buf_in, int buf_len,
		                          char *return_buf_in));
extern int	DoBase64Decode ARGS_DECL((char *buf_in, int buf_len,
		                          char *return_buf_in));
extern char	*Base64Encode ARGS_DECL((char*));
extern char	*Base64Decode ARGS_DECL((char*));
extern char	*FindAuthorization ARGS_DECL((char *pszHost, int nPort,
		                             char *pszScheme, char *pszRealm));
extern void	CommitAuthorization ARGS_DECL((void));
extern void	ResetAuthorization ARGS_DECL((void));
extern int	SetAuthorization ARGS_DECL((char *pszHost, int nPort,
		                            char *pszScheme, char *pszRealm,
		                            char *pszAuth));
extern void	HttpClearReferer ARGS_DECL((void));
extern void	HttpSetReferer ARGS_DECL((char*));
extern void	HttpFakeReferer ARGS_DECL((char*));
extern void	CleanUpHttp ARGS_DECL((void));
extern void	InitHttp ARGS_DECL((void));
extern char	*HttpHeaderGetVersion ARGS_DECL((void));
extern int	HttpHeaderGetResponseCode ARGS_DECL((void));
extern char	*HttpHeaderGetResponseStatus ARGS_DECL((void));
extern char	*HttpHeaderGetLastModified ARGS_DECL((void));
extern char	*HttpHeaderGetServer ARGS_DECL((void));
extern char	*HttpHeaderGetConnection ARGS_DECL((void));
extern char	*HttpHeaderGetLocation ARGS_DECL((void));
extern char	*HttpHeaderGetWWWAuthentication ARGS_DECL((void));
extern char	*HttpHeaderGetContentEncoding ARGS_DECL((void));
extern char	*HttpHeaderGetContentType ARGS_DECL((void));
extern long	HttpHeaderGetContentLength ARGS_DECL((void));
extern char	*HttpHeaderGetOtherField ARGS_DECL((char*));

extern int	HttpDoConnect ARGS_DECL((char *psz_host, int us_port,
		                         int *pn_socket));
extern int	HttpDoWrite ARGS_DECL((int n_socket, char *psz_path,
		                       char *psz_host, int us_port));
extern void	HttpDumpHeader ARGS_DECL((void));
extern char	*HttpExtractText ARGS_DECL((char *buf, int *pn_buf_sz,
		                            int *pn_html,
		                            char **ppsz_content_type));
extern int	HttpDoRead ARGS_DECL((int n_socket, char **ppsz_buf,
		                      int *pn_buf_sz));

#ifdef _INCLUDE_FROM_HTTP_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_HTTP_C_*/

#endif /*_TGIF_HTTP_E_*/
