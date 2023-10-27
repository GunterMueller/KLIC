/***************************************************************************
                             rmchat.c
                             -------------------
    begin                : February 2002
    copyright            : (C) 2002-2009 by Jorge Allyson Azevedo
                                       Milena Scanferla
                                       Daniel Sadoc
    email                : {allyson,milena,sadoc}@land.ufrj.br

    This program is a simple chat application that uses the Rmcast Library.

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include <rmcast.h> /* Rmcast Library header */

#define BUFFSIZE 200 /* Max buffer size for messages  */

#define TRUE 1
#define FALSE 0

/* Functions */
double generate_sample( float rate );

/* Global Variables */
int chatSock;

int end = 0;
int not_send_first = 1;

void callback_term( void )
{
    fprintf( stderr, "rmchat exiting ...\n" );
    exit( EXIT_SUCCESS );
}

/***************************************************************************
*   generate_sample( float rate )                                           *
*                                                                          *
*   Function to generate random samples with exponetial distribuition.     *
*                                                                          *
****************************************************************************/
double generate_sample( float rate )
{
    double sample,uniform;
    time_t seed;
    struct timeval tv;
    double e,lambda;

    gettimeofday( &tv, NULL );
    seed=time( (time_t *)&tv.tv_sec );
    srandom( (unsigned int)seed );

    e      = exp( 1 );
    lambda = rate;

    uniform = ( (double)random()/(double)RAND_MAX );
    sample  =-( log(uniform)/lambda );
    return sample;
}

/***************************************************************************
*   ReceivePacket                                                          *
*                                                                          *
*   Function (thread) to receive data from the network                     *
*                                                                          *
****************************************************************************/
void *ReceivePacket( void *arg )
{

    char receive_buffer[ BUFFSIZE ];
    int bytes_read;

    while( !end )
    {
        if( ( bytes_read = RM_recv( chatSock, receive_buffer, BUFFSIZE ) ) > 0  )
        {
             /* We have received data from the network, show it to the user */
             fprintf( stdout,"\n%s>", receive_buffer );
             fflush( stdout );
        }
        else
        {
             fprintf( stderr, "[ReceivePacket Error]: receiving data.\n" );
             break;
        }
    }

    return 0;
}

