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
*	@file event_msg_desc.cpp
*   @brief <b> Message events </b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram-II project
*   @since version 1.0
*
*   The detailed description is unavailable. 
*/

#include <iostream>

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_desc.h"
#include "system_state.h"
#include "random.h"
#include "distribution.h"


/** The global system description */
extern System_Description *the_system_desc;

//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
Event::Event() : Chained_Element()
{
   action_list = new Action_List();
   indep_chain_list = new Indep_Chain_List();
   strcpy (name,"");
   condition = NULL;
   distrib = NULL;
   det_id = 0;
   det_ev_rate = new Expr_Val(0.0);
   debug(4,"Event::Event(): creating object");
}
//------------------------------------------------------------------------------
Event::Event(char *a_name) : Chained_Element()
{
   action_list = new Action_List();
   indep_chain_list = new Indep_Chain_List();
   strcpy (name, a_name);
   condition = NULL;
   distrib = NULL;
   det_id = 0;
   det_ev_rate = new Expr_Val(0.0);
   debug(4,"Event::Event(char *): creating object");
}
//------------------------------------------------------------------------------
Event::~Event()
{
   /* delete the action list */
   delete action_list;

   /* delete the independent chain list */
   delete indep_chain_list;

   if (condition != NULL)
     delete condition;
   if (distrib != NULL)
     delete distrib;

   if (det_ev_rate != NULL)
     delete det_ev_rate;

   debug(4,"Event::~Event(): destroying object");
}
//------------------------------------------------------------------------------
//     M E T H O D S
//------------------------------------------------------------------------------
void Event::ch_name(char *ev_name)
{
  strcpy (name, ev_name);
}
//------------------------------------------------------------------------------
void Event::show_name(char *ev_name)
{
  strcpy (ev_name, name);
}
//------------------------------------------------------------------------------
void Event::ch_det_id(int a_id)
{
  det_id = a_id;
}
//------------------------------------------------------------------------------
int Event::show_det_id()
{
  return(det_id);
}
//------------------------------------------------------------------------------
Expr_Val *Event::eval_rate(Object_State *obj_st)
{
  Expression *rate;

  if (distrib->show_type() == EXPONENTIAL_DIST) {
    rate = ((Exponential_Distrib *)distrib)->show_rate();
    return (rate->evaluate(obj_st));
  }
  else
    if (the_system_desc->show_det_model())
      if (distrib->show_type() == DETERMINISTIC_DIST)
      {
        rate = ((Deterministic_Distrib *)distrib)->show_rate();
        return (rate->evaluate(obj_st));
      }

  debug(3, "Event::eval_rate: Trying to evaluate an non exponential distribution");
  return (NULL);
}
//------------------------------------------------------------------------------
void Event::ch_cond(Expression *new_cond)
{
  if (condition != NULL) {
    /* delete the existing condition */
    delete condition;
    condition = new_cond;
  } else
    condition = new_cond;
}
//------------------------------------------------------------------------------
int Event::eval_cond( Object_State *obj_st )
{
    Expr_Val           *cond_val;
    int                 val;

    cond_val = condition->evaluate( obj_st );
    val = (int) cond_val->show_value();

    delete cond_val;

    return( val );
}
//------------------------------------------------------------------------------
void Event::add_action(Action *new_action)
{
 if (new_action != NULL)
   action_list->add_tail_act(new_action);
 else
   debug(3, "Event::add_action: Can't add NULL pointer");
}
//------------------------------------------------------------------------------
Action_List *Event::show_action_list(int access)
{
  Action_List *act_list_aux;

  if (access == LIST_RW)
    return (action_list);
  else {
    act_list_aux = new Action_List(action_list);
    return (act_list_aux);
  }
}
//------------------------------------------------------------------------------
void Event::add_indep_chain(Indep_Chain *new_indep_chain)
{
 if (new_indep_chain != NULL)
   indep_chain_list->add_tail_indep_chain(new_indep_chain);
 else
   debug(3, "Event::add_indep_chain: Can't add NULL pointer");
}
//------------------------------------------------------------------------------
Indep_Chain_List *Event::show_indep_chain_list(int access)
{
  Indep_Chain_List *indep_chain_list_aux;

  if (access == LIST_RW)
    return (indep_chain_list);
  else {
    indep_chain_list_aux = new Indep_Chain_List(indep_chain_list);
    return (indep_chain_list_aux);
  }
}
//------------------------------------------------------------------------------
int Event::check_indep_chains(Object_Description *obj1, Object_Description *obj2)
{
   Indep_Chain *indep_chain;
   int obj_id1, obj_id2;
   int status;

   obj_id1 = obj1->show_id();
   obj_id2 = obj2->show_id();

   indep_chain = indep_chain_list->show_1st_indep_chain();
   while (indep_chain != NULL) {
     status = indep_chain->query_obj_id(obj_id1, obj_id2);
     if (status == 2)
       return (TRUE);
     if (status == 1)
       return (FALSE);
     indep_chain = indep_chain_list->show_next_indep_chain();
   }
   return (FALSE);
}
//------------------------------------------------------------------------------
void Event::add_st_to_indep_chains(System_State *sys_st, System_State_List *sys_st_list)
{
  Indep_Chain *indep_chain;

  indep_chain = indep_chain_list->show_1st_indep_chain();
  while (indep_chain != NULL) {
    indep_chain->add_states(sys_st, sys_st_list);

    indep_chain = indep_chain_list->show_next_indep_chain();
  }
}
//------------------------------------------------------------------------------
int Event::show_absorbing_indep_chain()
{
  Indep_Chain *indep_chain;
  int count = 1;

  indep_chain = indep_chain_list->show_1st_indep_chain();
  while (indep_chain != NULL) {
    if (indep_chain->is_absorbing())
      return(count);
    count++;
    indep_chain = indep_chain_list->show_next_indep_chain();
  }
  return(0);
}
//------------------------------------------------------------------------------
Expr_Val *Event::show_det_ev_rate()
{
  return (det_ev_rate);
}
//------------------------------------------------------------------------------
void Event::ch_det_ev_rate(Expr_Val *new_rate)
{
  Object_Description *obj_desc;
  char                obj_name[MAXNAME];

  /* if det_ev_rate has already been initialized */
  if (det_ev_rate->show_value() != 0) {
    /* if DET ev assumes different rates */
    if (det_ev_rate->show_value() != new_rate->show_value()){
      /* get the name of the object */
      obj_desc = the_system_desc->show_obj_desc_by_det_ev_id(det_id);
      if (obj_desc != NULL)
	obj_desc->show_name(obj_name);
      else
	strcpy (obj_name, "Unknown");
      /* print error message */
      fprintf ( stderr , "ERROR: DET event %s in object %s assumes different rate values\n", name, obj_name);
      /* Aborting */
      exit(-1);
    }
  } else
    det_ev_rate->ch_value(new_rate->show_value());
}
//------------------------------------------------------------------------------
int Event::show_no_indep_chains()
{
  return (indep_chain_list->show_no_indep_chain());
}
//------------------------------------------------------------------------------
int Event::show_indep_chain_num(int obj_id)
{
  Indep_Chain *indep_chain;
  int count = 0;

  indep_chain = indep_chain_list->show_1st_indep_chain();
  while (indep_chain != NULL) {
    if (indep_chain->query_obj_id(obj_id))
      return (count);
    count++;
    indep_chain = indep_chain_list->show_next_indep_chain();
  }
  return (-1);
}
//------------------------------------------------------------------------------
void Event::set_zero()
{
  Indep_Chain *indep_chain;
  int i;

  for (i=0; i<indep_chain_list->show_no_indep_chain(); i++) {
    indep_chain = indep_chain_list->get_1st_indep_chain();
    if (indep_chain->is_absorbing()) {
      indep_chain_list->add_head_indep_chain(indep_chain);
      break;
    } else
      indep_chain_list->add_tail_indep_chain(indep_chain);
  }
}
//------------------------------------------------------------------------------
void Event::ch_distrib(Distribution *new_dist)
{
  if (distrib != NULL) {
    /* delete the existing distribution */
    delete distrib;
    distrib = new_dist;
  } else
    distrib = new_dist;
}
//------------------------------------------------------------------------------
int Event::show_distrib_type()
{
  return (distrib->show_type());
}
//------------------------------------------------------------------------------
Distribution *Event::show_distrib()
{
    return(distrib);
}
//------------------------------------------------------------------------------
TGFLOAT Event::generate_sample(Object_State *obj_st)
{
  return (distrib->next_sample(obj_st));
}
//------------------------------------------------------------------------------
void Event::reset_distrib(int run)
{
  distrib->reset(run);
}
//------------------------------------------------------------------------------
void Event::print_event()
{
    Action           *act_aux;

    fprintf ( stdout , "*** Event ***\n");
    fprintf ( stdout , "Event name: %s\n", name);
    fprintf ( stdout , "Distribution: ");
    distrib->print_dist();
    fprintf ( stdout , "Cond: ");
    condition->print_expression();

    fprintf ( stdout , "*** Action\n");
    act_aux = action_list->show_1st_act();
    while (act_aux != NULL) {
      act_aux->print_action();
      act_aux = action_list->show_next_act();
    }
}

