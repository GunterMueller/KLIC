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
                             rmmsg.c
                             -------------------
    begin                : May 2001
    Authors              : Jorge Allyson Azevedo
                           Milena Scanferla
                           Daniel Sadoc
    email                : {allyson,milena,sadoc}@land.ufrj.br
 ***************************************************************************/

#ifndef MSG_C
#define MSG_C

#include <pthread.h>
#include "rmmsg.h"

int s_qid = -1; /* message queue identifier
                 * -1: queue not initialized
                 * -2: queue removed
                 * otherwise: queue identifier
                 */
#ifdef SOLARIS
pthread_mutex_t remove_queues_lock;
#else
pthread_mutex_t remove_queues_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

/*****************************************************************************************************
 *
 * int init_queue()
 *
 * Initializes the message queue.
 *
 * Side effects: updates s_qid.
 * Returns > 0 id if OK
 *         -1 if error
 *
 ******************************************************************************************************/
int init_queue()
{
    int queue_id;
    int QKEY;

#ifdef SOLARIS
    pthread_mutex_init(&remove_queues_lock,NULL);
#endif
    QKEY = QKEY_BASE;
    while( ( queue_id = msgget( QKEY, IPC_CREAT | QPERM | IPC_EXCL ) ) == -1 )
    {
        if( errno == EEXIST )
        {
#ifdef DEBUG_NET_MSGS
            fprintf( stderr,"DEBUG_NET_MSGS init_queue: Message queue key %d already used. Trying %d. \n",
                QKEY,
                QKEY + 1 );
#endif
            QKEY++;
        }
        else
        {
            perror( "init_queue: msgget failed" );
            return -1;
        }
    }
    s_qid = queue_id;

    return( queue_id );
}

/*****************************************************************************************************
 *
 * void remove_queues(int i)
 *
 * Removes the message queue.
 *
 * Side effects: sets s_qid to -2 on success.
 *
 ******************************************************************************************************/
void remove_queues( int i )
{

    pthread_mutex_lock( &remove_queues_lock );
    if( s_qid != -2 )
    {
        if( msgctl( s_qid, IPC_RMID, NULL ) == -1 )
            fprintf( stderr, "remove_queues ERROR: Failed to remove message queue %d.\n", s_qid );
    }

    s_qid = -2;
    pthread_mutex_unlock( &remove_queues_lock );
}

/*****************************************************************************************************
 *
 * int messageQueueEnter(char *objname, int len)
 *
 * Puts a message of size len in the message queue.
 *
 * Arguments:    objname,    the message to be enqueued;
 *                len,        the size of the message.
 *
 * Return value: 0 on success;
 *              -1 on error.
 *
 ******************************************************************************************************/
int messageQueueEnter( char *objname, int len )
{
    struct q_entry s_entry;
    int aux, ind;

    pthread_mutex_lock( &remove_queues_lock );

    if (len > MAXOBN)
    {
        fprintf( stderr, "messageQueueEnter ERROR: Name too long!\n" );
        return -1;
    }

    if( s_qid == -1 )
        s_qid = init_queue();

    if ( s_qid == -1 || s_qid == -2 )
    {
        fprintf( stderr,"messageQueueEnter ERROR: init_queue() failed!\n" );
        return -1;
    }

#ifdef DEBUG_NET_MSGS
    fprintf( stderr,"DEBUG_NET_MSGS messageQueueEnter: preparing to send data to app.\n" );
#endif

    s_entry.mtype = ( long )1;

    /* converts len from base 10 to base 256 - least signif. digit is stored in mtext[0] */

    for( aux = len, ind = 0; aux != 0; aux = aux/256, ind++ )
        s_entry.mtext[ ind ] = aux % 256;

    /* FIXME: the following test is really necessary? */
    if( ind > MSG_QUEUE_HEADER_SIZE )
    {
        fprintf( stderr, "messageQueueEnter ERROR: You must increment the MSG_QUEUE_HEADER_SIZE. ind=%d\n", ind );
        return -1;
    }

    memcpy( &(s_entry.mtext[MSG_QUEUE_HEADER_SIZE]), objname, len );

#ifdef DEBUG_NET_MSGS
    fprintf( stderr, "DEBUG_NET_MSGS messageQueueEnter: data ready!\n" );
#endif

    if( msgsnd( s_qid, &s_entry, len + MSG_QUEUE_HEADER_SIZE, 0 ) == -1 )
    {
        perror( "messageQueueEnter: Couldn't send message to app - msgsnd failed!" );
        pthread_mutex_unlock( &remove_queues_lock );
        return -1;
    }
#ifdef DEBUG_NET_MSGS
    fprintf( stderr, "DEBUG_NET_MSGS messageQueueEnter: data sent to app.\n" );
#endif

    pthread_mutex_unlock( &remove_queues_lock );

    return 0;
}
#endif
