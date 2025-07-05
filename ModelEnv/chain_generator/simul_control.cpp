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
*  @file simul_control.cpp
*  @brief <b>Simulator_Controler methods</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  The detailed description is unavailable. 
*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#ifdef TG_PARALLEL
#include <pvm3.h>
#endif

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"
#include "system_desc.h"
#include "random.h"
#include "simulator.h"
#include "simul_split.h"

/** @name PVM Defines
 *  Defines for Tangram parallel . 
 */
/*@{*/
#ifdef TG_PARALLEL
#define _RUNCONTEXTMESSAGE_ 1
#define _OTHERSCONTEXTMESSAGE_ 2
#define _TASKASSIGNMENT_ 3
#define _TASKEXIT_ 200
#endif
/*@}*/


/** the global system description */
extern System_Description *the_system_desc;
/** the confidence interval */
extern char Confidence_Interval[4];


#ifdef TG_PARALLEL
/** my task number */
extern int myTaskNumber;
/** vector of tasks numbers */
extern int *tids;
/** # of tasks */
extern int ntids;
#endif


/** output file */
extern FILE *output_file;
/** the begining time of the simulation */
struct timeval begin_simul_time;

//------------------------------------------------------------------------------
Simulator_Controler::Simulator_Controler(int a_total_run)
{
  original_runs = a_total_run;

  if (a_total_run < 1)
    total_runs = 1;
  else
    total_runs = a_total_run;

#ifdef TG_PARALLEL
  last_local_and_remote_run = total_runs;
#endif

  simulator = NULL;

  conf_interval = 1.96; /* 95% */

  rate_rew_list = new Simulator_Result_List();
  imp_rew_list  = new Simulator_Result_List();

  run_list = new Simulator_Run_List();

  /* used only in rare event simulation */
  simul_sched = NULL;

}
//------------------------------------------------------------------------------
Simulator_Controler::~Simulator_Controler()
{
  if (simulator != NULL)
    delete simulator;
  if (rate_rew_list != NULL)
    delete rate_rew_list;
  if (imp_rew_list != NULL)
    delete imp_rew_list;
  if (run_list != NULL)
    delete run_list;

  if (simul_sched != NULL)
    delete simul_sched;
}
//------------------------------------------------------------------------------
int Simulator_Controler::init_simulator( TGFLOAT max_time, int max_trans,
                                         char *stop_event, char *stop_state,
                                         int low_val, int upp_val,
                                         int a_rare_event,
                                         int confidence_interval,
                                         int rewards_toprint,
                                         Random_obj * action_r_obj,
                                         Random_obj * gr_function_r_obj )
{
    Object_Description *obj_desc;
    char *obj_name, *st_var_name;
    int  obj_id, st_var_id;
    State_Variable *low_st, *upp_st;

    switch( confidence_interval )
    {
        case 0:  conf_interval = 1.645; break; /* 90% */
        case 1:  conf_interval = 1.960; break; /* 95% */
        case 2:  conf_interval = 2.576; break; /* 99% */
        default: conf_interval = 1.960; break;
    }

    /* create the simulator structure */
    simulator = new Simulator(max_time, max_trans, rewards_toprint,
                              action_r_obj, gr_function_r_obj);

    /* create the event list for the simulator */
    simulator->create_event_list();

    /* initialize the stopping event */
    if (strcmp(stop_event, ""))
        simulator->set_stopping_event(stop_event);

    /* if rate rewards were defined in this model */
    if (the_system_desc->show_rate_reward() == TRUE)
    {
        /* create the rate rewards list for the simulation and           */
        /* create the unified rate reward sum list (if there is any one) */
        simulator->create_rate_rew_list();
    }

    /* if impulse rewards were defined in this model */
    if (the_system_desc->show_impulse_reward() == TRUE)
        /* create the impulse rewards list for the simulation */
        simulator->create_imp_rew_list();

    /* initialize the event list with all enabled events */
    simulator->initialize_event_list();

    /* define and create a stopping state */
    if (strcmp(stop_state, ""))
    {
        obj_name    = strtok(stop_state, ".");
        st_var_name = strtok(NULL, ".");

        obj_desc = the_system_desc->show_obj_desc(obj_name);
        if (obj_desc == NULL) 
        {
            fprintf( stderr, "ERROR: No object named %s in the system description.\n",obj_name);
            return(-1);
        }
        /* get the obj id and the state var id */
        obj_id = obj_desc->show_id();
        st_var_id = obj_desc->show_symbol_code(st_var_name);
        simulator->define_stopping_state(obj_id, st_var_id);
        /* create the limiting states for the simulator */
        low_st = new State_Variable (low_val, st_var_id);
        upp_st = new State_Variable (upp_val, st_var_id);
        simulator->set_stopping_state(low_st, upp_st);
    }

    /* flag indicating to use rare_event simulation */
    rare_event = a_rare_event;

    /* if this is a rare event simulation */
    if (rare_event == TRUE)
    {
        /* create a new rare event simulator */
        simul_sched = new Simulator_Scheduler(simulator);

        /* create the simulation level list */
        if (simul_sched->create_level_list() < 0)
        return(-1);

        /* create the empty rate reward list accumulator */
        if (simul_sched->create_rew_list(simulator) < 0)
        return(-1);
    }

    return(1);
}
//------------------------------------------------------------------------------
void Simulator_Controler::start_simulator( unsigned short seed_events[3],
                                           unsigned short seed_actions[3],
                                           unsigned short seed_get_random[3] )
{
    Simulator_Run *simul_run;
#ifdef TG_PARALLEL
    int assigned_runs=0;
    int total_runs_executed=0;
#endif
    int run;

    /* get the initial time of the simulation */
    gettimeofday( &begin_simul_time, 0 );

    if ( output_file == NULL )
    {
        fprintf( stderr,"Error: Output file is not defined!\n" );
        exit(0);
    }

    fprintf( output_file, "+-------------------------------------------------------------+\n");
    fprintf( output_file, "|  T A N G R A M - I I   S I M U L A T I O N   R E S U L T S  |\n");
    fprintf( output_file, "+-------------------------------------------------------------+\n");

    fprintf( output_file, "\nSeeds used by the Random Number Generators\n");
    fprintf( output_file, "Events      : %04X%04X%04X\n",
             seed_events[0], seed_events[1], seed_events[2] );
    fprintf( output_file, "Actions     : %04X%04X%04X\n",
             seed_actions[0], seed_actions[1], seed_actions[2] );
    fprintf( output_file, "get_random(): %04X%04X%04X\n",
             seed_get_random[0], seed_get_random[1], seed_get_random[2] );

    for( run = 0; run < original_runs; run++ )
    {
        #ifdef TG_PARALLEL
        if (--assigned_runs <= 0)
        {
            pvm_recv(-1,_TASKASSIGNMENT_); // anybody can send runs to me
            pvm_upkint(&assigned_runs,1,1);
            pvm_upkint(&run,1,1);
            if (run < 0) run = myTaskNumber;
            if (assigned_runs == 0) // no more runs to do
                break;
        }
        total_runs_executed++;

        if (myTaskNumber != 0)
            pvm_initsend(PvmDataDefault);
        #endif


        fprintf( output_file, "\n---------------------------------------------------------------\n");
        fprintf( output_file,   "                       R U N : %d\n", run+1);
        fprintf( output_file,   "---------------------------------------------------------------\n\n");
        fprintf( output_file,   "-------------------- Event Triggers --------------------\n\n");

        // if using rare event simulation
        if (rare_event == TRUE)
        {
            // start simulation
            simul_sched->start_simulation();
            // add this run to the run list
            simul_run = add_simul_run(simul_sched, run);
            // delete this simulator
            #ifdef TG_PARALLEL
            // Here we are kind of stressing PVM
            // all tasks send theirs messages although
            // the destination task may be still simulating
            // and not getting them. PVM manual sais
            // send is non blocking and the message is delivered
            // in an asynchonous way... let's see :)
            if (myTaskNumber != 0)
                simul_run->send_context();
            #endif
            simul_sched->reset();
        }
        else
        {
            // simulate the model in batch
            simulator->simulate_model_batch();
            // print out some results
            simulator->print_trans_time();
            simulator->print_no_triggers( output_file );
            simulator->print_rewards( output_file );
            // add this run to the run list
            simul_run = add_simul_run(simulator, run);

            #ifdef TG_PARALLEL
            // Here we are kind of stressing PVM
            // all tasks send theirs messages although
            // the destination task may be still simulating
            // and not getting them. PVM manual sais
            // send is non blocking and the message is delivered
            // in an asynchonous way... let's see :)
            if (myTaskNumber != 0)
                simul_run->send_context();
            #endif

            // zero all possible values of the simulator and set it to its
            // initial state, if it is not the last run.
            if( run != total_runs - 1 )
               simulator->reset(run);
            else simulator->reward_reset(run);
        }

        #ifdef TG_PARALLEL
		if (myTaskNumber == 0) // try to receive while doing simulation (- buffer overload)
		recv_only_run_context(total_runs_executed);
        #endif
        #ifdef TG_PARALLEL
		if (myTaskNumber != 0) // send context data message
		{
            if (pvm_send(tids[0],_RUNCONTEXTMESSAGE_) < 0)
            {
                fprintf(stderr,"Task %x::Could not send message to task %x on run %d\nIs pvm deamon still running?\n",myTaskNumber,tids[0],run);
                pvm_perror("pvm_send");
                return;
            }
		}
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&myTaskNumber,1,1);
		pvm_send(pvm_parent(),_TASKASSIGNMENT_);
        #endif

    }

    #ifdef TG_PARALLEL
    if (myTaskNumber != 0)
    {
        send_others_context();
    }
    else
    {
        recv_others_context(total_runs_executed);
        #endif
        // create the result list
        create_result_list();

        // calculate the mean value of all runs
        calculate_mean();

        // calculate the variance between all runs
        if (original_runs > 1)
        calculate_variance();

        // calculate the confidence interval
        if (original_runs > 1)
        calculate_interval();

        // print the final results
        print_results( run );
        #ifdef TG_PARALLEL
    }
    #endif
}
//------------------------------------------------------------------------------
#ifdef TG_PARALLEL
void Simulator_Controler::send_others_context()
{
    int has_rate_rew_list = (rate_rew_list != NULL),
        has_imp_rew_list = (imp_rew_list != NULL);

    pvm_initsend( PvmDataDefault );
    pvm_pkint(&has_rate_rew_list,1,1);
    if (has_rate_rew_list)
    {
        rate_rew_list->send_context();
    }
    pvm_pkint(&has_imp_rew_list,1,1);
    if (has_imp_rew_list)  imp_rew_list->send_context();
    if (pvm_send(tids[0],_OTHERSCONTEXTMESSAGE_) < 0)
    {
        fprintf(stderr,"Task %x::Could not send message to task %x\nIs pvm deamon still running?\n",myTaskNumber,tids[0]);
        pvm_perror("pvm_send");
        return;
    }
}
//------------------------------------------------------------------------------
void Simulator_Controler::recv_only_run_context(int &num_runs)
{
    Simulator_Run *simul_run;

    while (pvm_nrecv( -1, _RUNCONTEXTMESSAGE_ ) > 0)// _RUNCONTEXTMESSAGE_ messages from any task
    {
        simul_run = new Simulator_Run(num_runs);
        if (simul_run == NULL)
        {
            fprintf(stderr,"Out of memory in void Simulator_Controler::get_others_simulation_runs()\n");
            break;
        }
        simul_run->recv_context();
        run_list->add_tail_run(simul_run);
        num_runs++;
    }
    if (myTaskNumber == 0)
        while (pvm_nrecv( -1, _TASKEXIT_ ) > 0)// _TASKEXIT_ messages from any task
        {
            ntids--;
            pvm_upkint(tids,ntids,1); // one task has exited before the jobs have ended
        }
}

