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
#include <stdlib.h>
#include "sparseMatrix.h"


/*****************************************************************/
/* M points to a matrix in column-wise linked list 		 */
/* returns max|i-j|, for M(i,j) <> 0 				 */
/*****************************************************************/
int get_band(M)
Matrix *M;	  /* ptr to sparse transition matrix */
{
 Column **cp;	  /* ptr to the address of a element in a column list  */
 int i;           /* row noo.                                          */
 int j;           /* col no.                                           */
 int max;         /* band                                              */
 int d;           /* dimension of square sparse matrix M               */ 
 Column *element; /* ptr to column list                                */


 max = 0;

 d = M->num_col;

 for (j = 1; j <= d; j++)
 { 
   /* scans for all d columns  */
   /* set cp to the pointer of the j-th column */
   cp = (M->col_pp) + (j-1);

   if (*cp != NULL) 
   { 
      /* if column is filled  */

     element = *cp;                /* here there is at least one element in the col */
   
     while (element) 
     { 
       /* scans for all rows in this column  */
       i = element->index;
       
       if (max < ABS(i-j)) 
           max = ABS(i-j);
           
       element = element->next;    /* gets next in column */
     }
   }
 }
 
 /* if max = 0 , M is diagonal   */
 return(max); 
}



/************************************************************/
/*							    */
/*  int R_in_band(int band, int i, int j, int *prtition_vec */
/* routine which determine if R_{i,j} lies within diagonal  */
/* band of matrices that will be filled in.		    */
/* returns 1 if R is within band and 0 otherwise	    */
/************************************************************/
int R_in_band(band,i,j,V)
int band;
int i, j; /* R's block index in P */
int *V;   /* vector of partitions */
 {
  int k;
  int ur; /* (sum_{l=1}^{i-1} V[l]) + 1 ; upper row index of R_{i,j}   */
  int lr; /* ur-1+V[i] ; lower row index of R_{i,j}                    */
  int lc; /* (sum_{l=1}^{j-1} V[l]) + 1 ; left column index of R_{i,j} */
  int rc; /* lc-1+ v[j]; right column index of R_{i,j}                 */


     /* evaluates extremities of R_{i,j}  */
     ur = 0;
     for (k = 1; k < i; k++) 
       ur += V[k];
       
     ur++;
     lr = (ur-1) + V[i];
     lc = 0;
     
     for (k = 1; k < j; k++) 
       lc += V[k];
       
     lc++;
     rc = (lc-1) + V[j];

/***
printf("R_in_band:R e band? i=%d j=%d ur=%d lr=%d lc=%d rc=%d\n",i,j,ur,lr,lc,rc);
***/

     /* if R_{i,j} belongs to band, return 1 */
     /* check for off diag                   */
     /* if a diagonal block, do it !         */
     if ((band >= ABS(ur - rc)) || (band >= ABS(lr - lc))|| (ur == lc)) 
	return 1;
     else 
        return 0;
 }



