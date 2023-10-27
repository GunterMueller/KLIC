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
                             rmcast.c
                             -------------------
    begin                : May 2001
    Authors              : Jorge Allyson Azevedo
                           Milena Scanferla
                           Daniel Sadoc
    email                : {allyson,milena,sadoc}@land.ufrj.br
 ***************************************************************************/

#ifndef RMCAST_C
#define RMCAST_C

#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <sys/msg.h>
#include <netdb.h>
#ifdef LINUX
    #include <sys/ioctl.h>
    #include <net/if.h>             //where if_nameindex() was hiding
    #include <linux/sockios.h>      //for ioctl() arg
#endif

#include "rmcast.h"
#include "rminternals.h"
#include "rmtcp.h"

#ifdef SOLARIS
int siginterrupt( int sig, int flag )
{
    struct sigaction act;

    (void)sigaction( sig, NULL, &act );

    if( flag )
        act.sa_flags &= ~SA_RESTART;
    else
        act.sa_flags |= SA_RESTART;
    return sigaction( sig, &act, NULL);
}
#endif

pthread_t rcv_thread, signal_handler_thread;

extern int          h_errno;
extern EVENT_LIST   *event_list;
extern CACHE        *cache;
extern USER_INFO    local_user_info;
extern sigset_t     alrmset;
extern FILE         *logfile;
extern GLOBAL_OPTIONS rmcast_options;

void (*callback_terminate)( void );

#ifdef DSUPPRESSION
    FILE *suppressionfile;
#endif

time_t seed;

static int r_qid = -1; /* message queue identifier */
static int initialized = 0; /* flag that specifies if the user is connected to
                               the multicast group.
                               It may also be read as: did the user try to
                               get the current state?*/
static int sent_packets = 0; /* To count the number of packets sent by 
                               the member */

#ifdef SOLARIS
    pthread_mutex_t  change_local_user_sn;
#else
    pthread_mutex_t  change_local_user_sn = PTHREAD_MUTEX_INITIALIZER;
#endif

/*******************************************************************************
 *
 * void RM_readConfigFile(char *filename, char show_config_file)
 *
 *  Reads configuration file indicated by filename
 *
 * Parameters: filename, name of the configuration file.
 *             show_config_file, 1 shows config file contents in stdout
 *                               0 does not show config file contents
 *
 * Returns: 1, if config file reading was OK;
 *          0, otherwise
 *
 ******************************************************************************/
