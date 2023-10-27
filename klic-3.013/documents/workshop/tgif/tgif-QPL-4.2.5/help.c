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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/help.c,v 1.7 2011/05/16 16:21:57 william Exp $
 */

#define _INCLUDE_FROM_HELP_C_

#include "tgifdefs.h"
#include "patchlvl.h"

#include "color.e"
#include "dialog.e"
#include "file.e"
#include "help.e"
#include "menu.e"
#include "menuinfo.e"
#include "msg.e"
#include "navigate.e"
#include "remote.e"
#include "setup.e"
#include "strtbl.e"
#include "util.e"
#include "version.e"

#define HELP_ABOUT 0
#define HELP_COPYRIGHT 1
#define HELP_CURRENT 2

#define MAXHELPS 3

int numHelp=MAXHELPS;

/* ----------------------- About ----------------------- */

void About()
{
   char *c_ptr=NULL;

   GetTgifVersionAndPatchLevel(gszMsgBox, sizeof(gszMsgBox));
   c_ptr = (&gszMsgBox[strlen(gszMsgBox)]);
   sprintf(c_ptr, "\n\n%s", copyrightString);
   c_ptr = (&c_ptr[strlen(c_ptr)]);
   sprintf(c_ptr, TgLoadString(STID_HYPER_TEXT_HOME), homePageURL);
   c_ptr = (&c_ptr[strlen(c_ptr)]);
   sprintf(c_ptr, TgLoadString(STID_LATEST_REL_INFO), currentReleaseURL);
   c_ptr = (&c_ptr[strlen(c_ptr)]);
   sprintf(c_ptr, TgLoadString(STID_HYPER_GRAPHICS_INFO), hyperGraphicsURL);
   c_ptr = (&c_ptr[strlen(c_ptr)]);
   sprintf(c_ptr, TgLoadString(STID_MAILING_LIST_INFO),
         mailingList, joinMailingList, mailingURL);
   c_ptr = (&c_ptr[strlen(c_ptr)]);
   sprintf(c_ptr, TgLoadString(STID_SEND_BUG_REPORT_TO), "bill.cheng@acm.org");
   c_ptr = (&c_ptr[strlen(c_ptr)]);
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);

}

/* ----------------------- Copyright ----------------------- */

/* do not translate -- program constants */
static char *gszCopyright[] = {
   "",
#ifdef QPL_LICENSE
   "This software is distributed under the terms of the Q Public License",
   "as defined by Trolltech AS of Norway and appearing in the file",
   "LICENSE.QPL included in the packaging of this file.",
   "",
   "THIS SOFTWARE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING",
   "THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR",
   "PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,",
   "INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING",
   "FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,",
   "NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION",
   "WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.",
#else /* ~QPL_LICENSE */
   "William Cheng (\"Author\") grants to the party hereby receiving \"tgif\"",
   "(\"Recipient\") a non-exclusive, royalty-free license limited to copy,",
   "display, and distribute without charging for a fee, and produce derivative",
   "works of \"tgif\", provided that the above copyright notice appears in",
   "all copies made of \"tgif\" and both the copyright notice and this license",
   "appear in supporting documentation, and that the name of Author not be",
   "used in advertising or publicity pertaining to \"tgif\".  All other rights",
   "(including, but not limited to, the right to sell \"tgif\", the right to",
   "sell or distribute derivative",
   "works of \"tgif\", the right to distribute \"tgif\" for a fee, and",
   "the right to include \"tgif\" or derivative workds of \"tgif\" in a",
   "for-sale product) are reserved by the Author.",
   "",
   "\"Tgif\" is provided \"as is\" without express or implied warranty. Author",
   "does not and cannot warrant the performance of \"tgif\" or the results",
   "that may be obtained by its use or its fitness for any specific use by",
   "Recipient or any third party.  In no event shall Author become liable",
   "to Recipient or any other party, for any loss or damages,",
   "consequential or otherwise, including but not limited to time, money,",
   "or goodwill, arising from use or possession, authorized or not, of",
   "\"tgif\" by Recipient or any other party.",
#endif /* QPL_LICENSE */
   NULL
};

void Copyright()
{
   char **s_ptr=gszCopyright, *c_ptr=gszMsgBox;

   sprintf(c_ptr, "%s ", copyrightString);
   c_ptr = (&gszMsgBox[strlen(c_ptr)]);
   for ( ; *s_ptr != NULL; s_ptr++) {
      if (**s_ptr == '\0') {
         strcpy(c_ptr, "\n\n");
      } else {
         sprintf(c_ptr, "%s ", *s_ptr);
      }
      c_ptr = (&c_ptr[strlen(c_ptr)]);
   }
   MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
}

