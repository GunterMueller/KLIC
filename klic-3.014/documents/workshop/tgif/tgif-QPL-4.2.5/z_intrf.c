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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/z_intrf.c,v 1.11 2011/06/18 04:44:51 william Exp $
 */

#define _INCLUDE_FROM_Z_INTRF_C_

#include "tgifdefs.h"

#if (!defined(_NO_ZLIB) || defined(HAVE_LIBZ))
#include <zlib.h>
#endif /* (!defined(_NO_ZLIB) || defined(HAVE_LIBZ)) */

#include "dialog.e"
#include "file.e"
#include "msg.e"
#include "setup.e"
#include "strtbl.e"
#include "util.e"
#include "z_intrf.e"

/* --------------------- Utility Functions --------------------- */

static
void ByteStuff(buf_in, buf_len, buf_out, pn_buf_out)
   unsigned char *buf_in, *buf_out;
   unsigned int buf_len, *pn_buf_out;
{
   int i=0, count=0;

   for (i=0; i < buf_len; i++, buf_in++) {
      switch (*buf_in) {
      case '\0':
         *buf_out++ = ((unsigned char)0xff);
         *buf_out++ = ((unsigned char)0x30);
         count += 2;
         break;
      case ((unsigned char)0xff):
         *buf_out++ = ((unsigned char)0xff);
         *buf_out++ = ((unsigned char)0xff);
         count += 2;
         break;
      default:
         *buf_out++ = *buf_in;
         count++;
         break;
      }
   }
   if (pn_buf_out != NULL) *pn_buf_out = count;
}

static
int ByteUnStuff(buf_in, buf_len, buf_out, pn_buf_out, pn_has_left_over)
   unsigned char *buf_in, *buf_out;
   int buf_len, *pn_buf_out, *pn_has_left_over;
{
   int i=0, count=0;

   if (*pn_has_left_over) {
      if (buf_len == 0) return TRUE;
      switch (*buf_in) {
      case ((unsigned char)0x30):
         *buf_out++ = ((unsigned char)'\0');
         count++;
         buf_in++;
         break;
      case ((unsigned char)0xff):
         *buf_out++ = ((unsigned char)0xff);
         count++;
         buf_in++;
         break;
      default: return FALSE;
      }
      *pn_has_left_over = FALSE;
   }
   for (i=count; i < buf_len; i++, buf_in++) {
      if (*buf_in == ((unsigned char)0xff)) {
         i++;
         buf_in++;
         if (i >= buf_len) {
            *pn_has_left_over = TRUE;
            if (pn_buf_out != NULL) *pn_buf_out = count;
            return TRUE;
         }
         switch (*buf_in) {
         case ((unsigned char)0x30):
            *buf_out++ = ((unsigned char)'\0');
            count++;
            break;
         case ((unsigned char)0xff):
            *buf_out++ = ((unsigned char)0xff);
            count++;
            break;
         default: return FALSE;
         }
      } else {
         *buf_out++ = *buf_in;
         count++;
      }
   }
   if (pn_buf_out != NULL) *pn_buf_out = count;

   return TRUE;
}

/* --------------------- HasZlibSupport() --------------------- */

int HasZlibSupport()
{
#if (!defined(_NO_ZLIB) || defined(HAVE_LIBZ))
   return TRUE;
#else /* ~(!defined(_NO_ZLIB) || defined(HAVE_LIBZ)) */
   return FALSE;
#endif /* (!defined(_NO_ZLIB) || defined(HAVE_LIBZ)) */
}

/* --------------------- DeflateFile() --------------------- */

