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
                             rmevent.c
                             -------------------
    begin                : May 2001
    Authors              : Jorge Allyson Azevedo
                           Milena Scanferla
                           Daniel Sadoc
    email                : {allyson,milena,sadoc}@land.ufrj.br
 ***************************************************************************/

#ifndef RMEVENT_C
#define RMEVENT_C

#include <signal.h>
#include <stdlib.h>
#include <sys/time.h> 
#include <unistd.h>
#include <errno.h>
#include <pthread.h>


#include "rmstruct.h"
#include "rmevent.h"
#include "rminternals.h"
#include "rmwinmask.h"

#include "rmcast.h"

/*---------------- Global variables ------------------------------------------------------------------*/

#ifdef DSUPPRESSION
    extern FILE *suppressionfile;
#endif
extern pthread_t rcv_thread, signal_handler_thread;

extern GLOBAL_OPTIONS rmcast_options;

#ifndef RANDOM_TIMERS
    static long DEFAULT_TIMER_VALUE [7] = {     0,
                                                1000000,
                                                2000000,
                                                1000000,
                                                10000000,
                                                5000000,
                                                0 };
#endif
                                            
char *EVENT_ACTION_NAME[9] =          { "ZERO_ACTION", 
                                        "NAK_SND_WAIT", 
                                        "RET_RCV_WAIT", 
                                        "RET_SND_WAIT",
                                        "REF_SND_WAIT",
                                        "LEV_GRP_WAIT",
                                        "SUPPRESSED_NAK",
                                        "RET_RCV_EXPIRED",
                                        "UNKNOWN"};

#ifdef SOLARIS
pthread_mutex_t   event_list_mutex;
#else
pthread_mutex_t   event_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

extern CACHE *cache;
extern USER_INFO  local_user_info;

/*
 * inline means that the precompiler will replace the calling of getRemaining time with the
 * code of the function, i.e., the function will not exist in compile time
 */
inline void getRemainingTime(EVENT_LIST **el, long double*remaining_time, char *msg);

EVENT_LIST * eventListFind2(EVENT_LIST **el1,MEMBER_ID *member_id, char action, int sn, EVENT_LIST **antNode);

extern GLOBAL_OPTIONS rmcast_options;

/*---------------- Auxiliary functions to manipulate the timer ---------------------------------------*/


/***************************************************************************************
 *
 * float generateSample(char distribution)
 *
 * Generates a random sample.
 *
 * Arguments:	distribution, the distribution of the random var (not yet implemented).
 *
 * Returns: 	a random number between 0 and 1.
 *
 ***************************************************************************************/

float generateSample(char distribution)
{
    double sample=0,uniform=0;
    double e,lambda;
    
    switch(distribution)
    {
        case UNIFORM:
        
            sample=((double)random()/(double)RAND_MAX);
        
            break;
        
        case EXPONENTIAL:
        
            e = exp( 1 );
            
            /* Lambda must be log(number of members of the multicast group) + 1 */
            lambda = log( cacheCountMembers( cache,CACHE_STATUS_ACTIVE ) ) + 1;
            uniform = ((double)random()/(double)RAND_MAX);
            sample = -(log(uniform)/lambda);
            
            break;
            
        default:
        
            fprintf(stderr,"generateSample ERROR: unknown distribution=%c",distribution);
    }
    return ( sample );
}

/***************************************************************************************
 *
 * int generateTimerValue(MEMBER_ID member_id, char action, char distribution)
 *
 * Generates a timer value corresponding to the defined action.  This value indicates
 * in how many microseconds the action will be executed.
 *
 * Arguments:	member_id, member id of the member related to the event
 *              action, the action to be executed.
 *				distribution, the distribution of the random var (not yet implemented).
 *
 * Returns: 	the timer value, in microseconds.
 *
 ***************************************************************************************/


