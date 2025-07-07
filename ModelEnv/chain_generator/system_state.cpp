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
*	@file system_state.cpp
*   @brief <b> System_State methods </b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @version 1.0
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram II project
*   @since version 1.0
*
*/

#include <iostream>

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"
#include "system_desc.h"
#include "hashlib.h"
#include "distribution.h"
#include "simulator.h"

/** the global system description */
extern System_Description *the_system_desc;
/** the global hash table */
extern Hash               *hashtb;
/** the debug level variable */
extern int Debug_Level;
/** the execution context */
extern t_context context;

#ifdef _MEM_DEBUG
extern int DBG_System_State_counter;
extern int DBG_System_State_memory;
extern int DBG_System_State_List_counter;
extern int DBG_System_State_List_memory;
#endif

//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
System_State::System_State() : Chained_Element()
{
   obj_st_list = new Object_State_List();
   trans_list = new Transition_List();
   pend_msg_list = new Pending_Message_List();
   rate_rew_list = NULL;

#ifdef _MEM_DEBUG
   DBG_id = DBG_System_State_counter;
   DBG_System_State_counter++;
   DBG_System_State_memory++;
   debug_mem(DBG_id,"System_State::System_State(): creating object");
#endif
}
//------------------------------------------------------------------------------
System_State::System_State(System_State *sys_st) : Chained_Element()
{
   Object_State_List     *obj_st_list_aux;
   Object_State          *obj_st, *new_obj_st;
   Pending_Message_List  *pend_msg_list_aux;
   Pending_Message       *pend_msg, *new_pend_msg;

   obj_st_list = new Object_State_List();
   trans_list = new Transition_List();
   pend_msg_list = new Pending_Message_List();
   rate_rew_list = NULL;

   /* copy all objects */
   obj_st_list_aux = sys_st->show_obj_st_list(LIST_RO);
   obj_st = obj_st_list_aux->show_1st_obj_st();

   while (obj_st != NULL) {
     new_obj_st = new Object_State(obj_st);
     add_obj_st(new_obj_st);
     obj_st = obj_st_list_aux->show_next_obj_st();
   }
   delete obj_st_list_aux;

   /* copy all pending messages */
   pend_msg_list_aux = sys_st->show_pend_msg_list(LIST_RO);
   pend_msg = pend_msg_list_aux->show_1st_pend_msg();

   while (pend_msg != NULL)
   {
        new_pend_msg = new Pending_Message(pend_msg);
        add_pend_msg(new_pend_msg);
        pend_msg = pend_msg_list_aux->show_next_pend_msg();
   }
   delete pend_msg_list_aux;

#ifdef _MEM_DEBUG
   DBG_id = DBG_System_State_counter;
   DBG_System_State_counter++;
   DBG_System_State_memory++;
   debug(DBG_id,"System_State::System_State(): creating object");
#endif
}
//------------------------------------------------------------------------------
System_State::~System_State()
{
  /* delete all objects states */
  delete obj_st_list;

  /* delete all pending messages */
  delete pend_msg_list;

  /* delete all transitions */
  delete trans_list;

  /* delete the rate reward list */
  if (rate_rew_list != NULL)
    delete rate_rew_list;

#ifdef _MEM_DEBUG
   DBG_System_State_memory--;
   debug_mem(DBG_id,"System_State::~System_State(): destroying object");
#endif
}
//------------------------------------------------------------------------------
//     M E T H O D S
//------------------------------------------------------------------------------
/* Small routines. These are routines to change System State class'
attributes. */
void System_State::print_sys_st()
{
   Object_State *obj_st_aux;

   obj_st_aux = obj_st_list->show_1st_obj_st();
   cout << "{ ";
   while (obj_st_aux != NULL) {
      obj_st_aux->print_obj_st();
      obj_st_aux = obj_st_list->show_next_obj_st();
   }
   cout << " }\n";
}

void System_State::print_sys_st(FILE *file_out)
{
   Object_State *obj_st_aux;

   obj_st_aux = obj_st_list->show_1st_obj_st();

   fprintf(file_out,"(");
   while (obj_st_aux != NULL) {
      obj_st_aux->print_obj_st(file_out);
      obj_st_aux = obj_st_list->show_next_obj_st();
      if (obj_st_aux != NULL)
	fprintf (file_out,",");
   }
   fprintf(file_out,")\n");

}

void System_State::print_van_sys_st()
{
   Pending_Message *pend_msg_aux;

   print_sys_st();

   pend_msg_aux = pend_msg_list->show_1st_pend_msg();
   while (pend_msg_aux != NULL) {
      fprintf ( stdout, "-> ");
      pend_msg_aux->print_pend_msg();
      pend_msg_aux = pend_msg_list->show_next_pend_msg();
   }
}

void System_State::print_sys_st_trans()
{
   Transition *trans_aux;

   print_sys_st();

   trans_aux = trans_list->show_1st_trans();
   while (trans_aux != NULL) {
      fprintf ( stdout, "-> ");
      trans_aux->print_trans();
      trans_aux = trans_list->show_next_trans(); 
   } 
}


/* Routines for manipulation of system states. Dealing with Object
States, Transitions and so on. */