int DeflateFile(fname, deflated_fname)
   char *fname, *deflated_fname;
   /* no byte stuffing */
{
#if (!defined(_NO_ZLIB) || defined(HAVE_LIBZ))
   int level=Z_DEFAULT_COMPRESSION;
   int ret=0, flush=0;
   unsigned int block_sz=0x4000;
   unsigned int have=0;
   z_stream strm;
   FILE *in_fp=NULL, *out_fp=NULL;
   unsigned char in[0x4000], out[0x4000];
   int bytes_left=0, input_size=0;
   struct stat stat_buf;
   ProgressInfo pi;

   if ((in_fp=fopen(fname, "r")) == NULL) {
      FailToOpenMessage(fname, "r", NULL);
      return FALSE;
   }
   if (fstat(fileno(in_fp), &stat_buf) != 0) {
      fclose(in_fp);
      sprintf(gszMsgBox, TgLoadString(STID_FAIL_TO_FSTAT_ABORT_COPY), fname);
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
      return FALSE;
   }
   bytes_left = input_size = stat_buf.st_size;
   if ((out_fp=fopen(deflated_fname, "w")) == NULL) {
      fclose(in_fp);
      FailToOpenMessage(deflated_fname, "w", NULL);
      return FALSE;
   }
   /* allocate deflate state */
   memset(&strm, 0, sizeof(strm));
   strm.zalloc = Z_NULL;
   strm.zfree = Z_NULL;
   strm.opaque = Z_NULL;
   ret = deflateInit(&strm, level);
   if (ret != Z_OK) {
      ZlibError(ret, TRUE);
      return FALSE;
   }
   BeginProgress(&pi, input_size);
   do {
      int bytes_to_read=min(block_sz, bytes_left);

      strm.avail_in = fread(in, 1, bytes_to_read, in_fp);
      if (ferror(in_fp)) {
         (void)deflateEnd(&strm);
         ZlibError(ret, TRUE);
         return FALSE;
      }
      bytes_left -= bytes_to_read;

      flush = (bytes_left == 0) ? Z_FINISH : Z_NO_FLUSH;
      strm.next_in = in;

      UpdateProgress(&pi, input_size-bytes_left);
      /*
       * run deflate() on input until output buffer not full, finish compression
       *       if all of in_fp has been read in or in_buf is exhausted
       */
      do {
         strm.avail_out = block_sz;
         strm.next_out = out;
         ret = deflate(&strm, flush);    /* no bad return value */
         TgAssert(ret != Z_STREAM_ERROR,
               "deflate() returns Z_STREAM_ERROR in DeflateFile()", NULL);
               /* make sure that state is not clobbered */
         have = block_sz - strm.avail_out;

         if (fwrite(out, 1, have, out_fp) != have || ferror(out_fp)) {
            (void)deflateEnd(&strm);
            ZlibError(ret, TRUE);
            return FALSE;
         }
      } while (strm.avail_out == 0);
      TgAssert(strm.avail_in == 0,
            "un-deflated data left in input buffer in DeflateFile()", NULL);
            /* make sure that all input are used */
    
      /* done when last data in file processed */
   } while (flush != Z_FINISH);
   EndProgress(&pi);

   TgAssert(ret == Z_STREAM_END,
         "end-of-stream not detected in DeflateFile()", NULL);
         /* make sure that stream is complete */
   deflateEnd(&strm);
   fclose(in_fp);
   fclose(out_fp);

   return TRUE;
#else /* ~(!defined(_NO_ZLIB) || defined(HAVE_LIBZ)) */
   return FALSE;
#endif /* (!defined(_NO_ZLIB) || defined(HAVE_LIBZ)) */
}

/* --------------------- DoDeflate() --------------------- */

