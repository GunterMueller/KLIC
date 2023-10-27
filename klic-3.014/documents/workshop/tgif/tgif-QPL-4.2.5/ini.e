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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/ini.e,v 1.6 2011/05/16 16:21:58 william Exp $
 */

#ifndef _INI_E_
#define _INI_E_

#ifdef _INCLUDE_FROM_INI_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_INI_C_*/

   /*
    * tgCleanUpProfile() frees all file information.
    */
extern void	tgCleanUpProfile ARGS_DECL((void));

   /*
    * tgFreeProfileString() must be called to free any string returned by
    *      tgGetProfileString().
    * NULL is always returned;
    */
extern char	* tgFreeProfileString ARGS_DECL((char *pszStr));

   /*
    * tgBeginFastProfile() turns off file time checking for pszFile.
    * tgEndFastProfile() turns on file time checking for pszFile.
    * Normally, any of the tgGet...() and tgWrite...() routines below checks
    *      to see if the INI file is modified.  If the INI file is modified,
    *      the cache is flushed before any action is take.
    * Surrounding a set of tgGet...() and tgWrite...() routines by a
    *      tgBeginFastProfile() and tgEndFastProfile() pair turns off file
    *      time checking.  This approach should only be used if the tg...()
    *      routines are called in a tight loop.
    */
extern void	tgBeginFastProfile ARGS_DECL((char *pszFile));
extern void	tgEndFastProfile ARGS_DECL((char *pszFile));

   /*
    * tgSetProfileStripQuotes() turns on/off single and double quotes stripping
    *      for pszFile.  By default, quotes-stripping is turned on (TRUE).  If
    *      you are using an encrypted file, you should call this function with
    *      bStripQuotes set to FALSE.
    * It returns the current quotes-stripping setting for pszFile.
    */
extern int	tgSetProfileStripQuotes ARGS_DECL((char *pszFile,
		                                   int bStripQuotes));

   /*
    * tgGetProfileString() works the same way as GetPrivateProfileString()
    *      except that when pszSection is NULL, all sections are returned (in
    *      the way similar to the case where pszEntry is NULL).
    * If the returned string is non-NULL, it must be freed by calling
    *      tgFreeProfileString();
    */
extern char	* tgGetProfileString ARGS_DECL((char *pszSection,
		                                char *pszEntry,
		                                char *pszFile));

   /*
    * tgGetProfileInt() works the same way as GetPrivateProfileInt() except
    *      that when pszSection is NULL, the information cached for pszFile is
    *      discarded.  If pszSection is NULL and the file can not be written,
    *      the returned value will be !nDefault.
    */
extern int	tgGetProfileInt ARGS_DECL((char *pszSection, char *pszEntry,
                                           int nDefault, char *pszFile));

   /*
    * tgWriteProfileString() works the same way as WritePrivateProfileString()
    *      except that if pszSection is not NULL, the write is doesn't update
    *      pszFile (it uses write-back instead of write-through).  Therefore,
    *      one should always call this function again with a NULL pszSection to
    *      force out the write.
    * Returns FALSE if memory allocation fails.
    */
extern int	tgWriteProfileString ARGS_DECL((char *pszSection,
		                                char *pszEntry,
		                                char *pszString,
		                                char *pszFile));

   /*
    * tgBeginDupKeySection() turns on duplicate key retrieval for the pszSection
    *      of pszFile.  The pszSection can NOT be NULL.
    * tgEndFastProfile() turns it off.
    * Normally, any of the tgGet...() and tgWrite...() routines behaves like
    *      their Windows API counter-parts where duplicate entries are removed.
    * Surrounding a set of tgGet...() and tgWrite...() routines by a
    *      tgBeginDupKeySection() and tgEndDupKeySection() enables retrieval of
    *      entries with duplicate keys.  This approach should only be used if
    *      the tg...() routines are called in a tight loop.  Also, the
    *      pszSection in the tg...() calls must be the same as the pszSection
    *      used in tgBeginDupKeySection() and tgEndDupKeySection().
    */
extern void	tgBeginDupKeySection ARGS_DECL((char *pszSection,
		                                char *pszFile));
extern void	tgEndDupKeySection ARGS_DECL((char *pszSection,
		                              char *pszFile));

   /*
    * tgBeginValuelessKeySection() turns on valueless key writes for the
    *      pszSection of pszFile.  The pszSection can NOT be NULL.  Examples of
    *      valueless key section are sections in Ios.ini.
    * tgBeginValuelessKeySection() returns TRUE if pszSection of pszFile is
    *      valueless.
    * tgEndFastProfile() sets the valueless key flag for pszSection of pszFile
    *      to be bValuelessKey.
    * These two functions has no effect on reads!
    *
    * Surrounding a set of tgWriteProfileString() calls by
    *      tgBeginValuelessKeySection() and tgEndValuelessKeySection() enables
    *      writes of lines with valueless keys.  The pszSection in the
    *      tgWriteProfileString() calls must use the same pszSection as the
    *      pszSection used in tgBeginValuelessKeySection() and
    *      tgEndValuelessKeySection(). Furthermore, tgWriteProfileString() must
    *      be called with NULL pszValue.  Also, please make sure that
    *      tgWriteProfileString() is called with a NULL pszSection to force out
    *      the write before tgEndValuelessKeySection() is called.
    *
    * The following is an example of how to use these functions.
    *
    *      BOOL bValuelessKey=faBeginValuelessKeySection(pszSection, pszFile);
    *
    *      tgWriteProfileString(pszSection, pszEntry1, NULL, pszFile);
    *      tgWriteProfileString(pszSection, pszEntry2, NULL, pszFile);
    *      ...
    *      tgWriteProfileString(NULL, NULL, NULL, pszFile);
    *
    *      tgEndValuelessKeySection(pszSection, pszFile, bValuelessKey);
    */
extern int	tgBeginValuelessKeySection ARGS_DECL((char *pszSection,
		                                      char *pszFile));
extern void	tgEndValuelessKeySection ARGS_DECL((char *pszSection,
		                                    char *pszFile,
		                                    int bValuelessKey));

   /*
    * tgIsValuelessKeySection() returns TRUE if pszSection of pszFile is a
    *      valueless key section.
    */
extern int	tgIsValuelessKeySection ARGS_DECL((char *pszSection,
		                                   char *pszFile));

   /*
    * tgDeleteDupKeyValue() deletes the key/value pair specified by
    *      pszEntry/pszValue in pszSection of pszFile.
    * Returns FALSE if any of the parameters is NULL.
    * Internally, this function calls tgBeginDupKeySection() and
    *      tgEndDupKeySection() but it does NOT write out the file.
    */
extern int	tgDeleteDupKeyValue ARGS_DECL((char *pszSection,
		                               char *pszEntry,
		                               char *pszValue,
		                               char *pszFile));

#ifdef _INCLUDE_FROM_INI_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_INI_C_*/

#endif /*_INI_E_*/
