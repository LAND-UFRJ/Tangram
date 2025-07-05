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
        Last Update: 12/02/2004 - hugosato - added includes

******************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include "common.h"
#include "calc_loc.h"




/* global variables */


/* Type of generated packet */
t_gen_packet   *packet;


/* remote host information */
char   desthost[MAXSTRING];
char   port[MAXSTRING];

/* Descriptor to handle socket */
int    soc, soc2;

/* Number of packets */
int num_pack;

/* output file */
char   filename[MAXSTRING];

int usage(char *filename)
{
    printf ("Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("Usage: \n");
    printf ("<dest host> <port> <packets> <output>\n");
    printf ("where: \n");
    printf ("\t<dest host> \t\t\t\t\t host destination\n ");
    printf ("\t<port>      \t\t\t\t\t port number\n");
    printf ("\t<packets>   \t\t\t\t\t number of packets for each size\n");
    printf ("\t<output>    \t\t\t\t\t output file name\n");    
    return (1);
}

int parse_args(char **argv) 
{
  int i;
  
  i = 1;

  strcpy (desthost, argv[i]);
  i++;
  
  if ( (atoi(argv[i])) < 1024) {
    printf ("Invalid port: %s\n", argv[i]);
    return (-1);
  } 
  else
    strcpy (port, argv[i]);
  
  i++;
  
  if ( (atoi(argv[i])) < 0) {
    printf ("Invalid Number of Packets: %s\n", argv[i]);
    return (-1);
  }
  else 
    num_pack = atoi(argv[i]);
  
  i++;
  strcpy (filename, argv[i]);
  
  return (1);
}

/* Functions to initialize and clean struct of packet */
void clean_struct(void){
  free((void *)packet);
}

void initialize_packet(void){
    /* This is just to leave the variable allocated at the memory heap part */

    packet = (t_gen_packet*)malloc(sizeof(t_gen_packet)+1);
    packet->nseq  = 0;
    packet->type= DATA;
    packet->timestamp_tx = packet->timestamp_rx = 0;
}

inline int send_packet(int packsize)
{
  /* Increments the Number of sequence of each packet */
  packet->nseq++;
    
  /* Timestamp of packet outgoing */
  packet->timestamp_tx = get_time();
  packet->timestamp_rx = 0;
    
  if (send (soc, (char * )packet, packsize, 0) < 0)
     ;

  return(1);
}



int generate_traffic()
{
    int i, j;
    float packetsize;
    
/* Because all data links protocols supported by IP are supposed to have MTU of
at least 576 bytes, fragmentation is entire eliminated using MSS of 536 bytes */    

    for ( j = 1; j <= num_pack ; j++) {
      packetsize = 12.8;
      for ( i = 1; i <= 4 ; i++) {
	packetsize = packetsize * 2.5 ;
	if  ((j==num_pack)&&(i==4))
	{
          packet->type= END;
	} else
	{
          packet->type= DATA;
	}
	packet->nseq= (((j-1)*4)+i-1);
        usleep(100000);
	send_packet(packetsize);
      }
    }
        
    return ( TRUE );
}

int receive_traffic()
{

  /* Packet to receive data */
  t_gen_packet *pack;
  int end, byread;

  
  /* files with to analize */
  FILE *fsize;

  
  end = 0;

  /* This part is to allocate the packet */
  pack = (t_gen_packet*)malloc(sizeof(t_gen_packet)+1);

  /* Trying to create out file of all sizes*/
  if( !(fsize = fopen( filename ,"w+" ) ) )
  {
      printf("\n\tError: Can't create out file!\n");
      return( -1 );
  }

  while ( (!end) ){  
    byread = 0;

    /* system call to receive the packet */
    if ( (byread = recv(soc2,(char *)pack, sizeof(t_gen_packet), 0) ) < 0 )
      fprintf( stderr , "errno %d\n", errno );

    pack->timestamp_rx = get_time();
    
    fprintf(fsize, "%lld \t", pack->nseq);
    fprintf(fsize, "%7.0f \t", pack->timestamp_tx);
    fprintf(fsize, "%7.0f \t", pack->timestamp_rx - pack->timestamp_tx);
    fprintf(fsize, "%4d \n", byread  );
    fprintf(stderr, "#" );
    
    if ( pack->type == END ){
      /* end of transmission received */
      fprintf( stderr, "\nReceived a terminate message from traffic generator.\n" );
      end = 1;
    }
    }


    fclose(fsize);

    return ( TRUE );
}


int main (int argc, char **argv)
{
  int pid;

  /* Packet to receive data */
  t_gen_packet *init_pack;


  init_pack = NULL;
  /* check for the number of arguments */
  if( argc < 5 )
  {
    usage(argv[0]);
    return (-1);
  }
  
  /* parse the arguments */
  if (parse_args(argv) < 0) {
    usage(argv[0]);
    return (-1);
  }

  srand48 ( (long int) get_time()  );

  initialize_packet();
  
  
  /* create the socket */
  if ( (soc = connectUDP (desthost, port)) < 0) {
    perror ("socket creation");
    return (-1);
  }
  
  
  if ( (soc2 = passiveUDP (port, 0)) < 0) {
    perror("Cannot create socket");
    return (-1);
  }

  /* This part is to allocate the initial packet */
  init_pack = (t_gen_packet*)malloc(sizeof(t_gen_packet)+1);    
  init_pack->type= INIT;
  send_packet(28);

  if ((pid = fork()) > 0)
  {
     /* generate the traffic  */
     usleep(100000);
     generate_traffic();  
  }
  else
  {     
     /* receive the traffic  */
     fprintf( stderr, "Traffic Receiver is ready to collect trace.\nWaiting for packets from generator...\n" );
     receive_traffic();
  }
  
    
  clean_struct();
  
  fflush(stdout);

  return 0;  
  
}  
