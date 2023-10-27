/*
 * Some code in this file is derived from the public domain code in
 *              WWW/Library/Implementation/HTTP.c distributed with lynx-2.2,
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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/http.c,v 1.11 2011/05/16 16:21:57 william Exp $
 */

#define _INCLUDE_FROM_HTTP_C_

#include "tgifdefs.h"

#include "dialog.e"
#include "drawing.e"
#include "file.e"
#include "http.e"
#include "mainloop.e"
#include "msg.e"
#include "remote.e"
#include "setup.e"
#include "stream.e"
#include "strtbl.e"
#include "tcp.e"
#include "util.e"
#include "version.e"

int debugHttp=0;
int gnHttpKeepAlive=FALSE;
char *gpszFakedReferer=NULL;

typedef struct TgifHttpLineInfo {
   char *name;
   char *value;
   struct TgifHttpLineInfo *next;
} *TgifHttpLinePtr;

typedef struct TgifHttpHeaderInfo {
   char *version;
   int resp_code;
   char *resp_status;
   char *last_modified;
   char *server;
   char *connection;
   char *location;
   char *www_authenticate;
   char *content_encoding;
   char *content_type;
   long content_length;
   struct TgifHttpLineInfo *misc;
} *TgifHttpHeaderPtr;

typedef struct AuthInfo {
   char *host;
   int port;
   char *scheme; /* e.g., Basic */
   char *realm; /* e.g., WallyWorld */
   char *authorization; /* base64 encoded */
   struct AuthInfo *next, *prev;
} *AuthInfoPtr;

static struct AuthInfo *topAuthInfo=NULL, *botAuthInfo=NULL;
static struct AuthInfo curAuthorization;

static struct TgifHttpHeaderInfo tgifHttpHeaderInfo;

static char SZ_HTTP_VERSION[]="HTTP/1.0";
static char SZ_USER_AGENT[128];
static char SZ_USER_NAME[128];

typedef struct tagRefererInfo {
   char *referer;
   struct tagRefererInfo *next;
} RefererInfo;

static RefererInfo *topReferer=NULL;

void HttpFreeBuf(buf)
   char *buf;
{
   free(buf);
}

void HttpDebug(val)
   int val;
{
   debugHttp = val;
}

static
void CleanUpAuthInfo()
{
   struct AuthInfo *next_pai;

   for ( ; topAuthInfo != NULL; topAuthInfo=next_pai) {
      next_pai = topAuthInfo->next;
      if (topAuthInfo->host != NULL) free(topAuthInfo->host);
      if (topAuthInfo->scheme != NULL) free(topAuthInfo->scheme);
      if (topAuthInfo->realm != NULL) free(topAuthInfo->realm);
      if (topAuthInfo->authorization != NULL) free(topAuthInfo->authorization);
      free(topAuthInfo);
   }
   botAuthInfo = NULL;
}

