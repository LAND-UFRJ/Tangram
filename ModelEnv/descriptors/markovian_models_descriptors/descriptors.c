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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _SOLARIS_
 #include <strings.h>
#endif

#include <math.h>
#include <errno.h>

#include "sparseMatrix.h"

#define  TRUE  1
#define  FALSE 0
#define  NADA -1


/* Extern functions */
double *get_intervals();                  /* in module parameters.c         */
double *get_initial_pi();                 /* in module parameters.c         */
double *GTH_no_blocks();                  /* in module gth_no_block.c       */
void   lo_bound2();                       /* in module bounds.c             */
int    partial_poisson_sum();             /* in module bounds.c             */
double *new_omega();                      /* in module operational_time.c   */
int    *state_remapping();                /* in module interface_omega.c    */
double *pi_cond_non_burst();              /* in module burst_initial_prob.c */
double *calc_init_burst();                /* in module burst_initial_prob.c */


Matrix *read_matrix();
double Mult_results();
double Expect();
double Variance();
double Peak();
void   Autocov();
void   IDC();
void   Cum_burst_time_dist();
double *Expect_n();
double *Expect_n2();
double D_function();
void   Transpose_matrix();
double Expect_2();
void   usage();
int    exp2_max_bound();
int    exp2_min_bound();
int    uniformization_method_2();
int    slotted_method();
double *get_rewards();
void   Duplicate_matrix();


