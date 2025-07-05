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
#include <string.h>
#include <strings.h>
#include "sparseMatrix.h"


int gs_converged();


/************************************************************************/
/*                                                                      */
/*      solving x A = b by Gauss-Siedel interative method.              */
/*                                                                      */
/*      Iterative equation                                              */
/*      x_i^{k+1} = ( b_i - sum_{j=1}^{i-1} a_{ji}x_j^{k+1}             */
/*                        - sum{j=i+1}^N a_{ji}x_j^k ) / a_{ii}         */
/*                                                                      */
/************************************************************************/
double *gauss_siedel(A, x, b, precision, no_of_iterations,no_max_iterations,stop_condition)
double *x;	       /* pointer to a vector - contiguous memory of elements   */
double *b;	       /* pointer to a vector - contiguous memory of elements   */
double precision;      /* precision for numeric approximation                   */
Matrix *A;	       /* pointer to a matrix - an array of column pointers     */
int *no_of_iterations; /* no_of_iterations to converge using                    */
double no_max_iterations; /*no_max_iterations used in test of convergence*/
int    *stop_condition ;  /*stop_condition of the method 0 - Max_Iterations 1- Convergence */
{
	int col;		    /* column number                           */
	int N;			    /* matrix dimension ( number of columns )  */
	double a_ii;		/* diagonal element of the i-th column     */
	double sum_pre;		/* sum for sum_{j=1}^{i-1} a_{ji}x_j^{k+1} */
	double sum_post;	/* sum for sum{j=i+1}^N a_{ji}x_j^k        */
	double *lx;		    /* pointer to temporary solution vector    */
	Column **cp;		/* pointer of pointer to matrix columns    */
	Column *pcp;		/* pointer to matrix columns               */


	N = A->num_col;
	
	lx = (double *)my_calloc(N, sizeof(double));  /* temp. vector with N ele. */

    *no_of_iterations = 0;

	while((!gs_converged(x, lx, N, precision))&&((*no_of_iterations) < no_max_iterations )) 
	{
		cp = (Column **)A->col_pp;	     /* ptr to 1st column in A */
		bcopy(x, lx, N*sizeof(double));	     /* save the last solution */
	
		/* re-calculate each element of the x */
		for (col=1; col<=N; col++) 
		{
			pcp = *cp;		/* the i-th column in A     */
			a_ii = 0;		/* diag. of the i-th column */
			sum_pre = 0;
			sum_post = 0;
	
			while (pcp) 
			{
				if (pcp->index < col) 
				{
				    sum_pre += (pcp->value)*(x[pcp->index - 1]);
				} 
				else if (pcp->index > col) 
				     {
				         sum_post+=(pcp->value)*(lx[pcp->index - 1]);
				     } 
				     else 
				     {   /* none other than the diagonal */
				         a_ii = pcp->value;
				     }
				     pcp = pcp->next;
			}

			if (!a_ii) 
			{		 /* sth. impossible happens.. */
				fprintf(stderr, "panic at %d.\n", col);
				return((double *)NULL);
			}

			/* calculate by gauss-siedel equation the new x_col */
			x[col-1] = (-sum_pre - sum_post + b[col-1]) / a_ii;
			
			++cp;			/* advance to next A column */
		}
#ifdef DEBUG
		printf("Gauss-Siedel iteration:\n");
		put_vector(lx, N);
		put_vector(x, N);
#endif
		*no_of_iterations = *no_of_iterations +1;
	}
    
    if (*no_of_iterations == no_max_iterations)
       *stop_condition = MAX_ITERATIONS;
    else
       *stop_condition = CONVERGENCE;
         
	return (x);	/* got it */
}
