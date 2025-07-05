#include "train.h"

#include <math.h>
#include <stdlib.h>

#include "matrix.h"

typedef struct
{
    int first; /* first symbol in the batch */
    int ** n; /* the number of transitions from i to j within a batch */
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
      Eval_ProbObs( param, j, obsstats+(t+1) ) * \
      beta[t+1][j])

/*****************************************************************************/
/* Calculates the probability of being in state Si at time t given the model.*/
/*****************************************************************************/
#define Eval_Gamma( param, t, i ) (alpha[t][i] * beta[t][i] / scale[t])

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
/* Calculates the probability of observing a batch with parameters p         */
/* for the i-th state                                                        */
/*****************************************************************************/
static double Eval_ProbObs( Parameter * param, int state,
                            ObsStats * obsstats )
{
    int i, j, k;
    double ProbObs;

    /* initial state - assume equiprobable */
    ProbObs = param->r[state][obsstats->first];

    for( i = 0; i < param->M; i++ )
    {
        for( j = 0; j < param->M; j++ ) 
        {
            for( k = 0; k < obsstats->n[i][j]; k++ ) 
            {
                ProbObs *= param->p[state][i][j];
            }
        }
    }

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
                      Eval_ProbObs( param, i, obsstats+0 );

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
                            Eval_ProbObs( param, j, obsstats+(t+1) );
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
                              Eval_ProbObs( param, j, obsstats+(t+1) );
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

static void measure_fractions( Parameter * param, int * obs, int n_obs, ObsStats * obsstats )
{
    int i, j;

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

double *likelihood_fast( Parameter *param,
                         int n_obs, int *obs,
                         FILE *output )
{
    double LogProbSeq;
    double ** alpha, * scale;
    int t, nb, offset, size, i, j;
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

    alpha = create_zero_matrix( nb, param->N );
    scale = create_zero_vector( nb );

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
    double csi, gamma, sum_gamma, * sum_gamma_p;
    double LogProbSeq, old_LogProbSeq;
    double ** alpha, ** beta, * scale;
    double ** Expected_Transition,
           ** Expected_Emission_R,
           *** Expected_Emission_P;
    double * src, * dst;
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

    alpha = create_zero_matrix( nb, param->N );
    beta  = create_zero_matrix( nb, param->N );
    scale = create_zero_vector( nb );

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
            for( j = 0; j < param->M; j++ )
            {
                Expected_Emission_R[i][j] = 0.0;
                for( k = 0; k <  param->M; k++ )
                {
                    Expected_Emission_P[i][j][k] = 0.0;
                }
            }

            for( k = 0; k < param->N; k++ )
                Expected_Transition[i][k] = 0.0;

            sum_gamma = 0.0;
            for( j = 0; j < param->M; j++ )
            {
                sum_gamma_p[j] = 0.0;
            }

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

                Expected_Emission_R[i][obsstats[t].first] += gamma;

                for( j= 0; j < param->M; j++ )
                {
                    for( k = 0; k < param->M; k++ )
                    {
                        Expected_Emission_P[i][j][k] += obsstats[t].n[j][k] * gamma;
                        sum_gamma_p[j] += obsstats[t].n[j][k] * gamma;
                    }
                }
            }

            if( sum_gamma )
                for( k = 0; k < param->N; k++ )
                    Expected_Transition[i][k] /= sum_gamma;

            /* eval gamma in the last time instant */
            gamma = Eval_Gamma( param, t, i );

            // TODO: acho que precisa acumular gamma em Expected_Emission_R
            Expected_Emission_R[i][obsstats[t].first] +=  gamma;
            sum_gamma += gamma;

            for( j = 0; j < param->M; j++ )
            {
                for( k = 0; k < param->M; k++ )
                {
                    Expected_Emission_P[i][j][k] += obsstats[t].n[j][k] * gamma;
                    sum_gamma_p[j] += obsstats[t].n[j][k] * gamma;
                }
            }

            for( j = 0; j < param->M; j++ )
            {
                Expected_Emission_R[i][j] /= sum_gamma;
                for( k = 0; k < param->M; k++ )
                {
                    Expected_Emission_P[i][j][k] /= sum_gamma_p[j];
                }
            }

            param->pi[i] = Eval_Gamma( param, 0, i );
        }

        for( i = 0; i < param->N; i++ )
        {
            src = Expected_Transition[i];
            dst = param->A[i];
            for( j = 0; j < param->N; j++ )
                *dst++ = *src++;

            for( j = 0; j < param->M; j++ )
            {
                param->r[i][j] = Expected_Emission_R[i][j];
                for( k = 0; k < param->M; k++ )
                {
                    param->p[i][j][k] = Expected_Emission_P[i][j][k];
                }
            }
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
        param->gamma[i] = Eval_Gamma( param, nb-1, i );

    destroy_matrix( nb, param->N, alpha );
    destroy_matrix( nb, param->N, beta );
    destroy_vector( nb, scale );

    destroy_matrix( param->N, param->N, Expected_Transition );
    destroy_matrix( param->N, param->M, Expected_Emission_R );
    destroy_3d_matrix( param->N, param->M, param->M, Expected_Emission_P );

    return result;
}
