#include "transient.h"

#include <math.h>
#include <stdlib.h>

#include "forward.h"
#include "matrix.h"
#include "tangram2.h"

double *state_probability( Parameter *param,
                           int n_obs, int *obs,
                           FILE *output )
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
