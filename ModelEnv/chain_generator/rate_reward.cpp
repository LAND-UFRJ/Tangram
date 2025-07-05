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
*  @file rate_reward.cpp
*  @brief <b>Rate reward methods</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  The detailed description is unavailable. 
*/

#include <iostream>

using namespace std;

#include <stdio.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"

#ifdef _MEM_DEBUG
extern int DBG_Rate_Reward_counter;
extern int DBG_Rate_Reward_memory;
extern int DBG_Rate_Reward_List_counter;
extern int DBG_Rate_Reward_List_memory;
#endif

//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
Rate_Reward::Rate_Reward(int a_id, Expr_Val *a_value) : Chained_Element()
{
   id    = a_id;
   value = a_value;

#ifdef _MEM_DEBUG
   DBG_id = DBG_Rate_Reward_counter;
   DBG_Rate_Reward_counter++;
   DBG_Rate_Reward_memory++;
   debug_mem(DBG_id,"Rate_Reward::Rate_Reward(Expr_Val *): creating object");
#endif
}
//------------------------------------------------------------------------------
Rate_Reward::~Rate_Reward()
{
  delete value;
  
#ifdef _MEM_DEBUG
   DBG_Rate_Reward_memory--;
   debug_mem(DBG_id,"Rate_Reward::~Rate_Reward(): destroying object");
#endif
}
//------------------------------------------------------------------------------
//     M E T H O D S
//------------------------------------------------------------------------------
int Rate_Reward::show_id()
{
  return(id);
}
//------------------------------------------------------------------------------
void Rate_Reward::ch_value(Expr_Val *new_value)
{
  if (new_value != NULL) {
    delete value;
    value = new_value;
  } else
    debug(3,"Rate_Reward::ch_value: Can't change to a NULL pointer");
}
//------------------------------------------------------------------------------
Expr_Val *Rate_Reward::show_value()
{
   return(value);
}
//------------------------------------------------------------------------------
void Rate_Reward::print_reward()
{
  fprintf ( stdout, "Rate_Reward: ");
  value->print_expr();
  fprintf ( stdout, "\n");
}
//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R   ||||   L I S T
//------------------------------------------------------------------------------
Rate_Reward_List::Rate_Reward_List() : Chained_List()
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Rate_Reward_List_counter;
   DBG_Rate_Reward_List_counter++;
   DBG_Rate_Reward_List_memory++;
   debug_mem(DBG_id,"Rate_Reward_List::Rate_Reward_List(): creating object");
#endif
}
//------------------------------------------------------------------------------
Rate_Reward_List::Rate_Reward_List(Rate_Reward_List *rew_list) :
                 Chained_List( (Chained_List *)rew_list )
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Rate_Reward_List_counter;
   DBG_Rate_Reward_List_counter++;
   DBG_Rate_Reward_List_memory++;
   debug_mem(DBG_id,"Rate_Reward_List::Rate_Reward_List(Rate_Reward_List *): creating object");
#endif
}
//------------------------------------------------------------------------------
Rate_Reward_List::~Rate_Reward_List()
{
  Rate_Reward *rew_aux;
  
  if (access == LIST_RW)
    while (!is_empty()) {
      rew_aux = get_1st_rew();
      delete rew_aux;
    }
#ifdef _MEM_DEBUG
   DBG_Rate_Reward_List_memory--;
   debug_mem(DBG_id,"Rate_Reward_List::~Rate_Reward_List(): destroying object");
#endif
}
//------------------------------------------------------------------------------
//     M E T H O D S   |||   L I S T
//------------------------------------------------------------------------------
int Rate_Reward_List::add_tail_rew(Rate_Reward *new_rew)
{
  return ( add_tail( (Chained_Element *)new_rew) );
}
//------------------------------------------------------------------------------
Rate_Reward *Rate_Reward_List::show_1st_rew()
{
  return ( (Rate_Reward *)show_head() );
}
//------------------------------------------------------------------------------
Rate_Reward *Rate_Reward_List::get_1st_rew()
{
   return ( (Rate_Reward *) del_head() );
}
//------------------------------------------------------------------------------
Rate_Reward *Rate_Reward_List::show_next_rew()
{
  return ( (Rate_Reward *)show_next() );
}
//------------------------------------------------------------------------------
Rate_Reward *Rate_Reward_List::show_curr_rew()
{
  return ( (Rate_Reward *)show_curr() );
}
//------------------------------------------------------------------------------
int Rate_Reward_List::query_rew(int tg_rew_id)
{
    Rate_Reward *rew_aux;

    rew_aux = show_1st_rew();
    while (rew_aux != NULL) {
      if ( rew_aux->show_id() == tg_rew_id)
	return (1);
      rew_aux = show_next_rew();
    }
    return (0);
}
//------------------------------------------------------------------------------
int Rate_Reward_List::show_no_rew()
{
  return ( show_no_elem() );
}
//------------------------------------------------------------------------------
