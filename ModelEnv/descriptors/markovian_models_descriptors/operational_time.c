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
/*	    Routines for computing uniformization-based                     */
/*		  cumulative operational time distribution		*/
/*	        & other Omega function-related measurements.		*/
/*                                                                      */
/*		References:						*/
/*                                                                      */
/*		1."Calculating Cumulative Operational Time		*/
/*		   Distribution of Repairable Computer Systems",	*/
/*		   E. S. e. Silva, R. Gail,				*/
/*		   IEEE TC, C-35, No.4, pp. 322-332, April 1986.	*/
/*									*/
/*		2."Interval Availability distribution Computation",	*/
/*		   G. Rubino, R. Sericola,				*/
/*		   FTCS'23, pp. 1-8., 1993.				*/
/*                                                                      */
/*		Yuguang Wu UCLA CSD 8/17/93				*/
/*									*/
/************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#ifdef _SOLARIS_
#include <strings.h>
#endif
#include "define.h"
#include "misc.h"
#include "bounds.h"
#include "Matrix.h"
#include "interface_omega.h"


/************************************************************************/
/*									*/
/*	omega(int, double *, Matrix *, double, double, int, 		*/
/*	  double *, double *, double *, double *, double *, double *)	*/
/*									*/
/*	Based on Ref. 1.						*/
/*									*/
/*		assumptions:						*/
/*		P is square matrix, pi has the correct dimension.	*/
/*		All t's are positive.					*/
/*		If any of *ea, *re, or *el is non-null, it must point	*/
/*		to a pre-allocated space with the correct capacity to	*/
/*		store returned results.					*/
/*									*/
/************************************************************************/

double *omega(L, pi, P, Lambda, epsilon, num_pairs, s, t, ea, re, el, mf)

int    L;	/* number of operational states {1, 2, .., L} */
double *pi;	/* pointer to initial probabilities - contiguous memory */
Matrix *P;	/* pointer to transition prob. matrix - an array of ptr */
double Lambda;	/* uniformization constant */
double epsilon;	/* error */
int num_pairs;	/* number of elements in the lists of *s and *t below */
double *s;	/* list of s's under study */
double *t;	/* list of t's under study */
double *ea;	/* list of expected availability for diff. t's */
double *re;	/* list of reliability for diff. t's */
double *el;	/* list of expected lifetime for diff. t's */
double *mf;	/* mean time to failure */

