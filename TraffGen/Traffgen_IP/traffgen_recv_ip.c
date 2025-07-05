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
	Modified: Antonio Augusto Rocha
	Tool: Traffic Receiver
	LAND - Laboratorio de Analise e Desempenho - UFRJ/COPPE/COS
        Last Update: 6/06/2003
        Last changes: Output file and new output file layout

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include "mcast.h"
#include "common.h"
#include "traffgen_recv_ip.h"

#define UNICAST   1
#define MULTICAST 2

/* global variables */
int  soc;

/* Counters to received packets */
u_longlong_t norecv = 0;    


/* Initial time */
double start_time ;

/* Multicast */
int traff_type = UNICAST;
struct sockaddr_in mcast_sockaddr;

FILE *out;  

void usage()
{
    fprintf( stderr, "Traffic Receiver v3.1 - Multicast enabled - Copyright (C) 1999-2006\n");
    fprintf( stderr, "Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    fprintf( stderr, "LAND - Laboratory for Modeling and Development of Networks\n");
    fprintf( stderr, "\nUsage: \n");
    fprintf( stderr, "\n  Unicast:\n");
    fprintf( stderr, "    traffgen_recv_ip <port> <output_file> \n");  
    fprintf( stderr, "      <port> UDP listening port \n");
    fprintf( stderr, "      <output_file> Output File \n");
    fprintf( stderr, "\n  Multicast:\n");
    fprintf( stderr, "    traffgen_recv_ip <group_IP> <port> <output_file> \n");  
    fprintf( stderr, "      <group_IP> IP address of the multicast group\n");
    fprintf( stderr, "      <port> UDP listening port \n");
    fprintf( stderr, "      <output_file> Output File \n");
    fprintf( stderr, "\n" );
    return;
}

/* Function updated by Magnos Martinello 09/04/2001 */
/* Status messages included by Fernando Jorge 11/04/2001 */
/* Modified by Hugo 06/06/2003 */
void collect_trace( void )
{  
     /* Packet to receive data */
     t_gen_packet *pack;

     int end    = 0;
     int byread = 0;
     unsigned int len = 0;

     /* This part is to allocate the packet */
     pack = (t_gen_packet*)malloc(sizeof(t_gen_packet)+1);

     fprintf( stderr, "Traffic Receiver is ready to collect trace.\nWaiting for packets from generator...\n" );

     while( ( !end ) )
     {  
         byread = 0;

         /* system call to receive the packet depending on traff_type */
         if( traff_type == MULTICAST )
         {
             len = sizeof(mcast_sockaddr);
             if( ( byread = recvfrom( soc, (char *)pack, sizeof(t_gen_packet), 0,
	                     (struct sockaddr *)&mcast_sockaddr,
                          &len ) ) < 0 )
                 perror( "mcast recvfrom" );
         }
         else
         {
             if ( ( byread = recv(soc,(char *)pack, sizeof(t_gen_packet), 0 ) ) < 0 )
                 perror( "unicast recv" );
         }

         if( pack->type != END )
         {
             /* Increments the counter of packets really received  */
             norecv += 1;

             /* Initial time must begin when the first packet arrives */ 
             //if ( norecv == 1 ) start_time = get_time();

             //pack->timestamp_rx = get_time() - start_time ;	  

             fprintf( out, "  %lld \t\t", pack->nseq );
             fprintf( out, " %7.0f \t\t" , pack->timestamp_tx );
             
             pack->timestamp_rx = get_time();         
             
             fprintf( out, " %7.0f \t\t" , pack->timestamp_rx - pack->timestamp_tx );
             fprintf( out, " %d \n", byread );

             fprintf( stderr, "#" );
         }
         else 
         {
             /* end of transmission received */
             fprintf( stderr, "\nReceived a terminate message from traffic generator.\n" );
             end = 1;
         }
     }

    return;
  
}

int main (int argc, char **argv)  
{
    char filename[ 255 ];
    char port[ 6 ];
    char mcast_group[ 16 ];

    if( argc < 3 )
    {     
        usage();
        return( -1 );
    }
    else
    {
      if( argc == 3 )
      {
          traff_type = UNICAST;
          strcpy( port, argv[ 1 ] );
          strcpy( filename, argv[ 2 ] );
      }
      else
      {
          traff_type = MULTICAST;
          strcpy( mcast_group, argv[ 1 ] );
          strcpy( port, argv[ 2 ] );
          strcpy( filename, argv[ 3 ] );
      }
    }

    if ( ( out = fopen( filename , "w+" ) ) )
    {
        fprintf( out, "#Received Packet Statistics: \n" );  
        fprintf( out, "#Seq. Number \t" );
        fprintf( out, "Transmition Time (us) \t\t" );
        fprintf( out, "Delay (us) \t " );
        fprintf( out, "   Size (bytes)\n" );     
    }
    else
    {
        perror( "Cannot create output file\n" );
        return( -1 );
    }

    if( traff_type == UNICAST )
    {
        if ( ( soc = passiveUDP( port, 0 ) ) < 0 ) 
        {
          perror( "Cannot create socket\n" );
          return( -1 );
        }
    }
    else
    {    
        if( ( soc = mcastJoinGroup( mcast_group, atoi( port ), &mcast_sockaddr ) ) < 0 )
        {
          perror( "Cannot create multicast socket\n" );
          return( -1 );
        }
    }
    collect_trace();

    return( 0 );
}
