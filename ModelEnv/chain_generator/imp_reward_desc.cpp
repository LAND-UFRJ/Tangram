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
*	@file imp_reward_desc.cpp
*   @brief <b>Impulse Reward Descriptors</b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram-II project
*   @since version 1.0
*
*   The detailed description is unavailable. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"
#include "system_desc.h"

/* the option chosen by the user for rate rewards */
extern int Imp_Rew_Option;

//------------------------------------------------------------------------------
Imp_Reward_Desc::Imp_Reward_Desc(char *a_name, int a_id) : Chained_Element()
{
    strcpy (name, a_name);
    id = a_id;
    impulse = NULL;
    low_bounded = FALSE;
    upp_bounded = FALSE;
    low_bound = 0;
    upp_bound = 0;
    cr_initial_value = 0;
    rew_meas = NULL;

    debug(4,"Imp_Reward_Desc::Imp_Reward_Desc(char *): creating object");
}
//------------------------------------------------------------------------------
Imp_Reward_Desc::~Imp_Reward_Desc()
{
    /* free memory from impulse */
    if (impulse != NULL)
        delete impulse;

    debug(4,"Imp_Reward_Desc::~Imp_Reward_Desc(): destroying object");
}
//------------------------------------------------------------------------------
void Imp_Reward_Desc::show_name(char *rew_name)
{
  strcpy (rew_name, name);
}

int Imp_Reward_Desc::show_id()
{
  return (id);
}

void Imp_Reward_Desc::ch_name(char *rew_name)
{
  strcpy (name, rew_name);
}

void Imp_Reward_Desc::ch_impulse(Expression *new_imp)
{
  if (new_imp != NULL) {
    /* delete the old impulse */
    if (impulse != NULL)
      delete impulse;
    /* assing a new one */
    impulse = new_imp;
  }
  else   
    debug(3,"Imp_Reward_Desc::ch_impulse: Can't add NULL pointer");
}

Expr_Val *Imp_Reward_Desc::eval_impulse(Object_State *obj_st)
{ 
  Expr_Val *imp_val;

  if (impulse != NULL)
    imp_val = impulse->evaluate(obj_st);
  else {
   imp_val = new Expr_Val(0.0);
   debug(3,"Imp_Reward_Desc::eval_impulse: Can't evaluate a NULL impulse pointer");
  }
  return (imp_val);
}

TGFLOAT Imp_Reward_Desc::show_low_bound()
{
  return (low_bound);
}

void Imp_Reward_Desc::ch_low_bound(TGFLOAT new_bound)
{
   low_bound = new_bound;
}

TGFLOAT Imp_Reward_Desc::show_upp_bound()
{
  return (upp_bound);
}

void Imp_Reward_Desc::ch_upp_bound(TGFLOAT new_bound)
{
   upp_bound = new_bound;
}
//------------------------------------------------------------------------------
TGFLOAT Imp_Reward_Desc::show_cr_initial_value()
{
  return (cr_initial_value);
}
//------------------------------------------------------------------------------
void Imp_Reward_Desc::ch_cr_initial_value( TGFLOAT new_value )
{
   cr_initial_value = new_value;
}
//------------------------------------------------------------------------------
Reward_Measure *Imp_Reward_Desc::show_rew_meas()
{
  return ( rew_meas );
}
//------------------------------------------------------------------------------
void Imp_Reward_Desc::ch_rew_meas( Reward_Measure *a_rew_meas )
{
  rew_meas = a_rew_meas;
}
//------------------------------------------------------------------------------
int Imp_Reward_Desc::is_low_bounded()
{
  return (low_bounded);
}

void Imp_Reward_Desc::ch_low_bounded(int bounded)
{
  low_bounded = bounded;
}

int Imp_Reward_Desc::is_upp_bounded()
{
  return (upp_bounded);
}

void Imp_Reward_Desc::ch_upp_bounded(int bounded)
{
  upp_bounded = bounded;
}

void Imp_Reward_Desc::print_reward() 
{
  fprintf ( stdout, "Reward:\n");
  fprintf ( stdout, "\tname: %s\n", name);

  fprintf ( stdout, "\tbounded: low: %d --- upp: %d\n", low_bounded, upp_bounded);
  fprintf ( stdout, "\tlow_bound: %.5E\n", low_bound);
  fprintf ( stdout, "\tupp_bound: %.5E\n", upp_bound);

  if (impulse != NULL)
    fprintf ( stdout, "\tfd: %d\n", impulse->show_fd() );
}


/************************************************************************/

Imp_Reward_Desc_List::Imp_Reward_Desc_List() : Chained_List()
{
   debug(4,"Imp_Reward_Desc_List::Imp_Reward_Desc_List(): creating object");
}

Imp_Reward_Desc_List::Imp_Reward_Desc_List(Imp_Reward_Desc_List *rew_list) : 
                                                 Chained_List( (Chained_List *)rew_list)
{
   debug(4,"Imp_Reward_Desc_List::Imp_Reward_Desc_List(Imp_Reward_Desc_List *): creating object");
}

Imp_Reward_Desc_List::~Imp_Reward_Desc_List()
{
  Imp_Reward_Desc *rew_aux;

  if (access == LIST_RW) {
    rew_aux = get_1st_rew();
    while (rew_aux != NULL) {
      delete rew_aux;
      rew_aux = get_1st_rew();
    }
  }
  debug(4,"Imp_Reward_Desc_List::~Imp_Reward_Desc_List(): destroying object");
}

int Imp_Reward_Desc_List::add_tail_rew(Imp_Reward_Desc *new_rew)
{
  return ( add_tail( (Chained_Element *)new_rew) );
}

Imp_Reward_Desc *Imp_Reward_Desc_List::show_1st_rew()
{
  return ( (Imp_Reward_Desc *)show_head() );
}

Imp_Reward_Desc *Imp_Reward_Desc_List::get_1st_rew()
{
  return ( (Imp_Reward_Desc *)del_head() );
}

Imp_Reward_Desc *Imp_Reward_Desc_List::show_next_rew()
{
  return ( (Imp_Reward_Desc *)show_next() );
}

Imp_Reward_Desc *Imp_Reward_Desc_List::show_curr_rew()
{
  return ( (Imp_Reward_Desc *)show_curr() );
}

int Imp_Reward_Desc_List::query_rew(char *tg_rew_name)
{
    Imp_Reward_Desc *rew_aux;
    char                name[MAXSTRING];

    rew_aux = show_1st_rew();
    
    while (rew_aux != NULL) {
      rew_aux->show_name(name);
      if ( !strcmp(tg_rew_name, name) )
	return (1);
      rew_aux = show_next_rew();
    }
    return (0);
}

int Imp_Reward_Desc_List::show_no_rew()
{
  return ( show_no_elem() );
}

