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
/*              Sparse matrix input and output routines.                */
/*              input/output:                                           */
/*                      3-tuples of (source, dest, probability).        */
/*			when source<=0, input ends.	                                */
/*              output/input:                                           */
/*                      transition matrix in column-wise link lists.    */
/*                                                                      */
/*		Yuguang Wu UCLA CSD 8/12/93                                     */
/*                                                                      */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "define.h"
#include "Matrix.h"


char *my_calloc();


/****************************************************************************/
/*                                                                          */
/* initiate matrix n x n                                                    */
/* Morganna in 09/04/95                                                     */
/*                                                                          */
/****************************************************************************/
Matrix *initiate_matrix(num)
int num;                          /* matrix dimention */
{
   Column **cp_head;              /* ptr to the array of column ptrs */
   Matrix *mat;                   /* ptr to matrix                   */
 
   if (num <= 0)
   {
       printf("initiate_matrix: *** error - columns number of a matrix should be greater zero\n");
       exit(50);
   }

   /* allocate final probability matrix */
   mat = (Matrix *)my_calloc(1, sizeof(Matrix));

   /* set the number of states in the matrix struct */
   mat->num_col = num;

   /* allocate 'num_global_stat' of pointers of column structure. */
   cp_head = (Column **)my_calloc(num, sizeof(Column *));

   /* set the pointer to 1st column struct in the matrix struct */
   mat->col_pp = cp_head;

   return((Matrix *)mat);
}



/****************************************************************************/
/*                                                                          */
/*  put an element in a matrix                                              */
/*  - put "value" in row "s" and column "d" of matrix "mat"                 */
/*  by Morganna in 09/04/95                                                 */
/*                                                                          */
/****************************************************************************/
void put_matrix_position(s, d, value, mat)
int s;                             /* row number    */
int d;                             /* column number */
double value;                      /* value         */
Matrix *mat;                       /* ptr to matrix */
{
   Column *ptr_col, *cp;           /* ptr to a element in a column list */
   Column *pptr_col;               /* ptr to a element in a column list */
   Column **cp_col;                /* ptr to the array of column ptrs   */


   cp_col = mat->col_pp + d - 1;

   /* find the position in the column */
   pptr_col = NULL;
   ptr_col  = *cp_col;
   while (ptr_col && ptr_col->index < s) 
   {
       pptr_col = ptr_col;
       ptr_col  = ptr_col->next;
   }

   /* found the element */ 
   if (ptr_col && ptr_col->index == s) 
   {
       ptr_col->value = value;  /* set value */
       return;
   }

   /* *cp_col  = pointer to the first    column element */
   /* pptr_col = pointer to the previous column element */
   /* ptr_col  = pointer to the next     column element */

   /* allocate memory to the new element */
   cp = (Column *)my_calloc(1, sizeof(Column));

   /* set fields of the new element */
   cp->index = s;
   cp->value = value;
   cp->next  = NULL;

   /* it's the first element */
   if (*cp_col == NULL)
       *cp_col = cp;
   else
      if (pptr_col == NULL)  
      {   /* the new element will be the 1st in the column */
          *cp_col = cp;  
          cp->next = ptr_col;
      }
      else
      {
          cp->next = pptr_col->next;
          pptr_col->next = cp;
      }
}



/****************************************************************************/
/*                                                                          */
/*  add a value to an element of a matrix                                   */
/*  - add "value" in row "s" and column "d" of matrix "mat"                 */
/*  by Morganna in 05/10/95                                                 */
/*                                                                          */
/****************************************************************************/
void add_matrix_position(s, d, value, mat)                
int s;                            /* row number    */
int d;                            /* column number */
double value;                     /* value         */
Matrix *mat;                      /* ptr to matrix */
{
   Column *ptr_col, *cp;          /* ptr to a element in a column list */
   Column *pptr_col;              /* ptr to a element in a column list */
   Column **cp_col;               /* ptr to the array of column ptrs   */


   if (value == 0 && s != d)
      return;

   cp_col = mat->col_pp + d - 1;

   /* find the position in the column */
   pptr_col = NULL;
   ptr_col = *cp_col;
   while (ptr_col && ptr_col->index < s) 
   {
       pptr_col = ptr_col;
       ptr_col  = ptr_col->next;
   }

   /* found the element */ 
   if (ptr_col && ptr_col->index == s) 
   {
       ptr_col->value += value;  /* set value */
       return;
   }

   /* *cp_col  = pointer to the first    column element */
   /* pptr_col = pointer to the previous column element */
   /* ptr_col  = pointer to the next     column element */

   /* allocate memory to the new element */
   cp = (Column *)my_calloc(1, sizeof(Column));

   /* set fields of the new element */
   cp->index  = s;
   cp->value  = value;
   cp->next   = NULL;

   /* it's the first element */
   if (*cp_col == NULL)
       *cp_col = cp;
   else
      if (pptr_col == NULL)  
      {   /* the new element will be the 1st in the column */
          *cp_col = cp;  
          cp->next = ptr_col;
      }
      else
      {
          cp->next = pptr_col->next;
          pptr_col->next = cp;
      }
}



