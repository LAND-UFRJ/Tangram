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
#include <tiuser.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>


#include "common.h"
#include "traffgen_atm.h"

/* global variables */

/* Mode of transmission */
int    TXmode;

/* variable for errno */
extern int errno;
 
/* Descriptor to handle socket */
int    soc;

/* Packet size */
uint    packetsize;

/* Name of description traffic model file */
char   filename[MAXSTRING];

/* Initial time */
uint    start_time;

/* Description of traffic model to be generated */
t_source source;

/* Type of traffic model */
int      source_type;

/* Variable used to define the class of traffic */
t_traff  traff_desc;

/* Type of generated packet */
t_gen_packet   *packet;

/* Service class [UBR|VBR|CBR] */
int      service_class ;

/* Life time  expressed in miliseconds */
double lifetime ;        

/* Counter to inter generation times */
u_longlong_t inter_gen_counter;
u_longlong_t samples_counter;
double	moment_2nd_dep_counter; 




void usage(char *filename)
{
    printf ("ATM Traffic Generator v1.0 - pvc\n");
    printf ("Usage: ");
    printf ("%s [-c|-s] [UBR|CBR] <packetsize> [-d|-m|-t] <lifetime> \n", filename);
    printf ("where: [-c|-s] defines how to generate traffic \n");
    printf ("\t -c \t\t\t\t\t\t continuous way \n");
    printf ("\t -s \t\t\t\t\t\t spread     way \n");
    printf ("\t [UBR|CBR] defines the class of traffic \n");
    printf ("\t [UBR] <VPi> <VCi> \t\t\t\t traffic class UBR \n");
    printf ("\t [CBR] <VPi> <VCi> <peak> \t\t\t traffic class CBR <peak> in bytes/s \n");
    printf ("\t <packetsize> \t\t\t\t\t block size used to break large packets\n");
    printf ("\t -d <framesize> <time>  \t\t\t deterministic traffic model \n");
    printf ("\t -m <framesize> <model_file> <reward_file> \t markov traffic model \n");
    printf ("\t -t <tracefile> <data_unit> \t\t\t trace traffic model \n");
    printf ("\t <lifetime> \t\t\t\t\t total time of generation in sec \n");
    return;
}
    