int RM_readConfigFile( char *filename, char show_config_file )
{
    FILE *fpin = NULL;
    int  i, j, aux_estimated_delay, optionnumber, host_counter = -1;
    char option[ 255 ], value[ 255 ], trash[ 4 ];
    char line[ 255 ];
    char aux_host_name[ 255 ];

    int retval = FALSE;

    fpin = fopen( filename, "r" );

    if( fpin != NULL )
    {
        retval = TRUE;
        if( show_config_file == TRUE )
        {
            fprintf( stdout, "RELIABLE MULTICAST LIBRARY\n\n" );
            fprintf( stdout, "---- Reading configuration file ----\n" );
        }

        fgets( line, 255, fpin );

        while( !feof( fpin ) && ( retval==1 ) )
        {
            i = sscanf( line, " %[^=# ] %[=#] %s", option, trash, value );
            if( i != 3 || trash[ 0 ] == '#' )
            {
                // fprintf( stderr, "skiping line (%d)\n", i );
            }
            else
            {
                if( show_config_file == TRUE )
                    fprintf( stdout, "%s=%s\n", option, value );
                if( strcmp( option, "DEST_IP" ) == 0 )
                    optionnumber = DEST_IP;
                else if( strcmp( option, "DEST_PORT" ) == 0 )
                    optionnumber = DEST_PORT;
                else if( strcmp( option, "TTL" ) == 0 )
                    optionnumber = TTL;
                else if( strcmp( option, "MICROSLEEP" ) == 0 )
                    optionnumber = MICROSLEEP;
                else if( strcmp( option, "TIMER_PARAM_A" ) == 0 )
                    optionnumber = TIMER_PARAM_A;
                else if( strcmp( option, "TIMER_PARAM_B" ) == 0 )
                    optionnumber = TIMER_PARAM_B;
                else if( strcmp( option, "TIMER_PARAM_C" ) == 0 )
                    optionnumber = TIMER_PARAM_C;
                else if( strcmp( option, "TIMER_PARAM_D" ) == 0 )
                    optionnumber = TIMER_PARAM_D;
                else if( strcmp( option, "TIMER_PARAM_E" ) == 0 )
                    optionnumber = TIMER_PARAM_E;
                else if( strcmp( option, "TIMER_PARAM_F" ) == 0 )
                    optionnumber = TIMER_PARAM_F;
                else if( strcmp( option, "HOSTS_IDENTIFIED" ) == 0 )
                    optionnumber = HOSTS_IDENTIFIED;
                else if( strcmp( option, "TIMER_DISTRIBUTION" ) == 0 )
                    optionnumber = TIMER_DISTRIBUTION;
                else if( strcmp( option, "MAX_NAK" ) == 0 )
                    optionnumber = MAX_NAK;
                else if( strcmp( option, "MAX_MEMBER_CACHE_SIZE" ) == 0 )
                    optionnumber = MAX_MEMBER_CACHE_SIZE;
                else if( strcmp( option, "RM_VERSION" ) == 0 )
                    optionnumber = RM_VERSION;
                else if( strcmp( option, "TRANSMISSION_MODE" ) == 0 )
                    optionnumber = TRANSMISSION_MODE;
                else if( strcmp( option, "LOG_FILE" ) == 0 )
                    optionnumber = LOG_FILE;
                else if( strcmp( option, "NEW_MEMBER_SUPPORT" ) == 0 )
                    optionnumber = NEW_MEMBER_SUPPORT;
                else if( strcmp( option, "STATISTICS" ) == 0 )
                    optionnumber = STATISTICS;
                else if( strcmp( option, "REFRESH_TIMER" ) == 0 )
                    optionnumber = REFRESH_TIMER;
                else if(strcmp( option, "LOSS_PROB" ) == 0 )
                    optionnumber = LOSS_PROB;
                else if(strcmp( option, "LEAVE_GROUP_WAIT_TIME" ) == 0 )
                    optionnumber = LEAVE_GROUP_WAIT_TIME;
                else if( strcmp( option, "RCV_BUFFER_SIZE" ) == 0 )
                    optionnumber = RCV_BUFFER_SIZE;
                else
                {
                    fprintf( stderr, "RM_readConfigFile Warning - Invalid option in input file: %s", line );
                    fgets( line, 255, fpin );
                    continue;
                }

                switch( optionnumber )
                {

                    case LOG_FILE:
                    case DEST_IP:
                        RM_setOption( optionnumber, (void*)value );
                        break;
                    case HOSTS_IDENTIFIED:
                        host_counter = atoi( value );

                        if( ( host_counter > -1 ) && ( host_counter < MAX_HOSTS ) )
                        {
                            fgets( line, 255, fpin );
                            for( j = 0; j <= host_counter; j++ )
                            {
                                if( !feof( fpin ) )
                                {
                                    sscanf( line, "%s %d", aux_host_name, &aux_estimated_delay );
                                    if( RM_setHostDelay( aux_host_name, aux_estimated_delay ) > 0 )
                                    {
                                        if( ( show_config_file == TRUE ) &&
                                              ( RM_getHostDelay( aux_host_name, &aux_estimated_delay ) == 1 ) )
                                        {
                                            fprintf( stdout, "%s %d\n", aux_host_name, aux_estimated_delay );
                                        }
                                    }
                                    else
                                    {
                                        fprintf( stderr, "RM_readConfigFile Error: Unknown host %s\n", aux_host_name );
                                        retval = FALSE;
                                        break;
                                    }
                                }
                                else
                                {
                                    fprintf( stderr, "RM_readConfigFile Error reading timers in config file" );
                                    retval = FALSE;
                                }
                                fgets( line, 255, fpin );
                            }
                            host_counter=-1;
                        }
                        break;
                    default:
                        RM_setOption( optionnumber, (void*)atoi( value ) );
                }
            }
            fgets( line, 255, fpin );
        }

        if( ( show_config_file == TRUE ) && ( retval == TRUE ) )
            fprintf( stdout, "----- Configuration file OK.-----\n\n" );

        fclose( fpin );
    }
    return retval;
}

/*******************************************************************************
 *
 * void RM_initialize()
 *
 * Initializes internal structures (cache, event list and message queue) and
 * local user information (ip, pid and sn).
 *
 * Parameters: no parameteres.
 * Returns: 1, on success;
 *          0, on error.
 *
 ******************************************************************************/
#ifdef SOLARIS
int gethostname( char *name, int namelen );
#endif

