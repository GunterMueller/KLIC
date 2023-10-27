/*
 * Author:     Denise Jorge de Oliveira <dezinha@land.ufrj.br> in Dec, 1999
 *
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/wb_seg.c,v 1.2 2005/07/26 18:30:27 william Exp $
 */
#ifndef  _SEG_C
#define  _SEG_C

#define _INCLUDE_FROM_WB_SEG_C_

#include "tgifdefs.h"

#include "wb_seg.e"

struct SegmentationList *first=NULL;
struct SegmentationList *last=NULL;

/************************************************************************/
static
struct SegmentationList *NewNodeList ( struct SegmentationPack Pack )
{
   struct SegmentationList *indice;
   
   indice = (struct SegmentationList *)malloc( sizeof(struct SegmentationList));
   
   memcpy(indice->id, Pack.id, SEG_ID);
   indice->LogicalClock = Pack.LogicalClock;
   indice->NumPackets   = Pack.NumPackets;
   indice->ArrivedPcts  = 0;
   indice->Frag = (struct Fragment *)malloc(Pack.NumPackets*(sizeof(struct Fragment)));
   indice->prev = last;
   indice->next = NULL;
   memset( (char *)indice->Frag, 0, Pack.NumPackets*(sizeof(struct Fragment)) );
   
   if( last != NULL )
   {
       last->next = indice;
       last       = indice;
   }
   else
   {
       first = last = indice;
   }
   
   return( last );
}
/************************************************************************/
static
void DeleteNode( struct SegmentationList *indice )
{
    struct SegmentationList *prev, *next;
    
    prev = indice->prev;
    if( prev == NULL )
        first = indice->next;
    else
        prev->next = indice->next;
        
    next = indice->next;
    if( next == NULL )
        last = indice->prev;
    else
        next->prev = indice->prev;
        
    free( indice->Frag );
    free( indice );
}
/************************************************************************/
static
void NewPacket ( struct SegmentationPack *pack, char *id, char *data, int datasize, int  LogicalClock, int  NumPackets, int  NumSeq)
{
   /* fprintf(stderr, "\n****Antes de Converter %d\n", NumSeq ); */
   memcpy(pack->id, id, SEG_ID);
   memcpy(pack->data, data, SEG_DATA_SIZE);
   pack->DataSize     = htonl(datasize);
   pack->LogicalClock = htonl(LogicalClock);
   pack->NumPackets   = htonl(NumPackets);
   pack->NumSeq       = htonl(NumSeq);
   /* fprintf(stderr, "\n&&&&DEPOS de Converter %d\n", pack->NumSeq ); */
}
/************************************************************************/
void InitializeSegmentation ()
{
   first = last = NULL; 
}
/************************************************************************/
ptrSegPack Segment (char * Buffer, int BufLen, char *id, int LogicalClock,
                    int * NPackets)
{
   int   divisor;
   int   offset;
   int   i;
   int   qtd;
   char  aux[SEG_DATA_SIZE];
   ptrSegPack pack;

   divisor = (int) (BufLen/SEG_DATA_SIZE);
    
   if( BufLen % SEG_DATA_SIZE != 0 )
     *NPackets = divisor + 1;
   else *NPackets = divisor;

   pack = (ptrSegPack) malloc ((*NPackets)*(sizeof(SegPack)));

   offset = 0;
   for (i = 0; i < *NPackets; i++, offset += SEG_DATA_SIZE) {
      if( BufLen - offset > SEG_DATA_SIZE )
          qtd = SEG_DATA_SIZE;
      else
          qtd = BufLen - offset;
      memcpy (aux, &Buffer[offset], qtd );
      NewPacket (&pack[i], id, aux, qtd, LogicalClock, *NPackets, i);
   }
   
   return( pack );
}
/******************************************************************************
    Verifica se ja´ existe uma entrada para a fonte do pacote Pack. Se sim,
retorna TRUE e a posicao dessa entrada em indice.
*******************************************************************************/
static
struct SegmentationList *Compare( struct SegmentationPack Pack )
{
   int status;
   struct SegmentationList *indice;
   
   status = FALSE;
   indice = first;
   
   while( indice != NULL )
   {
       if( Pack.LogicalClock == indice->LogicalClock )
       {
           if( strcmp( Pack.id, indice->id ) == 0 )
           {
               status = TRUE;
               break;
           }
       }
       
       indice = indice->next;
   }

   if( !status )
       indice = NULL;

   return( indice );
}
/**************************************************************************************
    Copia o novo fragmento para a sua posicao na lista. Retorna TRUE se todos os
fragmentos de um pacote chegaram.
**************************************************************************************/
static
int FillFragment(struct SegmentationPack Pack, struct SegmentationList *index)
{
    int status;
    
    status = FALSE;
    if( !index->Frag[ Pack.NumSeq ].DataSize )
    {
        memcpy( index->Frag[ Pack.NumSeq ].data, Pack.data, Pack.DataSize );
        index->Frag[ Pack.NumSeq ].DataSize = Pack.DataSize;

        index->ArrivedPcts++;

        if( index->ArrivedPcts == index->NumPackets )
            status = TRUE;
    }
        
    return( status );
}
/************************************************************************/
static
void ConvertNetworkToHost (struct SegmentationPack *pack)
{
   /* fprintf(stderr, "\n****Antes de desconverter %d\n", pack->NumSeq ); */
   pack->DataSize     = ntohl(pack->DataSize);
   pack->LogicalClock = ntohl(pack->LogicalClock);
   pack->NumPackets   = ntohl(pack->NumPackets);
   pack->NumSeq       = ntohl(pack->NumSeq);
}
/************************************************************************/
char *DeSegment (struct SegmentationPack Pack, int *NumBytes )
{
    struct SegmentationList *indice;
    int packetLength;
    char *buffer;
    int i;

    indice = NULL;
    buffer = NULL;
    *NumBytes = packetLength = 0;
    ConvertNetworkToHost(&Pack);
    /* fprintf(stderr, "Chegou pacote %d\n", Pack.NumSeq ); */
    /* Check if this source was already inserted */    
    if( (indice = Compare( Pack )) == NULL )
        indice = NewNodeList( Pack );
    
    /* If this is the last fragment */
    if( FillFragment( Pack, indice ) )
    {
        packetLength = (indice->NumPackets - 1) * SEG_DATA_SIZE +
                        indice->Frag[indice->NumPackets - 1].DataSize;
        buffer = (char *)malloc( packetLength );
        for( i = 0; i < indice->NumPackets; i++ )
            memcpy( &buffer[ i * SEG_DATA_SIZE ], (char *)indice->Frag[i].data, indice->Frag[i].DataSize );

        *NumBytes = packetLength;           
        DeleteNode( indice );
    }
    
    return( buffer );
}
/************************************************************************/

#endif  /* _SEG_C */
