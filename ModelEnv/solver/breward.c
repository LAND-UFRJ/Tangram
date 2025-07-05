/*
 * Copyright (C) 1999-2009, Edmundo Albuquerque de Souza e Silva.
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE.QPL included in the packaging of this file.
 *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */


/****************************************************************************/
/*                                                                          */
/* rewards_alg1.c: algoritmo proposto pelo Sericola                         */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "sparseMatrix.h"

#define  MAXSTRING       255
#define  DEBUG_INFO      0
#define  PROGRESS_INFO   1

/* function defined in other modules */
extern void   lo_bound();                 /* module: bounds.c */
extern int    partial_poisson_sum();      /* module: bounds.c */

/****************************************************************************/
void debug_msg( char *msg )
{
   if( DEBUG_INFO )
   {
      fprintf( stdout, "%s\n", msg );
      fflush( stdout );
   }
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
void calculate_n_0_k_0(num_states, num_rew_rates, N, last_state, vector_b2, pi)
int    num_states;                /* number of states */
int    num_rew_rates;             /* number of reward rates */
int    N;                         /* upper limit */
int    *last_state;               /* the last state for a given reward */
double *vector_b2;                /* vector for cumulative values */
double *pi;                       /* initial state probs */
{

    int  n_k;                     /* index */
    int  s, j;

    for( j = 1; j < num_rew_rates; j++ )
    {
         n_k = (j - 1) * (N + 1) * num_states;

         /* calcule U_j */
         for( s = 1; s <= last_state[num_rew_rates - j - 1]; s++ )
             vector_b2[n_k + s - 1] = pi[ s - 1 ];

         /* calculate D_j */
         for( s = last_state[num_rew_rates - j - 1] + 1; s <= num_states; s++ )
             vector_b2[n_k + s - 1] = 0;
     } 
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
void calculate_k_0_U_j(n, j, num_states, num_rew_rates, N, last_state, vector_b2, 
                       pi_n, lb)
int    n;                         /* step number */
int    j;                         /* reward number */
int    num_states;                /* number of states */
int    num_rew_rates;             /* number of reward rates */
int    N;                         /* upper limit */
int    *last_state;               /* the last state for a given reward */
double *vector_b2;                /* vector for cumulative values */
double *pi_n;                     /* state probs after n transitions */
int     lb;                       /* index of reward associated with lower bound */
{
    int  s;                       /* temporary variables */
    int  i1, i2;                  /* index */

    if( j == lb )
    {
        i1 = (j - 1) * (N + 1) * num_states;
        for( s = 1; s <= last_state[num_rew_rates - j - 1]; s++ )
            vector_b2[i1 + s - 1] = pi_n[s - 1];
    }
    else
    {
        i1 = (j - 1) * (N + 1) * num_states;
        i2 = (j - 2) * (N + 1) * num_states + n * num_states;
        for( s = 1; s <= last_state[num_rew_rates - j - 1]; s++ )
            vector_b2[i1 + s - 1] = vector_b2[i2 + s - 1];
    }
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
void calculate_U_j(n, j, num_states, num_rew_rates, N, last_state, 
                   rew_rates_vector, vector_b1, vector_b2, P)
int    n;                         /* step number */
int    j;                         /* reward number */
int    num_states;                /* number of states */
int    num_rew_rates;             /* number of reward rates */
int    N;                         /* upper limit */
int    *last_state;               /* the last state for a given reward */
double *rew_rates_vector;         /* vector with reward values */
double *vector_b1;                /* vector for cumulative values */
double *vector_b2;                /* vector for cumulative values */
Matrix *P;                        /* trans. prob. matrix */
{
    int    k, s1, s2;             /* temporary variables */
    int    l;                     /* indices */
    int    n_k, n_k_1;            /* indices */
    double r_l, r_j, r_j_1;       /* rewards */
    double prob;                  /* trans. probability */
    double sum;                   /* temporary variables */

    r_j   = rew_rates_vector[j];
    r_j_1 = rew_rates_vector[j - 1];
    for( k = 1; k <= n; k++ )
    {
        n_k   = (j - 1) * (N + 1) * num_states + k * num_states;
        n_k_1 = (j - 1) * (N + 1) * num_states + (k-1) * num_states;

        for( s1 = 1; s1 <= last_state[num_rew_rates -j -1]; s1++ )
        {
            for( l = 0; s1 > last_state[ l ]; l++ );
                r_l = rew_rates_vector[ num_rew_rates - l - 1 ];

            sum = 0;
            for( s2 = 1; s2 <= num_states; s2++ )
            {
                prob = get_matrix_position( s2, s1, P );
                sum += prob * vector_b1[ n_k_1 + s2 - 1 ];
            }

            vector_b2[n_k + s1 - 1] =
                ((r_l - r_j) / (r_l - r_j_1)) * vector_b2[n_k_1 + s1 - 1] +
                ((r_j - r_j_1) / (r_l - r_j_1)) * sum;
        }
    }
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
void calculate_k_n_D_j( n, j, num_states, num_rew_rates, N, last_state,  
                        vector_b2, ub )
int    n;                         /* step number */
int    j;                         /* reward number */
int    num_states;                /* number of states */
int    num_rew_rates;             /* number of reward rates */
int    N;                         /* upper limit */
int    *last_state;               /* the last state for a given reward */
double *vector_b2;                /* vector for cumulative values */
int     ub;                       /* index of reward associated with upper bound */
{
    int  s;                       /* temporary variables */
    int  i1, i2, n_k;             /* indices */

    if( j == ub )
    {
        n_k = (j - 1) * (N + 1) * num_states + n * num_states;

        for( s = last_state[num_rew_rates - j - 1] + 1; s <= num_states; s++ )
           vector_b2[n_k + s - 1] = 0;
    }
    else
    {
        i1 = (j - 1) * (N + 1) * num_states + n * num_states;
        i2 = j * (N + 1) * num_states;

        for( s = last_state[num_rew_rates - j - 1] + 1; s <= num_states; s++ )
           vector_b2[i1 + s - 1] = vector_b2[i2 + s - 1];
    }
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
void calculate_D_j(n, j, num_states, num_rew_rates, N, last_state, 
     rew_rates_vector, vector_b1, vector_b2, P)
int    n;                         /* step number */
int    j;                         /* reward number */
int    num_states;                /* number of states */
int    num_rew_rates;             /* number of reward rates */
int    N;                         /* upper limit */
int    *last_state;               /* the last state for a given reward */
double *rew_rates_vector;         /* vector with reward values */
double *vector_b1;                /* vector for cumulative values */
double *vector_b2;                /* vector for cumulative values */
Matrix *P;                        /* trans. prob. matrix */
{
    int    k, s1, s2;             /* temporary variables */
    int    l;                     /* indices */
    int    n_k, n_k_1;            /* indices */
    double r_l, r_j, r_j_1;       /* rewards */
    double prob;                  /* trans. probability */
    double sum;                   /* temporary variables */

    r_j   = rew_rates_vector[ j ];
    r_j_1 = rew_rates_vector[ j - 1 ];

    for( k = n - 1; k >= 0; k-- )
    {
        n_k   = (j - 1) * (N + 1) * num_states + k * num_states;
        n_k_1 = (j - 1) * (N + 1) * num_states + (k+1) * num_states;

        for( s1 = last_state[num_rew_rates -j -1] + 1; s1 <= num_states; s1++ )
        {
            for( l = 0; s1 > last_state[l]; l++ );
                r_l = rew_rates_vector[num_rew_rates - l - 1];

            sum = 0;
              
            for( s2 = 1; s2 <= num_states; s2++ )
            {
                 prob = get_matrix_position( s2, s1, P );
                 sum += prob * vector_b1[ n_k + s2 - 1 ]; 
            }

            vector_b2[n_k + s1 - 1] =
                ((r_j_1 - r_l) / (r_j - r_l)) * vector_b2[n_k_1 + s1 - 1] +
                ((r_j - r_j_1) / (r_j - r_l)) * sum;
        }
    }
}
/****************************************************************************/
/*                                                                          */
/*  this routine calculate vectors                                          */
/*                                                                          */
/****************************************************************************/
void calculate_vectors( n, num_states, num_rew_rates, N, last_state,
                        rew_rates_vector, vector_b1, vector_b2, P, pi_n, lb, ub)
int       n;                        /* step number */
int       num_states;               /* number of states */
int       num_rew_rates;            /* number of reward rates */
int       N;                        /* upper limit */
int      *last_state;               /* the last state for a given reward */
double   *rew_rates_vector;         /* vector with reward values */
double   *vector_b1;                /* vector for cumulative values */
double   *vector_b2;                /* vector for cumulative values */
Matrix   *P;                        /* trans. prob. matrix */
double   *pi_n;                     /* state probs after n transitions */
int       lb, ub;                   /* indexes of rewards associated with bounds */

{
    int  j;                         /* reward number */

    if( n == 0 )
       calculate_n_0_k_0( num_states, num_rew_rates, N, last_state, vector_b2, 
                          pi_n );
    else
    {
        /* calcule U_j */
        for( j = lb; j < num_rew_rates; j++ )
        {
            calculate_k_0_U_j( n, j, num_states, num_rew_rates, N, last_state,
                               vector_b2, pi_n, lb );
            calculate_U_j( n, j, num_states, num_rew_rates, N, last_state,
                           rew_rates_vector, vector_b1, vector_b2, P, lb );
        }

        /* calculate D_j */
        for( j = ub; j >= lb; j-- )
        {
            calculate_k_n_D_j( n, j, num_states, num_rew_rates, N, last_state,
                               vector_b2, ub );
            calculate_D_j( n, j, num_states, num_rew_rates, N, last_state,
                           rew_rates_vector, vector_b1, vector_b2, P );
        }
    }
}
/****************************************************************************/
/*                                                                          */
/*  Algorithm :                                                             */
/*   - only reward rates are present in the model                           */
/*                                                                          */
/****************************************************************************/
double *cumulative_reward_distribution(num_rew_rates, timeout, lb, ub, lambda, 
                                       rew_rates_vector, last_state, pi, P,
                                       epsilon)

int     num_rew_rates;             /* number of rewards */
double  timeout;                   /* time value */
int     lb, ub;                    /* indexes of rewards associated with bounds */
double  lambda;                    /* uniformization rate */
double *rew_rates_vector;          /* vector with reward values */
int    *last_state;                /* the last state for a given reward */
double *pi;                        /* ptr to steady state probabilities */
Matrix *P;                         /* ptr to probability matrix */
double  epsilon;                   /* error upperbound in summation */

{
   int     j, k, n, s, n_k;        /* temporary variables */
   int     num_states;             /* number of states */
   int     N;                      /* upper limit */
   int     N_min;                  /* lower limit */
   double  factor;                 /* poisson sum */                        
   double *prob;                   /* probability */
   double  sum;                    /* temporary variable */
   double *vector_b1, *vector_b2;  /* vectores for cumulative values */
   double *vector_b3;              /* vectores for cumulative values */
  
   double *pi_n_1, *pi_n;          /* state probs after (n-1) and n transitions */
   double *aux;

   char    method[100];

   strcpy(method, "Bounded Cumulative Reward");

   /* number of states */
   num_states = P->num_col;

   /* calculate N and N_min */
   N = partial_poisson_sum( lambda * timeout, epsilon );
   lo_bound( &N_min, &factor, lambda * timeout );

   vector_b1 = (double *) malloc ( (N+1) * (num_states) * (num_rew_rates+1) * sizeof(double));
   vector_b2 = (double *) malloc ( (N+1) * (num_states) * (num_rew_rates+1) * sizeof(double));
   prob      = (double *) malloc (num_rew_rates * sizeof(double));
   pi_n_1    = (double *) malloc (num_states * sizeof(double));
   pi_n      = (double *) malloc (num_states * sizeof(double));

    if( (vector_b1 == NULL) || (vector_b2 == NULL) || (prob == NULL) ||
        (pi_n_1 == NULL) || (pi_n == NULL) )
    {
        fprintf( stderr, "%s: Could not allocate data structures\n", method );
        fprintf( stderr, "Some information:\n" );
        fprintf( stderr, "  Uniformization rate: %.5e\n", lambda );
        fprintf( stderr, "  Timeout: %.5e\n", timeout );
        fprintf( stderr, "  Error bound : %.5e\n", epsilon );
        fprintf( stderr, "  Truncation point: %d\n", N );
        fprintf( stderr, "  N_min : %d\n", N_min );
        exit( 40 );
    }

    for( s = 0; s < num_states; s++ )
        pi_n[ s ] = pi[ s ];
    for( j = 0; j < num_rew_rates; j++ )
        prob[ j ] = 0;

    for( n = 0; n <= N; n++ )
    {
        if( PROGRESS_INFO )
        {
            if( n % 10 == 0 )
                printf( " (%d/%d)\n", n, N );
            else
                printf (".");
            fflush(stdout);
        }

        calculate_vectors( n, num_states, num_rew_rates, N, last_state,
                       rew_rates_vector, vector_b1, vector_b2, P, pi_n, lb, ub);
        aux    = pi_n;
        pi_n   = pi_n_1;
        pi_n_1 = aux;
        pi_n   = vector_matrix_multiply( pi_n_1, P, pi_n );

        for( k = lb; k <= ub+1; k++ )
        {
            sum = 0;
            n_k = (k - 1) * num_states * (N + 1);
            for( s = 0; s < num_states; s++ )
                sum += vector_b2[ n_k + s ];

            if( n >= N_min )
                prob[ k - 1 ] += factor * sum;
        }

        if( n >= N_min )
           factor = (factor * (lambda * timeout)) / (n + 1);

        vector_b3 = vector_b1;
        vector_b1 = vector_b2;
        vector_b2 = vector_b3;
   }

   return( prob );
}

/****************************************************************************/
/*                                                                          */
/*  Algorithm :                                                             */
/*   - Calculate de expected value of the period above a given              */
/*    reward level.                                                         */
/*                                                                          */
/****************************************************************************/
double *expected_period( num_rew_rates, timeout, lb, ub, lambda, 
                         rew_rates_vector, last_state, pi, P, epsilon )

int     num_rew_rates;            /* number of rewards */
double  timeout;                  /* time value */
int     lb, ub;                   /* indexes of rewards associated with bounds */
double  lambda;                   /* uniformization rate */
double *rew_rates_vector;         /* vector with reward values */
int    *last_state;               /* the last state for a given reward */
double *pi;                       /* ptr to steady state probabilities */
Matrix *P;                        /* ptr to probability matrix */
double  epsilon;                  /* error upperbound in summation */
{
    int     j, k, n, s, n_k;       /* temporary variables */
    int     num_states;            /* number of states */
    int     N_min, N;              /* infinite series truncation points */
    double  pfactor, efactor;      /* poisson factor */ 
    double *prob;                  /* probability */
    double  sum;                   /* temporary variable */

    double *vector_b1, *vector_b2; /* vectors for cumulative values */

    double *pi_n_1, *pi_n;         /* state probs after (n-1) and n transitions */
    double *aux;

    char    method[100];

    strcpy( method, "Fraction of time the accumulated reward is above a level");

    /* number of states */
    num_states = P->num_col;

    /* calculate N and N_min */
    N = partial_poisson_sum( lambda * timeout, epsilon );
    lo_bound( &N_min, &pfactor, lambda * timeout );
    efactor = 1.0 - pfactor;

    vector_b1 = (double *) malloc ((N+1) * num_states * (num_rew_rates+1) * sizeof(double));
    vector_b2 = (double *) malloc ((N+1) * num_states * (num_rew_rates+1) * sizeof(double));
    prob      = (double *) malloc (num_rew_rates * sizeof(double));
    pi_n_1    = (double *) malloc (num_states * sizeof(double));
    pi_n      = (double *) malloc (num_states * sizeof(double));

    if( (vector_b1 == NULL) || (vector_b2 == NULL) || (prob == NULL) ||
        (pi_n_1 == NULL) || (pi_n == NULL) )
    {
        fprintf( stderr, "%s: Could not allocate data structures\n", method );
        fprintf( stderr, "Some information:\n" );
        fprintf( stderr, "  Uniformization rate: %.5e\n", lambda );
        fprintf( stderr, "  Timeout: %.5e\n", timeout );
        fprintf( stderr, "  Error bound : %.5e\n", epsilon );
        fprintf( stderr, "  Truncation point: %d\n", N );
        fprintf( stderr, "  N_min : %d\n", N_min );
        exit( 40 );
    }

     /* initialize vectors */
    for( s = 0; s < num_states; s++ )
       pi_n[ s ] = pi[ s ];
    for( j = 0; j < num_rew_rates; j++ )
       prob[ j ] = 0;

    for( n = 0; n <= N; n++ )
    {
        if( PROGRESS_INFO )
        {
            if( n % 10 == 0 )
                printf( " (%d/%d)\n", n, N );
            else
                printf( "." );
            fflush( stdout );
        }

        calculate_vectors(n, num_states, num_rew_rates, N, last_state,
                           rew_rates_vector, vector_b1, vector_b2, P, pi_n, lb, ub);
        aux    = pi_n;
        pi_n   = pi_n_1;
        pi_n_1 = aux;
        pi_n   = vector_matrix_multiply( pi_n_1, P, pi_n );

        for( k = lb; k <= ub+1; k++ )
        {
            n_k = (k - 1) * (N + 1) * num_states;
            sum = 0;
            for( s = 0; s < num_states; s++ )
                sum += vector_b2[ n_k + s ];

            prob[k-1] += efactor * sum;
        }

        if( n >= N_min )
        {
            pfactor  = (pfactor * (lambda * timeout)) / (n + 1);
            efactor -= pfactor;
        }
        
        aux = vector_b1;
        vector_b1 = vector_b2;
        vector_b2 = aux;
    }

    for( k = lb; k <= ub + 1; k++ )
        prob[ k - 1 ] /= lambda;

    return( prob );
}

int introduce_reward( rew_value, num_rew_rates, rew_rates_vector, last_state )
double     rew_value;
int       *num_rew_rates;
double    *rew_rates_vector;
int       *last_state;
{
    int  i, j;

    /* Introduces a new reward rate, if it doesn`t exit */
    if( rew_value < rew_rates_vector[ 0 ] )
        return( -1 );
    if( rew_value > rew_rates_vector[ *num_rew_rates - 1 ] )
        return( -1 );

    i = 0;
    while( rew_rates_vector[ i ] < rew_value )
        i++;

    if( rew_rates_vector[ i ] == rew_value )
        /* already exists, do nothing */ 
        return (i);
    else
    {
        for( j = *num_rew_rates; j > i; j-- )
            rew_rates_vector[ j ] = rew_rates_vector[ j - 1 ];
        for( j = *num_rew_rates; j > *num_rew_rates - i; j-- )
            last_state[ j ] = last_state[ j - 1 ];

        rew_rates_vector[ i ] = rew_value;
        last_state[*num_rew_rates - i] = last_state[*num_rew_rates - i - 1];

        (*num_rew_rates)++;

        return( i );
    }
}

int shift_reward( index, flag, num_rew_rates, rew_rates_vector, last_state )

int     index;
int     flag;
int    *num_rew_rates;
double *rew_rates_vector;
int    *last_state;
{
    int i, ri, li;

    if( flag < 0 )
    {
        if( (index - abs( flag )) < 0 )
            return( -1 );
        ri = index - abs( flag ) + 1;
        li = *num_rew_rates - index - 1;
    }
    else if( flag > 0 )
    {
        if( (index + abs( flag )) >= *num_rew_rates )
            return (-1);
        ri = index;
        li = *num_rew_rates - index - abs( flag ) - 1;
    }
    else
        return( 1 );

    for( i = ri; i < *num_rew_rates - abs( flag ); i++ )
        rew_rates_vector[i] = rew_rates_vector[ i + abs( flag ) ];

    for( i = li; i < *num_rew_rates - abs( flag ); i++ )
        last_state[ i ] = last_state[ i + abs( flag ) ];
 
    *num_rew_rates = *num_rew_rates - abs( flag );

    return( 1 );
}

/**************************************************************************/
/* main program                                              		  */
/**************************************************************************/
int breward(char *modelname, int option, double precision, char *method)
{
    char     filename[MAXSTRING];
    char     mat_type;               /* matrix type */
    int      i, j, l, pos;              /* temporary variables */
    int      num_rew_rates;          /* number of rewards associated to states */
    int      num_rew_rates_aux;
    int      num_rew_level;          /* number of reward levels */
    int      num_states;             /* number of states of a reward */
    int      state;                  /* state number */
    double   prob;                   /* transaction probability */
    double   timeout;                /* time value */
    int      no_intervals;
    double  *intervals;
    double   reward;                 /* reward value */
    double  *accum_rew;              /* accumulated reward (NOT a rate) */
    double   lambda;                 /* uniformization rate */
    int     *last_state;             /* the last state for a given reward */
    int     *last_state_aux;
    double  *rew_rates_vector;       /* vector with reward rates */
    double  *rew_rates_vector_aux;
    double  *pi;                     /* ptr to steady state probabilities */
    FILE    *f_in;                   /* ptr to in file */
    FILE    *f_out;                  /* ptr to out file */
    Matrix  *M;                      /* ptr to input matrix */
    Matrix  *P;                      /* ptr to probabilities matrix */
    double  *result;                 /* ptr to double */
    char     lower_bound[MAXSTRING]; /* lower bound to cumulative reward */
    char     upper_bound[MAXSTRING]; /* upper bound to cumulative reward */
    int      lb, ub;                 /* indexes of rewards associated with bounds */
    int      opt;
    float    val;

    f_out = NULL;
    sprintf( filename, "%s.reward_input", modelname );
    /* read input matrix */
    if( (f_in = fopen( filename, "r" )) == NULL )
    {
        fprintf(stderr, "%s: Error while opening file %s\n", method, filename);
        perror( "fopen" );
        return( 30 );
    }

    /* Read transition rate matrix */
    M = get_trans_matrix(f_in);
    debug_msg("Transition matrix read");

    /* Uniformize rate matrix */
    mat_type = 'Q';
    generate_diagonal(mat_type, M);
    P = uniformize_matrix(M, &lambda);

    /* Number of states */
    num_states = P->num_col;

    /* Read number of reward rates */
    fscanf( f_in, "%d", &num_rew_rates );
    if( num_rew_rates <= 0 )
    {
        fprintf(stderr, "%s: error - number of reward rates should be greater than zero\n", method);
        return( 54 );
    }

    /* allocate vectors */
    rew_rates_vector     = (double *)my_calloc(num_states+50, sizeof(double));
    rew_rates_vector_aux = (double *)my_calloc(num_states+50, sizeof(double));
    last_state           = (int    *)my_calloc(num_states+50, sizeof(int));
    last_state_aux       = (int    *)my_calloc(num_states+50, sizeof(int));
    pi                   = (double *)my_calloc(num_states,  sizeof(double));

    /* Read reward values */
    for( i = 0; i < num_rew_rates; i++ )
    {
        fscanf( f_in, "%lf", &reward );
        rew_rates_vector[ i ] = reward;
        if( i > 0 )
            if( rew_rates_vector[ i ] <= rew_rates_vector[ i - 1 ] )
            {
                fprintf( stderr, "%s: error - reward rates should be in crescent order\n", method);
                return( 54 );
            }
    }
    debug_msg( "Reward vector read" );

    /* Read last state vector */
    for( i = 0; i < num_rew_rates; i++ )
    {
        fscanf( f_in, "%d", &state );
        last_state[ i ] = state; 
        if( state > num_states || state < 1)
        {
            fprintf(stderr, "%s: error - last state is out of limits\n", method);
            return( 57 );
        }

        if( i > 0 )
            if( last_state[i] < last_state[ i - 1 ] )
            {
                fprintf(stderr, "%s: error - last state is not crescent\n", method);
                return( 57 );
            }
    }
    debug_msg( "Last state vector read" );

    /* Read initial probabilities */
    fscanf( f_in, "%d %lf", &state, &prob );
    while( state > 0 )
    {
        if( prob < 0 || prob > 1 )
        {
            fprintf( stderr, "%s: error - probability less than zero or greater than 1\n", method);
            return( 55 );
        }
        pi[ state - 1 ] = prob;
        fscanf( f_in, "%d %lf", &state, &prob );
    }
    debug_msg( "Initial probabilities read" );

    /* Read intervals */
    fscanf( f_in, "%d", &no_intervals );
    intervals = (double *) malloc( no_intervals * sizeof( double ) );
    for( i = 0; i < no_intervals; i++ )
    {
        fscanf( f_in, "%f\n", &val );
        intervals[ i ] = val;
    }
    debug_msg( "Intervals read" );

    /* Read reward levels */
    fscanf( f_in, "%d", &num_rew_level );
    accum_rew = (double *)my_calloc( num_rew_level, sizeof( double ) );
    for( i = 0; i < num_rew_level; i++ )
    {
        fscanf(f_in, "%f", &val);
        accum_rew[i] = val;
    } 
    debug_msg("Reward levels read");

    /* Read bounds to cumulative reward */
    fscanf (f_in, "%s %s", lower_bound, upper_bound);
    debug_msg("Bounds read");

    /* Close file */
    fclose(f_in);


    for( i = 0; i < no_intervals; i++ )
    {
        timeout = intervals[ i ];
      
        num_rew_rates_aux = num_rew_rates;
        /* Warning: The program is restricted to 48 levels, because of the
           allocation of the rew_rates_vector */
        for (j = 0; j < num_states+50; j++)
        {
            rew_rates_vector_aux[j] = rew_rates_vector[j];
            last_state_aux[j] = last_state[j];
        }

        /* if lower bound is defined, then                */
        /* introduce an artificial reward for lower bound */
        if( strcmp( lower_bound, "INF" ) )
        {
            val = atof(lower_bound)/timeout;
            pos = introduce_reward (val, &num_rew_rates_aux, 
                                         rew_rates_vector_aux, last_state_aux);

            if( pos < 0 )
            {
                fprintf(stderr, "%s: Invalid lower bound: %.5e\n", method, atof(lower_bound));
                fprintf(stderr, "%s: Value must be between %.5e and %.5e\n", method,
                        rew_rates_vector_aux[0] * timeout, 
                        rew_rates_vector_aux[num_rew_rates_aux-1] * timeout);
                return( 57 );
            }
            else
                lb = pos + 1;
        }
        else  
            lb = 1;
        /* Note: lb is the index of the reward that is greater than 
           "lower bound/timeout" */

        /* if upper bound is defined, then                 */
        /* introduce an artificial reward for upper bound  */
        if (strcmp(upper_bound, "INF"))
        {
            val = atof( upper_bound ) / timeout;
            pos = introduce_reward( val, &num_rew_rates_aux, 
                                    rew_rates_vector_aux, last_state_aux );
            if( pos < 1 )
            {
                fprintf(stderr, "%s: Invalid upper bound: %.5e\n", method, atof(upper_bound));
                fprintf(stderr, "%s: Value must be between %.5e and %.5e when time = %.5e since you cannot accumulate enough reward to achieve the bound by that time\n",
                        method, rew_rates_vector_aux[0] * timeout,
                        rew_rates_vector_aux[num_rew_rates_aux-1] * timeout, timeout);
                return( 57 );
            }
            else
                ub = pos;
        } 
        else
            ub = num_rew_rates_aux - 1;
        /* Note: up is the index of the reward that is equal to 
           "upper bound/timeout" */
        /* lower_bound != INF and upper_bound != INF */
        if( strcmp(lower_bound, "INF") != 0 && strcmp(upper_bound, "INF") != 0 )
        {
            /* Note that, because of the definition of the indeces lb and ub,
               they MUST be equal at this point, in order to satisfy the
               restrictions */
            if( lb < ub )
            {
                fprintf( stdout, "\n%s: Restriction violation\n", method );
                fprintf( stdout, "The following rewards are between bounds (%.3e,%.3e):\n",
                         rew_rates_vector_aux[lb-1], rew_rates_vector_aux[ub]);
                for( l = lb; l < ub; l++ )
                    fprintf( stdout, "%.5f\n", rew_rates_vector_aux[ l ] );

                fprintf( stdout, "Choose one option:\n" );
                fprintf( stdout, "   [0] - Cancel\n" );
                fprintf( stdout, "   [1] - Calculate lower bound on measure by shifting rewards, so that restrictions are satisfied \n");
                fprintf( stdout, "   [2] - Calculate upper bound on measure by shifting rewards, so that restrictions are satisfied  \n");
                fprintf( stdout, "Option: " );
                fflush( stdout );
                scanf( "%d", &opt );

                if( opt == 0 )
                    return ( 11 );
                else if( opt == 1 )
                {
                    shift_reward( ub - 1, -(ub - lb), &num_rew_rates_aux,
                                  rew_rates_vector_aux, last_state_aux );
                    ub -= (ub - lb);
                }
                else if( opt == 2 )
                {
                    shift_reward( lb, (ub - lb), &num_rew_rates_aux,
                                  rew_rates_vector_aux, last_state_aux );
                    ub -= (ub - lb);
                }
                else
                {
                    fprintf( stderr, "\n%s: Invalid option!\n", method );
                    return( 58 );
                }
                fprintf( stdout,"%d\n", opt );
            }
        }

        if( strcmp(lower_bound, "INF") == 0 && strcmp(upper_bound, "INF") == 0 )
            ; /* nothing to do */
 
        if( (strcmp(lower_bound, "INF") != 0 && strcmp(upper_bound, "INF") == 0) ||
            (strcmp(lower_bound, "INF") == 0 && strcmp(upper_bound, "INF") != 0) )
        {
            fprintf( stdout, "Current version does not handle this case yet.\n\
                     Please provide both limits to the reward or no limits." );
            exit( 1 );
        }

        /* Introduce an artificial reward for each reward level */
        for( j = 0; j < num_rew_level; j++ )
        {
            val = accum_rew[j]/timeout;
            if( (val <= rew_rates_vector_aux[ lb - 1 ]) ||
                (val >= rew_rates_vector_aux[ ub ]) )
            {
                fprintf( stderr, "%s: Invalid reward level: %.5f\n",
                         method, accum_rew[ j ] );
                fprintf( stderr, "%s: Value must be > %.5f and < %.5f\n",
                         method, timeout * rew_rates_vector_aux[ lb - 1 ],
                         timeout * rew_rates_vector_aux[ ub ] );
                return( 57 );
            }

            pos = introduce_reward( val, &num_rew_rates_aux,
                                    rew_rates_vector_aux, last_state_aux );
            ub++;
        }

        if( option == 1 )
        {
            sprintf( filename, "%s.TS.brew.cumulat_distrib.%.5e",
                     modelname, timeout );
            if( (f_out = fopen( filename, "w" )) == NULL )
            {
                fprintf( stderr, "%s: Error while opening file %s\n",
                         method, filename );
                perror( "fopen" );
                return( 31 );
            }
        }

        if( option == 2 )
        {
            sprintf( filename, "%s.TS.brew.expected_period.%.5e",
                     modelname, timeout );
            if( (f_out = fopen( filename, "w" )) == NULL )
            {
                fprintf( stderr, "%s: Error while opening file %s\n",
                         method, filename );
                perror( "fopen" );
                return( 31 );
            }
        }

        /* Cumulative Reward Distribution */
        if( option == 1 )
        {
            result = cumulative_reward_distribution(num_rew_rates_aux, timeout, 
                    lb, ub, lambda, rew_rates_vector_aux, last_state_aux, pi, P, precision);
 
            fprintf (f_out, "#Lower bound: %.5f\n#Upper bound: %.5f \n",
                  timeout * rew_rates_vector_aux[lb-1], timeout * rew_rates_vector_aux[ub]);
            fprintf (f_out, "#Prob[Cumulated_Reward(time = %.8f) > \"1st_column\"] = \"2nd_column\"\n",
                                                                                           timeout);

            for( j = lb; j <= ub + 1; j++ ) 
                fprintf(f_out, "%.8f %.8e %.8f\n", rew_rates_vector_aux[j-1]*timeout, result[j-1],
                                                                                     timeout);
            fprintf( f_out, "\n" );
        }

        /* E[period over a reward level] */
        if( option == 2 )
        { 
            result = expected_period( num_rew_rates_aux, timeout, lb, ub,
                                      lambda, rew_rates_vector_aux,
                                      last_state_aux, pi, P, precision );

            fprintf( f_out, "#Lower bound: %.5f\n#Upper bound: %.5f \n",
                  timeout * rew_rates_vector_aux[lb-1], timeout * rew_rates_vector_aux[ub]);
            fprintf( f_out, "#E[time fraction over reward level \"1st_column\"] = \"2nd_column\"\n");
            fprintf( f_out, "#Time = %.8f\n", timeout);

            for( j = lb; j <= ub + 1; j++ )
                fprintf(f_out, "%.8e %.8e %.8f\n", rew_rates_vector_aux[j-1]*timeout,
                                                   result[j-1]/timeout, timeout);
            fprintf( f_out, "\n" );
        }
        fclose( f_out );
    }

    return( 0 );
}
