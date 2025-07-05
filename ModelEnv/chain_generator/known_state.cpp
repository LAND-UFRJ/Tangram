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
*	@file known_state.cpp
*   @brief <b>unknown</b>.
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

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"
#include "system_desc.h"
#include "hash.h"

extern System_Description  *the_system_desc;
extern Hash                *hashtb;

#ifdef _MEM_DEBUG
extern int DBG_Known_State_counter;
extern int DBG_Known_State_memory;
extern int DBG_Known_State_List_counter;
extern int DBG_Known_State_List_memory;
#endif

/* Known_State methods. */

Known_State::Known_State() : Chained_Element()
{
   trans_list  = new Transition_List();
   reward_list = NULL;
   st_id       = 0;
   total_trans_rate = new Expr_Val();
   det_ev_id = 0;
   det_ev_rate = NULL;

#ifdef _MEM_DEBUG
   DBG_id = DBG_Known_State_counter;
   DBG_Known_State_counter++;
   DBG_Known_State_memory++;
   debug_mem(DBG_id,"Known_State::Known_State(): creating object");
#endif
} 

Known_State::Known_State(unsigned int a_st_id) : Chained_Element()
{
  trans_list = new Transition_List();
  reward_list = NULL;
  st_id = a_st_id;
  total_trans_rate = new Expr_Val();
  det_ev_id = 0;
  det_ev_rate = NULL;

#ifdef _MEM_DEBUG
   DBG_id = DBG_Known_State_counter;
   DBG_Known_State_counter++;
   DBG_Known_State_memory++;
   debug_mem(DBG_id,"Known_State::Known_State(System_State *): creating object");
#endif
}

Known_State::~Known_State()
{
  /* desallocate the transition List */
  delete trans_list;

  /* desallocate the reward List */  
  if (reward_list != NULL)
    delete reward_list;

  /* desallocate the total out rate */
  delete total_trans_rate;

  /* delete the DET event rate if any */
  if (det_ev_rate != NULL)
    delete det_ev_rate;

#ifdef _MEM_DEBUG
   DBG_id = DBG_Known_State_counter;
   DBG_Known_State_counter++;
   DBG_Known_State_memory++;
   debug_mem(DBG_id,"Known_State::~Known_State(): destroying object");
#endif
}

int Known_State::show_st_id()
{
   return(st_id);
}

Expr_Val *Known_State::show_trans_rate()
{
   return(total_trans_rate);
}

TGFLOAT Known_State::show_trans_rate_value()
{
   return(total_trans_rate->show_value());
}

int Known_State::get_st_vec(unsigned int *state_vec)
{  
  return (hashtb->num_to_vec(st_id, state_vec));
}

void Known_State::ch_st_id(int new_id)
{
  st_id = new_id;
}

void Known_State::add_trans(Transition *new_trans)
{
  Imp_Reward_List  *imp_rew_list_aux;
  Imp_Reward  *imp_rew_aux;
  Transition  *trans_aux;
  int          state_id;
  
  if (new_trans == NULL) {
    debug(3,"Known_State::add_trans: Can't add NULL pointer");
    return;
  }

  /* update the total transition rate of this state */
  total_trans_rate->add_expr_val(new_trans->show_rate());

  /* get the state destination of the new trans */
  state_id = new_trans->show_st_dest();
  /* if this transition already exists */
  if (trans_list->query_trans(state_id))
  {
    trans_aux = trans_list->show_curr_trans();
    /* check if the new transition is the same type of the old one */
    if( new_trans->show_ev_id() == trans_aux->show_ev_id() )
    {
        /* sum the rates of the new and existing trans */
        trans_aux->sum_rate( new_trans->show_rate() );    
        /* go through all rewards and merge the lists */
        imp_rew_list_aux = new_trans->show_imp_rew_list(LIST_RW);
        if (imp_rew_list_aux != NULL) {
          while (!imp_rew_list_aux->is_empty() ) {
	    imp_rew_aux = imp_rew_list_aux->get_1st_rew();
	    trans_aux->add_imp_rew (imp_rew_aux);
          }
        }
        /* delete the new transition */
        delete new_trans;
    }
    else
    {
      /* Set add_later flag on the first occurence of this transition */
      trans_aux->set_add_later();

      trans_list->add_tail_trans(new_trans);
    }
  }
  else 
    trans_list->add_tail_trans(new_trans);

}