{
	int i, j, k, l, m;		/* temporary variables.	      */
	/* int n; */			/* temporary variables.	      */
	int M;				/* total number of states.    */
	int N;				/* 1st approximation (ref. 1) */
	/* int C; */			/* 2nd approximation (ref. 1) */
	int avail = 0;			/* flag for availability list */
	int reli  = 0;			/* flag for reliability list  */
	int life  = 0;			/* flag for lifetime list     */
	int mttf  = 0;			/* flag for mean time to fail */
	double max_t;			/* maximum in the t list.     */
	double *p;			/* list of s/t		      */ 
	double pw;			/* temporary variable.	      */ 
	double pssn_fact;		/* Poisson factor.	      */
	double erlang_fact;		/* Erlang factor.	      */
	double beta;			/* sum of binomial factors.   */
	double *beta_prime;		/* list of binomial factors.  */
	double *cot;			/* list of cum. op. time.     */ 
	double e_prime;			/* e'(N, C) for 2nd cut-off.  */
	double tolerance;		/* temporary variable.	      */
	Omega_Head *array_omega;	/* ptr to array Omega[n, k].  */
	double *lst_omega;		/* ptr to list Omega[n,k,s].  */
	double *pre_lst_omega;		/* ptr to list Omega[n,k,s].  */
	double *tmp_omega;		/* ptr to list of temp. Omega */
	double *ptr;			/* ptr to a double variable   */
	double sum_states;			/* temporary variable.	      */
	double this_omega;		/* temporary variable.	      */
	double exponent;		/* temporary variable.	      */
	Column *col_ptr;		/* ptr to a column of P	      */
	double calc_omega_per_state();	/* calculate a state Omega    */


	if (!pi || !P || num_pairs <= 0) {
		fprintf(stderr, "Operational time and related measures: Null input, return null.\n");
		return((double *)NULL);
	}

	M = DIM_COLUMN(P);
	if (L < 1 || L > M) {		/* insane */
		fprintf(stderr, "Operational time and related measures: Number of operational states (%d) lower than number of states (%d)!!!\n", L, M);
		return((double *)NULL);
	}

	/* clear the return lists */
	if (ea) {
		avail = 1;
		bzero((char *)ea, num_pairs*sizeof(double));
	}

	if (re) {
		reli = 1;
		bzero((char *)re, num_pairs*sizeof(double));
	}

	if (el) {
		life = 1;
		bzero((char *)el, num_pairs*sizeof(double));
	}

	if (mf) {
		mttf = 1;
		bzero((char *)mf, sizeof(double));
	}

	/* allocate and calculate the list of p = s/t. */
	p = (double *)my_malloc(num_pairs*sizeof(double));

	max_t = 0;
	for (m=0; m<num_pairs; m++) {
		if (s[m] <= 0 || t[m] <= 0) {
			fprintf(stderr, "Operational time and related measures: Wrong time interval (t) and/or reward levels (s) definition.\n");
			free((char *)p);
			return((double *)NULL);
		}
		p[m] = s[m]/t[m];
		max_t = MAX(max_t, t[m]);
	}

        /* allocate the list of returned cumulative operation time dist. */
        cot = (double *)my_calloc(num_pairs, sizeof(double));

	/* 1st cut-off approximation */
	N = partial_poisson_sum(Lambda*max_t, epsilon/2);
        if (N < 0) {
                fprintf(stderr, "Operational time and related measures: N = %d < 0. Problems with precision or time intervals.\n", N);
		free((char *)p);
		free((char *)cot);
                return((double *)NULL);
        }

	beta_prime = (double *)my_malloc((N+1)*sizeof(double));
	array_omega = (Omega_Head *)my_malloc((N+1)*sizeof(Omega_Head));

	for (i=0; i<=N; i++)
	    array_omega[i].list_omega = (double *)my_calloc(M, sizeof(double));

	tmp_omega = (double *)my_calloc(M, sizeof(double));

	/****************************************************************/
	/*								*/
	/* The approximate sum (with error of epsilon) is as follows:	*/
	/*								*/
	/*  Prob.[O(t) <= p] = 						*/
	/*   sum_{c=1}^C sum_{n=c-1}^N exp(-Lambda*t) (Lambda*t)^n/n!	*/
	/*   Omega[n, n-c+1] sum_{i=n-c+1}^n C_n^i p^i (1-p)^{n-i}	*/
	/*								*/
	/* Summation is done column-wise w.r.t. Omega[n, k].		*/
	/*								*/
	/* See Ref. 1, pp. 326.						*/
	/*								*/
	/****************************************************************/

	/* The 1st column is different from other columns. */
	lst_omega = array_omega[0].list_omega;
	for (k = 0; k < M; k++) 	/* 1st element of 1st column */
	      lst_omega[k] = (k < L)? pi[k] : 0 ;

	for (j = 1; j <= N; j++) {	/* other elements of 1st column */

	      sum_states = 0;
	      pre_lst_omega = lst_omega;
	      lst_omega = array_omega[j].list_omega;

	      for (k = 0; k < M; k++) {

		 col_ptr = *(P->col_pp + k);	/* P's column of interest */

		 if (k < L) {		/* operational state */

		    /* Omega[i,j,k] = sum_{l=1}^M Omega[i-1,j,l] P_{lk}   */
		    lst_omega[k] =
			calc_omega_per_state(pre_lst_omega, col_ptr, M);
		    sum_states += lst_omega[k];

		 } else 
		    lst_omega[k] = 0;	/* failure state */

	      }
	      array_omega[j].Omega = sum_states;
	}


	if (mttf)
	    for (l = 0; l <= N; l++) {
		*mf += array_omega[l].Omega;
	    } /* for (l) */

	if (avail || reli || life)
	    for (m = 0; m < num_pairs; m++) {
		pssn_fact = 1;
		erlang_fact = 1 - exp(-Lambda*t[m]);
		for (l = 0; l <= N; l++) {

			this_omega = array_omega[l].Omega;

			if (avail) ea[m] += pssn_fact * this_omega;
			if (reli) re[m]  += pssn_fact * this_omega;
			if (life) el[m]  += erlang_fact * this_omega;

			pssn_fact *= (Lambda*t[m])/(l+1);
			erlang_fact -= exp(-Lambda*t[m]) * pssn_fact;

		} /* for (l) */
	    } /* for (m) */

	/* do other columns;  terminate when e'(N, C) <= epsilon/2 */
	e_prime = array_omega[N].Omega;
	tolerance = 1 - epsilon/2;

	/* Notice that the second column (column 1) is  */
	/* different in recursion from columns 2,3,...	*/ 
	/* For uniformity of recursion for all columns,	*/
	/* shift the 0-th column down by one		*/
	ptr = array_omega[N].list_omega;
	for (l = N; l > 0; l--)
		array_omega[l].list_omega = array_omega[l-1].list_omega;
	array_omega[0].list_omega = ptr;

	for (i = 1; e_prime < tolerance; i++) {

	   /* do Omega[j, j - i + 1] for each row j */
	   for (j = 0; j <= N-i+1; j++) {

	      if (i==1 && j==0) {	/* Omega[0, 0] is unique */
		    
		    for (k = 0; k < M; k++)
	                array_omega[0].list_omega[k] = (k < L)? 0 : pi[k] ;

		    continue;		/* jump to j=1 */
	      }

	      pre_lst_omega = array_omega[j-1].list_omega;
	      lst_omega = array_omega[j].list_omega;
	      bzero((char *)tmp_omega, M*sizeof(double));

	      for (k = 0; k < M; k++) {

		 col_ptr = *(P->col_pp + k);	/* the column of interest */

		 if (k < L) {	/* operating state */

		    /* Omega[i,j,k] = sum_{l=1}^M Omega[i-1,j-1,l] P_{lk} */
		    tmp_omega[k] = (j==0)? 0 :
			calc_omega_per_state(pre_lst_omega, col_ptr, M);

		 } else {	/* failing state */
		    /* Omega[i,j,k] = sum_{l=1}^M Omega[i-1,j,l] P_{lk} */
                    tmp_omega[k] = calc_omega_per_state(lst_omega, col_ptr, M);
		 }
	      } /* for (k) */

	      sum_states = 0;
	      for (k = 0; k < M; k++) {
		 lst_omega[k] = tmp_omega[k];
		 sum_states += tmp_omega[k];
	      }
	      array_omega[j].Omega = sum_states;

	   } /* for (j) */


           for (m = 0; m < num_pairs; m++) {

	      pssn_fact = 1;
	      for (j = 1; j < i; j++) pssn_fact *= Lambda*t[m]/j;

	      for (j = 0; j <= N-i+1; j++) {

		 this_omega = array_omega[j].Omega;
		 if (avail) ea[m] += pssn_fact*this_omega*j/(j+i);
		
		 /* Recursively calculate \sum C_i^n p^i (1-p)^{n-i} */
		 /* Different from but better than the one given at  */
		 /* the end of ref. 1.				     */
		 beta = 0;
		 if (j==0) {	/* yet another special case */
			pw = 1;
			for (l = i-1; l >= 0; l--) {
				beta_prime[l] = pw;
				pw *= p[m];
			}
			pw = 1;
			for (l = 0; l < i; l++) {
				beta_prime[l] *= pw;
				pw *= (1 - p[m]);
			}
			beta = 1;
		 }
		 else {		/* j > 0: do recursion */
			for (l = 0; l < i; l++) {
				beta_prime[l] *=  p[m]*(i+j-1)/(i+j-1-l);
				beta += beta_prime[l];
			}
		 }

		 cot[m] += pssn_fact * this_omega * beta;

		 pssn_fact *= (Lambda*t[m])/(j+i);
	      }
	   }

	   e_prime += array_omega[N-i+1].Omega;	/* \sum_c=0^C omega[N,N-c+1] */

	} /* upon exit, i is the number of columns computed */

	/* attach the proper factor of exp(-Lambda*t) or Lambda */
	for (m = 0; m < num_pairs; m++) {
		exponent = exp(-Lambda*t[m]);
		cot[m] *= exponent;
		if (avail) ea[m] *= exponent;
		if (reli)  re[m] *= exponent;
		if (life)  el[m] /= Lambda;
	}
	if (mttf)  *mf /= Lambda;

	/* free the allocated working-space */
	free((char *)tmp_omega);

	for (i=0; i<=N; i++)
		free((char *)(array_omega[i].list_omega));

	free((char *)array_omega);
	free((char *)beta_prime);
	free((char *)p);

	return((double*)cot);
} /* end of omega() */

