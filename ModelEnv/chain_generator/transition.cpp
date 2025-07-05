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
*	@file transition.cpp
*   @brief <b> Transition methods </b>.
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

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"
#include "system_desc.h"

#ifdef _MEM_DEBUG
extern int DBG_Transition_counter;
extern int DBG_Transition_memory;
extern int DBG_Transition_List_counter;
extern int DBG_Transition_List_memory;
#endif

/** global system description */
extern System_Description *the_system_desc;

Transition::Transition() : Chained_Element()
{
   sys_st_dest = NULL;
   st_dest     = 0;
   rate        = new Expr_Val();
   ev_id       = 0;
   disabled    = FALSE;
   obj_id      = 0;
   add_later   = 'n';
   if (the_system_desc->show_impulse_reward() == TRUE)
     impulse_rew_list = new Imp_Reward_List();
   else
     impulse_rew_list = NULL;

#ifdef _MEM_DEBUG
   DBG_id = DBG_Transition_counter;
   DBG_Transition_counter++;
   DBG_Transition_memory++;
   debug_mem(DBG_id,"Transition::Transition(): creating object");
#endif
} 

Transition::Transition(Expr_Val *a_rate) : Chained_Element()
{
   sys_st_dest = NULL;
   st_dest     = 0;
   rate        = a_rate;
   ev_id       = 0;
   disabled    = FALSE;
   obj_id      = 0;
   add_later   = 'n';
   if (the_system_desc->show_impulse_reward() == TRUE)
     impulse_rew_list = new Imp_Reward_List();
   else
     impulse_rew_list = NULL;

#ifdef _MEM_DEBUG
   DBG_id = DBG_Transition_counter;
   DBG_Transition_counter++;
   DBG_Transition_memory++;
   debug_mem(DBG_id,"Transition::Transition(Exor_Val *): creating object");
#endif
}

Transition::Transition(Transition *a_trans) : Chained_Element()
{
   Imp_Reward_List *a_imp_rew_list;
   Imp_Reward      *a_imp_rew;
   Imp_Reward      *imp_rew_aux;
   
   /* copy all attributes from a_trans */
   sys_st_dest = a_trans->show_sys_st_dest();
   st_dest     = a_trans->show_st_dest();
   rate        = new Expr_Val (a_trans->show_rate());
   ev_id       = a_trans->show_ev_id();
   disabled    = a_trans->show_disabled();
   obj_id      = a_trans->show_obj_id();
   add_later   = a_trans->show_add_later();

   /* copy the impulse reward list of a_trans */
   a_imp_rew_list = a_trans->show_imp_rew_list(LIST_RO);
   if (a_imp_rew_list != NULL) {
     impulse_rew_list = new Imp_Reward_List();
     a_imp_rew = a_imp_rew_list->show_1st_rew();
     while (a_imp_rew != NULL) {
       imp_rew_aux = new Imp_Reward(a_imp_rew);
       impulse_rew_list->add_tail_rew(imp_rew_aux);
       a_imp_rew = a_imp_rew_list->show_next_rew();
     }
     delete a_imp_rew_list;   
     
   } else
     impulse_rew_list = NULL;
   

#ifdef _MEM_DEBUG
   DBG_id = DBG_Transition_counter;
   DBG_Transition_counter++;
   DBG_Transition_memory++;
   debug_mem(DBG_id,"Transition::Transition(Transition *): creating object");
#endif
}

Transition::~Transition()
{
  /* delete impulse reward list */
  if (impulse_rew_list != NULL)
    delete impulse_rew_list;

  /* delete the rate */
  delete rate;

#ifdef _MEM_DEBUG
   DBG_Transition_memory--;
   debug_mem(DBG_id,"Transition::~Transition(): destroying object");
#endif
}

int Transition::show_st_dest()
{
   return(st_dest);
}

void Transition::ch_st_dest(int new_dest)
{
  st_dest = new_dest;
}
int Transition::show_ev_id()
{
   return(ev_id);
}

