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
#include <stdlib.h>
#include <string.h>
#include "sparseMatrix.h"
#include "functions_espa_esra.h"


#ifdef PI
#undef PI
#endif

/*#define DEBUG*/
#define COST

#ifdef COST
#define REDUCTION	1
#define CALC_W		2
#define CALC_Z		3
#define SOLVE_A_X_B	4
#define LU_ROUTINE	5
#define MULTIPLY_A_B	6
#define MUL_A_V		7
#define OTHER		8
#define ROUTINES	8

long int	n_mul[ROUTINES + 1];
int		caller = OTHER;
#endif


/********************************************************/
/*   void LU2(Matrix2 *): LU decomposition by Gauss   	*/
/*  elimination of a  matrix 		              	*/
/*  Does not do pivoting			      	*/
/*  this is algorithm 2.4 in "Introduction to matrix  	*/
/*  computations by G. Stewart, academic press, 1973  	*/
/*  Overwrites original matrix with the solution      	*/
/*  returns input matrix factorized		      	*/
/*  It's essentially the LU function from option_1.c 	*/
/*  slightly modified to avoid operations with null	*/
/*  elements of A beneath the diagonal.			*/
/********************************************************/
void LU2(A)
Matrix2 *A;	/* M is a Matrix2 structure  */
{
	int 	i, j, k, m, n, r;
	double 	**a;	/* ptr to ptr to array element  */
 

	m = A->num_row;
	if (m <= 1) return;	/* nothing to be done */
	n = A->num_col;
	a = A->array;	/* sets array to ptr to matrix  */

	/* sets r to the minimum between m-1 and n */
	r = MIN(m-1,n);

	for(k = 0; k < r; k++)
		for(i = k + 1; i < m; i++)
			if (a[i][k] != 0)
			{
				a[i][k] /= a[k][k];
				for(j = k + 1; j < n; j++)
					a[i][j] -= a[i][k] * a[k][j];
#ifdef COST
n_mul[LU_ROUTINE] += n - k - 1;
n_mul[caller] += n - k - 1;
#endif
			}
}

/********************************************************/
/* Matrix2 *solve_A_X_B(Matrix2 *, Matrix2 *)		*/
/* Solves LY=B, storing Y in the same array as X.	*/
/* Returns a pointer to answer matrix X, obtained from	*/
/* UX=Y.						*/
/* Matrix A should be in the form LU, where L is a	*/
/* lower triangular matrix with 1's in his diagonal;	*/
/* this is implicit, since A's diagonal elements are	*/
/* U's diagonal elements.				*/
/* x[0] and b[0] are the vectors' size not including	*/
/* zeroeth element.					*/
/********************************************************/
Matrix2 *solve_A_X_B(A, B)
Matrix2 *A, *B;
{
 	int 	i, j, k;
	int	limk;	/* limit value for k	*/
	int 	n,	/* A no. of row and col	*/
		p;	/* no. of columns of X, Y and B	*/
	double	**a;	/* ptr to ptr to A's elements	*/
	double	**b;	/* ptr to ptr to B's elements	*/
	Matrix2	*X;	/* solution matrix	*/
	double	**x;	/* array of X's elements	*/

	n = A->num_row;
	/* check for square matrix A	*/
	if (A->num_col != n)
	{
		fprintf(stderr, "solve_A_X_B: Matrix A isn't square.\n");
		exit(1);
	}

	p = B->num_col;
	/* check for matrices dimensions compatibility	*/
	if (B->num_row != n)
	{
		fprintf(stderr,
		"solve_A_X_B: No. of lines of matrices A and B don't match.\n");
		exit(1);
	}

	/* point a to array of A	*/
	a = A->array;
	/* point b to array of B	*/
	b = B->array;
	/* allocate memory for struct of X	*/
	X = (Matrix2 *) my_malloc(sizeof(Matrix2));
	X->num_row = n;
	X->num_col = p;
	x = (double **) my_malloc(n * sizeof(double *));
	for (i = 0; i < n; i++)
		x[i] = (double *) my_malloc(p * sizeof(double));
	X->array = x;

	/* solve L . Y = B, storing Y in X structure	*/
	for (i = 0; i < n; i++)		/* Initialize Y (=X) with B	*/
		for (j = 0; j < p; j++)
			x[i][j] = b[i][j];
	for (i = 0; i < n; i++)
	{
		limk = i - 1;
		for (k = 0; k < limk; k++)
			if (a[i][k] != 0)
			{
				for (j = 0; j < p; j++)
					x[i][j] -= a[i][k] * x[k][j];
#ifdef COST
n_mul[SOLVE_A_X_B] += p;
n_mul[caller] += p;
#endif
			}
	}

	/* solve U . X = Y (X stores initially Y elements	*/
	for (i = n - 1; i >= 0; i--)
	{
		for (k = i + 1; k < n; k++)
			if (a[i][k] != 0)
			{
				for (j = 0; j < p; j++)
					x[i][j] -= a[i][k] * x[k][j];
#ifdef COST
n_mul[SOLVE_A_X_B] += p;
n_mul[caller] += p;
#endif
			}
		for (j = 0; j < p; j++)
			x[i][j] /= a[i][i];
	}
#ifdef COST
n_mul[SOLVE_A_X_B] += n * p;
n_mul[caller] += n * p;
#endif
	return(X);
}