int parse_args(char **argv) 
{
    int i, aux; 
       
    /*Type of packet generation  */
    if ( !(strcmp(argv[1], "-c")) )
	TXmode = CONTIN;
    else 
	if ( !(strcmp(argv[1], "-s")) )
	    TXmode = SPREAD;
	else{
	    printf("\n\t The TXMode must be -c or -s\n");
	    return (-1);
	}
    
    i = 2; 
    
    /*CBR and UBR service classes are implemented */
    if ( !(strcmp(argv[i], "CBR")) ){
	service_class = CBR;
	i++;
	
	if ( (traff_desc.vpi = (atoi(argv[i]) ) ) < 0) {
	    printf("\n\t Error VPi < 0 \n");
	    return(-1);
	}
	i++;
	
	if ( (traff_desc.vci = (atoi(argv[i]) ) ) < 0) {
	    printf("\n\t Error VCi < 0 \n");
	    return(-1);
	}
	i++;

	if ( (traff_desc.peak = (atof(argv[i]) ) ) < 0) {
	    printf("\n\t Error Peak rate < 0 \n");
	    return(-1);
	}
	
    }
    else 
	if ( !(strcmp(argv[i], "UBR")) ){
	    service_class = UBR;
	    i++;
	    
	    if ( (traff_desc.vpi = (atoi(argv[i]) ) ) < 0) {
		printf("\n\t Error VPi < 0 \n");
		return(-1);
	    }
	    i++;
	    
	    if ( (traff_desc.vci = (atoi(argv[i]) ) ) < 0) {
		printf("\n\t Error VCi < 0 \n");
		return(-1);
	    } 
	    
	}
	else{
	    printf("\n\t Just UBR and CBR are available \n");
	    return (-1);
	}
    
    i++;
    
    if ( (packetsize = atoi (argv[i])) == 0 ){
	printf("\n\t Packetsize = 0, something is wrong \n");
	return(-1);
    }
    i++;

    
    if ( packetsize > MAXPACKSIZE) {
	printf ("Invalid packet size: %s\n", argv[2]);
	return (-1);
    } 
    
    
    if ( !(strcmp(argv[i], "-d")) ){
	source_type = DET;
	i++;
    	
	if ( (source.det.data = atof (argv[i])) == 0 ){
	    printf("\n\t Framesize = 0, It must be bigger than zero \n");
	    return(-1);
	}
	i++;
	
	if ( (source.det.interval = (1000 * (atof (argv[i]))) ) == 0 ){
	    printf("\n\t Framesize = 0, It must be bigger than zero \n");
	    return(-1);
	}
    }
    
    else 
	if ( !(strcmp(argv[i], "-m")) ){
	    source_type = MARKOV ;
	    i++;
	    
	    if ( (source.markov.data_unit = (atof (argv[i])) ) == 0 ){
		printf("\n\t Framesize = 0, It must be bigger than zero \n");
		return(-1);
	    }
	    i++;
	    
	    aux = strlen(argv[i]) ;
	    source.markov.model_file = (char *) malloc( (aux + 1) * sizeof(char)); 
	    strcpy ( source.markov.model_file, argv[i] );
	    printf(" %s \n", source.markov.model_file );

	    i++;
	    aux = strlen(argv[i]) ;
	    source.markov.reward_file = (char *) malloc( (aux + 1) * sizeof(char)); 
	    strcpy ( source.markov.reward_file, argv[i] );
	       
	}
	    
	else
	    if ( !(strcmp(argv[i], "-t")) ){
		source_type = TRACE;
		i++;
		
		strcpy( source.file.filename, argv[i] );
		i++;
		
		if ( (source.file.data_unit = (atof (argv[i])) ) == 0 ){
		    printf("\n\t Framesize = 0, It must be bigger than zero \n");
		    return(-1);
		}
	    }
    
	    else{
		printf("\n\t Traffic model error [-d|-m|-t] \n");
		return (-1);
	    }
    i++;
    
    if ( (lifetime = atof (argv[i]) * 1000 ) < 0) {
	printf ("Invalid life time: %s\n", argv[7]);
	return (-1);
    }
    
    return (1);
}


t_elem *initiate_matrix(int no_states)
{    
    t_elem *Matrix;
    int     i;

    Matrix = (t_elem *)malloc((no_states + 1) * sizeof(t_elem)) ;
    if (Matrix != NULL)
	for (i=0; i<=no_states; i++) {
	    Matrix[i].val = 0;
	    Matrix[i].st  = 0;
	    Matrix[i].next = NULL;
	}
    else
	printf ("ERROR: Malloc flipped while allocating Q matrix\n");

    return (Matrix);
}

int put_matrix (t_elem *Matrix, int i, int j, double val)
{
    t_elem *elem;

    elem = (t_elem *) malloc(sizeof(t_elem));
    elem->val = val;
    elem->st = j;
    
    elem->next = Matrix[i].next;
    Matrix[i].next = elem;
    return (1);
}

int uniformize_matrix(t_elem *Matrix)
{
    t_elem *elem;
    double  out_rate;
    int     i;

    for (i=1; i<=source.markov.no_states; i++) {
	elem = Matrix[i].next;
	out_rate = 0;
	while (elem != NULL) {
	    out_rate += elem->val;
	    elem = elem->next;
	}
	Matrix[i].val = out_rate;
	elem = Matrix[i].next;
	while (elem != NULL) {
	    elem->val /= out_rate;
	    elem = elem->next;
	}
    }
    return (1);
}



