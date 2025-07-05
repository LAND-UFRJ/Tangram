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
    double ** loss_rate, * loss_avg, * loss_std, acf, p;
    int i, j, k, h, continue_flag;
    char header[32];

    pi_matrix     = create_zero_matrix( param->B, param->N );
    old_pi_matrix = create_zero_matrix( param->B, param->N );

    loss_rate = create_zero_matrix( param->N, param->B );
    loss_avg = create_zero_vector( param->B );
    loss_std = create_zero_vector( param->B );

    pi = pi_matrix[0];
    old_pi = old_pi_matrix[0];
    if( get_steady_state_prob( param->N, param->A, old_pi ) < 0 )
    {
        fprintf( output, "# could not calculate steady state probabilities\n" );
        destroy_matrix( param->B, param->N, pi_matrix );
        destroy_matrix( param->B, param->N, old_pi_matrix );
        return -1;
    }

    /* copy to other time instants */
    for( j = 1; j < param->B; j++ )
        for( i = 0; i < param->N; i++ )
            old_pi_matrix[j][i] = old_pi[i];


    /* calculate loss rate for each state */
    for( i = 0; i < param->N; i++ )
        batch_loss( param, loss_rate[i], i );

    /* calculate average and stddev of loss rate at each time instant */
    for( j = 0; j < param->B; j++ )
    {
        loss_avg[j] = 0.0;
        for( i = 0; i < param->N; i++ )
            loss_avg[j] += old_pi_matrix[j][i] * loss_rate[i][j];

        loss_std[j] = sqrt( loss_avg[j] * (1.0 - loss_avg[j]) );
    }

    /* calculate initial state distribution given packet loss */
    for( j = 0; j < param->B; j++ )
    {
        for( i = 0; i < param->N; i++ )
        {
            old_pi_matrix[j][i] = loss_rate[i][j] * old_pi_matrix[j][i] /
                                  loss_avg[j];
        }
    }

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
                p += pi[i] * loss_rate[i][(k+h) % param->B];

            acf = loss_avg[k] * (p - loss_avg[(k+h) % param->B]) /
                   ( loss_std[k] * loss_std[(k+h) % param->B] );

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

    destroy_matrix( param->N, param->B, loss_rate );
    destroy_vector( param->B, loss_avg );
    destroy_vector( param->B, loss_std );

    return 0;
}
