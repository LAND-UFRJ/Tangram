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
    Tool: Traffic Generator
    LAND - Laboratorio de Analise e Desempenho - UFRJ/COPPE/COS
        Last Update: 30/07/2002

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "mcast.h"
#include "traffgen_ip.h"
#include "common.h"

/* global variables */

/* Mode of transmission */
int    TXmode;

double lifetime;   /* expressed in usec */

/* Initial and final of session transmission time */
double  start_time = 0;
double end_time;

/* Descriptor to handle socket */
int    soc;

int    protocol;
char   desthost[MAXSTRING];
char   port[MAXSTRING];
char   filename[MAXSTRING];

/* Type of traffic model */
int      source_type;

/* Description of traffic model to be generated */
t_source source;

/* Type of generated packet */
t_gen_packet   *packet;

uint      packetsize;

/* Multicast sockaddr_in */
struct sockaddr_in mcast_sockaddr;


int usage(char *filename)
{
    printf ("Traffic Generator v3.1 - Copyright (C) 1999-2006\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("Usage: \n");
    printf ("%s [UNI|MULT]  [-c|-s] <dest host> <port> <packetsize>  [-d|-m|-t] <lifetime>\n", filename );
    printf ("where:\n");
    printf ("\t[UNI|MULT]                                 \t defines the network protocol\n");
    printf ("\t -c                                        \t continuous way \n");
    printf ("\t -s                                        \t spread way \n");
    printf ("\t<dest host>                                \t host or group destination\n ");
    printf ("\t<port>                                     \t port number\n");
    printf ("\t<packetsize> (bytes)                       \t block size used to break large frames \n");
    printf ("\t -d <framesize> (bytes) <time> (seconds)   \t deterministic traffic model \n");
    printf ("\t -m <framesize> <model_file> <reward_file> \t markov traffic model \n");
    printf ("\t -t <tracefile> <data_unit>                \t trace traffic model \n");
    printf ("\t<lifetime>                                 \t session time (0 = infinity)\n");

    return (1);
}

int parse_args(char **argv)
{
  int i, aux;

  i = 1;

  if (!(strcmp(argv[i], "UNI")))
    protocol = UNI;
  else {
    if (!(strcmp(argv[i],"MULT")))
      protocol = MULT;
    else {
      fprintf ( stderr , "Invalid Protocol: %s\n",argv[i]);
      return (-1);
    }
  }
  i++;

  /*Type of packet generation  */
  if ( !(strcmp(argv[i], "-c")) )
    TXmode = CONTIN;
  else
    if ( !(strcmp(argv[i], "-C")) )
      TXmode = CONTIN2;
    else
      if ( !(strcmp(argv[i], "-s")) )
        TXmode = SPREAD;
      else{
        fprintf ( stderr , "\n\t The TXMode must be -c or -s\n");
        return (-1);
        }
  i++;

  strcpy (desthost, argv[i]);
  i++;

  if ( (atoi(argv[i])) < 1024) {
    fprintf ( stderr , "Invalid port: %s\n", argv[i]);
    return (-1);
  }
  else
    strcpy (port, argv[i]);
  i++;


  if ( (packetsize = atoi(argv[i])) > MAXPACKSIZE) {
    fprintf ( stderr , "Invalid packet size: %s\n", argv[i]);
    return (-1);
  }
  i++;

  if ( packetsize <= MINPACKSIZE ){
    fprintf ( stderr , "Invalid packet size: %s\n", argv[i]);
    return (-1);
  }

  if ( !(strcmp(argv[i], "-d")) ){
    source_type = DET;
    i++;

    if ( (source.det.data = atof (argv[i])) == 0 ){
      fprintf ( stderr , "\n\t Framesize = 0, It must be bigger than zero \n");
      return(-1);
    }
    i++;

    if ( (source.det.interval = (double) (USEC * (atof (argv[i]))) ) == 0 ){
      fprintf ( stderr , "\n\t Time = 0, It must be bigger than zero \n");
      return(-1);
    }
  }

  else
    if ( !(strcmp(argv[i], "-m")) ){
      source_type = MARKOV ;
      i++;

      if ( (source.markov.data_unit = (double) (atof (argv[i])) ) == 0 )
      {
          fprintf ( stderr , "\n\t Framesize = 0, It must be bigger than zero \n");
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

    if ( (source.file.data_unit = (double) (atof (argv[i])) ) == 0 ){
      fprintf ( stderr , "\n\t Framesize = 0, It must be bigger than zero \n");
      return(-1);
    }
      }

      else{
    fprintf ( stderr , "\n\t Traffic model error [-d|-m|-t] \n");
    return (-1);
      }


  i++;
  if ( (lifetime = atof (argv[i]) * USEC ) < 0) {
    fprintf ( stderr , "Invalid life time: %s\n", argv[7]);
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
    double flt;
    char lixo1[80], lixo2[80];

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

    fscanf(fd2,"%s %s\n",lixo1,lixo2);

    while( !feof(fd2) ){
      fscanf(fd2, "%d %lf\n",&i,&flt);
    printf("%d %f\n",i,flt);
    source.markov.rate_vec[i] = flt / USEC;

    }

    fclose(fd2);

    source.markov.Q = initiate_matrix(source.markov.no_states);

    while ( !feof(fd) ){
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

inline int send_packet( int packsize )
{
    if( packsize == 0 )
        return( 0 );

    if( packsize < MINPACKSIZE )
        packsize = HEADERSIZE;

    /* Increments the Number of sequence of each packet */
    packet->nseq++;

    /* Timestamp of packet outgoing */
    packet->timestamp_tx = get_time();
    packet->timestamp_rx = 0;

    if( protocol == UNI )
    {
        if( send( soc, (char * )packet, packsize, 0) < 0 );
            //fprintf( stderr , "Sending: errno %d\n", errno );
    }
    else
    {
        if( sendto( soc, (char *)packet, packsize, 0,
                    (struct sockaddr *)&mcast_sockaddr,
                    sizeof(mcast_sockaddr) ) < 0 );
            //fprintf( stderr , "Sending: errno %d\n", errno );
    }
    return( 1 );
}


int transmit_interval(double data, double interval)
{
  double   start_interval;
  double   curr_time;
  double   end_interval;
  double   time_slot;
  double   residual_time, aux;
  uint     i, N_Pack, R_Data;

  residual_time  = 0.0;
  start_interval = get_time();

  N_Pack = data / packetsize;

  /* data empty */
  if (data == 0 ){
    usleep ( interval );
    return ( TRUE );
  }

  /* It has just one fragment of packet */
  if ( N_Pack == 0 ){

    /* Residual packet */
    R_Data = (uint) data;

    send_packet ( R_Data % packetsize);

    curr_time = get_time();

    aux = (double) curr_time - start_interval;

    residual_time = (double) interval - aux;

    if ( residual_time > 0 ){
      usleep( residual_time  );
      return ( TRUE );
    }
    else
      return ( FALSE );
  }

  if (TXmode == CONTIN) {

    for (i = 1; i <= N_Pack ; i++) {
      send_packet(packetsize);
    }

    /* Residual packet */
    R_Data = (uint) data;

    send_packet ( R_Data % packetsize);

    printf("\t Start Time: %.0f us -- \t", start_interval - start_time);

    end_interval = get_time();

    printf("\t End Time: %.0f us \n",end_interval - start_time );

    residual_time = interval - (end_interval - start_interval);

    if (residual_time > 0){
      usleep ( residual_time );
      return ( TRUE );
    }
    else
      return ( FALSE );
  }

  if (TXmode == CONTIN2) {

    for (i = 1; i <= N_Pack ; i++) {
      send_packet(packetsize);
      curr_time = get_time();
      fprintf (stdout, "\t Time: %.0f -- us \n", curr_time);
    }

    /* Residual packet */
    R_Data = (uint) data;

    send_packet ( R_Data % packetsize);

    end_interval = get_time();

    residual_time = interval - (end_interval - start_interval);

    if (residual_time > 0){
      usleep ( residual_time );
      return ( TRUE );
    }
    else
      return ( FALSE );
  }

  if (TXmode == SPREAD) {

    R_Data = (uint) data;
    time_slot = interval / (data / packetsize) ;

    for (i = 1; i <= N_Pack ; i++) {

      //fprintf (stdout, "\t Time: %.0f -- us \n", (i)*time_slot + start_interval - start_time);

      send_packet(packetsize);

      curr_time = get_time();

      fprintf (stdout, "\t Time: %.0f -- us \n", curr_time);

      residual_time = ((i * time_slot) + start_interval) - curr_time;


      if (residual_time > 0)
    usleep ( residual_time );
    }

    /* Residual packet */

    if ( ( R_Data % packetsize ) > 0 ){

      /*  fprintf (stdout, "\t Res_Time: %.0f -- us \n", (i)*time_slot + start_interval - start_time); */

      send_packet ( R_Data % packetsize);

      curr_time = get_time();

      residual_time = interval - (curr_time - start_interval);

      if (residual_time > 0)
    usleep ( residual_time );
    }

    if ( residual_time > 0 )
      return ( TRUE );
    else
      return ( FALSE );
  }

  return ( TRUE );
}


int generate_file_traffic()
{
    FILE  *fd;
    int    infinity, i;
    char   buffer[80];
    double data,
      trace_time,
      last_time,
      gen_curr_time,
      real_curr_time,
      interval;

    if (lifetime == 0)
    infinity = TRUE;
    else
    infinity = FALSE;

    if ( (fd = fopen (source.file.filename, "r")) == NULL ) {
    perror ("fopen");
    return (-1);
    }

    gen_curr_time = 0;
    real_curr_time = 0;
    trace_time = 0;
    last_time = 0;

    for( i =0 ; i < 2; i++){
      fgets(buffer, 80, fd );
      printf("%s \n",buffer);
    }

    while (infinity || (real_curr_time < lifetime)) {
      fscanf (fd, "%lf %lf\n", &trace_time, &data);
      interval = (trace_time - last_time) * USEC;
      last_time = trace_time;
      data = (double) data * source.file.data_unit;
      transmit_interval(  data, interval );

      /* Model Generation time */
      gen_curr_time += interval;

      /* Clock Real time */
      real_curr_time = get_time() - start_time;


    if (feof(fd)) {
        rewind (fd);
        last_time = 0;
    }
        fflush( stdout );
    }

    return ( TRUE );

}

int generate_det_traffic()
{
    int    infinity;
    double gen_curr_time;
    double real_curr_time;

    if( lifetime == 0 )
        infinity = TRUE;
    else
        infinity = FALSE;

    gen_curr_time  = 0;
    real_curr_time = 0;

    while( infinity || ( real_curr_time < lifetime ) )
    {
        transmit_interval( source.det.data, source.det.interval );

        /* Model Generation time */
        gen_curr_time += source.det.interval;

        /* Clock Real time */
        real_curr_time = get_time() - start_time;

        fflush( stdout );
    }

    return( TRUE );
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

    while (elem != NULL) {
        if ( elem->val < uniform ) {
            uniform -= elem->val;
            *next_st = elem->st;
            /* advance to the next possible state */
            elem = elem->next;
        } else {
            *next_st = elem->st;
            break;
        }
    }

    return ( TRUE );

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

    if (lifetime == 0)
    infinity = TRUE;
    else
    infinity = FALSE;

    gen_curr_time = 0;
    real_curr_time = 0;
    curr_state = 1;

    while (infinity || (real_curr_time < lifetime)) {

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

    return ( TRUE );

}

void finalize(){
  int i;

  for ( i=0; i < TRYING; i++ ){
    packet->type= END;
    send_packet(1);
    sleep(1);
  }
  return;
}

int generate_traffic()
{
    int return_value = TRUE;

    switch( source_type )
    {
        case TRACE :
            generate_file_traffic();
            break;
        case DET :
            generate_det_traffic();
            break;
        case MARKOV :
            generate_markov_traffic();
            break;
        default:
            return_value = FALSE;
    }

    return( return_value );
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


int main( int argc, char **argv )
{
    /* check for the number of arguments */
    if( argc < 10 )
    {
        usage( argv[0] );
        return( -1 );
    }

    /* parse the arguments */
    if( parse_args( argv ) < 0 )
    {
        usage(argv[0]);
        return (-1);
    }

    srand48( (long int) get_time() );

    initialize_packet();

    /* create the socket */
    if( protocol == UNI )
    {
        soc = connectUDP (desthost, port);
        if( soc < 0 )
        {
            perror( "socket creation" );
            return( -1 );
        }
    }
    else
    {
        soc = mcastJoinGroup( desthost, atoi( port ), &mcast_sockaddr );
        if( soc < 0 )
        {
            perror( "socket creation" );
            return( -1 );
        }
    }

    /* read MARKOV description */
    if( source_type == MARKOV )
        read_markov_source();

    /* set the starting time */
    start_time = get_time();

    /* generate the traffic according to the source */
    generate_traffic();

    /* send control packet to finalize the communication */
    finalize();

    /* close the socket */
    if( protocol == UNI )
        close( soc );
    else
        mcastLeaveGroup( soc, desthost );

    end_time = get_time();

    clean_struct();

    fflush( stdout );

    return 0;
}