int read_markov_source()
{
    
    FILE *fd, *fd2;
    int   i, j;
    float flt;
 

    if ( (fd = fopen(source.markov.model_file, "r")) == NULL) {
	perror ("fopen");
	return (-1);
    }
    
    fscanf (fd, "%d\n", &source.markov.no_states);
    source.markov.rate_vec = (double *) malloc ((source.markov.no_states + 1) * 
						sizeof(double));
    source.markov.unif_vec = (double *) malloc ((source.markov.no_states + 1) * 
						sizeof(double));    
   
 
    for (i=1; i<=source.markov.no_states; i++) {
	source.markov.rate_vec[i] = 0;
	source.markov.unif_vec[i] = 0;
    }

     
    if ( (fd2 = fopen(source.markov.reward_file, "r")) == NULL) {
	perror ("fopen");
	return (-1);
    }
    
    while( !feof(fd2) ){
	fscanf(fd2, "%d %f\n",&i,&flt);
	source.markov.rate_vec[i] = flt/ 1000.0;
    }
    
    fclose(fd2);
    
    source.markov.Q = initiate_matrix(source.markov.no_states);
    
    while ( !feof(fd) ){
	fscanf (fd, "%d %d %f\n", &i, &j, &flt);
	put_matrix(source.markov.Q, i, j, flt / 1000.0);
    }
    
    uniformize_matrix(source.markov.Q);

    fclose (fd);

    return (1);	
}

inline int send_packet(int packsize)
{

    if (packsize == 0) 
	return(0);
    
    if ( packsize < MINPACKSIZE )
	packsize = MINPACKSIZE;
    
    /* Increments the Number of sequence of each packet */
    packet->nseq++;
    
    /* Timestamp of packet outgoing */
    packet->timestamp_tx = (u_longlong_t) get_time();
    packet->timestamp_rx = 0;
    
    
    if ( t_snd(soc, (char *)packet, packsize, 0 ) < 0 )
	printf("Errno=%d\n",errno);
	
    return (1);
    
}


inline uint get_time()
{
    
    struct timeval time;
    
    gettimeofday(&time, NULL);
    return ((time.tv_sec * 1000) + (time.tv_usec / 1000 ) );
}




inline int transmit_interval(int data, int interval)
{
    int    start_interval;
    int    curr_time;
    int    end_interval;
    int    time_slot;
    int    residual_time;
    int    overrun;
    u_longlong_t  i;

    start_interval = get_time();
    
    if (TXmode == CONTIN) {
	
	for (i = 0; i < (data / packetsize) ; i++) {
	    send_packet(packetsize);
	}
	send_packet (data % packetsize);
	
	printf ("\t Start Time: %d -- \t", start_interval - start_time); 
	
	end_interval = get_time();
	
	printf("\t End Time: %d \n",end_interval - start_time );  
	
	residual_time = interval - (end_interval - start_interval);
	
	inter_gen_counter += (end_interval - start_interval);
	
	samples_counter += i;
		
	moment_2nd_dep_counter += ( (end_interval-start_interval)*(end_interval-start_interval) );

	
	if (residual_time > MINTIMESLOT)	    
	    usleep ( residual_time * 1000);
	else
	    printf ("\t Warning: transmission interval overrun\n");  
    }
   
    
    if (TXmode == SPREAD) {

	time_slot = interval / ((data / packetsize) + 1);
	overrun = FALSE;
	
	for (i = 0; i < (data / packetsize) ; i++) {

	    printf ("\t Time: %d -- \n",(int) (i+1)*time_slot + start_interval - start_time);

	    send_packet(packetsize);
	    
	    curr_time = get_time();
	    
	    residual_time = (((i+1) * time_slot) + start_interval) - curr_time;
	    
	    if (residual_time > MINTIMESLOT)
		usleep ( residual_time * 1000);
	    else {
		printf ("\t Warning: transmission slot overrun\n");
		if (curr_time > start_interval + interval) {
		    overrun = TRUE;
		    break;
		}
	    }
	}

	
	if (!overrun) {
	    printf ("\t Time: %d -- \n ", (int) (i+1)*time_slot + start_interval - start_time);

	    send_packet (data % packetsize);

	    residual_time = interval - (get_time() - start_interval);
	
	    if (residual_time > MINTIMESLOT)
		usleep ( residual_time * 1000);
	    else
		printf ("\t Warning: transmission interval overrun\n");
	} else
	    printf ("\t Warning: transmission interval overrun\n");    
    }

    
    return (1);

}