void Simulator_Controler::recv_others_context(int num_runs)
{
  Simulator_Run *simul_run;
  int has_rate_rew_list, has_imp_rew_list;
  int i;
  int last_local_and_remote_run = num_runs;

// the thrist things sent are the runs

    for (i = 0; i < original_runs - num_runs; i++)
    {
        pvm_recv( -1, _RUNCONTEXTMESSAGE_ );// _RUNCONTEXTMESSAGE_ messages from any task
        simul_run = new Simulator_Run(last_local_and_remote_run);
        if (simul_run == NULL)
        {
            fprintf(stderr,"Out of memory in void Simulator_Controler::get_others_simulation_runs()\n");
            break;
        }
        simul_run->recv_context();
        run_list->add_tail_run(simul_run);
        last_local_and_remote_run++;
    }

    while (pvm_nrecv( -1, _TASKEXIT_ ) > 0)// _TASKEXIT_ messages from any task
    {
        ntids--;
        pvm_upkint(tids,ntids,1); // one task has exited before the jobs have ended
    }

    for (i = 1; i < ntids; i++)
    {
        if (pvm_recv( tids[i], _OTHERSCONTEXTMESSAGE_ ) > 0)
        {
            // _OTHERSCONTEXTMESSAGE_ messages from all tasks
            if (pvm_upkint(&has_rate_rew_list,1,1) < 0) return;
            if (has_rate_rew_list)
            {
                if (rate_rew_list == NULL)
                    rate_rew_list = new Simulator_Result_List();
                if (rate_rew_list != NULL)
                    rate_rew_list->recv_context();
            }
            if (pvm_upkint(&has_imp_rew_list,1,1) < 0) return;
            if (has_imp_rew_list)
            {
                if (imp_rew_list == NULL)
                    imp_rew_list = new Simulator_Result_List();
                if (imp_rew_list != NULL)
                    imp_rew_list->recv_context();
            }
        }
  }
}
#endif

