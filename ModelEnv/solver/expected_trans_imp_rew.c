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


#include <strings.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "sparseMatrix.h"
#ifdef _SOLARIS_
#include <strings.h>
#endif

/******************************************************************************/
/* This routine generate H matrix by multipling every element of R by its     */
/*    equivalent in P, i. e., H(i,j) = R(i,j)*P(i,j). The resulting matrix is */
/*    stored in R since it will be no longer needed.                          */
/* Return value:                                                              */
/*    true  : if the routine finished with success;                           */
/*    false : if it found an error.                                           */
/******************************************************************************/
static int generate_H_matrix( R, P )
Matrix *R, *P;
{
    int status;
    int col;
	Column *pp;           /* ptr to a element in a column list in R     */
	Column **pcp;         /* ptr to the addr of prev. ptr in R's column */

    status = 0;
    if( R && P && R->num_col == P->num_col )
    {
        pcp = R->col_pp;
        for( col = 1; col <= R->num_col; col++, pcp++ )
        {
            for( pp = *pcp; pp; pp = pp->next )
            {
                pp->value *= get_matrix_position( pp->index, col, P );
            }
        }
        status = 1;
    }

    return( status );
}


double *expected_trans_imp_reward( pi0, P, R, lambda, num_t_points,
                                   time_points, N_min, N_max, Min_poisson_value)
double *pi0;
Matrix *P, *R;
double lambda;
int num_t_points;
double *time_points;
int *N_min;
int *N_max;
double *Min_poisson_value;
{
    double *v_n;          /* current vector pi, i. e., pi[n]             */
    double *v_n_aux;
    double *exp_imp_t;    /* vector of expectations for each time point  */
    double *poisson_term; /* Poisson value at step n                     */
    double *vec_reward;
    double acc_mod;       /* accumulated sum of reward vector components */
    int n, i;
    

    v_n          = (double *) my_malloc(P->num_col   * sizeof(double));
    v_n_aux      = (double *) my_malloc(P->num_col   * sizeof(double));
    exp_imp_t    = (double *) my_malloc(num_t_points * sizeof(double));
    poisson_term = (double *) my_malloc(num_t_points * sizeof(double));
    vec_reward   = (double *) my_malloc(P->num_col   * sizeof(double));

    bcopy(pi0, v_n, P->num_col * sizeof(double));

    /* R(i,j)=R(i,j)*P(i,j) */
    generate_H_matrix( R, P );

    for( i = 0; i < num_t_points; i++ )
        if( N_min[ i ] == 0 )
            poisson_term[ i ] = Min_poisson_value[ i ];

    acc_mod = 0.0;
    /* the max value of N_max is that for the largest time point, i.e. */
    /* num_t_points-1.                                                 */
    for (n = 1; n <= N_max[num_t_points-1]; n++)
    {
        /**           Calculate E[ sigma(k) ]         **/
        vector_matrix_multiply( v_n,     R, vec_reward );
        /**           Calculate pi[ n ]               **/
        vector_matrix_multiply( v_n,     P, v_n_aux    );
        /**            Updating v(n)                  **/
        bcopy(v_n_aux, v_n, P->num_col * sizeof(double));

        for( i = 0; i < R->num_col; i++ )
            acc_mod += vec_reward[ i ];

        for( i = 0; i < num_t_points; i++ )
        {
            if( n >= N_min[ i ] )
            {
                /**    Initializing poisson term    **/
                if( n == N_min[ i ] )
                   poisson_term[i] = Min_poisson_value[i];
                /**      Updating poisson term      **/
                if( n > N_min[ i ] && n <= N_max[ i ] )
                   poisson_term[i] *= (lambda * time_points[i])/n;

                exp_imp_t[i] += poisson_term[i] * acc_mod;
            }
        }
    }

    free((double *) v_n);
    free((double *) v_n_aux);
    free((double *) poisson_term);
    free((double *) vec_reward);

    return( exp_imp_t );
}
