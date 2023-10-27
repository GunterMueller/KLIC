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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/remote.c,v 1.9 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_REMOTE_C_

#include "tgifdefs.h"
#include "patchlvl.h"

#include "cmd.e"
#include "cutpaste.e"
#include "dialog.e"
#include "drawing.e"
#include "file.e"
#include "ftp.e"
#include "http.e"
#include "mainloop.e"
#include "markup.e"
#include "menu.e"
#include "msg.e"
#include "names.e"
#include "navigate.e"
#include "page.e"
#include "remote.e"
#include "setup.e"
#include "stream.e"
#include "strtbl.e"
#include "tcp.e"
#include "util.e"
#include "version.e"

int postingCGIQuery=FALSE;
int gettingHttpHeaderOnly=FALSE;
char *fnameForPostingCGIQuery=NULL;
char gszLocalPID[MAXSTRING];
char gzipCmd[MAXPATHLENGTH+1];
char gunzipCmd[MAXPATHLENGTH+1];

static int debugRemote=FALSE;
static char uncompressCmd[MAXPATHLENGTH+1];
static char httpProxy[MAXPATHLENGTH+1];
static char ftpProxy[MAXPATHLENGTH+1];

typedef struct DownloadStatRec {
   int timestamp;
   int cumulative_bytes;
   struct DownloadStatRec *next, *prev;
} *DownloadStatRecPtr;

static struct DownloadStatRec *gpFirstDLS=NULL, *gpLastDLS=NULL;
static int gnDownloadStartTime=0;

void CleanUpDownloadStats()
{
   struct DownloadStatRec *pdls=NULL, *next_pdls=NULL;

   for (pdls=gpFirstDLS; pdls != NULL; pdls=next_pdls) {
      next_pdls = pdls->next;
      free(pdls);
   }
   gpFirstDLS = gpLastDLS = NULL;
   gnDownloadStartTime = 0;
}

#define THROUGHPUT_WINDOW 10

int UpdateDownloadStats(bytes, psz_buf)
   int bytes;
   char *psz_buf;
{
   struct DownloadStatRec *pdls=NULL;
   time_t tloc;
   int curtime=0, count=0, elapsetime=0, interval=0;
   float bps=(float)0;

   *psz_buf = '\0';
   time(&tloc);
   curtime = (int)tloc;
   if (gpFirstDLS == NULL) gnDownloadStartTime = curtime;
   pdls = gpLastDLS;
   if (pdls != NULL) {
      if (curtime == pdls->timestamp) {
         pdls->cumulative_bytes += bytes;
      } else {
         pdls = NULL;
      }
   }
   if (pdls == NULL) {
      pdls = (struct DownloadStatRec *)malloc(sizeof(struct DownloadStatRec));
      if (pdls == NULL) FailAllocMessage();
      memset(pdls, 0, sizeof(struct DownloadStatRec));
      pdls->timestamp = curtime;
      pdls->cumulative_bytes = (gpLastDLS==NULL ? 0 :
            gpLastDLS->cumulative_bytes);
      pdls->cumulative_bytes += bytes;
      pdls->prev = gpLastDLS;
      pdls->next = NULL;
      if (gpLastDLS == NULL) {
         gpFirstDLS = pdls;
      } else {
         gpLastDLS->next = pdls;
      }
      gpLastDLS = pdls;
   }
   if (gnDownloadStartTime == curtime) {
      return FALSE;
   }
   for (pdls=gpLastDLS, count=0; pdls != NULL && count < THROUGHPUT_WINDOW;
         pdls=pdls->prev, count++) {
   }
   elapsetime = gpLastDLS->timestamp - gnDownloadStartTime;
   if (pdls == NULL) {
      interval = elapsetime;
      bps = ((float)(gpLastDLS->cumulative_bytes)) / ((float)interval);
   } else {
      interval = gpLastDLS->timestamp - pdls->timestamp;
      bps = ((float)(gpLastDLS->cumulative_bytes - pdls->cumulative_bytes)) /
            ((float)interval);
   }
   bps /= (float)1000;
   sprintf(psz_buf, TgLoadCachedString(CSTID_PARANED_SEC_ELAPSED_AVG_SPEED),
         elapsetime, bps);
   return TRUE;
}

static int gnUserAgentInitialized=FALSE;
static char gszUserAgentName[MAXSTRING];

static
void InitUserAgentName()
{
   char *c_ptr=NULL;

   if (!gnUserAgentInitialized) {
      gnUserAgentInitialized = TRUE;
      *gszUserAgentName = '\0';
      if (!(PRTGIF && !cmdLineOpenDisplay) &&
            (c_ptr=XGetDefault(mainDisplay, TOOL_NAME, "UserAgentName")) !=
            NULL) {
         strcpy(gszUserAgentName, c_ptr);
         UtilTrimBlanks(gszUserAgentName);
      }
   }
}

int GetClientID(psz_buf, buf_sz)
   char *psz_buf;
   int buf_sz;
{
   char agent_name[128];

   *agent_name = '\0';
   InitUserAgentName();
   if (*gszUserAgentName != '\0') strcpy(agent_name, gszUserAgentName);

   if (*agent_name == '\0') {
      if (*specialBuild == '\0') {
         sprintf(agent_name, "%s/%s.%1d", TOOL_NAME, versionString,
               TGIF_PATCHLEVEL);
      } else {
         sprintf(agent_name, "%s/%s.%1d-%s", TOOL_NAME, versionString,
               TGIF_PATCHLEVEL, specialBuild);
      }
   }
   return UtilStrCpyN(psz_buf, buf_sz, agent_name);
}

void GetUserAgent(buf, buf_sz)
   char *buf;
   int buf_sz;
{
   InitUserAgentName();
   if (buf == NULL || buf_sz <= 0) return;
   *buf = '\0';
   UtilStrCpyN(buf, buf_sz, gszUserAgentName);
}

void SetUserAgent(buf)
   char *buf;
{
   InitUserAgentName();
   if (buf == NULL || *buf == '\0') {
      *gszUserAgentName = '\0';
   } else {
      UtilStrCpyN(gszUserAgentName, sizeof(gszUserAgentName), buf);
      UtilTrimBlanks(gszUserAgentName);
   }
}

int GetUserID(psz_buf, buf_sz)
   char *psz_buf;
   int buf_sz;
{
   char user_name[MAXSTRING+1];
   int total=0;

   sprintf(user_name, "%s@", TOOL_NAME);
   total = strlen(user_name);
   if (gethostname(&user_name[total], sizeof(user_name)-1-total) < 0) {
      sprintf(&user_name[total], "UNKNOWN");
   } else {
      struct hostent *p_hostent=gethostbyname(&user_name[total]);

      if (p_hostent != NULL && p_hostent->h_name != NULL &&
            *p_hostent->h_name != '\0') {
         if (strchr(p_hostent->h_name, '.') == NULL &&
               strchr(&user_name[total], '.') != NULL) {
         } else {
            strcpy(&user_name[total], p_hostent->h_name);
         }
      }
   }
   return UtilStrCpyN(psz_buf, buf_sz, user_name);
}

int UserAbortComm()
{
   return CheckInterrupt(TRUE);
}

int OtherAbortComm()
{
   XEvent ev;

   if (initializingMain || PRTGIF) return FALSE;

   while (XCheckMaskEvent(mainDisplay, StructureNotifyMask, &ev)) {
      if (iconWindowShown) {
         if ((ev.xany.window == iconBaseWindow && ev.type == UnmapNotify) ||
               (ev.xany.window == mainWindow && ev.type == MapNotify)) {
            XPutBackEvent(mainDisplay, &ev);
            return TRUE;
         }
      } else if ((ev.xany.window == iconBaseWindow && ev.type == MapNotify) ||
            (ev.xany.window == mainWindow && ev.type == UnmapNotify)) {
         XPutBackEvent(mainDisplay, &ev);
         return TRUE;
      } else if (ev.type == ConfigureNotify) {
         Reconfigure(FALSE);
      }
   }
   while (XCheckMaskEvent(mainDisplay, VisibilityChangeMask, &ev)) {
      if (iconWindowShown) {
         if (ev.xany.window == mainWindow && ev.type == VisibilityNotify &&
               ev.xvisibility.state == VisibilityUnobscured) {
            XPutBackEvent(mainDisplay, &ev);
            return TRUE;
         } else {
            ExposeEventHandler(&ev, TRUE);
         }
      } else {
         if (ev.xany.window == iconBaseWindow && ev.type == VisibilityNotify &&
               ev.xvisibility.state == VisibilityUnobscured) {
            XPutBackEvent(mainDisplay, &ev);
            return TRUE;
         } else {
            ExposeEventHandler(&ev, TRUE);
         }
      }
   }
   if (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) {
      ExposeEventHandler(&ev, TRUE);
      while (XCheckMaskEvent(mainDisplay, ExposureMask, &ev)) ;
   }
   return FALSE;
}

int GetPageNumFromPageSpec(psz_spec, pn_page_num)
   char *psz_spec;
   int *pn_page_num;
{
   if (*psz_spec == '#') {
      int i=atoi(&psz_spec[1]);

      if (i >= 1 && i <= lastPageNum) {
         if (pn_page_num != NULL) *pn_page_num = i;
         return TRUE;
      }
   } else {
      struct PageRec *page_ptr;
      int i;

      if (pn_page_num != NULL) *pn_page_num = (-1);
      for (i=1, page_ptr=firstPage; i <= lastPageNum; i++,
            page_ptr=page_ptr->next) {
         if (page_ptr->name != NULL && strcmp(psz_spec,page_ptr->name) == 0) {
            if (pn_page_num != NULL) *pn_page_num = i;
            return TRUE;
         }
      }
   }
   if (pn_page_num != NULL) *pn_page_num = (-1);
   return FALSE;
}

void FreeRemoteBuf(psz_buf)
   char *psz_buf;
{
   TcpFreeBuf(psz_buf);
}

