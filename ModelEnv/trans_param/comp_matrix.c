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
#include <math.h>

#include "formulc.h"

#include "sparseMatrix.h"

#define   MAXSTRING   255
double    EPSILON     = 1E-05;


Matrix  *m1, *m2;

Matrix *read_generator_matrix(char *basename)
{

	char  filename[MAXSTRING];
	FILE *fd;

	int    row, col;
	float  value;
    int    dimension;

	Matrix *Q;

	sprintf(filename, "%s", basename);

	if( (fd = fopen( filename, "r" )) == NULL )
    {
  	   perror( "fopen" );
	   return( NULL );
	}

	fscanf( fd, "%d\n", &dimension );

	Q = initiate_matrix( dimension );

	while( !feof( fd ) )
    {
	   fscanf (fd, "%d %d %f", &row, &col, &value);
	   
	   put_matrix_position(row, col, value, Q);
	}

	return( Q );
}


int main(int argc, char **argv)
{
    int  i,j;
    double e1,e2;

    if (argc > 3)
	EPSILON = atof(argv[3]);

    if (argc < 3) {
	printf ("Usage: %s <matrix 1> <matrix 2> [EPSILON]\n", argv[0]);
	return (-1);
    }

    m1 = read_generator_matrix(argv[1]);

    m2 = read_generator_matrix(argv[2]);

    for (i=1; i<=m1->num_col; i++) {
	for (j=1; j<=m1->num_col; j++) {
	    e1 = get_matrix_position (i,j,m1);
	    e2 = get_matrix_position (i,j,m2);
	    if ( fabs(e1-e2) > EPSILON ) {
		printf ("Matrix are not equal.\n");
		printf ("M1[%d,%d] = %.10E\n",i,j,e1);
		printf ("M2[%d,%d] = %.10E\n",i,j,e2);
		return(1);
	    }
	}
	
    }
    printf ("Both matrix are equal with EPSILON = %.10E\n", EPSILON);
    return( 0 );
}