int generateTimerValue(char action, char distribution, char *sender_ip)
{
    int retval;
    int estimated_delay, /* estimated one-way delay */
        timerlow, /* lower bound for the timer interval */
        timerhigh; /* higher bound for the timer interval */
    
    retval = 0;
    
    switch((int)action){
    
        case NAK_SND_WAIT:

#ifndef RANDOM_TIMERS        
            retval = DEFAULT_TIMER_VALUE[(int)action];
#else
            if (RM_getHostDelay(sender_ip,&estimated_delay) > 0){
                timerlow= rmcast_options.timer_paramA*estimated_delay;
                timerhigh= (rmcast_options.timer_paramA+rmcast_options.timer_paramB)*estimated_delay;
                retval = (int)1000*( (timerhigh - timerlow) * generateSample(distribution) + timerlow);
                /* fprintf(stderr,"Using timer related to %s:%d %d %d %d\n",sender_ip,(int)action,timerlow,timerhigh,retval);  */
            }
            else{
                RM_getHostDelay("DEFAULT",&estimated_delay);
                timerlow= rmcast_options.timer_paramA*estimated_delay;
                timerhigh= (rmcast_options.timer_paramA+rmcast_options.timer_paramB)*estimated_delay;
                retval = (int)1000*( (timerhigh - timerlow) * generateSample(distribution) + timerlow);
                /* fprintf(stderr,"Using timer related to %s:%d %d %d %d\n",sender_ip,(int)action,timerlow,timerhigh,retval); */
            }
#endif

        break;

        case RET_RCV_WAIT:
#ifndef RANDOM_TIMERS        
            retval = DEFAULT_TIMER_VALUE[(int)action];
#else
            if (RM_getHostDelay(sender_ip,&estimated_delay) > 0){
                timerlow= rmcast_options.timer_paramC*estimated_delay;
                timerhigh= (rmcast_options.timer_paramC+rmcast_options.timer_paramD)*estimated_delay;
                retval = (int)1000*( (timerhigh - timerlow) * generateSample(distribution) + timerlow);
                /* fprintf(stderr,"Using timer related to %s:%d %d %d %d\n",sender_ip,(int)action,timerlow,timerhigh,retval); */
            }
            else{
                RM_getHostDelay("DEFAULT",&estimated_delay);
                timerlow= rmcast_options.timer_paramA*estimated_delay;
                timerhigh= (rmcast_options.timer_paramA+rmcast_options.timer_paramB)*estimated_delay;
                retval = (int)1000*( (timerhigh - timerlow) * generateSample(distribution) + timerlow);
                /* fprintf(stderr,"Using timer related to %s:%d %d %d %d\n",sender_ip,(int)action,timerlow,timerhigh,retval); */
            }

#endif

        break;
        
        case RET_SND_WAIT:
#ifndef RANDOM_TIMERS        
            retval = DEFAULT_TIMER_VALUE[(int)action];
#else
            if (RM_getHostDelay(sender_ip,&estimated_delay) > 0){
                timerlow= rmcast_options.timer_paramE*estimated_delay;
                timerhigh= (rmcast_options.timer_paramE+rmcast_options.timer_paramF)*estimated_delay;
                retval = (int)1000*( (timerhigh - timerlow) * generateSample(distribution) + timerlow);
                /* fprintf(stderr,"Using timer related to %s:%d %d %d %d\n",sender_ip,(int)action,timerlow,timerhigh,retval); */
            }
            else{
                RM_getHostDelay("DEFAULT",&estimated_delay);
                timerlow= rmcast_options.timer_paramC*estimated_delay;
                timerhigh= (rmcast_options.timer_paramC+rmcast_options.timer_paramD)*estimated_delay;
                retval = (int)1000*( (timerhigh - timerlow) * generateSample(distribution) + timerlow);
                /* fprintf(stderr,"Using timer related to %s:%d %d %d %d\n",sender_ip,(int)action,timerlow,timerhigh,retval); */
            }
#endif
        
        break;
        
        case REF_SND_WAIT:

            retval = rmcast_options.refresh_timer*1000000;
            
        break;
        
        case LEV_GRP_WAIT:

            retval = rmcast_options.leave_group_wait_time;
            
        break;
        
        default:
            fprintf(stderr,"generateTimerValue Warning: Unknow action=%d\n",action);
    }

    return ( retval );
}

/***************************************************************************************
 *
 * inline void getRemainingTime(EVENT_LIST **el, long double*remaining_time, char *msg)
 *
 * Gets the remaining time to execute a specific action in the event list.
 *
 * Arguments:	el, the event list;
 *				remaining_time, the remaining time to execute the action;
 *				msg, an error message to be shown if remaining_time <= 0.
 *
 ***************************************************************************************/

inline void getRemainingTime(EVENT_LIST **el, long double *remaining_time, char *msg)
{
    struct timeval current_time;
    
    gettimeofday(&current_time, 0);    

	(*remaining_time) = (*el)->timer_value - ( getTimerValue(&(current_time))- getTimerValue(&((*el)->last_update_time)) );	

    if (*remaining_time <= 0)
    {
#ifdef DEBUG_EVENTS    
       fprintf(stderr,"Error: remaining time < 0\n");
       fprintf(stderr,"\tRemaining time: %.0Lf \n",*remaining_time);
       fprintf(stderr,"\tCurrent time   : %.0Lf \n",getTimerValue(&(current_time)) );
       fprintf(stderr,"\tLast update time    : %.0Lf \n",getTimerValue(&((*el)->last_update_time)) );
       if (msg!=NULL)
           fprintf(stderr,"\tmsg           : %s \n",msg );
#endif           
        (*remaining_time) = 0;
    }
}


/***************************************************************************************
 *
 * void setTimerValue(struct itimerval *value, long int time)
 *
 * Converts the time from microseconds to struct itimerval.
 *
 * Arguments:	value, where the converted value will be stored (a struct itimerval);
 *				time, the time in microseconds.
 *
 ***************************************************************************************/

void setTimerValue(struct itimerval *value, long int time)
{
	 (*value).it_interval.tv_sec = 0;
	 (*value).it_interval.tv_usec = 0;

 	 (*value).it_value.tv_sec = time/1000000;
	 (*value).it_value.tv_usec = time%1000000;
}

/***************************************************************************************
 *
 * long double getTimerValue(struct timeval *value )
 *
 * Converts the time from struct timeval to microseconds.
 *
 * Arguments:	value, the struct timeval source.
 *
 * Returns: 	the time, in microseconds.
 *
 ***************************************************************************************/


long double getTimerValue(/* struct itimerval *value */ struct timeval *value )
{
	return (((long double)(*value).tv_sec*1000000)+((long double)(*value).tv_usec));
}

/*---------------- Main functions to manipulate the event list ---------------------------------------*/

/***************************************************************************************
 *
 * void eventListInit(EVENT_LIST **el)
 *
 * Initialize the event list.
 *
 * Arguments:	el, the event list;
 *
 ***************************************************************************************/


