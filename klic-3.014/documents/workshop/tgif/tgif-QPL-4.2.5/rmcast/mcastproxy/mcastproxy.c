#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <signal.h>

#define FLAGS 1
#define SENT_BY_PROXY 1
#define NOTSENT_BY_PROXY 0
#define MAX( a, b ) ( (a) > (b) ? (a) : (b) )
#define MAX_PACK_SIZE ( 64 * 1024 )
#define get(dest, org, bytes) { bzero(dest,bytes); memcpy(dest,org, bytes); org+=bytes; }

/* Returned errors identification */
#define MP_ERROR_PIDFILE         1 /* Could not create pidfile */
#define MP_ERROR_NOHOME          2 /* HOME environment variable not found */
#define MP_ERROR_OPENCONFFILE    3 /* Could not open config file */
#define MP_ERROR_INVALIDCONFFILE 4 /* Invalid config file */
#define MP_ERROR_RESOLVHOST      5 /* Could not resolve host name */
#define MP_ERROR_MSOCKET         6 /* Could not create multicast socket */
#define MP_ERROR_USOCKET         7 /* Could not create unicast socket */

typedef struct
{
    char *group_addr;
    char **addr_list;
    int  n_addr;
    char ttl;
    int  reuse_addr;
    char enable_loopback;
    int  ucast_port;
    int  mcast_port;
} Config;

Config config;
FILE  *fp;
char   mcastproxy_config_file[255];

typedef void (*sighandler_t)(int);

int  read_config_file( FILE *, Config * );
void re_read_config_file( int );
int  create_ucast_socket( Config * );
int  create_mcast_socket( Config *, struct sockaddr_in *);
int  convert_addr( Config *, struct sockaddr_in *, struct sockaddr_in ** );
void close_mcast_sock( int , struct sockaddr_in * );
void showHosts( int );

int main( int argc, char * argv[] )
{
    FILE  *pidfile;
    struct sockaddr_in group_addr;
    struct sockaddr_in * addr_list;
    fd_set readfds;
    int    mcast_sock, ucast_sock, byread, bysent, i;
    char   pack_buf[MAX_PACK_SIZE];
    char   mcastproxy_pid_file[255];
    char  *homedir;
    unsigned char flags;

    signal( SIGUSR1, showHosts );
    signal( SIGHUP, re_read_config_file );

    if( ( homedir = getenv( "HOME" ) ) != NULL )
    {
         sprintf( mcastproxy_pid_file, "%s/.tgwb/mcastproxy.pid", homedir );
         if( ( pidfile = fopen( mcastproxy_pid_file, "w" ) ) == NULL )
         {
            fprintf( stderr, "\n[mcastproxy Error] Opening %s file.\n", mcastproxy_pid_file );
            exit( MP_ERROR_PIDFILE );
         }
        fprintf( pidfile, "%d", (int)getpid() );
        fclose( pidfile );
    }
    else
    {
        fprintf( stderr, "\n[mcastproxy Error] $HOME environment variable not found\n" );
        exit( MP_ERROR_NOHOME );
    }

    printf( "\n----------------------------\n" );
    printf( "Starting mcastproxy ...\n" );
    printf( "\tReading mcastproxy.conf ..." );

    sprintf( mcastproxy_config_file, "%s/.tgwb/mcastproxy.conf", homedir );
    if( ( fp = fopen( mcastproxy_config_file, "r" ) ) == NULL )
    {
         fprintf( stderr, "\n[mcastproxy Error] Opening configuration file '%s'\n", mcastproxy_config_file );
         exit( MP_ERROR_OPENCONFFILE );
    }

    if( read_config_file( fp, &config ) < 0 )
    {
        fprintf( stderr, "\n[mcastproxy Error] Invalid Configuration file." );
        exit( MP_ERROR_INVALIDCONFFILE );
    }
    printf( " OK!\n" );
    if( convert_addr( &config, &group_addr, &addr_list ) < 0 )
    {
        fprintf(stderr,"\n\n[mcastproxy Error] Could not resolve host.\n" );
        exit( MP_ERROR_RESOLVHOST );
    }

    mcast_sock = create_mcast_socket( &config, &group_addr );
    if( mcast_sock < 0 )
    {
        fprintf(stderr,"\n[mcastproxy Error] Creating the multicast socket." );
        exit( MP_ERROR_MSOCKET );
    }

    ucast_sock = create_ucast_socket( &config );
    if( ucast_sock < 0 )
    {
        fprintf(stderr,"\n[mcastproxy Error] Creating the unicast socket.\n" );
        exit( MP_ERROR_USOCKET );
    }

    printf( "mcastproxy is running.\n" );
    printf( "----------------------------\n" );
    flags = NOTSENT_BY_PROXY;
    while( 1 )
    {
         FD_ZERO( &readfds );
         FD_SET( mcast_sock, &readfds );
         FD_SET( ucast_sock, &readfds );
         if( select( MAX( mcast_sock, ucast_sock ) + 1, &readfds, NULL, NULL, NULL ) < 0 )
         {
              /* fprintf( stderr, "\n[mcastproxy warning] Select error." ); */
              continue;
         }
         if( FD_ISSET( mcast_sock, &readfds ) )
         {
              byread = recv( mcast_sock, pack_buf, MAX_PACK_SIZE, 0 );

              if( byread > 0 )
              {
                  for( i = 0; i < config.n_addr; i++ )
                  {
                      if( config.enable_loopback == 1 )
                          flags = pack_buf[ FLAGS ];

                      if( ( config.enable_loopback == 0 ) || ( flags != SENT_BY_PROXY ) )
                      {
                          bysent = sendto( ucast_sock, pack_buf, byread, 0,
                                           (struct sockaddr *)&addr_list[ i ],
                                           sizeof( struct sockaddr ) );
                          if( bysent < 0 )
                              perror( "sendto" );
                          usleep( 1 );
                      }
                  }
              }
         }
         if( FD_ISSET( ucast_sock, &readfds ) )
         {
              byread = recv( ucast_sock, pack_buf, MAX_PACK_SIZE, 0 );

              if( byread > 0 )
              {
                  memset( &group_addr, 0, sizeof( group_addr ) );
                  group_addr.sin_family      = AF_INET;
                  group_addr.sin_port        = htons(config.mcast_port);
                  group_addr.sin_addr.s_addr = inet_addr( config.group_addr );

                  if( config.enable_loopback == 1 )
                      pack_buf[ FLAGS ] = SENT_BY_PROXY;

                  bysent = sendto( mcast_sock, pack_buf, byread, 0,
                                   (struct sockaddr *) &group_addr,
                                   sizeof( struct sockaddr ) );
                  if( bysent < 0 ){
                      perror( "sendto" );
                  }
                  usleep( 1 );
              }
         }
    }
    close_mcast_sock( mcast_sock, &group_addr );
    close( ucast_sock );

    return EXIT_SUCCESS;
}