int main( argc, argv )
int  argc;
char *argv[];
{
   int    selection;               /* descriptor selection */
   int    slotted = 0;             /* indicates if model is slotted */
   double lambda;                  /* uniformization rate */
   double precision = 0;           /* error bound */
   double *pi;                     /* ptr to steady state */
   double *pi1 = 0;                /* ptr to state initial probabilities */
   double *intervals = 0;          /* ptr to intervals vector */
   double *reward;                 /* ptr to reward vector */
   Matrix *P;                      /* ptr to probabilities matrix */
   double Expect_value;            /* expect value of Markov chain */
   double Moment_2;                /* Second moment of Markov chain */
   double Variance_value;          /* variance value of Markov chain */
   int    size;                    /* number of chain states */
   FILE   *f_in;                   /* ptr. to generator matrix file */
   FILE   *f_out;                  /* ptr. to out file */
   FILE   *f_intervals = 0;        /* ptr. to intervals description file */
   FILE   *f_init_prob = 0;        /* ptr. to initial prob. desc. file */
   FILE   *f_reward;               /* ptr. to reward description file */
   char   basename[1000];          /* basename for input files */
   char   reward_name[1000];       /* reward name including object (obj.reward) */
   char   filename[1000];          /* auxiliar name for files */
   char   outname[1000];           /* name of output file */
   char   matrix_name[1000];       /* name of generator matrix */
   int    num_intervals;           /* number of intervals */
   double peak_value;              /* inform peak value */
   int    state;                   /* state of peak value */
   double burstness;               /* peak value divided by expect value */
   char   descriptor_name[50];     /* name of the descriptor */
   int    num_burst_st;            /* number of burst states */
   double max_non_burst_level = 0; /* Maximum rate below burst */

   #ifdef DEBUG
   int    i;                   /* temporary count variable */
   #endif



   /* Read input parameters */

   if( (argc < 4) || (argc > 6) )
   {
      usage();
      exit( 20 );
   }

   strcpy( basename, argv[ 1 ] );
   selection = atoi( argv[ 2 ] );
   strcpy( reward_name, argv[ 3 ] );
   if( selection != 3 )
   {
      if( argc > 3 )
         precision = atof( argv[ 4 ] );
      else
      {
         usage();
         exit( 20 );
      }

      if( selection == 0 || selection == 1 )
      {
         if( argc >= 6 )
            slotted = atoi( argv[ 5 ] );
         else
         {
            usage();
            exit( 20 );
         }
      }
   }
   if( selection >= 4 )
      max_non_burst_level = atof( argv[ 5 ] );

   switch( selection )
   {
      case 0 : strcpy( descriptor_name, "Model Autocorrelation" );
               break;
      case 1 : strcpy( descriptor_name, "Model Autocovariance" );
               break;
      case 2 : strcpy( descriptor_name, "Model IDC" );
               break;
      case 3 : strcpy( descriptor_name, "Model Stationary descriptors" );
               break;
      case 4 : strcpy( descriptor_name, "Model Burst Total Time Distribution" );
               break;
      case 5 : strcpy( descriptor_name, "Model Burst Duration Distribution" );
               break;
      case 6 : strcpy( descriptor_name, "Model Burst Total Time Expectation" );
               break;
      case 7 : strcpy( descriptor_name, "Model Burst Duration Expectation" );
               break;
      default: usage();
               exit( 21 );
   }


   if( selection != 3 )
   {
       /* open intervals description file */
       sprintf( filename, "%s.intervals", basename );
       if(! (f_intervals = fopen( filename, "r" )) )
       {
           fprintf(stderr,"%s: Error while opening file %s\n", descriptor_name, filename);
           exit( 30 );
       }
   }
  
   if( selection == 2 )
   {
       /* open initial prob. description file */
       sprintf( filename, "%s.init_prob", basename );
       if( !(f_init_prob = fopen( filename, "r" )) )
       {
           fprintf(stderr,"%s: Error while opening file %s\n", descriptor_name, filename);
           exit( 30 );
       }
   }

   /* open reward description file */
   sprintf( filename, "%s.rate_reward.%s", basename, reward_name );
   if( !(f_reward = fopen(filename, "r" )) )
   {
       fprintf(stderr,"%s: Error while opening file %s.rate_reward.%s\n", descriptor_name,
                                                              basename, reward_name);
       exit( 30 );
   }


   /* Building matrix name string */
   strcpy( matrix_name, basename );
   strcat( matrix_name, ".generator_mtx" );

   /* Reading matrix file, just to get the number of states */
   if( (f_in = fopen( matrix_name, "r" )) == NULL )
   {
       fprintf(stderr,"%s: Invalid matrix description file!\n", descriptor_name);
       exit( 30 );
   }

   /* Get number of states */
   fscanf( f_in, "%d\n", &size );
   fclose( f_in );


   if( selection != 3 )
   {
       /* Get intervals vector */
       intervals = get_intervals( f_intervals, &num_intervals );
       fclose( f_intervals );
   }

   if( selection == 2 )
   {
       /* Get initial probability vector */
       pi1 = get_initial_pi( f_init_prob, size );
       fclose( f_init_prob );
   }

   /* Get reward vector */
   reward = get_rewards( f_reward, size );
   fclose( f_reward );

   /* Read matrix */
   P = read_matrix(basename, &lambda, descriptor_name, reward, size, &num_burst_st,
                                                              max_non_burst_level);


#ifdef DEBUG
/**  Debug infos */
/**/ /***** put_trans_matrix(P); *****/
/**/ printf( "\nlambda = %.6e\n", lambda );
/**/ printf( "Size = %d\nBurst level = %.6e\nNumber of Burst states = %d\n\n",
                                    size, max_non_burst_level, num_burst_st );
/**/ for( i = 0; i < P->num_col; i++ ) printf( "pi1[%d]=%.6e\n", i, pi1[ i ] );
/**/ printf( "\n" );
/**/ for( i = 0; i < P->num_col; i++ ) printf( "rwd[%d]=%.6e\n", i, reward[i] );
/**/ printf( "\n" );
/**/ for( i = 0; i < num_intervals; i++) printf("interval[%d]=%.6e\n",i,intervals[i]);
/**/ printf( "\n" );
#endif


   /* Calculate overload time distribution. */
   /* Save distribution results in f_out.   */
   if( selection == 4 )
   {
      sprintf( outname, "%s.overload_time_dist_%6e", basename, max_non_burst_level);
      if( (f_out = fopen( outname, "w" )) == NULL )
      {
          fprintf( stderr, "%s: Cannot open file for output (%s)!\n", descriptor_name, outname);
          exit( 31 );
      }

      Cum_burst_time_dist( f_out, selection, num_intervals, lambda, intervals,
                           precision, P, reward, max_non_burst_level, 
                           descriptor_name, num_burst_st );
   }


   /* Calculate overload time expectation. */
   /* Save expectation results in f_out.   */
   if( selection == 6 )
   {
      sprintf( outname, "%s.mean_overload_time_%6e", basename, max_non_burst_level);
      if( (f_out = fopen( outname, "w" )) == NULL )
      {
          fprintf( stderr,"%s: Cannot open file for output (%s)!\n", descriptor_name, outname);
          exit( 31 );
      }

      Cum_burst_time_dist( f_out, selection, num_intervals, lambda, intervals,
                           precision, P, reward, max_non_burst_level, 
                           descriptor_name, num_burst_st );
   }


   /* Calculate overload duration distribution. */
   /* Save distribution results in f_out.       */
   if( selection == 5 )
   {
      sprintf( outname, "%s.overload_dur_dist_%6e", basename, max_non_burst_level);
      if( (f_out = fopen( outname, "w" )) == NULL )
      {
          fprintf(stderr,"%s: Cannot open file for output (%s)!\n", descriptor_name, outname);
          exit( 31 );
      }

      Cum_burst_time_dist( f_out, selection, num_intervals, lambda, intervals,
                           precision, P, reward, max_non_burst_level,
                           descriptor_name, num_burst_st );
   }


   /* Calculate overload duration expectation. */
   /* Save expectation results in f_out.       */
   if( selection == 7 )
   {
      sprintf( outname, "%s.mean_overload_dur_%6e", basename, max_non_burst_level);
      if( (f_out = fopen( outname, "w" )) == NULL )
      {
          fprintf(stderr,"%s: Cannot open file for output (%s)!\n", descriptor_name, outname);
          exit( 31 );
      }

      Cum_burst_time_dist( f_out, selection, num_intervals, lambda, intervals,
                           precision, P, reward, max_non_burst_level,
                           descriptor_name, num_burst_st );
   }


   /* Seek for peak value and its state */
   peak_value = Peak( reward, size, &state );

   
   /* Calculate IDC points and save all basic measures in f_out.           */
   if( selection == 2 )
   {
       strcpy( outname, basename );
       if( (f_out = fopen( strcat( outname, ".idc" ), "w" )) == NULL )
       {
          fprintf(stderr,"%s: Cannot open file for output (%s.idc)!\n", descriptor_name, outname);
          exit( 31 );
       }

       IDC( f_out, num_intervals, lambda, intervals, precision, pi1, P, reward,
                                                                   peak_value ); 
   }


   /* Calculate autocovariance function and save coordenates in f_out.     */
   if( selection == 1 )
   {
       strcpy( outname, basename );
       if( (f_out = fopen( strcat( outname, ".autocovariance"), "w" )) == NULL )
       {
          fprintf( stderr, "%s: Cannot open file for output (%s.autocovariance)!\n",
                   descriptor_name, outname );
          exit( 31 );
       }

       Autocov( f_out, num_intervals, lambda, intervals, precision, P, reward,
                &pi, &Expect_value, &Variance_value, peak_value, 1, slotted );
   }
    
    
   /* Calculate autocorrelation function and save coordenates in f_out.    */
   if( selection == 0 )
   {
       strcpy( outname, basename );
       if( (f_out = fopen( strcat( outname,".autocorrelation"), "w" )) == NULL )
       {
          fprintf( stderr, "%s: Cannot open file for output (%s.autocorrelation)!\n",
                   descriptor_name, outname );
          exit(31);
       }

       Autocov( f_out, num_intervals, lambda, intervals, precision, P, reward,
                &pi, &Expect_value, &Variance_value, peak_value, 0, slotted );
   }
   
   
   /* Case where stationary probabilities were not yet evaluated.          */
   if( selection == 3 )
   {
       pi = GTH_no_blocks( P );

       /* Calculate expect value */
       Expect_value = Expect( pi, reward, P->num_col );

       /* Calculate variance value */
       Variance_value = Variance( pi, reward, P->num_col, Expect_value );

       /* Second moment */
       Moment_2 = Expect_2( pi, reward, size );

       /* Calculate burstness value */
       burstness = peak_value / Expect_value;


       strcpy( outname, basename );
       if( (f_out = fopen(strcat(outname,".stationary_Descriptors"), "w")) == NULL)
       {
           fprintf( stderr, "%s: Cannot open file for output(%s.stationary_Descriptors)!\n",  	
                    descriptor_name, outname);
           exit( 31 );
       }

       fprintf( f_out, "# Stationary descriptors:\n" );
       fprintf( f_out, "# Expect value   = %.6e\n", Expect_value );
       fprintf( f_out, "# Second moment  = %.6e\n", Moment_2 );
       fprintf( f_out, "# Variance value = %.6e\n", Variance_value );
       fprintf( f_out, "# Peak value     = %.6e in state %d\n", peak_value, state);
       fprintf( f_out, "# Burstiness     = %.6e\n", burstness );

       fclose( f_out );
   }


   if( slotted )
       fprintf( stderr, "\n%s (slotted): Ended\n", descriptor_name );
   else
       fprintf( stderr, "\n%s: Ended\n", descriptor_name );

   
   /* Free allocated buffers */
   free_Matrix( 1, P );
   if( (selection == 0) || (selection == 1) || (selection == 3) )
       free( pi );
   if( selection != 3 )
       free( intervals );
   if( selection != 3 && selection != 0 && selection != 1 )
       free( pi1 );
   free( reward );

   return 0;
}


/*****************************************************************************/
/* Matrix *read_matrix(basename, lambda)                                     */
/*                                                                           */
/* returns a pointer to input matrix.                                        */
/*****************************************************************************/
Matrix *read_matrix( basename, lambda, descriptor_name, reward_vec, size,
                     num_burst_st, max_non_burst_level )