void eventListInit(EVENT_LIST **el)
{
#ifdef SOLARIS
    pthread_mutex_init(&event_list_mutex,NULL);
#endif   

    *el = NULL;
    
/*
 *     FIXME under unicast mode, it doesn't make sense to have long timer values 
 *     if (rmcast_options.transmission_mode == UNICAST)
 *     {  
 * 
 *        int i;
 * 
 *        for (i=0; i<6; i++)
 *             DEFAULT_TIMER_VALUE [i] /= 10000; 
 *     }
 */
    
}

/***************************************************************************************
 *
 * int eventListAllocNode(EVENT_LIST **el_node, MEMBER_ID *member, char action, 
 *							int sn, long timer_value)
 *
 * Allocates a event list node, and fills it with specified info.
 *
 * Arguments:	el_node, the node which will be created;
 *				member, action, sn and timer_value hold info. to be stored in new node.
 *
 * Returns: 	0 on error, 1 on success.
 *
 ***************************************************************************************/

int eventListAllocNode(EVENT_LIST **el_node, MEMBER_ID *member, 
                        char action, int sn, long timer_value)
{
    if ((*el_node = (EVENT_LIST*)malloc(sizeof(EVENT_LIST)))==NULL)
    {
        return (0);
    }
    (*el_node)->member_id = member;
    (*el_node)->action = action;
    (*el_node)->timer_value = timer_value;
    (*el_node)->sn = sn;
    gettimeofday( &((*el_node)->last_update_time), 0) ;    
    (*el_node)->next = NULL;    
    return (1);
}


/***************************************************************************************
 *
 * int	eventListInsert(EVENT_LIST **el, MEMBER_ID *member_id,	char *sender_ip, char action, int sn)
 *
 * Inserts an event node in the event list.
 *
 * Arguments:	el, the event list;
 *				member_id, the member identification;
 *              sender_ip, the IP address of the sender. Used for host related timer generation
 *				action, event action to be executed;
 *				sn, sequence number of the message to retransmited/requested.
 *
 ***************************************************************************************/

