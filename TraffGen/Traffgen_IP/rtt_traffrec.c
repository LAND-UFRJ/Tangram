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
	
	Author: Antonio Augusto Rocha
	Tool: Traffic Generator
	LAND - Laboratorio de Analise e Desempenho - UFRJ/COPPE/COS
        Last Update: 30/07/2002

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "rtt_traffrec.h"
#include "common.h"
#include <errno.h>
#include <sys/socket.h>
#include <string.h>

/* global variables */

/* Type of generated packet */
t_gen_packet   *packet;


/* remote host information */
char   desthost[MAXSTRING];
char   port[MAXSTRING];

/* Descriptor to handle socket */
int    soc, soc2;

/* Silent Mode - RTT Receiver wont reply ZERO PACKET */ 
int silent;

int usage(char *filename)
{
    printf ("Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("Usage: \n");
    printf ("<dest host> <port> [-s]\n");
    printf ("where: \n" );
    printf ("\t<dest host> \t\t\t host destination\n ");
    printf ("\t<port>      \t\t\t port number\n");
    printf ("\t[-s]        \t\t\t silent handshake mode\n");
        
    return (1);
}


int parse_args( int argc , char **argv ) 
{
  int i;
  
  i = 1;
  strcpy (desthost, argv[i]);
  
  i++;  
  if ( (atoi(argv[i])) < 1024) 
  {
     printf ("Invalid port: %s\n", argv[i]);
     return (-1);
  } 
  else
     strcpy (port, argv[i]);
    
  if( argc > 3 )
  {
     i++;
     if( !strcmp( "-s" , argv[i] ) ) 
     {
         fprintf( stderr , "Silent handshake.\n" );
         silent = 1;
     }
     else
     {
         printf( "Invalid argument: %s\n", argv[i]);
         return( -1 );
     }
  }
  
  return (1);
}


/* Functions to initialize and clean struct of packet */
void clean_struct(void){
  free((void *)packet);
}


void initialize_packet(void){
    /* This is just to leave the variable allocated at the memory heap part */

    packet = ( t_gen_packet* ) malloc( sizeof( t_gen_packet ) + 1 );
    packet->nseq  = 0;
    packet->type = DATA;
    packet->timestamp_tx = packet->timestamp_rx = 0;
}


int receive_traffic()
{

  /* Temporary Packet to receive data */
  t_gen_packet *pack; 
  
  int end, byread;
    
  end = 0;

  /* This part is to allocate the packet */
  pack = (t_gen_packet*)malloc(sizeof(t_gen_packet)+1);


   while ( ( !end ) )
   {  
       byread = 0;

       /* system call to receive the packet */
       if ( ( byread = recv( soc ,(char *)pack , sizeof( t_gen_packet ), 0 ) ) < 0 )
         fprintf( stderr , "errno %d\n", errno );
       else fprintf( stderr, "#" );       
       
       /* test if the received packet is the FINISH HIM packet! */
       if ( pack->type == END )
       {
         /* end of transmission received */
         fprintf( stderr, "\nReceived a terminate message from traffic generator.\n" );
         end = 1;
       }

       /* debug fprintf( stderr,"%lld\t%c\t%e\t%e\n" , pack->nseq , pack->type ,pack->timestamp_tx, pack->timestamp_rx );*/

       /* retransmit the packet: ROUND TRIP
          Timestamp of packet outgoing */
       packet->nseq = pack->nseq;
       packet->type = pack->type;    
       packet->timestamp_rx = pack->timestamp_rx;
       packet->timestamp_tx = pack->timestamp_tx;
       strcpy(packet->payload,pack->payload);       
       if( ( silent == 1 ) ) 
       {
         if( pack -> nseq != 0 )
         { 
            if ( send (soc2, (char * )packet, byread, 0 ) < 0 )   
               fprintf( stderr , "Error retransmitting the packet # %lld\n" , packet->nseq );
            else fprintf( stderr, "*" );  
         }
       }
       else 
       {
         if ( send (soc2, (char * )packet, byread, 0 ) < 0 )
            fprintf( stderr , "Error retransmitting the packet # %lld\n" , packet->nseq );
         else fprintf( stderr, "*" );  
       }

   }
   return ( TRUE );
}


int main (int argc, char **argv)
{  
  
  /* check for the number of arguments */
  if (argc < 3 ) {
    usage(argv[0]);
    return (-1);
  }
  
  /* parse the arguments */
  if (parse_args( argc , argv ) < 0) {
    usage(argv[0]);
    return (-1);
  }

  fprintf( stderr, "Traffic Receiver is ready to collect trace.\nWaiting for packets from generator...\n( # packets in , * packets out )\n" );

  initialize_packet();
  
  if ( (soc = passiveUDP (port, 0)) < 0) {
    perror("Cannot create socket");
    return (-1);
  }

  /* create the socket */
  if ( (soc2 = connectUDP (desthost, port)) < 0) {
    perror ("socket creation");
    return (-1);
  }

  /* receive the traffic  */
  receive_traffic();

  clean_struct();
  
  fflush(stdout);

  return 0;
}
