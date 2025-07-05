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
*	@file object_desc.cpp
*   @brief <b>Model Object Descriptor</b>.
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
#include <string.h>
#include <stdlib.h>

#include "prototypes.h"
#include "general.h"
#include "gramatica.h"
#include "chained_list.h"
#include "system_state.h"
#include "system_desc.h"


Object_Description::Object_Description() : Chained_Element()
{
  symbol_list = new Symbol_List();
  event_list = new Event_List();
  message_list = new Message_List();
  reward_list = new Rate_Reward_Desc_List();
  /* create an empty rate reward sum list */
  rate_rew_sum_list = new Rate_Reward_Sum_List();
  id = 0;
  strcpy(name, "");

  debug(4,"Object_Description::Object_Description(): creating object");
}

Object_Description::Object_Description(char *a_name) : Chained_Element()
{
  symbol_list = new Symbol_List();
  event_list = new Event_List();
  message_list = new Message_List();
  reward_list = new Rate_Reward_Desc_List();
  /* create an empty rate reward sum list */
  rate_rew_sum_list = new Rate_Reward_Sum_List();
  id = 0;
  strcpy(name, a_name);

  debug(4,"Object_Description::Object_Description(char *): creating object");
}

Object_Description::~Object_Description() 
{
  /* delete the lists */
  delete symbol_list;
  delete event_list;
  delete message_list;
  delete reward_list;
  delete rate_rew_sum_list;

  debug(4,"Object_Description::~Object_Description(): destroying object");
}

int Object_Description::show_id()
{
   return(id);
}

void Object_Description::ch_id(int new_id)
{
   id = new_id;
}

void Object_Description::show_name(char *obj_name)
{
   strcpy(obj_name, name);
}

void Object_Description::ch_name(char* obj_name)
{
  if (obj_name == NULL) 
    cerr << "Object_Description: Warning: adding null pointer to \"name\"\n";
  else
    strcpy (name, obj_name);
}


void Object_Description::add_symbol(Symbol *new_sym)
{
 if (new_sym != NULL) 
   symbol_list->add_tail_sym(new_sym);
 else
   debug(3, "Object_Description::add_symbol: Can't add NULL pointer");
}

void Object_Description::add_event(Event *new_event)
{
 if (new_event != NULL) 
   event_list->add_tail_event(new_event);
 else
   debug(3,"Object_Description::add_event: Can't add NULL pointer");
}

void Object_Description::add_msg(Message *new_msg)
{
  if (new_msg != NULL) 
    message_list->add_tail_msg(new_msg);
  else
    debug(3,"Object_Description::add_msg: Can't add NULL pointer");
}

void Object_Description::add_reward(Rate_Reward_Desc *new_reward)
{
 if (new_reward != NULL) 
   reward_list->add_tail_rew(new_reward);
 else
   debug(3,"Object_Description::add_reward: Can't add NULL pointer");
}
//------------------------------------------------------------------------------
void Object_Description::add_rate_rew_sum(Rate_Reward_Sum *new_rate_rew_sum)
{
    if (new_rate_rew_sum != NULL)
        rate_rew_sum_list->add_tail_rew(new_rate_rew_sum);
    else
        debug(3,"Object_Description::add_rate_rew_sum: Can't add NULL pointer");
}
//------------------------------------------------------------------------------

Symbol *Object_Description::show_symbol(char *sym_name)
{
  if (symbol_list->query_symbol(sym_name)  )
    return ( symbol_list->show_curr_sym() );
  else 
    return NULL;
}

Symbol_List *Object_Description::show_symbol_list(int access)
{
  Symbol_List *sym_list_aux;

  if (access == LIST_RW)
    return (symbol_list);
  else {
    sym_list_aux = new Symbol_List(symbol_list);
    return (sym_list_aux);
  }
}

int Object_Description::show_symbol_code(char *sym_name)
{
Symbol *sym;
int     sym_code;

  if ( symbol_list->query_symbol(sym_name) ) {
    sym = symbol_list->show_curr_sym();
    sym_code = sym->show_code();
    return(sym_code);
  }
  
  return (-1);
}

int  Object_Description::show_no_st_var()
{
  Symbol *sym_aux;
  int     no_st_var = 0;

  sym_aux = symbol_list->show_1st_sym();
  while (sym_aux != NULL) {
    if ((sym_aux->show_type() == STATE_VAR_TYPE) ||
// INSERTED BY BRUNO&KELVIN to have float states
        (sym_aux->show_type() == STATE_VAR_FLOAT_TYPE))
// END OF INSERTED BY BRUNO&KELVIN to have float states
      no_st_var += sym_aux->show_dimension();
    sym_aux = symbol_list->show_next_sym();
  }

  return (no_st_var);
}