void System_State::add_obj_st(Object_State *new_obj_st)
{
 if (new_obj_st != NULL) 
   obj_st_list->add_tail_obj_st(new_obj_st);
 else
   debug(3,"System_State::add_obj_st: Can't add NULL pointer");
}

Object_State_List *System_State::show_obj_st_list(int access)
{ 
  Object_State_List *obj_st_list_aux;

  if (access == LIST_RW)
    return (obj_st_list);
  else {
    obj_st_list_aux = new Object_State_List(obj_st_list);
    return (obj_st_list_aux);
  }
}

Object_State *System_State::show_obj_st(Object_Description *obj_desc)
{
    int obj_id;

    obj_id = obj_desc->show_id();
    if ( obj_st_list->query_obj_st(obj_id) )
      return ( obj_st_list->show_curr_obj_st() );
    else
      return (NULL);
}

Object_State *System_State::show_obj_st(int obj_id)
{
    if ( obj_st_list->query_obj_st(obj_id) )
      return ( obj_st_list->show_curr_obj_st() );
    else
      return (NULL);
}

void System_State::add_transition(Transition *new_trans)
{
  if (new_trans != NULL) 
    trans_list->add_tail_trans(new_trans);
  else
    debug (3, "System_State::add_transition: Can't add NULL pointer");
}

Transition_List *System_State::show_trans_list(int access)
{
  Transition_List *trans_list_aux;

  if (access == LIST_RW)
    return (trans_list);
  else {
    trans_list_aux = new Transition_List(trans_list);
    return (trans_list_aux);
  }
}

Transition *System_State::show_1st_trans()
{
  return (trans_list->show_1st_trans());
}

Transition *System_State::get_1st_trans()
{
  return (trans_list->get_1st_trans());
}

void System_State::add_pend_msg(Pending_Message *new_pend_msg)
{
  pend_msg_list->add_tail_pend_msg(new_pend_msg);
}

Pending_Message_List *System_State::show_pend_msg_list(int access)
{
  Pending_Message_List *pend_msg_list_aux;

  if (access == LIST_RW)
    return (pend_msg_list);
  else {
    pend_msg_list_aux = new Pending_Message_List(pend_msg_list);
    return (pend_msg_list_aux);
  }
}

int System_State::show_no_pend_msg()
{
  return( pend_msg_list->show_no_pend_msg() );
}

int System_State::set_state_var(unsigned int *state_vec)
{
    Object_State        *obj_st;
    State_Variable_List *st_var_list;
    State_Variable      *st_var;
    int                  i, index = 1;

    obj_st = obj_st_list->show_1st_obj_st();
    while (obj_st != NULL)
    {
      st_var_list = obj_st->show_st_var_list(LIST_RO);
      st_var = st_var_list->show_1st_st_var();
      while (st_var != NULL)
      {
            for (i=0; i<st_var->show_dimension(); i++)
            {
                st_var->ch_value(i, state_vec[index]);
                index++;
        	}
            st_var = st_var_list->show_next_st_var();
        }
        obj_st = obj_st_list->show_next_obj_st();
        delete st_var_list;
    }

    return (1);
}

int System_State::replace_obj_st(Object_State *new_obj_st)
{
   return ( obj_st_list->replace_obj_st (new_obj_st) );
}

int System_State::compare_sys_st(System_State *comp_sys_st)
{
  Object_State      *obj_st;
  Object_State_List *comp_obj_st_list;
  Object_State      *comp_obj_st;

  obj_st = obj_st_list->show_1st_obj_st();

  comp_obj_st_list = comp_sys_st->show_obj_st_list(LIST_RO);
  comp_obj_st = comp_obj_st_list->show_1st_obj_st();
  while (obj_st != NULL) {
    if (!obj_st->compare_obj_st(comp_obj_st))
      break;
    else {
      obj_st = obj_st_list->show_next_obj_st();
      comp_obj_st = comp_obj_st_list->show_next_obj_st();
    }
  }
  delete comp_obj_st_list;

  if (obj_st != NULL)
    return (0);
  else
    return (1);
}


int System_State::copy_pend_msg(Object_State *obj_st)
{
    Pending_Message_List  *obj_pend_msg_list;
    Pending_Message       *pend_msg;

    obj_pend_msg_list = obj_st->show_pend_msg_list(LIST_RW);
    
    while( !obj_pend_msg_list->is_empty() ) {
        pend_msg = obj_pend_msg_list->get_1st_pend_msg();
	add_pend_msg(pend_msg);
    }
    return (1);
}

// BRUNO&KELVIN State vector has to have integer values!!!
// but this is only for MARKOV CHAIN GENERATION

void System_State::get_st_vec(unsigned int *state_vec)
{
  Object_State         *obj_st;
  State_Variable_List  *st_var_list;
  State_Variable       *st_var;
  int                  *vec;
  int                   i = 1, j;
  
  if (state_vec == NULL) {
    debug (3, "System_State::get_st_vec: Cannot get the state vector into a NULL pointer.");
    return;
  }

  obj_st = obj_st_list->show_1st_obj_st();
  while (obj_st != NULL) {
    st_var_list = obj_st->show_st_var_list(LIST_RO);
    st_var = st_var_list->show_1st_st_var();
    while (st_var != NULL) {
      vec = st_var->get_value();
      for (j=0; j<st_var->show_dimension(); j++) {
	state_vec[i] = vec[j];
	i++;
      }
      st_var = st_var_list->show_next_st_var();
    }
    delete st_var_list;
    obj_st = obj_st_list->show_next_obj_st();
  }
  state_vec[0] = i - 1;

}

