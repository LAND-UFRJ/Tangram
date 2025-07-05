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

#include "ImpMatrix.h"

ImpMatrix  *init_ImpMatrix(int dim)
{
   ImpMatrix *IM;

   IM = (ImpMatrix *) calloc(1, sizeof(ImpMatrix));

   IM->dimension = dim;

   IM->imp_col_vector = (ImpCol *) calloc(dim + 1, sizeof(ImpCol));

   return(IM);
}

int put_impulse(ImpMatrix* IM, int row, int col, double val, double prob)
{
   Impulse *new_imp;   
   ImpElem *curr_imp_el, *prev_imp_el;
   ImpElem *new_imp_elem;

   if ((row < 1) || (row > IM->dimension))
      return(-1);

   if ((col < 1) || (col > IM->dimension))
      return(-1);
 
   new_imp = (Impulse *)calloc(1, sizeof(Impulse));
 
   new_imp->value  = val;
   new_imp->prob = prob;
   new_imp->next = NULL;

   curr_imp_el = IM->imp_col_vector[col].first_elem; 
   prev_imp_el = NULL;
   
   while (curr_imp_el != NULL) {

      if (curr_imp_el->index >= row)
         break;

      prev_imp_el = curr_imp_el;
      curr_imp_el = curr_imp_el->next;
   }

   if ((curr_imp_el != NULL) && (curr_imp_el->index == row)) { /* imp_elem already exists */

      /* insert new_imp */
      new_imp->next = curr_imp_el->first_imp;
      curr_imp_el->first_imp = new_imp;

   } else {

        /* allocate a new imp_elem */
        new_imp_elem = (ImpElem *) calloc(1, sizeof(ImpElem));
        new_imp_elem->index = row;
        new_imp_elem->first_imp = new_imp;
  
        /* insert it in the list */
        if (prev_imp_el == NULL) {
           new_imp_elem->next = IM->imp_col_vector[col].first_elem;
           IM->imp_col_vector[col].first_elem = new_imp_elem;
        } else {
             new_imp_elem->next = prev_imp_el->next;
             prev_imp_el->next  = new_imp_elem;
        }
   }
   return( 1 );
}

Impulse* get_impulse(ImpMatrix* IM, int row, int col)
{
   ImpElem  *imp_el_aux;


   if ((row < 1) || (row > IM->dimension))
      return(NULL);

   if ((col < 1) || (col > IM->dimension))
      return(NULL);

   imp_el_aux = IM->imp_col_vector[col].first_elem;

   while (imp_el_aux != NULL) {

      if (imp_el_aux->index >= row)
         break;

      imp_el_aux = imp_el_aux->next;
   }

   if (imp_el_aux == NULL)
      return (NULL);
   else {
      if (imp_el_aux->index == row)
         return (imp_el_aux->first_imp);
      else
         return (NULL);
   }
}

int read_ImpMatrix( ImpMatrix *IM, char *filename )
{
   FILE  *f;
   int    row, col;
   float  val, prob;

   if( (f = fopen( filename, "r" )) == NULL )
   {
      perror( "fopen" );
      return( -1 );
   }

   while( !feof( f ) )
   {
      fscanf( f, "%d %d %f %f", &row, &col, &val, &prob );
      put_impulse( IM, row, col, (double)val, (double)prob );
   }

   fclose( f );
   return( 1 );
}

void print_ImpMatrix (ImpMatrix *IM)
{
   int col;
   ImpElem  *imp_el;
   Impulse  *imp;

   for (col=1; col<=IM->dimension; col++) {
      imp_el = IM->imp_col_vector[col].first_elem;   
      while (imp_el != NULL) {
         imp = imp_el->first_imp;
         printf ("%d -> %d :", imp_el->index, col);
         while (imp != NULL) {
            printf (" (%.5f,%.5f)", imp->value, imp->prob);
            imp = imp->next;
         }
         printf("\n");
         imp_el = imp_el->next;
      }
   }
}
