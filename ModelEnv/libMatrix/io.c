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
/*       Sparse matrix input and output routines.	        	*/
/*	 Yuguang Wu UCLA CSD 8/12/93		         		*/
/*									*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "io.h"
#include "Matrix.h"


char *my_calloc();


/************************************************************************/
/*                                                                      */
/*	get_trans_matrix(FILE *):                                           */
/*      routine to convert 3-tuple trans. into a sparse matrix in       */
/*	the form of column-wise linked-lists.                               */
/*                                                                      */
/************************************************************************/
Matrix *get_trans_matrix(fp)
FILE *fp;           /* input file                             */
{
	int s;			/* source state of each state trans. pair */
	int d;			/* destination state of each state pair   */
    int num_stat;   /* number of states                       */
	double p;		/* transition probability                 */
	Matrix *mat;    /* ptr to resulting sparse trans. matrix  */


	/* get the max. number of states */
	fscanf(fp, "%d", &num_stat);  

	if (num_stat == 0) /* ignored case */
           return((Matrix *)NULL);
	
    /* allocate matrix structure */
    mat = initiate_matrix(num_stat);

	/* loop through all inputs of (source-state, dest.-state, prob.) */
	while (fscanf(fp, "%d %d %lf", &s, &d, &p)==3 && s>0) 
           put_matrix_position(s, d, p, mat);

	return((Matrix *)mat);	 /* got the data, thanks disk (or else). */
}



/************************************************************************/
/*                                                                      */
/*	get_trans_matrix(FILE *):                                           */
/*      routine to convert 3-tuple trans. into a sparse matrix in       */
/*	    the form of column-wise linked-lists.                           */
/*      P.S.: states number is knowing                                  */
/*      Morganna in 09/04/95                                            */
/*                                                                      */
/************************************************************************/
Matrix *get_trans_matrix2(fp, num_stat)
FILE *fp;          /* input file                              */
int num_stat;     /* states number                            */ 
{
	int s;			/* source state of each state trans. pair */
	int d;			/* destination state of each state pair   */
	double p;		/* transition probability                 */
	Matrix *mat;    /* ptr to resulting sparse trans. matrix  */


	if (num_stat == 0)	     /* ignored case */
	   return((Matrix *)NULL);
	
        /* allocate matrix structure */
        mat = initiate_matrix(num_stat);

	/* loop through all inputs of (source-state, dest.-state, prob.) */
	while (fscanf(fp, "%d %d %lf", &s, &d, &p)==3 && s>0) 
           put_matrix_position(s, d, p, mat);

	return((Matrix *)mat);	 /* got the data, thanks disk (or else). */
}



/************************************************************************/
/*                                                                      */
/*	get_trans_matrix_remap(FILE *, int *):                              */
/*      routine to convert 3-tuple trans. into a sparse matrix in       */
/*	the form of column-wise linked-lists. Before it stores the          */
/*      matrix it also remaps the states so that all states with        */
/*      reward = 1 are the first states in the state vector. This       */
/*      is necessary for the "cumulative operationa time" program       */
/*                                                                      */
/************************************************************************/
Matrix *get_trans_matrix_remap(fp, st_remap)
FILE   *fp;           /* input file */
int *st_remap;        /* st_remap[i] = j; i = new state member, j = original */
{
	int s;        /* source state of each state trans. pair */
	int s2;       /* source state translated */
	int d;        /* destination state of each state pair */
	int d2;       /* destination state translated */
    int num_stat; /* number of states */
	double p;     /* transition probability */
	Matrix *mat;  /* ptr to resulting sparse trans. matrix */

	/* get the max. number of states */
	/* get_trans_size(fp, &num_stat);*/
	fscanf(fp, "%d\n", &num_stat);

	if (num_stat == 0)	/* ignored case */
           return((Matrix *)NULL);
	
        /* allocate matrix structure */
        mat = initiate_matrix(num_stat);

	/* loop through all inputs of (source-state, dest.-state, prob.) */
	while (fscanf(fp, "%d %d %lf", &s, &d, &p)==3 && s>0)
	{
	   s2 = st_remap[s];
	   d2 = st_remap[d];
           put_matrix_position(s2, d2, p, mat);
        }

	return((Matrix *)mat);	/* got the data, thanks disk (or else). */
}


/************************************************************************/
/*                                                                      */
/*	put_trans_matrix(Matrix *):                                         */
/*      routine to print out a sparse matrix (column-wise link lists).	*/
/*	Essetially the reverse of get_trans_matrix() routine.               */
/*                                                                      */
/************************************************************************/
void put_trans_matrix(mat)
Matrix *mat;
{
    print_matrix( mat, stdout );
}