char   *basename;              /* the name of the input file */
double *lambda;                /* uniformization parameter   */
char   *descriptor_name;       /* descriptor name */
double *reward_vec;            /* reward vector */
int    size;                   /* Number of states */
int    *num_burst_st;          /* Number of burst states */
double max_non_burst_level;    /* Maximum value below burst */
{
   Matrix *M;                  /* ptr to rate matrix       */
   Matrix *P;                  /* ptr to prob. matrix      */
   char   matrix_name[ 1000 ]; /* matrix name string       */
   FILE   *f_in;               /* ptr. to input file       */
   int    *st_vec_remap;       /* remapped state vector    */
   double *modified_rwd_vec;   /* Modified reward vector   */
   int    i;                   /* index                    */
   double *pi1_remap;          /* vector with remapped pi1 */
   double *rwd_remap;          /* vector with remapped rewards */


   /* Building matrix name string */
   strcpy( matrix_name, basename );
   strcat( matrix_name, ".generator_mtx" );

   /* Reading matrix file */
   if( (f_in = fopen( matrix_name, "r" )) == NULL )
   {
       fprintf( stderr, "Invalid matrix description file !\n" );
       exit( 30 );
   }


   if( (!strcmp( descriptor_name, "Model Burst Duration Distribution" ))   ||
       (!strcmp( descriptor_name, "Model Burst Total Time Distribution" )) ||
       (!strcmp( descriptor_name, "Model Burst Duration Expectation" ))    ||
       (!strcmp( descriptor_name, "Model Burst Total Time Expectation" )))
   {
      modified_rwd_vec = (double *)my_malloc( size * sizeof( double ) );

      /* Preparing vector with boolean information for state remapping */
      for( i = 0; i < size; i++ )
         if( reward_vec[ i ] <= max_non_burst_level )
            modified_rwd_vec[ i ] = 0;
         else
            modified_rwd_vec[ i ] = 1;

      /* Remaps state vector */
      st_vec_remap = state_remapping( modified_rwd_vec, size, num_burst_st );

      /* allocates cont memory for initial probability */
      pi1_remap = (double *)my_malloc( size * sizeof( double ) );

      /* allocates cont memory for remapped reward vector */
      rwd_remap = (double *)my_malloc( size * sizeof( double ) );

      /* remaps rewards */
      /* Note: reward_vec and rwd_remap are 0-based vectors, and st_vec_map is 1-based */
      for( i = 0; i < size; i++ )
         rwd_remap[ st_vec_remap[ i + 1 ] - 1 ] = reward_vec[ i ];

      /* Rewrite original reward vector */
      for( i = 0; i < size; i++ )
         reward_vec[ i ] = rwd_remap[ i ];

      /* Read remapped matrix */
      M = get_trans_matrix_remap( f_in, st_vec_remap );

      free( modified_rwd_vec );
      free( pi1_remap );
      free( rwd_remap );
   }
   else
      /* Reading matrix */
      M = get_trans_matrix( f_in );


   fclose( f_in );


   generate_diagonal( 'Q', M );
   P = uniformize_matrix( M, lambda );


   return( P );
}


/*****************************************************************************/
/* double Mult_results(size, reward, pi, pi2)                                */
/*                                                                           */
/* Multiply each element with the same index in vectors and sum the results. */
/* The summation is returned by the function.                                */
/* Reward element is squared. All parameters are inputs.                     */
/*****************************************************************************/
double Mult_results( size, reward, pi, pi2 )
long   size;                /* number of chain states */
double *reward;             /* ptr to reward vector */
double *pi;                 /* ptr to steady state probability vector */
double *pi2;                /* ptr to state probabilitiy vector */
{
   double result = 0.0;
   long index;


   for( index = 0; index < size; index++ )
       result += (reward[ index ] * pi[ index + 1 ] * pi2[ index ] );

   return( (double)result );
}


/*****************************************************************************/
/* double Expect(pi, reward, size)                                           */
/*                                                                           */
/* Calculate expect value of Markov chain and return it.                     */
/* All parameters are inputs.                                                */
/*****************************************************************************/
double Expect(pi, reward, size)
double *pi;                    /* ptr to steady state */
double *reward;                /* ptr to reward vector */
long   size;                   /* number of chain states */
{
    long   ind;                /* index */
    double expect_value = 0.0; /* expect value of Markov chain */

    for (ind = 0; ind < size; ind++)
        expect_value += pi[ind + 1] * reward[ind];

    return(expect_value);
}


/*****************************************************************************/
/* double Expect_2(pi, reward, size)                                         */
/*                                                                           */
/* Calculates second moment of Markov chain and returns it.                  */
/* All parameters are inputs.                                                */
/*****************************************************************************/
double Expect_2(pi, reward, size)
double *pi;                      /* ptr to steady state */
double *reward;                  /* ptr to reward vector */
long   size;                     /* number of chain states */
{
    long   ind;                  /* index */
    double expect2_value = 0.0;  /* second moment of Markov chain */

    for (ind = 0; ind < size; ind++)
        expect2_value += pi[ind + 1] * (reward[ind] * reward[ind]);

    return(expect2_value);
}


/*****************************************************************************/
/* double Variance(pi, reward, size, expectation)                            */
/*                                                                           */
/* Calculate variance value of Markov chain and return it.                   */
/* All parameters are inputs.                                                */
/*****************************************************************************/
double Variance(pi, reward, size, expectation)
double *pi;                    /* ptr to steady state */
double *reward;                /* ptr to reward vector */
long   size;                   /* number of chain states */
double expectation;            /* expect value of Markov chain */
{
    long   ind;                /* index */
    double variance = 0.0;     /* expect value of Markov chain */

    for (ind = 0; ind < size; ind++)
        variance += pi[ind + 1] * (reward[ind] - expectation) 
                                * (reward[ind] - expectation);

    return(variance);
}


/*****************************************************************************/
/* double Peak(reward, size, state)                                          */
/*                                                                           */
/* Check for peak value of Markov chain and return it.                       */
/* Input parameters: reward, size.                                           */
/* Output parameter: state.                                                  */
/*****************************************************************************/
double Peak(reward, size, state)
double *reward;          /* ptr to reward vector */
long   size;             /* number of chain states */
long   *state;           /* state of peak rate */
{
    long   ind;          /* index */
    double peak_value;   /* peak value of Markov chain */

    peak_value = reward[0];
    *state = 1;
    for (ind = 1; ind < size; ind++)
        if (reward[ind] > peak_value)
        {
           peak_value = reward[ind]; 
           *state = ind + 1;
        }

    return(peak_value);
}


/*****************************************************************************/
/* void Autocov(f_out, num_intervals, lambda, Tmax, epsilon, pi1, P, reward, */
/*                              pi, expect, variance, lambda_max, selection) */
/*                                                                           */
/* Calculate and print in f_out autocovariance values for interval points.   */
/* All parameters are inputs except pi, expect and variance.                 */
/*****************************************************************************/
void Autocov(f_out, num_intervals, lambda, intervals, epsilon, P, reward, pi,
                            expect, variance, lambda_max, selection, slotted)