static
int ParseURL(url, protocol, host, port, path)
   char *url, **protocol, **host, **path;
   int *port;
{
   char *c_ptr=strchr(url, ':');

   *protocol = *host = *path = 0;
   if (c_ptr == NULL) return TG_REMOTE_STATUS_FORMAT;

   *c_ptr = '\0';
   *protocol = UtilStrDup(url);
   *c_ptr++ = ':';
   if (strncmp(c_ptr, "//", 2) == 0) {
      char *tmp_host=(&c_ptr[2]), *port_ptr;

      if ((c_ptr=strchr(tmp_host, '/')) == NULL) {
         *path = UtilStrDup("");
      } else {
         *path = UtilStrDup(c_ptr);
         *c_ptr = '\0';
      }
      if ((port_ptr=strchr(tmp_host, ':')) != NULL) {
         *port_ptr = '\0';
         *port = (int)atoi(&port_ptr[1]);
         *host = UtilStrDup(tmp_host);
         *port_ptr = ':';
         if (*port <= 0) {
            return TG_REMOTE_STATUS_FORMAT;
         }
      } else {
         *host = UtilStrDup(tmp_host);
      }
      if (c_ptr != NULL) *c_ptr = '/';
   } else {
      *host = UtilStrDup("localhost");
      *path = UtilStrDup(c_ptr);
   }
   return TG_REMOTE_STATUS_OK;
}

static
void DumpURL(host, port, path)
   char *host, *path;
   int port;
{
   /* debug, do not translate */
   fprintf(stderr, "\thost = %s\n", host);
   fprintf(stderr, "\tport = %1d\n", port);
   fprintf(stderr, "\tpath = %s\n", path);
   fprintf(stderr, "\n");
}

int DirIsRemote(psz_dir)
   char *psz_dir;
{
   int port=0;
   char *protocol=NULL, *host=NULL, *path=NULL;
   int status=ParseURL(psz_dir, &protocol, &host, &port, &path), rc=FALSE;

   if (status == TG_REMOTE_STATUS_OK && protocol != NULL) {
      if (UtilStrICmp(protocol, "http") == 0 ||
            UtilStrICmp(protocol, "ftp") == 0) {
         rc = TRUE;
      }
   }
   if (protocol != NULL) free(protocol);
   if (host != NULL) free(host);
   if (path != NULL) free(path);
   return rc;
}

int FileIsRemote(psz_file)
   char *psz_file;
{
   return DirIsRemote(psz_file);
}

int UrlIsHtml(psz_url)
   char *psz_url;
{
   char *ext_str=UtilStrRChr(psz_url, (int)'/');

   if (ext_str == NULL) return FALSE;
   if ((ext_str=strchr(ext_str, '.')) == NULL) return FALSE;
   ext_str++;
   return (UtilStrICmp(ext_str,"html")==0 || UtilStrICmp(ext_str,"htm")==0);
}

static
int FormLocalName(psz_file, psz_def_ext, psz_return)
   char *psz_file, *psz_def_ext, *psz_return;
{
   if (psz_def_ext != NULL) {
      char *slash_ptr=UtilStrRChr(psz_file, (int)DIR_SEP), *dot_ptr=NULL;

      if (slash_ptr == NULL) {
         dot_ptr = strchr(psz_file, '.');
      } else {
         dot_ptr = strchr(slash_ptr, '.');
      }
      if (dot_ptr == NULL) {
         if (slash_ptr != NULL && slash_ptr[1] == '\0') {
            sprintf(psz_return, "%s/index.%s", psz_file, psz_def_ext);
         } else {
            sprintf(psz_return, "%s.%s", psz_file, psz_def_ext);
         }
      } else {
         strcpy(psz_return, psz_file);
      }
   } else {
      strcpy(psz_return, psz_file);
   }
   return UtilShrinkName(psz_return);
}

int FormRemoteName(psz_file, psz_def_ext, psz_return)
   char *psz_file, *psz_def_ext, *psz_return;
{
   char *c_ptr=strstr(psz_file, "//"), *path;

   if (c_ptr == NULL) return FALSE;
   if ((c_ptr=strchr(&c_ptr[2], '/')) == NULL) {
      if (!FormLocalName("/", psz_def_ext, psz_return)) return FALSE;
      path = UtilStrDup(psz_return);
      sprintf(psz_return, "%s%s", psz_file, path);
      free(path);
   } else {
      if (!FormLocalName(c_ptr, psz_def_ext, psz_return)) return FALSE;
      path = UtilStrDup(psz_return);
      *c_ptr = '\0';
      sprintf(psz_return, "%s%s", psz_file, path);
      free(path);
      *c_ptr = '/';
   }
   return TRUE;
}

int FormNewFileName(psz_dir, psz_file, psz_def_ext, psz_return, ppsz_page_spec)
   char *psz_dir, *psz_file, *psz_def_ext, *psz_return, **ppsz_page_spec;
{
   int rc=TRUE;

   if (ppsz_page_spec != NULL) *ppsz_page_spec = NULL;
   if (DirIsRemote(psz_dir)) {
      if (*psz_file == '/') {
         int len;
         char *c_ptr=strstr(psz_dir, "//"), *fname=NULL;

         if (c_ptr == NULL) return FALSE;
         if ((c_ptr=strchr(&c_ptr[2], '/')) == NULL) return FALSE;
         *c_ptr = '\0';
         len = strlen(psz_dir)+strlen(psz_file);
         fname = (char*)malloc((len+1)*sizeof(char));
         if (fname == NULL) {
            *c_ptr = '/';
            return FailAllocMessage();
         }
         sprintf(fname, "%s%s", psz_dir, psz_file);
         *c_ptr = '/';
         if (!FormRemoteName(fname, psz_def_ext, psz_return)) rc = FALSE;
         free(fname);
      } else if (FileIsRemote(psz_file)) {
         if (!FormRemoteName(psz_file, psz_def_ext, psz_return)) rc = FALSE;
      } else {
         int len=strlen(psz_dir)+1+strlen(psz_file);
         char *fname=(char*)malloc((len+1)*sizeof(char));

         if (fname == NULL) return FailAllocMessage();

         sprintf(fname, "%s/%s", psz_dir, psz_file);
         if (!FormRemoteName(fname, psz_def_ext, psz_return)) rc = FALSE;
         free(fname);
      }
   } else {
      if (*psz_file == DIR_SEP) {
         if (!FormLocalName(psz_file, psz_def_ext, psz_return)) rc = FALSE;
      } else if (FileIsRemote(psz_file)) {
         if (!FormRemoteName(psz_file, psz_def_ext, psz_return)) rc = FALSE;
      } else {
         int len=strlen(psz_dir)+1+strlen(psz_file);
         char *fname=(char*)malloc((len+1)*sizeof(char));

         if (fname == NULL) return FailAllocMessage();

         sprintf(fname, "%s%c%s", psz_dir, DIR_SEP, psz_file);
         if (!FormLocalName(fname, psz_def_ext, psz_return)) rc = FALSE;
         free(fname);
      }
   }
   if (rc && ppsz_page_spec != NULL && psz_return != NULL &&
         *psz_return != '\0') {
      char *c_ptr=UtilStrRChr(psz_return, (int)'/');

      if (c_ptr != NULL) {
         for (++c_ptr; *c_ptr != '\0'; c_ptr++) {
            if (*c_ptr == '#') {
               *c_ptr++ = '\0';
               *ppsz_page_spec = UtilStrDup(c_ptr);
            } else if (*c_ptr == '?') {
               break;
            }
         }
      }
   }
   return rc;
}

void ShowRemoteStatus(psz_msg)
   char *psz_msg;
{
   if (PRTGIF) return;
   if (serializingFile || deserializingFile) return;

   SetStringStatus(psz_msg);
   XSync(mainDisplay, False);
}

char *WriteRemoteFileIntoTemp(psz_buf, buf_sz, psz_ext)
   char *psz_buf, *psz_ext;
   int buf_sz;
{
   char *tmp_fname=NULL;
   int fd=(-1);
   mode_t mode=((tmpFileMode == 0) ? 0700 : tmpFileMode);

   tmp_fname = (char*)malloc((strlen(tmpDir)+20)*sizeof(char));
   if (tmp_fname == NULL) {
      FailAllocMessage();
      return NULL;
   }
   if (MkTempFile(tmp_fname, (strlen(tmpDir)+19)*sizeof(char), tmpDir,
         TOOL_NAME) == NULL) {
      free(tmp_fname);
      return NULL;
   }
   if (psz_ext != NULL) strcat(tmp_fname, psz_ext);
   unlink(tmp_fname);
   if ((fd=open(tmp_fname, O_WRONLY|O_CREAT|O_TRUNC, mode)) == (-1)) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_CREATE_NAMED_TMP_FILE),
            tmp_fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      free(tmp_fname);
      return NULL;
   } else {
      int ok=TRUE;

      if (write(fd, psz_buf, buf_sz) != buf_sz) {
         sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_WRITE_TO_FILE),
               tmp_fname);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         ok = FALSE;
      }
      close(fd);
      if (!ok) {
         unlink(tmp_fname);
         free(tmp_fname);
         return NULL;
      }
      if (tmpFileMode != 0 && chmod(tmp_fname, tmpFileMode)) {
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_CHMOD), tmp_fname,
               tmpFileMode);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return tmp_fname;
   }
}

#define USE_GUNZIP 0
#define USE_UNCOMPRESS 1

