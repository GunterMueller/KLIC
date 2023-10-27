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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/pngtrans.c,v 1.11 2011/05/16 16:21:59 william Exp $
 */

#define _INCLUDE_FROM_PNGTRANS_C_

#include "tgifdefs.h"

#if (!defined(_NO_ZLIB) || defined(HAVE_LIBZ))
#include <zlib.h>
#endif /* (!defined(_NO_ZLIB) || defined(HAVE_LIBZ)) */

#include "dialog.e"
#include "pngtrans.e"
#include "msg.e"
#include "strtbl.e"
#include "util.e"

static int gnDebugPngParse=0;

static unsigned char pngHeader[8] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };

static
int PngShowOrPrintMsg(buf, use_msgbox)
   char *buf;
   int use_msgbox;
{
   if (gnDebugPngParse > 0) {
      return ShowOrPrintMsg(buf, use_msgbox);
   }
   return FALSE;
}

static
int VerifyPngHeader(fp)
   FILE *fp;
{
   unsigned char buf[8];

   if (fread(buf, sizeof(char), 8, fp) != 8) {
      strcpy(gszMsgBox, TgLoadString(STID_BADPNG_SHORT_HEADER));
      return PngShowOrPrintMsg(gszMsgBox, TRUE);
   }
   if (memcmp(pngHeader, buf, 8) != 0) {
      strcpy(gszMsgBox, TgLoadString(STID_BADPNG_BAD_HEADER));
      return PngShowOrPrintMsg(gszMsgBox, TRUE);
   }
   return TRUE;
}

static
int ReadPngDWord(pvalue32, fp)
   uint32_t *pvalue32;
   FILE *fp;
{
   unsigned char buf[4];
   uint32_t value32=(uint32_t)0;
   int bytes_read=fread(buf, sizeof(char), 4, fp);

   if (bytes_read != 4) {
      return bytes_read;
   }
   value32 += (uint32_t)(buf[0]);
   value32 <<= 8;
   value32 += (uint32_t)(buf[1]);
   value32 <<= 8;
   value32 += (uint32_t)(buf[2]);
   value32 <<= 8;
   value32 += (uint32_t)(buf[3]);

   *pvalue32 = value32;

   return 4;
}

static
int ReadPngShortWord(pvalue16, fp)
   uint16_t *pvalue16;
   FILE *fp;
{
   unsigned char buf[2];
   uint16_t value16=(uint16_t)0;
   int bytes_read=fread(buf, sizeof(char), 2, fp);

   if (bytes_read != 2) {
      return bytes_read;
   }
   value16 += (uint16_t)(buf[0]);
   value16 <<= 8;
   value16 += (uint16_t)(buf[1]);

   *pvalue16 = value16;

   return 2;
}

static
void PngHexDump(buf, len, pn_index, pn_line_num)
   unsigned char *buf;
   unsigned int len;
   int *pn_index, *pn_line_num;
{
   int index=(*pn_index), line_num=(*pn_line_num);

   while (len > 0) {
      if (index == 0) {
         fprintf(stdout, "%06x:  ", line_num<<4);
      }
      fprintf(stdout, "%02x ", (int)(unsigned int)(*buf));
      index++;
      if (index == 8) {
         fprintf(stdout, " ");
      } else if (index == 16) {
         index = 0;
         line_num++;
         fprintf(stdout, "\n");
      }
      len--;
      buf++;
   }
   *pn_index = index;
   *pn_line_num = line_num;
}

