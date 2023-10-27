/*
 * Copyright (C) 2002-2009, Edmundo Albuquerque de Souza e Silva.
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
 */

/***************************************************************************
                             rmcache.c
                             -------------------
    begin                : May 2001
    Authors              : Jorge Allyson Azevedo
                           Milena Scanferla
                           Daniel Sadoc
    email                : {allyson,milena,sadoc}@land.ufrj.br
 ***************************************************************************/

#ifndef RMCACHE_C

#define RMCACHE_C

#include "rmstruct.h"

#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include "rmcache.h"
#include "rmwinmask.h"

#ifndef RMINTERNALS_H
    #include "rminternals.h"
#endif

extern GLOBAL_OPTIONS rmcast_options;

#ifdef SOLARIS
pthread_mutex_t   cache_mutex;
#else
pthread_mutex_t   cache_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

/*---------------- Auxiliary functions to manipulate the cache ---------------*/

/*******************************************************************************
 *
 * int cacheAllocHeaderNode(CACHE **cache, MEMBER_ID *member)
 *
 * Allocates a cache header node.
 *
 * Arguments:    cache, the cache;
 *              member, the member to be inserted.
 *
 * Returns: 0 on error, 1 otherwise.
 *
 ******************************************************************************/
int cacheAllocHeaderNode(CACHE **cache, MEMBER_ID *member)
{
    if( ( *cache = (CACHE*)malloc( sizeof( CACHE ) ) ) == NULL )
    {
        fprintf( stderr, "cacheAllocHeaderNode ERROR: Allocating cache header node\n" );
        return 0;
    }
    bzero( (*cache), sizeof( CACHE ) );
    (*cache)->active = 1;

    if( member != NULL )
        (*cache)->sm_info.member_id = *member;

    (*cache)->sm_info.member_status.last_identified = -1;
    (*cache)->sm_info.member_status.first_rcv       = -1;
    (*cache)->sm_info.member_status.last_rcv        = -1;
    (*cache)->sm_info.member_status.last_seq_rcv    = -1;

#ifdef SINGLE_NACK

    (*cache)->sm_info.member_status.window_size = MAX_WINDOW_SIZE;
    (*cache)->sm_info.member_status.window_ini  = 0;
    memset( (*cache)->sm_info.member_status.window_mask, 0, MAX_WINDOW_SIZE );

#ifdef DSINGLE_NACK
    {
        int h = 0;
        int l = 0;
        window_mask_array2numbers( (*cache)->sm_info.member_status.window_mask, &h, &l, (*cache)->sm_info.member_status.window_ini );
        fprintf( stderr, "DSINGLE_NACK cacheAllocHeaderNode: window_mask h=%d l=%d\n", h, l );
    }
#endif

#endif

    return 1;
}

/*******************************************************************************
 *
 * int cacheAllocMessageNode(CACHE_NODE **cache_node, DATA_PACKET *node_info)
 *
 * Allocates a cache message node.
 *
 * Arguments: cache_node, the cache node to be created, it'll be allocated here;
 *            node_info, information about the message node to be created.
 *
 * Returns: 0 on error, 1 otherwise.
 *
 ******************************************************************************/
int cacheAllocMessageNode( CACHE_NODE **cache_node, DATA_PACKET *node_info )
{
    if( ( *cache_node = (CACHE_NODE*)malloc( sizeof( CACHE_NODE ) ) ) == NULL )
    {
        fprintf( stderr, "cacheAllocMessageNode ERROR: allocating cache message node" );
        return 0;
    }

    bzero( *cache_node, sizeof( CACHE_NODE ) );
    (*cache_node)->data_pckt.sn        = node_info->sn;
    (*cache_node)->data_pckt.data_size = node_info->data_size;
    (*cache_node)->data_pckt.data      = (BYTE*)malloc( ( node_info->data_size + 1 ) * sizeof( BYTE ) );

    memcpy( (*cache_node)->data_pckt.data,node_info->data, ( node_info->data_size + 1 ) );

    /* Observation:
       we allocate and copy data_size data bytes, 
       plus one byte for the string termination caracter */

    return 1;
}

