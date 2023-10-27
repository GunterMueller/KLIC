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
                             rmmsgpckt.c
                             -------------------
    begin                : May 2001
    Authors              : Jorge Allyson Azevedo
                           Milena Scanferla
                           Daniel Sadoc
    email                : {allyson,milena,sadoc}@land.ufrj.br
 ***************************************************************************/

#ifndef RMMSGPCKT_C

#define RMMSGPCKT_C

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ctype.h>

#include "rmstruct.h"
#include "rmmsgpckt.h"

#define MIN(a,b)  ((a<b)?a:b) 

#define get(dest, org, bytes) { bzero(dest,bytes); memcpy(dest,org, bytes); org+=bytes; }

#define getPCKInfo(dest, org,bytes) { bzero(dest,bytes); memcpy(dest,org, bytes); dest+=bytes; packet_size+=bytes; }

#define get_member_id(member_id, message)			\
{													\
	fprintf(stderr,"\tMember id:\n");				\
	get(&member_id,message,sizeof(MEMBER_ID));		\
	fprintf(stderr,"\t\tip: "); 					\
	for (k=0; k<IP_STRING_SIZE; k++)			\
	{												\
	if ((int)(member_id.ip[k])==0) break;			\
	fprintf(stderr,"%c",member_id.ip[k]);			\
	}												\
	fprintf(stderr,"\n");							\
	fprintf(stderr,"\t\tpid: %d\n",member_id.pid);	\
}													\

extern int rmcast_listening_tcp_port;
	
char *MESSAGE_TYPE_NAME[NUMBER_OF_MESSAGE_TYPES] = {  
                                "UN", /* "UNKNOWN PACKET TYPE" */
                                "DT", /* "DATA PACKET        ", */
                                "RT", /* "RETRANSMISSION PACKET ", */
                                "NK", /* "NAK PACKET         ", */
                                "RF", /*"REFRESH PACKET     " */ 
                                "JR", /*"JOIN REQUEST PACKET     " */ 
                                "JA", /*"JOIN ACCEPT PACKET     " */ 
                                "LG" /* "LEAVE GROUP PACKET" */
                                };


#define EXCHANGE_BYTE_ORDER(in_var)                                             \
    {                                                                           \
        int i = 0, j=sizeof(in_var)-1;                                          \
        unsigned char * var = (unsigned char*)&in_var;                          \
        char temp = 0;                                                          \
        for (i=0;i<j;i++, j--)                                                  \
        {                                                                       \
            temp       = var[ i ];                                              \
            var[ i ]   = var[ j ];                                              \
            var[ j ] = temp;                                                    \
        }                                                                       \
    }



/*******************************************************************************************************
 *
 * void msgPcktShowMessage(BYTE *message)
 *
 * Shows the message on the screen.
 *
 ******************************************************************************************************/

// #define DEBUG_SHOW
// #define DEBUG_MSG_CONTENTS