int DoDeflate(in_fp, in_buf, bytes_left, out_fp, use_def_compression, use_byte_stuffing, pn_rc)
   /*
    * This function is adapted from <URL:http://www.zlib.net/zpipe.c>
    *
    * zpipe.c - example of proper use of zlib's inflate() and deflate()
    * Not copyrighted -- provided to the public domain
    * Version 1.4  11 December 2005  Mark Adler
    */
   FILE *in_fp, *out_fp;
   char *in_buf;
   int bytes_left, use_byte_stuffing, *pn_rc;
   int use_def_compression; /* always use Z_DEFAULT_COMPRESSION */
{
#if (!defined(_NO_ZLIB) || defined(HAVE_LIBZ))
   int level=Z_DEFAULT_COMPRESSION;
   int ret=0, flush=0;
   unsigned int block_sz=0x4000;
   unsigned int have=0;
   z_stream strm;
   unsigned char in[0x4000], out[0x4000];

   TgAssert(in_fp == NULL || in_buf == NULL,
         "in_fp and in_buf cannot be both NULL in DoDeflate()", NULL);
   /* allocate deflate state */
   memset(&strm, 0, sizeof(strm));
   strm.zalloc = Z_NULL;
   strm.zfree = Z_NULL;
   strm.opaque = Z_NULL;
   ret = deflateInit(&strm, level);
   if (ret != Z_OK) {
      if (pn_rc != NULL) *pn_rc = ret;
      return FALSE;
   }
   if (in_fp != NULL) rewind(in_fp);
   rewind(out_fp);
   do {
      int bytes_to_read=min(block_sz, bytes_left);

      if (in_fp != NULL) {
         strm.avail_in = fread(in, 1, bytes_to_read, in_fp);
         if (ferror(in_fp)) {
            (void)deflateEnd(&strm);
            if (pn_rc != NULL) *pn_rc = Z_ERRNO;
            return FALSE;
         }
      } else {
         memcpy(in, in_buf, bytes_to_read);
         strm.avail_in = bytes_to_read;
      }
      bytes_left -= bytes_to_read;

      flush = (bytes_left == 0) ? Z_FINISH : Z_NO_FLUSH;
      strm.next_in = in;

      /*
       * run deflate() on input until output buffer not full, finish compression
       *       if all of in_fp has been read in or in_buf is exhausted
       */
      do {
         strm.avail_out = block_sz;
         strm.next_out = out;
         ret = deflate(&strm, flush);    /* no bad return value */
         TgAssert(ret != Z_STREAM_ERROR,
               "deflate() returns Z_STREAM_ERROR in DoDeflate()", NULL);
               /* make sure that state is not clobbered */
         have = block_sz - strm.avail_out;

         if (use_byte_stuffing) {
            unsigned int have2=0;
            unsigned char out2[0x8000];

            have2 = 0;
            ByteStuff(out, have, out2, &have2);
            if (fwrite(out2, 1, have2, out_fp) != have2 || ferror(out_fp)) {
               (void)deflateEnd(&strm);
               if (pn_rc != NULL) *pn_rc = Z_ERRNO;
               return FALSE;
            }
         } else {
            if (fwrite(out, 1, have, out_fp) != have || ferror(out_fp)) {
               (void)deflateEnd(&strm);
               if (pn_rc != NULL) *pn_rc = Z_ERRNO;
               return FALSE;
            }
         }
      } while (strm.avail_out == 0);
      TgAssert(strm.avail_in == 0,
            "un-deflated data left in input buffer in DoDeflate()", NULL);
            /* make sure that all input are used */
    
      /* done when last data in file processed */
   } while (flush != Z_FINISH);

   TgAssert(ret == Z_STREAM_END,
         "end-of-stream not detected in DoDeflate()", NULL);
         /* make sure that stream is complete */
   deflateEnd(&strm);

   return TRUE;
#else /* ~(!defined(_NO_ZLIB) || defined(HAVE_LIBZ)) */
   return FALSE;
#endif /* (!defined(_NO_ZLIB) || defined(HAVE_LIBZ)) */
}

/* --------------------- DoInflate() --------------------- */

