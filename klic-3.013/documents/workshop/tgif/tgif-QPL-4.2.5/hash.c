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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/hash.c,v 1.7 2011/05/16 16:21:57 william Exp $
 */

#define _INCLUDE_FROM_HASH_C_

#include "tgifdefs.h"

#include "hash.e"
#include "msg.e"
#include "util.e"

#define INT_HASH_DATA 0
#define STR_HASH_DATA 1

typedef struct tagHashData {
   char *psz_key;
   int key_sz;
   int type;
   union {
      char *sval;
      int ival;
   } detail;
} HashData;

/* ------------------ Utility Functions ------------------ */

static
void FreeHashData(phd)
   HashData *phd;
{
   switch (phd->type) {
   case INT_HASH_DATA: break;
   case STR_HASH_DATA: UtilFree(phd->detail.sval); break;
   }
   UtilFree(phd->psz_key);
   free(phd);
}

static
HashData *NewIntHashData(psz_key, key_sz, ival)
   char *psz_key;
   int key_sz, ival;
{
   HashData *phd=(HashData*)malloc(sizeof(HashData));

   if (phd == NULL) FailAllocMessage();

   memset(phd, 0, sizeof(HashData));
   phd->psz_key = (char*)malloc(key_sz);
   if (phd->psz_key == NULL) FailAllocMessage();
   memcpy(phd->psz_key, psz_key, key_sz);
   phd->key_sz = key_sz;
   phd->type = INT_HASH_DATA;
   phd->detail.ival = ival;

   return phd;
}

static
HashData *NewStrHashData(psz_key, key_sz, psz_value)
   char *psz_key, *psz_value;
   int key_sz;
{
   HashData *phd=(HashData*)malloc(sizeof(HashData));

   if (phd == NULL) FailAllocMessage();

   memset(phd, 0, sizeof(HashData));
   phd->psz_key = (char*)malloc(key_sz);
   if (phd->psz_key == NULL) FailAllocMessage();
   memcpy(phd->psz_key, psz_key, key_sz);
   phd->type = STR_HASH_DATA;
   phd->detail.sval = UtilStrDup(psz_value);
   if (phd->detail.sval == NULL) FailAllocMessage();

   return phd;
}

static
unsigned int GetBucket(p_hash, psz_key, key_sz)
   TgHash *p_hash;
   char *psz_key;
   int key_sz;
{
   unsigned int ival=0, ival2=0, bucket=0;

   switch (key_sz) {
   case 0: return FALSE;
   case 1:
      ival = (unsigned int)(unsigned char)*psz_key;
      break;
   case 2:
      ival = ((((unsigned int)(unsigned char)psz_key[0]) & 0x0ff) << 8) |
             (((unsigned int)(unsigned char)psz_key[1]) & 0x0ff);
      break;
   case 3:
      ival = ((((unsigned int)(unsigned char)psz_key[0]) & 0x0ff) << 16) |
             ((((unsigned int)(unsigned char)psz_key[1]) & 0x0ff) << 8) |
             (((unsigned int)(unsigned char)psz_key[2]) & 0x0ff);
      break;
   default:
      for ( ; key_sz >= 4; key_sz-=4) {
         ival2 =
               ((((unsigned int)(unsigned char)psz_key[0]) & 0x0ff) << 24) |
               ((((unsigned int)(unsigned char)psz_key[1]) & 0x0ff) << 16) |
               ((((unsigned int)(unsigned char)psz_key[2]) & 0x0ff) << 8) |
               (((unsigned int)(unsigned char)psz_key[3]) & 0x0ff);

         ival = (ival>>1) ^ ival2;
         psz_key = &psz_key[4];
      }
      switch (key_sz) {
      case 0: break;
      case 1:
         ival2 = ((((unsigned int)(unsigned char)psz_key[0]) & 0x0ff) << 24);
         ival = (ival>>1) ^ ival2;
         break;
      case 2:
         ival2 =
               ((((unsigned int)(unsigned char)psz_key[0]) & 0x0ff) << 24) |
               ((((unsigned int)(unsigned char)psz_key[1]) & 0x0ff) << 16);
         ival = (ival>>1) ^ ival2;
         break;
      case 3:
         ival2 =
               ((((unsigned int)(unsigned char)psz_key[0]) & 0x0ff) << 24) |
               ((((unsigned int)(unsigned char)psz_key[1]) & 0x0ff) << 16) |
               ((((unsigned int)(unsigned char)psz_key[2]) & 0x0ff) << 8);
         ival = (ival>>1) ^ ival2;
         break;
      }
      break;
   }
   bucket = ival % p_hash->num_buckets;

   return bucket;
}