void LatestReleaseInfo()
{
   char tmp_fname[MAXPATHLENGTH+1], final_url[MAXPATHLENGTH+1];
   char current_url[MAXPATHLENGTH<<1];
   char *content_type=NULL, *page_spec=NULL;
   int is_html=FALSE, rc=FALSE;

   *final_url = '\0';
   strcpy(current_url, currentReleaseURL);
   navigateRefresh = TRUE;
   rc = DownloadRemoteFile(current_url, &content_type, &page_spec,
         &is_html, tmp_fname, final_url, sizeof(final_url));  
   navigateRefresh = FALSE;
   if (content_type != NULL) FreeRemoteBuf(content_type);
   if (page_spec != NULL) UtilFree(page_spec);
   if (rc) {
      FILE *fp=fopen(tmp_fname, "r");
      char version_str[MAXPATHLENGTH<<1];

      *version_str = '\0';
      if (fp == NULL) {
         sprintf(gszMsgBox, TgLoadString(STID_OPEN_URL_FAIL_USE_BROWSER),
               currentReleaseURL, currentReleaseURL);
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      } else {
         /* do not translate -- program constants */
         int len=strlen("The current release of tgif is"), scanning=TRUE;
         char *buf;

         while (scanning && (buf=UtilGetALine(fp)) != NULL) {
            /* do not translate -- program constants */
            if (strncmp(buf, "The current release of tgif is", len) == 0) {
               char *buf1=UtilGetALine(fp);

               scanning = FALSE;
               if (buf1 != NULL) {
                  /* do not translate -- program constants */
                  char *version_ptr=strstr(buf1, "Version"), *c_ptr=NULL;

                  if (version_ptr != NULL &&
                        (c_ptr=strchr(version_ptr, '<')) != NULL) {
                     *c_ptr = '\0';
                     strcpy(version_str, version_ptr);
                     *c_ptr = '<';
                  }
                  UtilFree(buf1);
               }
            }
            UtilFree(buf);
         }
         fclose(fp);

         if (*version_str == '\0') {
            sprintf(gszMsgBox, TgLoadString(STID_FIND_VER_FAIL_USE_BROWSER),
                  currentReleaseURL, currentReleaseURL);
         } else if (TGIF_PATCHLEVEL == 0) {
            if (*specialBuild == '\0') {
               sprintf(gszMsgBox, TgLoadString(STID_TOOL_CUR_VER_IS_DIFF_INFO),
                     TOOL_NAME, version_str, TOOL_NAME, versionString,
                     (*final_url=='\0' ? currentReleaseURL : final_url));
            } else {
               sprintf(gszMsgBox,
                     TgLoadString(STID_TOOL_CUR_VER_IS_DIFF_SPC_INFO),
                     TOOL_NAME, version_str, TOOL_NAME, versionString,
                     specialBuild,
                     (*final_url=='\0' ? currentReleaseURL : final_url));
            }
         } else {
            if (*specialBuild == '\0') {
               sprintf(gszMsgBox, TgLoadString(STID_TOOL_CUR_VER_PATCH_INFO),
                     TOOL_NAME, version_str, TOOL_NAME, versionString,
                     TGIF_PATCHLEVEL,
                     (*final_url=='\0' ? currentReleaseURL : final_url));
            } else {
               sprintf(gszMsgBox,
                     TgLoadString(STID_TOOL_CUR_VER_PATCH_SPC_INFO),
                     TOOL_NAME, version_str, TOOL_NAME, versionString,
                     TGIF_PATCHLEVEL, specialBuild,
                     (*final_url=='\0' ? currentReleaseURL : final_url));
            }
         }
         MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      }
   } else {
      sprintf(gszMsgBox, TgLoadString(STID_CANT_DOWNLOAD_VER_USE_BROWSER),
            currentReleaseURL, currentReleaseURL);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
   if (*tmp_fname != '\0') unlink(tmp_fname);
}

/* ----------------------- Init and Clean Up ----------------------- */

void CleanUpHelp()
{
}

void InitHelp()
{
}

/* ----------------------- Menu Functions ----------------------- */

void RefreshHelpMenu(menu)
   TgMenu *menu;
{
}

int HelpMenu(X, Y, TrackMenubar)
   int X, Y, TrackMenubar;
{
   int rc=INVALID;
   TgMenu *menu=(helpMenuInfo.create_proc)(NULL, X, Y, &helpMenuInfo, FALSE);

   activeMenu = MENU_HELP;
   if (menu != NULL) {
      menu->track_menubar = TrackMenubar;

      rc = TgMenuLoop(menu);
      TgDestroyMenu(menu, TRUE);
   }
   return rc;
}