Message_List *Object_Description::show_message_list(int access)
{
  Message_List *msg_list_aux;

  if (access == LIST_RW)
    return (message_list);
  else {
    msg_list_aux = new Message_List(message_list);
    return (msg_list_aux);
  }
}

Message *Object_Description::show_message(char *port_name)
{
  if (message_list->query_msg(port_name) )
    return ( message_list->show_curr_msg() );
  else
    return (NULL);
}

Event_List *Object_Description::show_event_list(int access)
{
  Event_List *ev_list_aux;

  if (access == LIST_RW)
    return (event_list);
  else {
    ev_list_aux = new Event_List(event_list);
    return (ev_list_aux);
  }
}

Event *Object_Description::show_det_ev_by_id(int ev_id)
{
   Event *ev;
   
   ev = event_list->show_1st_event();
   while (ev != NULL) {
     if (ev->show_det_id() == ev_id)
       return (ev);
     ev = event_list->show_next_event();
   }
   return (NULL);
}

Event *Object_Description::show_ev_by_name(char *ev_name)
{
   Event *ev;
   
   if (event_list->query_event(ev_name)) {
     ev = event_list->show_curr_event();
     return (ev);
   }
   return (NULL);
}

Rate_Reward_Desc_List *Object_Description::show_reward_list(int access)
{
  Rate_Reward_Desc_List *rew_list_aux;

  if (access == LIST_RW)
    return (reward_list);
  else {
    rew_list_aux = new Rate_Reward_Desc_List(reward_list);
    return (rew_list_aux);
  }
}
//------------------------------------------------------------------------------
Rate_Reward_Sum_List *Object_Description::show_rate_rew_sum_list(int access)
{
    Rate_Reward_Sum_List  *rate_rew_sum_list_aux;

    if (access == LIST_RW)
        return (rate_rew_sum_list);
    else
    {
        rate_rew_sum_list_aux = new Rate_Reward_Sum_List(rate_rew_sum_list);
        return (rate_rew_sum_list_aux);
    }
}
//------------------------------------------------------------------------------
void strreplace(char *s1, char *sub, char *repl)
{
  char  s2[MAXSTRING];
  char *pos1, *pos2;
  
  pos1 = strstr(s1, sub);
  while (pos1 != NULL) {
     memcpy (s2, s1, (pos1 - s1) );
     pos2 = s2 + (pos1 - s1);
     pos1 = pos1 + strlen(sub);
     memcpy (pos2, repl, strlen(repl) );
     pos2 = pos2 + strlen(repl);
     memcpy (pos2, pos1, strlen(pos1));
     pos2 = pos2 + strlen(pos1);
     *pos2 = '\0';
     strcpy (s1, s2);
     /* search for the next substring */
     pos1 = strstr(s1, sub);
  }
}

void Object_Description::replace_symbols(Object_State *obj_st, char *str)
{
    Symbol *sym_aux;
    char    str_aux[MAXSTRING];
    char    val_str[MAXSTRING];
    char    str_tmp[MAXSTRING];
    t_value value;
// INSERTED BY BRUNO&KELVIN to have float states
    TGFLOAT   st_var_float_val;
// END OF INSERTED BY BRUNO&KELVIN to have float states
    int     st_var_val;
    int     sym_type;
    char    sym_name[MAXSTRING];

    strcpy( str_aux, str );

    /* first step: substitute variables and constants */
    sym_aux = symbol_list->show_1st_sym(); 
    while( sym_aux != NULL )
    {
        /* MUST be initialized with 0 */
        memset( &value, 0, sizeof(value) );

        sym_type = sym_aux->show_type();
        sym_aux->show_value( &value );

        /* substitute state variables */
        if( sym_type == STATE_VAR_TYPE )
        {
            sym_aux->show_name( str_tmp );
            st_var_val = obj_st->show_st_var_int_value( str_tmp );
            sprintf( val_str, "%d", st_var_val );
            sprintf( sym_name, " %s ", str_tmp );
            strreplace( str_aux, sym_name, val_str );       

            if( value.int_vec )
            {
                delete[] value.int_vec;
                value.int_vec = NULL;
            }
        }

        if( sym_type == STATE_VAR_FLOAT_TYPE )
        {
            fprintf(stderr," void Object_Description::replace_symbols(Object_State *obj_st, char *str): float state" );
            sym_aux->show_name( str_tmp );
            st_var_float_val = obj_st->show_st_var_float_value(str_tmp);
            sprintf (val_str, "%.10E", st_var_float_val);
            sprintf (sym_name, " %s ",str_tmp);
            strreplace(str_aux, sym_name, val_str );

            if( value.tgf_vec )
            {
                delete[] value.tgf_vec;
                value.tgf_vec = NULL;
            }
        }

        /* substitute integer constants */
        if (sym_type == INT_CONST_TYPE)
        {
            sprintf (val_str, "%d", value.i);
            sym_aux->show_name(str_tmp);
            sprintf (sym_name, " %s ",str_tmp);
            strreplace(str_aux, sym_name, val_str );
        } 

        /* substitute float constants */
        if (sym_type == FLOAT_CONST_TYPE)
        {
            sprintf (val_str, "%.10E", value.f);
            sym_aux->show_name(str_tmp);
            sprintf (sym_name, " %s ",str_tmp);
            strreplace(str_aux, sym_name, val_str );
        }
        sym_aux = symbol_list->show_next_sym();
   }

   /* second step: substitute parameters */
   sym_aux = symbol_list->show_1st_sym(); 
   while (sym_aux != NULL) {
     sym_type = sym_aux->show_type();
     if (sym_type == INT_PARAM_TYPE || sym_type == FLOAT_PARAM_TYPE) {
       sprintf (val_str, "%c", sym_aux->show_char_code() );
       sym_aux->show_name(str_tmp);
       sprintf (sym_name, " %s ",str_tmp);
       strreplace(str_aux, sym_name, val_str );
     }
     sym_aux = symbol_list->show_next_sym();
   }

   strcpy (str, str_aux);
}

