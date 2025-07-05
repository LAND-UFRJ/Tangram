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
*    @file system_desc.cpp
*   @brief <b> System_Description methods </b>.
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
#include <string.h>
#include <stdlib.h>

#ifdef __APPLE__
  #include <limits.h>
  #include <float.h>
  #ifndef MAXINT
    #define MAXINT INT_MAX
  #endif
#else
// The values.h is deprecated.
#include <values.h>
#endif

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "string_list.h"
#include "gramatica.h"
#include "system_desc.h"


System_Description::System_Description()
{
  obj_desc_list = new Object_Description_List();
  last_obj_id = 0;
  chain_type = NUMERICAL;
  rate_reward = FALSE;
  impulse_reward = FALSE;
  det_model = FALSE;
  global_rew_desc_list = new Rate_Reward_Desc_List();
  no_det_events = 0;

  /* allocate initially 10 positions for the imp rew name table */
  imp_rew_size = 10;
  imp_rew_tb = (id_name_t *) malloc (imp_rew_size * sizeof(id_name_t));

  debug(4,"System_Description::System_Description(): creating object");
}

System_Description::~System_Description()
{
  /* delete the object state list */
  delete obj_desc_list;

  /* delete the global reward desc list */
  delete global_rew_desc_list;

  /* delete the imp_rew_tb */
  free (imp_rew_tb);

  debug(4,"System_Description::~System_Description(): destroying object");
}

void System_Description::add_obj_desc(Object_Description* new_obj_desc)
{
  last_obj_id++;
  new_obj_desc->ch_id(last_obj_id);

  if (new_obj_desc != NULL)
    obj_desc_list->add_tail_obj_desc(new_obj_desc);
  else
    debug(3,"System_Description::add_obj_desc: Can't add NULL pointer");
}

Object_Description_List *System_Description::show_obj_desc_list(int access)
{
  Object_Description_List *obj_desc_list_aux;

  if (access == LIST_RW)
    return (obj_desc_list);
  else {
    obj_desc_list_aux = new Object_Description_List(obj_desc_list);
    return (obj_desc_list_aux);
  }
}

Object_Description *System_Description::show_obj_desc(char *obj_name)
{
   Object_Description       *obj_desc_aux = NULL;

   if ( obj_desc_list->query_obj_desc(obj_name) )
     obj_desc_aux = obj_desc_list->show_curr_obj_desc();

   return (obj_desc_aux);
}

Object_Description *System_Description::show_obj_desc(int obj_id)
{
   Object_Description       *obj_desc_aux = NULL;

   if ( obj_desc_list->query_obj_desc(obj_id) )
     obj_desc_aux = obj_desc_list->show_curr_obj_desc();

   return (obj_desc_aux);
}


void System_Description::add_global_rew_desc(Rate_Reward_Desc* new_rew_desc)
{
  if (new_rew_desc != NULL)
    global_rew_desc_list->add_tail_rew(new_rew_desc);
  else
    debug(3,"System_Description::add_global_rew_desc: Can't add NULL pointer");
}

Rate_Reward_Desc_List *System_Description::show_global_rew_desc_list(int access)
{
  Rate_Reward_Desc_List *rate_rew_desc_list_aux;

  if (access == LIST_RW)
    return (global_rew_desc_list);
  else {
    rate_rew_desc_list_aux = new Rate_Reward_Desc_List(global_rew_desc_list);
    return (rate_rew_desc_list_aux);
  }
}

Rate_Reward_Desc *System_Description::show_global_rew_desc(char *rew_name)
{
   Rate_Reward_Desc  *rew_desc_aux = NULL;

   if ( global_rew_desc_list->query_rew(rew_name) )
     rew_desc_aux = global_rew_desc_list->show_curr_rew();

   return (rew_desc_aux);
}


void System_Description::set_chain_type(int a_type)
{
  chain_type = a_type;
}

int  System_Description::show_chain_type()
{
  return (chain_type);
}

void System_Description::set_rate_reward(int a_rate)
{
  rate_reward = a_rate;
}

int  System_Description::show_rate_reward()
{
  return (rate_reward);
}

void System_Description::set_impulse_reward(int a_impulse)
{
  impulse_reward = a_impulse;
}

int  System_Description::show_impulse_reward()
{
  return (impulse_reward);
}

void System_Description::set_det_model(int a_det)
{
  det_model = a_det;
}

int  System_Description::show_det_model()
{
  return (det_model);
}

void System_Description::set_no_det_ev(int a_no_det)
{
  no_det_events = a_no_det;
}

int  System_Description::show_no_det_ev()
{
  return (no_det_events);
}

Event *System_Description::show_ev_by_name(char *comp_name)
{
  Object_Description  *obj_desc;
  Event               *ev;
  char *obj_name, *ev_name;

  obj_name = strtok(comp_name, ".");
  ev_name = strtok(NULL, ".");

  if (obj_name == NULL || ev_name == NULL) {
    fprintf( stderr, "ERROR: wrong name for <obj_name>.<event_name>\n");
    return (NULL);
  }

  if (obj_desc_list->query_obj_desc(obj_name)) {
    obj_desc = obj_desc_list->show_curr_obj_desc();
    ev = obj_desc->show_ev_by_name(ev_name);
    return (ev);
  }

  fprintf( stderr, "Severe ERROR: System_Description::show_det_ev_by_id: should not return a NULL pointer\n");
  return (NULL);
}

