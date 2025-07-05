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


#include<math.h>
#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include "sparseMatrix.h"
#include "pmf.h"
#include "structs.h"
#include "functions_espa_esra.h"


/*******************************************************************************/
/*free_Matrix_2:  This function is used to free the Matrix_2 structs. This was */
/*implemented by Yuguang Wu UCLA, in 1993.  Ana Paula Couto Silva modifies the */ 
/* test made before free the memory.                                           */           
/*  Output:                                                                    */
/*          num:  number of matrix structures;                                 */
/*          A:    ptr to matrix structure;                                     */
/*******************************************************************************/ 
void free_Matrix_2(num, A)
int    num;                    /* number of matrix structures */
Matrix2 **A;                   /* ptr to matrix structure     */
{
   int    i, j, row;           /* temporary variables              */
   int    num_col, num_row;    /* number of columns/number of rows */
   double **pR, *ptr;


   for(i = 0; i < num; i++)      
   {                  	
       for(j = 0; j < num; j++) 
       {                  	
           pR      = (A[i][j]).array;
           num_row = (A[i][j]).num_row;
           num_col = (A[i][j]).num_col;

           for (row = 0; row < num_row && pR; row++)
           {
               ptr = pR[row];
               free((char *)ptr);
           } 
           free((char **)pR);
       }                  	
   }
   
   free((char **)A);
}

/************************************************************************/
/*	update_w_1: make w <- z(r-1) . lambda / (lambda + LAMBDA)       */
/*       +R1j/(lambda + LAMBDA)                                         */
/*	Inputs:								*/
/*	z = partitioned vector z(r);					*/
/*      var_add = constant = R1j/ (lambda + LAMBDA)                     */
/*	c = double value = lambda / (lambda + LAMBDA).			*/
/*      act_state = actual destination state.                           */
/*	After executation		       			        */
/*	w = partitioned vector w(0)(r).					*/
/************************************************************************/
void update_w_1(w, z, c,var_add,act_state)
double	**w;	/* partitioned vector w(0)(r)	*/
double	**z;	/* partitioned vector z(r)	*/
double	c;	/* lambda / (lambda + LAMBDA)	*/
double var_add; /*R1j/ (lambda + LAMBDA)        */
int act_state; /*actual destination state    */
 {
	int	K;	/* number of partitions	*/
	int	i, j;	/* iteration variables	*/
	int	sizei;	/* size of ith partition*/
	int count;      /*aux. that specify when add var_add*/

	/* get no. of partitions from vector z's structure	*/
	K = z[0][0];	
	/* check if w and z have the same dimensions	*/
	if (w[0][0] != K)
	{
		fprintf(stderr,
		"update_w: vectors w and z have different dimensions.\n");
		exit(1);
	}

	/* loop through all partitions of vector z	*/
	count = 0;
	for (i = 1; i <= K; i++)
	{
		sizei = z[i][0];	/* get size of ith partition	*/
		/* loop through all elements in ith partition	*/
		for (j = 1; j <= sizei; j++)		/* w <- z . c	*/
			{
			   w[i][j] = z[i][j] * c;
			   count = count +1;
		           /*only adds with pos = dest_state*/
			   if (count == act_state) 
			      w[i][j] = w[i][j] + var_add;
			}
		             		   
	}
}


/************************************************************************/
/*	update_w_set: make w <- z(r-1) . lambda / (lambda + LAMBDA)     */
/*       +R1j/(lambda + LAMBDA)                                         */
/*	Inputs:								*/
/*	z = partitioned vector z(r);					*/
/*	c = double value = lambda / (lambda + LAMBDA).			*/
/*      d = double value = 1/LAMBDA+lambda                              */
/*      rewvector = R1j vector                                          */
/*	After executation		       			        */
/*	w = partitioned vector w(0)(r).					*/
/************************************************************************/
void update_w_set(w, z, c,d,rewvector)
double	**w;	/* partitioned vector w(0)(r)	*/
double	**z;	/* partitioned vector z(r)	*/
double	c;	/* lambda / (lambda + LAMBDA)	*/
double  d;       /*1/ (lambda + LAMBDA)        */
double *rewvector; /*R*1j vector    */
{
	int	K;	/* number of partitions	*/
	int	i, j;	/* iteration variables	*/
	int	sizei;	/* size of ith partition*/
	int count;      /*aux. that specify when add var_add*/

	/* get no. of partitions from vector z's structure	*/
	K = z[0][0];	
	/* check if w and z have the same dimensions	*/
	if (w[0][0] != K)
	{
		fprintf(stderr,
		"update_w: vectors w and z have different dimensions.\n");
		exit(1);
	}

	/* loop through all partitions of vector z	*/
	count = 1;
	for (i = 1; i <= K; i++)
	{
		sizei = z[i][0];	/* get size of ith partition	*/
		/* loop through all elements in ith partition	*/
		for (j = 1; j <= sizei; j++)		/* w <- z . c	*/
			{
			   w[i][j] = z[i][j] * c + d * rewvector[count];
			   count = count + 1;	
			}
		             		   
	}
}