int  eventListInsert(EVENT_LIST **el, MEMBER_ID *member_id, char *sender_ip, char action, int sn)
{
    EVENT_LIST *newNode, *auxNode, *antNode;
    long int timer_value=0, accumulator=0;
	long double remaining_time; 
    struct timeval current_time;                             
    int retval = -2;
    
#ifdef SINGLE_NACK

    int i             = -1;
    int nack_sn       = -1;
    int aux_bit_value =  0;
    int event_flag    =  0;
    int control       =  0;
    int cont          =  0;

    CACHE *aux_cache_member;

    cacheShow(cache);
    
    aux_cache_member = cacheLookForMember(&cache, member_id);
    
#ifdef DSINGLE_NACK    
    fprintf(stderr,"DSINGLE_NACK eventListInsert: [aux_cache_member: %p &cache: %p member_id: %p]\n",aux_cache_member,&cache,member_id);
#endif
#ifdef DNAK_RCV
    fprintf(stderr,"DNAK_RCV eventListInsert: entering. Action=%d sn=%d\n",action,sn);    
#endif    
    if( aux_cache_member != NULL )
    {
         switch ( action ) 
         {
         
            case NAK_SND_WAIT: /* Trying to insert a single NACK */

                for ( i =  aux_cache_member->sm_info.member_status.last_seq_rcv+1;
                      i <= aux_cache_member->sm_info.member_status.window_size + aux_cache_member->sm_info.member_status.last_seq_rcv ;
                      i ++ )
                {
                     /* Note: the first bit of the mask always refer to the packet with sn = last_seq_rcv + 1 */

    #ifdef DWINDOW_MASK    
                    fprintf(stderr,"DWINDOW_MASK eventListInsert: before window_mask_get_bit call. Event action=%d sn=%d i=%d\n",action,sn,i);
    #endif
                    aux_bit_value = window_mask_get_bit( aux_cache_member->sm_info.member_status.window_mask,i, &aux_cache_member->sm_info.member_status );

                    if  ( ( aux_bit_value == NAK_SND_WAIT ) || ( aux_bit_value == RET_RCV_WAIT ) )
                    {
                        /*   NAK_SND_WAIT  or RET_RCV_WAIT already exists */
                        control = aux_bit_value;
                        break;
                    }

                }
                
                if ( control == RET_RCV_WAIT )
                {
                    /* RET_RCV_WAIT event exists - just set the bit to RET_RCV_WAIT, the nack will be send in the next time */
                    
                    window_mask_set_bit( aux_cache_member->sm_info.member_status.window_mask,sn, &aux_cache_member->sm_info.member_status, RET_RCV_WAIT );
                }
                else
                {

                    if( cacheUpdateNakList(&cache, member_id, sn) > 0 )
                    {
                        window_mask_set_bit( aux_cache_member->sm_info.member_status.window_mask,sn, &aux_cache_member->sm_info.member_status, NAK_SND_WAIT );
                    }
                    else
                    {
                        /*
                         * We have sent the max number of NAK (see rmcast_options structure) we didn't recover the packet
                         * so we must exit because we cannot keep the reliability 
                         */

                        fprintf(stderr,"********\n");
                        fprintf(stderr,"eventListInsert ERROR: Recovering packet failed. Max number (%d) of NAKs reached!\n",rmcast_options.max_nak);
                        fprintf(stderr,"\tHost IP:PID=%s:%d\tsn=%d\n",member_id->ip,member_id->pid,i); 
                        fprintf(stderr,"********\n");

                        RM_leaveGroup(local_user_info.socket, local_user_info.member_id.ip);
                    }
                    
                    if( control == 0 )
                    {
                        /* We have to insert a new event */
                        event_flag = NAK_SND_WAIT;
                    }
                }    
                
                break;
                
            case RET_RCV_EXPIRED: /* RET_RCV_WAIT event has expired: change bits 2 to 1 in the mask and create NAK_SND_WAIT event */

                for ( i = aux_cache_member->sm_info.member_status.last_seq_rcv+1;
                      i <= aux_cache_member->sm_info.member_status.window_size + 
                          aux_cache_member->sm_info.member_status.last_seq_rcv ;
                      i ++ )
                {
                     /* Note: the first bit of the mask always refer to the packet with sn = last_seq_rcv + 1 */

    #ifdef DWINDOW_MASK    
                    fprintf(stderr,"DWINDOW_MASK eventListInsert: before window_mask_get_bit call. Event action=%d sn=%d i=%d\n",action,sn,i);
    #endif
                    aux_bit_value = window_mask_get_bit( aux_cache_member->sm_info.member_status.window_mask,i, &aux_cache_member->sm_info.member_status );

                    if  (  aux_bit_value == RET_RCV_WAIT )
                    {
                        if( cacheUpdateNakList(&cache, member_id, i) > 0 )
                        {
                            window_mask_set_bit( aux_cache_member->sm_info.member_status.window_mask,i, &aux_cache_member->sm_info.member_status, NAK_SND_WAIT );
                        }
                        else
                        {
                            /*
                             * We have sent the max number of NAK (see rmcast_options structure) we didn't recover the packet
                             * so we must exit because we cannot keep the reliability 
                             */

                            fprintf(stderr,"********\n");
                            fprintf(stderr,"eventListInsert ERROR: Recovering packet failed. Max number (%d) of NAKs reached!\n",rmcast_options.max_nak);
                            fprintf(stderr,"\tHost IP:PID=%s:%d\tsn=%d\n",member_id->ip,member_id->pid,i); 
                            fprintf(stderr,"********\n");

                            RM_leaveGroup(local_user_info.socket, local_user_info.member_id.ip);

                        }
                    }
                }
                
                event_flag = NAK_SND_WAIT;
                
                break;

                
            case RET_RCV_WAIT: /* NAK_SND_WAIT has expired: change bits 1 to 2 in the mask and create RET_RCV_WAIT event */
            
                for( i = aux_cache_member->sm_info.member_status.last_seq_rcv+1;
                      i <= aux_cache_member->sm_info.member_status.window_size + 
                          aux_cache_member->sm_info.member_status.last_seq_rcv ;
                      i ++ )
                {
                     /* Note: the first bit of the mask always refer to the packet with sn = last_seq_rcv + 1 */

    #ifdef DWINDOW_MASK    
                    fprintf(stderr,"DWINDOW_MASK eventListInsert: before window_mask_get_bit call. Event action=%d sn=%d i=%d\n",action,sn,i);
    #endif
                    aux_bit_value = window_mask_get_bit( aux_cache_member->sm_info.member_status.window_mask,i, &aux_cache_member->sm_info.member_status );

                    if( ( aux_bit_value == NAK_SND_WAIT ) || ( aux_bit_value == SUPPRESSED_NAK ) )
                    {
                        window_mask_set_bit( aux_cache_member->sm_info.member_status.window_mask, i, &aux_cache_member->sm_info.member_status, RET_RCV_WAIT );
                    }
                }
                
                #ifdef DNAK_RCV
                    fprintf( stderr, "DNAK_RCV eventListInsert: Now we have to insert a RET_RCV_WAIT event!\n");
                #endif
                event_flag = RET_RCV_WAIT;
                
                break;
                
            case SUPPRESSED_NAK: /* Suppress NAK identified by sn */

                for( i = aux_cache_member->sm_info.member_status.last_seq_rcv+1;
                     i <= aux_cache_member->sm_info.member_status.window_size + 
                          aux_cache_member->sm_info.member_status.last_seq_rcv ;
                     i ++ )
                {
                     /* Note: the first bit of the mask always refer to the packet with sn = last_seq_rcv + 1 */

    #ifdef DWINDOW_MASK    
                    fprintf(stderr,"DWINDOW_MASK eventListInsert: before window_mask_get_bit call. Event action=%d sn=%d i=%d\n",action,sn,i);
    #endif
                    aux_bit_value = window_mask_get_bit( aux_cache_member->sm_info.member_status.window_mask,i, &aux_cache_member->sm_info.member_status );

                    if( ( aux_bit_value == NAK_SND_WAIT )  )
                    {
                        /* NAK_SND_WAIT already exists, add 1 to cont */
                        control = aux_bit_value;
                        nack_sn = i;
                        cont++;
                        
                    }
                    if( aux_bit_value == RET_RCV_WAIT )
                    {   
                        /* RET_RCV_WAIT already exists */
                        control = aux_bit_value;
                        break;
                    }

                }
                
                if (control == RET_RCV_WAIT )
                {
                    /* No need to set the bit to SUPPRESSED_NAK because no NAK_SND_WAIT event exists */
                    window_mask_set_bit( aux_cache_member->sm_info.member_status.window_mask, sn, &aux_cache_member->sm_info.member_status, RET_RCV_WAIT );
                }
                else if( control == NAK_SND_WAIT )
                {
                    window_mask_set_bit( aux_cache_member->sm_info.member_status.window_mask, sn, &aux_cache_member->sm_info.member_status, SUPPRESSED_NAK );
                    #ifdef DSUPPRESSION
                        fprintf(suppressionfile,"nk %s %s %d %d\n",sender_ip,member_id->ip,member_id->pid,sn);
                    #endif
                    
                    if( ( nack_sn == sn ) && ( cont == 1) )
                    {
                       /*
                        *    We have changed the last NAK bit to SUPPRESSED_NAK.                                  
                        *    now we have to change each SUPPRESSED_NAK bit to RET_RCV_WAIT                        
                        *    and insert a RET_RCV_WAIT event                                                      
                        */

                        #ifdef DSUPPRESSION
                            fprintf(suppressionfile,"NK %s %s %d %d\n",sender_ip,member_id->ip,member_id->pid,sn);
                        #endif

                        for( i = aux_cache_member->sm_info.member_status.last_seq_rcv+1;
                             i <= aux_cache_member->sm_info.member_status.window_size + 
                                  aux_cache_member->sm_info.member_status.last_seq_rcv ;
                             i ++ )
                        {
                             /* Note: the first bit of the mask always refer to the packet with sn = last_seq_rcv + 1 */

            #ifdef DWINDOW_MASK    
                            fprintf(stderr,"DWINDOW_MASK eventListInsert: before window_mask_get_bit call. Event action=%d sn=%d i=%d\n",action,sn,i);
            #endif
                            aux_bit_value = window_mask_get_bit( aux_cache_member->sm_info.member_status.window_mask,i, &aux_cache_member->sm_info.member_status );

                            if( ( aux_bit_value == NAK_SND_WAIT )  )
                            {
                                window_mask_set_bit( aux_cache_member->sm_info.member_status.window_mask, i, &aux_cache_member->sm_info.member_status, RET_RCV_WAIT );
                            }
                        }

                        event_flag = RET_RCV_WAIT;
                    }
                }
                else
                {
                    window_mask_set_bit( aux_cache_member->sm_info.member_status.window_mask, sn, &aux_cache_member->sm_info.member_status, RET_RCV_WAIT );
                    event_flag = RET_RCV_WAIT;
                }
                
                break;
                
            default:
            
                event_flag = action;
                
        }
    }
    else
    {
        event_flag = action;
    }
    
    if ( event_flag > 0 )
    {
        action = event_flag;
        
#endif /* Single NACK */
    
#ifdef DNAK_RCV
    fprintf(stderr,"After switch (action) --> action == %d\n",action);
#endif     
        gettimeofday(&current_time, 0);    

        auxNode = *el;


        if (*el!=NULL)
        {

            getRemainingTime(el,&remaining_time,"(Called from eventListInsert)\n");

        }
        else
        {
            remaining_time = 0;
        }        
    #ifdef DEBUG_EVENTS    
        fprintf(stderr,"DEBUG_EVENTS eventListInsert: current event list:\n");

        eventListShow(*el);

        fprintf(stderr,"DEBUG_EVENTS eventListInsert: node to be inserted:\n");
        fprintf(stderr,"DEBUG_EVENTS eventListInsert: action: %d sn: %d\n", action, sn);
        fprintf(stderr,"DEBUG_EVENTS eventListInsert: remaining time: %.0Lf\n", remaining_time);
        fprintf(stderr,"DEBUG_EVENTS eventListInsert: current time %.0Lf\n\n", getTimerValue(&current_time) );    
    #endif    

        if (*el!=NULL)
        {

		      /* .. and set the timer value of the header of the list to the 
		  	         remaining time to the first event occur */		  

	         (*el)->timer_value = remaining_time;
             (*el)->last_update_time = current_time;

        }

        timer_value=generateTimerValue(action,rmcast_options.timer_distribution,sender_ip);

    #ifdef DEBUG_EVENTS        
        fprintf(stderr,"DEBUG_EVENTS eventListInsert: time %ld\n\n", timer_value );    
    #endif    

        if ((eventListAllocNode(&newNode,member_id,action,sn,timer_value))==(int)NULL)

        {    
             return (0);    
        }    
        if (auxNode == NULL) /* if the list is empty... */
        {
             (*el) = newNode; 
        }
        else
        {
            if (newNode->timer_value < auxNode->timer_value)
            {

		         /* if the node to be inserted is the first of the list...*/

                (newNode)->next = auxNode;
                (*el) = newNode;
                (newNode)->next->timer_value -= newNode->timer_value;
            }
            else
            {
                antNode = auxNode;
                accumulator = auxNode->timer_value;
                while(accumulator < newNode->timer_value)
                {
                    antNode = auxNode;
	    		    auxNode = auxNode->next;
    			    if (auxNode==NULL)
                        break;
                    accumulator += auxNode->timer_value;	  
                }
                if (auxNode == NULL)
                {
		    	    /* ... if the node to be inserted is the last of the list... */

                    antNode->next = newNode;              
                    newNode->timer_value -= accumulator;
                }
                else
                {
                    if (auxNode!=antNode)
					{

					   /* ... if the node to be inserted is in the middle of the list */
					   newNode->next = auxNode;   
					   antNode->next = newNode;
#ifdef DEBUG_EVENTS												
 					   fprintf(stderr,"DEBUG_EVENTS eventListInsert: accumulator: %ld newNode->timer_value: %ld antNode->timer_value: %ld auxNode->timer_value: %ld", 
					 	 accumulator , newNode->timer_value ,  antNode->timer_value, auxNode->timer_value);
#endif						
					   newNode->timer_value -= (accumulator - auxNode->timer_value);
                	   auxNode->timer_value -= newNode->timer_value;
					}
					else
    				{

                     /* ... finally, if the node to be inserted has a time value 
				     equal to the first of the list... */

	                   newNode->next = antNode->next;   
					   antNode->next = newNode;
					   newNode->timer_value = 0;
					}
                }
            }
        } 

    #ifdef DEBUG_SHOW	 
	     eventListShow(*el);	 
    #endif

    #ifdef  DEBUG_EVENTS
        fprintf(stderr,"DEBUG_EVENTS eventListInsert: sending SIGUSR1 signal\n");
    #endif	 
        retval = pthread_kill(signal_handler_thread, SIGUSR1);       

        if ( retval )
        {
            fprintf(stderr,"eventListInsert ERROR: pthread_kill error: %d \n",retval);

        }
    #ifdef  DEBUG_EVENTS
        else
        {
            fprintf(stderr,"DEBUG_EVENTS eventListInsert: signal sent\n");
        }    
    #endif    
    #ifdef DNAK_RCV
        fprintf(stderr, "eventListInsert: showing Event List before leaving\n");
        eventListShow(*el);
    #endif 

#ifdef SINGLE_NACK
    } /* if (control >0 ) */
#endif     

    return (1);
}

