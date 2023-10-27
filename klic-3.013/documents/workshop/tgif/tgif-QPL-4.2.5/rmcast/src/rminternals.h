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
                             rminternals.h
                             -------------------
    begin                : May 2001
    Authors              : Jorge Allyson Azevedo
                           Milena Scanferla
                           Daniel Sadoc
    email                : {allyson,milena,sadoc}@land.ufrj.br
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#ifndef RMINTERNALS_H
#define RMINTERNALS_H

#define NUM_TIMERS 5  /* number of timers used by the program */

#define RECEIVE             'R'
#define SEND                'S'
#define RECEIVE_AND_LOSE    'L'

#include "rmstruct.h"
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/utsname.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "rmmsgpckt.h"
#include "rmevent.h"
#include "rmcache.h"
#include "rmmsg.h"

#define FOREVER while(1)

void *rmcastHandleSignals( void *arg );

int   rmcastCreateSocket( int port );

void  rmcastSetTTLValue( int soc, int ttl );
void  rmcastSetLoopBack( int soc, int loop );

void  rmcastJoinGroup( int soc, char *group_addr );
int   rmcastLeaveGroup( int soc, char *group_addr );

int   rmcastSendPacket( int soc, BYTE *message, int message_size );
void  *rmcastReceivePackets( void *soc );

int   rmcastCatchEvents( int i );

/****************************************************************************************************/

/* Routines To Facilitate the Interface with the Event List and the Cache */

int    rmcastRemoveEvent( PACKET_INFO *packt, int event_type );
int    rmcastFindEvent( PACKET_INFO *packt, int event_type );
int    rmcastCacheContains( PACKET_INFO *packt );
int    rmcastInsertEvent( PACKET_INFO *packt, int event_type );
void   rmcastProcessDataPacket( PACKET_INFO pckt_info );
void   rmcastProcessRefreshPacket( PACKET_INFO pckt_info );
double generate_uniform( void );

void lock_eventlist_and_cache();
void unlock_eventlist_and_cache();

void showLogMsg( char type, PACKET_INFO *pckt_info );

/****************************************************************************************************/

#define MAX_HOSTS 10 /* Maximum number of hosts which timers will be stored */

struct SHOSTS_DELAYS
{
    char IP[IP_STRING_SIZE];   /* Host IP */
    int estimated_delay;       /* Estimated one-way delay to host */
};

typedef struct SHOSTS_DELAYS HOSTS_DELAYS;

struct SGLOBAL_OPTIONS
{

    int        *pipe;
    char       tcp_ip[ IP_STRING_SIZE ];
    int        tcp_port;
    void       (*shut_down_routine)(void);
    int        cur_state_server_is_up;
    char       dest_ip[IP_STRING_SIZE]; /* this is the destination ip used in the connection to the multicast group (or unicast destination host) */
    int        dest_port;
    int        ttl;
    unsigned int microsleep;
    int        hosts_identified;
    int        timer_distribution;
    int        max_nak; /* max number of NAKs that can be sent for each sn */
/*
 * We will be able to retransmit the last max_member_cache_size packets from each member of the multicast group,
 * i.e., we will store the last max_member_cache_size packets from each member of the multicast group in the cache.
 */
    int        max_member_cache_size;
    int        version;
    int        transmission_mode;
    char       log_file[ 255 ];
    int        new_member_support;
    int        statistics;
    int        refresh_timer;
    float      loss_prob;
    int        leave_group_wait_time;
    int        rcv_buffer_size;
    HOSTS_DELAYS hosts_delays[ MAX_HOSTS ];
    int        timer_paramA;
    int        timer_paramB;
    int        timer_paramC;
    int        timer_paramD;
    int        timer_paramE;
    int        timer_paramF;
};

typedef struct SGLOBAL_OPTIONS GLOBAL_OPTIONS;

/****************************************************************************************************/

struct SUSER_INFO
{
	struct TIMER
	{
		int min,max;
	} timer[ NUM_TIMERS ];

	struct GROUP_ID
	{
		char ip[ IP_STRING_SIZE ]; /* ip address and... */
		int port;    /* port of the group  */
	} group_id;

	MEMBER_ID member_id;
	int cache_size;
	int sn;
	int socket;
};

typedef struct SUSER_INFO USER_INFO;

/****************************************************************************************************/

#endif