/************************************************************/
/*							    */
/* Matrix2 init_P(Matrix *, int *): 			    */
/* routine to initialize matrix P and R_{i,j}               */
/* P is a dynamic nXn dimension matrix. n is the no of      */
/* partitions of the transition matrix A, according to the  */
/* given partition vector. Each element of P, R_{i,j} is    */
/* a structure which points to a dynamic jXl dimension      */
/* matrix, where j is the j-th partition size (same for l). */
/* a P element point to NULL if R_{i,j} will not undergo    */
/* filling in (R_{i,j} does not belongs to filled band of   */
/* the transition matrix during GTH algorithm)		    */
/* Uses extern routine get_band.			    */
/* Returns pointer to P matrix.				    */
/************************************************************/
Matrix2 **init_P(M,V)  
Matrix *M;   /* transition sparse matrix, column wise       */
int *V;      /* partition vector. V[0]=n, no. of partitions */
	     /* pointer to contiguous memory                */
{

 Matrix2 **P;    /* pointer to ptr to Matrix2 structures (R_{i,j})            */
 int n;          /* no. of partitions                                         */
 int i;          /* P target row or M's partition                             */
 int j;          /* P target column or M's partition                          */
 int row;        /* R or M row                                                */
 int col;        /* R or M column                                             */
 int ur;         /* (sum_{l=1}^{i-1} V[l]) + 1 ; upper row index of R_{i,j}   */
 int lr;         /* ur-1+V[i] ; lower row index of R_{i,j}                    */
 int lc;         /* (sum_{l=1}^{j-1} V[l]) + 1 ; left column index of R_{i,j} */
 int rc;         /* lc-1+ v[j]; right column index of R_{i,j}                 */
 int k;
 int band;
 double **pR;    /* ptr to R{i,j} matrix                            */
 Column *cp;     /* pointer to transition matrix element            */
 int sumi, sumj; /* highest row/column no. of R_{i,j} as indexed in */
		 /* trans. matrix                                   */
 int Rrow, Rcol; /* R_{i,j}`s peer indexes to trans matrix entries  */



 /* P's dimension; P is nXn   */
 n = V[0];


 /* the following structure may be found in                          */
 /* "C as a second language", by T. Muldner & P. Steele,             */
 /* Addison Wesley Publishing, 1989 reprint, pp 236, "Construct (d)" */
 /* chapter 8.2, multi-dimensional arrays                            */

 /* allocates P matrix  */
 P = (Matrix2 **)my_malloc(n * sizeof(Matrix2 *));  /*row ptrs */ 

 /* allocates columns in target row  */
 /* note that calloc initialized fields to zero bytes  */
 /* so, R_{i,j} points to NULL   */
 for (i = 0; i < n ; i++)
    P[i] = (Matrix2 *)my_calloc(n, sizeof(Matrix2)); 
 

 /* initialization of R_{i,j}  is done if points B=(ur,rc) and/or  */
 /* D=(lr,lc), extremum points of the rectangule R{i,j}, ly within */
 /* band of filling of GH algorithm  */

 /* gets band  */
 band = get_band(M);

 /* if trans. matrix is diagonal there will be prob. */
 if (band == 0)
 {
    fprintf(stderr,"init_P: trans. matrix is diagonal. Algorithm will not work: steady state vector is zero\n");
    return(NULL);
 }


 for (i = 1; i <= n; i++)
  for (j = 1;j <= n; j++) 
  {
     /* evaluates extremities of R_{i,j}  */
     ur = 0;
     for (k = 1; k < i; k++) 
        ur += V[k];
     ur++;
     
     lr = (ur-1) + V[i];
     lc = 0;
     
     for (k = 1; k < j; k++) 
        lc += V[k];
        
     lc++;
     rc = (lc-1) + V[j];


     /*if R_{i,j} belongs to band, allocate structure  */
     if ((band >= ABS(ur - rc)) || (band >= ABS(lr - lc))|| (ur == lc)) 
                                            /* check for off diag */
	                                    /* if a diagonal block, init it !  */
       {
	 /* allocates a V[i] x V[j] structure like that of P */
	 /* except that now a matrix element is of double    */
	 pR = (double **)my_malloc(V[i] * sizeof(double *));
	 
	  
	 /* allocate rows of zeroes  */
	 for (row = 0; row < V[i]; row++)
	   pR[row] = (double *)my_calloc(V[j], sizeof(double));

         
	 /* assigns structure to R{i,j} */
	 (P[(i-1)][(j-1)]).array = pR; /* address of array */
	 (P[(i-1)][(j-1)]).num_row = V[i];
	 (P[(i-1)][(j-1)]).num_col = V[j];
       }
   }

 /* now copies non-zero entries of transition matrix to R's */
 j = 0;    /* j of R_{i,j} */
 sumj = 0; /* accumulator of blocks sizes for columns  */

 for (col = 1; col <= (M->num_col); col++) 
 { /* loops for all columns */

   /* cp points to 1st element of col-th column */
   cp = *(M->col_pp + (col-1));

   /* skips if column empty */
   if (cp == NULL) 
      continue;              /* should it be cp==(Matrix **)NULL ?  */
			     /* cast is not necessary for NULL !    */
   

   /* add until col lies within block */
   while (col > sumj)
   { /* add until col lies in the block */
     j++;
     sumj += V[j];  /* it should do at most once per column */
		    /* since only at extremities need to add  */
		    /* sumj= highest column index of partition */
		    /*j wrt to trans matrix columns indexes*/
   }


   i = 0;     /* i of R_{i,j} */
   sumi = 0;  /* accumulator of block sizes for rows  */
   
   while (cp)
   {
     row = cp->index;

     /*find which R_{i,j} this element belongs to */
     while (row > sumi)
     {
       i++;
       sumi += V[i];  /* assumes rows are ordered */
		      /* sumi= highest row index of partition */
		      /* i wrt to trans matrix columns indexes*/
     }


     /* stores value in R_{i,j}  */

     /*if R_{i,j} not initialized... error! */
     pR = (P[(i-1)][(j-1)]).array; 
     
     /* should it be pr==((double **)NULL)? */
     /* casting is not needed for NULL ! */
     if (pR == NULL) 
     {
       fprintf(stderr,"init_P: attempt to access R_{i,j} not initialized\n");
       return(NULL);
     }


     /* translating trans. matrix rows/cols to R`s indexes */
     Rrow = (row - (sumi - V[i])) - 1;    /* -1 because storage indexing */
     Rcol = (col - (sumj - V[j])) - 1;    /* begins in 0                 */

     pR[Rrow][Rcol] = cp->value;          /* copy trans. prob. */

     /* get next row element in this column */
     cp = cp->next;
   }
 }   /* for col .... */
 
 return((Matrix2 **)P);
}



