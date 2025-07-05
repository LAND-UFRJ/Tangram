#include "train.h"

#include <math.h>
#include <stdlib.h>

#include "matrix.h"
#include "forward.h"
#include "backward.h"

#define BATCH(t) ((t)*param->B)
#define  IDX(t,b) (BATCH(t)+(b))

typedef struct
{
    int first; /* first symbol in the batch */
    int **n; /* the number of transitions from i to j within a batch */
} ObsStats;

/*****************************************************************************/
/* Training flag used to interrupt the train() routine.                      */
/*****************************************************************************/
int training_flag = 0;

/*****************************************************************************/
/* Calculates the probability of being in state Si at time t and state Sj at */
/* time t+1, given the model and the observation sequence.                   */
/* This is a define because it is much faster than a function call.          */
/*****************************************************************************/
#define Eval_Csi( param, obs, t, i, j ) \
     (ALPHA(IDX(t,param->B-1),i) * param->A[i][j] * \
      PROB_FIRST_OBS(BATCH(t+1),j) * \
      BETA(IDX(t+1,0),j))

/*****************************************************************************/
/* Calculates the probability of being in state Si at time t given the model.*/
/*****************************************************************************/
#define Eval_Gamma( param, t, i ) \
     (ALPHA(IDX(t,param->B-1),i) * BETA(IDX(t,param->B-1),i) / \
      SCALE(IDX(t,param->B-1)))

static void fixMatrix( Parameter * param )
{
    int i, j, k, some_r_nan, some_p_nan;

    some_r_nan = some_p_nan = 0;
    for( i = 0; i < param->N; i++ )
    {
        // A
        for( j = 0; j < param->N; j++ )
        {
            if( isnan( param->A[i][j] ) )
            {
                param->A[i][j] = 0.0;
            }
        }
	
        // r
        for( j = 0; j < param->M && some_r_nan == 0; j++ )
        {
            if( isnan( param->r[i][j] ) )
            {
                some_r_nan = 1;
                break;
            }
        }

        // p
        for( j = 0; j < param->M && some_p_nan == 0; j++ )
        {
            for( k = 0; k < param->M; k++ )
            {
                if( isnan( param->p[i][j][k] ) )
                {
                    some_p_nan = 1;
                    break;
                }
            }
        }
    }

    // Fixing r
    if( some_r_nan || some_p_nan )
    {
        for( i = 0; i < param->N; i++ )
        {
            // r
            for( j = 0; j < param->M && some_r_nan == 1; 
                 j++ )
            {
                param->r[i][j] = (double) 1 / param->M;
            }

            // p
            for( j = 0; j < param->M && some_p_nan == 1; 
                 j++ )
            {
                for( k = 0; k < param->M; k++ )
                {
                    param->p[i][j][k] = (double) 1 / param->M;
                }
            }
        }
    }

}

/*****************************************************************************/
/* Calculates the probability of observing a batch with parameters r and p   */
/* for the i-th state                                                        */
/*****************************************************************************/
static double Eval_LogProbObs( Parameter * param, int state,
                               ObsStats * obsstats )
{
    double sum;
    unsigned int i, j;

    sum = 0;

    sum += log( param->r[state][obsstats->first] );

    for( i = 0; i < param->M; i++ )
    {
        for( j = 0; j < param->M; j++ )
        {
            sum += obsstats->n[i][j] * log( param->p[state][i][j] );
        }
    }

    return sum;
}

static void measure_fractions( Parameter * param, int * obs, int n_obs, ObsStats * obsstats )
{
    unsigned int i, j;

    obsstats->first = obs[0];

    for( i = 0; i < param->M; i++ )
    {
        for( j = 0; j < param->M; j++ )
        {
            obsstats->n[i][j] = 0;
        }
    }

    for( i = 1; i < n_obs; i++ )
    {
        obsstats->n[obs[i-1]][obs[i]]++;
    }
}

double *likelihood( Parameter * param, int n_obs, int * obs, FILE * output )
{
    double LogProbSeq;
    double *result;    

    if( param->N < 1 || param->B < 1  || param->M < 1 )
    {
        fprintf( output, "invalid model dimensions" );
        return NULL;
    }

    forward_filter( param, 0, n_obs, obs, 0, 0, &LogProbSeq, 0 );

    fprintf( output, "log-likelihood = %.10e\tlikelihood = %.10e\n",
             LogProbSeq, exp( LogProbSeq ) );

    result = (double*)malloc( 2 * sizeof( double ) );
    result[0] = LogProbSeq;
    result[1] = exp( LogProbSeq );

    return result;
}