void msgPcktShowMessage(BYTE *message)
{

#ifdef DEBUG_SHOW

	char type;
    BYTE flags;	
	
	MEMBER_ID member_id;  char string[1001];
	
	int sn, pckt_size, data_size, k, port;

#ifdef DEBUG_MSG_CONTENTS   
    int i;
#endif
	
	get(&type,message,sizeof(char));

	fprintf(stderr,"DEBUG_SHOW msgPcktShowMessage BEGIN -------------------\n");
    fprintf(stderr,"\ntype: %d\n",type);
	
    get(&flags,message,sizeof(BYTE));
    fprintf(stderr,"\nflags: %d\n",flags);
    fprintf(stderr,"member info: \n");

	get_member_id(member_id, message);
    
#ifdef SOLARIS
    EXCHANGE_BYTE_ORDER(member_id.pid);
    
    fprintf(stderr,"SOLARIS byte order pid #: %d\n",member_id.pid);
#endif
    
    
	get(&pckt_size,message,sizeof(int));  
    
    
#ifdef SOLARIS
    EXCHANGE_BYTE_ORDER(pckt_size);
#endif
    

    fprintf(stderr,"packet size: %d\n",pckt_size);

	
	switch (type)
	{
		
	case DATA_PACKET_TYPE:
    
		fprintf(stderr,"data packet: \n");

		get(&sn,message,sizeof(int)); 

#ifdef SOLARIS
        EXCHANGE_BYTE_ORDER(sn);
#endif

        fprintf(stderr,"\tsn: %d\n",sn);

		get(&data_size,message,sizeof(int)); 

#ifdef SOLARIS
        EXCHANGE_BYTE_ORDER(data_size);
#endif

        fprintf(stderr,"\tdata size: %d\n",data_size);

		memcpy(string,message, MIN((sizeof(BYTE)*data_size),1000)); string[MIN(data_size,1000)]='\0'; 

		fprintf(stderr,"\tdata: ");

#ifdef DEBUG_MSG_CONTENTS
		for(i=0; i<data_size; i++)
		{
     		fprintf(stderr,"%c",string[i]);
		}
#endif        
		break;
		
	case RETRANSM_PACKET_TYPE: 
		
		get_member_id(member_id, message);
        
#ifdef SOLARIS
        EXCHANGE_BYTE_ORDER(member_id.pid);
    
        fprintf(stderr,"SOLARIS byte order pid #: %d\n",member_id.pid);
#endif
        
        get(&sn,message,sizeof(int)); 
		get(&data_size,message,sizeof(int)); 
		memcpy(string,message, sizeof(BYTE)*data_size); 
        string[data_size]='\0'; 

        fprintf(stderr,"retransmition packet: \n"); 
		fprintf(stderr,"\tdata packet: \n");
        
#ifdef SOLARIS
        EXCHANGE_BYTE_ORDER(sn);
        EXCHANGE_BYTE_ORDER(data_size);        
#endif
        
        fprintf(stderr,"\t\tsn: %d\n",sn);
        fprintf(stderr,"\t\tdata size: %d\n",data_size);
 		fprintf(stderr,"\tdata: %s\n",string);

			
#ifdef DEBUG_MSG_CONTENTS
		for(i=0; i<data_size; i++)
		{
     		fprintf(stderr,"%c",string[i]);
		}
#endif
       
		fprintf(stderr,"\n\n");


	break;
		
	case NAK_PACKET_TYPE:
		
		get_member_id(member_id, message);	

#ifdef SOLARIS
        EXCHANGE_BYTE_ORDER(member_id.pid);
    
        fprintf(stderr,"SOLARIS byte order pid #: %d\n",member_id.pid);
#endif
        
		
		get(&sn,message,sizeof(int)); 
                
        fprintf(stderr,"nak packet: \n");
        
        fprintf(stderr,"\tsn: %d\n",sn);

#ifdef SOLARIS
        EXCHANGE_BYTE_ORDER(sn);
#endif  

#ifdef  SINGLE_NACK       
    
        {
            int base_sn, window_size, hmask, lmask;
            
    		get(&(base_sn), message, sizeof(int));	
	    	get(&(window_size),message, sizeof(int));	        
		    get(&(hmask), message, sizeof(int));	        
    		get(&(lmask), message, sizeof(int));	        

#ifdef SOLARIS
            EXCHANGE_BYTE_ORDER(base_sn);
            EXCHANGE_BYTE_ORDER(window_size);
            EXCHANGE_BYTE_ORDER(hmask);
            EXCHANGE_BYTE_ORDER(lmask);
#endif  
            
            fprintf(stderr,"\tbase sn: %d window size: %d \n\thigher part of the mask: %d lower part of the mask: %d\n", 
                base_sn, window_size, hmask, lmask);
        }
        
#endif        // single nack


    break;
		
		
	case LEAVE_GROUP_PACKET_TYPE:

		fprintf(stderr,"leave group packet: \n");

		break;
        
    case REFRESH_PACKET_TYPE:
    
		get(&sn,message,sizeof(int)); 

#ifdef SOLARIS
        EXCHANGE_BYTE_ORDER(sn);
#endif  
        
    	
        fprintf(stderr, "\tsequence number of last message sent: %d\n", sn);
    
		break;
        
	case JOIN_ACCEPT_PACKET_TYPE: 
    
  		get(&port,message,sizeof(int)); 

#ifdef SOLARIS
        EXCHANGE_BYTE_ORDER(port);
#endif  
        
		
  		fprintf(stderr,"join accept packet (port: %d). \n",port);
        
        break;
        
	case JOIN_REQUEST_PACKET_TYPE:         

        break;

    default:
    
        fprintf(stderr,"Unknown packet type: %d\n",type);
        
        
	}

	fprintf(stderr,"DEBUG_SHOW msgPcktShowMessage END -------------------\n");
#endif

	return;
}


