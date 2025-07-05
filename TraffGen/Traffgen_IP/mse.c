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
        Last Update: 20/06/2003

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
//#include <unistd.h>
#include "mse.h"

/* global variables */



/* file name */
char   file[MAXSTRING], trace_file[MAXSTRING], out_file[MAXSTRING];

/* Distributions to Plot */
char   plots[MAXSTRING];


/* files with to analize */
FILE *fp, *fa, *out_f;

/* To keep the data of files */
t_aix *aix;

/* To keep the data of files */
t_aix *dist;

/*  minimun delay of trace*/
double min_delay, max_delay;


/* size of trace*/
long int size_trace=0;



int usage(char *filename)
{
    printf ("Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("Usage: \n");
    printf ("-<measure options> <file> \n");
    printf ("where: \n\t -<measure options>\t any combination of 'e','n','l','g','p' and 'w', or 'a' where:");
    printf( "\n\t\ta	: All");
    printf( "\n\t\te	: Exponencial");
    printf( "\n\t\tn	: Normal(Gaussian)");
    printf( "\n\t\tl	: Log Normal");
    printf( "\n\t\tg	: Gamma");
    printf( "\n\t\tp	: Pareto");
    printf( "\n\t\tw	: Weibull");
    printf ("\t<file> \t\t\t file with trece\n");

    return (1);
}


int parse_args(char **argv, int num_arg)
{
  int i;

  i = 1;

  strcpy (plots, argv[i]);
  i++;


  strcpy (file, argv[i]);

  return (1);
}

/* Functions used to get value of Trace */
/*-----------------------------------------------------------------*/
int getTrace( char v_file[MAXSTRING] )
{
     long long int x_aux;
     long double y_aux;

     size_trace = 0;

     /* Opening input file for reading */
     if( ( fp = fopen( v_file, "r" ) ) == NULL )
     {
         fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", v_file );
         return( -1 );
     }

     aix = NULL;

     while( ! feof( fp ) )
     {
         if( fgets( v_file, MAXLINE, fp ) != NULL )
         {
             if( (sscanf(v_file, "%lld %Lf", &x_aux, &y_aux)) == 2 )
             {
	             size_trace++;
                 if( ( aix = (t_aix *)realloc( aix, size_trace * sizeof(t_aix) ) ) == NULL )
                 {
                     fprintf( stderr, "\n\n\tError: Memory allocation!" );
                     return( -1 );
                 }
                 aix[size_trace-1].x_aix = x_aux;
                 aix[size_trace-1].y_aix = y_aux;
             }
         }
     }
    fclose( fp );

    return( 0 );
}

/* Functions used to get value of Distribution */
/*-----------------------------------------------------------------*/
int getDist( char v_file[MAXSTRING] )
{
     long long int x_aux;
     long double y_aux;
     long int size_dist;
     int clean_trace=0;

     size_dist = 0;

     /* Opening input file for reading */
     if( ( fp = fopen( v_file, "r" ) ) == NULL )
     {
         fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", v_file );
         return( -1 );
     }

     dist = NULL;

     while( clean_trace < 4 )
     {
         if( fgets( v_file, MAXLINE, fp ) != NULL )
         {
             if( v_file[0] == '#' )
             {
	             clean_trace++;
             }
         }
     }

     while( ! feof( fp ) )
     {
         if( fgets( v_file, MAXLINE, fp ) != NULL )
         {
	         if( ( sscanf( v_file, "%lld %Lf", &x_aux, &y_aux ) ) == 2 )
	         {
	             size_dist++;
                 if( ( dist = (t_aix *)realloc( dist, size_dist * sizeof(t_aix) ) ) == NULL )
                 {
        	         fprintf( stderr, "\n\n\tError: Memory allocation!" );
        	         return( -1 );
                 }
                 dist[size_dist-1].x_aix = x_aux;
                 dist[size_dist-1].y_aix = y_aux;
	         }
         }
     }
    fclose( fp );

    return( 0 );
}



/* Functions used to generate de trace of a exponential distributions */
/*-----------------------------------------------------------------*/
double generateExponential( char v_file[MAXSTRING] )
{
   char dist_file[MAXSTRING], tmp_file[MAXSTRING], result_file[MAXSTRING];
   char tmp_line[MAXSTRING];
   long int count_mse;
   double total_mse, temp_dist, temp_aix;

   strcpy (dist_file, v_file);
   strncat ( dist_file, ".EXP.dist", 9 );
   /* Reading DISTRIBUTION file */
   if( ( fa = fopen(dist_file,"r") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", dist_file );
       return( -1 );
   }
   if( fgets( tmp_line, MAXLINE, fa ) != NULL );
   fclose(fa);

   strcpy (result_file, v_file);
   strncat ( result_file, ".EXP.mse.dist", 13 );

   /* Creating temporary file for plotting */
   strcpy (tmp_file, "temp_exp.MSE.txt");
   if( ( fp = fopen(tmp_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", tmp_file );
       return( -1 );
   }

   fprintf( fp, "set terminal table\n" );
   fprintf( fp, "set output '%s'\n", result_file );
   fprintf( fp, "set sample %ld\n", size_trace );
   fprintf( fp, "plot '%s', %s \n", trace_file, tmp_line);

   fclose(fp);

   system("gnuplot 'temp_exp.MSE.txt' > tmp.mse");
   getDist(result_file);

   total_mse = 0;
   count_mse=0;

   /* Creating Partial Result file */
   strcpy (result_file, v_file);
   strncat ( result_file, ".EXP.mse", 8 );
   if( ( fp = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   while( count_mse<=size_trace-1)
   {
      temp_dist = dist[count_mse].y_aix;
      temp_aix = aix[count_mse].y_aix;
      total_mse = total_mse + pow(temp_aix - temp_dist, 2);
      fprintf(fp, "%lld %lf\n", aix[count_mse].x_aix, pow(temp_aix - temp_dist, 2));
      count_mse++;
   }

   fclose(fp);

   total_mse = (double)total_mse / (double)size_trace;
   return(total_mse);
}

/* Functions used to generate de trace of a GAUSSIAN(NORMAL) distributions */
/*-----------------------------------------------------------------*/
double generateGaussian( char v_file[MAXSTRING] )
{
   char dist_file[MAXSTRING], tmp_file[MAXSTRING], result_file[MAXSTRING];
   char tmp_line[MAXSTRING];
   long int count_mse;
   double total_mse, temp_dist, temp_aix;

   strcpy (dist_file, v_file);
   strncat ( dist_file, ".GAUSS.dist", 11 );
   /* Reading DISTRIBUTION file */
   if( ( fa = fopen(dist_file,"r") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", dist_file );
       return( -1 );
   }
   if( fgets( tmp_line, MAXLINE, fa ) != NULL );
   fclose(fa);

   strcpy (result_file, v_file);
   strncat ( result_file, ".GAUSS.mse.dist", 15 );

   /* Creating temporary file for plotting */
   strcpy (tmp_file, "temp_gauss.MSE.txt");
   if( ( fp = fopen(tmp_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", tmp_file );
       return( -1 );
   }

   fprintf( fp, "set terminal table\n" );
   fprintf( fp, "set output '%s'\n", result_file );
   fprintf( fp, "set sample %ld\n", size_trace );
   fprintf( fp, "plot '%s', %s \n", trace_file, tmp_line);

   fclose(fp);

   system("gnuplot 'temp_gauss.MSE.txt' > tmp.mse");
   getDist(result_file);

   total_mse = 0;
   count_mse=0;

   /* Creating Partial Result file */
   strcpy (result_file, v_file);
   strncat ( result_file, ".GAUSS.mse", 10 );
   if( ( fp = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   while( count_mse<=size_trace-1)
   {
      temp_dist = dist[count_mse].y_aix;
      temp_aix = aix[count_mse].y_aix;
      total_mse = total_mse + pow(temp_aix - temp_dist, 2);
      fprintf(fp, "%lld %lf\n", aix[count_mse].x_aix, pow(temp_aix - temp_dist, 2));
      count_mse++;
   }

   fclose(fp);

   total_mse = (double)total_mse / (double)size_trace;
   return(total_mse);
}

/* Functions used to generate de trace of a LOG-NORMAL distributions */
/*-----------------------------------------------------------------*/
double generateLognormal( char v_file[MAXSTRING] )
{
   char dist_file[MAXSTRING], tmp_file[MAXSTRING], result_file[MAXSTRING];
   char tmp_line[MAXSTRING];
   long int count_mse;
   double total_mse, temp_dist, temp_aix;

   strcpy (dist_file, v_file);
   strncat ( dist_file, ".LNORM.dist", 11 );
   /* Reading DISTRIBUTION file */
   if( ( fa = fopen(dist_file,"r") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", dist_file );
       return( -1 );
   }
   if( fgets( tmp_line, MAXLINE, fa ) != NULL );
   fclose(fa);

   strcpy (result_file, v_file);
   strncat ( result_file, ".LNORM.mse.dist", 15 );

   /* Creating temporary file for plotting */
   strcpy (tmp_file, "temp_lnorm.MSE.txt");
   if( ( fp = fopen(tmp_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", tmp_file );
       return( -1 );
   }

   fprintf( fp, "set terminal table\n" );
   fprintf( fp, "set output '%s'\n", result_file );
   fprintf( fp, "set sample %ld\n", size_trace );
   fprintf( fp, "plot '%s', %s \n", trace_file, tmp_line);

   fclose(fp);

   system("gnuplot 'temp_lnorm.MSE.txt' > tmp.mse");
   getDist(result_file);

   total_mse = 0;
   count_mse=0;

   /* Creating Partial Result file */
   strcpy (result_file, v_file);
   strncat ( result_file, ".LNORM.mse", 10 );
   if( ( fp = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   while( count_mse<=size_trace-1)
   {
      temp_dist = dist[count_mse].y_aix;
      temp_aix = aix[count_mse].y_aix;
      total_mse = total_mse + pow(temp_aix - temp_dist, 2);
      fprintf(fp, "%lld %lf\n", aix[count_mse].x_aix, pow(temp_aix - temp_dist, 2));
      count_mse++;
   }

   fclose(fp);

   total_mse = (double)total_mse / (double)size_trace;
   return(total_mse);
}


/*-----------------------------------------------------------------*/
double generatePareto( char v_file[MAXSTRING] )
{
   char dist_file[MAXSTRING], tmp_file[MAXSTRING], result_file[MAXSTRING];
   char tmp_line[MAXSTRING];
   long int count_mse;
   double total_mse, temp_dist, temp_aix;

   strcpy (dist_file, v_file);
   strncat ( dist_file, ".PARETO.dist", 12 );
   /* Reading DISTRIBUTION file */
   if( ( fa = fopen(dist_file,"r") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", dist_file );
       return( -1 );
   }
   if( fgets( tmp_line, MAXLINE, fa ) != NULL );
   fclose(fa);

   strcpy (result_file, v_file);
   strncat ( result_file, ".PARETO.mse.dist", 16 );

   /* Creating temporary file for plotting */
   strcpy (tmp_file, "temp_pareto.MSE.txt");
   if( ( fp = fopen(tmp_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", tmp_file );
       return( -1 );
   }

   fprintf( fp, "set terminal table\n" );
   fprintf( fp, "set output '%s'\n", result_file );
   fprintf( fp, "set sample %ld\n", size_trace );
   fprintf( fp, "plot '%s', %s \n", trace_file, tmp_line);

   fclose(fp);

   system("gnuplot 'temp_pareto.MSE.txt' > tmp.mse");
   getDist(result_file);

   total_mse = 0;
   count_mse=0;

   /* Creating Partial Result file */
   strcpy (result_file, v_file);
   strncat ( result_file, ".PARETO.mse", 11 );
   if( ( fp = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   while( count_mse<=size_trace-1)
   {
      temp_dist = dist[count_mse].y_aix;
      temp_aix = aix[count_mse].y_aix;
      if (temp_dist < 0)
      {
         temp_dist = 0;
      }
      total_mse = total_mse + pow(temp_aix - temp_dist, 2);
      fprintf(fp, "%lld %lf\n", aix[count_mse].x_aix, pow(temp_aix - temp_dist, 2));
      count_mse++;
   }

   fclose(fp);

   total_mse = (double)total_mse / (double)size_trace;
   return(total_mse);
}

/* Functions used to generate de trace of a GAMMA distributions */
/*-----------------------------------------------------------------*/
double generateGamma( char v_file[MAXSTRING] )
{
   char dist_file[MAXSTRING], tmp_file[MAXSTRING], result_file[MAXSTRING];
   char tmp_line[MAXSTRING];
   long int count_mse;
   double total_mse, temp_dist, temp_aix;

   strcpy (dist_file, v_file);
   strncat ( dist_file, ".GAMMA.dist", 11 );
   /* Reading DISTRIBUTION file */
   if( ( fa = fopen(dist_file,"r") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", dist_file );
       return( -1 );
   }
   if( fgets( tmp_line, MAXLINE, fa ) != NULL );
   fclose(fa);

   strcpy (result_file, v_file);
   strncat ( result_file, ".GAMMA.mse.dist", 15 );

   /* Creating temporary file for plotting */
   strcpy (tmp_file, "temp_gamma.MSE.txt");
   if( ( fp = fopen(tmp_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", tmp_file );
       return( -1 );
   }

   fprintf( fp, "set terminal table\n" );
   fprintf( fp, "set output '%s'\n", result_file );
   fprintf( fp, "set sample %ld\n", size_trace );
   fprintf( fp, "plot '%s', %s \n", trace_file, tmp_line);

   fclose(fp);

   system("gnuplot 'temp_gamma.MSE.txt' > tmp.mse");
   getDist(result_file);

   total_mse = 0;
   count_mse=0;

   /* Creating Partial Result file */
   strcpy (result_file, v_file);
   strncat ( result_file, ".GAMMA.mse", 10 );
   if( ( fp = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   while( count_mse<=size_trace-1)
   {
      temp_dist = dist[count_mse].y_aix;
      temp_aix = aix[count_mse].y_aix;
      total_mse = total_mse + pow(temp_aix - temp_dist, 2);
      fprintf(fp, "%lld %lf\n", aix[count_mse].x_aix, pow(temp_aix - temp_dist, 2));
      count_mse++;
   }

   fclose(fp);

   total_mse = (double)total_mse / (double)size_trace;
   return(total_mse);
}



/*-----------------------------------------------------------------*/
double generateWeibull( char v_file[MAXSTRING] )
{
   char dist_file[MAXSTRING], tmp_file[MAXSTRING], result_file[MAXSTRING];
   char tmp_line[MAXSTRING];
   long int count_mse;
   double total_mse, temp_dist, temp_aix;

   strcpy (dist_file, v_file);
   strncat ( dist_file, ".WEIBULL.dist", 13 );
   /* Reading DISTRIBUTION file */
   if( ( fa = fopen(dist_file,"r") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", dist_file );
       return( -1 );
   }
   if( fgets( tmp_line, MAXLINE, fa ) != NULL );
   fclose(fa);

   strcpy (result_file, v_file);
   strncat ( result_file, ".WEIBULL.mse.dist", 17 );

   /* Creating temporary file for plotting */
   strcpy (tmp_file, "temp_weibull.MSE.txt");
   if( ( fp = fopen(tmp_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", tmp_file );
       return( -1 );
   }

   fprintf( fp, "set terminal table\n" );
   fprintf( fp, "set output '%s'\n", result_file );
   fprintf( fp, "set sample %ld\n", size_trace );
   fprintf( fp, "plot '%s', %s \n", trace_file, tmp_line);

   fclose(fp);

   system("gnuplot 'temp_weibull.MSE.txt' > tmp.mse");
   getDist(result_file);

   total_mse = 0;
   count_mse=0;

   /* Creating Partial Result file */
   strcpy (result_file, v_file);
   strncat ( result_file, ".WEIBULL.mse", 12 );
   if( ( fp = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   while( count_mse<=size_trace-1)
   {
      temp_dist = dist[count_mse].y_aix;
      temp_aix = aix[count_mse].y_aix;
      total_mse = total_mse + pow(temp_aix - temp_dist, 2);
      fprintf(fp, "%lld %lf\n", aix[count_mse].x_aix, pow(temp_aix - temp_dist, 2));
      count_mse++;
   }

   fclose(fp);

   total_mse = (double)total_mse / (double)size_trace;
   return(total_mse);
}



/* Functions used to get the distributions traces */
/*-----------------------------------------------------------------*/
void getDistribution( char v_file[MAXSTRING] )
{

   double freturn;
   int string_pointer;

   string_pointer = 1;

    /* Parameters measure options */
    if((plots[0] == '-') && (plots[1] != '\n'))
    {
        while( string_pointer < strlen(plots) )
        {
            if((plots[string_pointer] == 'a' )||(plots[string_pointer] == 'A' ))
	    {
	       freturn = generateExponential(v_file);
               fprintf(out_f, "EXP: %.10lf\n", freturn);
               freturn = generateGaussian(v_file);
               fprintf(out_f, "GAUSS: %.10lf\n", freturn);
	       freturn = generateLognormal(v_file);
               fprintf(out_f, "LNORM: %.10lf\n", freturn);
	       freturn = generateGamma(v_file);
               fprintf(out_f, "GAMMA: %.10lf\n", freturn);
	       freturn = generatePareto(v_file);
               fprintf(out_f, "PARETO: %.10lf\n", freturn);
	       freturn = generateWeibull(v_file);
               fprintf(out_f, "WEIBULL: %.10lf\n", freturn);
	       string_pointer = strlen(plots);
	    }
	    else
            if((plots[string_pointer] == 'e' )||(plots[string_pointer] == 'E' ))
	    {
	       freturn = generateExponential(v_file);
               fprintf(out_f, "EXP: %lf\n", freturn);
	    }
	    else
	    if((plots[string_pointer] == 'n' )||(plots[string_pointer] == 'N' ))
	    {
	        freturn = generateGaussian(v_file);
                fprintf(out_f, "GAUSS: %lf\n", freturn);
            }
	    else
            if((plots[string_pointer] == 'l' )||(plots[string_pointer] == 'L' ))
	    {
	        freturn = generateLognormal(v_file);
                fprintf(out_f, "LNORM: %lf\n", freturn);
            }
	    else
            if((plots[string_pointer] == 'g' )||(plots[string_pointer] == 'G' ))
	    {
	        freturn = generateGamma(v_file);
                fprintf(out_f, "GAMMA: %lf\n", freturn);
            }
	    else
            if((plots[string_pointer] == 'p' )||(plots[string_pointer] == 'P' ))
	    {
	        freturn = generatePareto(v_file);
                fprintf(out_f, "PARETO: %lf\n", freturn);
            }
	    else
            if((plots[string_pointer] == 'w' )||(plots[string_pointer] == 'W' ))
	    {
	    	freturn = generateWeibull(v_file);
                fprintf(out_f, "WEIBULL: %lf\n", freturn);
            }
	    string_pointer++;
        }
    }
    else
    {
        fprintf( stderr, "\n\tError: <measure options>  must begin with '-' signal plus [tenlgpw]\n\n" );
        usage( plots );
    }
}


int main (int argc, char **argv)
{
  char   aux_file[MAXSTRING];

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


  strcpy (trace_file, file);
  strncat ( trace_file, ".dist", 5 );
  strcpy (aux_file, trace_file);


  getTrace(aux_file);

   /* Creating Result file */
   strcpy (out_file, file);
   strncat ( out_file, ".mse.out", 8 );
   if( ( out_f = fopen(out_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", out_file );
       return( -1 );
   }

  getDistribution(file);

  fclose(out_f);

  free( aix );
  return(0);

}
