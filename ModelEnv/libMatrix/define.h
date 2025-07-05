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
/*									*/
/*		Header File: structure definitions			*/
/*									*/
/*		Yuguang Wu 8/12/93					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*		Elements of a matrix column in linked-list structure.	*/
/************************************************************************/
typedef struct column {
	int index;		/* element row number */
	double value;		/* element value (e.g., prob.) */
	struct column *next;	/* pointer to next element in the column */
} Column;


/************************************************************************/
/*		Elements of a matrix column in linked-list structure.	*/
/************************************************************************/
typedef struct matrix {		
	int num_col;		/* the number of columns in the matrix */	
	Column **col_pp;	/* pointer to the first column list    */
} Matrix;

/***********************************************************************/
/* another type of matrix structure, it is different to Matrix struct  */
/***********************************************************************/
typedef struct matrix2 {        /* struct of elements in matrix P      */
	int num_row;            /* no. rows in R_{i,j} = ||P_ii||      */ 
	int num_col;            /*  no. col in R_{i,j} = ||P_jj||      */
	double **array;         /* pointer to a 2 dimensional array    */
			        /* (*array) points to the row list     */
			        /* (array) points to a row element     */
			        /* i e, R(i,j)                         */
			        /* array==NULL if R_{i,j} is not initialized */ 
} Matrix2;

/***********************************************************************/
/* list of omegas                                                      */
/***********************************************************************/
typedef struct omega_head {
	double Omega;		/* Omega[n, k]=sum_{s=1}^M Omega[n,k,s] */
	double *list_omega;	/* list of Omega[n, k, s] for fixed n,k */
} Omega_Head;

/************************************************************************/
/*		vector of transitions caused by timeout         	*/
/************************************************************************/
typedef struct timeout_state{
	int final_state;         	  /* final state number */
	struct timeout_state *next;	  /* pointer to next element */
} Timeout_state;

/************************************************************************/
/*		Elements of a global states.	                        */
/************************************************************************/
typedef struct entry {	
        char state[15];	          /* global state description */
} Entry;                                     
                                   
/************************************************************************/
/*		Event definition                                        */
/************************************************************************/
typedef struct event_def {                          	
        double timeout;              /* timeout */
        int    num_chains;           /* number of chains */
        int    *entries_list;        /* list of chains entries */
        int    num_global_stat;      /* number of global states of event */
        int    num_abs_global_stat;  /* number of absorbing states of event */
        int    *global_states;       /* global states of event */
        int    num_abs_stat;         /* number of absorbing states of preemptive chain */
        int    **desc_states;        /* descricao dos estados das cadeias independentes */
        double lambdas_sum;          /* lambdas sum */
        double *lambdas;             /* lambdas list */
        Matrix *P;                   /* probability matrix of chains of event */
} Event;

/************************************************************************/
/*		Upsilon definition                                      */
/*  - only impulse rewards are present                                  */ 
/************************************************************************/
typedef struct upsilon_s {                      	
        double accum_rew;         /* total accumulated reward */
        double prob;              /* probability of reward accumulated */
        struct upsilon_s *next;   /* next element */                      	
} Upsilon_s;

typedef struct states_upsilons {                          	
        Upsilon_s *previous;      /* upsilon_s[n-1, r] */
        Upsilon_s *actual;        /* upsilon_s[n, r] */
} States_upsilons;     

/************************************************************************/
/*  cumulative impulse rewards vector definition                        */
/*  - impulse and states rewards are present                            */
/************************************************************************/
typedef struct cum_imp_rew {                          	
        int    new_ptr;           /* it is a new cumulative reward ? */
        double value;             /* cumulative impulse reward value */
        double *previous_Psi;     /* ptr to Psi[i, n-1, u, r] */
        double *actual_Psi;       /* ptr to Psi[i, n, u, r] */
        struct cum_imp_rew *next; /* ptr to next element */
} Cum_imp_rew;     



/************************************************************************/
/*       Iterative Method                                               */
/************************************************************************/

#define CONVERGENCE 0
#define MAX_ITERATIONS 1


/************************************************************************/

#define MARKUP 0.001	        /* experience by Edmundo */

/************************************************************************/
/*              Misc. macros that might be useful in future.		*/
/************************************************************************/
#define MAX(a, b) ((a)>(b)? (a):(b))
#define MIN(a, b) ((a)>(b)? (b):(a))
#define ABS(a) ((a)>0? (a):-(a))

#define DIM_COLUMN(P) ((P)->num_col)

/*************************************************************************/
/* MACROS for options choosed by user in Rate Cumulative Reward          */
/*  based in approximation transient method                      	 */
/*************************************************************************/

#define STATE_REWARD 1
#define SET_REWARD   2


/*************************************************************************/
/* MACROS for options choosed by user in Efficient transient State       */
/*  Probability   Approximation method                                   */
/*************************************************************************/


#define STATE_PROB 1
#define SET_PROB   2
#define EXPECTED_VAL 3


/***************************************************************************/
/*  Struct used in Cumulative Rate Reward and Efficient                    */
/* transient State Probability Approximation method(Iterative Technique)   */
/***************************************************************************/
typedef struct {
        int R;
        double step;
        int total_points;
}info_intervals;

/**************************************************************************/
/* Iterative Methods used in Cumulative Rate Reward and Efficient         */
/* transient State Probability Approximation method(Iterative Technique)  */
/**************************************************************************/

#define SOR_METHOD          1
#define GAUSS_METHOD        2
#define JACOBI_METHOD       3
#define POWER_METHOD        4