/************************************************************************/
/*									*/
/*	calc_omega_per_state(double *, int, int):			*/
/*									*/
/*	calculates Omega[i,j,k] = sum_{l=1}^M Omega[i-1,j-1,l] P_{lk}	*/
/*				- OR -					*/
/*	calculates Omega[i,j,k] = sum_{l=1}^M Omega[i-1,j,l] P_{lk}	*/
/*									*/
/*	Here *ptr points to the list of Omega[i-1,j(-1),s], 1<=s<=M.	*/
/*									*/
/************************************************************************/
double calc_omega_per_state(om_ptr, cl_ptr, M)
double *om_ptr;	/* pointer to the list of Omega[i-1,j(-1),s]	   */
Column *cl_ptr;	/* ptr to the target column in transition matrix P */
int M;		/* total states */
{
	double sum = 0;

	while (cl_ptr) {

		if (cl_ptr->index > M) {
			fprintf(stderr, "Operational time and related measures: ABORTED in calc_omega_per_state!\n");
			exit(86);
		}
		sum += cl_ptr->value * om_ptr[cl_ptr->index - 1];
		cl_ptr = cl_ptr->next;
	}

	return(sum);
} /* end of calc_omega_per_state() */


/************************************************************************/
/*									*/
/*	new_omega(int, double *, Matrix *, double, double, int,		*/
/*	  double *, double *, double *, double *, double *, double *)	*/
/*									*/
/*	Based on Ref. 2.						*/
/*									*/
/*		assumptions:						*/
/*		Same as those for omega().				*/
/*									*/
/************************************************************************/