/************************************************************************/
/* ESPA_dir_reward: Efficient Transient Cumulative Rate Reward   	*/
/*		using direct method for linear equation solution	*/
/*									*/
/* This routine calculates, by approximation, the transient state 	*/
/* cumulative reward      of a Markov chain to a number of time 	*/
/* instants identically spaced. It uses a newly proposed direct method	*/
/* to solve the A x = b equation. It's based on "Efficient Solutions	*/
/* for an Approximation Technique for the Transient Analysis of		*/
/* Markovian Models", by Rosa M. L. R. Carmo, Edmundo de Souza e Silva	*/
/* and Raymond Marie - INRIA research report no. 3055, november 1996.	*/
/*                                                                      */
/* This routine was implemented by Joao Abdalla Ney Silva, January 1998 */
/*(to calculate probability distribution).                              */
/* Ana Paula Couto Silva modifies this routine: incresing the arguments */
/* and modifing the calculate of w(0) for each destination state	*/
/*                                                                      */
/*   Input:P =  ptr to probability  matrix P;				*/
/*	  R = av. no. of aux events Y(r) in each interval of interest;	*/
/*	  t = total interval of interest; 				*/
/*	  n_t = number of instants of interest in interval [0,t];	*/
/*	  pi0 = initial probabilities vector PI(0);			*/
/*	  partv = partition vector;					*/
/*	  destv = destination-states vector;                            */
/*        rewv = reward vector for each state;                          */
/*	  uniformi_rate = uniformization rate of matrix Q;        	*/		                                       
/*									*/
/* Output:              - vector with n_t ptr's to vectors CR(t)	*/
/*			(one for each instant ti). CR(t) has as many	*/
/*			entries as destination states.			*/
/*									*/
/* Remark: Every vector have its dimension stored in 1st position of	*/
/*	  its implementing structure. Hence, its implementation size is	*/
/*	  (N + 1), where N = dimension of the vector.			*/
/************************************************************************/