void Transition::ch_ev_id(int new_id)
{
  ev_id = new_id;
}
int Transition::show_disabled()
{
   return(disabled);
}

void Transition::ch_disabled(int new_disabled)
{
  disabled = new_disabled;
}

int Transition::show_obj_id()
{
   return(obj_id);
}

void Transition::ch_obj_id(int new_obj_id)
{
  obj_id = new_obj_id;
}

System_State *Transition::show_sys_st_dest()
{
   return(sys_st_dest);
}

void Transition::ch_sys_st_dest(System_State *new_sys_dest)
{
  sys_st_dest = new_sys_dest;
}


void Transition::add_imp_rew(int imp_id, Expr_Val *imp_val, Expr_Val *imp_prob)
{
   Imp_Reward  *imp_rew_aux;
   Imp_Rew_Val *imp_rew_val_aux;
   Expr_Val    *new_imp_val;


   /* if the impulse already exists */
   if (impulse_rew_list->query_rew(imp_id)) {
     imp_rew_aux = impulse_rew_list->show_curr_rew();
     imp_rew_val_aux = imp_rew_aux->show_1st_imp_val();
     /* change the value of the impulse*/
     new_imp_val = new Expr_Val( imp_rew_val_aux->show_value() );
     new_imp_val->add_expr_val(imp_val);

     imp_rew_val_aux->ch_value(new_imp_val);
     /* change the prob of the impulse */
     imp_rew_val_aux->ch_prob(imp_prob);
   } else {
     /* create a new impulse reward */
     imp_rew_aux = new Imp_Reward(imp_id);
     /* create a new impulse reward value and prob */
     imp_rew_val_aux = new Imp_Rew_Val(imp_val, imp_prob);
     imp_rew_aux->add_impulse(imp_rew_val_aux);
     /* add it to the impulse rew list */
     impulse_rew_list->add_tail_rew(imp_rew_aux);
   }

}

void Transition::add_imp_rew(Imp_Reward *new_imp_rew)
{
   Imp_Reward       *imp_rew_aux;
   Imp_Rew_Val_List *imp_rew_val_list_aux;
   Imp_Rew_Val      *imp_rew_val_aux;
   int               imp_id;

   if (new_imp_rew == NULL) {
     debug(3,"Transition::add_imp_rew: Can't add NULL pointer");
     return;
   }

   /* get the impulse id */
   imp_id = new_imp_rew->show_id();
   /* if the impulse already exists */
   if (impulse_rew_list->query_rew(imp_id)) {
     /* get the correspondent impulse */
     imp_rew_aux = impulse_rew_list->show_curr_rew();
     /* get the impulse value list of the new impulse */
     imp_rew_val_list_aux = new_imp_rew->show_imp_val_list(LIST_RW);
     /* go trough all entries of impulse value and add it to the existing impulse */
     while (! imp_rew_val_list_aux->is_empty() ) {
       imp_rew_val_aux = imp_rew_val_list_aux->get_1st_rew();
       imp_rew_aux->add_impulse(imp_rew_val_aux);
     }
     /* delete the new impulse reward */
     delete new_imp_rew;
   } else
     impulse_rew_list->add_tail_rew(new_imp_rew);

}

Imp_Reward_List *Transition::show_imp_rew_list(int access)
{
  Imp_Reward_List *imp_rew_list_aux;
  
  /* if this transition has no impulse reward list */
  if (impulse_rew_list == NULL)
    return (NULL);
  
  /* check the access requested */
  if (access == LIST_RW)
    return (impulse_rew_list);
  else {
    imp_rew_list_aux = new Imp_Reward_List(impulse_rew_list);
    return (imp_rew_list_aux);
  }
}

Expr_Val *Transition::show_rate()
{
  return (rate);
}

void Transition::ch_rate(Expr_Val *new_rate)
{
  if (new_rate != NULL) {
    /* delete the existing rate */
    delete rate;
    rate = new_rate;
  } else
    debug(3,"Transition::ch_rate: Can't change to a NULL pointer");
}

