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
	Tool: Traffic Generator
	LAND - Labor. de Analise e Desempenho - UFRJ/COPPE/COS
        Last Update: 09/04/2001

******************************************************************************/
#include <time.h> 

#ifndef __COMMON__
#define __COMMON__


/* Constants to types of packets */
#define INIT    'I'     /* Control type to initinalize the session */
#define END     'E'     /* Control type to finalize the session */
#define DATA    'D'     /* Data type */
#define KILLALL 'K'


//#ifndef _SOLARIS_
typedef unsigned long long int u_longlong_t;
//#endif

#define MINPACKSIZE  28
#define HEADERSIZE   28


/* #define MAXPACKSIZE  ( (64*1024) - MINPACKSIZE ) */

/* Size for specification of user UDP frame datagram   */ 
#define	MAXPACKSIZE  65476


#define USEC         1000000


typedef struct
{
    u_longlong_t nseq;                  /* Sequence number */
    char         type;                  /* Type of packet  */
    double       timestamp_tx;          /* Timestamp in usec */
    double       timestamp_rx;          /* Timestamp in usec */
    char         payload[MAXPACKSIZE];  /* payload */
} t_gen_packet;



/* prototype of a particular get_time() in usec */
inline double get_time( void );

int connectsock( char *, char *, char * );
int connectTCP( char *, char * );
int connectUDP( char *, char * );

int passivesock( char *, char *, int );
int passiveUDP( char *, int );
int passiveTCP( char *, int );

#endif
