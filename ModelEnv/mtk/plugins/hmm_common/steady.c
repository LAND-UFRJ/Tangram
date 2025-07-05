#include "steady.h"

#include <stdlib.h>
#include <math.h>

#include "tangram2.h"
#include "matrix.h"
#include "forward.h"

double sym_time_average( Parameter * param, FILE * output )
{
    double * pi;
    double s_tavg;
    int i;

    pi = create_zero_vector( param->N );

    if( get_steady_state_prob( param->N, param->A, pi ) < 0 )
    {
        fprintf( output, "# could not calculate steady state probabilities\n" );
        destroy_vector( param->N, pi );
        return -1;
    }

    s_tavg = 0.0;
    for( i = 0; i < param->N; i++ )
        s_tavg += pi[i] * BATCH_LOSS_RATE( i );

    fprintf( output, "# Symbol Emission Time Average (when t tends to infinity)\n" );
    fprintf( output, "%.10e\n", s_tavg );

    destroy_vector( param->N, pi );
    
    return s_tavg;
}

/*
*
* Below, there are two versions of the function "ss_burst_size". Both have
* the same purpose, and work, basically, in the same way. They are not used
* in MTK, at this moment, because we are not certain that either one of them
* calculates, in the correct way, the burst size distribution of the model.
*
*/

/**
  Calculates the burst size acumulated probability distribution
  for the given symbol "symbol".

  Created by Edmundo Grune
  
  Warning: needs revision.
**/
double *ss_burst_size( Parameter *param,
                       int symbol, double threshold,
                       int max_samples, FILE *output )
{
    int i, j;
    double *pi0, *temp, P, p, sum;
    double *result;

    fprintf( stderr, "Vou analisar batches de %d's\n", symbol );
    
    pi0  = create_zero_vector( param->N );
    temp = create_zero_vector( param->N );

    /* Get steady hidden state probabilities */
    if( get_steady_state_prob( param->N, param->A, pi0 ) < 0 )
    {
        fprintf( output, "# could not calculate steady state probabilities\n" );
        destroy_vector( param->N, pi0 );
        return NULL;
    }

    /* Adjust probabilities to reflect the condition on the previous symbol */
    P = 0.0;
    for( i = 0; i < param->N; i++ )
    {
        /* p = P[ I_B = 0 | S = i ] after B emissions in hidden state i */
        p = LOSS_RATE( i, param->B-1 );

        /* pi0 = P[ S = i and I_B = (1 - symbol) ] */
        pi0[i] = pi0[i] * ( symbol ? p : 1 - p ); /* was "symbol ? 1 - p : p" */
        P += pi0[i];
    }
    /* After normilization, pi0 = P[ S = i | I_B = (1 - symbol) ] */
    fprintf( stderr, "pi0 = P[ S = i | I_B = (1 - symbol) = %d ]\n", 1-symbol );
    for( i = 0; i < param->N; i++ )
    {
        pi0[i] /= P;

        fprintf( stderr, "pi0[ %d ] = %lf\n", i, pi0[i] );
    }
    fprintf( stderr, "****************\n" );

    /* Account for the hidden transition made after the last emission */
    for( i = 0; i < param->N; i++ )
    {
        sum = 0.0;
        for( j = 0; j < param->N; j++ )
        {
            /* P[ S_t = i, S_(t-1) = j | I_B = (1 - symbol) ] = P[ S_(t-1) = j | I_B = (1 - symbol) ] * P[ S_t = i | S_(t-1) = j ] */
            sum += pi0[j] * param->A[j][i];
            fprintf( stderr, "P[ %d, %d ] = %lf * %lf\n", j, i, pi0[j], param->A[j][i] );
        }
        /* temp[i] = P[ S_t = i | I_b = (1 - symbol) ] */
        temp[i] = sum;
        fprintf( stderr, "P[ S=%d | last symbol emitted was %d ] = %lf \n\n", i, 1-symbol, temp[i] );
    }
    for( i = 0; i < param->N; i++ )
        pi0[i] = temp[i];

    /* calculate burst size distribution using pi0 */
    result = forward_test( param, pi0, symbol, max_samples, threshold, output );

    if( !result )
    {
        destroy_vector( param->N, pi0 );
        return NULL;
    }

    destroy_vector( param->N, pi0 );

    return result;
}

/**
  Calculates the burst size acumulated probability distribution
  for the given symbol "symbol".

  Created by Fernando Silveira.
  
  Warning: needs revision.
**/
//double *ss_burst_size( Parameter *param,
//                       int symbol, double threshold,
//                       int max_samples, FILE *output )
//{
//    int i;
//    double *pi0, R, r;
//    double *result;
//
//    pi0 = create_zero_vector( param->N );
//
//    /* Get steady hidden state probabilities */
//    if( get_steady_state_prob( param->N, param->A, pi0 ) < 0 )
//    {
//        fprintf( output, "# could not calculate steady state probabilities\n" );
//        destroy_vector( param->N, pi0 );
//        return NULL;
//    }
//
//    /* Adjust probabilities to reflect the condition on the previous symbol */
//    R = 0.0;
//    for( i = 0; i < param->N; i++ )
//    {
//        /* r = P[ I_0 | S_i] after B emissions */
//        r = LOSS_RATE( i, param->B-1 );
//
//        /* pi0 = P[ S_i and I_symbol ] */
//        pi0[i] = pi0[i] * ( symbol ? 1 - r : r );
//        R += pi0[i];
//    }
//    /* After normilization, pi0 = P[ S_i | I_symbol] */
//    for( i = 0; i < param->N; i++ )
//        pi0[i] /= R;
//
//    /* calculate burst size distribution using pi0 */
//    result = forward_test( param, pi0, symbol, max_samples, threshold, output );
//
//     if( !result )
//     {
//         destroy_vector( param->N, pi0 );
//         return NULL;
//     }
//
//    destroy_vector( param->N, pi0 );
//
//    return result;
//}