/*********************************************************/
/* double *solve_A_x_b(Matrix2 *, double *, double *)    */
/* overwrites vector b with the solution of Ly=b         */
/* return a pointer to answer vector x, obtained from    */
/* Ux=y.						 */
/* matrix A should be in the form LU, where L is a lower */
/* triangular matrix with 1's in his diagonal; that is   */
/* implicit, since A's diagonal elements are U's diagonal*/
/* elements						 */
/* x[0] and b[0] are the vectors' size not including     */
/* zeroeth element					 */
/*********************************************************/
double *solve_A_x_b(A,x,b)
Matrix2 *A;
double  *x, *b;
{
 int    i, j;
 int    m, n;    /* A no. of row and col       */
 double **array; /* ptr to ptr to A's elements */
 double acc;     /* accumulator                */
 
 
 m = A->num_row;
 n = A->num_col;

 /* check if vectors have the correct size */
 if ((m != ((int)b[0])) || (n != ((int)x[0]))) 
 {
   fprintf(stderr,"GTH block elimination method (solve_A_x_b): vector doesn't have correct dimension\n");
   exit(56);
 }

 array = A->array;

 /* first solve Ly=b; ie, lower triang. system                    */
 /* y_1 = b_1; y_j = b_j - sum_{i=1}^{j-1} L_{i,j} * y_i          */
 /* that is, b_1 = b_1; b_j = b_j - sum_{i=1}^{j-1} A_{i,j} * b_i */
 for(j = 2; j <= m; j++)
 {
   acc = 0;
   for (i = 1; i < j; i++) 
     acc += array[j-1][i-1] * b[i];
   b[j] -= acc;
 }


/* printf("intermediate solution y:\n");
   put_vector(b+1,m);                    */


 /* Now solve Ux = y                                      */
 /* x_j = (y_j - sum_{i=j+1}^{n} U_{j,i} * y_i) / U_{j,j} */
 /* note that U_{j,i} = A_{j,i}  and y_j = b_j            */
 for (j = n; j >= 1; j--)
 {
   acc = 0;
   for (i = j+1; i <= n; i++) 
     acc += array[j-1][i-1] * x[i];
   x[j] = (b[j] - acc) / array[j-1][j-1];	
 }
 
 return(x);
}