double *complete_likelihood( Parameter *param,
                             int n_obs, int *obs,
                             int *states,
                             FILE * output )
{
    double LogProbSeq;
    int t, nb, size, offset, i, j;
    ObsStats * obsstats;
    double *result;    

    if( param->N < 1 || param->B < 1 || param->M < 1 )
    {
        fprintf( output, "invalid model dimensions" );
        return NULL;
    }

    nb = n_obs / param->B;
    if( n_obs % param->B > 0 )
        nb++;

    obsstats = (ObsStats *)malloc( nb * sizeof(ObsStats) );
    ObsStats * aux;
    for( i = 0; i < nb; i++ )
    {
        aux = obsstats+i;
        aux->n = (int **)malloc( param->M * sizeof( int * ) );
        for( j = 0; j < param->M; j++ )
        {
            aux->n[j] = (int *)malloc( param->M * sizeof( int ) );
        }
    }

    /* evaluate loss fractions for each batch */
    for( offset = 0, t = 0, size = param->B;
         t < nb;
         t++, offset += param->B )
    {
        if( t == (nb-1) && (n_obs % param->B) > 0 )
            size = n_obs % param->B;

        measure_fractions( param, obs+offset, size, obsstats+t );
    }

    LogProbSeq = log( param->pi[states[0]] ) +
                 Eval_LogProbObs( param, states[0], obsstats+0 );

    for( t = 1; t < nb; t++ )
    {
        LogProbSeq += log( param->A[states[t-1]][states[t]] ) + 
                      Eval_LogProbObs( param, states[t], obsstats+t );
    }

    fprintf( output, "log-likelihood = %.10e\tlikelihood = %.10e\n",
             LogProbSeq, exp( LogProbSeq ) );

    free( obsstats );

    result = (double*)malloc( 2 * sizeof( double ) );
    result[0] = LogProbSeq;
    result[1] = exp( LogProbSeq );

    return result;
}