Event *System_Description::show_det_ev_by_id(int ev_id)
{
  Object_Description  *obj_st_desc;
  Event               *ev;

  if ( (ev_id <= 0) || (ev_id > no_det_events) ) {
    fprintf( stderr, "Severe ERROR: System_Description::show_det_ev_by_id: should not return a NULL pointer\n");
    return(NULL);
  }

  obj_st_desc = obj_desc_list->show_1st_obj_desc();
  while (obj_st_desc != NULL) {
    ev = obj_st_desc->show_det_ev_by_id(ev_id);
    if (ev != NULL)
      return (ev);
    obj_st_desc =  obj_desc_list->show_next_obj_desc();
  }

  fprintf( stderr, "Severe ERROR: System_Description::show_det_ev_by_id: should not return a NULL pointer\n");
  return (NULL);
}

Object_Description *System_Description::show_obj_desc_by_det_ev_id(int ev_id)
{
  Object_Description  *obj_st_desc;
  Event               *ev;

  if ( (ev_id <= 0) || (ev_id > no_det_events) ) {
    fprintf( stderr, "Severe ERROR: System_Description::show_det_ev_by_id: should not return a NULL pointer\n");
    return(NULL);
  }

  obj_st_desc = obj_desc_list->show_1st_obj_desc();
  while (obj_st_desc != NULL) {
    ev = obj_st_desc->show_det_ev_by_id(ev_id);
    if (ev != NULL)
      return (obj_st_desc);
    obj_st_desc =  obj_desc_list->show_next_obj_desc();
  }

  fprintf( stderr, "Severe ERROR: System_Description::show_det_ev_by_id: should not return a NULL pointer\n");
  return (NULL);
}

void System_Description::det_model_error(int ev_id1, int ev_id2)
{
  Event *ev1, *ev2;
  char   ev_name1[MAXSTRING], ev_name2[MAXSTRING];

  ev1 = show_det_ev_by_id(ev_id1);
  ev2 = show_det_ev_by_id(ev_id2);
  ev1->show_name(ev_name1);
  ev2->show_name(ev_name2);

  fprintf( stderr, "ERROR: More than one deterministic events enables at the same time\n");
  fprintf( stderr, "Events: %s and %s were enabled\n", ev_name1, ev_name2);

}

void System_Description::set_imp_rew_name(char *name, int id)
{
  id_name_t *id_name_aux;

  if (id < imp_rew_size) {
    /* copy the name/id to the table */
    strcpy(imp_rew_tb[id].name, name);
    imp_rew_tb[id].id = id;
  } else {
    /* resize the table to fit more entries */
    id_name_aux = (id_name_t *)realloc(imp_rew_tb, 2*imp_rew_size*sizeof(id_name_t));
    if (id_name_aux != NULL) {
      imp_rew_tb = id_name_aux;
      imp_rew_size = 2 * imp_rew_size;
      /* copy the name/id to the table */
      strcpy(imp_rew_tb[id].name, name);
      imp_rew_tb[id].id = id;
    } else
      debug(3, " System_Description::set_imp_rew_name: No space to realloc imp_rew_tb\n");
  }
}

void System_Description::show_imp_rew_name(char *name, int id)
{
  if (name != NULL)
    strcpy (name, imp_rew_tb[id].name);
}

int System_Description::show_no_obj()
{
  return (obj_desc_list->show_no_obj_desc() );
}

int System_Description::show_no_st_var()
{
  Object_Description  *obj_st_desc;
  int                  no_vars = 0;

  obj_st_desc = obj_desc_list->show_1st_obj_desc();
  while (obj_st_desc != NULL) {
    no_vars += obj_st_desc->show_no_st_var();
    obj_st_desc =  obj_desc_list->show_next_obj_desc();
  }

  return (no_vars);
}

int System_Description::show_no_st_var(int *obj_ids, int no_objs)
{
  Object_Description  *obj_st_desc;
  int                  no_vars = 0;
  int                  k;

  for (k=0; k<no_objs; k++) {
    obj_st_desc = show_obj_desc(obj_ids[k]);
    no_vars += obj_st_desc->show_no_st_var();
  }

  return (no_vars);
}