/*******************************************************************************
 *
 * int cacheFreeMessageNode(CACHE_NODE **node)
 *
 * Free a cache message node.
 *
 * Arguments:    cache_node, the cache node to be released.
 *
 * Returns: 1 allways.
 *
 ******************************************************************************/
int cacheFreeMessageNode( CACHE_NODE **node )
{
    free( (*node)->data_pckt.data );
    free( *node );
    *node = NULL;
    return 1;
}

/*---------------- Main functions to manipulate the cache --------------------*/

/*******************************************************************************
 *
 * void cacheInit(CACHE **cache)
 *
 * Initialize the cache.
 *
 * Arguments:    cache, the cache;
 *
 ******************************************************************************/
void cacheInit( CACHE **cache )
{

#ifdef SOLARIS
    pthread_mutex_init( &cache_mutex, NULL );
#endif

    *cache = NULL;
}

/*******************************************************************************
 *
 * int cacheInsertMessage(CACHE **cache, MEMBER_ID *member, DATA_PACKET *data_packet)
 *
 * Inserts a message in the cache.
 *
 * Arguments:    cache, the cache;
 *                member, the member who originaly sent the message;
 *                data_packet, message data.
 *
 * Side Effects:    this routine updates, in the cache, the following fields regarding
 *                  the member who originaly sent the message:
 *
 *                      - number_of_nodes
 *
 *                      SM_INFO->MEMBER_STATUS
 *
 *                          - first_rcv
 *                          - last_rcv
 *                          - last_seq_rcv
 *                          - last_identified
 *                      #ifdef SINGLE_NACK
 *                          - window_size;
 *                          - window_mask[MAX_WINDOW_SIZE];
 *                          - window_ini
 *                      #ifdef
 *
 *                      - CACHE_NODE *first
 *                      - CACHE_NODE *last
 *                      - CACHE_NODE *last_inserted;
 *
 * Return value:    0,    on error;
 *                  1,    if the inserted message was in sequence,
 *                        i.e. the message received was the one expected;
 *                  2,    if the inserted message was not is sequence.
 *
 ******************************************************************************/
#define CHECK_INTEGRITY_OF_CACHE                                                                  \
                                                                                                  \
        if( newMessageCacheNode -> next == NULL && memberHeaderNode->last != newMessageCacheNode )\
        {                                                                                         \
            fprintf(stderr,"ATTENTION!!!! memberHeaderNode->last != newMessageCacheNode           \
                && newMessageCacheNode->next == NULL" );                                          \
        }