/****************************************************************************/
/*                                                                          */
/*  return the value in a position of matrix                                */
/*  - which is the element in row "i" and column "j" of matrix "mat" ?      */
/*  by Morganna in 09/04/95                                                 */
/*                                                                          */
/****************************************************************************/
double get_matrix_position(i, j, mat)
int i;                            /* row number    */
int j;                            /* column number */
Matrix *mat;                      /* ptr to matrix */
{
  double value = 0;               /* return zero                       */
  Column *ptr_aux;                /* ptr to a element in a column list */
  Column **ptr_aux2;              /* ptr to the address of column      */


  ptr_aux2 = mat->col_pp + j - 1; /* ptr to the column list        */
  ptr_aux  = *ptr_aux2;           /* ptr to the 1st column element */
  
  while (ptr_aux)
  {
      if (ptr_aux->index > i)
          return(value);
      if (ptr_aux->index == i)
          return(ptr_aux->value);
      ptr_aux = ptr_aux->next;
  }
  
  return(value);
}



/************************************************************************/
/*                                                                      */
/* generate diagonal of a matrix                                        */
/* by Morganna in 09/04/95                                              */
/*                                                                      */
/************************************************************************/
void generate_diagonal(mat_type, mat)
char   mat_type;                /* matrix type: prob. or rates */
Matrix *mat;	               	/* ptr to matrix               */
{
        int i;                  /* temporary variables                     */
        int num_states;         /* number of states                        */
        double *sum;            /* sum of a column                         */ 
        Column *cp;             /* ptr to a element in a column list       */
        Column **pcp;           /* ptr to the address of previous col. ptr */


	num_states = mat->num_col;
        pcp        = mat->col_pp;
	sum        = (double *)my_calloc(num_states, sizeof(double)); 

        for (i = 1; i <= num_states; i++)
        {
            cp = *pcp;

            /* find the proper position to insert the new element */
            while (cp) 
            {
                if (cp->index != i)
                   sum[cp->index - 1] += cp->value;
	        cp = cp->next;
            } 
            pcp++;
        }

        for (i = 1; i <= num_states; i++) 
        {
            if (mat_type == 'P')
               put_matrix_position(i, i, 1 - sum[i-1], mat);
            else
               if (sum == 0)
                  put_matrix_position(i, i, sum[i-1], mat);
               else
                  put_matrix_position(i, i, -sum[i-1], mat);
        }

        free((char *)sum);
}



/************************************************************************/
/*                                                                      */
/*      check_diagonal(Matrix *, int):					*/
/*		checking the existence of all diagonal elements.	*/
/*									*/
/*									*/
/************************************************************************/
int check_diagonal(M, N)
Matrix *M;				/* ptr to the matrix */
int N;					/* number of columns */
{
	int col;			/* column number */
	Column **cp;			/* ptr to ptr to matrix columns */
	Column *pcp;			/* ptr to matrix columns */


	cp = (Column **)M->col_pp;	/* ptr to 1st column in A */

	/* check all columns for their diagonal elements */
	for (col=1; col<=N; col++) 
	{
		pcp = *cp;		/* get the current column */

		while (pcp) 
		{
			if (pcp->index == col) break;	/* it's diag. */
			pcp = pcp->next;
		}

		if (!pcp) return(0);	/* this col. has no diag.     */
		++cp;			/* advance to the next column */
	}
	return(1);
}