/*********************************************************/
/* inverse_GH(M1,M2): (I - R_{(m+1),(m+1)}^{m+1})^-1     */
/* scratch matrix M2 must be given   			 */
/*********************************************************/
void inverse_GH(M1,M2)
Matrix2 *M1;          /* matrix to be found inverse                */
Matrix2 *M2;          /* scratch matrix which stores (I-M1)        */
	              /* and then its LU decomp.; ptr to structure */
	              /* M1 = (I - M1)^-1                          */
{
 /* external functions  */
 void   LU();
 double *solve_A_x_b();

 int    i, j;
 double **cp1, **cp2; /* ptr to arrays  */
 double *vec_x, *vec_b; /* ptr to contiguos memory */


 /* check if matrices are alike  */
 if (M1->num_row != M2->num_row  || M1->num_col != M2->num_col)
 {
   fprintf(stderr,"GTH block elimination method (inverse_GH): matrices doesn't have same dimension\n");
   exit(50);
 }

 /* check if matrices are square */
 if (M1->num_row != M1->num_col) 
 {
   fprintf(stderr,"GTH block elimination method (inverse_GH): matrices are not square\n");
   exit(50);
 }


 cp1 = M1->array;
 cp2 = M2->array;

 /* set scratch to bear (I-M1)  */
 for (i = 0; i < M1->num_row; i++)
  for (j = 0; j < M1->num_col; j++)
  {
    if (i == j) 
      cp2[i][j] = 1-cp1[i][j]; /* is a diag. elem. */
    else 
      cp2[i][j] = (-1.0) * cp1[i][j];
  }

 /* find LU decomposition of (I-M1) and stores it in scratch */
 LU(M2);

 /* Allocates vectors x and b */
 vec_x = (double *)my_calloc(1 + M1->num_row, sizeof(double));
 vec_b = (double *)my_calloc(1 + M1->num_row, sizeof(double));

 /* find each column of (I-M1)^1 by solving Ax = b  */
 /* where A = (I-M1) and b = e_i, i-th basis element for R^n  */
 /* stores x = (I-M1)_i^-1 in M1 */
 for (i = 1; i <= M1->num_col; i++)
 {
   vec_x[0] = M1->num_row;
   vec_b[0] = vec_x[0];

   /* resets b to zero and sets b to e_i  */
   for (j = 1; j <= M1->num_row; j++)
     vec_b[j] = 0.0;
   vec_b[i] = 1.0;

   /* find (I-M1)_{i}^{-1}; i e, the i-th column of the inverse  */
   solve_A_x_b(M2,vec_x,vec_b);

   /* save into M1; so diag. P[m][m] is the inverse */
   for (j = 1; j <= M1->num_row; j++)
     cp1[j-1][i-1] = vec_x[j]; /* remember: this' the i-th COL! */
 }

 /* releases vec_x and vec_b */
 free((char *)vec_x);
 free((char *)vec_b);
}



/********************************************************/
/*  multiply_A_B(M1,M2,M3): M1 x M2 = M3 		*/
/*  solution matrix M3 must be furnished	 	*/
/********************************************************/
int multiply_A_B(M1,M2,M3)
Matrix2 *M1, *M2, *M3;       /* prt to structures */
{
 int    i, j, k;
 double **cp1, **cp2, **cp3; /* ptr to two dimensional arrays */
 double sum;


 /* check possibility of multiplication and store space */
 if (M1->num_col != M2->num_row  || M1->num_row != M3->num_row ||
     M2->num_col != M3->num_col)
 {
   fprintf(stderr,"multiply_A_B: matrices doesn't have correct dimension\n");
   return(-1);
 }
 
 cp1 = M1->array;
 cp2 = M2->array;
 cp3 = M3->array;
 
 for (i = 0; i < M1->num_row; i++)
   for (j = 0; j < M2->num_col; j++) 
   {
     sum = 0;
     for (k = 0; k < M1->num_col; k++) 
       sum += cp1[i][k] * cp2[k][j];
       
     cp3[i][j] = sum;
   }
   
  return(0);
}



