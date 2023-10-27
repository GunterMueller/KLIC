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
                             rmcast.h
                             -------------------
    begin                : May 2001
    Authors              : Jorge Allyson Azevedo
                           Milena Scanferla
                           Daniel Sadoc
    email                : {allyson,milena,sadoc}@land.ufrj.br
 ***************************************************************************/

#ifndef RMCAST_H
#define RMCAST_H

#define RM_USE_CURRENT_CONFIG      -1

#define PIPE                    1 /* this option cannot figure in the config. file  */
#define TCP_IP                  2 /* this option cannot be modified by the user */
#define TCP_PORT                3 /* this option cannot be modified by the user */
#define SHUT_DOWN_ROUTINE       4 /* this option cannot figure in the config. file  */
#define CUR_STATE_SERVER_IS_UP  5 /* this option cannot be modified by the user */
#define DEST_IP                 6
#define DEST_PORT               7
#define TTL                     8
#define MICROSLEEP              9
#define TIMER_DISTRIBUTION      10
#define MAX_NAK                 11
#define MAX_MEMBER_CACHE_SIZE   12
#define RM_VERSION              13 /* still not used */
#define TRANSMISSION_MODE       14
#define LOG_FILE                15
#define NEW_MEMBER_SUPPORT      16
#define STATISTICS              17 /* still not used */
#define REFRESH_TIMER           18
#define LOSS_PROB               19
#define LEAVE_GROUP_WAIT_TIME   20
#define RCV_BUFFER_SIZE         21
#define HOSTS_IDENTIFIED        22
#define TIMER_PARAM_A           23
#define TIMER_PARAM_B           24
#define TIMER_PARAM_C           25
#define TIMER_PARAM_D           26
#define TIMER_PARAM_E           27
#define TIMER_PARAM_F           28

struct tagCurStatus
{
    int   size;
    char *data;
};

typedef struct tagCurStatus CurStatus;

int  RM_initialize( void *callbackterm( void ) );
int  RM_joinGroup( char *group, int port);
void RM_leaveGroup( int sock, char *group);
void RM_terminate( void );
int  RM_sendto( int socket, void *buffer, int buffsize );
int  RM_recv( int socket, void *buffer, int buffsize );
int  RM_getCurStatus( char *group, int port, CurStatus *current_status );
int  RM_sendCurStatus( int connfd, char *buff, int buffsize );
int  RM_readConfigFile( char *filename, char show_config_file );
void RM_getOption( int opt, void *return_value );
void RM_setOption( int opt, void *optvalue );
int  RM_setHostDelay( char *hostname, int estimated_one_way_delay );
int  RM_getHostDelay( char *hostname, int *estimated_one_way_delay );
void RMDEBUG_setpidip( int pid, char *ip );
void RMDEBUG_setsn( int sn );
void RM_getVersion( char *buf, int buf_sz );

#endif