double **ESPA_dir_reward(P, R, t, n_t, pi0, partv, destv,rewv,uniformi_rate)
Matrix	*P;	/* ptr to transition rate matrix	                 */
int	R;	/* average no. of events Y(r) in each interval	         */
double	t;	/* total interval of interest	                         */
int	n_t;	/* number of instants of interest in interval [0,t]	 */
double	*pi0;	/* initial probabilities vector PI(0)	                 */
int	*partv;	/* partition vector	                                 */
int	*destv;	/* destination-states vector	                         */
double     *rewv;  /*reward-states vector                                */
double  uniformi_rate; /*uniformization rate LAMBDA                      */
{
	double	step_t,
		prev_t;	/* aux var used to build vector intermediate[]	*/
	int	ndest;	/* number of destination states	                */
	int	idest;	/* counter for destination states	        */
	int	dest;	/* dest state to which we want prob vec z(i, R)	*/
	double	LAMBDA;	/* uniformization constant	                */
	double	lambda;	/* rate of aux events Y(r) (Ross's theory)	*/
	double	*intermediate;	/* ptr to vec w/ interm inst of interest*/
	int 	j, k;	/* auxiliary counting variables	                */
	int	r;	/* counts the auxiliary events Y(r)	        */
	int	N;	/* number of states in the model	        */
	Matrix	*A;	/* ptr to matrix A of eq. A z(i,r) = b1 z(i,r-1)*/
	Matrix2	**p_A;	/* partitioned matrix A	                        */
	double b2;      /* aux value = time between events(obser or trans)*/
	double	b1;	/* aux value = prob of aux event Y(1) happen 1st*/
	int	K;	/* number of partitions	                        */
	double	*z;	/* vector z(r)	                                */
	double	**p_z;	/* partitioned vector: z(r)	                */
	double	*w;	/* vector w = z(i,r-1).lambda/(LAMBDA+lambda)	*/
	double	**p_w;	/* partitioned vector w	                        */
	double	**pi;	/* array of prob. vectors (one vec. for each t)	*/
	double  rew_add; /*constant add  = R1j/(LAMBDA + lambda)        */
	int	count;	/* auxiliary counting variable	                */

	
    rew_add = 0;
    /* get no. of destination states*/
	ndest = destv[0];
	/* get no. of partitions from partition vector*/
	K = partv[0];
	N = P->num_col;	/* get the no. of states from P struct*/
       
	/* allocate contiguous memory space for vector intermediate[]*/
	intermediate = (double *)malloc((n_t + 1)* sizeof(double));
	/* set equally spaced interm instants within interval [0,t]*/
	step_t = t / n_t;/* calc lenght of each interval*/
	prev_t = 0;/* init previous instant prev_t*/
	intermediate[0] = n_t;	/* 1st position stores vector dimension	*/
	for (j = 1; j <= n_t; j++)
	{
		prev_t +=step_t;
		intermediate[j] = prev_t;
	}

	A = (Matrix *) my_calloc(1, sizeof(Matrix));/* allocate A*/
	/* allocates the column pointer	*/
	A->col_pp = (Column **) my_calloc(N, sizeof(Column *));
	/* set the number of states in the matrix A structure*/
	A->num_col = N;
	
	/* allocate contiguous memory space for vector of ptrs pi[]*/
	pi = (double **) my_malloc((n_t + 1) * sizeof(double *));
	pi[0] = (double *) my_malloc(sizeof(double));
	pi[0][0] = n_t;	/* vector of vectors pi has dimension = n_t*/
	/* alloc. all vectors pointed to by pi[], init elements with 0*/
	for (j = 1; j <= n_t; j++)
	   {
		/* alloc pi[j] (1 of the n_t probability vectors)*/
		pi[j] = (double *) my_calloc(ndest + 1, sizeof(double));
		pi[j][0] = ndest;
	   }
		/* allocate contiguous memory space for vector z[]*/
		z = (double *) my_calloc(N + 1, sizeof(double));
		z[0] = N;/* store dim. of z in 1st pos.	*/

	/* create p_z structure based on z and partv*/
	p_z = vpartition(z, partv);
	/* allocate contiguous memory space for vector w[]*/
	w = (double *) my_calloc(N + 1, sizeof(double));
	w[0] = N;/* store dim. of w in 1st pos.	*/
	
	lambda = R / step_t;/* calculate lambda*/

	LAMBDA = uniformi_rate;
	A = calculate_A(P, LAMBDA, lambda);
#ifdef DEBUG
	printf("LAMBDA = %.10e; lambda = %.10e\n", LAMBDA, lambda);
	printf("A:\n");
	put_trans_matrix(A);
#endif
	/* calc coef that multiplies z(r-1) in eq.*/
	b1 = lambda / (LAMBDA + lambda);
	
	/*calc coef that is add in the right side in equation.*/
	b2 = 1/(LAMBDA + lambda);
	
	/* obtain partitioned matrix A	*/
	p_A = mpartition(A, partv);
#ifdef DEBUG
	printf("Matriz A particionada:\n");
	print_matrix_P(p_A, partv);
#endif
	/* call reduction procedure to pre-calculate useful blocks*/
	reduction(p_A, K);
#ifdef DEBUG

	printf("Matriz A particionada, apos reducao:\n");
	print_matrix_P(p_A, partv);
#endif	
	/* calculate z(i,R) and pi(t) for each destination state*/
	for (idest = 1; idest <= ndest; idest++)
	{
		dest = destv[idest];	/* get destination state*/
		/* initialize w with z(0) . lambda/(lambda+LAMBDA)
		+ R1j*b2 (that is the reward rate in the destination state
		, withz(0) = 1i (ith element=1; all others=0) (it makes sense:
		prob{state(t) = dest | state (0) = j} when t -> 0).*/
		for (j = 1; j <= N; j++)
			if (j == dest)
				{
				   w[j] = (rewv[j]*b2);
				   rew_add = rewv[j]*b2;
				}  
			else	w[j] = 0;

#ifdef DEBUG
printf("\nw:\n");
for (count = 1; count <= N; count++)
	printf("%10.4e ", w[count]);
printf("\n");
#endif
		/* partition w, creating p_w*/
		p_w = vpartition(w, partv);
#ifdef DEBUG
printf("\npartitioned w:\n");
for (count = 1; count <= K; count++)
{
	printf("w%d:\n", count);
	for (aux = 1; aux <= p_w[count][0]; aux++)
		printf("%10.4e ", p_w[count][aux]);
	printf("\n");
}
printf("\n");
#endif

		/* calc z(i,R) for each instant of interest*/
		for (j = 1; j <= n_t; j++)
		{	/* calc z(R) by iteration on r*/
			
	
			for (r = 1; r <= R; r++)
			{
				/* calculate wi(k)(r-1) for all i and k	*/
				calc_w(p_A, p_w);
#ifdef DEBUG
printf("\npartitioned w:\n");
for (count = 1; count <= K; count++)
{
	printf("w%d:\n", count);
	for (aux = 1; aux <= p_w[count][0]; aux++)
		printf("%10.4e ", p_w[count][aux]);
	printf("\n");
}
printf("\n");

#endif
				/* calculate zi(r) for all 1 <= i <= K	*/
				calc_z(p_A, p_w, p_z);
#ifdef DEBUG
printf("\npartitioned z:\n");
for (count = 1; count <= K; count++)
{
	printf("z%d:\n", count);
	for (aux = 1; aux <= p_z[count][0]; aux++)
		printf("%10.4e ", p_z[count][aux]);
	printf("\n");
}
printf("\n");
#endif
				/* calculate w(0)(r) for next iteration	*/
				update_w_1(p_w, p_z, b1,rew_add, dest);
#ifdef DEBUG
printf("\npartitioned w:\n");
for (count = 1; count <= K; count++)
{
	printf("w%d:\n", count);
	for (aux = 1; aux <= p_w[count][0]; aux++)
		printf("%10.4e ", p_w[count][aux]);
	printf("\n");
}
printf("\n");
#endif
			}
			vunpartition(p_z, z);
			
#ifdef DEBUG
printf("\nz:\n");
for (count = 1; count <= N; count++)
	printf("%10.4e ", z[count]);
printf("\n");
#endif
			
				/* PIi(j) = PI(0) . zi(j)*/
				for (k = 1; k <= N; k++)
					{
					   pi[j][idest] += pi0[k] * z[k];
					}   
		}
		/* release memory allocated for p_w*/
		for (count = 0; count <= K; count++)
			free(p_w[count]);
		free(p_w);			
	}
	/* rel. mem. alloc. for P, intermediate, A, p_A, z, p_z and w*/
	free_Matrix(1, P);
	free(intermediate);
	free_Matrix(1, A);
	free_Matrix_2(K, p_A);
	for (count = 0; count <= K; count++)	/* rel. p_z mem	*/
		free(p_z[count]);
	free(p_z);	
	free(w);
	free(z);
	return(pi);
}