int cacheInsertMessage( CACHE **cache, MEMBER_ID *member, DATA_PACKET *data_packet )
{
    int count   = 0;
    int isfirst = 0;
    CACHE *memberHeaderNode = NULL;
    CACHE_NODE *newMessageCacheNode = NULL, *auxMessageNode = NULL;

    /* ============== Allocating space for the new message ================== */

    memberHeaderNode = cacheLookForMember(cache, member);

    if( memberHeaderNode == NULL )
    {
        if( ( memberHeaderNode = cacheInsertMember( cache, member ) ) == NULL)
        {
            return 0;
        }
    }

    if( memberHeaderNode->number_of_nodes == rmcast_options.max_member_cache_size )
    {
        if( memberHeaderNode->last->data_pckt.sn <= memberHeaderNode->sm_info.member_status.last_seq_rcv )
        {
            /* The last node can be removed only if it was sent to the application */
            ((memberHeaderNode->last)->previous)->next = NULL;
            auxMessageNode = (memberHeaderNode->last);
            (memberHeaderNode->last) = ((memberHeaderNode->last)->previous);
            cacheFreeMessageNode( &auxMessageNode );
            memberHeaderNode->number_of_nodes--;
        }
        else
        {
            return 0;
        }
    }

    if( cacheAllocMessageNode( &newMessageCacheNode, data_packet ) == (int)NULL )
    {
        return 0;
    }

    /* ================ Inserting the new message =========================== */

    if( memberHeaderNode->number_of_nodes == 0 )
    {
        /* we will insert the first message node*/
        memberHeaderNode->last = memberHeaderNode->first = newMessageCacheNode;
        isfirst = 1;

#ifdef DEBUG_CACHE
        fprintf( stderr, "DEBUG_CACHE cacheInsertMessage: we will insert the first message node\n" );
#endif
    }
    else
    {
#ifdef DEBUG_CACHE
        fprintf( stderr, "DEBUG_CACHE cacheInsertMessage: a message node will be inserted\n" );
#endif
        auxMessageNode = memberHeaderNode->first;

        while( auxMessageNode != NULL )
        {
            if(auxMessageNode->data_pckt.sn <= newMessageCacheNode->data_pckt.sn )
                break;
            auxMessageNode = auxMessageNode->next;
            count++;
        }

        if( auxMessageNode )
        {
            if( auxMessageNode->data_pckt.sn == newMessageCacheNode->data_pckt.sn )
            {
                return 0; /* repeated node */
            }
        }

        if( count == 0 )
        {
            memberHeaderNode->first = newMessageCacheNode;
        }

#ifdef DEBUG_CACHE
        fprintf( stderr, "DEBUG_CACHE cacheInsertMessage: adjusting pointers\n" );
#endif

        if( auxMessageNode )
        {
            newMessageCacheNode->next     = auxMessageNode;
            newMessageCacheNode->previous = auxMessageNode->previous;

            if( auxMessageNode->previous != NULL )
                auxMessageNode->previous->next = newMessageCacheNode;

            auxMessageNode->previous = newMessageCacheNode;
        }
        else
        {
#ifdef DEBUG_CACHE
            fprintf( stderr, "DEBUG_CACHE cacheInsertMessage: inserting at the end..." );
            cacheShow( *cache );
#endif
            newMessageCacheNode->previous = memberHeaderNode->last;
            memberHeaderNode->last->next  = newMessageCacheNode;
            memberHeaderNode->last        = newMessageCacheNode;
        }
#ifdef DEBUG_CACHE
        CHECK_INTEGRITY_OF_CACHE;
#endif
    }

#ifdef DEBUG_CACHE
    fprintf( stderr, "DEBUG_CACHE cacheInsertMessage: node (%d) inserted in cache\n", data_packet->sn );
#endif

    /* Updating fields regarding the member  who originaly sent the msg ======*/

    memberHeaderNode->last_inserted = newMessageCacheNode;
    memberHeaderNode->number_of_nodes++;

    if( memberHeaderNode->sm_info.member_status.first_rcv == -1 )
        memberHeaderNode->sm_info.member_status.first_rcv = data_packet->sn;

    memberHeaderNode->sm_info.member_status.last_rcv = data_packet->sn;

    if( memberHeaderNode->sm_info.member_status.last_identified < data_packet->sn )
    {
        memberHeaderNode->sm_info.member_status.last_identified  = data_packet->sn;
    }


    if( ( memberHeaderNode->sm_info.member_status.last_seq_rcv + 1 ) == newMessageCacheNode->data_pckt.sn )
    {
        while( ( newMessageCacheNode != NULL ) &&
                  ( memberHeaderNode->sm_info.member_status.last_seq_rcv + 1 == newMessageCacheNode->data_pckt.sn ) )
        {
            memberHeaderNode->sm_info.member_status.last_seq_rcv++;

#ifdef SINGLE_NACK
            {

#ifdef DSINGLE_NACK
                int l = 0;
                int h = 0;

                window_mask_array2numbers( memberHeaderNode->sm_info.member_status.window_mask,
                                           &h,
                                           &l,
                                           memberHeaderNode->sm_info.member_status.window_ini);

               /* fprintf(stderr,"DSINGLE_NACK cacheInsertMessage: window_mask before l=%d h=%d\n",l,h); */
#endif

                window_mask_set_bit2( memberHeaderNode->sm_info.member_status.window_mask, memberHeaderNode->sm_info.member_status.window_ini, 0 );
                memberHeaderNode->sm_info.member_status.window_ini = ( memberHeaderNode->sm_info.member_status.window_ini + 1 ) % MAX_WINDOW_SIZE;

#ifdef DSINGLE_NACK
                window_mask_array2numbers( memberHeaderNode->sm_info.member_status.window_mask,
                       &h,
                       &l,
                       memberHeaderNode->sm_info.member_status.window_ini);
#endif
            }
#endif
            newMessageCacheNode = newMessageCacheNode -> previous;
        }
#ifdef DEBUG_CACHE
        fprintf( stderr, "DEBUG_CACHE cacheInsertMessage: node (%d) inserted in cache, in sequence\n", data_packet->sn );
#endif
        return 1;
    }
    else
    {
#ifdef SINGLE_NACK

        if( ( memberHeaderNode->sm_info.member_status.last_seq_rcv <= data_packet->sn ) &&
             ( data_packet->sn <= memberHeaderNode->sm_info.member_status.last_seq_rcv + MAX_WINDOW_SIZE ) )
        {
            memberHeaderNode->sm_info.member_status.window_mask[ ( memberHeaderNode->sm_info.member_status.window_ini +
                                                                   data_packet->sn -
                                                                   (memberHeaderNode->sm_info.member_status.last_seq_rcv + 1 ) ) % MAX_WINDOW_SIZE ] = 0;
        }
#endif

#ifdef DEBUG_CACHE
        fprintf( stderr, "DEBUG_CACHE cacheInsertMessage: node (%d) inserted in cache, not in sequence\n", data_packet->sn );
#endif
        /* the packet is not in sequence - it will be needed to send a nak */
        return 2;
    }
}