static
int TgifCompressedUrl(url)
   char *url;
{
   char *ext_ptr=NULL;
   int len, count=0, obj_len, sym_len, pin_len, uncompress_method=INVALID;

   if (url == NULL) return uncompress_method;
   len = strlen(url);
   for (ext_ptr=(&url[len-1]); ext_ptr != url; ext_ptr--) {
      if (*ext_ptr == '.') {
         if (++count == 2) break;
      }
   }
   if (count != 2) return uncompress_method;
   ext_ptr++;
   obj_len = strlen(OBJ_FILE_EXT);
   sym_len = strlen(SYM_FILE_EXT);
   pin_len = strlen(PIN_FILE_EXT);

   if (strcmp(ext_ptr,"obj.gz")==0 || strcmp(ext_ptr,"sym.gz")==0 ||
         strcmp(ext_ptr,"pin.gz")==0) {
      uncompress_method = USE_GUNZIP;
   } else if (strcmp(ext_ptr,"obj.Z")==0 || strcmp(ext_ptr,"sym.Z")==0 ||
         strcmp(ext_ptr,"pin.Z")==0) {
      uncompress_method = USE_UNCOMPRESS;
   } else if ((strncmp(ext_ptr,OBJ_FILE_EXT,obj_len)==0 &&
         strcmp(&ext_ptr[obj_len],".gz")==0) ||
         (strncmp(ext_ptr,SYM_FILE_EXT,sym_len)==0 &&
         strcmp(&ext_ptr[sym_len],".gz")==0) ||
         (strncmp(ext_ptr,PIN_FILE_EXT,pin_len)==0 &&
         strcmp(&ext_ptr[pin_len],".gz")==0)) {
      uncompress_method = USE_GUNZIP;
   } else if ((strncmp(ext_ptr,OBJ_FILE_EXT,obj_len)==0 &&
         strcmp(&ext_ptr[obj_len],".Z")==0) ||
         (strncmp(ext_ptr,SYM_FILE_EXT,sym_len)==0 &&
         strcmp(&ext_ptr[sym_len],".Z")==0) ||
         (strncmp(ext_ptr,PIN_FILE_EXT,pin_len)==0 &&
         strcmp(&ext_ptr[pin_len],".Z")==0)) {
      uncompress_method = USE_UNCOMPRESS;
   }
   return uncompress_method;
}

static
void UncompressTgifFile(url, ppsz_buf, pn_buf_sz, pn_html)
   char *url, **ppsz_buf;
   int *pn_buf_sz, *pn_html;
{
   FILE *pfp=NULL;
   char *uncompress_cmd=NULL, *tmp_fname=NULL, *cmd=NULL;
   int uncompress_method;

   if (ppsz_buf == NULL || *ppsz_buf == NULL || pn_buf_sz == NULL ||
         *pn_buf_sz <= 0) {
      return;
   }
   uncompress_method = TgifCompressedUrl(url);

   switch (uncompress_method) {
   case USE_GUNZIP:
      uncompress_cmd = gunzipCmd;
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_UNZIPPING_WITH_CMD_DOTS),
            gunzipCmd);
      ShowRemoteStatus(gszMsgBox);
      break;
   case USE_UNCOMPRESS:
      uncompress_cmd = uncompressCmd;
      sprintf(gszMsgBox, TgLoadCachedString(CSTID_UNCOMPRESSING_WITH_CMD_DOTS),
            uncompressCmd);
      ShowRemoteStatus(gszMsgBox);
      break;
   default: return;
   }
   if ((tmp_fname=WriteRemoteFileIntoTemp(*ppsz_buf, *pn_buf_sz,
         (uncompress_method==USE_UNCOMPRESS ? ".Z" : NULL))) == NULL) {
      return;
   }
   cmd = (char*)malloc((strlen(tmp_fname)+strlen(uncompress_cmd)+20) *
         sizeof(char));
   if (cmd == NULL) FailAllocMessage();

   if (strstr(uncompress_cmd, "%s") == NULL) {
      sprintf(cmd, "%s %s", uncompress_cmd, tmp_fname);
   } else {
      sprintf(cmd, uncompress_cmd, tmp_fname);
   }
   if (!FindProgramInPath(cmd, NULL, FALSE)) {
      free(cmd);
      unlink(tmp_fname);
      free(tmp_fname);
      return;
   }
   if ((pfp=(FILE*)popen(cmd, "r")) == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANNOT_EXECUTE_GIVEN_CMD), cmd);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else {
      int buf_sz=0x1000, total=0, chunk_sz=0x1000, bytes_read;
      char *buf=(char*)malloc((buf_sz+1)*sizeof(char));

      if (buf == NULL) FailAllocMessage();
      while (buf != NULL && (bytes_read=fread(&buf[total], sizeof(char),
            chunk_sz, pfp)) > 0) {
         total += bytes_read;
         buf_sz += chunk_sz;
         buf = (char*)realloc(buf, buf_sz+1);
      }
      buf[total] = '\0';
      pclose(pfp);
      if (buf != NULL && total > 0) {
         free(*ppsz_buf);
         *ppsz_buf = buf;
         *pn_buf_sz = total;
      } else if (buf != NULL) {
         free(buf);
      }
   }
   unlink(tmp_fname);
   free(tmp_fname);
   if (cmd != NULL) free(cmd);
}

static
char *GetProxyHostAndPort(proxy_spec, def_port, pn_port)
   char *proxy_spec;
   int def_port, *pn_port;
{
   char *colon_ptr=strchr(proxy_spec, ':'), *return_buf=NULL;
   int len=(-1);

   if (colon_ptr == NULL) {
      len = strlen(proxy_spec);
      if (len > 0 && proxy_spec[len-1] == '/') {
         proxy_spec[len-1] = '\0';
      } else {
         len = (-1);
      }
      *pn_port = def_port;
      return_buf = UtilStrDup(proxy_spec);
   } else {
      *colon_ptr = '\0';
      len = strlen(proxy_spec);
      if (len > 0 && proxy_spec[len-1] == '/') {
         proxy_spec[len-1] = '\0';
      } else {
         len = (-1);
      }
      *pn_port = atoi(&colon_ptr[1]);
      return_buf = UtilStrDup(proxy_spec);
      *colon_ptr = ':';
   }
   if (len != (-1)) proxy_spec[len-1] = '/';
   return return_buf;
}

static
int LoadHttpIntoMem(url, host, port, path, ppsz_buf, ppsz_content_type,
      pn_buf_sz, pn_html, pn_http_extracted_text)
   char *url, *host, *path, **ppsz_buf, **ppsz_content_type;
   int port, *pn_buf_sz, *pn_html, *pn_http_extracted_text;
{
   int status=TG_REMOTE_STATUS_OK, n_socket=0, proxy_port=0;
   char *proxy_host=NULL, port_str[20];

   if (pn_http_extracted_text != NULL) *pn_http_extracted_text=FALSE;
   if (port == 0) port = 80;
   if (debugRemote) DumpURL(host, port, path);

   ShowInterrupt(1);
   if (*httpProxy == '\0') {
      sprintf(port_str, "%1d", port);
      status = HttpDoConnect(host, port, &n_socket);
   } else {
      proxy_host = GetProxyHostAndPort(httpProxy, 80, &proxy_port);
      sprintf(port_str, "%1d", proxy_port);
      if (proxy_host == NULL) {
         status = INVALID;
      } else {
         status = HttpDoConnect(proxy_host, proxy_port, &n_socket);
      }
   }
   if (status == TG_REMOTE_STATUS_HOST) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_LOCATE_NAMED_SERVER_TRY),
            "HTTP", (*httpProxy == '\0' ? host : proxy_host));
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
   } else if (status == TG_REMOTE_STATUS_INTR) {
      sprintf(gszMsgBox, TgLoadString(STID_CONNECTION_INTERRUPTED), "HTTP");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      close(n_socket);
   } else if (status < 0) {
      /* n_socket already closed in TcpDoDonnect() */
      if (*httpProxy == '\0') {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_CONN_TO_HOST_PORT), "HTTP",
               host, port==80 ? "" : ":", port==80 ? "" : port_str);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_CONN_TO_HOST_PORT), "HTTP",
               proxy_host, proxy_port==80 ? "" : ":",
               proxy_port==80 ? "" : port_str);
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else if (status == TG_REMOTE_STATUS_OK) {
      status = HttpDoWrite(n_socket, (*httpProxy=='\0' ? path : url),
            host, port);
      if (status == TG_REMOTE_STATUS_OK) {
         char *buf=NULL;
         int buf_sz=0;

         if ((status=HttpDoRead(n_socket, &buf, &buf_sz)) ==
               TG_REMOTE_STATUS_OK && buf != NULL && *buf != '\0') {
            *ppsz_buf = HttpExtractText(buf, pn_buf_sz, pn_html,
                  ppsz_content_type);
            if (*ppsz_buf != NULL && pn_buf_sz != NULL &&
                  ppsz_content_type != NULL) {
               if (!ParseMarkUpFile(*ppsz_buf, *pn_buf_sz, pn_html,
                     *ppsz_content_type)) {
               }
            }
            if (*ppsz_buf != NULL && debugHttp > 0) {
               if ((debugHttp % 100) == 99) {
                  /*
                   * Because of the way HttpExtractText() is coded,
                   * if *ppsz_buf != NULL, (*ppsz_buf)[*pn_buf_sz] is '\0'
                   * no matter what kind of file it is!
                   */
                  if (cmdLineDumpURL) {
                     (void)fwrite(*ppsz_buf, sizeof(char), (*pn_buf_sz),
                           stdout);
                  }
               }
            }
            if (pn_http_extracted_text != NULL) *pn_http_extracted_text = TRUE;
            FreeRemoteBuf(buf);
         } else if (status == TG_REMOTE_STATUS_INTR) {
            sprintf(gszMsgBox, TgLoadString(STID_CONNECTION_INTERRUPTED),
                  "HTTP");
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         } else if (buf == NULL || *buf == '\0') {
            sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_GET_DATA_FROM_URL),
                  "HTTP", url);
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         } else {
            if (*httpProxy == '\0') {
               sprintf(gszMsgBox,
                     TgLoadString(STID_NETWORK_ERR_TALK_TO_HOST_PORT),
                     "HTTP", host, port==80 ? "" : ":",
                     port==80 ? "" : port_str);
            } else {
               sprintf(gszMsgBox,
                     TgLoadString(STID_NETWORK_ERR_TALK_TO_HOST_PORT),
                     "HTTP", proxy_host, proxy_port==80 ? "" : ":",
                     proxy_port==80 ? "" : port_str);
            }
            MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         }
      }
      close(n_socket);
   } else {
      if (*httpProxy == '\0') {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_CONN_TO_HOST_PORT),
               "HTTP", host, port==80 ? "" : ":", port==80 ? "" : port_str);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_CONN_TO_HOST_PORT),
               "HTTP", proxy_host, proxy_port==80 ? "" : ":",
               proxy_port==80 ? "" : port_str);
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   HideInterrupt();
   return status;
}

