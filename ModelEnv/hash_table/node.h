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

#ifndef __NODE_H__
#define __NODE_H__


#ifndef __ULONGLONG_T__
#define __ULONGLONG_T__
typedef unsigned long long int u_longlong_t;
#endif 

#ifndef __BOOLEAN__
#define __BOOLEAN__
typedef unsigned int boolean;
#endif 

#ifndef __UINT__
#define __UINT__
typedef unsigned int uint;
#endif 

#define TRUE (1)
#define FALSE (0)

class Node {
  
 protected:
  u_longlong_t value;
  uint order;
  
 public:
  // Constructors 
  Node();
  Node( u_longlong_t , uint );

  // Destructor
  ~Node();

  // Methods
  u_longlong_t getvalue();
  void         setvalue( u_longlong_t );
  uint         getorder();
  void         setorder( uint );
  void         print();

  // Overloading of operators 
  
  boolean operator==( Node& );
  boolean operator<( Node& );
  boolean operator>( Node& );
  
};

#endif