static
int DoInflateForPng(in_buf, v32_length)
   unsigned char *in_buf;
   uint32_t v32_length;
{
#if (!defined(_NO_ZLIB) || defined(HAVE_LIBZ))
   int ret=0, has_left_over=FALSE, index=0, line_num=0;
   int bytes_left=(int)(unsigned int)v32_length;
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
      return FALSE;
   }
   do {
      int bytes_to_read=min(block_sz, bytes_left);

      if (bytes_to_read == 0) break;
      memcpy(in, in_buf, bytes_to_read);

      bytes_left -= bytes_to_read;
      in_buf += bytes_to_read;

      strm.avail_in = bytes_to_read;
      strm.next_in = in;

      /* run inflate() on input until output buffer not full */
      do {
         strm.avail_out = block_sz;
         strm.next_out = out;
         ret = inflate(&strm, Z_NO_FLUSH);
         if (ret == Z_STREAM_ERROR) return FALSE;

         switch (ret) {
         case Z_NEED_DICT:
            ret = Z_DATA_ERROR;     /* and fall through */
         case Z_DATA_ERROR:
         case Z_MEM_ERROR:
            (void)inflateEnd(&strm);
            return FALSE;
         }
         have = block_sz - strm.avail_out;

         if (gnDebugPngParse > 1) PngHexDump(out, have, &index, &line_num);
      } while (strm.avail_out == 0);

      /* done when inflate() says it's done */
   } while (ret != Z_STREAM_END);

   if (has_left_over) {
      (void)inflateEnd(&strm);
      return FALSE;
   }
   /* clean up and return */
   inflateEnd(&strm);

   if (ret == Z_STREAM_END) {
      return TRUE;
   }
   return FALSE;
#else /* ~(!defined(_NO_ZLIB) || defined(HAVE_LIBZ)) */
   return FALSE;
#endif /* (!defined(_NO_ZLIB) || defined(HAVE_LIBZ)) */
}

static
void SetPngFileTransColor(pphi, red, green, blue)
   PngHeaderInfo *pphi;
   unsigned int red, green, blue;
{
   pphi->trans_color_pixel_found = TRUE;
   pphi->trans_color_pixel_red = (unsigned char)(red & 0x00ff);
   pphi->trans_color_pixel_green = (unsigned char)(green & 0x00ff);
   pphi->trans_color_pixel_blue = (unsigned char)(blue & 0x00ff);
}

