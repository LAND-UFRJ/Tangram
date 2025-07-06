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

using namespace std;

#include "node.h"

Node::Node ( u_longlong_t val, uint ord )
{
  value = val; 
  order = ord;   
}

Node::~Node ()
{

}

u_longlong_t Node::getvalue( void )
{
  return value;
}

uint Node::getorder( void )
{
  return order;
}

void Node::setvalue( u_longlong_t new_val )
{
  value = new_val;
}

void Node::setorder( uint val )
{
  order = val;
}


void Node::print( void )
{
  cout << "Value = " << value << "\t Order =" << order << "\n" ;
}

boolean Node::operator<( Node& a )
{
  
  if ( value  < a.getvalue()  )
    return TRUE;
  else
    return FALSE;
  
}

boolean Node::operator>( Node& a )
{
  if ( value > a.getvalue() )
    return TRUE;
  else
    return FALSE;
}

boolean Node::operator==( Node& a )
{
  if ( value == a.getvalue() )
    return TRUE;
  else
    return FALSE;
}










