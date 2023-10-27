/*
 * Author:     Denise Jorge de Oliveira <dezinha@land.ufrj.br> in Dec, 1999
 *
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/wb_seg.e,v 1.2 2005/07/26 18:30:27 william Exp $
 */
#ifndef  _WB_SEG_E
#define  _WB_SEG_E

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>

#ifndef FALSE
#define  FALSE            0
#define  TRUE             1
#endif /* ~FALSE */

#define  SEG_ID           256
#define  SEG_DATA_SIZE    512

typedef struct SegmentationPack{
    char    id[SEG_ID];
    char    data[SEG_DATA_SIZE];
    int     DataSize;
    int     LogicalClock;
    int     NumSeq;
    int     NumPackets;
}* ptrSegPack, SegPack;

typedef struct SegmentationList{
    char                    id[SEG_ID];
    int                     LogicalClock;
    int                     NumPackets;
    int                     ArrivedPcts;
    struct Fragment         *Frag;
    struct SegmentationList *prev, *next;
}* ptrSegList;

typedef struct Fragment{
    char    data[SEG_DATA_SIZE];
    int     DataSize;
}* ptrSegVec;

extern struct SegmentationList *first;
extern struct SegmentationList *last;

#ifdef _INCLUDE_FROM_WB_SEG_C_
#undef extern   
#define extern  
#endif /*_INCLUDE_FROM_WB_SEG_C_*/

extern void		InitializeSegmentation ARGS_DECL((void));
extern ptrSegPack	Segment ARGS_DECL((char *, int, char *, int, int *));
extern char		*DeSegment ARGS_DECL((struct SegmentationPack, int *));

#ifdef _INCLUDE_FROM_WB_SEG_C_
#undef extern
#ifndef _NO_RECURSIVE_EXTERN
#define extern extern
#endif /* ~_NO_RECURSIVE_EXTERN */
#endif /*_INCLUDE_FROM_WB_SEG_C_*/

#endif  /* _WB_SEG_E */