double *train( Parameter *param,
               int n_obs, int *obs,
               int nit, double threshold,
               FILE * output )
{
    int i, j, k, t, it, nb, offset, size;
    double csi, gamma, sum_gamma, * sum_gamma_p;
    double LogProbSeq, old_LogProbSeq;
    double ** alpha, ** beta, * scale;
    double **  Expected_Transition,
           **  Expected_Emission_R,
           *** Expected_Emission_P;
    ObsStats * obsstats;
    double *result = NULL;
    int result_size = 0;    

    if( param->N < 1 || param->B < 1 || param->M < 1 )
    {
        fprintf( output, "invalid model dimensions" );
        return NULL;
    }

    /* calculate the number of batches in the sample */
    nb = n_obs / param->B;
    if( n_obs % param->B > 0 )
        nb++;

    alpha = create_zero_matrix( n_obs, param->N );
    beta  = create_zero_matrix( n_obs, param->N );
    scale = create_zero_vector( n_obs );

    sum_gamma_p = create_zero_vector( param->M );

    Expected_Transition = create_zero_matrix( param->N, param->N );
    Expected_Emission_R = create_zero_matrix( param->N, param->M );
    Expected_Emission_P = create_zero_3d_matrix( param->N, param->M, param->M );

    obsstats = (ObsStats *)malloc( nb * sizeof(ObsStats) );
    ObsStats * aux;
    for( i = 0; i < nb; i++ )
    {
        aux = obsstats+i;
        aux->n = (int **)malloc( param->M * sizeof( int * ) );
        for( j = 0; j < param->M; j++ )
        {
            aux->n[j] = (int *)malloc( param->M * sizeof( int ) );
        }
    }

    /* evaluate parameters for each batch */
    for( offset = 0, t = 0, size = param->B;
         t < nb;
         t++, offset += param->B )
    {
        if( t == (nb-1) && (n_obs % param->B) > 0 )
            size = n_obs % param->B;

        measure_fractions( param, obs+offset, size, obsstats+t );
    }

    /* perform EM iterations */
    LogProbSeq = 0.0;
    for( it = 0; it < nit && !training_flag; it++ )
    {
        old_LogProbSeq = LogProbSeq;

        forward_filter( param, 0, n_obs, obs, alpha, scale, &LogProbSeq, 0 );

        if( LogProbSeq == 0.0 )
        {
            fprintf( output, "  LogProbSeq = 0.0\n" );
            break;
        }
        if( isnan( LogProbSeq ) )
        {
            fprintf( output, "  LogProbSeq = NaN\n" );
            fixMatrix( param );
            break;
        }

        backward_filter( param, n_obs, obs, scale, beta );

        if( it == 0 )
            fprintf( output, "# %10s %20s %20s\n",
                     "iteration", "log-likelihood", "likelihood" );

        fprintf( output, "  %10d %20.10e %20.10e\n",
                 it, LogProbSeq, exp( LogProbSeq ) );
                 
        // Save result
        result = (double *)realloc( result, ( ++result_size + 1 ) *
                                            sizeof(double ) );
        result[ result_size ] = LogProbSeq;                 

        if( it > 0 && (LogProbSeq-old_LogProbSeq) < threshold )
        {
            fprintf( output, "# likelihood threshold reached\n" );
            break;
        }

        /* estimate parameter for each state i=0,...,N-1 */
        for( i = 0; i < param->N; i++ )
        {
            for( j = 0; j < param->M; j++ ) 
            {
                Expected_Emission_R[i][j] = 0.0;
                for( k = 0; k < param->M; k++ )
                    Expected_Emission_P[i][j][k] = 0.0;
            }

            for( k = 0; k < param->N; k++ )
                Expected_Transition[i][k] = 0.0;

            sum_gamma = 0.0;

            for( j = 0; j < param->M; j++ ) 
                sum_gamma_p[j] = 0.0;

            // From Batch 0 to T-1
            for( t = 0; t < nb - 1; t++ )
            {
                gamma = 0.0;
                for( j = 0; j < param->N; j++ )
                {
                    csi    = Eval_Csi( param, obs, t, i, j );
                    gamma += csi;
                    Expected_Transition[i][j] += csi;
                }
                sum_gamma += gamma;

                Expected_Emission_R[i][obsstats[t].first] +=  gamma;

                for( j = 0; j < param->M; j++ )
                {
                    for( k = 0; k < param->M; k++ )
                    {
                        Expected_Emission_P[i][j][k] += obsstats[t].n[j][k] * gamma;
                        sum_gamma_p[j] += obsstats[t].n[j][k] * gamma;
                    }
                }
            }

            // Expected_Transition
            if( sum_gamma )
                for( k = 0; k < param->N; k++ )
                    Expected_Transition[i][k] /= sum_gamma;

            /* eval gamma in the last time instant */
            gamma = alpha[n_obs-1][i] * beta[n_obs-1][i] / scale[n_obs-1];
            /*gamma = Eval_Gamma( param, t, i );*/

            // TODO: acho que precisa acumular gamma em Expected_Emission_R
            Expected_Emission_R[i][obsstats[t].first] +=  gamma;
            sum_gamma += gamma;

            // Expected_Emission_P
            for( j = 0; j < param->M; j++ )
            {
                for( k = 0; k < param->M; k++ )
                {
                    Expected_Emission_P[i][j][k] += obsstats[t].n[j][k] * gamma;
                    sum_gamma_p[j] += obsstats[t].n[j][k] * gamma;
                }
            }

            // Expected_Emission_R
            if( sum_gamma )
            {
                for( j = 0; j < param->M; j++ )
                {
                    Expected_Emission_R[i][j] /= sum_gamma;
                }
            }

            for( j = 0; j < param->M; j++ )
            {
                if( sum_gamma_p[j] )
                {
                    for( k = 0; k < param->M; k++ )
                    {
                        Expected_Emission_P[i][j][k] /= sum_gamma_p[j];
                    }
                }
            }

            param->pi[i] = Eval_Gamma( param, 0, i );
        }

        destroy_matrix( param->N, param->N, param->A );
        param->A = copy_matrix( param->N, param->N, Expected_Transition );
        destroy_3d_matrix( param->N, param->M, param->M, param->p );
        param->p = copy_3d_matrix( param->N, param->M, param->M, Expected_Emission_P );
        destroy_matrix( param->N, param->M, param->r );
        param->r = copy_matrix( param->N, param->M, Expected_Emission_R );
    }

    /* display final likelihood */
    forward_filter( param, 0, n_obs, obs, alpha, scale, &LogProbSeq, 0 );

    if( it == 0 )
        fprintf( output, "# %10s %20s %20s\n",
                 "iteration", "log-likelihood", "likelihood" );

    fprintf( output, "  %10d %20.10e %20.10e\n",
             it, LogProbSeq, exp( LogProbSeq ) );
             
    result = (double *)realloc( result, ( ++result_size + 1 ) *
                                        sizeof( double ) );
    result[ result_size ] = LogProbSeq;
    result[0] = result_size;             

    free( obsstats );

    /* Saving last known state distribution ( Pi_{O_T} ). */
    for( i = 0; i < param->N; i++ )
        param->gamma[i] = Eval_Gamma( param, nb-1, i );

    destroy_matrix( n_obs, param->N, alpha );
    destroy_matrix( n_obs, param->N, beta );
    destroy_vector( n_obs, scale );

    destroy_matrix( param->N, param->N, Expected_Transition );
    destroy_matrix( param->N, param->M, Expected_Emission_R );
    destroy_3d_matrix( param->N, param->M, param->M, Expected_Emission_P );

    return result;
}

