/*
 * Some code in this file is derived from the public domain code in
 *              WWW/Library/Implementation/HTTCP.c distributed with lynx-2.2,
 *              whose original author is Tim Berners-lee <timbl@info.cern.ch>.
 *
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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tcp.c,v 1.6 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_TCP_C_

#include "tgifdefs.h"

#include "msg.e"
#include "remote.e"
#include "strtbl.e"
#include "tcp.e"
#include "util.e"

void SetSocketBlockingState(pn_socket, n_blocking)
   int *pn_socket, n_blocking;
{
   int rc;
#ifdef O_NONBLOCK
   int flags = fcntl(*pn_socket, F_GETFL);

   rc = fcntl(*pn_socket, F_SETFL,
            n_blocking ? (flags & (~O_NONBLOCK)) : (flags | O_NONBLOCK));
#else /* ~O_NONBLOCK */
   int val=(!n_blocking);

   rc = ioctl(*pn_socket, FIONBIO, &val);
#endif /* O_NONBLOCK */
   if (rc == (-1)) {
      fprintf(stderr, "%s\n",
            TgLoadString(n_blocking ? STID_FAIL_TO_MAKE_SOCKET_BLOCK :
            STID_FAIL_TO_MAKE_SOCKET_NON_BLOCK));
   }
}

void TcpFreeBuf(buf)
   char *buf;
{
   free(buf);
}

static int gnPipeBroken=FALSE;

static
void BrokenPipe(nSig)
   int nSig;
{
   if (nSig == SIGPIPE) {
      gnPipeBroken = TRUE;
      signal(SIGPIPE, SIG_DFL);
   }
}

int TcpDoConnect(psz_host, us_port, pn_socket)
   char *psz_host;
   int us_port, *pn_socket;
{
   static int not_initialized=TRUE;
   struct sockaddr_in soc_address;
   struct sockaddr_in *sin=(&soc_address);
   struct hostent *p_hostent=NULL;
   int status=TG_REMOTE_STATUS_OK;

   if (not_initialized) {
      not_initialized = FALSE;
      signal(SIGPIPE, BrokenPipe);
   }
   if (*psz_host >= '0' && *psz_host <= '9') {
      sin->sin_addr.s_addr = inet_addr(psz_host);
   } else {
      p_hostent = gethostbyname(psz_host);
      if (p_hostent == NULL) {
         return TG_REMOTE_STATUS_HOST;
      }
      memcpy(&sin->sin_addr, p_hostent->h_addr, p_hostent->h_length);
   }
   sin->sin_family = AF_INET;
   sin->sin_port = htons((unsigned short)us_port);
   *pn_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

   SetSocketBlockingState(pn_socket, FALSE);

   status = connect(*pn_socket, (struct sockaddr*)&soc_address,
         sizeof(soc_address));
#ifdef EAGAIN
   if ((status < 0) && (errno==EINPROGRESS || errno==EAGAIN)) {
#else /* ~EAGAIN */
   if ((status < 0) && (errno == EINPROGRESS)) {
#endif /* EAGAIN */
      struct timeval timeout;
      int rc=0;

      timeout.tv_sec = 0;
      timeout.tv_usec = 100000;
      while (rc <= 0) {
         fd_set writefds;

         FD_ZERO(&writefds);
         FD_SET(*pn_socket, &writefds);
#ifdef __hpux
         rc = select(FD_SETSIZE, NULL, (int*)&writefds, NULL, &timeout);
#else /* ~__hpux */
         rc = select(FD_SETSIZE, NULL, &writefds, NULL, &timeout);
#endif /* __hpux */
         if ((rc < 0)&&(errno != EALREADY)) {
            status = rc;
            break;
         } else if (rc > 0) {
            gnPipeBroken = FALSE;
            status = connect(*pn_socket, (struct sockaddr*)&soc_address,
                  sizeof(soc_address));
            if (gnPipeBroken) {
               fprintf(stderr, TgLoadString(STID_BROKEN_PIPE_CONTACT_HOST),
                     psz_host);
               fprintf(stderr, "\n");
            }
            if ((status < 0)&&(errno == EISCONN)) status = TG_REMOTE_STATUS_OK;
            if (errno == EALREADY) {
               rc = 0;
            } else {
               break;
            }
         } else {
            status = connect(*pn_socket, (struct sockaddr*)&soc_address,
                  sizeof(soc_address));
#ifdef EAGAIN
            if ((status < 0) && (errno != EALREADY) && (errno != EISCONN) &&
                  (errno != EAGAIN))
#else /* ~EAGAIN */
            if ((status < 0) && (errno != EALREADY) && (errno != EISCONN))
#endif /* EAGAIN */
               break;
         }
         if (UserAbortComm()) {
            status = TG_REMOTE_STATUS_INTR;
            errno = EINTR;
            break;
         }
      }
   }
   if (status >= 0) {
      SetSocketBlockingState(pn_socket, TRUE);
   } else {
      close(*pn_socket);
   }
   return status;
}

int TcpDoWrite(n_socket, buf, buf_sz)
   int n_socket, buf_sz;
   char *buf;
{
   int status=0;

   if (buf == NULL) return TG_REMOTE_STATUS_OK;

   status = write(n_socket, buf, (int)buf_sz);
   if (status <= 0) {
      if (status == 0) {
         fprintf(stderr, "%s\n", TgLoadString(STID_WRITE_TO_SOCKET_FAILED));
      } else if ((errno == ENOTCONN || errno == ECONNRESET || errno == EPIPE)) {
         fprintf(stderr, "%s\n",
               TgLoadString(STID_UNEXP_NETWORK_ERR_WRITE_SOCK));
         return TG_REMOTE_STATUS_WRITE;
      }
   }
   return TG_REMOTE_STATUS_OK;
}

#define MIN_READ_SIZE 0x100

int TcpDoRead(n_socket, ppsz_buf, pn_buf_sz)
   int n_socket, *pn_buf_sz;
   char **ppsz_buf;
{
   int buf_sz=0x400, len=0, end_of_file=FALSE;
   char *buf=(char*)malloc(buf_sz*sizeof(char));

   if (pn_buf_sz != NULL) *pn_buf_sz = 0;
   *ppsz_buf = NULL;
   if (buf == NULL) {
      FailAllocMessage();
      return TG_REMOTE_STATUS_MEM;
   }
   do {
      int bytes_read;

      if (buf_sz - len < MIN_READ_SIZE) {
         buf_sz += 0x400;
         if ((buf=(char*)realloc(buf, buf_sz)) == NULL) {
            FailAllocMessage();
            return TG_REMOTE_STATUS_MEM;
         }
      }
      bytes_read = read(n_socket, &buf[len], buf_sz-len-1);
      if (bytes_read <= 0) {
         if (bytes_read < 0 && (errno == ENOTCONN || errno == ECONNRESET ||
               errno == EPIPE)) {
            free(buf);
            fprintf(stderr, "%s\n",
                  TgLoadString(STID_READ_FROM_SOCKET_FAILED));
            return TG_REMOTE_STATUS_READ;
         } else if (bytes_read < 0) {
            free(buf);
            fprintf(stderr, "%s\n",
                  TgLoadString(STID_UNEXP_NETWORK_ERR_READ_SOCK));
            return TG_REMOTE_STATUS_NET;
         }
         end_of_file = TRUE;
      } else {
         len += bytes_read;
      }
   } while (!end_of_file);
   buf[len] = '\0';
   *ppsz_buf = buf;
   if (pn_buf_sz != NULL) *pn_buf_sz = len;
   return TG_REMOTE_STATUS_OK;
}

