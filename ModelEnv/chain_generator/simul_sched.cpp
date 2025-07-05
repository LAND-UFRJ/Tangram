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
*  @file simul_sched.cpp
*  @brief <b>Simulator_Scheduler methods</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  The detailed description is unavailable. 
*/

#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"
#include "system_desc.h"
#include "random.h"
#include "simulator.h"
#include "simul_split.h"

/** The global base filename of the model */
extern char                base_filename[MAXSTRING];
/** The global system description */
extern System_Description *the_system_desc;
/** the debug level variable */
extern int Debug_Level;

/** @name pthread_mutex_t
 *  simulators lock 
 */
/*@{*/
pthread_mutex_t simul_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sched_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t simulator_lock = PTHREAD_MUTEX_INITIALIZER;
/*@}*/

/** @name pthread_cond_t
 *  signal to scheduler 
 */
/*@{*/
pthread_cond_t simul_sig   = PTHREAD_COND_INITIALIZER;
pthread_cond_t sched_sig   = PTHREAD_COND_INITIALIZER;
/*@}*/

/** level of finished simulator */
int simul_finished_level   = 0;


Simulator_Scheduler::Simulator_Scheduler(Simulator *a_simul)
{
   /* set the initial simulator */
   simulator = new Simulator (a_simul);
   
   /* create an empty level list */
   level_list = new Simulator_Level_List();

   /* create an empty reward measure list */
   rate_rew_list  = new Reward_Measure_List();

   /* initialize the product of the number of splits */
   mult_splits = 1.0;

   /* initialize the last splitting level */
   last_level = 1;
}

Simulator_Scheduler::~Simulator_Scheduler()
{
   /* delete the simulator */
   delete simulator;

   /* delete the level list */
   delete level_list;

   /* delete the reward measures */
   delete rate_rew_list;
}

void Simulator_Scheduler::reset()
{
   Reward_Measure  *rew_meas;
   Simulator_Level *level;

   /* reset the simulator to its initial state*/
   simulator->reset(0);

   /* clear the values in the rate reward list */
   rew_meas = rate_rew_list->show_1st_rew();
   while (rew_meas != NULL) {
     rew_meas->clear_values(0,0.0);
     rew_meas = rate_rew_list->show_next_rew();
   }

   /* clear the level list */
   level = level_list->show_1st_level();
   while (level != NULL) {
     level->clear_values();
     level = level_list->show_next_level();
   }

}


Simulator *Simulator_Scheduler::show_simulator()
{
  return(simulator);
}

Reward_Measure_List *Simulator_Scheduler::show_rate_rew_list(int access)
{
  Reward_Measure_List *rew_list_aux;

  if (access == LIST_RW)
    return (rate_rew_list);
  else {
    rew_list_aux = new Reward_Measure_List(rate_rew_list);
    return (rew_list_aux);
  }
}