void System_State::get_st_vec(unsigned int *state_vec, int *obj_ids, int no_objs)
{
  Object_State         *obj_st;
  State_Variable_List  *st_var_list;
  State_Variable       *st_var;
  int                  *vec;
  int                   i = 1, j;
  int k;

  if (state_vec == NULL) {
    debug (3, "System_State::get_st_vec: Cannot get the state vector into a NULL pointer.");
    return;
  }

  for (k=0; k<no_objs; k++) {
    obj_st = show_obj_st(obj_ids[k]);
    st_var_list = obj_st->show_st_var_list(LIST_RO);
    st_var = st_var_list->show_1st_st_var();
    while (st_var != NULL) {
      vec = st_var->get_value();
      for (j=0; j<st_var->show_dimension(); j++) {
	state_vec[i] = vec[j];
	i++;
      }
      st_var = st_var_list->show_next_st_var();
    }
    delete st_var_list;
  }
  
  state_vec[0] = i - 1;
}

void System_State::ch_st_var_val(char *objname, char *varname, int varvalue)
{
  Object_Description      *obj_desc;
  Object_State            *obj_st;
  int                      obj_id;

  obj_desc = the_system_desc->show_obj_desc( objname );
  if (obj_desc != NULL)
  {
    obj_id = obj_desc->show_id();
    if( obj_st_list->query_obj_st(obj_id))
    {
      obj_st = obj_st_list->show_curr_obj_st();
      obj_st->ch_st_var_value(varname, varvalue);
    }
  }
}

void System_State::ch_st_var_val( char *objname, char *varname, int varindex,
                                  int varvalue )
{
    Object_Description      *obj_desc;
    Object_State            *obj_st;
    int                      obj_id;
    int                      dimension;
    int                     *oldvalues;

    obj_desc = the_system_desc->show_obj_desc(objname);
    if (obj_desc != NULL)
    {
        obj_id = obj_desc->show_id();
        if( obj_st_list->query_obj_st(obj_id) )
        {
            obj_st = obj_st_list->show_curr_obj_st();
            oldvalues = obj_st->show_st_var_vec_value( varname );
            dimension = obj_st->show_st_var_dimension( varname );
            if( dimension > varindex )
            {
                oldvalues[ varindex ] = varvalue;
                obj_st->ch_st_var_value( varname, oldvalues );
            }
        }
    }

}

int *System_State::show_st_var_val(char *objname, char *varname)
{
  Object_Description      *obj_desc;
  Object_State            *obj_st;
  int                      obj_id;
  int                     *varvalue = NULL;
  
  obj_desc = the_system_desc->show_obj_desc(objname);
  if (obj_desc != NULL) {
    obj_id = obj_desc->show_id();
    if (obj_st_list->query_obj_st(obj_id)) {
      obj_st = obj_st_list->show_curr_obj_st();
      varvalue = obj_st->show_st_var_vec_value(varname);
    } else
      debug(1,"System_State::show_st_var_val: No such object in system state\n");    
  } else
    debug(1,"System_State::show_st_var_val: Object Description in NULL\n");

  return (varvalue);
}

