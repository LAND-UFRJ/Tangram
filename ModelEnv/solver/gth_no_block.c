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


/************************************************************/
/*                                                          */
/*   divide the matrix into two matrices:                   */
/*   - upper triangular matrix with diagonal zero;          */
/*   - lower triangular matrix with diagonal zero;          */
/*   Ex:                                                    */
/*      input:                                              */
/*      ------                                              */
/*                  | a b c d |                             */
/*        P_upper = | e f g h |                             */
/*                  | i j k l |                             */
/*                  | m n o p |                             */
/*                                                          */
/*      output:                                             */
/*      -------                                             */
/*                  | - b c d |                             */
/*        P_upper = | - - g h |                             */
/*                  | - - - l |                             */
/*                  | - - - - |                             */
/*                                                          */
/*                  | e i m |                               */
/*        P_lower = | - j n |                               */
/*                  | - - o |                               */
/*							    */
/************************************************************/
void generate_triangular_matrices(P_upper, P_lower)
Matrix *P_upper;                    /* ptr to upper triangular matrix  */
Matrix *P_lower;                    /* ptr to lower triangular matrix  */
{
  int    n;                         /* temporary variables               */
  Column *ptr_pos, *ptr_pos2;       /* ptr to a element in a column list */
  Column *pptr_pos, *pptr_pos2;     /* ptr to a element in a column list */
  Column **ptr_col, **ptr_col2;     /* ptr to the address of column      */

  ptr_col = P_upper->col_pp;        /* ptr to the first column */

  for (n = 1; n <= P_upper->num_col; n++, ptr_col++)
  {
      ptr_pos  = *ptr_col;
      pptr_pos = NULL;

      while (ptr_pos && ptr_pos->index < n)
      {
          pptr_pos = ptr_pos;
          ptr_pos  = ptr_pos->next;
      }

      /* there is no element for lower triangular matrix */
      if (!ptr_pos)
          continue;

      /* the first element belongs to the lower triangular matrix */
      if (!pptr_pos)
         *ptr_col = NULL;
      else
         pptr_pos->next = NULL;

      /* eliminate the diagonal */
      if (ptr_pos->index == n)
      {
          pptr_pos = ptr_pos;
          ptr_pos  = ptr_pos->next;
          free(pptr_pos); 
      }

      /* transfer the elements to the lower traingular matrix */
      while (ptr_pos)
      {
          pptr_pos = ptr_pos;
          ptr_pos  = ptr_pos->next;

          if (pptr_pos->value == 0)
          {
               free(pptr_pos);
               continue;
          }

          ptr_col2        = P_lower->col_pp + pptr_pos->index - 2; 
          ptr_pos2        = *ptr_col2;
          pptr_pos->index = n;
          pptr_pos->next  = NULL;
          pptr_pos2       = NULL;

          /* find the last element of the column of the lower triang. matrix */
          while (ptr_pos2)
          {
              pptr_pos2 = ptr_pos2;
              ptr_pos2  = ptr_pos2->next;
          }

          /* it's the first element of the column */
          if (pptr_pos2 == NULL)
             *ptr_col2 = pptr_pos;
          else
             pptr_pos2->next = pptr_pos;
      }
  }
}



/************************************************************/
/*							    */
/* method. Reference: Computational Solution Methods for    */
/*                    Markov Chains. R. Muntz & E. Silva    */
/* see algorithm page 25				    */
/* returns steady state probability vector pi		    */
/*							    */
/* Morganna in 30/09/95.                                    */
/************************************************************/
double *GTH_no_blocks(P_upper)
Matrix *P_upper;                      /* ptr to probability trans. matrix  */
{
  int    n;                           /* temporary variables               */
  double value, sum;                  /* temporary variables               */
  double *vec_pi;                     /* ptr state prob. vector            */
  Column *ptr_pos_up, *ptr_pos_low;   /* ptr to a element in a column list */
  Column *pptr_pos;                   /* ptr to a previous element         */
  Column **ptr_col_up, **ptr_col_low; /* ptr to the address of column      */
  Matrix *P_lower;                    /* ptr to trans. matrix              */

  P_lower = initiate_matrix(P_upper->num_col - 1);
  generate_triangular_matrices(P_upper, P_lower);

  ptr_col_up  = P_upper->col_pp + P_upper->num_col - 1; 
  ptr_col_low = P_lower->col_pp + P_lower->num_col - 1; 

  /* calculate matrix U - equation 2.10, page 26 */
  for (n = P_upper->num_col; n > 1; n--, ptr_col_up--, ptr_col_low--)
  {
      /* printf("n = %d\n", n); */

      /* calculate sum = 1/(1 - sum of row elements) */
      ptr_pos_low = *ptr_col_low;
      sum         = 0;
      while(ptr_pos_low)
      {
          sum        += (ptr_pos_low)->value;
          ptr_pos_low = ptr_pos_low->next;
      }
      if (sum == 0)
         continue;
      sum = 1 / sum;

      /* calculate A1 */
      ptr_pos_low  = *ptr_col_low;
      *ptr_col_low = NULL;
      while(ptr_pos_low)
      {
          ptr_pos_up = *ptr_col_up;

          while(ptr_pos_up) 
          {
              if (ptr_pos_up->index != ptr_pos_low->index)
              {
                 value = sum * ptr_pos_low->value * ptr_pos_up->value;
                 /*********************************************************/
                 /* the values less than 1.0e-30 are considered as zero   */
                 /* it's to reduce the band of matrix, so the computation */
                 /*********************************************************/
                 if (value > 1.0e-30)
                 {
                    if (ptr_pos_up->index < ptr_pos_low->index)
                      add_matrix_position(ptr_pos_up->index,ptr_pos_low->index, 
                                          value, P_upper); 
                    else
                      add_matrix_position(ptr_pos_low->index,ptr_pos_up->index-1,
                                          value,P_lower);
                 }
              }
              ptr_pos_up = ptr_pos_up->next;
          }

          pptr_pos    = ptr_pos_low;
          ptr_pos_low = ptr_pos_low->next;
          free(pptr_pos);
      } 

      /* calculate last column */
      ptr_pos_up = *ptr_col_up;           /* ptr to the 1st column element */
      while (ptr_pos_up)
      {
          ptr_pos_up->value *= sum;
          pptr_pos   = ptr_pos_up;
          ptr_pos_up = ptr_pos_up->next;
      }
  }

  /*release P_lower */
  free_Matrix(1, P_lower);

  /*allocate steady state vector pi*/
  vec_pi = (double *)my_calloc(P_upper->num_col + 1, sizeof(double));
 
  /* printf("calculate pi\n"); */

  /* calculate pi - equation 2.9, page 25 */
  vec_pi[1]  = 1;
  sum        = 1;
  ptr_col_up = P_upper->col_pp + 1;              /* ptr to the column list */
  for (n = 2; n <= P_upper->num_col; n++, ptr_col_up++)
  {
      ptr_pos_up = *ptr_col_up;           /* ptr to the 1st column element */
      while(ptr_pos_up)
      {
          vec_pi[n] += ptr_pos_up->value * vec_pi[ptr_pos_up->index];
          ptr_pos_up = ptr_pos_up->next;
      }
      sum += vec_pi[n];
  }

  /* normalize pi */
  for (n = 1; n <= P_upper->num_col; n++)
      vec_pi[n] /= sum;

  return(vec_pi);
}