/****************************************************************/
/*  void mul_A_B_op(A, B, C): C = A . B				*/
/*  Solution matrix C must be furnished initialized with 0's	*/
/*  Does matrix mulptiplication in a Outer-Product way:		*/
/*								*/
/* |a b| |g h|   |a|	     |a|	 |b|	     |b|	*/
/* |c d|.|i j| = |c|.|g h| + |c|.|i j| + |d|.|g h| + |d|.|i j|	*/
/* |e f|	 |e|	     |e|	 |f|	     |f|	*/
/*								*/
/* This is made to save operations with null elements of B, 	*/
/* saving the most time when B is sparse (more than A).		*/
/****************************************************************/
void mul_A_B_op(A, B, C)
Matrix2 *A, *B, *C; /* prt to structures */
{
	int	i, j, k;
	int	m,		/* no. of lines of A and C	*/
		n,		/* no. of columns of A and rows of B	*/
		p;		/* no. of columns of B and C	*/
	double 	**a, **b, **c;	/* ptr to two dimensional arrays */

	m = A->num_row;
	n = A->num_col;
	p = B->num_col;
	/* check possibility of multiplication and store space */
	if (B->num_row != n || C->num_row != m || C->num_col != p)
	{
		fprintf(stderr,
		"mul_A_B_op: matrices don't have correct dimensions\n");
		exit(1);
	}

	a = A->array;
	b = B->array;
	c = C->array;

	for (i = 0; i < n; i++)		/* loop through B lines	*/
		for (j = 0; j < p; j++)	/* loop thr. els. of this line 	*/
			if (b[i][j] != 0)
			{
				for (k = 0; k < m; k++)
					c[k][j] += a[k][i] * b[i][j];
#ifdef COST
n_mul[MULTIPLY_A_B] += n;
n_mul[caller] += n;
#endif
			}
}

/************************************************/
/*	Routine to calculate the matrix		*/
/*						*/
/*	A = (I - P . LAMBDA/(LAMBDA + lambda))	*/
/*						*/
/* Input:	ptr to Matrix P;		*/
/*	  	Rates  LAMBDA, lambda.		*/
/*						*/
/* Output:	ptr to Matrix A.		*/
/*						*/
/************************************************/

Matrix *calculate_A (P, LAMBDA, lambda)
Matrix *P;	/* ptr to transition probabilities (uniformized) matrix	*/
double	LAMBDA;	/* uniformization constant	*/
double 	lambda;	/* rate of auxiliary events Y(r), from Ross's theory	*/
{
	double	ptrans;	/* aux value = prob of a transition happens 1st	*/
	Matrix *A;	/* ptr to matrix A = I-P.LAMBDA/(LAMBDA+lambda)	*/
	int	N;	/* number of states	*/
	int 	col;	/* auxiliary column counting variable	*/
	Column **Pcp, **Acp;	/* ptr's to columns linked lists of P, A*/
	Column **App;	/* ptr to the addr of prev element in A's column*/
	Column *Pp;	/* ptr to an element in a column list in P or A	*/

	ptrans = LAMBDA / (LAMBDA + lambda);	/* calculates ptrans	*/
	N = P->num_col;		/* gets the no. of states from P struct	*/

	A = (Matrix *) my_calloc(1, sizeof(Matrix));	/* allocates A	*/
	/* allocates the column pointer	*/
	A->col_pp = (Column **) my_calloc(N, sizeof(Column *));
	/* set the number of states in the matrix A structure	*/
	A->num_col = N;

	/* set ptr's to the 1st column lists of matrices P and A	*/
	Pcp = P->col_pp;
	Acp = A->col_pp;

	/* makes A = - P . LAMBDA / (LAMBDA + lambda)	*/
	for (col = 1; col <= N; Pcp++, Acp++, col++)
	{
		/* set App to the addr of the prev ptr of col list of A	*/
		App = Acp;
		/* set Pp to the 1st element of the current col of P	*/
		Pp = *Pcp;

		while (Pp)	/* copy this col (div by ptrans) to A	*/
		{
			/* allocates new element of A	*/
			*App = (Column *) my_calloc(1, sizeof(Column));

			(*App)->index = Pp->index;
			(*App)->value = -(Pp->value * ptrans);
			/* update ptr to next element	*/
			App = &((*App)->next);
			Pp = Pp->next;
		}
	}

	/* resets Acp to point to 1st col of A	*/
	Acp = A->col_pp;
	/* makes A = I + A	*/
	for (col = 1; col <= N; Acp++, col++)
	{
		/* set App to the addr of the prev ptr of col list of A	*/
		App = Acp;
		/* set Pp to the 1st element of the current col of A	*/
		Pp = *Acp;

		while (Pp && Pp->index < col)	/* look for the diag	*/
		{
			App = &(Pp->next);
			Pp = Pp->next;	/* go to next element	*/
		}
		if (Pp && Pp->index == col)	/* found diag element	*/
			Pp->value += 1;		/* A(i,i) = A(i,i) + 1	*/
		else	/* diagonal element non-existing, create it	*/
		{
			/* allocate linking to the prev element in col	*/
			*App = (Column *) my_calloc(1, sizeof(Column));
			(*App)->index = col;
			(*App)->value = 1;
			/* link to next element in column	*/
			(*App)->next = Pp;
		}
	}

	return A;
}