/* do not translate -- program constants */
static char gszEncode[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static int gnDecode[] = {
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
   -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
   -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};

void DoBase64Encode(buf_in, buf_len, return_buf_in)
   char *buf_in, *return_buf_in;
   int buf_len;
{
   unsigned char *buf=(unsigned char *)buf_in;
   unsigned char *return_buf=(unsigned char *)return_buf_in;

   while (buf_len > 0) {
      switch (buf_len) {
      case 1:
         return_buf[0] = gszEncode[(int)((buf[0]>>2) & 0x3f)];
         return_buf[1] = gszEncode[(int)((buf[0]<<4) & 0x3f)];
         return_buf[2] = '=';
         return_buf[3] = '=';
         return_buf[4] = '\0';
         buf++;
         buf_len--;
         return;
      case 2:
         return_buf[0] = gszEncode[(int)((buf[0]>>2) & 0x3f)];
         return_buf[1] = gszEncode[(int)(((buf[0]<<4) | (buf[1]>>4)) & 0x3f)];
         return_buf[2] = gszEncode[(int)((buf[1]<<2) & 0x3f)];
         return_buf[3] = '=';
         return_buf[4] = '\0';
         buf += 2;
         buf_len -= 2;
         return;
      default: /* convert 3 bytes */
         return_buf[0] = gszEncode[(int)((buf[0]>>2) & 0x3f)];
         return_buf[1] = gszEncode[(int)(((buf[0]<<4) | (buf[1]>>4)) & 0x3f)];
         return_buf[2] = gszEncode[(int)(((buf[1]<<2) | (buf[2]>>6)) & 0x3f)];
         return_buf[3] = gszEncode[(int)(buf[2] & 0x3f)];
         return_buf[4] = '\0';
         buf += 3;
         buf_len -= 3;
         break;
      }
      return_buf += 4;
   }
}

char *Base64Encode(buf)
   char *buf;
{
   int len=strlen(buf), new_len;
   char *return_buf;

   new_len = (int)(len/3);
   new_len += 3;
   new_len <<= 2;
   return_buf = (char*)malloc(new_len*sizeof(char));
   if (return_buf == NULL) return NULL;
   DoBase64Encode(buf, len, return_buf);
   return return_buf;
}

static
int Convert(buf4, count, buf3)
   int *buf4, count;
   char *buf3;
{
   switch (count) {
   case 2:
      buf3[0] = ((buf4[0]<<2) & 0xfc) | ((buf4[1]>>4) & 0x03);
      buf3[1] = '\0';
      return 1;
   case 3:
      buf3[0] = ((buf4[0]<<2) & 0xfc) | ((buf4[1]>>4) & 0x03);
      buf3[1] = ((buf4[1]<<4) & 0xf0) | ((buf4[2]>>2) & 0x0f);
      buf3[2] = '\0';
      return 2;
   case 4:
      buf3[0] = (char)(((buf4[0]<<2) & 0xfc) | ((buf4[1]>>4) & 0x03));
      buf3[1] = (char)(((buf4[1]<<4) & 0xf0) | ((buf4[2]>>2) & 0x0f));
      buf3[2] = (char)(((buf4[2]<<6) & 0xc0) | (buf4[3] & 0x3f));
      buf3[3] = '\0';
      return 3;
   default:
#ifdef _TGIF_DBG /* debug, do not translate */
      Error("Convert()", "Bad count passed to Convert().");
#endif /* _TGIF_DBG */
      break;
   }
   return 0;
}

int DoBase64Decode(buf, buf_sz, return_buf)
   char *buf, *return_buf;
   int buf_sz;
{
   int i=0, j=0, bytes_decoded=0, intbuf[5];

   intbuf[4] = (-1);
   for (i=0, j=0; i < buf_sz; i++) {
      intbuf[j++] = gnDecode[(int)buf[i]];
      if (j == 4) {
         char out_buf[4];
         int k=0, bytes=0;

         if (intbuf[3] == (-1)) {
            if (intbuf[2] == (-1)) {
               bytes = Convert(intbuf, 2, out_buf);
            } else {
               bytes = Convert(intbuf, 3, out_buf);
            }
         } else {
            bytes = Convert(intbuf, 4, out_buf);
         }
         for (k=0; k < bytes; k++) {
            *return_buf++ = (char)(out_buf[k] & 0x0ff);
         }
         j = 0;
         bytes_decoded += bytes;
      }
   }
   *return_buf = '\0';
   return bytes_decoded;
}

char *Base64Decode(buf)
   char *buf;
{
   int len=strlen(buf);
   char *return_buf=(char*)malloc(len*sizeof(char));

   if (return_buf == NULL) {
      FailAllocMessage();
      return NULL;
   }
   (void)DoBase64Decode(buf, len, return_buf);

   return return_buf;
}

static
void BubbleAuthInfoToTop(pai)
   struct AuthInfo *pai;
{
   if (pai == topAuthInfo) return;
   if (pai->next != NULL) {
      pai->next->prev = pai->prev;
   } else {
      botAuthInfo = pai->prev;
   }
   pai->prev->next = pai->next;
   pai->prev = NULL;
   pai->next = topAuthInfo;
   topAuthInfo = pai;
}

char *FindAuthorization(pszHost, nPort, pszScheme, pszRealm)
   char *pszHost, *pszScheme, *pszRealm;
   int nPort;
{
   struct AuthInfo *pai;

   for (pai=topAuthInfo; pai != NULL; pai=pai->next) {
      if (pai->port == nPort &&
            pai->host != NULL && UtilStrICmp(pai->host, pszHost) == 0 &&
            pai->scheme != NULL && UtilStrICmp(pai->scheme, pszScheme) == 0 &&
            pai->realm != NULL && UtilStrICmp(pai->realm, pszRealm) == 0) {
         BubbleAuthInfoToTop(pai);
         return pai->authorization;
      }
   }
   return NULL;
}

void CommitAuthorization()
{
   struct AuthInfo *pai=(struct AuthInfo *)malloc(sizeof(struct AuthInfo));

   if (pai == NULL) {
      FailAllocMessage();
      return;
   }
   memset(pai, 0, sizeof(struct AuthInfo));
   if (curAuthorization.host != NULL) {
      pai->host = UtilStrDup(curAuthorization.host);
   }
   if (curAuthorization.scheme != NULL) {
      pai->scheme = UtilStrDup(curAuthorization.scheme);
   }
   if (curAuthorization.realm != NULL) {
      pai->realm = UtilStrDup(curAuthorization.realm);
   }
   if (curAuthorization.authorization != NULL) {
      pai->authorization = UtilStrDup(curAuthorization.authorization);
   }
   pai->port = curAuthorization.port;
   if (topAuthInfo != NULL) {
      topAuthInfo->prev = pai;
   } else {
      botAuthInfo = pai;
   }
   pai->prev = NULL;
   pai->next = topAuthInfo;
   topAuthInfo = pai;
}

void ResetAuthorization()
{
   if (curAuthorization.host != NULL) free(curAuthorization.host);
   if (curAuthorization.scheme != NULL) free(curAuthorization.scheme);
   if (curAuthorization.realm != NULL) free(curAuthorization.realm);
   if (curAuthorization.authorization != NULL) {
      free(curAuthorization.authorization);
   }
   memset(&curAuthorization, 0, sizeof(struct AuthInfo));
}

int SetAuthorization(pszHost, nPort, pszScheme, pszRealm, pszEncodedAuth)
   char *pszHost, *pszScheme, *pszRealm, *pszEncodedAuth;
   int nPort;
{
   ResetAuthorization();
   curAuthorization.host = UtilStrDup(pszHost);
   curAuthorization.scheme = UtilStrDup(pszScheme);
   curAuthorization.realm = UtilStrDup(pszRealm);
   curAuthorization.authorization = UtilStrDup(pszEncodedAuth);
   curAuthorization.port = nPort;
   if (curAuthorization.host==NULL || curAuthorization.scheme==NULL ||
         curAuthorization.realm==NULL || curAuthorization.authorization==NULL) {
      ResetAuthorization();
      return FALSE;
   }
   return TRUE;
}

void HttpClearReferer()
{
   RefererInfo *pri=topReferer;

#ifdef _TGIF_DBG /* debug, do not translate */
   TgAssert(pri != NULL, "NULL topReferer in HttpClearReferer()", NULL);
#endif /* TGIF_DBG */
   if (pri == NULL) return;
   pri = pri->next;
   UtilFree(topReferer->referer);
   free(topReferer);
   topReferer = pri;
}

void HttpSetReferer(buf)
   char *buf;
{
   RefererInfo *pri=(RefererInfo*)malloc(sizeof(RefererInfo));

   if (pri == NULL) FailAllocMessage();
   pri->next = topReferer;
   pri->referer = UtilStrDup(buf);
   if (pri->referer == NULL) FailAllocMessage();
   topReferer = pri;
}

void HttpFakeReferer(buf)
   char *buf;
{
   UtilFree(gpszFakedReferer);
   gpszFakedReferer = NULL;
   if (buf != NULL && *buf != '\0') {
      gpszFakedReferer = UtilStrDup(buf);
      if (gpszFakedReferer == NULL) FailAllocMessage();
   }
}

static
void CleanUpHttpReferer()
{
   while (topReferer != NULL) {
      HttpClearReferer();
   }
}

static
void CleanUpHttpHeaderInfo()
{
   struct TgifHttpLineInfo *pthli, *next_thli;

   if (tgifHttpHeaderInfo.version != NULL) free(tgifHttpHeaderInfo.version);
   if (tgifHttpHeaderInfo.resp_status != NULL) {
      free(tgifHttpHeaderInfo.resp_status);
   }
   if (tgifHttpHeaderInfo.last_modified != NULL) {
      free(tgifHttpHeaderInfo.last_modified);
   }
   if (tgifHttpHeaderInfo.server != NULL) free(tgifHttpHeaderInfo.server);
   if (tgifHttpHeaderInfo.connection != NULL) {
      free(tgifHttpHeaderInfo.connection);
   }
   if (tgifHttpHeaderInfo.location != NULL) free(tgifHttpHeaderInfo.location);
   if (tgifHttpHeaderInfo.www_authenticate != NULL) {
      free(tgifHttpHeaderInfo.www_authenticate);
   }
   if (tgifHttpHeaderInfo.content_encoding != NULL) {
      free(tgifHttpHeaderInfo.content_encoding);
   }
   if (tgifHttpHeaderInfo.content_type != NULL) {
      free(tgifHttpHeaderInfo.content_type);
   }
   for (pthli=tgifHttpHeaderInfo.misc; pthli != NULL; pthli=next_thli) {
      next_thli = pthli->next;
      if (pthli->name != NULL) free(pthli->name);
      if (pthli->value != NULL) free(pthli->value);
      free(pthli);
   }
   memset(&tgifHttpHeaderInfo, 0, sizeof(struct TgifHttpHeaderInfo));
}

void CleanUpHttp()
{
   ResetAuthorization();
   CleanUpAuthInfo();
   CleanUpHttpHeaderInfo();
   CleanUpHttpReferer();

   UtilFree(gpszFakedReferer);
   gpszFakedReferer = NULL;
}

void InitHttp()
{
   topAuthInfo = botAuthInfo = NULL;
   memset(&curAuthorization, 0, sizeof(struct AuthInfo));
   memset(&tgifHttpHeaderInfo, 0, sizeof(struct TgifHttpHeaderInfo));
   topReferer = NULL;
}

char *HttpHeaderGetVersion() { return tgifHttpHeaderInfo.version; }

int HttpHeaderGetResponseCode() { return tgifHttpHeaderInfo.resp_code; }

char *HttpHeaderGetResponseStatus() { return tgifHttpHeaderInfo.resp_status; }

char *HttpHeaderGetLastModified() { return tgifHttpHeaderInfo.last_modified; }

char *HttpHeaderGetServer() { return tgifHttpHeaderInfo.server; }

char *HttpHeaderGetConnection() { return tgifHttpHeaderInfo.connection; }

char *HttpHeaderGetLocation() { return tgifHttpHeaderInfo.location; }

char *HttpHeaderGetWWWAuthentication()
{
   return tgifHttpHeaderInfo.www_authenticate;
}

char *HttpHeaderGetContentEncoding()
{
   return tgifHttpHeaderInfo.content_encoding;
}

char *HttpHeaderGetContentType() { return tgifHttpHeaderInfo.content_type; }

long HttpHeaderGetContentLength() { return tgifHttpHeaderInfo.content_length; }

char *HttpHeaderGetOtherField(name)
   char *name;
{
   struct TgifHttpLineInfo *pthli;

   for (pthli=tgifHttpHeaderInfo.misc; pthli != NULL; pthli=pthli->next) {
      if (pthli->name != NULL && UtilStrICmp(pthli->name, name) == 0) {
         return pthli->value;
      }
   }
   return NULL;
}

static int gnUserAgentNameInitialized=FALSE;

static
void InitUserAgentName()
{
   if (gnUserAgentNameInitialized) return;
   gnUserAgentNameInitialized = TRUE;

   GetClientID(SZ_USER_AGENT, sizeof(SZ_USER_AGENT));
   GetUserID(SZ_USER_NAME, sizeof(SZ_USER_NAME));
}

int HttpDoConnect(psz_host, us_port, pn_socket)
   char *psz_host;
   int us_port, *pn_socket;
{
   int rc, len=strlen(psz_host)+80;
   char *msg=(char*)malloc((len+1)*sizeof(char));

   if (msg == NULL) {
      FailAllocMessage();
      return TG_REMOTE_STATUS_MEM;
   }
   sprintf(msg, TgLoadCachedString(CSTID_MAKING_CONN_TO_HOST_PORT),
         "HTTP", psz_host, us_port);
   ShowRemoteStatus(msg);

   rc = TcpDoConnect(psz_host, us_port, pn_socket);

   if (rc == TG_REMOTE_STATUS_OK) {
      sprintf(msg, TgLoadCachedString(CSTID_CONN_TO_HOST_PORT_ESTB),
            "HTTP", psz_host, us_port);
   } else {
      sprintf(msg, TgLoadString(STID_FAIL_TO_CONN_TO_HOST_PORT),
            "HTTP", psz_host, us_port);
   }
   ShowRemoteStatus(msg);
   free(msg);

   return rc;
}

static
char *AppendSimpleString(buf, name, value)
   char *buf, *name, *value;
{
   int cur_len=strlen(buf);

   if (name == NULL && value == NULL) {
      int new_len=cur_len+2;

      if ((buf=(char*)realloc(buf, new_len+1)) == NULL) {
         return NULL;
      }
      sprintf(&buf[cur_len], "\r\n");
   } else {
      int new_len=cur_len+strlen(name)+2+strlen(value)+2;

      if ((buf=(char*)realloc(buf, new_len+1)) == NULL) {
         return NULL;
      }
      sprintf(&buf[cur_len], "%s: %s\r\n", name, value);
   }
   return buf;
}

/* do not translate -- program constants */
static char SZ_CONTENT_TYPE[]="Content-type:";
static char SZ_CONTENT_LENGTH[]="Content-length:";
static char SZ_POST_CONTENT_TYPE[]="application/x-www-form-urlencoded";

static char *pszAccept[] = {
/* "image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png", */
   "text/plain",
   "text/html",
   "application/x-tgif",
   "*/*",
   NULL
};

static
char *AppendAcceptStrings(buf)
   char *buf;
{
   char **s_ptr;
   int cur_len=strlen(buf);

   for (s_ptr=pszAccept; *s_ptr != NULL; s_ptr++) {
      int len=strlen(*s_ptr);
      int new_len=cur_len+len+2+8;

      if ((buf=(char*)realloc(buf, new_len+1)) == NULL) {
         return NULL;
      }
      /* do not translate -- program constants */
      sprintf(&buf[cur_len], "Accept: %s\r\n", *s_ptr);
      cur_len = new_len;
   }
   return buf;
}

static
char *AppendKeepAliveStrings(buf)
   char *buf;
{
   if (gnHttpKeepAlive == FALSE) return buf;

   /* do not translate -- program constants */
   return AppendSimpleString(buf, "Connection", "Keep-Alive");
}

static
char *AppendRefererStrings(buf)
   char *buf;
{
   if (topReferer == NULL) {
      if (gpszFakedReferer == NULL) {
         return buf;
      }
      /* do not translate -- program constants */
      return AppendSimpleString(buf, "Referer", gpszFakedReferer);
   }
   /* do not translate -- program constants */
   return AppendSimpleString(buf, "Referer", topReferer->referer);
}

static
char *AppendUserAgentString(buf)
   char *buf;
{
   InitUserAgentName();
   /* do not translate -- program constants */
   return AppendSimpleString(buf, "User-Agent", SZ_USER_AGENT);
}

static
char *AppendFromString(buf)
   char *buf;
{
   InitUserAgentName();
   /* do not translate -- program constants */
   return AppendSimpleString(buf, "From", SZ_USER_NAME);
}

static
char *AppendHostString(buf, psz_host, us_port)
   char *buf, *psz_host;
   int us_port;
{
   char tmp_buf[MAXSTRING];

   sprintf(tmp_buf, "%s:%1d", psz_host, us_port);
   /* do not translate -- program constants */
   return AppendSimpleString(buf, "Host", tmp_buf);
}

static
char *AppendAuthorizationString(buf)
   char *buf;
{
   char *pszReturn;
   char *pszAuth=(char*)malloc(
         (strlen(curAuthorization.scheme)+
         strlen(curAuthorization.authorization)+2)*sizeof(char));

   if (pszAuth == NULL) return NULL;
   sprintf(pszAuth, "%s %s", curAuthorization.scheme,
         curAuthorization.authorization);
   /* do not translate -- program constants */
   pszReturn = AppendSimpleString(buf, "Authorization", pszAuth);
   free(pszAuth);
   return pszReturn;
}

static
char *AppendPostContentTypeString(buf)
   char *buf;
{
   /* do not translate -- program constants */
   return AppendSimpleString(buf, "Content-type", SZ_POST_CONTENT_TYPE);
}

static
char *AppendPostContentLengthString(buf, content_length)
   char *buf;
   int content_length;
{
   char len_str[20];

   sprintf(len_str, "%1d", content_length);
   /* do not translate -- program constants */
   return AppendSimpleString(buf, "Content-length", len_str);
}

static
char *AppendPostContentString(buf, fp, content_length)
   char *buf;
   FILE *fp;
   int content_length;
{
   int cur_len=strlen(buf), bytes_read, total_read=0;
   int new_len=cur_len+content_length;
   char tmp_buf[512];

   if ((buf=(char*)realloc(buf, new_len+1)) == NULL) {
      return NULL;
   }
   while ((bytes_read=fread(tmp_buf, sizeof(char), sizeof(tmp_buf),
         fp)) > 0) {
      if (bytes_read+total_read > content_length) {
         bytes_read = content_length-total_read;
         fprintf(stderr, TgLoadString(STID_LINES_TOO_LONG_CONTENT_LENGTH),
               "HTTP");
         fprintf(stderr, "\n");
      }
      strncpy(&buf[cur_len+total_read], tmp_buf, bytes_read);
      total_read += bytes_read;
   }
   buf[cur_len+content_length] = '\0';
   return buf;
}

static
char *AppendCRLFString(buf)
   char *buf;
{
   return AppendSimpleString(buf, NULL, NULL);
}

int HttpDoWrite(n_socket, psz_path, psz_host, us_port)
   int n_socket, us_port;
   char *psz_path, *psz_host;
{
   int status=0, total_sz=0;
   FILE *fp=NULL;
   char *buf=(char*)malloc((strlen(psz_path)+5+2+31)*sizeof(char)), msg[40];

   if (buf == NULL) {
      FailAllocMessage();
      return TG_REMOTE_STATUS_MEM;
   }
   if (postingCGIQuery) {
      /* do not translate -- program constants */
      sprintf(buf, "POST %s %s\r\n", psz_path, SZ_HTTP_VERSION);
   } else if (gettingHttpHeaderOnly) {
      /* do not translate -- program constants */
      sprintf(buf, "HEAD %s %s\r\n", psz_path, SZ_HTTP_VERSION);
   } else {
      /* do not translate -- program constants */
      sprintf(buf, "GET %s %s\r\n", psz_path, SZ_HTTP_VERSION);
   }
   if ((buf=AppendKeepAliveStrings(buf)) == NULL) {
      FailAllocMessage();
      return TG_REMOTE_STATUS_MEM;
   }
   if ((buf=AppendRefererStrings(buf)) == NULL) {
      FailAllocMessage();
      return TG_REMOTE_STATUS_MEM;
   }
   if ((buf=AppendAcceptStrings(buf)) == NULL) {
      FailAllocMessage();
      return TG_REMOTE_STATUS_MEM;
   }
   if ((buf=AppendUserAgentString(buf)) == NULL) {
      FailAllocMessage();
      return TG_REMOTE_STATUS_MEM;
   }
   if ((buf=AppendFromString(buf)) == NULL) {
      FailAllocMessage();
      return TG_REMOTE_STATUS_MEM;
   }
   if ((buf=AppendHostString(buf, psz_host, us_port)) == NULL) {
      FailAllocMessage();
      return TG_REMOTE_STATUS_MEM;
   }
   if (curAuthorization.scheme != NULL &&
         curAuthorization.authorization != NULL) {
      if ((buf=AppendAuthorizationString(buf)) == NULL) {
         FailAllocMessage();
         return TG_REMOTE_STATUS_MEM;
      }
   }
   if (postingCGIQuery && fnameForPostingCGIQuery != NULL) {
      int bytes_read;
      char tmp_buf[512];

      if ((fp=fopen(fnameForPostingCGIQuery, "r")) == NULL) {
         fprintf(stderr, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
               fnameForPostingCGIQuery);
         fprintf(stderr, "\n");
         return TG_REMOTE_STATUS_READ;
      }
      while ((bytes_read=fread(tmp_buf, sizeof(char), sizeof(tmp_buf),
            fp)) > 0) {
         total_sz += bytes_read;
      }
      rewind(fp);
      if ((buf=AppendPostContentTypeString(buf)) == NULL) {
         fclose(fp);
         FailAllocMessage();
         return TG_REMOTE_STATUS_MEM;
      }
      if ((buf=AppendPostContentLengthString(buf, total_sz)) == NULL) {
         fclose(fp);
         FailAllocMessage();
         return TG_REMOTE_STATUS_MEM;
      }
   }
   if ((buf=AppendCRLFString(buf)) == NULL) {
      FailAllocMessage();
      return TG_REMOTE_STATUS_MEM;
   }
   if (fp != NULL) {
      buf = AppendPostContentString(buf, fp, total_sz);
      fclose(fp);
      if (buf == NULL) {
         FailAllocMessage();
         return TG_REMOTE_STATUS_MEM;
      }
   }
   sprintf(msg, TgLoadCachedString(CSTID_SENDING_REQUESTS_DOTS), "HTTP");
   ShowRemoteStatus(msg);

   if (debugHttp == 999 || ((debugHttp % 100) == 99 && cmdLineDumpURL &&
         cmdLineDumpURLWithHeader)) {
      /* debug, do not translate */
      fprintf(stderr, "************************\n");
      fprintf(stderr, "* Begin Request Header *\n");
      fprintf(stderr, "************************\n");
      (void)fwrite(buf, sizeof(char), strlen(buf), stderr);
      fprintf(stderr, "************************\n");
      fprintf(stderr, "*  End Request Header  *\n");
      fprintf(stderr, "************************\n");
   }
   status = TcpDoWrite(n_socket, buf, (int)strlen(buf));
   free(buf);

   if (status != TG_REMOTE_STATUS_OK) {
      sprintf(msg, TgLoadString(STID_FAIL_TO_SEND_REQ), "HTTP");
      ShowRemoteStatus(msg);
   }
   return status;
}

#ifdef NOT_DEFINED
#ifdef _TGIF_DBG /* debug, do not translate */
static char SZ_DEF_GIF_NAME[]="/tmp/htclient.gif";

static
void DebugHttpDumpResponse(buf)
   char *buf;
{
   char *c_ptr=strchr(buf, '\n'), *line_ptr=buf, *content_type=NULL;
   int content_length=(-1), gif87a=FALSE;
   int len1=strlen(SZ_CONTENT_TYPE), len2=strlen(SZ_CONTENT_LENGTH);
   FILE *fp=stderr;

   while (c_ptr != NULL) {
      char *prev_ptr=c_ptr;

      if (prev_ptr != line_ptr && *(--prev_ptr) == '\r') {
         *prev_ptr = '\0';
      } else {
         prev_ptr = NULL;
         *c_ptr = '\0';
      }
      fprintf(fp, "%s\n", line_ptr);
      if (content_type == NULL &&
            UtilStrNCaseCmp(line_ptr, SZ_CONTENT_TYPE, len1) == 0) {
         content_type = UtilStrDup(&line_ptr[len1]);
         if (content_type != NULL) {
            UtilTrimBlanks(content_type);
         }
      } else if (content_length == (-1) &&
            UtilStrNCaseCmp(line_ptr, SZ_CONTENT_LENGTH, len2) == 0) {
         char *tmp_ptr=UtilStrDup(&line_ptr[len2]);

         if (tmp_ptr != NULL) {
            UtilTrimBlanks(tmp_ptr);
            content_length = atoi(tmp_ptr);
            free(tmp_ptr);
         }
      }
      if (prev_ptr == NULL) {
         *c_ptr = '\n';
      } else {
         *prev_ptr = '\r';
      }
      line_ptr = &c_ptr[1];
      if (content_type != NULL && content_length != (-1)) {
         /* do not translate -- program constants */
         if (UtilStrICmp(content_type, "image/gif") == 0 &&
               UtilStrNCaseCmp(line_ptr, "GIF87a", 6) == 0) {
            gif87a = TRUE;
            break;
         }
      }
      c_ptr = strchr(line_ptr, '\n');
   }
   if (gif87a) {
      FILE *gif_fp;

      if ((gif_fp=fopen(SZ_DEF_GIF_NAME, "w")) == NULL) {
         fprintf(stderr, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
               SZ_DEF_GIF_NAME);
         fprintf(stderr, "\n");
      } else {
         if (fwrite(line_ptr, sizeof(char), content_length, gif_fp) !=
               content_length) {
            FailToWriteFileMessage(SZ_DEF_GIF_NAME);
         } else {
            /* debug, do not translate */
            fprintf(stderr, "GIF87a image written to '%s'.\n", SZ_DEF_GIF_NAME);
         }
         fclose(gif_fp);
      }
   } else if (line_ptr != NULL) {
      int len=strlen(line_ptr);

      if (len > 0 && line_ptr[len-1] == '\r') {
         line_ptr[len-1] = '\0';
         fprintf(fp, "%s\n", line_ptr);
         line_ptr[len-1] = '\r';
      } else {
         fprintf(fp, "%s\n", line_ptr);
      }
   }
}
#endif /* _TGIF_DBG */
#endif /* NOT_DEFINED */

static
int ParseBadFormat(buf)
   char *buf;
{
   fprintf(stderr, TgLoadString(STID_INVALID_FORMAT_IN_HEADER), "HTTP");
   fprintf(stderr, "\n");
   if (buf != NULL) free(buf);
   return TG_REMOTE_STATUS_FORMAT;
}

static
int ParseNoMem(buf)
   char *buf;
{
   FailAllocMessage();
   if (buf != NULL) free(buf);
   return TG_REMOTE_STATUS_MEM;
}

static
int HttpParseHeaderLine(buf, first_line)
   char *buf;
   int first_line;
{
   char *dup_buf, *colon_ptr;

   if (*buf == '\0') return TG_REMOTE_STATUS_OK;

   if (first_line) {
      char *version_ptr, *code_ptr, *status_ptr;

      dup_buf = UtilStrDup(buf);
      if (dup_buf == NULL) return ParseNoMem(NULL);;
      version_ptr = strtok(dup_buf, " \t\n\r");
      code_ptr = (version_ptr == NULL ? NULL : strtok(NULL, " \t\n\r"));
      status_ptr = (code_ptr == NULL ? NULL : strtok(NULL, " \t\n\r"));
      if (version_ptr == NULL) return ParseBadFormat(dup_buf);
      tgifHttpHeaderInfo.version = UtilStrDup(version_ptr);
      if (code_ptr == NULL) return ParseBadFormat(dup_buf);
      tgifHttpHeaderInfo.resp_code = atoi(code_ptr);
      if (status_ptr != NULL) {
         tgifHttpHeaderInfo.resp_status = UtilStrDup(status_ptr);
         if (tgifHttpHeaderInfo.resp_status == NULL) {
            return ParseBadFormat(dup_buf);
         }
      }
      free(dup_buf);
      return TG_REMOTE_STATUS_OK;
   }
   dup_buf = UtilStrDup(buf);
   if (dup_buf == NULL) return ParseNoMem(NULL);

   colon_ptr = strchr(dup_buf, ':');
   if (colon_ptr == NULL) return ParseBadFormat(dup_buf);
   *colon_ptr = '\0';
   UtilTrimBlanks(dup_buf);
   /* do not translate -- program constants */
   if (UtilStrICmp(dup_buf, "Last-Modified") == 0) {
      tgifHttpHeaderInfo.last_modified = UtilStrDup(&colon_ptr[1]);
      if (tgifHttpHeaderInfo.last_modified == NULL) return ParseNoMem(dup_buf);;
      UtilTrimBlanks(tgifHttpHeaderInfo.last_modified);
   } else if (UtilStrICmp(dup_buf, "Server") == 0) {
      tgifHttpHeaderInfo.server = UtilStrDup(&colon_ptr[1]);
      if (tgifHttpHeaderInfo.server == NULL) return ParseNoMem(dup_buf);;
      UtilTrimBlanks(tgifHttpHeaderInfo.server);
   } else if (UtilStrICmp(dup_buf, "Connection") == 0) {
      tgifHttpHeaderInfo.connection = UtilStrDup(&colon_ptr[1]);
      if (tgifHttpHeaderInfo.connection == NULL) return ParseNoMem(dup_buf);;
      UtilTrimBlanks(tgifHttpHeaderInfo.connection);
   } else if (UtilStrICmp(dup_buf, "Location") == 0) {
      tgifHttpHeaderInfo.location = UtilStrDup(&colon_ptr[1]);
      if (tgifHttpHeaderInfo.location == NULL) return ParseNoMem(dup_buf);;
      UtilTrimBlanks(tgifHttpHeaderInfo.location);
   } else if (UtilStrICmp(dup_buf, "WWW-Authenticate") == 0) {
      tgifHttpHeaderInfo.www_authenticate = UtilStrDup(&colon_ptr[1]);
      if (tgifHttpHeaderInfo.www_authenticate == NULL) {
         return ParseNoMem(dup_buf);;
      }
      UtilTrimBlanks(tgifHttpHeaderInfo.www_authenticate);
   } else if (UtilStrICmp(dup_buf, "Content-Encoding") == 0) {
      tgifHttpHeaderInfo.content_encoding = UtilStrDup(&colon_ptr[1]);
      if (tgifHttpHeaderInfo.content_encoding == NULL) {
         return ParseNoMem(dup_buf);;
      }
      UtilTrimBlanks(tgifHttpHeaderInfo.content_encoding);
   } else if (UtilStrICmp(dup_buf, "Content-Type") == 0) {
      tgifHttpHeaderInfo.content_type = UtilStrDup(&colon_ptr[1]);
      if (tgifHttpHeaderInfo.content_type == NULL) return ParseNoMem(dup_buf);;
      UtilTrimBlanks(tgifHttpHeaderInfo.content_type);
   } else if (UtilStrICmp(dup_buf, "Content-Length") == 0) {
      char *length_ptr=(&colon_ptr[1]);

      UtilTrimBlanks(length_ptr);
      if (sscanf(length_ptr, "%ld", &tgifHttpHeaderInfo.content_length) != 1) {
         tgifHttpHeaderInfo.content_length = 0L;
      }
   } else {
      struct TgifHttpLineInfo *pthli;

      pthli = (struct TgifHttpLineInfo*)malloc(sizeof(struct TgifHttpLineInfo));
      if (pthli == NULL) return ParseNoMem(dup_buf);;
      memset(pthli, 0, sizeof(struct TgifHttpLineInfo));
      pthli->name = UtilStrDup(dup_buf);
      if (pthli->name == NULL) return ParseNoMem(dup_buf);;
      pthli->value = UtilStrDup(&colon_ptr[1]);
      if (pthli->value == NULL) return ParseNoMem(dup_buf);;
      pthli->next = tgifHttpHeaderInfo.misc;
      tgifHttpHeaderInfo.misc = pthli;
   }
   *colon_ptr = ':';
   free(dup_buf);
   return TG_REMOTE_STATUS_OK;
}

void HttpDumpHeader()
{
   struct TgifHttpLineInfo *pthli;

   if (tgifHttpHeaderInfo.version != NULL) {
      fprintf(stderr, "%s %1d", tgifHttpHeaderInfo.version,
            tgifHttpHeaderInfo.resp_code);
      if (tgifHttpHeaderInfo.resp_status != NULL) {
         fprintf(stderr, " %s", tgifHttpHeaderInfo.resp_status);
      }
      fprintf(stderr, "\n");
   }
   /* do not translate -- program constants */
   if (tgifHttpHeaderInfo.last_modified != NULL) {
      fprintf(stderr, "Last-Modified: %s\n", tgifHttpHeaderInfo.last_modified);
   }
   if (tgifHttpHeaderInfo.server != NULL) {
      fprintf(stderr, "Server: %s\n", tgifHttpHeaderInfo.server);
   }
   if (tgifHttpHeaderInfo.connection != NULL) {
      fprintf(stderr, "Connection: %s\n", tgifHttpHeaderInfo.connection);
   }
   if (tgifHttpHeaderInfo.location != NULL) {
      fprintf(stderr, "Location: %s\n", tgifHttpHeaderInfo.location);
   }
   if (tgifHttpHeaderInfo.www_authenticate != NULL) {
      fprintf(stderr, "WWW-Authentication: %s\n",
            tgifHttpHeaderInfo.www_authenticate);
   }
   if (tgifHttpHeaderInfo.content_encoding != NULL) {
      fprintf(stderr, "Content-Encoding: %s\n",
            tgifHttpHeaderInfo.content_encoding);
   }
   if (tgifHttpHeaderInfo.content_type != NULL) {
      fprintf(stderr, "Content-Type: %s\n", tgifHttpHeaderInfo.content_type);
   }
   if (tgifHttpHeaderInfo.content_length != 0) {
      fprintf(stderr, "Content-Length: %ld\n",
            tgifHttpHeaderInfo.content_length);
   }
   for (pthli=tgifHttpHeaderInfo.misc; pthli != NULL; pthli=pthli->next) {
      fprintf(stderr, "%s: %s\n",
            (pthli->name == NULL ? TgLoadCachedString(CSTID_PARANED_UNKNOWN) :
            pthli->name), (pthli->value == NULL ?
            TgLoadCachedString(CSTID_PARANED_NONE) : pthli->value));
   }
}

char *HttpExtractText(buf, pn_buf_sz, pn_html, ppsz_content_type)
   char *buf, **ppsz_content_type;
   int *pn_buf_sz, *pn_html;
{
   char *c_ptr=strchr(buf, '\n'), *line_ptr=buf, *content_type=NULL;
   int content_length=(-1), text_type=FALSE, first_line=TRUE;
   int len1=strlen(SZ_CONTENT_TYPE), len2=strlen(SZ_CONTENT_LENGTH);
   FILE *fp=stderr;

   CleanUpHttpHeaderInfo();

   if (pn_buf_sz != NULL) *pn_buf_sz = 0;
   if (pn_html != NULL) *pn_html = FALSE;
   if (ppsz_content_type != NULL) *ppsz_content_type = NULL;
   while (c_ptr != NULL) {
      char *prev_ptr=c_ptr;
      int rc;

      if (prev_ptr != line_ptr && *(--prev_ptr) == '\r') {
         *prev_ptr = '\0';
      } else {
         prev_ptr = NULL;
         *c_ptr = '\0';
      }
      if (debugHttp > 0) {
         if ((debugHttp % 100) == 99 && cmdLineDumpURL &&
               !cmdLineDumpURLWithHeader) {
         } else if (!deserializingFile) {
            fprintf(fp, "%s\n", line_ptr);
         }
      }
      rc = HttpParseHeaderLine(line_ptr, first_line);
      if (rc != TG_REMOTE_STATUS_OK) {
         /* well, should break... */
      }
      first_line = FALSE;
      if (*line_ptr == '\0') {
         /* empty line, end of header */
         if (prev_ptr == NULL) {
            *c_ptr = '\n';
         } else {
            *prev_ptr = '\r';
         }
         line_ptr = &c_ptr[1];
         break;
      }
      if (content_type == NULL &&
            UtilStrNCaseCmp(line_ptr, SZ_CONTENT_TYPE, len1) == 0) {
         content_type = UtilStrDup(&line_ptr[len1]);
         if (content_type != NULL) {
            char *psz;

            UtilTrimBlanks(content_type);
            if ((psz=strchr(content_type, ';')) != NULL) *psz = '\0';
            if (ppsz_content_type != NULL) {
               *ppsz_content_type = UtilStrDup(content_type);
            }
            /* do not translate -- program constants */
            if (UtilStrNCaseCmp(content_type, "text/", 5) == 0) {
               text_type = TRUE;
               if (strcmp(&content_type[5], "html") == 0) {
                  if (pn_html != NULL) *pn_html = TRUE;
               }
            }
         }
      } else if (content_length == (-1) &&
            UtilStrNCaseCmp(line_ptr, SZ_CONTENT_LENGTH, len2) == 0) {
         char *tmp_ptr=UtilStrDup(&line_ptr[len2]);

         if (tmp_ptr != NULL) {
            UtilTrimBlanks(tmp_ptr);
            content_length = atoi(tmp_ptr);
            free(tmp_ptr);
         }
      }
      if (prev_ptr == NULL) {
         *c_ptr = '\n';
      } else {
         *prev_ptr = '\r';
      }
      line_ptr = &c_ptr[1];
      c_ptr = strchr(line_ptr, '\n');
   }
   if (content_type != NULL) free(content_type);
   if (cmdLineDumpURL && cmdLineDumpURLWithHeader && cmdLineDumpURLHeaderOnly) {
      /* only want the header */
   } else if (text_type || content_length == (-1)) {
      int buf_len=strlen(line_ptr);
      char *return_buf;

      if (content_length == (-1)) {
         content_length = buf_len;
         return_buf = (char*)malloc((buf_len+1)*sizeof(char));
      } else {
         return_buf = (char*)malloc((content_length+1)*sizeof(char));
      }
      if (return_buf == NULL) {
         FailAllocMessage();
         return NULL;
      }
      if (buf_len <= content_length) {
         memcpy(return_buf, line_ptr, content_length);
      } else {
         while (buf_len > content_length) {
            if (*line_ptr == '\r' || *line_ptr == '\n') {
               line_ptr++;
               buf_len--;
            } else {
               break;
            }
         }
         memcpy(return_buf, line_ptr, content_length);
      }
      return_buf[content_length] = '\0';
      if (pn_buf_sz != NULL) *pn_buf_sz = content_length;
      return return_buf;
   } else if (content_length != (-1)) {
      char *return_buf=(char*)malloc((content_length+1)*sizeof(char));

      if (return_buf == NULL) {
         FailAllocMessage();
         return NULL;
      }
      memcpy(return_buf, line_ptr, content_length);
      return_buf[content_length] = '\0';
      if (pn_buf_sz != NULL) *pn_buf_sz = content_length;
      return return_buf;
   }
   return NULL;
}

static
int GetContentLength(buf, pn_header_len)
   char *buf;
   int *pn_header_len;
{
   char *c_ptr, *line_ptr=buf;
   int content_len=(-1), just_lf=IsJustLF(buf);
   int inc=(just_lf?1:2);

   for (c_ptr=GetHeaderLine(line_ptr, just_lf); c_ptr != NULL;
         c_ptr=GetHeaderLine(line_ptr, just_lf)) {
      char *colon_ptr=NULL;

      if (c_ptr == line_ptr) {
         /* reach the end of header, now decide if the header is good */
         *pn_header_len = ((&c_ptr[inc]) - buf);
         return (content_len==(-1) ? 0 : content_len);
      }
      *c_ptr = '\0';
      colon_ptr = strchr(line_ptr, ':');
      if (colon_ptr != NULL) {
         *colon_ptr = '\0';
         /* do not translate -- program constants */
         if (UtilStrICmp(line_ptr, "Content-Length") == 0) {
            char length_str[MAXSTRING];
            int len=0;

            UtilStrCpyN(length_str, MAXSTRING-1, &colon_ptr[1]);
            UtilTrimBlanks(length_str);
            if (sscanf(length_str, "%d", &len) == 1) {
               content_len = len;
            }
         }
         *colon_ptr = ':';
      }
      *c_ptr = (just_lf ? '\n' : '\r');
      line_ptr = (&c_ptr[inc]);
   }
   return (-1);
}

static
int ReadFromSocket(n_socket)
   int n_socket;
   /* return TRUE means there's data at n_socket */
   /* return FALSE means abort */
{
   for (;;) {
      struct timeval timeout;
      fd_set fdset;
      int select_width=XConnectionNumber(mainDisplay)+1, status=0, nfds=0;

      nfds = max(select_width, n_socket+1);

      timeout.tv_sec = 0;
      timeout.tv_usec = 100000;

      FD_ZERO(&fdset);
      FD_SET(select_width-1, &fdset);
      FD_SET(n_socket, &fdset);
#ifdef __hpux
      status = select(nfds, (int*)&fdset, NULL, NULL, &timeout);
#else /* !__hpux */
      status = select(nfds, &fdset, NULL, NULL, &timeout);
#endif /* __hpux */
      if (status < 0) {
         if (errno == EINTR) {
            /* interrupted by a system call, not a problem */
            CheckInterrupt(TRUE);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_FUNC_SELECT_SYS_CALL_FAILED),
                  "ReadFromSocket()"),
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
            break;
         }
      } else if (status == 0) {
         /* spin the interrupt thing */
         CheckInterrupt(TRUE);
      } else if (status == 1) {
         if (FD_ISSET(n_socket, &fdset)) {
            /* character waiting to be processed at n_socket */
            return TRUE;
         } else if (FD_ISSET(select_width-1, &fdset)) {
            while (XPending(mainDisplay) > 0) {
               if (TryProcessAnAbortXEvent(TRUE, NULL, NULL) != 0) {
                  return FALSE;
               }
            }
         }
      } else if (status == 2) {
         return TRUE;
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_FUNC_INVALID_RC_FOR_SELECT),
               "ReadFromSocket()", status);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         break;
      }
   }
   return FALSE;
}

