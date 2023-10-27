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
                             rminternals.c
                             -------------------
    begin                : May 2001
    Authors              : Jorge Allyson Azevedo
                           Milena Scanferla
                           Daniel Sadoc
    email                : {allyson,milena,sadoc}@land.ufrj.br
 ***************************************************************************/

#ifndef RMINTERNALS_C
#define RMINTERNALS_C

#include "rminternals.h"
#include "rmcast.h"

#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include "rmwinmask.h"

extern pthread_t rcv_thread, signal_handler_thread;
extern char *EVENT_ACTION_NAME[6];
extern char *MESSAGE_TYPE_NAME[NUMBER_OF_MESSAGE_TYPES];
FILE        *logfile=NULL;

#ifdef DSUPPRESSION
    extern FILE *suppressionfile;
#endif

int STOP_TRANSMISSION=0;

EVENT_LIST *event_list=NULL;

#ifdef SINGLE_NACK

    EVENT_LIST *nak_event=NULL;
    EVENT_LIST *wait_for_retransm_event=NULL;

#endif

CACHE      *cache=NULL;
USER_INFO  local_user_info;
sigset_t   alrmset;

GLOBAL_OPTIONS rmcast_options =  {
    /*   int        *pipe; */
    0 ,
    /*    char       tcp_ip[IP_STRING_SIZE]; */
    { 0 },
    /*    int        tcp_port; */
    10013,
    /*    void      ( *shut_down_routine )( void ) = NULL; */
    0,
    /*    int        cur_state_server_is_up = 0; */
    0,
    /*    char       dest_ip[IP_STRING_SIZE];  */
    { "225.1.2.3" } ,
    /*    int        dest_port; */
    5151,
    /* int        ttl; */
    1,
    /* unsigned int        microsleep; */
    10,
    /* int     hosts_identified; default 0*/
    0,
    /* int     timer_distribution; */
    UNIFORM,
    /* int     max_nak; default 100 */
    100,
    /* int     max_member_cache_size; default 4000 */
    4000,
    /* int     version; */
    1,
    /* int        transmission_mode; */
    MULTICAST,
    /* char       log_file[255]; */
    "NULL",
    /* int        new_member_support; */
    0,
    /* int        statistics; */
    0,
    /* int        refresh_timer; */
    10,
    /* float      loss_prob; */
    0,
    /* int        leave_group_wait_time; */
    500000,
    /* int        rcv_buffer_size;         */
    10000,
    /* HOSTS_DELAYS hosts_delays */
    {
        { "DEFAULT", /* host IP */
          50000 /* one-way delay to host( microseconds ) */
        }
    },
    /* parameters to obtain the timer interval */
    2,
    2,
    5,
    2,
    2,
    2
};

extern pthread_mutex_t  change_local_user_sn;
extern pthread_mutex_t  event_list_mutex;
extern pthread_mutex_t  cache_mutex;

/*********************** Main routines ************************************************************/

void lock_eventlist_and_cache( )
{
    pthread_mutex_lock( &event_list_mutex );
    pthread_mutex_lock( &cache_mutex );
}

void unlock_eventlist_and_cache( )
{
    pthread_mutex_unlock( &cache_mutex );
    pthread_mutex_unlock( &event_list_mutex );
}


/*****************************************************************************************************
 *
 * int    rmcastCreateSocket ( int port )
 *
 * Creates a multi-cast socket, using the specified port.
 *
 * Returns the socket handler.
 *
 ******************************************************************************************************/
int rmcastCreateSocket( int port )
{

    int    aux = 1  ;
    struct sockaddr_in saddr;

    local_user_info.socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

    bzero(( char* ) &saddr, sizeof( saddr ) );

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons( port );
    saddr.sin_addr.s_addr = htonl( INADDR_ANY );

    if( setsockopt( local_user_info.socket, SOL_SOCKET, SO_REUSEADDR, ( char* )&aux, sizeof( aux ) ) == -1 )
        perror( "reuse addr error" );

    if( bind( local_user_info.socket, ( struct sockaddr * )&saddr, sizeof( saddr ) ) == -1 )
    {
    perror( "binding error" );
    close( local_user_info.socket );
    local_user_info.socket = -1;
    }

    if( rmcast_options.transmission_mode == MULTICAST )
    {
        rmcastSetTTLValue( local_user_info.socket, rmcast_options.ttl );
        rmcastSetLoopBack( local_user_info.socket, 1 );
    }

    return( local_user_info.socket );
}

void rmcastSetTTLValue( int soc, int ttl )
{
#ifndef SOLARIS
    if( setsockopt( soc, IPPROTO_IP, IP_MULTICAST_TTL, ( char* )&ttl, sizeof( ttl ) ) ==-1 )
    perror( "ttl" );
#endif
}

void rmcastSetLoopBack( int soc, int loop )
{
#ifndef SOLARIS
    if( setsockopt( soc, IPPROTO_IP, IP_MULTICAST_LOOP, ( char* )&loop, sizeof( loop ) ) ==-1 )
        perror( "loop" );
#endif
}

/*****************************************************************************************************
 *
 * void rmcastJoinGroup ( int soc, char *group_addr )
 *
 * Joins the user using the socket soc to the multicast group.
 *
 * Arguments:   the socket, in soc;
 *              the multicast group ip, in group_addr
 *
 ******************************************************************************************************/
void rmcastJoinGroup ( int soc, char *group_addr )
{
    struct sockaddr_in group;
    struct ip_mreq mreq;

    if( rmcast_options.transmission_mode == MULTICAST )
    {
        fprintf( stdout,"rmcastJoinGroup: Joining group %s\n", group_addr );

        if(( group.sin_addr.s_addr = inet_addr( group_addr ) ) == ( unsigned )-1 )
            perror( "rmcastJoingGroup: inet_addr" );

        mreq.imr_multiaddr      = group.sin_addr;
        mreq.imr_interface.s_addr = INADDR_ANY;

        if( setsockopt( soc, IPPROTO_IP, IP_ADD_MEMBERSHIP, ( char * )&mreq, sizeof( mreq ) ) == -1 )
            perror( "rmcastJoingGroup: ADD_MEMBERSHIP" );
    }
}

/*****************************************************************************************************
 *
 * void rmcastLeaveGroup ( int soc, char *group_addr )
 *
 * Used when the user using the socket soc wants to leave the multicast group.
 *
 * Arguments:   the socket, in soc;
 *              the multicast group ip, in group_addr
 * Returns: 1, on success;
 *          0, on error.
 *
 ******************************************************************************************************/
int rmcastLeaveGroup ( int soc, char *group_addr )
{
    BYTE *buffer;
    int buffsize = 0;

    PACKET_INFO pckt_info;

#ifdef REFRESH
/* Mounting and sending a Refresh packet before leaving the group but only
if we have already sent one data packet  */
    if( local_user_info.sn > 0 ){

        pckt_info.type = REFRESH_PACKET_TYPE;
        pckt_info.flags = 0;
        pckt_info.sender_id = local_user_info.member_id;
        pckt_info.packet_size = sizeof( REFRESH_PACKET );

        pthread_mutex_lock( &change_local_user_sn );
        pckt_info.packet_data.refresh_packet.sn_of_last_msg_sent = local_user_info.sn - 1;
        pthread_mutex_unlock( &change_local_user_sn );

    #ifdef DEBUG_EVENTS
        fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: Mounting refresh packet.\n" );
    #endif
        msgPcktMountMessage( &pckt_info, ( BYTE** )&buffer, &buffsize );
        if ( rmcastSendPacket( soc, buffer, buffsize ) == 0 )
            return 0;
    }
#endif /* REFRESH */

/* Mounting and sending a Leave group packet */
    pckt_info.type = LEAVE_GROUP_PACKET_TYPE;
    pckt_info.flags = 0;
    pckt_info.sender_id = local_user_info.member_id;
    pckt_info.packet_size = 0;

#ifdef DEBUG_NET_MSGS
    fprintf( stderr, "DEBUG_NET_MSGS rmcastLeaveGroup: mounting message\n" );
#endif

    msgPcktMountMessage( &pckt_info, ( BYTE** )&buffer, &buffsize );

#ifdef DEBUG_NET_MSGS
    fprintf( stderr, "DEBUG_NET_MSGS rmcastLeaveGroup: mounted 'leave group' message\n" );
#endif

    if ( rmcastSendPacket( soc, buffer, buffsize ) == 0 )
        return 0;

#ifdef DEBUG_NET_MSGS
    fprintf( stderr,  "DEBUG_NET_MSGS rmcastLeaveGroup: sent 'leave group' message\n" );
#endif

#ifdef DEBUG_NET_MSGS
    fprintf( stderr,  "DEBUG_NET_MSGS rmcastLeaveGroup: Disconnecting... %s\n", group_addr );
#endif

    lock_eventlist_and_cache( );
    eventListInsert( &event_list,NULL, NULL, LEV_GRP_WAIT, -1 );
    unlock_eventlist_and_cache( );

#ifdef DEBUG_NET_MSGS
    fprintf( stderr,  "DEBUG_NET_MSGS rmcastLeaveGroup: returning\n" );
#endif

    return 1;
}

/*****************************************************************************************************
 *
 * void showLogMsg( char type, PACKET_INFO *pckt_info )
 *
 * Prints a message from type 'type', related to the packet 'pckt_info', in the log file 'logfile'.
 *
 * Arguments:   the message type and the packet information.
 *
 * Side effects:  changes the log file.
 *
 ******************************************************************************************************/