double *new_omega(L, pi, P, Lambda, epsilon, num_alfa_points, num_t_points,  
                  p, t, ea, re, el, mf)

int    L;            /* number of operational states {1, 2, .., L} */
double *pi;          /* pointer to initial probabilities - contiguous memory */
Matrix *P;           /* pointer to transition prob. matrix - an array of ptr */
double Lambda;       /* uniformization constant */
double epsilon;      /* error */
int num_alfa_points; /* number of points in a interval */
int num_t_points;    /* number of intervals */
double *p;           /* list of s/t */
double *t;           /* list of t's under study */
double *ea;          /* list of expected availability for diff. t's */
double *re;          /* list of reliability for diff. t's */
double *el;          /* list of expected lifetime for diff. t's */
double *mf;          /* mean time to failure */

{
	int c, n;			/* temporary variables.	          */
	/* int l, k; */                 /* temporary variables.	          */
	int i, j, m;                    /* temporary variables.	          */
	int M;				/* total number of states.        */
	int N;				/* 1st approximation (ref. 1)     */
	int C;				/* 2nd approximation (ref. 2)     */
	int avail = 0;			/* flag for availability list     */
	int reli  = 0;			/* flag for reliability list      */
	int life  = 0;			/* flag for lifetime list         */
	int mttf  = 0;			/* flag for mean time to fail     */
        int num_pairs;                  /* num_t_points * num_alfa_points */
	int *N_min, N_min_aux;          /* low bounds                     */
	double max_t;			/* maximum in the t list.         */
	double min_p;			/* p = min {s/t}, ops. region     */ 
	double sum_omega;		/* \sum_k Omega[n, k].            */ 
	double sum_omega0 = 0;          /* \sum Omega[n, n+1].            */ 
	/* double exponent, value; */   /* temporary variables.	          */
        double factor;                  /* combination                    */
        double *val_N_min,val_N_min_aux;/* N_min values                   */
	double *Pn;			/* list of P(n) for each t        */ 
	double *Bin;			/* list of Bin(n,i) for p=s/t     */ 
	double *cot;			/* list of cum. op. time.         */ 
	double *bin_omega;		/* products of binom. & omega     */
        Omega_Head *array_omega;        /* ptr to array Omega[n, k].      */
        double *tmp_omega;              /* ptr to list of temp. Omega     */
        void init_omega();              /* initiaalize the first row      */
        void calc_omega_by_row();       /* calculate the omega's row      */
        double calc_combination();      /* calculate combination          */

	if (!pi || !P || !p || !t)  
          {
	    fprintf(stderr, "Operational time and related measures: Null input, return null.\n");
	    return((double *)NULL);
	  }

	M = DIM_COLUMN(P);
	if (L < 1) {
		fprintf(stderr, "Operational time and related measures: Number of operational states (%d) lower than 1!!!\n", L);
		return((double *)NULL);
	}
	if (L > M) {
		fprintf(stderr, "Operational time and related measures: Number of operational states (%d) higher than number of states (%d)!!!\n", L, M);
		return((double *)NULL);
	}

	/* the min s/t and max t */
	min_p     = p[0];
	max_t     = t[num_t_points - 1];
        num_pairs = num_alfa_points * num_t_points;

	/* 1st cut-off approximation */
	N = partial_poisson_sum(Lambda*max_t, epsilon/2);
	if (N < 0) 
          {
	    fprintf(stderr, "Operational time and related measures: N = %d < 0. Problems with precision or time intervals.\n", N);
	    free((char *)p);
	    return((double *)NULL);
	  }

	/* 2nd cut-off approximation */
	C = partial_poisson_sum(Lambda*max_t*(1-min_p), epsilon/2); 
	C = MAX(2, MIN(C, N));	/* C is no bigger than N, but at least 2 */

	/* low bound */
	N_min     = (int    *)my_calloc(num_t_points, sizeof(int));
	val_N_min = (double *)my_calloc(num_t_points, sizeof(double));

        for (i = 0; i < num_t_points; i++)
          {
	    lo_bound(&N_min_aux, &val_N_min_aux, Lambda * t[i]); 
            N_min[i]     = N_min_aux;
            val_N_min[i] = val_N_min_aux;

#ifdef DEBUG
fprintf(stderr, "Operational time and related measures: N_min[%d] = %d\n", i, N_min[i]);
#endif
          }
#ifdef DEBUG
fprintf(stderr, "Operational time and related measures: N = %d, C = %d\n", N, C);
#endif

        array_omega = (Omega_Head *)my_malloc((C+1)*sizeof(Omega_Head));
        for (i = 0; i <= C; i++)
          array_omega[i].list_omega = (double *)my_calloc(M, sizeof(double));

        tmp_omega = (double *)my_calloc(M, sizeof(double));

	if (ea) {
		avail = 1;
		bzero((char *)ea, num_t_points * sizeof(double));
	}

	if (re) {
		reli = 1;
		bzero((char *)re, num_t_points * sizeof(double));
	}

	if (el) {
		life = 1;
		bzero((char *)el, num_t_points * sizeof(double));
	}

	if (mf) {
		mttf = 1;
		bzero((char *)mf, sizeof(double));
	}

	/* allocate lists of temporary variables (Yak!). */
	Pn        = (double *)my_calloc(num_t_points,    sizeof(double));
	Bin       = (double *)my_calloc(num_alfa_points, sizeof(double));
	bin_omega = (double *)my_calloc(num_alfa_points, sizeof(double));

	/* allocate list of returned cumulative operation time dist.*/
	cot = (double *)my_calloc(num_pairs, sizeof(double));

	/****************************************************************/
	/*								*/
	/* The approximate sum (with error of epsilon) is as follows:	*/
	/*								*/
	/*  Prob.[O(t) <= p] = sum_{n=0}^N P(n)				*/
	/*		       sum_{j=0}^MIN(C,n) Bin(n, j)		*/
	/*		       sum_{k=n-j+1}^{n+1} Omega(n, k).	        */
	/*								*/
	/* Here p = s/t, P(n) = exp(-Lambda*t) (Lambda*t)^n/n!,		*/
	/*      Bin(n, j) = C_n^j p^{n-j} (1-p)^j.			*/
	/*								*/
	/****************************************************************/

        for (m = 0; m < num_t_points; m++) 
          {
            if (N_min[m] == 0)
               Pn[m] = exp(-Lambda*t[m]);
            else
               Pn[m] = val_N_min[m];
          }

#ifdef DEBUG
        fprintf(stderr, "Operational time methods: n = ");
#endif

	for (n = 0; n <= N; n++) 
          {
#ifdef DEBUG
            fprintf(stderr, "%d, ", n);
#endif
            fflush(stdout);
	    for (m = 0; m < num_alfa_points; m++) 
              {
		bin_omega[m] = 0;
                Bin[m] = 0;
	      }

	    sum_omega = 0;
            factor = 0;

	    if (n == 0)
		init_omega(array_omega, L, M, pi);
	    else 
		calc_omega_by_row(array_omega, MIN(n+1,C), L, M, P, tmp_omega);

            for (j = 0; j < MIN(n+1, C); j++) 
               {
                 sum_omega += array_omega[MIN(n+1, C) - j].Omega;

                 for (m = 0; m < num_alfa_points; m++) 
                   {
		     Bin[m]  = pow(p[m], n+1-MIN(n+1,C)+j) *
		               pow(1-p[m], MIN(n+1,C)-j-1);
                     factor  = calc_combination(n, n+1-MIN(n+1,C)+j);
                     Bin[m] *= factor;
                     bin_omega[m] += Bin[m] * sum_omega;

                   } /* m */
            } /* j */

            i = 0;
            j = 0;
            for (m = 0; m < num_pairs; m++)
              {

                if (n >= N_min[i])
                   cot[m] += Pn[i] * bin_omega[j];

                j++;
                if (j >= num_alfa_points)
                  {
                    j = 0;
                    i++;
                  }
              }

	    /* other misc. measures */
	    sum_omega   = 0;	
	    sum_omega0 += array_omega[0].Omega;
	    if (avail)
	        for (c = 0; c <= MIN(n, C); c++) 
                   sum_omega += array_omega[c].Omega*(n+1-c)/(n+1);

	    if (avail || reli || life)
		for (m = 0; m < num_t_points; m++) 
                  {
                    if (n >= N_min[m])
                      {
		        if (avail) ea[m] += Pn[m] * sum_omega;
		        if (reli)  re[m] += Pn[m] * array_omega[0].Omega;
		        if (life)  el[m] += Pn[m] * (sum_omega0 / (n+1));
                      }

        	  }

	    if (mttf)
		*mf += array_omega[0].Omega;
	    
	    /* advancing to next round of iteration */
            for (m = 0; m < num_t_points; m++) 
              {
                if (n >= N_min[m])
                   Pn[m] *= Lambda*t[m]/(n+1);
              }
	}  /* n */
/*
        for (m = 0; m < num_pairs; m++)
           cot[m] = 1 - cot[m];
*/
        /* attach the proper factor of exp(-Lambda*t) or Lambda */
	if (life) 
           for (m = 0; m < num_t_points; m++)
               el[m] *= t[m];

        if (mttf)  
          {
            while(array_omega[0].Omega > epsilon)
              {
		calc_omega_by_row(array_omega, 0, L, M, P, tmp_omega);
		*mf += array_omega[0].Omega;
              }
            *mf /= Lambda;
          }

        for (i=0; i<=C; i++)
          free((char *)(array_omega[i].list_omega));

        free((char *)array_omega);
        free((char *)tmp_omega);
	free((char *)Pn);
	free((char *)Bin);
	free((char *)bin_omega);
	free((char *)N_min);
	free((char *)val_N_min);

	return((double *)cot);
} /* end of new_omega() */

