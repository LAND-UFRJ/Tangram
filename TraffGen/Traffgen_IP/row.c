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
        Last Update: 08/04/2003

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

#include <math.h>
#include "row.h"

/* global variables */



/* file name */
char   file[MAXSTRING];

/* Column of File to Plot*/
int column;
char   column_c[MAXSTRING];

/* Sequence for difference*/
char   sequence[MAXSTRING];

/* files with to analize */
FILE *fp;

/* To keep the data of files */
t_aix *aix;

/* size of trace*/
long int size_trace=0;



int usage(char *filename)
{
    printf ("Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("Usage: \n");
    printf ("-<column option> -<pair option> <file>\n");
    printf ("where: \n\t-<column option> \t row (1 or 2) to calculate values between rows\n");
    printf ("\t-<pair option> \t\t comparation between rows (continuos or pair)\n");
    printf ("\t<file> \t\t\t file with trece\n");
    
    return (1);
}


int parse_args(char **argv, int num_arg) 
{
  int i;
  
  i = 1;
    
  strcpy (column_c, argv[i]);
  i++;

  /* Verifying column number (argv[3]) */
  column = atoi( column_c );
  column = column*(-1);
  
  
  if( column < 1 )
  {
      fprintf( stderr, "\n\tError: Column option should be a positive real value\n\n" );
      return(-1);
  }  

  strcpy (sequence, argv[i]);
  i++;
  
  strcpy (file, argv[i]);
  
  return (1);
}

/* Functions used to get value of files */
/*-----------------------------------------------------------------*/
int getValue( char file[MAXSTRING] )
{

   long long int x_aux, y_aux;
   int z_aux;

   size_trace = 0;

   /* Opening input file for reading */
   if( ( fp = fopen(file,"r") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file );
       return( -1 );
   } 

   aix = NULL;  

   while( ! feof( fp ) )
   {
       if( fgets( file, MAXLINE, fp ) != NULL )
       {
          if (column == 2)
	  {
             if( (sscanf(file, "%d %lld %lld", &z_aux, &x_aux, &y_aux)) == 3 );

	  }
          if (column == 1)
	  {
             if( (sscanf(file, "%d %lld %lld", &z_aux, &y_aux, &x_aux)) == 3 );
	  }
	  if ((column == 1)||(column == 2))
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

             } else
	     {
                fprintf( stderr, "\n\n\tError: Allocating File in Memory!" );
	     }
       }
     }
     
  fclose( fp );
  return (0);
}



int main (int argc, char **argv)
{
char aux_file[MAXSTRING];

int cont_trace, dif_row;


  /* check for the number of arguments */
  if ( argc < 3 ) {
    usage(argv[0]);
    return (-1);
  }

  /* parse the arguments */
  if (parse_args(argv, argc) < 0) {
    usage(argv[0]);
    return (-1);
  }
  
  strcpy(aux_file, file);
  
  getValue(file);
  

  strncat (aux_file, ".row", 4);  
  /* Trying to create out file*/
  if( !(fp = fopen(aux_file,"w+") ) )
  {
      printf("\n\tError: Can't create out file!\n");
      return( -1 );
  }
  
  if (( sequence[1]=='c')||( sequence[1]=='C'))
  {  
     for (cont_trace = 1; cont_trace < size_trace; cont_trace++)
     {
	dif_row = aix[cont_trace].y_aix - aix[cont_trace-1].y_aix;
	fprintf(fp, "%d\t%d\n", aix[cont_trace-1].z_aix, dif_row);
     }
  } else
  if (( sequence[1]=='p')||( sequence[1]=='P'))
  {  
     for (cont_trace = 1; cont_trace < size_trace; cont_trace++)
     {
        if (aix[cont_trace-1].z_aix == aix[cont_trace].z_aix)
	{
	   dif_row = aix[cont_trace].y_aix - aix[cont_trace-1].y_aix;
	   fprintf(fp, "%d\t%d\n", aix[cont_trace-1].z_aix, dif_row);
           cont_trace++;
	}
     }
  } else
  {
     printf("\n\tError: The sequence should be -c or -p!\n");
     return( -1 );
  }  
  
  fclose( fp );
  free( aix );
  return(0);
   
}  