static
int LoadFtpIntoMem(url, host, port, path, ppsz_buf, ppsz_content_type,
      pn_buf_sz, pn_html)
   char *url, *host, *path, **ppsz_buf, **ppsz_content_type;
   int port, *pn_buf_sz, *pn_html;
{
   int status=TG_REMOTE_STATUS_OK, n_socket=0, proxy_port=0;
   char *proxy_host=NULL, port_str[20];

   if (port == 0) port = 21;
   if (debugRemote) DumpURL(host, port, path);

   ShowInterrupt(1);
   if (*ftpProxy == '\0') {
      sprintf(port_str, "%1d", port);
      status = FtpDoConnect(host, port, &n_socket);
   } else {
      proxy_host = GetProxyHostAndPort(ftpProxy, 21, &proxy_port);
      sprintf(port_str, "%1d", proxy_port);
      if (proxy_host == NULL) {
         status = INVALID;
      } else {
         status = FtpDoConnect(proxy_host, proxy_port, &n_socket);
      }
   }
   if (status == TG_REMOTE_STATUS_HOST) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_LOCATE_NAMED_SERVER_TRY),
            "FTP", (*ftpProxy == '\0' ? host : proxy_host));
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
   } else if (status == TG_REMOTE_STATUS_INTR) {
      sprintf(gszMsgBox, TgLoadString(STID_CONNECTION_INTERRUPTED), "FTP");
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      close(n_socket);
   } else if (status < 0) {
      /* n_socket already closed in TcpDoDonnect() */
      if (*ftpProxy == '\0') {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_CONN_TO_HOST_PORT),
               "FTP", host, port==21 ? "" : ":", port==21 ? "" : port_str);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_CONN_TO_HOST_PORT),
               "FTP", proxy_host, proxy_port==21 ? "" : ":",
               proxy_port==21 ? "" : port_str);
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   } else if (status == TG_REMOTE_STATUS_OK) {
      char *buf=NULL;
      int buf_sz=0;

      sprintf(gszMsgBox, TgLoadCachedString(CSTID_SENDING_REQUESTS_DOTS),
            "FTP");
      ShowRemoteStatus(gszMsgBox);
      if (((status=FtpDoTalk(n_socket, (*ftpProxy=='\0' ? path : url),
            &buf, &buf_sz))&(~FTP_LOGGED_IN)) == TG_REMOTE_STATUS_OK &&
            buf != NULL && *buf != '\0') {
         *ppsz_buf = buf;
         if (pn_buf_sz != NULL) *pn_buf_sz = buf_sz;
         if (*ppsz_buf != NULL && pn_buf_sz != NULL && debugFtp == -99) {
            /*
             * Because of the way FtpDoTalk() is coded,
             * if *ppsz_buf != NULL, (*ppsz_buf)[*pn_buf_sz] is '\0'
             * no matter what kind of file it is!
             */
            if (cmdLineDumpURL) {
               (void)fwrite(*ppsz_buf, sizeof(char), (*pn_buf_sz), stdout);
            }
         }
         sprintf(gszMsgBox, TgLoadCachedString(CSTID_DATA_RECEIVED), "FTP");
         ShowRemoteStatus(gszMsgBox);
      } else if ((status & (~FTP_LOGGED_IN)) == TG_REMOTE_STATUS_INTR) {
         sprintf(gszMsgBox, TgLoadString(STID_CONNECTION_INTERRUPTED), "FTP");
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else if (status & FTP_LOGGED_IN) {
         sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_GET_DATA_FROM_URL),
               "FTP", url);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else {
         if (*ftpProxy == '\0') {
            sprintf(gszMsgBox, TgLoadString(STID_FAILED_TO_LOGIN_TO_HOST_PORT),
                  "FTP", host, port==21 ? "" : ":", port==21 ? "" : port_str);
         } else {
            sprintf(gszMsgBox, TgLoadString(STID_FAILED_TO_LOGIN_TO_HOST_PORT),
                  "FTP", proxy_host, proxy_port==21 ? "" : ":",
                  proxy_port==21 ? "" : port_str);
         }
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      close(n_socket);
   } else {
      if (*ftpProxy == '\0') {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_CONN_TO_HOST_PORT),
               "FTP", host, port==21 ? "" : ":", port==21 ? "" : port_str);
      } else {
         sprintf(gszMsgBox, TgLoadString(STID_CANT_CONN_TO_HOST_PORT),
               "FTP", proxy_host, proxy_port==21 ? "" : ":",
               proxy_port==21 ? "" : port_str);
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   HideInterrupt();
   return status;
}

static
void ResetRemoteBuf(ppsz_buf, ppsz_content_type, pn_buf_sz, pn_html)
   char **ppsz_buf, **ppsz_content_type;
   int *pn_buf_sz, *pn_html;
{
   if (ppsz_buf != NULL && *ppsz_buf != NULL) {
      free(*ppsz_buf);
      *ppsz_buf = NULL;
   }
   if (ppsz_content_type != NULL && *ppsz_content_type != NULL) {
      free(*ppsz_content_type);
      *ppsz_content_type = NULL;
   }
   if (pn_buf_sz != NULL) *pn_buf_sz = 0;
   if (pn_html != NULL) *pn_html = FALSE;
}

static int gnForwardCount=0;
static int gnAuthCount=0;

static
int LoadForwardedFileInMem(url, ppsz_buf, ppsz_content_type, pn_buf_sz,
      pn_html, force_load, psz_final_url, cb_final_url)
   char *url, **ppsz_buf, **ppsz_content_type, *psz_final_url;
   int *pn_buf_sz, *pn_html, force_load, cb_final_url;
{
   char *location=HttpHeaderGetLocation(), *dup_location;
   int rc=TRUE;

   if (location == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_LOCATION_IN_HDR),
            "HTTP", url);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return TRUE;
   }

   if (gnForwardCount >= 5) {
      sprintf(gszMsgBox, TgLoadString(STID_LNK_FWD_5_TIMES_LOAD_ABORT),
            "HTTP", url);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return TRUE;
   }
   dup_location = UtilStrDup(location);
   if (dup_location == NULL) {
      FailAllocMessage();
      return TRUE;
   }
   if (psz_final_url != NULL && cb_final_url > 0) {
      UtilStrCpyN(psz_final_url, cb_final_url, dup_location);
   }
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_FORWARDING_URL_TO_URL),
         "HTTP", url, location);
   Msg(gszMsgBox);
   ShowRemoteStatus(gszMsgBox);
   /*
    * After calling ResetRemoteBuf(),
    *     can only return TRUE if buffer is filled.
    */
   ResetRemoteBuf(ppsz_buf, ppsz_content_type, pn_buf_sz, pn_html);
   gnForwardCount++;
   rc = LoadRemoteFileInMem(dup_location, ppsz_buf, ppsz_content_type,
         pn_buf_sz, pn_html, force_load, psz_final_url, cb_final_url);
   gnForwardCount--;
   free(dup_location);
   return rc;
}

static
char *ParseAuthFields(psz_scheme)
   char *psz_scheme;
{
   char *realm_ptr, *realm_value;

   UtilTrimBlanks(psz_scheme);
   realm_ptr = strchr(psz_scheme, ' ');
   if (realm_ptr != NULL) {
      *realm_ptr++ = '\0';
      while (*realm_ptr == ' ') {
         realm_ptr++;
      }
      if (*realm_ptr == '\0') {
         return NULL;
      } else if ((realm_value=strchr(realm_ptr, '=')) != NULL) {
         return ++realm_value;
      }
   }
   return NULL;
}

static
int LoadAuthenticatedFileInMem(url, host, port, ppsz_buf, ppsz_content_type,
      pn_buf_sz, pn_html, force_load)
   char *url, *host, **ppsz_buf, **ppsz_content_type;
   int port, *pn_buf_sz, *pn_html, force_load;
{
   char *www_authenticate=HttpHeaderGetWWWAuthentication();
   char *scheme=NULL, *realm=NULL, *encoded_auth=NULL;
   int rc=TRUE;

   if (www_authenticate == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_WWW_AUTHEN_IN_HDR),
            "HTTP", url);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return TRUE;
   }
   scheme = UtilStrDup(www_authenticate);
   if (scheme == NULL) {
      FailAllocMessage();
      return TRUE;
   }
   if ((realm=ParseAuthFields(scheme)) == NULL) {
      free(scheme);
      return TRUE;
   }
   /* realm has double-quotes around it */
   if ((encoded_auth=FindAuthorization(host, port, scheme, realm)) != NULL) {
      /* committed authorization is cached */
      /*
       * After calling ResetRemoteBuf(),
       *     can only return TRUE if buffer is filled.
       */
      ResetRemoteBuf(ppsz_buf, ppsz_content_type, pn_buf_sz, pn_html);
      SetAuthorization(host, port, scheme, realm, encoded_auth);
      gnAuthCount++;
      rc = LoadRemoteFileInMem(url, ppsz_buf, ppsz_content_type, pn_buf_sz,
            pn_html, force_load, NULL, 0);
      gnAuthCount--;
      ResetAuthorization();
   } else {
      char *psz=NULL, user_name[MAXSTRING+1], password[MAXSTRING+1];

      *user_name = *password = '\0';
      sprintf(gszMsgBox, TgLoadString(STID_AUTHEN_REQ_FOR_REALM),
            "HTTP", realm);
      ShowRemoteStatus(gszMsgBox);
      sprintf(gszMsgBox, TgLoadString(STID_ENTER_USERNAME_FOR_REALM), realm);
      if (mainDisplay == NULL) {
         fprintf(stdout, "%s ", gszMsgBox);
         fflush(stdout);
         psz = UtilGetALine(stdin);
         if (psz != NULL) {
            UtilStrCpyN(user_name, sizeof(user_name), psz);
            UtilFree(psz);
         }
      } else {
         Dialog(gszMsgBox, TgLoadCachedString(CSTID_DLG_ACCEPT_CANCEL),
               user_name);
      }
      UtilTrimBlanks(user_name);
      if (*user_name != '\0') {
         *password = '\0';
         sprintf(gszMsgBox, TgLoadString(STID_ENTER_PASSWORD_FOR_REALM), realm);
         doPassword = TRUE;
         if (mainDisplay == NULL) {
            fprintf(stdout, "%s ", gszMsgBox);
            fflush(stdout);
            psz = UtilGetALine(stdin);
            if (psz != NULL) {
               UtilStrCpyN(password, sizeof(password), psz);
               UtilFree(psz);
            }
         } else {
            Dialog(gszMsgBox, NULL, password);
         }
         doPassword = FALSE;
      }
      if (*user_name != '\0' && *password != '\0') {
         char *tmp_auth;

         sprintf(gszMsgBox, "%s:%s", user_name, password);
         if ((tmp_auth=UtilStrDup(gszMsgBox)) == NULL) FailAllocMessage();
         if ((encoded_auth=Base64Encode(tmp_auth)) == NULL) FailAllocMessage();
         free(tmp_auth);
         if (!SetAuthorization(host, port, scheme, realm, encoded_auth)) {
            FailAllocMessage();
         } else {
            /*
             * After calling ResetRemoteBuf(),
             *     can only return TRUE if buffer is filled.
             */
            ResetRemoteBuf(ppsz_buf, ppsz_content_type, pn_buf_sz, pn_html);
            SetAuthorization(host, port, scheme, realm, encoded_auth);
            gnAuthCount++;
            rc = LoadRemoteFileInMem(url, ppsz_buf, ppsz_content_type,
                  pn_buf_sz, pn_html, force_load, NULL, 0);
            gnAuthCount--;
            ResetAuthorization();
         }
         free(encoded_auth);
      }
   }
   free(scheme);
   return rc;
}

