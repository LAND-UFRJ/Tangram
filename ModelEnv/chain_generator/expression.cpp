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
*	@file expression.cpp
*   @brief <b> Expressions evaluation methods </b>.
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
#include "gramatica.h"
#include "system_state.h"
#include "system_desc.h"
#include "random.h"
#include "simulator.h"

/* Global simulator_controler */
extern Simulator_Controler *the_simulator_controler;
/* Global simulator_interactive */
extern Simulator_Controler *the_simulator_interactive;

Expression::Expression(int a_fd)
{
  fd = a_fd;
  type = EXEC_TYPE;
  strcpy (expr_code, "");

  debug(4,"Expression::Expression(int): creating object");
}

Expression::Expression(int a_fd, char *a_expr_code, int a_type) 
{
  fd = a_fd;
  type = a_type;
  if (a_expr_code != NULL)
    strcpy (expr_code, a_expr_code);
  else
    strcpy (expr_code, "");

  debug(4,"Expression::Expression(int, char *, int): creating object");
}

Expression::~Expression()
{
  debug(4,"Expression::~Expression(): destroying object");
}

void Expression::ch_fd(int a_fd)
{
  fd = a_fd;
}

int Expression::show_fd()
{
  return(fd);
}

void Expression::ch_str(char *a_expr_code)
{
  if (a_expr_code != NULL)
    strcpy (expr_code, a_expr_code);
}

void Expression::show_str(char *a_expr_code)
{
  strcpy(a_expr_code, expr_code);
}

void Expression::ch_type(int a_type)
{
  type = a_type;
}

int Expression::show_type()
{
  return(type);
}

Expr_Val *Expression::evaluate(Object_State *obj_st)
{
  Object_Description *obj_desc;
  Expr_Val  *expr_val;
  Simulator *simulator = NULL;
  TGFLOAT     value;
  char       str[MAXSTRING];

  if (type == EXEC_TYPE) {
      if ( the_simulator_controler != NULL )
          simulator = the_simulator_controler->show_simulator();
      else if( the_simulator_interactive != NULL )
          simulator = the_simulator_interactive->show_simulator();
      value = function_handler(fd, obj_st, simulator, NULL) ;
      expr_val = new Expr_Val(value);
  } else {
      /* call obj_desc->replace_symbols to substitute variables, constants and parameters */
      obj_desc = obj_st->show_obj_desc();
      strcpy (str, expr_code);
      obj_desc->replace_symbols(obj_st, str);
      
      expr_val = new Expr_Val(str);
  }
  return (expr_val);
}

Expr_Val *Expression::evaluate(Object_State *obj_st, System_State *sys_st)
{
  Object_Description *obj_desc;
  Expr_Val  *expr_val;
  Simulator *simulator = NULL;
  TGFLOAT     value;
  char       str[MAXSTRING];

  if (type == EXEC_TYPE) {
      if ( the_simulator_controler != NULL )
          simulator = the_simulator_controler->show_simulator();
      else if( the_simulator_interactive != NULL )
          simulator = the_simulator_interactive->show_simulator();
      value = function_handler(fd, obj_st, simulator, sys_st) ;
      expr_val = new Expr_Val(value);
  } else {
      /* call obj_desc->replace_symbols to substitute variables, constants and parameters */
      /* only used when sys_st is NULL */
      obj_desc = obj_st->show_obj_desc();
      
      strcpy (str, expr_code);
      obj_desc->replace_symbols(obj_st, str);
      
      expr_val = new Expr_Val(str);
  }
  return (expr_val);
}

TGFLOAT Expression::evaluate_numeric(Object_State *obj_st)
{
  TGFLOAT value;
  Simulator *simulator = NULL;

  if (type == EXEC_TYPE)
  {
      if ( the_simulator_controler != NULL )
          simulator = the_simulator_controler->show_simulator();
      else if( the_simulator_interactive != NULL )
          simulator = the_simulator_interactive->show_simulator();
      value = function_handler(fd, obj_st, simulator, NULL);
      return (value);
  }
  else
    fprintf( stdout , "Expression::evaluate_numeric: Trying to evaluate an expression of INTERP_TYPE.\n");
  return (0);
}

void Expression::print_expression()
{
   fprintf ( stdout , "%d\n", fd);
}


/**********************************************************************/


Expression_List::Expression_List() : Chained_List()
{
  debug(4,"Expression_List::Expression_List(): creating object");
}

Expression_List::Expression_List(Expression_List *expr_list) :
                                 Chained_List( (Chained_List *)expr_list)
{
  debug(4,"Expression_List::Expression_List(Expression_List *): creating object");
}

Expression_List::~Expression_List()
{
  Expression *expr_aux;

  if (access == LIST_RW) {
    expr_aux = get_1st_expr();
    while (expr_aux != NULL) {
      delete expr_aux;
      expr_aux = get_1st_expr();
    }
  }
  debug(4,"Expression_List::~Expression_List(): destroying object");
}

int Expression_List::add_tail_expr(Expression *new_expr)
{
  return ( add_tail( (Chained_Element *)new_expr) );
}

Expression *Expression_List::show_1st_expr()
{
  return ( (Expression *)show_head() );
}

Expression *Expression_List::get_1st_expr()
{
  return ( (Expression *)del_head() );
}

Expression *Expression_List::show_next_expr()
{
  return ( (Expression *)show_next() );
}

Expression *Expression_List::show_curr_expr()
{
  return ( (Expression *)show_curr() );
}

int Expression_List::query_expr(int tg_fd)
{
    Expression *expr_aux;
    int         fd;

    expr_aux = show_1st_expr();
    
    while (expr_aux != NULL) {
      fd = expr_aux->show_fd();
      if ( fd == tg_fd )
	return (1);
      expr_aux = show_next_expr();
    }
    return (0);
}

int Expression_List::show_no_expr()
{
  return ( show_no_elem() );
}