#define MIN_READ_SIZE 0x100

int HttpDoRead(n_socket, ppsz_buf, pn_buf_sz)
   int n_socket, *pn_buf_sz;
   char **ppsz_buf;
{
   int buf_sz=0x400, len=0, end_of_file=FALSE;
   int status=TG_REMOTE_STATUS_OK, content_len=(-1), header_len=0;
   char *buf=(char*)malloc(buf_sz*sizeof(char)), msg[MAXSTRING];

   if (pn_buf_sz != NULL) *pn_buf_sz = 0;
   *ppsz_buf = NULL;
   if (buf == NULL) {
      FailAllocMessage();
      return TG_REMOTE_STATUS_MEM;
   }
   sprintf(msg, TgLoadCachedString(CSTID_READING_RESPONSE_DOTS), "HTTP");
   ShowRemoteStatus(msg);
   CleanUpDownloadStats();
   ResetMultipartReplace(FALSE);

   do {
      int bytes_read=0;
      char progress_buf[MAXSTRING];

      *progress_buf = '\0';
      if (buf_sz - len < MIN_READ_SIZE) {
         buf_sz += 0x400;
         if ((buf=(char*)realloc(buf, buf_sz)) == NULL) {
            FailAllocMessage();
            status = TG_REMOTE_STATUS_MEM;
            break;
         }
      }
      if (PRTGIF || ReadFromSocket(n_socket)) {
         bytes_read = read(n_socket, &buf[len], buf_sz-len-1);
      } else {
         /* abort */
         status = TG_REMOTE_STATUS_INTR;
         break;
      }
      if (bytes_read <= 0) {
         if (bytes_read < 0 && (errno == ENOTCONN || errno == ECONNRESET ||
               errno == EPIPE)) {
            fprintf(stderr, TgLoadString(STID_NETWORK_READ_ERROR), "HTTP");
            fprintf(stderr, "\n");
            status = TG_REMOTE_STATUS_READ;
         } else if (bytes_read < 0) {
            fprintf(stderr, TgLoadString(STID_NETWORK_ERROR), "HTTP");
            fprintf(stderr, "\n");
            status = TG_REMOTE_STATUS_NET;
         }
         if (!UpdateDownloadStats(0, progress_buf)) {
            *progress_buf = '\0';
         }
         end_of_file = TRUE;
      } else {
         if (!UpdateDownloadStats(bytes_read, progress_buf)) {
            *progress_buf = '\0';
         }
         len += bytes_read;
      }
      if (status == TG_REMOTE_STATUS_OK && !end_of_file &&
            (UserAbortComm() || OtherAbortComm())) {
         /* abort */
         status = TG_REMOTE_STATUS_INTR;
         break;
      } else {
         int chopped_len=(-1);

         if (status == TG_REMOTE_STATUS_OK && content_len == (-1)) {
            buf[len] = '\0';
            content_len = GetContentLength(buf, &header_len);
         }
         if (content_len == (-1) || content_len == 0) {
            /* do not translate -- program constants */
            sprintf(msg, "HTTP: %1d bytes %s...", len,
                  (*progress_buf=='\0' ? "" : progress_buf));
            if (len >= 200) {
               chopped_len = len;
               HandleMultipartReplace(buf, &chopped_len);
            }
         } else {
            /* do not translate -- program constants */
            sprintf(msg, "HTTP: %1d of %1d bytes %s...", len-header_len,
                  content_len, (*progress_buf=='\0' ? "" : progress_buf));
            if (status == TG_REMOTE_STATUS_OK &&
                  len == header_len+content_len) {
               end_of_file = TRUE;
            }
         }
         ShowRemoteStatus(msg);
         if (PRTGIF && cmdLineDumpURL && cmdLineDumpURLShowStatus) {
            if (content_len == (-1) || content_len == 0) {
               /* do not translate -- program constants */
               fprintf(stderr, "HTTP: %1d bytes %s...\r", len,
                     (*progress_buf=='\0' ? "" : progress_buf));
            } else {
               /* do not translate -- program constants */
               fprintf(stderr, "HTTP: %1d of %1d bytes %s...\r", len-header_len,
                     content_len, (*progress_buf=='\0' ? "" : progress_buf));
            }
         }
         if (chopped_len != (-1)) {
            len = chopped_len;
         }
      }
   } while (status == TG_REMOTE_STATUS_OK && !end_of_file);

   if (status == TG_REMOTE_STATUS_OK) {
      buf[len] = '\0';
      *ppsz_buf = buf;
      if (pn_buf_sz != NULL) *pn_buf_sz = len;
      sprintf(msg, TgLoadCachedString(CSTID_RESPONSES_RECEIVED), "HTTP");
      if ((debugHttp % 100) == 99) {
         /* debug, do not translate */
         if (!cmdLineDumpURL) {
            fprintf(stderr, "\n==========>>>\n");
            (void)fwrite(buf, sizeof(char), len, stderr);
            fprintf(stderr, "\n<<<==========\n");
         }
      }
   } else {
      if (buf != NULL) free(buf);

      if (status == TG_REMOTE_STATUS_INTR) {
         if (PRTGIF && cmdLineDumpURL && cmdLineDumpURLShowStatus) {
            fprintf(stderr, "\n");
         }
         /* should get rid of all key presses */
         while (ESCPressed()) ;

         ResetMultipartReplace(TRUE);

         sprintf(msg, TgLoadString(STID_CONN_ABORT_BY_USER), "HTTP");
      } else {
         sprintf(msg, TgLoadString(STID_ERROR_ENCOUNTERED_WHILE_RECV), "HTTP");
      }
   }
   ShowRemoteStatus(msg);
   if (PRTGIF && cmdLineDumpURL && cmdLineDumpURLShowStatus) {
      fprintf(stderr, "\n");
   }
   return status;
}
