#include "forward.h"

#include <math.h>

#include "matrix.h"
#include "batches.h"

/*****************************************************************************/
/* Calculates the probability of the partial observation sequence from the   */
/* beginning to time t and state Sj at time t, given the model.              */
/*****************************************************************************/
int forward_filter( Parameter * param, double * pi0,
                    int n_obs, int * obs,
                    double ** alpha, double * scale,
                    double * ll, double * final_pi )
{
    int i, j, b, t, nb;
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

    /* First observation */
    SCALE(0) = 1.0 / invScale;
    for( i = 0; i < param->N; i++ )
    {
        ALPHA(0,i) *= SCALE(0);
    }

    /* First batch */
    nb = 0;
    for( b = 1; b < batches[nb].size; b++ )
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

    /* From second to the last batch */
    for( nb++; nb < num_batches; nb++ )
    {
        t = batches[nb].index;

        /* First observation within this batch */
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

        /* From second to last observation within this batch  */
        for( b = 1; b < batches[nb].size; b++ )
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
        {
            double l = log( SCALE(t) );

            if( isnan( l ) )
                l = 0.0;

            *ll -= l;
        }
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

