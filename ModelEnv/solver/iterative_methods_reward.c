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
#include <math.h>
#include <stdlib.h>

#include "sparseMatrix.h"


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
int sor_converged_1(a, b, n,first,precision)
double *a;				/* solution vector 1 */
double *b;				/* solution vector 2 */
int n;
int first;
double precision;
{
	int i;

	for (i=0; i<n; i++)
		if (((ABS(a[i]-b[i])) > (ABS(a[i]) * precision))||(first==1)) {
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
double max1_1(a, b, n)
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
double opt_w_1( w, teta)
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
/*      converged_reward(double *, double *,int, int, double):   	*/
/*		checking convergence of two solutions.	                */
/*									*/
/*	Only a template here.						*/
/*									*/
/************************************************************************/
int converged_reward(a, b, n,Flag, Epsilon)
double *a;				/* solution vector 1 */
double *b;				/* solution vector 2 */
int n;
int Flag;
double Epsilon;                         /* Precision for numeric approx. */
{
    int i;
	
    for (i = 0; i < n; i++)
        if (((ABS(a[i]-b[i])) >  (ABS(a[i]) * Epsilon))||(Flag == 1)) 
        {
#ifdef DEBUG
            printf("Over Epsilon at %d\n", i);
#endif      	
	    return(0);
        }
    return(1);
}



/************************************************************************/
/*                                                                      */
/*	solving x A = b by jacobi interative method.			*/
/*									*/
/*	Iterative equation						*/
/*	x_i^{k+1} = ( b_i - sum_{j=1, j!=i}^N a_{ji}x_j^k ) / a_{ii}	*/
/*                                                                      */
/*	Note: very similar to gauss-siedel(); can combine them in one.	*/
/*                                                                      */
/*      Modified by Ana Paula in 07/2000 - New stop condition           */
/*      and flag to convergence test                                    */
/************************************************************************/
double *jacobi_reward(A, x, b, precision, no_of_iterations,flag,no_max_iterations,stop_condition)
Matrix *A;	       /* pointer to a matrix - an array of column pointer    */
double *x;	       /* pointer to a vector - contiguous memory of elements */
double *b;	       /* pointer to a vector - contiguous memory of elements */
double precision;      /* precision for numeric approximation                 */
int *no_of_iterations; /* no_of_iterations to converge using precision        */
int flag;              /* flag used in convergence test*/
double no_max_iterations; /*no_max_iterations used in test of convergence*/
int    *stop_condition ;  /*stop_condition of the method 0 - Max_Iterations 1- Convergence */

{
	int col;		/* column number                          */
	int N;			/* matrix dimension ( number of columns ) */
	double a_ii;		/* diagonal element of the i-th column    */
	double sum;		/* sum for sum_{j=1,j!=i}^N a_{ji}x_j^k   */
	double *lx;		/* pointer to temporary solution vector   */
	Column **cp;		/* pointer of pointer to matrix columns   */
	Column *pcp;		/* pointer to matrix columns */


	N = A->num_col;	
	
	/* temp. vector with N ele. */
	lx = (double *)my_calloc(N, sizeof(double));	

       *no_of_iterations = 0;

	while((!converged_reward(x, lx, N,flag,precision))&&((*no_of_iterations) < no_max_iterations)) 
	{
		cp = (Column **)A->col_pp;	/* ptr to 1st column in A */
		bcopy(x, lx, N*sizeof(double));	/* save the last solution */
	
		/* re-calculate each element of the x */
		for (col=1; col<=N; col++) 
		{
			pcp = *cp;		/* the i-th column in A     */
			a_ii = 0;		/* diag. of the i-th column */
			sum  = 0;
	
			while (pcp) 
			{
				if (pcp->index == col) 
				    a_ii = pcp->value;
				else
				    sum += (pcp->value)*(lx[pcp->index - 1]);

				pcp = pcp->next;
			}

			if (!a_ii) 
			{		       /* sth. impossible happens.. */
				fprintf(stderr, "panic at %d.\n", col);
				return((double *)NULL);
			}

			/* calculate by jacobi equation the new x_col */
			x[col-1] = ( b[col-1] - sum ) / a_ii;
	
			++cp;		      /* advance to next A column */
		}
#ifdef DEBUG
		printf("Jacobi iteration:\n");
		put_vector(lx, N);
		put_vector(x, N);
#endif
	*no_of_iterations = *no_of_iterations +1;
	flag = 0;
	}
    
    if ((*no_of_iterations) == no_max_iterations)
       *stop_condition = MAX_ITERATIONS;
    else
       *stop_condition = CONVERGENCE;


	return (x);	/* got it */
}

/************************************************************************/
/*                                                                      */
/*      solving x A = b by Gauss-Siedel interative method.              */
/*                                                                      */
/*      Iterative equation                                              */
/*      x_i^{k+1} = ( b_i - sum_{j=1}^{i-1} a_{ji}x_j^{k+1}             */
/*                        - sum{j=i+1}^N a_{ji}x_j^k ) / a_{ii}         */
/*                                                                      */
/*      Modified by Ana Paula in 07/2000 - New stop condition           */
/*      and flag to convergence test                                    */
/************************************************************************/
double *gauss_siedel_reward(A, x, b, precision, no_of_iterations,flag,no_max_iterations,stop_condition)
Matrix *A;	       /* pointer to a matrix - an array of column pointers     */
double *x;	       /* pointer to a vector - contiguous memory of elements   */
double *b;	       /* pointer to a vector - contiguous memory of elements   */
double precision;      /* precision for numeric approximation                   */
int *no_of_iterations; /* no_of_iterations to converge using                    */
int flag;              /* flag used in convergence test*/
double no_max_iterations; /*no_max_iterations used in test of convergence*/
int    *stop_condition ;  /*stop_condition of the method 0 - Max_Iterations 1- Convergence */
{
	int col;		/* column number                           */
	int N;			/* matrix dimension ( number of columns )  */
	double a_ii;		/* diagonal element of the i-th column     */
	double sum_pre;		/* sum for sum_{j=1}^{i-1} a_{ji}x_j^{k+1} */
	double sum_post;	/* sum for sum{j=i+1}^N a_{ji}x_j^k        */
	double *lx;		/* pointer to temporary solution vector    */
	Column **cp;		/* pointer of pointer to matrix columns    */
	Column *pcp;		/* pointer to matrix columns               */


	N = A->num_col;
	
	lx = (double *)my_calloc(N, sizeof(double));  /* temp. vector with N ele. */

        *no_of_iterations = 0;

	while((!converged_reward(x, lx, N,flag,precision))&&((*no_of_iterations) < no_max_iterations )) 
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
		flag = 0;
	}
    
    if (*no_of_iterations == no_max_iterations)
       *stop_condition = MAX_ITERATIONS;
    else
       *stop_condition = CONVERGENCE;
         
	return (x);	/* got it */
}

/************************************************************************/
/*                                                                      */
/*	solving x A = x by power method 	                        */ 
/*      Morganna in 16/05/94                                            */
/*      Modified by Ana Paula in 07/2000 - New stop condition           */
/*      and flag to convergence test                                    */
/*      Modified by Ana Paula in 09/2000 - Now, we use a vector b!=null */
/************************************************************************/
double *power_reward(A, x, b, precision, no_of_iterations,flag,no_max_iterations,stop_condition)
Matrix *A;	       /* pointer to sthocastic matrix - an array of column pointers   */
double *x;	       /* pointer to a vector - contiguous memory of elements          */
double *b;
double precision;      /* precision for numeric approximation                          */
int *no_of_iterations; /* no_of_iterations to converge using precision                 */
int flag;
double no_max_iterations; /*no_max_iterations used in test of convergence*/
int    *stop_condition ;  /*stop_condition of the method 0 - Max_Iterations 1- Convergence */

{
	int col;		/* column number                           */
	int N;			/* matrix dimension ( number of columns )  */
	double a_ii;		/* diagonal element of the i-th column     */
	double sum_pre;		/* sum for sum_{j=1}^{i-1} a_{ji}x_j^{k+1} */
	double sum_post;	/* sum for sum{j=i+1}^N a_{ji}x_j^k        */
	double *lx;		/* pointer to temporary solution vector    */
	Column **cp;		/* pointer of pointer to matrix columns    */
	Column *pcp;		/* pointer to matrix columns               */


	/* dim. of A, and x must have N elements */
	N = A->num_col;	

	/* temp.vect.with N ele. */
	lx = (double *)my_calloc(N, sizeof(double));

   *stop_condition = 2;     
   *no_of_iterations = 0;

	while((!converged_reward(x, lx, N,flag,precision))&& ((*no_of_iterations) < no_max_iterations )) 
	{
   		  cp = (Column **)A->col_pp;	     /* ptr to 1st column in A */

	       bcopy(x, lx, N*sizeof(double));	/* save the last solution */
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
		printf("Power method:\n");
		put_vector(lx, N);
		put_vector(x, N);
#endif
		*no_of_iterations = *no_of_iterations + 1;
		flag = 0;
	}
    
    if ((*no_of_iterations) == no_max_iterations)
       *stop_condition = MAX_ITERATIONS;
    else
       *stop_condition = CONVERGENCE;

    return (x);	/* got it */
}

/************************************************************************/
/*                                                                      */
/*      sor_reward(Matrix *,double *, double *, int *):			*/
/*		solving x A = b by SOR interative method.	        */
/*									*/
/*	Iterative equation						*/
/*	x_i^{k+1} = w * ( b_i - sum_{j=1}^{i-1} a_{ji}x_j^{k+1} 	*/
/*			  - sum{j=i+1}^N a_{ji}x_j^k ) / a_{ii}		*/
/*                  + (1-w) * x_i^{k}                                   */
/*									*/
/* Modified on November, 29th, 1997. Changed the way w is updated, 	*/ 
/* defining wmax to avoid divergence and getting w_ini from the call	*/
/* Modified by Ana Paula in 07/2000 - New stop condition                */
/* and flag to convergence test and precision                           */
/************************************************************************/
double *sor_reward(A, x, b, no_of_iterations, w_ini, w_max_ini,flag,precision,no_max_iterations,stop_condition)
Matrix *A;	/* pointer to a matrix - an array of column pointers	*/
double *x;	/* pointer to a vector - contiguous memory of elements	*/
                /* initially contains the first guessed solution        */
double *b;	/* pointer to a vector - contiguous memory of elements	*/
int *no_of_iterations; /*no. of iter. to convergence*/
double *w_ini;
double *w_max_ini;
int flag;        /* flag used in convergence test*/
double precision;/* precision for numeric approximation                 */
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
	double max1_1();        /* max absolute coordinate */
	double opt_w_1();	/* evaluates optimum sor parameter  */


	double	last_w, w_max;	
	double	stab_teta;
	double Epsilon2; 
	int	est_count;
        int First;	
	
        
	w = 0;
    First = 0;
    last_w = 1;
	w_max = *w_max_ini;
	est_count = 0;
	Epsilon2 = precision;
	stab_teta = Epsilon2/10;


	if (!x || !b || !A )	/* nil vector or matrix is ignored */
		return((double *)NULL);

	N = A->num_col;	/* dim. of A, and both b and x must have N elements */


	if(!check_diagonal(A, N)) {	/* some disgonal element is missing */
		fprintf(stderr, "Missing diag., SOR'd topple!\n");
		return((double *)NULL);
	}

	lx =
	(double *)my_calloc(N, sizeof(double));	/* temp. vector with N ele. */

	if (First == 1)
	   delta_u =-1;
	else
	   delta_u = max1_1(x,lx,N);   

	counter = 1;
	lteta_k = 0;

	while(!sor_converged_1(x, lx, N,flag,precision)&&(counter < no_max_iterations)) 
	{
		First = 0;
                if (counter<=10) w = *w_ini;     /* first 2 it. keep same w */
		cp = (Column **)A->col_pp;	/* ptr to 1st column in A */
             
		delta_d = delta_u;               		/* get old greatest diference */
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
		delta_u = max1_1(x, lx, N);
		if (delta_u == 0)
		   return(x);  
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
				w = opt_w_1(w,teta_k);
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
	/* w_max_ini will return last value of w_max*/
	*w_max_ini = w_max;
 
       if (*no_of_iterations == no_max_iterations)
       *stop_condition = MAX_ITERATIONS;
       else
       *stop_condition = CONVERGENCE;
        
       return (x);	/* got it */
}
	

