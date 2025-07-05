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

#include "mcast.h"

void mcastReusePort( int soc )
{
    int one = 1;

    if( setsockopt( soc, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one)) == -1)
    {
        fprintf( stderr, "mcastReusePort: ERROR - setsockopt\n" );
        exit( 1 );
    }
}

void mcastSetTTLValue( int soc, u_char *i )
{
    if( setsockopt( soc, IPPROTO_IP, IP_MULTICAST_TTL,
                    (char *)i, sizeof(u_char)
                  ) == -1)
    {
        fprintf( stderr, "mcastSetTTLValue: ERROR - setsockopt\n");
    }

}

void mcastSetLoopback( int soc, u_char loop )
{
    if( setsockopt( soc, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loop,
                    sizeof(u_char) ) == -1 )
    {
        fprintf(stderr, "mcastSetLoopback: ERROR - setsockopt\n");
    }
}

int mcastJoinGroup( char *group, int port, struct sockaddr_in *mcast_sockaddr )
{
    u_char loop;
    int soc;
    u_char TimeToLive;
    struct sockaddr_in groupHost, groupStruct;
    struct ip_mreq mreq;

    TimeToLive = DEFAULT_MULTICAST_TTL_VALUE;

    fprintf( stdout, "Joining mutilcast group %s port: %d\n", group, port );

    groupHost.sin_family      = AF_INET;
    groupHost.sin_port        = htons( port );
    groupHost.sin_addr.s_addr = htonl( INADDR_ANY );

    soc = socket( PF_INET, SOCK_DGRAM, 0 );
    if( soc < 0 )
    {
        fprintf( stderr, "mcastJoingGroup: ERROR - Could not create multicast socket\n");
        exit( 1 );
    }

    mcastReusePort( soc );
  
    if( ( bind( soc, (struct sockaddr *)&groupHost, sizeof(groupHost) ) == -1 ) )
    {
        perror( "mcastJoingGroup: ERROR - bind\n" );
        exit( 1 );
    }

    mcastSetTTLValue( soc, &TimeToLive );
    loop = 1;
    mcastSetLoopback( soc, loop );

    if( ( groupStruct.sin_addr.s_addr = inet_addr( group ) ) == -1 )
        fprintf( stderr, "mcastJoinGroup: ERROR - inet_addr, Invalid group IP.\n");

    mreq.imr_multiaddr = groupStruct.sin_addr;
    mreq.imr_interface.s_addr = INADDR_ANY;

    if( setsockopt( soc, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq,
                    sizeof(mreq) ) == -1 )
    {
        fprintf( stderr, "mcastJoinGroup: ERROR - setsockopt\n" );
        exit(1);
    }

    mcast_sockaddr->sin_family      = AF_INET;
    mcast_sockaddr->sin_port        = htons( port );
    mcast_sockaddr->sin_addr.s_addr = inet_addr( group );

    return soc;
}

void mcastLeaveGroup( int sock, char *group )
{
    struct sockaddr_in groupStruct;
    struct ip_mreq dreq;

    if( ( groupStruct.sin_addr.s_addr = inet_addr( group ) ) == -1)
        fprintf( stderr, "mcastLeaveGroup: ERROR - inet_addr\n");

    dreq.imr_multiaddr        = groupStruct.sin_addr;
    dreq.imr_interface.s_addr = INADDR_ANY;

    if (setsockopt( sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&dreq,
        sizeof(dreq)) == -1)
    {
        fprintf( stderr, "mcastLeaveGroup: ERROR - setsockopt. Terminating anyway.\n" );
    }

    close( sock );
}

