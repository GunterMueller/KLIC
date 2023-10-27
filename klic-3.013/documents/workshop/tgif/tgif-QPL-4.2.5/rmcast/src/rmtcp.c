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
                             rmtcp.c
                             -------------------
    begin                : 2002
    Authors              : Jorge Allyson Azevedo
                           Milena Scanferla
                           Daniel Sadoc
    email                : {allyson,milena,sadoc}@land.ufrj.br
 ***************************************************************************/

#include "rmtcp.h"
#include "rminternals.h"

#include <ctype.h>
#include <signal.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

extern USER_INFO    local_user_info;
extern sigset_t     alrmset;
extern FILE         *logfile;

/*
extern char         rmcast_tcp_ip[IP_STRING_SIZE];
extern int          *rmcast_pipe; 
extern int          rmcast_tcp_port;
*/

extern GLOBAL_OPTIONS rmcast_options;

extern CACHE        *cache;

extern int          errno;

#ifdef SOLARIS
     #include <sys/types.h>
     #include <sys/socket.h>
     #include <netinet/in.h>
     #include <arpa/inet.h>
#endif     

#include <pthread.h>

#define min(a,b) ((a<b)?a:b)

#ifdef SOLARIS
int
inet_pton(int family, const char *strptr, void *addrptr)
{
    if (family == AF_INET)
    {
        unsigned long int in_val;
        
        if ( ( (long int) (in_val = inet_addr(strptr)) ) != -1 ) {
            memcpy(addrptr, &in_val, sizeof(struct in_addr));
            return (1);
        }
        return (0);
    }
    errno = EAFNOSUPPORT;
    return (-1);
}
#endif

int rmcastReceiveTCPStatus(char *ip, int port, CurStatus *cur_status)
{
	int					sockfd;
    int                 retval = 1;
	struct sockaddr_in	servaddr;
	char				buffer[MAXLINE];
    CurStatus           cs_cache;


	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror( "socket error ");
      
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
    
#ifdef DEBUG_TCP    
    fprintf(stderr,"Trying to connect port %d\n",port);
#endif    
    
	servaddr.sin_port   = htons(port);	
    
	if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0)
    {
        sprintf( buffer, "inet_pton error for %s", ip);
        perror ( buffer );
    }
#ifndef SOLARIS    
	if ((retval=connect(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr))) < 0)
#else
	if ((retval=connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0)
#endif    
    {
       perror("connect error on rmcastSendTCPStatus");
       return FALSE;
    }
    else
    {
#ifdef DEBUG_TCP        
        fprintf( stderr, "connect ok!!\n");   
#endif        
    }
        
        
    read(sockfd, &(cur_status->size), sizeof(int));
        
    cur_status->data  = (char*)malloc((cur_status->size)+1);    
    
    retval = read(sockfd, cur_status->data, cur_status->size);        

#ifdef DEBUG_TCP    
    {
        int auxi;
        for(auxi=0; auxi<retval; auxi++)
        {
            fprintf(stderr,"[(%d) %c]", cur_status->data[(int)auxi], isalnum((int)cur_status->data[(int)auxi])?cur_status->data[(int)auxi]:'-');
        }
    }   
    
    fprintf(stderr,"\n");
#endif    
    
    /* Now, we will read the cache */
    

    read(sockfd, &(cs_cache.size), sizeof(int));
        
    cs_cache.data  = (char*)malloc((cs_cache.size)+1);    
    
    retval = read(sockfd, cs_cache.data, cs_cache.size);        
    
    cacheUnpack(&cache, cs_cache.data, cs_cache.size);
    
    cacheShow(cache);
   
    close(sockfd);     
    
    return TRUE;
}

#ifndef SOLARIS        
     
     #include <sys/types.h>
     #include <sys/socket.h>
     
typedef socklen_t   SOCKLEN_DECL_TYPE;
#else
typedef int         SOCKLEN_DECL_TYPE;
#endif

void * rmcastSendTCPStatus(void *arg)
{
    int                 retval;
	int					listenfd, connfd;
	SOCKLEN_DECL_TYPE		len;
	struct sockaddr_in	servaddr, cliaddr;
    
#ifdef DEBUG_TCP
    char buff[10000];
#endif    

    int cont = 0;

/*
 * #ifdef SOLARIS
 *     
 *     sigset_t     alrmset;
 *     
 *     sigfillset(&alrmset);            
 *     
 *     
 * 	
 * 	pthread_sigmask(SIG_BLOCK, &alrmset, NULL);    
 * 	
 * #endif
 */
    
#ifdef DEBUG_TCP    
    fprintf(stderr,"Initializing the 'current status server'\n");
#endif    

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket error");
        return FALSE;
    }

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(10013); /* 271d hexa */	
    
    retval = -1;
    
    while (retval==-1)
    {
#ifndef SOLARIS    
	    if ((retval = bind(listenfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)))==-1)
#else
	    if ((retval = bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)))==-1)
#endif        
        {
#ifdef DEBUG_TCP        
            fprintf(stderr,"bind error: %s (%d)\n",strerror(errno),retval);
#endif            
            cont ++;
            
        	servaddr.sin_port = htons(10013 + cont);	/* trying next port */
        }
    }
    
    rmcast_options.tcp_port = 10013 + cont;
    
#ifdef DEBUG_TCP   
    fprintf(stderr,"Using tcp port %d\nWaiting listen... \n", rmcast_options.tcp_port);
#endif    
    
  	if ((retval = listen(listenfd, LISTENQ)))
    {
        fprintf(stderr,"listen error: %s\n",strerror(errno));
        
        close (listenfd); 
        
        return FALSE;
    }
    
    len = sizeof(cliaddr);
    
    while (1)
    {
        char tmp_buf[80];

#ifdef DEBUG_TCP       
        fprintf(stderr,"Waiting for tcp connection... \n");
#endif        


        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len);
        
#ifdef DEBUG_TCP
#ifndef SOLARIS
        fprintf(stderr,"Connection from %s, port %d\n",
		       inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
		       ntohs(cliaddr.sin_port));
#endif
#endif               

        /* write connfd to pipe */
        
        sprintf(tmp_buf, "c%1d", connfd);
        write(rmcast_options.pipe[1], (const void *)tmp_buf, (size_t)(strlen(tmp_buf)+1));
        
    }

    close (listenfd); 

    return NULL;        
}



