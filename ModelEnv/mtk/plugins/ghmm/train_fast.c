#include "train.h"

#include <math.h>
#include <stdlib.h>

#include "matrix.h"

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
int fast_training_flag = 0;

/*****************************************************************************/
/* Calculates the probability of being in state Si at time t and state Sj at */
/* time t+1, given the model and the observation sequence.                   */
/* This is a define because it is much faster than a function call.          */
/*****************************************************************************/
#define Eval_Csi( param, obsstats, t, i, j ) \
     (alpha[t][i] * param->A[i][j] * \
      Eval_ProbObs( param->r[j], param->p[j], param->q[j], obsstats+(t+1) ) * \
      beta[t+1][j])

/*****************************************************************************/
/* Calculates the probability of being in state Si at time t given the model.*/
/*****************************************************************************/
#define Eval_Gamma( param, t, i ) (alpha[t][i] * beta[t][i] / scale[t])

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
/* Calculates the probability of observing a batch with parameters p and q   */
/*****************************************************************************/
static double Eval_ProbObs( double r, double p, double q,
                            ObsStats * obsstats )
{
    int i;
    double ProbObs;

    /* initial state - assume equiprobable */
    if( obsstats->first == 0 )
        ProbObs = 1-r;
    else if( obsstats->first == 1 )
        ProbObs = r;

    for( i = 0; i < obsstats->n00; i++ )
        ProbObs *= (1-p);
    for( i = 0; i < obsstats->n01; i++ )
        ProbObs *= p;
    for( i = 0; i < obsstats->n10; i++ )
        ProbObs *= q;
    for( i = 0; i < obsstats->n11; i++ )
        ProbObs *= (1-q);

    return ProbObs;
}

/*****************************************************************************/
/* Calculates the probability of the partial observation sequence from the   */
/* beginning to time t and state Sj at time t, given the model.              */
/*****************************************************************************/
static double Eval_Alpha( Parameter * param, ObsStats * obsstats, int nb,
                          double ** alpha, double * scale )
{
    int i, j, t;
    double sum;
    double invScale;

    invScale = 0.0;
    for( i = 0; i < param->N; i++ )
    {
        alpha[0][i] = param->pi[i] *
                      Eval_ProbObs( param->r[i], param->p[i], param->q[i],
                                    obsstats+0 );

        invScale += alpha[0][i];
    }
    scale[0] = 1.0 / invScale;

    for( i = 0; i < param->N; i++ )
    {
        alpha[0][i] *= scale[0];
    }

    for( t = 0; t < nb - 1; t++ )
    {
        invScale = 0.0;
        for( j = 0; j < param->N; j++ )
        {
            sum = 0.0;
            for( i = 0; i < param->N; i++ )
                sum += alpha[t][i] * param->A[i][j];

            alpha[t+1][j] = sum *
                            Eval_ProbObs( param->r[j], param->p[j], param->q[j],
                                          obsstats+(t+1) );
            invScale += alpha[t+1][j];
        }
        scale[t+1] = 1.0 / invScale;

        for( i = 0; i < param->N; i++ )
        {
            alpha[t+1][i] *= scale[t+1];
        }
    }

    sum = 0.0;
    for( i = 0; i < nb; i++ )
        sum -= log( scale[i] ); /* alpha[nb-1][i]; */

    return( sum );
}

/*****************************************************************************/
/* Calculates the probability of the partial observation sequence from t+1   */
/* to the end, given the state Si at time t and the model.                   */
/*****************************************************************************/
static void Eval_Beta( Parameter * param, ObsStats * obsstats, int nb,
                       double ** beta, double * scale )
{
    int i, j, t;

    for( i = 0; i < param->N; i++ )
    {
        beta[nb-1][i] = scale[nb-1];
    }

    for( t = nb - 2; t >= 0; t-- )
    {
        for( i = 0; i < param->N; i++ )
        {
            beta[t][i] = 0.0;
            for( j = 0; j < param->N; j++ )
                beta[t][i] += param->A[i][j] * beta[t+1][j] *
                              Eval_ProbObs( param->r[j], param->p[j],
                                            param->q[j], obsstats+(t+1) );
            beta[t][i] *= scale[t];
        }
    }
}

