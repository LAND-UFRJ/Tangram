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

#include "convolution.h"



/* global variables */

/* to get temporarily a string */
char strfile[MAXLINE];

/* remote host information */
char   file1[MAXSTRING], file2[MAXSTRING];

/* files with to analize */
FILE *fp;


long int size_trace;
long long int max_x_1, max_x_2, min_x_1, min_x_2;
long int size_list_1, size_list_2, size_list_3;
long int cont_trace_1, cont_trace_2;



t_aix *aix, *aux;


int usage(char *filename)
{
    printf ("Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("Usage: \n");
    printf ("<file1> <file2>\n");
    printf ("where: \n\t<file1> and <file2> \t\t\t are the files with treces\n ");

    return (1);
}


int parse_args(char **argv, int num_arg)
{
  int i;

  i = 1;


  strcpy (file1, argv[i]);
  i++;
  strcpy (file2, argv[i]);
  i++;

  return (1);
}

/* Functions used to get value of x in function */
/*-----------------------------------------------------------------*/
long double get_Y_Value( long long int ver_x, int flag )
{

   cont_trace_1 = 1;
   cont_trace_2 = 1;

   if (flag == 1)
   {
      while (( aix[cont_trace_1-1].x_aix < ver_x )&&( cont_trace_1 < size_list_1 ))
      {
         cont_trace_1++;
      }
      if ( aix[cont_trace_1-1].x_aix == ver_x )
      {
         return(aix[cont_trace_1-1].y_aix);
      }
      else
      {
         return(0);
      }
   }
   else
   {
      while (( aux[cont_trace_2-1].x_aix < ver_x )&&( cont_trace_2 < size_list_2 ))
      {
         cont_trace_2++;
      }
      if ( aux[cont_trace_2-1].x_aix == ver_x )
      {
         return(aux[cont_trace_2-1].y_aix);
      }
      else
      {
         return(0);
      }
   }

}

/* Functions used to get convolution result */
/*-----------------------------------------------------------------*/
long int getResult( long long int max_x, long long int min_x)
{

   long long int x_aux, var_x, min_x_total;
   long double y_aux;
   long double y_aux_1, y_aux_2;

   if (min_x_1 < min_x_2)
   {
      min_x_total = min_x_1;
   }
   else
   {
      min_x_total = min_x_2;
   }

   size_trace=0;

   for (x_aux = min_x; x_aux <= max_x; x_aux++)
//   for (x_aux = 0; x_aux <= max_x; x_aux++)
   {
      y_aux = 0;
      for ( var_x = min_x_total; var_x <= x_aux; var_x++)
//      for ( var_x = 0; var_x <= x_aux; var_x )
      {
         y_aux_1 = get_Y_Value(var_x, 1);
	 y_aux_2 = get_Y_Value(x_aux - var_x, 2);
         //printf("+ %.10llf * %.10llf\n", y_aux_1, y_aux_2);
         y_aux = y_aux + ((long double) y_aux_1 * y_aux_2);
      }
      //if ( y_aux != 0)
      //{
         printf("%lld\t%.10Lf\n", x_aux * 1000, y_aux);
      //}

   }


   return(size_trace);
}


/* Functions used to get trace */
/*-----------------------------------------------------------------*/
int getFile( char file[MAXSTRING], int flag )
{

   long long int x_aux;
   long double y_aux;


  size_trace=0;
  /* Opening input file for reading */
  if( ( fp = fopen(file,"r") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file );
      return( -1 );
  }
  if (flag == 1)
  {
     aix = NULL;
  }
  else
  {
     aux = NULL;
  }
  while( ! feof( fp ) )
  {
      if( fgets( strfile, MAXLINE, fp ) != NULL )
      {
              if( (sscanf(strfile, "%lld %Lf", &x_aux, &y_aux)) == 2 )
              {
		  size_trace++;
		  if (flag == 1)
		  {
        	      if( ( aix = (t_aix *)realloc( aix, size_trace * sizeof(t_aix) ) ) == NULL )
        	      {
                	  fprintf( stderr, "\n\n\tError: Memory allocation!" );
                	  return( -1 );
        	      }
        	      aix[size_trace-1].x_aix = x_aux / 1000;
        	      aix[size_trace-1].y_aix = y_aux;
		  }
		  else
		  {
        	      if( ( aux = (t_aix *)realloc( aux, size_trace * sizeof(t_aix) ) ) == NULL )
        	      {
                	  fprintf( stderr, "\n\n\tError: Memory allocation!" );
                	  return( -1 );
        	      }
        	      aux[size_trace-1].x_aix = x_aux / 1000;
        	      aux[size_trace-1].y_aix = y_aux;
		  }
              }
      }
  }
  fclose( fp );
  return(size_trace);
}


int main( int argc, char **argv )
{

    /* check for the number of arguments */
    if( argc < 3 )
    {
        usage(argv[0]);
        return (-1);
    }

    /* parse the arguments */
    if( parse_args( argv, argc ) < 0 )
    {
        usage( argv[0] );
        return( -1 );
    }

    size_list_1 = getFile( file1, 1 );
    min_x_1     = aix[0].x_aix;
    max_x_1     = aix[size_list_1-1].x_aix;
    size_list_2 = getFile( file2, 2 );
    min_x_2     = aux[0].x_aix;
    max_x_2     = aux[size_list_2-1].x_aix;

    size_list_3 = getResult( max_x_1 + max_x_2, min_x_1 + min_x_2 );

    free( aix );
    free( aux );
    return( 0 );
}
