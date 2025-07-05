/*
 * Copyright (C) 1999-2009, Edmundo Albuquerque de Souza e Silva.
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

/******************************************************************************

    Author: Magnos Martinello
    Tool: Traffic Generator & Receiver
    LAND - Labor. de Analise e Desempenho - UFRJ/COPPE/COS
        Last Update: 22/11/2000

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#ifdef _SOLARIS_
#include <strings.h>
#endif

#include "common.h"

/* u_short htons(), ntohs(); */

u_short portbase = 0;

#ifndef INADDR_NONE
#define INADDR_NONE    0xffffffff
#endif

int passivesock( char *service, char *protocol, int qlen )
{
    struct servent *pse;
    struct protoent *ppe;
    struct sockaddr_in sin;
    int s,type;
    int one = 1;

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    if( ( pse = getservbyname( service, protocol ) ) )
        sin.sin_port = htons( ntohs( (u_short)pse->s_port) + portbase );
    else if( !( sin.sin_port = htons( (u_short)atoi( service ) ) ) )
    {
        printf( "Can't get \"%s\" service entry\n", service );
        exit( 1 );
    }

    if( ( ppe = getprotobyname( protocol ) ) == 0 )
    {
        printf( "can't get \"%s\" protocol entry\n", protocol );
        exit( 1 );
    }

    if( strcmp( protocol, "udp" ) == 0 )
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    s = socket( PF_INET, type, ppe->p_proto );
    if( s<0 )
    {
        printf( "can't create socket!\n" );
        exit( 1 );
    }

    if( setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one) ) < 0 )
    {
        perror( "setsockopt" );
        return( -1 );
    }

    if( bind( s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 )
    {
        printf( "can't bind to %s port!\n", service );
        exit( 1 );
    }

    if( type == SOCK_STREAM && listen( s, qlen ) < 0 )
    {
        printf( "can't listen on %s port!\n", service );
        exit( 1 );
    }

    return s;
}

int passiveUDP( char *service, int qlen )
{
    return passivesock( service, "udp", qlen );
}

int passiveTCP( char *service, int qlen )
{
    return passivesock( service, "tcp", qlen );
}

int connectsock( char *host, char *service, char *protocol )
{
    struct hostent *phe;
    struct servent *pse;
    struct protoent *ppe;
    struct sockaddr_in sin;
    int s,type;

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;

    if( ( pse = getservbyname( service, protocol ) ) )
        sin.sin_port = pse->s_port;
    else if( ( sin.sin_port = htons( (u_short)atoi( service ) ) ) == 0 )
    {
        printf( "Can't get \"%s\" service entry\n", service );
        exit( 1 );
    }

    if( ( phe = gethostbyname( host ) ) )
        bcopy( phe->h_addr, (char *)&sin.sin_addr, phe->h_length );
    else if( ( sin.sin_addr.s_addr = inet_addr(host) ) == INADDR_NONE )
    {
        printf( "Can't get \"%s\" host entry\n", host );
        exit( 1 );
    }

    if( ( ppe = getprotobyname(protocol) ) == 0 )
    {
        printf( "Can't get \"%s\" protocol entry\n", protocol );
        exit( 1 );
    }

    if( strcmp( protocol, "udp" ) == 0 )
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    s = socket( PF_INET, type, ppe->p_proto );
    if( s < 0 )
    {
        printf( "Can't create socket!\n" );
        exit( 1 );
    }

    if( connect( s, (struct sockaddr *)&sin, sizeof( sin ) ) < 0 )
    {
        printf( "Can't connect to %s.%s\n", host, service );
        exit( 1 );
    }

    return s;
}

int connectTCP( char *host, char *service )
{
    return connectsock( host, service, "tcp" );
}

int connectUDP( char *host, char *service )
{
    return connectsock( host, service, "udp" );
}


/* Specific get_time() to convert time in units of usec */
/* Function updated at 02/07/2001 */

inline double get_time()
{
    struct timeval time;
    double sec, usec;

    gettimeofday(&time, NULL);
    sec  = time.tv_sec;
    usec = time.tv_usec;
    return (double) ((sec * USEC ) + (usec) );
}


/*
  usleep -- support routine for system call emulations
  delay in microseconds
*/
int usleep( usec )
long usec;
{
    struct timeval delay;

    delay.tv_sec  = usec / 1000000L;
    delay.tv_usec = usec % 1000000L;

    return select( 0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &delay );
}