/************************************************************************/
/*                                                                      */
/*  get_vector(FILE *):                                                 */
/*      routine to a vector, first number denoting # of elements.       */
/*                                                                      */
/************************************************************************/
double *get_vector(fp)
FILE *fp;
{
	int i;
	int num_stat;		/* number of states in the model */
	double *vec;		/* ptr to resulting vector       */


	if (fscanf(fp, "%d", &num_stat)!=1 || num_stat<= 0) 
	{       /* ignored case */
		fprintf(stderr, "empty space?\n");
		return((double *)NULL);
	}
	
	/* allocate the vector structure */
	vec = (double *)my_calloc(num_stat, sizeof(double));

	for (i=0; i<num_stat; i++)
		if (fscanf(fp, "%lf", &vec[i])!=1) 
		{
			fprintf(stderr, "bad input vector\n");
			free((char *)vec);
			return((double *)NULL);
		}
		
	return ((double *)vec);
}



/************************************************************************/
/*                                                                      */
/*  put_vector(double *, int):                                          */
/*      print a vector with known dimension.                            */
/*                                                                      */
/************************************************************************/
void put_vector(v, n)
double *v;		/* ptr to a vector           */
int n;			/* number of vector elements */
{
	int i;


	/* printf("vector at %x\n", v); */
	for (i=0; i<n; i++)
		printf("%f ", v[i]);
	printf("\n");
}


/************************************************************************/
/*                                                                      */
/*      print_matrix(char, Matrix *mat, FILE *fout):                    */
/*              print matrix mat onto file fout.                        */
/*                                                                      */
/************************************************************************/
void print_matrix(mat, fout)
Matrix *mat;            /* ptr to the matrix to be printed */
FILE *fout;             /* ptr to the destination file     */
{
	int num_stat;           /* number of states in the model           */
	int col;		/* index to column number                  */
        Column *cp;             /* ptr to a element in a column list       */
        Column **pcp; 		/* ptr to the address of previous col. ptr */


	if (!mat) return;

	num_stat = mat->num_col;	/* max. state no. in matrix       */
	pcp = mat->col_pp;		/* pointer to the 1st column list */

	/* loop through all columns */
	for ( col = 1; col <= num_stat; pcp++, col++ ) 
	{
		cp = *pcp;    /* ptr to 1st element of the current column */

		while (cp) 
		{	/* loop through all elements in this column */
			fprintf( fout, "%d %d %.10e\n",
                                 cp->index, col, cp->value);
			cp = cp->next;
		}

	}
}

/************************************************************************/
/*                                                                      */
/*	get_impulse_matrix(FILE *, number_states):				         	*/
/*      routine to convert 3-tuple trans. into a sparse matrix in 	    */
/*	the form of column-wise linked-lists. Reads the transition's reward */
/*  and probability and stores the product into the new matrix.         */
/************************************************************************/
Matrix *get_impulse_matrix(fp, number_states)
FILE *fp;                       /* input file */
int number_states;
{
	int s;              /* source state of each state trans. pair */
	int d;              /* destination state of each state pair   */
    char line[ 256 ];
	double rew, prob;   /* transition probability                 */
	Matrix *mat;        /* ptr to resulting sparse trans. matrix  */


	/* get the max. number of states */
	//fscanf(fp, "%d", &num_stat);  

	if (number_states == 0)	         /* ignored case */
           return((Matrix *)NULL);
	
        /* allocate matrix structure */
        mat = initiate_matrix(number_states);

	/* loop through all inputs of (source-state, dest.-state, prob.) */
	while( !feof( fp ) )
    {
        fgets( line, 256, fp );
        sscanf(line, "%d %d %lf %lf", &s, &d, &rew, &prob);
        put_matrix_position(s, d, rew * prob, mat);
    }

	return((Matrix *)mat);	 /* got the data, thanks disk (or else). */
}


/************************************************************************/
/*  reads the probability distribution from 'fpi'. This routine can be  */
/* used to read both .SS. as .TS. files.                                */
/* Parameters:                                                          */
/*   fpi        - pointer to probability distribution file;             */
/*   num_states - total number of states in the model.                  */
/************************************************************************/
double *get_prob( fpi, num_states )
FILE *fpi;
int   num_states;
{
    int     state;       /* state of the model*/
    char   *ptr;         /* auxiliary ptr - file manipulation*/  
    char    line[ 256 ];
    double *pi;          /* vector with probability distribution*/
    double  prob;        /* state probability*/ 

    pi = (double *)malloc( sizeof( double ) * num_states );
    if( pi != NULL )
    {
        while( !feof( fpi ) )
        {
            fgets( line, 256, fpi );
            ptr = line;
            while( *ptr == ' ' && *ptr != 0 )
                ptr++;
            if( *ptr != '#' )
            {
                sscanf( line, "%d %lf", &state, &prob );
                if( state <= num_states )
                    pi[ state - 1 ] = prob;
            }
        }
    }
    return( pi );
}
