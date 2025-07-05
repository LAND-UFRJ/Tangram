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
#include "pmf.h"
#include "structs.h"
#include "sparseMatrix.h"
#include "functions_espa_esra.h"


/************************************************************************/
/* ESPA_itr_reward: Efficient Transient Cumulative Rate Reward   	*/
/*		using iterative method for linear equation solution	*/
/*									*/
/* This routine calculates, by approximation, the cumulative rate  	*/
/* reward               of a Markov chain to a number of time instants	*/
/*  identically spaced. It uses a newly proposed iterative method	*/
/* to solve the A x = b equation. It's based on "Efficient Solutions	*/
/* for an Approximation Technique for the Transient Analysis of		*/
/* Markovian Models", by Rosa M. L. R. Carmo, Edmundo de Souza e Silva	*/
/* and Raymond Marie - INRIA research report no. 3055, november 1996.	*/
/*									*/
/* Input: ptr to probability matrix P;			         	*/
/*	  R = av. no. of aux events Y(r) in each interval of interest;	*/
/*	  t = total interval of interest; 				*/
/*	  n_t = number of instants of interest in interval [0,t];	*/
/*	  pi0 = initial probabilities vector PI(0);			*/
/*	  destv = destination-states vector;				*/
/*        vreward=  rate reward vector                                  */
/*        uniformization_rate = uniformization rate of continuous proc. */
/*									*/
/* Output:               vector with n_t ptr's to vectors CR(t)  	*/
/*			(one for each instant ti). CR(t) has as many	*/
/*			entries as destination states.			*/
/*									*/
/* Remark: Every vector, except for c and t, have its dimension stored	*/
/*	  in 1st position of its implementing structure. Hence, its	*/
/*	  implementation size is (N + 1), where N = dim. of the vector.	*/
/*									*/
/*	  z and w don't have their dimensions stored in 1st position	*/
/************************************************************************/

double **ESPA_itr_reward(P, set_of_R, num_total_point,set_of_intervals,pi0,destv,vreward,uniformization_rate,solu_option,precision,max_no_iteration,stop_condition)
Matrix	*P;	             /* ptr to transition rate matrix	             */
info_intervals *set_of_R;    /* struct with Erlang stages,number of intervals*/ 
int num_total_point;         /* number of instants of interest               */	
int set_of_intervals;        /* total intervals with != Rs                   */
double	*pi0;	             /* initial probabilities vector PI(0)           */
int	*destv;	             /* destination-states vector                    */
double  *vreward;            /*rate reward vector                            */
double  uniformization_rate; /*uniformization rate of continuous proc.       */
int     solu_option;         /*iterative method choice                       */
double  precision;           /*precision                                     */
double  max_no_iteration;    /*max no of iterations - used in stop condition */
int     *stop_condition;     /*stop_condition 0 - Max_Iterations 1- Conver   */ 

