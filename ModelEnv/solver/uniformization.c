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
#include <strings.h>
#include <math.h>
#include <time.h>
#include "sparseMatrix.h"
#include "bounds.h"


/****************************************************************************/
/*                                                                          */
/*  Uniformization method for transient evaluation of pi                    */
/*  see equation 6.15 page 158 in                                           */
/*  "Computational Solution Methods for Markov Chains: Applications         */
/*   to Computer and Communication Systems"                                 */
/*                                                                          */
/****************************************************************************/
int uniformization_method(num_intervals, intervals, lambda, epsilon, 
                                          N_min, N_max, pi1, pi2, P) 
int    num_intervals;             /* number of intervals           */
double *intervals;                /* vector of time intervals      */
double lambda;                    /* uniformization rate           */
double epsilon;                   /* error                         */
int    *N_min;                    /* lower limit                   */
int    *N_max;                    /* upper limit                   */
double *pi1;                      /* initial state probabilities   */
double **pi2;                     /* final state probabilities     */
Matrix *P;                        /* transition probability matrix */
{
   int    i, n, k;                /* temporary variables  */
   int    N_min2;                 /* lower limit          */
   int    N_max2 = 0;             /* upper limit          */
   int    num_states;             /* number of states     */
   double factor;                 /* sum of Poisson serie */
   double *poisson_factor;        /* sum of Poisson serie */
   double *pi_aux1, *pi_aux2;     /* probability vectores */   


   /* number of states of probability matrix */
   num_states = P->num_col;


   /* allocate auxiliar vectors */
   pi_aux1        = (double *)my_calloc(num_states, sizeof(double));        
   poisson_factor = (double *)my_calloc(num_intervals, sizeof(double));        

   bcopy(pi1, pi_aux1, num_states * sizeof(double));


   /* calculate N_min and N_max */
   for (n = 0; n < num_intervals; n++)  
   {
       /* N_max */
       N_max[n] = partial_poisson_sum(lambda * intervals[n], epsilon); 

       if (N_max[n] > N_max2)
           N_max2 = N_max[n];

       /* N_min */
       lo_bound(&N_min2, &factor, lambda * intervals[n]);
       
       N_min[n] = N_min2;
       poisson_factor[n] = factor;
   }


   for (n = 0; n <= N_max2; n++)  /* total number of transactions */
   {
       for (i = 0; i < num_intervals; i++)
       {
            
           if (n >= N_min[i] && n <= N_max[i])
           {
               pi_aux2 = pi2[i];
               
               for (k = 0; k < num_states; k++)
                   pi_aux2[k] += poisson_factor[i] * pi_aux1[k];
                   
               poisson_factor[i] = (poisson_factor[i] * lambda * intervals[i]) / (n+1);
           }
       }

       if (n < N_max2)
       {
           bcopy(pi_aux1, pi1, num_states * sizeof(double));
           
           if ((vector_matrix_multiply(pi1, P, pi_aux1)) == NULL)
               return(86);
       }
   } 

   free((char *) pi_aux1);
   free((char *) poisson_factor);
   
   return(0);
}


