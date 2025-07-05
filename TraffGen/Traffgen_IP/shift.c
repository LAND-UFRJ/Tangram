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
        Last Update: 06/02/2003

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

#include "shift.h"



/* global variables */

/* to get temporarily a string */
char strfile[MAXLINE];

/* remote host information */
char   file1[MAXSTRING], file2[MAXSTRING], aux_file[MAXSTRING], new_file[MAXSTRING];

/* files with to analize */
FILE *fp, *fp_result_1, *fp_result_2;


long int size_trace, size_aux;

t_aix *aix, *aux;


int usage(char *filename)
{
    printf ("Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("Usage: \n");
    printf ("<file1>\n");
    printf ("<file2>\n");
    printf ("where: \n\t<file1> \t\t\t files with trece to detect and clean SHIFT\n ");
    printf ("\n\t\t\t<file2>\t\t\t files with trece to clean DRIFT detected in <file1>\n ");
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



/* Functions used to get the file trace */
/*-----------------------------------------------------------------*/
int getFile( char file1[MAXSTRING] )
{

   long long int x_aux, y_aux;
   int z_aux;


  size_trace=0;
  /* Opening input file for reading */
  if( ( fp = fopen(file1,"r") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file1 );
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


/* Functions used to get the file trace */
/*-----------------------------------------------------------------*/
int getAux( char file2[MAXSTRING] )
{

   long long int x_aux, y_aux;
   int z_aux;


  size_aux=0;
  /* Opening input file for reading */
  if( ( fp = fopen(file2,"r") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file2 );
      return( -1 );
  }
  aux = NULL;
  while( ! feof( fp ) )
  {
      if( fgets( strfile, MAXLINE, fp ) != NULL )
      {
              if( (sscanf(strfile, "%d %lld %lld", &z_aux, &x_aux, &y_aux)) == 3 )
              {
		  size_aux++;
        	  if( ( aux = (t_aix *)realloc( aux, size_aux * sizeof(t_aix) ) ) == NULL )
        	  {
                      fprintf( stderr, "\n\n\tError: Memory allocation!" );
                      return( -1 );
        	  }
        	  aux[size_aux-1].x_aix = x_aux;
        	  aux[size_aux-1].y_aix = y_aux;
		  aux[size_aux-1].z_aix = z_aux;
              }
      }
  }
  fclose( fp );
  return(size_aux);
}

/* Functions used to correct drift in the file trace */
/*-----------------------------------------------------------------*/
void shiftDetectPos( int posic, double mean_alfa, int size_list_1, int size_list_2 )
{
   int dif_posic, cont_correct, tot_value;
   long int point_err, acuml_err;

   dif_posic = aix[posic].z_aix - aix[posic-1].z_aix;
   tot_value = mean_alfa * dif_posic;
   point_err = aix[posic-1].x_aix + tot_value;
   acuml_err = aix[posic].x_aix - point_err;

   for ( cont_correct = posic; cont_correct < size_list_1; cont_correct++ )
   {
      aix[cont_correct].x_aix = aix[cont_correct].x_aix - acuml_err;
      aix[cont_correct].y_aix = aix[cont_correct].y_aix + acuml_err;
   }

   cont_correct = 0;

   while((aux[cont_correct].x_aix + aux[cont_correct].y_aix ) < aix[posic-1].x_aix)
   {
      cont_correct++;
   };
      /*printf("**%d**\n", cont_correct);
      printf("**%lld**\n", aix[cont_correct].x_aix);
      printf("**%lld**\n", aix[cont_correct].y_aix);
      printf("**%lld**\n", aix[cont_correct].x_aix + aix[cont_correct].y_aix); */


   for ( posic = cont_correct; posic < size_list_2; posic++)
   {
      aux[posic].y_aix = aux[posic].y_aix - acuml_err;
   }


}


/* Functions used to correct drift in the file trace */
/*-----------------------------------------------------------------*/
void shiftDetectNeg( int posic, double mean_alfa, int size_list_1, int size_list_2 )
{
   int dif_posic, cont_correct, tot_value;
   long int point_err, acuml_err;

   dif_posic = aix[posic].z_aix - aix[posic-1].z_aix;
   tot_value = mean_alfa * dif_posic;
   point_err = aix[posic-1].x_aix + tot_value;
   acuml_err = aix[posic].x_aix - point_err;


   for ( cont_correct = posic; cont_correct < size_list_1; cont_correct++ )
   {
      aix[cont_correct].x_aix = aix[cont_correct].x_aix + acuml_err;
      aix[cont_correct].y_aix = aix[cont_correct].y_aix - acuml_err;
   }

   cont_correct = 0;

   while((aux[cont_correct].x_aix + aux[cont_correct].y_aix ) < aix[posic-1].x_aix)
   {
      cont_correct++;
   };

      //printf("**%d**\n", cont_correct);

   for ( posic = cont_correct; posic < size_list_2; posic++)
   {
      aux[posic].y_aix = aux[posic].y_aix + acuml_err;
   }

}

/* Functions used to correct freeze in the file trace */
/*-----------------------------------------------------------------*/
void freezeDetect( int posic, double mean_alfa, double mean_delay, int size_list_1, int size_list_2 )
{
   long int acuml_err;
   double alfa;
   int  cont_rem, cont_correct;

   alfa = (double)( aix[posic-1].x_aix - aix[posic].x_aix ) / (double) ( aix[posic-1].z_aix - aix[posic].z_aix );
   acuml_err = ((int)alfa / (int)mean_alfa)*2;

   cont_correct = 0;

   while((aux[cont_correct].x_aix + aux[cont_correct].y_aix ) < aix[posic-1].x_aix)
   {
      cont_correct++;
   }

   //printf("**%d**\n", cont_correct);

   for ( cont_rem = cont_correct-1; cont_rem < cont_correct+acuml_err; cont_rem++)
   {
      //aux[cont_rem].y_aix = aux[posic-10].y_aix;
      //aux[cont_rem].y_aix = mean_delay;
      aux[cont_rem].y_aix = 0;
      //printf("====\n");
   }

}

int main (int argc, char **argv)
{
  int size_list_1, size_list_2, cont_file;
  double mean_alfa, alfa, beta;
  long double mean_delay;

  mean_alfa  = 0.0;
  mean_delay = 0.0;
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


  size_list_1 = getFile(file1);
  size_list_2 = getAux(file2);

  strcpy (new_file, file1);
  strncat (new_file, ".shift", 6);
  /* Trying to create out file*/
  if( !(fp_result_1 = fopen(new_file,"w+") ) )
  {
      printf("\n\tError: Can't create out file!\n");
      return( -1 );
  }

  strcpy (new_file, file2);
  strncat (new_file, ".shift", 6);
  /* Trying to create out file*/
  if( !(fp_result_2 = fopen(new_file,"w+") ) )
  {
      printf("\n\tError: Can't create out file!\n");
      return( -1 );
  }

  for ( cont_file = 1; cont_file < size_list_1; cont_file++ )
  {

      alfa = (double)( aix[cont_file-1].x_aix - aix[cont_file].x_aix ) / (double) ( aix[cont_file-1].z_aix - aix[cont_file].z_aix );
      beta = mean_delay - aix[cont_file].y_aix ;
      //beta = (double)( mean_delay - aix[cont_file].y_aix ) / (double) ( aix[cont_file-1].x_aix - aix[cont_file].x_aix );
      //beta = (double)( aix[cont_file-1].y_aix - aix[cont_file].y_aix ) / (double) ( aix[cont_file-1].x_aix - aix[cont_file].x_aix );


      //printf("%d\t%lf\n", aix[cont_file].z_aix, alfa);

     if ( cont_file > 1 )
     {
	//if (( alfa > (mean_alfa + (double)(mean_alfa * 0.5))))
	if ( alfa > (2*mean_alfa))
	{
	//printf("1ALFA-%d\t%lf\n", aix[cont_file].z_aix, alfa);
	   if (aix[cont_file].y_aix < mean_delay-(alfa*0.5))
	   {
              shiftDetectPos(cont_file, mean_alfa, size_list_1, size_list_2);
	//printf("1BETA-%lf\n", beta);
	   } else
	   {
              freezeDetect(cont_file, mean_alfa, mean_delay, size_list_1, size_list_2);
	   }
	} else
        //if ((alfa < (mean_alfa - (double)(mean_alfa * 0.5))))
	if ( alfa > (2*mean_alfa))
	{
	//printf("2ALFA-%d\t%lf\n", aix[cont_file].z_aix, alfa);
	   if (aix[cont_file].y_aix > mean_delay-(alfa*0.5))
	   {
              shiftDetectNeg(cont_file, mean_alfa, size_list_1, size_list_2);
	//printf("2BETA-%lf\n", beta);
	   } else
	   {
              freezeDetect(cont_file, mean_alfa, mean_delay, size_list_1, size_list_2);
	   }
        }
	else
	{
	   mean_alfa = (long double)((long double)(mean_alfa * (cont_file-1) ) +  alfa) / (long double) cont_file;
	   mean_delay = (long double)(aix[cont_file].y_aix + (mean_delay*(cont_file-1))) / (long double) cont_file;
	}

     } else
     {
        mean_alfa = alfa;
	mean_delay = aix[cont_file].y_aix;
     }
  }

  for ( cont_file = 0; cont_file < size_list_1; cont_file++ )
  {
     if ( aix[cont_file].y_aix != 0 )
     {
        fprintf( fp_result_1, "%d\t%lld\t%lld\n", aix[cont_file].z_aix, aix[cont_file].x_aix, aix[cont_file].y_aix );
     }
  }
  for ( cont_file = 0; cont_file < size_list_2; cont_file++ )
  {
     if ( aux[cont_file].y_aix != 0 )
     {
        fprintf( fp_result_2, "%d\t%lld\t%lld\n", aux[cont_file].z_aix, aux[cont_file].x_aix, aux[cont_file].y_aix );
     }
  }

  fclose( fp_result_1 );
  fclose( fp_result_2 );
  free( aix );
  free( aux );
  return( 0 );
}
