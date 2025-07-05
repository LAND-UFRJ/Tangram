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
    int n00, /* the number of transitions from 0 to 0 within a batch */
        n01, /* the number of transitions from 0 to 1 within a batch */
        n10, /* the number of transitions from 1 to 0 within a batch */
        n11; /* the number of transitions from 1 to 1 within a batch */
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
    int i, j;

    for( i = 0; i < param->N; i++ )
    {
        for( j = 0; j < param->N; j++ )
            if( isnan( param->A[i][j] ) )
                param->A[i][j] = 0.0;

        if( isnan( param->r[i] ) )
            param->r[i] = 0.5;

        if( isnan( param->p[i] ) )
            param->p[i] = 0.5;

        if( isnan( param->q[i] ) )
            param->q[i] = 0.5;
    }
}

/*****************************************************************************/
/* Calculates the probability of observing a batch with parameters r, p and q*/
/*****************************************************************************/
static double Eval_LogProbObs( double r, double p, double q,
                               ObsStats * obsstats )
{
    double sum;

    sum = 0;

    if( obsstats->first == 0 )
        sum += log( 1-r );
    else if( obsstats->first == 1 )
        sum += log( r );

    sum += obsstats->n01 * log( p ) + obsstats->n00 * log( 1-p ) +
           obsstats->n10 * log( q ) + obsstats->n11 * log( 1-q );

    return sum;
}

static void measure_fractions( int * obs, int n_obs, ObsStats * obsstats )
{
    int i;

    obsstats->first = obs[0];

    obsstats->n00 = obsstats->n01 = obsstats->n10 = obsstats->n11 = 0;

    for( i = 1; i < n_obs; i++ )
    {
        if( obs[i-1] == 0 )
        {
            if( obs[i] == 0 )
                obsstats->n00++;
            else if( obs[i] == 1 )
                obsstats->n01++;
        }
        else if( obs[i-1] == 1 )
        {
            if( obs[i] == 0 )
                obsstats->n10++;
            else if( obs[i] == 1 )
                obsstats->n11++;
        }
    }
}

double *likelihood( Parameter * param, int n_obs, int * obs, FILE * output )
{
    double LogProbSeq;
    double *result;

    if( param->N < 1 || param->B < 1 )
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
                             int n_obs, int * obs,
                             int * states, FILE * output )
{
    double LogProbSeq;
    int t, nb, size, offset;
    ObsStats * obsstats;
    double *result;

    if( param->N < 1 || param->B < 1 )
    {
        fprintf( output, "invalid model dimensions" );
        return NULL;
    }

    nb = n_obs / param->B;
    if( n_obs % param->B > 0 )
        nb++;

    obsstats = (ObsStats *)malloc( nb * sizeof(ObsStats) );

    /* evaluate loss fractions for each batch */
    for( offset = 0, t = 0, size = param->B;
         t < nb;
         t++, offset += param->B )
    {
        if( t == (nb-1) && (n_obs % param->B) > 0 )
            size = n_obs % param->B;

        measure_fractions( obs+offset, size, obsstats+t );
    }

    LogProbSeq = log( param->pi[states[0]] ) +
                 Eval_LogProbObs( param->r[states[0]], param->p[states[0]],
                                  param->q[states[0]], obsstats+0 );

    for( t = 1; t < nb; t++ )
    {
        LogProbSeq += log( param->A[states[t-1]][states[t]] ) + 
                   Eval_LogProbObs( param->r[states[t]], param->p[states[t]],
                                    param->q[states[t]], obsstats+t );
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
    double csi, gamma, sum_gamma, sum_gamma_p, sum_gamma_q;
    double LogProbSeq, old_LogProbSeq;
    double ** alpha, ** beta, * scale;
    double ** Expected_Transition,
            * Expected_Emission_R,
            * Expected_Emission_P,
            * Expected_Emission_Q;
    double * src, * dst;
    ObsStats * obsstats;
    double *result = NULL;
    int result_size = 0;

    if( param->N < 1 || param->B < 1 )
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

    Expected_Transition = create_zero_matrix( param->N, param->N );
    Expected_Emission_R = create_zero_vector( param->N );
    Expected_Emission_P = create_zero_vector( param->N );
    Expected_Emission_Q = create_zero_vector( param->N );

    obsstats = (ObsStats *)malloc( nb * sizeof(ObsStats) );

    /* evaluate gilbert parameters for each batch */
    for( offset = 0, t = 0, size = param->B;
         t < nb;
         t++, offset += param->B )
    {
        if( t == (nb-1) && (n_obs % param->B) > 0 )
            size = n_obs % param->B;

        measure_fractions( obs+offset, size, obsstats+t );
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

        /* Save result */
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
            Expected_Emission_R[i] = 0.0;
            Expected_Emission_P[i] = 0.0;
            Expected_Emission_Q[i] = 0.0;

            for( k = 0; k < param->N; k++ )
                Expected_Transition[i][k] = 0.0;

            sum_gamma = 0.0;
            sum_gamma_p = 0.0;
            sum_gamma_q = 0.0;

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

                Expected_Emission_R[i] += obsstats[t].first * gamma;

                Expected_Emission_P[i] += obsstats[t].n01 * gamma;
                sum_gamma_p += (obsstats[t].n00 + obsstats[t].n01) * gamma;
                Expected_Emission_Q[i] += obsstats[t].n10 * gamma;
                sum_gamma_q += (obsstats[t].n10 + obsstats[t].n11) * gamma;
            }

            if( sum_gamma )
                for( k = 0; k < param->N; k++ )
                    Expected_Transition[i][k] /= sum_gamma;

            /* eval gamma in the last time instant */
            gamma = alpha[n_obs-1][i] * beta[n_obs-1][i] / scale[n_obs-1];
            /*gamma = Eval_Gamma( param, t, i );*/

            /* TODO: acho que precisa acumular gamma em Expected_Emission_R */
            Expected_Emission_R[i] += obsstats[t].first * gamma;
            sum_gamma += gamma;

            Expected_Emission_P[i] += obsstats[t].n01 * gamma;
            sum_gamma_p += (obsstats[t].n00 + obsstats[t].n01) * gamma;
            Expected_Emission_Q[i] += obsstats[t].n10 * gamma;
            sum_gamma_q += (obsstats[t].n10 + obsstats[t].n11) * gamma;

            if( sum_gamma )
                Expected_Emission_R[i] /= sum_gamma;
            if( sum_gamma_p )
                Expected_Emission_P[i] /= sum_gamma_p;
            if( sum_gamma_q )
                Expected_Emission_Q[i] /= sum_gamma_q;

            param->pi[i] = Eval_Gamma( param, 0, i );
        }

        for( i = 0; i < param->N; i++ )
        {
            src = Expected_Transition[i];
            dst = param->A[i];
            for( j = 0; j < param->N; j++ )
                *dst++ = *src++;

            param->r[i] = Expected_Emission_R[i];
            param->p[i] = Expected_Emission_P[i];
            param->q[i] = Expected_Emission_Q[i];
        }
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
        param->gamma[i] = alpha[n_obs-1][i] * beta[n_obs-1][i] / scale[n_obs-1];

    destroy_matrix( n_obs, param->N, alpha );
    destroy_matrix( n_obs, param->N, beta );
    destroy_vector( n_obs, scale );

    destroy_matrix( param->N, param->N, Expected_Transition );
    destroy_vector( param->N, Expected_Emission_R );
    destroy_vector( param->N, Expected_Emission_P );
    destroy_vector( param->N, Expected_Emission_Q );

    return result;
}