/* ------------------ CleanUpHash() ------------------ */

void CleanUpHash(p_hash)
   TgHash *p_hash;
{
   int i=0;

   for (i=0; i < p_hash->num_buckets; i++) {
      CVList *p_list=(&p_hash->buckets[i]);

      if (ListLength(p_list) > 0) {
         CVListElem *e=NULL;

         for (e=ListFirst(p_list); e != NULL; e=ListNext(p_list,e)) {
            HashData *phd=(HashData*)(e->obj);

            FreeHashData(phd);
         }
         ListUnlinkAll(p_list);
      }
   }
   free(p_hash->buckets);
}

/* ------------------ InitHash() ------------------ */

int InitHash(p_hash, which)
   TgHash *p_hash;
   int which;
{
   int i=0;

   switch (which) {
   case TG_HASH_SIZE_SMALL: break;
   case TG_HASH_SIZE_MEDIUM: break;
   case TG_HASH_SIZE_LARGE: break;
   default: return FALSE;
   }
   p_hash->num_buckets = which;
   p_hash->buckets = (CVList*)malloc(which*sizeof(CVList));
   if (p_hash->buckets == NULL) FailAllocMessage();
   memset(p_hash->buckets, 0, which*sizeof(CVList));

   for (i=0; i < which; i++) {
      CVListInit(&p_hash->buckets[i]);
   }
   return TRUE;
}

/* ------------------ HashStoreInt() ------------------ */

int HashStoreInt(p_hash, psz_key, key_sz, ival)
   TgHash *p_hash;
   char *psz_key;
   int key_sz, ival;
{
   unsigned int bucket=GetBucket(p_hash, psz_key, key_sz);
   CVList *list=(&p_hash->buckets[bucket]);
   HashData *phd=NewIntHashData(psz_key, key_sz, ival);

   ListPrepend(list, phd);

   return TRUE;
}

/* ------------------ HashStoreStr() ------------------ */

int HashStoreStr(p_hash, psz_key, key_sz, psz_val)
   TgHash *p_hash;
   char *psz_key, *psz_val;
   int key_sz;
{
   unsigned int bucket=GetBucket(p_hash, psz_key, key_sz);
   CVList *list=(&p_hash->buckets[bucket]);
   HashData *phd=NewStrHashData(psz_key, key_sz, psz_val);

   ListPrepend(list, phd);

   return TRUE;
}

/* ------------------ HashLookUpInt() ------------------ */

int HashLookUpInt(p_hash, psz_key, key_sz, pn_value_return)
   TgHash *p_hash;
   char *psz_key;
   int key_sz, *pn_value_return;
{
   unsigned int bucket=GetBucket(p_hash, psz_key, key_sz);
   CVList *list=(&p_hash->buckets[bucket]);
   CVListElem *e=NULL;

   for (e=ListFirst(list); e != NULL; e=ListNext(list,e)) {
      HashData *phd=(HashData*)(e->obj);

      if (phd->key_sz == key_sz && memcmp(phd->psz_key, psz_key, key_sz) == 0) {
         if (phd->type == INT_HASH_DATA) {
            *pn_value_return = phd->detail.ival;
            return TRUE;
         }
         return FALSE;
      }
   }
   return FALSE;
}

/* ------------------ HashLookUpStr() ------------------ */

char *HashLookUpStr(p_hash, psz_key, key_sz)
   TgHash *p_hash;
   char *psz_key;
   int key_sz;
{
   unsigned int bucket=GetBucket(p_hash, psz_key, key_sz);
   CVList *list=(&p_hash->buckets[bucket]);
   CVListElem *e=NULL;

   for (e=ListFirst(list); e != NULL; e=ListNext(list,e)) {
      HashData *phd=(HashData*)(e->obj);

      if (phd->key_sz == key_sz && memcmp(phd->psz_key, psz_key, key_sz) == 0) {
         if (phd->type == STR_HASH_DATA) {
            return phd->detail.sval;
         }
         return NULL;
      }
   }
   return NULL;
}

