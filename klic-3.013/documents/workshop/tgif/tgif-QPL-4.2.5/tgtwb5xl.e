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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgtwb5xl.e,v 1.10 2011/05/16 16:22:00 william Exp $
 */

#ifndef _NO_TGTWB5

#ifndef _TGTWB5XL_E_
#define _TGTWB5XL_E_

#define MAXZYFH_CHARS 16

#define PH_Er  8
#define PH_An  12
#define PH_En  (PH_An+1)
#define PH_Ang (PH_An+2)
#define PH_Eng (PH_An+3)
#define PH_Ai  16
#define PH_Ei  (PH_Ai+1)
#define PH_Ao  (PH_Ai+2)
#define PH_Ou  (PH_Ai+3)
#define PH_A   20
#define PH_O   (PH_A+1)
#define PH_E   (PH_A+2)
#define PH_Eh  (PH_A+3)
#define PH_Y   24
#define PH_W   (PH_Y+1)
#define PH_Yu  (PH_Y+2)
#define PH_Z   28
#define PH_C   (PH_Z+1)
#define PH_S   (PH_Z+2)
#define PH_Zh  32
#define PH_Ch  (PH_Zh+1)
#define PH_Sh  (PH_Zh+2)
#define PH_R   (PH_Zh+3)
#define PH_J   36
#define PH_Q   (PH_J+1)
#define PH_X   (PH_J+2)
#define PH_G   40
#define PH_K   (PH_G+1)
#define PH_H   (PH_G+2)
#define PH_D   44
#define PH_T   (PH_D+1)
#define PH_N   (PH_D+2)
#define PH_L   (PH_D+3)
#define PH_B   48
#define PH_P   (PH_B+1)
#define PH_M   (PH_B+2)
#define PH_F   (PH_B+3)

extern char	*firstLargeTransTbl[];
extern char	*secondLargeTransTbl[];
extern char	*thirdLargeTransTbl[];
extern char	*fourthLargeTransTbl[];
extern char	*singleExtention[];

extern char	*pinyin_et_big5[];
extern char	*pinyin_gb2312[];

#endif /*_TGTWB5XL_E_*/

#endif /* ~_NO_TGTWB5 */
