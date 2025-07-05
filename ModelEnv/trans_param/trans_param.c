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
#include <string.h>

#include "formulc.h"

#include "sparseMatrix.h"

#define   MAXSTRING   12255

typedef struct {
	char   code;
	char   name[MAXSTRING];
	double value;
} t_param;

t_param  *param_tb;


typedef struct {
        char   expr[MAXSTRING];
	double value;
} t_expr;

t_expr   *expr_tb;

/* number of parameters in the table */
int no_param;

/* number of expressions in the table */
int no_expr;


int read_expression_file(char *basename)
{
        char  filename[MAXSTRING];
	FILE *fd;
	int i, aux;

        sprintf (filename, "%s.generator_mtx.expr", basename);

	if ( (fd = fopen (filename, "r")) == NULL) {
	   perror ("fopen");
	   return (-1);
	}

	/* read the number of parameters */
	fscanf (fd, "%d\n", &no_param);
	
	/* alloc the table of parameters */
	param_tb = (t_param *) malloc (no_param * sizeof (t_param) );

	/* read all the parameters to the table */
	for (i=0; i<no_param; i++) 
	    fscanf (fd, "%c\t%s\n", &(param_tb[i].code), param_tb[i].name);
	

        /* read the number of expressions */
        fscanf (fd, "%d\n", &no_expr);

        /* alloc the table of expressions */
        expr_tb = (t_expr *) malloc (no_expr * sizeof (t_expr) );

        /* read all the expressions to the table */
        for (i=0; i<no_expr; i++)
            fscanf (fd, "%d\t%s\n", &aux, expr_tb[i].expr);

	fclose (fd);
	
	return (1);
}


int initialize_parameters()
{
	int   i;
	float aux;

	for (i=0; i<no_param; i++) {
	    printf ("Enter the value for %s: ",param_tb[i].name);
	    scanf ("%f", &aux);
	    param_tb[i].value = aux;
	}

	return (1);
}


int evaluate_expressions()
{
    formu                f;          /* pointer to formula (typedef of library formulc)*/
    int                  i;          /*counter */
    char                *variables;  /* string with the parameters that are used in expression */ 
    int                  leng;       /* exigence of formulc */
    int                  error;      /* exigence of formulc */
    double               result;     /* value of the function applied to the expression */

    /* allocate memory to parameters */
    if ((variables = (char*) malloc (no_param)) == NULL) {
        printf("Memory allocation failure.\n");
        return -1;
    }
    
    /* fill the string variables with the parameters code */
    for (i=0; i<no_param;i++)
        variables[i] = param_tb[i].code;
    variables[no_param] = '\0';
    
    /* substitutes the parameters values for this expression */
    for (i=0; i<no_param; i++) 
	make_var(param_tb[i].code, param_tb[i].value);

    for (i=0; i<no_expr; i++) {

      /* translate the formula to f */
      f = translate(expr_tb[i].expr, variables, &leng, &error);

      /* compute the result */
      result = fval_at (f);
        
      expr_tb[i].value = result;
    }

    free(variables);
    return( 1 );
}


Matrix *read_generator_matrix(char *basename)
{
	char  filename[MAXSTRING];
	FILE *fd;

        char   buff[MAXSTRING];
	int    row, col;
	double value;
        int    expr_num;
        int    dimension;

	Matrix *Q;

	sprintf (filename, "%s.generator_mtx.param", basename);

	if ( (fd = fopen(filename, "r")) == NULL) {
  	   perror ("fopen");
	   return (NULL);
	}

	fscanf (fd, "%d\n", &dimension);

	Q = initiate_matrix(dimension);

	while ( !feof(fd) ) {
	
	   fscanf (fd, "%d %d %s", &row, &col, buff);
	   
	   if ( strchr (buff, '[') == NULL ) {
	      value = atof(buff);
	   }
	   else {
	      expr_num = atoi( strchr(buff,'[') + 1);
              value = expr_tb[expr_num].value;
	   }

	   put_matrix_position(row, col, value, Q);

	}

	return (Q);
}

int print_Matrix(char *basename, Matrix *Q)
{
	char   filename[MAXSTRING];
	FILE  *fd;
	int    i, j, dimension;
	double el;

	sprintf (filename, "%s.generator_mtx", basename);

	if ( (fd = fopen(filename, "w")) == NULL) {
  	   perror ("fopen");
	   return (-1);
	}

	dimension = Q->num_col;

	fprintf (fd, "%d\n", dimension);

	for (i=1; i<=dimension; i++)
	    for (j=1; j<=dimension; j++) {
		el = get_matrix_position (i,j,Q);
		if (el != 0)
		    fprintf (fd, "%d %d %.10E\n", i, j, el);
	    }
	
	fclose (fd);

	return (1);
}


int main (int argc, char **argv)
{      
    Matrix *Q;

    if (argc != 2) {
        printf ("Usage: %s <base model name>\n", argv[0]);
        return (-1);
    }

    if (read_expression_file(argv[1]) < 0)
        return (-1);

    if (initialize_parameters() < 0)
        return (-1);

    if (evaluate_expressions() < 0)
        return (-1);

    if ( (Q = read_generator_matrix(argv[1])) == NULL)
        return (-1);

    if ( print_Matrix(argv[1], Q) < 0)
        return (-1);

    return( 0 );
}