Simulator_Run *Simulator_Controler::add_simul_run(Simulator *simul, int run)
{
  Simulator_Run       *simul_run;
  Reward_Measure_List *rew_list, *rew_list_aux;

  /* create a new run */
  simul_run = new Simulator_Run( run, simul->show_simul_time(), simul->show_transitions() );

  /* create and copy the rate reward list */
  rew_list_aux = simul->show_rate_rew_list(LIST_RO);
  rew_list = rew_list_aux->duplicate();
  delete rew_list_aux;
  simul_run->ch_rate_rew_list(rew_list);

  /* create and copy the impulse reward list */
  rew_list_aux = simul->show_imp_rew_list(LIST_RO);
  rew_list = rew_list_aux->duplicate();
  delete rew_list_aux;
  simul_run->ch_imp_rew_list(rew_list);

  /* add this run to the list */
  run_list->add_tail_run( simul_run );
  return (simul_run);
}

Simulator_Run *Simulator_Controler::add_simul_run(Simulator_Scheduler *simul_sched, int run)
{
  Simulator_Run       *simul_run;
  Reward_Measure_List *rew_list, *rew_list_aux;
  Simulator           *simul;

  /* get the simulator inside the scheduler */
  simul = simul_sched->show_simulator();

  /* create a new run */
  simul_run = new Simulator_Run(run, simul->show_simul_time(),
				     simul->show_transitions());

  /* create and copy the rate reward list */
  rew_list_aux = simul_sched->show_rate_rew_list(LIST_RO);
  rew_list = rew_list_aux->duplicate();
  delete rew_list_aux;
  simul_run->ch_rate_rew_list(rew_list);

  /* create an empty impulse reward list */
  rew_list = new Reward_Measure_List();
  simul_run->ch_imp_rew_list(rew_list);

  /* add this run to the list */
  run_list->add_tail_run(simul_run);
  return (simul_run);
}


