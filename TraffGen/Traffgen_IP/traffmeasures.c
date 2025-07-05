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

    Author: Fabiano Portella
    Tool: Traffic Generator
    LAND - Laboratorio de Analise e Desempenho - UFRJ/COPPE/COS

    Last Update: 26/may/2003
      updated by Hugo Sato : hugosato@land.ufrj.br
    Last implementation: 
      * New measure of interest (success) 
      * Modification in loss function (conditional loss is implemented)
    PS: We considered as loss unordered packets and uncoming packets !

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
  #include <limits.h>
  #include <float.h>
  #ifndef MAXDOUBLE
    #define MAXDOUBLE DBL_MAX
  #endif
#else
// The values.h is deprecated.
#include <values.h>
#endif

#include "common.h"
#include "traffmeasures.h"

#define NUMARGS 4


/* Functions used in qsort */
/*-----------------------------------------------------------------*/
int compareLong( const void *a, const void *b )
{
    if( *(u_longlong_t *)a > *(u_longlong_t *)b )    return( 1 );
    else                                             return( -1 );
}
/*-----------------------------------------------------------------*/


/*-----------------------------------------------------------------*/
int compareDouble( const void *a, const void *b )
{
    if( *(double *)a > *(double *)b )    return( 1 );
    else                                 return( -1 ); 
}
/*-----------------------------------------------------------------*/



/*-----------------------------------------------------------------*/
int compareTrace( const void *a, const void *b )
{
    if( (*(t_trace *)a).nseq > (*(t_trace *)b).nseq )    return( 1 );
    else                                                 return( -1 );
}
/*-----------------------------------------------------------------*/



