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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <errno.h>
#include "bounds.h"
#include "sparseMatrix.h"

/*****************************************************************************/
/* double *expected_trans_reward(pi1, P, lambda, num_intervals, time, reward,*/
/*                               N_min, N_max, Min_poisson_val)              */
/*                                                                           */
/* Return E[CR(t)/t]                                                         */
/*****************************************************************************/
double *expected_trans_reward(pi1, P, lambda, num_t_points, time, reward, N_min, N_max, Min_poisson_value)
double *pi1;                /* ptr to initial state probabilities */
Matrix *P;                  /* ptr to probabilities matrix */
double lambda;              /* uniformization rate */
int    num_t_points;        /* number of time points */
double *time;               /* vector of time points */
double *reward;             /* ptr to reward vector */
int    *N_min;              /* ptr to low bound for each interval */
int    *N_max;              /* ptr to up bound for each interval */
double *Min_poisson_value;  /* ptr to Poisson evaluated at Nmin for a give time */

{
   int    n, i, k;      /* temporary var. */
   double rv_n;         /* result of vectors multiplication reward x v(n) */
   double f_n;          /* result of resursive function f(n) */
   double f_pre_n;      /* result of resursive function f(n-1) */
   double *v_n;         /* vector of v(n), state probab. at discrete time n */
   double *v_n_aux;     /* auxiliar ptr to transient states probabil. */
   double *exp_t;       /* vector of expectations for each time point */
   double *poisson_term; /* Poisson value at step n */

   v_n     = (double *) my_malloc(P->num_col * sizeof(double));
   v_n_aux = (double *) my_malloc(P->num_col * sizeof(double));
   exp_t   = (double *) my_malloc(num_t_points * sizeof(double));
   poisson_term = (double *) my_malloc(num_t_points * sizeof(double));

   bcopy(pi1, v_n, P->num_col * sizeof(double));

   f_pre_n = 0;
   for (n = 0; n <= N_max[num_t_points-1]; n++)
   /* the max value of N_max is that for the largest time point, i.e. 
      num_t_points-1 */
   {

       /* Vector x vector multiplication for updating r*v(n) */
       for (k = 0, rv_n = 0; k < P->num_col; k++)
           rv_n += reward[k] * v_n[k];

       /* Updating f(n) */
       if (n == 0)
           f_n = rv_n;
       else
           f_n = (n/(n + 1.0)) * f_pre_n + (1.0/(n + 1.0)) * rv_n;

       f_pre_n = f_n;

       for (i = 0; i < num_t_points; i++)
       {     
           if (n == N_min[i])
           {
              poisson_term[i] = Min_poisson_value[i];
              exp_t[i] += poisson_term[i] * f_n; 
           }
           if (n > N_min[i] && n <= N_max[i])
           {
              /* Updating poisson term */
              poisson_term[i] *= (lambda * time[i])/n;
              exp_t[i] += poisson_term[i] * f_n; 
           }
       }

       /* Updating v(n) */
       bcopy(v_n, v_n_aux, P->num_col * sizeof(double));
       vector_matrix_multiply(v_n_aux, P, v_n);
   }

   for (i = 0; i < num_t_points; i++)
       exp_t[i] *= time[i];

   free((double *) v_n);
   free((double *) v_n_aux);
   free((double *) poisson_term);

   return(exp_t);
}
