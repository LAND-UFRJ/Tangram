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

/******************************************************************************

   This class inheritances from the template class for generic binary trees, 
adding the balancing characteristics to these trees.
   
******************************************************************************/

#ifndef __TREEAVL_H__
#define __TREEAVL_H__


#include "tree.h"

template <class T>
class TreeAVL : public Tree<T> {

 private:
  int balance;
  TreeAVL* rotationleft( TreeAVL * ) ;
  TreeAVL* rotationright( TreeAVL * ) ;
  TreeAVL* set_bal_left( void ) ;
  TreeAVL* set_bal_right( void ) ;
  
 public:
  TreeAVL();
  ~TreeAVL()  ;
  TreeAVL* insert( T* value) ;
  void print() ;
};

#endif


