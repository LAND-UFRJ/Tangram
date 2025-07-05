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
*	@file action_desc.cpp
*   @brief <b> Action code methods </b>.
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
#include "simulator.h"

/** Global Debug Level */
extern int Debug_Level;
/* Global simulator_controler */
//extern Simulator_Controler *the_simulator_controler;

Action::Action() : Chained_Element()
{
   impulse_list = new Imp_Reward_Desc_List();
   prob = NULL;
   fd = 0;

   debug(4,"Action::Action(): creating object");
}

Action::Action(int a_fd, Expression *a_prob) : Chained_Element()
{
   impulse_list = new Imp_Reward_Desc_List();
   prob = a_prob;
   fd = a_fd;

   debug(4,"Action::Action(int, Expression): creating object");
}

Action::~Action()
{
  /* delete the impulse reward list */
  delete impulse_list;

  /* delete the prob if there is one */
  if (prob != NULL)
    delete prob;

  debug(4,"Action::~Action(): destroying object");
}

void Action::ch_prob(Expression *new_prob)
{ 
  if (new_prob != NULL) 
    if (prob == NULL) {
      /* delete the old prob */
      delete prob;
      /* assing the new one */
      prob = new_prob;
    }
    else 
      /* assing the new one */
      prob = new_prob;
  else   
    debug(3,"Action::ch_prob: Can't change to a NULL pointer");
}

Expr_Val *Action::eval_prob(Object_State *obj_st)
{
   Expr_Val *expr_aux;

   if (prob == NULL)
   {
     expr_aux = new Expr_Val (1.0);
     return (expr_aux);
   }
   else
   {
     return(prob->evaluate(obj_st));
    }
}

void Action::ch_fd(int a_fd) 
{ 
   fd = a_fd;; 
}

int Action::show_fd() 
{ 
   return(fd);
}

Object_State *Action::evaluate(Object_State *obj_st, Simulator *simulator)
{
  Object_State *new_obj_st;

  new_obj_st = new Object_State (obj_st);
  function_handler (fd, new_obj_st, simulator , NULL);

  return (new_obj_st);
}

void Action::add_impulse(Imp_Reward_Desc *new_imp)
{
 if (new_imp != NULL)
   impulse_list->add_tail_rew(new_imp);
 else
   debug(3, "Event::add_impulse: Can't add NULL pointer");
}

Imp_Reward_Desc_List *Action::show_impulse_list(int access)
{
  Imp_Reward_Desc_List *imp_reward_desc_list_aux;

  if (access == LIST_RW)
    return (impulse_list);
  else {
    imp_reward_desc_list_aux = new Imp_Reward_Desc_List(impulse_list);
    return (imp_reward_desc_list_aux);
  }
}

void Action::print_action()
{
   Imp_Reward_Desc *imp_aux;

   fprintf( stdout , "function number = %d\n",fd );
   fprintf( stdout , "Prob: " );
   if (prob != NULL)
     prob->print_expression();
   else
     fprintf ( stdout , "1.0\n" );

   fprintf ( stdout , "==== Reward\n" );
   imp_aux = impulse_list->show_1st_rew();
   while (imp_aux != NULL) {
     imp_aux->print_reward();
     imp_aux = impulse_list->show_next_rew();
   }

}


/************************************************************************/

Action_List::Action_List() : Chained_List()
{
   debug(4,"Action_List::Action_List(): creating object");
}

Action_List::Action_List(Action_List *act_list) : Chained_List( (Chained_List *) act_list )
{
   debug(4,"Action_List::Action_List(Action_List): creating object");
}

Action_List::~Action_List()
{
  Action *act_aux;

  if (access == LIST_RW) {
    act_aux = get_1st_act();
    while (act_aux != NULL) {
      delete act_aux;
      act_aux = get_1st_act();
    }
  }
  debug(4,"Action_List::~Action_List(): destroying object");
}

int Action_List::add_tail_act(Action *new_act)
{
  return ( add_tail( (Chained_Element *)new_act) );
}

Action *Action_List::show_1st_act()
{
  return ( (Action *)show_head() );
}

Action *Action_List::get_1st_act()
{
  return ( (Action *)del_head() );
}

Action *Action_List::show_next_act()
{
  return ( (Action *)show_next() );
}

int Action_List::show_no_act()
{
  return ( show_no_elem() );
}