/************************************************************************/
/*                                                                      */
/*      vector_matrix_multiply(double *, double *, Matrix *):		*/
/*      routine to multiply a vector with a matrix.			*/
/*                                                                      */
/*	Note:								*/
/*	The column-major representation of matrix is for ease of	*/
/*	vector-matrix multiplication.  However, the row-major		*/
/*	representation also yields easy vec-mat multiplication,		*/
/*	so either way is ok.						*/
/*									*/
/************************************************************************/
double *vector_matrix_multiply(vec, mat, res_vec)
double *vec;	 /* pointer to a vector - contiguous memory of elements    */
Matrix *mat;	 /* square matrix - consisting an array of column pointers */
double *res_vec; /* pointer to the resulting vector                        */
{
	int i;
	int mc;			/* matrix dimension ( number of columns ) */
	double *dp;		/* pointer to vector elements             */
	Column **cp;		/* pointer of pointer to matrix columns   */
	Column *pcp;		/* pointer to matrix columns              */



	if (!vec || !mat)	/* nil vec or matrix is ignored */
		return((double *)NULL);

	mc = DIM_COLUMN(mat);	/* square matrix dimension = vector dim */

	/* allocate the return vector here if needed. */
	/* res_vec = (double *)my_calloc(mc, sizeof(double)); */

	if (res_vec == NULL) return(NULL);
	bzero((char *)res_vec, mc*sizeof(double));

	dp = res_vec;	           /* first element of the allocated vector */
	cp = (Column **)mat->col_pp;	/* pointer to the 1st matrix column */

	/* calculate each element of the vector */
	for (i=0; i<mc; i++) 
	{
		pcp = *cp;	/* get the current column */

		/* iterate through the column */
		while (pcp) 
		{
			/* multiply and add */
			*dp += (pcp->value)*(vec[pcp->index - 1]);

			pcp = pcp->next;
		}

		++dp;		/* advance the vector element */
		++cp;		/* advance the matrix column */
	}

	return (res_vec);	/* here we go */
	
} /* end of vector_matrix_multiply() */


/******************************************************************************/
/* This routine returns the uniformization factor, also known as lambda, for  */
/*     rate matrix Q.                                                         */
/*  Input:                                                                    */
/*        Q      : rate matrix                                                */
/*  Output:                                                                   */
/*        uniformization factor                                               */
/******************************************************************************/
double get_uniformization_factor( Q )
Matrix *Q;
{
	int num_stat;         /* number of states in the model              */
	int col;              /* index to column number                     */
	double max;           /* maximum element in matrix                  */
	Column *pp;           /* ptr to a element in a column list in Q     */
	Column **pcp;         /* ptr to the addr of prev. ptr in P's column */

	if( !Q ) return(0.0);   /* empty matrix; ignore */

	num_stat = Q->num_col;  /* max. state no. in matrix */
	pcp      = Q->col_pp;   /* pointer to the 1st column list */

	if( !num_stat ) return(0.0);    /* empty matrix; ignore */

	max = 0.0;      /* least transition probability in Q; -1 tops all */

	for(col=1;col<=num_stat;pcp++,col++) 
	{       /* loop through every column */
		/* loop through all elements in this column */
		
		for (pp = *pcp; pp; pp = pp->next) 
		{
			/* only a diagonal element matters */
			if (pp->index == col) 
			{
				max = MAX(max, ABS(pp->value));
				break;  /* go to next column */
			}
		}
	} /* found max in matrix Q */

    return( max + MARKUP );	/* MARKUP = 0.001 by Edmundo experience */

}


