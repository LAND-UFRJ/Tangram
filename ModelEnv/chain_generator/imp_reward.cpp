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
*	@file imp_reward.cpp
*   @brief <b>Impulse rewards methods</b>.
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


/* Impulse_Reward methods. */

#ifdef _MEM_DEBUG
extern int DBG_Imp_Reward_counter;
extern int DBG_Imp_Reward_memory;
extern int DBG_Imp_Reward_List_counter;
extern int DBG_Imp_Reward_List_memory;

extern int DBG_Imp_Rew_Val_counter;
extern int DBG_Imp_Rew_Val_memory;
extern int DBG_Imp_Rew_Val_List_counter;
extern int DBG_Imp_Rew_Val_List_memory;
#endif

Imp_Reward::Imp_Reward(int a_id) : Chained_Element()
{
  id = a_id;
  imp_val_list = new Imp_Rew_Val_List();

#ifdef _MEM_DEBUG
   DBG_id = DBG_Imp_Reward_counter;
   DBG_Imp_Reward_counter++;
   DBG_Imp_Reward_memory++;
   debug_mem(DBG_id,"Imp_Reward::Imp_Reward(char *): creating object");
#endif
}

Imp_Reward::Imp_Reward(Imp_Reward *a_imp_rew) : Chained_Element()
{
  Imp_Rew_Val_List  *a_imp_rew_val_list;
  Imp_Rew_Val       *a_imp_rew_val;
  Imp_Rew_Val       *imp_rew_val_aux;

  /* copy the id from a_imp_rew */
  id = a_imp_rew->show_id();

  imp_val_list = new Imp_Rew_Val_List();

  /* copy the impulse reward value list of a_imp_rew */
   a_imp_rew_val_list = a_imp_rew->show_imp_val_list(LIST_RO);
   a_imp_rew_val = a_imp_rew_val_list->show_1st_rew();
   while (a_imp_rew_val != NULL) {
     imp_rew_val_aux = new Imp_Rew_Val(a_imp_rew_val->show_value(), 
				       a_imp_rew_val->show_prob());
     imp_val_list->add_tail_rew(imp_rew_val_aux);
     a_imp_rew_val = a_imp_rew_val_list->show_next_rew();
   }
   delete a_imp_rew_val_list;   

#ifdef _MEM_DEBUG
   DBG_id = DBG_Imp_Reward_counter;
   DBG_Imp_Reward_counter++;
   DBG_Imp_Reward_memory++;
   debug_mem(DBG_id,"Imp_Reward::Imp_Reward(Imp_Reward *): creating object");
#endif
}

Imp_Reward::~Imp_Reward()
{
  /* delete the impulse value list */
  delete imp_val_list;

#ifdef _MEM_DEBUG
   DBG_Imp_Reward_memory--;
   debug_mem(DBG_id,"Imp_Reward::~Imp_Reward(): destroying object");
#endif
}

int Imp_Reward::show_id()
{
  return (id);
}

void Imp_Reward::add_impulse(Imp_Rew_Val *new_imp_val)
{
    Imp_Rew_Val *imp_val_aux;
    Expr_Val    *imp_val, *imp_prob;

    if( new_imp_val == NULL )
    {
        debug( 3, "Imp_Reward::add_impulse: Can't add NULL pointer" );
        return;
    }

    /* get the value of the new impulse reward */
    imp_val = new_imp_val->show_value();
    /* if the value exists, then sum the probs for the same value */
    if( imp_val_list->query_rew( imp_val ) )
    {
        imp_val_aux = imp_val_list->show_curr_rew();
        imp_prob    = imp_val_aux->show_prob();
        imp_prob->add_expr_val( new_imp_val->show_prob() );
        /* delete the new impulse */
        delete new_imp_val;
    }
    else
    {
        /* add the new impulse value */
        imp_val_list->add_tail_rew( new_imp_val );
    }
}

Imp_Rew_Val_List *Imp_Reward::show_imp_val_list(int access)
{
  Imp_Rew_Val_List *imp_val_list_aux;

  if (access == LIST_RW)
    return (imp_val_list);
  else {
    imp_val_list_aux = new Imp_Rew_Val_List(imp_val_list);
    return (imp_val_list_aux);
  }
}