/*************************************************************/

Event_List::Event_List() : Chained_List()
{
   debug(4,"Event_List::Event_List(): creating object");
}

Event_List::Event_List(Event_List *ev_list) : Chained_List( (Chained_List *)ev_list)
{

   debug(4,"Event_List::Event_List(Event_List *): creating object");
}

Event_List::~Event_List()
{
  Event *ev_aux;

  if (access == LIST_RW) {
    ev_aux = get_1st_event();
    while (ev_aux != NULL) {
      delete ev_aux;
      ev_aux = get_1st_event();
    }
  }

   debug(4,"Event_List::~Event_List(): destroying object");
}

int Event_List::add_tail_event(Event *new_event)
{
  return ( add_tail( (Chained_Element *)new_event) );
}

Event *Event_List::show_1st_event()
{
  return ( (Event *)show_head() );
}

Event *Event_List::get_1st_event()
{
  return ( (Event *)del_head() );
}

Event *Event_List::show_next_event()
{
  return ( (Event *)show_next() );
}

Event *Event_List::show_curr_event()
{
  return ( (Event *)show_curr() );
}

int Event_List::query_event(char *tg_ev_name)
{
    Event  *ev_aux;
    char    ev_name[MAXSTRING];

    ev_aux = show_1st_event();

    while (ev_aux != NULL) {
      ev_aux->show_name(ev_name);
      if (!strcmp(ev_name, tg_ev_name) )
	return (1);
      ev_aux = show_next_event();
    }

    return (0);
}