Transition_List *Known_State::show_trans_list(int access)
{
  Transition_List *trans_list_aux;

  if (access == LIST_RW)
    return (trans_list);
  else {
    trans_list_aux = new Transition_List(trans_list);
    return (trans_list_aux);
  }
}

void Known_State::add_reward(Rate_Reward *new_reward)
{
  /* create the list if there isn't one */
  if (reward_list == NULL)
    reward_list = new Rate_Reward_List();

  if (new_reward != NULL) 
    reward_list->add_tail_rew(new_reward);
  else   
    debug(3,"Known_State::add_reward: Can't add NULL pointer");
}

Rate_Reward_List *Known_State::show_reward_list(int access)
{
  Rate_Reward_List *reward_list_aux;

  if (reward_list == NULL) {
    debug(3,"Known_State::show_reward_list: List has a NULL pointer");
    return (NULL);
  }
    
  if (access == LIST_RW)
    return (reward_list);
  else {
    reward_list_aux = new Rate_Reward_List(reward_list);
    return (reward_list_aux);
  }
}

void Known_State::ch_reward_list(Rate_Reward_List *new_rew_list)
{
  if (reward_list == NULL)
    reward_list = new_rew_list;
  else 
    debug(3,"Known_State::ch_reward_list: Can't replace a not NULL list");
}

int Known_State::show_det_ev_id()
{
  return(det_ev_id);
}

void Known_State::ch_det_ev_id(int new_id)
{
  det_ev_id = new_id;
}

Expr_Val *Known_State::show_det_ev_rate()
{
  return (det_ev_rate);
}

void Known_State::ch_det_ev_rate(Expr_Val *new_rate)
{
  if (new_rate != NULL) {
    det_ev_rate = new_rate;
  }
}

void Known_State::print_st_id()
{
  fprintf ( stdout, "%d ", st_id);
}

/**********************************************************************************/

Known_State_List::Known_State_List() : Chained_List()
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Known_State_List_counter;
   DBG_Known_State_List_counter++;
   DBG_Known_State_List_memory++;
   debug_mem(DBG_id,"Known_State_List::Known_State_List(): creating object");
#endif
}

Known_State_List::Known_State_List(Known_State_List *known_st_list) :
                 Chained_List( (Chained_List *)known_st_list )
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Known_State_List_counter;
   DBG_Known_State_List_counter++;
   DBG_Known_State_List_memory++;
   debug_mem(DBG_id,"Known_State_List::Known_State_List(Known_List *): creating object");
#endif
}

Known_State_List::~Known_State_List()
{
  Known_State *known_st_aux;
  
  if (access == LIST_RW)
    while (!is_empty()) {
      known_st_aux = get_1st_known_st();
      delete known_st_aux;
    }
#ifdef _MEM_DEBUG
   DBG_Known_State_List_memory--;
   debug_mem(DBG_id,"Known_State_List::~Known_State_List(): destroying object");
#endif
}

int Known_State_List::add_tail_known_st(Known_State *new_known_st)
{
  return ( add_tail( (Chained_Element *)new_known_st) );
}

int Known_State_List::add_prev_known_st(Known_State *new_known_st)
{
  return ( add_prev( (Chained_Element *)new_known_st) );
}

Known_State *Known_State_List::show_1st_known_st()
{
  return ( (Known_State *)show_head() );
}

Known_State *Known_State_List::get_1st_known_st()
{
   return ( (Known_State *) del_head() );
}

Known_State *Known_State_List::show_next_known_st()
{
  return ( (Known_State *)show_next() );
}

Known_State *Known_State_List::show_curr_known_st()
{
  return ( (Known_State *)show_curr() );
}

int Known_State_List::show_no_known_st()
{
  return ( show_no_elem() );
}

int Known_State_List::query_known_st(int st_id)
{
  Known_State    *known_st_aux;
  int            known_st_id;

  known_st_aux = show_1st_known_st();

  while (known_st_aux != NULL){
    known_st_id = known_st_aux->show_st_id();
    if (known_st_id == st_id)
      break;
    else
      known_st_aux = show_next_known_st();
  }

  if (known_st_aux == NULL)
    return(0);
  else
    return(1);
}

