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
        Last Update: 07/11/2002

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

#include "adjust_trace.h"



/* global variables */



/* file with trace to be clean */
char   file1[MAXSTRING];
/* file with offset and skew to remove of file 1 */
char   file2[MAXSTRING];

/* files with to analize */
FILE *fp;

char strfile[MAXLINE];

long int size_trace=0;
double skew=0;
double offset=0;
double start_trace=0;


t_aix *aix;
t_aix *stack;


int usage(char *filename)
{
    printf ("Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("Usage: \n");
    printf ("<file1> <file2>\n");
    printf ("where:\n\t[<file1>] \t\t\t file with trece\n");
    printf ("\t<file2> \t\t\t file with offset and skew information\n");    
    
    return (1);
}


int parse_args(char **argv, int num_arg) 
{
  int i;
  
  i = 1;

  strcpy (file1, argv[i]);
  i++;
  strcpy (file2, argv[i]);
    
  return (1);
}



/* Functions used to get File */
/*-----------------------------------------------------------------*/
int getFile( char file_trace[MAXSTRING], char file_data[MAXSTRING] )
{

   long long int x_aux, y_aux;
   int z_aux, i;
   double data_aux;
   char data_type[MAXLINE];

   size_trace=0;

  /* Opening input file trace for reading */
  if( ( fp = fopen(file_trace,"r") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file_trace );
      return( -1 );
  } 

  aix = NULL;  
  
  while( ! feof( fp ) )
  {
      if( fgets( strfile, MAXLINE, fp ) != NULL )
      {
	 if( (sscanf(strfile, "%d %lld %lld", &z_aux, &x_aux, &y_aux)) == 3 )
	 {
	     size_trace++;
             if( ( aix = (t_aix *)realloc( aix, size_trace * sizeof(t_aix) ) ) == NULL )
             {
        	 fprintf( stderr, "\n\n\tError: Memory allocation!" );
        	 return( -1 );
             }
             aix[size_trace-1].x_aix = x_aux;
             aix[size_trace-1].y_aix = y_aux;
	     aix[size_trace-1].z_aix = z_aux;
	 }
      }
  }
  
  fclose( fp );

  /* Opening input file DATA for reading */
  if( ( fp = fopen(file_data,"r") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file_data );
      return( -1 );
  } 
  i = 0;
  while( ! feof( fp ) )
  {
      if( fgets( strfile, MAXLINE, fp ) != NULL )
      {
	 if( (sscanf(strfile, "%s %lf", data_type, &data_aux)) == 2 )
	 {   
	     i++;
	     if (i == 1)
                start_trace = data_aux;
	     else if ( i == 2)
	        skew = data_aux;
	     else if ( i == 3)
	        offset = data_aux;
	 }
      }
  }

  /*fprintf( stdout, "start_trace: %lf\n", start_trace);
  fprintf( stdout, "skew: %1.10f\n", skew);
  fprintf( stdout, "offset: %lf\n", offset);*/
  
  fclose( fp );
  
  return(0);

}
/*-----------------------------------------------------------------*/


/* Functions used to get linear equation */
/*-----------------------------------------------------------------*/
int remSkew( char file_trace[MAXSTRING] )
{

  double new_y = 0, temp_y = 0;
  long int cont_trace;
  

  strncat(file_trace, ".trace", 6);
  
  /* Creating output file for writing */
  if( ( fp = fopen(file_trace,"w+") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file_trace );
      return( -1 );
  }


  for ( cont_trace = 1; cont_trace <= size_trace; cont_trace++ )
  {
    temp_y = ((double)((double)(aix[cont_trace-1].x_aix - start_trace) * (double)skew));
    
    new_y = ( aix[cont_trace-1].y_aix - temp_y - offset);


    fprintf( fp, "%d \t %lld \t %.0f\n", aix[cont_trace-1].z_aix, aix[cont_trace-1].x_aix, new_y ); 

  }

  fclose(fp);

  return(0);

}



int main (int argc, char **argv)
{
  int ret_func;
  
  /* check for the number of arguments */
  if ( argc < 2 ) {
    usage(argv[0]);
    return (-1);
  }

  /* parse the arguments */
  if (parse_args(argv, argc) < 0) {
    usage(argv[0]);
    return (-1);
  }

  if ( argc == 3 )
  {

    ret_func = getFile( file1, file2 );
    ret_func = remSkew ( file1 );
    free( aix );    
  }
  
     
  return( 0 );
}
