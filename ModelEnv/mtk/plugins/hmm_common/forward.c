#include "forward.h"

#include <math.h>
#include <stdlib.h>

#include "matrix.h"

/*****************************************************************************/
/* Calculates the probability of the partial observation sequence from the   */
/* beginning to time t and state Sj at time t, given the model.              */
/*****************************************************************************/
int forward_filter( Parameter * param, double * pi0,
                    int n_obs, int * obs,
                    double ** alpha, double * scale,
                    double * ll, double * final_pi )
{
    int i, j, b, t;
    double sum, invScale;
    char free_alpha, free_scale;

    if( !param || !obs )
        return -1;

    if( !pi0 )
        pi0 = param->pi;

    /*********************************************************************
     *  Initialization                                                   *
     *********************************************************************/
    free_alpha = 0;
    if( !alpha )
    {
        free_alpha = 1;
        alpha = create_zero_matrix( n_obs, param->N );
    }

    free_scale = 0;
    if( !scale )
    {
        free_scale = 1;
        scale = create_zero_vector( n_obs );
    }

    /*********************************************************************
     *                                                                   *
     *  BEGIN algorithm                                                  *
     *                                                                   *
     *********************************************************************/
    invScale = 0.0;
    for( i = 0; i < param->N; i++ )
    {
        ALPHA(0,i) = pi0[i] * PROB_FIRST_OBS(0,i);
        invScale += ALPHA(0,i);
    }

    SCALE(0) = 1.0 / invScale;
    for( i = 0; i < param->N; i++ )
    {
        ALPHA(0,i) *= SCALE(0);
    }

    for( b = 1; b < param->B && b < n_obs; b++ )
    {
        invScale = 0.0;
        for( i = 0; i < param->N; i++ )
        {
            ALPHA(b,i) = ALPHA(b-1,i) * PROB_OBS(0,b,i);
            invScale += ALPHA(b,i);
        }

        SCALE(b) = 1.0 / invScale;
        for( i = 0; i < param->N; i++ )
            ALPHA(b,i) *= SCALE(b);
    }

    for( t = param->B; t < n_obs-(int)(param->B); t += param->B )
    {
        invScale = 0.0;
        for( i = 0; i < param->N; i++ )
        {
            sum = 0.0;
            for( j = 0; j < param->N; j++ )
                sum += ALPHA(t-1,j) * param->A[j][i];

            ALPHA(t,i) = sum * PROB_FIRST_OBS(t,i);
            invScale += ALPHA(t,i);
        }

        SCALE(t) = 1.0 / invScale;
        for( i = 0; i < param->N; i++ )
            ALPHA(t,i) *= SCALE(t);

        for( b = 1; b < param->B; b++ )
        {
            invScale = 0.0;
            for( i = 0; i < param->N; i++ )
            {
                ALPHA(t+b,i) = ALPHA(t+b-1,i) * PROB_OBS(t,b,i);
                invScale += ALPHA(t+b,i);
            }

            SCALE(t+b) = 1.0 / invScale;
            for( i = 0; i < param->N; i++ )
                ALPHA(t+b,i) *= SCALE(t+b);
        }
    }

    if( t < n_obs )
    {
        invScale = 0.0;
        for( i = 0; i < param->N; i++ )
        {
            sum = 0.0;
            for( j = 0; j < param->N; j++ )
                sum += ALPHA(t-1,j) * param->A[j][i];

            ALPHA(t,i) = sum * PROB_FIRST_OBS(t,i);
            invScale += ALPHA(t,i);
        }

        SCALE(t) = 1.0 / invScale;
        for( i = 0; i < param->N; i++ )
            ALPHA(t,i) *= SCALE(t);

        for( b = 1; b < param->B && t+b < n_obs; b++ )
        {
            invScale = 0.0;
            for( i = 0; i < param->N; i++ )
            {
                ALPHA(t+b,i) = ALPHA(t+b-1,i) * PROB_OBS(t,b,i);
                invScale += ALPHA(t+b,i);
            }

            SCALE(t+b) = 1.0 / invScale;
            for( i = 0; i < param->N; i++ )
                ALPHA(t+b,i) *= SCALE(t+b);
        }
    }

    /*********************************************************************
     *                                                                   *
     *  END of algorithm                                                 *
     *                                                                   *
     *********************************************************************/

    if( ll )
    {
        *ll = 0.0;
        for( t = 0; t < n_obs; t++ )
            *ll -= log( SCALE(t) );
    }

    if( final_pi )
    {
        for( i = 0; i < param->N; i++ )
            final_pi[i] = ALPHA(n_obs-1,i);
    }

    /*********************************************************************
     *  Cleanup                                                          *
     *********************************************************************/
    if( free_alpha )
        destroy_matrix( n_obs, param->N, alpha );

    if( free_scale )
        destroy_vector( n_obs, scale );

    return 0;
}

