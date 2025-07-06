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
*	@file simul_split.cpp
*   @brief <b> Simulator_Split methods </b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @version 2.0
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram II project
*   @since version 1.0
*
*/

#include <stdio.h>
#include <pthread.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "random.h"
#include "simulator.h"
#include "simul_split.h"

/** @name pthread_mutex_t
 *  simulators lock 
 */
/*@{*/
extern pthread_mutex_t simul_lock;
extern pthread_mutex_t sched_lock;
/*@}*/

/** @name pthread_cond_t
 *  signal to scheduler 
 */
/*@{*/
extern pthread_cond_t simul_sig;
extern pthread_cond_t sched_sig;
/*@}*/

/** level of finished simulator */
extern int simul_finished_level;

void *thread_runner(void *arg)
{
  Simulator_Split *simul_split;

  simul_split = (Simulator_Split *)arg;
  if (simul_split->run_simulator() < 0)
    fprintf( stderr, "thread_runner: ERROR: Simulator didn't run ok.\n");

  return(NULL);
}

Simulator_Split::Simulator_Split(Simulator *a_simul, int a_level, 
				 int a_last, int a_last_tree)
{
   simulator = a_simul;
   level     = a_level;
   last      = a_last;
   last_tree = a_last_tree;
   status    = WAITING;
   thread_id = 0;
}

Simulator_Split::~Simulator_Split()
{
   /* wait for the joining of the thread */
   if (thread_id != 0)
     if (pthread_join(thread_id, NULL) != 0)
       perror ("pthread_join");
}

Simulator *Simulator_Split::show_simulator()
{
  return (simulator);
}

int Simulator_Split::show_status()
{
  return (status);
}

void Simulator_Split::ch_status(int new_status)
{
  status = new_status;
}

pthread_t Simulator_Split::show_thread_id()
{
  return (thread_id);
}

int Simulator_Split::is_last()
{
  return (last);
}
int Simulator_Split::show_last_tree()
{
  return (last_tree);
}

int Simulator_Split::create_thread()
{
  pthread_attr_t attr;
  
  /* initialize attr with default attributes */
  if (pthread_attr_init(&attr) != 0) {
    perror("pthread_attr_init");
    return (-1);
  }

  /* system-wide contention */
  if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM) != 0) {
    perror("pthread_attr_setscope");
    return (-1);
  }

  /* create a new thread */
/*  static int n=0;*/
  if ( pthread_create(&thread_id, &attr, thread_runner, (void *)this) != 0) {
    perror ("pthread_create");
    return (-1);
  }

  /* destroy the initialization object attribute */
  if (pthread_attr_destroy(&attr) != 0) {
    perror("pthread_attr_destroy");
    return(-1);
  }

  return(1);
}

int Simulator_Split::run_simulator()
{
  /* change the status to running */
  status = RUNNING;

  /* simulate the model in batch */
  simulator->simulate_model_batch();

  /* change the status to finished */
  status = FINISHED;

  /* get the simulators mutex */
  if (pthread_mutex_lock(&simul_lock) != 0)
    perror ("pthread_mutex_lock");

  /* get the sched mutex */
  if (pthread_mutex_lock(&sched_lock) != 0)
    perror ("pthread_mutex_lock");

  /* set the global level variable */
  simul_finished_level = level;

  /* change the status to finished */
  status = REMOVE;

  /* wake up the scheduler */
  if (pthread_cond_signal(&sched_sig) != 0)
    perror ("pthread_cond_signal");

  /* free the mutex and wait for the signal */
  if (pthread_cond_wait(&simul_sig, &sched_lock) != 0)
    perror ("pthread_cond_wait");

  /* free the mutex */
  if (pthread_mutex_unlock(&simul_lock) != 0)
    perror ("pthread_mutex_unlock");

  return (1);
}

void Simulator_Split::print_split()
{
   fprintf( stdout, "\tLevel: %d --- ", level);
   fprintf( stdout, "Status: %d --- ", status);
   fprintf( stdout, "Thread_id: %d\n", (unsigned long)thread_id);
   //   printf ("System_State: ");
   //   simulator->print_curr_st();
   //   printf ("\n");
}


/******************************************************************************/


Simulator_Split_List::Simulator_Split_List() : Chained_List()
{
  debug(4,"Simulator_Split_List::Simulator_Split_List(): creating object");
}

Simulator_Split_List::Simulator_Split_List(Simulator_Split_List *ev_list) : Chained_List( (Chained_List *)ev_list)
{
  debug(4,"Simulator_Split_List::Simulator_Split_List(Simulator_Split_List *): creating object");
}

Simulator_Split_List::~Simulator_Split_List()
{
  Simulator_Split *split_aux;

  if (access == LIST_RW) {
    split_aux = get_1st_split();
    while (split_aux != NULL) {
      delete split_aux;
      split_aux = get_1st_split();
    }
  }

  debug(4,"Simulator_Split_List::~Simulator_Split_List(): destroying object");
}

int Simulator_Split_List::add_tail_split(Simulator_Split *new_split)
{
  return ( add_tail( (Chained_Element *)new_split) );
}

Simulator_Split *Simulator_Split_List::show_1st_split()
{
  return ( (Simulator_Split *)show_head() );
}

Simulator_Split *Simulator_Split_List::get_1st_split()
{
  return ( (Simulator_Split *)del_head() );
}

Simulator_Split *Simulator_Split_List::get_curr_split()
{
  return ( (Simulator_Split *)del_elem() );
}

Simulator_Split *Simulator_Split_List::show_next_split()
{
  return ( (Simulator_Split *)show_next() );
}

Simulator_Split *Simulator_Split_List::show_curr_split()
{
  return ( (Simulator_Split *)show_curr() );
}

int Simulator_Split_List::query_split(int tg_status)
{
  Simulator_Split *split_aux;

  split_aux = show_1st_split();
  while (split_aux != NULL){
    if (tg_status == split_aux->show_status())
      return(1);
    else
      split_aux = show_next_split();
  }
  return(0);
}

int Simulator_Split_List::show_no_split()
{
  return ( show_no_elem() );
}