int main( int argc, char **argv )
{
    pthread_t tid; /* Thread Id */
    int port;
    struct timespec sleep_time;
    char rmcast_version[ 10 ];
    char buffer[ BUFFSIZE ];
    char aux[ 16 ];
    char user_name[ 20 ];
    char msg_buff[ BUFFSIZE - 20 ];

    tid  = 0;
    port = 0;

    /* Checking the input parameters */
    if( argc != 2 )
    {
        fprintf( stderr, "Usage:\n\t%s <config_file>\n", argv[ 0 ] );
        exit( EXIT_FAILURE );
    }

    /* Setting and trying to read the configuration file first. */
    if( !RM_readConfigFile( argv[ 1 ], TRUE ) )
    {
        fprintf( stderr, "[rmchat Error]: Couldn't read config file (%s)\n", argv[ 1 ] );
        exit( EXIT_FAILURE );
    }

    /* Initialize the Rmcast Library functions */
    RM_initialize( (void *)callback_term );

/*
 *     Join the Multicast Group.
 *     If you want to set a "hardcoded" Multicast group IP and port replace the
 *     RM_USE_CURRENT_CONFIG constants, otherwise we will use the IP and Port
 *     from the configuration file or from the default options
 */

    chatSock = RM_joinGroup( (char*)RM_USE_CURRENT_CONFIG, RM_USE_CURRENT_CONFIG );

    RM_getVersion( rmcast_version, sizeof( rmcast_version ) );
    fprintf( stdout, "RMCHAT using rmcast version: %s\n", rmcast_version );

    fprintf( stdout,"\nEnter a user name (max 16 chars):\n" );
    scanf( "%s", user_name );
    aux[ 0 ] = '\0';
    strcat( aux, "[" );
    strcat( aux, user_name );
    strcat( aux, "]>" );

    fprintf( stdout, "\n*************\nHello %s!\n", user_name );
    fprintf( stdout, "Now you can send messages to the Multicast Group!\n" );
    fprintf( stdout, "Type \"exit\" if you want to quit the program\n*************\n>" );

    /* Now we will create a thread to receive data from the network */
    if( pthread_create( &tid, NULL, ReceivePacket, NULL ) != 0 )
    {
        /* Error, terminate the application */
        fprintf( stderr, "[rmchat Error]: Could not create Receive Thread\n" );
        RM_leaveGroup( chatSock, (char*)RM_USE_CURRENT_CONFIG );
    }

    while( TRUE )
    {
        buffer[ 0 ] = '\0';

        /* Now we will read the messages from the standard input */
        fgets( msg_buff, BUFFSIZE - 21, stdin );

        if( strcmp( msg_buff, "exit\n" ) == 0 )
        {
            /* The user wants to terminate the application */
            fprintf( stdout, "\n*************\n" );
            fprintf( stdout, "Exiting rmchat ...\n" );
            break;
        }
        else
        {
            if( strcmp( msg_buff, "send\n" ) == 0 )
            {
                /* Send command: will send packets with a number to the group,
                   just for tests purposes */
                int j;
                int num;
                float rate;
                char aux2[ 10 ];
                long long int aux_sample;
                char dist[ 3 ];

                num  = 0;
                rate = 0;
                aux_sample = 0;

                /* Ask the user how many packets we are supposed to send */
                fprintf( stdout, "Enter:\n DET <num_pack_to_send> <time_between_sendings>\n\tor\n" );
                fprintf( stdout, " EXP <num_pack_to_send> <send_rate>\n" );
                fprintf( stdout, "\t(time in milisseconds and rate in packets/milisseconds)\n" );
                scanf( "%s %d %f", dist, &num, &rate );
                printf( "num=%d rate=%f\n", num, rate );

                for( j = 0; j < num; j++ )
                {
                    /*Fill the message with a number */
                    buffer[ 0 ] = '\0';
                    strcat( buffer, aux );
                    sprintf( aux2, "%d\n", j );
                    strcat( buffer, aux2 );

                    if( ( strcmp( dist, "EXP" ) == 0 ) )
                    {
                        aux_sample = (long long int)( 1000000 * generate_sample( rate ) );
                    }
                    else
                    {
                        aux_sample = (long long int)( rate * 1000000 );
                    }

                    if( aux_sample >= (1000000000) )
                    {
                        sleep_time.tv_nsec = aux_sample % 1000000000;
                        sleep_time.tv_sec  = (long long int)( aux_sample / 1000000000 );
                    }
                    else
                    {
                        sleep_time.tv_nsec = aux_sample;
                        sleep_time.tv_sec  = 0;
                    }

                    /*printf("sleep s=%ld ns=%ld\n",sleep_time.tv_sec,sleep_time.tv_nsec);*/
                    nanosleep( &sleep_time, NULL );

                    /* Send the message to the Multicast Group */
                    RM_sendto( chatSock, buffer, BUFFSIZE );

                    fprintf( stdout, "\n%s>", buffer);
                }
            }
            else
            {
                strcat( buffer, aux );
                strcat( buffer, msg_buff );

                if( !not_send_first )
                    /* Send the message to the Multicast Group */
                    RM_sendto( chatSock, buffer, BUFFSIZE );
                else
                    not_send_first = 0;

                fprintf( stdout, "\n%s>", buffer );
            }
        }
    }

    /* Getting Multicast Group IP and Port from the Rmcast Library */
    RM_getOption( DEST_IP, (void *)aux );
    RM_getOption( DEST_PORT, (void *)&port );

    /* Stop the Receive thread */
    pthread_cancel( tid );

    /* Leave the Multicast Group */
    fprintf( stdout, "Leaving Multicast Group %s:%d \n", aux, port );
    RM_leaveGroup( chatSock, (char*)RM_USE_CURRENT_CONFIG );

    return 0;
}