static double Eval_LogProbSeq( int nb, double * scale )
{
    int i;
    double sum;

    sum = 0.0;
    for( i = 0; i < nb; i++ )
        sum -= log( scale[ i ] );

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

double *likelihood_fast( Parameter * param, int n_obs, int * obs, FILE * output )
{
    double LogProbSeq;
    double ** alpha, * scale;
    int t, nb, offset, size;
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

    alpha = create_zero_matrix( nb, param->N );
    scale = create_zero_vector( nb );

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

    Eval_Alpha( param, obsstats, nb, alpha, scale );
    LogProbSeq = Eval_LogProbSeq( nb, scale );

    fprintf( output, "log-likelihood = %.10e\tlikelihood = %.10e\n",
             LogProbSeq, exp( LogProbSeq ) );

    destroy_matrix( nb, param->N, alpha );
    destroy_vector( nb, scale );

    free( obsstats );

    result = (double*)malloc( 2 * sizeof( double ) );
    result[0] = LogProbSeq;
    result[1] = exp( LogProbSeq );

    return result;
}

double *train_fast( Parameter * param, int n_obs, int * obs, int nit,
                    double threshold, FILE * output )
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

    alpha = create_zero_matrix( nb, param->N );
    beta  = create_zero_matrix( nb, param->N );
    scale = create_zero_vector( nb );

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

    for( it = 0; it < nit && !fast_training_flag; it++ )
    {
        old_LogProbSeq = LogProbSeq;

        LogProbSeq = Eval_Alpha( param, obsstats, nb, alpha, scale );
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
        Eval_Beta( param, obsstats, nb, beta, scale );

        LogProbSeq = Eval_LogProbSeq( nb, scale );

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
                    csi    = Eval_Csi( param, obsstats, t, i, j );
                    gamma += csi;
                    Expected_Transition[i][j] += csi;
                }
                sum_gamma += gamma;

                if( obsstats[t].first )
                    Expected_Emission_R[i] += gamma;

                Expected_Emission_P[i] += obsstats[t].n01 * gamma;
                sum_gamma_p += (obsstats[t].n00 + obsstats[t].n01) * gamma;
                Expected_Emission_Q[i] += obsstats[t].n10 * gamma;
                sum_gamma_q += (obsstats[t].n10 + obsstats[t].n11) * gamma;
            }

            for( k = 0; k < param->N; k++ )
                Expected_Transition[i][k] /= sum_gamma;

            /* eval gamma in the last time instant */
            gamma = Eval_Gamma( param, t, i );

            /* TODO: acho que precisa acumular gamma em Expected_Emission_R */
            Expected_Emission_R[i] += obsstats[t].first * gamma;
            sum_gamma += gamma;

            Expected_Emission_P[i] += obsstats[t].n01 * gamma;
            sum_gamma_p += (obsstats[t].n00 + obsstats[t].n01) * gamma;
            Expected_Emission_Q[i] += obsstats[t].n10 * gamma;
            sum_gamma_q += (obsstats[t].n10 + obsstats[t].n11) * gamma;

            Expected_Emission_R[i] /= sum_gamma;
            Expected_Emission_P[i] /= sum_gamma_p;
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
    Eval_Alpha( param, obsstats, nb, alpha, scale );
    LogProbSeq = Eval_LogProbSeq( nb, scale );

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
    
    destroy_matrix( nb, param->N, alpha );
    destroy_matrix( nb, param->N, beta );
    destroy_vector( nb, scale );

    destroy_matrix( param->N, param->N, Expected_Transition );
    destroy_vector( param->N, Expected_Emission_R );
    destroy_vector( param->N, Expected_Emission_P );
    destroy_vector( param->N, Expected_Emission_Q );

    return result;
}