FILE   *f_out;              /* ptr to ptr to out file */
int    num_intervals;       /* number of intervals */
double lambda;              /* uniformization rate */
double *intervals;          /* ptr. to intervals */
double epsilon;             /* error */
Matrix *P;                  /* ptr to probabilities matrix */
double *reward;             /* ptr to reward vector */
double **pi;                /* reference to ptr to steady state */
double *expect;             /* expect value of Markov chain */
double *variance;           /* variance value of Markov chain */
double lambda_max;          /* maximum reward value */
int    selection;           /* Type of return value */
int    slotted;             /* boolean var indicating if the model is slotted or not */
{
   double **pi2;            /* ptr to state time probabilities */
   double *reward_copy;     /* copy of reward vector */
   double result;           /* final result of autocovariance */
   double status;           /* used to test if a problem ocurred */
   double result_cor;       /* final result of E[X(t)X(t+s)] */
   int    *N_min = 0;       /* ptr to low bound for each interval */
   int    *N_max = 0;       /* ptr to up bound for each interval */
   long   ind;              /* index variable */
   Matrix *P_transp;        /* P transposed */
   double err_divisor;      /* error divisor to be used to calculate Nmin */
   double sec_moment;       /* Second_moment */
      


   /* Allocate buffers for results */
   if (!slotted)
   {
      N_min  = (int    *) my_malloc(num_intervals * sizeof(int));
      N_max  = (int    *) my_malloc(num_intervals * sizeof(int));
   }
   pi2    = (double **)my_malloc(num_intervals * sizeof(double *));

   for (ind = 0; ind < num_intervals; ind++)
   {
      pi2[ind] = (double *) my_malloc(P->num_col * sizeof(double));
      memset(pi2[ind], 0, P->num_col * sizeof(double));
   }

   reward_copy = (double *) my_malloc(P->num_col * sizeof(double));
   bcopy(reward, reward_copy, P->num_col * sizeof(double));


   Transpose_matrix(P, &P_transp);


   /* Adequate error bound to autocovariance calculus */
   epsilon /= (lambda_max * lambda_max);
   err_divisor = (lambda_max * lambda_max);


   if (!slotted)
   {
      if ((uniformization_method_2(num_intervals, lambda, intervals, epsilon, N_min, N_max,
                                            reward_copy, pi2, P_transp, err_divisor)) != 0)
      {
         if (selection == 0)
            fprintf(stderr,"Autocorrelation: descriptor evaluation did not work.\n");
         else
  	    fprintf(stderr,"Autocovariance: descriptor evaluation did not work.\n");

         exit(87);
      }
   }
   else
      if ((slotted_method(num_intervals, lambda, intervals, reward_copy, pi2, P_transp)) != 0)
      {
         if (selection == 0)
            fprintf(stderr,"Autocorrelation: descriptor evaluation did not work.\n");
         else
  	    fprintf(stderr,"Autocovariance: descriptor evaluation did not work.\n");

         exit(87);
      }


   /* Calculate stationary probability vector                */
   /* ATTENTION: P is altered by the function, so it must be */
   /* called after other functions that uses P.              */
   *pi = GTH_no_blocks(P);


   /* Calculate expect value */
   *expect = Expect(*pi, reward, P->num_col);

   /* Calculate variance value */
   *variance = Variance(*pi, reward, P->num_col, *expect);

   /* Calculate second moment value */
   sec_moment = *variance + (*expect * *expect);


   /* Autocorrelation case */
   if (selection == 0)
   {
      fprintf(f_out,"# Autocorrelation points:\n");
      fprintf(f_out,"# time x Cor[X(t), X(t + time)]\n");
   }

   /* Autocovariance case */
   if (selection == 1)
   {
      fprintf(f_out,"# Autocovariance points:\n");
      fprintf(f_out,"# time x Cov[X(t), X(t + time)]\n");
   }

   for (ind = 0; ind < num_intervals; ind ++)
   {
       /* Multiply vectors */
       result = (double) Mult_results(P->num_col, reward, *pi, pi2[ind]);

       status = result;

       /* Calculate autocovariance */
       if ((*expect) != 0.0)
          result -= ((*expect) * (*expect));

       /* Autocorrelation case */
       if (selection == 0)
       {
          /* Calculate autocorrelation */
          result_cor = result/(*variance);

          if ((result_cor < 0.0) && (status != 0.0))
             fprintf(stderr,"Model Autocorrelation: Negative value for time %.6f MAY be caused by relative high precision value.\n", intervals[ind]);

          fprintf(f_out, "%.6f %.10e\n", intervals[ind], result_cor);
       }
       
       /* Autocovariance case */
       if (selection == 1)
       {
          if ((result < 0.0) && (status != 0.0))
             fprintf(stderr,"Model Autocovariance: Negative value for time %.6f MAY be caused by relative high precision value.\n", intervals[ind]);

          fprintf(f_out, "%.6f %.10e\n", intervals[ind], result);
       }
   }


   fclose(f_out);


   /* Free allocated buffers, if not slotted */
   if (!slotted)
   {
       free(N_min);
       free(N_max);
   }

   for (ind = 0; ind < num_intervals; ind++)
       free(pi2[ind]);

   free(pi2);
   free_Matrix(1, P_transp);
}


/****************************************************************************/
/*                                                                          */
/*  Same as uniformization_method function in module uniformization.c       */
/*  Modified by Sidney Lucena to use lo_bound2 and adapted to error bounds  */
/*  for autocovariance evaluation.                                          */
/*                                                                          */
/****************************************************************************/
int uniformization_method_2(num_intervals, lambda, time_int, epsilon, N_min,
                                        N_max, rewards, pi2, P, err_divisor)
int    num_intervals;             /* number of intervals */
double lambda;                    /* uniformization rate */
double *time_int;                 /* ptr. to time values */
double epsilon;                   /* error */
int    *N_min;                    /* lower limit */
int    *N_max;                    /* upper limit */
double *rewards;                  /* rewards vector */
double **pi2;                     /* final state probabilities */
Matrix *P;                        /* transition probability matrix */
double err_divisor;               /* error divisor */
{
   int    i, n, k;                /* temporary variables */
   int    N_min2;                 /* lower limit */
   int    N_max2 = 0;             /* upper limit */
   int    num_states;             /* number of states */
   double factor;                 /* sum of Poisson serie */
   double *poisson_factor;        /* sum of Poisson serie */
   double *aux1, *aux2;           /* auxiliar vectors */
   double error;                  /* calculated error to br used in lo_boun2 */ 


   /* number of states of probability matrix */
   num_states = P->num_col;

   /* allocate auxiliar vectors */
   aux1           = (double *)my_malloc(num_states * sizeof(double));
   poisson_factor = (double *)my_malloc(num_intervals * sizeof(double));
       
   bcopy(rewards, aux1, num_states * sizeof(double));

   error = 1.0e-30/(err_divisor);

   /* calculate N_min and N_max */
   for (n = 0; n < num_intervals; n++)  
   {
       /* N_max */
       if (time_int[n] != 0)
           N_max[n] = partial_poisson_sum(lambda * time_int[n], epsilon);
       else
           N_max[n] = 0;
       if (N_max[n] > N_max2)
           N_max2 = N_max[n];

       /* if (N_max[n] < 0)
           return((int)(-1 * N_max[n])); */

       /* N_min */
       lo_bound2(&N_min2, &factor, lambda * time_int[n], error);
       N_min[n] = N_min2;
       poisson_factor[n] = factor;

       /* if (N_min[n] < 0)
           return((int)(-1 * N_min[n])); */
   }

   for (n = 0; n <= N_max2; n++)  /* total number of transitions */
   {
       for (i = 0; i < num_intervals; i++)
       {
           if (n >= N_min[i] && n <= N_max[i])
           {
               aux2 = pi2[i];
               for (k = 0; k < num_states; k++)
                   aux2[k] += poisson_factor[i] * aux1[k];
               poisson_factor[i] = (poisson_factor[i] * lambda * time_int[i])/(n+1);
           }
       }

       if (n < N_max2)
       {
           bcopy(aux1, rewards, num_states * sizeof(double));
           vector_matrix_multiply(rewards, P, aux1);
       }
   } 

   free(aux1);
   free(poisson_factor);

   return (0);
}