/*******************************************************************************
 *
 * void cachePack(CACHE **cache, char **buf, int *buffsize)
 *
 * Packs the cache information in a buffer.
 *
 * Arguments:    cache, the cache;
 *                buf, points to the place where the packed info will be stored;
 *                buffsize, returns the number of bytes in buf.
 *
 * Side effects: this routine allocates the buf variable, and stores its
 *               final size in buffsize.
 *
 ******************************************************************************/
void cachePack( CACHE **cache, char **buf, int *buffsize )
{
    CACHE *current_header_node;
    char *buf_next_write_pos;
    int nelem = 0;

    current_header_node = *cache;
    *buf = NULL;

    while( current_header_node != NULL )
    {
        nelem ++;

        /* allocating space to store info about the new cache header node */
        *buf = realloc( *buf, ( nelem * sizeof(CACHE)) );

        /* writing the information about the new cache header node */
        buf_next_write_pos = *buf + ( (nelem - 1) * sizeof(CACHE) );
        memcpy(buf_next_write_pos, current_header_node, sizeof(CACHE) );

        current_header_node = current_header_node->next;
    }

    *buffsize = nelem * sizeof(CACHE);
}


/*******************************************************************************
 *
 * void cacheUnpack(CACHE **cache, char *buf, int buffsize)
 *
 * Unpacks the cache information contained in a buffer.
 *
 * Arguments:  cache, the cache;
 *             buf, points to the place from where the packed info will be read;
 *             buffsize, the number of bytes in buf.
 *
 * Side effects: updates the cache.
 *
 ******************************************************************************/
void cacheUnpack( CACHE **cache, char *buf, int buffsize )
{
    char *buf_next_read_pos;
    /* number of bytes read */
    int nbread = 0;
    CACHE *cur_member;
    CACHE cur_member_buf;

    buf_next_read_pos = buf;

    for( nbread = 0; nbread < buffsize; nbread += sizeof( CACHE ) )
    {
        memcpy( &cur_member_buf, buf_next_read_pos, sizeof( CACHE ) );
        cur_member = cacheInsertMember( cache, &(cur_member_buf.sm_info.member_id ) );
        cur_member->sm_info.member_status                 = cur_member_buf.sm_info.member_status;
        cur_member->sm_info.member_status.last_rcv        = cur_member->sm_info.member_status.last_seq_rcv;
        cur_member->sm_info.member_status.last_identified = cur_member->sm_info.member_status.last_seq_rcv;

        buf_next_read_pos += sizeof( CACHE );
    }
}

/*******************************************************************************
 *
 * CACHE *cacheInsertMember(CACHE **cache, MEMBER_ID *member)
 *
 * Inserts a member in the cache.
 *
 * Arguments:    cache, the cache;
 *                member, the member identification;
 *
 * Return value:    a pointer to the inserted member; NULL on error.
 *
 ******************************************************************************/