/* The search for leaves... */
System_State_List *System_State::search_for_leaves(int *det_ev, Expr_Val **det_ev_rate)
{
  System_State_List    *van_sys_st_list;   /* list of vanishing states found*/
  System_State_List    *tang_sys_st_list;  /* list of tangible states found */
  System_State_List    *new_sys_st_list;   /* list of tangible states found */
  System_State         *new_sys_st;        /* the new system state found (vanishing or tangible) */
  System_State         *van_sys_st;        /* a vanishing system state */
  Object_State         *obj_st;            /* a object state of the current system state */
  Object_State_List    *obj_st_list_aux;   /* a object state of the current system state */

  Object_Description   *obj_desc;          /* the object desc of the above object */
  Event_List           *ev_list;           /* List of events of an object */
  Event                *ev;                /* an event description */
  Pending_Message_List *van_pend_msg_list, *new_pend_msg_list;
  Pending_Message      *van_pend_msg;
  char                  src_obj[MAXSTRING], port[MAXSTRING];
  int                   obj_det_ev_id = 0;
  int                   det_ev_id = 0;

  /* initialize both lists */
  van_sys_st_list  = new System_State_List();
  tang_sys_st_list = new System_State_List();

  /* get the DET event that is enabled, if any */
  if (the_system_desc->show_det_model()) {
    if (get_enabled_det_ev(&obj_det_ev_id, &det_ev_id) < 0) {
      exit(-1);
    }
    /* if there are any DET event enabled */
    if (det_ev_id > 0) {
      /* set the rate of the DET event */
      ev = the_system_desc->show_det_ev_by_id(det_ev_id);
      *det_ev_rate = ev->show_det_ev_rate();
    }
  }
  /* set the the parameter det_event id */
  *det_ev = det_ev_id;

  obj_st_list_aux = show_obj_st_list(LIST_RO);
  /* get the first object state of this system state */
  obj_st = obj_st_list_aux->show_1st_obj_st();

  /* go through all the object states of this system state */
  while (obj_st != NULL) {
    
    /* get the description of the object state */
    obj_desc = obj_st->show_obj_desc();

    /* update the object name in the context */
    obj_desc->show_name(context.obj_name);

    /* get the first event of the object above */
    ev_list = obj_desc->show_event_list(LIST_RO);
    ev = ev_list->show_1st_event();

    /* go through all the events of the object */
    while (ev != NULL) {
      
        /* update the object name in the context */
        ev->show_name(context.ev_msg_name);
	context.ev_msg_type = CONTEXT_EV_TYPE;

	/* process the current event */
	new_sys_st_list = process_event(obj_st, ev, det_ev_id, obj_det_ev_id);
	
	/* go through the new system state list from the events */
	while (! new_sys_st_list->is_empty() ) {

	  new_sys_st = new_sys_st_list->get_1st_sys_st();
	  
	  /* if no messages where sent */
	  if (new_sys_st->show_no_pend_msg() == 0)
	    /* add the new system state to the tangible state list */
	    tang_sys_st_list->add_tail_sys_st(new_sys_st);
	  
	  else 
	    /* add the new system state to the vanishing state list */
	    /* adding to the head of the list makes it possible to manipulate the list as a stack */
	    van_sys_st_list->add_head_sys_st(new_sys_st);
	}
	/* delete the new system state list */
	delete new_sys_st_list;
	
	/* go to the next event */
	ev = ev_list->show_next_event();
	
    }
    delete ev_list;
    
    /* go to the next object */
    obj_st = obj_st_list_aux->show_next_obj_st();
  }
  delete obj_st_list_aux;

  /* go through all vainshing states */
  while (! van_sys_st_list->is_empty() ) {

    /* print the vanishing states */
    if (Debug_Level > 1) {
	fprintf ( stdout, "\nVanishing States: \n");
	van_sys_st_list->print_van_sys_st_list();
    }
    
    /* get the first (and remove it) from the vanishing state list */
    van_sys_st = van_sys_st_list->get_1st_sys_st();
    
    /* get the first message stored in the vanishing state */
    van_pend_msg_list = van_sys_st->show_pend_msg_list(LIST_RW);
    van_pend_msg = van_pend_msg_list->get_1st_pend_msg();

    /* check to see multiple destination */
    if (van_pend_msg->is_multidest()) {
      /* get the information of the pending message */
      van_pend_msg->show_src_obj_name(src_obj);
      van_pend_msg->show_port_name(port);
      /* expand multiple destinations to different pend messages */
      new_pend_msg_list = van_pend_msg->expand();
      /* concat the lists */
      van_pend_msg_list->concat_pend_msg_list(new_pend_msg_list);
      /* check for some destination for the message */
      if (van_pend_msg_list->is_empty()) {
	fprintf ( stderr, "ERROR: Object %s sent a message through port \"%s\" but no other object is connected to this port.\n", src_obj, port);
	exit(-1);
      }
      /* delete the multiple destination message */
      delete van_pend_msg;
      /* get the first real pending message */
      van_pend_msg = van_pend_msg_list->get_1st_pend_msg();
    }

    /* update the execution context */
    van_pend_msg->show_dest_obj_name(context.obj_name);
    van_pend_msg->show_port_name(context.ev_msg_name);
    context.ev_msg_type = CONTEXT_MSG_TYPE;
    
    new_sys_st_list = van_sys_st->process_pend_msg(van_pend_msg, det_ev_id, obj_det_ev_id);
    
    /* desallocate the memory of the vanishing state processed */
    delete van_sys_st;

    /* delete the first pending message that has been delivered*/
    delete van_pend_msg;

    while (! new_sys_st_list->is_empty() ) {
      
      new_sys_st = new_sys_st_list->get_1st_sys_st();

      /* if there aren't any messages to be sent */
      if (new_sys_st->show_no_pend_msg() == 0)
	/* add the new system state to the tangible state list */
	tang_sys_st_list->add_tail_sys_st(new_sys_st);	      
      
      else 
	/* add the new system state to the vanishing state list */
	/* adding to the head of the list makes it possible to manipulate the list as a stack */
	van_sys_st_list->add_head_sys_st(new_sys_st);    
    }

    /* desallocate the memory of the new system state list */
    delete new_sys_st_list;
    
  }

  /* delete the vanishing state list */
  delete van_sys_st_list;

  /* return the list of tangible states */
  return (tang_sys_st_list);
}


System_State_List *System_State::process_event(Object_State *obj_st, Event *ev,
					       int det_ev_id, int obj_det_ev_id)
{
System_State_List  *new_sys_st_list;
Action_List        *act_list;
Transition         *trans;
Expr_Val           *rate;

  /* if condition of the event is TRUE execute the actions */
  if ( ev->eval_cond(obj_st) == TRUE) {

    /* create a new transition with the appropriate rate */
    rate = ev->eval_rate(obj_st);
    trans = new Transition( rate );
    
    /* set the event id that originated this transition */
    trans->ch_ev_id(ev->show_det_id());
    
    /* get the object id of the object state and set the transition */
    trans->ch_obj_id(obj_st->show_id());

    /* append the new transition to this system state */
    this->add_transition(trans);
		
    /* get the action_list of the event */
    act_list = ev->show_action_list(LIST_RO);

    /* process the actions of the event */
    new_sys_st_list = process_act_list(obj_st, act_list, det_ev_id, obj_det_ev_id);
    
    /* free memory allocated to the action list */
    delete act_list;

  }
  else {
    /* return an empty list of System State */
    new_sys_st_list = new System_State_List();
  }

  return ( new_sys_st_list );  
}