int LoadRemoteFileInMem(url, ppsz_buf, ppsz_content_type, pn_buf_sz, pn_html,
      force_load, psz_final_url, cb_final_url)
   char *url, **ppsz_buf, **ppsz_content_type, *psz_final_url;
   int *pn_buf_sz, *pn_html, force_load, cb_final_url;
   /* if the remote type is http, the url has an extension and it */
   /*       is not html but the retrieved file is html, then it's */
   /*       probably an error */
{
   int status=0, rc;
   int port=0, protocol_is_http=FALSE, http_extracted_text=FALSE;
   char *protocol=NULL, *host=NULL, *path=NULL, *proxy_host=NULL;
   char port_str[20];
   struct URLCacheRec *url_cache=NULL;

   *port_str = '\0';
   if (pn_buf_sz != NULL) *pn_buf_sz = 0;
   if (pn_html != NULL) *pn_html = FALSE;
   *ppsz_buf = NULL;
   
   if (!force_load) url_cache = FindURLCache(url, TRUE);
   if (url_cache != NULL && url_cache->remote_buf_sz > 0 &&
         url_cache->remote_buf != NULL) {
      *ppsz_buf = (char*)malloc((url_cache->remote_buf_sz)*sizeof(char));
      if (*ppsz_buf == NULL) FailAllocMessage();
      memcpy(*ppsz_buf, url_cache->remote_buf, url_cache->remote_buf_sz);
      *pn_buf_sz = url_cache->remote_buf_sz;
      if (ppsz_content_type != NULL) {
         *ppsz_content_type = UtilStrDup(url_cache->content_type);
      }
      *pn_html = url_cache->is_html;
      return TRUE;
   }
   status = ParseURL(url, &protocol, &host, &port, &path);
   if (status != TG_REMOTE_STATUS_OK) {
      if (protocol != NULL) free(protocol);
      if (host != NULL) free(host);
      if (path != NULL) free(path);
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_PARSE_GIVEN_URL), url);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      return FALSE;
   }
   if (UtilStrICmp(protocol, "http") == 0) {
      protocol_is_http = TRUE;
      status = LoadHttpIntoMem(url, host, port, path, ppsz_buf,
            ppsz_content_type, pn_buf_sz, pn_html, &http_extracted_text);
   } else if (UtilStrICmp(protocol, "ftp") == 0) {
      status = LoadFtpIntoMem(url, host, port, path, ppsz_buf,
            ppsz_content_type, pn_buf_sz, pn_html);
   } else {
      if (debugRemote) DumpURL(host, port, path);
      sprintf(gszMsgBox, TgLoadString(STID_TOOL_NOT_KNOW_HOW_TALK_PROTO),
            protocol);
      if (PRTGIF) {
         fprintf(stderr, "%s\n", gszMsgBox);
      } else {
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
      status = TG_REMOTE_STATUS_FORMAT;
   }
   rc = TRUE;
   if (status==TG_REMOTE_STATUS_OK && protocol_is_http && http_extracted_text) {
      int resp_code=HttpHeaderGetResponseCode();

      switch (resp_code) {
      case 301:
      case 302:
         rc = LoadForwardedFileInMem(url, ppsz_buf, ppsz_content_type,
               pn_buf_sz, pn_html, force_load, psz_final_url, cb_final_url);
         break;
      case 401:
         rc = LoadAuthenticatedFileInMem(url, host, port,
               ppsz_buf, ppsz_content_type, pn_buf_sz, pn_html, force_load);
         break;
      default:
         if (gnAuthCount > 0) {
            /* password verified? */
            CommitAuthorization();
         }
         break;
      }
   }
   if (status == TG_REMOTE_STATUS_OK) {
      UncompressTgifFile(url, ppsz_buf, pn_buf_sz, pn_html);
      UpdateURLCache(url, *ppsz_buf,
            (ppsz_content_type==NULL ? NULL : *ppsz_content_type),
            *pn_buf_sz, *pn_html);
   }
   if (protocol != NULL) free(protocol);
   if (host != NULL) free(host);
   if (path != NULL) free(path);
   if (proxy_host != NULL) free(proxy_host);
   return rc;
}

static
void LaunchViewer(launch_remote_file, psz_viewer, psz_url,
      psz_new_fname)
   int launch_remote_file;
   char *psz_viewer, *psz_url, *psz_new_fname;
{
   if (strcmp(psz_viewer, "NONE") != 0) {
#ifndef _BACKGROUND_DONT_FORK
      int pid=0;
#endif /* ~_BACKGROUND_DONT_FORK */
      char cmd[MAXSTRING<<2];

      if (launch_remote_file) {
         sprintf(cmd, psz_viewer, psz_url);
      } else {
         sprintf(cmd, psz_viewer, psz_new_fname);
      }
      sprintf(gszMsgBox, TgLoadString(STID_LAUNCH_GIVEN_VIEWER), cmd);
      Msg(gszMsgBox);
      ShowRemoteStatus(gszMsgBox);

#ifdef _BACKGROUND_DONT_FORK
      strcat(cmd, " &");
      (void)system(cmd);
#else /* ~_BACKGROUND_DONT_FORK */
      pid = fork();
      if (pid == 0) {
         (void)system(cmd);
         exit(0);
      }
#endif /* _BACKGROUND_DONT_FORK */
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_FILE_SAVED), psz_new_fname);
      Msg(gszMsgBox);
   }
}

struct MimeTypeRec {
   char *main_type, *sub_type;
   struct MimeTypeRec *next;
};

struct MailCapRec {
   char *main_type, *sub_type;
   char *cmd, *params;
   struct MailCapRec *next, *prev;
};

static struct MimeTypeRec *topMimeTypesInfo=NULL;
static struct MailCapRec *topMailCapInfo=NULL, *botMailCapInfo=NULL;

static
void AddAMimeType(main_type, sub_type)
   char *main_type, *sub_type;
{
   struct MimeTypeRec *mime_type_ptr;

   mime_type_ptr = (struct MimeTypeRec *)malloc(sizeof(struct MimeTypeRec));
   if (mime_type_ptr == NULL) { FailAllocMessage(); return; }
   memset(mime_type_ptr, 0, sizeof(struct MimeTypeRec));
   mime_type_ptr->main_type = UtilStrDup(main_type);
   mime_type_ptr->sub_type = UtilStrDup(sub_type);
   mime_type_ptr->next = topMimeTypesInfo;
   topMimeTypesInfo = mime_type_ptr;
}

static
void AddAMailCap(main_type, sub_type, cmd, params)
   char *main_type, *sub_type, *cmd, *params;
{
   struct MailCapRec *mail_cap_ptr;

   mail_cap_ptr = (struct MailCapRec *)malloc(sizeof(struct MailCapRec));
   if (mail_cap_ptr == NULL) { FailAllocMessage(); return; }
   memset(mail_cap_ptr, 0, sizeof(struct MailCapRec));
   mail_cap_ptr->main_type = UtilStrDup(main_type);
   mail_cap_ptr->sub_type = UtilStrDup(sub_type);
   mail_cap_ptr->cmd = UtilStrDup(cmd);
   mail_cap_ptr->params = (params==NULL ? NULL : UtilStrDup(params));
   mail_cap_ptr->prev = botMailCapInfo;
   mail_cap_ptr->next = NULL;
   if (botMailCapInfo == NULL) {
      topMailCapInfo = mail_cap_ptr;
   } else {
      botMailCapInfo->next = mail_cap_ptr;
   }
   botMailCapInfo = mail_cap_ptr;
}

static
void FreeMimeTypesInfo()
{
   struct MimeTypeRec *next_mime_type_ptr;

   for ( ; topMimeTypesInfo != NULL; topMimeTypesInfo=next_mime_type_ptr) {
      next_mime_type_ptr = topMimeTypesInfo->next;
      if (topMimeTypesInfo->main_type!=NULL) free(topMimeTypesInfo->main_type);
      if (topMimeTypesInfo->sub_type!=NULL) free(topMimeTypesInfo->sub_type);
      free(topMimeTypesInfo);
   }
}

static
void FreeMailCapInfo()
{
   struct MailCapRec *next_mail_cap_ptr;

   for ( ; topMailCapInfo != NULL; topMailCapInfo=next_mail_cap_ptr) {
      next_mail_cap_ptr = topMailCapInfo->next;
      if (topMailCapInfo->main_type != NULL) free(topMailCapInfo->main_type);
      if (topMailCapInfo->sub_type != NULL) free(topMailCapInfo->sub_type);
      if (topMailCapInfo->cmd != NULL) free(topMailCapInfo->cmd);
      if (topMailCapInfo->params != NULL) free(topMailCapInfo->params);
      free(topMailCapInfo);
   }
   botMailCapInfo = NULL;
}