/**************************************************************/
/* int sum_A_B(Matrix2 *, Matrix2 *):			      */
/* adds B to A and stores result in A			      */
/**************************************************************/
int sum_A_B(M1,M2)
Matrix2 *M1, *M2;
{  
 int    i, j;
 double **cp1, **cp2;

 /* check if matrices are alike  */
 if (M1->num_row != M2->num_row  || M1->num_col != M2->num_col)
 {
   fprintf(stderr,"sum_A_B: matrices doesn't have same dimension\n");
   return(-1);
 }

 cp1 = M1->array;
 cp2 = M2->array;
 
 for (i = 0; i < M1->num_row; i++)
   for (j = 0; j < M1->num_col; j++)
     cp1[i][j] += cp2[i][j];

 return(0);
}



/**************************************************************/
/* copy_A_B(Matrix2 *, Matrix2 *):			      */
/* copies matrix B into A				      */
/**************************************************************/
int copy_A_B(M1, M2)
Matrix2 *M1, *M2;       /* ptr to structure */
{
 int    i, j;
 double **cp1, **cp2;   /* pointers to arrays */


 /* check if matrices have same dimension  */
 if (M1->num_row != M2->num_row  || M1->num_col != M2->num_col)
 {
   fprintf(stderr,"copy_A_B: matrices doesn't have same dimension\n");
   return(-1);
 }
 
 cp1 = M1->array;
 cp2 = M2->array;

 for (i = 0; i < M1->num_row; i++)
   for (j = 0; j < M1->num_col; j++)
     cp1[i][j] = cp2[i][j];
     
 return(0);
}



/************************************************************/
/* print_struct(Matrix2 **)				    */
/* prints R_{i,j} structure				    */
/************************************************************/
void print_struct(S)
Matrix2 *S;

{
 int    row, col; /* array indexes   */
 int    i, j;     /* num of row,col  */
 double **array;
 
 
 i = S->num_row;
 j = S->num_col;
 array = S->array;
 
 printf("print_struct:No. rows=%d, no. col=%d\n",i,j);
 
 for (row = 0; row < i; row++)
  for (col = 0; col < j; col++)
   printf("print_struct:R[%d][%d]=%f\n",row+1,col+1,array[row][col]);
}



/************************************************************/
/* print_matrix_P(M **, V *)				    */
/* prints matrix of block elimination 			    */
/************************************************************/
int print_matrix_P(M,V)
Matrix2 **M; /* ptr to ptr to structures. M is block-wise */
int     *V;  /* ptr to vector of partitions               */
{
 int    i, j;     /* partitions           */
 int    row, col; /* R_{i,j} row/columns  */
 double **pR;     /* ptr to R_{i,j} array */ 
 
  
 for (i = 0; i < V[0]; i++)
   for (j = 0; j < V[0]; j++) 
   {
     pR = M[i][j].array;
     
     if (pR == NULL) 
     { /* NULL bolck  */
     
       printf("print_matrix_P:P[%d][%d] is a null block\n",i+1,j+1);
       continue;
     }
     else 
     { /* there are  non null elements  */
       /* check if it was initialized: num_row/num_col must be !=0 */
       
       if (M[i][j].num_row == 0 || M[i][j].num_col == 0)
       {
         fprintf(stderr,"print_P:accessed matrix not initialized \n");
         return(-1);
       }
       
       for (row = 0; row < M[i][j].num_row; row++)
	 for (col = 0; col < M[i][j].num_col; col++)
	   printf("print_matrix_P:R_{%d,%d}[%d][%d] = %f\n",i+1,j+1,row+1,
		   col+1,pR[row][col]);
     }
   }
   
   return(0);
}



