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
/*	solving x A = x by power method 	                        */ 
/*      Morganna in 16/05/94                                            */
/*                                                                      */
/************************************************************************/
double *power(A, x, precision, no_of_iterations,no_max_iterations,stop_condition)
Matrix *A;	       /* pointer to sthocastic matrix - an array of column pointers   */
double *x;	       /* pointer to a vector - contiguous memory of elements          */
double precision;      /* precision for numeric approximation                          */
int *no_of_iterations; /* no_of_iterations to converge using precision                 */
double no_max_iterations; /*no_max_iterations used in test of convergence*/
int    *stop_condition ;  /*stop_condition of the method 0 - Max_Iterations 1- Convergence */

{
	int N;			/* matrix dimension ( number of columns ) */
	double *lx;             /* pointer to temporary solution vector */


	/* dim. of A, and x must have N elements */
	N = A->num_col;	

	/* temp.vect.with N ele. */
	lx = (double *)my_calloc(N, sizeof(double));

   *stop_condition = 2;     
   *no_of_iterations = 0;

	while((!gs_converged(x, lx, N, precision))&& ((*no_of_iterations) < no_max_iterations )) 
	{
	       bcopy(x, lx, N*sizeof(double));	/* save the last solution */
           vector_matrix_multiply(lx,A,x);   

#ifdef DEBUG
		printf("Power method:\n");
		put_vector(lx, N);
		put_vector(x, N);
#endif
		*no_of_iterations = *no_of_iterations +1;
	}
    
    if ((*no_of_iterations) == no_max_iterations)
       *stop_condition = MAX_ITERATIONS;
    else
       *stop_condition = CONVERGENCE;

    return (x);	/* got it */
}
