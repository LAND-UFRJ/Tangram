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


/************************************************************************/
/*                                                                      */
/*		SOR interative method routine				*/
/*		input:							*/
/*			pointer to vector x in contiguous memory,	*/
/*			pointer to vector b in contiguous memory,	*/
/*			pointer to matrix A in column-wise linked-list.	*/
/*		solve:							*/
/*			x A = b						*/
/*			( instead of A x = b, as A is column-major )	*/
/*		output:							*/
/*			pointer to final vector x in contiguous memory.	*/
/*                                                                      */
/*		assumptions:						*/
/*		A is square matrix, b and x have the correct dimension.	*/
/*									*/
/*		Joao Carlos Guedes UCLA CSD 8/19/93			*/
/*		Altered in 11/3/93 to also return no. of iterations     */
/*	        to converge. Also, now 1st parameter is the matrix!!!   */
/*									*/
/*	This is a modification on option2.c to let initial parameter w	*/
/*	be set by user, and returning the last w obtained		*/
/*									*/
/************************************************************************/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "sparseMatrix.h"
#include <string.h>
#include <strings.h>


/************************************************************************/
/*                                                                      */
/*      sor_converged(double *, double *, int):				*/
/*		checking sor convergence of two solutions.	        */
/*									*/
/* Modified in June, 1997 by Joao Abdalla to avoid zero division when 	*/
/* a[i] == 0.								*/
/*									*/ 
/* Modified in Nov, 20th, 1997 by Joao Abdalla to use Epsilon2 		*/
/* different from Epsilon, in order to have lesser precision and cost. 	*/
/*									*/
/************************************************************************/
int sor_converged_flex(a, b, n,precision)
double *a;				/* solution vector 1 */
double *b;				/* solution vector 2 */
int n;
double precision;
{
	int i;

	for (i=0; i<n; i++)
		if (ABS(a[i]-b[i]) > (ABS(a[i]) * precision)) {
#ifdef DEBUG
			printf("Over Epsilon at %d\n", i);
#endif
			return(0);
		}
	return(1);
}
/************************************************************************/
/* 									*/
/*                                                                      */
/*      max1(double a, double b, int n):				*/
/*		infinite norm of vector subtraction			*/
/*                                                                      */
/************************************************************************/
double max1_flex(a, b, n)
double *a;				/* solution vector 1  */
double *b;				/* solution vector 2  */
int n;					/* vectors dimension  */
{
	int i;
	double dif;			/* current greatest value */
	dif = 0;
	for( i=0; i<n; i++) 
		if (dif < ABS(a[i]-b[i])) 
			dif = ABS(a[i]-b[i]);
       	return (dif);
}
/***********************************************************************/
/*								       */
/*	opt_w(double w, double teta):				       */
/*		evaluates optimum sor parameter			       */
/*								       */
/*								       */
/***********************************************************************/
double opt_w_flex( w, teta)
double w;				/* last sor parameter  */
double teta;				/* estimate of convergence rate  */
{
	double mi;			/* subdominant eigenvalue of T_w */
	double w_o;			/* optimum parameter */
	double den1;
	double den2;

	den1 = w * sqrt (teta);
	if(den1 == 0){
	  fprintf(stderr,"opt_w: denominator is 0!\n");
          exit(1);
 	}

	mi = (teta + w - 1) / (w * sqrt (teta));

	den2 = 1 - mi * mi;
 	if(den2 < 0){
          fprintf(stderr,"opt_w: denominator is negative!\n");
          exit(1);
        }

	w_o = 2 / (1 + sqrt(1 - mi * mi));
	if (w >= 2)  {			/* sor is unstable  */
	 	perror("SOR is not stable!");
		exit(1);
	}
	return(w_o);
}	


/************************************************************************/
/*                                                                      */
/*      sor(Matrix *,double *, double *, int *):			*/
/*		solving x A = b by SOR interative method.	        */
/*									*/
/*	Iterative equation						*/
/*	x_i^{k+1} = w * ( b_i - sum_{j=1}^{i-1} a_{ji}x_j^{k+1} 	*/
/*			  - sum{j=i+1}^N a_{ji}x_j^k ) / a_{ii}		*/
/*                  + (1-w) * x_i^{k}                                   */
/*									*/
/* Modified on November, 29th, 1997. Changed the way w is updated, 	*/ 
/* defining wmax to avoid divergence and getting w_ini from the call	*/
/************************************************************************/
double *sor_flex(A, x, b, no_of_iterations, w_ini, w_max_ini,precision,max_no_iteration,stop_condition)
double *x;	/* pointer to a vector - contiguous memory of elements	*/
                /* initially contains the first guessed solution        */