/***************************************************************************************
 *
 * void eventListShow(EVENT_LIST *el)
 *
 * Shows the event list.
 *
 * Arguments:	el, the event list.
 *
 ***************************************************************************************/

void eventListShow(EVENT_LIST *el)
{

#ifdef DEBUG_EVENTS

     EVENT_LIST *aux;
 	 int i=0;
     long double remaining_time;
     
    struct timeval current_time;
    
    gettimeofday(&current_time, 0);    
     
    aux = el;
     
	 fprintf(stderr,"\n----------EventList------------\n\n");
     
     if (aux==NULL)
     {
        fprintf(stderr,"EventList is EMPTY\n");
        fprintf(stderr,"\n------------------------------\n\n");
        return;
     }
     
     fprintf(stderr,"aux: %p\n",aux);
         
	 for (;aux!=NULL;aux=aux->next)
	 {
        fprintf(stderr," [%p](ip=%s pid=%d action=%2d sn=%2d timer=%8ld) -> ", 
            aux,
            ((aux->member_id!=NULL)?aux->member_id->ip:"NULL"),  
            ((aux->member_id!=NULL)?aux->member_id->pid:-1),
            aux->action, 
            aux->sn, 
            aux->timer_value);
		if (i%2) fprintf(stderr,"\n");
 	    i++;
	 }
	 fprintf(stderr," NULL\n");
     fprintf(stderr," eventListShow: number of nodes in event list=%d\n",i);
     if (el!=NULL)
     {
	     getRemainingTime(&el,&remaining_time,"Called from eventListShow");
     }

     fprintf(stderr,"\n------------------------------\n\n");     

#endif
}


