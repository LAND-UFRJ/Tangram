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

#include <math.h>
#include <unistd.h>
#include "delay_histogram.h"

/* global variables */



/* file name */
char   file[MAXSTRING], tmp_file[MAXSTRING];

/* Distributions to Plot */
char   plots[MAXSTRING];

/* Column of File to Plot*/
int column;
char   column_c[MAXSTRING];

/* files with to analize */
FILE *fp, *fc, *fa;

/* To keep the data of files */
t_aix *aix;

/* To keep the data of files */
t_aix *hist;

/*time slot among different stages*/
double slot;
int cont_hist;


/*  minimun delay of trace*/
double min_delay, max_delay;

/*  mean , variance and coeficient of variation of delay of trace*/
double mean_delay, var_delay, coef_var;
long double sum_delay=0;

/* size of trace*/
long int size_trace=0;



int usage(char *filename)
{
    printf ("Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("Usage: \n");
    printf ("-<measure options> -<column option> <file> <slot> \n");
    printf ("where: \n\t -<measure options>\t any combination of 't','e','n','l','g','p' and 'w' where:");
    printf( "\n\t\tt	: Trace");
    printf( "\n\t\te	: Exponencial");
    printf( "\n\t\tn	: Normal(Gaussian)");
    printf( "\n\t\tl	: Log Normal");
    printf( "\n\t\tg	: Gamma");
    printf( "\n\t\tp	: Pareto");
    printf( "\n\t\tw	: Weibull");
    printf ("\n\t-<column option> \t column (1 or 2) to generate the histogram\n");
    printf ("\t<file> \t\t\t file with trece\n");
    printf ("\t<slot> \t\t\t time (in microseconds) of slot among different stages\n ");

    return (1);
}


int parse_args(char **argv, int num_arg)
{
  int i;

  i = 1;

  strcpy (plots, argv[i]);
  i++;

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


  strcpy (file, argv[i]);
  i++;

  /* Verifying float number (argv[3]) */
  if( ( slot = strtod( argv[i], NULL ) ) < 1 )
  {
      fprintf( stderr, "\n\tError: Time interval should be a positive real\n\n" );
      return(-1);
  }


  return (1);
}

/* Functions used to get value of files */
/*-----------------------------------------------------------------*/
int histDist( char aux_file[MAXSTRING] )
{
   int i, pos_hist;
   double pdist=0, pacum=0;
   char comp_file[MAXSTRING], cuml_file[MAXSTRING];
   double rest, junk;

/*Código comentado devido a erro no cálculo da banda,
alterações foram feitas, e é necessario fazer novos testes
para calculo do OWD*/

   hist = NULL;

   if( ( hist = (t_aix *)realloc( hist, cont_hist * sizeof(t_aix) ) ) == NULL )
//   if( ( hist = (t_aix *)realloc( hist, 10000000 * sizeof(t_aix) ) ) == NULL )
   {
       fprintf( stderr, "\n\n\tError: Memory allocation!" );
       return( -1 );
   }

   rest = (int)min_delay % (int)slot;
   junk = min_delay / slot;

   i = 0;


   while ( max_delay >= (min_delay + (float)(i * slot)))
   {
      //fprintf( stderr , "debub %d\n",i );
      hist[i].x_aix = min_delay + (float)(i * slot);
      hist[i].y_aix = 0;
      i++;
   }


   for ( i=0; i<size_trace; i++)
   {
      pos_hist = ((aix[i].y_aix - min_delay) / slot);
      //pos_hist = ((aix[i].y_aix - min_delay) / slot) + junk;
      //printf("%d\n", pos_hist);
      hist[pos_hist].y_aix++;
   }


   /*for ( i=0; i<10000; i++)
   {
      hist[i].x_aix = rest + (float)(i * slot);
      hist[i].y_aix = 0;
   }*/

   /*for ( i=size_trace; i<10000; i++)
   {
      hist[i].x_aix = rest + (float)(i * slot);
      hist[i].y_aix = 0;
   }*/

   pos_hist=cont_hist;

   strcpy( comp_file, aux_file);
   strcpy( cuml_file, aux_file);
   strncat(aux_file, ".pmf", 4);

   if( ( fp = fopen(aux_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file );
       return( -1 );
   }
   strncat(comp_file, ".comp", 5);

   if( ( fc = fopen(comp_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file );
       return( -1 );
   }
   strncat(cuml_file, ".dist", 5);

   if( ( fa = fopen(cuml_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file );
       return( -1 );
   }

   fprintf( fc, "%lld\t1\n", hist[0].x_aix-1);

   for ( i=0; i<pos_hist; i++)
//   for ( i=0; i<10000; i++)
   {
      pdist = ((double)hist[i].y_aix / (double)size_trace);
      pacum = pacum + pdist;
      fprintf( fp, "%.0lld\t%lf\n", hist[i].x_aix , pdist);
      fprintf( fc, "%.0lf\t%lf\n", hist[i].x_aix + slot-1, 1- pacum);
      fprintf( fa, "%.0lld\t%lf\n", hist[i].x_aix , pacum);
   }

   fclose(fp);
   fclose(fa);
   fclose(fc);


   return(0);



}

/* Functions used to get parameters mean and variance of trace */
/*-----------------------------------------------------------------*/
void getParameters( void )
{

   double aux_var=0;

   int cont_size = 0;

   mean_delay = (double)((double)sum_delay / (double)size_trace);

  for ( cont_size = 0; cont_size < size_trace; cont_size++ )
  {
     aux_var = aux_var + pow(( aix[cont_size].y_aix - mean_delay ), 2);
  }

  var_delay = ((float)1 / (float)(size_trace - 1)) * ( aux_var );
  coef_var = (double)((double)sqrt (var_delay) / (double)mean_delay);

  fprintf( stdout, "mean: %lf\n", mean_delay);
  fprintf( stdout, "var: %lf\n", var_delay);
  fprintf( stdout, "coef. var: %lf\n", coef_var);

}

/* Functions used to generate de trace of a exponential distributions */
/*-----------------------------------------------------------------*/
int generateExponential( char dist_file[MAXSTRING] )
{

   char result_file[MAXSTRING];
   /*long int i;*/
   double lambda;
   /*double lambda_x;*/

   strcpy (result_file, dist_file);
   strncat ( result_file, ".EXP.pdf", 8 );

   /* Creating output PDF file */
   if( ( fp = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   strcpy (result_file, dist_file);
   strncat ( result_file, ".EXP.dist", 9 );

   /* Creating output DISTRIBUTION file */
   if( ( fa = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   strcpy (result_file, dist_file);
   strncat ( result_file, ".EXP.comp", 9 );

   /* Creating output COMPLEMENTARY file */
   if( ( fc = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }


   lambda = (double)1 / (double) mean_delay;
   fprintf( fp, "%.10lf * exp(-%.10lf * x)\n", lambda , lambda);
   fprintf( fp, "LAMBDA= %.10lf\n", lambda);
   fprintf( fa, "1 - exp(-%.10lf * x)\n", lambda);
   fprintf( fc, "exp(-%.10lf * x)\n", lambda);

   fclose(fp);
   fclose(fa);
   fclose(fc);

   return(0);

}

/* Functions used to generate de trace of a GAUSSIAN(NORMAL) distributions */
/*-----------------------------------------------------------------*/
int generateGaussian( char dist_file[MAXSTRING] )
{

   char result_file[MAXSTRING];
   /*long int i;*/
   double mi, sigma, pi;

   strcpy (result_file, dist_file);
   strncat ( result_file, ".GAUSS.pdf", 10 );

   /* Creating output PDF file */
   if( ( fp = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   strcpy (result_file, dist_file);
   strncat ( result_file, ".GAUSS.dist", 11 );

   /* Creating output DISTRIBUTION file */
   if( ( fa = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   strcpy (result_file, dist_file);
   strncat ( result_file, ".GAUSS.comp", 11 );

   /* Creating output COMPLEMENTARY file */
   if( ( fc = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   pi = M_PI;

   mi = mean_delay;
   sigma = sqrt ( var_delay );

   fprintf( fp, "( 1 / (%.10lf * sqrt( 2 * %.10lf ))) * exp(-( ((x - %.10lf)**2)/( 2 * (%.10lf**2)))) \n", sigma, pi , mi, sigma);
   fprintf( fp, "SIGMA= %.10lf\n", sigma);
   fprintf( fp, "MI= %.10lf\n", mi);
   fprintf( fa, "norm((x - %.10lf )/ %.10lf )\n", mi, sigma );
   fprintf( fc, "1 - norm((x - %.10lf )/ %.10lf )\n", mi, sigma );



   fclose(fp);
   fclose(fa);
   fclose(fc);

   return(0);


}

/* Functions used to generate de trace of a LOG-NORMAL distributions */
/*-----------------------------------------------------------------*/
int generateLognormal( char dist_file[MAXSTRING] )
{

   char result_file[MAXSTRING];
   double mi, sigma, pi;

   strcpy (result_file, dist_file);
   strncat ( result_file, ".LNORM.pdf", 10 );

   /* Creating output PDF file */
   if( ( fp = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   strcpy (result_file, dist_file);
   strncat ( result_file, ".LNORM.dist", 11 );

   /* Creating output DISTRIBUTION file */
   if( ( fa = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   strcpy (result_file, dist_file);
   strncat ( result_file, ".LNORM.comp", 11 );

   /* Creating output COMPLEMENTARY file */
   if( ( fc = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   mi = log((double)mean_delay/sqrt((double)pow(coef_var, 2)+ 1));
   sigma = sqrt((double) log((double) pow(coef_var, 2) + 1));

   pi = M_PI;


   fprintf( fp, "(exp(-0.5*(((log(x) - %.10lf)/%.10lf)**2))/(x*%.10lf*sqrt(2*%.10lf)))\n", mi, sigma, sigma, pi);
   fprintf( fp, "SIGMA= %.10lf\n", sigma);
   fprintf( fp, "MI= %.10lf\n", mi);
   fprintf( fa, "norm((log(x) - %.10lf )/ %.10lf )\n", mi, sigma );
   fprintf( fc, "1 - norm((log(x) - %.10lf )/ %.10lf )\n", mi, sigma );

   fclose(fp);
   fclose(fa);
   fclose(fc);

   return(0);
}


/*-----------------------------------------------------------------*/
int generatePareto( char dist_file[MAXSTRING] )
{

   char result_file[MAXSTRING];
   /*long int i;*/
   double alfa, ka;

   strcpy (result_file, dist_file);
   strncat ( result_file, ".PARETO.pdf", 11 );

   /* Creating output PDF file */
   if( ( fp = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   strcpy (result_file, dist_file);
   strncat ( result_file, ".PARETO.dist", 12 );

   /* Creating output DISTRIBUTION file */
   if( ( fa = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   strcpy (result_file, dist_file);
   strncat ( result_file, ".PARETO.comp", 12 );

   /* Creating output COMPLEMENTARY file */
   if( ( fc = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }


   ka = min_delay;
   alfa = ((double) mean_delay / (double)( mean_delay - ka ));

   fprintf( fp, "%.10lf * %.10lf**%.10lf * x**-(%.10lf-1) \n", alfa, ka, alfa, alfa );
   fprintf( fp, "ALFA= %.10lf\n", alfa);
   fprintf( fp, "KA= %.10lf\n", ka);
   fprintf( fa, "1 - ( %.10lf / x )**%.10lf\n", ka, alfa );
   fprintf( fc, "( %.10lf / x )**%.10lf\n", ka, alfa );

   fclose(fp);
   fclose(fa);
   fclose(fc);

   return(0);
}

/* Functions used to generate de trace of a GAMMA distributions */
/*-----------------------------------------------------------------*/
int generateGamma( char dist_file[MAXSTRING] )
{

   char result_file[MAXSTRING];
   double lambda, alfa;

   strcpy (result_file, dist_file);
   strncat ( result_file, ".GAMMA.pdf", 10 );

   /* Creating output PDF file */
   if( ( fp = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   strcpy (result_file, dist_file);
   strncat ( result_file, ".GAMMA.dist", 11 );

   /* Creating output DISTRIBUTION file */
   if( ( fa = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   strcpy (result_file, dist_file);
   strncat ( result_file, ".GAMMA.comp", 11 );

   /* Creating output COMPLEMENTARY file */
   if( ( fc = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }


   lambda = ((double)mean_delay / (double) var_delay );
   alfa = ((double)(pow(mean_delay, 2)) / (double) var_delay );

   fprintf( fp, "(%.10lf**%.10lf) * (x**(%.10lf - 1)) * (exp**(-%.10lf*x)) \n", lambda , alfa, alfa, lambda );
   fprintf( fp, "ALFA= %.10lf\n", alfa);
   fprintf( fp, "LAMBDA= %.10lf\n", lambda);
   fprintf( fa, "(igamma(%.10lf, %.10lf * x) / gamma( %.10lf ))\n", alfa, lambda, alfa);
   fprintf( fc, "1-(igamma(%.10lf, %.10lf * x) / gamma( %.10lf ))\n", alfa, lambda, alfa);

   fclose(fp);
   fclose(fa);
   fclose(fc);

   return(0);

}



/*-----------------------------------------------------------------*/
int generateWeibull( char dist_file[MAXSTRING] )
{

   char result_file[MAXSTRING];
   /*long int i;*/
   double alfa, lambda, h_lambda;
   FILE *temp;
   double x_wei, y_wei;
   char z_wei;

   alfa     = 0.0;
   h_lambda = 0.0;
   strcpy (result_file, dist_file);
   strncat ( result_file, ".WEIBULL.pdf", 12 );

   /* Creating output PDF file */
   if( ( fp = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   strcpy (result_file, dist_file);
   strncat ( result_file, ".WEIBULL.dist", 13 );

   /* Creating output DISTRIBUTION file */
   if( ( fa = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }

   strcpy (result_file, dist_file);
   strncat ( result_file, ".WEIBULL.comp", 13 );

   /* Creating output COMPLEMENTARY file */
   if( ( fc = fopen(result_file,"w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", result_file );
       return( -1 );
   }


   /********** getting WEIBULL parameters *************/

   /* Creating temporary files to get WEIBULL parameters */
   if( ( temp = fopen("temp_weibull.txt","w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file temp_weibull.txt\n\n");
       return( -1 );
   }

   fprintf( temp, "set terminal table\n" );
   fprintf( temp, "set output 'temp_weibull_result.txt'\n" );
   fprintf( temp, "set sample 1000\n" );
   fprintf( temp, "plot [0:][%.4lf:%.4lf]", coef_var, coef_var + 0.001);
   fprintf( temp, "(sqrt(gamma(1+(2/x)))-(gamma(1+(1/x))*gamma(1+(1/x)))/gamma(1+(1/x)))");
   fprintf( temp, " with points\n");

   fclose(temp);

   system("gnuplot 'temp_weibull.txt'");

   strcpy( tmp_file, "temp_weibull_result.txt");

   /* Opening temporary files to get WEIBULL parameters */
   if( ( temp = fopen( tmp_file,"r") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", tmp_file );
       return( -1 );
   }

   if( fgets( tmp_file, MAXLINE, temp ) != NULL );
   if( fgets( tmp_file, MAXLINE, temp ) != NULL );

    while( ! feof( temp ) )
   {
       if( fgets( tmp_file, MAXLINE, temp ) != NULL )
       {
          if( (sscanf(tmp_file, "%lf %lf %c", &x_wei, &y_wei, &z_wei )) == 3 );
       }
       if(z_wei=='i')
       {
          alfa = x_wei;
       }
   }

   fclose(temp);

   /* Creating temporary files to get WEIBULL parameters */
   if( ( temp = fopen("temp_weibull2.txt","w+") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file temp_weibull.txt\n\n");
       return( -1 );
   }

   fprintf( temp, "set terminal table\n" );
   fprintf( temp, "set output 'temp_weibull_result.txt'\n" );
   fprintf( temp, "set sample 2\n" );
   fprintf( temp, "plot [0:] gamma(1+(1/%.20lf)) with points\n", alfa);

   fclose(temp);

   system("gnuplot 'temp_weibull.txt'");

   strcpy( tmp_file, "temp_weibull_result.txt");

   /* Opening temporary files to get WEIBULL parameters */
   if( ( temp = fopen( tmp_file,"r") ) == NULL )
   {
       fprintf( stderr, "\n\tError: Can't create file '%s'\n\n", tmp_file );
       return( -1 );
   }

   if( fgets( tmp_file, MAXLINE, temp ) != NULL );
   if( fgets( tmp_file, MAXLINE, temp ) != NULL );

    while( ! feof( temp ) )
   {
       if( fgets( tmp_file, MAXLINE, temp ) != NULL )
       {
          if( (sscanf(tmp_file, "%lf %lf %c", &x_wei, &y_wei, &z_wei )) == 3 );
       }
       if(z_wei == 'i')
       {
          h_lambda = (double)pow(((double)mean_delay / (double) x_wei), alfa);
       }
   }

   fclose(temp);



   lambda = (double)1 / (double) h_lambda;

   fprintf( fp, "%.20lf * %.20lf * x**(%.20lf -1) * exp(-%.20lf * x**(%.20lf))\n", lambda, alfa, alfa, lambda, alfa);
   fprintf( fp, "ALFA= %.20lf\n", alfa);
   fprintf( fp, "LAMBDA= %.20lf\n", lambda);
   fprintf( fa, "1 - exp(-%.20lf * x**(%.20lf))\n", lambda, alfa);
   fprintf( fc, "exp(-%.20lf * x**(%.20lf))\n", lambda, alfa);

   fclose(fp);
   fclose(fa);
   fclose(fc);

   return(0);
}



/* Functions used to get the distributions traces */
/*-----------------------------------------------------------------*/
void getDistribution( char dist_file[MAXSTRING] )
{

   int freturn, string_pointer;

   string_pointer = 1;

    /* Parameters measure options */
    if(( (plots[0] == '-') && (plots[1] != '\n') )||( (column_c[0] == '-') && (column_c[1] != '\n') ))
    {
        while( string_pointer < strlen(plots) )
        {
            if((plots[string_pointer] == 't' )||(plots[string_pointer] == 'T' ));
	    else
            if((plots[string_pointer] == 'e' )||(plots[string_pointer] == 'E' ))
	       freturn = generateExponential(dist_file);
	    else
	    if((plots[string_pointer] == 'n' )||(plots[string_pointer] == 'N' ))
	        freturn = generateGaussian(dist_file);
            else
            if((plots[string_pointer] == 'l' )||(plots[string_pointer] == 'L' ))
	        freturn = generateLognormal(dist_file);
            else
            if((plots[string_pointer] == 'g' )||(plots[string_pointer] == 'G' ))
	        freturn = generateGamma(dist_file);
            else
            if((plots[string_pointer] == 'p' )||(plots[string_pointer] == 'P' ))
	        freturn = generatePareto(dist_file);
            else
            if((plots[string_pointer] == 'w' )||(plots[string_pointer] == 'W' ))
	    	freturn = generateWeibull(dist_file);
            string_pointer++;
        }
    }
    else
    {
        fprintf( stderr, "\n\tError: <measure options> and <column option> must begin with '-' signal plus [tenlgpw]\n\n" );
        usage( plots );
    }
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
       //Second condition added by HugoSato. Its necessary for rtt delay evaluation
       if( fgets( file, MAXLINE, fp ) != NULL && file[0]!='#' )
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

		       sum_delay = sum_delay + y_aux;

		       if ( size_trace == 1 )
		       {
	              min_delay = y_aux;
		           max_delay = y_aux;
		       }
		       else
		       {
        	        if ( y_aux < min_delay )
                     min_delay = y_aux;
        	        if ( y_aux > max_delay )
                     max_delay = y_aux;
		       }
          }
          else
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
char dist_file[MAXSTRING], aux_file[MAXSTRING];


  /* check for the number of arguments */
  if ( argc < 5 ) {
    usage(argv[0]);
    return (-1);
  }

  /* parse the arguments */
  if (parse_args(argv, argc) < 0) {
    usage(argv[0]);
    return (-1);
  }

  strcpy(aux_file, file);
  strcpy(dist_file, file);

  getValue(file);

  if ( slot != 0 )
  {
     cont_hist = ( max_delay - min_delay ) / slot ;
     cont_hist++;
  }
  else
  {
     cont_hist = 0;
  }



  histDist(aux_file);
  getParameters();
  getDistribution(dist_file);

  free( aix );
  free( hist );
  return(0);

}
