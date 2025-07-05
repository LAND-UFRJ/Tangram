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

/**
*	@file clone_ev.cpp
*   @brief <b> Clone event methods </b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram-II project
*   @since version 1.0
*
*   The detailed description is unavailable. 
*/

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
  #include <limits.h>
  #include <float.h>
  #include <math.h>
#else
// The values.h is deprecated.
#include <values.h>
#endif

#include <string.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "random.h"
#include "simulator.h"


/* Global Debug Level */
extern int Debug_Level;

Clone_Event::Clone_Event(char *a_name)
{
    strcpy (event_name, a_name);
    no_clones = 1;

    debug(4,"Clone_Event::Clone_Event(char *): creating object");
}

Clone_Event::Clone_Event(Clone_Event *clone)
{
    char ev_name[MAXSTRING];

    clone->show_name(ev_name);
    strcpy (event_name, ev_name);
    no_clones = clone->show_no_clones();

    debug(4,"Clone_Event::Clone_Event(Clone_Event *): creating object");
}

Clone_Event::~Clone_Event()
{
  /* do nothing */
  debug(4,"Clone_Event::~Clone_Event(): destroying object");
}

void Clone_Event::show_name(char *a_name)
{
  strcpy (a_name, event_name);
}

void Clone_Event::ch_name(char *new_name)
{
  strcpy (event_name, new_name);
}

int Clone_Event::show_no_clones()
{
  return(no_clones);
}

void Clone_Event::ch_no_clones(int new_clones)
{
   no_clones = new_clones;
}

void Clone_Event::inc_no_clones()
{
   no_clones++;
}

void Clone_Event::print_clone()
{
  fprintf( stdout , "Event: %s --- No. of clones: %d", event_name, no_clones);
}

/*****************************************************************/

Clone_Event_List::Clone_Event_List() : Chained_List()
{
  debug(4,"Clone_Event_List::Clone_Event_List(): creating object");
}

Clone_Event_List::Clone_Event_List(Clone_Event_List *ev_list) : Chained_List( (Chained_List *)ev_list)
{
  debug(4,"Clone_Event_List::Clone_Event_List(Clone_Event_List *): creating object");
}

Clone_Event_List::~Clone_Event_List()
{
  Clone_Event *clone_aux;

  if (access == LIST_RW) {
    clone_aux = get_1st_clone();
    while (clone_aux != NULL) {
      delete clone_aux;
      clone_aux = get_1st_clone();
    }
  }

  debug(4,"Clone_Event_List::~Clone_Event_List(): destroying object");
}

int Clone_Event_List::add_order_clone(char *new_ev_name)
{
  Clone_Event  *clone_ev;
   
  if (new_ev_name == NULL) {
    debug (3, "Clone_Event_List::add_order_clone: ERROR: Trying to add a NULL pointer.");
    return (-1);
  }
  /* if the event already exists then increment the number of clones */
  if ( query_clone(new_ev_name) ) {
    clone_ev = show_curr_clone();
    clone_ev->inc_no_clones();
  } else {
    /* else create a new clone entry for this event */
    clone_ev = new Clone_Event (new_ev_name);
    add_tail_clone(clone_ev);
  }
  return (1);
}

int Clone_Event_List::add_tail_clone(Clone_Event *new_clone)
{
  return ( add_tail( (Chained_Element *)new_clone) );
}

Clone_Event *Clone_Event_List::show_1st_clone()
{
  return ( (Clone_Event *)show_head() );
}

Clone_Event *Clone_Event_List::get_1st_clone()
{
  return ( (Clone_Event *)del_head() );
}

Clone_Event *Clone_Event_List::show_next_clone()
{
  return ( (Clone_Event *)show_next() );
}

Clone_Event *Clone_Event_List::show_curr_clone()
{
  return ( (Clone_Event *)show_curr() );
}

int Clone_Event_List::query_clone (char *tg_ev_name)
{
  Clone_Event *clone_ev;
  char         clone_ev_name[MAXSTRING];

  clone_ev = show_1st_clone();
  while (clone_ev != NULL){
    clone_ev->show_name(clone_ev_name);
    if (!strcmp(clone_ev_name, tg_ev_name))
      break;
    else
      clone_ev = show_next_clone();
  }

  if (clone_ev == NULL)
    return(0);
  else
    return(1);
}

int Clone_Event_List::show_no_clone()
{
  return ( show_no_elem() );
}