/*******************************************************************************************************
 *
 * void msgPcktExchangeByteOrder(PACKET_INFO *pckt_info)
 *
 * Exchange byte order in message (Linux versus Solaris).
 *
 * Arguments:	pckt_info,	a pointer to the strucutre holding the message.
 *
 ******************************************************************************************************/

#ifdef SOLARIS

void msgPcktExchangeByteOrder(PACKET_INFO *pckt_info)
{
    EXCHANGE_BYTE_ORDER(pckt_info->sender_id.pid);
    EXCHANGE_BYTE_ORDER(pckt_info->packet_size);    
    switch (pckt_info->type)
    {
    case DATA_PACKET_TYPE:

        EXCHANGE_BYTE_ORDER(pckt_info->packet_data.data_packet.sn);
        EXCHANGE_BYTE_ORDER(pckt_info->packet_data.data_packet.data_size);
        break;

    
	case RETRANSM_PACKET_TYPE: 
        

        EXCHANGE_BYTE_ORDER(pckt_info->packet_data.retransm_packet.original_sender_id.pid);
        EXCHANGE_BYTE_ORDER(pckt_info->packet_data.retransm_packet.data_packet.sn);        
        EXCHANGE_BYTE_ORDER(pckt_info->packet_data.retransm_packet.data_packet.data_size);
        break;

    
	case NAK_PACKET_TYPE:    
    
        EXCHANGE_BYTE_ORDER(pckt_info->packet_data.nak_packet.requested_member_id.pid);
        EXCHANGE_BYTE_ORDER(pckt_info->packet_data.nak_packet.sn);
        
#ifdef  SINGLE_NACK                

        EXCHANGE_BYTE_ORDER(pckt_info->packet_data.nak_packet.base_sn);
        EXCHANGE_BYTE_ORDER(pckt_info->packet_data.nak_packet.window_size);
        EXCHANGE_BYTE_ORDER(pckt_info->packet_data.nak_packet.hmask);
        EXCHANGE_BYTE_ORDER(pckt_info->packet_data.nak_packet.lmask);
        
#endif
        
        break;                
    
    case REFRESH_PACKET_TYPE:    

        EXCHANGE_BYTE_ORDER(pckt_info->packet_data.refresh_packet.sn_of_last_msg_sent);
        break;
    
	case LEAVE_GROUP_PACKET_TYPE:     
        break;
    
	case JOIN_ACCEPT_PACKET_TYPE:     
        EXCHANGE_BYTE_ORDER(pckt_info->packet_data.join_accept_packet.port);
        break;
    
	case JOIN_REQUEST_PACKET_TYPE:             
        break;
    }
}

#endif

/*******************************************************************************************************
 *
 * void msgPcktUnmountMessage(PACKET_INFO *pckt_info, BYTE *message)
 *
 * Unmount the message come from the network, filling PACKET_INFO structure with the apropriate values.
 *
 * Arguments:	message,	a pointer to the message to be unmounted (source of information);
 *				pckt_info,	a pointer to the target strucutre, which will hold the message info.
 *							(this structure must have been previously allocated).
 *
 ******************************************************************************************************/


