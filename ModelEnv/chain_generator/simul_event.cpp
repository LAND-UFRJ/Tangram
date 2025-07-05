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
*  @file simul_event.cpp
*  @brief <b>Simulator_Event, Simulator_Event_List, Event_Sample, and 
*  Event_Sample_List methods</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  The detailed description is unavailable. 
*/

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
  #include <limits.h>
  #include <float.h>
  #ifndef MAXDOUBLE
    #define MAXDOUBLE DBL_MAX
  #endif
#else
// The values.h is deprecated.
#include <values.h>
#endif

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"
#include "system_desc.h"
#include "random.h"
#include "simulator.h"
#include "distribution.h"

/** Global simulator_controler */
extern Simulator_Controler *the_simulator_controler;


Simulator_Event::Simulator_Event(Event *a_event, Object_Description *a_obj_desc)
{
    ev_sample_list = new Event_Sample_List();

    enable      = FALSE;
    no_triggers = 0;
    event = a_event;
    obj_desc = a_obj_desc;

    debug(4,"Simulator_Event::Simulator_Event(Event *, Object_Description *): creating object");
}

Simulator_Event::Simulator_Event(Simulator_Event *simul_ev)
{
    Event_Sample_List  *ev_sample_list_aux;
    Event_Sample       *ev_sample, *ev_sample_aux;

    /* copy the attributes */
    enable      = simul_ev->show_enable();
    no_triggers = simul_ev->show_no_triggers();
    event       = simul_ev->show_event();
    obj_desc    = simul_ev->show_obj_desc();

    /* copy the event sample list */
    ev_sample_list = new Event_Sample_List();
    ev_sample_list_aux = simul_ev->show_ev_sample_list(LIST_RO);
    ev_sample_aux = ev_sample_list_aux->show_1st_sample();
    while (ev_sample_aux != NULL) {
      ev_sample = new Event_Sample(ev_sample_aux);
      ev_sample_list->add_order_sample(ev_sample);
      ev_sample_aux = ev_sample_list_aux->show_next_sample();
    }
    delete ev_sample_list_aux;
    
    debug(4,"Simulator_Event::Simulator_Event(Simulator_Event *): creating object");
}

Simulator_Event::~Simulator_Event()
{
    /* delete the sample list */
    delete ev_sample_list;

    debug(4,"Simulator_Event::~Simulator_Event(): destroying object");
}

Event_Sample_List *Simulator_Event::show_ev_sample_list(int access)
{
  Event_Sample_List *ev_sample_list_aux;

  if (access == LIST_RW)
    return (ev_sample_list);
  else {
    ev_sample_list_aux = new Event_Sample_List (ev_sample_list);
    return (ev_sample_list_aux);
  }
}

Event_Sample *Simulator_Event::show_1st_ev_sample()
{
   return (ev_sample_list->show_1st_sample());
}

Event_Sample *Simulator_Event::get_1st_ev_sample()
{
   return (ev_sample_list->get_1st_sample());
}

void Simulator_Event::clear_ev_sample_list()
{
  Event_Sample *ev_aux;

  /* delete all entries of the sample list */
  ev_aux = ev_sample_list->get_1st_sample();
  while (ev_aux != NULL) {
    delete ev_aux;
    ev_aux = get_1st_ev_sample();
  }
}

int Simulator_Event::show_no_samples()
{
  return(ev_sample_list->show_no_sample());
}

int Simulator_Event::show_enable()
{
  return (enable);
}

void Simulator_Event::ch_enable(int new_enable)
{
  enable = new_enable;
}

Event *Simulator_Event::show_event()
{
  return (event);
}

Object_Description *Simulator_Event::show_obj_desc()
{
  return (obj_desc);
}
//------------------------------------------------------------------------------
int Simulator_Event::eval_cond(Object_State *obj_st)
{
  return ( event->eval_cond(obj_st) );
}
//------------------------------------------------------------------------------
void Simulator_Event::generate_sample( Object_State *obj_st,
                                       TGFLOAT       simul_time )
{
    Event_Sample *ev_sample;
    TGFLOAT       time;

    /* generate the time for the next event */
    time      = simul_time + event->generate_sample( obj_st );
    ev_sample = new Event_Sample( time );

    /* add the sample to the list */
    ev_sample_list->add_order_sample( ev_sample );
}
//------------------------------------------------------------------------------
int Simulator_Event::show_distrib_type()
{
   return (event->show_distrib_type());
}

void Simulator_Event::inc_no_triggers()
{
  no_triggers += 1;
}

void Simulator_Event::clear_no_triggers()
{
  no_triggers = 0;
}

int Simulator_Event::show_no_triggers()
{
  return (no_triggers);
}

void Simulator_Event::print_simul_ev()
{
  char ev_name[MAXSTRING];

  event->show_name(ev_name);
  fprintf( stdout, "%s - %d\n", ev_name, show_enable());
}

void Simulator_Event::print_ev_sample_list()
{
   Event_Sample *ev_sample;

   ev_sample = ev_sample_list->show_1st_sample();
   while (ev_sample != NULL) {
     ev_sample->print_sample();
     ev_sample = ev_sample_list->show_next_sample();
   }
}

/******************************************************************************/


Simulator_Event_List::Simulator_Event_List() : Chained_List()
{
  debug(4,"Simulator_Event_List::Simulator_Event_List(): creating object");
}

