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

#include "band.h"



/* global variables */

/* to get temporarily a string */
char strfile[MAXLINE];

/* Size of Packets Send in Bits */
int pack[4];

/* Minimum delay get for each trace */
long long int min_pack_delay[4];

/* remote host information */
char   file1[MAXSTRING], aux_file[MAXSTRING], new_file[MAXSTRING], sel_file[MAXSTRING];

/* files with to analize */
FILE *fp, *fp_result_1, *fp_result_2;


long int size_aux, size_aix;

t_aix *aux, *aix;


int usage(char *filename)
{
    printf ("Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("Usage: \n");
    printf ("<file1>\n");
    printf ("where: \n\t<file1> \t\t files with trace to detect Bottleneck Band\n ");
    return (1);
}


int parse_args(char **argv, int num_arg)
{
  int i;

  i = 1;


  strcpy (file1, argv[i]);


  return (1);
}

/* Functions used to get the file trace */
/*-----------------------------------------------------------------*/
long long int getMinValue( char file2[MAXSTRING] )
{
    long long int z_aix, x_aix, y_aix, min_value;
    long long int last_z = 0;

    /*To remove gcc warning min_value must be initialized here*/
    min_value = 0;
    /* Opening input file for reading */
    if( ( fp = fopen( file2, "r" ) ) == NULL )
    {
        fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file2 );
        return( -1 );
    }

    while( !feof( fp ) )
    {
        if( fgets( strfile, MAXLINE, fp ) != NULL )
        {
            if( ( sscanf( strfile, "%lld %lld %lld", &z_aix, &x_aix, &y_aix ) ) == 3 )
            {
                if( last_z == 0 )
                {
                   min_value = y_aix;
                }
                if( y_aix < min_value )
                {
                   min_value = y_aix;
                }
                last_z = z_aix;
            }
        }
    }
    fclose( fp );
    return(min_value);
}


/* Functions used to get the file trace */
/*-----------------------------------------------------------------*/
int getAix( char file2[MAXSTRING], int n_seq )
{
   long long int z_aix, x_aix, y_aix;
   long long int last_z = 0;


  /* Opening input file for reading */
  if( ( fp = fopen(file2,"r") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file2 );
      return( -1 );
  }

  size_aix=0;
  size_aux=1;
  aix = NULL;

  while( ! feof( fp ) )
  {
      if( fgets( strfile, MAXLINE, fp ) != NULL )
      {
              if( (sscanf(strfile, "%lld %lld %lld", &z_aix, &x_aix, &y_aix)) == 3 )
              {
	          if (z_aix == last_z)
		  {
		     size_aux++;
		  }
	          if (( y_aix <= (min_pack_delay[n_seq-1]+((double)min_pack_delay[n_seq-1] * 0.1)))&&(z_aix != last_z))
		  {
		     size_aix++;
        	     if( ( aix = (t_aix *)realloc( aix, size_aix * sizeof(t_aix) ) ) == NULL )
        	     {
                	 fprintf( stderr, "\n\n\tError: Memory allocation!" );
                	 return( -1 );
        	     }
		     if (z_aix==aux[size_aux-1].x_aix)
		     {
        		 aix[size_aix-1].x_aix = z_aix;
			 /* Include in Y_AUX the Band for the PAIR in Bits/seg from
			 the PACKET ROW accumulated in the latter function, this
			 sequence accumulate GUTO's Propose for the Packet Pair*/
                	 aix[size_aix-1].y_aix = aux[size_aux-1].y_aix;
		     }
                  }
                  last_z = z_aix;
              }
      }
  }
  fclose( fp );
  return(size_aix);
}



