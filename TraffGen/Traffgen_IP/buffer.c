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

/* to get temporarily a string */
char opt_met[MAXSTRING];

/* Size of Packets Send in Bits */
int pack[4];

/* Minimum delay get for each trace */
long long int min_pack_delay[4];

/* Band estimated by the method choosen */
long long int band;

/* remote host information */
char   file1[MAXSTRING], aux_file[MAXSTRING], new_file[MAXSTRING], sel_file[MAXSTRING];

/* files with to analize */
FILE *fp, *fp_result_1;


long int size_aux, size_aix;

t_aix *aux, *aix;


int usage(char *filename)
{
    printf ("Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("Usage: -<opt_band> <file1>\n");
    printf ("where: \n\t-<opt_band> \t\t method used to estimate Bottleneck Band\n ");
    printf ("\t\t 1: \t\t traditional method\n ");
    printf ("\t\t 2: \t\t Tamgram method\n ");
    printf ("\t<file1> \t\t files with trece to estimate buffer size\n ");
    return (1);
}


int parse_args(char **argv, int num_arg)
{
  int i;

  i = 1;


  strcpy (opt_met, argv[i]);
  i++;
  strcpy (file1, argv[i]);


  return (1);
}

/* Functions used to get Band of the file trace */
/*-----------------------------------------------------------------*/
long long int getBand( char file2[MAXSTRING] )
{
    long long int x_aix, max_value;
    long double last_y, y_aix;
    int flag=0;

    /*To avoid gcc warning max_value must be initialized here*/
    max_value = 0;
    last_y    = -10;

    /* Opening input file for reading */
    if( ( fp = fopen( file2, "r") ) == NULL )
    {
        fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file2 );
        return( -1 );
    }

    while( ! feof( fp ) )
    {
        if( fgets( strfile, MAXLINE, fp ) != NULL )
        {
            if( ( sscanf( strfile, "%lld %Lf", &x_aix, &y_aix ) ) == 2 )
            {
	            if( flag == 0 )
                {
	                max_value = x_aix;
	                last_y    = y_aix;
	                flag      = 1;
                }
                else if( y_aix > last_y )
                {
	                max_value = x_aix;
	                last_y    = y_aix;
                }
            }
        }
    }
    fclose( fp );
    return( max_value );
}

/* Functions used to get the file trace */
/*-----------------------------------------------------------------*/
long long int getMinValue( char file2[MAXSTRING] )
{
    long long int z_aix, x_aix, y_aix, min_value;
    long long int last_z = 0;

    /*To avoid gcc warning min_value must be initialized here*/
    min_value = 0;

    /* Opening input file for reading */
    if( ( fp = fopen( file2, "r" ) ) == NULL )
    {
        fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file2 );
        return( -1 );
    }

    while( ! feof( fp ) )
    {
        if( fgets( strfile, MAXLINE, fp ) != NULL )
        {
            if( ( sscanf( strfile, "%lld %lld %lld", &z_aix, &x_aix, &y_aix)) == 3 )
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
    return( min_value );
}



/* Functions used to get the file trace */
/*-----------------------------------------------------------------*/
int getAux( char file2[MAXSTRING], int n_seq )
{
   long long int z_aux, x_aux, y_aux;

  /* Opening input file for reading */
  if( ( fp = fopen(file2,"r") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file2 );
      return( -1 );
  }
  if (n_seq == 1)
  {
     size_aux=0;
     aux = NULL;
  }
  while( ! feof( fp ) )
  {
      if( fgets( strfile, MAXLINE, fp ) != NULL )
      {
              if( (sscanf(strfile, "%lld %lld %lld", &z_aux, &x_aux, &y_aux)) == 3 )
              {
		  size_aux++;
        	  if( ( aux = (t_aix *)realloc( aux, size_aux * sizeof(t_aix) ) ) == NULL )
        	  {
                      fprintf( stderr, "\n\n\tError: Memory allocation!" );
                      return( -1 );
        	  }
        	  aux[size_aux-1].x_aix = z_aux;
		  /* Include in Y_AUX the Estimated Buffer Size*/
                  aux[size_aux-1].y_aix = y_aux-min_pack_delay[n_seq-1];
                  aux[size_aux-1].y_aix =(double)((double)(y_aux-min_pack_delay[n_seq-1])/1000000)*band;
		  /* Transform to Bytes*/
                  aux[size_aux-1].y_aix = (double)((double)aux[size_aux-1].y_aix/8);
               }
      }
  }
  fclose( fp );
  return(size_aux);
}


int main (int argc, char **argv)
{
  long int size_t_list, cont_trace;

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

  /* parse the arguments */
  if ((opt_met[1]!='1')&&(opt_met[1]!='2')) {
    usage(argv[0]);
    return (-1);
  }

  pack[0] = 32 * 8;
  pack[1] = 80 * 8;
  pack[2] = 200 * 8;
  pack[3] = 500 * 8;

/*  strcpy (sel_file, file1);
  strcat(sel_file, ".1.pair");
  min_pack_delay[0] = getMinValue(sel_file);

  strcpy (sel_file, file1);
  strcat(sel_file, ".2.pair");
  min_pack_delay[1] = getMinValue(sel_file);

  strcpy (sel_file, file1);
  strcat(sel_file, ".3.pair");
  min_pack_delay[2] = getMinValue(sel_file);*/

  strcpy (sel_file, file1);
  strcat(sel_file, ".4.pair");
  min_pack_delay[3] = getMinValue(sel_file);


  if (opt_met[1]=='1')
  {
     strcpy (sel_file, file1);
     strcat(sel_file, ".pair.band.pmf");
     band = getBand(sel_file);
  } else
  if (opt_met[1]=='2')
  {
     strcpy (sel_file, file1);
     strcat(sel_file, ".pair.band.new.pmf");
     band = getBand(sel_file);
  }


/*  strcpy (aux_file, file1);
  strcat(aux_file, ".1.loss");
  size_t_list = getAux(aux_file, 1);

  strcpy (aux_file, file1);
  strcat(aux_file, ".2.loss");
  size_t_list = getAux(aux_file, 2);

  strcpy (aux_file, file1);
  strcat(aux_file, ".3.loss");
  size_t_list = getAux(aux_file, 3);*/

  strcpy (aux_file, file1);
  strcat(aux_file, ".4.loss");
  size_t_list = getAux(aux_file, 4);


  strcpy (new_file, file1);
  strcat (new_file, ".pair.buffer");
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

  free( aux );
  free( aix );

  return( 0 );
}