Simulator_Event_List::Simulator_Event_List(Simulator_Event_List *ev_list) : Chained_List( (Chained_List *)ev_list)
{
  debug(4,"Simulator_Event_List::Simulator_Event_List(Simulator_Event_List *): creating object");
}

Simulator_Event_List::~Simulator_Event_List()
{
  Simulator_Event *ev_aux;

  if (access == LIST_RW) {
    ev_aux = get_1st_event();
    while (ev_aux != NULL) {
      delete ev_aux;
      ev_aux = get_1st_event();
    }
  }

  debug(4,"Simulator_Event_List::~Simulator_Event_List(): destroying object");
}

int Simulator_Event_List::add_tail_event(Simulator_Event *new_event)
{
  return ( add_tail( (Chained_Element *)new_event) );
}

Simulator_Event *Simulator_Event_List::show_1st_event()
{
  return ( (Simulator_Event *)show_head() );
}

Simulator_Event *Simulator_Event_List::get_1st_event()
{
  return ( (Simulator_Event *)del_head() );
}

Simulator_Event *Simulator_Event_List::show_next_event()
{
  return ( (Simulator_Event *)show_next() );
}

Simulator_Event *Simulator_Event_List::show_next_event(Simulator_Event *simul_ev)
{
  return ( (Simulator_Event *)show_next((Chained_Element *) simul_ev) );
}

Simulator_Event *Simulator_Event_List::show_curr_event()
{
  return ( (Simulator_Event *)show_curr() );
}

Simulator_Event *Simulator_Event_List::show_smallest_time()
{
    Simulator_Event *simul_ev;
    Simulator_Event *smallest_simul_ev;
    Event_Sample    *ev_sample;
    TGFLOAT          min_time;

    min_time          = MAXDOUBLE;
    smallest_simul_ev = NULL;
    simul_ev          = show_1st_event();
    
    while( simul_ev != NULL )
    {
        if( simul_ev->show_enable() == TRUE )
        {
            ev_sample = simul_ev->show_1st_ev_sample();

            if( ev_sample != NULL )
            {
                if( ev_sample->show_time() <= min_time )
                {
                    smallest_simul_ev = simul_ev;
                    min_time          = ev_sample->show_time();
                    
                    // Force INIT event to run before all others
                    if( simul_ev->show_event()->show_distrib()->show_type() == INIT_DIST )
                        break;
                }
            }
            else
                debug( 3,"Simulator_Event_List::show_smallest_time: Warning: Event was enabled but had no sample generated!" );
        }
        simul_ev = show_next_event();
    }
    
    return smallest_simul_ev;
}

int Simulator_Event_List::show_no_event()
{
  return show_no_elem();
}

/******************************************************************/

Event_Sample::Event_Sample(TGFLOAT a_time)
{
  time = a_time;

  debug(4,"Event_Sample::Event_Sample(TGFLOAT): creating object");
}

Event_Sample::Event_Sample(Event_Sample *ev_sample)
{
  time = ev_sample->show_time();

  debug(4,"Event_Sample::Event_Sample(Event_Sample *): creating object");
}

Event_Sample::~Event_Sample()
{
  debug(4,"Event_Sample::~Event_Sample(TGFLOAT): destroying object");
}

TGFLOAT Event_Sample::show_time()
{
  return (time);
}

void Event_Sample::ch_time(TGFLOAT new_time)
{
  time = new_time;
}

void Event_Sample::print_sample()
{
  fprintf( stdout, "sample time: %.8E\n", time);
}

/*****************************************************************/

Event_Sample_List::Event_Sample_List() : Chained_List()
{
  debug(4,"Event_Sample_List::Event_Sample_List(): creating object");
}

Event_Sample_List::Event_Sample_List(Event_Sample_List *ev_list) : Chained_List( (Chained_List *)ev_list)
{
  debug(4,"Event_Sample_List::Event_Sample_List(Event_Sample *): creating object");
}

Event_Sample_List::~Event_Sample_List()
{
  Event_Sample *ev_aux;

  if (access == LIST_RW) {
    ev_aux = get_1st_sample();
    while (ev_aux != NULL) {
      delete ev_aux;
      ev_aux = get_1st_sample();
    }
  }
  debug(4,"Event_Sample_List::~Event_Sample_List(): destroying object");
}

int Event_Sample_List::add_order_sample( Event_Sample *new_sample )
{
    Event_Sample *ev_sample;
    TGFLOAT       new_time;
   
    if( new_sample == NULL )
    {
        debug( 3, "Event_Sample_List::add_order_sample: ERROR: Trying to add a NULL pointer." );
        return -1;
    }

    new_time  = new_sample->show_time();
    ev_sample = show_1st_sample();
    while( ev_sample != NULL )
    {
        if( ev_sample->show_time() > new_time )
            break;
        ev_sample = show_next_sample();
    }
    
    if( ev_sample == NULL )
        add_tail( (Chained_Element *)new_sample );
    else
        add_prev( (Chained_Element *)new_sample );

    return 1;
}

Event_Sample *Event_Sample_List::show_1st_sample()
{
  return ( (Event_Sample *)show_head() );
}

Event_Sample *Event_Sample_List::get_1st_sample()
{
  return ( (Event_Sample *)del_head() );
}

Event_Sample *Event_Sample_List::show_next_sample()
{
  return ( (Event_Sample *)show_next() );
}

Event_Sample *Event_Sample_List::show_curr_sample()
{
  return ( (Event_Sample *)show_curr() );
}

int Event_Sample_List::show_no_sample()
{
  return ( show_no_elem() );
}
