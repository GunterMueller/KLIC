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
                             rmwinmask.c
                             -------------------
    begin                : Feb 2002
    Authors              : Jorge Allyson Azevedo
                           Milena Scanferla
                           Daniel Sadoc
    email                : {allyson,milena,sadoc}@land.ufrj.br
 ***************************************************************************/

#ifndef _RMWINMASK_C

#include "rmwinmask.h"

#ifdef SINGLE_NACK

#define _RMWINMASK_C

#include "rmcache.h"
#include "rminternals.h"
#include "rmstruct.h"

#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>



pthread_mutex_t nak_window_mask_mutex = PTHREAD_MUTEX_INITIALIZER;

/*********************** Routines to handle the window nack mask ************************************/



int window_mask_get_bit2(int *window_mask, int position)
{
    int retval = -1;
    
#ifdef DWINDOW_MASK
   fprintf(stderr,"DWINDOW_MASK window_mask_get_bit2: entering\n");
#endif
    if (position < MAX_WINDOW_SIZE)
    {

#ifdef DWINDOW_MASK
   fprintf(stderr,"DWINDOW_MASK window_mask_get_bit2: inside if\n");
#endif
        pthread_mutex_lock(&nak_window_mask_mutex);
#ifdef DWINDOW_MASK
   fprintf(stderr,"DWINDOW_MASK window_mask_get_bit2: inside if, after locking nak_window_mask_mutex\n");
#endif
        retval = window_mask[position];
        pthread_mutex_unlock(&nak_window_mask_mutex);    
    }
#ifdef DWINDOW_MASK
   fprintf(stderr,"DWINDOW_MASK window_mask_get_bit2: leaving return=%d\n",retval);
#endif
    return(retval);
}


/***************************************************************************************
 *
 * int window_mask_sn_within_window(int *window_mask, int sn, MEMBER_STATUS *mstatus)
 *
 * Arguments:	window_mask - pointer to the member's window mask;
 *				sn - sn number;
                mstatus - pointer to the member's status structure;
 *
 * Return value: 1 on success;
 *				 0 otherwise.
 *
 ***************************************************************************************/

int window_mask_sn_within_window(int *window_mask, int sn, MEMBER_STATUS *mstatus)
{

    if ( sn > ( (mstatus->last_seq_rcv) + MAX_WINDOW_SIZE ) || sn < (mstatus->last_seq_rcv + 1) )
    {
        return 0;
    }
    else
    {
        return 1;
    }

}

int window_mask_get_bit(int *window_mask, int sn, MEMBER_STATUS *mstatus)
{

    int aux_position=0;
    int retval=-1;
    int out_retval=0;
    
#ifdef DWINDOW_MASK
   fprintf(stderr,"DWINDOW_MASK window_mask_get_bit: Inside window_mask_get_bit\n");
   fprintf(stderr,"DWINDOW_MASK window_mask_get_bit: mstatus=%p window_mask=%p sn=%d\n",mstatus,window_mask,sn);
#endif
   
   if( (mstatus!=NULL) && (out_retval=window_mask_sn_within_window(window_mask, sn, mstatus)))
   {
      
       aux_position= ( mstatus->window_ini + ( sn - (mstatus->last_seq_rcv +1))) % MAX_WINDOW_SIZE;
#ifdef DWINDOW_MASK
       fprintf(stderr,"DWINDOW_MASK window_mask_get_bit: aux_position=%d\n",aux_position);
#endif
       retval=window_mask_get_bit2 ( mstatus->window_mask,aux_position );
   }
   else
   {
#ifdef DWINDOW_MASK
       if (out_retval==0)
           fprintf(stderr,"window_mask_get_bit Warning: sn=%d out of window!\n",sn);
#endif
   };

   return (retval);

}


int window_mask_set_bit2(int *window_mask, int position, int value)
{

    int retval=1;
    
#ifdef DWINDOW_MASK
    fprintf(stderr,"DWINDOW_MASK window_mask_set_bit2: position=%d value=%d\n",position,value);
#endif

    if (position <= MAX_WINDOW_SIZE)
    {
        pthread_mutex_lock(&nak_window_mask_mutex);
        window_mask[position] = value;
        pthread_mutex_unlock(&nak_window_mask_mutex);
        
        retval=0;
    }

    return (retval);
}



int window_mask_set_bit(int *window_mask, int sn, MEMBER_STATUS *mstatus, int value)
{

    int retval=0;
    
#ifdef DWINDOW_MASK
    int l=0;
    int h=0;
    window_mask_array2numbers(window_mask,&h,&l,mstatus->window_ini);
    fprintf(stderr,"DWINDOW_MASK window_mask_set_bit: window_mask before h=%d l=%d \n",h,l);
#endif

    if (window_mask_sn_within_window(window_mask, sn, mstatus))
    {
        retval=window_mask_set_bit2( window_mask, 
                          ( mstatus->window_ini + ( sn - (mstatus->last_seq_rcv + 1 )) ) % MAX_WINDOW_SIZE,
                          value );
#ifdef DWINDOW_MASK
    window_mask_array2numbers(window_mask,&h,&l,mstatus->window_ini);
    fprintf(stderr,"DWINDOW_MASK window_mask_set_bit: window_mask after h=%d l=%d \n",h,l);
#endif

   }
    return retval;
   
}


int window_mask_numbers2array(int *window_mask, int  hmask, int  lmask)
{
    int cont = 0;
    
    pthread_mutex_lock(&nak_window_mask_mutex);    
    
    for (cont = 0; cont < min(MAX_WINDOW_SIZE,8*sizeof(int)); cont ++)
    {
        window_mask[ cont ] = (( lmask >> cont ) & 1 );
    }
    for (cont = sizeof(int) * 8; cont < min(MAX_WINDOW_SIZE,2*8*sizeof(int)); cont ++)
    {
        window_mask[ cont ] = ( ( hmask >> (cont - (sizeof(int)*8)) ) & 1 );
    }
    
    pthread_mutex_unlock(&nak_window_mask_mutex);
    
    return 1;
}

int window_mask_array2numbers(int *window_mask, int *hmask, int *lmask, int mask_ini)
{
    int cont = 0;
    
    pthread_mutex_lock(&nak_window_mask_mutex);    
    
    *lmask = 0; *hmask = 0;
    
    for (cont = 0; cont < min(MAX_WINDOW_SIZE,8*sizeof(int)); cont ++)
    {
        if (window_mask[ (cont + mask_ini)%MAX_WINDOW_SIZE ] == 1)
        {
                *lmask  |= (1 << cont);
        }

    }
    for (cont = sizeof(int) * 8; cont < min(MAX_WINDOW_SIZE,2*8*sizeof(int)); cont ++)
    {
        if (window_mask[ (cont + mask_ini)%MAX_WINDOW_SIZE ] == 1)
        {
                *hmask  |= 1 << (cont - sizeof(int) * 8);
        }

    }
    
    pthread_mutex_unlock(&nak_window_mask_mutex);    
    
    return 1;
}
#endif  /* SINGLE_NACK */
#endif  /* _RMWINMASK_C */

