#include "backward.h"

#include "batches.h"

/*****************************************************************************/
/* Calculates the probability of the partial observation sequence from t+1   */
/* to the end, given the state Si at time t and the model.                   */
/*****************************************************************************/
int backward_filter( Parameter * param,
                     int n_obs, int * obs,
                     double * scale,
                     double ** beta )
{
    int i, j, t, b, size, nb;

    /* Last batch */
    nb = num_batches-1;

    /* Determine the size of the last batch */
    size = batches[nb].size;

    /* Determine the start of the last batch */
    t = batches[nb].index;
    b = size-1;

    for( i = 0; i < param->N; i++ )
        BETA(t+b,i) = SCALE(t+b);

    /* handle last (and possibly incomplete) batch */
    for( b = size-2; b >= 0; b-- )
    {
        for( i = 0; i < param->N; i++ )
        {
            BETA(t+b,i) = PROB_OBS(t,b+1,i) * BETA(t+b+1,i);
            BETA(t+b,i) *= SCALE(t+b);
        }
    }

    for( nb--; nb >= 0; nb-- )
    {
        // batch size
        t = batches[nb].index;
        b = batches[nb].size - 1;

        // Last observation within this batch
        for( i = 0; i < param->N; i++ )
        {
            BETA(t+b,i) = 0.0;
            for( j = 0; j < param->N; j++ )
            {
                BETA(t+b,i) += param->A[i][j] * BETA(t+b+1,j) *
                               PROB_FIRST_OBS(t+b+1,j);
            }
            BETA(t+b,i) *= SCALE(t+b);
        }

        // Another observations
        for( b--; b >= 0; b-- )
        {
            for( i = 0; i < param->N; i++ )
            {
                BETA(t+b,i) = PROB_OBS(t,b+1,i) * BETA(t+b+1,i);
                BETA(t+b,i) *= SCALE(t+b);
            }
        }
    }

    return 0;
}