/************************************************************************/
/*	vpartition: Partitions an ordinary vector (v) according to a	*/
/* partitioning vector (partvec).					*/
/*	Inputs:								*/
/*	v 	= vector of double type elements (contiguous memory	*/
/*		positions), 1st position containing number of elements;	*/
/*	partvec = contains no. of partitions in first position ([0])	*/
/*		and size of partition j in (j + 1)th position ([j]).	*/
/*	Output:								*/
/*	p_v = vector with pointers to (p + 1) vectors (p = # of parts).	*/
/*	      The first vector ([0]) contains only the number p. The	*/
/*	      other vectors contain the data of each block: the number	*/
/*	      elements in the block, followed by each el. of the block.	*/
/************************************************************************/
double	**vpartition(v, partvec)
double	*v;		/* input vector	*/
int	*partvec;	/* partition vector	*/
{
	double	**p_v;	/* vector v partitioned	*/
	int	N;	/* number of elements of v	*/
	int	p;	/* number of partitions	*/
	int	i;	/* index for elements of input vector v	*/
	int	partno;	/* partition block number	*/
	int	ii;	/* index for elements within a block	*/
	int	count;	/* auxiliary counting variable	*/

	/* get  no. of partitions from partition vector	*/
	p = partvec[0];
	/* check for p positive	*/
	if (p <= 0)
	{
		fprintf(stderr, "vpartition: number of partitions <= 0.\n");
		exit(1);
	}

	/* get size of v from its 1st element	*/
	N = v[0];
	/* check if partvec fits to v's dimension	*/
	for (partno = 1, count = 0; partno <= p; partno++)
		count += partvec[partno];
	if (count != N)
	{
		fprintf(stderr, 
	"vpartition: partition vector uncompatible with input vector:\n");
		fprintf(stderr, "total size taken from partition vector: %d\n",
			count);
		fprintf(stderr, 
		"size of input vector (read from its 1st position): %d\n", N);
		exit(1);
	}

	/* allocate memory for new partitioned vector p_v	*/
	p_v = (double **) my_malloc((p + 1) * sizeof(double *));
	p_v[0] = (double *) my_calloc(1, sizeof(double));
	for (partno = 1; partno <= p; partno++)
		p_v[partno] = (double *) my_calloc(partvec[partno] + 1,
						sizeof(double));

	/* store number of blocks in p_v's first position	*/
	p_v[0][0] = p;
	/* set 1st position of 1st block vector with its block size	*/
	p_v[1][0] = partvec[1];
	/* initialize partno and ii	*/
	partno = 1;
	ii = 0;

	/* loop through all elements of v	*/
	for (i = 1; i <= N; i++)
	{
		/* increment ii and ask if it's a new block	*/
		if (++ii > partvec[partno])
		{
			partno++;	/* update no. of current block	*/
			/* set 1st position of this block with its size	*/
			p_v[partno][0] = partvec[partno];
			ii = 1;	/* reset block's elements counter	*/
		}
		/* copy el. from v to its position in a block in p_v	*/
		p_v[partno][ii] = v[i];
	}

	return p_v;
}


/************************************************************************/
/*	mpartition: Partitions a matrix (A) according to a partitioning	*/
/* vector (partvec). 							*/
/*	Inputs:								*/
/*	A 	= ptr to a square matrix in Matrix type;		*/
/*	partvec = contains no. of partitions in first position ([0])	*/
/*		and size of partition j in (j + 1)th position ([j]).	*/
/*	Output:								*/
/*	part_A: array with blocks in Matrix2 type. A blocks with all 	*/
/*		elements = 0 have a NULL pointer (it saves memory in	*/
/*		this structure and will save time in further matrix	*/
/*		computaions).						*/
/************************************************************************/
Matrix2 **mpartition(A, partvec)
Matrix	*A; 			/* input square matrix to be partitoned	*/
int	*partvec;		/* input partitioning vector	*/
{
	Matrix2	**part_A;	/* output: partitoned A	*/
	int	N;		/* dimension of A	*/
	int	p;		/* number of partitions	*/
	Column	**colp;		/* pointer to column in A	*/
	Column	*elp;		/* ptr to an element in a column of A	*/
	int	count;		/* auxiliary counting variable	*/
	int	i, j;		/* indexes of blocks	*/
	int	ii, jj;		/* indexes of elements within a block	*/
	int	row, col;	/* row and column in original matrix A	*/
	int	firstrow;	/* row of A where this block begins	*/
	int	lastrow;	/* row of A where this block ends	*/
	double	**pBlock;	/* array: only the elements of a block	*/

	/* get number of columns (and rows) of A	*/
	N = A->num_col;
	/* get number of partitions from partitioning vector	*/
	p = partvec[0];
	/* check if partvec fits to A's dimension	*/
	for (i = 1, count = 0; i <= p; i++)
		count += partvec[i];
	if (count != N)
	{
		fprintf(stderr, 
		"mpartition: partition vector uncompatible with matrix.\n");
		exit(1);
	}

	/* allocate memory for part_A	*/
	part_A = (Matrix2 **) my_malloc(p * sizeof(Matrix2 *));
	for (i = 0; i < p; i++)	/* note: init. block ptrs with NULL	*/
		part_A[i] = (Matrix2 *) my_calloc(p, sizeof(Matrix2));

	/* get initial column pointer	*/
	colp = A->col_pp;
	/* initializes column indexes j and jj	*/
	j = 0;
	jj = -1;	/* it'll be incremented upon entering the loop	*/
	/* loop through all columns of A	*/
	for(col = 1; col <= N; col++, colp++)
	{	/* is it a new block?	*/
		if (jj >= partvec[j + 1] - 1)
		{
			j++;	/* update 2nd index of current block	*/
			jj = 0;	/* reset block column counter	*/
		}
		else	jj++;	/* increment block column counter	*/

		i = 0;		/* reset 1st block index i	*/
		/* initialize beginning and ending row # of this block	*/ 
		firstrow = 1;
		lastrow = partvec[1];

		/* loop through all elements of current column of A	*/
		for (elp = *colp; elp; elp = elp->next)
		{
			row = elp->index;	/* set current row	*/
			while (row > lastrow)	/* is the wrong block?	*/
			{
				firstrow = lastrow + 1;
				/* update 1st index of current block	*/
				i++;
				lastrow += partvec[i + 1];
			}
			ii = row - firstrow;	/* set block row index	*/
			/* nothing in this block yet?	*/
			if (part_A[i][j].array == NULL)
			{	/* allocate memory for a new block	*/
				pBlock = (double **) my_malloc(partvec[i + 1] 
					* sizeof(double *)); 
				for (count = 0; count < partvec[i + 1]; count++)
					pBlock[count] = (double *) 
							my_calloc(partvec[j + 1], 
							sizeof(double));
				/* point actual block array 
					to newly allocated memory	*/
				part_A[i][j].array = pBlock;
			}
			else	/* point pBlock to actual block array	*/
				pBlock =  part_A[i][j].array;
			/* fill proper block position with element of A	*/
			pBlock[ii][jj] = elp->value;
		}
	}
	/* set num_row and num_col of ALL (including NULL) blocks	*/
	for (i = 0; i < p; i++)
		for (j = 0 ; j < p; j++)
		{
			part_A[i][j].num_row = partvec[i + 1];
			part_A[i][j].num_col = partvec[j + 1];
		}
	return part_A;
}