void showLogMsg( char type, PACKET_INFO *pckt_info )
{
    struct timeval current_time;
    char buffer[200];
    struct tm time;
#ifdef DSHOW_COMPLETE_MASK_IN_LOG
    char auxbuffer[100];
    int n = 0;
#endif

    memset( buffer,0,sizeof( buffer ) );

    if( logfile!= NULL )
    {
        gettimeofday( &current_time, 0 );
        time = *gmtime(( const time_t* )&current_time );
        current_time. tv_sec =    (( time.tm_hour * 60 + time.tm_min ) * 60 ) + time.tm_sec;

        if( pckt_info !=NULL )
        {
            switch( pckt_info->type )
            {
/*
---------------------------------------------------------------------------------------------------------------------------
Time Snd/Rcv/Loss type sender_ip       sender_pid requested_ip    requested_pid  sn  [{base sn} {win size} {hmask} {lmask}]
---------------------------------------------------------------------------------------------------------------------------
01234567890   x   yz   abc.def.ghi.jkl 1234567890 abc.def.ghi.jkl 1234567890   123456   123456   1234       1234    1234
*/
                case DATA_PACKET_TYPE:

                     sprintf( buffer,"%15s %10d                              %6d",
                         pckt_info->sender_id.ip,pckt_info->sender_id.pid, pckt_info->packet_data.data_packet.sn );

                     break;

                case NAK_PACKET_TYPE:
#ifndef SINGLE_NACK
                     sprintf( buffer,"%15s %10d %15s %10d   %6d",
                             pckt_info->sender_id.ip,
                             pckt_info->sender_id.pid,
                             pckt_info->packet_data.nak_packet.requested_member_id.ip,
                             pckt_info->packet_data.nak_packet.requested_member_id.pid,
                             pckt_info->packet_data.nak_packet.sn
                             );
#else
                     sprintf( buffer,"%15s %10d %15s %10d            %6d   %4d       %4u    %4u   ",
                             pckt_info->sender_id.ip,
                             pckt_info->sender_id.pid,
                             pckt_info->packet_data.nak_packet.requested_member_id.ip,
                             pckt_info->packet_data.nak_packet.requested_member_id.pid,
                             pckt_info->packet_data.nak_packet.base_sn,
                             pckt_info->packet_data.nak_packet.window_size,
                             pckt_info->packet_data.nak_packet.hmask,
                             pckt_info->packet_data.nak_packet.lmask );
#ifdef DSHOW_COMPLETE_MASK_IN_LOG
                            for( n= ( pckt_info->packet_data.nak_packet.window_size / 2 )-1; n>=0; n-- )
                            {
                                sprintf( auxbuffer,"%d", (( unsigned int )pckt_info->packet_data.nak_packet.hmask >> n ) & 1 );
                                strcat( buffer,auxbuffer );
                            }

                            sprintf( auxbuffer,"-" );
                            strcat( buffer,auxbuffer );

                            for( n= ( pckt_info->packet_data.nak_packet.window_size / 2 )-1; n>=0; n-- )
                            {
                                sprintf( auxbuffer,"%d", (( unsigned int )pckt_info->packet_data.nak_packet.lmask >> n ) & 1 );
                                strcat( buffer,auxbuffer );
                            }
#endif /*DSHOW_COMPLETE_MASK_IN_LOG */

#endif
                     break;

                case RETRANSM_PACKET_TYPE:
                     sprintf( buffer,"%15s %10d %15s %10d   %6d",
                             pckt_info->sender_id.ip,
                             pckt_info->sender_id.pid,
                             pckt_info->packet_data.retransm_packet.original_sender_id.ip,
                             pckt_info->packet_data.retransm_packet.original_sender_id.pid,
                             pckt_info->packet_data.retransm_packet.data_packet.sn );
                     break;

#ifdef REFRESH
                case REFRESH_PACKET_TYPE:
                     sprintf( buffer,"%15s %10d                              %6d",
                             pckt_info->sender_id.ip,pckt_info->sender_id.pid,
                             pckt_info->packet_data.refresh_packet.sn_of_last_msg_sent );
                     break;
#endif
                case JOIN_REQUEST_PACKET_TYPE:
                     sprintf( buffer,"%15s %10d",pckt_info->sender_id.ip,pckt_info->sender_id.pid );
                     break;

                case JOIN_ACCEPT_PACKET_TYPE:
                     sprintf( buffer,"%15s %10d",pckt_info->sender_id.ip,pckt_info->sender_id.pid );
                     break;

                case LEAVE_GROUP_PACKET_TYPE:
                     sprintf( buffer,"%15s %10d",pckt_info->sender_id.ip,pckt_info->sender_id.pid );
                     break;

                default:
                     fprintf( stderr, "Unknown packet type: %c\n",pckt_info->type );
                     RM_leaveGroup( local_user_info.socket, local_user_info.member_id.ip );
            }

            fprintf( logfile,"%.0Lf   %c   %s   %s\n",
               (((( long double )current_time.tv_sec )*1000000+( long double )current_time.tv_usec ) ) ,
                type,
                MESSAGE_TYPE_NAME[( int )pckt_info->type],
                buffer );
        }
        else
        {
           fprintf( logfile,"%.0Lf     %c\n",
                   (((( long double )current_time.tv_sec )*1000000+( long double )current_time.tv_usec ) ) ,
                    type );
        }
    }
}

/*****************************************************************************************************
 *
 * void* rmcastReceivePackets( void *user_soc )
 *
 * Receives packets from socket soc.
 *
 * Arguments:   the socket number, in soc;
 *
 * Side effects:  may change the state of the event list or cache.
 *
 ******************************************************************************************************/