Imp_Rew_Val *Imp_Reward::show_1st_imp_val()
{
  return (imp_val_list->show_1st_rew());
}

void Imp_Reward::print_reward()
{
  Imp_Rew_Val  *imp_val_aux;

  fprintf ( stdout, "Impulse reward values:\n");
  imp_val_aux = imp_val_list->show_1st_rew();
  while (imp_val_aux != NULL) {
    imp_val_aux->print_imp_val();
    imp_val_aux = imp_val_list->show_next_rew();
  }
}

/**********************************************************************************/

Imp_Reward_List::Imp_Reward_List() : Chained_List()
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Imp_Reward_List_counter;
   DBG_Imp_Reward_List_counter++;
   DBG_Imp_Reward_List_memory++;
   debug_mem(DBG_id,"Imp_Reward_List::Imp_Reward_List(): creating object");
#endif
}

Imp_Reward_List::Imp_Reward_List(Imp_Reward_List *rew_list) :
                 Chained_List( (Chained_List *)rew_list )
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Imp_Reward_List_counter;
   DBG_Imp_Reward_List_counter++;
   DBG_Imp_Reward_List_memory++;
   debug_mem(DBG_id,"Imp_Reward_List::Imp_Reward_List(Imp_Reward_List *): creating object");
#endif
}

Imp_Reward_List::~Imp_Reward_List()
{
  Imp_Reward *rew_aux;
  
  if (access == LIST_RW)
    while (!is_empty()) {
      rew_aux = get_1st_rew();
      delete rew_aux;
    }

#ifdef _MEM_DEBUG
   DBG_Imp_Reward_List_memory--;
   debug_mem(DBG_id,"Imp_Reward_List::~Imp_Reward_List(): destroying object");
#endif
}

int Imp_Reward_List::add_tail_rew(Imp_Reward *new_rew)
{
  return ( add_tail( (Chained_Element *)new_rew) );
}

Imp_Reward *Imp_Reward_List::show_1st_rew()
{
  return ( (Imp_Reward *)show_head() );
}

Imp_Reward *Imp_Reward_List::get_1st_rew()
{
   return ( (Imp_Reward *) del_head() );
}

Imp_Reward *Imp_Reward_List::show_next_rew()
{
  return ( (Imp_Reward *)show_next() );
}

Imp_Reward *Imp_Reward_List::show_curr_rew()
{
  return ( (Imp_Reward *)show_curr() );
}

int Imp_Reward_List::query_rew(int tg_rew_id)
{
    Imp_Reward *rew_aux;

    rew_aux = show_1st_rew();
    while (rew_aux != NULL) {
      if ( rew_aux->show_id() == tg_rew_id)
	return (1);
      rew_aux = show_next_rew();
    }
    return (0);
}

int Imp_Reward_List::show_no_rew()
{
  return ( show_no_elem() );
}

/************************************************************************/
/* Impulse Reward Values methods. */

Imp_Rew_Val::Imp_Rew_Val() : Chained_Element()
{
  value = new Expr_Val();
  prob  = new Expr_Val();

#ifdef _MEM_DEBUG
   DBG_id = DBG_Imp_Rew_Val_counter;
   DBG_Imp_Rew_Val_counter++;
   DBG_Imp_Rew_Val_memory++;
   debug_mem(DBG_id,"Imp_Rew_Val::Imp_Rew_Val(): creating object");
#endif
} 

Imp_Rew_Val::Imp_Rew_Val(Expr_Val *a_val, Expr_Val *a_prob) : Chained_Element()
{
  value = new Expr_Val(a_val);
  prob  = new Expr_Val(a_prob);

#ifdef _MEM_DEBUG
   DBG_id = DBG_Imp_Rew_Val_counter;
   DBG_Imp_Rew_Val_counter++;
   DBG_Imp_Rew_Val_memory++;
   debug_mem(DBG_id,"Imp_Rew_Val::Imp_Rew_Val(Expr_Val *, Expr_Val *): creating object");
#endif
}