/* Functions used to get the file trace */
/*-----------------------------------------------------------------*/
int getAux( char file2[MAXSTRING], int n_seq )
{
   long long int x_aux, y_aux;

  /* Opening input file for reading */
  if( ( fp = fopen(file2,"r") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file2 );
      return( -1 );
  }

  size_aux=0;
  aux = NULL;

  while( ! feof( fp ) )
  {
      if( fgets( strfile, MAXLINE, fp ) != NULL )
      {
              if( (sscanf(strfile, "%lld %lld", &x_aux, &y_aux)) == 2 )
              {
		  size_aux++;
        	  if( ( aux = (t_aix *)realloc( aux, size_aux * sizeof(t_aix) ) ) == NULL )
        	  {
                      fprintf( stderr, "\n\n\tError: Memory allocation!" );
                      return( -1 );
        	  }
        	  aux[size_aux-1].x_aix = x_aux;
		  /* Include in Y_AUX the Band for the PAIR in Bits/seg */
                  aux[size_aux-1].y_aix = (double)(((double)pack[n_seq-1]/(double)((double)y_aux/1000000)));
               }
      }
  }
  fclose( fp );
  return(size_aux);
}


int main (int argc, char **argv)
{
  long int size_t_list, cont_trace, size_aix_list;

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

  pack[0] = 32 * 8;
  pack[1] = 80 * 8;
  pack[2] = 200 * 8;
  pack[3] = 500 * 8;

/* Os pacotes != de 500 bytes estavam estimando banda errada*/
/*  strcpy (aux_file, file1);
  strcat(aux_file, ".1.pair.row");
  size_t_list = getAux(aux_file, 1);

  strcpy (aux_file, file1);
  strcat(aux_file, ".2.pair.row");
  size_t_list = getAux(aux_file, 2);

  strcpy (aux_file, file1);
  strcat(aux_file, ".3.pair.row");
  size_t_list = getAux(aux_file, 3);*/

  strcpy (aux_file, file1);
  strcat(aux_file, ".4.pair.row");
  size_t_list = getAux(aux_file, 4);


  strcpy (new_file, file1);
  strcat (new_file, ".pair.TM.band");
  /* Trying to create out file*/
  if( !(fp_result_1 = fopen(new_file,"w+") ) )
  {
      printf("\n\tError: Can't create out file!\n");
      return( -1 );
  }


  for (cont_trace = 0; cont_trace < size_t_list; cont_trace++)
  {
     fprintf(fp_result_1, "%lld\t%.0Lf\n", aux[cont_trace].x_aix, aux[cont_trace].y_aix);
  }

  fclose( fp_result_1 );

/* Os pacotes != de 500 bytes estavam estimando banda errada*/
/*  strcpy (sel_file, file1);
  strcat(sel_file, ".1.pair");
  min_pack_delay[0] = getMinValue(sel_file);
  size_aix_list = getAix(sel_file, 1);

  strcpy (sel_file, file1);
  strcat(sel_file, ".2.pair");
  min_pack_delay[1] = getMinValue(sel_file);
  size_aix_list = getAix(sel_file, 2);

  strcpy (sel_file, file1);
  strcat(sel_file, ".3.pair");
  min_pack_delay[2] = getMinValue(sel_file);
  size_aix_list = getAix(sel_file, 3);  */

  strcpy (sel_file, file1);
  strcat(sel_file, ".4.pair");
  min_pack_delay[3] = getMinValue(sel_file);
  size_aix_list = getAix(sel_file, 4);

  strcpy (new_file, file1);
  strcat (new_file, ".pair.TM.band.new");
  /* Trying to create out file*/
  if( !(fp_result_2 = fopen(new_file,"w+") ) )
  {
      printf("\n\tError: Can't create out file!\n");
      return( -1 );
  }


  for (cont_trace = 0; cont_trace < size_aix_list; cont_trace++)
  {
     fprintf(fp_result_2, "%lld\t%.0Lf\n", aix[cont_trace].x_aix, aix[cont_trace].y_aix);
  }

  fclose( fp_result_2 );
  free( aux );
  free( aix );

  return( 0 );
}