void *rmcastReceivePackets( void *user_soc )
{
    int    soc;
    struct sockaddr_in sender;
    static int first;
    static int first_pckts_lost;
    socklen_t len;
    int sn;
    int nbytes; /* number of bytes received */
    char message[rmcast_options.rcv_buffer_size];
    PACKET_INFO pckt_info;
    MEMBER_ID aux_member_id;
#ifdef DPROCESSING_TIME
    struct timeval time_before,time_after;
    struct tm time;
    FILE *proc_log;
    char tipo;
#endif

    soc   = ( int )user_soc;
    sn    = 0;
    first = 1;
    first_pckts_lost = 10;

#ifndef SOLARIS
    sigset_t alrmset;
    sigfillset( &alrmset );            /* fills the alarm set with all the signals */

    /* blocks all the signals, because they won´t be handled by this thread */
    pthread_sigmask( SIG_BLOCK, &alrmset, NULL );
#endif
#ifdef DPROCESSING_TIME
    proc_log=fopen( "proc_times.log","w" );
#endif
    do{
        bzero( message, rmcast_options.rcv_buffer_size );

    #ifdef DEBUG_NET_MSGS
        fprintf( stderr, "DEBUG_NET_MSGS rmcastReceivePackets: Waiting to receive a message...\n" );
    #endif
        len    = sizeof( struct sockaddr_in );
        nbytes = ( int )recvfrom( soc, message, rmcast_options.rcv_buffer_size, 0, ( struct sockaddr * ) &sender, &len );

    #ifdef DEBUG_NET_MSGS
        fprintf( stderr, "DEBUG_NET_MSGS rmcastReceivePackets: A message was received from the network!( message size=%d )\n",nbytes );
    #endif

        if( nbytes <= 0 )
        {
            fprintf( stderr, "rmcastReceivePackets: Error while receiving a message from the net - nbytes=%d \n",nbytes );
        }
        else /* nbytes > 0 */
        {
#ifdef DEBUG_PCKT
            {
                int cont;
                fprintf( stderr, "DEBUG_PCKT rmcastReceivePackets: Received message:\n" );
                for( cont = 0; cont < nbytes; cont++ )
                {
                    fprintf( stderr, "%d", ( ( signed char * )message )[cont] );
                }
                fprintf( stderr, "\n" );
            }
#endif
            msgPcktUnmountMessage( &pckt_info, ( BYTE* )message );

#ifdef DPROCESSING_TIME
        gettimeofday( &time_before, 0 );
        time = *gmtime(( const time_t* )&time_before );
        time_before.tv_sec = (( time.tm_hour * 60 + time.tm_min ) * 60 ) + time.tm_sec;
        tipo = pckt_info.type;

#endif

            if( memberIsEqual( &( pckt_info.sender_id ), &( local_user_info.member_id ) ) )
            {
                /* We have received our own packet, so we just log and go to the next packet */
                showLogMsg( RECEIVE,&pckt_info );
                continue;
            }


#ifdef DEBUG_NET_MSGS
            fprintf( stderr, "DEBUG_NET_MSGS rmcastReceivePackets: message type=%s\n",MESSAGE_TYPE_NAME[( int )( pckt_info.type )] );
#endif

            switch( pckt_info.type )
            {
                case DATA_PACKET_TYPE:
                     sn = pckt_info.packet_data.data_packet.sn; break;
                case NAK_PACKET_TYPE:
                     sn = pckt_info.packet_data.nak_packet.sn;
                     #ifdef DSINGLE_NACK

                        fprintf( stderr, "NAK received:\n\tFrom:%s:%d \n\tRequesting: %s:%d:%d \n\twin_size:%d \tbase_sn:%d \thmask:%u \tlmask:%u \n",
                                         pckt_info.sender_id.ip,
                                         pckt_info.sender_id.pid,
                                         pckt_info.packet_data.nak_packet.requested_member_id.ip,
                                         pckt_info.packet_data.nak_packet.requested_member_id.pid,
                                         pckt_info.packet_data.nak_packet.sn,
                                         pckt_info.packet_data.nak_packet.window_size,
                                         pckt_info.packet_data.nak_packet.base_sn,
                                        ( unsigned int )pckt_info.packet_data.nak_packet.hmask,
                                        ( unsigned int )pckt_info.packet_data.nak_packet.lmask );
                        {

                            int n;

                            fprintf( stderr, "\tHMASK:" );
                            for( n = ( pckt_info.packet_data.nak_packet.window_size / 2 )-1; n>=0; n-- )
                            {
                                fprintf( stderr, "%d", (( unsigned int )pckt_info.packet_data.nak_packet.hmask >> n ) & 1 );
                            }

                            fprintf( stderr, "\n\tLMASK:" );

                            for( n = ( pckt_info.packet_data.nak_packet.window_size / 2 )-1; n>=0; n-- )
                            {
                                fprintf( stderr, "%d", (( unsigned int )pckt_info.packet_data.nak_packet.lmask >> n ) & 1 );
                            }

                            fprintf( stderr, "\n" );
                        }


                     #endif
                     break;
                case RETRANSM_PACKET_TYPE:
                     sn = pckt_info.packet_data.retransm_packet.data_packet.sn; break;
                case REFRESH_PACKET_TYPE:
                     sn = pckt_info.packet_data.refresh_packet.sn_of_last_msg_sent; break;
                case JOIN_REQUEST_PACKET_TYPE:
                     sn = -1; break;
                case JOIN_ACCEPT_PACKET_TYPE:
                     sn = -1; break;
                case LEAVE_GROUP_PACKET_TYPE:
                     sn = -1; break;
                default:
                     fprintf( stderr, "rmcastReceivePackets: Unknown packet type.\n" );
                     RM_leaveGroup( local_user_info.socket, local_user_info.member_id.ip );
            }
#ifdef DEBUG_NET_MSGS
            fprintf( stderr, "DEBUG_NET_MSGS rmcastReceivePackets: sn=%d\n",sn );
#endif

#ifdef DEBUG_SHOW
            msgPcktShowMessage(( BYTE * )message );
#endif

/* ------------------------ Loss Simulation begin----------------------------------------- */
            if( rmcast_options.loss_prob > 0 )
            {
                /* Uncomment the following line if don't want naks and retransmission to be lost

                if( pckt_info.type != NAK_PACKET_TYPE && pckt_info.type != RETRANSM_PACKET_TYPE ) */
                {
                    if( generateSample( UNIFORM ) < rmcast_options.loss_prob ||
                        (( pckt_info.type == DATA_PACKET_TYPE ) &&( sn == 0 ) &&( first == 1 ) ) ||
                        ( first_pckts_lost > 0 )
                      )
                        /*
                         *  We are always losing the first 'first_pckts_lost'( e.g., 10 ) received packets,
                         *  and the first received data packet -
                         *  that is, the first burst of received packets.
                         *  After that, packets are lost according to the rmcast_options.loss_prob.
                         */
                    {
    #ifdef DEBUG_NET_MSGS
                        fprintf( stderr, "\nDEBUG_NET_MSGS rmcastReceivePackets: PACKET LOST sn=%d\n\n",sn );
    #endif
                        if( logfile != NULL )
                        {
                            showLogMsg( RECEIVE_AND_LOSE,&pckt_info );
                        }

                        if( pckt_info.type == DATA_PACKET_TYPE )
                            first = 0;

                        if( first_pckts_lost>0 )
                            first_pckts_lost--;

                        continue;
                    }
                }
            }
/* ------------------------ Loss Simulation end----------------------------------------- */
            lock_eventlist_and_cache( );
#ifdef DMUTEX
            fprintf( stderr, "DMUTEX rmcastReceivePackets: event list and cache LOCKED\n" );
            showLogMsg( 'K', NULL );
#endif
            if( logfile != NULL )
            {
                showLogMsg( RECEIVE,&pckt_info );
            }

            switch( pckt_info.type )
            {
                case NAK_PACKET_TYPE:
#ifdef SINGLE_NACK
                    {
                        int i,aux_window_mask[MAX_WINDOW_SIZE];

                        memset( aux_window_mask, 0, ( MAX_WINDOW_SIZE*( sizeof( int ) ) ) );
                        window_mask_numbers2array( aux_window_mask, pckt_info.packet_data.nak_packet.hmask, pckt_info.packet_data.nak_packet.lmask );
    #ifdef DEBUG_NET_MSGS
                        fprintf( stderr,  "DEBUG_NET_MSGS rmcastReceivePackets: a NAK was received!\n" );
    #endif
                        if( !memberIsEqual( &( pckt_info.packet_data.nak_packet.requested_member_id ),&( local_user_info.member_id ) ) )
                        {
                            for( i = 0; i < MAX_WINDOW_SIZE; i++ )
                            {
                                if( aux_window_mask[i] == 1 )
                                {
                                    /* Setting the real value to the sn */

                                    pckt_info.packet_data.nak_packet.sn = pckt_info.packet_data.nak_packet.base_sn + i + 1;
                                    #ifdef DNAK_RCV
                                        fprintf( stderr,  "DNAK_RCV we will try to remove event NAK_SND_WAIT for sn=%d\n",pckt_info.packet_data.nak_packet.sn );
                                    #endif

                                    if( rmcastRemoveEvent( &pckt_info,NAK_SND_WAIT ) )
                                    {
                    #ifdef DEBUG_NET_MSGS
                                        fprintf( stderr,  "DEBUG_NET_MSGS rmcastReceivePackets: a NAK was received. inserting a RET_RCV_WAIT or a SUPPRESSED_NAK.\n" );
                    #endif

                    #ifdef DNAK_RCV
                                        fprintf( stderr,  "Inserting a SUPPRESSED_NAK\n" );
                    #endif
                                        rmcastInsertEvent( &pckt_info, SUPPRESSED_NAK );
                                    }
                                }
                            }
                        }
                        for( i = 0; i < MAX_WINDOW_SIZE; i++ )
                        {
                            if( aux_window_mask[i] == 1 )
                            {
                                 /* Setting the real value to the sn */
                                pckt_info.packet_data.nak_packet.sn = pckt_info.packet_data.nak_packet.base_sn + i + 1;

                                #ifdef DNAK_RCV
                                    fprintf( stderr, "DNAK_RCV rmcastReceivePackets: Before calling rmcastCacheContains sn=%d\n",pckt_info.packet_data.nak_packet.sn );
                                #endif

                                if( rmcastCacheContains( &pckt_info ) &&( rmcastFindEvent( &pckt_info,RET_SND_WAIT ) == 0 ) )
                                {
            #ifdef DEBUG_NET_MSGS
                                    fprintf( stderr,  "DEBUG_NET_MSGS rmcastReceivePackets: a NAK was received. inserting a RET_SND_WAIT.\n" );
            #endif

            #ifdef DNAK_RCV
                                    fprintf( stderr,  "Inserting a RET_SND_WAIT: %s:%d:%d\n",
                                                     pckt_info.packet_data.nak_packet.requested_member_id.ip,
                                                     pckt_info.packet_data.nak_packet.requested_member_id.pid,
                                                     pckt_info.packet_data.nak_packet.sn );
            #endif
                                    rmcastInsertEvent( &pckt_info,RET_SND_WAIT );
                                }
                            }
                        }
                    }
#else /* using multiple nacks */
    #ifdef DEBUG_NET_MSGS
                    fprintf( stderr,  "DEBUG_NET_MSGS rmcastReceivePackets: a NAK was received!\n" );
    #endif

                    if( !memberIsEqual( &( pckt_info.packet_data.nak_packet.requested_member_id ),&( local_user_info.member_id ) )
                         && rmcastRemoveEvent( &pckt_info,NAK_SND_WAIT ) )
                    {
    #ifdef DEBUG_NET_MSGS
                        fprintf( stderr,  "DEBUG_NET_MSGS rmcastReceivePackets: a NAK was received. inserting a RET_RCV_WAIT.\n" );
    #endif
                        rmcastInsertEvent( &pckt_info,RET_RCV_WAIT );
                    }
                    else if( !rmcastFindEvent( &pckt_info,RET_SND_WAIT ) )
                    {
                        if( rmcastCacheContains( &pckt_info ) )
                        {
    #ifdef DEBUG_NET_MSGS
                            fprintf( stderr,  "DEBUG_NET_MSGS rmcastReceivePackets: a NAK was received. inserting a RET_SND_WAIT.\n" );
    #endif
                            rmcastInsertEvent( &pckt_info,RET_SND_WAIT );
                        }
                    }
#endif /* SINGLE_NACK */
                    break;

                case LEAVE_GROUP_PACKET_TYPE:
                     /*
                      * FIXME
                      * By now it is OK if the member does not exist in cache,
                      * because he is leaving the group anyway :- ) So we don't
                      * care about the return value for cacheDeactivateMember
                      */
                    cacheDeactivateMember( cache,&( pckt_info.sender_id ) );
                    break;

                case JOIN_REQUEST_PACKET_TYPE:
                    {
                        char *buffer;
                        int buffsize;

                        PACKET_INFO pckt_info;

                        if( rmcast_options.cur_state_server_is_up )
                        {
                            pckt_info.type = JOIN_ACCEPT_PACKET_TYPE;
                            pckt_info.flags = 0;
                            pckt_info.sender_id = local_user_info.member_id;
                            pckt_info.packet_size = sizeof( int );
                            pckt_info.packet_data.join_accept_packet.port = rmcast_options.tcp_port;
        #ifdef DEBUG_NET_MSGS
                            fprintf( stderr, "DEBUG_NET_MSGS rmcastReceivePackets: mounting the 'join accept packet'( port: %d )...", rmcast_options.tcp_port );
        #endif
                            msgPcktMountMessage( &pckt_info, ( BYTE** )&buffer, &buffsize );
        #ifdef DEBUG_SHOW
                            fprintf( stderr, "DEBUG_SHOW rmcastReceivePackets: ok!\n" );
                            msgPcktShowMessage(( BYTE* )buffer );
        #endif
        #ifdef DEBUG_NET_MSGS
                            fprintf( stderr, "DEBUG_NET_MSGS rmcastReceivePackets: now, sending the message to the mcast group\n" );
        #endif
                            rmcastSendPacket( local_user_info.socket, ( BYTE* )buffer, buffsize );
                        }
                    }
                    break;

                case JOIN_ACCEPT_PACKET_TYPE:
                     /*
                      * Ignore this packet. He is handled only when we are entering the group.
                      * See RM_getCurState at rmcast.c
                      */
                     break;

                case REFRESH_PACKET_TYPE:
#ifdef REFRESH
                     rmcastProcessRefreshPacket( pckt_info );
#endif
                     break;

                case RETRANSM_PACKET_TYPE:

#ifdef DSUPPRESSION
                     if( rmcastRemoveEvent( &pckt_info,RET_SND_WAIT ) ){
                         fprintf( suppressionfile,"RT %s %d %s %d %d\n",
                                 pckt_info.sender_id.ip,
                                 pckt_info.sender_id.pid,
                                 pckt_info.packet_data.retransm_packet.original_sender_id.ip,
                                 pckt_info.packet_data.retransm_packet.original_sender_id.pid,
                                 pckt_info.packet_data.retransm_packet.data_packet.sn );
                     }
#else
                     rmcastRemoveEvent( &pckt_info,RET_SND_WAIT );
#endif
                    if( !rmcastRemoveEvent( &pckt_info,NAK_SND_WAIT ) &&
                        !rmcastRemoveEvent( &pckt_info,RET_RCV_WAIT ) )
                    {
                        if( rmcastCacheContains( &pckt_info ) )
                            break;
                    }

                    /* Now we will get the content of the retransmition and put it in the cache. */
                    strcpy( aux_member_id.ip, pckt_info.packet_data.retransm_packet.original_sender_id.ip );
                    aux_member_id.pid=pckt_info.packet_data.retransm_packet.original_sender_id.pid;
                    pckt_info.packet_data.data_packet = pckt_info.packet_data.retransm_packet.data_packet;
                    pckt_info.sender_id = aux_member_id;

                    /* continue... */
                case DATA_PACKET_TYPE:
                    rmcastProcessDataPacket( pckt_info ); /* inserts data in cache... */

#ifdef DSINGLE_NACK
                    fprintf( stderr, "DSINGLE_NACK rmcastReceivePackets: after executing rmcastProcessDataPacket.\n" );
                    eventListShow( event_list );
#endif
                    break;

                default:
                    fprintf( stderr, "rmcastReceivePackets: Invalid message type.( %d )\n",pckt_info.type );
                    RM_leaveGroup( local_user_info.socket, local_user_info.member_id.ip );

            } /* End switch */
            unlock_eventlist_and_cache( );
#ifdef DMUTEX
            showLogMsg( 'U', NULL );
            fprintf( stderr, "DMUTEX rmcastReceivePackets: event list and cache UNLOCKED\n" );
#endif
        } /* nbytes > 0 */

#ifdef DEBUG_NET_MSGS
    fprintf( stderr, "DEBUG_NET_MSGS rmcastReceivePackets: received, ok!\n" );
#endif
#ifdef DPROCESSING_TIME
    gettimeofday( &time_after, 0 );
    time = *gmtime(( const time_t* )&time_after );
    time_after.tv_sec = (( time.tm_hour * 60 + time.tm_min ) * 60 ) + time.tm_sec;

    time_after.tv_sec = time_after.tv_sec - time_before.tv_sec;
    time_after.tv_usec = time_after.tv_usec - time_before.tv_usec;

    fprintf( proc_log,"%d %.0Lf\n",tipo, (( long double )time_after.tv_sec*1000000+( long double )time_after.tv_usec ) );
#endif

    } FOREVER;

    return NULL;
}