Imp_Rew_Val::~Imp_Rew_Val()
{
  delete value;
  delete prob;

#ifdef _MEM_DEBUG
   DBG_Imp_Rew_Val_memory--;
   debug_mem(DBG_id,"Imp_Rew_Val::~Imp_Rew_Val(): destroying object");
#endif
}

void Imp_Rew_Val::ch_value(Expr_Val *new_val)
{
  if (new_val != NULL) {
    delete value;    
    value = new_val;
  } else
    debug(3, "Imp_Rew_Val::ch_value: Can't change to a NULL pointer");
}

Expr_Val *Imp_Rew_Val::show_value()
{
  return (value);
}

void Imp_Rew_Val::ch_prob(Expr_Val *new_prob)
{
  if (new_prob != NULL) {
    delete prob;
    prob = new_prob;
  } else
    debug(3, "Imp_Rew_Val::ch_prob: Can't change to a NULL pointer");
}

Expr_Val *Imp_Rew_Val::show_prob()
{
  return (prob);
}

void Imp_Rew_Val::print_imp_val()
{
  fprintf ( stdout, "Value: ");
  value->print_expr();
  fprintf ( stdout, "\n");
  fprintf ( stdout, "Prob: ");
  prob->print_expr();
  fprintf ( stdout, "\n");
}

/**********************************************************************************/

Imp_Rew_Val_List::Imp_Rew_Val_List() : Chained_List()
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Imp_Rew_Val_List_counter;
   DBG_Imp_Rew_Val_List_counter++;
   DBG_Imp_Rew_Val_List_memory++;
   debug_mem(DBG_id,"Imp_Rew_Val_List::Imp_Rew_Val_List(): creating object");
#endif
}

Imp_Rew_Val_List::Imp_Rew_Val_List(Imp_Rew_Val_List *rew_list) :
                 Chained_List( (Chained_List *)rew_list )
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Imp_Rew_Val_List_counter;
   DBG_Imp_Rew_Val_List_counter++;
   DBG_Imp_Rew_Val_List_memory++;
   debug_mem(DBG_id,"Imp_Rew_Val_List::Imp_Rew_Val_List(Imp_Rew_Val_List *): creating object");
#endif
}

Imp_Rew_Val_List::~Imp_Rew_Val_List()
{
  Imp_Rew_Val *rew_aux;
  
  if (access == LIST_RW)
    while (!is_empty()) {
      rew_aux = get_1st_rew();
      delete rew_aux;
    }

#ifdef _MEM_DEBUG
   DBG_Imp_Rew_Val_List_memory--;
   debug_mem(DBG_id,"Imp_Rew_Val_List::~Imp_Rew_Val_List(): destroying object");
#endif
}

int Imp_Rew_Val_List::add_tail_rew(Imp_Rew_Val *new_rew)
{
  return ( add_tail( (Chained_Element *)new_rew) );
}

Imp_Rew_Val *Imp_Rew_Val_List::show_1st_rew()
{
  return ( (Imp_Rew_Val *)show_head() );
}

Imp_Rew_Val *Imp_Rew_Val_List::get_1st_rew()
{
   return ( (Imp_Rew_Val *) del_head() );
}

Imp_Rew_Val *Imp_Rew_Val_List::show_next_rew()
{
  return ( (Imp_Rew_Val *)show_next() );
}

Imp_Rew_Val *Imp_Rew_Val_List::show_curr_rew()
{
  return ( (Imp_Rew_Val *)show_curr() );
}

int Imp_Rew_Val_List::query_rew(Expr_Val *tg_imp_val)
{
    Imp_Rew_Val *rew_aux;
    Expr_Val    *imp_val;

    rew_aux = show_1st_rew();
    
    while (rew_aux != NULL) {
      imp_val = rew_aux->show_value();
      if ( imp_val->comp_expr_val(tg_imp_val) )
	return (1);
      rew_aux = show_next_rew();
    }
    return (0);
}

int Imp_Rew_Val_List::show_no_rew()
{
  return ( show_no_elem() );
}