void msgPcktUnmountMessage(PACKET_INFO *pckt_info, BYTE *message)
{
#ifdef DEBUG_PCKT
    fprintf(stderr,"DEBUG_PCKT msgPcktUnmountMessage: [char: %d flag: %d MEMBER_ID: %d int: %d]\n",
        sizeof(char),
        sizeof(BYTE),     /* Flags related */
        sizeof(MEMBER_ID),
        sizeof(int));
#endif    
	
	get(&(pckt_info->type),message,sizeof(char));

    /* Flags related */
    get(&(pckt_info->flags),message,sizeof(BYTE));
	
    get(&(pckt_info->sender_id),message,sizeof(MEMBER_ID));
	get(&(pckt_info->packet_size),message,sizeof(int));
	
	switch (pckt_info->type)
	{
	case DATA_PACKET_TYPE:
		
		get(&(pckt_info->packet_data.data_packet.sn),message,sizeof(int)); 
    
    	get(&(pckt_info->packet_data.data_packet.data_size),message,sizeof(int));
		(pckt_info->packet_data.data_packet.data)=(BYTE*)message;
		break;
		
	case RETRANSM_PACKET_TYPE: 
		
		get(&(pckt_info->packet_data.retransm_packet.original_sender_id),message,sizeof(MEMBER_ID));
		get(&(pckt_info->packet_data.retransm_packet.data_packet.sn),message,sizeof(int));
		get(&(pckt_info->packet_data.retransm_packet.data_packet.data_size),message,sizeof(int));
		(pckt_info->packet_data.retransm_packet.data_packet.data) = (BYTE*)message;
		break;
		
	case NAK_PACKET_TYPE:
		
		get(&(pckt_info->packet_data.nak_packet.requested_member_id),message,sizeof(MEMBER_ID));	
        get(&(pckt_info->packet_data.nak_packet.sn),message,sizeof(int));	
        
#ifdef  SINGLE_NACK        

		get(&(pckt_info->packet_data.nak_packet.base_sn), message, sizeof(int));	
		get(&(pckt_info->packet_data.nak_packet.window_size),message, sizeof(int));	        
		get(&(pckt_info->packet_data.nak_packet.hmask), message, sizeof(int));	        
		get(&(pckt_info->packet_data.nak_packet.lmask), message, sizeof(int));	        
        
#endif        
        
		break;
		
#ifdef REFRESH
    
    case REFRESH_PACKET_TYPE:
    
    	get(&(pckt_info->packet_data.refresh_packet.sn_of_last_msg_sent), message, sizeof(int));
        break;
#endif
		
	case LEAVE_GROUP_PACKET_TYPE: 
		
		/* do nothing. We already have the sender id*/
		
		break;
        
	case JOIN_ACCEPT_PACKET_TYPE: 
    
       	get(&(pckt_info->packet_data.join_accept_packet.port), message, sizeof(int));
		break;

	case JOIN_REQUEST_PACKET_TYPE:         
        
        break;
        
    default:
    
        fprintf(stderr,"Unknown packet type: %d\n",pckt_info->type);
        
   
        break;
        
	}
#ifdef SOLARIS    

    fprintf(stderr,"SOLARIS msgPcktUnmountMessage: That's SOLARIS so, excanching by order:\n[sender pid] ");
    fprintf(stderr,"%d %x\n",pckt_info->sender_id.pid,pckt_info->sender_id.pid);

    msgPcktExchangeByteOrder(pckt_info);

    fprintf(stderr,"SOLARIS msgPcktUnmountMessage: [sender pid]: %d %x\n",pckt_info->sender_id.pid,pckt_info->sender_id.pid);
    
#endif    

#ifdef DEBUG_PCKT
    fprintf(stderr,"DEBUG_PCKT msgPcktUnmountMessage: returning\n");
#endif    

	return; 
    
}