/******************************************************/
/*  LU(Matrix2 *): LU decomposition by Gauss    */
/*  elimination of a  matrix 		              */
/*  Does not do pivoting			      */
/*  this is algorithm 2.4 in "Introduction to matrix  */
/*  computations by G. Stewart, academic press, 1973  */
/*  Overwrites original matrix with the solution      */
/*  returns input matrix factorized		      */
/******************************************************/
void LU(M)
Matrix2 *M;              /* M is a Matrix2 structure    */
{
 int    i, j, k, m, n, r;
 double **array;         /* ptr to ptr to array element */
 

 m = M->num_row;
 
 if (m <= 1) 
    return;        /* nothing to be done */
    
 n = M->num_col;
 array = M->array; /* sets array to ptr to matrix  */

 /* sets r to the minimum between m-1 and n */
 r = MIN(m-1,n);


 for (k = 1; k <= r; k++)
   for (i = k+1; i <= m; i++)
   {
     array[i-1][k-1] = array[i-1][k-1] / array[k-1][k-1];
     
     for (j = k+1; j <= n; j++)
       array[i-1][j-1] = array[i-1][j-1] - 
			 array[i-1][k-1] * array[k-1][j-1];
   }
}



/************************************************************/
/*							    */
/* void find_pi_m(int, double *, int *, Matrix2 **)         */
/* sets pi_{m} = sum_{i=1}^{m-1} pi_{i} R_{i,m}^(m)         */
/* pi_{i} has size = partition[i] and is part of vec_pi     */
/************************************************************/
void find_pi_m(m,vec_pi,V,P)
int     m;       /* partition to be find             */
double  *vec_pi; /* steady state vector              */
int     *V;      /* vector of partitons              */
Matrix2 **P;     /* ptr to ptr to structures R_{i,j} */
{
 double sum_prod;       /* sum_{i=1}^{m-1} (pi_{i}.R_{i,m}^(m))_{col}     */
 int    i;              /* indexes in P array                             */
 int    row,col;        /* indexes in pi vector and R_{i,j} array         */
 double **A;            /* ptr to ptr to R_{i,j}`s element                */
 int    base_i, base_m; /* base_i + 1 = first pi_{i} index in whole vec_pi*/
 
 
 base_m = 0;
 
 for (i = 1; i < m; i++) 
   base_m += V[i];

 for (row = 1; row <= V[m]; row++) 
   vec_pi[base_m + row] = 0.0;


 base_i = 0;
 
 for (i = 1; i < m; i++)
 { /* sum_{i=1}^{m-1} */
 
   A = (P[i-1][m-1]).array; /* R_{i,m} */

   /* pi_{i} *  R_{i,m} */
   for (col = 1; col <= ((P[i-1][m-1]).num_col); col++) 
   {/* col<=V[m]*/
   
     sum_prod = 0.0;
     for (row = 1; row <= ((P[i-1][m-1]).num_row); row++)  /* row <= V[i] */
       sum_prod += vec_pi[base_i + row] * A[row-1][col-1];
     
     /* pi_m */
     vec_pi[base_m + col] += sum_prod;
   }

   base_i += V[i]; /* add except for 1st part. */
 }
}



