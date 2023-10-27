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
                             rmmsg.h
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

#ifndef MSG_H
#define MSG_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#ifndef MSG_QUEUE_HEADER_SIZE

#define MSG_QUEUE_HEADER_SIZE 2 /* number of bytes used to store the data size */

#endif    

extern int errno;

#define MAXOBN 20000
#define MAXPRIOR 10

#define QPERM 0660
#define QKEY_BASE (key_t) 0105

struct q_entry {
    long mtype;
    char mtext[ MAXOBN + 1 ];
};

int  init_queue();
int  messageQueueEnter( char *objname, int len );
void remove_queues( int i );

#endif