void Simulator_Controler::create_result_list()
{
   Simulator_Run       *simul_run;
   Reward_Measure_List *rew_list;
   Reward_Measure      *rew;
   Simulator_Result    *simul_result;
   char                 name[MAXSTRING];

   /* get the first run simulated */
   simul_run = run_list->show_1st_run();

   /* create and build the rate reward list */
   rew_list = simul_run->show_rate_rew_list(LIST_RO);
   rew = rew_list->show_1st_rew();
   while (rew != NULL) {
     rew->show_name(name);
     simul_result = new Simulator_Result (name);
     simul_result->ch_has_level( rew->show_has_level() );
     rate_rew_list->add_tail_result(simul_result);
     rew = rew_list->show_next_rew();
   }
   delete rew_list;

   /* create and build the impulse reward list */
   rew_list = simul_run->show_imp_rew_list(LIST_RO);
   rew = rew_list->show_1st_rew();
   while (rew != NULL) {
     rew->show_name(name);
     simul_result = new Simulator_Result (name);
     simul_result->ch_has_level( rew->show_has_level() );
     imp_rew_list->add_tail_result(simul_result);
     rew = rew_list->show_next_rew();
   }
   delete rew_list;

}

void Simulator_Controler::calculate_mean()
{
   Simulator_Run       *simul_run;
   Reward_Measure_List *rew_list;
   Reward_Measure      *rew;
   Simulator_Result    *simul_result;
   TGFLOAT               simul_time, sum;

   /* go through all the runs */
   simul_run = run_list->show_1st_run();
   while (simul_run != NULL) {
     /* get the simul time of this run */
     simul_time = simul_run->show_simul_time();
     /* sum the rate reward values */
     simul_result = rate_rew_list->show_1st_result();
     rew_list     = simul_run->show_rate_rew_list(LIST_RO);
     rew          = rew_list->show_1st_rew();
     while (simul_result != NULL) {
       /* calculate cr mean */
       sum = simul_result->show_cr_mean() + rew->show_cr_value();
       simul_result->ch_cr_mean(sum);
       /* calculate acr mean */
       sum = simul_result->show_acr_mean() + (rew->show_cr_value() / simul_time);
       simul_result->ch_acr_mean(sum);
       /* calculate atc mean */
       sum = simul_result->show_atc_mean() + (rew->show_avg_cr_value() / simul_time);
       simul_result->ch_atc_mean(sum);
       /* calculate tbl mean */
       sum = simul_result->show_tbl_mean() + (rew->show_time_abv_lv() / simul_time);
       simul_result->ch_tbl_mean(sum);

       rew          = rew_list->show_next_rew();
       simul_result = rate_rew_list->show_next_result();
     }
     delete rew_list;
     /* sum the impulse reward values */
     simul_result = imp_rew_list->show_1st_result();
     rew_list     = simul_run->show_imp_rew_list(LIST_RO);
     rew          = rew_list->show_1st_rew();
     while (simul_result != NULL) {
       /* calculate cr mean */
       sum = simul_result->show_cr_mean() + rew->show_cr_value();
       simul_result->ch_cr_mean(sum);
       /* calculate acr mean */
       sum = simul_result->show_acr_mean() + (rew->show_cr_value() / simul_time);
       simul_result->ch_acr_mean(sum);
       /* calculate atc mean */
       sum = simul_result->show_atc_mean() + (rew->show_avg_cr_value() / simul_time);
       simul_result->ch_atc_mean(sum);

       rew          = rew_list->show_next_rew();
       simul_result = imp_rew_list->show_next_result();
     }
     delete rew_list;

     /* go to the next run */
     simul_run = run_list->show_next_run();
   }

   /* divide the values in the rate list by the total number of runs */
   simul_result = rate_rew_list->show_1st_result();
   while (simul_result != NULL) {
     /* calculate cr mean */
     sum = simul_result->show_cr_mean() / original_runs;
     simul_result->ch_cr_mean(sum);
     /* calculate acr mean */
     sum = simul_result->show_acr_mean() / original_runs;
     simul_result->ch_acr_mean(sum);
     /* calculate atc mean */
     sum = simul_result->show_atc_mean() / original_runs;
     simul_result->ch_atc_mean(sum);
     /* calculate tbl mean */
     sum = simul_result->show_tbl_mean() / original_runs;
     simul_result->ch_tbl_mean(sum);
     /* go to the next simul result */
     simul_result = rate_rew_list->show_next_result();
   }
   /* divide the values in the impulse list by the total number of runs */
   simul_result = imp_rew_list->show_1st_result();
   while (simul_result != NULL) {
     /* calculate cr mean */
     sum = simul_result->show_cr_mean() / original_runs;
     simul_result->ch_cr_mean(sum);
     /* calculate acr mean */
     sum = simul_result->show_acr_mean() / original_runs;
     simul_result->ch_acr_mean(sum);
     /* calculate atc mean */
     sum = simul_result->show_atc_mean() / original_runs;
     simul_result->ch_atc_mean(sum);
     /* go to the next simul result */
     simul_result = imp_rew_list->show_next_result();
   }

}