System_State_List *System_State::process_pend_msg(Pending_Message *pend_msg, int det_ev_id, int obj_det_ev_id) 
{
  System_State_List   *new_sys_st_list;
  Object_Description  *obj_desc_dest, *obj_desc_src;
  Object_State        *obj_st_dest;
  Message             *msg;
  Action_List         *act_list;
  Pending_Message     *pend_msg_aux;
  char                 src_obj_name[MAXSTRING];  /* name of the object sending a message */
  char                 dest_obj_name[MAXSTRING];  /* name of the object receiving a message */
  char                 port_name[MAXSTRING];     /* name of the port receiveing a message */
  Event               *det_ev; /* DET event enabled if any */
  char                 det_ev_name[MAXSTRING]; /* name of the DET event enabled */

  /* get the name of the source and dest object and the destination portname */
  pend_msg->show_src_obj_name(src_obj_name);
  pend_msg->show_dest_obj_name(dest_obj_name);
  pend_msg->show_port_name(port_name);
      
  /* get the description of the destination object of the message */
  obj_desc_dest = the_system_desc->show_obj_desc(dest_obj_name);
  if (obj_desc_dest == NULL) 
  {
    fprintf ( stderr, "ERROR: No object named %s in the system description.\n",dest_obj_name);
    exit(-1);
  }

  /* if the model has deterministic events */
  if (det_ev_id != 0) {    
    /* get the obj_desc of the source object */
    obj_desc_src = the_system_desc->show_obj_desc(src_obj_name);
    det_ev = the_system_desc->show_det_ev_by_id(det_ev_id);
    /* check for idependent chain violation */
    if (det_ev->check_indep_chains(obj_desc_src, obj_desc_dest) == FALSE) {
      fprintf ( stderr, "WARNING: Independet chain violation.\n");
      fprintf ( stderr, "         Objects that do not belong to the same independent chain are exchanging messages.\n");
      det_ev->show_name(det_ev_name);
      fprintf ( stderr, "Event %s enabled. \n", det_ev_name);
      fprintf ( stderr, "Message going from object %s to %s\n", src_obj_name, dest_obj_name); 
      /* exit(-1); */
    }
  }

  /* get the object state destination of the message */
  obj_st_dest = this->show_obj_st(obj_desc_dest);
  
  /* get the message description to be delivered to the object */
  msg = obj_desc_dest->show_message(port_name);
  if (msg == NULL) {
    fprintf ( stderr, "ERROR: Object %s ins't able to receive a message from object %s outgoing on port %s.\n", dest_obj_name, src_obj_name, port_name);
    exit(-1);
  }

  /* get the action_list of the message part */
  act_list = msg->show_action_list(LIST_RO);
      
  /* add a copy of the pending message to the object state */
  pend_msg_aux = new Pending_Message(pend_msg);
  obj_st_dest->add_pend_msg(pend_msg_aux);

  /* process the actions of the message */
  new_sys_st_list = process_act_list(obj_st_dest, act_list, det_ev_id, obj_det_ev_id);

  /* remove and delete the pending message that was delivered */
  pend_msg_aux = obj_st_dest->get_1st_pend_msg();
  delete pend_msg_aux;

  /* free up the memory allocated to the action list */
  delete act_list;

  return ( new_sys_st_list );
  
}

