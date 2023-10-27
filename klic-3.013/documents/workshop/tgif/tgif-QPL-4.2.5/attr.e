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
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/attr.e,v 1.8 2011/05/16 16:21:56 william Exp $
 */

#ifndef _ATTR_E_
#define _ATTR_E_

typedef struct tagAttrGroupInfo {
    char *group_name;
    char *group_value;
    char *displayed_names; /* better looking group_value */
    int num_attrs;
    char **attr_name; /* just pointers, do not free */
} AttrGroupInfo;

extern int	dropObsIconAttrWhenUpdate;
extern int	maxAttrGroups;

extern AttrGroupInfo	**gAttrGroupInfo;
extern EditAttrInfo	*gpEditAttrInEditorAttrInfo;

#ifdef _INCLUDE_FROM_ATTR_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_ATTR_C_*/

extern void	LinkInAttr ARGS_DECL((struct AttrRec *, struct AttrRec *,
		                      struct AttrRec *));
extern void	FreeAttr ARGS_DECL((struct AttrRec *));
extern void	UnlinkAttr ARGS_DECL((struct AttrRec *));
extern void	UpdAttrOwner ARGS_DECL((struct AttrRec *first_attr,
		                        struct ObjRec *owner_obj));
extern void	ParseAttrStr ARGS_DECL((char *buf, char *name, int name_sz,
		                        char *value, int value_sz));
extern void	UpdateAttr ARGS_DECL((struct TextRec *, struct AttrRec *));
extern void	DrawAttrs ARGS_DECL((Window, int X, int Y, struct AttrRec *));
extern void	MoveAttrs ARGS_DECL((struct AttrRec *, int Dx, int Dy));
extern void	DelAllAttrs ARGS_DECL((struct AttrRec *));
extern struct AttrRec	* AddAttrByNameAndValue ARGS_DECL((struct ObjRec *,
		                                           char *AttrName,
		                                           char *AttrValue));
extern void	DupAttrs ARGS_DECL((struct ObjRec *From, struct ObjRec *To));
extern void	AddAttrs ARGS_DECL((void));
extern void	SaveAttrs ARGS_DECL((FILE *, struct AttrRec *));
extern char	* ReadAttrString ARGS_DECL((char *));
extern int	ReadAttr ARGS_DECL((FILE *, struct AttrRec **));
extern struct ObjRec	*FindTopLevelObjWithType ARGS_DECL((char*));
extern void	ShowAllAttrNames ARGS_DECL((void));
extern void	ShowAllAttrs ARGS_DECL((void));
extern void	HideAllAttrs ARGS_DECL((void));
extern void	HideAllAttrNames ARGS_DECL((void));
extern void	DetachGroupAttrs ARGS_DECL((struct ObjRec *,
		                            struct SelRec **Top,
		                            struct SelRec **Bot));
extern void	DetachAllObjAttrs ARGS_DECL((struct ObjRec *,
		                             struct SelRec **Top,
		                             struct SelRec **Bot));
extern void	DetachAttrs ARGS_DECL((void));
extern void	UpdAttr ARGS_DECL((struct AttrRec *));
extern TgMenu	*CreateAttrMenu ARGS_DECL((TgMenu*, int x, int y, int num_items,
		                           char **menu_strings,
		                           char **status_strings, int *pixels));
extern void	FreeEditAttrInfo ARGS_DECL((EditAttrInfo*));
extern int	HasEditAttrsInContextMenu ARGS_DECL((struct ObjRec *,
		                                     struct AttrRec **));
extern void	FreeRestrictedAttrNames ARGS_DECL((char **ppsz_restricted,
		                                   int num_restricted));
extern void	GetRestrictedAttrNames ARGS_DECL((struct ObjRec *,
		                                  char ***pppsz_restricted,
		                                  int *pn_num_restricted));
extern void	GetAttrGroupAttrNames ARGS_DECL((struct ObjRec *,
                                                 int group_index,
		                                 char ***pppsz_restricted,
		                                 int *pn_num_restricted));
extern int	IsRestrictedAttr ARGS_DECL((char *attr_name,
		                            char **ppsz_restricted,
		                            int num_restricted));
extern EditAttrInfo	*CreateEditAttrInfo ARGS_DECL((struct ObjRec *));
extern EditAttrInfo	*CreateEditAttrGroupInfo ARGS_DECL((struct ObjRec *));
extern void	MoveAttr ARGS_DECL((void));
extern void	CopyAndUpdateAttrs ARGS_DECL((struct ObjRec *To,
		                              struct ObjRec *From));
extern void	ToggleNamedAttrShown ARGS_DECL((char *));
extern void	ToggleEqAttrShown ARGS_DECL((void));
extern void	AddFileAttrs ARGS_DECL((void));
extern void	DetachFileAttrs ARGS_DECL((void));
extern int	BlankStr ARGS_DECL((char*));
extern void	EditFileAttrs ARGS_DECL((void));
extern void	EditAttrs ARGS_DECL((void));

extern void	CleanUpAttr ARGS_DECL((void));
extern int	InitAttr ARGS_DECL((void));

#ifdef _INCLUDE_FROM_ATTR_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_ATTR_C_*/

#endif /*_ATTR_E_*/