/************************************************************************/
/* transpose(A):							*/
/*	Returns a pointer to a new matrix that is equal to the input	*/
/*	square matrix A transposed.					*/
/************************************************************************/

Matrix *transpose (A)
Matrix *A;	/* input square matrix to be transposed	*/
{
	int	N;	/* dimension of the input matrix	*/
	Matrix	*At;	/* matrix = A transposed	*/
	Column	**colp;	/* vector with pointers to column lists	*/
	Column	*elp;	/* pointer to an element	*/
	int	col;	/* column counter	*/
	
	N = A->num_col;	/* set N	*/
	/* allocate new transposed matrix	*/
	At = (Matrix *) my_calloc(1, sizeof(Matrix));
	At->num_col = N;
	At->col_pp = (Column **) my_calloc(N, sizeof(Column *));
	
	/* point colp to 1st column list of A	*/
	colp = A->col_pp;
	/* loop through all columns of A	*/
	for (col = 1; col <= N; colp++, col++)
		/* loop through all elements of current column of A	*/
		for (elp = *colp; elp; elp = elp->next)
			/* put element of A in proper position of At	*/
			put_matrix_position(col, elp->index, elp->value, At);

	/* done, return pointer to At	*/
	return (At);
}

/************************************************************************/
/*	transpose2: transpose SQUARE matrix in Matrix2 form. Stores	*/
/* result in same input structure.					*/
/************************************************************************/
void transpose2(A)
Matrix2	*A;	/* input matrix	*/
{
	int	i, j;	/* auxiliary iteration variables	*/
	int	nrows;	/* number of rows of A	*/
	double	**a;	/* auxiliary array pointer related to A	*/
	double 	aux;	/* auxiliary temporary variable	*/

	/* set pointer a to arrau of A	*/
	a = A->array;
	/* get nrows from A structure	*/
	nrows = A->num_row;
	/* check for square matrix 	*/
	if (A->num_col != nrows)
	{
		fprintf(stderr, "transpose2: input matrix isn't square.\n");
		exit(1);
	}

	/* copy each element to proper position	*/
	for (i = 0; i < nrows; i++)
		for (j = 0; j < i; j++)
		{
			aux = a[i][j];
			a[i][j] = a[j][i];
			a[j][i] = aux;
		}
}

/************************************************************************/
/*	diff_A_B: Calculates A - B and stores the result in input	*/
/* matrix A (A <- A - B).						*/
/*	Inputs:								*/
/*	A = first input matrix in Matrix2 form;				*/
/*	B = second input matrix in Matrix2 form.			*/
/*	Output:								*/
/*	A = receive the result of A - B.				*/
/************************************************************************/
void diff_A_B(A, B)
Matrix2	*A, *B;	/* input matrices	*/
{
	int	i, j;	/* auxiliary iteration variables	*/
	double	**a,
		**b;	/* auxiliary arrays pointers related to A and B	*/
	int	m, n;	/* dimensions of the matrices	*/

	/* get m and n from A structure	*/
	m = A->num_row;
	n = A->num_col;

	/* match A and B dimensions	*/
	if (B->num_row != m || B->num_col != n)
	{
		fprintf(stderr,
		"diff_A_B: matrices doesn't have same dimensions\n");
		exit(1);
	}

	/* set a and b to point to arrays of A and B	*/
	a = A->array;
	b = B->array;

	/* subtract from each A element the corresponding element of B	*/ 
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			a[i][j] -= b[i][j];
}





/************************************************************************/
/*	reduction: perform reduction procedure on input partitioned 	*/
/* matrix A.								*/
/*	Inputs:								*/
/*	A = partitioned matrix as array of Matrix2 blocks;		*/
/*	K = number of blocks per column or row of A.			*/
/*	Output:								*/
/*	A receives in its structure, replacing block Aij(0):		*/
/*	- LU( Akk(k - 1) )			with i = j = k;		*/
/*	- Aij(i - 1)				with i < j;		*/
/*	- Cik = Aik(k - 1).(Akk(k - 1))^-1	with i > j = k		*/
/************************************************************************/

