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
*  @file simul_run.cpp
*  @brief <b>Simulator_Run methods</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  The detailed description is unavailable. 
*/

#include <stdio.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "random.h"
#include "simulator.h"
#ifdef TG_PARALLEL
#include <pvm3.h>
#endif

Simulator_Run::Simulator_Run(int a_run, TGFLOAT a_time, int a_trans)
{
  no_run = a_run;
  simul_time = a_time;
  no_transitions = a_trans;
  rate_rew_list = NULL;
  imp_rew_list = NULL;
}

#ifdef TG_PARALLEL
Simulator_Run::Simulator_Run( int a_run )
{
  no_run = a_run;
  simul_time = 0;
  no_transitions = 0;
  rate_rew_list = NULL;
  imp_rew_list = NULL;
}
#endif

Simulator_Run::~Simulator_Run()
{
  /* delete the rate reward list if any */
  if (rate_rew_list != NULL)
    delete rate_rew_list;

  /* delete the impulse reward list if any */
  if (imp_rew_list != NULL)
    delete imp_rew_list;

}

int Simulator_Run::show_run()
{
  return (no_run);
}

TGFLOAT Simulator_Run::show_simul_time()
{
  return (simul_time);
}

int Simulator_Run::show_no_transitions()
{
  return (no_transitions);
}

int Simulator_Run::ch_rate_rew_list(Reward_Measure_List *rew_list)
{
    if (rew_list == NULL) {
      debug(3, "Simulator_Run::ch_rate_rew_list: Can't change list to NULL pointer");
      return(-1);
    }

    if (rate_rew_list == NULL)
      rate_rew_list = rew_list;
    else {
      delete rate_rew_list;
      rate_rew_list = rew_list;
    }
    return(1);
}

#ifdef TG_PARALLEL
void Simulator_Run::send_context()
{
    int has_rate_rew_list, has_imp_rew_list;

    pvm_pkint(&no_run,1,1);
    PVM_PKTGFLOAT(&simul_time,1,1);
    pvm_pkint(&no_transitions,1,1);
    has_rate_rew_list = (rate_rew_list != NULL);
    has_imp_rew_list = (imp_rew_list != NULL);
    pvm_pkint(&has_rate_rew_list,1,1);
    pvm_pkint(&has_imp_rew_list,1,1);
    if (has_rate_rew_list)
      rate_rew_list->send_context();
    if (has_imp_rew_list)
      imp_rew_list->send_context();
}

void Simulator_Run::recv_context()
{
    int remote_run;
    int has_rate_rew_list, has_imp_rew_list;

    // relocate remote runs
    pvm_upkint(&remote_run,1,1);
    no_run = no_run + remote_run;

    PVM_UPKTGFLOAT(&simul_time,1,1);
    pvm_upkint(&no_transitions,1,1);
    pvm_upkint(&has_rate_rew_list,1,1);
    pvm_upkint(&has_imp_rew_list,1,1);
    if (has_rate_rew_list)
    {
      if (rate_rew_list == NULL)
        rate_rew_list = new Reward_Measure_List();
      if (rate_rew_list != NULL)
        rate_rew_list->recv_context();
      else fprintf(stderr,"Out of memory in void Simulator_Run::recv_context()\n");
    }
    if (has_imp_rew_list)
    {
      if (imp_rew_list == NULL)
        imp_rew_list = new Reward_Measure_List();
      if (imp_rew_list != NULL)
        imp_rew_list->recv_context();
      else fprintf(stderr,"Out of memory in void Simulator_Run::recv_context()\n");
    }
}
#endif

Reward_Measure_List *Simulator_Run::show_rate_rew_list(int access)
{
  Reward_Measure_List *rew_list_aux;

  if (access == LIST_RW)
    return (rate_rew_list);
  else {
    rew_list_aux = new Reward_Measure_List(rate_rew_list);
    return (rew_list_aux);
  }

}

int Simulator_Run::ch_imp_rew_list(Reward_Measure_List *rew_list)
{
    if (rew_list == NULL) {
      debug(3, "Simulator_Run::ch_rate_rew_list: Can't change list to NULL pointer");
      return(-1);
    }

    if (imp_rew_list == NULL)
      imp_rew_list = rew_list;
    else {
      delete imp_rew_list;
      imp_rew_list = rew_list;
    }
    return(1);
}

Reward_Measure_List *Simulator_Run::show_imp_rew_list(int access)
{
  Reward_Measure_List *rew_list_aux;

  if (access == LIST_RW)
    return (imp_rew_list);
  else {
    rew_list_aux = new Reward_Measure_List(imp_rew_list);
    return (rew_list_aux);
  }

}

/************************************************************************/

Simulator_Run_List::Simulator_Run_List() : Chained_List()
{
   debug(4,"Simulator_Run_List::Simulator_Run_List(): creating object");
}

Simulator_Run_List::Simulator_Run_List(Simulator_Run_List *simul_run_list) : Chained_List( (Chained_List *)simul_run_list)
{

   debug(4,"Simulator_Run_List::Simulator_Run_List(Simulator_Run_List *): creating object");
}

Simulator_Run_List::~Simulator_Run_List()
{
  Simulator_Run *simul_run_aux;

  if (access == LIST_RW) {
    simul_run_aux = get_1st_run();
    while (simul_run_aux != NULL) {
      delete simul_run_aux;
      simul_run_aux = get_1st_run();
    }
  }

   debug(4,"Simulator_Run_List::~Simulator_Run_List(): destroying object");
}

int Simulator_Run_List::add_tail_run(Simulator_Run *new_run)
{
  return ( add_tail( (Chained_Element *)new_run) );
}

Simulator_Run *Simulator_Run_List::show_1st_run()
{
  return ( (Simulator_Run *)show_head() );
}

Simulator_Run *Simulator_Run_List::get_1st_run()
{
  return ( (Simulator_Run *)del_head() );
}

Simulator_Run *Simulator_Run_List::show_next_run()
{
  return ( (Simulator_Run *)show_next() );
}

Simulator_Run *Simulator_Run_List::show_curr_run()
{
  return ( (Simulator_Run *)show_curr() );
}

int Simulator_Run_List::query_run(int tg_run_id)
{
    Simulator_Run  *simul_run_aux;

    simul_run_aux = show_1st_run();
    while (simul_run_aux != NULL) {
      if (simul_run_aux->show_run() == tg_run_id)
	return (1);
      simul_run_aux = show_next_run();
    }

    return (0);
}

int Simulator_Run_List::show_no_run()
{
  return ( show_no_elem() );
}