int train_complete( Parameter * param, int n_obs, int * obs, int * states,
                    FILE * output )
{
    unsigned int i, j, t, size, offset, nb;
    ObsStats * obsstats;
    int * n0_slots, * n1_slots, * n_batches;

    if( param->N < 1 || param->B < 1 )
    {
        fprintf( output, "invalid model dimensions\n" );
        return 0;
    }

    /* calculate the number of batches in the sample */
    nb = n_obs / param->B;
    if( n_obs % param->B > 0 )
        nb++;

    n_batches = (int *)malloc( param->N * sizeof(int) );
    n0_slots  = (int *)malloc( param->N * sizeof(int) );
    n1_slots  = (int *)malloc( param->N * sizeof(int) );
    obsstats  = (ObsStats *)malloc( nb * sizeof(ObsStats) );

    /* evaluate loss fractions for each batch */
    for( offset = 0, t = 0, size = param->B;
         t < nb;
         t++, offset += param->B )
    {
        if( t == (nb-1) && (n_obs % param->B) > 0 )
            size = n_obs % param->B;

        measure_fractions( obs+offset, size, obsstats+t );
    }

    for( i = 0; i < param->N; i++ )
    {
        param->pi[i] = 0.0;

        for( j = 0; j < param->N; j++ )
            param->A[i][j] = 0.0;

        param->r[i] = 0.0;
        param->p[i] = 0.0;
        param->q[i] = 0.0;

        n_batches[i] = 0;
        n0_slots[i] = 0;
        n1_slots[i] = 0;
    }

    i = states[0];

    param->pi[i]++;

    param->r[i] += obsstats[0].first;
    param->p[i] += obsstats[0].n01;
    param->q[i] += obsstats[0].n10;

    n_batches[i]++;
    n0_slots[i] += obsstats[0].n01 + obsstats[0].n00;
    n1_slots[i] += obsstats[0].n11 + obsstats[0].n10;

    for( t = 1; t < nb; t++ )
    {
        i = states[t-1];
        j = states[t];

        param->A[i][j]++;

        param->r[j] += obsstats[t].first;
        param->p[j] += obsstats[t].n01;
        param->q[j] += obsstats[t].n10;

        n_batches[j]++;
        n0_slots[j] += obsstats[t].n01 + obsstats[t].n00;
        n1_slots[j] += obsstats[t].n11 + obsstats[t].n10;
    }

    normalize_vector( param->N, param->pi );
    normalize_matrix( param->N, param->N, param->A );

    for( i = 0; i < param->N; i++ )
    {
        if( n_batches[i] > 0 )
            param->r[i] /= (double)n_batches[i];
        if( n0_slots[i] > 0 )
            param->p[i] /= (double)n0_slots[i];
        if( n1_slots[i] > 0 )
            param->q[i] /= (double)n1_slots[i];
    }

    free( n_batches );
    free( n0_slots );
    free( n1_slots );
    free( obsstats );

    return 1;
}