void reduction(A, K)
Matrix2	**A;	/* initially = A(0); After, contains desired results	*/
int	K;	/* number of blocks per column or row of A	*/
{
	int	i, j;	/* block indexes	*/
	int	k;	/* counter for reduction iteration	*/
	Matrix2	*Akk,
		*Aik,
		*Akj,
		*Aij;	/* auxiliary block pointers	*/
	Matrix2	*C;	/* auxiliary matrix	*/
	double	**c;	/* array of auxiliary matrix C	*/
	int	m, n;	/* dimensions of Aik=Aik(k - 1).(Akk(k - 1))^-1	*/
	int	r;	/* number of columns oc C = Aik . Akj	*/
	int	p;	/* number of columns of Aij and Akj	*/
	double	**akk,
		**aik,
		**aij;	/* auxilary arrays related to Akk, Aik and Aij	*/
	double	*Arow,
		*x;	/* auxiliary vectors	*/
	int	row, 
		col;	/* indexes of elements within a block	*/
	int	count;	/* aux var	*/
	double	aux;

	/* allocate memory for auxiliary matrix C	*/
	C = (Matrix2 *) my_calloc(1, sizeof(Matrix2));
#ifdef COST
caller = REDUCTION;
#endif


#ifdef DEBUG
Akk0 = (Matrix2 *) my_calloc(1, sizeof(Matrix2));
#endif
	
	/* main reduction iteration	*/
	for (k = 1; k < K; k++)
	{
		Akk = &(A[k - 1][k - 1]);	/* get Akk(k - 1)	*/
		if (Akk == NULL)		/* check for null block	*/
		{
			fprintf(stderr, 
				"reduction: invalid empty diagonal block.\n");
			exit(1);
		}

		n = Akk->num_row;	/* set n to its right value	*/
		if (Akk->num_col != n)	/* check if Akk is square	*/
		{
			fprintf(stderr,
				"reduction: non-square diagonal block.\n");
			exit(1);
		}

#ifdef DEBUG
printf("\nA%d%d:\n", k, k);
for (auxi = 0; auxi < n; auxi++)
{
	for (auxj = 0; auxj < n; auxj++)
		printf("%6.4e ", Akk->array[auxi][auxj]);
	printf("\n");
}		

Akk0->num_row = n;
Akk0->num_col = n;
akk0 = (double **) my_malloc(n * sizeof(double *));
for (count = 0; count < n; count++)
	akk0[count] = (double *) my_calloc(p, sizeof(double));
Akk0->array = akk0;
for (auxi = 0; auxi < n; auxi++)
	for (auxj = 0; auxj < n; auxj++)
		Akk0->array[auxi][auxj] = Akk->array[auxi][auxj];
#endif

		transpose2(Akk);	/* Akk <- Akk'	*/
#ifdef DEBUG
printf("\nA%d%d':\n", k, k);
for (auxi = 0; auxi < n; auxi++)
{
	for (auxj = 0; auxj < n; auxj++)
		printf("%6.4e ", Akk->array[auxi][auxj]);
	printf("\n");
}
#endif		

		LU2(Akk);		/* decompose Akk in L, U	*/

#ifdef DEBUG
printf("\nLU(A%d%d'):\n", k, k);
for (auxi = 0; auxi < n; auxi++)
{
	for (auxj = 0; auxj < n; auxj++)
		printf("%6.4e ", Akk->array[auxi][auxj]);
	printf("\n");
}		
#endif
		/* prepare aux vectors to be used in the following loop	*/
		/* allocate memory for Arow	*/
		Arow = (double *) my_malloc((n + 1) * sizeof(double));
		Arow[0] = n;		/* puts size of row in 1st pos.	*/
		/* allocate memory for x	*/
		x = (double *) my_calloc(n + 1, sizeof(double));
		x[0] = n;		/* put size in 1st position	*/

		/* loop through first block index	*/
		for (i = k + 1; i <= K; i++)
		{
			Aik = &(A[i - 1][k - 1]);	/* set Aik ptr	*/
			/* match dimensions of Aik and Akk	*/
			if (Aik->num_col != n)
			{
				fprintf(stderr,
				"reduction: Aik and Akk sizes don't match.\n");
				exit(1);
			}

			m = Aik->num_row;		/* get m	*/
			aik = Aik->array;		/* set aik ptr	*/
			/* if Aik is a null-matrix, bypass operations.	*/
			/* Aik = Cik = Aik(k - 1) . (Akk(k - 1))^-1 = 0	*/
			/* => Aik, (A[i - 1][k - 1].array) remain NULL.	*/
			/* Aij(k) = Aij(k - 1) => no change.	*/
			if (aik != NULL)
			{
			
#ifdef DEBUG
printf("\nA%d%d:\n", i, k);
for (auxi = 0; auxi < m; auxi++)
{
	for (auxj = 0; auxj < n; auxj++)
		printf("%6.4e ", aik[auxi][auxj]);
	printf("\n");
}		
#endif
			
				/* Aik <- Aik(k - 1) . (Akk(k - 1))^-1	*/
				for (row = 0; row < m; row++)
				{	/* solve row by row	*/
					/* copy row of Aik into Arow	*/
					for (col = 0; col < n; col++)
						Arow[col+1] = aik[row][col];
					/* x = row of Aik(k-1)/Akk(k-1)	*/
					solve_A_x_b(Akk, x, Arow);

#ifdef COST
n_mul[REDUCTION] += n * n;
n_mul[SOLVE_A_X_B] += n * n;
#endif


					/* copy x back to Aik row	*/
					for (col = 0; col < n; col++)
						aik[row][col] = x[col + 1];
				}
				/* set number of rows of aux matrix C	*/
				C->num_row = m;	/* (same as Aik)	*/
				/* loop through 2nd block index to get:	*/
				/* Aij(k) <- Aij(k-1) - Aik . Akj(k-1)	*/
				/* in which Aik has just been obtained	*/
				for (j = k + 1; j <= K; j++)
				{
					/* point Akj to its struct	*/
					Akj = &(A[k - 1][j - 1]);
					/* set Aij pointer	*/
					Aij = &(A[i - 1][j - 1]);
					/* get # of cols of Aij	*/
					p = Aij->num_col;
					/* match dimensions of Aij, Akj,
							Aik and Akk	*/
					if (Akj->num_row != n || 
					Akj->num_col != p || Aij->num_row != m)
					{
						fprintf(stderr,
			    "reduction: Akj or Aij don't match other sizes.\n");
						exit(1);
					}
#ifdef DEBUG
if (Akj->array != NULL)
{
	printf("\nA%d%d:\n", k, j);
	for (auxi = 0; auxi < n; auxi++)
	{
		for (auxj = 0; auxj < p; auxj++)
			printf("%6.4e ", Akj->array[auxi][auxj]);
		printf("\n");
	}
}		
else	printf("\nA%d%d is null.\n", k, j);

if (Aij->array != NULL)
{
	printf("\nA%d%d(k - 1):\n", i, j);
	for (auxi = 0; auxi < m; auxi++)
	{
		for (auxj = 0; auxj < p; auxj++)
			printf("%6.4e ", Aij->array[auxi][auxj]);
		printf("\n");
	}		
}		
else	printf("\nA%d%d is null.\n", i, j);
#endif

					/* if Akj is null, bypass ops.	*/
					/* (Akj=0) => C = Aik . Akj = 0	*/
					/*  => Aij(k) = Aij(k - 1) 	*/
					if (Akj->array != NULL)
					{
					  /* set # of cols of C	*/
					  r = Akj->num_col;
					  C->num_col = r;
					  /* allocate array of C	*/
					  c = (double **) my_malloc(m *
					  		sizeof(double *));
					  for (row = 0; row < m; row++)
					  	c[row] = (double *)
					  	my_calloc(r, sizeof(double));
					  C->array = c;
					  
					  /* C <- Aik . Akj	*/
					  mul_A_B_op(Aik, Akj, C);

					  /* if Aij is null, allocate and
					  initialize it with 0's now, cause
					  it will no longer be null.	*/
					  if (Aij->array == NULL)
					  {
					    aij = (double **)
					    my_malloc(m * sizeof(double *));
					    for (count = 0; count < m; count++)
					      aij[count] = (double *)
					      my_calloc(p, sizeof(double));
					    Aij->array = aij;
					  }
					  /* Aij <- Aij - C	*/
					  diff_A_B(Aij, C);
					  /* free internal ptrs of C	*/
					  for (row = 0; row < m; row++)
					  	free(c[row]);
					  free(c);
					}

#ifdef DEBUG
if (Aij->array != NULL)
{
	printf("\nA%d%d(k):\n", i, j);
	for (auxi = 0; auxi < m; auxi++)
	{
		for (auxj = 0; auxj < p; auxj++)
			printf("%6.4e ", Aij->array[auxi][auxj]);
		printf("\n");
	}		
}		
else	printf("\nA%d%d is null.\n", i, j);

#endif


				}
			}		/* end of if (aik != NULL)	*/

#ifdef DEBUG
printf("\nA%d%d*:\n", i, k);
if (aik != NULL)
	for (auxi = 0; auxi < m; auxi++)
	{
		for (auxj = 0; auxj < n; auxj++)
			printf("%6.4e ", aik[auxi][auxj]);
		printf("\n");
	}
else	printf("Null matrix.\n");
#endif

		}			/* for (i = k + 1; i <= K; i++)	*/
		/* release memory for aux. vectors Arow and x	*/
		free(Arow);
		free(x);

		/* let Akk = LU(Akk) (by now it stores LU(Akk')	*/
		/* It can be shown that 
			if B1 = LU(B') and B2 = LU(B), then:	
		b2ii = b1ii		(same diagonal elements)
		b2ij = b1ji / b1jj;	for all i > j
		b2ij = b1ji * b1ii;	for all i < j		*/
		akk = Akk->array;		/* set akk ptr	*/
		for(row = 0; row < n; row++)
		    for (col = 0; col < row; col++)
		    {
			aux = akk[row][col];
			/* case of i > j :	*/
			akk[row][col] = akk[col][row] / akk[col][col];
			/* case of i < j :	*/
			akk[col][row] = aux * akk[col][col];
		    }

#ifdef COST
n_mul[REDUCTION] += n * (n - 1);
#endif


#ifdef DEBUG
LU2(Akk0);
printf("\nLU(A%d%d(0)):\n", k, k);
for (auxi = 0; auxi < n; auxi++)
{
	for (auxj = 0; auxj < n; auxj++)
		printf("%7.4e ", Akk0->array[auxi][auxj]);
	printf("\n");
}

printf("\nLU(A%d%d) (obtido a partir de LU(A%d%d'):\n", k, k, k, k);
for (auxi = 0; auxi < n; auxi++)
{
	for (auxj = 0; auxj < n; auxj++)
		printf("%7.4e ", akk[auxi][auxj]);
	printf("\n");
}
#endif

	}				/* for (k = 1; k < K; k++)	*/

	Akk = &(A[K - 1][K - 1]);	/* get Akk(k - 1)	*/
	
#ifdef DEBUG
for (row = 0; row < Akk->num_row; row++)
	for (col = 0; col < Akk->num_col; col++)
		printf("AKK[%d][%d] = %.10e\n", row + 1, col + 1,
			Akk->array[row][col]);
#endif

	if (Akk == NULL)		/* check for null block	*/
	{
		fprintf(stderr, 
			"reduction: invalid empty diagonal block.\n");
		exit(1);
	}
	/* calculate LU(AKK(K-1)) (last diagonal block)	*/
	LU2(Akk);		/* decompose Akk in L, U	*/

#ifdef DEBUG
for (row = 0; row < Akk->num_row; row++)
	for (col = 0; col < Akk->num_col; col++)
		printf("AKK[%d][%d] = %.10e\n", row + 1, col + 1,
			Akk->array[row][col]);
#endif
	if (Akk->num_col != Akk->num_row)	/* check for Akk square	*/
	{
		fprintf(stderr, "reduction: non-square diagonal block.\n");
		exit(1);
	}	
	free(C);		/* free mem for aux matrix C
			      (internal ptrs have already been freed)	*/
#ifdef COST
caller = OTHER;
#endif

}

