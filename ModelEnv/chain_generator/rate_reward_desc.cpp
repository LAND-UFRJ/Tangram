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
*  @file rate_reward_desc.cpp
*  @brief <b>Rate reward descriptors</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  The detailed description is unavailable. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"
#include "system_desc.h"

/** the option chosen by the user for rate rewards */
extern int Rate_Rew_Option;

/** the global system description */
extern System_Description *the_system_desc;

/** the name of the model */
extern char   base_filename[MAXSTRING];

/** the global debug level */
extern int    Debug_Level;

//------------------------------------------------------------------------------
Rate_Reward_Desc::Rate_Reward_Desc(char *a_name, int a_id) : Chained_Element()
{
   strcpy (name, a_name);
   id = a_id;
   /* cond_list and rate_list are associate. they get the pair condition and value
   of the reward definition in the user code at tgif*/
   cond_list = new Expression_List();
   rate_list = new Expression_List();
   /* value_list get the ir values set automatically */
   value_list = new Expr_Val_List();
   low_bounded = FALSE;
   upp_bounded = FALSE;
   low_bound = 0;
   upp_bound = 0;
   cr_initial_value = 0;
   /* the ir_user_set_flag variable indicates whether IR (impulse reward) is set
   automatically by the pair condition/rate or IR was set manually by the command set_ir  */
   ir_user_set_flag = FALSE;
   /* ir value set manually by the command set_ir(value); */
   ir_user_set_value = 0;
   /* used to see if this reward belongs to a rate reward sum. */
   sum_bounded = FALSE;
   /* the sum bound that monitor this reward */
   sum_bound = NULL;
   /* flag that indicates the definition of a level */
   has_level = FALSE;
   rew_level = 0;
   /* pointer to the Reward_Measure that keeps the info of this Rate_Reward_Desc */
   rew_meas = NULL;
   /* Initialize last val with 0 */
   /* If the first ir was different from 0, the need_evaluation flag will */
   /*   be set unecessarly. But this could happen just in the first time. */
   last_ir_val = 0;

   debug(4,"Rate_Reward_Desc::Rate_Reward_Desc(): creating object");
}
//------------------------------------------------------------------------------
Rate_Reward_Desc::~Rate_Reward_Desc()
{
  /* free memory from condition list */
  delete cond_list;

  /* free memory from rate list */
  delete rate_list;

  /* free memory from value list */
  delete value_list;

  debug(4,"Rate_Reward_Desc::~Rate_Reward_Desc(): destroying object");
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::show_name(char *rew_name)
{
  strcpy (rew_name, name);
}
//------------------------------------------------------------------------------
int Rate_Reward_Desc::show_id()
{
  return(id);
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::ch_name(char *rew_name)
{
  strcpy (name, rew_name);
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::add_cond(Expression *new_cond)
{
  if (new_cond != NULL)
    cond_list->add_tail_expr(new_cond);
  else
    debug(3,"Rate_Reward_Desc::add_cond: Can't add NULL pointer");
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::add_rate(Expression *new_rate)
{
  if (new_rate != NULL)
    rate_list->add_tail_expr(new_rate);
  else
    debug(3,"Rate_Reward_Desc::add_rate: Can't add NULL pointer");
}
//------------------------------------------------------------------------------
Expr_Val *Rate_Reward_Desc::eval_rate(Object_State *obj_st)
{
   return(eval_rate(obj_st, NULL));
}
//------------------------------------------------------------------------------
Expr_Val *Rate_Reward_Desc::eval_rate(Object_State *obj_st, System_State *sys_st)
{
    Object_Description *obj_desc;
    Expression  *rate;
    Expression  *cond;
    Expr_Val    *rate_val, *cond_val;
    TGFLOAT      num_rate_val;
    int          no_eval_rates;
    char         rew_name[MAXSTRING];
    char         obj_name[MAXSTRING];

    /* get the first rate/cond */
    rate = rate_list->show_1st_expr();
    cond = cond_list->show_1st_expr();
    /* go through all rates  */
    while (cond != NULL)
    {
        /* if condition is true save the rate in rate_values vector */
        cond_val = cond->evaluate(obj_st, sys_st);
        if ( ((int) cond_val->show_value()) == TRUE)
        {
            rate_val = rate->evaluate(obj_st, sys_st);
            value_list->add_tail_expr_val(rate_val);
        }
        delete cond_val;
        /* go to the next rate/cond */
        rate = rate_list->show_next_expr();
        cond = cond_list->show_next_expr();
    }
    /* get the number of rates evaluated as TRUE */
    no_eval_rates = value_list->show_no_expr_val();

    /* if the system is LITERAL, than no enhanced checking over the rate values is done */
    if (the_system_desc->show_chain_type() == LITERAL)
    {
        /* check to see if there was more than one TRUE condition */
        if (no_eval_rates > 1)
        {
            this->show_name(rew_name);
            obj_desc = obj_st->show_obj_desc();
            obj_desc->show_name(obj_name);
            fprintf ( stderr, "ERROR: Reward %s evaluated TRUE %d times in object %s with the object state: ",rew_name, no_eval_rates, obj_name);
            obj_st->print_obj_st();
            exit(-1);
        }

        /* if only one condition evaluated TRUE */
        if (no_eval_rates == 1)
            rate_val = value_list->get_1st_expr_val();
        else
            rate_val = new Expr_Val(0.0);

        /* free the memory from the rate_values vector */
        value_list->clear_expr_val();

        return (rate_val);
    }

    /* the chain is NUMERICAL */
    /* if there was at least one rate evaluating to TRUE */
    if (no_eval_rates > 1)
    {
        /* choose adequate reward option */
        num_rate_val = 0;
        switch (Rate_Rew_Option)
        {
            case RATE_MEAN  :
                num_rate_val = 0;
                rate_val = value_list->show_1st_expr_val();
                while (rate_val != NULL)
                {
                    num_rate_val += rate_val->show_value();
                    rate_val = value_list->show_next_expr_val();
                }
                num_rate_val = num_rate_val / no_eval_rates;
            break;
            case RATE_MAX   :
                rate_val = value_list->show_1st_expr_val();
                num_rate_val = rate_val->show_value();
                rate_val = value_list->show_next_expr_val();
                while (rate_val != NULL)
                {
                    if (rate_val->show_value() > num_rate_val)
                    num_rate_val = rate_val->show_value();
                    rate_val = value_list->show_next_expr_val();
                }
            break;
            case RATE_MIN   :
                rate_val = value_list->show_1st_expr_val();
                num_rate_val = rate_val->show_value();
                rate_val = value_list->show_next_expr_val();
                while (rate_val != NULL)
                {
                    if (rate_val->show_value() < num_rate_val)
                        num_rate_val = rate_val->show_value();
                    rate_val = value_list->show_next_expr_val();
                }
            break;
            case RATE_ERROR :
                this->show_name(rew_name);
                obj_desc = obj_st->show_obj_desc();
                obj_desc->show_name(obj_name);
                fprintf ( stderr, "ERROR: Reward %s evaluated TRUE %d times in object %s with the object state: ",rew_name, no_eval_rates, obj_name);
                obj_st->print_obj_st();
                fprintf ( stdout, "\n");
                exit(-1);
        }
        rate_val = new Expr_Val(num_rate_val);
    }
    else
    {
        /* end the if */
        /* if only one or none values were calculated */
        if (no_eval_rates == 1)
            rate_val = value_list->get_1st_expr_val();
        else
            rate_val = new Expr_Val(0.0);
    }
    /* free the memory from the rate_values vector */
    value_list->clear_expr_val();

    return (rate_val);
}
//------------------------------------------------------------------------------
int Rate_Reward_Desc::read_rew_level(const char *obj_name)
{
  int   no_levels;
  TGFLOAT level;
  FILE *fd;
  char  filename[MAXSTRING];

  sprintf (filename, "%s.reward_levels.%s.%s", base_filename, obj_name, name);
  if ( (fd = fopen(filename, "r")) == NULL) {
    if (Debug_Level > 0) {
      perror ("fopen");
      fprintf ( stderr, "WARNING: No reward level specified \n");
    }
    return (0);
  }

  fscanf (fd, "%d\n", &no_levels);
  if (no_levels > 0)
    fscanf (fd, "%lf\n", &level);

  ch_has_level( TRUE );
  ch_rew_level((TGFLOAT) level);
  fclose (fd);

  return (1);
}
//------------------------------------------------------------------------------
TGFLOAT Rate_Reward_Desc::show_rew_level()
{
  return (rew_level);
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::ch_rew_level(TGFLOAT new_level)
{
   rew_level = new_level;
}
//------------------------------------------------------------------------------
int Rate_Reward_Desc::show_has_level()
{
  return ( has_level );
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::ch_has_level( int a_has_level )
{
   has_level = a_has_level;
}
//------------------------------------------------------------------------------
TGFLOAT Rate_Reward_Desc::show_low_bound()
{
  return (low_bound);
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::ch_low_bound(TGFLOAT new_bound)
{
   low_bound = new_bound;
}
//------------------------------------------------------------------------------
TGFLOAT Rate_Reward_Desc::show_upp_bound()
{
  return (upp_bound);
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::ch_upp_bound(TGFLOAT new_bound)
{
   upp_bound = new_bound;
}
//------------------------------------------------------------------------------
int Rate_Reward_Desc::is_low_bounded()
{
  return (low_bounded);
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::ch_low_bounded(int bounded)
{
  low_bounded = bounded;
}
//------------------------------------------------------------------------------
int Rate_Reward_Desc::is_upp_bounded()
{
  return (upp_bounded);
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::ch_upp_bounded(int bounded)
{
  upp_bounded = bounded;
}
//------------------------------------------------------------------------------
int Rate_Reward_Desc::is_sum_bounded()
{
    return (sum_bounded);
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::ch_sum_bounded(int bounded)
{
    sum_bounded = bounded;
}
//------------------------------------------------------------------------------
TGFLOAT Rate_Reward_Desc::show_last_ir_val()
{
    return ( last_ir_val );
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::ch_last_ir_val( TGFLOAT new_ir )
{
    last_ir_val = new_ir;
}
//------------------------------------------------------------------------------
Rate_Reward_Sum *Rate_Reward_Desc::show_sum_bound()
{
    return (sum_bound);
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::ch_sum_bound(Rate_Reward_Sum *new_bound)
{
   sum_bound = new_bound;
}
//------------------------------------------------------------------------------
Reward_Measure *Rate_Reward_Desc::show_rew_meas()
{
    return (rew_meas);
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::ch_rew_meas(Reward_Measure *new_rew)
{
   rew_meas = new_rew;
}
//------------------------------------------------------------------------------
TGFLOAT Rate_Reward_Desc::show_cr_initial_value()
{
  return (cr_initial_value);
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::ch_cr_initial_value(TGFLOAT new_value)
{
   cr_initial_value = new_value;
}
//------------------------------------------------------------------------------
int Rate_Reward_Desc::show_ir_user_set_flag()
{
  return (ir_user_set_flag);
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::ch_ir_user_set_flag(int opt)
{
  ir_user_set_flag = opt;
}
//------------------------------------------------------------------------------
TGFLOAT Rate_Reward_Desc::show_ir_user_set_value()
{
  return (ir_user_set_value);
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::ch_ir_user_set_value(TGFLOAT value)
{
  ir_user_set_value = value;
}
//------------------------------------------------------------------------------
void Rate_Reward_Desc::print_reward()
{
  Expression *cond;
  Expression *rate;

  fprintf ( stdout, "Reward:\n");
  fprintf ( stdout, "\tname: %s\n", name);

  fprintf ( stdout, "\tbounded: low: %d --- upp: %d\n", low_bounded, upp_bounded);
  fprintf ( stdout, "\tlow_bound: %.5E\n", low_bound);
  fprintf ( stdout, "\tupp_bound: %.5E\n", upp_bound);

  fprintf ( stdout, "\tcond: ");
  cond = cond_list->show_1st_expr();
  while (cond != NULL) {
    fprintf ( stdout, "%d ", cond->show_fd());
    cond = cond_list->show_next_expr();
  }

  fprintf ( stdout, "\n\trate: ");
  rate = rate_list->show_1st_expr();
  while (rate != NULL) {
    fprintf ( stdout, "%d ", rate->show_fd());
    rate = rate_list->show_next_expr();
  }
  fprintf ( stdout, "\n");
}
//------------------------------------------------------------------------------

/************************************************************************/

Rate_Reward_Desc_List::Rate_Reward_Desc_List() : Chained_List()
{
  debug(4,"Rate_Reward_Desc_List::Rate_Reward_Desc_List(): creating object");
}

Rate_Reward_Desc_List::Rate_Reward_Desc_List(Rate_Reward_Desc_List *rew_list) : 
                                                 Chained_List( (Chained_List *)rew_list)
{
  debug(4,"Rate_Reward_Desc_List::Rate_Reward_Desc_List(Rate_Reward_Desc_List *): creating object");
}

Rate_Reward_Desc_List::~Rate_Reward_Desc_List()
{
  Rate_Reward_Desc *rew_aux;

  if (access == LIST_RW) {
    rew_aux = get_1st_rew();
    while (rew_aux != NULL) {
      delete rew_aux;
      rew_aux = get_1st_rew();
    }
  }
  
  debug(4,"Rate_Reward_Desc_List::~Rate_Reward_Desc_List(): destroying object");
}

int Rate_Reward_Desc_List::add_tail_rew(Rate_Reward_Desc *new_rew)
{
  return ( add_tail( (Chained_Element *)new_rew) );
}

Rate_Reward_Desc *Rate_Reward_Desc_List::show_1st_rew()
{
  return ( (Rate_Reward_Desc *)show_head() );
}

Rate_Reward_Desc *Rate_Reward_Desc_List::get_1st_rew()
{
  return ( (Rate_Reward_Desc *)del_head() );
}

Rate_Reward_Desc *Rate_Reward_Desc_List::show_next_rew()
{
  return ( (Rate_Reward_Desc *)show_next() );
}

Rate_Reward_Desc *Rate_Reward_Desc_List::show_curr_rew()
{
  return ( (Rate_Reward_Desc *)show_curr() );
}

//-----------------------------------------------------------------------------
int Rate_Reward_Desc_List::query_rew(char *tg_rew_name)
{
    Rate_Reward_Desc *rew_aux;
    char              name[MAXSTRING];

    rew_aux = show_1st_rew();
    while (rew_aux != NULL)
    {
        rew_aux->show_name(name);
        if ( !strcmp(tg_rew_name, name) )
	    return (1);
        rew_aux = show_next_rew();
    }
    return (0);
}
//-----------------------------------------------------------------------------
int Rate_Reward_Desc_List::show_no_rew()
{
    return ( show_no_elem() );
}
//-----------------------------------------------------------------------------