CACHE *cacheInsertMember( CACHE **cache, MEMBER_ID *member )
{
    CACHE *auxHeaderNode = *cache, *newHeaderNode;

    if( cacheAllocHeaderNode( &newHeaderNode, member ) == (int)NULL )
    {
        return NULL;
    }

    if( cacheIsEmpty( cache ) )
    {
        /* cache is empty */
        *cache = newHeaderNode;
    }
    else
    {
        /* inserts new node in the end of the list */
#ifdef DEBUG_CACHE
        fprintf( stderr, "DEBUG_CACHE cacheInsertMember: searching where to insert new member node...\n" );
#endif
        while( auxHeaderNode->next != NULL )
        {
            auxHeaderNode = auxHeaderNode->next;
        }

#ifdef DEBUG_CACHE
        fprintf( stderr, "DEBUG_CACHE cacheInsertMember: member node will be inserted...\n" );
#endif
        auxHeaderNode->next = newHeaderNode;
    }
    return newHeaderNode;
}


/*******************************************************************************
 *
 * int cacheIsEmpty(CACHE **cache)
 *
 * Checks if the cache is empty.
 *
 * Arguments:    cache, the cache;
 *
 * Return value:    1 if the cache is empty; 0 otherwise.
 *
 ******************************************************************************/
int cacheIsEmpty( CACHE **cache )
{
    if( *cache == NULL )
        return 1;
    else
        return 0;
}


/*******************************************************************************
 *
 * int cacheOfAnyMemberIsFull(CACHE *cache)
 *
 * Checks if the cache space reserved for any member is full.
 *
 * Arguments:    cache, the cache;
 *
 * Return value:    1 if the the cache of any member is full; 0 otherwise.
 *
 ******************************************************************************/
int cacheOfAnyMemberIsFull( CACHE *cache )
{
    CACHE *auxCacheHeaderNode = cache;
    while( auxCacheHeaderNode->number_of_nodes < rmcast_options.max_member_cache_size )
    {
        auxCacheHeaderNode = auxCacheHeaderNode->next;
        if( auxCacheHeaderNode == NULL )
            return 0;
    }
    return !( auxCacheHeaderNode->number_of_nodes < rmcast_options.max_member_cache_size );
}

int memberIsEqual( MEMBER_ID *member1, MEMBER_ID *member2 )
{
    if( ( member1->pid == member2->pid ) && ( !strcmp( member1->ip, member2->ip ) ) )
        return 1;
    else
        return 0;
}

/*******************************************************************************
 *
 * CACHE* cacheLookForMember(CACHE **cache1, MEMBER_ID *member_id)
 *
 * Searches for a member in the cache.
 *
 * Arguments:    cache, the cache;
 *                member_id, the member identification.
 *
 * Return value:    a pointer to the requested member node,
 *                  or NULL if the member does not exist.
 *
 ******************************************************************************/
CACHE* cacheLookForMember( CACHE **cache1, MEMBER_ID *member_id )
{
    CACHE *auxHeaderNode = NULL;
    CACHE *cache         = NULL;

    if( !cacheIsEmpty( cache1 ) && ( member_id != NULL ) )
    {
        cache         = *cache1;
        auxHeaderNode = cache;

        while( !memberIsEqual( member_id, &auxHeaderNode->sm_info.member_id ) )
        {
            auxHeaderNode = auxHeaderNode->next;
            if( auxHeaderNode == NULL )
                break;
        }
    }
#ifdef DEBUG_CACHE
    else
    {
        fprintf( stderr, "DEBUG_CACHE cacheLookForMember: Cache is empty.\n" );
    }
#endif
    return (auxHeaderNode);
}

/*******************************************************************************
 *
 * int cacheDesactivateMember(CACHE *cache, MEMBER_ID *member)
 *
 * Deactivate a cache member, setting its activate bit to zero.
 *
 * Arguments:    cache, the cache;
 *                member_id, the member to be deactivated.
 *
 * Return value:    1 on success, 0 on fail.
 *
 ******************************************************************************/
