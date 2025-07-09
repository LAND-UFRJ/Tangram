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
#include <iostream>
#include <climits>

using namespace std;

#include <stdlib.h>
#include <math.h>
#include "hashlib.h"

Hash::Hash(uint *range, uint num)
{
  uint i;
  
  if ( ( max_vec = new uint [num] ) == NULL)
    cerr << "ERROR: Hash::Hash: No space to alloc max vector";
  
  for ( i = 0; i < num; i++ )
    max_vec[i] = range[i] + 1;
  
  num_elem  = num; 
  tree      = new TreeAVL<Node>; 
  count     = 0 ;
  node_aux  = new Node;
  max_value = 1;
  mem_size  = 10240;
  mapp_vec  = NULL;
} 

boolean Hash::init(void){
  uint i;
  u_longlong_t maxvalonglong = 1ULL;
  double maxval = 1.0;

  mapp_vec  = (u_longlong_t * ) calloc( mem_size, sizeof(u_longlong_t) );
  
  if ( mapp_vec == NULL ) 
    return FALSE;

  for ( i = 1; i < num_elem; i++ ){
    maxval *= (double) max_vec[i];
    maxvalonglong *= (u_longlong_t) max_vec[i];
  }

  if ( maxval > (double) ULLONG_MAX)
    return FALSE;
  
  else
  {
    max_value = maxvalonglong;
    return TRUE;
  }
}

Hash::~Hash()
{
  delete tree;
  delete node_aux;
  delete max_vec;
  free (mapp_vec);
}

uint Hash::show_no_states( void )
{
  return count ;
}

void Hash::print(void)
{
  tree->print();
}

boolean Hash::vec_to_hash_num( u_longlong_t *num, uint *vec )
{
  u_longlong_t total, aux;
  uint j, i;
  
  for ( i = 1; i < num_elem; i++)
    if ( vec[i] > (max_vec[i] - 1 ) )
      return FALSE;
  
  total = (u_longlong_t) vec[1];
  
  for( i = 2; i < num_elem; i++){
    aux = 1;
    for ( j = i - 1 ; j >= 1 ; j--)
      aux *= (u_longlong_t ) max_vec[j] ;
    
    total += (u_longlong_t ) vec[i] * aux;
  }
  
  *num = total; 
  if ( *num > max_value)
    return FALSE;
  else
    return TRUE;
}

boolean Hash::hash_num_to_vec( u_longlong_t num, uint *vec )
{
  uint j, i ;
  u_longlong_t aux;
  
  
  if ( num > max_value )
    return FALSE;

  vec[1] = num % max_vec[1];
  
  for( i = 2; i < num_elem; i++){
    aux = 1;
    for ( j = i - 1; j >= 1; j--)
      aux *= (u_longlong_t) max_vec[j];
    
    vec[i] = (num / aux) % max_vec[i];
  }
  
  return TRUE;
}

boolean Hash::num_to_vec( uint index, uint *vec )
{
  uint j, i ;
  u_longlong_t aux;
  u_longlong_t num;

  vec [ 0 ] = num_elem - 1;

  if ( index >= mem_size )
    return FALSE;
  
  num = mapp_vec[ index ];
  
  if ( num > max_value )
    return FALSE;
  
  
  vec[1] = num % max_vec[1];
  
  for( i = 2; i < num_elem; i++){
    aux = 1;
    for ( j = i - 1; j >= 1; j--)
      aux *= (u_longlong_t) max_vec[j];
    
    vec[i] = (num / aux) % max_vec[i];
  }
  
  return TRUE;
}

boolean Hash::vec_to_num( uint *index, uint *vec )
{
  u_longlong_t total, aux, num;
  uint j, i, order;
  Node *temp;
  
  
  for ( i = 1; i < num_elem; i++)
    if ( vec[i] > max_vec[i] )
      return FALSE;
  
  vec_to_hash_num( &num, vec );
  
  total = (u_longlong_t) vec[1];
  
  for( i = 2; i < num_elem; i++){
    aux = 1;
    for ( j = i - 1 ; j >= 1 ; j--)
      aux *= (u_longlong_t ) max_vec[j] ;
    
    total += (u_longlong_t ) vec[i] * aux;
  }
  
  node_aux->setvalue(num);
  
  if ( ( temp = tree->find(node_aux) ) == NULL )
    return FALSE;
  
  order = temp->getorder();
  
  *index = order; 
  
  return TRUE;
}

boolean Hash::insert(uint *vec)
{
  u_longlong_t num, *map_aux;
  Node        *nod;
  uint i, mem_aux ;
  

  if ( !vec_to_hash_num( &num, vec ) )
    return FALSE;
  
  if ( count >= ( mem_size -1 ) ) 
    {
      
      mem_aux  = 2 * mem_size;
      map_aux = (u_longlong_t * ) realloc( mapp_vec, 
					   sizeof(u_longlong_t) * mem_aux );

      if ( map_aux == NULL )
	return FALSE;
      
      for ( i = mem_size; i < mem_aux; i++ )
	map_aux[i] = 0;
	
      mem_size = mem_aux;
      mapp_vec = map_aux;      
    }
  
  node_aux->setvalue(num);
  
  if ( tree->find( node_aux ) == NULL )
    {
      count++;
      mapp_vec[count] = num;
      nod = new Node( num, count );
      tree=tree->insert( nod );
    }
  return TRUE;
}

boolean Hash::find( uint index )
{
  u_longlong_t num;
  
  if ( index >= mem_size )
    return FALSE;
  
  num = mapp_vec[ index ];
  
  node_aux->setvalue(num);
  
  if ( num > max_value )
    return FALSE;
  
  if ( tree->find(node_aux) == NULL )
    return FALSE;  
  else
    return TRUE;
}

boolean Hash::find( uint *vec )
{
  u_longlong_t  num;
  
  if ( !vec_to_hash_num(&num, vec) )
    return FALSE;
  
  node_aux->setvalue(num);
  
  if ( tree->find(node_aux) == NULL )
    return FALSE;
  else
    return TRUE;
}