System_State_List *System_State::process_act_list(Object_State *obj_st, Action_List *act_list, 
						  int det_ev_id, int obj_det_ev_id)
{
  System_State_List  *sys_st_list;
  System_State       *new_sys_st;
  Object_State       *new_obj_st;
  Object_State       *obj_det_ev;
  Action             *act;
  Event              *ev;
  Expr_Val           *prob_val;
  Transition         *trans, *trans_aux;
  int                 act_count = 1;
  TGFLOAT             prob_sum = 0;
  int                 prob_type = NUMERICAL;

  sys_st_list = new System_State_List(); 

  act = act_list->show_1st_act();

  /* get the transition created by the firing of an event */
  trans_aux = this->get_1st_trans();

  /* go through all actions in the message part */
  while (act != NULL) {

    /* update the execution context structure */
    context.act_num = act_count;
    act_count++;
	    
    /* create and add a new transition to the new sys state */
    /* this will be a transition from this system state to new_sys_st */
    /* temporarily stored in new_sys_st (from itself to itself) */

    /* create a transition duplicating the existing one */
    trans = new Transition(trans_aux);

    /* get the prob value of this action */
    prob_val = act->eval_prob(obj_st);
    /* change the rate of the new trans */
    trans->mul_rate(prob_val);
    
    /* add the prob_val to sum 1 */
    if (prob_val->show_type() == NUMERICAL)
      prob_sum += prob_val->show_value();
    else
      prob_type = LITERAL;

    /* delete the prob value */
    delete prob_val;

    /* check to see if the rate transition value is zero */
    if (trans->rate_is_zero()) {
      /* delete the transition just created */
      delete trans;
      /* go to the next action */
      act = act_list->show_next_act();
      continue;
    }

    /* execute the action (user code associates with it) */
    new_obj_st = act->evaluate(obj_st, NULL);
        
    /* create a copy of this system state */
    new_sys_st = new System_State(this);
    
    /* replace the object modified by the action just executed */
    new_sys_st->replace_obj_st(new_obj_st);
	  
    /* add the transition to the new system state */
    new_sys_st->add_transition(trans);

    /* copy the messages sent in the action stored in the object 
       state to the new system state */
    new_sys_st->copy_pend_msg(new_obj_st);
    
    /* if there is an DET event enabled */
    if (det_ev_id > 0) {
      /* if this trans was originated by an EXP event */
      if (trans->show_ev_id() == 0) {
	/* get the det event */
	ev = the_system_desc->show_det_ev_by_id(det_ev_id);
	/* check to see if the DET event was disabled */
        obj_det_ev = new_sys_st->show_obj_st(obj_det_ev_id);
	if (! ev->eval_cond(obj_det_ev)) {
	  /* change the disabled FLAG to TRUE */
	  trans->ch_disabled(TRUE);
	  //	  printf ("disabling DET event\n");
	}
      }
    }

    /* if impulse rewards were defined for this system desc */
    if (the_system_desc->show_impulse_reward() == TRUE)
      /* calculate the impulse rewards for the new transition */
      new_sys_st->calculate_impulse_rewards(obj_st, act);
    
    /* add the state to the new system state list */
    sys_st_list->add_tail_sys_st(new_sys_st);

    /* go to the next action */
    act = act_list->show_next_act();
  }

  /* delete the temporarily transition */
  delete trans_aux;

  /* check the probabilities of the action (must sum 1) */
  if (prob_type == NUMERICAL)
    if ( (prob_sum <  (1.0 - EPSILON)) || (prob_sum > (1.0 + EPSILON)) ) {
      fprintf ( stderr, "ERROR: Probabilities do not sum one (1): %.8e\n", prob_sum);
      print_context();
      fprintf ( stderr, "Aborting...\n");
      exit (-1);
    }


  return (sys_st_list);
}


int System_State::get_enabled_det_ev(int *obj_det_ev_id, int *det_ev_id)
{
   Object_State_List  *obj_st_list_aux;
   Object_State       *obj_st;
   Object_Description *obj_desc;
   Event_List         *ev_list;
   Event              *ev;
   int                 ev_id  = 0;
   int                 obj_id = 0;
   int                 det_ev_enabled = FALSE;
   Expr_Val           *det_rate;

  obj_st_list_aux = show_obj_st_list(LIST_RO);
  /* get the first object state of this system state */
  obj_st = obj_st_list_aux->show_1st_obj_st();
  
  /* go through all the object states of this system state */
  while (obj_st != NULL) {
    
    /* get the description of the object state */
    obj_desc = obj_st->show_obj_desc();

    /* update the object name in the context */
    obj_desc->show_name(context.obj_name);

    /* get the first event of the object above */
    ev_list = obj_desc->show_event_list(LIST_RO);
    ev = ev_list->show_1st_event();

    /* go through all the events of the object */
    while (ev != NULL) {
      
        /* update the object name in the context */
        ev->show_name(context.ev_msg_name);

	/* if the event is deterministic */
	if ( ev->show_det_id() > 0) {
	  /* if condition of the event is TRUE */
	  if ( ev->eval_cond(obj_st) == TRUE) {
	    /* if it was enabled */
	    if (det_ev_enabled) {
	      the_system_desc->det_model_error(ev_id, ev->show_det_id());
	      return (-1);
	    }
	    /* calculate the rate of this DET event */
	    det_rate = ev->eval_rate(obj_st);
	    /* set the DET rate of this event */
	    ev->ch_det_ev_rate(det_rate);
	    /* get the det_ev_id of this event */
	    ev_id = ev->show_det_id();
            /* get the id of the object that contains the DET event */
            obj_id = obj_st->show_id();
	    /* set the enabled flag */
	    det_ev_enabled = TRUE;
	  }
	  
	}
	/* go to the next event */
	ev = ev_list->show_next_event();
	
    }
    delete ev_list;
    
    /* go to the next object */
    obj_st = obj_st_list_aux->show_next_obj_st();
  }
  delete obj_st_list_aux;
  
  /* return the id of the enabled DET event and the object if any */
  *obj_det_ev_id = obj_id;
  *det_ev_id = ev_id;
  return (1);
}