/****************************************************************************/
/*                                                                          */
/*  Evaluate pi(t) for a known numbers of transitions.                      */
/*                                                                          */
/****************************************************************************/
int slotted_method(num_intervals, lambda, time_int, rewards, pi2, P)
int    num_intervals;             /* number of intervals */
double lambda;                    /* uniformization rate */
double *time_int;                 /* ptr. to time values */
double *rewards;                  /* rewards vector */
double **pi2;                     /* final state probabilities */
Matrix *P;                        /* transition probability matrix */
{
   int    i, n;                   /* temporary variables */
   int    N_max;                  /* upper limit */
   char   N_max_str[20];          /* string with N_max */
   int    num_states;             /* number of states */
   double *aux1;                  /* auxiliar vectors */
   double time_slot;              /* time interval of a slot */


   /* number of states of probability matrix */
   num_states = P->num_col;

   /* Time interval of a slot, discounts MARKUP (0.001, see uniformize_matrix in Matrix.c). */
   time_slot = 1.0/(lambda - 0.001);

   /* Maximum number of transitions */
   sprintf(N_max_str, "%.0f", time_int[num_intervals - 1]/time_slot);
   N_max = atoi(N_max_str);


   /* allocate auxiliar vectors */
   aux1 = (double *)my_malloc(num_states * sizeof(double));

   bcopy(rewards, aux1, num_states * sizeof(double));


   for (i = 0, n = 0; n <= N_max; n++)
   {
       while (time_int[i] >= (n * time_slot) && time_int[i] < ((n + 1) * time_slot) && i < num_intervals)
       {
          bcopy(aux1, pi2[i], num_states * sizeof(double));
          i++;
       }

       if (n < N_max)
       {
          bcopy(aux1, rewards, num_states * sizeof(double));
          vector_matrix_multiply(rewards, P, aux1);
       }
   }


   /* Releasing vector */
   free(aux1);

   return (0);
}


/******************************************************************************/
/* void IDC(f_out, num_intervals, lambda, intervals, epsilon, pi1, P, reward, */
/*                                                                lambda_max) */
/*                                                                            */
/* Calculate and print in f_out IDC values and all other basic measures, or   */
/* only E[N(t)], for the interval points.                                     */
/* All parameters are inputs.                                                 */
/******************************************************************************/
void IDC(f_out, num_intervals, lambda, time, epsilon, pi1, P, reward, lambda_max)
FILE   *f_out;              /* ptr to ptr to out file */
int    num_intervals;       /* number of intervals */
double lambda;              /* uniformization rate */
double *time;               /* ptr. to intervals vector */
double epsilon;             /* error */
double *pi1;                /* ptr to initial state probabilities */
Matrix *P;                  /* ptr to probabilities matrix */
double *reward;             /* ptr to reward vector */
double lambda_max;          /* maximum reward value */
{
   double *expect;          /* expect points of N(t) */
   double *expect_2;        /* expect points of N(t)^2 */
   double *variance;        /* variance points of N(t) */
   int    *N_min;           /* ptr to low bound for each interval */
   int    *N_max;           /* ptr to up bound for each interval */
   long   ind;              /* index variable */
   double *poisson_factor;  /* vector of sum of Poiss. serie for each interv. */
   double factor;           /* sum of Poisson serie */
   int    N_min_aux;        /* lower limit */
   int    N_max2 = 0;       /* upper limit */
   int    n, i;             /* temporary var. */
   double epsilon2;         /* error bound to pass as parameter */
   double error;            /* calculated error to pass to lo_bound2 function */


   /* Allocate buffers for results */
   N_min          = (int    *) my_malloc(num_intervals * sizeof(int));
   N_max          = (int    *) my_malloc(num_intervals * sizeof(int));
   poisson_factor = (double *) my_malloc(num_intervals * sizeof(double));
   expect         = (double *) my_malloc((unsigned)(num_intervals) *
                                                                sizeof(double));
   expect_2       = (double *) my_malloc((unsigned)(num_intervals) *
                                                                sizeof(double));
   variance       = (double *) my_malloc((unsigned)(num_intervals) *
                                                                sizeof(double));

   epsilon2 = epsilon;

   /* calculate N_min and N_max */
   for (n = 0; n < num_intervals; n++)  
   {
       epsilon2 = epsilon/(time[n] * lambda_max);

       /* N_max */
       if (time[n] != 0)
           N_max[n] = partial_poisson_sum(lambda * time[n], epsilon);
       else
           N_max[n] = 0;

       if (N_max[n] > N_max2)
           N_max2 = N_max[n];

       /* if (N_max[n] < 0)
       {
	   fprintf(stderr,"IDC: descriptor evaluation did not work.\n");
           exit((int)(-1 * N_max[n]));
       } */

       /* N_min */
       error = 1.0e-30/(lambda_max * time[n]);

       lo_bound2(&N_min_aux, &factor, lambda * time[n], error);
       N_min[n] = N_min_aux; 
       poisson_factor[n] = factor;

       /* if (N_min[n] < 0)
       {
	       fprintf(stderr,"IDC: descriptor evaluation did not work.\n");
           exit((int)(-1 * N_min[n]));
       } */
   }

   /* Save poisson factors vector for next use */
   /* bcopy(poisson_factor, save_poisson, num_intervals * sizeof(double)); */

   expect = Expect_n(time, lambda, P, reward, pi1, N_max, N_min, poisson_factor,
                                                         num_intervals, N_max2);

   N_max2 = 0;
   for (n = 0; n < num_intervals; n++)  
   {
       N_min[n] = exp2_min_bound(time[n], lambda, lambda_max, reward, 
                                             P->num_col, &(poisson_factor[n]));

       N_max[n] = exp2_max_bound(epsilon, time[n], lambda, lambda_max, reward,
                                                                   P->num_col);

       if (N_max[n] > N_max2)
           N_max2 = N_max[n];

#ifdef DEBUG
       printf("\n%d Nmin= %d Nmax= %d\n", n, N_min[n], N_max[n]); 
#endif

       /* if (N_max[n] < 0)
       {
	       fprintf(stderr,"IDC: descriptor evaluation did not work.\n");
           exit((int)(-1 * N_max[n]));
       } */
       
       /* if (N_min[n] < 0)
       {
	   fprintf(stderr,"IDC: descriptor evaluation did not work.\n");
           exit((int)(-1 * N_min[n]));
       } */
   }

   expect_2 = Expect_n2(time, lambda, P, reward, pi1, N_max, N_min, 
                                         poisson_factor, num_intervals, N_max2);

   for (i = 0; i < num_intervals; i++)
       variance[i] = expect_2[i] - (expect[i] * expect[i]);

   fprintf(f_out,"# IDC(t) and Basic measures points:\n");
   fprintf(f_out,"# Time x IDC x E[N(t)] x Var[N(t)] x E[N^2(t)]\n");

   for (ind = 0; ind < num_intervals; ind ++)
       if (time[ind] == 0)
          fprintf(f_out, "%.6e %.10e %.10e %.10e %.10e\n", time[ind], 0.0, 0.0, 0.0, 0.0);
       else
       {
          if (variance[ind] < 0.0)
             fprintf(stderr,"Model IDC: Negative value for time %.6f MAY be caused by relative low precision value.\n", time[ind]);

          if ((expect[ind] == 0.0) && (N_max[ind] != -1))
             fprintf(stderr,"Model IDC: Infinity value for time %.6f MAY be caused by relative high precision value.\n", time[ind]);
          else
             if ((variance[ind] == 0.0) && (N_max[ind] != -1))
                fprintf(stderr,"Model IDC: Zero value for time %.6f MAY be caused by relative high precision value.\n", time[ind]);

          if ((variance[ind] > 0.0) && (expect[ind] > 0.0))
             fprintf(f_out, "%.6e %.10e %.10e %.10e %.10e\n", time[ind],
                     variance[ind]/expect[ind], expect[ind], variance[ind], expect_2[ind]);
       }
	      
   fclose(f_out);

   /* Free allocated buffers */
   free(N_min);
   free(N_max);
   free(expect);
   free(expect_2);
   free(variance);
   free(poisson_factor);
}