double *b;	/* pointer to a vector - contiguous memory of elements	*/
Matrix *A;	/* pointer to a matrix - an array of column pointers	*/
int *no_of_iterations; /*no. of iter. to converege using global Epsilon */
double *w_ini;
double *w_max_ini;
double precision;
double max_no_iteration;
int *stop_condition;
{
	int col;		/* column number */
	int N;			/* matrix dimension ( number of columns ) */
	double a_ii;		/* diagonal element of the i-th column */
	double sum_pre;		/* sum for sum_{j=1}^{i-1} a_{ji}x_j^{k+1} */
	double sum_post;	/* sum for sum{j=i+1}^N a_{ji}x_j^k */
	double *lx;		/* pointer to temporary solution vector */
	Column **cp;		/* pointer of pointer to matrix columns */
	Column *pcp;		/* pointer to matrix columns */
	double w;		/* SOR parameter */
	double delta_u;  	/* ||x^{k+1} - x^{k}||oo  */
	double delta_d;		/* ||x{k} - x^{k-1}||oo  */
        double teta_k;		/* delta_u / delta_d, iterate k  */
        double lteta_k;    	/* last teta_k  */     
	int counter;		/* no. maximum Gauss_Seidel turns */
	double max1(); 		/* max absolute coordinate */
	double opt_w();		/* evaluates optimum sor parameter  */
        double Epsilon2;

	double	last_w, w_max;	
	double	stab_teta;
	int	est_count;	

        w = 0;
	Epsilon2 = precision;
        stab_teta = Epsilon2/10;
        last_w = 1;
	w_max = *w_max_ini;
	est_count = 0;


	if (!x || !b || !A )	/* nil vector or matrix is ignored */
		return((double *)NULL);

	N = A->num_col;	/* dim. of A, and both b and x must have N elements */


	if(!check_diagonal(A, N)) {	/* some disgonal element is missing */
		fprintf(stderr, "Missing diag., SOR'd topple!\n");
		return((double *)NULL);
	}

	lx =
	(double *)my_calloc(N, sizeof(double));	/* temp. vector with N ele. */
	delta_u = max1(x, lx, N);

	counter = 1;
	lteta_k = 0;
	while(!sor_converged_flex(x, lx, N,precision)&&(counter < max_no_iteration))  
	{
		if (counter<=2) w = *w_ini;     /* first 2 it. keep same w */
		cp = (Column **)A->col_pp;	/* ptr to 1st column in A */
		delta_d = delta_u;		/* get old greatest diference */
		bcopy(x, lx, N*sizeof(double));	/* save the last solution */
	
		/* re-calculate each element of the x */
		for (col=1; col<=N; col++) {

			pcp = *cp;		/* the i-th column in A */
			a_ii = 0;		/* diag. of the i-th column */
			sum_pre = 0;
			sum_post = 0;
	
			while (pcp) {
				if (pcp->index < col) {
				    sum_pre += (pcp->value)*(x[pcp->index - 1]);
				} else if (pcp->index > col) {
				    sum_post+=(pcp->value)*(lx[pcp->index - 1]);
				} else { /* none other than the diagonal */
				    a_ii = pcp->value;
				}
				pcp = pcp->next;
			}

			if (!a_ii) {		/* sth. impossible happens.. */
				fprintf(stderr, "panic at %d.\n", col);
				return((double *)NULL);
			}

			/* calculate by sor equation the new x_col */
			x[col-1] = (w * (-sum_pre - sum_post + b[col-1]) 
				      / a_ii) + ((1-w) * lx[col-1]);
	
			++cp;			/* advance to next A column */
		}
		delta_u = max1_flex(x, lx, N);
		teta_k = delta_u / delta_d;
		if (ABS(teta_k - lteta_k) < stab_teta)
			est_count++;
		else	est_count = 0;
		/* check for stabilization of teta or divergence of solution */
		if ((est_count >= 10) || ((delta_u > 1) && (delta_d < delta_u)))
		     {
			est_count = 0;	/* reset counter for stabilization */
			if (teta_k < 1)	/* solution vector is converging */
			{
				last_w = w;
				w = opt_w_flex(w,teta_k);
				if (w > w_max)
					w = (last_w + w_max)/2;
			}
			else	/* solution vector is diverging	*/
			{
				/* keep old solution (before divergence) */
				bcopy(lx, x, N*sizeof(double));
				teta_k = lteta_k;
				delta_u = delta_d;
				/* apart lx from x keeping diff. = delta_d */
				lx[0] += delta_d;
				w_max = w;		/* limit value of w */
				w = (w + last_w)/2;	/* lower w	*/
			}
		     }
		lteta_k = teta_k;
		++counter;
#ifdef DEBUG
		printf("SOR iteration:\n");
		put_vector(lx, N);
		put_vector(x, N);
#endif
	}
	*no_of_iterations = counter;
	/* w_ini will return the last value of w*/
	*w_ini = w;
	/* w_max_ini will return last value of w_max	*/
	*w_max_ini = w_max;

       if (*no_of_iterations == max_no_iteration)
       *stop_condition = MAX_ITERATIONS;
       else
       *stop_condition = CONVERGENCE;


	return (x);	/* got it */
}
