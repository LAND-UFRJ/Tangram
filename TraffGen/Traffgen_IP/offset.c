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

#include "offset.h"



/* global variables */

/* to get temporarily a string */
char strfile[MAXLINE];

/* remote host information */
char   file[MAXSTRING], aux_file[MAXSTRING], new_file[MAXSTRING];

/* files with to analize */
FILE *fp;


long int size_trace;

t_aix *aix;


int usage(char *filename)
{
    printf ("Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("Usage: \n");
    printf ("offset <file>\n");
    printf ("where: \n");
    printf ("\t<file> \t\t\t File with trace\n ");
    
    return (1);
}


int parse_args(char **argv, int num_arg) 
{
  int i;
  
  i = 1;

  
  strcpy (file, argv[i]);
  i++;
  
  
  return (1);
}



/* Functions used to get linear equation */
/*-----------------------------------------------------------------*/
int getFile( char file[MAXSTRING] )
{

   long long int x_aux, y_aux;
   int z_aux;


  size_trace=0;
  /* Opening input file for reading */
  if( ( fp = fopen(file,"r") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file );
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
  return(size_trace);
}


int main (int argc, char **argv)
{
  int size_list, list, sec_list, cont_pack;
  double psize;
  long int posic_min;
   
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


  size_list = getFile(file);

  strcpy (new_file, file);
  strncat (new_file, ".offset", 7);  
  /* Trying to create out file*/
  if( !(fp = fopen(new_file,"w+") ) )
  {
      printf("\n\tError: Can't create out file!\n");
      return( -1 );
  }   

  list = 1;
  sec_list = 0;
  psize = 12.8;
  while ( sec_list < 4 )
  {
     cont_pack = (aix[sec_list].z_aix-1)%4;
     psize = psize * 2.5 ;
     posic_min=-1;
     while ( list <= size_list )
     { 
       if ( sec_list == (aix[list-1].z_aix-1)%4 )
       {
          if ( posic_min == -1 )
	  {
	     posic_min = list-1;
	  } else
	  {
             if ( aix[list-1].y_aix < aix[posic_min].y_aix )
	     {
		posic_min = list-1;
	     }
	  }
       }
       list++;
     }

     fprintf(fp, "%3.0f \t", psize);
     fprintf(fp, "%lld \t", aix[posic_min].x_aix);
     fprintf(fp, "%lld \n", aix[posic_min].y_aix);     

     sec_list++;
     list=1;
  }
  fclose(fp);


  list = 1;
  sec_list = 0;
  psize = 12.8;
  while ( sec_list < 4 )
  {
     strcpy (new_file, file);
     cont_pack = (aix[sec_list].z_aix-1)%4;
     strncat (new_file, "xxxx", sec_list+1);
     psize = psize * 2.5 ;

     /* Trying to create out file*/
     if( !(fp = fopen(new_file,"w+") ) )
     {
         printf("\n\tError: Can't create out file!\n");
         return( -1 );
     }    
     while ( list <= size_list )
     { 
       if ( sec_list == (aix[list-1].z_aix-1)%4 )
       {
          //fprintf(fp, "%3.0f \t", psize);
	  fprintf(fp, "%d \t", aix[list-1].z_aix);
	  fprintf(fp, "%lld \t", aix[list-1].x_aix);
	  fprintf(fp, "%lld \n", aix[list-1].y_aix);
       }
       list++;
     }
     sec_list++;
     list=1;
     fclose(fp);
  }
  
  
  free( aix );
  return( 0 );
}