/*******************************************************************************************************
 *
 * void msgPcktMountMessage(PACKET_INFO *pckt_info, BYTE **message, int *msgsize)
 *
 * Mount the message to be sent over the network, getting data from the PACKET_INFO structure.
 *
 * Arguments:	pckt_info,	a pointer to the source structure, from which data will be retrieved;
 *				message,	a pointer to the message to be mounted - 
 *							(the space needed to store the message will be allocated as necessary);
 *				msgsize,	a pointer to an integer which will return the effective size of the message.
 *
 ******************************************************************************************************/

void msgPcktMountMessage(PACKET_INFO *pckt_info, BYTE **message, int *msgsize)
{
	BYTE *aux_message_pt;
	BYTE *common_header;
	
	int *sn_pointer;
	int common_header_size;
	int msg_specific = 0, data_size = 0;
	int packet_size = 0;
    
#ifdef DEBUG_PCKT
	fprintf(stderr,"DEBUG_PCKT msgPcktMountMessage: mounting packet\n");
#endif

#ifdef SOLARIS
    fprintf(stderr,"That's SOLARIS so, excanching by order:\n[sender pid] ");
    fprintf(stderr,"%d %x\n",pckt_info->sender_id.pid,pckt_info->sender_id.pid);

    msgPcktExchangeByteOrder(pckt_info);
    
#endif

#ifdef DEBUG_PCKT
    fprintf(stderr,"DEBUG_PCKT msgPcktMountMessage: [sender pid] ");
    fprintf(stderr,"%d %x\n",pckt_info->sender_id.pid,pckt_info->sender_id.pid);
#endif    

	
	/* info about type of the message, sender_id and packet_size */
    common_header_size= sizeof(char)+sizeof(BYTE)+sizeof(MEMBER_ID)+sizeof(int);     /* sizeof(BYTE) is Flags related */
	common_header = (BYTE*)malloc(common_header_size);
	
	aux_message_pt = common_header;

#ifdef DEBUG_PCKT
	fprintf(stderr,"DEBUG_PCKT msgPcktMountMessage:common_header allocated at: %p\n", common_header);
#endif	  
	
    /* Flags are not used by now, so we just set it to zero */
    pckt_info->flags='0';
    
	getPCKInfo(aux_message_pt, &(pckt_info->type), sizeof(char));
    
    /* Flags related */
	getPCKInfo(aux_message_pt, &(pckt_info->flags), sizeof(BYTE));
    
    getPCKInfo(aux_message_pt, &(pckt_info->sender_id), sizeof(MEMBER_ID));
	getPCKInfo(aux_message_pt, &(pckt_info->packet_size), sizeof(int));
  
	packet_size = 0;
	
	/* specific info plus data */
	
	switch(pckt_info->type)
	{
		
	    case DATA_PACKET_TYPE: 

		    msg_specific = sizeof(DATA_PACKET)-sizeof(BYTE*);
		    data_size = (pckt_info)->packet_data.data_packet.data_size;

		    (*message) = (BYTE*)malloc(common_header_size+msg_specific+data_size);
		    aux_message_pt = *message;
		    getPCKInfo(aux_message_pt,common_header,common_header_size);

		    sn_pointer = (int*)aux_message_pt;

		    getPCKInfo(aux_message_pt,&(pckt_info->packet_data.data_packet.sn), sizeof(int));

		    getPCKInfo(aux_message_pt,&(pckt_info->packet_data.data_packet.data_size), sizeof(int));
		    getPCKInfo(aux_message_pt, (pckt_info->packet_data.data_packet.data), data_size);


		    break;

	    case RETRANSM_PACKET_TYPE: 

		    msg_specific = sizeof(RETRANSM_PACKET)-sizeof(BYTE*);


		    data_size = sizeof(BYTE)*(pckt_info)->packet_data.retransm_packet.data_packet.data_size;


		    (*message) = (BYTE*)malloc(common_header_size+msg_specific+data_size);
		    aux_message_pt = *message;
		    getPCKInfo(aux_message_pt,common_header,common_header_size);


		    getPCKInfo(aux_message_pt,(void*)(&(pckt_info->packet_data.retransm_packet)), msg_specific);
		    getPCKInfo(aux_message_pt,(pckt_info->packet_data.retransm_packet.data_packet.data), data_size);		   

		    break;

	    case NAK_PACKET_TYPE:

		    (*message) = (BYTE*)malloc(common_header_size+sizeof(NAK_PACKET));
		    aux_message_pt = *message;
		    getPCKInfo(aux_message_pt,common_header,common_header_size);
		    getPCKInfo(aux_message_pt, &(pckt_info->packet_data.nak_packet.requested_member_id), sizeof(MEMBER_ID));        
		    getPCKInfo(aux_message_pt, &(pckt_info->packet_data.nak_packet.sn), sizeof(int));

    #ifdef  SINGLE_NACK        

		    getPCKInfo(aux_message_pt, &(pckt_info->packet_data.nak_packet.base_sn), sizeof(int));	
		    getPCKInfo(aux_message_pt, &(pckt_info->packet_data.nak_packet.window_size), sizeof(int));	        
		    getPCKInfo(aux_message_pt, &(pckt_info->packet_data.nak_packet.hmask), sizeof(int));	        
		    getPCKInfo(aux_message_pt, &(pckt_info->packet_data.nak_packet.lmask), sizeof(int));	        

    #endif        

		    break;


    #ifdef REFRESH

        case REFRESH_PACKET_TYPE:

		    (*message) = (BYTE*)malloc(common_header_size+sizeof(int));
		    aux_message_pt = *message;
		    getPCKInfo(aux_message_pt,common_header,common_header_size);

            getPCKInfo(aux_message_pt, &(pckt_info->packet_data.refresh_packet.sn_of_last_msg_sent), sizeof(int));

		    break;    

    #endif

	    case LEAVE_GROUP_PACKET_TYPE:



   		    (*message) = (BYTE*)malloc(common_header_size);
		    aux_message_pt = *message;
		    getPCKInfo(aux_message_pt,common_header,common_header_size);


		    /* do nothing. The packet is already built */

		    break;	

	    case JOIN_ACCEPT_PACKET_TYPE:    

		    (*message) = (BYTE*)malloc(common_header_size+sizeof(int));
		    aux_message_pt = *message;
		    getPCKInfo(aux_message_pt,common_header,common_header_size);

            getPCKInfo(aux_message_pt, &(pckt_info->packet_data.join_accept_packet.port), sizeof(int));

            break;

        case JOIN_REQUEST_PACKET_TYPE:  
            (*message) = (BYTE*)malloc(common_header_size);
            aux_message_pt = *message;

		    getPCKInfo(aux_message_pt,common_header,common_header_size);

            break;

        default:

            fprintf(stderr,"DEBUG_PCKT msgPcktMountMessage: Unknown packet type: %d\n",pckt_info->type);
	}

	
	packet_size++;
	
	aux_message_pt =  *message + sizeof(char) + sizeof(BYTE) + sizeof(MEMBER_ID);
	memcpy(aux_message_pt, &packet_size,sizeof(int));
	
	*msgsize = packet_size;
    
    
#ifdef DEBUG_PCKT
    {
        int cont;
        fprintf(stderr,"DEBUG_PCKT msgPcktMountMessage: Mounted message: \n");
        for (cont = 0; cont < packet_size; cont++)
        {
            fprintf(stderr,"%d ", ((signed char*)(*message))[cont]);
        }	
               
    }
    fprintf(stderr,"\nhere\n");
    
    msgPcktShowMessage((BYTE*)*message) ;
#endif 
    
   
    free (common_header);
	
}
#endif