int Simulator_Scheduler::create_level_list()
{
    char  filename[MAXSTRING];
    FILE *fd;
    Object_Description *obj_desc;
    char   obj_st_var_name[MAXSTRING];
    char  *obj_name;
    char  *st_var_name;
    int    obj_id, st_var_id;
    State_Variable *st_low, *st_upp;
    Simulator_Level *simul_level;
    int    state, prev_state, splits, prev_splits, level;

    /* open the file with level definition */
    sprintf (filename, "%s.threshold", base_filename);
    if ( (fd = fopen (filename, "r")) == NULL) {
      perror ("fopen");
      fprintf ( stderr, "ERROR: while openning the %s file.\n", filename);
      return(-1);
    }

    /* read the first line from the file */
    strcpy(obj_st_var_name, "");
    fscanf(fd, "%s\n", obj_st_var_name);

    /* parse the name of the object and the name of the state var */
    if (strcmp(obj_st_var_name, "")) {
      obj_name    = strtok(obj_st_var_name, ".");
      st_var_name = strtok(NULL, ".");
    } else {
      fprintf ( stderr, "ERROR: Invalid format while reading the %s file\n", filename);
      return(-1);
    }

    /* get the obj id and the state var id */
    obj_desc = the_system_desc->show_obj_desc(obj_name);
    obj_id = obj_desc->show_id();
    st_var_id = obj_desc->show_symbol_code(st_var_name);

    state = 0;
    level = 0;
    prev_state = 0;
    prev_splits = 1;
    while (!feof(fd)) {
      /* read from the file the threshold */
      fscanf(fd, "%d %d\n", &state, &splits);
      /* error checking */
      if ((state == 0) || (splits == 0)) {
	fprintf ( stderr, "ERROR: Invalid format while reading the %s file\n", filename);
	return (-1);
      }
      /* check to see if thresholds are in increasing order */
      if (prev_state > state) {
	fprintf ( stderr, "ERROR: Invalid order of states while reading the %s file\n", filename);
	return (-1);
      }
      /* create the lower state variable limit */
      st_low = new State_Variable (prev_state - 1, st_var_id);
      /* create the upper state variable limit */
      st_upp = new State_Variable (state, st_var_id);
      /* create a new level  and insert it in the list */
      simul_level = new Simulator_Level (level, prev_splits, MAX_CONCURRANCY,
					 st_low, st_upp);
      level_list->add_tail_level (simul_level);
      /* update the productory of the no splits */
      mult_splits *= prev_splits;
      /* go to the next level */
      level++;
      prev_state  = state;
      prev_splits = splits;
    }
    /* create the last level */
    /* create the lower state variable limit */
    st_low = new State_Variable (prev_state - 1, st_var_id);
    /* create the upper state variable limit */
    st_upp = NULL;
    /* create a new level  and insert it in the list */
    simul_level = new Simulator_Level (level, prev_splits, MAX_CONCURRANCY,
				       st_low, st_upp);
    level_list->add_tail_level (simul_level);
    /* update the productory of the no splits */
    mult_splits *= prev_splits;
    /* update the last level */
    last_level = level;

    /* define stopping states in the simulator */
    simulator->define_stopping_state(obj_id, st_var_id);

    fclose(fd);
    return (1);
}

int Simulator_Scheduler::end_of_simulation()
{
  Simulator_Level *simul_level;
  
  /* check to see if there is any simulator running or waiting to run */
  simul_level = level_list->show_1st_level();
  while (simul_level != NULL) {
    if ((simul_level->show_running() != 0) || 
	(simul_level->show_waiting() != 0))
      return(0);
    simul_level = level_list->show_next_level();
  }
  return(1);
}

int Simulator_Scheduler::start_simulation()
{
  Simulator_Level *simul_level;
  int              finished = 0;

  /* get the sched mutex */
  if (pthread_mutex_lock(&sched_lock) != 0)
    perror ("pthread_mutex_lock");

  /* start the simulator in level 0 */
  if (level_list->query_level(0)) {
    simul_level = level_list->show_curr_level();
    simul_level->run_simulator(simulator, 0, 0);
  }
  
  /* while not end of simulation */
  while (!end_of_simulation()) {
    
    /* wait for simulations to end */
    wait_for_simulation();

    /* one more finished simulator */
    finished++;

    /* print out some debug info */
    if (Debug_Level > 0)
      if (!(finished % 5000)) {
	print_sched();
      }
    
  }

  /* free the sched mutex */
  if (pthread_mutex_unlock(&sched_lock) != 0)
    perror ("pthread_mutex_lock");

  /* print the final state of the scheduler */
  print_sched();

  /* print the final simulator */
  simulator->print_trans_time();
  
  /* update the rate_rew_list */
  rate_rew_list->divide_by(mult_splits);

  /* print the final rate reward list */
  rate_rew_list->print_rew(NULL);

  return(1);

}

