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
    Last Update: 05/10/2004

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

#include "check_dup.h"


 
/* global variables */



/* file trace */
char   file[MAXSTRING];


/* files with to analize */
FILE *fp, *okfp;

char strfile[MAXLINE];

long int size_trace, size_inic, i;

t_aix *aix;
t_inic *aux;


int usage(char *filename)
{
    printf ("Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("Usage: \n");
    printf ("\t%s <file>\n",filename);
    printf ("where: \n\t<file> \t\t\t file with trace\n ");
    
    return (1);
}


int parse_args(char **argv, int num_arg) 
{
  int i;
  
  i = 1;

  
  strcpy (file, argv[i]);
  
  return (1);
}


/* Function used to get dup packs */
/*-----------------------------------------------------------------*/
int getDup( char file[MAXSTRING] )
{

   long long int x_aux, y_aux;
   int z_aux, w_aux;

  size_trace=0;
  size_inic=0;
    
  /* Opening input file for reading */
  if( ( fp = fopen(file,"r") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file );
      return( -1 );
  } 

  aix = NULL;
  aux = NULL;
  
  while( ! feof( fp ) )
  {
      if( fgets( strfile, MAXLINE, fp ) != NULL )
      {
      if (strfile[0] == '#')
      {
	        size_inic++;
        	if( ( aux = (t_inic *)realloc( aux, size_inic * sizeof(t_inic) ) ) == NULL )
        	{
        	    fprintf( stderr, "\n\n\tError: Memory allocation!" );
        	    return( -1 );
            }
            strcpy (aux[size_inic-1].strfile, strfile);
      }else
	  if( (sscanf(strfile, "%d %lld %lld %d", &z_aux, &x_aux, &y_aux, &w_aux)) == 4 )
	  {
	 
	     if ( size_trace == 0 )
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
	        aix[size_trace-1].w_aix = w_aux;            
         }
	     else
         if ( aix[size_trace-1].x_aix < x_aux )
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
               aix[size_trace-1].w_aix = w_aux;  
          } //else printf("%d----%lld\n", z_aux, x_aux);
      }
  }
  }
  
  fclose( fp );
    
  i=0;

  /* Opening file for writing */
  if( ( okfp = fopen(file,"w+") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file );
      return( -1 );
  }

  for( i=0; i<size_inic; i++ )
  {
      fprintf( okfp, "%s", aux[i].strfile);
  }
    
  for( i=0; i<size_trace; i++ )
  {
      fprintf( okfp, "%d\t%lld\t%lld\t%d\n", aix[i].z_aix, aix[i].x_aix, aix[i].y_aix, aix[i].w_aix);
  }
  
  fclose( okfp );
  
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

  getDup( file );
  free( aix );

  return( 0 );
}