//------------------------------------------------------------------------------
void System_State::calculate_rate_rewards( int check_ir )
{
    Rate_Reward_List      *reward_list;
    Rate_Reward           *rew_aux;
    Object_State          *obj_st;
    Object_Description    *obj_desc;
    Rate_Reward_Desc_List *rew_desc_list;
    Rate_Reward_Desc      *rew_desc;
    Expr_Val              *rate_val;
    Reward_Measure        *rew_meas;
    RR_Event              *rr_ev;
    RR_Event_List         *rr_ev_list;
    Rew_Reach_Distrib     *rew_reach_dist;

    /* initialize the reward lists */
    reward_list  = new Rate_Reward_List();

    /* get the first object state of this system state */
    obj_st = obj_st_list->show_1st_obj_st();

    /* go through all the object states of this system state */
    while (obj_st != NULL)
    {
        /* get the description of the object state */
        obj_desc = obj_st->show_obj_desc();

        /* get the first reward of the object above */
        rew_desc_list = obj_desc->show_reward_list(LIST_RO);
        rew_desc = rew_desc_list->show_1st_rew();

        /* go through all the rewards of the object */
        while (rew_desc != NULL)
        {
            /* Verify if the ir should be copied from eval_rate */
            /*   or ir_user_set_value (manually set).           */
            if ( rew_desc->show_ir_user_set_flag() == TRUE )
            {
                rate_val = new Expr_Val( rew_desc->show_ir_user_set_value() );
            }
            else
            {
                /* calculate all the rates for the reward */
                rate_val = rew_desc->eval_rate(obj_st);

                /* verification of the need for evaluation */
                if ( (check_ir == CHECK_IR_CHANGE) &&
                     (rew_desc->show_last_ir_val() != rate_val->show_value()) )
                {
                    /* The state change causes a change in CR slope (ir changed) */
                    /* So the events that has this reward in it's condition must */
                    /*   be notified throught the flag need_evaluation.          */
                    rew_meas = rew_desc->show_rew_meas();
                    if ( rew_meas != NULL )
                    {
                        if ( rew_meas->exists_RR_events_list() )
                        {
                            rr_ev_list = rew_meas->show_RR_events_list( LIST_RO );
                            rr_ev = rr_ev_list->show_1st_rr_ev();
                            while ( rr_ev != NULL )
                            {
                                rew_reach_dist = (Rew_Reach_Distrib *)rr_ev->show_event()->show_distrib();
                                rew_reach_dist->ch_need_evaluation( TRUE );
                                /* go to the next rr_event */
                                rr_ev = rr_ev_list->show_next_rr_ev();
                            }
                            delete rr_ev_list;
                        }
                    }
                }
            }

            /* create a new reward */
            rew_aux = new Rate_Reward (rew_desc->show_id(), rate_val);

            /* add it to the reward list */
            reward_list->add_tail_rew(rew_aux);

            /* go to the next reward description */
            rew_desc = rew_desc_list->show_next_rew();
        }
        delete rew_desc_list;

        /* go to the next object state */
        obj_st = obj_st_list->show_next_obj_st();
    }

    /* update the rate reward list */
    if (rate_rew_list == NULL)
        rate_rew_list = reward_list;
    else
        fprintf( stderr, "ERROR: Evaluating the rate reward more than once.\n");
}
//------------------------------------------------------------------------------
void System_State::calculate_global_rate_rewards()
{
    Rate_Reward_Desc_List *rew_desc_list;
    Rate_Reward_Desc      *rew_desc;
    Expr_Val              *rate_val;
    Rate_Reward           *rew_aux;

    /* get the first reward of the object above */
    rew_desc_list = the_system_desc->show_global_rew_desc_list(LIST_RO);
    rew_desc = rew_desc_list->show_1st_rew();

    /* go through all the rewards of the object */
    while (rew_desc != NULL) {

      /* calculate all the rates for the reward */
      rate_val = rew_desc->eval_rate(NULL, this);

      /* create a new reward */
      rew_aux = new Rate_Reward (rew_desc->show_id(), rate_val);
      
      /* add it to the reward list */
      if (rate_rew_list != NULL)
	rate_rew_list->add_tail_rew(rew_aux);
      else
	fprintf ( stderr, "ERROR: Evaluating the global rate reward on a NULL list\n");
  
      /* go to the next reward description */
      rew_desc = rew_desc_list->show_next_rew();
    }
    delete rew_desc_list;
}