int Simulator_Scheduler::wait_for_simulation()
{
    Simulator       *simul_aux;
    Simulator_Split *simul_split;
    Simulator_Level *level;
    int              finish_point, last_tree;

    /* wait for the finishing of a simulator */
    /* free the scheduler mutex */
    if ( pthread_cond_wait(&sched_sig, &sched_lock) != 0)
      perror ("pthread_cond_wait");
    
    /* get the finishing simulator */
    if (level_list->query_level(simul_finished_level)) {
      level = level_list->show_curr_level();
      /* get the simulator splitted in this level */
      simul_split = level->get_finished_split();
      /* get the simulator */
      simul_aux = simul_split->show_simulator();
      /* sched any waiting simulators in this level */
      level->sched_simulator();
      /* get the point where the simulator finished */
      finish_point = simul_aux->show_finish_point();
      switch (finish_point) {
	case MAX_TIME :
	  /* this simulator has finished */
	  if (level->show_level() == last_level)
	    /* process the rewards accumulated in this simulator */
	    add_rew_list(simul_aux);
	  /* do nothing for now */
	  fprintf( stdout, "MAX_TIME reached - Level: %d\n", level->show_level());
	break;
	case UPPER_ST :
	  /* split and run some more simulations */
	  if (level_list->query_level(simul_finished_level + 1)) {
	    /* go to he next level */
	    level = level_list->show_curr_level();
	    /* set the last tree id in case this split was the last one */
	    if (simul_split->is_last())
	      last_tree = simul_split->show_last_tree() + 1;
	    else
	      last_tree = 1;
	    /* start the simulators in this level */
	    level->run_simulator(simul_aux, 0, last_tree);
	  } else {
	    fprintf ( stderr, "Simulator_Scheduler::wait_for_simulation: ");
	    fprintf ( stderr, "ERROR: No such upper level defined: %d\n", simul_finished_level+1);
	  }
	break;
	case LOWER_ST :
	  /* if it was running in the last level */
	  if (level->show_level() == last_level) {
	    /* process the rewards accumulated in this simulator */
	    add_rew_list(simul_aux);
	    /* clear the accumulated reward in this simulator */
	    simul_aux->clear_reward_values(-2);
	  }
	  /* don't split. Run in the lower level */
	  /* only if its the last simulator and last_tree > 0 */
	  if (simul_split->is_last()) {
	    last_tree = simul_split->show_last_tree();
	    if (last_tree > 0) {
	      /* update the last_tree count */
	      last_tree--;
	      if (level_list->query_level(simul_finished_level - 1)) {
		/* go to he next level */
		level = level_list->show_curr_level();
		/* start the simulators in this level */
		level->run_simulator(simul_aux, 1, last_tree);
	      } else {
		fprintf ( stderr, "Simulator_Scheduler::wait_for_simulation: ");
		fprintf ( stderr, "ERROR: No such lower level defined: %d\n", 
			simul_finished_level-1);
	      }
	    }
	  }
	  break;
      }
    } else {
      fprintf ( stderr, "Simulator_Scheduler::wait_for_simulation: ERROR: No such level defined: %d\n"
	      , simul_finished_level);
      return(-1);
    }

    /* free the scheduler mutex */
    if (pthread_mutex_unlock(&sched_lock) != 0)
      perror ("pthread_mutex_unlock");

    /* send the signal to the waiting simulator */
    if (pthread_cond_signal(&simul_sig) != 0)
      perror("pthread_cond_signal");

    /* delete the simulator split */
    delete simul_split;
    /* delete the older simulator */
    delete simulator;

    /* keep this finished simulator */
    simulator = simul_aux;
    
    return(1);
}

int Simulator_Scheduler::create_rew_list(Simulator *simul)
{
  Reward_Measure_List *rew_list_aux;

  /* create and copy the rate reward list */
  rew_list_aux = simul->show_rate_rew_list(LIST_RO);
  rate_rew_list = rew_list_aux->duplicate();
  delete rew_list_aux;

  return(1);
}

int Simulator_Scheduler::add_rew_list(Simulator *simul)
{
  Reward_Measure_List *rew_list_aux;
  Reward_Measure      *rew, *rew_aux;
  TGFLOAT sum;
  
  /* get the reward list of the simulator */
  rew_list_aux = simul->show_rate_rew_list(LIST_RO);
  /* get the first value of both lists */
  rew_aux = rew_list_aux->show_1st_rew();
  rew     = rate_rew_list->show_1st_rew();
  /* go through all reward measures */
  while (rew != NULL) {
    /* add up the value CR(t) */
    sum = rew->show_cr_value() + rew_aux->show_cr_value();
    rew->ch_cr_value(sum);
    /* add up the value TC(t) */
    sum = rew->show_avg_cr_value() + rew_aux->show_avg_cr_value();
    rew->ch_avg_cr_value(sum);
    /* go to the next reward measure */
    rew_aux = rew_list_aux->show_next_rew();
    rew     = rate_rew_list->show_next_rew();
  }
  delete rew_list_aux;

  return(1);
}


void Simulator_Scheduler::print_sched()
{
   Simulator_Level *level;

   fprintf ( stdout, "The Scheduler:\n");
   level = level_list->show_1st_level();
   while (level != NULL) {
     level->print_level();
     level = level_list->show_next_level();
   }
}

