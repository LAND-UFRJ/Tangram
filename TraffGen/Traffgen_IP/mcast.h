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

#ifndef MCAST_H 
#define MCAST_H 
 
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <math.h> 
 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <signal.h> 
#include <fcntl.h> 
#include <sys/ioctl.h> 
#include <sys/time.h> 
#include <sys/uio.h> 
#include <netinet/in.h> 
#include <netdb.h> 
#include <pwd.h> 
#include <arpa/inet.h>
#include <unistd.h>

#include <errno.h>

#define DEFAULT_MULTICAST_TTL_VALUE	255 

/* Global variables */
//int mcastSocket = -1;

/* Functions */
void mcastReusePort( int soc );
void mcastSetTTLValue( int soc, u_char *i );
void mcastSetLoopback( int soc, u_char loop );
int  mcastJoinGroup( char *group, int port, struct sockaddr_in *mcast_sockaddr );
void mcastLeaveGroup( int sock, char *group );

#endif 
