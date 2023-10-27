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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/hash.e,v 1.5 2011/05/16 16:21:57 william Exp $
 */

#ifndef _HASH_E_
#define _HASH_E_

#include "list.e"

#define TG_HASH_SIZE_SMALL   251
#define TG_HASH_SIZE_MEDIUM  509
#define TG_HASH_SIZE_LARGE  2039

typedef struct tagTgHash {
   int num_buckets;
   CVList *buckets;
} TgHash;

#ifdef _INCLUDE_FROM_HASH_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_HASH_C_*/

extern void	CleanUpHash ARGS_DECL((TgHash*));
extern int	InitHash ARGS_DECL((TgHash*, int which));

extern int	HashStoreInt ARGS_DECL((TgHash*, char *key, int key_sz,
				int value));
extern int	HashStoreStr ARGS_DECL((TgHash*, char *key, int key_sz,
				char *value));
extern int	HashLookUpInt ARGS_DECL((TgHash*, char *key, int key_sz,
				int *pn_value_return));
extern char	*HashLookUpStr ARGS_DECL((TgHash*, char *key, int key_sz));

#ifdef _INCLUDE_FROM_HASH_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_HASH_C_*/

#endif /*_HASH_E_*/