int generate_exp(int mean)
{
    double uniform;
    double exp;


    while ( (uniform = drand48()) == 0) ;
    
    exp = - (double) mean * log(uniform);

    return ( (int) exp);
}


int generate_file_traffic()
{
    FILE  *fd;
    int    infinity;
    int    curr_time;
    int    interval;
    float  trace_time, last_time;
    int    data;

    if (lifetime == 0)
	infinity = TRUE;
    else 
	infinity = FALSE;

    if ( (fd = fopen (source.file.filename, "r")) == NULL ) {
	perror ("fopen");
	return (-1);
    }
   
    curr_time = 0;
    trace_time = 0;
    last_time = 0;
    while (infinity || (curr_time < lifetime)) {
	fscanf (fd, "%f %d\n", &trace_time, &data);
	interval = (trace_time - last_time) * 1000;
	last_time = trace_time;
	data = (double)data * source.file.data_unit;
	transmit_interval(data, interval);
	curr_time += interval;

	if (feof(fd)) {
	    rewind (fd);
	    last_time = 0;
	}
    }
    return (1);
}

int generate_det_traffic()
{
    int infinity;
    int curr_time;

    if (lifetime == 0)
	infinity = TRUE;
    else 
	infinity = FALSE;

    curr_time = 0;
    while (infinity || (curr_time < lifetime)) {
	transmit_interval(source.det.data, source.det.interval);
	curr_time += source.det.interval;
    }
    return (1);
}

int step_markov(int curr_st, int *residence_time, int *next_st)
{
    double  uniform;
    t_elem *elem;

    *residence_time = generate_exp(1.0/source.markov.Q[curr_st].val);
  
    while ( (uniform = drand48()) == 0) ;

    elem = source.markov.Q[curr_st].next;

    while (elem != NULL) {
        if ( elem->val < uniform ) {
            uniform -= elem->val;
            *next_st = elem->st;
            elem = elem->next;
        } else {
            *next_st = elem->st;
            break;
        }
    }
  
    return(1);
}

int generate_markov_traffic()
{
    int    infinity;
    int    curr_time;
    int    curr_state, next_state;
    double trans_rate;
    int    residence_time;
    int    data;

    if (lifetime == 0)
	infinity = TRUE;
    else 
	infinity = FALSE;

    curr_time = 0;
    curr_state = 1;
    while (infinity || (curr_time < lifetime)) {
	/*	printf ("State: %d\n", curr_state); */
	
	trans_rate = source.markov.rate_vec[curr_state];
	step_markov( curr_state, &residence_time, &next_state);
     
        data = (double) trans_rate * residence_time * source.markov.data_unit ;

	transmit_interval(data, residence_time);
	curr_state = next_state;
	curr_time += residence_time;
    }
    
    return(1);
        
}


int generate_traffic()
{
    switch (source_type) {
    case TRACE :
	generate_file_traffic();
	break;
    case DET :
	generate_det_traffic();
	break;
    case MARKOV :
	generate_markov_traffic();
	break;
    }
    
    /* Packet to finalize the transmisson */
    packet->type=END;
    send_packet ( packetsize );
    
    return(1);
}


void initialize_packet(){

    /* This is just to leave the variable at the memory heap part */
    packet = (t_gen_packet*)malloc(sizeof(t_gen_packet)+1);
    packet->nseq  = 0;
    packet->type= DATA;
    packet->timestamp_tx = packet->timestamp_rx = 0;
    
}

/* Function to free the allocated structures */
void clean_struct(){
    free((void *)packet);
}
    

