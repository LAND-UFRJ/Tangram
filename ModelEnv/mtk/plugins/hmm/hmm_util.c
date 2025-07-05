#include "hmm_util.h"

#include <math.h>
#include <stdio.h>

/*****************************************************************************/
/* Calculates the probability of the partial observation sequence from the   */
/* beginning to time t and state Sj at time t, given the model.              */
/*****************************************************************************/
double Eval_Alpha( Parameter * param, int * obs, int n_obs,
                   double ** alpha, double * scale )
{
    int i, j, t;
    double sum;
    double invScale;

    invScale = 0.0;
    for( i = 0; i < param->N; i++ )
    {
        alpha[0][i] = param->pi[i] * param->B[i][obs[0]];
        invScale += alpha[0][i];
    }
    scale[0] = 1.0 / invScale;

    for( i = 0; i < param->N; i++ )
        alpha[0][i] *= scale[0];

    for( t = 0; t < n_obs - 1; t++ )
    {
        invScale = 0.0;
        for( j = 0; j < param->N; j++ )
        {
            sum = 0.0;
            for( i = 0; i < param->N; i++ )
                sum += alpha[t][i] * param->A[i][j];

            alpha[t+1][j] = sum * param->B[j][obs[t+1]];
            invScale += alpha[t+1][j];
        }
        scale[t+1] = 1.0 / invScale;

        for( i = 0; i < param->N; i++ )
            alpha[t+1][i] *= scale[t+1];
    }

    sum = 0.0;
    for( i = 0; i < param->N; i++ )
        sum += alpha[n_obs-1][i];

    return( sum );
}

/*****************************************************************************/
/* Updates the probability of the partial observation sequence from the      */
/* beginning to time t and state Sj at time t, given the model.              */
/*****************************************************************************/
double Update_Alpha( Parameter * param, int * obs, int n_obs, int begin,
                     double ** alpha, double * scale )
{
    int i, j, t;
    double sum;
    double invScale;

    for( t = begin-1; t < n_obs - 1; t++ )
    {
        invScale = 0.0;
        for( j = 0; j < param->N; j++ )
        {
            sum = 0.0;
            for( i = 0; i < param->N; i++ )
                sum += alpha[t][i] * param->A[i][j];

            alpha[t+1][j] = sum * param->B[j][obs[t+1]];
            invScale += alpha[t+1][j];
        }
        scale[t+1] = 1.0 / invScale;

        for( i = 0; i < param->N; i++ )
            alpha[t+1][i] *= scale[t+1];
    }

    sum = 0.0;
    for( i = 0; i < param->N; i++ )
        sum += alpha[n_obs-1][i];

    return( sum );
}

/*****************************************************************************/
/* Calculates the probability of the partial observation sequence from t+1   */
/* to the end, given the state Si at time t and the model.                   */
/*****************************************************************************/
void Eval_Beta( Parameter * param, int * obs, int n_obs,
                double ** beta, double * scale )
{
    int i, j, t;

    for( i = 0; i < param->N; i++ )
        beta[n_obs-1][i] = scale[n_obs-1];

    for( t = n_obs - 2; t >= 0; t-- )
    {
        for( i = 0; i < param->N; i++ )
        {
            beta[t][i] = 0.0;
            for( j = 0; j < param->N; j++ )
                beta[t][i] += param->A[i][j] * beta[t+1][j] *
                              param->B[j][obs[t+1]];
            beta[t][i] *= scale[t];
        }
    }
}

double Eval_LogProbSeq( int n_obs, double * scale )
{
    int i;
    double sum;

    sum = 0.0;
    for( i = 0; i < n_obs; i++ )
        sum -= log( scale[ i ] );

    return sum;
}