static
int ReadChunk(pphi, psz_chunk_type, v32_length, fp)
   PngHeaderInfo *pphi;
   char *psz_chunk_type;
   uint32_t v32_length;
   FILE *fp;
{
   int i=0;
   uint32_t bytes_read=0, bytes_left=v32_length;
   uint16_t v16=(uint16_t)0;

   if (gnDebugPngParse > 0) {
      /* debug, do not translate */
      fprintf(stdout, "%s: length = %1d (0x%08x)\n", psz_chunk_type,
            (int)(unsigned int)v32_length, (int)(unsigned int)v32_length);
      fflush(stdout);
   }
   if (strncmp(psz_chunk_type, "IHDR", 4) == 0) {
      if (v32_length == 13) {
         if ((bytes_read=ReadPngDWord(&pphi->image_w, fp)) != 4) return FALSE;
         if ((bytes_read=ReadPngDWord(&pphi->image_h, fp)) != 4) return FALSE;
         if ((bytes_read=fread(&pphi->bit_depth, sizeof(char), 1, fp)) != 1) return FALSE;
         if ((bytes_read=fread(&pphi->color_type, sizeof(char), 1, fp)) != 1) return FALSE;
         if ((bytes_read=fread(&pphi->compression_method, sizeof(char), 1, fp)) != 1) return FALSE;
         if ((bytes_read=fread(&pphi->filter_method, sizeof(char), 1, fp)) != 1) return FALSE;
         if ((bytes_read=fread(&pphi->interlace_method, sizeof(char), 1, fp)) != 1) return FALSE;

         pphi->valid = TRUE;

         if (gnDebugPngParse > 0) {
            /* debug, do not translate */
            fprintf(stdout, "Width = %1d (0x%08x), Height = %1d (0x%08x), Color type = %1d, Bit depth = %1d, Compression = %1d, Filter = %1d, Interlace = %1d\n",
                  (int)(unsigned int)pphi->image_w,
                  (int)(unsigned int)pphi->image_w,
                  (int)(unsigned int)pphi->image_h,
                  (int)(unsigned int)pphi->image_h,
                  (int)(unsigned int)pphi->color_type,
                  (int)(unsigned int)pphi->bit_depth,
                  (int)(unsigned int)pphi->compression_method,
                  (int)(unsigned int)pphi->filter_method,
                  (int)(unsigned int)pphi->interlace_method);
         }
         switch (pphi->color_type) {
         case 0: break; /* Greyscale, allowed bit depths are 1, 2, 4, 8, 16 */
         case 2: break; /* Truecolor, allowed bit depths are 8, 16 */
         case 3: break; /* Indexed-colour, allowed bit depths are 1, 2, 4, 8 */

         case 4: /* don't know how to handle Greyscale with alpha */
         case 6: /* don't know how to handle Truecolour with alpha */
            if (gnDebugPngParse > 0) {
               /* debug, do not translate */
               fprintf(stdout, "Transparent color type unsupported.\n");
            }
            return FALSE;
         }
         return TRUE;
      } else {
         if (gnDebugPngParse > 0) {
            /* debug, do not translate */
            fprintf(stdout, "Unexpected IHDR chunk size: %1d (0x%08x).\n",
                  (int)(unsigned int)v32_length, (int)(unsigned int)v32_length);
         }
         strcpy(gszMsgBox, TgLoadString(STID_BADPNG_BAD_HEADER));
         return PngShowOrPrintMsg(gszMsgBox, TRUE);
      }
   } else if (strncmp(psz_chunk_type, "IDAT", 4) == 0) {
      if (pphi->valid && pphi->compression_method == 0) {
         if (gnDebugPngParse > 0) {
            unsigned char *buf=(unsigned char *)malloc(v32_length);

            if (buf == NULL) FailAllocMessage();
            if ((bytes_read=fread(buf, sizeof(char), v32_length, fp)) != v32_length) return FALSE;
            if (!DoInflateForPng(buf, v32_length)) {
               /* debug, do not translate */
               fprintf(stdout, "DoInflateForPng() failed.\n");
               fflush(stdout);
               return FALSE;
            }
            free(buf);
            return TRUE;
         }
      }
   } else if (strncmp(psz_chunk_type, "PLTE", 4) == 0) {
      if (pphi->valid && v32_length % 3 == 0) {
         int num_entries=v32_length/3;
         unsigned char *psz=NULL;

         pphi->palette = (unsigned char *)malloc(v32_length);
         if (pphi->palette == NULL) FailAllocMessage();
         if ((bytes_read=fread(pphi->palette, sizeof(char), v32_length, fp)) != v32_length) return FALSE;
         pphi->palette_sz = v32_length;

         if (gnDebugPngParse > 0) {
            psz = pphi->palette;
            for (i=0; i < num_entries; i++) {
               /* debug, do not translate */
               fprintf(stdout, "%04d: %02x%02x%02x\n", i, (int)psz[0], (int)psz[1], (int)psz[2]);
               psz = &psz[3];
            }
         }
         return TRUE;
      } else {
         strcpy(gszMsgBox, TgLoadString(STID_BADPNG_BAD_PALETTE_LEN));
         return PngShowOrPrintMsg(gszMsgBox, TRUE);
      }
   } else if (strncmp(psz_chunk_type, "tRNS", 4) == 0) {
      if (pphi->valid && pphi->color_type == 0 && v32_length == 2) {
         /* Greyscale, allowed bit depths are 1, 2, 4, 8, 16 */
         if ((bytes_read=ReadPngShortWord(&v16, fp)) != 2) return FALSE;
         switch (pphi->bit_depth) {
         case 1: v16 = ((v16 & 0x0001) == 0x0001) ? 0x00ff : 0; break;
         case 2:
            v16 = v16 & 0x0003;
            v16 = v16 | ((v16<<2)&0x000c) | ((v16<<4)&0x0030) | ((v16<<6)&0x00c0);
            break;
         case 4:
            v16 = v16 & 0x000f;
            v16 = v16 | ((v16<<4)&0x00f0);
            break;
         case 8: v16 = v16 & 0x00ff; break;
         case 16: v16 = (v16>>8) & 0x00ff; break;
         default: return FALSE;
         }
         SetPngFileTransColor(pphi, (unsigned int)v16, (unsigned int)v16, (unsigned int)v16);
         return TRUE;
      } else if (pphi->valid && pphi->color_type == 2 && v32_length == 6) {
         /* Truecolor, allowed bit depths are 8, 16 */
         uint16_t v16r=(uint16_t)0, v16g=(uint16_t)0, v16b=(uint16_t)0;

         if ((bytes_read=ReadPngShortWord(&v16r, fp)) != 2) return FALSE;
         if ((bytes_read=ReadPngShortWord(&v16b, fp)) != 2) return FALSE;
         if ((bytes_read=ReadPngShortWord(&v16g, fp)) != 2) return FALSE;
         switch (pphi->bit_depth) {
         case 8:
            v16r = v16r & 0x00ff;
            v16g = v16g & 0x00ff;
            v16b = v16b & 0x00ff;
            break;
         case 16: v16 = (v16>>8) & 0x00ff; break;
         default: return FALSE;
         }
         SetPngFileTransColor(pphi, (unsigned int)v16r, (unsigned int)v16g, (unsigned int)v16b);
         return TRUE;
      } else if (pphi->valid && pphi->color_type == 3 && v32_length == 1) {
         /* Indexed-colour, allowed bit depths are 1, 2, 4, 8 */
         unsigned char palette_index='\0', *psz=NULL;

         if ((bytes_read=fread(&palette_index, sizeof(char), 1, fp)) != 1) return FALSE;
         if (pphi->palette == NULL || palette_index >= pphi->palette_sz) return FALSE;
         psz = &pphi->palette[palette_index*3];
         /*
          * the bit_depth is used to control the number of entries in the
          * palette, not the number of bits in RGB
          */
         switch (pphi->bit_depth) {
         case 1: break;
         case 2: break;
         case 4: break;
         case 8: break;
         default: return FALSE;
         }
         SetPngFileTransColor(pphi, (unsigned int)psz[0], (unsigned int)psz[1], (unsigned int)psz[2]);
         return TRUE;
      } else {
         if (gnDebugPngParse > 0) {
            /* debug, do not translate */
            fprintf(stdout, "Transparent color type unsupported.\n");
         }
         return FALSE;
      }
   }
   if (fseek(fp, (long)bytes_left, SEEK_CUR) != 0) {
      return FALSE;
   }
   return TRUE;
}