void Simulator_Controler::calculate_variance()
{
   Simulator_Run       *simul_run;
   Reward_Measure_List *rew_list;
   Reward_Measure      *rew;
   Simulator_Result    *simul_result;
   TGFLOAT              simul_time, sum;

   /* go through all the runs */
   simul_run = run_list->show_1st_run();
   while (simul_run != NULL) {
     /* get the simul time of this run */
     simul_time = simul_run->show_simul_time();
     /* sum the rate reward values */
     simul_result = rate_rew_list->show_1st_result();
     rew_list     = simul_run->show_rate_rew_list(LIST_RO);
     rew          = rew_list->show_1st_rew();
     while (simul_result != NULL) {
       /* calculate cr var */
       sum = simul_result->show_cr_var() +
	     pow ( rew->show_cr_value() - simul_result->show_cr_mean(), 2);
       simul_result->ch_cr_var(sum);
       /* calculate acr var */
       sum = simul_result->show_acr_var() +
	     pow ( (rew->show_cr_value() / simul_time) - simul_result->show_acr_mean(), 2);
       simul_result->ch_acr_var(sum);
       /* calculate atc var */
       sum = simul_result->show_atc_var() +
	     pow ( (rew->show_avg_cr_value() / simul_time) - simul_result->show_atc_mean(), 2);
       simul_result->ch_atc_var(sum);
       /* calculate tbl var */
       sum = simul_result->show_tbl_var() +
	     pow ( (rew->show_time_abv_lv() / simul_time) - simul_result->show_tbl_mean(), 2);
       simul_result->ch_tbl_var(sum);

       rew          = rew_list->show_next_rew();
       simul_result = rate_rew_list->show_next_result();
     }
     delete rew_list;

     /* sum the impulse reward values */
     simul_result = imp_rew_list->show_1st_result();
     rew_list     = simul_run->show_imp_rew_list(LIST_RO);
     rew          = rew_list->show_1st_rew();
     while (simul_result != NULL) {
       /* calculate cr var */
       sum = simul_result->show_cr_var() +
	     pow ( rew->show_cr_value() - simul_result->show_cr_mean(), 2);
       simul_result->ch_cr_var(sum);
       /* calculate acr var */
       sum = simul_result->show_acr_var() +
	     pow ( (rew->show_cr_value() / simul_time) - simul_result->show_acr_mean(), 2);
       simul_result->ch_acr_var(sum);
       /* calculate atc var */
       sum = simul_result->show_atc_var() +
	     pow ( (rew->show_avg_cr_value() / simul_time) - simul_result->show_atc_mean(), 2);
       simul_result->ch_atc_var(sum);

       rew          = rew_list->show_next_rew();
       simul_result = imp_rew_list->show_next_result();
     }
     delete rew_list;

     /* go to the next run */
     simul_run = run_list->show_next_run();
   }

   /* divide the values in the rate list by the total number of runs */
   simul_result = rate_rew_list->show_1st_result();
   while (simul_result != NULL) {
     /* calculate cr var */
     sum = simul_result->show_cr_var() / (original_runs - 1);
     simul_result->ch_cr_var(sum);
     /* calculate acr var */
     sum = simul_result->show_acr_var() / (original_runs - 1);
     simul_result->ch_acr_var(sum);
     /* calculate atc var */
     sum = simul_result->show_atc_var() / (original_runs - 1);
     simul_result->ch_atc_var(sum);
     /* calculate tbl var */
     sum = simul_result->show_tbl_var() / (original_runs - 1);
     simul_result->ch_tbl_var(sum);
     /* go to the next simul result */
     simul_result = rate_rew_list->show_next_result();
   }
   /* divide the values in the impulse list by the total number of runs */
   simul_result = imp_rew_list->show_1st_result();
   while (simul_result != NULL) {
     /* calculate cr var */
     sum = simul_result->show_cr_var() / (original_runs - 1);
     simul_result->ch_cr_var(sum);
     /* calculate acr var */
     sum = simul_result->show_acr_var() / (original_runs - 1);
     simul_result->ch_acr_var(sum);
     /* calculate atc var */
     sum = simul_result->show_atc_var() / (original_runs - 1);
     simul_result->ch_atc_var(sum);
     /* go to the next simul result */
     simul_result = imp_rew_list->show_next_result();
   }

}

