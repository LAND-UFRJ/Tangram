#include "viterbi.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/*****************************************************************************/
/* Calculates the probability of observing a batch with parameters r, p and q*/
/*****************************************************************************/
static double Eval_LogProbObs( Parameter * param, int state, int * obs,
                               int n_obs )
{
    int i, j;
    double ProbObs, **l;

    l = (double **)malloc( sizeof(double *) * param->M );
    for( i = 0; i < param->M; i++ )
    {
        l[ i ] = (double *)malloc( sizeof(double) * param->M );
    }

    /* initial state - assume equiprobable */
    ProbObs = log( param->r[state][obs[0]] );

    for( i = 0; i < param->M; i++ )
    { 
        for( j = 0; j < param->M; j++ )
        {
            l[i][j]  = log( param->p[state][i][j] );
        }
    }

    for( i = 1; i < n_obs; i++ )
    {
        ProbObs += l[obs[i-1]][obs[i]];
    }

    for( i = 0; i < param->M; i++ )
    {
        free( l[i] );
    }

    free( l );

    return ProbObs;
}

double *viterbi( Parameter *param,
                 int n_obs,
                 int *obs,
                 int *states,
                 FILE * out )
{
    int i, j, t, nb, offset, size;
    int argmax;
    double max, deltaaux;
    double ** delta;
    int ** psi;
    double *result;    

    if( param->N < 1 || param->B < 1 || param->M < 1 )
    {
        fprintf( out, "invalid model dimensions" );
        return NULL;
    }

    nb = n_obs / param->B;
    if( n_obs % param->B > 0 )
        nb++;

    delta = (double **)malloc( sizeof(double *) * nb );
    psi   = (int **)malloc( sizeof(int *) * nb );
    for( i = 0; i < nb; i++ )
    {
        psi[ i ] = (int *)malloc( sizeof(int) * param->N ); /* Era unsigned int, o que provocava waring. */
        delta[ i ] = (double *)malloc( sizeof(double) * param->N );
    }

    if( nb == 1 && (n_obs % param->B) > 0 )
        size = n_obs % param->B;
    else
        size = param->B;

    for( i = 0; i < param->N; i++ )
    {
        psi[0][i]   = 0.0;
        delta[0][i] = log( param->pi[i] ) +
                      Eval_LogProbObs( param, i, obs, size );
    }

    offset = param->B;
    for( t = 1; t < nb; t++, offset += param->B )
    {
        if( t == (nb-1) && (n_obs % param->B) > 0 )
            size = n_obs % param->B;

        for( j = 0; j < param->N; j++ )
        {
            for( i = 0; i < param->N; i++ )
            {
                deltaaux = delta[t-1][i] + log( param->A[i][j] );
                if( deltaaux > max || i == 0 )
                {
                    max    = deltaaux;
                    argmax = i;
                }
            }
            delta[t][j] = max + 
                          Eval_LogProbObs( param, j, obs+offset, size );
            psi[t][j]   = argmax;
        }
    }

    for( i = 0; i < param->N; i++ )
    {
        if( delta[nb-1][i] > max || i == 0 )
        {
            max = delta[nb-1][i];
            states[nb-1] = i;
        }
    }

    for( t = nb - 2; t >= 0; t-- )
        states[t] = psi[t+1][states[t+1]];

    fprintf( out, "Initial state = %d\tLast state = %d\t\tScore = %e\n",
             states[0], states[nb-1], max );

    for( i = 0; i < nb; i++ )
    {
        free( psi[i] );
        free( delta[i] );
    }

    free( psi );
    free( delta );

    result = (double *)malloc( sizeof( double ) );
    result[0] = max;

    return result;
}