int cacheDeactivateMember( CACHE *cache, MEMBER_ID *member )
{
    CACHE* auxCacheHeaderNode = cacheLookForMember( &cache,member );
    if( auxCacheHeaderNode != NULL )
    {
        auxCacheHeaderNode -> active = 0;
        return 1;
    }
    else
        return 0;
}

/*******************************************************************************
 *
 * void cacheShow(CACHE *cache)
 *
 * Show the cache on the screen.
 *
 * Arguments:    cache, the cache;
 *
 ******************************************************************************/
void cacheShow( CACHE *cache )
{
#ifdef DEBUG_SHOW
    CACHE* auxCacheHeaderNode = cache;
    CACHE_NODE *auxMessageNode;
    char aux_string[ IP_STRING_SIZE ];
    int  i;
#ifdef SINGLE_NACK
#ifdef DSINGLE_NACK
    int  h = 0;
    int  l = 0;
    char aux_str[ 200 ];
#endif
#endif

    fprintf( stderr, "\n-----cacheShow-------------------------------------------------------\n" );

    while( auxCacheHeaderNode != NULL )
    {
        for ( i = 0; i < IP_STRING_SIZE; i++ )
            aux_string[ i ] = auxCacheHeaderNode->sm_info.member_id.ip[ i ];

        auxMessageNode = auxCacheHeaderNode->first;

        fprintf( stderr, "[%x %x %sactive {first:%d last:%d last_seq:%d last identified:%d}] (%d nodes) %s %d :\n",
            (unsigned int)auxCacheHeaderNode->first,
            (unsigned int)auxCacheHeaderNode->last,
            (auxCacheHeaderNode->active==1?"   ":"des"),
            auxCacheHeaderNode->sm_info.member_status.first_rcv,
            auxCacheHeaderNode->sm_info.member_status.last_rcv,
            auxCacheHeaderNode->sm_info.member_status.last_seq_rcv,
            auxCacheHeaderNode->sm_info.member_status.last_identified,
            auxCacheHeaderNode->number_of_nodes,
            aux_string,
            auxCacheHeaderNode->sm_info.member_id.pid
            );
#ifdef SINGLE_NACK
#ifdef DSINGLE_NACK
        window_mask_array2numbers( auxCacheHeaderNode->sm_info.member_status.window_mask, &h, &l, auxCacheHeaderNode->sm_info.member_status.window_ini );

        sprintf( aux_str, "DSINGLE_NACK (DEBUG_SHOW) cacheShow: window_ini=%d h=%d l=%d\n", auxCacheHeaderNode->sm_info.member_status.window_ini, h, l );
        for( i = 0; i < MAX_WINDOW_SIZE; i++ )
            sprintf( aux_str, "%s%s%d", aux_str, ( i == auxCacheHeaderNode->sm_info.member_status.window_ini )?"*":"", auxCacheHeaderNode->sm_info.member_status.window_mask[ i ] );

        fprintf( stderr, "%s\n", aux_str );
#endif
#endif /* SINGLE_NACK */

#ifdef DCACHE_CONTENTS
        while( auxMessageNode != NULL )
        {
            fprintf( stderr, " \t([%08x %08x %08x] sn: %d pck size: %d %s)\n",
                (unsigned int)auxMessageNode->previous,
                (unsigned int)auxMessageNode,
                (unsigned int)auxMessageNode->next,
                auxMessageNode->data_pckt.sn,
                auxMessageNode->data_pckt.data_size,
                (char*)auxMessageNode->data_pckt.data
                );
            auxMessageNode = auxMessageNode->next;
        }
#endif
        auxCacheHeaderNode = auxCacheHeaderNode->next;
    }

    fprintf( stderr, "\n--------------------------------------------------------------------\n\n" );
#endif
}

/*******************************************************************************
 *
 * void cacheGetSMInfo(CACHE *cache, SM_INFO **sm_table, int *nusers)
 *
 * Gets session message information from the cache.
 *
 * Arguments: cache,    the cache;
 *            sm_table, a pointer to where the destination info. will be stored -
 *                      it´s allocated here;
 *            nusers,   returns the number of users in the cache -
 *                      it must be previously allocated.
 * Returns: 1, on success;
 *          0, on error.
 *
 ******************************************************************************/