/************************************************************************/
/*                                                                      */
/*	int init_omega(Omega_Head *, int, int, double *)		*/
/*                                                                      */
/*	Initialize the first row of Omega's,				*/
/*	i.e., Omega(0,1) & Om(0,0).					*/
/*                                                                      */
/************************************************************************/

void init_omega(array_omega, L, M, pi)
Omega_Head *array_omega;	/* ptr to a row of omega's */
int L;				/* number of operational states */
int M;				/* total number of states */
double *pi;			/* initial state dist. probability */
{
	int s;
	double sum = 0;

	for (s=0; s<M; s++) {
	    array_omega[0].list_omega[s] = (s < L)? pi[s] : 0;
	    array_omega[1].list_omega[s] = (s < L)? 0 : pi[s];
	    sum += (s < L)? pi[s] : 0;
	}
	array_omega[0].Omega = sum;
	array_omega[1].Omega = 1- sum;
} /* end of init_omega() */

/************************************************************************/
/*                                                                      */
/*      int calc_omega_by_row(Omega_Head *, int, int, int, Matrix *,	*/
/*				 double *)				*/
/*                                                                      */
/*	Calculating the omega's row by row:				*/
/*	given Omega[n,.], calculate Omega[n+1,.] in place.		*/
/*                                                                      */
/************************************************************************/