static
int ProcessChunk(pphi)
   PngHeaderInfo *pphi;
{
   uint32_t v32_length=(uint32_t)0, v32_crc=(uint32_t)0;
   int bytes_read=0;
   char sz_chunk_type[5];
   FILE *fp=pphi->fp;

   if ((bytes_read=ReadPngDWord(&v32_length, fp)) == 0) return FALSE;
   if (bytes_read != 4) {
      sprintf(gszMsgBox, TgLoadString(STID_BADPNG_BAD_CHUNK_LEN), bytes_read);
      return PngShowOrPrintMsg(gszMsgBox, TRUE);
   }
   if ((bytes_read=fread(sz_chunk_type, sizeof(unsigned char), 4, fp)) != 4) {
      strcpy(gszMsgBox, TgLoadString(STID_BADPNG_SHORT_CHUNK_TYPE));
      return PngShowOrPrintMsg(gszMsgBox, TRUE);
   }
   sz_chunk_type[4] = '\0';
   if (!ReadChunk(pphi, sz_chunk_type, v32_length, fp)) return FALSE;

   if ((bytes_read=ReadPngDWord(&v32_crc, fp)) != 4) {
      strcpy(gszMsgBox, TgLoadString(STID_BADPNG_SHORT_CRC));
      return PngShowOrPrintMsg(gszMsgBox, TRUE);
   }
   return TRUE;
}