/***************************************************************************************
 *
 * int eventListRemoveFirst(EVENT_LIST **el)
 *
 * Removes the first element of the event list.  
 * 
 * Arguments: el, the event list.
 *
 * Return:	1, on success;
 *			0, on fail.
 *
 * Pay Attention! 
 *
 * This function neither sends any signal, nor calls any time related function.
 *
 ***************************************************************************************/

int eventListRemoveFirst(EVENT_LIST **el)
{
	EVENT_LIST *ant;
    
/* #ifdef SINGLE_NACK

    int i = 0;
    
    CACHE *aux_cache_member = cacheLookForMember(&cache, (*el)->member_id);

    if ( (*el != NULL) && (aux_cache_member != NULL ))
    {
        if ( (*el) -> action == NAK_SND_WAIT )
        {
            
            for ( i = 0; i < MAX_WINDOW_SIZE ; i ++ )
            {
                if  (  window_mask_get_bit2( aux_cache_member->sm_info.member_status.window_mask,
                        i ) == NAK_SND_WAIT )
                {
                    window_mask_set_bit2( aux_cache_member->sm_info.member_status.window_mask,
                        i , RET_RCV_WAIT );
                }
            }

        } else if ( (*el) -> action == RET_RCV_WAIT )  {
            
            for ( i = 0; i < MAX_WINDOW_SIZE ; i ++ )
            {
                if  (  window_mask_get_bit2( aux_cache_member->sm_info.member_status.window_mask,
                        i ) == RET_RCV_WAIT )
                {
                    window_mask_set_bit2( aux_cache_member->sm_info.member_status.window_mask,
                        i , NAK_SND_WAIT );
                }
            }

        }
        
    }

#endif    */
    
  
	if (*el==NULL)
	{
        return (0);
	}
	else
	{
    
		ant = *el;
		(*el) = (*el)->next;
      
		free(ant);
        return (1);
	}
}

/***************************************************************************************
 *
 * int eventListRemove(EVENT_LIST **el, MEMBER_ID *member_id, char action, int sn)
 *
 * Arguments:	el, the event list;
 *				member_id, action and sn, identify the node to be removed;
 *
 * Return value: 1 on success;
 *				 0 otherwise.
 *
 ***************************************************************************************/