/************************************************************************/
/* ESPA_dir_set_reward: Efficient Transient Cumulative Rate Reward    	*/
/*		 using direct method for linear equation solution ---	*/
/*		 applied to get the probability of a set of state or	*/
/*									*/
/* This routine calculates, by approximation, tthe transient state 	*/
/* cumulative reward of a Markov chain to a number of time 	        */
/* instants identically spaced. It uses a newly proposed direct method	*/
/* to solve the A x = b equation. It's based on "Efficient Solutions	*/
/* for an Approximation Technique for the Transient Analysis of		*/
/* Markovian Models", by Rosa M. L. R. Carmo, Edmundo de Souza e Silva	*/
/* and Raymond Marie - INRIA research report no. 3055, november 1996.	*/
/*                                                                      */
/* This routine was implemented by Joao Abdalla Ney Silva, January 1998 */
/*(to calculate probability distribution).                              */
/* Ana Paula Couto Silva modifies this routine: incresing the arguments */
/* and modifing the calculate of w(0).                          	*/
/*									*/
/* Input: ptr to transition rate matrix P;				*/
/*	  R = av. no. of aux events Y(r) in each interval of interest;	*/
/*	  t = total interval of interest; 				*/
/*	  n_t = number of instants of interest in interval [0,t];	*/
/*	  pi0 = initial probabilities vector PI(0);			*/
/*	  partv = partition vector;					*/
/*	  rvec = value-by-state vector;  				*/
/*        rewv = vector with rate reward in each state;                 */
/*        uniformi_rate = rate of uniformization process;               */
/*									*/
/* Output:  vector with the cumulative reward of a set of states, given */
/*          a initial distribution.                                     */        
/* Remark: Every vector have its dimension stored in 1st position of	*/
/*	  its implementing structure. Hence, its implementation size is	*/
/*	  (N + 1), where N = dimension of the vector.			*/
/************************************************************************/