int Object_Description::has_port(char *port_name)
{
  /* return true if there is a port named port_name in */
  /* the message list of this object */
  return (message_list->query_msg(port_name));
}


void Object_Description::print_obj_desc()
{
  Symbol       *sym_aux;
  Event        *event_aux;
  Message      *msg_aux;
  Rate_Reward_Desc *rew_aux;

  fprintf ( stdout, "Object Name: %s\n", name);

  sym_aux = symbol_list->show_1st_sym();
  fprintf ( stdout, "*** SYMBOLS ***\n");
  while (sym_aux != NULL) {
    sym_aux->print_sym();
    sym_aux = symbol_list->show_next_sym();
  }

  event_aux = event_list->show_1st_event();
  while (event_aux != NULL) {
    event_aux->print_event();
    event_aux = event_list->show_next_event();
  }

  msg_aux = message_list->show_1st_msg();
  while (msg_aux != NULL) {
    msg_aux->print_message();
    msg_aux = message_list->show_next_msg();
  }

  rew_aux = reward_list->show_1st_rew();
  while (rew_aux != NULL) {
    rew_aux->print_reward();
    rew_aux = reward_list->show_next_rew();
  }

}


/********************************************************************/

Object_Description_List::Object_Description_List() : Chained_List()
{
  debug(4,"Object_Description_List::Object_Description_List(): creating object");
}

Object_Description_List::Object_Description_List(Object_Description_List *obj_desc_list) :
                         Chained_List( (Chained_List *)obj_desc_list )
{
  debug(4,"Object_Description_List::Object_Description_List(Object_Description_List *): creating object");
}

Object_Description_List::~Object_Description_List()
{
  Object_Description *obj_desc_aux;

  if (access == LIST_RW) {
    obj_desc_aux = get_1st_obj_desc();
    while (obj_desc_aux != NULL) {
      delete obj_desc_aux;
      obj_desc_aux = get_1st_obj_desc();
    }
  }
  debug(4,"Object_Description_List::~Object_Description_List(): destroying object");
}

int Object_Description_List::add_tail_obj_desc(Object_Description *new_obj_desc)
{
  return ( add_tail( (Chained_Element *)new_obj_desc) );
}

Object_Description *Object_Description_List::show_1st_obj_desc()
{
  return ( (Object_Description *)show_head() );
}

Object_Description *Object_Description_List::get_1st_obj_desc()
{
  return ( (Object_Description *)del_head() );
}

Object_Description *Object_Description_List::del_obj_desc()
{
  return ( (Object_Description *)del_elem() );
}

Object_Description *Object_Description_List::show_next_obj_desc()
{
  return ( (Object_Description *)show_next() );
}

Object_Description *Object_Description_List::show_curr_obj_desc()
{
  return ( (Object_Description *)show_curr() );
}

int Object_Description_List::query_obj_desc(char *tg_obj_name)
{
    Object_Description  *obj_desc_aux;
    char                 obj_name[MAXSTRING];

    obj_desc_aux = show_1st_obj_desc();

    while (obj_desc_aux != NULL) 
    {
        obj_desc_aux->show_name(obj_name);
        if (!strcmp(obj_name, tg_obj_name) )
        {
             return (1);
        }
        obj_desc_aux = show_next_obj_desc();
    }

    return (0);
}

int Object_Description_List::query_obj_desc(int tg_obj_id)
{
    Object_Description  *obj_desc_aux;

    obj_desc_aux = show_1st_obj_desc();

    while (obj_desc_aux != NULL) {
      if (obj_desc_aux->show_id() == tg_obj_id)
	return (1);
      obj_desc_aux = show_next_obj_desc();
    }

    return (0);
}

int Object_Description_List::show_no_obj_desc()
{
  return ( show_no_elem() );
}