/******************************************************************************/
/* This routine creates the probability matrix given the rate matrix Q and the*/
/*    uniformization factor Lambda.                                           */
/*  Input:                                                                    */
/*        Q      : rate matrix to be uniformized                              */
/*        Lambda : uniformization factor                                      */
/*  Output:                                                                   */
/*        probability matrix                                                  */
/******************************************************************************/
Matrix *create_prob_matrix( Q, Lambda )
Matrix *Q;
double Lambda;
{
	int num_stat;         /* number of states in the model              */
	Matrix *P;            /* ptr to the uniformized matrix P            */
	int col;              /* index to column number                     */
	Column **pcp, **ppp;  /* ptr to the addr of prev. ptr in P's column */
	Column **qcp;         /* ptr to the addr of prev. ptr in Q's column */
	Column *pp;           /* ptr to a element in a column list in Q     */


	num_stat   = Q->num_col;
	/* now we creat the brand-new uniformized matrix P. */
	/* allocate the matrix and an array of column ptrs. */
	P = (Matrix *)my_calloc(1, sizeof(Matrix));
	P->col_pp = (Column **)my_calloc(num_stat,sizeof(Column *));

	/* set the number of states in the matrix struct */
	P->num_col = num_stat;

	/* pointers to the 1st column list of matrix P and Q */
	pcp = P->col_pp;		
	qcp = Q->col_pp;

	/* copy matrix Q so P = Q/Lambda. */
	for ( col = 1; col <= num_stat; pcp++, qcp++, col++ ) 
	{
		ppp = pcp;	/* ptr to the address of the prev. ptr */
		pp = *qcp;	/* ptr to 1st element of the current column */

		while (pp) 
		{	/* copy this column to P */
			*ppp = (Column *)my_calloc(1, sizeof(Column));
			(*ppp)->index = pp->index;
			(*ppp)->value = pp->value / Lambda;
			
			ppp = &((*ppp)->next);	   /* update the ptr to ptr */

			pp = pp->next;
		}
	}

	pcp = P->col_pp;		
	/* make P = P + I. */
	for ( col = 1; col <= num_stat; pcp++, qcp++, col++ ) 
	{	/* loop cols */

		ppp = pcp;	/* ptr to the address of the prev. ptr      */
		pp = *pcp;	/* ptr to 1st element of the current column */

		while (pp && pp->index < col) 
		{				/* look for the diagonal */
			ppp = &(pp->next);	/* update the ptr to ptr */
			pp = pp->next;		/* go to next element    */
		}
		
		if (pp && pp->index == col) 
		{				/* found the diagonal	 */
			pp->value += 1;		/* p(i,i) = p(i,i) + 1.  */
		} 
		else 
		{				/* non-existent, add it. */
		
			/* link to previous element in the column */
			*ppp = (Column *)my_calloc(1, sizeof(Column));

			(*ppp)->index = col;
			(*ppp)->value = 1;
			(*ppp)->next = pp;	/* link to next in column */
		}
	}

	return((Matrix *) P);	/* uniformization done. */
}
/************************************************************************/
/*                                                                      */
/* Routine for sparse matrix uniformization.                            */
/*  input:                                                              */
/*        transition matrix in column-wise link lists.                  */
/*        pointer to matrix Q                                           */
/*  output:	                                                            */
/*        uniformized matrix in column-wise link lists.	                */
/*        pointer to matrix P                                           */
/*                                                                      */
/*  Yuguang Wu UCLA CSD 8/12/93                                         */
/*                                                                      */
/*                                                                      */
/* uniformize_matrix fucntion has been changed such that it returns     */
/* LAMBDA                                                               */
/* Joao Carlos Guedes 11/1/94                                           */
/*                                                                      */
/************************************************************************/
Matrix *uniformize_matrix( Q, LAMBDA )
Matrix *Q;
double *LAMBDA;
{
    double lambda;
    Matrix *P;

    P      = NULL;
    lambda = get_uniformization_factor( Q );
    if( lambda != 0.0 )
        P = create_prob_matrix( Q, lambda );

    if( LAMBDA != NULL )
        *LAMBDA = lambda;

    return( P );
}

/************************************************************************/
/*									*/
/* Routine for sparse matrix uniformization.	                	*/
/*  input:						        	*/
/*        transition matrix in column-wise link lists.          	*/
/*        pointer to matrix Q				                */
/*  output:						        	*/
/*        uniformized matrix in column-wise link lists.	                */
/*        pointer to matrix Q		                 		*/
/*									*/
/*  Yuguang Wu UCLA CSD 8/12/93	                			*/
/*									*/
/*                                                                      */
/* uniformize_matrix fucntion has been changed such that it returns     */
/* LAMBDA								*/
/* Joao CArlos Guedes 11/1/94 						*/
/*									*/
/*									*/
/* altered by Morganna in 06/10/95                                      */
/* - included Matrix pointers for rows                                  */
/* - it is used in the new GTH rotine   				*/
/*									*/
/************************************************************************/
void uniformize_matrix2(Q,LAMBDA)
Matrix *Q;
double *LAMBDA;

