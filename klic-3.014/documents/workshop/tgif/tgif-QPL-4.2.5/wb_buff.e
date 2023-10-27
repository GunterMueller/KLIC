/*
 * Author:      Magnos Martinello, <magnos@land.ufrj.br> in April, 1999.
 *
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/wb_buff.e,v 1.2 2008/10/16 21:13:03 william Exp $
 */

#ifndef _WB_BUFF_E_
#define _WB_BUFF_E_

/* Comentario Principal */

#define DUPLICATED 4
#define SORTED     2
#define UNSORTED   1

#ifdef _INCLUDE_FROM_WB_BUFF_C_
#undef extern
#define extern
#endif /*_INCLUDE_FROM_WB_BUFF_C_*/

extern int buff_show(int bd);

extern int	buff_init ARGS_DECL((
			int n, int size, int option,
			int (*compar)(const void *, const void *)));

extern void	buff_cleanup ARGS_DECL((void));

extern int	buff_ins ARGS_DECL((int bd, const void *buffer));

extern int	buff_rem ARGS_DECL((int bd, void **buffer));

extern int	buff_destroy ARGS_DECL((int bd));

extern int	buff_empty ARGS_DECL((int bd));

extern int	buff_emptyn ARGS_DECL((int bd, int n));

extern int	buff_items ARGS_DECL((int bd));

extern int	buff_conc ARGS_DECL((int bd1, int bd2));

extern int	buff_sort ARGS_DECL((
			int bd, int (*compar)(const void *, const void *)));

#ifdef _INCLUDE_FROM_WB_BUFF_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_WB_BUFF_C_*/

#endif /*_WB_BUFF_E_*/
