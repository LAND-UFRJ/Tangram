#include "train.h"

#include <math.h>
#include <stdlib.h>

#include "matrix.h"
#include "hmm_util.h"

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
     (alpha[t][i] * param->A[i][j] * param->B[j][obs[t+1]] * beta[t+1][j])

#define Eval_Csi_K( param, obs, t, i, j, k ) \
     (alpha[k][t][i] * param->A[i][j] * param->B[j][obs[t+1]] * beta[k][t+1][j])

/*****************************************************************************/
/* Calculates the probability of being in state Si at time t given the model.*/
/*****************************************************************************/
#define Eval_Gamma( param, t, i ) (alpha[t][i] * beta[t][i] / scale[t])

#define Eval_Gamma_K( param, t, i, k ) (alpha[k][t][i] * beta[k][t][i] \
                                      / scale[k][t])

static void fixMatrix( Parameter * param )
{
    int i, j;

    for( i = 0; i < param->N; i++ )
    {
        for( j = 0; j < param->N; j++ )
            if( isnan( param->A[i][j] ) )
                param->A[i][j] = 0.0;
    }

    for( i = 0; i < param->N; i++ )
    {
        for( j = 0; j < param->M; j++ )
            if( isnan( param->B[i][j] ) )
                param->B[i][j] = 0.0;
    }
}

double *likelihood( Parameter * param, int n_obs, int * obs, FILE * output )
{
    double LogProbSeq;
    double ** alpha, * scale;
    double *result;    

    if( param->N < 1 || param->M < 1 )
    {
        fprintf( output, "invalid model dimensions" );
        return NULL;
    }

    alpha = create_zero_matrix( n_obs, param->N );
    scale = create_zero_vector( n_obs );

    Eval_Alpha( param, obs, n_obs, alpha, scale );
    LogProbSeq = Eval_LogProbSeq( n_obs, scale );

    fprintf( output, "log-likelihood = %.10e\tlikelihood = %.10e\n",
             LogProbSeq, exp( LogProbSeq ) );

    destroy_matrix( n_obs, param->N, alpha );
    destroy_vector( n_obs, scale );
    
    result = (double*)malloc( 2 * sizeof( double ) );
    result[0] = LogProbSeq;
    result[1] = exp( LogProbSeq );    

    return result;
}

double *complete_likelihood( Parameter *param,
                             int n_obs, int *obs,
                             int *states, FILE * output )
{
    double LogProbSeq;
    int t;
    double *result;

    if( param->N < 1 || param->M < 1 )
    {
        fprintf( output, "invalid model dimensions" );
        return 0;
    }

    LogProbSeq = log( param->pi[states[0]] ) +
                 log( param->B[states[0]][obs[0]] );

    for( t = 1; t < n_obs; t++ )
    {
        LogProbSeq += log( param->A[states[t-1]][states[t]] ) + 
                      log( param->B[states[t]][obs[t]] );
    }

    fprintf( output, "log-likelihood = %.10e\tlikelihood = %.10e\n",
             LogProbSeq, exp( LogProbSeq ) );

    result = (double*)malloc( 2 * sizeof( double ) );
    result[0] = LogProbSeq;
    result[1] = exp( LogProbSeq );

    return result;
}