/*****************************************************************************************************
 *
 * void  rmcastSendPacket( int soc, BYTE *message, int message_size )
 *
 * Sends a message packet of size message_size to socket soc.
 *
 * Returns: 1, on success;
 *          0, on error.
 *
 ******************************************************************************************************/
int rmcastSendPacket( int soc, BYTE *message, int message_size )
{
    PACKET_INFO pckt_info;
    struct sockaddr_in dest;
    int sn;

#ifdef DEBUG_SHOW2
    int i;
#endif

    int retval;
    char type = *(( char* )message );
    char local_message[ message_size + 1 ];
    struct timespec sleep_time;

#ifdef DEBUG_SHOW2
    fprintf( stderr, "DEBUG_SHOW2 - rmcastSendPacket: will copy a message of size %d to local var.\n",message_size );
    for( i=0; i< message_size; i++ )
    {
        fprintf( stderr, "( %d )%d",i, ( unsigned int )message[i] );
    }
    fprintf( stderr, "\n" );
#endif

    memcpy( local_message, message, message_size );
    dest.sin_family      = AF_INET;
    dest.sin_port        = htons   ( local_user_info.group_id.port );
    dest.sin_addr.s_addr = inet_addr( local_user_info.group_id.ip );

    if( type == DATA_PACKET_TYPE )
    {
          /* REFRESH event is now inserted only after user sends the first packet, i.e., local_user_info.sn==0 */
        if( local_user_info.sn == 0 )
        {
            eventListInsert( &event_list, &( local_user_info.member_id ), NULL, REF_SND_WAIT, -1 );
        }
        pthread_mutex_lock( &change_local_user_sn );
        local_user_info.sn++;
        pthread_mutex_unlock( &change_local_user_sn );
    }

    msgPcktUnmountMessage( &pckt_info, ( BYTE * )local_message );

    switch( pckt_info.type )
    {
        case DATA_PACKET_TYPE:
            sn = pckt_info.packet_data.data_packet.sn; break;
        case NAK_PACKET_TYPE:
            sn = pckt_info.packet_data.nak_packet.sn; break;
        case RETRANSM_PACKET_TYPE:
            sn = pckt_info.packet_data.retransm_packet.data_packet.sn; break;
        case REFRESH_PACKET_TYPE:
            sn = pckt_info.packet_data.refresh_packet.sn_of_last_msg_sent; break;
        case JOIN_REQUEST_PACKET_TYPE:
            sn = -1; break;
        case JOIN_ACCEPT_PACKET_TYPE:
            sn = -1; break;
        default:
            sn = -1;
    }
#ifdef DEBUG_NET_MSGS
    fprintf( stderr, "DEBUG_NET_MSGS rmcastSendPacket: will send a %s - sn: %d\n", MESSAGE_TYPE_NAME[( int )type],sn );
#endif

    if( logfile != NULL )
    {
        showLogMsg( SEND,&pckt_info );
    }

    retval = sendto( soc, local_message, message_size, 0, ( struct sockaddr * )&dest, sizeof( dest ) );

    if( retval < 0 )
    {
        fprintf( stderr, "ERROR - rmcastSendPacket: retval < 0\n" );
        perror( "send_message" );

        msgPcktShowMessage( message );

        return 0;
    }
    /* fprintf( stderr, " Packet size RML: %d\n",message_size ); FIXME teste pra saber o tamanho dos pacotes */

    if( ( rmcast_options.microsleep > 0 ) &&( rmcast_options.microsleep < 65536 ) )
    {
        /* Converting rmcast_options.microsleep to nanoseconds as needed by nanosleep function */
        sleep_time.tv_nsec= ( rmcast_options.microsleep*1000 );
        sleep_time.tv_sec=0;
        nanosleep( &sleep_time,NULL );
        /* usleep( rmcast_options.microsleep ); */
    }
    return 1;
}

/*****************************************************************************************************
 *
 *  void *rmcastHandleSignals( void *arg )
 *
 *  This thread will be the only one that threats the alarm events.
 *
 *  Communication with it should be done using SIGUSR1.
 *
 *****************************************************************************************************/
extern const char * const sys_siglist[];
#define _GNU_SOURCE
#include <string.h>
char *strsignal( int sig );
extern const char * const sys_siglist[];

