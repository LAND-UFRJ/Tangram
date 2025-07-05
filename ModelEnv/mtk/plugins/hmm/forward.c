#include "forward.h"

#include <math.h>

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
    int i, j, t;
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
        ALPHA(0,i) = pi0[i] * param->B[i][obs[0]];
        invScale += ALPHA(0,i);
    }
    SCALE(0) = 1.0 / invScale;

    for( i = 0; i < param->N; i++ )
        ALPHA(0,i) *= SCALE(0);

    for( t = 0; t < n_obs - 1; t++ )
    {
        invScale = 0.0;
        for( j = 0; j < param->N; j++ )
        {
            sum = 0.0;
            for( i = 0; i < param->N; i++ )
                sum += ALPHA(t,i) * param->A[i][j];

            ALPHA(t+1,j) = sum * param->B[j][obs[t+1]];
            invScale += ALPHA(t+1,j);
        }
        SCALE(t+1) = 1.0 / invScale;

        for( i = 0; i < param->N; i++ )
            ALPHA(t+1,i) *= SCALE(t+1);
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
