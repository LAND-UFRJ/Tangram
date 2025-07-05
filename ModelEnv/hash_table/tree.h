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
   This class implements a template class for generic binary trees.

   The class which wishes to use this tree, needs to overload the following
   operators: ( ==, < , > ). This is the one constrain of this class.

   In the example below, the class node implements the operators, and it 
   shows the utilization of the class tree.
******************************************************************************/

#ifndef __TREE_H__
#define __TREE_H__


#include "node.h"


template <class T>
class Tree{


 protected:
  T    *nod;
  Tree *left;
  Tree *right;
  
  
 public:
  
  Tree() 
    { 
      left=right=NULL; 
      nod=NULL; 
    }
  
  ~Tree()
    { 
    } 
  
  void setnod ( T* value){
    nod = value;
  }
  
  T getnod( void ){
    return *nod;
  }

  boolean insert( T* value)
    {
      if ( nod == NULL ){
	nod = value;	
	return TRUE;
      }
      else{
	if ( *value == *nod ) 
	  return FALSE;
	else
	  if ( *value < *nod )
	    
	    if ( left == NULL){
	      left = new Tree<T>;
	      left->nod = value ;
	      return TRUE;
	    }    
	    else
	      return left->insert(value) ;
	
	  else
	    if ( right == NULL){
	      right = new Tree<T>;
	      right->nod = value ;
	      return TRUE;
	    }
	    else
	      return right->insert(value);
      }
    }
  
  
  
  T* find( T* value)
    {
      if ( nod == NULL ) {
	return NULL;
      }
      else {
	
	if ( *value  == *nod )
	  return nod;
	else
	  
	  if ( *value < *nod )
	    if ( left == NULL)
	      return NULL;
	    else
	      return left->find(value);
	
	  else
	    if ( right == NULL  )
	      return NULL;
	    else
	      return right->find(value);
      }
    }
  
  void print( void )
    {
      if ( nod == NULL )
	return;
      else{
	nod->print();
	if ( left)
	  left->print();
	if ( right )
	  right->print();
	return;
      }
    }
  
};

#endif


