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
                             rmcache.h
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

#ifndef RMCACHE_H

#define RMCACHE_H

#include "rmstruct.h"

#ifndef RMINTERNALS_H
    #include "rminternals.h"
#endif

void cacheInit(CACHE **cache);

int cacheAllocHeaderNode(CACHE **cache, MEMBER_ID *member);

int cacheAllocMessageNode(CACHE_NODE **cache_node, DATA_PACKET *node_info);

int cacheFreeMessageNode(CACHE_NODE **node);

int cacheInsertMessage(CACHE **cache, MEMBER_ID *member, DATA_PACKET *data_packet);

CACHE *cacheInsertMember(CACHE **cache, MEMBER_ID *member);

int cacheIsEmpty(CACHE **cache);

int cacheOfAnyMemberIsFull(CACHE *cache);

int memberIsEqual(MEMBER_ID *member1, MEMBER_ID *member2);

CACHE *cacheLookForMember(CACHE **cache, MEMBER_ID *member_id);

DATA_PACKET *cacheLookForMessage(CACHE **cache1, MEMBER_ID *member_id,int message_sn);      

int cacheDeactivateMember(CACHE *cache, MEMBER_ID *member);

int cacheGetSMInfo(CACHE *cache, SM_INFO **sm_info, int *nusers);

void cacheShow(CACHE *cache);

int cacheUpdateNakList(CACHE **cache, MEMBER_ID *member, int sn);

int cacheDeleteNakNode(CACHE *cache, MEMBER_ID *member, int sn);

void cacheUnpack(CACHE **cache, char *buf, int buffsize);

void cachePack(CACHE **cache, char **buf, int *buffsize);

int cacheCountMembers( CACHE *cache, int member_required_status);

#endif