/************************************************************************/
/*	mul_A_v: multiplies a matrix A by a vector v. It regards x=A.v	*/
/* as a linear combination of A's columns done by v's elements:		*/
/*									*/
/*	            |a b c|   |j|         |a|         |b|         |c|	*/
/*	x = A . v = |d e f| . |k|  =  j . |d|  +  k . |e|  +  l . |f| 	*/
/*	            |g h i|   |l|         |g|         |h|         |i|	*/
/*									*/
/*	This way, it can bypass many unnecessary operations with null	*/
/* elements of v. If v is all null, all operations will be bypassed.	*/
/*	Furthermore, it checks if A is all null, in this case the whole	*/
/* operation will be bypassed too.					*/
/*	Inputs:								*/
/*	A = pointer to a Matrix2 struct. If A is a null matrix, 	*/
/* 	    A->array is a NULL ptr;					*/
/*	v = vector of double type elements. v[0] contains number of	*/
/*	    elements of v.						*/
/*	Outputs:							*/
/*	x = solution vector. x[0] contains number of elements of x.	*/
/************************************************************************/
double	*mul_A_v(A, v)
Matrix2	*A;		/* input matrix A	*/
double	*v;		/* input vector v	*/
{
	int	i;	/* index to A's rows	*/
	int	j;	/* index to A's columns and v's elements	*/
	int	m;	/* number of rows of A	*/
	int	n;	/* number of columns of A and elements of v	*/
	double	*x;	/* output vector	*/
	double	**a;	/* auxiliary pointer to array of A	*/
	double	v_el;	/* element of v	*/

	n = A->num_col;		/* get no. of cols of A from A's struct	*/
	/* check dimensions of A and v	*/
	if (v[0] != n)
	{
		fprintf(stderr,
		"mul_A_v: matrix and vector dimensions don't match.\n");
		exit(1);
	}

	m = A->num_row;		/* get no. of rows of A from A's struct	*/
	a = A->array;			/* point a to array of A	*/

	/* allocate memory for x, initializing with 0's	*/
	x = (double *) my_calloc(m + 1, sizeof(double));
	/* store no. of elements in 1st position of x	*/
	x[0] = m;
	/* check for null matrix	*/
	if (a == NULL)
		return(x);	/* rerturn x = <0 ... 0>	*/
	/* loop through elements of v and columns of A	*/
	for (j = 1; j <= n; j++)
		/* check if this element of v is non-zero	*/
		if ((v_el = v[j]) != 0)
		{
			/* loop through elements of jth column of A	*/
			for (i = 1; i <= m; i++)
				/* update ith element of sol. vector x	*/
				x[i] += v_el * a[i - 1][j - 1];

#ifdef COST
n_mul[MUL_A_V] += m;
n_mul[caller] += m;
#endif
		}
	return(x);
}