/*
*
* Below, there are two versions of the function "forward_test". Both have
* the same purpose, and work, basically, in the same way. They are not used
* in MTK, at this moment, because we are not certain that either one of them
* calculates, in the correct way, the burst size distribution of the model.
*
*/

/**
  Calculates the burst size acumulated probability distribution
  for the given symbol "symbol".

  Created by Edmundo Grune.
  
  Warning: needs revision.
**/
double *forward_test( Parameter * param, double * pi0,
                      int symbol,
                      int max_size, double threshold,
                      FILE * output )
{
    int i, j, b, t, n, time_step;
    int obs[] = { symbol, symbol };
    double sum;
    double invScale;
    double ac_prob, indv_prob, log_ac_prob;
    double ** alpha;
    double scale;
    double log_threshold;
    double avg;
    double *result = NULL;
    int result_size = 0;

    if( !param || !output )
        return NULL;

    log_threshold = log( threshold );

    /* alpha will represent P[ S = i | I_n = symbol, I_(n-1) = symbol, ..., I_1 = symbol ] */
    alpha = create_zero_matrix( 2, param->N );

    /* set previous state distribution */
    for( i = 0; i < param->N; i++ )
        alpha[0][i] = pi0[i];

/**************  DEBUG  *****************
fprintf( stderr, "\nAlpha Inicial: (P[ S_i | last symbol emitted was %d ] in last time_step)\n", 1-symbol );
for( i=0; i < param->N; i++ )
    fprintf( stderr, "   P[ S=%d | last symbol emitted was %d ] in last time_step) = %lf\n", i, 1-symbol, alpha[0][i] );
fprintf( stderr, "\n" );
****************************************/

    n = 1;
    avg = 0.0;
    log_ac_prob = 0.0;
    for( t = 1, time_step = 1; log_ac_prob > log_threshold && n <= max_size; time_step++, t = 1-t )
    {
        fprintf( stderr, "************************** time step = %d (n = %d) **************************\n\n", time_step, n );
        
        /* Considering first observation emitted in a batch. */
        invScale = 0.0;
        //fprintf( stderr, "\n" );
        for( i = 0; i < param->N; i++ )
        {
            /* ALPHA(t,i) = P[ S = i and I_n = symbol | I_(n-1) = symbol, ..., I_1 = symbol ] */
            ALPHA(t,i) = ALPHA(1-t,i) * PROB_FIRST_OBS( symbol, i );
            //fprintf( stderr, "P[ S_%d and I = %d ] in time_step %d = %lf * PROB_FIRST_OBS(%d,%d) = %lf\n", i, symbol, time_step, ALPHA(1-t,i), i, symbol, ALPHA(t,i) );

            /* invScale = P[ I_n = symbol | I_(n-1) = symbol, ..., I_1 = symbol ] */
            invScale += ALPHA(t,i);
        }

        //fprintf( stderr, "\nThus, P(I_%d) in time_step %d, batch step 1 = %lf\n", symbol, t, invScale );
        
        if( fabs( invScale ) < TOLERANCE ) /* A burst of this size is impossible */
        {
            fprintf( output, "  %10d %20s %20.10e\n",
                     n, "-inf", 0.0 );
            fprintf( output, "\n A burst of %d %d's is impossible\n", n, symbol );

            return NULL;
        }

        if( n == 1 )
        {
            fprintf( output, "\n" );
            fprintf( output, "# burst size distribution for %d's\n", symbol );
            fprintf( output, "# %10s %20s %20s\n",
                     "n", "log(P[ N >= n ])", "P[ N >= n ]" );

            /* P[ N >= 1 ] = 1.0 */
            ac_prob = 1.0;
            log_ac_prob = log( ac_prob );
            fprintf( output, "  %10d %20.10e %20.10e\n",
                     n++, log_ac_prob, ac_prob );
            
            /* indv_prob = P[N=1] */
            indv_prob = invScale;
            avg += indv_prob; /* Verify if this is correct */
        }
        else
        {
            /* P[ N >= n ] = P[ N >= (n-1) ] - P[ N = n-1 ] */
            fprintf( stderr, "(ac-1)%lf - (indv)%lf = ", ac_prob, indv_prob );
            ac_prob = ac_prob - indv_prob;
            fprintf( stderr, "(ac)%lf\n", ac_prob );
            log_ac_prob = log( ac_prob );            
            fprintf( output, "  %10d %20.10e %20.10e\n",
                     n++, log(ac_prob), ac_prob );
            
            indv_prob = invScale;
            avg += indv_prob; /* Verify if this is correct */
        }
                 
        /* Save result */
        result = (double *)realloc( result, ( ++result_size + 1 ) *
                                            sizeof(double ) );
        result[ result_size ] = ac_prob;

        /* scale = 1/P[I_n = symbol | I_(n-1) = symbol, ..., I_1 = symbol ] */
        scale = 1.0 / invScale;
        for( i = 0; i < param->N; i++ )
        {
            ALPHA(t,i) *= scale; /* Now, after this normalization,
                                    ALPHA(t,i) = P[ S = i | I_n = symbol, ..., I_1 = symbol ] */
            //fprintf( stderr, "P[ S_%d | I = %d ] in time_step %d = %lf\n", i, symbol, time_step, ALPHA(t,i) );
        }


        /* Calculates statistic probabilities for the rest of the batch emitted in time step t */
        for( b = 1, t = 1-t;
             b < param->B && log_ac_prob > log_threshold && n <= max_size;
             b++, t = 1-t )
        {
            //fprintf( stderr, "\n------------- batch step = %d (n = %d) (B = %d)-------\n", b+1, n, param->B );
            
            invScale = 0.0;
            //fprintf( stderr, "\n" );
            for( i = 0; i < param->N; i++ )
            {
                ALPHA(t,i) = ALPHA(1-t,i) * PROB_OBS( symbol, 1, i );
                //fprintf( stderr, "P[ S_%d and I = %d ] in time_step %d, batch step %d = %lf * %lf = %lf\n", i, symbol, time_step, b+1, ALPHA(1-t,i), PROB_OBS( symbol, 1, i ), ALPHA(t,i) );

                invScale += ALPHA(t,i);
            }

            //fprintf( stderr, "\nThus, P(I_%d) in time_step %d, batch step %d, given burst of %d %d's = %lf\n", symbol, time_step, b+1, n-1, symbol, invScale );

            if( fabs( invScale ) < TOLERANCE ) /* A burst of this size is impossible */
            {
                fprintf( output, "  %10d %20s %20.10e\n",
                         n, "-inf", 0.0 );
                fprintf( output, "\n A burst of %d %d's is impossible\n", n, symbol );

                return NULL;
            }

            /* P[ N >= n ] = P[ N >= (n-1) ] - P[ N = n-1 ] */
            fprintf( stderr, "(ac-1)%lf - (indv)%lf = ", ac_prob, indv_prob );
            ac_prob = ac_prob - indv_prob;
            fprintf( stderr, "(ac)%lf\n", ac_prob );
            log_ac_prob = log( ac_prob );            
            fprintf( output, "  %10d %20.10e %20.10e\n",
                     n++, log(ac_prob), ac_prob );
            
            indv_prob = invScale;
            avg += indv_prob; /* Verify if this is correct */

            /* Save result */
            result = (double *)realloc( result, ( ++result_size + 1 ) *
                                                sizeof(double ) );
            result[ result_size ] = ac_prob;                 

            /* scale = 1/P[I_n = symbol | I_(n-1) = symbol, ..., I_1 = symbol ] */
            scale = 1.0 / invScale;
            for( i = 0; i < param->N; i++ )
                ALPHA(t,i) *= scale;   /* Now, after this normalization,
                                          ALPHA(t,i) = P[ S = i | I_n = symbol, ..., I_1 = symbol ] */
        }

        /* Batch has ended. Now, account for hidden state transition */
        //fprintf( stderr, "\n====== Batch has ended. Calculating new hidden state distribution, given history of obs.\n" );
        invScale = 0.0;
        for( i = 0; i < param->N; i++ )
        {
            sum = 0.0;
            //fprintf( stderr, "sum = " );
            for( j = 0; j < param->N; j++ )
            {
                sum += ALPHA(1-t,j) * param->A[j][i];
                //fprintf( stderr, " (%lf x %lf) +", ALPHA(1-t,j), param->A[j][i] );
            }
            //fprintf( stderr, " = %lf\n", sum );

            /* ALPHA(t,i) = P[ S = i | I_n = symbol, ..., I_1 = symbol, S_(t-1) = j ] */
            ALPHA(t,i) = sum;
            //fprintf( stderr, "P[ S_%d | burst of %d's in time step %d ] = %lf \n\n", i, symbol, time_step, ALPHA(t,i) );
        }
        
        fprintf( stderr, "++++++++++++++++ log = %lf e n = %d +++++++++++++\n", log_ac_prob, n );
    }

    if( ac_prob < log_threshold ) /* Threshold was reached */
    {
        fprintf( output, "# Burst size threshold probability reached.\n" );
        fprintf( output, "# estimated %.0f-percentile: %d\n", 
                         ( 1 - threshold ) * 100.0, n - 1 );
    }
    else /* Maximum burst size was reached */
    {
        fprintf( output, "# Maximum specified burst size reached.\n" );
        fprintf( output, "# estimated %.0f-percentile is bigger than %d\n", 
                         ( 1 - threshold ) * 100.0, n - 1 );
    }
    
    fprintf( output, "# estimated average: %20.10e\n", avg );

    destroy_matrix( 2, param->N, alpha );
    
    result[0] = result_size;

    return result;
}