{
	int	ndest;	/* number of destination states          	*/
	int	idest;	/* counter for destination states	        */
	int	dest;	/* dest state to which we want prob vec z(i, R)	*/
	double	LAMBDA;	/* uniformization constant	                */
	double	lambda;	/* rate of aux events Y(r) (Ross's theory)	*/
	int 	j, k;	/* auxiliary counting variables	                */
	int	r;	/* counts the auxiliary events Y(r)     	*/
	int	N;	/* number of states in the model	        */
	Matrix	*A;	/* ptr to matrix A of eq. A z(i,r) = b1 z(i,r-1)*/
	double	b1;	/* aux value = prob of aux event Y(1) happen 1st*/
	double  b2;     /* aux value = time between events              */
	double	*z;	/* vector of cont. mem. space  = z(i,r)	        */
	double	*w;	/* vector b = z(i,r-1).lambda/(LAMBDA+lambda)	*/
	double	**pi;	/* array of prob. vectors (one vec. for each t)	*/
	int	count;	/* auxiliary counting variable	                */
	int	aux;	/* just for debug	                        */
	int	iter;
	long int total_iter;
	int     First;
	int     init;
	double	*w_ini, *w_max;
 

	w_ini = (double *) my_malloc(sizeof(double));
	w_max = (double *) my_malloc(sizeof(double));
	*w_ini = 1;
	*w_max = 2;
	total_iter = 0;
	iter = 0;

	/* get no. of destination states*/
	ndest = destv[0];
	N = P->num_col;	/* get the no. of states from P struct	*/
        LAMBDA = uniformization_rate;
#ifdef DEBUG
printf("P: \n");
put_trans_matrix(P);
#endif
	A = (Matrix *) my_calloc(1, sizeof(Matrix));/* allocate A*/
	/* allocates the column pointer	*/
	A->col_pp = (Column **) my_calloc(N, sizeof(Column *));
	/* set the number of states in the matrix A structure*/
	A->num_col = N;
	
	/* allocate contiguous memory space for vector of ptrs pi[]*/
	pi = (double **) my_malloc((num_total_point + 1) * sizeof(double *));
	pi[0] = (double *) my_malloc(sizeof(double));
	pi[0][0] = num_total_point;/* vector of vectors pi has dimension = n_t	*/
	/* alloc. all vectors pointed to by pi[], init elements with 0*/
	for (j = 1; j <= num_total_point; j++)
	   {
		/* alloc pi[j] (1 of the n_t probability vectors)*/
		pi[j] = (double *) my_calloc(ndest + 1, sizeof(double));
		pi[j][0] = ndest;
	   }

	/* allocate contiguous memory space for vector z[]*/
	z = (double *) my_calloc(N, sizeof(double));
	/* allocate contiguous memory space for vector w[]*/
	w = (double *) my_calloc(N, sizeof(double));
    
        for (idest = 1; idest <= ndest; idest++)
	   {
                
        	dest = destv[idest];	/* get destination state*/

	        /* initialize z with zeros*/
      	        for (k = 1; k <= N; k++)
		   z[k - 1] = 0;
                init  = 1;      
                for (count = 0;count < set_of_intervals;count ++)
		   {         

#ifdef DEBUG
printf("\nw:\n");
for (count = 1; count <= N; count++)
	printf("%10.4e ", w[count]);
printf("\n");
#endif
                       lambda = set_of_R[count].R/set_of_R[count].step;
	               A = calculate_A(P, LAMBDA, lambda);
#ifdef DEBUG
	printf("LAMBDA = %.10e; lambda = %.10e\n", LAMBDA, lambda);
	printf("A:\n");
	put_trans_matrix(A);
#endif	
	

	              /* transpose matrix A, since SOR solves x.A = b*/
	              A = transpose(A);
	              /* calc coef that multiplies z(r-1) in eq.*/
	              b1 = lambda / (LAMBDA + lambda);
	              b2 = 1/(LAMBDA + lambda);
	              /* calculate z(i,R) and pi(t) for each destination state*/

		      /* calc z(i,R) for each instant of interest*/
		
		      First = 1;
                      aux = init + set_of_R[count].total_points;
                      for (j = init; j < aux; j++)
		         {	            
                            
                            for (r = 1; r <= set_of_R[count].R; r++)
			       {
				
                                   /* w = z(i,r-1).lambda/(lambda+LAMBDA)	*/
				   for (k = 0; k < N; k++)
				      {
					   w[k] = b1 * z[k];
					   if ((k+1) == dest)
					      w[k] = w[k]+(b2*vreward[dest]);
				      }   	    

		                  switch (solu_option)
                                     {
                                        case (SOR_METHOD):
                                                          z = sor_reward(A, z, w, &iter, w_ini, w_max,First,precision,max_no_iteration,stop_condition);
				                          First = 0;
                                                          break;
                   
                                        case (GAUSS_METHOD):
                                                          z = gauss_siedel_reward(A, z, w, precision, &iter,First,max_no_iteration,stop_condition);
                                                          First = 0;
                                                          break;
			   
			                case (JACOBI_METHOD):
					                  z = jacobi_reward(A, z, w, precision, &iter,First,max_no_iteration,stop_condition);
                                                          First = 0;
                                                          break;
                            
			                case (POWER_METHOD):
					                   z = power_reward(A, z, w,precision, &iter,First,max_no_iteration,stop_condition);
                                                           First = 0;
			                                   break;                            
                                    }          

				total_iter += iter;
			     }
		          /* PIi(j) = PI(0) . zi(j)*/
			  for (k = 1; k <= N; k++)
			      pi[j][idest] += pi0[k] * z[k - 1];
	            	} 
                   init = j;
                   }
           }
	/* rel. mem. alloc. for P, intermediate, A, z and w*/
	free_Matrix(1, P);
	free_Matrix(1, A);
	free(w);
	free(z);
	free(w_ini);
	free(w_max);
        return(pi);
}

