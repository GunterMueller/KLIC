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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/version.c,v 1.16 2011/05/16 16:22:00 william Exp $
 */

#define _INCLUDE_FROM_VERSION_C_

#include "tgifdefs.h"
#include "patchlvl.h"

#include "version.e"

char *versionString="4.2";
char *authorEmailString="bill.cheng@acm.org";
#ifdef QPL_LICENSE
char *copyrightString="Copyright (C) 2001-2011, William Chia-Wei Cheng";
#else /* ~QPL_LICENSE */
char *copyrightString="Copyright (C) 1990-2011, William Chia-Wei Cheng";
#endif /* QPL_LICENSE */
char *homePageURL="http://bourbon.usc.edu/tgif/";
char *oldMailingList="kragen-tgif@kragen.dnaco.net";
char *mailingList="tgif4-announce@yahoogroups.com";
char *joinMailingList="tgif4-announce-subscribe@yahoogroups.com";
char *mailingURL="http://bourbon.usc.edu/tgif/faq/mailings.html";
char *currentReleaseURL="http://bourbon.usc.edu/tgif/download.html";
char *hyperGraphicsURL="http://bourbon.usc.edu/tgif/index.obj";

char fullVersionString[MAXSTRING];
char fullToolName[MAXSTRING];
#ifdef QPL_LICENSE
char *specialBuild="QPL";
#else /* ~QPL_LICENSE */
char *specialBuild="";
#endif /* QPL_LICENSE */

void SetFullVersionString()
{
   if (*specialBuild == '\0') {
      sprintf(fullVersionString, "%s.%1d", versionString, TGIF_PATCHLEVEL);
   } else {
      sprintf(fullVersionString, "%s.%1d-%s", versionString, TGIF_PATCHLEVEL,
            specialBuild);
   }
   sprintf(fullToolName, "%s-%s", TOOL_NAME, fullVersionString);
}

void InitVersion()
{
}