int eventListRemove(EVENT_LIST **el, MEMBER_ID *member_id, char action, int sn)
{
    EVENT_LIST *ant_node;  

    int retval = 0;
    EVENT_LIST *node_to_be_removed;
    long double remaining_time=0;

    struct timeval current_time;
    
#ifdef SINGLE_NACK

    int i = 0;
    
    CACHE *aux_cache_member;

    aux_cache_member = cacheLookForMember(&cache, member_id);

    if(aux_cache_member!=NULL)
    {
#ifdef DSINGLE_NACK
        fprintf(stderr,"DSINGLE_NACK eventListRemove: Trying to remove event action=%d\n",action);
#endif   
#ifdef DNAK_RCV
        fprintf(stderr, "eventListRemove: Trying to remove event action=%d\n",action);
#endif 

        if ( action == NAK_SND_WAIT  || action == RET_RCV_WAIT )
        {
            if( ( sn <= aux_cache_member->sm_info.member_status.last_seq_rcv ) 
                || ( sn > aux_cache_member->sm_info.member_status.last_seq_rcv + aux_cache_member->sm_info.member_status.window_size ) )
            {
             #ifdef DNAK_RCV
                fprintf( stderr,"eventListRemove: There is no such event because sn=%d is out of the window_mask action=%d\n",sn,action);
             #endif
                return 0;
            }
                
    #ifdef DSINGLE_NACK
        fprintf(stderr,"DSINGLE_NACK eventlistremove: member_status.last_seq_rcv=%d,member_status.window_ini=%d\n",aux_cache_member->sm_info.member_status.last_seq_rcv,aux_cache_member->sm_info.member_status.window_ini);    
    #endif        
            if ( window_mask_get_bit( aux_cache_member->sm_info.member_status.window_mask,

                sn, &(aux_cache_member->sm_info.member_status) ) == action )
            {

                window_mask_set_bit( aux_cache_member->sm_info.member_status.window_mask,

                    sn, &(aux_cache_member->sm_info.member_status), 0 );
                
                /* Remove node from member's NAK list */
                /* FIXME cacheDeleteNakNode( cache, member_id, sn); */

                for ( i = aux_cache_member->sm_info.member_status.last_seq_rcv + 1;
                      i <= aux_cache_member->sm_info.member_status.window_size + aux_cache_member->sm_info.member_status.last_seq_rcv ;
                      i ++ )
                {

                    if  (  window_mask_get_bit( aux_cache_member->sm_info.member_status.window_mask,
                            i, &aux_cache_member->sm_info.member_status ) == action )
                    {
                        return 1;
                    }
                }
            }
            else
                return 0;
        }
    }    
    
#endif    /*SINGLE NACK*/

    node_to_be_removed= eventListFind(el,member_id,action,sn,&ant_node);    
#ifdef DSINGLE_NACK
    fprintf(stderr,"DSINGLE_NACK eventlistRemove: after looking for event, action=%d,sn=%d.\n",action,sn);
#endif
#ifdef DNAK_RCV
        fprintf(stderr, "eventlistRemove: after looking for event, action=%d,sn=%d.\n",action,sn);
#endif 
    
    if (node_to_be_removed==NULL)
    {   
#ifdef DSINGLE_NACK
    fprintf(stderr,"DSINGLE_NACK eventlistRemove:event (action=%d sn=%d) not found.\n",action,sn);
#endif
#ifdef DNAK_RCV
        fprintf(stderr, "eventlistRemove:event (action=%d sn=%d) not found.\n",action,sn);
#endif 

        return (0);
    }
    else
    {
#ifdef DSINGLE_NACK
    fprintf(stderr,"DSINGLE_NACK eventlistRemove: event (action=%d sn=%d) found.\n",action,sn);
#endif    
#ifdef DNAK_RCV
        fprintf(stderr, "eventlistRemove: event (action=%d sn=%d) found.\n",action,sn);
#endif 

        if (ant_node == NULL)
        {       
             /* the node to be removed is the first node of the list */
            
             if (*el!=NULL)
             {
                 getRemainingTime(el,&remaining_time,"(eventListRemove)\n");
             }
             else
             {
                 remaining_time = 0;
             }        
			 *el = (*el)->next;
				
			 if (*el!=NULL)
             {  
                  (*el)->timer_value += remaining_time;
                  
                  gettimeofday(&current_time, 0);

                  (*el)->last_update_time = current_time;

                  if ((*el)->timer_value == 0)
                  {
                  
#ifdef DEBUG_EVENTS                  
                    fprintf(stderr,"DEBUG_EVENTS eventListRemove warning: the alarm was restarted.\n");
#endif                 
                    eventListShow(*el);
                  }
			 }
			
             retval = pthread_kill(signal_handler_thread, SIGUSR1);       

             if ( retval )
             {
                 fprintf(stderr,"evenListRemove ERROR: trying to kill signal handler thread retval=%d \n",retval);

             }
             

        }
        else
        {
            if (node_to_be_removed->next!=NULL)
            {
                 node_to_be_removed->next->timer_value += node_to_be_removed->timer_value;
                 ant_node->next = node_to_be_removed -> next;
            }
            else
            {
                 /* the node to be removed  is the last node of the list */
                 ant_node->next = NULL;
            }
        }
    }
    free(node_to_be_removed);
    node_to_be_removed = NULL;
    
    return (1);
}

