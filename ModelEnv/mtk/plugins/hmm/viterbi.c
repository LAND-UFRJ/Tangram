#include "viterbi.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

double *viterbi( Parameter *param,
                 int n_obs, int *obs,
                 int *states, FILE * out )
{
    int i, j, t;
    int argmax;
    double max, deltaaux;
    double ** delta;
    int ** psi;
    double *result;    

    if( param->N < 1 || param->M < 1 )
    {
        if( out )
            fprintf( out, "invalid model dimensions" );
        return NULL;
    }

    delta = (double **)malloc( sizeof(double *) * n_obs );
    psi   = (int **)malloc( sizeof(int *) * n_obs );
    for( i = 0; i < n_obs; i++ )
    {
        psi[ i ] = (int *)malloc( sizeof(int) * param->N ); /* Era unsigned int, o que provocava um warning: pointer targets in assignment differ in signedness */
        delta[ i ] = (double *)malloc( sizeof(double) * param->N );
    }

    for( i = 0; i < param->N; i++ )
    {
        psi[0][i]   = 0;
        delta[0][i] = log( param->pi[i] ) + log( param->B[i][obs[0]] );
    }

    for( t = 1; t < n_obs; t++ )
    {
        for( j = 0; j < param->N; j++ )
        {
            max    = -100000.0;
            argmax = 0;
            for( i = 0; i < param->N; i++ )
            {
                deltaaux = delta[t-1][i] + log( param->A[i][j] );
                if( deltaaux > max )
                {
                    max    = deltaaux;
                    argmax = i;
                }
            }
            delta[t][j] = max + log( param->B[j][obs[t]] );
            psi[t][j]   = argmax;
        }
    }

    max = -100000.0;
    for( i = 0; i < param->N; i++ )
    {
        if( delta[n_obs-1][i] > max )
        {
            max = delta[n_obs-1][i];
            states[n_obs-1] = i;
        }
    }

    for( t = n_obs - 2; t >= 0; t-- )
        states[t] = psi[t+1][states[t+1]];

    if( out )
        fprintf( out, "Initial state = %d\tLast state = %d\t\tScore = %e\n",
                 states[0], states[n_obs-1], max );

    for( i = 0; i < n_obs; i++ )
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
