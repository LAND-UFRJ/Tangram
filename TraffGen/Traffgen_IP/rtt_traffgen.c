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
   
   Last Update: 10/June/2003
      updated by Hugo Sato : hugosato@land.ufrj.br
      Last implementations:
      * Trace File Packet Generation mode
      * Markov Packet Generation mode

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "rtt_traffgen.h"
#include <errno.h>
#include <math.h>

/*=========================== global variables ==============================*/

   /* Transmitted packet */
   t_gen_packet   *packet;

   /* remote host information */
   char   desthost[MAXSTRING];
   char   port[MAXSTRING];

   /* output file*/
   char   filename[MAXSTRING];

   /* Initial and final of session transmission time */ 
   double  start_time = 0;
   double end_time;

   /* Descriptor to handle socket */
   int    soc, soc2;

   /* Number of packets */
   int num_pack;
   
   /* Packet Size */
   u_longlong_t size_packet;
   
   /* Sending total TIME , interval between packets */
   double total_time = 0, packet_interval = 0;

   /* Type of traffic model */
   int traffic_mode;

   /* Description of traffic model to be generated */
   t_source source;
   

/*================================ HELP ===================================*/
int usage(char *filename)
{
    printf ("Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("\nUsage: \n");
    printf ("<dest host> <port> <file> <mode> [mode options] <size> <time> \n");
    printf ("where: \n" );
    printf ("\t<dest host> \t\t host destination\n ");
    printf ("\t<port> \t\t\t port number\n");
    printf ("\t<file> \t\t\t output file\n");
    printf ("\n");
    printf ("\t<mode> \t\t\t Trafic Generation Mode \n" );
    printf ("\t    -d <interval>: Deterministic traffic model\n" );
    printf ("\t       <interval> Interval between packets in seconds\n");
    printf ("\t    -t <trace_file> : Trace File traffic model\n" );
    printf ("\t       <trace_file> input file\n");
    printf ("\t    -m <framesize> <model_file> <reward_file>:  Markov traffic model\n" );    
    printf ("\t       <framesize> Packet frames size\n");
    printf ("\t       <model_file> Model file\n");
    printf ("\t       <reward_file> Reward file\n");
    printf ("\n");
    printf ("\t<size>     \t Size of the packets in bytes\n");
    printf ("\t<time>     \t total time of generation in seconds\n");
    
    return (1);
}


/*========================= ARGUMENT PARSER ===================================*/
int parse_args(char **argv) 
{
  int i;
  
  i = 1;

  /*dest host*/
  strcpy (desthost, argv[i]);
  i++;
  
  /*dest port*/
  if ( (atoi(argv[i])) < 1024) {
    printf ("\nInvalid port: %s\n\n", argv[i]);
    return (-1);
  } 
  else
    strcpy (port, argv[i]);
    
  /*output filename*/
  i++; 
  strcpy (filename, argv[i]);
  
  /*Traffic Generation MODE*/  
  i++;
  if ( !strcmp( argv[i] , "-d" ) )
  { 
      
      traffic_mode = DET;                   
      /*Interval Between Packets*/
      i++;
      if ( ( atof( argv[i] ) ) < 0 ) 
      {
         printf ("\nInvalid Interval Between Packets: %s\n\n", argv[i]);
         return (-1);
      }
      else 
         packet_interval = atof(argv[i]);

  }
  else if ( !strcmp( argv[i] , "-t" ) ) 
  {
      traffic_mode = TRACE;
      i++;
      if ( !strcpy( source.file.filename , argv[i] ) )
      { 
         printf( "\nFile Name Error: %s\n\n", argv[i] );
         return( -1 );
      } 
          
  }
  else if ( !strcmp( argv[i] , "-m" ) ) 
  {
     traffic_mode = MARKOV;
     i++;
     if ( (source.markov.data_unit = (double) (atof (argv[i])) ) <= 0 )
     {
	      fprintf ( stderr , "\n\t Framesize = 0, It must be bigger than zero \n");
	      return(-1);
     }
     i++;       
     source.markov.model_file = (char *) malloc( ( strlen( argv[i] ) + 1 ) * sizeof(char) ); 
     strcpy( source.markov.model_file, argv[i] );      
     i++;
     source.markov.reward_file = (char *) malloc( ( strlen( argv[i] ) + 1 ) * sizeof(char) ); 
     strcpy( source.markov.reward_file, argv[i] );
  }
  else 
  {
     printf( "\nInvalid Traffic Generation Mode: %s\n\n", argv[i] );
     return( -1 );
  }  
      
   /*Packet Size*/
   i++;  
   if (((atof(argv[i])) < 28)||((atof(argv[i])) > MAXPACKSIZE)) 
   {
      printf ("\nInvalid Size of Packets: %s\n\n", argv[i]);
      return (-1);
   }
   else 
     size_packet = atof(argv[i]);  
  
  /*Time of Packet Generation*/
  i++;  
  if ( (atof(argv[i])) < 0) {
    printf ("\nInvalid Time of Generation: %s\n\n", argv[i]);
    return (-1);
  }
  else 
    total_time = atof(argv[i]);   

  return (1);
}


/*============== Functions to initialize and clean struct of packet ==========*/
void clean_struct(void)
{
    free((void *)packet);
}

void initialize_packet(void)
{
    /* This is just to leave the variable allocated at the memory heap part */

    packet = (t_gen_packet*)malloc(sizeof(t_gen_packet)+1);
    packet->nseq  = 0;
    packet->type = DATA;
    packet->timestamp_tx = packet->timestamp_rx = 0;
}


/*========================= Function to send packet ==========================*/
inline int send_packet(int packsize)
{  
  if (packsize == 0) 
    return(0);
  
  if ( packsize < MINPACKSIZE )
    packsize = HEADERSIZE;
    
  /* Increments the Number of sequence of each packet */
  packet->nseq++;
    
  /* Timestamp of packet outgoing */
  packet->timestamp_tx = get_time();
  packet->timestamp_rx = 0;
    
  if (send (soc, (char * )packet, packsize, 0) < 0);

  return(1);
}


/*================ Function to generate deterministic traffic ===============*/
int generate_det_traffic()
{
    int end;
    double stop_time, init_time;
    
    init_time = 0;
    
    end=0;
    init_time = get_time();
    
    /*to do not pass the max number of packets*/
    usleep( packet_interval * 0.05 );
    
    while (!end) {
      stop_time = get_time();
      stop_time = stop_time - init_time + packet_interval;
      if  (stop_time >= total_time)
      {
          packet->type= END;
	  end=1;
      } else
      {
         packet->type= DATA;
      }
	 send_packet( size_packet );
	 usleep( packet_interval );
    }
    return ( TRUE );
} 


/*============== Functions to generate trace file traffic ===================*/
int transmit_interval(double data, double interval)
{
  double   start_interval;
  double   curr_time;
  double   end_interval;
  double   residual_time, aux;
  uint     i, N_Pack, R_Data;

  R_Data = 0;
  start_interval = get_time();

  N_Pack = data / size_packet;

  /* data empty */
  if( data == 0 )
  {
    usleep ( interval );
    return ( TRUE );
  }

  /* It has just one fragment of packet */
  if( N_Pack == 0 )
  {
    /* Residual packet */
    R_Data = (uint) data; 
    
    send_packet ( R_Data % size_packet );
    
    curr_time = get_time();
	  
    aux = (double) curr_time - start_interval;
	  
    residual_time = (double) interval - aux; 
    
    if( residual_time > 0 )
    {
      usleep( residual_time  );
      return ( TRUE );
    }
    else
      return ( FALSE );
  }

    for( i = 1; i <= N_Pack ; i++ )
    /* Residual packet */
    R_Data = (uint) data; 
    
    send_packet ( R_Data % size_packet );

    printf ("\t Start Time: %.0f us -- \t", start_interval - start_time); 
    
    end_interval = get_time();
    
    printf("\t End Time: %.0f us \n",end_interval - start_time );  
    
    residual_time = interval - (end_interval - start_interval);
    
    if (residual_time > 0)
    {	    
      usleep ( residual_time );
      return ( TRUE );
    }
    else
      return ( FALSE );

}

int generate_file_traffic()
{
    FILE  *fd;
    int    infinity;
    /*int   i;*/
    /*char   buffer[80];*/
    double data, 
           trace_time, 
           last_time,
           gen_curr_time,
           real_curr_time,
           interval;    

    if (total_time == 0) infinity = TRUE;
    else infinity = FALSE;

    if ( (fd = fopen (source.file.filename, "r")) == NULL ) 
    {
	    perror ("fopen");
	    return (-1);
    }
   
    gen_curr_time = 0;
    real_curr_time = 0;
    trace_time = 0;
    last_time = 0;
    
/* debug , what porra is that?
    for( i =0 ; i < 2; i++)
    {
       fgets(buffer, 80, fd );
       printf("%s \n",buffer);
    }*/
    
    /* set the starting time */
    
    start_time = get_time();
    
    while (infinity || (real_curr_time < total_time)) 
    {
      fscanf (fd, "%lf %lf\n", &trace_time, &data);
      interval = (trace_time - last_time) * USEC;
      last_time = trace_time;
      data = (double) data /*debug * source.file.data_unit */;
      transmit_interval( data, interval );

      // Model Generation time   
      gen_curr_time += interval;

      // Clock Real time   
      real_curr_time = get_time() - start_time;
      //fprintf( stderr , "%f\n", real_curr_time );

	   if (feof(fd)) 
      {
	      rewind (fd);
	      last_time = 0;
	   }
      fflush( stdout );
   }
   
  /* force finish */
  packet->nseq++; 
  packet->timestamp_tx = get_time();
  packet->timestamp_rx = 0;
  packet->type =  END; 
  send (soc, (char * )packet, size_packet, 0);

  return ( TRUE );
}


/*================ Functions to generate markov traffic ===================*/
t_elem *initiate_matrix(int no_states)
{    
    t_elem *Matrix;
    int     i;

    Matrix = (t_elem *)malloc((no_states + 1) * sizeof(t_elem)) ;
    if (Matrix != NULL)
	 for (i=0; i<=no_states; i++) 
    {
	    Matrix[i].val = 0;
	    Matrix[i].st  = 0;
	    Matrix[i].next = NULL;
	 }
    else	printf ("ERROR: Malloc flipped while allocating Q matrix\n");

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

    for (i=1; i<=source.markov.no_states; i++) 
    {
	    elem = Matrix[i].next;
	    out_rate = 0;
	    while (elem != NULL) 
       {
	       out_rate += elem->val;
	       elem = elem->next;
	    }
	    Matrix[i].val = out_rate;
	    elem = Matrix[i].next;
	    while (elem != NULL) 
       {
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
    double flt;
    char lixo1[80], lixo2[80];

    if ( (fd = fopen(source.markov.model_file, "r")) == NULL) 
    {
	   perror ("fopen");
	   return (-1);
    }
    
    fscanf (fd, "%d\n", &source.markov.no_states);
    source.markov.rate_vec = (double *) malloc ((source.markov.no_states + 1) * 
						sizeof(double));
    source.markov.unif_vec = (double *) malloc ((source.markov.no_states + 1) * 
						sizeof(double));    
   
 
    for (i=1; i<=source.markov.no_states; i++) 
    {
	   source.markov.rate_vec[i] = 0;
	   source.markov.unif_vec[i] = 0;
    }

     
    if ( (fd2 = fopen(source.markov.reward_file, "r")) == NULL) 
    {
	   perror ("fopen");
	   return (-1);
    }

    fscanf(fd2,"%s %s\n",lixo1,lixo2);

    while( !feof(fd2) )
    {
      fscanf(fd2, "%d %lf\n",&i,&flt);
	   printf("%d %f\n",i,flt);
	   source.markov.rate_vec[i] = flt / USEC;	
    }

    fclose(fd2);
    
    source.markov.Q = initiate_matrix(source.markov.no_states);
    
    while ( !feof(fd) )
    {
	    fscanf (fd, "%d %d %lf\n", &i, &j, &flt);
	    put_matrix(source.markov.Q, i, j, flt / USEC );
    }
    
    uniformize_matrix(source.markov.Q);

    fclose (fd);

    return (1);	
}

double generate_exp(double mean)
{
    double uniform;
    double exp;


    while ( (uniform = drand48()) == 0) ;
    
    exp = - (double) mean * log(uniform);

    return ( exp);
}

int step_markov(int curr_st, double *residence_time, int *next_st)
{
    double  uniform;
    t_elem *elem;
    double aux ;

    aux = (double) 1.0/source.markov.Q[curr_st].val ;

    *residence_time = generate_exp( aux );
  
    while ( (uniform = drand48()) == 0) ;

    elem = source.markov.Q[curr_st].next;

    while (elem != NULL) 
    {
        if ( elem->val < uniform ) 
        {
            uniform -= elem->val;
            *next_st = elem->st;
            /* advance to the next possible state */
            elem = elem->next;
        } 
        else 
        {
            *next_st = elem->st;
            break;
        }
    }
   
    return ( TRUE );

}

void finalize()
{    
  int i;

  for ( i=0; i < TRYING; i++ )
  {
    packet->type= END;
    send_packet(1);
    sleep(1);
  }
  return;
}

int generate_markov_traffic()
{
   int    
      infinity,
      curr_state, 
      next_state;

   double    
      gen_curr_time,
      real_curr_time,
      trans_rate,
      data,
      residence_time;
      

   if ( total_time == 0) infinity = TRUE;
   else infinity = FALSE;

   gen_curr_time = 0;
   real_curr_time = 0;
   curr_state = 1;
   
   /* set the starting time */
   start_time = get_time();  
   
   while ( infinity || ( real_curr_time < total_time ) ) 
   {    
      trans_rate = source.markov.rate_vec[curr_state];
      step_markov( curr_state, &residence_time, &next_state);

      data = (double) trans_rate * residence_time * source.markov.data_unit ;

      transmit_interval(data, residence_time);
      curr_state = next_state;

      /* Model Generation time */
      gen_curr_time += residence_time;

      /* Clock Real time */
      real_curr_time = get_time() - start_time;

      fflush( stdout );
   }

   /* force finish */
   packet->nseq++; 
   packet->timestamp_tx = get_time();
   packet->timestamp_rx = 0;
   packet->type =  END; 
   send (soc, (char * )packet, size_packet, 0);
   
   return ( TRUE );
}
 
 
/*========================= Function to receive packets ========================*/
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
  if( !(fsize = fopen(filename,"w+") ) )
  {
      printf("\n\tError: Can't create out file!\n");
      return( -1 );
  }

  fprintf( stderr, "Traffic Receiver is ready to collect trace.\nWaiting for packets from generator...\n" );
  while ( (!end) )
  {  
    byread = 0;

    /* system call to receive the packet */
    if ( (byread = recv(soc2,(char *)pack, sizeof(t_gen_packet), 0) ) < 0 )
      fprintf( stderr , "errno%d \n", errno );
    else fprintf( stderr, "#" );

    pack->timestamp_rx = get_time();
    fprintf ( fsize, "%lld \t", pack->nseq );
    fprintf ( fsize, "%7.0f \t", pack->timestamp_tx );
    fprintf ( fsize, "%7.0f \t", pack->timestamp_rx - pack->timestamp_tx );
    fprintf ( fsize, "%d \n", byread );
    fflush ( fsize );
    
    if ( pack->type == END )
    {
      /* end of transmission received */
      fprintf ( stderr, "\nReceived a terminate message from traffic generator.\n" );
      end = 1;
    }
  }

  fclose(fsize);

  return ( TRUE );
}


/*============================ THE MAIN FUNCTION ============================*/
int main (int argc, char **argv)
{
  int pid;   


  /* check for the number of arguments */
  if ( argc < 6 ) 
  {
     fprintf( stderr, "\nInvalid number of arguments.\n\n" );
     usage(argv[0]);
     return (-1);
  }  
  if ( !strcmp( argv[4] , "-d" ) )
  {
    if ( argc != 8 ) 
    {
      fprintf( stderr, "\nInvalid number of arguments for Deterministic Traffic.\n\n" );
      usage(argv[0]);
      return (-1);
    }
  }
  else if ( !strcmp( argv[4] , "-t" ) )
  {
    if ( argc != 8 ) 
    {
      fprintf( stderr, "\nInvalid number of arguments for Tracefile traffic.\n\n" );
      usage(argv[0]);
      return (-1);
    }
  } 
  else if ( !strcmp( argv[4] , "-m" ) )
  {
    if ( argc != 10 )
    {
      fprintf( stderr, "\nInvalid number of arguments for Markovian Traffic.\n\n" );
      usage(argv[0]);
      return (-1);
    }
  }
  else 
  {
     fprintf( stderr, "\nInvalid traffic mode.\n\n" );
     usage(argv[0]);
     return (-1);
  }
  
  /* parse the arguments */
  if (parse_args(argv) < 0) 
  {
    usage(argv[0]);
    return (-1);
  }
  
  total_time = total_time * 1000000;               /*sec to usec*/
  packet_interval = packet_interval * 1000000;     /*set to usec*/
    
  srand48 ( (long int) get_time()  );

  initialize_packet();
  
  /* create the socket */
  if ( (soc = connectUDP (desthost, port)) < 0) 
  {
    perror ("socket creation");
    return (-1);
  }  

  if ( (soc2 = passiveUDP (port, 0)) < 0) 
  {
    perror("Cannot create socket");
    return (-1);
  }
  
  if ((pid = fork()) > 0)
  {     
     /* Parent Process */
     /* receive the traffic  */
     receive_traffic();
  }
  else
  {
     /* Child Process */
     /* generate the traffic  */
     switch ( traffic_mode )
     {
         case DET:    generate_det_traffic(); 
                      break;
         case TRACE:  generate_file_traffic(); 
                      break;
         case MARKOV: read_markov_source(); 
                      generate_markov_traffic(); 
                      break;
     }       
  }  
  
  clean_struct();
  
  fflush(stdout);

  return 0;  
  
}  