void *rmcastHandleSignals( void *arg )
{
    int sig;
    int err;
    int finish = 0;

#ifndef SOLARIS
    struct itimerval value;
#else
     sigset_t     alrmset;
     sigfillset( &alrmset );
     pthread_sigmask( SIG_BLOCK, &alrmset, NULL );
#endif

    while( 1 )
    {
#ifdef DEBUG_EVENTS
        fprintf( stderr, "DEBUG_EVENTS rmcastHandleSignals: Before sigwait( )\n" );
#endif
        err = sigwait( &alrmset, &sig );
#ifdef DEBUG_EVENTS
        fprintf( stderr,  "DEBUG_EVENTS rmcastHandleSignals: After sigwait( ) \n" );
#endif
#ifdef DEBUG_EVENTS
#ifdef SOLARIS
        fprintf( stderr, "DEBUG_EVENTS rmcastHandleSignals( SOLARIS ): signal   ( %d ) received\n", sig );
#else
        fprintf( stderr, "DEBUG_EVENTS rmcastHandleSignals: signal %s( %d ) received\n",sys_siglist[sig], sig );
#endif
#endif
        if( err || ( sig != SIGALRM && sig != SIGUSR1 ) )
        {
            fprintf( stderr, "rmcastHandleSignals: [%s signal catched]\n",strsignal( sig ) );
            if( sig == SIGINT || sig == SIGTERM )
            {
                fprintf( stderr, "rmcastHandleSignals: Signal %s received. Exiting.\n",strsignal( sig ) );
                RM_leaveGroup( local_user_info.socket, local_user_info.member_id.ip );
            }
            else if( sig == SIGTSTP )
            {
                fprintf( stderr, "DEBUG_EVENTS rmcastHandleSignals: SIGSTP received. Nothing to be done.\n" );
                /* FIXME( in a future release, we may be able to handle the SIGTSTP signal ) */
            }
            else if( sig == SIGUSR2 )
            {
                STOP_TRANSMISSION = 1;
            }
            else continue;
        }
        lock_eventlist_and_cache( );

#ifdef DMUTEX
        showLogMsg( 'k', NULL );
        fprintf( stderr, "DMUTEX rmcastHandleSignals: event list and cache LOCKED\n" );
#endif
        if( sig == SIGALRM ||( event_list != NULL && event_list->timer_value == 0 ) )
        {
#ifdef DSINGLE_NACK
            fprintf( stderr, "DSINGLE_NACK rmcastHandleSignals: calling rmcastCatchEvents\n" );
#endif
            finish = rmcastCatchEvents( 0 );

#ifdef DSINGLE_NACK
            fprintf( stderr, "DSINGLE_NACK rmcastHandleSignals: back from rmcastCatchEvents\n" );
#endif
            if( finish )
                break;
        }
#ifdef DSINGLE_NACK
        fprintf( stderr, "DSINGLE_NACK rmcastHandleSignals: showing eventlist:\n" );
        eventListShow( event_list );
#endif

        if( event_list != NULL )
        {
#ifdef DEBUG_EVENTS
           fprintf( stderr, "DEBUG_EVENTS rmcastHandleSignals: setting timer to %ld( rmcastHandleSignals )\n",event_list->timer_value );
#endif

#ifndef SOLARIS
           setTimerValue( &value,event_list->timer_value );
           setitimer( ITIMER_REAL, &value,NULL );
#else
           alarm( event_list->timer_value/1000000 + 1 );
#endif
        }
        else
        {
#ifdef DEBUG_EVENTS
           fprintf( stderr, "DEBUG_EVENTS rmcastHandleSignals: resetting timer( rmcastHandleSignals )\n" );
#endif

#ifndef SOLARIS
           setTimerValue( &value,0 );
           setitimer( ITIMER_REAL,&value,NULL );
#else
           alarm( 0 );
#endif
        }
        unlock_eventlist_and_cache( );
#ifdef DMUTEX
        showLogMsg( 'u', NULL );
        fprintf( stderr, "DMUTEX rmcastHandleSignals: cache and event list UNLOCKED\n" );
#endif
    }
    return  0;
}

/*****************************************************************************************************
 *
 * int  rmcastCatchEvents  ( int i )
 *
 * Catch and execute event list events.
 *
 ******************************************************************************************************/
int rmcastCatchEvents( int i )
{
    int retval = 0;
    int function_retval = 0;
    struct sockaddr_in group;
    struct ip_mreq mreq;
    CACHE *cache_sender_entry;
#ifndef SINGLE_NACK
    int update_nak_list_retval;
#endif
    EVENT_LIST first;
    int msgsize;
    BYTE *message;
    PACKET_INFO pack;
    DATA_PACKET *data;

#ifdef DEBUG_EVENTS
    fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: Event catched!\n" );
#endif
    if( event_list == NULL )
    {
        #ifdef DEBUG_EVENTS
        fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: Internal warning! first == NULL\n" );
        #endif
        return retval;
    }
    first = *event_list;

#ifdef DSINGLE_NACK
    fprintf( stderr, "DSINGLE_NACK rmcastCatchEvents: showing event list:\n" );
#endif
    eventListShow( event_list );

#ifdef DEBUG_EVENTS
    fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: event list: %p\n",&( *event_list ) );
#endif

    do
    {
#ifdef DEBUG_EVENTS
    fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: Removing event: action %s timer value: %ld\n",
        EVENT_ACTION_NAME[( int )first.action],
        first.timer_value );
#endif
#ifdef DNAK_RCV
    fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: Removing event: action %s timer value: %ld\n",
        EVENT_ACTION_NAME[( int )first.action],
        first.timer_value );
#endif
        if( eventListRemoveFirst( &event_list ) == -1 )
        {
#ifdef DEBUG_EVENTS
            fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: warning eventListRemoveFirst( &event_list ) == -1\n" );
#endif
        }
        eventListShow( event_list );

    switch( first.action )
    {
        case NAK_SND_WAIT:
             pack.type=NAK_PACKET_TYPE;
             pack.flags=0;
             pack.sender_id = local_user_info.member_id;
             pack.packet_size=sizeof( NAK_PACKET );
             strcpy( pack.packet_data.nak_packet.requested_member_id.ip,first.member_id->ip );
             pack.packet_data.nak_packet.requested_member_id.pid=first.member_id->pid;
             pack.packet_data.nak_packet.sn=first.sn;
#ifdef SINGLE_NACK
            {
                 CACHE *aux_cache_member = NULL;
                 aux_cache_member = cacheLookForMember( &cache, first.member_id );

                 if( aux_cache_member!=NULL )
                 {
                     pack.packet_data.nak_packet.base_sn=aux_cache_member->sm_info.member_status.last_seq_rcv;
                     pack.packet_data.nak_packet.window_size=aux_cache_member->sm_info.member_status.window_size;

                     window_mask_array2numbers( aux_cache_member->sm_info.member_status.window_mask,
                     &pack.packet_data.nak_packet.hmask, &pack.packet_data.nak_packet.lmask, aux_cache_member->sm_info.member_status.window_ini );
                 }
             }
#endif
             msgPcktMountMessage( &pack, &message, &msgsize );
#ifdef DEBUG_EVENTS
             fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: will send NAK ...\n" );
#endif
             rmcastSendPacket( local_user_info.socket, message, msgsize );

#ifdef DEBUG_EVENTS
             fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: NAK sent OK.\n" );
#endif
             cache_sender_entry = cacheLookForMember( &cache,first.member_id );

             if( cache_sender_entry != NULL )
                 retval = eventListInsert( &event_list,&( cache_sender_entry->sm_info.member_id ), cache_sender_entry->sm_info.member_id.ip, RET_RCV_WAIT, first.sn );

#ifdef DEBUG_EVENTS
             fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: RET_RCV_WAIT was inserted in the event list.\n" );
#endif
             break;

        case RET_RCV_WAIT:

             /* Next we will check if it is possible to send one more NAK for the lost message,
             i.e., the NAK should be sent only rmcast_options.max_nak times */
#ifdef DEBUG_EVENTS
             fprintf( stderr, " \nDEBUG_EVENTS - rmcastCatchEvents: Waiting for retransmission timed out\n ip: %s - pid: %d - sn: %d\n",
             first.member_id->ip, first.member_id->pid, first.sn );
#endif
#ifndef SINGLE_NACK
             /* When we are using SINGLE_NACK this check is done in the event list insert funciton */
             if(( update_nak_list_retval = cacheUpdateNakList( &cache, first.member_id, first.sn ) ) > 0 )
             {
#endif
                 cache_sender_entry = cacheLookForMember( &cache,first.member_id );

                 if( cache_sender_entry != NULL )
#ifdef SINGLE_NACK
                     retval = eventListInsert( &event_list,&( cache_sender_entry->sm_info.member_id ), cache_sender_entry->sm_info.member_id.ip, RET_RCV_EXPIRED, first.sn );
#else
                     retval = eventListInsert( &event_list,&( cache_sender_entry->sm_info.member_id ), cache_sender_entry->sm_info.member_id.ip, NAK_SND_WAIT, first.sn );
#endif /* SINGLE_NACK */

#ifdef DEBUG_EVENTS
                 fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: NAK_SND_WAIT was inserted in the event list.\n" );
#endif

#ifndef SINGLE_NACK
             }else if( update_nak_list_retval == 0 )
                 {
                     /*
                      * We have sent the max number of NAK( see rmcast_options structure ) we didn't recover the packet
                      * so we must exit because we cannot keep the reliability
                      */
                     fprintf( stderr, "********\n" );
                     fprintf( stderr, "rmcastCatchEvents ERROR: Recovering packet failed. Max number( %d ) of NAKs reached!\n",rmcast_options.max_nak );
                     fprintf( stderr, "\tHost IP:PID=%s:%d\tsn=%d\n",first.member_id->ip,first.member_id->pid,first.sn );
                     fprintf( stderr, "********\n" );

                     RM_leaveGroup( local_user_info.socket, local_user_info.member_id.ip );
             }
#endif
        break;

        case RET_SND_WAIT:

             if(( data=cacheLookForMessage( &cache, first.member_id, first.sn ) )!=NULL )
             {
                 pack.type = RETRANSM_PACKET_TYPE;
                 pack.flags = 0;
                 pack.sender_id = local_user_info.member_id;
                 pack.packet_size = sizeof( RETRANSM_PACKET );
                 pack.packet_data.retransm_packet.original_sender_id = *first.member_id;
                 pack.packet_data.retransm_packet.data_packet = *data;
                 msgPcktMountMessage( &pack, &message, &msgsize );

         #ifdef DEBUG_EVENTS
                 fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: will send a RET... " );
         #endif
                 rmcastSendPacket( local_user_info.socket, message, msgsize );
         #ifdef DEBUG_EVENTS
                 fprintf( stderr, "ok( rmcastCatchEvents )\n" );
         #endif
              }
#ifdef DEBUG_EVENTS
                 fprintf( stderr, " \nDEBUG_EVENTS - rmcastCatchEvents: Message retransmited\n ip: %s - pid: %d - sn: %d\n",
                         first.member_id->ip, first.member_id->pid, first.sn );
#endif
        break;

#ifdef REFRESH
        case REF_SND_WAIT:
#ifdef DEBUG_EVENTS
             fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: Processing the refresh event.\n" );
#endif
             pack.type = REFRESH_PACKET_TYPE;
             pack.flags = 0;
             pack.sender_id = local_user_info.member_id;
             pack.packet_size = sizeof( REFRESH_PACKET );

             pthread_mutex_lock( &change_local_user_sn );
             pack.packet_data.refresh_packet.sn_of_last_msg_sent = local_user_info.sn - 1;
             pthread_mutex_unlock( &change_local_user_sn );

#ifdef DEBUG_EVENTS
             fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: Mounting refresh packet.\n" );
#endif
             msgPcktMountMessage( &pack, &message, &msgsize );
             if( rmcastSendPacket( local_user_info.socket, message, msgsize ) == 1 )
                 eventListInsert( &event_list, NULL, NULL, REF_SND_WAIT, -1 );
                 /* We are not supposed to set a valid sn in the sn field
                    when we are inserting a REF_SND_WAIT in the event list,
                    so we use a -1 value to the sn in this eventListInsert
                    function call*/

#ifdef DEBUG_EVENTS
             fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: refresh packet mounted.\n" );
#endif
#ifdef DEBUG_EVENTS
             fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: Refresh event processed.\n" );
#endif
             break;

#endif

        case LEV_GRP_WAIT:

#ifdef DEBUG_EVENTS
             fprintf( stderr, "DEBUG_EVENTS - rmcastCatchEvents: Processing 'leave group'  event.\n" );
#endif
             if(( group.sin_addr.s_addr = inet_addr( local_user_info.group_id.ip ) ) == ( unsigned )-1 )
                 perror( "rmcastLeaveGroup: inet_addr" );

             mreq.imr_multiaddr      = group.sin_addr;
             mreq.imr_interface.s_addr = INADDR_ANY;

             if( rmcast_options.transmission_mode == MULTICAST )
             {
                 if( setsockopt( local_user_info.socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, ( char * )&mreq, sizeof( mreq ) ) == -1 )
                     perror( "rmcastLeaveGroup: DROP_MEMBERSHIP" );
             }

#ifdef DEBUG_NET_MSGS
             fprintf( stderr,  "DEBUG_NET_MSGS rmcastLeaveGroup: disconnected\n" );
#endif
             remove_queues( 0 );

             if( rmcast_options.shut_down_routine!=NULL )
                 rmcast_options.shut_down_routine( );

             function_retval = 1;
             RM_terminate();

             break;

        default:
             fprintf( stderr, "rmcastCatchEvents Error: Action Undefined %d\n",first.action );
    }

        if( event_list == NULL )
            break;

        first = *event_list;

    } while( first.timer_value == 0 && function_retval != 1 );

    eventListShow( event_list );

    return function_retval;
}
/*********************** End of the Main routines *************************************************/