Rate_Reward_List *System_State::get_rate_reward_list()
{
  Rate_Reward_List *rate_rew_list_aux;

  rate_rew_list_aux = rate_rew_list;
  rate_rew_list = NULL;

  return(rate_rew_list_aux);
}
//-----------------------------------------------------------------------------
Rate_Reward_List *System_State::show_rate_rew_list(int access)
{
    Rate_Reward_List *rate_rew_list_aux;

    if (rate_rew_list == NULL)
    {
        fprintf( stderr, "It's impossible to create another list from a NULL one. ");
        fprintf( stderr, "System_State:rate_rew_list=NULL.\n");
        fflush(stderr);
    }
    if (access == LIST_RW)
        return (rate_rew_list);
    else
    {
        rate_rew_list_aux = new Rate_Reward_List(rate_rew_list);
        return (rate_rew_list_aux);
    }
}
//-----------------------------------------------------------------------------
TGFLOAT System_State::show_reward_val(const char *a_rew_name)
{
    char  comp_name[MAXSTRING];
    char *obj_name, *rew_name;
    Object_Description    *obj_desc;
    Rate_Reward_Desc_List *rew_desc_list;
    Rate_Reward_Desc      *rew_desc;
    Rate_Reward_List      *rate_reward_list = NULL;
    Rate_Reward           *rate_rew;
    Expr_Val              *rate_val;
    TGFLOAT                value = 0;

    /* split the comp_name into obj_name and reward name */
    strcpy (comp_name, a_rew_name);
    obj_name = strtok (comp_name, ".");
    rew_name = strtok (NULL, ".");

    /* get the correct object desc for this name */
    obj_desc = the_system_desc->show_obj_desc(obj_name);
    /* get the reward description list of this object */
    rew_desc_list = obj_desc->show_reward_list(LIST_RO);
    /* find the correct reward description for this name */
    if (rew_desc_list->query_rew(rew_name))
    {
        rew_desc = rew_desc_list->show_curr_rew();
        /* find the value of this reward */
        rate_reward_list = show_rate_rew_list(LIST_RO);
        if (rate_reward_list->query_rew(rew_desc->show_id()))
        {
            rate_rew = rate_reward_list->show_curr_rew();
            rate_val = rate_rew->show_value();
            value = rate_val->show_value();
        }
    }
    else
    {
        fprintf ( stderr, "ERROR: Reward %s not found!",a_rew_name);
    }
    /* delete the temporary lists */
    delete( rew_desc_list );
    delete( rate_reward_list );
    return( value );
}
//-----------------------------------------------------------------------------
void System_State::calculate_impulse_rewards(Object_State *obj_st, Action *act)
{
    Transition           *trans;
    Imp_Reward_Desc      *imp_rew_aux;
    Imp_Reward_Desc_List *imp_rew_list_aux;
    Expr_Val             *imp_value;
    int                   imp_id;

    /* get the transition */
    trans = show_1st_trans();

    /* calculate all impulse rewards */
    imp_rew_list_aux = act->show_impulse_list(LIST_RO);
    imp_rew_aux = imp_rew_list_aux->show_1st_rew();
    /* go through all impulse rewards description of this action */
    while (imp_rew_aux != NULL) {
      /* get the value of the impulse */
      imp_value = imp_rew_aux->eval_impulse(obj_st);
      /* add the reward to the transition */
      imp_id = imp_rew_aux->show_id();
      trans->add_imp_rew(imp_id, imp_value, trans->show_rate() );
      /* delete the value just used */
      delete imp_value;
      /* go to the next impulse reward description */
      imp_rew_aux = imp_rew_list_aux->show_next_rew();
    }

    delete imp_rew_list_aux;
}

/*********************************************************************************/

System_State_List::System_State_List() : Chained_List()
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_System_State_List_counter;
   DBG_System_State_List_counter++;
   DBG_System_State_List_memory++;
   debug_mem(DBG_id,"System_State_List::System_State_List(): creating object");
#endif
}

System_State_List::System_State_List(System_State_List *sys_st_list) :
                   Chained_List( (Chained_List *)sys_st_list)
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_System_State_List_counter;
   DBG_System_State_List_counter++;
   DBG_System_State_List_memory++;
   debug_mem(DBG_id,"System_State_List::System_State_List(): creating object");
#endif
}

System_State_List::System_State_List(System_State *new_sys_st) : Chained_List()
{
  add_tail_sys_st(new_sys_st);
#ifdef _MEM_DEBUG
   DBG_id = DBG_System_State_List_counter;
   DBG_System_State_List_counter++;
   DBG_System_State_List_memory++;
   debug_mem(DBG_id,"System_State_List::System_State_List(): creating object");
#endif
}

System_State_List::~System_State_List()
{
  System_State *sys_st_aux;

  if (access == LIST_RW)
    while (!is_empty()) {
      sys_st_aux = get_1st_sys_st();
      delete sys_st_aux;
    }

#ifdef _MEM_DEBUG
   DBG_System_State_List_memory--;
   debug_mem(DBG_id,"System_State_List::~System_State_List(): destroying object");
#endif
}

int System_State_List::add_head_sys_st(System_State *new_sys_st)
{
  return (add_head( (Chained_Element *)new_sys_st ) );
}

int System_State_List::add_tail_sys_st(System_State *new_sys_st)
{
  return (add_tail( (Chained_Element *)new_sys_st ) );
}

System_State *System_State_List::show_1st_sys_st()
{
  return ( (System_State *) show_head() );
}

System_State *System_State_List::get_1st_sys_st()
{
  return ( (System_State *) del_head() );
}

System_State *System_State_List::show_next_sys_st()
{
  return ( (System_State *) show_next() );
}

System_State *System_State_List::show_curr_sys_st()
{
  return ( (System_State *) show_curr() );
}

int System_State_List::show_no_sys_st()
{
  return ( show_no_elem() ); 
}

void System_State_List::print_sys_st_list()
{
   System_State  *sys_st_aux;

   sys_st_aux = show_1st_sys_st();
   while (sys_st_aux != NULL) {
     sys_st_aux->print_sys_st();
     sys_st_aux = show_next_sys_st();
   }
}

void System_State_List::print_van_sys_st_list()
{
   System_State  *sys_st_aux;

   sys_st_aux = show_1st_sys_st();
   while (sys_st_aux != NULL) {
     sys_st_aux->print_van_sys_st();
     sys_st_aux = show_next_sys_st();
   }
}