/************************************************************************/
/*	calc_w: Calculates w based on the equation:			*/
/*	wi(k) = wi(k - 1) - Aik(k - 1) . (Akk(k - 1))^-1 . wk(k - 1)	*/
/*	Inputs:								*/
/*	A = partitioned matrix in form of array of Matrix2 blocks. It	*/
/*	    contain the results of the reduction procedure as defined	*/
/*	    in "void reduction(A, K)";					*/
/*	w = partitioned vector with the same partition of A. It must	*/
/*	    contain, initially, the proper w(0).			*/
/*	Output:								*/
/*	w = same var used as input will store w(K) (K = no. of blocks).	*/
/*	Assumption:							*/
/*	A and w sizes and partitions fit perfectly.			*/
/************************************************************************/
void calc_w(A, w)
Matrix2	**A;	/* input matrix as the output of "void reduction(A, K)"	*/
double	**w;	/* partitioned vector w	*/
{
	int	i, j, k;	/* iteration variables	*/
	int	K;		/* number of partition blocks	*/
	int	sizei;		/* size of ith vector partition	*/
	double	*v;		/* auxiliary temporary vector	*/

	/* get no. of blocks from vector w	*/
	K = w[0][0];
	/* main loop	*/
	for (k = 1; k < K; k++)
		/* loop through blocks of w to obtain wi(k)	*/
		for (i = k + 1; i <= K; i++)
			if (A[i - 1][k - 1].array != NULL)
			{	/* if Aik(k-1) is non-null...	*/

#ifdef COST
caller = CALC_W;
#endif

				/* v <- Aik(k-1).(Akk(k-1))^-1.wk(k-1)	*/
				v = mul_A_v(&(A[i - 1][k - 1]), w[k]);

#ifdef COST
caller = OTHER;
#endif


				/* wi(k) <- wi(k-1) - v	*/
				sizei = w[i][0];	/* size block i	*/
				for (j = 1; j <= sizei; j++)
					w[i][j] -= v[j];

				free(v);	/* rel mem for vector v	*/
			}
}