double *train( Parameter * param, int n_obs, int * obs, int nit,
               double threshold, FILE * output )
{
    int i, j, k, t, it;
    double csi, gamma, sum_gamma;
    double LogProbSeq, old_LogProbSeq;
    double ** alpha, ** beta, * scale;
    double ** Expected_Transition, ** Expected_Emission;
    double * src, * dst;
    double *result = NULL;
    int result_size = 0;    

    if( param->N < 1 || param->M < 1 )
    {
        fprintf( output, "invalid model dimensions" );
        return NULL;
    }

    alpha = create_zero_matrix( n_obs, param->N );
    beta  = create_zero_matrix( n_obs, param->N );
    scale = create_zero_vector( n_obs );

    Expected_Transition = create_zero_matrix( param->N, param->N );
    Expected_Emission   = create_zero_matrix( param->N, param->M );

    for( it = 0; it < nit && !training_flag; it++ )
    {
        old_LogProbSeq = LogProbSeq;

        LogProbSeq = Eval_Alpha( param, obs, n_obs, alpha, scale );
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
        Eval_Beta( param, obs, n_obs, beta, scale );

        LogProbSeq = Eval_LogProbSeq( n_obs, scale );

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
            for( k = 0; k < param->M; k++ )
                Expected_Emission[i][k] = 0.0;

            for( k = 0; k < param->N; k++ )
                Expected_Transition[i][k] = 0.0;

            sum_gamma = 0.0;
            for( t = 0; t < n_obs - 1; t++ )
            {
                gamma = 0.0;
                for( j = 0; j < param->N; j++ )
                {
                    csi    = Eval_Csi( param, obs, t, i, j );
                    gamma += csi;
                    Expected_Transition[i][j] += csi;
                }

                sum_gamma += gamma;
                Expected_Emission[ i ][ obs[ t ] ] += gamma;
            }

            for( k = 0; k < param->N; k++ )
                Expected_Transition[i][k] /= sum_gamma;

            /* eval gamma in the last time instant */
            gamma = Eval_Gamma( param, t, i );

            sum_gamma += gamma;
            Expected_Emission[ i ][ obs[ t ] ] += gamma;

            for( k = 0; k < param->M; k++ )
                Expected_Emission[i][k] /= sum_gamma;

            param->pi[i] = Eval_Gamma( param, 0, i );
        }

        for( i = 0; i < param->N; i++ )
        {
            src = Expected_Transition[i];
            dst = param->A[i];
            for( j = 0; j < param->N; j++ )
                *dst++ = *src++;

            src = Expected_Emission[i];
            dst = param->B[i];
            for( k = 0; k < param->M; k++ )
                *dst++ = *src++;
        }
    }

    /* display final likelihood */
    Eval_Alpha( param, obs, n_obs, alpha, scale );
    LogProbSeq = Eval_LogProbSeq( n_obs, scale );
    if( LogProbSeq == 0.0 )
    {
        fprintf( output, "  LogProbSeq = 0.0\n" );
    }
    if( isnan( LogProbSeq ) )
    {
        fprintf( output, "  LogProbSeq = NaN\n" );
        fixMatrix( param );
    }

    if( it == 0 )
        fprintf( output, "# %10s %20s %20s\n",
                 "iteration", "log-likelihood", "likelihood" );

    fprintf( output, "  %10d %20.10e %20.10e\n",
             it, LogProbSeq, exp( LogProbSeq ) );

    result = (double *)realloc( result, ( ++result_size + 1 ) *
                                        sizeof( double ) );
    result[ result_size ] = LogProbSeq;
    result[0] = result_size;
    
    /* Saving last known state distribution ( Pi_{O_T} ). */
    for( i = 0; i < param->N; i++ )
        param->gamma[i] = Eval_Gamma( param, n_obs-1, i );

    destroy_matrix( n_obs, param->N, alpha );
    destroy_matrix( n_obs, param->N, beta );
    destroy_vector( n_obs, scale );

    destroy_matrix( param->N, param->N, Expected_Transition );
    destroy_matrix( param->N, param->M, Expected_Emission );

    return result;
}