static
void DumpMimeTypesInfo()
{
   struct MimeTypeRec *mime_type_ptr;

   /* do not translate -- program constants */
   fprintf(stderr, "***  Mime Types  ***\n");
   for (mime_type_ptr=topMimeTypesInfo; mime_type_ptr != NULL;
         mime_type_ptr=mime_type_ptr->next) {
      fprintf(stderr, "%s/%s\n",
            mime_type_ptr->main_type != NULL ? mime_type_ptr->main_type : "",
            mime_type_ptr->sub_type != NULL ? mime_type_ptr->sub_type : "");
   }
   fprintf(stderr, "\n");
}

static
void DumpMailCapInfo()
{
   struct MailCapRec *mail_cap_ptr;

   /* do not translate -- program constants */
   fprintf(stderr, "***  Mail Capabilities  ***\n");
   for (mail_cap_ptr=topMailCapInfo; mail_cap_ptr != NULL;
         mail_cap_ptr=mail_cap_ptr->next) {
      fprintf(stderr, "%s/%s; %s; %s\n",
            mail_cap_ptr->main_type != NULL ? mail_cap_ptr->main_type : "",
            mail_cap_ptr->sub_type != NULL ? mail_cap_ptr->sub_type : "",
            mail_cap_ptr->cmd != NULL ? mail_cap_ptr->cmd : "",
            mail_cap_ptr->params != NULL ? mail_cap_ptr->params : "");
   }
   fprintf(stderr, "\n");
}

static
void FillMimeTypesInfo()
{
   char *c_ptr, *buf, fname[MAXPATHLENGTH+1];
   FILE *fp;
   int line_num=0;

   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"MimeTypesFile")) != NULL) {
      strcpy(fname, c_ptr);
   } else {
      sprintf(fname, "%s%c.mime.types", homeDir, DIR_SEP);
   }
   if ((fp=fopen(fname, "r")) == NULL) return;
   while ((buf=UtilGetALine(fp)) != NULL) {
      line_num++;
      if (*buf != '#') {
         char *c_ptr1=strtok(buf, " \t\n\r");

         if (c_ptr1 != NULL && *c_ptr1 != '\0') {
            int ok=TRUE;
            char *sub_type=NULL;

            if ((sub_type=strchr(c_ptr1, '/')) == NULL) {
               ok = FALSE;
            } else {
               *sub_type++ = '\0';
               if (*sub_type == '\0') ok = FALSE;
               if (*c_ptr1 == '*') ok = FALSE;
            }
            if (!ok) {
               sprintf(gszMsgBox,
                     TgLoadCachedString(CSTID_MALFORMED_LINE_NUM_IN_FILE),
                     line_num, fname);
               Msg(gszMsgBox);
            } else {
               AddAMimeType(c_ptr1, sub_type);
            }
         }
      }
      free(buf);
   }
   fclose(fp);
   if (debugRemote) DumpMimeTypesInfo();
}

static
void FillMailCapInfo()
{
   char *spec=NULL, *fname=NULL, *colon_ptr=NULL, *c_ptr=NULL;

   if ((c_ptr=getenv("MAILCAPS")) != NULL) {
      spec = UtilStrDup(c_ptr);
   } else {
      sprintf(gszMsgBox, "%s%c.mailcap", homeDir, DIR_SEP);
      spec = UtilStrDup(gszMsgBox);
   }
   if (spec == NULL) FailAllocMessage();

   fname = spec;
   colon_ptr = strchr(fname, ':');
   while (fname != NULL) {
      if (colon_ptr != NULL) *colon_ptr = '\0';
      if (*fname != '\0') {
         FILE *fp=NULL;

         if ((fp=fopen(fname, "r")) == NULL) {
            sprintf(gszMsgBox, TgLoadString(STID_CANT_OPEN_GIVEN_MAILCAP_FILE),
                  fname);
            Msg(gszMsgBox);
         } else {
            int line_num=0;
            char *buf=NULL;

            while ((buf=UtilGetAContinuedLine(fp)) != NULL) {
               line_num++;
               UtilTrimBlanks(buf);
               if (*buf != '\0' && *buf != '#') {
                  char *first_semi_ptr=strchr(buf, ';');
                  char *second_semi_ptr=NULL, *sub_type=NULL;
                  int ok=TRUE;

                  if (first_semi_ptr == NULL) {
                     ok = FALSE;
                  } else {
                     *first_semi_ptr++ = '\0';
                     while (*first_semi_ptr == ' ' && *first_semi_ptr == '\t') {
                        first_semi_ptr++;
                     }
                     second_semi_ptr = strchr(first_semi_ptr, ';');
                     if (second_semi_ptr != NULL) {
                        *second_semi_ptr++ = '\0';
                     }
                     if ((sub_type=strchr(buf, '/')) == NULL) {
                        ok = FALSE;
                     } else {
                        *sub_type++ = '\0';
                     }
                  }
                  if (!ok) {
                     sprintf(gszMsgBox,
                           TgLoadCachedString(CSTID_MALFORMED_LINE_NUM_IN_FILE),
                           line_num, fname);
                     Msg(gszMsgBox);
                  } else {
                     if (first_semi_ptr!=NULL) UtilTrimBlanks(first_semi_ptr);
                     if (second_semi_ptr!=NULL) UtilTrimBlanks(second_semi_ptr);
                     AddAMailCap(buf, sub_type, first_semi_ptr,
                           second_semi_ptr);
                  }
               }
               free(buf);
            }
            fclose(fp);
         }
      }
      if (colon_ptr == NULL) break;
      *colon_ptr++ = ':';
      fname = colon_ptr;
      colon_ptr = strchr(fname, ':');
   }
   UtilFree(spec);
   if (debugRemote) DumpMailCapInfo();
}

static
void MalformedMailCapEntryMessage(mail_cap_ptr)
   struct MailCapRec *mail_cap_ptr;
{
   if (mail_cap_ptr->params == NULL) {
      sprintf(gszMsgBox, TgLoadString(STID_MALFORMED_MAILCAP_ENTRY),
            mail_cap_ptr->main_type, mail_cap_ptr->sub_type, mail_cap_ptr->cmd);
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_MALFORMED_MAILCAP_ENTRY_PARAM),
            mail_cap_ptr->main_type, mail_cap_ptr->sub_type, mail_cap_ptr->cmd,
            mail_cap_ptr->params);
   }
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
}

static
char *GetContentValue(name, params)
   char *name, *params;
{
   char *buf, *c_ptr;

   if (params == NULL) return NULL;

   buf = UtilStrDup(params);
   if (buf == NULL) { FailAllocMessage(); return NULL; }
   for (c_ptr=strtok(buf, ";"); c_ptr != NULL; c_ptr=strtok(NULL, ";")) {
      char *equal_ptr=strchr(c_ptr, '-');

      if (equal_ptr != NULL) {
         char *c_ptr1=c_ptr;

         while (*c_ptr1 == ' ' || *c_ptr1 == '\t') c_ptr1++;
         *equal_ptr = '\0';
         if (UtilStrICmp(c_ptr1, name) == 0) {
            char *return_buf=UtilStrDup(&equal_ptr[1]);

            free(buf);
            return return_buf;
         }
         *equal_ptr = '=';
      }
   }
   free(buf);
   return NULL;
}

static int mimeViewerInitialized=FALSE;

