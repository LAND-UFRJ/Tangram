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

#include <stdio.h>
#include "chained_list.h"
#include "connections_list.h"

Connection::Connection( uint iid, double cap, double mean , double var ) : Chained_Element()
{
  id                        = iid;
  efec_capacity             = cap;
  mean_rate                 = mean;
  variance_rate             = var;
}

Connection::~Connection()
{
  /* do nothing */
}

void Connection::print()
{
  cout <<  "Id =" << id << " Effective Capacity=" <<   efec_capacity << " Mean=" << mean_rate << " Variance=" << variance_rate << "\n" ;
}

double Connection::show_capacity()
{
  return efec_capacity;
}

double Connection::show_mean()
{
  return mean_rate;
}

double Connection::show_variance()
{
  return variance_rate;
}


int Connection::compare_connec( uint a_id) 
{
  if ( id == a_id )
    return 1;
  else
    return 0;
}

/*****************************************************************************/

Connection_List::Connection_List() : Chained_List()
{

}

Connection_List::~Connection_List()
{
}



int Connection_List::add_connec( uint id, double cap, double mean, double var) 
{
  Connection  *connec;
  int     i = 0;
  
  connec = show_1st_connec();
  while (connec != NULL) {
    /* if the connection exists return its position in the list */
    if (connec->compare_connec(id) )
      return (i);
    connec = show_next_connec();
    i++;
  }
  /* if the connection doesn't exist create a new connection and */
  /* add it to the end of the list. Return the position of the new connection */
  connec = new Connection(id, cap, mean,var);
  add_tail_connec(connec);
  return (i);
}

int Connection_List::add_tail_connec(Connection *new_connec)
{
  return ( add_tail( (Chained_Element *)new_connec) );
}


Connection *Connection_List::show_1st_connec()
{
  return ( (Connection *)show_head() );
}

Connection *Connection_List::get_1st_connec()
{
  return ( (Connection *)del_head() );
}

Connection *Connection_List::show_next_connec()
{
  return ( (Connection *)show_next() );
}

Connection *Connection_List::show_curr_connec()
{
  return ( (Connection *)show_curr() );
}


Connection *Connection_List::del_elem_connec()
{
  return ( (Connection *)del_elem() );
}

int Connection_List::query_connec(uint id)
{
  Connection  *connec_aux;
  
  connec_aux = show_1st_connec();
  while (connec_aux != NULL) {
    if (connec_aux->compare_connec(id) )
      return (1);
    connec_aux = show_next_connec();
  }
  return (0);
}


int Connection_List::show_no_connec()
{
  return ( show_no_elem() );
}