double *train_multiple( Parameter * param, int n_samples, Sample ** samples,
                        int nit, double threshold, FILE * output )
{
    int i, j, k, t, it;
    double csi, gamma, sum_gamma_pi, sum_gamma_transition, sum_gamma_emission;
    double LogProbSeq, old_LogProbSeq;
    double *** alpha, *** beta, ** scale;
    double ** Expected_Transition, ** Expected_Emission;
    double * src, * dst;
    double *result = NULL;
    int result_size = 0;     

    if( param->N < 1 || param->M < 1 )
    {
        fprintf( output, "invalid model dimensions" );
        return NULL;
    }

    alpha = (double ***)malloc( n_samples * sizeof(double **) );
    for( k = 0; k < n_samples; k++ )
        alpha[k] = create_zero_matrix( samples[k]->size, param->N );

    beta = (double ***)malloc( n_samples * sizeof(double **) );
    for( k = 0; k < n_samples; k++ )
        beta[k] = create_zero_matrix( samples[k]->size, param->N );

    scale = (double **)malloc( n_samples * sizeof(double *) );
    for( k = 0; k < n_samples; k++ )
        scale[k] = create_zero_vector( samples[k]->size );

    Expected_Transition = create_zero_matrix( param->N, param->N );
    Expected_Emission   = create_zero_matrix( param->N, param->M );

    for( it = 0; it < nit && !training_flag; it++ )
    {
        old_LogProbSeq = LogProbSeq;

        for( k = 0; k < n_samples; k++ )
        {
            LogProbSeq = Eval_Alpha( param, samples[k]->data, samples[k]->size,
                                     alpha[k], scale[k] );
            if( LogProbSeq == 0.0 )
            {
                fprintf( output, "#  LogProbSeq = 0.0\n" );
                break;
            }
            if( isnan( LogProbSeq ) )
            {
                fprintf( output, "#  LogProbSeq = NaN\n" );
                fixMatrix( param );
                break;
            }
            Eval_Beta( param, samples[k]->data, samples[k]->size,
                                  beta[k], scale[k] );
        }

        LogProbSeq = 0.0;
        for( k = 0; k < n_samples; k++ )
            LogProbSeq += Eval_LogProbSeq( samples[k]->size, scale[k] );

        if( it == 0 )
            fprintf( output, "# %10s %20s %20s\n",
                     "iteration", "log-likelihood", "likelihood" );

        if( it > 0 && (LogProbSeq-old_LogProbSeq) < threshold )
        {
            fprintf( output, "# likelihood threshold reached\n" );
            break;
        }

        fprintf( output, "  %10d %20.10e %20.10e\n",
                 it, LogProbSeq, exp( LogProbSeq ) );

        // Save result
        result = (double *)realloc( result, ( ++result_size + 1 ) *
                                            sizeof(double ) );
        result[ result_size ] = LogProbSeq;

        for( i = 0; i < param->N; i++ )
        {
            for( k = 0; k < param->M; k++ )
                Expected_Emission[i][k] = 0.0;

            for( k = 0; k < param->N; k++ )
                Expected_Transition[i][k] = 0.0;

            sum_gamma_pi = 0.0;
            sum_gamma_transition = 0.0;
            sum_gamma_emission = 0.0;

            for( k = 0; k < n_samples; k++ )
            {
                sum_gamma_pi += Eval_Gamma_K( param, 0, i, k );

                for( t = 0; t < samples[k]->size - 1; t++ )
                {
                    gamma = 0.0;
                    for( j = 0; j < param->N; j++ )
                    {
                        csi    = Eval_Csi_K(param, samples[k]->data, t,i,j,k);
                        gamma += csi;
                        Expected_Transition[i][j] += csi;
                    }
                    sum_gamma_transition += gamma;
                    sum_gamma_emission += gamma;
                    Expected_Emission[ i ][ samples[k]->data[t] ] += gamma;
                }

                gamma = Eval_Gamma_K( param, t, i, k );
                sum_gamma_emission += gamma;
                Expected_Emission[ i ][ samples[k]->data[t] ] += gamma;
            }

            for( k = 0; k < param->N; k++ )
                Expected_Transition[i][k] /= sum_gamma_transition;

            for( k = 0; k < param->M; k++ )
                Expected_Emission[i][k] /= sum_gamma_emission;

            param->pi[i] = sum_gamma_pi/n_samples;
        }

        for( i = 0; i < param->N; i++ )
        {
            src = Expected_Transition[i];
            dst = param->A[i];
            for( j = 0; j < param->N; j++ )
                *dst++ = *src++;

            src = Expected_Emission[i];
            dst = param->B[i];
            for( k = 0; k < param->M; k++ )
                *dst++ = *src++;
        }
    }

    /* display final likelihood */
    for( k = 0; k < n_samples; k++ )
    {
        LogProbSeq = Eval_Alpha( param, samples[k]->data, samples[k]->size,
                                 alpha[k], scale[k] );
        if( LogProbSeq == 0.0 )
        {
            fprintf( output, "#  LogProbSeq = 0.0\n" );
            break;
        }
        if( isnan( LogProbSeq ) )
        {
            fprintf( output, "#  LogProbSeq = NaN\n" );
            fixMatrix( param );
            break;
        }
        Eval_Beta( param, samples[k]->data, samples[k]->size,
                              beta[k], scale[k] );
    }

    LogProbSeq = 0.0;
    for( k = 0; k < n_samples; k++ )
        LogProbSeq += Eval_LogProbSeq( samples[k]->size, scale[k] );

    if( it == 0 )
        fprintf( output, "# %10s %20s %20s\n",
                 "iteration", "log-likelihood", "likelihood" );

    fprintf( output, "  %10d %20.10e %20.10e\n",
             it, LogProbSeq, exp( LogProbSeq ) );

    result = (double *)realloc( result, ( ++result_size + 1 ) *
                                        sizeof( double ) );
    result[ result_size ] = LogProbSeq;
    result[0] = result_size;

    for( k = 0; k < n_samples; k++ )
        destroy_matrix( samples[k]->size, param->N, alpha[k] );
    free( alpha );

    for( k = 0; k < n_samples; k++ )
        destroy_matrix( samples[k]->size, param->N, beta[k] );
    free( beta );

    for( k = 0; k < n_samples; k++ )
        destroy_vector( samples[k]->size, scale[k] );
    free( scale );

    destroy_matrix( param->N, param->N, Expected_Transition );
    destroy_matrix( param->N, param->M, Expected_Emission );

    return result;
}

int train_complete( Parameter * param, int nobs, int * obs, int * states )
{
    unsigned int i, j, k, t;

    if( param->N < 1 || param->M < 1 )
        return 0;

    for( i = 0; i < param->N; i++ )
    {
        param->pi[i] = 0.0;

        for( j = 0; j < param->N; j++ )
            param->A[i][j] = 0.0;

        for( j = 0; j < param->M; j++ )
            param->B[i][j] = 0.0;
    }

    i = states[0];
    k = obs[0];

    param->pi[i] = 1.0;
    param->B[i][k]++;

    for( t = 1; t < nobs; t++ )
    {
        i = states[t-1];
        j = states[t];
        k = obs[t];

        param->A[i][j]++;
        param->B[j][k]++;
    }

    normalize_vector( param->N, param->pi );
    normalize_matrix( param->N, param->N, param->A );
    normalize_matrix( param->N, param->M, param->B );

    return 1;
}