static
char *GetMimeViewer(psz_content_type)
   char *psz_content_type;
{
   struct MailCapRec *mail_cap_ptr;
   struct MimeTypeRec *mime_type_ptr;
   char *main_type=UtilStrDup(psz_content_type), *sub_type=NULL;
   char *params=NULL, *buf=NULL, *c_ptr;
   int buf_sz=0, buf_index=0;

   if (main_type == NULL) { FailAllocMessage(); return NULL; }
   UtilTrimBlanks(main_type);
   params = strchr(main_type, ';');
   if (params != NULL) *params++ = '\0';

   if (UtilStrICmp(main_type, "application/x-tgif") == 0) {
      free(main_type);
      return NULL;
   }
   sub_type = strchr(main_type, '/');
   if (sub_type == NULL) { free(main_type); return NULL; }
   *sub_type++ = '\0';
   if (UtilStrICmp(main_type, "multipart") == 0) {
      free(main_type);
      sprintf(gszMsgBox, TgLoadString(STID_TOOL_CANT_HANDLE_MP_MIME),
            TOOL_NAME);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return NULL;
   }

   if (!mimeViewerInitialized) {
      mimeViewerInitialized = TRUE;
      FillMimeTypesInfo();
      FillMailCapInfo();
   }
   for (mime_type_ptr=topMimeTypesInfo; mime_type_ptr != NULL;
         mime_type_ptr=mime_type_ptr->next) {
      if (UtilStrICmp(main_type, mime_type_ptr->main_type) == 0) {
         if (strcmp(mime_type_ptr->sub_type, "*") == 0) {
            break;
         } else if (UtilStrICmp(sub_type, mime_type_ptr->sub_type) == 0) {
            break;
         }
      }
   }
   if (mime_type_ptr == NULL) { free(main_type); return NULL; }

   for (mail_cap_ptr=topMailCapInfo; mail_cap_ptr != NULL;
         mail_cap_ptr=mail_cap_ptr->next) {
      if (UtilStrICmp(main_type, mail_cap_ptr->main_type) == 0) {
         if (strcmp(mail_cap_ptr->sub_type, "*") == 0) {
            break;
         } else if (UtilStrICmp(sub_type, mail_cap_ptr->sub_type) == 0) {
            break;
         }
      }
   }
   free(main_type);
   if (mail_cap_ptr == NULL) return NULL;

   buf_sz = 0x100;
   buf = (char*)malloc((buf_sz+1)*sizeof(char));
   if (buf == NULL) { FailAllocMessage(); return NULL; }
   c_ptr = mail_cap_ptr->cmd;
   while (*c_ptr != '\0') {
      if (*c_ptr == '\\' && c_ptr[1] == '\0') {
         MalformedMailCapEntryMessage(mail_cap_ptr);
         free(buf);
         return NULL;
      }
      if (*c_ptr == '%') {
         char *tmp_buf=NULL, *right_bracket;
         int num_ch_to_skip=(-1), len;

         switch (c_ptr[1]) {
         case 's': tmp_buf=UtilStrDup("%s"); num_ch_to_skip=2; break;
         case 'S': tmp_buf=UtilStrDup("%S"); num_ch_to_skip=2; break;
         case 't':
            len = strlen(main_type)+strlen(sub_type)+1;
            tmp_buf = (char*)malloc((len+1)*sizeof(char));
            if (tmp_buf == NULL) FailAllocMessage();
            sprintf(tmp_buf, "%s/%s", main_type, sub_type);
            num_ch_to_skip = 2;
            break;
         case '{':
            if ((right_bracket=strchr(&c_ptr[2], '}')) == NULL) {
               if (mail_cap_ptr->params == NULL) {
                  sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_RT_BRACK_MC),
                        mail_cap_ptr->main_type, mail_cap_ptr->sub_type,
                        mail_cap_ptr->cmd);
               } else {
                  sprintf(gszMsgBox,
                        TgLoadString(STID_CANT_FIND_RT_BRACK_MC_PARAM),
                        mail_cap_ptr->main_type, mail_cap_ptr->sub_type,
                        mail_cap_ptr->cmd, mail_cap_ptr->params);
               }
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               free(buf);
               return NULL;
            }
            num_ch_to_skip = right_bracket-c_ptr+1;
            *right_bracket = '\0';
            if ((tmp_buf=GetContentValue(&c_ptr[2], params)) == NULL) {
               sprintf(gszMsgBox, TgLoadString(STID_CANT_FIND_NAMED_INFO_IN_CT),
                     "HTTP", &c_ptr[2]);
               MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
               *right_bracket = '}';
               free(buf);
               return NULL;
            }
            *right_bracket = '}';
            break;
         default:
            MalformedMailCapEntryMessage(mail_cap_ptr);
            free(buf);
            return NULL;
         }
         if (num_ch_to_skip < 0 || tmp_buf == NULL) {
            free(buf);
            FailAllocMessage();
            return NULL;
         }
         len = strlen(tmp_buf);
         while (buf_index+len+1 >= buf_sz) {
            buf_sz += 0x100;
            if ((buf=(char*)realloc(buf, buf_sz+1)) == NULL) return NULL;
         }
         strcpy(&buf[buf_index], tmp_buf);
         buf_index += len;

         free(tmp_buf);
         c_ptr += num_ch_to_skip;
      } else {
         if (buf_index+1 >= buf_sz) {
            buf_sz += 0x100;
            if ((buf=(char*)realloc(buf, buf_sz+1)) == NULL) return NULL;
         }
         if (*c_ptr == '\\') c_ptr++;
         buf[buf_index++] = *c_ptr++;
      }
   }
   buf[buf_index] = '\0';
   return buf;
}

static
int SaveTmpURLToFile(psz_url, no_viewer_found, tmp_fname, new_file_name)
   char *psz_url, *tmp_fname, *new_file_name;
   int no_viewer_found;
{
   char msg[MAXSTRING<<2];
   char *msg1=(char*)malloc((strlen(curDirIsLocal ? curDir : curLocalDir)+30) *
         sizeof(char));
   int rc=TG_REMOTE_STATUS_OK;

   if (msg1 == NULL) { FailAllocMessage(); return INVALID; }

   if (no_viewer_found) {
      sprintf(msg, TgLoadString(STID_NO_VIEWER_SAVE_GIVEN_STR_AS), psz_url);
   } else {
      sprintf(msg, TgLoadString(STID_SAVE_GIVEN_STR_AS), psz_url);
   }
   sprintf(msg1, TgLoadString(STID_WORKING_DIRECTORY_IS),
         (curDirIsLocal ? curDir : curLocalDir));
   *new_file_name = '\0';
   Dialog(msg, msg1, new_file_name);
   free(msg1);
   UtilTrimBlanks(new_file_name);

   if (*new_file_name == '\0') return INVALID;
   if (!OkayToCreateFile(new_file_name)) return INVALID;

   rc = UtilCopyFile(tmp_fname, new_file_name);
   if (rc != TG_REMOTE_STATUS_OK) {
      switch (rc) {
      case TG_REMOTE_STATUS_READ:
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_READING),
               tmp_fname);
         break;
      case TG_REMOTE_STATUS_WRITE:
         sprintf(gszMsgBox, TgLoadString(STID_CANNOT_OPEN_FILE_FOR_WRITING),
               new_file_name);
         break;
      case TG_REMOTE_STATUS_FILE:
         sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_WRITE_TO_FILE),
               new_file_name);
         break;
      default:
         strcpy(gszMsgBox, TgLoadString(STID_UNKNOWN_ERR_IN_COPYING_FILES));
         break;
      }
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return INVALID;
   }
   return rc;
}

static
int UseMimeViewer(psz_url, psz_content_type, tmp_fname)
   char *psz_url, *psz_content_type, *tmp_fname;
   /* returns FALSE if psz_url is to be interpreted as a tgif file or text */
   /* returns TRUE if external viewer is launched */
   /* returns INVALID if there's an error or if the user canceled */
{
   char new_file_name[MAXPATHLENGTH+1];
   char *viewer=NULL, *cmd=NULL, *big_s=NULL;
   int launch_remote_file=FALSE;

   if (psz_content_type == NULL ||
         UtilStrICmp(psz_content_type, "application/x-tgif") == 0 ||
         strncmp(psz_url, "http:", 5) != 0) {
      return FALSE;
   }
   sprintf(gszMsgBox, "%s", psz_url);
   Msg(gszMsgBox);
   sprintf(gszMsgBox, "    Content-type: %s", psz_content_type);
   Msg(gszMsgBox);
   /*
    * fprintf(stderr, "URL: %s\n", psz_url);
    * fprintf(stderr, "Content-Type: %s\n", psz_content_type);
    * fprintf(stderr, "tmp_fname: %s\n", tmp_fname);
    */
   viewer = GetMimeViewer(psz_content_type);

   if (viewer == NULL) {
      char *slash_ptr=(psz_content_type == NULL ? NULL :
            strchr(psz_content_type, '/'));

      if (slash_ptr != NULL) {
         *slash_ptr = '\0';
         if (UtilStrICmp(psz_content_type, "application") == 0 ||
               UtilStrICmp(psz_content_type, "audio") == 0 ||
               UtilStrICmp(psz_content_type, "image") == 0 ||
               UtilStrICmp(psz_content_type, "video") == 0 ||
               ((*psz_content_type == 'X' || *psz_content_type == 'x') &&
               psz_content_type[1] == '-')) {
            *slash_ptr = '/';
            SaveTmpURLToFile(psz_url, TRUE, tmp_fname, new_file_name);
            return TRUE;
         }
         *slash_ptr = '/';
      }
      return FALSE;
   }
   /*
    * fprintf(stderr, "Launch viewer: %s\n", viewer);
    */
   if ((big_s=strstr(viewer, "%S")) == NULL) {
      if (SaveTmpURLToFile(psz_url, FALSE, tmp_fname, new_file_name) ==
            INVALID) {
         free(viewer);
         return INVALID;
      }
      cmd = (char*)malloc((strlen(viewer)+strlen(new_file_name)+40) *
            sizeof(char));
   } else {
      big_s[1] = 's';
      launch_remote_file = TRUE;
      cmd = (char*)malloc((strlen(viewer)+strlen(psz_url)+40)*sizeof(char));
   }
   if (cmd == NULL) {
      FailAllocMessage();
      free(viewer);
      return INVALID;
   }
   LaunchViewer(launch_remote_file, viewer, psz_url, new_file_name);
   free(viewer);
   free(cmd);
   return TRUE;
}

static char *gpszViewerInfo=NULL;

static
char *GetViewer(ext_str)
   char *ext_str;
   /* caller needs to call free() if not return NULL */
{
   int ext_len=strlen(ext_str), value_len, orig_len;
   char resource_str[MAXSTRING+1];
   char *value=NULL, *c_ptr;

   for (c_ptr=gpszViewerInfo; c_ptr != NULL && *c_ptr != '\0'; ) {
      char *cr=strchr(c_ptr, '\n'), *next_cr=NULL;
      int cmp_result;

      if (cr == NULL) {
         free(gpszViewerInfo);
         gpszViewerInfo = NULL;
         return NULL;
      }
      *cr = '\0';
      cmp_result = UtilStrICmp(c_ptr, ext_str);
      *cr++ = '\n';
      next_cr = strchr(cr, '\n');
      if (next_cr == NULL) {
         free(gpszViewerInfo);
         gpszViewerInfo = NULL;
         return NULL;
      }
      if (cmp_result == 0) {
         char *return_str;

         *next_cr = '\0';
         return_str = (*cr == '\0' ? NULL : UtilStrDup(cr));
         *next_cr = '\n';
         return return_str;
      }
      c_ptr = &next_cr[1];
   }
   sprintf(resource_str, "%sViewer", ext_str);
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,resource_str)) != NULL) {
      int count=0;

      value = (char*)malloc((strlen(c_ptr)+2)*sizeof(char));
      if (value == NULL) {
         FailAllocMessage();
         return NULL;
      }
      sprintf(value, "%s\n", c_ptr);
      for (c_ptr=strstr(value,"%s"); c_ptr != NULL;
            c_ptr=strstr(&c_ptr[2],"%s")) {
         count++;
      }
      for (c_ptr=strstr(value,"%S"); c_ptr != NULL;
            c_ptr=strstr(&c_ptr[2],"%S")) {
         count++;
      }
      if (count > 1) {
         sprintf(gszMsgBox, TgLoadString(STID_INVALID_XDEF), TOOL_NAME,
               resource_str, value);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
         strcpy(value, "\n");
      }
   } else {
      if ((value=UtilStrDup("\n")) == NULL) return NULL;
   }
   value_len = strlen(value);
   orig_len = (gpszViewerInfo == NULL ? 0 : strlen(gpszViewerInfo));
   if ((gpszViewerInfo=(gpszViewerInfo == NULL ?
         (char*)malloc((orig_len+ext_len+1+value_len+2)*sizeof(char)) :
         (char*)realloc(gpszViewerInfo, orig_len+ext_len+1+value_len+2))) ==
         NULL) {
      FailAllocMessage();
      return NULL;
   }
   sprintf(&gpszViewerInfo[orig_len], "%s\n%s", ext_str, value);
   if (value[value_len-1] == '\n') value[--value_len] = '\0';
   if (*value == '\0') {
      free(value);
      return NULL;
   }
   return value;
}