void Simulator_Controler::calculate_interval()
{
   Simulator_Result *simul_result;
   TGFLOAT            inter;

   /* go through all the rate reward values */
   simul_result = rate_rew_list->show_1st_result();
   while (simul_result != NULL) {
     /* calculate cr inter */
     inter = conf_interval * sqrt (simul_result->show_cr_var() / original_runs );
     simul_result->ch_cr_inter(inter);
     /* calculate acr inter */
     inter = conf_interval * sqrt (simul_result->show_acr_var() / original_runs );
     simul_result->ch_acr_inter(inter);
     /* calculate atc inter */
     inter = conf_interval * sqrt (simul_result->show_atc_var() / original_runs );
     simul_result->ch_atc_inter(inter);
     /* calculate tbl inter */
     inter = conf_interval * sqrt (simul_result->show_tbl_var() / original_runs );
     simul_result->ch_tbl_inter(inter);
     /* go to the next simul result */
     simul_result = rate_rew_list->show_next_result();
   }

   /* go through all the impulse reward values */
   simul_result = imp_rew_list->show_1st_result();
   while (simul_result != NULL) {
     /* calculate cr inter */
     inter = conf_interval * sqrt (simul_result->show_cr_var() / original_runs );
     simul_result->ch_cr_inter(inter);
     /* calculate acr inter */
     inter = conf_interval * sqrt (simul_result->show_acr_var() / original_runs );
     simul_result->ch_acr_inter(inter);
     /* calculate atc inter */
     inter = conf_interval * sqrt (simul_result->show_atc_var() / original_runs );
     simul_result->ch_atc_inter(inter);
     /* go to the next simul result */
     simul_result = imp_rew_list->show_next_result();
   }

}
//------------------------------------------------------------------------------
Simulator *Simulator_Controler::show_simulator()
{
  return( simulator );
}
//------------------------------------------------------------------------------
void Simulator_Controler::print_results( int num_runs )
{
    Simulator_Result  *simul_result;
#ifndef TG_PARALLEL
    struct timeval     end_time;
    double             simul_time_ms;
#endif
    if ( output_file == NULL )
    {
        fprintf( stderr,"Error: Output file is not defined!\n" );
        exit(0);
    }
    // print final results (rewards results)
    fprintf( output_file, "\n\n---------------------------------------------------------------\n");
    fprintf( output_file, "                 R E W A R D S   R E S U L T S                \n");
    fprintf( output_file,     "---------------------------------------------------------------\n");

    //----- print some statistics -------------------

#ifndef TG_PARALLEL
    /* calculate the simulation execution time */
    gettimeofday( &end_time, 0 );
    end_time.tv_sec = end_time.tv_sec - begin_simul_time.tv_sec;
    end_time.tv_usec = end_time.tv_usec - begin_simul_time.tv_usec;
    if ( end_time.tv_usec < 0 )
    {
        end_time.tv_sec -= 1;
        end_time.tv_usec += 1000000;
    }
    simul_time_ms = end_time.tv_sec * 1000.0 + end_time.tv_usec/1000.0;

    // print the simul_time of each run
    fprintf( output_file, "\n Simulation time : %f\n", simulator->show_simul_time() );
    // print number of runs
    fprintf( output_file, " Number of runs  : %d\n", num_runs );
    // print the total simul_time
    fprintf( output_file, " Total simulation time : %f\n", simulator->show_simul_time()*num_runs );
#endif
    // print Confidence_Interval used for the results
    fprintf( output_file, " Confidence interval   : %s\n",Confidence_Interval);
#ifndef TG_PARALLEL
    // print the execution times
    fprintf( output_file, " Simulation execution real time : %.3f milliseconds\n", simul_time_ms );

#endif
    //----- print the reward measures results ------
    if (the_system_desc->show_rate_reward() == TRUE)
    {
        fprintf( output_file, "\n----------------- Rate Rewards Results -----------------\n");
        simul_result = rate_rew_list->show_1st_result();
        while (simul_result != NULL)
        {
            simul_result->print();
            simul_result = rate_rew_list->show_next_result();
        }
    }

    if (the_system_desc->show_impulse_reward() == TRUE)
    {
        fprintf( output_file, "\n---------------- Impulse Rewards Results ---------------\n");
        simul_result = imp_rew_list->show_1st_result();
        while (simul_result != NULL)
        {
            simul_result->print();
            simul_result = imp_rew_list->show_next_result();
        }
    }
    fprintf( output_file, "\n--------------------------------------------------------\n");
}
//------------------------------------------------------------------------------
