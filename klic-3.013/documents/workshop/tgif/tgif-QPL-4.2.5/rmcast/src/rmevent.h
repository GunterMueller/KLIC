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
                             rmevent.h
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

#ifndef RMEVENT_H
#define RMEVENT_H

#include <string.h>
 
#include "rmstruct.h"

#define NAK_SND_WAIT    1    /* Waiting to send a NAK */
#define RET_RCV_WAIT    2    /* Waiting to receive a retransmission */
#define RET_SND_WAIT    3    /* Waiting to send a retransmission */
#define REF_SND_WAIT    4    /* Waiting to send a refresh message */
#define LEV_GRP_WAIT    5    /* Waiting to leave the multicast group */

#ifdef SINGLE_NACK
    /* Special actions for SINGLE NACK */
    #define SUPPRESSED_NAK   6    /* Intermediate state between NAK_SND_WAIT and RET_RCV_WAIT */
    #define RET_RCV_EXPIRED 7    /* Event RET_RCV_WAIT has expired create NAK_SND_WAIT event */
#endif

void eventListInit(EVENT_LIST **el);
int  eventListAllocNode(EVENT_LIST **el_node, MEMBER_ID *member, char action, int sn, long timer_value);
int  eventListInsert(EVENT_LIST **el, MEMBER_ID *member_id, char *sender_ip, char action, int sn);
void eventListShow(EVENT_LIST *el);
int  eventListRemove(EVENT_LIST **el, MEMBER_ID *member_id, char action, int sn);
int  eventListRemoveFirst(EVENT_LIST **el);
EVENT_LIST * eventListFind(EVENT_LIST **el,MEMBER_ID *member_id, char action, int sn, EVENT_LIST **antNode);
int  eventListIsEqual(EVENT_LIST *node1, EVENT_LIST *node2);

float generateSample(char distribution);
int generateTimerValue(char action, char distribution, char *sender_ip);

void setTimerValue(struct itimerval *value, long int time);
long double getTimerValue(/* struct itimerval *value */ struct timeval *value);

#endif