/**
  Calculates the burst size acumulated probability distribution
  for the given symbol "symbol".

  Created by Fernando Silveira.
  
  Warning: needs revision.
**/
//double *forward_test( Parameter * param, double * pi0,
//                      int symbol,
//                      int max_size, double threshold,
//                      FILE * output )
//{
//   int i, j, b, t, n;
//   int obs[] = { symbol, symbol };
//   double sum;
//   double invScale;
//   double log_probseq, old_log_probseq;
//   double ** alpha;
//   double scale;
//   double log_threshold;
//   double avg, probseq;
//   double *result = NULL;
//   int result_size = 0;
//
//   if( !param || !output )
//       return NULL;
//
//   log_threshold = log( threshold );
//
//   /* alpha = P[ S_i | I_symbol ] in t */
//   alpha = create_zero_matrix( 2, param->N );
//
//   /* set previous state distribution */
//    for( i = 0; i < param->N; i++ )
//        alpha[0][i] = pi0[i];
//
//    n = 1;
//    avg = 0.0;
//    log_probseq = 0.0;
//
//    for( t = 1; log_probseq > log_threshold && n <= max_size; )
//    {
//        fprintf( stderr, "***************** t = %d ***************\n\n", t );
//        
//        invScale = 0.0;
//        for( i = 0; i < param->N; i++ )
//         {
//             sum = 0.0;
//            /* fprintf( stderr, "sum = " ); */
//            for( j = 0; j < param->N; j++ )
//            {
//                sum += ALPHA(1-t,j) * param->A[j][i];
//                /* fprintf( stderr, " (%lf x %lf) +", ALPHA(1-t,j), param->A[j][i] ); */
//            }
//            /* fprintf( stderr, " = %lf\n", sum ); */
//
//            /* ALPHA(t,i) = P[ S_i and I_0 ] in t */
//            ALPHA(t,i) = sum * PROB_FIRST_OBS(0,i);
//            /* fprintf( stderr, "P[ S_%d and I_0 ] in %d = %lf * PROB_FIRST_OBS(0,%d) = %lf\n\n", i, t, sum, i, ALPHA(t,i) ); */
//
//            /* invScale = P[ I_0 ] in t */
//            invScale += ALPHA(t,i);
//        }
//
//        /* fprintf( stderr, "Thus, P(I_0) in %d = %lf\n", t, invScale ); */
//        scale = 1.0 / invScale;
//        for( i = 0; i < param->N; i++ )
//        {
//            ALPHA(t,i) *= scale; /* Now, after this normalization,
//                                    ALPHA(t,i) = P[ S_i | I_0 ] in t. */
//            /* fprintf( stderr, "P[ S_%d | I = 0 ] in %d = %lf\n", i, t, ALPHA(t,i) ); */
//        }
//
//        old_log_probseq = log_probseq;
//        /* fprintf( stderr, "\n log_probseq = %lf - %lf\n", log_probseq, log( scale ) ); */
//        log_probseq -= log( scale ); /* -log(1/A) = -(log(1) - log(A)) = log(A).
//                                        In this case, the sequence probability
//                                       is old_prob + P[I_0] in t. */
//
//       if( n == 1 )
//       {
//           /* print header */
//           fprintf( output, "# probability of no loss %.10e\n", 
//                            1 - exp( log_probseq ) );
//
//
//           /* Save result */
//           result = (double *)realloc( result, ( ++result_size + 1 ) *
//                                               sizeof(double ) );
//           result[ result_size ] = 1 - exp( log_probseq );
//
//           fprintf( output, "# burst size distribution for %d's\n", symbol );
//           fprintf( output, "# %10s %20s %20s\n",
//                    "n", "log(P[ N >= n ])", "P[ N >= n ]" );
//
//           log_probseq = 0.0;
//       }
//
//       probseq = exp( log_probseq );
//       avg += probseq;
//       fprintf( output, "  %10d %20.10e %20.10e\n",
//                n++, log_probseq, probseq );
//                
//       /* Save result */
//       result = (double *)realloc( result, ( ++result_size + 1 ) *
//                                           sizeof(double ) );
//       result[ result_size ] = probseq;                 
//
//       /* Calculates probabilities for a same batch */
//       for( b = 1, t = 1-t;
//            b < param->B && log_probseq > log_threshold && n <= max_size;
//            b++, t = 1-t )
//       {
//           invScale = 0.0;
//
//           for( i = 0; i < param->N; i++ )
//           {
//               ALPHA(t,i) = ALPHA(1-t,i) * PROB_OBS(0,1,i);
//
//               invScale += ALPHA(t,i);
//           }
//
//           scale = 1.0 / invScale;
//           for( i = 0; i < param->N; i++ )
//                ALPHA(t,i) *= scale;
//
//            old_log_probseq = log_probseq;
//            log_probseq -= log( scale );
//
//            probseq = exp( log_probseq );
//            avg += probseq;
//            fprintf( output, "  %10d %20.10e %20.10e\n",
//                     n++, log_probseq, probseq );
//
//            /* Save result */
//            result = (double *)realloc( result, ( ++result_size + 1 ) *
//                                                sizeof(double ) );
//           result[ result_size ] = probseq;                 
//       }
//   }
//
//   fprintf( output, "# estimated %.0f-percentile: %d\n", 
//                    ( 1 - threshold ) * 100.0, n - 1 );
//   fprintf( output, "# estimated average: %20.10e\n", avg );
//
//   destroy_matrix( 2, param->N, alpha );
//   
//   result[0] = result_size;
//
//   return result;
//}