int read_config_file( FILE * fp, Config * config )
{
    char line[100];
    char *temp;
    int  i;

    if( fgets( line, 100, fp ) == NULL )
        return -1;

    if( ( temp = strchr( line, '=' ) ) == NULL )
        return -1;

    temp++;
    if( temp[ strlen( temp ) - 1 ] == '\n' )
        temp[ strlen( temp ) - 1 ] = '\0';

    config->group_addr = strdup( temp );

    if( fgets( line, 100, fp ) == NULL )
        return -1;

    if( ( temp = strchr( line, '=' ) ) == NULL )
        return -1;

    temp++;
    config->n_addr    = atoi( temp );
    config->addr_list = ( char ** ) malloc( sizeof(char *) * config->n_addr );

    if( fgets( line, 100, fp ) == NULL )
        return -1;

    /* We will read one IP address or hostname per line */
    for ( i = 0; i < config->n_addr; i++ )
    {
        if( fgets( line, 100, fp ) == NULL )
            return -1;

        line[ strlen( line ) - 1 ] = '\0';
        config->addr_list[ i ]     = strdup( line );
    }

    if( fgets( line, 100, fp ) == NULL )
        return -1;

    if( ( temp = strchr( line, '=' ) ) == NULL )
        return -1;
    temp++;
    config->ttl = (char) atoi( temp );

    if( fgets( line, 100, fp ) == NULL )
        return -1;

    if( ( temp = strchr( line, '=' ) ) == NULL )
        return -1;
    temp++;
    config->reuse_addr = atoi( temp );

    if( fgets( line, 100, fp ) == NULL )
        return -1;

    if( ( temp = strchr( line, '=' ) ) == NULL )
        return -1;
    temp++;
    config->enable_loopback = (char) atoi( temp );

    if( fgets( line, 100, fp ) == NULL )
        return -1;

    if( ( temp = strchr( line, '=' ) ) == NULL )
        return -1;
    temp++;
    config->ucast_port = atoi( temp );

    if( fgets( line, 100, fp ) == NULL )
        return -1;

    if( ( temp = strchr( line, '=' ) ) == NULL )
        return -1;
    temp++;
    config->mcast_port = atoi( temp );

    return 0;
}

void re_read_config_file( int signo )
{
    if( ( fp = fopen( mcastproxy_config_file, "r" ) ) == NULL )
    {
         fprintf( stderr, "\n[mcastproxy Error] Opening configuration file '%s'\n", mcastproxy_config_file );
         exit( MP_ERROR_OPENCONFFILE );
    }
    if( read_config_file( fp, &config ) < 0 )
    {
        fprintf( stderr, "\n[mcastproxy Error] Invalid new configuration file." );
        exit( MP_ERROR_INVALIDCONFFILE );
    }
}