int main (int argc, char **argv)
{

  char *options = NULL;
  /* User_options */
  usercfg_opts_t user_options;
  uint end_time;
  double mean_depart_time, utilization, var_dep_time, confidence, bandwidth;


  if ( argc < 9 ) {
      usage (argv[0] );
      return (-1);
  }
  
  /* check for the number of arguments */
  if ( parse_args(argv ) < 0 ) {
      usage( argv[0] );
      return (-1);
  }
 
  
  initialize_packet(); 
  
  if( ( soc = open_device( DEVICE ) ) < 0   )
      return (-1);
  
  if( pvc_bind( soc ) < 0 )
      return (-1);

  switch( service_class ) {
  case CBR:
      traff_desc.peak /= 53;
      user_options.vc_qos.qosclass = T_ATM_CBR;
      user_options.vc_qos.pcr	     = (int)traff_desc.peak;
      user_options.vc_qos.scr	     =  T_ATM_ABSENT;
      user_options.vc_qos.mbs	     = T_ATM_ABSENT; 
      user_options.vc_qos.mcr	     = T_ATM_ABSENT;
      user_options.vc_qos.icr	     = T_ATM_ABSENT;
      user_options.aal	             = 5;
      user_options.vpi	             = traff_desc.vpi;
      user_options.vci	             = traff_desc.vci;
      user_options.batchsize         = T_ATM_ABSENT;
      user_options.vc_direction      = XTI_VC_DIR_DUPLEX;
      user_options.optlen	     = 0;
      
      break;

 case UBR:
      	user_options.vc_qos.qosclass = T_ATM_NULL;
	user_options.vc_qos.pcr	     = T_ATM_ABSENT;
	user_options.vc_qos.scr	     = T_ATM_ABSENT;
	user_options.vc_qos.mbs	     = T_ATM_ABSENT;
	user_options.vc_qos.mcr	     = T_ATM_ABSENT;
	user_options.vc_qos.icr	     = T_ATM_ABSENT;
	user_options.aal	     = 5;
	user_options.vpi	     = traff_desc.vpi;
	user_options.vci	     = traff_desc.vci;
	user_options.batchsize       = T_ATM_ABSENT;
	user_options.vc_direction    = XTI_VC_DIR_DUPLEX;
	user_options.optlen	     = 0;
	
	break;
  default:
      printf(" Just CBR and UBR are available \n");
  }
  
  options = construct_pvc_options(soc, &user_options);
      
  if( options == NULL )
      return(-1);

  if( pvc_connect(soc, traff_desc.vpi, traff_desc.vci, options, user_options.optlen) < 0) {
      return(-1);
  }

  if( options != (char *) NULL) {
      free(options);
  }
    
  /* read the source description */
  
 
  /* read MARKOV description */
  if( source_type == MARKOV)
      read_markov_source();
  
  start_time = get_time();
  
  inter_gen_counter = 0;
  samples_counter = 0;
  moment_2nd_dep_counter = 0;
  
  generate_traffic();

  
  /* sending disconnect signal */
  if( pvc_disconnect(soc) < 0) {
      return(-1);
  }
  
  clean_up(soc);

  clean_struct();
  
  end_time = get_time();
  
  end_time = (end_time - start_time) /1000;
  
  mean_depart_time = (double) inter_gen_counter / samples_counter ;
  
  utilization = (double) inter_gen_counter / lifetime ;
  
  /* Variance estimator */
  var_dep_time = ( moment_2nd_dep_counter - (2*mean_depart_time*inter_gen_counter) + ( samples_counter*(mean_depart_time * mean_depart_time) ) ) / (samples_counter - 1);
  
  confidence = (1.64 * sqrt(var_dep_time))/sqrt(samples_counter);
  
  bandwidth = (double) (packetsize*8*samples_counter)/end_time;
  bandwidth /= (1000000);
  
  printf("\n\t Total generation time = %d sec", end_time);
  printf("\n\t Total time inter frame = %lld ms",inter_gen_counter);
  printf("\n\t Number total of packets =  %lld",samples_counter);
  printf("\n\t Mean inter packet generation = (%6.3f +/- %.3f) ms/packet", mean_depart_time, confidence );
  printf("\n\t Interval Utilization = %.6f \n", utilization );
  printf("\t Bandwidth generated = %6.2f Mbps\n\n", (float) bandwidth);
  return 0;
  
}