/*****************************************************************************/
/* double *Expect_n(time, lambda, P, reward, pi1, N_max, N_min,              */
/*                     poisson_factor, erlang_factor, num_intervals, N_max2) */
/*                                                                           */
/* Return expectation of arrival count for different interval times.         */
/* All parameters are inputs.                                                */
/*****************************************************************************/
double *Expect_n(time, lambda, P, reward, pi1, N_max, N_min, poisson_factor,
                                                      num_intervals, N_max2)
double *time;               /* vector of interval times */
double lambda;              /* uniformization rate */
Matrix *P;                  /* ptr to probabilities matrix */
double *reward;             /* ptr to reward vector */
double *pi1;                /* ptr to initial state probabilities */
int    *N_min;              /* ptr to low bound for each interval */
int    *N_max;              /* ptr to up bound for each interval */
double *poisson_factor;     /* sum of Poisson serie */
int    num_intervals;       /* number of intervals */
int    N_max2;              /* maximum N_max */
{
   double rv_n;         /* result of vectors multiply reward x v(n) */
   double f_n;          /* result of resursive function f(n) */
   double f_pre_n = 0;  /* result of resursive function f(n-1) */
   double *v_n;         /* vector of v(n), state probab. at discrete time n */
   double *v_n_aux;     /* auxiliar ptr to transient states probabil. */
   double *exp_t;       /* vector of expectations for each interval time */
   int    n, i, k;      /* temporary var. */


   v_n     = (double *) my_malloc(P->num_col * sizeof(double));
   v_n_aux = (double *) my_malloc(P->num_col * sizeof(double));
   exp_t   = (double *) my_malloc(num_intervals * sizeof(double));
   memset(exp_t, 0, num_intervals * sizeof(double));

   bcopy(pi1, v_n, P->num_col * sizeof(double));

   for (n = 0; n <= N_max2; n++)
   {
       /* Vector x vector multiplication for updating r*v(n) */
       for (k = 0, rv_n = 0; k < P->num_col; k++)
           rv_n += reward[k] * v_n[k];

       /* Updating f(n) */
       if (n == 0)
           f_n = rv_n;
       else
           f_n = (n/(n + 1.0)) * f_pre_n + (1.0/(n + 1.0)) * rv_n;

       f_pre_n = f_n;

       for (i = 0; i < num_intervals; i++)
       {     
           if (n >= N_min[i] && n <= N_max[i])
           {
               /* exp_t[i] += exp(-lambda * time[i]) * 
                              poisson_factor[i] * f_n; */

               /* To be used if N_min is used */
               exp_t[i] += poisson_factor[i] * f_n; 

               /* Updating poisson factor */
               poisson_factor[i] *= (lambda * time[i])/(n+1);
           }
       }

       /* Updating v(n) */
       bcopy(v_n, v_n_aux, P->num_col * sizeof(double));
       vector_matrix_multiply(v_n_aux, P, v_n);
   }

   for (i = 0; i < num_intervals; i++)
       exp_t[i] *= time[i];

   free(v_n);
   free(v_n_aux);

   return exp_t;
}

/*****************************************************************************/
/* int exp2_max_bound(epsilon, time, lambda, lambda_max, reward, size)       */
/*                                                                           */
/* Returns N_max to calculate second moment for a given interval time.       */
/* All parameters are inputs.                                                */
/*****************************************************************************/
int exp2_max_bound(epsilon, time, lambda, lambda_max, reward, size)
double epsilon;             /* error */
double time;                /* interval time */
double lambda;              /* uniformization rate */
double lambda_max;          /* maximum reward value */
double *reward;             /* ptr to reward vector */
int    size;                /* size of Markov chain */
{
   double sum_lambda;       /* sum of rewards */
   int    N;                /* Nmax */
   double error;            /* calculated error to pass as parameter */
   int    n;                /* index */


   for (n = 0, sum_lambda = 0.0; n < size; n++)
       sum_lambda += reward[n];

   error = epsilon/(lambda_max * sum_lambda * (time * time));

#ifdef DEBUG
printf("Error: %.6e\n", error);
#endif

   if (time != 0)
       N = partial_poisson_sum(lambda * time, error);
   else
       N = 0;

   return (N);
}

/*****************************************************************************/
/* int exp2_min_bound(time, lambda, lambda_max, reward, size, Nmin,          */
/*                                                                val_n_min) */
/*                                                                           */
/* Returns N_min to calculate second moment for a given interval time.       */
/* All parameters are inputs except the last, which gives the poisson term   */
/* for Nmin.                                                                 */
/*****************************************************************************/
int exp2_min_bound(time, lambda, lambda_max, reward, size, val_n_min)
double time;                /* interval time */
double lambda;              /* uniformization rate */
double lambda_max;          /* maximum reward value */
double *reward;             /* ptr to reward vector */
int    size;                /* size of Markov chain */
double *val_n_min;          /* ptr to poisson factor for Nmin */
{
   double sum_lambda;       /* sum of rewards */
   int    N;                /* Nmin - 3 */
   double error;            /* calculated error to pass as parameter */
   int    n;                /* index */


   for (n = 0, sum_lambda = 0.0; n < size; n++)
       sum_lambda += reward[n];

   error = 1.0e-30/(lambda_max * sum_lambda * (time * time));

   lo_bound2(&N, val_n_min, lambda * time, error);

   /* if (N != 0) - Causa problema e nao tem maiores efeitos.
   {
       N++;
       *val_n_min *= (lambda * time)/N;
       N++;
       *val_n_min *= (lambda * time)/N;
   } */

   return (N);
}

/*****************************************************************************/
/* double *Expect_n2(time, lambda, P, reward, pi1, N_max, N_min,             */
/*                                    poisson_factor, num_intervals, N_max2) */
/*                                                                           */
/* Return second moment of arrival count for different interval times.       */
/* All parameters are inputs.                                                */
/*****************************************************************************/
double *Expect_n2(time, lambda, P, reward, pi1, N_max, N_min, poisson_factor,
                                                       num_intervals, N_max2)