/************************************************************************/
/* ESPA_itr_set_reward: Efficient Transient Set Cumulative Rate Reward  */
/*Approximatio using iterative method for linear equation solution	*/
/*									*/
/* This routine calculates, by approximation, the cumulative rate  	*/
/* reward               of a Markov chain to a number of time instants	*/
/*  identically spaced. It uses a newly proposed iterative method	*/
/* to solve the A x = b equation. It's based on "Efficient Solutions	*/
/* for an Approximation Technique for the Transient Analysis of		*/
/* Markovian Models", by Rosa M. L. R. Carmo, Edmundo de Souza e Silva	*/
/* and Raymond Marie - INRIA research report no. 3055, november 1996.	*/
/*									*/
/* Input: ptr to probability matrix P;			         	*/
/*	  R = av. no. of aux events Y(r) in each interval of interest;	*/
/*	  t = total interval of interest; 				*/
/*	  n_t = number of instants of interest in interval [0,t];	*/
/*	  pi0 = initial probabilities vector PI(0);			*/
/*	  destv = destination-states vector;				*/
/*        vreward=  rate reward vector                                  */
/*        uniformization_rate = uniformization rate of continuous proc. */
/*									*/
/* Output: 	         vector with n_t ptr's to vectors PI(t) 	*/
/*			(one for each instant ti). PI(t) has as many	*/
/*			entries as destination states.			*/
/*									*/
/* Remark: Every vector, except for c and t, have its dimension stored	*/
/*	  in 1st position of its implementing structure. Hence, its	*/
/*	  implementation size is (N + 1), where N = dim. of the vector.	*/
/*									*/
/*	  z and w don't have their dimensions stored in 1st position	*/
/************************************************************************/

double *ESPA_itr_set_reward(P, set_of_R, num_total_point, set_of_intervals, pi0,vreward,rvec,uniformization_rate,solu_option,precision,max_no_iteration, stop_condition)
Matrix	*P;	              /* ptr to transition rate matrix	              */
info_intervals *set_of_R;     /* struct with Erlang stages,number of intervals*/
int     num_total_point;      /* number of instants of interest               */	
int     set_of_intervals;     /*intervals with != Rs                          */
double	*pi0;	              /* initial probabilities vector PI(0)	      */
double  *vreward;             /*rate reward vector                            */
int	*rvec;	              /* value-by-state (or set-belonging) vector     */
double  uniformization_rate;  /*uniformization rate of continuous proc.       */
int     solu_option;          /*iterative method choice                       */
double  precision;            /*precision                                     */
double  max_no_iteration;     /*max no of iterations - used in stop condition  */
int     *stop_condition;      /*stop_condition 0 - Max_Iterations 1- Conver   */  