int eventListIsEqual(EVENT_LIST *node1, EVENT_LIST *node2)
{
    if ((node1->member_id == NULL) || (node2->member_id == NULL))
    {

#ifdef SINGLE_NACK

    #ifdef DSINGLE_NACK
        fprintf(stderr,"DSINGLE_NACK eventListIsEqual: node null, node1=%p node2=%p\n",node1->member_id,node2->member_id);
    #endif
    
        if ( node1->action == NAK_SND_WAIT || node1->action == RET_RCV_WAIT )
        {
                if ( node1->action == node2->action )
                {

#ifdef DSINGLE_NACK
        fprintf(stderr,"DSINGLE_NACK eventListIsEqual: (first return) event is equal action=%d\n",node1->action);
#endif
                    return 1;
                }
                else
                {
#ifdef DSINGLE_NACK
        fprintf(stderr,"DSINGLE_NACK eventListIsEqual: (first return) event is not equal action=%d\n",node1->action);
#endif
                    return 0;
                }
        }
#endif
        if ( (node1->action == node2->action)  &&
             (node1->sn     == node2->sn))
             
            return 1;
            
        else
        
            return 0;
    }
    
#ifdef SINGLE_NACK
    
    if ( node1->action == NAK_SND_WAIT || node1->action == RET_RCV_WAIT )
    {
    
        if ((!strcmp(node1->member_id->ip, node2->member_id->ip))
            && (node1->member_id->pid == node2->member_id->pid)
            && (node1->action == node2->action) 
            )
        {
#ifdef DSINGLE_NACK
            fprintf(stderr,"DSINGLE_NACK eventListIsEqual: (second return) event is equal action=%d\n",node1->action);
#endif            
            return (1);

        }
        else
        {            
#ifdef DSINGLE_NACK
            fprintf(stderr,"DSINGLE_NACK eventListIsEqual: (second return) event is equal action=%d\n",node1->action);
#endif            
            return (0);

        }        
    }
    
#endif    
    
    if ((!strcmp(node1->member_id->ip, node2->member_id->ip))
        && (node1->member_id->pid == node2->member_id->pid)
        && (node1->action == node2->action) 
        && (node1->sn     == node2->sn))

        return (1);
    else            
        return (0);
}

/***************************************************************************************
 *
 * EVENT_LIST * eventListFind(EVENT_LIST **el1,MEMBER_ID *member_id, char action, 
 *							  int sn, EVENT_LIST **antNode)
 *
 * Search for a node in the event list.
 *
 * Arguments:	el1, the event list;
 *				member_id, action and sn, identify the node to be found;
 *				antNode, return a pointer to the node that points to the one,
 *						 or NULL if the node removed is the last one.
 *
 * Return value: a pointer to the node found, or NULL if the node wasn't found.
 *
 ***************************************************************************************/


EVENT_LIST * eventListFind(EVENT_LIST **el1,MEMBER_ID *member_id, char action, int sn, EVENT_LIST **antNode)
{
    EVENT_LIST *el;
    
    EVENT_LIST *event_node, *auxPointer;
    
    *antNode = NULL;
    
    el = *el1;
    
    auxPointer = el;
    
#ifdef SINGLE_NACK

    {
         
         CACHE *aux_cache_member = NULL;

         aux_cache_member = cacheLookForMember(&cache, member_id);

         #ifdef DNAK_RCV
             fprintf(stderr,"eventListFind: looking for ip:pid:sn:action = %s:%d:%d:%d\n",
                                member_id->ip,
                                member_id->pid,
                                sn,
                                action);
             eventListShow(el);
         #endif

         if ( aux_cache_member != NULL )
         {
             if (  sn  > ( aux_cache_member->sm_info.member_status.last_seq_rcv +  aux_cache_member->sm_info.member_status.window_size ) )
             {

#ifdef DSINGLE_NACK
                  fprintf(stderr,"DSINGLE_NACK eventListFind: sn=%d is out of window(last_seq_rcv+1,last_seq_rcv+MAX_WINDOW_SIZE)=(%d,%d)!\n",
                                  sn,
                                  aux_cache_member->sm_info.member_status.last_seq_rcv,
                                  aux_cache_member->sm_info.member_status.last_seq_rcv + MAX_WINDOW_SIZE);
#endif
            #ifdef DNAK_RCV
                  fprintf(stderr,"eventListFind: sn=%d is out of window(last_seq_rcv+1,last_seq_rcv+MAX_WINDOW_SIZE)=(%d,%d)!\n",
                                  sn,
                                  aux_cache_member->sm_info.member_status.last_seq_rcv,
                                  aux_cache_member->sm_info.member_status.last_seq_rcv + MAX_WINDOW_SIZE);

            #endif 

                  return NULL;
             }
         }
         else
         {

#ifdef DSINGLE_NACK
             fprintf(stderr,"DSINGLE_NACK eventListFind: no such member found in the cache!\n");
#endif
#ifdef DNAK_RCV
    fprintf(stderr,"eventListFind: member found in the cache: %s:%d\n",member_id->ip,member_id->pid);
#endif
             return NULL;
         }
    }
      
#endif    
    
#ifdef DEBUG_EVENTS
  
      fprintf(stderr,"DEBUG_EVENTS eventListFind: Inside listfind\n ip: %s\n pid: %d\n action:%d\n sn: %d\n######\n", 
                      member_id->ip,
                      member_id->pid,
                      action,
                      sn);
  
#ifdef DEBUG_SHOW   
    eventListShow(el);
#endif    

#endif    
    
    if ((eventListAllocNode(&event_node, member_id, action, sn, 0))==(int)NULL)
    {
        perror("eventListFind: Alloc list node at eventListFind");
        RM_leaveGroup(local_user_info.socket, local_user_info.member_id.ip);
    }
    
    while (auxPointer!=NULL)
    {
    
        if (eventListIsEqual(event_node, auxPointer))
        {
            /*  The event was found! */
            
            break;
        }
        *antNode = auxPointer;
        auxPointer = auxPointer->next;
    }

    free(event_node);

    return (auxPointer);  
}

#endif