/************************************************************************/
/*	calc_z: Calculates z based on the equation (1):			*/
/*									*/
/*	          (i-1) -1     (i-1)        K-i-1  (i-1)		*/
/*	z (r) = (A     )   . [w     (r-1) -  SUM {A      . z   (r)}]	*/
/*	 i        i,i          i             j=0   i,K-j    K-j		*/
/*									*/
/*	Inputs:								*/
/*	A = partitioned matrix in form of array of Matrix2 blocks. It	*/
/*	    contain the results of the reduction procedure as defined	*/
/*	    in "void reduction(A, K)";					*/
/*	w = partitioned vector with the same partition of A. It must	*/
/*	    contain the proper w(r-1).					*/
/*	Output:								*/
/*	z = partitioned vector with the same structure as w. It will	*/
/*	    contain z(r) calculated from A and w(r-1).			*/
/*	Assumption:							*/
/*	A and w sizes and partitions fit perfectly.			*/
/************************************************************************/
void calc_z(A, w, z)
Matrix2	**A;	/* input matrix as the output of "void reduction(A, K)"	*/
double	**w;	/* partitioned vector w	*/
double	**z;	/* solution partitioned vector z(r)	*/
{
	int	i, j;		/* iteration variables	*/
	int	K;		/* number of partition blocks	*/
	int	sizei;		/* size of ith vector partition	*/
	double	*u, *v;		/* auxiliary temporary vectors	*/
	int	topj;		/* maximum value of j	*/
	int	count;		/* auxiliary counting variable	*/

	/* get number of partitions from w structure	*/
	K = w[0][0];
	/* check if z and w have the same dimension	*/
	if (z[0][0] != K)
	{
		fprintf(stderr,
			"calc_z: w and z don't have the same dimension.\n");
		exit(1);
	}
	
	/* loop bakwards through blocks of z, w and A aplying eq. (1)	*/
	for (i = K; i >= 1; i--)
	{
		sizei = w[i][0];	/* get size of this block	*/
		/* create auxiliary temporary vector v	*/
		v = (double *) my_calloc(sizei + 1, sizeof(double));
		v[0] = sizei;

		/*       K-i-1  (i-1)			*/
		/* v <-   SUM {A      . z   (r)}	*/
		/*        j=0   i,K-j    K-j		*/
		topj = K - i - 1;	/* set maximum value for j	*/
		for (j = 0; j <= topj; j++)
			if (A[i - 1][K - j - 1].array != NULL)
			{	/* if Ai,K-j(i-1) is non-null	*/
#ifdef COST
caller = CALC_Z;
#endif
				/* u <- Ai,K-j(i-1) . zK-j(r)	*/
				u = mul_A_v(&(A[i - 1][K - j - 1]), z[K - j]);
#ifdef COST
caller = OTHER;
#endif


#ifdef DEBUG
printf("\nu:\n");
for (count = 1; count <= sizei; count++)
	printf("%10.4e ", u[count]);
printf("\n");
#endif

				/* v <- v + u	*/
				for (count = 1; count <= sizei; count++)
					v[count] += u[count];

				free(u);	/* rel. mem. pontd by u	*/
			}
#ifdef DEBUG
printf("\nv:\n");
for (count = 1; count <= sizei; count++)
	printf("%10.4e ", v[count]);
printf("\n\nwi:\n");
for (count = 1; count <= sizei; count++)
	printf("%10.4e ", w[i][count]);
printf("\n");
#endif

		/* v <- wi(i-1)(r-1) - v (see equation (1))	*/
		for (count = 1; count <= sizei; count++)
			v[count] = w[i][count] - v[count];

#ifdef DEBUG
printf("\nv = wi - v:\n");
for (count = 1; count <= sizei; count++)
	printf("%10.4e ", v[count]);
printf("\n");
#endif

		z[i][0] = sizei;	/* set 1st position of z[i]	*/
		/* find zi = (Aii(i-1))^-1 . v (see equation (1))	*/
		/* Remark: as output from reduction(), Aii already	*/
		/* contains LU(Aii) in the proper form.			*/
		solve_A_x_b(&(A[i - 1][i - 1]), z[i], v);

#ifdef COST
n_mul[CALC_Z] += sizei * sizei;
n_mul[SOLVE_A_X_B] += sizei * sizei;
#endif


#ifdef DEBUG
printf("\nzi:\n");
for (count = 1; count <= sizei; count++)
	printf("%10.4e ", z[i][count]);
printf("\n");
#endif

		/* release memory allocated for v	*/
		free(v);
	}
}

/************************************************************************/
/*	update_w: make w <- z . lambda / (lambda + LAMBDA)		*/
/*	Inputs:								*/
/*	z = partitioned vector z(r);					*/
/*	c = double value = lambda / (lambda + LAMBDA).			*/
/*	Output:								*/
/*	w = partitioned vector w(0)(r).					*/
/************************************************************************/
void update_w(w, z, c)
double	**w;	/* partitioned vector w(0)(r)	*/
double	**z;	/* partitioned vector z(r)	*/
double	c;	/* lambda / (lambda + LAMBDA)	*/
{
	int	K;	/* number of partitions	*/
	int	i, j;	/* iteration variables	*/
	int	sizei;	/* size of ith partition	*/

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
	for (i = 1; i <= K; i++)
	{
		sizei = z[i][0];	/* get size of ith partition	*/
		/* loop through all elements in ith partition	*/
		for (j = 1; j <= sizei; j++)		/* w <- z . c	*/
			w[i][j] = z[i][j] * c;
	}
}

/************************************************************************/
/*	vunpartition: do reverse action of vpartition			*/
/*	Input:								*/
/*	v = partitioned vector						*/
/*	Output:								*/
/*	u = unpartitioned vector					*/
/************************************************************************/
void vunpartition(v, u)
double **v;	/* input partitioned vector	*/
double	*u;	/* output unpartitioned vector	*/
{
	int	N;	/* dimension of the vectors	*/
	int	i,	/* index to partition blocks of the vector	*/
		j,	/* index to element within a block	*/
		i_u;	/* index to element in unpartitioned vector	*/
	int	K;	/* number of partition blocks	*/
	int	sizei;	/* size of ith partition block	*/
	
	/* get no. of blocks from v structure (1st position)	*/
	K = v[0][0];
	/* get total no. of elements in vector	*/
	N = 0;						/* initialize N	*/
	for (i = 1; i <= K; i++)
		N += v[i][0];		/* add no. of el. of this block	*/
	/* check if v and u have same dimension	*/
	if ((u[0] =! N))
	{
		fprintf(stderr,
			"vunpartition: v and u don't have same dimension.\n");
		exit(1);
	}
			
	i_u = 1;		/* initialize index of u	*/
	/* copy v elements to u	*/
	for (i = 1; i <= K; i++)
	{
		sizei = v[i][0];		/* get size of block i	*/
		for (j = 1; j <= sizei; j++)
			u[i_u++] = v[i][j];	/* copy el., inc. index	*/
	}
}	

