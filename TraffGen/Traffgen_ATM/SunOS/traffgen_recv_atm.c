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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>

#include "common.h"
#include "traffgen_recv_atm.h"

/* global variables */

/* variable for errno */
extern int errno;
 
/* Descriptor to handle socket */
int    soc;

/* Variable used to define the class of traffic */
t_traff  traff_class;

/* Type of service class */
int service_class;

/* Packet size */
int packetsize ;

void usage(char *filename)
{
    printf ("traffgen_recv v1.0 \n");
    printf ("Usage: ");
    printf ("%s <vpi> <vci> <packetsize> \n", filename);
    printf ("\t<vpi> Virtual path identifier \n");
    printf ("\t<vci> Virtual circuit identifier \n");
    printf ("\t<packetsize> Size of packet \n");
    
    return;
}
    
inline u_int get_time()
{
    
    struct timeval time;
    
    gettimeofday(&time, NULL);
    return ((time.tv_sec * 1000) + (time.tv_usec / 1000 ) );
}

void print_vector( t_store *index){
    
    printf("Nseq = %lld \n", index->nseq);
    printf("Type = %c \n", index->type);
    printf("ts_tx = %lld \n", index->timestamp_tx);
    printf("tx_rx = %lld \n", index->timestamp_rx);
	   
}




void generate_statistics( t_store *vec ){

    /* Cumulative  variables */
    double inter_arrival_time        = 0;
    double delay_time                = 0;
    double cumul_inter_arrival       = 0;
    double cumul_delay               = 0;
    double cumul_2nd_moment_tdelay   = 0 ;
    double cumul_2nd_moment_iarrival = 0 ; 
    double mean_iarr, mean_tdelay, var_iarr, var_tdelay ;
    double delta_iarrival, delta_tdelay;
    double bandwidth = 0;

    /* Fraction of lost packets */
    double loss_ratio;

    u_longlong_t lost = 0;    
    u_longlong_t total = 0;

    int i = 1;

    while ( (vec[i+1].type != END) && (vec[i+1].type) ){
	print_vector( &vec[i] );   
	
	if ( (vec[i+1].nseq - vec[i].nseq) > 1 ){
	    lost += (vec[i+1].nseq - vec[i].nseq ) - 1;
	    total += (vec[i+1].nseq - vec[i].nseq);
	}
	else{
	    total +=1;
	    inter_arrival_time = (double) (vec[i+1].timestamp_rx - vec[i].timestamp_rx) ; 
	    delay_time = (double) (vec[i].timestamp_rx - vec[i].timestamp_tx ); 
	    cumul_inter_arrival += inter_arrival_time;
	    cumul_delay         += delay_time;
	    cumul_2nd_moment_iarrival += (inter_arrival_time * inter_arrival_time);
	    cumul_2nd_moment_tdelay   += ( delay_time * delay_time );
	}
	i++;
    }


    /* Means */
    mean_iarr = cumul_inter_arrival / (i-1);
    mean_tdelay =  cumul_delay / (i);
    
    /* Variance estimators */
    var_iarr = ( cumul_2nd_moment_iarrival - (2*mean_iarr*cumul_inter_arrival) + ( (i-1)*(mean_iarr * mean_iarr) ) ) / (i-2) ;
    
    var_tdelay = ( cumul_2nd_moment_tdelay - (2*mean_tdelay*cumul_delay) + ( i * mean_tdelay * mean_tdelay ) ) / (i-1);
    

    /* Delta to interval confidence 90% */
    delta_tdelay = (1.64 * sqrt(var_tdelay))/sqrt(i);
    delta_iarrival = (1.64 * sqrt(var_iarr))/sqrt(i-1);

    /* Effective bandwidth */
    bandwidth = (i*8*packetsize); /* total  transmited bits */
    bandwidth /= (vec[i].timestamp_rx - vec[1].timestamp_tx ); /* msec */
    bandwidth *= 1000; /* sec */
    bandwidth /= (1024 * 1024); /* Mbits/sec */
    

    loss_ratio = (double) lost/total ;
    
    
    printf("\n\n Estatistics (confidence interval 90%%):\n\n");
    
    printf("\t Total bits                  = %6.3f \n",(float)(i*8*packetsize) );

    printf("\t Total delay                 = %6.3f \n",(float)(vec[i].timestamp_rx - vec[1].timestamp_tx ) );
    
    printf("\t Total lost                  = %6.3f %% \n\n", (loss_ratio * 100) );
    
    printf("\t Bandwidth                   = %6.2f Mbps\n\n", (float) bandwidth);
	   
    printf("\t Mean Packet delay           = (%6.3f +/- %.3f) ms\n",
	   mean_tdelay, delta_tdelay );
    
    printf("\t Number of samples           = %d\n\n",i);
    
    printf("\t Mean inter packet delay     = (%6.3f +/- %.3f) ms\n",
	   mean_iarr, delta_iarrival );

    printf("\t Number of samples           = %d\n\n",(i-1));
    
}
    
	
void collect_statistics ( void ){
    
    int end, byread, flags;
    
    /* Packet to receive data */
    t_gen_packet *pack;
    
    t_store *vecstore;
    
    /* Counters to number of received packets */
    u_longlong_t norecv = 0;    

    end = 0;

    /* This part is to allocate the packet at the heap memory part */
    pack = (t_gen_packet*)malloc(sizeof(t_gen_packet)+1);
    vecstore = (t_store *)malloc(SIZEVEC * sizeof(t_store)+1);
    
    while ( !end ){
	
	byread = 0;
	
	/* system call to receive the packet */
	if ( (byread = t_rcv(soc,(char *)pack, sizeof(t_gen_packet),&flags ) ) < 0 ) 
	    if( t_errno == 0 )
		continue;
	    else
		printf("t_errno%d \n",t_errno );
	
	/* end of transmission received */
	if ( pack->type == END ){ 
	    printf( " End of collected statistics\n");
	    end = 1;
	}
	
	/* Increments the counter of packets really received  */
	norecv+= 1;
	
	/* Store the packet */
	if ( norecv < SIZEVEC ){
	    vecstore[norecv].type = pack->type;
	    vecstore[norecv].nseq = pack->nseq;
	    vecstore[norecv].timestamp_tx = pack->timestamp_tx;
	    vecstore[norecv].timestamp_rx = get_time();
	    
	}
	
    }	
    
    generate_statistics(vecstore);
}

int main (int argc, char **argv)
{

  int vpi, vci;
  long optlen = 0;
  char *options = NULL;


  if ( argc < 3 ) {
      usage (argv[0] );
      return (-1);
  }
  
    
  vpi = atoi(argv[1]);
  vci = atoi(argv[2]);
  packetsize = atoi(argv[3]);
  
  if( ( soc = open_device( DEVICE ) ) < 0   )
      return (-1);
     
  if( pvc_bind( soc ) < 0 )
      return (-1);

  if( pvc_connect(soc, vpi, vci, options , optlen) < 0) 
      return(-1);
  


  if( options != (char *) NULL) {
      free(options);
  }
    

  collect_statistics ();
  
  /* sending disconnect signal */
  if( pvc_disconnect(soc) < 0) {
      return(-1);
  }
  
  clean_up(soc);

  return 0;

}