/************************************************************/
/* Block elimination Grassmann-heymann method  		    */
/*                                                          */
/* Reference: Computational Solution Methods for            */
/* Markov Chains. R. Muntz & E. Silva			    */
/* see equation (4.11)					    */
/* returns pointer to vector of dimension 1 + dimension of  */
/* M1. partition_vector[0] is the no. of partitions   	    */
/* outputs steady state probability vector pi		    */
/************************************************************/
double *GTH(M1, partition_vector)
Matrix *M1;            /* ptr to trans. matrix                */
int *partition_vector; /* ptr to partition vector             */
	  	       /* array of partitions of trans matrix */
		       /* 0-th element = no. partitions       */
		       /* contiguous memory                   */
{

 Matrix2 **P;                /* 2 dim. array of pointers to matrices R */
 double  *vec_pi;            /* ptr state prob. vector; contiguous memory */
 int     i,j;                /* partitions  */
 int     m,n;
 double  **A;                /* ptr to ptr to array */
 Matrix2 *S;                 /* ptr to struct  */
 double  **scratch_I_array;  /* scratch array to find inverse */
 Matrix2 *scratch_I_struct;  /* scratch struct for inverse */
 double  **scratch_P_array;  /* scratch array to find product */ 
 Matrix2 *scratch_P_struct;  /* scratch struct for product */
 int     max;                /* greatest partition size */
 int     row, col;           /* R's indexes */
 int     band;               /* strip of filled elements counting from diagonal */
 double  save;               /* temp. save a value  */



 /* gets band  */
 band = get_band(M1);


 /* if trans. matrix is diagonal there will be problems */
 if (band == 0)
    /* GH: trans. matrix is diagonal. Algorithm will not work: steady state vector is zero */
    return(NULL);


 P = init_P(M1,partition_vector); /* initialize matrices R */
 if (P == NULL)
    return(NULL);

 n = partition_vector[0];


 /* finds greatest partition */
 max = 0;
 for (i = 1; i <= n; i++)
   if (max < partition_vector[i]) 
      max = partition_vector[i];


 /* allocates inverse matrix scratch */
 scratch_I_struct = (Matrix2 *)my_calloc(1,sizeof(Matrix2));
 scratch_I_array = (double **)my_calloc(max,sizeof(double *));
 
 for (row = 0; row < max; row++)
   scratch_I_array[row] = (double *)my_calloc(max,sizeof(double));
   
 scratch_I_struct->array = scratch_I_array;



 /* allocates product matrix scratch */
 scratch_P_struct = (Matrix2 *)my_calloc(1,sizeof(Matrix2));
 scratch_P_array = (double **)my_calloc(max,sizeof(double *));
 
 for (row = 0; row < max; row++)
   scratch_P_array[row] = (double *)my_calloc(max,sizeof(double));
   
 scratch_P_struct->array = scratch_P_array;



 for (m = (n-1); m > 0; m--) 
 {
   /* sets scratch to bear (I-R_{(m+1),(m+1)}^(m+1)) */
   /* size of R_{(m+1),(m+1)}^(m+1)                  */
   scratch_I_struct->num_row = partition_vector[m+1];
   scratch_I_struct->num_col = partition_vector[m+1];


   /* find (I-R_{(m+1),(m+1)}^(m+1))^-1 */
   inverse_GH( (&(P[m][m])), scratch_I_struct);


  /* For test purposes
  printf("GH:(I-R_{%d,%d})^-1 is ...\n",m+1,m+1);
  print_struct(&(P[m][m]));
  */


  for (i = 1; i <= m; i++)
  {
   /* does R_{i,(m+1} belong to band? */
   if (0 == R_in_band(band,i,m+1,partition_vector))
     continue;            /* skip for next row i */

	
   /* sets scratch to bear product of R_{i,(m+1)}^(m+1)) and inv. */
   /* size of R_{i,(m+1)}^(m+1) */
   scratch_P_struct->num_row = partition_vector[i];
   scratch_P_struct->num_col = partition_vector[m+1];

   if ((multiply_A_B((&(P[i-1][m])), (&(P[m][m])), scratch_P_struct)) == -1)
      return (NULL);


   /* save to new R_{i,(m+1)} */
   if ((copy_A_B((&(P[i-1][m])), scratch_P_struct)) == -1)
      return (NULL);


   for (j = 1; j <= m; j++)
   {
      /* does R_{(m+1),j} belong to band? */
      if (0 == R_in_band(band,m+1,j,partition_vector))
         continue;         /* skip for next column j   */
		           /* R_{i,j} keeps unchanged  */


      /* sets scratch to bear product R_{i,(m+1)}^(m+1) */
      /* by R_{(m+1),j}^(m+1)                           */
      /* size of R_{i,j}^(m+1)                          */
      scratch_P_struct->num_row = partition_vector[i];
      scratch_P_struct->num_col = partition_vector[j];


      if ((multiply_A_B( (&(P[i-1][m])), (&(P[m][j-1])), scratch_P_struct)) == -1)
         return (NULL);


     /* add and save to new R_{i,j} */
     if ((sum_A_B((&(P[i-1][j-1])), scratch_P_struct)) == -1)
        return (NULL);

   } /* for j  */

  } /* for i */

#ifdef DEBUG
 printf("\nMatrix P for iterate m=%d\n",m);
 print_matrix_P(P,partition_vector);        /* print matrices R */
#endif

 } /* for m */


 /*allocate steady state vector pi*/
 vec_pi = (double *)my_calloc(1+(M1->num_col),sizeof(double));
 
 vec_pi[0] = (double)M1->num_col; 


 /* set P_{1,1} to (I-P_{1,1}^t) */
 S = (&(P[0][0]));
 A = S->array;


 /*check for correct size */
 if ((S->num_row) != (S->num_col)) 
 {
   fprintf(stderr,"GH: P_{1,1} is not square\n");
   return(NULL);
 }


 /* check P_{1,1} dim. to first partition size */
 if ((S->num_row) != partition_vector[1]) 
 {
   fprintf(stderr,"GH: P_{1,1} dim. differs from partit. size\n");
   return(NULL);
 }


 for (row = 1; row <= S->num_col; row++) 
 {
   A[row-1][row-1] = 1.0 - A[row-1][row-1];
   for (col = row+1; col <= S->num_col; col++) 
   {
     save = A[row-1][col-1];
     A[row-1][col-1] = (-1.0) * A[col-1][row-1];
     A[col-1][row-1] = (-1.0) * save;
   }
 }


 /* factor (I-P_{1,1}^t) = LU */
 LU(S);


 /* initially solve pi_{1} . P_{1,1} = pi_{1}    */
 /* i e, solve A p = 0, A=(I-P_{1,1}^t), p_{n}=1 */
 /* see teor. 2.1.1                              */
 vec_pi[partition_vector[1]] = 1.0;
 
 for (row = ((partition_vector[1])-1); row > 0; row--) 
 {
   save = 0.0; 
   for (col = row + 1; col <= (S->num_col); col++)
     save += A[row-1][col-1] * vec_pi[col];
     
   vec_pi[row] = (-1.0)*save/A[row-1][row-1];
 }

 /* find pi_{m} = sum_{i=1}^{m-1} pi_{i} R_{i,m}^(m) */
 for (m = 2; m <= partition_vector[0]; m++)
   find_pi_m(m,vec_pi,partition_vector,P);

 
 /* normalize vec_pi */
 save = 0.0;
 for (row = 1; row <= M1->num_col; row++)
   save += vec_pi[row];
 for (row = 1; row <= M1->num_col; row++)
   vec_pi[row] = vec_pi[row] / save;
    

 /* releasing P structure : do it!!!!!!!!!!!!! */
 free_Matrix2(n, P);

 for (row = 0; row < max; row++)
   free(scratch_I_array[row]);
   
 free(scratch_I_array);
 free(scratch_I_struct); 

 for (row = 0; row < max; row++)
   free(scratch_P_array[row]);
   
 free(scratch_P_array);
 free(scratch_P_struct); 

 return(vec_pi);
}