{
	int num_stat;           /* number of states in the model              */
	int col;                /* index to column number                     */
    double min = 1;         /* the smallest transaction probability       */
	double Lambda;          /* Lambda, the uniformization constant        */
	Column *qp;            	/* ptr to a element in a column list in Q     */
	Column **qcp;           /* ptr to the addr of prev. ptr in Q's column */

	Lambda = get_uniformization_factor( Q );

#ifdef DEBUG
	printf("Lambda = %f\n", Lambda);
#endif

    num_stat = Q->num_col;

	/* pointer to the 1st column list of matrix Q */
	qcp  = Q->col_pp;

	/* Q = Q/Lambda + I. */
	for ( col = 1; col <= num_stat; qcp++, col++ ) 
        {

		qp = *qcp;	/* ptr to 1st element of the current column */

		while (qp) 
                {
                    if (qp->index == col)
                       qp->value = (qp->value / Lambda) + 1;
                    else
                       qp->value = qp->value / Lambda;

                    min = MIN(min, qp->value);
                    qp = qp->next;
		}
	}

        printf("min = %.10e\n", min);
	*LAMBDA = Lambda;
}



/************************************************************************/
/*									*/
/* input: trans probability matrix                                      */
/* output: P-I; P is overwritten by P-I                                 */
/*									*/
/************************************************************************/
Matrix *set_M(P)
Matrix *P;

{
    Column *cp;    /*ptr to a element in a column list  */
    Column **pcp;  /*ptr to the array of column ptr  */
    int col;


    for (col = 1; col <= P->num_col; col++)
    {
        /* set pcp to the pointer of the col-th column  */
        pcp = P->col_pp + (col-1);

        if (*pcp == NULL) 
        {  /* column does not exist  */
            fprintf(stderr,"set_M: column does not exist!\n");
            exit(50);
        }

        /* here, there is at least one element in the column  */
        cp = *pcp;

        /* find position of diagonal element  */
        while (cp && cp->index < col) cp = cp->next;

        if (cp && cp->index == col) 
            (cp->value) -= 1;  /* got it  */
            
        else 
        {   /* problem: row not in this column  */
            fprintf(stderr,"set_M: diagonal element (%d,%d) not found!\n",col,col);
            exit(50);
        }
    }
    return (P);
}


int matrix_multiply( A, B, result )
Matrix *A, *B, *result;
{
    int ok;
    int i, j, k;
    double sum;

    ok = 0;
    if( A->num_col == B->num_col && B->num_col == result->num_col )
    {
        for( i = 1; i <= A->num_col; i++ )
            for( j = 1; j <= B->num_col; j++ )
            {
                sum = 0.0;
                for( k = 1; k <= B->num_col; k++ )
                    sum += get_matrix_position(i, k, A) * 
                           get_matrix_position(k, j, B);
                if( sum != 0.0 )
                    put_matrix_position(i, j, sum, result);
            }
        ok = 1;
    }
    return( ok );
}

/******************************************************************************/
/* This routine generate H matrix by multipling every element of R by its     */
/*    equivalent in P, i. e., H(i,j) = R(i,j)*P(i,j). The resulting matrix is */
/*    stored in R since it will be no longer needed.                          */
/* Return value:                                                              */
/*    true  : if the routine finished with success;                           */
/*    false : if it found an error.                                           */
/******************************************************************************/
int generate_H_matrix( R, P )
Matrix *R, *P;
{
    int status;
    int col;
	Column *pp;           /* ptr to a element in a column list in R     */
	Column **pcp;         /* ptr to the addr of prev. ptr in R's column */

    status = 0;
    if( R && P && R->num_col == P->num_col )
    {
        pcp = R->col_pp;
        for( col = 1; col <= R->num_col; col++, pcp++ )
        {
            for( pp = *pcp; pp; pp = pp->next )
            {
                pp->value *= get_matrix_position( pp->index, col, P );
            }
        }
        status = 1;
    }

    return( status );
}
