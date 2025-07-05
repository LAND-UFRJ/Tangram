#include "forecast.h"

#include "hmm_util.h"
#include "matrix.h"

#include <math.h>

#define MIN(a,b) ((a)<(b)?(a):(b))

/**************************************************************************
* This function calculates the symbol emission distribution at each time  *
* step in the forecasting window F.                                       *
*                                                                         *
* Parameters:                                                             *
*    param : Hmm's parameter information structure.                       *
*    pi : Last known state distribution.                                  *
*    F : Forecasting window.                                              *
*    output : Output file descriptor.                                     *
*                                                                         *
* OBS:                                                                    *
* M[i][j] = P[emiting symbol j after t transitions | initial state i].    *
* or                                                                      *
* M[i][j] = P( X_t = j | Y_1 = i )                                        *
***************************************************************************/
void Eval_Omega( Parameter * param, double * pi, int F, Sample * out, FILE * output )
{
    double ** M, ** old_M, * omega;
    double ** matrix[2];
    int c, i, j, k, f, argmax;
    double entropy;

    matrix[0] = create_zero_matrix( param->N, param->M );
    matrix[1] = create_zero_matrix( param->N, param->M );
    omega     = create_zero_vector( param->M );

    c = 0;
    M = matrix[c];
    old_M = matrix[1-c];

    /* Initializes M = B */
    for( i = 0; i < param->N; i++ )
        for( j = 0; j < param->M; j++ )
            M[i][j] = param->B[i][j];

    for( f = 0; f < F; f++ )
    {
        c = 1-c;
        M = matrix[c];
        old_M = matrix[1-c];

        /* M[i][j] = P[ X_F = j | Y_(F-f) = i ] */
        for( i = 0; i < param->N; i++ )
        {
            for( j = 0; j < param->M; j++ )
            {
                M[i][j] = 0.0;
                for( k = 0; k < param->N; k++ )
                    M[i][j] += param->A[i][k] * old_M[k][j];
            }
        }

        /* omega = sum in i of P[Y_1 = i] * P[ X_F = j | Y_1 = i ]; omega = P[ X_t = j ] */
        for( j = 0; j < param->M; j++ )
        {
            omega[j] = 0.0;
            for( k = 0; k < param->N; k++ )
                omega[j] += pi[k] * M[k][j];
        }

        fprintf( output, "(Time Step %d):", f+1 );
        argmax = 0;
        entropy = 0.0;
        fprintf( output, " distribution: [" );
        for( j = 0; j < param->M; j++ )
        {
            fprintf( output, " %.10e", omega[j] );
            
            /* If omega[j] = 0 than log(omega[j]) is -inf, so it should not be added */
            if( omega[j] != 0 )
                entropy -= omega[j] * (log(omega[j])/log(2));
            
            /* Chooses most probable symbol */
            if( omega[j] > omega[argmax] )
                argmax = j;
        }
        fprintf( output, " ]; most probable symbol: %d ; entropy (in bits): %.10e\n", argmax, entropy );
        
        if( out )
            out->data[f] = argmax;
    }

    destroy_matrix( param->N, param->M, matrix[0] );
    destroy_matrix( param->N, param->M, matrix[1] );
    destroy_vector( param->N, omega );
}

/*****************************************************************************/
/* Calculates the probability distribution for the observation symbols at    */
/* each time step in the future, given the model parameters and the history  */
/* observations from the past.                                               */
/*****************************************************************************/
int forecast( Parameter * param, Sample * in, Sample * out, int F, FILE * output )
{
    double ** alpha, * scale, * pi;
    int i;
    double sum;
    
    alpha = create_zero_matrix( in->size, param->N );
    scale = create_zero_vector( in->size );
    pi    = create_zero_vector( param->N );

    /* Calculating Pi based on newly observed symbols */
    
    /* Obtaining last known state distribution */
    for( i = 0; i < param->N; i++ )
        pi[i] = param->gamma[i];

    /* Evaluate Alpha */
    sum = Eval_Alpha( param, in->data, in->size, alpha, scale );
    if( isnan(sum) )
    {
        fprintf( output, "Eval_Alpha returned NaN.\n" );
        fprintf( output, "Check if observation sequence is valid in the current HMM.\n" );
    }

    /* Evaluate new Pi using Alpha */
    for( sum = 0.0, i = 0; i < param->N; i++ )
        sum += alpha[(in->size)-1][i];
    for( i = 0; i < param->N; i++ )
        pi[i] = alpha[(in->size)-1][i]/sum;

    /* Calculating emission distribution */
    Eval_Omega( param, pi, F, out, output );

    destroy_matrix( in->size, param->N, alpha );
    destroy_vector( in->size, scale );

    return 0;
}