int create_ucast_socket( Config * config )
{
    int    sckd;
    struct sockaddr_in addr;

    memset( &addr, 0, sizeof( addr ) );

    sckd = socket( AF_INET, SOCK_DGRAM, 0 );

    if( sckd > 0 )
    {
        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port        = htons( config->ucast_port );

        if( bind( sckd, (struct sockaddr *) &addr, sizeof( addr ) ) < 0 )
        {
            perror( "Binding" );
            return -1;
        }
    }

    return sckd;
}

int create_mcast_socket( Config * config , struct sockaddr_in * group_addr)
{
    int sckd;
    unsigned char byte;           /* variable used to enable/disable loopback */
    struct sockaddr_in groupHost; /* multicast group host info structure */
    struct ip_mreq mreq;          /* multicast group info structure */

    byte = 1;

    /* Get the multicast group host information */
    groupHost.sin_family      = AF_INET;
    groupHost.sin_port        = htons(config->mcast_port);
    groupHost.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Allocate a UDP socket and set the multicast options */
    if( ( sckd = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
    {
        perror( "create socket" );
        return -1;
    }

    /* allow multipule processes to bind to same multicast port */
    if( setsockopt( sckd, SOL_SOCKET, SO_REUSEADDR, &(config->reuse_addr), sizeof( config->reuse_addr ) ) == -1 )
    {
        perror( "setsockopt SO_REUSEADDR" );
        return -1;
    }

    /* bind the UDP socket to the mcast address to recv messages from the group */
    if( bind( sckd,(struct sockaddr *) &groupHost, sizeof( groupHost ) ) < 0 )
    {
        perror( "bind" );
        return -1;
    }

    byte = config->ttl;
    if( setsockopt( sckd, IPPROTO_IP, IP_MULTICAST_TTL, &byte, sizeof( byte ) ) < 0 )
    {
        perror( "setsockopt IP_MULTICAST_TTL" );
        return -1;
    }

    byte = config->enable_loopback;  /*enable loopback*/
    if( setsockopt( sckd, IPPROTO_IP,IP_MULTICAST_LOOP, (char *)&byte, sizeof( byte ) ) < 0 )
    {
        perror( "setsockopt IP_MULTICAST_LOOP" );
        return -1;
    }

    /* check if group address is indeed a Class D address */
    mreq.imr_multiaddr        = group_addr->sin_addr;
    mreq.imr_interface.s_addr = INADDR_ANY;

    if ( setsockopt( sckd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                     (char *) &mreq, sizeof( mreq ) ) < 0 )
    {
        perror( "setsockopt IP_ADD_MEMBERSHIP" );
        return -1;
    }

    return sckd;
}

int convert_addr( Config * config, struct sockaddr_in * group_addr,
                struct sockaddr_in ** addr_list )
{
    struct hostent *host;
    int i;

    memset( group_addr, 0, sizeof( *group_addr ) );
    group_addr->sin_family      = AF_INET;
    group_addr->sin_port        = htons( config->mcast_port );
    group_addr->sin_addr.s_addr = inet_addr( config->group_addr );
    (*addr_list) = (struct sockaddr_in *) malloc( config->n_addr *
                                                  sizeof( struct sockaddr_in ) ) ;
    for( i = 0; i < config->n_addr; i++ )
    {
        memset( &((*addr_list)[ i ]), 0, sizeof( (*addr_list)[ i ] ) );
        (*addr_list)[ i ].sin_family = AF_INET;
        (*addr_list)[ i ].sin_port   = htons( config->ucast_port );

        printf( "\tAdding host %s ...", config->addr_list[ i ] );
        host = gethostbyname( config->addr_list[ i ] );
        if( host == 0 )
        {
            fprintf( stderr, "\n[mcastproxy Error] Resolving host '%s'\n", config->addr_list[ i ] );
            return -1;
        }
        printf( " OK!\n" );

        memcpy( &((*addr_list)[ i ].sin_addr), host->h_addr, host->h_length );
    }
    return 0;
}

void close_mcast_sock( int mcast_sock, struct sockaddr_in * group_addr )
{
    struct ip_mreq dreq;  /* multicast group info structure */

    dreq.imr_multiaddr        = group_addr->sin_addr;
    dreq.imr_interface.s_addr = INADDR_ANY;

    setsockopt( mcast_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
              (char *) &dreq,sizeof( dreq ) );
}

void showHosts( int signo )
{
    int i;

    for( i = 0; i < config.n_addr; i++ )
        fprintf( stdout, "Host %2d: %s\n", i, config.addr_list[ i ] );
    return;
}