/**************** Routines to facilitate the interface with the event list and the cache **********/

/*****************************************************************************************************
 *
 * void rmcastProcessDataPacket( PACKET_INFO pckt_info )
 *
 * Processes the data packet.
 *
 * Arguments: the packet to be processed, in pckt_info;
 *
 * Side effects: may affect the event list, the cache and the message queue.
 *
 ******************************************************************************************************/
void rmcastProcessDataPacket( PACKET_INFO pckt_info )
{
    int i, retval;

    MEMBER_ID *aux_member_id = &( pckt_info.sender_id );
    CACHE_NODE *aux_pckt_info;
    CACHE *p_old_cache_sender_entry = NULL, *p_new_cache_sender_entry = NULL;
#ifdef SINGLE_NACK
    CACHE *cache_aux1_entry = NULL;
#endif
    CACHE *aux_cache_member;
    CACHE old_cache_sender_entry, new_cache_sender_entry;

    bzero(( &old_cache_sender_entry ),sizeof( CACHE ) );

   ( old_cache_sender_entry ).sm_info.member_status.last_identified = -1;
   ( old_cache_sender_entry ).sm_info.member_status.first_rcv       = -1;
   ( old_cache_sender_entry ).sm_info.member_status.last_rcv        = -1;
   ( old_cache_sender_entry ).sm_info.member_status.last_seq_rcv    = -1;

#ifdef SINGLE_NACK
   ( old_cache_sender_entry ).sm_info.member_status.window_size = MAX_WINDOW_SIZE;
   ( old_cache_sender_entry ).sm_info.member_status.window_ini = 0;
    memset(( old_cache_sender_entry ).sm_info.member_status.window_mask, 0, MAX_WINDOW_SIZE );
#endif

    p_old_cache_sender_entry = cacheLookForMember( &cache, aux_member_id );

    if( ( p_old_cache_sender_entry != NULL ) )
    {
        old_cache_sender_entry = *p_old_cache_sender_entry;

        if( pckt_info.type == DATA_PACKET_TYPE )
        {
            /* Delete all events about this packet */

            if( !rmcastRemoveEvent( &pckt_info,NAK_SND_WAIT ) )
            {
                #ifdef DSINGLE_NACK
                fprintf( stderr, "DSINGLE_NACK rmcastProcessDataPacket: no NAK_SND_WAIT event found.\n" );
                #endif
                if( !rmcastRemoveEvent( &pckt_info,RET_RCV_WAIT ) )
                {
                    #ifdef DSINGLE_NACK
                    fprintf( stderr, "DSINGLE_NACK rmcastProcessDataPacket: no RET_RCV_WAIT found.\n" );
                    #endif

                    rmcastRemoveEvent( &pckt_info,RET_SND_WAIT );

                    #ifdef DSINGLE_NACK
                    fprintf( stderr, "DSINGLE_NACK rmcastProcessDataPacket: After rmcastRemoveEvent( &pckt_info,RET_SND_WAIT ).\n" );
                    #endif
                }
            }
        }
    }

    pckt_info.type = DATA_PACKET_TYPE;
#ifdef DEBUG_CACHE
    fprintf( stderr, "DEBUG_CACHE rmcastProcessDataPacket: Inserting data in cache: ip: %s pid: %d\n",aux_member_id->ip,aux_member_id->pid );
#endif
    aux_cache_member=cacheLookForMember( &cache, &( pckt_info.sender_id ) );

#ifdef SINGLE_NACK /*block begin*/
{
    int aux_sn;
    int aux_cont;
    int aux_last_seq_rcv;
    EVENT_LIST *aux_list_node;

    aux_sn   = -1;
    aux_cont = 0;
    aux_list_node    = NULL;
    aux_last_seq_rcv = -1;

    if( aux_cache_member != NULL )
    {
        aux_last_seq_rcv = aux_cache_member->sm_info.member_status.last_seq_rcv;
    }
#endif
     if(( aux_cache_member == NULL ) ||( pckt_info.packet_data.data_packet.sn > aux_cache_member->sm_info.member_status.last_seq_rcv ) )
        retval = cacheInsertMessage( &cache, &( pckt_info.sender_id ), &( pckt_info.packet_data.data_packet ) );
     else
        return; /* Old packet. We don't need to handle it again */

    p_new_cache_sender_entry = cacheLookForMember( &cache, aux_member_id );
    /* We don't need to test if p_new_cache_sender_entry is NULL
       because we have just inserted it into the cache */
#ifdef SINGLE_NACK
     /* When we are sliding the window we must check if we have to send a nack to the next sn available in the window_mask */

    if( p_new_cache_sender_entry->sm_info.member_status.last_seq_rcv > aux_last_seq_rcv )
    {

        aux_cont = ( p_new_cache_sender_entry->sm_info.member_status.last_identified -
                    p_new_cache_sender_entry->sm_info.member_status.last_seq_rcv );

        if( aux_cont > MAX_WINDOW_SIZE )
        {
            aux_cont = MAX_WINDOW_SIZE;
        }

        for( i = p_new_cache_sender_entry->sm_info.member_status.last_seq_rcv + 1;
              i <= ( aux_cont + p_new_cache_sender_entry->sm_info.member_status.last_seq_rcv );
              i++ )
        {
            aux_sn = i;

            if( cacheLookForMessage( &cache, &( pckt_info.sender_id ), aux_sn ) == NULL )
            {
#ifdef DSINGLE_NACK
                fprintf( stderr, "DSINGLE_NACK rmcastProcessDataPacket: trying to insert NAK_SND_WAIT event, sn=%d\n",aux_sn );
#endif
                if( eventListFind( &event_list,&( pckt_info.sender_id ),NAK_SND_WAIT,aux_sn,&aux_list_node ) != NULL )
                {
#ifdef DSINGLE_NACK
                    fprintf( stderr, "DSINGLE_NACK rmcastProcessDataPacket: NAK_SND_WAIT exists,just set sn=%d bit\n",aux_sn );
#endif
                    window_mask_set_bit( aux_cache_member->sm_info.member_status.window_mask,aux_sn,&( aux_cache_member->sm_info.member_status ),NAK_SND_WAIT );
                }
                else
                {
                    if( eventListFind( &event_list,&( pckt_info.sender_id ),RET_RCV_WAIT,aux_sn,&aux_list_node ) != NULL )
                    {
#ifdef DSINGLE_NACK
                        fprintf( stderr, "DSINGLE_NACK rmcastProcessDataPacket: RET_RCV_WAIT event exists, just set sn=%d bit\n",aux_sn );
#endif
                        window_mask_set_bit( aux_cache_member->sm_info.member_status.window_mask,aux_sn,&( aux_cache_member->sm_info.member_status ),RET_RCV_WAIT );
                    }
                    else
                    {
#ifdef DSINGLE_NACK
                        fprintf( stderr, "DSINGLE_NACK rmcastProcessDataPacket:Inserting NAK_SND_WAIT event, sn=%d:\n",aux_sn );
#endif
                        eventListInsert( &event_list,&( aux_cache_member->sm_info.member_id ),aux_cache_member->sm_info.member_id.ip, NAK_SND_WAIT,aux_sn );
                    }
                }
            }
        }
    }
}
#endif    /*SINGLE_NACK block end*/


    new_cache_sender_entry = *p_new_cache_sender_entry;
    cacheShow( cache );
    eventListShow( event_list );
    aux_pckt_info = new_cache_sender_entry.last_inserted;

    if( retval == 1 ) /* the inserted packet is in sequence -> we will send data to the application */
    {
#ifdef DEBUG_CACHE
        fprintf( stderr, "DEBUG_CACHE rmcastProcessDataPacket: The inserted packet is in sequence...\n" );
#endif
        for( i  = old_cache_sender_entry.sm_info.member_status.last_seq_rcv + 1;
             i <= new_cache_sender_entry.sm_info.member_status.last_seq_rcv;
             i++ )
        {
    #ifdef DEBUG_CACHE
            fprintf( stderr, "DEBUG_CACHE rmcastProcessDataPacket: A \"reliable\" message will be enqueued:( sn: %d size: %d )\n",
                        i, pckt_info.packet_data.data_packet.data_size );
    #endif
            if( messageQueueEnter(( char* )( aux_pckt_info->data_pckt.data ),pckt_info.packet_data.data_packet.data_size ) != -1 )
            {
                aux_pckt_info = aux_pckt_info->previous;
            }
            else
            {
                fprintf( stderr, "messageQueueEnter ERROR!\n" );
                RM_leaveGroup( local_user_info.socket, local_user_info.member_id.ip );
            }
        }
#ifdef DSINGLE_NACK
        eventListShow( event_list );
#endif
    }
    else if( retval == 2 ) /* the inserted packet is not in sequence -> we will send nak's through the network */
    {
#ifdef DEBUG_CACHE
        fprintf( stderr, "DEBUG_CACHE rmcastprocessdatapacket: The inserted packet is NOT in sequence... \n" );
        fprintf( stderr, "DEBUG_CACHE rmcastProcessDataPacket: showing eventlist:\n" );
        eventListShow( event_list );
#endif
        for( i = old_cache_sender_entry.sm_info.member_status.last_identified+1;
             i <  new_cache_sender_entry.sm_info.member_status.last_identified;
             i++ )
        {
            pckt_info.packet_data.data_packet.sn = i;

            if( cacheLookForMessage( &cache, aux_member_id, i )==NULL )
            {

#ifdef DEBUG_EVENTS
                fprintf( stderr, "DEBUG_EVENTS rmcastProcessDataPacket: trying to schedule a NAK_SND_WAIT event to( sn: %d )\n", i );
                fprintf( stderr, "DEBUG_EVENTS rmcastProcessDataPacket: new_cache_sender_entry.sm_info.member_status.last_seq_rcv=%d\n",
                                new_cache_sender_entry.sm_info.member_status.last_seq_rcv );
#endif
#ifdef SINGLE_NACK
                if( i < new_cache_sender_entry.sm_info.member_status.last_seq_rcv + MAX_WINDOW_SIZE )
                {
#endif
                    if( !rmcastFindEvent( &pckt_info,NAK_SND_WAIT ) )
                    {
                        if( !rmcastFindEvent( &pckt_info, RET_RCV_WAIT ) )
                        {
#ifndef SINGLE_NACK
                            /* when we are using SINGLE_NACK this check is done inside eventListInsert */
                            if( cacheUpdateNakList( &cache, &( pckt_info.sender_id ), i )!=0 )
                            {
#endif
                                rmcastInsertEvent( &pckt_info,NAK_SND_WAIT );
                                #ifdef DEBUG_EVENTS
                                fprintf( stderr, "DEBUG_EVENTS rmcastProcessDataPacket: NAK_SND_WAIT was inserted in the event list.\n" );
                                #endif
#ifndef SINGLE_NACK
                            }
                            else
                            {
                                /*
                                * We have sent the max number of NAK( see rmcast_options structure ) we didn't recover the packet
                                * so we must exit because we cannot keep the reliability
                                */
                                fprintf( stderr, "********\n" );
                                fprintf( stderr, "rmcastProcessDataPacket ERROR: Recovering packet failed. Max number( %d ) of NAKs reached!\n",rmcast_options.max_nak );
                                fprintf( stderr, "\tHost IP:PID=%s:%d\tsn=%d\n",aux_member_id->ip,aux_member_id->pid,i );
                                fprintf( stderr, "********\n" );

                                RM_leaveGroup( local_user_info.socket, local_user_info.member_id.ip );
                            }
            #endif
                        }
#ifdef SINGLE_NACK
                        else
                        {
                             cache_aux1_entry=cacheLookForMember( &cache, aux_member_id );
                             #ifdef DSINGLE_NACK
                              fprintf( stderr, "DSINGLE_NACK rmcastProcessDataPacket: before window_mask_set_bit\n" );
                              fprintf( stderr, "DSINGLE_NACK rmcastProcessDataPacket: there is a RET_RCV_WAIT, just set sn=%d\n",i );
                             #endif
                             window_mask_set_bit( cache_aux1_entry->sm_info.member_status.window_mask,
                                                 i,&cache_aux1_entry->sm_info.member_status,RET_RCV_WAIT );
                        }
#endif
                    }
#ifdef SINGLE_NACK
                    else
                    {
                        int aux_retval=0;

                    #ifdef DSINGLE_NACK
                        fprintf( stderr, "DSINGLE_NACK rmcastProcessDataPacket: found a NAK_SND_WAIT event.\n" );
                    #endif
                        cache_aux1_entry=cacheLookForMember( &cache, aux_member_id );

                    #ifdef DWINDOW_MASK
                        fprintf( stderr, "DWINDOW_MASK rmcastProcessDataPacket: before window_mask_get_bit\n" );
                        fprintf( stderr, "DWINDOW_MASK rmcastProcessDataPacket: window_mask=%p\n",cache_aux1_entry->sm_info.member_status.window_mask );
                        fprintf( stderr, "DWINDOW_MASK rmcastProcessDataPacket: sn=%d\n",i );
                        fprintf( stderr, "DWINDOW_MASK rmcastProcessDataPacket: member_status=%p\n",&cache_aux1_entry->sm_info.member_status );
                    #endif

                        aux_retval=window_mask_get_bit( cache_aux1_entry->sm_info.member_status.window_mask,i,&cache_aux1_entry->sm_info.member_status );

                        if( aux_retval != RET_RCV_WAIT ) /* FIXME - este teste pode nao ser necessario pois nao queremos eventos NAK e RET_RCV concorrentes */
                        {
                            if( cacheUpdateNakList( &cache, &( pckt_info.sender_id ), i )!=0 )
                            {
                                #ifdef DEBUG_EVENTS
                                fprintf( stderr, "DEBUG_EVENTS rmcastProcessDataPacket: NAK_SND_WAIT event exists, just set sn=%d to NAK_SND_WAIT\n",i );
                                #endif
                                window_mask_set_bit( cache_aux1_entry->sm_info.member_status.window_mask,i,&cache_aux1_entry->sm_info.member_status,NAK_SND_WAIT );
                            }
                            else
                            {
                                /*
                                * We have sent the max number of NAK( see rmcast_options structure ) we didn't recover the packet
                                * so we must exit because we cannot keep the reliability
                                */

                                fprintf( stderr, "********\n" );
                                fprintf( stderr, "rmcastProcessDataPacket ERROR: Recovering packet failed. Max number( %d ) of NAKs reached!\n",rmcast_options.max_nak );
                                fprintf( stderr, "\tHost IP:PID=%s:%d\tsn=%d\n",aux_member_id->ip,aux_member_id->pid,i );
                                fprintf( stderr, "********\n" );

                                RM_leaveGroup( local_user_info.socket, local_user_info.member_id.ip );
                            }
                        }
                    }

                } /* i is in the window_mask range*/
#endif /*SINGLE_NACK */
            } /* message exists we don't need to send a NACK */
        } /* for( i = old_cache_sender_entry.sm_info.member_status.last_identified+1; ... */
    } /*else if( retval==2 ) */
    else
    {
        #ifdef DEBUG_CACHE
         fprintf( stderr, "DEBUG_CACHE rmcastProcessDataPacket: Couldn´t insert cache node.\n" );
        #endif
    }

}

