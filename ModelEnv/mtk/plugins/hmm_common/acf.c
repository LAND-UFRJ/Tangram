#include "acf.h"

#include <math.h>

#include "matrix.h"
#include "tangram2.h"

int loss_autocorrelation( Parameter * param,
                          double theta, int max_samples,
                          FILE * output )
{
    double * pi, * old_pi;
    double ** pi_matrix, ** old_pi_matrix;
    double R, acf, p;
    int i, j, k, h, continue_flag;
    char header[32];

    pi_matrix     = create_zero_matrix( param->B, param->N );
    old_pi_matrix = create_zero_matrix( param->B, param->N );

    pi = pi_matrix[0];
    if( get_steady_state_prob( param->N, param->A, pi ) < 0 )
    {
        fprintf( output, "# could not calculate steady state probabilities\n" );
        destroy_matrix( param->B, param->N, pi_matrix );
        destroy_matrix( param->B, param->N, old_pi_matrix );
        return -1;
    }

    /* calculate average loss rate */
    R = 0.0;
    for( i = 0; i < param->N; i++ )
        R += pi[i] * param->p[i];

    /* calculate initial state distribution given packet loss */
    for( i = 0; i < param->N; i++ )
        old_pi[i] = param->p[i] * pi[i] / R;

    /* copy change to other time instants */
    for( j = 1; j < param->B; j++ )
        for( i = 0; i < param->N; i++ )
            old_pi_matrix[j][i] = old_pi[i];


    /* print first two lines */
    fprintf( output, "# %10s", "h" );
    for( i = 0; i < param->B; i++ )
    {
        sprintf( header, "rho(%d,h)", i+1 );
        fprintf( output, " %20s", header );
    }
    fprintf( output, "\n" );

    fprintf( output, "  %10d", 0 );
    for( i = 0; i < param->B; i++ )
        fprintf( output, " %20.10e", 1.0 );
    fprintf( output, "\n" );

    /* generate autocorrelation samples */
    for( h = 1; h <= max_samples; h++ )
    {
        continue_flag = 0;

        fprintf( output, "  %10d", h );

        for( k = 0; k < param->B; k++ )
        {
            pi = pi_matrix[k];
            old_pi = old_pi_matrix[k];

            if( (k+h) % param->B == 0 )
            {
                for( i = 0; i < param->N; i++ )
                {
                    pi[i] = 0.0;
                    for( j = 0; j < param->N; j++ )
                        pi[i] += old_pi[j] * param->A[j][i];
                }
            }
            else
            {
                for( i = 0; i < param->N; i++ )
                    pi[i] = old_pi[i];
            }

            p = 0.0;
            for( i = 0; i < param->N; i++ )
                p += pi[i] * param->p[i];

            acf = (p-R)/(1.0-R);

            fprintf( output, " %20.10e", acf );

            if( fabs(acf) >= theta )
                continue_flag = 1;

            for( i = 0; i < param->N; i++ )
                old_pi[i] = pi[i];
        }
        fprintf( output, "\n" );

        if( !continue_flag )
            break;
    }

    destroy_matrix( param->B, param->N, pi_matrix );
    destroy_matrix( param->B, param->N, old_pi_matrix );

    return 0;
}