double *time;               /* vector of interval times */
double lambda;              /* uniformization rate */
Matrix *P;                  /* ptr to probabilities matrix */
double *reward;             /* ptr to reward vector */
double *pi1;                /* ptr to initial state probabilities */
int    *N_min;              /* ptr to low bound for each interval */
int    *N_max;              /* ptr to up bound for each interval */
double *poisson_factor;     /* sum of Poisson serie */
int    num_intervals;       /* number of intervals */
int    N_max2;              /* maximum N_max */
{
   int    t, j, m, k;       /* temporary var. */
   double *sum2;            /* vector of partial summation */
   double sum;              /* partial summation */
   double *exp2_t;          /* result vector */
   double *D_novo;          /* vector of new D results for each state */
   double *D_antes;         /* vector of previous D results for each state */
   double *pi;              /* ptr to transient states probabil. */
   double *pi_aux;          /* auxiliar ptr to transient states probabil. */
   double *poisson_aux;     /* auxiliar ptr to poisson factors */



   /* Allocate vectors */
   sum2 = (double *) my_malloc(num_intervals * sizeof(double));
   exp2_t = (double *) my_malloc(num_intervals * sizeof(double));
   memset(exp2_t, 0, num_intervals * sizeof(double));
   D_novo = (double *) my_malloc(P->num_col * sizeof(double));
   memset(D_novo, 0, P->num_col * sizeof(double));
   D_antes = (double *) my_malloc(P->num_col * sizeof(double));
   memset(D_antes, 0, P->num_col * sizeof(double));
   pi = (double *) my_malloc(P->num_col * sizeof(double));
   pi_aux = (double *) my_malloc(P->num_col * sizeof(double));
   poisson_aux  = (double *) my_malloc(num_intervals * sizeof(double));

   /* Initializations */
   bcopy(pi1, pi, P->num_col * sizeof(double));

#ifdef DEBUG
   printf("Nmax2= %d\n",N_max2);
#endif

   for (j = 0; j <= N_max2; j++)
   {
       /* Initializing poisson factor to be used */
       for (t = 0; t < num_intervals; t++)
       {
           poisson_aux[t] = poisson_factor[t];
           sum2[t] = 0;
       }

       /* To be used in Erlang calculus */
       for (m = 0; m <= j + 1; m++)
       {
           for (t = 0; t < num_intervals; t++)
           {     
               if (m >= N_min[t])
               {
                   /* Using N_min */
                   sum2[t] += poisson_aux[t];

#ifdef DEBUG
printf("j = %d  m = %d  Poisson term = %.6e\n", j, m, poisson_aux[t]);
printf("1 - Erlang[%d] = %.6e\n", j, sum2[t]);
#endif

                   /* Updating poisson factor */
                   poisson_aux[t] *= (lambda * time[t])/(m+1);
               }
           }
       }

       /* Calculate Erlang(j + 2, lambda * t) */
       /* This may be removed and the subtraction put at the end of calculus */
       for (t = 0; t < num_intervals; t++)
       {
           sum2[t] = 1 - sum2[t];
#ifdef DEBUG
printf("Erlang[%d] = %.6e\n", j, sum2[t]);
#endif
       }

       /* S_j */
       for (k = 0, sum = 0; k < P->num_col; k++)
       {
           if (j)
               D_novo[k] = D_function(D_antes, P, reward, pi, k);
           else
               D_novo[k] = pi[k] * reward[k];

#ifdef DEBUG
printf("D[%d,%d] = %.6e  Rwd[%d] = %.6e\n", k, j, D_novo[k], k, reward[k]);
#endif
           sum += D_novo[k] * reward[k];
       }

#ifdef DEBUG
printf("Sum[%d] = %.6e\n", j, sum);
#endif

       /* Update next D(j-1) */
       for (k = 0; k < P->num_col; k++)
           D_antes[k] = D_novo[k];

       /* Updating pi */
       vector_matrix_multiply(pi, P, pi_aux);
       bcopy (pi_aux, pi, P->num_col * sizeof(double));

       for (t = 0; t < num_intervals; t++)
           if (j <= N_max[t])
               exp2_t[t] += (sum2[t] * sum);
               /* In the case that no previous subtraction was done */
               /* exp2_t[t] += (sum - (sum2[t] * sum)); */
 
   }

   for (t = 0; t < num_intervals; t++)
       exp2_t[t] *= (2/(lambda * lambda));

   free(pi);
   free(pi_aux);
   free(poisson_aux);
   free(sum2);
   free(D_novo);
   free(D_antes);

   return exp2_t;
}


/*****************************************************************************/
/* double D_function(D_antes, P, reward, pi, k)                              */
/*                                                                           */
/* Returns D[k]^j, which is Sum_{s=1}^N{D_s^{j-1} Psk} + reward[k] pi[k]^j.  */
/*****************************************************************************/
double D_function(D_antes, P, reward, pi, k)
double *D_antes;         /* previous D[k] */
Matrix *P;               /* ptr to probabilities matrix */
double *reward;          /* ptr to reward vector */
double *pi;              /* ptr to transient state prob. in j discrete time */
int    k;                /* index of D */
{
   double sum;           /* partial summation */
   double D_novo;        /* result value */
   double Psk;           /* P[s][k] value */
   int    s;             /* index variable */


   sum = 0;

   for (s = 0; s < P->num_col; s++)
   {
       Psk = get_matrix_position(s + 1, k + 1, P);
       sum += (D_antes[s] * Psk);
   }

   D_novo = sum + reward[k] * pi[k];

   return D_novo;
}


/*****************************************************************************/
/* void Cum_burst_time_dist(f_out, f_out2, num_intervals, lambda, intervals, */
/*                                   epsilon, P, trigger, pi1, ea, el, mttf) */
/*                                                                           */
/* Calculate and print in f_out overload time and duration statistics for    */
/* the interval points.                                                      */
/* All parameters are inputs except ea, el and mttf.                         */
/*****************************************************************************/
void Cum_burst_time_dist(f_out, selection, num_intervals, lambda, time, epsilon,
                              P, reward, trigger, descriptor_name, num_burst_st)