/*****************************************************************************************************
 *
 * void rmcastProcessRefreshPacket( PACKET_INFO pckt_info )
 *
 * Processes the refresh packet.
 *
 * Arguments: the packet to be processed, in pckt_info;
 *
 * Side effects: may affect the event list, the cache and the message queue.
 *
 ******************************************************************************************************/
#ifdef REFRESH
void rmcastProcessRefreshPacket( PACKET_INFO pckt_info )
{
    int i, new_value_of_last_identified;
    CACHE *cache_sender_entry;
    cache_sender_entry = cacheLookForMember( &cache, &( pckt_info.sender_id ) );

    if( pckt_info.packet_data.refresh_packet.sn_of_last_msg_sent < 0 )
        return;

    if( cache_sender_entry == NULL )
    {
        cache_sender_entry  = cacheInsertMember( &cache, &( pckt_info.sender_id ) );
    }

    new_value_of_last_identified = pckt_info.packet_data.refresh_packet.sn_of_last_msg_sent;

#ifdef SINGLE_NACK
    if( new_value_of_last_identified >( cache_sender_entry->sm_info.member_status.last_seq_rcv + MAX_WINDOW_SIZE ) )
    {
        /* Send NACKs that fit in the window */
        new_value_of_last_identified = cache_sender_entry->sm_info.member_status.last_seq_rcv + MAX_WINDOW_SIZE;
    }

    for( i = new_value_of_last_identified;
         i > cache_sender_entry->sm_info.member_status.last_identified;
         i -- )
    {
        if( !rmcastFindEvent( &pckt_info,NAK_SND_WAIT ) )
        {
            if( !rmcastFindEvent( &pckt_info,RET_RCV_WAIT ) )
            {
                pckt_info.packet_data.refresh_packet.sn_of_last_msg_sent = i;
                rmcastInsertEvent( &pckt_info,NAK_SND_WAIT );
#ifdef DSINGLE_NACK
                fprintf( stderr, "DSINGLE_NACK rmcastProcessRefreshPacket: NAK_SND_WAIT insertion caused by refresh message\n" );
                fprintf( stderr, "DSINGLE_NACK rmcastProcessRefreshPacket: host:pid:sn=%s:%d:%d\n",
                                pckt_info.sender_id.ip,pckt_info.sender_id.pid,i );
#endif
            }
            else
            {
                /* RET_RCV_WAIT exists, just set the i bit in the window_mask */
                window_mask_set_bit( cache_sender_entry->sm_info.member_status.window_mask,
                                     i,
                                     &( cache_sender_entry->sm_info.member_status ),
                                     RET_RCV_WAIT );
#ifdef DSINGLE_NACK
                fprintf( stderr, "DSINGLE_NACK rmcastProcessRefreshPacket: bit settled to RET_RCV_WAIT caused by refresh message\n" );
                fprintf( stderr, "DSINGLE_NACK rmcastProcessRefreshPacket: host:pid:sn=%s:%d:%d\n",
                                pckt_info.sender_id.ip,pckt_info.sender_id.pid,i );
#endif
            }
        }
        else
        {
            /* NAK_SND_WAIT exists, just set the i bit in the window_mask */
            window_mask_set_bit( cache_sender_entry->sm_info.member_status.window_mask,
                                 i,
                                 &( cache_sender_entry->sm_info.member_status ),
                                 NAK_SND_WAIT );
#ifdef DSINGLE_NACK
            fprintf( stderr, "DSINGLE_NACK rmcastProcessRefreshPacket: bit settled to NAK_RCV_WAIT caused by refresh message\n" );
            fprintf( stderr, "DSINGLE_NACK rmcastProcessRefreshPacket: host:pid:sn=%s:%d:%d\n",
                            pckt_info.sender_id.ip,pckt_info.sender_id.pid,i );
#endif
        }
    }
#else
    for( i = pckt_info.packet_data.refresh_packet.sn_of_last_msg_sent;
         i > cache_sender_entry->sm_info.member_status.last_identified;
         i -- )
        {
           #ifdef DEBUG_EVENTS
              fprintf( stderr, "Will wait to send a nak, i: %d last received or nak: %d.\n", i,
                    cache_sender_entry->sm_info.member_status.last_identified );
           #endif

            pckt_info.packet_data.refresh_packet.sn_of_last_msg_sent = i;
            rmcastInsertEvent( &pckt_info,NAK_SND_WAIT );
        }
#endif /* SINGLE_NACK */
   if( cache_sender_entry->sm_info.member_status.last_identified < new_value_of_last_identified )
       cache_sender_entry->sm_info.member_status.last_identified = new_value_of_last_identified;
}
#endif

