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
/*              Miscellaneous routines.					*/
/*									*/
/*              Yuguang Wu UCLA CSD 8/12/93				*/
/*									*/
/************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "define.h"
#include "misc.h"



/************************************************************************/
char *my_malloc(a)
unsigned a;
{
	char *p;
	
	if ((p = (char *)malloc(a)) == NULL) 
        {
            printf("malloc capsized!");
            exit(40);
        }
        memset(p,0,a);        
	return((char *)p);
}



/************************************************************************/
char *my_calloc(a, b)
unsigned a;
unsigned b;
{
	char *p;

	p = (char *)calloc(a, b);
	
	if (p == NULL) 
        {
            printf("calloc flipped!");
            exit(40);
        }
        memset(p,0,a*b); 
	return((char *)p);
}



/************************************************************************/
void free_Matrix(num, A)
int    num;                    /* number of matrix structures */
Matrix *A;                     /* matrix structure            */
{
   int    i, j, num2;          /* temporary variables                */
   Column *cp, *cp2;           /* ptrs to a element in a column list */
   Column **pcp;               /* ptrs to the address of column      */
   Matrix *A2;                 /* matrix structure                   */


   A2 = A;            
   for(i = 0; i < num; i++)        /* release matrix structure    */
   {                  	
       pcp  = A->col_pp;           /* ptr to structure of columns */
       num2 = A->num_col;          /* number of columns           */

       for(j = 0; j < num2; j++)   /* release the columns         */
       {                  	
           cp = *pcp;
           while (cp)              /* release the elements of each column */
           {
               cp2 = cp;
               cp  = cp->next;
               free((char *)cp2);
           } 
           pcp++;
       }                  	

       pcp = A->col_pp;   /* release ptr to the first elements of columns */ 
       free((char *)pcp);
       A++;
   }
   
   free((char *)A2);
}



/************************************************************************/
void free_Matrix2(num, A)
int    num;                    /* number of matrix structures */
Matrix2 **A;                   /* ptr to matrix structure     */
{
   int    i, j, row;           /* temporary variables              */
   int    num_col, num_row;    /* number of columns/number of rows */
   double **pR, *ptr;


   for(i = 0; i < num; i++)      
   {                  	
       for(j = 0; j < num; j++) 
       {                  	
           pR      = (A[i][j]).array;
           num_row = (A[i][j]).num_row;
           num_col = (A[i][j]).num_col;

           for (row = 0; row < num_row; row++)
           {
               ptr = pR[row];
               free((char *)ptr);
           } 
           free((char **)pR);
       }                  	
   }
   
   free((char **)A);
}
/************************************************************************/
