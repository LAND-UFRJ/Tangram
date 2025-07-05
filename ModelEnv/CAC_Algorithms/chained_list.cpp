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

#include "chained_list.h"


Chained_Element::Chained_Element()
{
    next = prev = NULL;
}

Chained_Element *Chained_Element::show_prev()
{
  return (prev);
}

void Chained_Element::set_prev(Chained_Element *el)
{
  prev = el;
}

Chained_Element *Chained_Element::show_next()
{
  return (next);
}

void Chained_Element::set_next(Chained_Element *el)
{
  next = el;
}

/***************************************************************************/

Chained_List::Chained_List()
{
  first_el = last_el = curr_el = NULL;
  no_elem = 0;
  access = LIST_RW;
}

Chained_List::Chained_List(Chained_Element *el)
{
  first_el = last_el = curr_el = el;
  no_elem = 1;
  access = LIST_RW;
}

Chained_List::Chained_List(Chained_List *ch_list)
{  
  first_el = ch_list->show_head();
  curr_el  = ch_list->show_head();
  last_el  = ch_list->show_tail();
  no_elem  = ch_list->show_no_elem();
  access   = LIST_RO;
}

Chained_Element *Chained_List::show_head()
{
  curr_el = first_el;
  return (curr_el);
}

Chained_Element *Chained_List::show_tail()
{
  curr_el = last_el;
  return (curr_el);
}

Chained_Element *Chained_List::show_next()
{
  if (is_last(curr_el))
    return(NULL);
  else {
    curr_el = curr_el->show_next();
    return (curr_el);
  }
}

Chained_Element *Chained_List::show_next(Chained_Element *el)
{
  if (is_last(el))
    return(NULL);
  else {
    curr_el = el->show_next();
    return (curr_el);
  }
}

Chained_Element *Chained_List::show_prev()
{
  if (is_first(curr_el))
    return(NULL);
  else {
    curr_el = curr_el->show_prev();
    return (curr_el);
  }
}

Chained_Element *Chained_List::show_prev(Chained_Element *el)
{
  if (is_first(el))
    return(NULL);
  else {
    curr_el = el->show_prev();
    return (curr_el);
  }
}

Chained_Element *Chained_List::show_curr()
{
  return (curr_el);
}

int Chained_List::show_no_elem()
{
  return (no_elem);
}

int Chained_List::is_empty()
{
  if (no_elem == 0)
    return (1);
  else
    return (0);
}

int Chained_List::is_first(Chained_Element *el)
{
  if (el == NULL)
    return (-1);

  if (el->show_prev() == NULL)
    return (1);
  else
    return (0);
}

int Chained_List::is_last(Chained_Element *el)
{
  if (el == NULL)
    return (-1);

  if (el->show_next() == NULL)
    return (1);
  else
    return (0);
}

int Chained_List::add_head(Chained_Element *new_el)
{
  if (access == LIST_RO) {
    return (-1);
  }

  if (new_el == NULL)
    return(-1);

  if ( is_empty() ) {
    first_el = last_el = curr_el = new_el;
    no_elem = 1;
  } else {
    first_el->set_prev(new_el);
    new_el->set_next(first_el);
    new_el->set_prev(NULL);
    first_el = new_el;
    no_elem++;
    curr_el = new_el;
  }
  return (no_elem);
}

int Chained_List::add_tail(Chained_Element *new_el)
{
  if (access == LIST_RO) 
    return (-1);

  if (new_el == NULL)
    return(-1);

  if ( is_empty() ) {
    first_el = last_el = curr_el = new_el;
    no_elem = 1;
  } else {
    last_el->set_next(new_el);
    new_el->set_prev(last_el);
    new_el->set_next(NULL);
    last_el = new_el;
    no_elem++;
    curr_el = new_el;
  }
  return(no_elem);
}

int Chained_List::add_prev(Chained_Element *new_el)
{
  Chained_Element *el_aux;
 
  if (access == LIST_RO) 
    return (-1);

  if (new_el == NULL)
    return(-1);
  
  if (curr_el == NULL)
    return (add_head (new_el) );

  if (is_first(curr_el))
    return ( add_head(new_el) );
  
  el_aux = curr_el->show_prev();

  new_el->set_next(curr_el);
  curr_el->set_prev(new_el);
  new_el->set_prev(el_aux);
  el_aux->set_next(new_el);

  curr_el = new_el;
  no_elem++;

  return(no_elem);
}

int Chained_List::add_next(Chained_Element *new_el)
{
  Chained_Element *el_aux;

  if (access == LIST_RO) 
    return (-1);

  if (new_el == NULL)
    return(-1);

  if (curr_el == NULL)
    return (add_head (new_el) );

  if (is_last(curr_el))
    return ( add_tail(new_el) );
  
  el_aux = curr_el->show_next();

  new_el->set_prev(curr_el);
  curr_el->set_next(new_el);
  new_el->set_next(el_aux);
  el_aux->set_prev(new_el);

  curr_el = new_el;
  no_elem++;

  return (no_elem);
}


Chained_Element *Chained_List::del_head()
{
  Chained_Element *del_el;

  if (access == LIST_RO) 
    return (NULL);

  if ( is_empty() )
    return (NULL);

  del_el = first_el;

  if ( is_last (del_el) ) {
    first_el = last_el = curr_el = NULL;
    no_elem = 0;
  } else {
    first_el = first_el->show_next();
    first_el->set_prev(NULL);
    del_el->set_next(NULL);
    curr_el = first_el;
    no_elem--;
  }

  return (del_el);
}

Chained_Element *Chained_List::del_tail()
{
  Chained_Element *del_el;

  if (access == LIST_RO) 
    return (NULL);

  if ( is_empty() )
    return (NULL);

  del_el = last_el;

  if ( is_first (del_el) ) {
    first_el = last_el = curr_el = NULL;
    no_elem = 0;
  } else {
    last_el = last_el->show_prev();
    last_el->set_next(NULL);
    del_el->set_prev(NULL);
    curr_el = last_el;
    no_elem--;
  }

  return (del_el);
}


Chained_Element *Chained_List::del_elem()
{
  Chained_Element *del_el;
  Chained_Element *aux_el_prev, *aux_el_next;

  if (access == LIST_RO) 
    return (NULL);

  if ( is_empty() )
    return (NULL);

  del_el = curr_el;

  if ( is_first(del_el) )
    return ( del_head() );

  if ( is_last(del_el) )
    return ( del_tail() );

  aux_el_prev = del_el->show_prev();
  aux_el_next = del_el->show_next();

  aux_el_prev->set_next(aux_el_next);
  aux_el_next->set_prev(aux_el_prev);

  del_el->set_next(NULL);
  del_el->set_prev(NULL);

  curr_el = aux_el_next;
  no_elem--;

  return (del_el);
}

int Chained_List::replace_elem(Chained_Element *new_el)
{
  if (access == LIST_RO) 
    return (-1);

  if (new_el == NULL)
    return(-1);

  if (is_first(curr_el)){
    del_head();
    add_head(new_el);
    return(1);
  }

  if (is_last(curr_el)){
    del_tail();
    add_tail(new_el);
    return(1);
  }
    
  del_elem();
  add_prev(new_el);
  return(1);
}