/*-----------------------------------------------------------------*/
void usage(char *filename)
{
    fprintf( stderr, "Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    fprintf( stderr, "Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    fprintf( stderr, "LAND - Laboratory for Modeling and Development of Networks \n");
    fprintf( stderr, "\nUsage: %s -<measure options> <input file> [time interval] [-o] [output file]", filename);
    fprintf( stderr, "\n\t<measure options>         : any combination of 'j', 'l' and 't' where");    
    fprintf( stderr, "\n\t\tl                 : call for loss function");
    fprintf( stderr, "\n\t\ts                 : call for success function");
    fprintf( stderr, "\n\t\tj                 : call for jitter function");
    fprintf( stderr, "\n\t\tt                 : call for throughput function");
    fprintf( stderr, "\n\t<input file>              : file from traffgen_recv");
    fprintf( stderr, "\n\t[time interval] (usec)    : interval to verify jitter in network (used only in jitter function) \n\n");
    fprintf( stderr, "\n\t[-o]                      : optional parameter (default output file: out)");
    fprintf( stderr, "\n\t[output file]             : name of output file\n\n");

    exit( -1 );
}
/*-----------------------------------------------------------------*/



/* Function to calculate losses */
/*--------------------------------------------------------------------------------------------------*/
int losses(char *name_file_in, t_trace *trace, u_longlong_t size_trace, u_longlong_t total_packets )
{
    u_longlong_t i=0, j=0, size_buff_losses, total_loss=0, no_loss_prob=total_packets;
    u_longlong_t seq_packet, *buff_losses;
    long long int diff;
    float loss_ratio;
    FILE *loss_file = NULL, *pmf_loss_file = NULL;
    char *name_loss_file, *name_pmf_loss_file;
    fpos_t file_position;
    
    /* Memory allocation for losses' buffer */
    if( ( buff_losses = (u_longlong_t *)malloc( size_trace * sizeof(u_longlong_t) ) ) == NULL )
    {
        printf("\n\n\tError: Memory allocation!");
        return( -1 );
    }

    if( !(name_loss_file = malloc(sizeof(char)*(strlen( name_file_in ) + strlen(".TM.loss_trace") + 1)) ))
    {
        fprintf( stderr, "\n\n\tError: Memory allocation!");
        return( -1 );
    }

    if( !(name_pmf_loss_file = malloc(sizeof(char)*(strlen( name_file_in ) + strlen(".TM.loss_pmf") + 1)) ))
    {
        fprintf( stderr, "\n\n\tError: Memory allocation!");
        return( -1 );
    }

    /* Creating names for out files */
    strcpy(name_loss_file, name_file_in);
    strcat(name_loss_file, ".TM.loss_trace");

    strcpy(name_pmf_loss_file, name_file_in);
    strcat(name_pmf_loss_file, ".TM.loss_pmf");

    seq_packet    = 0;    /* packet in increasing order */
    loss_ratio    = 0.0;

    for( i=0; i<size_trace; i++ )
    {
        seq_packet++;
        diff = trace[i].nseq - seq_packet;

        if( diff < 0 )
        {
        /*desconsider delayed arrives*/         
            seq_packet--;
        }
        else if( diff > 0 ) 
        {
            if(loss_file == NULL)
            {
                /* Trying to create out file */
                if( !(loss_file = fopen(name_loss_file,"w+") ) )
                {
                    printf("\n\tError: Can't create out file!\n");
                    return( -1 );
                }

                fprintf(loss_file, "# Measure of Interest: Loss\n");
                fprintf(loss_file, "# This file contains the consecutive packets lost\n" );
                fprintf(loss_file, "# X-axis: Packet number / Y-axis: Consecutive losses\n");
                /* Getting file positon to overwrite after calculating loss ratio */
                fgetpos(loss_file, &file_position);
                fprintf(loss_file, "# Loss Ratio: %0.10E\n", 0.0);
                fprintf(loss_file, "# ");
                fprintf(loss_file, "\n\t0   0"); /* Artificial point to plot from (0,0) */
            }
            
            fprintf(loss_file,"\n\t%Ld   %Ld", seq_packet, diff - 1 );
            /* New variable to calculate conditional pmf (added in dec, 19)*/
            total_loss      += diff;
            seq_packet       = trace[i].nseq;
            loss_ratio      += diff;
            buff_losses[j++] = diff;  /* Buffer used in pmf_losses */
            no_loss_prob--; /* Count value of aleatory variable = 0 */
        }
    }
    /* Writting loss ratio on the header */
    loss_ratio /= total_packets;
    
    if( loss_file != NULL )
    {
        fsetpos(loss_file, &file_position);
        fprintf(loss_file, "# Loss Ratio: %0.10E\n", loss_ratio);
    }
    
    buff_losses[j] = 0; /* End of buffer */
    size_buff_losses = j;

    if( size_buff_losses == 0 )
    {
        printf("\tThere weren't lost packets!\n");
        return( 0 );
    }

    /* PMF losses: Now, we replace total_packets -> total_loss (conditional probability) */

    /* Trying to create out files */
    if( !(pmf_loss_file = fopen(name_pmf_loss_file,"w+") ) )
    {
        printf("\n\tError: Can't create out file!\n");
        return( -1 );
    }


    fprintf(pmf_loss_file, "# Measure of Interest: PMF[Loss size]\n");
    fprintf(pmf_loss_file, "# This file contains the PMF of Loss size");
    /* pmf of loss = 0 */
    /*fprintf(pmf_loss_file, "\n\t0  %e", (double)no_loss_prob/(double)total_loss);*/


    /* Sorting buffer to optimize result of pmf */
    qsort( buff_losses, size_buff_losses, sizeof(u_longlong_t), compareLong );

    i = 0; j = 1;
    while( i < size_buff_losses )
    {
        if( buff_losses[i] != buff_losses[i+1] )
        {
            fprintf(pmf_loss_file, "\n\t%Ld  %e", buff_losses[i], (double)j / (double)size_buff_losses);
            j=1;
        }
        else
        {
            j++;
        }
        i++;
    }


    free( buff_losses );
    free( name_loss_file );
    free( name_pmf_loss_file );
    if ( loss_file ) fclose( loss_file );
    if ( pmf_loss_file ) fclose( pmf_loss_file );
    return( 0 );
}
/*--------------------------------------------------------------------------------------------------*/



/* Function to calculate success */
/*----------------------------------------------------------------------------------------------------*/
int success( char *name_file_in, t_trace *trace, u_longlong_t size_trace, u_longlong_t total_packets )
{
    u_longlong_t i=0, j=0, size_buff_success, success_counter, no_success_prob;
    u_longlong_t seq_packet, *buff_success;
    long long int diff;
    float success_ratio;
    FILE *success_file = NULL, *pmf_success_file = NULL;
    char *name_success_file, *name_pmf_success_file;
    fpos_t file_position;

    /* Memory allocation for success' buffer */
    if( ( buff_success = (u_longlong_t *)malloc( size_trace * sizeof(u_longlong_t) ) ) == NULL )
    {
        printf("\n\n\tError: Memory allocation!");
        return( -1 );
    }
    /**/
    
    /* Memory allocation for files' names */
    if( !(name_success_file = malloc(sizeof(char)*(strlen( name_file_in ) + strlen(".TM.success_trace") + 1)) ))    
    {
        fprintf( stderr, "\n\n\tError: Memory allocation!");
        return( -1 );
    }

    if( !(name_pmf_success_file = malloc(sizeof(char)*(strlen( name_file_in ) + strlen(".TM.success_pmf") + 1)) ))
    {
        fprintf( stderr, "\n\n\tError: Memory allocation!");
        return( -1 );
    }
    /**/

    /* Creating names for out files */
    strcpy(name_success_file, name_file_in);
    strcat(name_success_file, ".TM.success_trace");

    strcpy(name_pmf_success_file, name_file_in);
    strcat(name_pmf_success_file, ".TM.success_pmf");
    /**/
    
    seq_packet    = 0;  /* packet in increasing order */
    success_counter = 0;
    success_ratio   = 0;
    no_success_prob = 0;
     
    /* Trying to create out file */
    if( !(success_file = fopen(name_success_file,"w+") ) )
    {
        printf("\n\tError: Can't create out file!\n");
        return( -1 );
    }
    else
    {
        fprintf(success_file, "# Measure of Interest: Success\n");
        fprintf(success_file, "# This file contains the consecutive arrived packets\n");
        fprintf(success_file, "# X-axis: Packet number / Y-axis: Consecutive success\n");
        /* Getting file positon to overwrite after calculating success ratio */
        fgetpos(success_file, &file_position);
        fprintf(success_file, "# Success Ratio: %0.10E\n", 0.0);
        fprintf(success_file, "# ");
        fprintf(success_file, "\n\t0   0"); /* Artificial point to plot from (0,0) */
    }
     
    /* Success algorithm */
    for( i=0; i<size_trace; i++ )
    {
        seq_packet++;
        
        diff = trace[i].nseq - seq_packet;
                
        if( diff == 0 )
        /*right packet*/
        {
            success_counter++;
            success_ratio++;
        }
        else if( diff < 0 )
        /*desconsider delayed arrives*/
        {            
            seq_packet--;                  
        }
        else 
        /* if( diff > 1 ) -> losses */
        {            
            if ( i >0 ) fprintf(success_file,"\n\t%Ld   %Ld", trace[i-1].nseq - ( success_counter - 1 ) , success_counter - 1 );
            seq_packet = trace[i].nseq;
            success_ratio++;          
            buff_success[j] = success_counter; j++; /* Buffer used in pmf_success */
            success_counter = 1;
            no_success_prob++; /* Count value of aleatory variable = 0 */
            
        }
    }
    
    if ( success_counter ) 
    {              
       fprintf(success_file,"\n\t%Ld   %Ld", trace[size_trace-1].nseq - ( success_counter - 1 ) , success_counter - 1 );
       buff_success[j] = success_counter; 
       j++;   /*takes last sequence of packet*/
    }
    /* End of algorithm */
    
    
    /* Writting success ratio on the header */
    success_ratio /= total_packets;
 
 
    if( success_file != NULL )
    {
       fsetpos(success_file, &file_position);
       fprintf(success_file, "# Success Ratio: %0.10E\n", success_ratio);
    }
    /**/
    
    buff_success[j] = 0; /* End of buffer */
    size_buff_success = j;
      
    
    if( size_buff_success == 1 )
    {
        printf("\tThere werent lost packets!\n");
        return( 0 );
    }    

    /* PMF success */

    /* Trying to create out files */
    if( !(pmf_success_file = fopen(name_pmf_success_file,"w+") ) )
    {
        printf("\n\tError: Can't create out file!\n");
        return( -1 );
    }


    fprintf(pmf_success_file, "# Measure of Interest: PMF[Success size]\n");
    fprintf(pmf_success_file, "# This file contains the PMF of the number of packets between two losses");

    /* Sorting buffer to optimize result of pmf */
    qsort( buff_success, size_buff_success, sizeof(u_longlong_t), compareLong );
    
    i = 0; j = 1;
    while( i < size_buff_success )
    {
        /*statitics about the number of packets received in a row*/
        if( buff_success[i] != buff_success[i+1] )
        {
            fprintf(pmf_success_file, "\n\t%Ld  %e", buff_success[i], (double)j / (double)size_buff_success);
            j=1;
        }
        else
        {
            j++;
        }
        i++;
    }

    free( buff_success );
    free( name_success_file );
    free( name_pmf_success_file );

    if ( success_file ) fclose( success_file );
    if ( pmf_success_file ) fclose( pmf_success_file ); 

    return( 0 );
}
/*----------------------------------------------------------------------------------------------------*/



/* Function to calculate jitter */
/*------------------------------------------------------------------------------------------------------------------*/
int jitter( char *name_file_in, t_trace *trace, u_longlong_t size_trace, u_longlong_t total_packets, double delta_t )
{
    u_longlong_t i, counter;
    char *name_jitter_file, *name_pmf_jitter_file;
    double *buff_jitter, expect_value_jitter;
    FILE *jitter_file, *pmf_jitter_file;
       
    expect_value_jitter = 0;

    /* Memory allocation for jitter's buffer */
    if( ( buff_jitter = (double *)malloc( size_trace * sizeof(double) ) ) == NULL )
    {
        printf("\n\n\tError: Memory allocation!");
        return( -1 ); 
    }
    
    if( !(name_jitter_file = malloc(sizeof(char)*(strlen( name_file_in ) + strlen(".TM.jitter_trace") + 1)) ))    
    {
        fprintf( stderr, "\n\n\tError: Memory allocation!");
        return( -1 );
    }
    if( !(name_pmf_jitter_file = malloc(sizeof(char)*(strlen( name_file_in ) + strlen(".TM.jitter_pmf") + 1)) ))
    {
        fprintf( stderr, "\n\n\tError: Memory allocation!");
        return( -1 );
    }

            
    /* Creating names for out files */
    strcpy(name_jitter_file, name_file_in);
    strcat(name_jitter_file, ".TM.jitter_trace");

    strcpy(name_pmf_jitter_file, name_file_in);
    strcat(name_pmf_jitter_file, ".TM.jitter_pmf");

    /* Trying to create out files */
    if( !(jitter_file = fopen(name_jitter_file,"w+"))|| !(pmf_jitter_file = fopen(name_pmf_jitter_file,"w+")) )
    {
        printf("\n\tError: Can't create out file!\n");
        return( -1 );
    }
    
    /* Sorting trace in order of packet number */
    qsort( trace, size_trace, sizeof(t_trace), compareTrace );

    fprintf(jitter_file,"# Measure of Interest: Jitter\n");
    fprintf(jitter_file,"# This file contains the jitter of each packet\n");
    fprintf(jitter_file,"# X-axis: Packet Number / Y-axis: Jitter (msec)\n");
    /* Code for expected value of jitter
    fprintf(jitter_file,"# Expected Value = %.4f\n", expect_value_jitter);*/
/*debug - versao do fabiano*/
/*
    for( i=1; i<size_trace; i++ )
    {
      // Jitter is calculate between consecutive packets only 
      if( (trace[i].nseq - trace[i-1].nseq) == 1 )
      {
        buff_jitter[i] = (double)( ( trace[i].timestamp_rx - trace[i-1].timestamp_rx ) - delta_t  );
        fprintf( jitter_file,"\t%Ld   %.3f\n", trace[i].nseq, buff_jitter[i] / MSEC );      
        buff_jitter[i] = (int)( buff_jitter[i] * MSEC );
        expect_value_jitter += buff_jitter[i];
      }
      else
        buff_jitter[i] = MAXDOUBLE;
    }
    */
/**/
/*minha versao, considera jitters com perdas*/
    for( i=1; i<size_trace; i++ )
    { 
      if( (trace[i].nseq - trace[i-1].nseq) == 1 )
      {
        buff_jitter[i] = (double)( ( trace[i].timestamp_rx - trace[i-1].timestamp_rx ) - delta_t  );
        fprintf( jitter_file,"\t%Ld    %30.3f\n", trace[i].nseq, buff_jitter[i] * 1e-3 );  /*usec to msec*/                                                                    
        buff_jitter[i] = (int)( buff_jitter[i] * 1e3 );  /*msec to usec*/
        expect_value_jitter += buff_jitter[i];
      }
      else
      {
        buff_jitter[i] = (double)( ( trace[i].timestamp_rx - trace[i-1].timestamp_rx ) - delta_t * ( trace[i].nseq - trace[i-1].nseq )  );
        fprintf( jitter_file,"\t%Ld    %30.3f\n", trace[i].nseq, buff_jitter[i] * 1e-3 );  /*usec to msec*/                                                                    
        buff_jitter[i] = (int)( buff_jitter[i] * 1e3 );  /*msec to usec*/
        expect_value_jitter += buff_jitter[i];
      }      
    }
/**/
    expect_value_jitter /= size_trace;

    fprintf( pmf_jitter_file,"# Measure of Interest: PMF[Jitter]\n" );
    fprintf( pmf_jitter_file,"# This file contains the PMF of Jitter (msec)\n" );
    /* Code for expected value of jitter
    fprintf(pmf_jitter_file,"# Expected Value = %.4f\n", expect_value_jitter);*/

    /* Sorting buff_jitter to calculate PMF Jitter */
    qsort( buff_jitter, size_trace, sizeof(double), compareDouble );

    i = 0; counter = 1;

    while( (i < size_trace) && (buff_jitter[i] != MAXDOUBLE) )
    {
        if( ( buff_jitter[i] != buff_jitter[i+1] ) && ( buff_jitter[i] ) )
        {
            fprintf( pmf_jitter_file, "\t%.0f %.4f\n", buff_jitter[i], (double)counter / (double)total_packets );
            counter = 1;
        }
        else
        {
            counter++;
        }
        i++;
    }
    free( name_pmf_jitter_file );
    free( name_jitter_file );
    free( buff_jitter );
    if ( jitter_file ) fclose( jitter_file );
    if ( pmf_jitter_file )fclose( pmf_jitter_file );
    return( 0 );
}
/*------------------------------------------------------------------------------------------------------------------*/



/* New function to calculate the transfer's tax or throughput ( bytes/sec ) */
/*-------------------------------------------------------------------------------------------------------*/
int throughput( char *name_file_in, t_trace *trace, u_longlong_t size_trace, u_longlong_t total_packets )
{
     char *name_throughput_file;
     double tax, sum_size;
     u_longlong_t i, seq_packet;
     FILE *throughput_file;

     /* Initialing variables */
     sum_size = 0;
     seq_packet = 1;


    if( !( name_throughput_file = malloc( sizeof(char) * (strlen( name_file_in ) + strlen( ".TM.throughput" ) + 1) ) ) )    
    {
        fprintf( stderr, "\n\n\tError: Memory allocation!");
        return( -1 );
    }


     /* Creating output file */
     strcpy( name_throughput_file, name_file_in );
     strcat( name_throughput_file, ".TM.throughput" );

     if( !(throughput_file = fopen( name_throughput_file,"w+" ) ) )
     {
         fprintf( stderr, "\n\tError: Can't create out file!\n" );
         return ( -1 );
     }

     for( i=0; i<size_trace; i++ )
         sum_size += (double) trace[i].size;          
     
     
     /* Calculating: tax = total bytes/time of last arrived packet */
     
     /* Fabiano's Version
     tax = ( sum_size * USEC ) / ( (double)trace[size_trace-1].timestamp_rx );
     */
     
     /* Hugo's Version */
     if ( size_trace ) 
     {
         tax = ( sum_size * USEC ) / ( (double)trace[size_trace-1].timestamp_rx - (double)trace[0].timestamp_rx );
     }
     else tax = 0;
     
     /* Printing result */
     fprintf( throughput_file, "# Measure of Interest: Throughput\n" );
     fprintf( throughput_file, "# Throughput: %.2f bytes/sec\n", tax );

     free( name_throughput_file );
     if ( throughput_file ) fclose( throughput_file );
     return( 0 );
}
/*-------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------*/
int main( int argc, char **argv )
{
    int string_pointer=1;
    
    /* Variables used to call functions */
    int bool_throughput=0, 
        bool_jitter=0, 
        bool_loss=0, 
        bool_success=0;
        
    double delta_t;
    char strfile[MAXLINE];
    char *output_file;
    u_longlong_t total_packets, size_trace=0;
    u_longlong_t trash1;
    double trash2, trash3;
    int trash4;
    t_trace   *trace;
    FILE      *fp;

    delta_t = 0.0;
    /* Parsing command line */
    if( ( (argc < NUMARGS-1) ) || ( (argc > NUMARGS) && (strcmp(argv[argc-2],"-o")) ) || ( argc > NUMARGS+2 ) )
    {
        usage( argv[0] );
    }

    /* Parsing measure options */
    if( (argv[1][0] == '-') && (argv[1][1] != '\n') )
    {
        while( string_pointer < strlen(argv[1]) )
        {
            if( (argv[1][string_pointer] == 'l' ) /*&& !(bool_loss) */)              bool_loss = 1;
            else if( (argv[1][string_pointer] == 's' ) /*&& !(bool_success)*/ )      bool_success = 1;
            else if( (argv[1][string_pointer] == 'j' ) /*&& !(bool_jitter)*/ )       bool_jitter  = 1;
            else if( (argv[1][string_pointer] == 't' ) /*&& !(bool_throughput)*/)    bool_throughput = 1;
            else
            {
                  fprintf( stderr, "\n\tError: <measure options> must begin with '-' signal plus [jtl]\n\n" );
                   usage( argv[0] );
            }
            string_pointer++;
        }
    }
    else
    {
        fprintf( stderr, "\n\tError: <measure options> must begin with '-' signal plus [jtls]\n\n" );
        usage( argv[0] );
    }

    /* Opening input file for reading */
    if( ( fp = fopen(argv[2],"r") ) == NULL )
    {
        fprintf( stderr, "\n\tError: Can't open file '%s' or file doesn't exist\n\n", argv[2] );
        return( -1 );
    }
    
    if( bool_jitter )
    {    
        if( argc >= NUMARGS )
        {        
            /* Verifying float number (argv[3]) */
            if( ( delta_t = atof( argv[3] ) ) <= 0.0 )
            {
                fprintf( stderr, "\n\tError: Time interval should be a positive real\n\n" );
                if( fp ) fclose( fp );
                usage( argv[0] );
            }
        }
        else
        {
            if( fp )    
                fclose( fp );
            usage( argv[0] );
        }
    }

    if( !(strcmp(argv[argc-2],"-o")) )
        output_file = argv[argc-1];
    else
    {
        output_file = malloc( sizeof(char)*4 );
        strcpy( output_file,"out" );
    }
    
    
    /* Counting and reading received packets */

    /* First 2 lines is header */
    fgets( strfile, MAXLINE, fp );
    fgets( strfile, MAXLINE, fp );    
    
    trace = NULL;
    while( ! feof( fp ) )
    {
        if( fgets( strfile, MAXLINE, fp ) != NULL )
        {
            if( (sscanf(strfile, "%Ld %lf %lf %d", &trash1, &trash2, &trash3, &trash4)) == 4 )
            {
                size_trace++;                
                if( ( trace = (t_trace *)realloc( trace, size_trace * sizeof(t_trace) ) ) == NULL )   /*ATENCAO */
                {
                    fprintf( stderr, "\n\n\tError: Memory allocation!" );
                    return( -1 );
                }
                trace[size_trace-1].nseq = trash1;                   /*sequence number*/
                trace[size_trace-1].timestamp_tx = trash2;           /*transmition time*/
                trace[size_trace-1].timestamp_rx = trash2 + trash3;  /*arrived time*/
                trace[size_trace-1].size = trash4;                   /*packet size*/
            }         
        }
        /*elses aren't necessary because we desconsider empty/wrong lines*/
    }
    
    /*fprintf( stderr ,"Debug %d" , size_trace );*/
        
    /* Stopped reading, close file */
    fclose( fp );
    
    /*debug for ( i = 0 ; i < size_trace ; i++ ) fprintf( stderr , "%Ld %lf %d\n" , trace[i].nseq ,trace[i].timestamp_rx , trace[i].size );*/

    /* Testing empty input files */
    if( !size_trace )
    {
        fprintf( stderr, "\nError: File is EMPTY\n" );
        return( -1 );
    }

    total_packets = trace[size_trace-1].nseq;  /*sequence number of last packet*/

    fprintf( stderr, "Traffic Generator v2.0 - Copyright (C) 1999-2009\n");
    fprintf( stderr, "Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    fprintf( stderr, "LAND - Laboratory for Modeling and Development of Networks \n");    
    fprintf( stderr ,"Generating measures:\n" );
    
    /* Calling measures functions */
    if( bool_loss )
    {    
         fprintf( stderr ,"Loss...\n" );
         losses( output_file, trace, size_trace, total_packets );
    }
    if( bool_success )
    {
         fprintf( stderr ,"Sucess...\n" );
         success( output_file, trace, size_trace, total_packets );
    }
    if( bool_jitter )   
    {
         fprintf( stderr ,"Jitter...\n" ); 
         jitter( output_file, trace, size_trace, total_packets, delta_t );
    }
    if( bool_throughput )
    {  
         fprintf( stderr ,"Throughtput...\n" );
         throughput( output_file, trace, size_trace, total_packets );
    }
    fprintf( stderr ,"End of measures generation.\n" );
    /* heap memory free, closing file */
    free( trace );
    return( 0 );
}
/*-------------------------------------------------------------------------------------------------------*/
