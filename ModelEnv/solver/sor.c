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


#include <math.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "sparseMatrix.h"

#define SOR_THRESHOLD   20

int sor_converged();


/************************************************************************/
/* 									*/
/*                                                                      */
/*      max1(double a, double b, int n):				*/
/*		infinite norm of vector subtraction			*/
/*                                                                      */
/************************************************************************/
double max1(a, b, n)
double *a;				/* solution vector 1  */
double *b;				/* solution vector 2  */
int n;					/* vectors dimension  */
{
	int i;
	double dif;			/* current greatest value */
	
	
	dif = 0;
	
	for(i = 0; i < n; i++) 
		if (dif < ABS(a[i]-b[i])) 
		    dif = ABS(a[i]-b[i]);
		    
	return (dif);
}


/***********************************************************************/
/*								       */
/*	opt_w(double w, double teta):				       */
/*		evaluates optimum sor parameter			       */
/*								       */
/***********************************************************************/
double opt_w( w, teta)
double w;			/* last sor parameter            */
double teta;			/* estimate of convergence rate  */
{
	double mi;			/* subdominant eigenvalue of T_w */
	double w_o;			/* optimum parameter             */
	double den1;
	double den2;


	den1 = w * sqrt (teta);
	
	if(den1 == 0)
	{
	  fprintf(stderr,"opt_w: denominator is 0!\n");
          return(-1);
 	}

	mi = (teta + w - 1) / (w * sqrt (teta));

	den2 = 1 - mi * mi;
 	if(den2 < 0)
 	{
          fprintf(stderr,"SOR method: in function opt_w, denominator is negative!\n");
          return(-1);
        }

	w_o = 2 / (1 + sqrt(1 - mi * mi));
	
	if (w >= 2)  
	{	/* sor is unstable  */
	 	perror("SOR is not stable!");
		return(-1);
	}
	
	return(w_o);
}	

/************************************************************************/
/*                                                                      */
/*	solving x A = b by SOR interative method.		        */
/*									*/
/*	Iterative equation						*/
/*	x_i^{k+1} = w * ( b_i - sum_{j=1}^{i-1} a_{ji}x_j^{k+1} 	*/
/*	            - sum{j=i+1}^N a_{ji}x_j^k ) / a_{ii}		*/
/*                  + (1-w) * x_i^{k}                                   */
/*									*/
/************************************************************************/
double *sor(A, x, b, precision, no_of_iterations,no_max_iterations,stop_condition)
double *x;	       /* pointer to a vector - contiguous memory of elements */
                       /* initially contains the first guessed solution       */
double *b;	       /* pointer to a vector - contiguous memory of elements */
double precision;      /* precision for numeric approximation                 */
Matrix *A;	       /* pointer to a matrix - an array of column pointers   */
int *no_of_iterations; /* no. of iter. to converege using precision           */
double no_max_iterations; /*no_max_iterations used in test of convergence*/
int    *stop_condition ;  /*stop_condition of the method 0 - Max_Iterations 1- Convergence */


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


	/* dim. of A, and both b and x must have N elements */
	N = A->num_col;	

	/* temp. vector with N ele. */
	lx = (double *)my_calloc(N, sizeof(double));	

	counter = 1;
	lteta_k = 0;
    w       = 1;
	while((!sor_converged(x, lx, N, precision))&&( counter < no_max_iterations)) 
	{
		if (counter <= SOR_THRESHOLD) 
		    w = 1;                      /* first SOR_THRESHOLD iter. are Gauss */
		    
		cp = (Column **)A->col_pp;	/* ptr to 1st column in A */
		delta_d = max1(x, lx, N);	/* gets greatest diference */
		bcopy(x, lx, N*sizeof(double));	/* save the last solution */
	
		/* re-calculate each element of the x */
		for (col=1; col<=N; col++) {

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
				     { /* none other than the diagonal */
				         a_ii = pcp->value;
				     }
				pcp = pcp->next;
			}

			if (!a_ii) 
			{		/* sth. impossible happens.. */
				fprintf(stderr, "panic at %d.\n", col);
				return((double *)NULL);
			}

			/* calculate by sor equation the new x_col */
			x[col-1] = (w * (-sum_pre - sum_post + b[col-1]) 
			                   / a_ii) + ((1-w) * lx[col-1]);
	
			++cp;			/* advance to next A column */
		}

		delta_u = max1(x, lx, N);
		teta_k = delta_u / delta_d;


		if (ABS(teta_k - lteta_k) < precision) 
		{
			w = opt_w(w,teta_k);         /* teta is stable; get better w */
			
			if (w == -1)                 /* err indication */
			    return((double *)NULL);
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
    
    
    
    if (*no_of_iterations == no_max_iterations)
       *stop_condition = MAX_ITERATIONS;
    else
       *stop_condition = CONVERGENCE;
       
       
	
	return (x);	/* got it */
}


