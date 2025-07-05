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
#include "sparseMatrix.h"

/************************************************************************/
/*                                                                      */
/*      gs_converged(double *, double *, int):				*/
/*		checking gauss-siedel convergence of two solutions.	*/
/*									*/
/*	Only a template here.						*/
/*									*/
/************************************************************************/
int gs_converged(a, b, n, Epsilon)
double *a;				/* solution vector 1 */
double *b;				/* solution vector 2 */
int n;
double Epsilon;                         /* Precision for numeric approx. */
{
    int i;
	
    for (i = 0; i < n; i++)
        if (ABS(a[i] - b[i]) > Epsilon) 
        {
#ifdef DEBUG
            printf("Over Epsilon at %d\n", i);
#endif
            return(0);
        }
    return(1);
}

/************************************************************************/
/*                                                                      */
/*      sor_converged(double *, double *, int):				*/
/*		checking sor convergence of two solutions.	        */
/*									*/
/*									*/
/************************************************************************/
int sor_converged(a, b, n, Epsilon)
double *a;				/* solution vector 1 */
double *b;				/* solution vector 2 */
int n;
double Epsilon;                         /* Precision for numeric approx. */
{
	int i;

	for (i = 0; i < n; i++)
		if (ABS((a[i]-b[i]) / a[i]) > Epsilon) 
		{
#ifdef DEBUG
			printf("Over Epsilon at %d\n", i);
#endif
			return(0);
		}
	return(1);
}


