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

#include "linear.h"


 
/* global variables */



/* file trace */
char   file[MAXSTRING];


/* files with to analize */
FILE *lossfp, *okfp, *fp;

char strfile[MAXLINE];

long int size_trace1, size_trace2;

t_aix *aix1, *aix2;


int usage(char *filename)
{
    printf ("Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("Usage: \n");
    printf ("<file>\n");
    printf ("where: \n\t<file> \t\t\t file with trece\n ");
    
    return (1);
}


int parse_args(char **argv, int num_arg) 
{
  int i;
  
  i = 1;

  
  strcpy (file, argv[i]);
  
  return (1);
}


/* Functions used to get linear equation */
/*-----------------------------------------------------------------*/
int getLoss( char file[MAXSTRING] )
{

   long long int x_aux, y_aux;
   int z_aux;
   int i,j;
   char aux_file[MAXSTRING];


  size_trace1=0;
  size_trace2=0;
    
  strcpy(aux_file, file);

  /* Opening input file for reading */
  if( ( fp = fopen(file,"r") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file );
      return( -1 );
  } 

  aix1 = NULL;
  aix2 = NULL;
  
  while( ! feof( fp ) )
  {
      if( fgets( strfile, MAXLINE, fp ) != NULL )
      {
	 if( (sscanf(strfile, "%d %lld %lld", &z_aux, &x_aux, &y_aux)) == 3 )
	 {
	 
	     if ( size_trace1 == 0 )
	     {
	        size_trace1++;
        	if( ( aix1 = (t_aix *)realloc( aix1, size_trace1 * sizeof(t_aix) ) ) == NULL )
        	{
        	    fprintf( stderr, "\n\n\tError: Memory allocation!" );
        	    return( -1 );
                }
                aix1[size_trace1-1].x_aix = x_aux;
                aix1[size_trace1-1].y_aix = y_aux;
	        aix1[size_trace1-1].z_aix = z_aux;		
             }
	     else
	     {
		if ( aix1[size_trace1-1].z_aix != z_aux )
		{
	           size_trace1++;
        	   if( ( aix1 = (t_aix *)realloc( aix1, size_trace1 * sizeof(t_aix) ) ) == NULL )
        	   {
        	       fprintf( stderr, "\n\n\tError: Memory allocation!" );
        	       return( -1 );
                   }
                   aix1[size_trace1-1].x_aix = x_aux;
                   aix1[size_trace1-1].y_aix = y_aux;
	           aix1[size_trace1-1].z_aix = z_aux;
        	} else
		{
                   size_trace2++;
        	   if( ( aix2 = (t_aix *)realloc( aix2, size_trace2 * sizeof(t_aix) ) ) == NULL )
        	   {
        	       fprintf( stderr, "\n\n\tError: Memory allocation!" );
        	       return( -1 );
        	   }	 	     
                   aix2[size_trace2-1].x_aix = x_aux;
                   aix2[size_trace2-1].y_aix = y_aux;
	           aix2[size_trace2-1].z_aix = z_aux;

        	}
             }
	 }
      }
  }
  
  fclose( fp );
    
  i=0; j=0;

  strncat( aux_file, ".ok", 3);
    
  /* Opening input file for reading */
  if( ( okfp = fopen(aux_file,"w+") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file );
      return( -1 );
  }
  
  strcpy(aux_file, file);
  strncat( aux_file, ".loss", 5);
  
  /* Opening input file for reading */
  if( ( lossfp = fopen(aux_file,"w+") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file );
      return( -1 );
  } 

  
  while ( i < size_trace1 )
  {
     if ( aix1[i].z_aix == aix2[j].z_aix )
     {
        fprintf( okfp, "%d\t%lld\t%lld\n", aix1[i].z_aix, aix1[i].x_aix, aix1[i].y_aix);
        fprintf( okfp, "%d\t%lld\t%lld\n", aix2[j].z_aix, aix2[j].x_aix, aix2[j].y_aix);
        j++;
     }
     else
     {
        fprintf( lossfp, "%d\t%lld\t%lld\n", aix1[i].z_aix, aix1[i].x_aix, aix1[i].y_aix);
     }
     i++;
  }  
  fclose( okfp );
  fclose( lossfp );
  
  return (1); 
  
}
/*-----------------------------------------------------------------*/


int main (int argc, char **argv)
{
   
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

  getLoss( file );
  free( aix1 );
  free( aix2 );    

  return( 0 );
}