void calc_omega_by_row(array_omega, bd, L, M, P, tmp_omega)
Omega_Head *array_omega;	/* ptr to a row of omega's */
int bd;				/* boundary of the useful strip */
int L;				/* number of operational states */
int M;				/* total number of states */
Matrix *P;			/* the transition probability matrix */
double *tmp_omega;		/* ptr to list of temp. vars for omega's */
{
	int k;		               /* temp. counters */
	int s;		               /* integral var for state */
	double sum;	               /* temp. var */
	double *self_omega_pt;         /* ptr to current Omega */
	double *prev_omega_pt;         /* ptr to previous Omega */
	Column *col_ptr;               /* ptr to a column of matrix P*/
	double calc_omega_per_state(); /* calculate a state Omega    */

	for (k = bd; k >= 0; k--) {

            self_omega_pt = array_omega[k].list_omega;
	    prev_omega_pt = ( k > 0 ) ? (array_omega[k-1].list_omega) : NULL;

            for (s=0; s<M; s++) {

                col_ptr = *(P->col_pp + s);            

                if (s < L) 	/* s is a operational state */
                    tmp_omega[s] =
			calc_omega_per_state(self_omega_pt, col_ptr, M);
            	else 		/* s is a failure state */
		    tmp_omega[s] = (k == 0)? 0 :
		        calc_omega_per_state(prev_omega_pt, col_ptr, M);

            } /* for (s) */

	    /* record newly calculated results */
	    sum = 0;
	    for (s=0; s<M; s++) {
		self_omega_pt[s] = tmp_omega[s];
		sum += tmp_omega[s];
	    }
	    array_omega[k].Omega = sum;

	} /* for (k) */

} /* end of calc_omega_by_row() */

/* That's all the Omega calculation routines.  Try to simplify them.. */

/************************************************************************/
/*                                                                      */
/*	Calculating combination         				*/
/*	by Morganna in 08/10/96.                             		*/
/*                                                                      */
/************************************************************************/

double calc_combination(n, k)
int n;  			/* number of elements */
int k;				/* number of combinations */
{
	double i, comb;         /* temporary variables */

        comb = 1;

	for (i = 0; i < k; i++) 
            comb = (comb * (n-i)) / (k-i);

        return(comb);
}