void System_Description::reorder_objects()
{
  Object_Description *obj_st_desc;
  Symbol_List *sym_list;
  Symbol      *sym;
  int          sym_count, sym_id, min_sym_val;
  int          obj_count, obj_id, min_obj_val;
  int          i;

  /* put the highest state variable min value as the last symbol inside the object
     and put this object (with the highiest min_value) as the last object in the
     obj_desc_list */

  obj_st_desc = obj_desc_list->show_1st_obj_desc();
  obj_count = 0;
  obj_id = 0;
  min_obj_val = MAXINT;

  /* go through all objects */
  while (obj_st_desc != NULL) {

    sym_list = obj_st_desc->show_symbol_list(LIST_RW);
    sym = sym_list->show_1st_sym();
    sym_count = 0;
    sym_id = 0;
    min_sym_val = MAXINT;
    /* go through all symbols */
    while (sym != NULL) {
      /* get the highiest min value symbol */
      if (sym->show_type() == STATE_VAR_TYPE)
    if (min_sym_val > sym->show_max_value()) {
      min_sym_val = sym->show_max_value();
      sym_id = sym_count;
    }
      sym_count++;
      sym = sym_list->show_next_sym();

    }

    /* put the highest min value symbol as the last symbol in the list */
    sym = sym_list->show_1st_sym();
    for (i=0; i<sym_id; i++)
      sym = sym_list->show_next_sym();
    sym = sym_list->del_sym();
    sym_list->add_tail_sym(sym);

    /* get the obj desc with the highiest min value symbol */
    if (min_obj_val > sym->show_max_value() ) {
      min_obj_val = sym->show_max_value();
      obj_id = obj_count;
    }
    obj_count++;
    obj_st_desc = obj_desc_list->show_next_obj_desc();
  }

  /* put the obj desc with the highiest min val symbol as the last obj in the list */
  obj_st_desc = obj_desc_list->show_1st_obj_desc();
  for (i=0; i<obj_id; i++)
    obj_st_desc = obj_desc_list->show_next_obj_desc();
  obj_st_desc = obj_desc_list->del_obj_desc();
  obj_desc_list->add_tail_obj_desc(obj_st_desc);

}


// BRUNO&KELVIN MAX VALUE ESTAH SENDO IGNORADA NO MOMENTO

void System_Description::get_max_value_vec(unsigned int *state_vec)
{
  Object_Description *obj_st_desc;
  Symbol_List *sym_list;
  Symbol      *sym;
  int          i = 1, j;

  obj_st_desc = obj_desc_list->show_1st_obj_desc();
  /* go through all objects */
  while (obj_st_desc != NULL) {

    sym_list = obj_st_desc->show_symbol_list(LIST_RO);
    sym = sym_list->show_1st_sym();
    /* go through all symbols */
    while (sym != NULL) {
      /* get the max value of symbol in case its a V_STATE_TYPE */
      if (sym->show_type() == STATE_VAR_TYPE)
    for (j=0; j<sym->show_dimension(); j++) {
      state_vec[i] = sym->show_max_value();
      i++;
    }
      sym = sym_list->show_next_sym();
    }
    delete sym_list;
    obj_st_desc = obj_desc_list->show_next_obj_desc();
  }
  state_vec[0] = show_no_st_var();

}

void System_Description::get_max_value_vec(unsigned int *state_vec, int *obj_ids, int no_objs)
{
  Object_Description *obj_st_desc;
  Symbol_List *sym_list;
  Symbol      *sym;
  int          i = 1, j, k;

  /* go through all objects in the obj_ids vector*/
  for (k=0; k<no_objs; k++) {

    /* get the object desc of the k-th obj */
    obj_st_desc = show_obj_desc(obj_ids[k]);

    /* get the symbol list of this obj desc */
    sym_list = obj_st_desc->show_symbol_list(LIST_RO);
    sym = sym_list->show_1st_sym();
    /* go through all symbols */
    while (sym != NULL) {
      /* get the max value of symbol in case its a V_STATE_TYPE */
      if (sym->show_type() == STATE_VAR_TYPE)
    for (j=0; j<sym->show_dimension(); j++) {
      state_vec[i] = sym->show_max_value();
      i++;
    }
      sym = sym_list->show_next_sym();
    }
    delete sym_list;
  }

  state_vec[0] = show_no_st_var(obj_ids, no_objs);
}

String_List *System_Description::show_objs_by_port (char *port_name)
{
    String_List        *obj_list;
    String             *str;
    Object_Description *obj_desc_aux;
    char                obj_name[MAXSTRING];

    /* create an empty string list */
    obj_list = new String_List ();
    /* go through all objects in the system desc */
    obj_desc_aux = obj_desc_list->show_1st_obj_desc();
    while (obj_desc_aux != NULL) {
      /* if this object has a port named port_name */
      if (obj_desc_aux->has_port(port_name)) {
    obj_desc_aux->show_name(obj_name);
    /* create a new string */
    str = new String (obj_name);
    /* add this object name to the string list */
    obj_list->add_tail_str(str);
      }
      obj_desc_aux = obj_desc_list->show_next_obj_desc();
    }

    /* return the string list with the object names */
    return (obj_list);
}

void System_Description::print_sys_desc()
{
   Object_Description   *obj_desc_aux;

   obj_desc_aux = obj_desc_list->show_1st_obj_desc();

   while (obj_desc_aux != NULL) {
     printf ("----------------------------------------\n");
     obj_desc_aux->print_obj_desc();
     obj_desc_aux = obj_desc_list->show_next_obj_desc();
   }

}
