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
                             rmstruct.h
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

#ifndef MSTRUCT_H

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#ifdef SOLARIS
#include <strings.h>
#endif

#ifndef SOLARIS        
#define SOCKLEN_TYPE       (socklen_t*)
#else
#define SOCKLEN_TYPE       
#endif

#define MSTRUCT_H

#ifdef SINGLE_NACK
    #define MAX_WINDOW_SIZE 64
#endif

/* Transmission modes */
#define MULTICAST 0
#define UNICAST 1

/* Packet types */
#define DATA_PACKET_TYPE     		1
#define RETRANSM_PACKET_TYPE 		2
#define NAK_PACKET_TYPE      		3
#define REFRESH_PACKET_TYPE         4
#define JOIN_REQUEST_PACKET_TYPE    5
#define JOIN_ACCEPT_PACKET_TYPE     6
#define LEAVE_GROUP_PACKET_TYPE     7

#define IP_STRING_SIZE  16

/* Member status in the cache */
#define CACHE_STATUS_DEACTIVE 0
#define CACHE_STATUS_ACTIVE   1
#define CACHE_STATUS_ANY      2

#ifndef TRUE

    #define TRUE  1
    #define FALSE 0

#endif

#ifdef RANDOM_TIMERS
/* Distributions available for the random timers */
    #define UNIFORM     0
    #define EXPONENTIAL 1
#endif

typedef unsigned char BYTE;


/*************** members info *****************************************/

struct SMEMBER_ID
{
    char ip[IP_STRING_SIZE]; /* ip address and... */
    int pid;     /* process id of the member */

};

typedef struct SMEMBER_ID MEMBER_ID;

struct SMEMBER_STATUS
{
    int first_rcv;    /* first packet received from a specific member */
    int last_rcv;     /* last packet received from a specific member */
    int last_seq_rcv; /* last packet received in the correct order 
                         from a specific member. Please, note that this
                         is also used as the base for the sliding window
                         used to restrict the sending of NACKs.  
                         See documentation for more details. */  
    int last_identified; /* last packet received, or to which a nack was sent */
    
#ifdef SINGLE_NACK
    int window_size;
    int window_mask[MAX_WINDOW_SIZE]; 
    int window_ini;
#endif
    
};

typedef struct SMEMBER_STATUS MEMBER_STATUS;


/************** session member information ***************************/

struct SSM_INFO 
{
    MEMBER_ID member_id;
    MEMBER_STATUS member_status;
};

typedef struct SSM_INFO  SM_INFO ;


/**************** data packet info *************************************/

struct SDATA_PACKET
{
    int sn;        /* sequence number */
    int data_size; /* number of data bytes */
    BYTE *data;    /* data to be sent or data received */
};

typedef struct SDATA_PACKET DATA_PACKET;


/**************** cache info ******************************************/

struct SCACHE_NODE
{
    DATA_PACKET data_pckt;
    struct SCACHE_NODE *next, *previous;  /* pointers to next and previous nodes */
                                                 
};

typedef struct SCACHE_NODE CACHE_NODE;

struct SNAK_LIST
{
    int sn;
    int counter;
    struct SNAK_LIST *previous;
    struct SNAK_LIST *next;
};

typedef struct SNAK_LIST NAK_LIST;

struct SCACHE
{
    int number_of_nodes; /* number of nodes currently stored for a specific member */
    
    int active;          /* active == 0 means the member has left the multicast group */
    
    SM_INFO sm_info;

    CACHE_NODE *first,         /* pointer to the first of the packets stored for a specific member */
               *last,          /* pointer to the last of the packets stored for a specific member */
               *last_inserted; /* pointer to the last of the packets inserted for a specific member */
    
    NAK_LIST *nak_list; /* Pointer to nak list */
    
    struct SCACHE *next;
    
};

typedef struct SCACHE CACHE;

/*************** events information**************************************/

struct SEVENT_LIST
{
    MEMBER_ID *member_id;
    
    char action;       /* action to be executed */
     
    long timer_value;  /* time when this action will be executed */

    int sn;            /* sequence number */
    
    struct timeval last_update_time; /* Last time when the timer was updated */
                         
    struct SEVENT_LIST *next;
};

typedef struct SEVENT_LIST EVENT_LIST;

/*************** packet information**************************************/

struct SRETRANSM_PACKET
{
    MEMBER_ID original_sender_id;
    DATA_PACKET data_packet;
};

typedef struct SRETRANSM_PACKET RETRANSM_PACKET; 


struct SNAK_PACKET
{
    MEMBER_ID requested_member_id;
    int sn;
#ifdef SINGLE_NACK
    int base_sn;
    int window_size;
    int hmask, lmask; /* higher and lower parts of the mask, respectively */
#endif
};

typedef struct SNAK_PACKET NAK_PACKET;



struct SREFRESH_PACKET
{
	int sn_of_last_msg_sent;	/* sequence number of last message sent */
};


typedef struct SREFRESH_PACKET REFRESH_PACKET;



struct SJOIN_ACCEPT_PACKET
{
	int port;	
};


typedef struct SJOIN_ACCEPT_PACKET JOIN_ACCEPT_PACKET;


struct SPACKET_INFO
{
    char type; /* the type of the packet */
    
    BYTE flags; /* flags field */
    
    MEMBER_ID  sender_id; /* id of the sender of the packet */
    
    int packet_size; /* the size, in bytes, of the packet */

    union PACKET_DATA
    {
            DATA_PACKET data_packet;
            RETRANSM_PACKET retransm_packet;
            NAK_PACKET nak_packet;
            REFRESH_PACKET	refresh_packet;
            JOIN_ACCEPT_PACKET join_accept_packet;
    } packet_data;
};

typedef struct SPACKET_INFO PACKET_INFO; 

/************************************************************************/

#endif