int DoInflate(in_buf, bytes_left, out_fp, use_byte_unstuffing, pn_rc)
   /*
    * This function is adapted from <URL:http://www.zlib.net/zpipe.c>
    *
    * zpipe.c - example of proper use of zlib's inflate() and deflate()
    * Not copyrighted -- provided to the public domain
    * Version 1.4  11 December 2005  Mark Adler
    */
   char *in_buf;
   int bytes_left, use_byte_unstuffing, *pn_rc;
   FILE *out_fp;
{
#if (!defined(_NO_ZLIB) || defined(HAVE_LIBZ))
   int ret=0, has_left_over=FALSE;
   unsigned int block_sz=0x4000;
   unsigned int have=0;
   z_stream strm;
   unsigned char in[0x4000], out[0x4000];

   /* allocate inflate state */
   memset(&strm, 0, sizeof(strm));
   strm.zalloc = Z_NULL;
   strm.zfree = Z_NULL;
   strm.opaque = Z_NULL;
   strm.avail_in = 0;
   strm.next_in = Z_NULL;
   ret = inflateInit(&strm);
   if (ret != Z_OK) {
      if (pn_rc != NULL) *pn_rc = ret;
      return FALSE;
   }
   rewind(out_fp);
   /* decompress until deflate stream ends or end of file */
   do {
      int bytes_to_read=min(block_sz, bytes_left), bytes_to_read2=0;
      unsigned char in2[0x4000];

      if (bytes_to_read == 0) break;
      memcpy(in, in_buf, bytes_to_read);

      bytes_left -= bytes_to_read;
      in_buf += bytes_to_read;

      if (use_byte_unstuffing) {
         if (!ByteUnStuff(in, bytes_to_read, in2, &bytes_to_read2,
               &has_left_over)) {
            (void)inflateEnd(&strm);
            if (pn_rc != NULL) *pn_rc = Z_DATA_ERROR;
            return FALSE;
         }
         strm.avail_in = bytes_to_read2;
         strm.next_in = in2;
      } else {
         strm.avail_in = bytes_to_read;
         strm.next_in = in;
      }
      /* run inflate() on input until output buffer not full */
      do {
         strm.avail_out = block_sz;
         strm.next_out = out;
         ret = inflate(&strm, Z_NO_FLUSH);
         TgAssert(ret != Z_STREAM_ERROR,
               "inflate() returns Z_STREAM_ERROR in DoInflate()", NULL);
               /* make sure that state is not clobbered */
         switch (ret) {
         case Z_NEED_DICT:
            ret = Z_DATA_ERROR;     /* and fall through */
         case Z_DATA_ERROR:
         case Z_MEM_ERROR:
            (void)inflateEnd(&strm);
            if (pn_rc != NULL) *pn_rc = ret;
            return FALSE;
         }
         have = block_sz - strm.avail_out;

         if (fwrite(out, 1, have, out_fp) != have || ferror(out_fp)) {
            (void)inflateEnd(&strm);
            if (pn_rc != NULL) *pn_rc = Z_ERRNO;
            return FALSE;
         }
      } while (strm.avail_out == 0);
    
      /* done when inflate() says it's done */
   } while (ret != Z_STREAM_END);

   if (has_left_over) {
      (void)inflateEnd(&strm);
      if (pn_rc != NULL) *pn_rc = Z_DATA_ERROR;
      return FALSE;
   }
   /* clean up and return */
   inflateEnd(&strm);

   if (ret == Z_STREAM_END) {
      return TRUE;
   }
   if (pn_rc != NULL) *pn_rc = Z_DATA_ERROR;
   return FALSE;
#else /* ~(!defined(_NO_ZLIB) || defined(HAVE_LIBZ)) */
   return FALSE;
#endif /* (!defined(_NO_ZLIB) || defined(HAVE_LIBZ)) */
}

/* --------------------- InflateInit() --------------------- */

/* --------------------- ZlibError() --------------------- */

void ZlibError(status, deflate)
   int status, deflate;
{
#if (!defined(_NO_ZLIB) || defined(HAVE_LIBZ))
   char buf[MAXSTRING];

   switch (status) {
   case Z_ERRNO:
      snprintf(buf, sizeof(buf), "%s-%s.",
            "File I/O error during z",
            deflate ? "compression" : "decompression");
      break;
   case Z_STREAM_ERROR:
      snprintf(buf, sizeof(buf), "%s-%s.",
            "Invalid compression level used for z",
            deflate ? "compression" : "decompression");
      break;
   case Z_DATA_ERROR:
      snprintf(buf, sizeof(buf), "%s-%s.",
            "Corrupted data encountered during z",
            deflate ? "compression" : "decompression");
      break;
   case Z_MEM_ERROR:
      snprintf(buf, sizeof(buf), "%s-%s.", "Out of memory during z",
            deflate ? "compression" : "decompression");
      break;
   case Z_VERSION_ERROR:
      snprintf(buf, sizeof(buf), "%s-%s.", "Zlib version mismatch for z",
            deflate ? "compression" : "decompression");
      break;
   }
   if (deflate) {
      snprintf(gszMsgBox, sizeof(gszMsgBox), "%s\n\n%s", buf,
            "Continue without z-compression.");
   } else {
      UtilStrCpyN(gszMsgBox, sizeof(gszMsgBox), buf);
   }
   if (PRTGIF && !cmdLineOpenDisplay) {
      fprintf(stderr, "%s\n", gszMsgBox);
   } else {
      MsgBox(gszMsgBox, TOOL_NAME, INFO_MB);
   }
#endif /* (!defined(_NO_ZLIB) || defined(HAVE_LIBZ)) */
}