/*****************************************************************************************************
 *
 * int rmcastRemoveEvent( PACKET_INFO *packt, int event_type )
 *
 * Removes an event from the event list.
 *
 * Arguments: the network packet, in packt;
 *            the event type, in event_type.
 *
 * Returns: 1, on sucess;
 *          0, otherwise.
 *
 * Side effects: it affects the event list.
 *
 ******************************************************************************************************/
int rmcastRemoveEvent( PACKET_INFO *packt, int event_type )
{
    int retval;

    retval = 0;

    switch( packt->type )
    {
        case RETRANSM_PACKET_TYPE:
             retval = eventListRemove( &event_list, &( packt->packet_data.retransm_packet.original_sender_id ),
                                     event_type, packt->packet_data.retransm_packet.data_packet.sn );
             break;

        case DATA_PACKET_TYPE:
             retval = eventListRemove( &event_list, &( packt->sender_id ),
                                     event_type, packt->packet_data.data_packet.sn );
             break;
        case NAK_PACKET_TYPE:
             #ifdef DNAK_RCV
                 fprintf( stderr,  "rmcastRemoveEvent: calling eventListRemove\n" );
             #endif
             retval = eventListRemove( &event_list, &( packt->packet_data.nak_packet.requested_member_id ),
                                     event_type, packt->packet_data.nak_packet.sn );
             #ifdef DNAK_RCV
                 fprintf( stderr,  "rmcastRemoveEvent: after calling eventListRemove\n" );
             #endif
             break;
    }
    return  retval;
}

/*****************************************************************************************************
 *
 * int rmcastFindEvent( PACKET_INFO *packt, int event_type )
 *
 * Searches for an event in the event list.
 *
 * Arguments: the network packet, in packt;
 *            the event type, in event_type.
 *
 * Returns: 1, on sucess;
 *          0, otherwise.
 *
 ******************************************************************************************************/
int rmcastFindEvent( PACKET_INFO *packt, int event_type )
{
    int retval;

    EVENT_LIST *aux;

    switch( packt->type )
    {
        case NAK_PACKET_TYPE:
             #ifdef DNAK_RCV
                 fprintf( stderr,  "rmcastFindEvent: calling eventlistFind to look for event:%d\n",event_type );
             #endif

             retval = (( eventListFind( &event_list, &( packt->packet_data.nak_packet.requested_member_id ),
                                      event_type, packt->packet_data.nak_packet.sn,&aux ) )!= NULL );
             #ifdef DNAK_RCV
                 fprintf( stderr,  "rmcastFindEvent: after calling eventlistFind to look for event:%d, retval=%d\n",event_type,retval );
             #endif
             break;
        case DATA_PACKET_TYPE:
             retval = (( eventListFind( &event_list, &( packt->sender_id ),
                                     event_type, packt->packet_data.data_packet.sn,&aux ) )!=NULL );
             break;
        case REFRESH_PACKET_TYPE:
             retval = (( eventListFind( &event_list, &( packt->sender_id ),
                                     event_type, packt->packet_data.refresh_packet.sn_of_last_msg_sent,&aux ) )!=NULL );
             break;
        default:
             fprintf( stderr, "rmcastFindEvent ERROR: Not yet implemented event=%d packet_type=%d.\n",event_type,packt->type );
             return 0;
    }
    return retval;
}

/*****************************************************************************************************
 *
 * int rmcastCacheContains( PACKET_INFO *packt )
 *
 * Searches for a packet in the cache.
 *
 * Arguments: the network packet, in packt;
 *
 * Returns: 1, on sucess;
 *          0, otherwise.
 *
 ******************************************************************************************************/
int rmcastCacheContains( PACKET_INFO *packt )
{
    int retval;

    retval = 0;

    switch( packt->type )
    {
        case NAK_PACKET_TYPE:
             retval = (( cacheLookForMessage( &cache, &( packt->packet_data.nak_packet.requested_member_id ),
                                           packt->packet_data.nak_packet.sn ) )!= NULL );
             break;
        case RETRANSM_PACKET_TYPE:
             retval = (( cacheLookForMessage( &cache, &( packt->packet_data.retransm_packet.original_sender_id ),
                                           packt->packet_data.retransm_packet.data_packet.sn ) )!= NULL );
             break;
        default:
             fprintf( stderr, "rmcastCacheContains: Unknown packet type:%d\n",packt->type );
             RM_leaveGroup( local_user_info.socket, local_user_info.member_id.ip );
    }
    return retval;
}

/*****************************************************************************************************
 *
 * int rmcastInsertEvent( PACKET_INFO *packt, int event_type )
 *
 * Inserts an event in the event list.
 *
 * Arguments: the network packet, in packt;
 *            the event type, in event_type.
 *
 * Returns: 1, on sucess;
 *          0, otherwise.
 *
 * Side effects: it affects the event list.
 *
 ******************************************************************************************************/
int rmcastInsertEvent( PACKET_INFO *packt, int event_type )
{
    int retval;
    CACHE *cache_sender_entry;

    retval = 0;

    switch( packt->type )
    {
        case NAK_PACKET_TYPE:
             cache_sender_entry = cacheLookForMember( &cache,
                                         &( packt->packet_data.nak_packet.requested_member_id ) );
             #ifdef DNAK_RCV
                 fprintf( stderr,  "rmcastInsertEvent: calling eventListInsert\n" );
             #endif

             if( cache_sender_entry != NULL )
                 retval = eventListInsert( &event_list, &( cache_sender_entry->sm_info.member_id ),
                                         packt->sender_id.ip, event_type, ( packt->packet_data.nak_packet.sn ) );
             #ifdef DNAK_RCV
                 fprintf( stderr,  "rmcastInsertEvent: after calling eventListInsert cache_sender_entry:%p\n",cache_sender_entry );
             #endif

             break;
        case DATA_PACKET_TYPE:
             cache_sender_entry = cacheLookForMember( &cache, &( packt->sender_id ) );
             if( cache_sender_entry!=NULL )
                 retval = eventListInsert( &event_list,&( cache_sender_entry->sm_info.member_id ),
                                      packt->sender_id.ip, event_type, packt->packet_data.data_packet.sn );
             break;
        case REFRESH_PACKET_TYPE:
             cache_sender_entry = cacheLookForMember( &cache, &( packt->sender_id ) );

             if( cache_sender_entry!=NULL )
                 retval = eventListInsert( &event_list,&( cache_sender_entry->sm_info.member_id ),
                                      packt->sender_id.ip,event_type, packt->packet_data.refresh_packet.sn_of_last_msg_sent );

             break;
        default:
             fprintf( stderr, "rmcastInsertEvent: Unknown event type:%d\n",packt->type );
             RM_leaveGroup( local_user_info.socket, local_user_info.member_id.ip );

    }
#ifdef DNAK_RCV
    if( event_type == NAK_PACKET_TYPE )
    {
        fprintf( stderr, "rmcastInsertEvent: Show cache after NACK event insertion\n" );
        cacheShow( cache );
    }
#endif

    return retval;
}

/**********End of the Routines to facilitate the interface with the event list and the cache ******/
#endif
