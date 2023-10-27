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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tcp.e,v 1.6 2011/05/16 16:21:59 william Exp $
 */

#ifndef _TGIF_TCP_E_
#define _TGIF_TCP_E_

#ifdef _INCLUDE_FROM_TCP_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_TCP_C_*/

extern void	SetSocketBlockingState ARGS_DECL((int *pn_socket, int));
extern void	TcpFreeBuf ARGS_DECL((char *buf));
extern int	TcpDoConnect ARGS_DECL((char *psz_host, int us_port,
		                        int *pn_socket));
extern int	TcpDoWrite ARGS_DECL((int n_socket, char *buf, int buf_sz));
extern int	TcpDoRead ARGS_DECL((int n_socket, char **ppsz_buf,
		                     int *pn_buf_sz));

#ifdef _INCLUDE_FROM_TCP_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_TCP_C_*/

#endif /*_TGIF_TCP_E_*/