int Transition::show_rate_type()
{
  return (rate->show_type());
}

TGFLOAT Transition::show_rate_value()
{
  return (rate->show_value());
}

void Transition::show_rate_str(char *a_str)
{
  strcpy( a_str, rate->show_str_val() );
}

void Transition::sum_rate(Expr_Val *a_val)
{
  rate->add_expr_val(a_val);
}

void Transition::mul_rate(Expr_Val *a_val)
{
  rate->mul_expr_val(a_val);
}
 
int Transition::rate_is_zero()
{
  return(rate->is_zero());
}

void Transition::set_add_later()
{
  add_later = 'a';
}

char Transition::show_add_later()
{
  return( add_later );
}

void Transition::markDeleted()
{
  add_later = 'd';
}

Transition *Transition::getSameTransition()
{
    Transition *trans_aux;

    trans_aux = (Transition *)get_next();
    while( trans_aux != NULL )
    {
        if( trans_aux->show_st_dest() == show_st_dest() )
            break;
        trans_aux = (Transition *)(trans_aux->get_next());
    }

    
    return trans_aux;
}

void Transition::print_trans()
{
  rate->print_expr();
  fprintf ( stdout, " ( %d ) \n", st_dest);
}

/**********************************************************************************/

Transition_List::Transition_List() : Chained_List()
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Transition_List_counter;
   DBG_Transition_List_counter++;
   DBG_Transition_List_memory++;
   debug_mem(DBG_id,"Transition_List::Transition_List(): creating object");
#endif
}

Transition_List::Transition_List(Transition_List *trans_list) : 
                 Chained_List( (Chained_List *)trans_list )
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Transition_List_counter;
   DBG_Transition_List_counter++;
   DBG_Transition_List_memory++;
   debug_mem(DBG_id,"Transition_List::Transition_List(Transition_List *): creating object");
#endif
}

Transition_List::~Transition_List()
{
  Transition *trans_aux;
  
  if (access == LIST_RW)
    while (!is_empty()) {
      trans_aux = get_1st_trans();
      delete trans_aux;
    }
#ifdef _MEM_DEBUG
   DBG_Transition_List_memory--;
   debug_mem(DBG_id,"Transition_List::~Transition_List(): destroying object");
#endif
}

int Transition_List::add_tail_trans(Transition *new_trans)
{
  return ( add_tail( (Chained_Element *)new_trans) );
}

int Transition_List::add_prev_trans(Transition *new_trans)
{
  return ( add_prev( (Chained_Element *)new_trans) );
}

Transition *Transition_List::show_1st_trans()
{
  return ( (Transition *)show_head() );
}

Transition *Transition_List::get_1st_trans()
{
   return ( (Transition *) del_head() );
}

Transition *Transition_List::show_next_trans()
{
  return ( (Transition *)show_next() );
}

Transition *Transition_List::show_curr_trans()
{
  return ( (Transition *)show_curr() );
}

int Transition_List::show_no_trans()
{
  return ( show_no_elem() );
}

int Transition_List::query_trans(int st_id)
{
  Transition    *trans_aux;
  int            trans_st_id;

  trans_aux = show_1st_trans();

  while (trans_aux != NULL){
    trans_st_id   = trans_aux->show_st_dest();
    if (trans_st_id == st_id)
      break;
    else
      trans_aux = show_next_trans();
  }

  if (trans_aux == NULL)
    return(0);
  else
    return(1);
}

int Transition_List::query_trans(System_State *sys_st)
{
  Transition    *trans_aux;
  System_State  *trans_sys_st;

  trans_aux = show_1st_trans();

  while (trans_aux != NULL){
    trans_sys_st = trans_aux->show_sys_st_dest();
    if (sys_st->compare_sys_st(trans_sys_st))
      break;
    else
      trans_aux = show_next_trans();
  }

  if (trans_aux == NULL)
    return(0);
  else
    return(1);
}



