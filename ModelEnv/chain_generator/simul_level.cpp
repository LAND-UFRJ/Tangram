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
*  @file simul_level.cpp
*  @brief <b>Simulator_Level methods</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  The detailed description is unavailable. 
*/

#include <stdio.h>
#include <pthread.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"
#include "random.h"
#include "simulator.h"
#include "simul_split.h"


Simulator_Level::Simulator_Level(int a_num, int a_splits, int a_concurrancy,
				 State_Variable *low_st, State_Variable *upp_st)
{
   number      = a_num;
   no_splits   = a_splits;
   concurrancy = a_concurrancy;
   low_st_var  = low_st;
   upp_st_var  = upp_st;
   running     = waiting = finished = 0;
   split_list  = new Simulator_Split_List();
} 

Simulator_Level::~Simulator_Level()
{
  /* delete the limiting state variables */
  if (low_st_var != NULL)
    delete low_st_var;
  if (upp_st_var != NULL)
    delete upp_st_var;

  /* delete the split list */
  delete split_list;
}

void Simulator_Level::clear_values()
{
  running = waiting = finished = 0;
}

int Simulator_Level::show_level()
{
  return (number);
}

int Simulator_Level::show_no_splits()
{
  return (no_splits);
}

int Simulator_Level::show_running()
{
  return (running);
}
int Simulator_Level::show_waiting()
{
  return (waiting);
}
int Simulator_Level::show_finished()
{
  return (finished);
}

int Simulator_Level::run_simulator(Simulator *simulator, int total_splits, int last_tree)
{
  Simulator *simul_aux;
  Simulator_Split *simul_split;
  State_Variable *low_st, *upp_st;
  int i, is_last;

  /* set the number of splits of this simualtor */
  if (total_splits == 0)
    total_splits = no_splits;

  /* set the limiting states of this simulator */
  if (low_st_var != NULL)
    low_st = new State_Variable(low_st_var);
  else
    low_st = NULL;
  if (upp_st_var != NULL)
    upp_st = new State_Variable(upp_st_var);
  else
    upp_st = NULL;

  simulator->set_stopping_state(low_st, upp_st);

  /* create the total number of splits */
  for (i=1; i<=total_splits; i++) {
    /* create a new simulator */
    simul_aux = new Simulator(simulator);
    /* set the last_id flag for the simulator */
    if (i == total_splits)
      is_last = 1;
    else
      is_last = 0;
    /* create a new split with this simulator */
    simul_split = new Simulator_Split(simul_aux, number, is_last, last_tree);
    /* update the number of waiting simulators */
    waiting++;
    /* run this simulator if concurrancy has not been reached */
    if (running < concurrancy) {
      /* start the thread for this simulator */
      simul_split->create_thread();
      /* set the new status (just to guarantee) */
      simul_split->ch_status(RUNNING);
      /* update the number of waiting / running simulators */
      waiting--;
      running++;
    }
    /* add this simulator split to the list */
    split_list->add_tail_split(simul_split);
  }
  return(1);
}

void Simulator_Level::sched_simulator()
{
  Simulator_Split *simul_split;
  int end;

  /* put some waiting simulators to run */

  /* check to see if maximum concurrancy has been reached */
  if (running < concurrancy)
    end = 0;
  else
    end = 1;

  while (!end) {
    /* query for a waiting simulator */
    if (split_list->query_split(WAITING)) {
      /* show this split on the list */
      simul_split = split_list->show_curr_split();
      /* start the thread for this simulator */
      simul_split->create_thread();
      /* set the new status (just to guarantee) */
      simul_split->ch_status(RUNNING);
      /* update the number of waiting / running simulators */
      waiting--;
      running++;
    } else
      /* no waiting simulators on the list */
      end = 1;
    /* if max concurrancey reached then end */
    if (running == concurrancy)
      end = 1;
  }
}

Simulator_Split *Simulator_Level::get_finished_split()
{
   Simulator_Split *simul_split;

   /* query for a finished simulator */
   if (split_list->query_split(REMOVE)) {
     /* remove this split from the list */
     simul_split = split_list->get_curr_split();
     /* update the number of running/finished simulators threads */
     running--;
     finished++;
   } else {
     fprintf( stderr, "Simulator_Level::get_finished_simul: No simulator has finished in level %d\n"
	     , number);
     simul_split = NULL;
   }

   return(simul_split);
}

void Simulator_Level::print_level()
{ 
  Simulator_Split  *simul_split;

  fprintf( stdout, "Level: %d --- Splits: %d\n", number, no_splits);
  fprintf( stdout, "Limits: low: ");
  if (low_st_var != NULL)
    low_st_var->print_st_var();
  else
    fprintf( stdout, "NULL");
  fprintf( stdout, " --- upp: ");
  if (upp_st_var != NULL)
    upp_st_var->print_st_var();
  else
    fprintf( stdout, "NULL");
  fprintf( stdout, "\n");
  fprintf( stdout, "Simulators: Running: %d --- Waiting: %d --- Finished: %d\n",
	  running, waiting, finished);

  simul_split = split_list->show_1st_split();
  while (simul_split != NULL) {
    simul_split->print_split();
    simul_split = split_list->show_next_split();
  }

}

/******************************************************************************/


Simulator_Level_List::Simulator_Level_List() : Chained_List()
{
  debug(4,"Simulator_Level_List::Simulator_Level_List(): creating object");
}

Simulator_Level_List::Simulator_Level_List(Simulator_Level_List *ev_list) : Chained_List( (Chained_List *)ev_list)
{
  debug(4,"Simulator_Level_List::Simulator_Level_List(Simulator_Level_List *): creating object");
}

Simulator_Level_List::~Simulator_Level_List()
{
  Simulator_Level *level_aux;

  if (access == LIST_RW) {
    level_aux = get_1st_level();
    while (level_aux != NULL) {
      delete level_aux;
      level_aux = get_1st_level();
    }
  }

  debug(4,"Simulator_Level_List::~Simulator_Level_List(): destroying object");
}

int Simulator_Level_List::add_tail_level(Simulator_Level *new_level)
{
  return ( add_tail( (Chained_Element *)new_level) );
}

Simulator_Level *Simulator_Level_List::show_1st_level()
{
  return ( (Simulator_Level *)show_head() );
}

Simulator_Level *Simulator_Level_List::get_1st_level()
{
  return ( (Simulator_Level *)del_head() );
}

Simulator_Level *Simulator_Level_List::show_next_level()
{
  return ( (Simulator_Level *)show_next() );
}

Simulator_Level *Simulator_Level_List::show_curr_level()
{
  return ( (Simulator_Level *)show_curr() );
}

int Simulator_Level_List::query_level(int tg_level)
{
  Simulator_Level *level_aux;

  level_aux = show_1st_level();
  while (level_aux != NULL){
    if (tg_level == level_aux->show_level())
      return(1);
    else
      level_aux = show_next_level();
  }
  return(0);
}

int Simulator_Level_List::show_no_level()
{
  return ( show_no_elem() );
}