int RM_initialize( void *callbackterm( void ) )
{
    int    i;
    char   localhost_ip[ IP_STRING_SIZE ], localhost_name[ 31 ];
    struct timeval tv;
    struct timezone tz;
#ifdef LINUX
    int    auxsock = 0;
    struct ifreq ifreq;
    struct sockaddr_in *saptr = NULL;
    struct if_nameindex *iflist = NULL, *listsave = NULL;
#else
    struct hostent *localhost_ent;
#endif /* LINUX */

callback_terminate = (void *)callbackterm;

/*
 * #ifdef  SOLARIS
 *
 *     sigset_t     alrmset;
 *
 *     sigfillset(&alrmset);
 *
 *
 *
 *     pthread_sigmask(SIG_BLOCK, &alrmset, NULL);
 *
 *     pthread_mutex_init(&change_local_user_sn,NULL);
 *
 * #endif
 */
    /* Create seed for random numbers generation */
    gettimeofday( &tv, &tz );
    seed = time( (time_t *)&tv.tv_sec );
    srandom( (unsigned int)seed );

    rmcast_options.cur_state_server_is_up = 0;

    initialized = 0;

    if( ( rmcast_options.log_file[ 0 ] != 0 ) && ( strcmp( rmcast_options.log_file, "NULL" ) != 0 ) )
    {
        if( ( logfile = fopen( rmcast_options.log_file,"w" ) ) == NULL )
        {
            fprintf( stderr, "RM_initialize Error: could not open %s.", rmcast_options.log_file );
            return 0;
        }

        if( setvbuf( logfile, NULL, _IONBF, 0 ) != 0 )
            fprintf( stderr, "RM_initialize ERROR: setvbuf failed.\n" );
    }
    else
    {
        logfile = NULL;
    }

#ifdef DSUPPRESSION
    if( ( suppressionfile = fopen("log.suppression","w") ) == NULL )
    {
        fprintf( stderr, "RM_initialize Error: could not open 'log.suppression'." );
        return 0;
    }
#endif

    for( i=0; i < IP_STRING_SIZE; i++ )
        localhost_ip[ i ] = 0;

    /* localhost_ip <- local ip */
    gethostname( localhost_name, 30 );

    if( logfile != NULL )
    {
        fprintf( logfile, "host: %s\n", localhost_name );
    }

#ifdef DEBUG_NET_MSGS
    fprintf( stderr, "DEBUG_NET_MSGS RM_initialize: local host name: %s\n", localhost_name );
#endif

#ifdef LINUX
    /* need a socket for ioctl() */
    if( ( auxsock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        perror( "RM_initialize: socket for ioctl" );
        return 0;
    }

    /* returns pointer to dynamically allocated list of structs */
    iflist = listsave = if_nameindex();

    /* walk thru the array returned and query for each */
    /* interface's address */
    for( ; *(char *)iflist != 0; iflist++ )
    {
        /* copy in the interface name to look up address of */
        strncpy( ifreq.ifr_name, iflist->if_name, IF_NAMESIZE );

        /* get the address for this interface */
        if( ioctl( auxsock, SIOCGIFADDR, &ifreq ) < 0 )
        {
            if( errno =! 99 )
            {
                perror( "RM_initialize: ioctl" );
                return 0;
            }
        }

        /* Take a valid address */
        saptr = (struct sockaddr_in *)&ifreq.ifr_addr;
        strcpy( localhost_ip, inet_ntoa( saptr->sin_addr ) );

        /* Take the first ip different from 127.0.0.1 */
        if( strcmp( "127.0.0.1", localhost_ip ) != 0 )
            break;
    }

    /* free the dynamic memory kernel allocated for us */
    if_freenameindex( listsave );
#else
    localhost_ent  = gethostbyname( localhost_name );

    if( localhost_ent != NULL )
    {
        strcpy( localhost_ip,
            inet_ntoa( *((struct in_addr*)(localhost_ent->h_addr_list[ 0 ])) ) );
    }
    else
    {
        fprintf (stderr, "RM_initialize ERROR: Unable to resolve host name: %s\n", localhost_name );
        return 0;
    }
#endif /* LINUX */

    /* initialization of the local cache, event list and message queue */

    cacheInit( &cache );
    eventListInit( &event_list );

    if( ( r_qid == -1 ) && ( r_qid = init_queue() ) == -1 )
    {
        fprintf( stderr, "RM_initialize ERROR: Couldn't create message queue.\n" );
        return 0;
    }

    /* initialization of local user info */
    pthread_mutex_lock( &change_local_user_sn );
    local_user_info.sn = 0;
    pthread_mutex_unlock( &change_local_user_sn );

    local_user_info.member_id.pid = (int)getpid();
    strncpy( local_user_info.member_id.ip, localhost_ip, IP_STRING_SIZE );

    if( logfile != NULL )
    {
        fprintf( logfile, "ip: %s\n", localhost_ip );
        fprintf( logfile, "pid: %d\n", local_user_info.member_id.pid );

        fprintf( logfile, "---------------------------------------------------------------------------------------------------------------------------\n" );
        fprintf( logfile, "Time Snd/Rcv/Loss type sender_ip       sender_pid requested_ip    requested_pid  sn  [{base sn} {win size} {hmask} {lmask}]\n" );
        fprintf( logfile, "---------------------------------------------------------------------------------------------------------------------------\n" );
    }
#ifdef DEBUG_NET_MSGS
    fprintf( stderr, "DEBUG_NET_MSGS RM_initialize: local host ip: %s\n", localhost_ip );
#endif
    return 1;
}

#ifdef SUPPORT_FOR_NEW_MEMBERS
static void recvfrom_alarm( int signo )
{
    /* just interrupt the recvfrom() */
    return;
}
#endif

/*******************************************************************************
 *
 * int    RM_getCurStatus (char *group, int port, CurStatus *c)
 *
 * When this routine is used, RM_initialize must have been called at least once.
 *
 * Sets up multicast group information, creates the thread responsible for receiving
 * packets and activates the alarm responsible for catching events from the event list.
 * If the CurState parameter is different from NULL, the current state of the multicast
 * group will be catched.
 *
 * Parameters: group, the multicast group ip;
 *             port,  the multicast group port;
 *             c_status, pointer to the location where the current state of
 *                    the multicast group will be stored.
 *
 * Returns: the socket created.  If c is not equal to NULL, it will point to the current
 *          state of the group.
 *
 ******************************************************************************/
pthread_t pthread_cur_state_server;

#ifndef SOLARIS
#define SOCKLEN_TYPE       (socklen_t*)
#else
#define SOCKLEN_TYPE
#endif

#ifdef SOLARIS
    int siginterrupt( int sig, int flag );
#endif

int RM_getCurStatus( char *group1, int port1, CurStatus *current_status )
{
#ifdef SOLARIS
    sigset_t     alrmset;
#endif

    char *group;
    int  port;

    if( ( (int)group1 ) == RM_USE_CURRENT_CONFIG )
    {
        group = rmcast_options.dest_ip;
    }
    else
    {
        group = group1;
    }

    if( ( (int)port1 ) == RM_USE_CURRENT_CONFIG )
    {
        port = rmcast_options.dest_port;
    }
    else
    {
        port = port1;
    }

    /* Now, we are setting the multicast info (multicast port and multicast ip), */
    local_user_info.group_id.port = port ;
    strncpy( local_user_info.group_id.ip, group, IP_STRING_SIZE );
    local_user_info.socket = rmcastCreateSocket( port );
    rmcastJoinGroup( local_user_info.socket, group );

    if( logfile != NULL )
        fprintf( logfile, "joined to group\n" );

#ifdef DEBUG_NET_MSGS
    fprintf( stderr, "DEBUG_NET_MSGS RM_getCurStatus: joined to group\n" );
#endif

#ifdef SUPPORT_FOR_NEW_MEMBERS
    if( current_status != NULL )
    {
        char *buffer;
        int  buffsize;
        int  timeout;
        int  len, nbytes;
        char message[ rmcast_options.rcv_buffer_size ];
        CurStatus       retval;
        PACKET_INFO     pckt_info;
        struct sockaddr sender;

        timeout = 0;

        pckt_info.type        = JOIN_REQUEST_PACKET_TYPE;
        pckt_info.flags       = 0;
        pckt_info.sender_id   = local_user_info.member_id;
        pckt_info.packet_size = 0;

        msgPcktMountMessage( &pckt_info, (BYTE**)&buffer, &buffsize );
        msgPcktShowMessage( (BYTE*)buffer );

        if ( rmcastSendPacket( local_user_info.socket, (BYTE*)buffer, buffsize ) == 0 )
        {
            current_status = NULL;
            return 0;
        }

        /* waiting to receive a "join accept packet" */

#ifdef SOLARIS

/*
 *         sigemptyset( &alrmset );
 *         sigaddset  ( &alrmset, SIGALRM );
 *
 *         pthread_sigmask( SIG_UNBLOCK, &alrmset, NULL );
 */

#endif
        siginterrupt( SIGALRM, 1 ); /* please, see siginterrupt(3) */

        if( signal( SIGALRM, recvfrom_alarm ) == SIG_ERR )
        {
            fprintf( stderr,"RM_getCurStatus ERROR: alarm failed" );
            current_status = NULL;
            return 0;
        }

        alarm( 2 );

#ifdef  DEBUG_NET_MSGS
        fprintf( stderr, "DEBUG_NET_MSGS RM_getCurStatus: Waiting for a 'join accept message'\n");
#endif
        do
        {
            static int first = 1;
#ifdef  DEBUG_NET_MSGS
            fprintf( stderr, "DEBUG_NET_MSGS RM_getCurStatus: Still waiting for a 'join accept message'\n" );
#endif
            if( !first )
                first = 0;

            bzero( message, rmcast_options.rcv_buffer_size );
            len = sizeof( struct sockaddr_in );

            if( ( nbytes = (int)recvfrom( local_user_info.socket, message, rmcast_options.rcv_buffer_size, 0,
                          (struct sockaddr *) &sender, SOCKLEN_TYPE &len )) < 0 )
            {
                if( errno != EINTR )
                    continue;
            }
            if( nbytes > 0 )
            {
#ifdef DEBUG_PCKT
                {
                    int cont;
                    fprintf( stderr, "DEBUG_PCKT RM_getCurStatus: Received message: \n" );
                    for( cont = 0; cont < nbytes; cont++ )
                    {
                        fprintf( stderr, "%d [%c] ", ((signed char*)(message))[ cont ], (message)[ cont ] );
                    }
                }
#endif
                msgPcktUnmountMessage( &pckt_info, (BYTE*)message );
#ifdef DEBUG_SHOW
                fprintf( stderr, "DEBUG_SHOW RM_getCurStatus: Showing message:\n" );
                msgPcktShowMessage( (BYTE*)message );
#endif
                if( !memberIsEqual( &local_user_info.member_id, &pckt_info.sender_id) )
                    timeout=0;
            }
            else
            {
                timeout        =  1;
                pckt_info.type = -1;
            }
        }
        while( ( pckt_info.type != JOIN_ACCEPT_PACKET_TYPE ) && ( timeout == 0 ) );

        siginterrupt( SIGALRM, 0 );   /* the default behaviour in Linux */
#ifdef SOLARIS

/*
 *         sigemptyset( &alrmset );
 *         sigaddset  ( &alrmset, SIGALRM );
 *
 *
 *         pthread_sigmask( SIG_BLOCK, &alrmset, NULL );
 */

#endif
        if( !timeout )
        {
#ifdef DEBUG_NET_MSGS
            fprintf( stderr, "DEBUG_NET_MSGS RM_getCurStatus: 'join accept packet' received\n" );
#endif
            if( rmcastReceiveTCPStatus( pckt_info.sender_id.ip, pckt_info.packet_data.join_accept_packet.port, &retval ) == FALSE )
            {
#ifdef DEBUG_NET_MSGS
                fprintf( stderr, "DEBUG_NET_MSGS RM_getCurStatus: TCP status received\n" );
#endif
                retval.size = -1;
            }
        }
        else
        {
#ifdef DEBUG_NET_MSGS
            fprintf( stderr, "DEBUG_NET_MSGS RM_getCurStatus: timer expired probably I'm the first of the group!\n" );
#endif
            retval.size = -1;
        }

        *current_status = retval;
    }
#endif

#ifndef SOLARIS 
     sigfillset(&alrmset);
     sigdelset(&alrmset, SIGTSTP);
     pthread_sigmask(SIG_BLOCK, &alrmset, NULL);
#endif

#ifdef SUPPORT_FOR_NEW_MEMBERS
    if( pthread_create( &pthread_cur_state_server, NULL, rmcastSendTCPStatus, NULL ) != 0 )
        fprintf( stderr, "RM_getCurStatus ERROR: Couldn't create the new thread to send the current status: the 'current status server'\n" );
    else
        rmcast_options.cur_state_server_is_up = 1;
#endif
    initialized = 1;

    return local_user_info.socket;
}

/*******************************************************************************
 *
 * int    RM_joinGroup (char *group, int port)
 *
 * When this routine is used, RM_initialize must have been called at least once.
 *
 * Sets up multicast group information, creates the thread responsible for receiving
 * packets and activates the alarm responsible for catching events from the event list.
 *
 * Parameters: group, the multicast group ip;
 *               port,  the multicast group port;
 *
 * Returns: the socket created.  If c is not equal to NULL, it will point to the current
 *          state of the group.
 *
 ******************************************************************************/
int RM_joinGroup( char *group1, int port1 )
{
#ifdef SOLARIS
    sigset_t   alrmset;
#endif

    char *group;
    int port;

    if( (int)group1 == RM_USE_CURRENT_CONFIG )
        group = rmcast_options.dest_ip;
    else
        group = group1;

    if(  (int)port1 == RM_USE_CURRENT_CONFIG )
        port = rmcast_options.dest_port;
    else
        port = port1;

#ifdef DEBUG_NET_MSGS
    fprintf( stderr, "DEBUG_NET_MSGS RM_joinGroup: %s initialized, will try to get the current state.\n", initialized?"":"not" );
#endif

    if( !initialized )
        RM_getCurStatus( group, port, NULL );

    /* creating the thread responsible for receiving packets */
    pthread_create( &rcv_thread, NULL, rmcastReceivePackets, (void*)local_user_info.socket );

    /* creating the signal handler thread */
    pthread_create( &signal_handler_thread, NULL, rmcastHandleSignals, NULL );

    return local_user_info.socket;
}

int RM_sendCurStatus( int connfd, char *buff, int buffsize )
{

/* FIXME: this routine may consume a lot of time
 *        it's necessary to implement it as a new thread or process
 */
    int n;
    char *packed_cache;
    int cache_size;

    write( connfd, &buffsize, sizeof(int) );
    n = write( connfd, buff, buffsize );
    if( n < 0 )
        perror( "RM_SendCurStatus ERROR: write error" );
#ifdef DEBUG_SHOW
    fprintf( stderr, "DEBUG_SHOW RM_SendCurStatus: Showing the cache...\n" );
    cacheShow( cache );
#endif

    cachePack( &cache, &packed_cache, &cache_size );

    write( connfd, &cache_size, sizeof(int) );
    n = write( connfd, packed_cache, cache_size );
    if( n < 0 )
        perror( "RM_SendCurStatus ERROR: write error" );

    close( connfd );

    return n;
}

void RM_getOption( int opt, void *return_value )
{
    switch( opt )
    {
        case NEW_MEMBER_SUPPORT:
            *((int *)return_value) = rmcast_options.new_member_support;
            break;
        case DEST_IP:
            strcpy( (char *)return_value, rmcast_options.dest_ip );
            break;
        case DEST_PORT:
            *((int *)return_value) = rmcast_options.dest_port;
            break;
        case TTL:
            *((int *)return_value) = rmcast_options.ttl;
            break;
        case MICROSLEEP:
            *((unsigned int *)return_value) = rmcast_options.microsleep;
            break;
        case TIMER_PARAM_A:
            *((int *)return_value) = rmcast_options.timer_paramA;
            break;
        case TIMER_PARAM_B:
            *((int *)return_value) = rmcast_options.timer_paramB;
            break;
        case TIMER_PARAM_C:
            *((int *)return_value) = rmcast_options.timer_paramC;
            break;
        case TIMER_PARAM_D:
            *((int *)return_value) = rmcast_options.timer_paramD;
            break;
        case TIMER_PARAM_E:
            *((int *)return_value) = rmcast_options.timer_paramE;
            break;
        case TIMER_PARAM_F:
            *((int *)return_value) = rmcast_options.timer_paramF;
            break;
        case HOSTS_IDENTIFIED:
            *((int *)return_value) = rmcast_options.hosts_identified;
            break;
        case TIMER_DISTRIBUTION:
            *((int *)return_value) = rmcast_options.timer_distribution;
            break;
        case MAX_NAK:
            *((int *)return_value) = rmcast_options.max_nak;
            break;
        case MAX_MEMBER_CACHE_SIZE:
            *((int *)return_value) = rmcast_options.max_member_cache_size;
            break;
        case RM_VERSION:
            *((int *)return_value) = rmcast_options.version;
            break;
        case TRANSMISSION_MODE:
            *((int *)return_value) = rmcast_options.transmission_mode;
            break;
        case LOG_FILE:
            strcpy( (char *)return_value, rmcast_options.log_file );
            break;
        case STATISTICS:
            *((int *)return_value) = rmcast_options.statistics;
            break;
        case REFRESH_TIMER:
            *((int *)return_value) = rmcast_options.refresh_timer;
            break;
        case LOSS_PROB:
            *((int *)return_value) = rmcast_options.loss_prob*100;
            break;
        case LEAVE_GROUP_WAIT_TIME:
            *((int *)return_value) = rmcast_options.leave_group_wait_time;
            break;
        case RCV_BUFFER_SIZE:
            *((int *)return_value) = rmcast_options.rcv_buffer_size;
            break;
        default:
            fprintf( stderr, "RM_getOption warning: Trying to get an invalid option: %d\n", opt );
            break;
    }

}

void RM_setOption( int opt, void *opt_value )
{
    char logfilename[ 255 ], localhost_name[ 127 ];
    char pid[ 16 ];

    switch( opt )
    {
        case PIPE:
            /* address of the pipe used to make the connection between the application and the multicast library */
            rmcast_options.pipe = (int*)opt_value;
            break;
        case SHUT_DOWN_ROUTINE:
            /* pointer to the routine called when shutting down the protocol */
            rmcast_options.shut_down_routine = (void (*)(void))opt_value;
            break;
        case DEST_IP:
            strcpy( rmcast_options.dest_ip , (char*)opt_value );
            break;
        case DEST_PORT:
            rmcast_options.dest_port = (int)opt_value ;
            break;
        case TTL:
            rmcast_options.ttl = (int)opt_value;
            break;
        case MICROSLEEP:
            rmcast_options.microsleep = (unsigned int)opt_value;
            break;
        case TIMER_PARAM_A:
            rmcast_options.timer_paramA = (int)opt_value;
            break;
        case TIMER_PARAM_B:
            rmcast_options.timer_paramB = (int)opt_value;
            break;
        case TIMER_PARAM_C:
            rmcast_options.timer_paramC = (int)opt_value;
            break;
        case TIMER_PARAM_D:
            rmcast_options.timer_paramD = (int)opt_value;
            break;
        case TIMER_PARAM_E:
            rmcast_options.timer_paramE = (int)opt_value;
            break;
        case TIMER_PARAM_F:
            rmcast_options.timer_paramF = (int)opt_value;
            break;
        case HOSTS_IDENTIFIED:
            rmcast_options.hosts_identified = (int)opt_value;
            break;
        case TIMER_DISTRIBUTION:
            rmcast_options.timer_distribution = (int)opt_value;
            break;
        case MAX_NAK:
            rmcast_options.max_nak = (int)opt_value;
            break;
        case MAX_MEMBER_CACHE_SIZE:
            rmcast_options.max_member_cache_size = (int)opt_value;
            break;
        case RM_VERSION:
            rmcast_options.version = (int)opt_value;
            break;
        case TRANSMISSION_MODE:
            rmcast_options.transmission_mode = (int)opt_value;
            break;
        case LOG_FILE:
            if( strcmp( (char*)opt_value, "NULL" ) == 0 )
                rmcast_options.log_file[ 0 ] = 0;
            else
            {
                gethostname( localhost_name, 30 );
                strcpy( logfilename, "");
                strcat( logfilename, (char*)opt_value );
                strcat( logfilename, "." );
                strcat( logfilename, localhost_name );
                strcat( logfilename, "." );
                sprintf( pid, "%d", (int)getpid() );
                strcat( logfilename, pid );
                strcpy ( rmcast_options.log_file, logfilename );
            }
            break;
        case NEW_MEMBER_SUPPORT:
            rmcast_options.new_member_support = (int)opt_value;
            break;
        case STATISTICS:
            rmcast_options.statistics = (int)opt_value;
            break;
        case REFRESH_TIMER:
            rmcast_options.refresh_timer = (int)opt_value;
            break;
        case LOSS_PROB:
            rmcast_options.loss_prob = (int)opt_value / (float)100;
            break;
        case LEAVE_GROUP_WAIT_TIME:
            rmcast_options.leave_group_wait_time = (int)opt_value;
            break;
        case RCV_BUFFER_SIZE:
            rmcast_options.rcv_buffer_size = (int)opt_value;
            break;
        default:
            fprintf( stderr, "RM_setOption warning: Trying to set and invalid option: %d\n", opt );
            break;
    }
}

/*******************************************************************************
 *
 * int    RM_setHostDelay(char *hostname, int estimated_one_way_delay )
 *
 * Updated the estimated one-way delay if host identified by 'hostname' exists
 * Try to insert a new entry if the host identfied by 'hostname' does not exist
 *
 * Parameters: hostname, the host name
 *             estimated_one_way_delay, the estimated one-way delay for 'hostname'
 *
 * Returns:  1 if the host delay was updated
 *           2 if the host delay was inserted as a new entry
 *          -1 if there is no room for another host in the rmcast_options.hosts_delays structure
 *          -2 if the IP address for 'hostname' was not found
 *
 ******************************************************************************/
int RM_setHostDelay( char *hostname, int estimated_one_way_delay )
{
    struct hostent *host_info;
    int i,retval = -1;
    char aux_host_ip[ IP_STRING_SIZE ];

    if( strcmp( hostname, "DEFAULT" ) == 0 )
    {
        rmcast_options.hosts_delays[ 0 ].estimated_delay = 1000 * estimated_one_way_delay;
        retval = 1;
    }
    else if( ( host_info = gethostbyname( hostname ) ) != NULL )
    {
        sprintf( aux_host_ip, "%s", inet_ntoa( *((struct in_addr*)(host_info->h_addr_list[ 0 ])) ) );
        i = 1;
        while( ( i <= rmcast_options.hosts_identified ) && ( retval != 1 ) )
        {
            if( strcmp( rmcast_options.hosts_delays[ i ].IP, aux_host_ip ) == 0 )
            {
                rmcast_options.hosts_delays[i].estimated_delay = 1000 * estimated_one_way_delay;
                retval = 1;
            }
            else
                i++;
        }
        if( ( retval == -1 ) && ( i < MAX_HOSTS ) )
        {
            sprintf( rmcast_options.hosts_delays[ i ].IP, "%s", aux_host_ip );
            rmcast_options.hosts_delays[ i ].estimated_delay = 1000 * estimated_one_way_delay;

            RM_setOption( HOSTS_IDENTIFIED, (void*)( rmcast_options.hosts_identified + 1 ));

            retval = 2;
        }
        else if( i >= MAX_HOSTS )
            retval = -2;
    }

    return retval;
}

/*******************************************************************************
 *
 * int    RM_getHostDelay(char *hostname, int *estimated_one_way_delay )
 *
 * Get the estimated_one_way_delay stored for the host identified by 'hostname' if it exists
 *
 * Parameters: hostname, the host name
 *             estimated_one_way_delaylowest_timer, the estimated one-way delay for the 'hostname'
 *
 * Returns:  1 if the host identified by 'hostname' exists
 *          -1 if the host identified by 'hostname' does not exist in the
 *             rmcast_options.hosts_delays structure
 *          -2 if the IP address for 'hostname' was not found
 *
 ************************************************************************************/
int RM_getHostDelay( char *hostname, int *estimated_one_way_delay )
{
    struct hostent *host_info;
    int i, retval = -1;
    char aux_host_ip[ IP_STRING_SIZE ];

    if( ( strcmp( hostname, "DEFAULT" ) == 0 ) || ( hostname == NULL ) )
    {
        *estimated_one_way_delay = ( rmcast_options.hosts_delays[ 0 ].estimated_delay / 1000 );
        retval = 1;
    }
    else if( ( host_info = gethostbyname( hostname ) ) != NULL )
    {
        sprintf( aux_host_ip, "%s",inet_ntoa( *((struct in_addr*)(host_info->h_addr_list[ 0 ])) ) );
        for( i = 1; i < MAX_HOSTS; i++ )
        {
            if( strcmp( rmcast_options.hosts_delays[ i ].IP, aux_host_ip ) == 0 )
            {
                *estimated_one_way_delay = ( rmcast_options.hosts_delays[ i ].estimated_delay / 1000 );
                retval = 1;
                break;
            }
        }
    }
    else
    {
        retval = -2;
    }

    return retval;
}

/*******************************************************************************
 *
 * void    RM_leaveGroup(int  sock,   char *group)
 *
 * Used to leave from the multicast group.
 *
 * Parameters: sock, the multicast socket;
 *               group, the multicast ip.
 *
 ******************************************************************************/
void RM_leaveGroup( int sock, char *group1 )
{
    char *group;

    if( (int)group1 == RM_USE_CURRENT_CONFIG )
        group = rmcast_options.dest_ip;
    else
        group = group1;

    rmcastLeaveGroup( sock, group );
    pthread_join( signal_handler_thread, NULL );
}

/*******************************************************************************
 *
 * void RM_terminate( void )
 *
 * Close logfile if needed and call the terminate callback function
 *
 ******************************************************************************/
void RM_terminate( void )
{
#ifdef DEBUG_NET_MSGS
    fprintf( stderr,"DEBUG_NET_MSGS RM_terminate\n" );
#endif
    if( logfile != NULL )
    {
        fprintf( logfile, "Sent_Packets: %d", sent_packets );
        fclose( logfile );
    }
    callback_terminate();
}

/*******************************************************************************
 *
 * int    RM_sendto     (int socket, char *buffer, int buffsize)
 *
 * Sends a message, contained in buffer, to the multicast group.
 *
 * Parameters: socket, the socket number;
 *               buffer, message to be sent;
 *               buffsize, maximum number of chars to be sent.
 *
 * Returns: 1, on success;
 *          0, on error.
 *
 ******************************************************************************/
int RM_sendto( int socket, void *buffer, int buffsize )
{
    PACKET_INFO pckt_info;
    BYTE* local_buffer = NULL;
    int final_message_size, retval;

    pckt_info.type  = DATA_PACKET_TYPE;
    pckt_info.flags = 0;
    strcpy( pckt_info.sender_id.ip, local_user_info.member_id.ip );
    pckt_info.sender_id.pid = local_user_info.member_id.pid;

    lock_eventlist_and_cache();

    pthread_mutex_lock( &change_local_user_sn );
    pckt_info.packet_data.data_packet.sn = local_user_info.sn;
    pthread_mutex_unlock( &change_local_user_sn );

    pckt_info.packet_data.data_packet.data_size = buffsize;

    pckt_info.packet_data.data_packet.data = (BYTE*)malloc( sizeof( char ) * buffsize + 1 );
    memcpy( (char*)( pckt_info.packet_data.data_packet.data ), buffer, buffsize );

    retval = cacheInsertMessage( &cache, &(pckt_info.sender_id), &(pckt_info.packet_data.data_packet) );

    unlock_eventlist_and_cache();

    if( retval == 0 )
    {
        fprintf( stderr, "RM_sendto ERROR: inserting message in local cache.\n" );
        return 0;
    }

    msgPcktMountMessage( &pckt_info, &local_buffer, &final_message_size );
#ifdef DEBUG_NET_MSGS
    fprintf( stderr, "DEBUG_NET_MSGS RM_sendto: Will send a message... \n" );
#endif

    if( rmcastSendPacket( socket, local_buffer, final_message_size ) == 0 )
        return 0;
    sent_packets++; /* counting sent packets */

#ifdef DEBUG_NET_MSGS
    fprintf( stderr, "DEBUG_NET_MSGS RM_sendto: Message sent ok!\n" );
#endif

#ifdef DEBUG_SHOW
    msgPcktShowMessage( local_buffer );
#endif

    free( local_buffer );
    return TRUE;
}

/*******************************************************************************
 *
 * int    RM_recv      (int socket, char *buffer, int buffsize)
 *
 * Waits to receive a message.
 *
 * Parameters: socket, the socket number;
 *               buffer, message received;
 *               buffsize, maximum number of chars to be received.
 *
 * Returns: number of bytes received, on success;
 *            -1, on error.
 *
 ******************************************************************************/
int RM_recv( int socket, void *buffer, int buffsize )
{
    struct q_entry r_entry = { 0 };
    int mlen = 0, size = 0, power = 0, ind = 0;

#ifdef DEBUG_NET_MSGS
    fprintf( stderr, "DEBUG_NET_MSGS RM_recv: Waiting to receive a \"reliable\" message...\n" );
#endif

    /* waiting to receive a ´reliable´ message trhough the message queue */
    if( ( mlen = msgrcv( r_qid, &r_entry, MAXOBN, 0, MSG_NOERROR ) ) == -1 )
    {
#ifdef DEBUG_NET_MSGS
        perror("DEBUG_NET_MSGS RM_recv: msgrcv failed");
#endif
        if( errno == EIDRM || errno ==  EINVAL )
            return -1;
    }
    else
    {
        /* a ´reliable´ message was received */
        size =0;

        /* convert mtext from base 256 to base 10 - mtext[0] is the least signif. digit of mtext */
        for( power = 1, ind = 0; ind < MSG_QUEUE_HEADER_SIZE;  power = (int)pow( 256, ind ) )
        {
            size += ((unsigned char)(r_entry.mtext[ ind ])) * power;

#ifdef DEBUG_NET_MSGS
            fprintf( stderr, "DEBUG_NET_MSGS RM_recv: power: %d r_entry.mtext[%d]=%d size=%d\n", power, ind, r_entry.mtext[ ind ], size );
#endif
            ind++;
        }

        if( size > buffsize )
        {
            fprintf( stderr, "RM_recv warning: Message greater than buffer. message size=%d, buffer size=%d\n", size, buffsize );
            return -1;
        }
        else
            memcpy( buffer, &( r_entry.mtext[ MSG_QUEUE_HEADER_SIZE ] ), size );

#ifdef DEBUG_NET_MSGS
        fprintf( stderr, "DEBUG_NET_MSGS RM_recv: A \"reliable\" message was received=%s!\n", (char*)buffer );
#endif
    }

#ifdef DEBUG_NET_MSGS
    fprintf( stderr, "DEBUG_NET_MSGS RM_recv: sending message to application. Message size=%d\n", size );
#endif

    return size;
}

/******************************************************************************/
void RMDEBUG_setpidip( int pid, char *ip )
{
    local_user_info.member_id.pid = pid;
    strncpy( local_user_info.member_id.ip, ip, IP_STRING_SIZE );
}

/******************************************************************************/
void RMDEBUG_setsn( int sn )
{
    pthread_mutex_lock( &change_local_user_sn );
    local_user_info.sn = sn;
    pthread_mutex_unlock( &change_local_user_sn );

}

void RM_getVersion( char *buf, int buf_sz )
{
    strncpy( buf, "RMCAST "STR_VERSION, buf_sz );
}

#endif
