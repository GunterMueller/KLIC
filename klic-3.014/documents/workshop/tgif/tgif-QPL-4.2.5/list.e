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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/list.e,v 1.6 2011/05/16 16:21:58 william Exp $
 */

#ifndef _TGIF_LIST_E_
#define _TGIF_LIST_E_

typedef int (CMPFN)ARGS_DECL((void*, void*)); /* a comparison function */

typedef struct tagCVListElem {
    void *obj;
    struct tagCVListElem *next;
    struct tagCVListElem *prev;
} CVListElem;

typedef struct tagCVList {
    int num_members;
    CVListElem anchor;

    int  (*Length)(struct tagCVList *);
    int  (*Empty)(struct tagCVList *);

    int  (*Append)(struct tagCVList *, void*);
    int  (*Prepend)(struct tagCVList *, void*);
    void (*Unlink)(struct tagCVList *, CVListElem*);
    void (*UnlinkAll)(struct tagCVList *);
    int  (*InsertBefore)(struct tagCVList *, void*, CVListElem*);
    int  (*InsertAfter)(struct tagCVList *, void*, CVListElem*);

    CVListElem *(*First)(struct tagCVList *);
    CVListElem *(*Last)(struct tagCVList *);
    CVListElem *(*Next)(struct tagCVList *, CVListElem *cur);
    CVListElem *(*Prev)(struct tagCVList *, CVListElem *cur);

    /* returns the CVListElem that contains obj */
    CVListElem *(*Find)(struct tagCVList *, void*, CMPFN*);
} CVList;

#ifdef _INCLUDE_FROM_LIST_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_LIST_C_*/

extern int	CompareForStringList ARGS_DECL((void *pv1, void *pv2));

extern int	ListLength ARGS_DECL((CVList*));
extern int	ListEmpty ARGS_DECL((CVList*));

extern int	ListAppend ARGS_DECL((CVList*, void*));
extern int	ListPrepend ARGS_DECL((CVList*, void*));

extern void	ListUnlink ARGS_DECL((CVList*, CVListElem*));
extern void	ListUnlinkAll ARGS_DECL((CVList*));

extern int	ListInsertAfter ARGS_DECL((CVList*, void*, CVListElem*));
extern int	ListInsertBefore ARGS_DECL((CVList*, void*, CVListElem*));

extern CVListElem	*ListFirst ARGS_DECL((CVList*));
extern CVListElem	*ListLast ARGS_DECL((CVList*));
extern CVListElem	*ListNext ARGS_DECL((CVList*, CVListElem*));
extern CVListElem	*ListPrev ARGS_DECL((CVList*, CVListElem*));

extern CVListElem	*ListFind ARGS_DECL((CVList*, void*, CMPFN*));

extern void	CVListCleanUp ARGS_DECL((CVList*));
extern int	CVListInit ARGS_DECL((CVList*));

#ifdef _INCLUDE_FROM_LIST_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_LIST_C_*/

#endif /*_TGIF_LIST_E_*/