static
int UseViewer(psz_url, psz_content_type, tmp_fname)
   char *psz_url, *psz_content_type, *tmp_fname;
   /* returns FALSE if psz_url is to be interpreted as a tgif file or text */
   /* returns TRUE if external viewer is launched */
   /* returns INVALID if there's an error or if the user canceled */
{
   char *ext_str=UtilStrRChr(psz_url, (int)'/');
   char new_file_name[MAXPATHLENGTH+1];
   char *viewer=NULL, *cmd=NULL, *big_s=NULL, *separator, saved_ch='\0';
   int launch_remote_file=FALSE;

   if (ext_str == NULL) {
      return UseMimeViewer(psz_url, psz_content_type, tmp_fname);
   }
   for (separator=(&ext_str[1]); *separator != '\0'; separator++) {
      if (*separator == '#' || *separator == '?') {
         saved_ch = *separator;
         *separator = '\0';
         break;
      }
   }
   if ((ext_str=strchr(ext_str, '.')) == NULL) {
      if (separator != NULL) *separator = saved_ch;
      return UseMimeViewer(psz_url, psz_content_type, tmp_fname);
   }
   ext_str++;
   viewer = GetViewer(ext_str);
   if (separator != NULL) *separator = saved_ch;
   if (viewer == NULL) {
      return UseMimeViewer(psz_url, psz_content_type, tmp_fname);
   }
   if ((big_s=strstr(viewer, "%S")) == NULL) {
      if (SaveTmpURLToFile(psz_url, FALSE, tmp_fname, new_file_name) ==
            INVALID) {
         free(viewer);
         return INVALID;
      }
      cmd = (char*)malloc((strlen(viewer)+strlen(new_file_name)+40) *
            sizeof(char));
   } else {
      big_s[1] = 's';
      launch_remote_file = TRUE;
      cmd = (char*)malloc((strlen(viewer)+strlen(psz_url)+40)*sizeof(char));
   }
   if (cmd == NULL) {
      FailAllocMessage();
      free(viewer);
      return INVALID;
   }
   LaunchViewer(launch_remote_file, viewer, psz_url, new_file_name);
   free(viewer);
   free(cmd);
   return TRUE;
}

int UseExternalViewer(is_html, psz_url, psz_content_type, tmp_fname)
   int is_html;
   char *psz_url, *psz_content_type, *tmp_fname;
   /* returns FALSE if psz_url is to be interpreted as a tgif file or text */
   /* returns TRUE if external viewer is launched */
   /* returns INVALID if there's an error or if the user canceled */
{
   char *viewer=NULL;
   int rc=FALSE;
   int url_is_html=UrlIsHtml(psz_url);

   if (url_is_html || !is_html) {
      if (UseViewer(psz_url, psz_content_type, tmp_fname) != FALSE) {
         rc = TRUE;
      }
   } else if (is_html && (viewer=GetViewer("html")) != NULL) {
      char new_file_name[MAXPATHLENGTH+1], *cmd=NULL, *big_s=NULL;
      int launch_remote_file=FALSE;

      if ((big_s=strstr(viewer, "%S")) == NULL) {
         if (SaveTmpURLToFile(psz_url, FALSE, tmp_fname, new_file_name) ==
               INVALID) {
            free(viewer);
            return INVALID;
         }
         cmd = (char*)malloc((strlen(viewer)+strlen(new_file_name)+40) *
               sizeof(char));
      } else {
         big_s[1] = 's';
         launch_remote_file = TRUE;
         cmd = (char*)malloc((strlen(viewer)+strlen(psz_url)+40)*sizeof(char));
      }
      if (cmd == NULL) {
         FailAllocMessage();
         free(viewer);
         rc = INVALID;
      } else {
         LaunchViewer(launch_remote_file, viewer, psz_url, new_file_name);
         free(viewer);
         free(cmd);
         rc = TRUE;
      }
   } else if (UseMimeViewer(psz_url, psz_content_type, tmp_fname) != FALSE) {
      rc = TRUE;
   }
   return rc;
}

int LoadRemoteFileFromMem(psz_url, psz_buf, psz_content_type, buf_sz, is_html)
   char *psz_url, *psz_buf, *psz_content_type;
   int buf_sz, is_html;
{
   char *tmp_fname=WriteRemoteFileIntoTemp(psz_buf, buf_sz, NULL);
   int navigating=navigatingBackAndForth, rc_loadfile;

   if (tmp_fname == NULL) return FALSE;

   if (UseExternalViewer(is_html, psz_url, psz_content_type, tmp_fname) !=
         FALSE) {
      unlink(tmp_fname);
      free(tmp_fname);
      return FALSE;
   }
   if (!navigating) {
      BeforeNavigate();
      navigatingBackAndForth = TRUE;
   }
   rc_loadfile = LoadFile(tmp_fname, (-1), FALSE);
   if (!navigating) navigatingBackAndForth = FALSE;

   if (!rc_loadfile) {
      NewProc();
      PasteString(psz_buf, TRUE, TRUE);
   }
   SetCurDir(psz_url);
   curFileDefined = TRUE;
   RedrawTitleWindow();
   sprintf(gszMsgBox, TgLoadCachedString(CSTID_CUR_FILE_IS), psz_url);
   Msg(gszMsgBox);
   unlink(tmp_fname);
   free(tmp_fname);
   if (!navigating) CommitNavigate();
   return TRUE;
}

static
void ModifyProxy(proxy_host)
   char *proxy_host;
{
   char *c_ptr, *host=NULL, *port;

   if ((c_ptr=strstr(proxy_host, "//")) == NULL) {
      if ((host=UtilStrDup(proxy_host)) == NULL) return;
   } else {
      if ((host=UtilStrDup(&c_ptr[2])) == NULL) return;
   }
   port = strchr(host, ':');
   if (port != NULL) *port++ = '\0';
   sprintf(proxy_host, "%s%s%s",
         host, port==NULL ? "" : ":", port==NULL ? "" : port);
   free(host);
}

void InitLocalPID()
{
   static int stInitialized=FALSE;

   if (!stInitialized) {
      int pid=(int)getpid();
      char sz_host_name[MAXSTRING];

      *gszLocalPID = *sz_host_name = '\0';
      if (gethostname(sz_host_name, sizeof(sz_host_name)) < 0) {
         fprintf(stderr, "%s\n",
               TgLoadString(STID_CANT_GETHOSTNAME_USE_LOCALH));
         strcpy(sz_host_name, "localhost");
      }
      sprintf(gszLocalPID, "%1d:%s", pid, sz_host_name);

      stInitialized = TRUE;
   }
}

void InitRemote()
{
   char *c_ptr=NULL;
   int val=0;

   InitLocalPID();

   autoHyperSpaceOnRemote = TRUE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"AutoHyperSpaceOnRemote")) !=
         NULL && UtilStrICmp(c_ptr, "false") == 0) {
      autoHyperSpaceOnRemote = FALSE;
   }
   allowLaunchInHyperSpace = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"AllowLaunchInHyperSpace")) !=
         NULL && UtilStrICmp(c_ptr, "true") == 0) {
      allowLaunchInHyperSpace = TRUE;
   }
   *gzipCmd = '\0';
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"GZipCmd")) == NULL) {
      strcpy(gzipCmd, "gzip -c");
   } else {
      strcpy(gzipCmd, c_ptr);
   }
   *gunzipCmd = '\0';
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"GUnZipCmd")) == NULL) {
      strcpy(gunzipCmd, "gunzip -c");
   } else {
      strcpy(gunzipCmd, c_ptr);
   }
   *uncompressCmd = '\0';
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"UncompressCmd")) == NULL) {
      strcpy(uncompressCmd, "uncompress -c");
   } else {
      strcpy(uncompressCmd, c_ptr);
   }
   *httpProxy = '\0';
   if ((c_ptr=getenv("http_proxy")) != NULL) {
      strcpy(httpProxy, c_ptr);
      ModifyProxy(httpProxy);
   } else if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"HttpProxy")) != NULL) {
      strcpy(httpProxy, c_ptr);
      ModifyProxy(httpProxy);
   } else {
      *httpProxy = '\0';
   }
   *ftpProxy = '\0';
   if ((c_ptr=getenv("ftp_proxy")) != NULL) {
      strcpy(ftpProxy, c_ptr);
      ModifyProxy(ftpProxy);
   } else if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"FtpProxy")) != NULL) {
      strcpy(ftpProxy, c_ptr);
      ModifyProxy(ftpProxy);
   } else {
      *ftpProxy = '\0';
   }
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DebugHttp")) != NULL &&
         sscanf(c_ptr, "%d", &val) == 1 && val >= 0) {
      HttpDebug(val);
   }
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"DebugFtp")) != NULL &&
         sscanf(c_ptr, "%d", &val) == 1 && val >= 0) {
      FtpDebug(val);
   }
   gnHttpKeepAlive = FALSE;
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"HttpKeepAlive")) != NULL &&
         UtilStrICmp(c_ptr, "true") == 0) {
      gnHttpKeepAlive = TRUE;
   }
   if ((c_ptr=XGetDefault(mainDisplay,TOOL_NAME,"FakedReferer")) != NULL) {
      HttpFakeReferer(c_ptr);
   }
   InitHttp();
   InitMime();
}

void CleanUpRemote()
{
   gnUserAgentInitialized = FALSE;
   *gszUserAgentName = '\0';
   CleanUpDownloadStats();
   CleanUpMime();
   CleanUpHttp();
   if (gpszViewerInfo != NULL) {
      free(gpszViewerInfo);
      gpszViewerInfo = NULL;
   }
   FreeMimeTypesInfo();
   FreeMailCapInfo();
}