/* ----------------------- ParseTransColor() ----------------------- */

int ParseTransColor(char *buf, unsigned char *puch_trans_color_pixel_r,
      unsigned char *puch_trans_color_pixel_g,
      unsigned char *puch_trans_color_pixel_b)
{
   char buf_r[5], buf_g[5], buf_b[5];
   int r=0, g=0, b=0;

   if (*buf != '#' || strlen(buf) != 7) {
      return FALSE;
   }
   buf_r[0] = '0';
   buf_r[1] = '0';
   buf_r[2] = buf[1];
   buf_r[3] = buf[2];
   buf_r[4] = '\0';

   buf_g[0] = '0';
   buf_g[1] = '0';
   buf_g[2] = buf[3];
   buf_g[3] = buf[4];
   buf_g[4] = '\0';

   buf_b[0] = '0';
   buf_b[1] = '0';
   buf_b[2] = buf[5];
   buf_b[3] = buf[6];
   buf_b[4] = '\0';

   if (sscanf(buf_r, "%x", &r) != 1) return FALSE;
   if (sscanf(buf_g, "%x", &g) != 1) return FALSE;
   if (sscanf(buf_b, "%x", &b) != 1) return FALSE;

   *puch_trans_color_pixel_r = (unsigned char)(r & 0x0ff);
   *puch_trans_color_pixel_g = (unsigned char)(g & 0x0ff);
   *puch_trans_color_pixel_b = (unsigned char)(b & 0x0ff);

   return TRUE;
}

/* ----------------------- ResetPngHeaderInfo() ----------------------- */

void ResetPngHeaderInfo(pphi)
   PngHeaderInfo *pphi;
{
   memset(pphi, 0, sizeof(PngHeaderInfo));
}

/* ----------------------- SetPngHeaderInfoForTransColor() ----------------------- */

void SetPngHeaderInfoForTransColor(pphi, trans_color_pixel_r,
      trans_color_pixel_g, trans_color_pixel_b)
   PngHeaderInfo *pphi;
   unsigned char trans_color_pixel_r, trans_color_pixel_g, trans_color_pixel_b;
{
   ResetPngHeaderInfo(pphi);

   pphi->valid = TRUE;
   pphi->trans_color_pixel_found = TRUE;
   pphi->trans_color_pixel_red = trans_color_pixel_r;
   pphi->trans_color_pixel_green = trans_color_pixel_g;
   pphi->trans_color_pixel_blue = trans_color_pixel_b;
}

/* ----------------------- PngFileGetTransColorInit() ----------------------- */

int PngFileGetTransColorInit(pphi, fname)
   PngHeaderInfo *pphi;
   char *fname;
{
   ResetPngHeaderInfo(pphi);

   pphi->fp = fopen(fname, "r");
   if (pphi->fp == NULL) return FailToOpenMessage(fname, "r", NULL);
   UtilStrCpyN(pphi->fname, sizeof(pphi->fname), fname);

   return TRUE;
}

/* ----------------------- PngFileGetTransColor() ----------------------- */

int PngFileGetTransColor(pphi)
   PngHeaderInfo *pphi;
   /*
    * returns TRUE if transparent pixel found
    * returns FALSE if there is a problem parsing the PNG file or if it does not contain transparent pixels
    */
{
   if (!VerifyPngHeader(pphi->fp)) {
      if (pphi->fp != NULL) fclose(pphi->fp);
      ResetPngHeaderInfo(pphi);
      return FALSE;
   }
   while (ProcessChunk(pphi)) ;

   if (pphi->fp != NULL) {
      fclose(pphi->fp);
      pphi->fp = NULL;
   }
   if (pphi->valid && pphi->palette_sz > 0 && pphi->palette != NULL) {
      free(pphi->palette);
   }
   pphi->palette = NULL;

   return (pphi->valid && pphi->trans_color_pixel_found);
}