double *ESPA_dir_set_reward(P, R, t, n_t, pi0, partv, rvec,rewv,uniformi_rate)
Matrix	*P;	/* ptr to transition rate matrix	                */
int	R;	/* average no. of events Y(r) in each interval	        */
double	t;	/* total interval of interest	                        */
int	n_t;	/* number of instants of interest in interval [0,t]	*/
double	*pi0;	/* initial probabilities vector PI(0)	                */
int	*partv;	/* partition vector	                                */
int	*rvec;	/* value-by-state (or set-belonging) vector	        */
double  *rewv;  /* vector with rate reward in each state                */
double  uniformi_rate; /*uniformization rate                            */ 
{
	double	step_t,
		prev_t;	/* aux var used to build vector intermediate[]	*/
	double	LAMBDA;	/* uniformization constant	                */
	double	lambda;	/* rate of aux events Y(r) (Ross's theory)	*/
	double	*intermediate;	/* ptr to vec w/ interm inst of interest*/
	int 	j, k;	/* auxiliary counting variables	                */
	int	r;	/* counts the auxiliary events Y(r)	        */
	int	N;	/* number of states in the model	        */
	Matrix	*A;	/* ptr to matrix A of eq. A z(i,r) = b1 z(i,r-1)*/
	Matrix2	**p_A;	/* partitioned matrix A	                        */
	double	b1;	/* aux value = prob of aux event Y(1) happen 1st*/
	double  b2;     /* aux value = time between events              */
	int	K;	/* number of partitions	                        */
	double	*z;	/* vector z(r)	                                */
	double	**p_z;	/* partitioned vector: z(r)	                */
	double	*w;	/* vector w = z(i,r-1).lambda/(LAMBDA+lambda)	*/
	double	**p_w;	/* partitioned vector w	                        */
	double	*pi;	/* set probabilities/exp value (one for each t)	*/
	int	count;	/* auxiliary counting variable	                */

	/* get no. of partitions from partition vector*/
	K = partv[0];
	N = P->num_col;	/* get the no. of states from Q struct	*/
	/* calc unif const and trans prob matrix of uniformized process	*/ 
	/* allocate contiguous memory space for vector intermediate[]*/
	intermediate = (double *) my_calloc(n_t + 1, sizeof(double));
	/* set equally spaced interm instants within interval [0,t]*/
	step_t = t / n_t;/* calc lenght of each interval	*/
	prev_t = 0;/* init previous instant prev_t	*/
	intermediate[0] = n_t;	/* 1st position stores vector dimension	*/
	for (j = 1; j <= n_t; j++)
	{
		prev_t += step_t;
		intermediate[j] = prev_t;
	}

	A = (Matrix *) my_calloc(1, sizeof(Matrix));/* allocate A*/
	/* allocates the column pointer	*/
	A->col_pp = (Column **) my_calloc(N, sizeof(Column *));
	/* set the number of states in the matrix A structure	*/
	A->num_col = N;
	
	/* allocate contiguous memory space for vector pi[]*/
	pi = (double *) my_calloc(n_t + 1,sizeof(double));
	pi[0] = n_t;	/* vector pi has dimension = n_t*/
	/* allocate contiguous memory space for vector z[]*/
	z = (double *) my_calloc(N + 1, sizeof(double));
	z[0] = N;/* store dim. of z in 1st pos.*/

	/* create p_z structure based on z and partv*/
	p_z = vpartition(z, partv);
	/* allocate contiguous memory space for vector w[]*/
	w = (double *) my_calloc(N + 1, sizeof(double));
	w[0] = N;/* store dim. of w in 1st pos.*/
	
	lambda = R / step_t;/* calculate lambda	*/ 
    
	LAMBDA = uniformi_rate;
	A = calculate_A(P, LAMBDA, lambda);
#ifdef DEBUG
	printf("LAMBDA = %.10e; lambda = %.10e\n", LAMBDA, lambda);
	printf("A:\n");
	put_trans_matrix(A);
#endif
	/* calc coef that multiplies z(r-1) in eq.*/
	b1 = lambda / (LAMBDA + lambda);
	
	/*calc coef that multiplies the vector R1j*/
	b2 = 1/(LAMBDA + lambda);
	
	/* obtain partitioned matrix A	*/
	p_A = mpartition(A, partv);
#ifdef DEBUG
	printf("Matriz A particionada:\n");
	print_matrix_P(p_A, partv);
#endif
	/* call reduction procedure to pre-calculate useful blocks	*/
	reduction(p_A, K);
#ifdef DEBUG

	printf("Matriz A particionada, apos reducao:\n");
	print_matrix_P(p_A, partv);
#endif	
       /*calculate the vector R1j*/
       for (j = 1;j<=N;j++)
          rewv[j]= rvec[j]*rewv[j];
	  
#ifdef DEBUG
      for (j = 1;j<=N;j++)
       printf("Rew eh %lf\n",rewv[j]);  
#endif	  

	/* calculate z(i,R) and pi(t) for the set of states or
	y(i,R) and y(t) (expected value of a variable) for the system	*/
	
	/* initialize w with z(0) . lambda/(lambda+LAMBDA), with
	z(0) = value-by-state-vector(0) (ith element = value of the
	variable in state i). If we are interested in the prob. of a set
	of states we let z(i) = 1 if i is in the set, else z(i) = 0.	*/
	for (j = 1; j <= N; j++)
		w[j] = b2*rewv[j];

#ifdef DEBUG
printf("\nw:\n");
for (count = 1; count <= N; count++)
printf("%10.4e ", w[count]);
printf("\n");
#endif
	/* partition w, creating p_w	*/
	p_w = vpartition(w, partv);
#ifdef DEBUG
printf("\npartitioned w:\n");
for (count = 1; count <= K; count++)
{
printf("w%d:\n", count);
for (aux = 1; aux <= p_w[count][0]; aux++)
	printf("%10.4e ", p_w[count][aux]);
printf("\n");
}
printf("\n");
#endif

	/* calc z(i,R) for each instant of interest	*/
	for (j = 1; j <= n_t; j++)
	{	/* calc z(R) by iteration on r	*/
		
		
		for (r = 1; r <= R; r++)
		{
			/* calculate wi(k)(r-1) for all i and k	*/
			calc_w(p_A, p_w);
#ifdef DEBUG
printf("\npartitioned w:\n");
for (count = 1; count <= K; count++)
{
printf("w%d:\n", count);
for (aux = 1; aux <= p_w[count][0]; aux++)
	printf("%10.4e ", p_w[count][aux]);
printf("\n");
}
printf("\n");

#endif
			/* calculate zi(r) for all 1 <= i <= K	*/
			calc_z(p_A, p_w, p_z);
#ifdef DEBUG
printf("\npartitioned z:\n");
for (count = 1; count <= K; count++)
{
printf("z%d:\n", count);
for (aux = 1; aux <= p_z[count][0]; aux++)
	printf("%10.4e ", p_z[count][aux]);
printf("\n");
}
printf("\n");
#endif
			/* calculate w(0)(r) for next iteration	*/
			update_w_set(p_w, p_z, b1,b2,rewv);
#ifdef DEBUG
printf("\npartitioned w:\n");
for (count = 1; count <= K; count++)
{
printf("w%d:\n", count);
for (aux = 1; aux <= p_w[count][0]; aux++)
	printf("%10.4e ", p_w[count][aux]);
printf("\n");
}
printf("\n");
#endif
		}
		vunpartition(p_z, z);
		
#ifdef DEBUG
printf("\nz:\n");
for (count = 1; count <= N; count++)
printf("%10.4e ", z[count]);
printf("\n");
#endif

		
			/* PIi(j) = PI(0) . zi(j)*/
			for (k = 1; k <= N; k++)
				pi[j] += pi0[k] * z[k];
	}
	/* release memory allocated for p_w*/
	for (count = 0; count <= K; count++)
		free(p_w[count]);
	free(p_w);			

	/* rel. mem. alloc. for P, intermediate, A, p_A, z, p_z and w	*/
	free_Matrix(1, P);
	free(intermediate);
	free_Matrix(1, A);
	free_Matrix_2(K, p_A);
	for (count = 0; count <= K; count++)/* rel. p_z mem	*/
		free(p_z[count]);
	free(p_z);	
	free(w);
	free(z);
	return(pi);
}