int cacheGetSMInfo( CACHE *cache, SM_INFO **sm_table, int *nusers )
{
    int i = 0;
    CACHE* auxCacheHeaderNode = cache;

    *sm_table = NULL;
    *nusers   = 0;

    for( auxCacheHeaderNode=cache; auxCacheHeaderNode != NULL; auxCacheHeaderNode=auxCacheHeaderNode->next )
    {
        if( ( *sm_table = (SM_INFO*)realloc( *sm_table, ( i + 1 )*sizeof( SM_INFO ) ) ) == NULL )
        {
            fprintf(stderr, "cacheGetSMInfo ERROR: allocating sm_info\n" );
            return 0;
        }

        memcpy( (*sm_table)+i, &auxCacheHeaderNode->sm_info, sizeof( SM_INFO ) );
        i++; (*nusers)++;
    }
    return 1;
}

/*******************************************************************************
 *
 * DATA_PACKET *cacheLookForMessage(CACHE **cache1, MEMBER_ID *member_id,int message_sn)
 *
 * Searches for a message in the cache.
 *
 * Arguments:    cache1,     the cache;
 *                member_id,    original sender of the message information;
 *                message_sn, message sequence number;
 *
 * Returns:     a pointer to the data packet, or NULL when not found.
 *
 ******************************************************************************/
DATA_PACKET *cacheLookForMessage( CACHE **cache1, MEMBER_ID *member_id, int message_sn )
{
    CACHE *auxMember;
    CACHE_NODE *auxMsg;
    CACHE *cache;
    DATA_PACKET *auxDatapkg;

    cache      = *cache1;
    auxDatapkg = NULL;

#ifdef DSINGLE_NACK
      fprintf( stderr, "DSINGLE_NACK cacheLookForMessage: entering, looking for %s:%d:%d\n", member_id->ip, member_id->pid, message_sn );
#endif
#ifdef DNAK_RCV
      fprintf( stderr, "DNAK_RCV cacheLookForMessage: entering, looking for %s:%d:%d\n", member_id->ip, member_id->pid, message_sn );
#endif
    if( ( auxMember = cacheLookForMember( &cache, member_id ) ) != NULL )
    {
        auxMsg = auxMember->first;

        while( ( auxMsg != NULL ) && ( auxMsg->data_pckt.sn != message_sn ) )
        {
            auxMsg=auxMsg->next;
        }

        if( auxMsg != NULL && auxMsg->data_pckt.sn == message_sn )
        {
            auxDatapkg=&auxMsg->data_pckt;
        }
    }

    return auxDatapkg;
}

/*---- Auxiliary functions to manipulate the nak list ------------------------*/

/*******************************************************************************
 *
 * int cacheAllocNAKNode(NAK_LIST **nak_node, int sn)
 *
 * Allocates a new nak (not acknowledge) node, and fills it with specified info.
 *
 * Arguments:    nak_node, the node which will be created;
 *                sn, the sequence number of the nak node.
 *
 * Returns:     0 on error, 1 on success.
 *
 ******************************************************************************/
int cacheAllocNAKNode( NAK_LIST **nak_node, int sn )
{
    if( ( *nak_node = (NAK_LIST*)malloc( sizeof( NAK_LIST ) ) ) == NULL )
    {
        fprintf( stderr, "Error allocating cache message node\n" );
        return 0;
    }

    bzero( *nak_node, sizeof( NAK_LIST ) );
    (*nak_node)->sn      = sn;
    /* at least one nak was sent */
    (*nak_node)->counter = 1;

    return 1;
}

/*******************************************************************************
 *
 * int cacheUpdateNakList(CACHE **cache1, MEMBER_ID *member, int sn)
 *
 * Updates the list of naks sent with respect to a specific member.
 *
 * Arguments:    cache1, the cache;
 *                member, the original sender of the message;
 *                sn, the sequence number of the message that we will wait for.
 *
 * Returns:     0 on error, 1 on success.
 *
 ******************************************************************************/