{
        double	LAMBDA;	/* uniformization constant	                */
	double	lambda;	/* rate of aux events Y(r) (Ross's theory)	*/
	int 	j, k;	/* auxiliary counting variables	                */
	int	r;	/* counts the auxiliary events Y(r)	        */
	int	N;	/* number of states in the model	        */
	Matrix	*A;	/* ptr to matrix A of eq. A z(i,r) = b1 z(i,r-1)*/
	double	b1;	/* aux value = prob of aux event Y(1) happen 1st*/
	double  b2;     /* aux value = time between events              */
	double	*z;	/* vector of cont. mem. space  = z(i,r)	        */
	double	*w;	/* vector b = z(i,r-1).lambda/(LAMBDA+lambda)	*/
	double	*pi;	/* array of prob. vectors (one vec. for each t)	*/
	int	count;	/* auxiliary counting variable	                */
	int	aux;	/* just for debug	                        */
	int	iter;
        long int total_iter;
	int     First; 
	int     init;

	double	*w_ini, *w_max;

	w_ini = (double *) my_malloc(sizeof(double));
	w_max = (double *) my_malloc(sizeof(double));
	*w_ini = 1;
	*w_max = 2;
        total_iter = 0;
        iter =0;
	

	
	N = P->num_col;	/* get the no. of states from P struct*/
        LAMBDA = uniformization_rate;
#ifdef DEBUG
printf("P: \n");
put_trans_matrix(P);
#endif

	A = (Matrix *) my_calloc(1, sizeof(Matrix));/* allocate A*/
	/* allocates the column pointer	*/
	A->col_pp = (Column **) my_calloc(N, sizeof(Column *));
	/* set the number of states in the matrix A structure*/
	A->num_col = N;

	/* allocate contiguous memory space for vector of ptrs pi[]*/
	/* allocate contiguous memory space for vector pi[]*/
	pi = (double *) my_calloc( num_total_point + 1,sizeof(double));
	pi[0] = num_total_point;	/* vector pi has dimension = n_t*/

	/* allocate contiguous memory space for vector z[]*/
	z = (double *) my_calloc(N, sizeof(double));
	/* allocate contiguous memory space for vector w[]*/
	w = (double *) my_calloc(N, sizeof(double));
	
       /* initialize z with zeros*/
	for (k = 1; k <= N; k++)
            z[k - 1] = 0;
	
	First = 1;
        init = 1;
	for (count = 0;count< set_of_intervals;count ++)
	{ 
	  lambda = set_of_R[count].R /set_of_R[count].step ;/* calculate lambda*/
  	  A = calculate_A(P, LAMBDA, lambda);
#ifdef DEBUG
	printf("LAMBDA = %.10e; lambda = %.10e\n", LAMBDA, lambda);
	printf("A:\n");
	put_trans_matrix(A);
#endif
	  /* transpose matrix A, since SOR solves x.A = b*/
	  A = transpose(A);
	  /* calc coef that multiplies z(r-1) in eq.*/
	  b1 = lambda / (LAMBDA + lambda);
	  b2 = 1/(LAMBDA + lambda);
	
		  
#ifdef DEBUG
printf("\nw:\n");
for (count = 1; count <= N; count++)
	printf("%10.4e ", w[count]);
printf("\n");
#endif
	  /* calc z(i,R) for each instant of interest*/
		
	  aux = init + set_of_R[count].total_points;
          for (j = init; j < aux; j++)
             {	/* calc z(R) by iteration on r*/
						
		for (r = 1; r <= set_of_R[count].R; r++)
		   {
			/* w = z(i,r-1).lambda/(lambda+LAMBDA)*/
			for (k = 0; k < N; k++)
			   w[k] = (b1 * z[k])+(b2*vreward[k+1]*rvec[k+1]);
			    

			switch (solu_option)
                          {
                            case (SOR_METHOD):
                                            z = sor_reward(A, z, w, &iter, w_ini, w_max,First,precision,max_no_iteration,stop_condition);
				            First = 0;
                            break;
                   
                            case (GAUSS_METHOD):
                                            z = gauss_siedel_reward(A, z, w, precision, &iter,First,max_no_iteration,stop_condition);
                                            First = 0;
                            break;
			   
			    case (JACOBI_METHOD):
					   z = jacobi_reward(A, z, w, precision, &iter,First,max_no_iteration,stop_condition);
                                           First = 0;
			   
			    break;
			   
                            case (POWER_METHOD):
					   z = power_reward(A, z, w, precision, &iter,First,max_no_iteration,stop_condition);
                                           First = 0;
			   
			    break;                             
                            
                          }          
                
		       total_iter += iter;
		    }
		/* PIi(j) = PI(0) . zi(j)*/
		for (k = 1; k <= N; k++)
		    pi[j] += pi0[k] * z[k - 1];
	      }
         init = j; 
         }
       
	/* rel. mem. alloc. for P, intermediate, A, z and w*/
	free_Matrix(1, P);
	free_Matrix(1, A);
	free(w);
	free(z);
	free(w_ini);
	free(w_max);
	return(pi);
}