int train_complete( Parameter * param, int n_obs, int * obs, int * states,
                    FILE * output )
{
    unsigned int i, j, k, t, m, size, offset, nb;
    ObsStats * obsstats;
    int ** n_slots, * n_batches;

    if( param->N < 1 || param->B < 1 || param->M < 1 )
    {
        fprintf( output, "invalid model dimensions\n" );
        return 0;
    }

    /* calculate the number of batches in the sample */
    nb = n_obs / param->B;
    if( n_obs % param->B > 0 )
        nb++;

    n_batches = (int *)malloc( param->N * sizeof(int) );
    n_slots  = (int **)malloc( param->N * sizeof(int *) );
    for( i = 0; i < param->N; i++ )
    {
        n_slots[i] = (int *) malloc( param->M * sizeof( int ) );
    }
    obsstats  = (ObsStats *)malloc( nb * sizeof(ObsStats) );
    ObsStats * aux;
    for( i = 0; i < nb; i++ )
    {
        aux = obsstats+i;
        aux->n = (int **)malloc( param->M * sizeof( int * ) );
        for( j = 0; j < param->M; j++ )
        {
            aux->n[j] = (int *)malloc( param->M * sizeof( int ) );
        }
    }

    /* evaluate loss fractions for each batch */
    for( offset = 0, t = 0, size = param->B;
         t < nb;
         t++, offset += param->B )
    {
        if( t == (nb-1) && (n_obs % param->B) > 0 )
            size = n_obs % param->B;

        measure_fractions( param, obs+offset, size, obsstats+t );
    }

    for( i = 0; i < param->N; i++ )
    {
        param->pi[i] = 0.0;

        for( j = 0; j < param->N; j++ )
            param->A[i][j] = 0.0;

        for( j = 0; j < param->M; j++ )
        {
            param->r[i][j] = 0.0;
            for( k = 0; k < param->M; k++ )
                param->p[i][j][k] = 0.0;
        }

        n_batches[i] = 0;
        for( j = 0; j < param->M; j++ )
            n_slots[i][j] = 0;
    }

    i = states[0];

    param->pi[i]++;

    param->r[i][obsstats[0].first] += obsstats[0].first;
    for( j = 0; j < param->M; j++ )
    {
        for( k = 0; k < param->M; k++ )
        {
            if( j != k )
            {
                param->p[i][j][k] += obsstats[0].n[j][k];
            }
        }
    }

    n_batches[i]++;
    for( j = 0; j < param->M; j++ )
    {
        for( k = 0; k < param->M; k++ )
        {
            n_slots[i][j] += obsstats[0].n[j][k];
        }
    }

    for( t = 1; t < nb; t++ )
    {
        i = states[t-1];
        j = states[t];

        param->A[i][j]++;

        param->r[j][obsstats[t].first]++;
        for( k = 0; k < param->M; k++ )
        {
            for( m = 0; m < param->M; m++ )
            {
                param->p[j][k][m] += obsstats[t].n[k][m];
            }
        }

        n_batches[j]++;
        for( k = 0; k < param->M; k++ )
        {
            for( m = 0; m < param->M; m++ )
            {
                n_slots[j][k] += obsstats[t].n[k][m];
            }
        }
    }

    normalize_vector( param->N, param->pi );
    normalize_matrix( param->N, param->N, param->A );

    for( i = 0; i < param->N; i++ )
    {
        for( j = 0; j < param->M; j++ )
        {
            if( n_batches[i] > 0 )
                param->r[i][j] /= (double)n_batches[i];

            if( n_slots[i][j] > 0 )
            {
                for( k = 0; k < param->M; k++ )
                {
                    param->p[i][j][k] /= (double)n_slots[i][j];
                }
            }
        }
    }

    free( n_batches );
    for( i = 0; i < param->M; i++ )
    {
        free( n_slots[i] );
    }
    free( obsstats );

    return 1;
}
