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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/list.c,v 1.6 2011/05/16 16:21:58 william Exp $
 */

#define _INCLUDE_FROM_LIST_C_

#include "tgifdefs.h"

#include "list.e"
#include "msg.e"

/* -------------------- Utility Functions -------------------- */

int CompareForStringList(void *pv1, void *pv2)
{
   return strcmp(((char*)pv1), ((char*)pv2));
}

/* -------------------- CVListElem -------------------- */

static
CVListElem *NewCVListElem(void *obj)
{
   CVListElem *new_elem=(CVListElem*)malloc(sizeof(CVListElem));

   if (new_elem == NULL) {
      FailAllocMessage();
      return NULL;
   }
   memset(new_elem, 0, sizeof(CVListElem));
   new_elem->obj = obj;

   return new_elem;
}

/* -------------------- CVList -------------------- */

int ListLength(CVList *list)
{
   return list->num_members;
}

int ListEmpty(CVList *list)
{
   return (list->num_members <= 0);
}

int ListAppend(CVList *list, void *obj)
{
   CVListElem *new_elem=NewCVListElem(obj);

   if (new_elem == NULL) return FALSE;

   new_elem->next = &(list->anchor);
   new_elem->prev = list->anchor.prev;
   list->anchor.prev->next = new_elem;
   list->anchor.prev = new_elem;
   list->num_members++;

   return TRUE;
}

int ListPrepend(CVList *list, void *obj)
{
   CVListElem *new_elem=NewCVListElem(obj);

   if (new_elem == NULL) return FALSE;

   new_elem->prev = &(list->anchor);
   new_elem->next = list->anchor.next;
   list->anchor.next->prev = new_elem;
   list->anchor.next = new_elem;
   list->num_members++;

   return TRUE;
}

void ListUnlink(CVList *list, CVListElem *elem)
   /* this will not free elem */
{
   elem->prev->next = elem->next;
   elem->next->prev = elem->prev;
   elem->next = elem->prev = NULL;
   list->num_members--;
}

void ListUnlinkAll(CVList *list)
   /* this frees all the elements */
{
   CVListElem *elem=NULL, *next_elem=NULL;

   for (elem=ListFirst(list); elem != NULL; elem=next_elem) {
       next_elem = ListNext(list, elem);
       elem->prev = elem->next = NULL;
       free(elem);
   }
   list->num_members = 0;
   list->anchor.next = list->anchor.prev = (&list->anchor);
}

int ListInsertAfter(CVList *list, void *obj, CVListElem *elem)
{
   if (list->Empty(list) || elem == NULL) {
      return ListPrepend(list, obj);
   } else {
      CVListElem *new_elem=NewCVListElem(obj);

      if (new_elem == NULL) return FALSE;

      new_elem->next = elem->next;
      new_elem->prev = elem;
      new_elem->next->prev = new_elem;
      new_elem->prev->next = new_elem;
      list->num_members++;
   }
   return TRUE;
}

int ListInsertBefore(CVList *list, void *obj, CVListElem *elem)
{
   if (list->Empty(list) || elem == NULL) {
      return ListAppend(list, obj);
   } else {
      CVListElem *new_elem=NewCVListElem(obj);

      if (new_elem == NULL) return FALSE;

      new_elem->prev = elem->prev;
      new_elem->next = elem;
      new_elem->next->prev = new_elem;
      new_elem->prev->next = new_elem;
      list->num_members++;
   }
   return TRUE;
}

CVListElem *ListFirst(CVList *list)
{
   if (list->Empty(list)) return NULL;

   return list->anchor.next;
}

CVListElem *ListLast(CVList *list)
{
   if (list->Empty(list)) return NULL;

   return list->anchor.prev;
}

CVListElem *ListNext(CVList *list, CVListElem *cur)
{
   return (cur->next == (&list->anchor) ? NULL : cur->next);
}

CVListElem *ListPrev(CVList *list, CVListElem *cur)
{
   return (cur->prev == (&list->anchor) ? NULL : cur->prev);
}

CVListElem *ListFind(CVList *list, void *obj, CMPFN *pcmpfunc)
{
   CVListElem *ptr=NULL;

   for (ptr=ListFirst(list); ptr != NULL; ptr=ListNext(list, ptr)) {
      if (pcmpfunc == NULL) {
         if (ptr->obj == obj) {
            return ptr;
         }
      } else if ((pcmpfunc)(ptr->obj, obj) == 0) {
         return ptr;
      }
   }
   return NULL;
}

void CVListCleanUp(CVList *list)
{
   if (list == NULL) return;

   if (list->num_members) {
      CVListElem *elem=NULL, *next_elem=NULL;

      for (elem=ListFirst(list); elem != NULL; elem=next_elem) {
         next_elem = ListNext(list, elem);
         free(elem);
      }
   }
   memset(list, 0, sizeof(CVList));
}

int CVListInit(CVList *list)
{
   if (list == NULL) return FALSE;

   memset(list, 0, sizeof(CVList));

   list->num_members = 0;
   list->anchor.prev = list->anchor.next = (&list->anchor);

   list->First = ListFirst;
   list->Last = ListLast;
   list->Next = ListNext;
   list->Prev = ListPrev;

   list->Length = ListLength;

   list->Append = ListAppend;
   list->Prepend = ListPrepend;
   list->Unlink = ListUnlink;
   list->UnlinkAll = ListUnlinkAll;
   list->InsertBefore = ListInsertBefore;
   list->InsertAfter = ListInsertAfter;

   list->Find = ListFind;

   list->Empty = ListEmpty;

   return TRUE;
}

