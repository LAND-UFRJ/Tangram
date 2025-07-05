#include "param.h"

#include <math.h>

#include "matrix.h"

/**
  Given the hidden state S_i, returns the probability of its
  internal chain being in state 0 at the end of B
  emissions.
  
  Warning: Either this function has a very bad name, or
    its return value was modified for some specific
    purpose. One of these needs to be changed. Notice that
    "sum" is not even used, so, probably, the return value
    is the one that needs to be modified.
**/
double eval_loss_rate( Parameter * param, int i, int B )
{
    double sum;
    double pi[2], new_pi[2];
    int b;

    sum = 0.0;
    pi[0] = 1.0 - param->r[i];
    pi[1] = param->r[i];

    for( b = 1; b <= B; b++ )
    {
        sum += pi[1];

        new_pi[0] = (pi[1] * param->q[i]) + (pi[0] * (1.0-param->p[i]));
        new_pi[1] = (pi[1] * (1.0-param->q[i])) + (pi[0] * param->p[i]);

        pi[0] = new_pi[0];
        pi[1] = new_pi[1];
    }

    return pi[0];
}

/**
  Given the hidden state S_i, returns the expected
  loss rate in a batch. Variable "loss_prob" stores
  the loss probability in each step of the batch.
**/
double batch_loss( Parameter * param, double * loss_prob, int i )
{
    double sum;
    double pi[2], new_pi[2];
    int b;

    sum = 0.0;
    pi[0] = 1.0 - param->r[i];
    pi[1] = param->r[i];

    for( b = 0; b < param->B; b++ )
    {
        sum += pi[1];

        if( loss_prob )
            loss_prob[b] = pi[1];

        new_pi[0] = (pi[1] * param->q[i]) + (pi[0] * (1.0-param->p[i]));
        new_pi[1] = (pi[1] * (1.0-param->q[i])) + (pi[0] * param->p[i]);

        pi[0] = new_pi[0];
        pi[1] = new_pi[1];
    }

    return sum/param->B;
}

/**
  Calculates the probability distribution of the number of losses
  observed on the next f packets transmited from the t-th
  observation. This distribution is stored in vector l.
**/
static void loss_rate_distribution( double * l, double f,
                                    double r, double p, double q )
{
    double A[2][2], B[2][2], pi[2];
    double ** R, ** M;
    int i, j, k, x;

    pi[0] = 1-r;
    pi[1] = r;

    A[0][0] = 1-p;
    A[0][1] = p;
    A[1][0] = q;
    A[1][1] = 1-q;

    B[0][0] = B[1][1] = 1;
    B[0][1] = B[1][0] = 0;

    /* get distribution conditioned on state */
    R = create_zero_matrix( 2, f+1 );
    M = create_zero_matrix( 2, f+1 );

    /* R = B */
    for( i = 0; i < 2; i++ )
        for( j = 0; j < 2; j++ )
            R[i][j] = B[i][j];

    for( k = 2; k <= f; k++ )
    {
        /* M = A R_{k-1} */
        for( i = 0; i < 2; i++ )
            for( x = 0; x < k; x++ )
            {
                M[i][x] = 0.0;
                for( j = 0; j < 2; j++ )
                    M[i][x] += A[i][j] * R[j][x];
            }

        /* R_k = sum_x  */
        for( i = 0; i < 2; i++ )
            for( j = 0; j <= k; j++ )
            {
                R[i][j] = 0.0;
                for( x = 0; x < 2; x++ )
                    R[i][j] += M[i][j-x] * B[i][x];
            }
    }

    for( i = 0; i <= f; i++ )
        l[i] = pi[0] * R[0][i] + pi[1] * R[1][i];

    destroy_matrix( 2, f+1, R );
    destroy_matrix( 2, f+1, M );
}

/**
  For each hidden state S_i, calculates the probability
  distribution of the number of losses observed 
  in the batch emitted by this hidden state.
**/
void get_aggregate_matrix( Parameter * param, double ** B )
{
    int i;

    for( i = 0; i < param->N; i++ )
        loss_rate_distribution( B[i], param->B,
                                param->r[i], param->p[i], param->q[i] );

}
