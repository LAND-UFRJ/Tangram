#include "transient.h"

#include <math.h>
#include <stdlib.h>

#include "forward.h"
#include "matrix.h"
#include "tangram2.h"

static void step_probability( Parameter * param, double * pi0, double * pi1 )
{
    int i, j;

    for( i = 0; i < param->N; i++ )
    {
        pi0[i] = 0.0;
        for( j = 0; j < param->N; j++ )
        {
            pi0[i] += pi1[j] * param->A[j][i];
        }
    }
}

double *symbol_sum_distribution( Parameter * param, int f, int n_obs, int * obs,
                                 FILE * output )
{
    int i, j, k, x;
    double **B, **R, **M;
    double *pi_0, *pi_t, *r;
    double avg;
    double *result;
    int result_index;

    /* get pi after history */
    pi_0 = create_zero_vector( param->N );
    pi_t = create_zero_vector( param->N );
    
    /* Obtaining last known state distribution */
    for( i = 0; i < param->N; i++ )
        pi_0[i] = param->gamma[i];

    if( forward_filter( param, pi_0, n_obs, obs, 0, 0, 0, pi_t ) < 0 )
    {
        destroy_vector( param->N, pi_0 );
        destroy_vector( param->N, pi_t );
        return NULL;
    }

    for( i = 0; i < param->N; i++ )
    {
        if( isnan( pi_t[i] ) )
        {
            for( i = 0; i < param->N; i++ )
                pi_t[i] = pi_0[i];
            break;
        }
    }

    /* pi_0 = pi_t A */
    step_probability( param, pi_0, pi_t );

    /* get distribution conditioned on state */
    B = create_zero_matrix( param->N, param->B+1 );
    R = create_zero_matrix( param->N, f*param->B+1 );
    M = create_zero_matrix( param->N, f*param->B+1 );

    /* B */
    get_aggregate_matrix( param, B );

    /* R = B */
    for( i = 0; i < param->N; i++ )
        for( j = 0; j <= param->B; j++ )
            R[i][j] = B[i][j];

    for( k = 2; k <= f; k++ )
    {
        /* M = A R_{k-1} */
        for( i = 0; i < param->N; i++ )
            for( x = 0; x <= (k-1)*param->B; x++ )
            {
                M[i][x] = 0.0;
                for( j = 0; j < param->N; j++ )
                    M[i][x] += param->A[i][j] * R[j][x];
            }

        /* R_k = sum_x  */
        for( i = 0; i < param->N; i++ )
            for( j = 0; j <= k*param->B; j++ )
            {
                R[i][j] = 0.0;
                for( x = 0; x <= param->B && x <= j; x++ )
                    R[i][j] += M[i][j-x] * B[i][x];
            }
    }

    r = create_zero_vector( f*param->B+1 );

    for( i = 0; i < param->N; i++ )
    {
        for( j = 0; j <= f*param->B; j++ )
            r[j] += pi_0[i] * R[i][j];
    }

    /* Prepare result */
    result_index = 0;
    result = (double *)malloc( ( f * param->B + 3 ) * sizeof( double ) );
    /* Note: result[0] represents vector size */
    result[ result_index++ ] = f * param->B + 1;

    fprintf( output, "# %8s %13s %d %11s\n", "sum(Obs)", "P[sum(Obs) after", f, 
                     "time steps]" );

    avg = 0.0;
    for( j = 0; j <= f*param->B; j++ )
    {
        avg += j*r[j];
        fprintf( output, "  %4d %27.10e\n", j, r[j] );
        result[ result_index++ ] = r[j];
    }
    fprintf( output, "\n# estimated average of sum(Obs): %10.10e\n", avg );
    result[ result_index++ ] = avg;

    destroy_matrix( param->N, param->B+1, B );
    destroy_matrix( param->N, f*param->B+1, R );
    destroy_matrix( param->N, f*param->B+1, M );

    return result;
}

double *tr_burst_size( Parameter * param, int symbol,
                       int n_obs, int * obs,
                       int max_size, double threshold,
                       FILE * output )
{
    double * pi_0,
           * pi_t;
    double *result;
    int i;

    pi_0 = create_zero_vector( param->N );
    pi_t = create_zero_vector( param->N );
   
    /* Obtaining last known state distribution */
    for( i = 0; i < param->N; i++ )
        pi_0[i] = param->gamma[i];

    if( forward_filter( param, pi_0, n_obs, obs, 0, 0, 0, pi_t ) < 0 )
    {
        destroy_vector( param->N, pi_0 );
        destroy_vector( param->N, pi_t );
        return NULL;
    }

    /* pi_0 = pi_t A */
    step_probability( param, pi_0, pi_t );

    result = forward_test( param, pi_0, symbol, max_size, threshold, output );
    if( !result )
    {
        destroy_vector( param->N, pi_0 );
        destroy_vector( param->N, pi_t );
        return NULL;
    }

    destroy_vector( param->N, pi_0 );
    destroy_vector( param->N, pi_t );

    return result;
}

double *state_probability( Parameter *param,
                           int n_obs, int *obs,
                           FILE * output )
{
    double * pi_0,
           * pi_t;
    int i;
    double *result;

    pi_0 = create_zero_vector( param->N );

    /* Obtaining last known state distribution */
    for( i = 0; i < param->N; i++ )
        pi_0[i] = param->gamma[i];

    if( obs )
    {
        pi_t = create_zero_vector( param->N );

        if( forward_filter( param, pi_0, n_obs, obs, 0, 0, 0, pi_t ) < 0 )
        {
            destroy_vector( param->N, pi_0 );
            destroy_vector( param->N, pi_t );
            return NULL;
        }
    }
    else
        pi_t = pi_0;

    // Save result
    result = (double *)malloc( ( param->N + 1 ) * sizeof( double ) );
    result[0] = param->N;

    for( i = 0; i < param->N; i++ )
    {
        fprintf( output, "%2d: [ %.5e ]\n", i+1, pi_t[i] );
        result[ i + 1 ] = pi_t[i];
    }

    destroy_vector( param->N, pi_0 );
    if( obs )
        destroy_vector( param->N, pi_t );

    return result;
}