FILE   *f_out;              /* ptr to distribution file */
int    selection;           /* descriptor selection */
int    num_intervals;       /* number of intervals */
double lambda;              /* uniformization rate */
double *time;               /* ptr. to intervals vector */
double epsilon;             /* error */
Matrix *P;                  /* ptr to probabilities matrix */
double *reward;             /* ptr to reward vector */
double trigger;             /* trigger value for burst decision */
char   *descriptor_name;    /* descriptor name */
int    num_burst_st;        /* number of burst states */
{
 double *pi1_b;                /* ptr to burst state prob. */
 double *cot = 0;              /* ptr to cum. oper. time  */
 double *p;                    /* ptr to s/t values */
 double *t;                    /* ptr to t values */
 double *re;                   /* ptr to burst duration dist. */
 double *ea;                   /* ptr to expected availability */
 double *el;                   /* ptr to expected lifetime */
 /* double mttf; */            /* expected burst duration, t -> infinity */
 int    i;                     /* temporary variable */
 double *pi_non_burst;         /* ptr to non-burst states probabilities */
 Matrix *P_dup;                /* ptr to duplicated transition prob. matrix */
 double *pi;                   /* ptr to steady-state probabilities */



 /* Allocate vectors */
 p  = (double *)my_malloc(num_intervals * sizeof(double));
 t  = (double *)my_malloc(num_intervals * sizeof(double));
 re = (double *)my_malloc(num_intervals * sizeof(double));
 ea = (double *)my_malloc(num_intervals * sizeof(double));
 el = (double *)my_malloc(num_intervals * sizeof(double));


 /* Avaliate number of bursty states */
 if (num_burst_st == P->num_col)
 {
     fprintf(stderr,"%s: Maximum underload level (%f) implies in all states to be overload states!\n",
                                                                            descriptor_name, trigger);
     fprintf(stderr,"%s: You probably should increase maximum underload level.\n", descriptor_name);
     exit(57);
 }

 /* Avaliate number of bursty states */
 if (num_burst_st == 0)
 {
     fprintf(stderr,"%s: Maximum underload level (%f) implies in all states to be underload states!\n",
                                                                             descriptor_name, trigger);
     fprintf(stderr,"%s: You probably should low maximum underload level.\n", descriptor_name);
     exit(57);
 }

 /* calculate interval between two s/t values */
 for (i = 0; i < num_intervals; i++)
     p[i] = time[i]/time[num_intervals - 1];


 /* Duplicating trans. matrix to use it for steady-state prob. evaluation */
 Duplicate_matrix(P, &P_dup);


 /* Calculate stationary probability vector                */
 /* ATTENTION: P is altered by the function, so it must be */
 /* called after other functions that uses P.              */
 pi = GTH_no_blocks(P_dup);

 /* Calculate stationary bursty states entries probabilities */
 pi_non_burst = pi_cond_non_burst(pi, reward, P->num_col, trigger);
 pi1_b = calc_init_burst(P, P->num_col, pi_non_burst);


#ifdef DEBUG
 /**/ printf("\n");
 /**/ for (i = 0; i < P->num_col; i++) printf("pi1_b[%d]=%.6e\n",i,pi1_b[i]);
#endif


 /* Evaluates measures */
 /* Overload Time Distribution */
 if (selection == 4)
 {
     cot = new_omega(num_burst_st, pi1_b, P, lambda, epsilon, num_intervals, (int)1, p,
                                     &(time[num_intervals - 1]), NULL, NULL, NULL, NULL);

     if (cot == NULL)
     {
         fprintf(stderr,"%s: descriptor evaluation did not work.\n", descriptor_name);
         exit (87);
     }

     fprintf(f_out,"# Overload time distribution:\n");
     fprintf(f_out,"# Maximum underload level = %.10e\n# Obs.time = %.10e\n",
                                         trigger, time[num_intervals - 1]);
     fprintf(f_out,"# t  x  P[Overload Time(obs.time) < t] x Max.under.level\n");
     if (p[0] != 0.0)
         fprintf(f_out,"%.6e %.10e %.6e\n", (double)0, (double)0, trigger);
     for (i = 0; i < (num_intervals - 1); i++)
         fprintf(f_out,"%.6e %.10e %.6e\n", time[num_intervals - 1] * p[i], cot[i], trigger);
     fprintf(f_out,"%.6e %.10e %.6e\n\n", time[num_intervals - 1], (double)1, trigger);

 }

 /* Burst Duration distribution or Expectation of burst duration or total time distributions. */
 if ((selection == 5) || (selection == 6) || (selection == 7))
 {
     /* Calculating t's for burst duration evaluation */
     for (i = 0; i < num_intervals; i++)
         t[i] = p[i] * time[num_intervals - 1];

     if (selection == 5)
     {
         /* Overload Duration Distribution */
         /* Values of p, ea2, el2 and mttf2 doesn't matter */
         cot = new_omega(num_burst_st, pi1_b, P, lambda, epsilon, (int)1, num_intervals, p, t,
                                                                        NULL, re, NULL, NULL);

         if (cot == NULL)
         {
             fprintf(stderr,"%s: descriptor evaluation did not work.\n", descriptor_name);
             exit (87);
         }

         fprintf(f_out,"# Overload Duration Distribution:\n");
         fprintf(f_out,"# Maximum underload level = %.10e\n# Obs.time = %.10e\n",
                                             trigger, time[num_intervals - 1]);
         fprintf(f_out,"# t  x  P[Overload Duration(obs.time) > t] x Max.under.level\n");
         if (t[0] != 0.0)
             fprintf(f_out,"%.6e %.10e %.6e\n", (double)0, (double)1, trigger);
         for (i = 0; i < (num_intervals); i++)
             fprintf(f_out, "%.6e %.10e %.6e\n", t[i], re[i], trigger);
         fprintf(f_out, "\n");
     }

     if (selection == 6)
     {
         /* Expected Overload Time */
         cot = new_omega(num_burst_st, pi1_b, P, lambda, epsilon, (int)1, num_intervals, p, t,
                                                                        ea, NULL, NULL, NULL);

         if (cot == NULL)
         {
             fprintf(stderr,"%s: descriptor evaluation did not work.\n", descriptor_name);
             exit (87);
         }

         fprintf(f_out, "# Maximum underload level x E[Overload Time(obs.time)] x Obs.time:\n");
         for (i = 0; i < (num_intervals); i++)
             fprintf(f_out, "%.6e %.10e %.6e\n", trigger, (ea[i] * t[i]), t[i]);
     }

     if (selection == 7)
     {
         /* Expected Overload Duration */
         cot = new_omega(num_burst_st, pi1_b, P, lambda, epsilon, (int)1, num_intervals, p, t,
                                                                        NULL, NULL, el, NULL);

         if (cot == NULL)
         {
             fprintf(stderr,"%s: descriptor evaluation did not work.\n", descriptor_name);
             exit (87);
         }

         fprintf(f_out, "# Maximum underload level x E[Overload Duration(obs.tme)] x Obs.time:\n");
         for (i = 0; i < (num_intervals); i++)
             fprintf(f_out, "%.6e %.10e %.6e\n", trigger, el[i], t[i]);
     }
 }


 fclose(f_out);


 /* release vectors */
 free_Matrix(1, P_dup);
 free((double *)pi);
 free((double *)pi_non_burst);
 free((double *)pi1_b);
 free((double *)p);
 free((double *)t);
 free((double *)re);
 free((double *)ea);
 free((double *)el);
 free((double *)cot);
}


/**************************************************************************/
/* void Duplicate_matrix(P, P_dup)                                        */
/*                                                                        */
/* Duplicates P and saves it in P_dup.                                    */
/**************************************************************************/
void Duplicate_matrix(P, P_dup)
Matrix *P;                  /* ptr to probabilities matrix */
Matrix **P_dup;             /* ref. to ptr to transposed probab. matrix */
{
   int    i, j;             /* index variables */
   double value;            /* value of matrix position */

   *P_dup = initiate_matrix(P->num_col);

   for (i = 1; i <= P->num_col; i++)
       for (j = 1; j <= P->num_col; j++)
       {
           value = get_matrix_position(i, j, P);
           put_matrix_position(i, j, value, *P_dup);
       }
}


/*****************************************************************************/
/* void Transpose_matrix(P, P_transp)                                        */
/*                                                                           */
/* Transpose P and saves it in P_transp.                                     */
/*****************************************************************************/
void Transpose_matrix(P, P_transp)
Matrix *P;                  /* ptr to probabilities matrix */
Matrix **P_transp;          /* ref. to ptr to transposed probab. matrix */
{
   int    i, j;             /* index variables */
   double value;            /* value of matrix position */

   *P_transp = initiate_matrix(P->num_col);

   for (i = 1; i <= P->num_col; i++)
       for (j = 1; j <= P->num_col; j++)
       {
           value = get_matrix_position(j, i, P);
           put_matrix_position(i, j, value, *P_transp);
       }
}


void usage()
{
   fprintf(stdout,"\nUsage: descriptors <basename> <option> <reward> [precision] [slotted] [overload_level]\n");
   fprintf(stdout,"basename:    base model name;\n");
   fprintf(stdout,"options:     0 - Autocorrelation\n");
   fprintf(stdout,"             1 - Autocovariance\n");
   fprintf(stdout,"             2 - IDC\n");
   fprintf(stdout,"             3 - Stationary descriptors (mean, variance, etc)\n");
   fprintf(stdout,"             4 - Overload Time Distribution\n");
   fprintf(stdout,"             5 - Overload Duration Distribution;\n");
   fprintf(stdout,"             6 - Overload Time Expectation\n");
   fprintf(stdout,"             7 - Overload Duration Expectation;\n");
   fprintf(stdout,"reward:      rate reward file name;\n");
   fprintf(stdout,"precision:   error precision for numeric evaluation (except option 3);\n");
   fprintf(stdout,"slotted:     1 if model is slotted and 0 if not, needed for options 0 and 1\n");
   fprintf(stdout,"             (MUST NOT be used on others);\n");
   fprintf(stdout,"overload_level: maximum rate before overload, needed for options 4, 5, 6 and 7\n");
   fprintf(stdout,"                (MUST NOT to be used on others).\n");
   fprintf(stdout,"\n");
}