int Event_List::show_no_event()
{
  return ( show_no_elem() );
}


/*************************************************************/

/* The message defines the behavior of an object that interact with
another one. A message may produce another message and/or a state 
change. A message may be accepted by only pre-specified objects (not 
implemented yet). */

Message::Message() : Chained_Element()
{
   action_list = new Action_List();
   strcpy (port_name, "");

   debug(4,"Message::Message(): creating object");
}

Message::Message(char *a_port_name) : Chained_Element()
{
   action_list = new Action_List();
   strcpy (port_name, a_port_name);

   debug(4,"Message::Message(char *): creating object");
}

Message::~Message()
{
  /* delete the action list */
   delete action_list;

   debug(4,"Message::~Message(): destroying object");
}

void Message::ch_port_name(char *name) 
{
  strcpy (port_name, name);
}

void Message::show_port_name(char *name) 
{
  strcpy (name, port_name);
}

void Message::add_action(Action *new_action)
{ 
  if (new_action != NULL) 
    action_list->add_tail_act(new_action);
  else
    debug(3, "Message::add_action: Can't add NULL pointer");
}

Action_List *Message::show_action_list(int access)
{
  Action_List *act_list_aux;

  if (access == LIST_RW)
    return (action_list);
  else {
    act_list_aux = new Action_List(action_list);
    return (act_list_aux);
  }
}

void Message::print_message()
{
    Action     *act_aux;

    fprintf ( stdout , "*** Message ***\n");
    fprintf ( stdout , "port name: %s\n", port_name);

    fprintf ( stdout , "*** Action\n");
    act_aux = action_list->show_1st_act();
    while (act_aux != NULL) {
      act_aux->print_action();
      act_aux = action_list->show_next_act();
    }
}

/***********************************************************************/

Message_List::Message_List() : Chained_List()
{

   debug(4,"Message_List::Message_List(): creating object");
}

Message_List::Message_List(Message_List *msg_list) : Chained_List( (Chained_List *)msg_list)
{

   debug(4,"Message_List::Message_List(Message_List *): creating object");
}

Message_List::~Message_List()
{
  Message *msg_aux;

  if (access == LIST_RW) {
    msg_aux = get_1st_msg();
    while (msg_aux != NULL) {
      delete msg_aux;
      msg_aux = get_1st_msg();
    }
  }

  debug(4,"Message_List::~Message_List(): destroying object");
}

int Message_List::add_tail_msg(Message *new_msg)
{
  return ( add_tail( (Chained_Element *)new_msg) );
}

Message *Message_List::show_1st_msg()
{
  return ( (Message *)show_head() );
}

Message *Message_List::get_1st_msg()
{
  return ( (Message *)del_head() );
}

Message *Message_List::show_next_msg()
{
  return ( (Message *)show_next() );
}

Message *Message_List::show_curr_msg()
{
  return ( (Message *)show_curr() );
}

/*
int Message_List::query_msg(char *tg_src_name, char *tg_port_name) 
{ 
    Message *msg_aux;
    char     src_name[MAXSTRING];
    char     port_name[MAXSTRING];

    msg_aux = show_1st_msg();
    
    while (msg_aux != NULL) {
      msg_aux->show_src_obj(src_name);
      msg_aux->show_port_name(port_name);
      if ( !strcmp(src_name, tg_src_name) && !strcmp(port_name, tg_port_name) )
	return (1);
      msg_aux = show_next_msg();
    }
    return (0);
}
*/

int Message_List::query_msg(char *tg_port_name)
{
    Message *msg_aux;
    char     port_name[MAXSTRING];

    msg_aux = show_1st_msg();
    
    while (msg_aux != NULL) {
      msg_aux->show_port_name(port_name);
      if ( !strcmp(port_name, tg_port_name) )
	return (1);
      msg_aux = show_next_msg();
    }
    return (0);
}

int Message_List::show_no_msg()
{
  return ( show_no_elem() );
}