int cacheUpdateNakList( CACHE **cache1, MEMBER_ID *member, int sn )
{
    NAK_LIST *auxNakNode, *aux2, *new_nak_node;
    CACHE *memberHeaderNode;
    CACHE *cache;

    aux2  = NULL;
    cache = *cache1;

    if( ( memberHeaderNode = cacheLookForMember( &cache, member ) ) == NULL )
    {
        fprintf( stderr, "cacheUpdateNakList - Error: member (%s:%d) not found in cache.\n", member->ip, member->pid );
        return 0;
    }

    auxNakNode = memberHeaderNode->nak_list;

    if( auxNakNode == NULL )
    {
        if( cacheAllocNAKNode( &new_nak_node, sn ) == 0 )
        {
            fprintf( stderr, "cacheUpdateNakList - Error: cacheAllocNAKNode failed.\n" );
            return 0;
        }
        memberHeaderNode->nak_list = new_nak_node;
    }
    else
    {
        while( auxNakNode != NULL )
        {
            if( auxNakNode->sn != sn )
            {
                aux2       = auxNakNode;
                auxNakNode = auxNakNode->next;
            }
            else
            {
                if( auxNakNode->counter >= rmcast_options.max_nak )
                {
                    return 0;
                }
                else
                {
                    auxNakNode->counter++;
                    break;
                }
            }
        }

        if( auxNakNode == NULL )
        {
            if( cacheAllocNAKNode( &new_nak_node, sn ) == 0 )
            {
                fprintf( stderr, "cacheUpdateNakList - Error: cacheAllocNAKNode failed.\n" );
                return 0;
            }
            aux2->next = new_nak_node;
        }
    }

    return 1;
}

int cacheDeleteNakNode( CACHE *cache, MEMBER_ID *member, int sn )
{
    NAK_LIST *auxNakNode, *auxNakNode2;
    CACHE *memberHeaderNode;
    int retval = 1;
    int node_not_found = 1;

    if( ( memberHeaderNode = cacheLookForMember( &cache, member ) ) != NULL )
    {
        auxNakNode = memberHeaderNode->nak_list;

        while( ( auxNakNode != NULL ) && node_not_found )
        {
            if( auxNakNode->sn != sn )
            {
                auxNakNode = auxNakNode->next;
            }
            else
            {
                auxNakNode2       = auxNakNode;
                auxNakNode        = auxNakNode->next;
                auxNakNode2->next = NULL;
                free( auxNakNode2 );
                node_not_found    = 0;
                fprintf( stderr, "nak node removed: (%s:%d:%d)\n", member->ip, member->pid, sn);
            }
        }

        if( node_not_found )
        {
            fprintf( stderr, "cacheDeleteNakNode - Error: Nak node (%s:%d:%d) not found.\n", member->ip, member->pid, sn );
            retval = 0;
        }
    }
    else
    {
        fprintf( stderr, "cacheDeleteNakNode - Error: member (%s:%d) not found in cache.\n", member->ip, member->pid );
        retval = 0;
    }

    return retval;
}

/*---- End of auxiliary functions to manipulate the nak list -----------------*/

/*******************************************************************************
 *
 * int cacheCountMembers( CACHE *cache, int member_status_required)
 *
 * Count the members in the cache according to the status specified in member_status_required
 *
 * Arguments:    cache, pointer to the first node of the cache
 *                member_status_required, the member status required to consider a member
 *                                      while counting
 *
 * Returns:     Number of members found in the cache according to the status specified
 *              in member_status_required
 *
 ******************************************************************************/
int cacheCountMembers( CACHE *cache, int member_status_required )
{
    CACHE* auxCacheHeaderNode = cache;
    int member_counter        = 0;

    while( auxCacheHeaderNode != NULL )
    {
        if( member_status_required == CACHE_STATUS_ANY )
        {
            /* We must count all registered members */
            member_counter++;
        }
        else
        {
            /* We will count only the members who has active == member_status_required */
            if( auxCacheHeaderNode->active == member_status_required )
                member_counter++;
        }

        auxCacheHeaderNode = auxCacheHeaderNode->next;
    }

    return( member_counter );
}
#endif /* RMCACHE_C */
