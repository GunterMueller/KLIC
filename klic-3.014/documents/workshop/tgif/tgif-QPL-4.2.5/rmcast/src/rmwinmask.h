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
                             rmwinmask.h
                             -------------------
    begin                : Feb 2002
    Authors              : Jorge Allyson Azevedo
                           Milena Scanferla
                           Daniel Sadoc
    email                : {allyson,milena,sadoc}@land.ufrj.br
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#ifndef _RMWINMASK_H

#define _RMWINMASK_H

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



extern pthread_mutex_t nak_window_mask_mutex;

/*********************** Routines to handle the window nack mask ************************************/


#define min(a,b)  ( ( (a)<(b))?a:b ) 

#define MAX_WINDOW_SIZE 64

int window_mask_get_bit2(int *window_mask, int position);

int window_mask_sn_within_window(int *window_mask, int sn, MEMBER_STATUS *mstatus);

int window_mask_get_bit(int *window_mask, int sn, MEMBER_STATUS *mstatus);


int window_mask_set_bit2(int *window_mask, int position, int value);



int window_mask_set_bit(int *window_mask, int sn, MEMBER_STATUS *mstatus, int value);


int window_mask_numbers2array(int *window_mask, int  hmask, int  lmask);

int window_mask_array2numbers(int *window_mask, int *hmask, int *lmask, int mask_ini);

#define SHOW    for( i = MAX_WINDOW_SIZE - 1; i >= 0 ; i--) \
    {\
        fprintf(stderr,"%d",    c->sm_info.member_status.window_mask[ i ] );\
        if ( i ==  c->sm_info.member_status.window_ini )\
            fprintf ( stderr, "}" );\
        if ( i ==  c->sm_info.member_status.window_ini + c->sm_info.member_status.window_size ) \
            fprintf ( stderr, "{" );        \
    }\
    fprintf(stderr,"\n");


#endif

