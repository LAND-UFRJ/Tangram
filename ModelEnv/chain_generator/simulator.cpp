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
* @file simulator.cpp
* @brief <b> Simulator class definitions </b>.
* @remarks Lot of.
* @author LAND/UFRJ
* @date 1999-2009
* @warning Do not modify this class before knowing the whole Tangram II project
* @since version 1.0
*
* Definition of Simulator methods.
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifdef __APPLE__
  #include <limits.h>
  #include <float.h>
  #ifndef MAXDOUBLE
    #define MAXDOUBLE DBL_MAX
  #endif
#else
// The values.h is deprecated.
#include <values.h>
#endif

#include <string.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"
#include "system_desc.h"
#include "distribution.h"
#include "random.h"
#include "simulator.h"

/** the global system description */
extern System_Description *the_system_desc;
/** the first system state described */
extern System_State *first_system_state;
/** the debug level variable */
extern int Debug_Level;
/** the reward trace option */
extern int Rew_Files;
/** the base filename of the model */
extern char base_filename[MAXSTRING];
/** the execution context */
extern t_context context;
/** controls the structures' access */
extern pthread_mutex_t simulator_lock;
/** output file */
extern FILE *output_file;

/* used to scape from numeric problems of digital computers */
/** TIME values differing x_epsilon are considered the same. */
TGFLOAT x_epsilon = 1e-10;
/** CR values differing +/- y_epsilon from expr are considered equal to expr. */
TGFLOAT y_epsilon = 1e-10;

//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
Simulator::Simulator( TGFLOAT a_max_time, int a_max_trans, int reward_toprint,
                      Random_obj * action_r_obj,
                      Random_obj * gr_function_r_obj )
{
    simulation_time = 0;
    last_simul_time = 0;
    transitions     = 0;
    vanishing_trans = 0;
    reward_to_print = reward_toprint;

    stopping_event  = NULL;
    max_simul_time  = a_max_time;
    max_simul_trans = a_max_trans;
    stopping_state  = FALSE;
    stop_obj_id     = stop_st_var_id  = 0;
    stop_low_st_var = stop_upp_st_var = NULL;
    finish_point    = 0;

    action_random_obj      = action_r_obj;
    gr_function_random_obj = gr_function_r_obj;

    /* Initializated with false indicates no */
    /* presence of the REWARD_REACHED events */
    rr_ev_flag = FALSE;

    /* Indicates the run number - 1st run is 0 */
    run_number = 0;

    /* create an empty event list */
    event_list = new Simulator_Event_List();

    /* create an empty clone event list */
    clone_ev_list = new Clone_Event_List();

    /* create an empty rate reward list */
    rate_rew_list = new Reward_Measure_List();

    /* create an empty impulse reward list */
    imp_rew_list = new Reward_Measure_List();

    /* create an empty rate reward sum list */
    rate_rew_sum_list = new Rate_Reward_Sum_List();

    /* set the event - object_state shortcut to NULL */
    obj_ev_tb = NULL;

    /* set the current system state to the first system state */
    curr_sys_st = new System_State(first_system_state);

    debug(4,"Simulator::Simulator(TGFLOAT, int): creating object");
}
//------------------------------------------------------------------------------
Simulator::Simulator(Simulator *simul)
{
    State_Variable       *st_var;
    Simulator_Event_List *ev_list_aux;
    Simulator_Event      *ev, *ev_aux, *stopping_ev_aux;
    int                   no_objs;
    int                   i, j;
    Clone_Event_List     *cl_list_aux;
    Clone_Event          *cl, *cl_aux;
    Reward_Measure_List  *meas_list;
    Reward_Measure       *meas, *meas_aux;
    Rate_Reward_Sum_List *ra_rew_sum_list;
    Rate_Reward_Sum      *ra_rew_sum;
    Rate_Reward_Sum      *ra_rew_sum_aux = NULL;
    System_State         *sys_st;
    t_obj_ev             *obj_ev_aux;

    stopping_event  = NULL;
    /* copy all direct attributes */
    simulation_time = simul->show_simul_time();
    last_simul_time = simul->show_last_simul_time();
    transitions     = simul->show_transitions();
    vanishing_trans = simul->show_vanishing_trans();
    reward_to_print = simul->reward_to_print;

    /* copy all direct attributes */
    max_simul_time  = simul->show_max_simul_time();
    max_simul_trans = simul->show_max_simul_trans();
    stopping_state  = simul->show_stopping_state();
    stop_obj_id     = simul->show_stop_obj_id();
    stop_st_var_id  = simul->show_stop_st_var_id();
    st_var          = simul->show_low_st_var();
    rr_ev_flag      = simul->show_rr_ev_flag();
    run_number      = simul->show_run_number();

    /* copy all direct attributes */
    action_random_obj      = simul->action_random_obj;
    gr_function_random_obj = simul->gr_function_random_obj;

    /* copy stopping states */
    if (st_var != NULL)
        stop_low_st_var = new State_Variable(st_var);
    else
        stop_low_st_var = NULL;

    st_var = simul->show_upp_st_var();

    if (st_var != NULL)
        stop_upp_st_var = new State_Variable(st_var);
    else
        stop_upp_st_var = NULL;

    /* copy finishing point */
    finish_point = simul->show_finish_point();

    /* create an empty event list and copy all the events */
    event_list = new Simulator_Event_List();
    ev_list_aux = simul->show_simul_ev_list(LIST_RO);
    ev_aux = ev_list_aux->show_1st_event();
    /* get the stopping_event */
    stopping_ev_aux = simul->show_stopping_event();
    /* get the obj state x event shortcut table */
    obj_ev_aux = simul->show_obj_ev_tb();
    /* get the number of objects in the system */
    no_objs = the_system_desc->show_no_obj();
    /* create the event_object shortcut table */
    obj_ev_tb = (t_obj_ev *)malloc ( (no_objs + 1) * sizeof (t_obj_ev));
    /* initialize the event_object shortcut table */
    for (i=0; i<(no_objs+1); i++)
    {
        obj_ev_tb[i].simul_ev  = NULL;
        obj_ev_tb[i].no_events = obj_ev_aux[i].no_events;
        for (j=0; j<obj_ev_tb[i].no_events; j++)
        {
            ev = new Simulator_Event (ev_aux);
            event_list->add_tail_event(ev);
            /* check to see if this is the first event of this object */
            if (j == 0)
	        obj_ev_tb[i].simul_ev = ev;
            /* check to see if this is the stopping event */
            if (ev_aux == stopping_ev_aux)
                stopping_event = ev;
            ev_aux = ev_list_aux->show_next_event();
        }
    }
    delete ev_list_aux;

    /* create an empty clone event list and copy all the elements */
    clone_ev_list = new Clone_Event_List();
    cl_list_aux = simul->show_clone_ev_list(LIST_RO);
    cl_aux = cl_list_aux->show_1st_clone();
    while (cl_aux != NULL)
    {
        cl = new Clone_Event (cl_aux);
        clone_ev_list->add_tail_clone(cl);
        cl_aux = cl_list_aux->show_next_clone();
    }
    delete cl_list_aux;

    /* create an empty rate reward list and copy the elements */
    rate_rew_list = new Reward_Measure_List();
    meas_list = simul->show_rate_rew_list(LIST_RO);
    meas_aux = meas_list->show_1st_rew();
    while (meas_aux != NULL)
    {
        meas = new Reward_Measure (meas_aux);
        rate_rew_list->add_tail_rew(meas);
        meas_aux = meas_list->show_next_rew();
    }
    delete meas_list;

    /* create an empty impulse reward list and copy the elements */
    imp_rew_list = new Reward_Measure_List();
    meas_list = simul->show_imp_rew_list(LIST_RO);
    meas_aux = meas_list->show_1st_rew();
    while (meas_aux != NULL)
    {
        meas = new Reward_Measure (meas_aux);
        imp_rew_list->add_tail_rew(meas);
        meas_aux = meas_list->show_next_rew();
    }
    delete meas_list;

    /* create an empty rate reward sum list and copy the elements */
    rate_rew_sum_list = new Rate_Reward_Sum_List();
    ra_rew_sum_list = simul->show_rate_rew_sum_list(LIST_RO);
    ra_rew_sum = ra_rew_sum_list->show_1st_rew();
    while ( ra_rew_sum != NULL)
    {
        ra_rew_sum_aux = new Rate_Reward_Sum( ra_rew_sum );
        rate_rew_sum_list->add_tail_rew( ra_rew_sum_aux );
        ra_rew_sum = ra_rew_sum_list->show_next_rew();
    }
    delete ra_rew_sum_list;

    /* set the current system state to the system state in the simulator */
    sys_st = simul->show_curr_sys_st();
    curr_sys_st = new System_State(sys_st);

    debug(4,"Simulator::Simulator(Simulator *): creating object");
}
//------------------------------------------------------------------------------
Simulator::~Simulator()
{
    /* delete the event list */
    delete event_list;

    /* delete the clone event list */
    delete clone_ev_list;

    /* delete the accumulator for the rate reward */
    delete rate_rew_list;

    /* delete the accumulator for the impulse reward */
    delete imp_rew_list;

    /* delete the current system state */
    delete curr_sys_st;

    /* delete the shortcut table to the event list */
    if( obj_ev_tb != NULL )
        free(obj_ev_tb);

    /* delete the stopping states */
    if( stop_low_st_var != NULL )
        delete stop_low_st_var;
    if( stop_upp_st_var != NULL )
        delete stop_upp_st_var;

    debug(4,"Simulator::~Simulator(): destroying object");
}
//------------------------------------------------------------------------------
//     M E T H O D S
//------------------------------------------------------------------------------
TGFLOAT Simulator::show_simul_time()
{
    return (simulation_time);
}
//------------------------------------------------------------------------------
TGFLOAT Simulator::show_last_simul_time()
{
    return (last_simul_time);
}
//------------------------------------------------------------------------------
int Simulator::show_transitions()
{
    return (transitions);
}
//------------------------------------------------------------------------------
int Simulator::show_vanishing_trans()
{
    return (vanishing_trans);
}
//------------------------------------------------------------------------------
System_State *Simulator::show_curr_sys_st()
{
    return (curr_sys_st);
}
//------------------------------------------------------------------------------
Simulator_Event *Simulator::show_stopping_event()
{
    return (stopping_event);
}
//------------------------------------------------------------------------------
TGFLOAT Simulator::show_max_simul_time()
{
    return (max_simul_time);
}
//------------------------------------------------------------------------------
int Simulator::show_max_simul_trans()
{
    return (max_simul_trans);
}
//------------------------------------------------------------------------------
int Simulator::show_stopping_state()
{
    return (stopping_state);
}
//------------------------------------------------------------------------------
int Simulator::show_stop_obj_id()
{
    return (stop_obj_id);
}
//------------------------------------------------------------------------------
int Simulator::show_stop_st_var_id()
{
    return (stop_st_var_id);
}
//------------------------------------------------------------------------------
State_Variable *Simulator::show_low_st_var()
{
    return (stop_low_st_var);
}
//------------------------------------------------------------------------------
State_Variable *Simulator::show_upp_st_var()
{
    return (stop_upp_st_var);
}
//------------------------------------------------------------------------------
Simulator_Event_List *Simulator::show_simul_ev_list(int access)
{
    Simulator_Event_List *event_list_aux;

    if (access == LIST_RW)
        return (event_list);
    else
    {
        event_list_aux = new Simulator_Event_List (event_list);
        return (event_list_aux);
    }
}
//------------------------------------------------------------------------------
Clone_Event_List *Simulator::show_clone_ev_list(int access)
{
    Clone_Event_List *clone_list_aux;

    if (access == LIST_RW)
        return (clone_ev_list);
    else
    {
        clone_list_aux = new Clone_Event_List (clone_ev_list);
        return (clone_list_aux);
    }
}
//------------------------------------------------------------------------------
t_obj_ev *Simulator::show_obj_ev_tb()
{
    return (obj_ev_tb);
}
//------------------------------------------------------------------------------
Reward_Measure_List *Simulator::show_rate_rew_list(int access)
{
    Reward_Measure_List *rew_list_aux;

    if (access == LIST_RW)
        return (rate_rew_list);
    else
    {
        rew_list_aux = new Reward_Measure_List(rate_rew_list);
        return (rew_list_aux);
    }
}
//------------------------------------------------------------------------------
Reward_Measure_List *Simulator::show_imp_rew_list(int access)
{
    Reward_Measure_List *rew_list_aux;

    if (access == LIST_RW)
        return (imp_rew_list);
    else
    {
        rew_list_aux = new Reward_Measure_List(imp_rew_list);
        return (rew_list_aux);
    }
}
//------------------------------------------------------------------------------
Rate_Reward_Sum_List *Simulator::show_rate_rew_sum_list( int access )
{
    Rate_Reward_Sum_List *rate_rew_sum_list_aux;

    if ( access == LIST_RW )
        return ( rate_rew_sum_list );
    else
    {
        rate_rew_sum_list_aux = new Rate_Reward_Sum_List( rate_rew_sum_list );
        return ( rate_rew_sum_list_aux );
    }
}
//------------------------------------------------------------------------------
TGFLOAT Simulator::get_random()
{
    return gr_function_random_obj->next_randval();
}
//------------------------------------------------------------------------------
void Simulator::create_event_list()
{
    Object_Description_List *obj_desc_list;
    Object_Description      *obj_desc;
    Event_List              *ev_list;
    Event                   *ev;
    Simulator_Event         *simul_ev;
    int                      no_objs, id, first_ev, i;

    /* go through all obj desc of the system desc */
    obj_desc_list = the_system_desc->show_obj_desc_list(LIST_RO);
    obj_desc = obj_desc_list->show_1st_obj_desc();
    /* get the number of objects */
    no_objs = obj_desc_list->show_no_obj_desc();
    /* create the event_object shortcut table */
    obj_ev_tb = (t_obj_ev *)malloc ( (no_objs + 1) * sizeof (t_obj_ev));
    /* initialize the event_object shortcut table */
    for (i=0; i<(no_objs+1); i++)
    {
        obj_ev_tb[i].simul_ev  = NULL;
        obj_ev_tb[i].no_events = 0;
    }
    /* go through all the objects in the system */
    while (obj_desc != NULL)
    {
        ev_list = obj_desc->show_event_list(LIST_RO);
        ev = ev_list->show_1st_event();
        /* mark the first event of this object */
        first_ev = TRUE;
        /* go through all the event desc of this obj desc */
        while (ev != NULL)
        {
            /* create a new simulator event for this event */
            simul_ev = new Simulator_Event(ev, obj_desc);
            /* add the simulator event to the event list */
            event_list->add_tail_event(simul_ev);
            /* add the reference of this simul event to obj_ev table */
            if (first_ev == TRUE)
            {
                id = obj_desc->show_id();
                /* set the pointer to the first simul_ev */
                obj_ev_tb[id].simul_ev = simul_ev;
                /* set the number of events of this object */
                obj_ev_tb[id].no_events = ev_list->show_no_event();
                first_ev = FALSE;
            }
            /* go to the next event desc */
            ev = ev_list->show_next_event();
        }
        /* delete the event list desc of this object desc */
        delete ev_list;
        /* go to the next obj desc */
        obj_desc = obj_desc_list->show_next_obj_desc();
    }
    delete obj_desc_list;
}
//------------------------------------------------------------------------------
void Simulator::initialize_event_list()
{
    Simulator_Event    *simul_ev;
    Object_State       *obj_st;
    Object_Description *obj_desc;
    Rew_Reach_Distrib  *rew_reach_dist;

    /* go through all events */
    simul_ev = event_list->show_1st_event();
    while( simul_ev != NULL )
    {
        /* get the object state of curr_sys_st */
        obj_desc = simul_ev->show_obj_desc();
        obj_st   = curr_sys_st->show_obj_st( obj_desc );

        /* evaluate the condition under this object state */
        if( simul_ev->eval_cond( obj_st ) == TRUE )
        {
            /* If the event is REWARD_REACHED type they receives a diff */
            /*   treatment. All the other are treated the same way.     */        
            if( simul_ev->show_event()->show_distrib()->show_type() == REW_REACH_DIST )
            {
                    /* reset distrib before evaluate */
                    /* the parameter passed to reset() are unused for this distrib. */
                    simul_ev->show_event()->reset_distrib(0);

                    rew_reach_dist = (Rew_Reach_Distrib *)simul_ev->show_event()->show_distrib();
                    // This method analyse every special argument
                    eval_rr_special_args( rew_reach_dist );
            }

            /* generate a new sample for this event */
            simul_ev->generate_sample( obj_st, simulation_time );

            /* enable the event */
            simul_ev->ch_enable(TRUE);
        }
                
        /* go to the next event */
        simul_ev = event_list->show_next_event();
    }
}
//------------------------------------------------------------------------------
void Simulator::reset_event_list(int run)
{
    Object_Description_List *obj_desc_list;
    Object_Description      *obj_desc;
    Event_List              *ev_list;
    Event                   *ev;

    /* go through all obj desc of the system desc */
    obj_desc_list = the_system_desc->show_obj_desc_list(LIST_RO);
    obj_desc = obj_desc_list->show_1st_obj_desc();
    /* go through all the objects in the system */
    while (obj_desc != NULL)
    {
        ev_list = obj_desc->show_event_list(LIST_RO);
        ev = ev_list->show_1st_event();
        /* go through all the event desc of this obj desc */
        while (ev != NULL)
        {
            ev->reset_distrib(run);
            ev = ev_list->show_next_event();
        }
        /* delete the event list desc of this object desc */
        delete ev_list;
        /* go to the next obj desc */
        obj_desc = obj_desc_list->show_next_obj_desc();
    }
    delete obj_desc_list;
}
//------------------------------------------------------------------------------
void Simulator::process_event_list(Object_State *obj_st)
{
    Simulator_Event      *simul_ev;
    int                   i, id;
    Rew_Reach_Distrib    *rew_reach_dist = NULL;

    id = obj_st->show_id();

    /* get the first event of this object */
    simul_ev = obj_ev_tb[id].simul_ev;

    /* go through all events of this object */
    for( i = 0; i < obj_ev_tb[id].no_events; i++ )
    {
        /* If the event is REWARD_REACHED type then it's receives a diff */
        /*   treatment. All the other are treated the same way.     */
        if( simul_ev->show_event()->show_distrib()->show_type() == REW_REACH_DIST )
        {
            rew_reach_dist = (Rew_Reach_Distrib *)simul_ev->show_event()->show_distrib();
            // If trigger_now == TRUE then the sample list remains untouched. This assures
            //   that the event will happen in it's correct time (very close to now).
            if( ( rew_reach_dist->show_trigger_now()     == FALSE ) &&
                ( rew_reach_dist->show_need_evaluation() == TRUE ) )
            {
                //the RR event must be scheduled.

                /* reset samples - the parameter passed to reset() are unused for this distrib. */
                simul_ev->show_event()->reset_distrib(0);
                if( simul_ev->show_no_samples() != 0 )
                {
                    simul_ev->clear_ev_sample_list();
                }

                // Evaluation of the event condition without special arguments
                /* evaluate the condition under this object state of */
                if ( simul_ev->eval_cond( obj_st ) == TRUE )
                {
                    // At this point cond == TRUE, so the other expressions apart from
                    //   special allows the ocurrence of the event.
                    // So the special events must be analysed to get the delta_time.
                    // If no special argument represent a cross in the future delta_time
                    //   will be set to infinite. (The sample are created but if it's
                    //   value are equal to infinite, they will never trigger).

                    // This method analyse every special argument
                    eval_rr_special_args( rew_reach_dist );

                    // At this point delta_time is set in Rew_Reach_Dist so a new sample
                    // is generated. Note that the samples were removed before.
                    simul_ev->generate_sample( obj_st, simulation_time );
                    simul_ev->ch_enable( TRUE );
                }
                else
                {
                    /* set the enable flag to FALSE */
                    simul_ev->ch_enable(FALSE);
                }

                /* The event was evaluated, so the flag need_evaluation turns to FALSE */
                rew_reach_dist->ch_need_evaluation( FALSE );
            }
        }
        // other types of events, excluding INIT
        else if( simul_ev->show_event()->show_distrib()->show_type() != INIT_DIST )
        {
            /* evaluate the condition under this object state of */
            if( simul_ev->eval_cond(obj_st) == TRUE )
            {
                /* If enable was FALSE (just became enabled) or the sample list */
                /*   is empty (last event just triggered) resample the event.   */
                if( ( simul_ev->show_enable()     == FALSE ) ||
                    ( simul_ev->show_no_samples() == 0 ) )
                {
                    /* schedule a new sample if one of the conditions are satisfied */
                    simul_ev->generate_sample( obj_st, simulation_time );
                    simul_ev->ch_enable( TRUE );
                }
            }
            else
            {
                /* clear all samples of this event if it's not enabled */
                simul_ev->clear_ev_sample_list();
                /* set the enable flag to FALSE */
                simul_ev->ch_enable( FALSE );
            }
        }

        /* go to the next event */
        simul_ev = event_list->show_next_event( simul_ev );
    }
}
//------------------------------------------------------------------------------
// This method creates a list of Reward_Measure to be used by the simulator.
// First creates the Reward_Measures to represent each rate_reward of each
// object. In the tail of this list are added the Reward_Measures that
// represents the global rewards. After, a new list of rate_rew_sum objects
// are created, with a copy of each rate_rew_sum of each object. This unified
// list speeds up the simulation and simplify the work for the simulator.
// For the last, Reward_Measures that represent these rate_rew_sums are added
// in the tail of the Reward_Measure_List.
//------------------------------------------------------------------------------
void Simulator::create_rate_rew_list()
{
    Reward_Measure          *rew_meas = NULL;
    Object_State_List       *obj_st_list;
    Object_State            *obj_st;
    Object_Description      *obj_desc;
    Object_Description_List *obj_desc_list;
    Rate_Reward_Desc_List   *rew_desc_list;
    Rate_Reward_Desc        *rew_desc;
    char                     obj_name[MAXSTRING];
    char                     rew_name[MAXSTRING];
    char                     obj_rew_name[MAXSTRING];
    char                     rew_file_name[MAXSTRING];
    TGFLOAT                  bound, init_cr_val;
    Rate_Reward_Sum_List    *ra_rew_sum_list;
    Rate_Reward_Sum         *ra_rew_sum;
    Rate_Reward_Sum         *ra_rew_sum_aux = NULL;
    Reward_Sum_List         *rew_sum_list;
    Reward_Sum_List         *rew_sum_list_aux;
    Reward_Sum              *rew_sum;
    Reward_Sum              *rew_sum_aux;
    TGFLOAT                  cr_sum;

    /* get the object list of the current state */
    obj_st_list = curr_sys_st->show_obj_st_list(LIST_RO);

    /* get the first object state of this system state */
    obj_st = obj_st_list->show_1st_obj_st();

    /* go through all the object states of this system state */
    while (obj_st != NULL)
    {
        /* get the description of the object state */
        obj_desc = obj_st->show_obj_desc();

        /* get the name of the object */
        obj_desc->show_name(obj_name);

        /* get the first reward of the object above */
        rew_desc_list = obj_desc->show_reward_list(LIST_RO);
        rew_desc = rew_desc_list->show_1st_rew();

        /* go through all the rate rewards of the object */
        while (rew_desc != NULL)
        {
            /* get the name of the reward */
            rew_desc->show_name(rew_name);
            sprintf (obj_rew_name, "%s.%s",obj_name,rew_name);

            /* create a zeroed reward measure */
            rew_meas = new Reward_Measure (obj_rew_name, rew_desc->show_id() );

            /* define the type */
            rew_meas->ch_type( RATE_REWARD_TYPE );

            /* set the flag has level */
            rew_meas->ch_has_level(rew_desc->show_has_level());

            /* set the reward level */
            rew_meas->ch_rew_level(rew_desc->show_rew_level());

            /* if the reward is lower bounded then set the bounds */
            if (rew_desc->is_low_bounded())
            {
                rew_meas->ch_low_bounded(TRUE);
                bound = rew_desc->show_low_bound();
                rew_meas->ch_low_bound(bound);
            }

            /* if the reward is upper bounded then set the bounds */
            if (rew_desc->is_upp_bounded())
            {
                rew_meas->ch_upp_bounded(TRUE);
                bound = rew_desc->show_upp_bound();
                rew_meas->ch_upp_bound(bound);
            }

            /* set the cr initial value */
            /* only respects the local bound at this point */
            init_cr_val = rew_desc->show_cr_initial_value();
            if ( (rew_meas->is_upp_bounded()) && (init_cr_val > rew_meas->show_upp_bound()) )
                init_cr_val = rew_meas->show_upp_bound();
            if ( (rew_meas->is_low_bounded()) && (init_cr_val < rew_meas->show_low_bound()) )
                init_cr_val = rew_meas->show_low_bound();
            rew_meas->ch_cr_value( init_cr_val );
            rew_meas->set_first_cr( init_cr_val );
            /* open the trace files */
            if (Rew_Files == TRACE)
                rew_meas->open_trace_files(NULL);

            /* updates the pointer to the structure */
            rew_desc->ch_rew_meas( rew_meas );
            //fprintf(stderr,"saved rew_meas - %s\n",rew_name);

            /* add it to the reward list */
            rate_rew_list->add_tail_rew(rew_meas);

            /* go to the next reward description */
            rew_desc = rew_desc_list->show_next_rew();
        }
        delete rew_desc_list;

        /* go to the next object state */
        obj_st = obj_st_list->show_next_obj_st();
    }
    delete obj_st_list;

    //------------------------------------------------------------------
    /* insert the global rewards into the rew_measure list */
    rew_desc_list = the_system_desc->show_global_rew_desc_list(LIST_RO);
    rew_desc = rew_desc_list->show_1st_rew();
    /* go through all the global rate rewards description */
    while (rew_desc != NULL)
    {
        /* get the name of the reward */
        rew_desc->show_name(rew_name);
        sprintf (obj_rew_name, "%s.%s","GlobalReward",rew_name);

        /* create a zeroed reward measure */
        rew_meas = new Reward_Measure (obj_rew_name, rew_desc->show_id() );

        /* define the type */
        rew_meas->ch_type( GLOBAL_RATE_REWARD_TYPE );

        /* set the flag has level */
        rew_meas->ch_has_level(rew_desc->show_has_level());

        /* set the reward level */
        rew_meas->ch_rew_level(rew_desc->show_rew_level());

        /* if the reward is lower bounded then set the bounds */
        if (rew_desc->is_low_bounded())
        {
            rew_meas->ch_low_bounded(TRUE);
            bound = rew_desc->show_low_bound();
            rew_meas->ch_low_bound(bound);
        }

        /* if the reward is upper bounded then set the bounds */
        if (rew_desc->is_upp_bounded())
        {
            rew_meas->ch_upp_bounded(TRUE);
            bound = rew_desc->show_upp_bound();
            rew_meas->ch_upp_bound(bound);
        }

        /* set the cr initial value */
        init_cr_val = rew_desc->show_cr_initial_value();
        if ( (rew_meas->is_upp_bounded()) && (init_cr_val > rew_meas->show_upp_bound()) )
            init_cr_val = rew_meas->show_upp_bound();
        if ( (rew_meas->is_low_bounded()) && (init_cr_val < rew_meas->show_low_bound()) )
            init_cr_val = rew_meas->show_low_bound();
        rew_meas->ch_cr_value( init_cr_val );
        rew_meas->set_first_cr( init_cr_val );

        /* open the trace files */
        if (Rew_Files == TRACE)
            rew_meas->open_trace_files(NULL);

        /* add it to the reward list */
        rate_rew_list->add_tail_rew(rew_meas);

        /* go to the next reward description */
        rew_desc = rew_desc_list->show_next_rew();
    }
    delete rew_desc_list;

    //------------------------------------------------------------------
    /* creates a unified list of rate_rew_sum and insert the rate     */
    /* rewards sum into the tail of rew_measure list                  */
    /* IMPORTANT: The simulator uses the new list for every action    */
    /* correlated so the old distributed list is obsoleted from this  */
    /* point.                                                         */
    //------------------------------------------------------------------

    /* go through all obj desc of the system desc */
    obj_desc_list = the_system_desc->show_obj_desc_list(LIST_RO);
    obj_desc = obj_desc_list->show_1st_obj_desc();
    /* go through all the objects in the system */
    while (obj_desc != NULL)
    {
        /* get the name of the object */
        obj_desc->show_name( obj_name );

        ra_rew_sum_list = obj_desc->show_rate_rew_sum_list(LIST_RW);
        ra_rew_sum = ra_rew_sum_list->show_1st_rew();
        /* go through all the rate_reward_sum of the object */
        while( ra_rew_sum != NULL )
        {
            /* add rate_reward_sum in the simulator rate_rew_sum_list */
            ra_rew_sum_aux = new Rate_Reward_Sum( ra_rew_sum );
            rate_rew_sum_list->add_tail_rew( ra_rew_sum_aux );

            cr_sum = 0;

            /* update rew_measure attribute in each Reward_Sum of the */
            /* new instance.                                          */
            /* update rew_measure in the original too!                */
            rew_sum_list_aux = ra_rew_sum_aux->show_rew_sum_list(LIST_RW);
            rew_sum_aux = rew_sum_list_aux->show_1st_rew();
            rew_sum_list = ra_rew_sum->show_rew_sum_list(LIST_RW);
            rew_sum = rew_sum_list->show_1st_rew();
            /* go through all the reward_sum of the rate_reward_sum */
            while( rew_sum_aux != NULL )
            {
                /* get the reward name */
                rew_sum_aux->show_rew_name( rew_name );
                /* concat the obj name with rew name */
                sprintf (obj_rew_name, "%s.%s",obj_name, rew_name);
                if( rate_rew_list->query_rew( obj_rew_name ) > 0 )
                {
                    /* updates both reward_measure and rew_sum to became consistent */
                    rew_meas = rate_rew_list->show_curr_rew();
                    rew_meas->ch_rate_sum_bounded( TRUE );
                    rew_meas->ch_rate_sum_bound( ra_rew_sum_aux );
                    rew_sum_aux->ch_rew_measure( rew_meas );
                    rew_sum->ch_rew_measure( rew_meas );
                }
                else
                    fprintf( stderr, "Internal Error! Class:Simulator  Method: create_rate_rew_sum_list()\nReward %s not found.", rew_name );

                cr_sum += rew_meas->show_cr_value();

                rew_sum_aux = rew_sum_list_aux->show_next_rew();
                rew_sum = rew_sum_list->show_next_rew();
            }
            /* Updates the cr initial value that should be used in the */
            /*   begining of the simulation.                           */
            ra_rew_sum_aux->ch_cr_initial_value( cr_sum );

            //------------------------------------------------------------
            /* create a Reward_Measure to represent the Rate_Reward_Sum */

            /* get the name of the reward */
            ra_rew_sum->show_name( rew_name );
            sprintf (obj_rew_name, "%s.%s",obj_name,rew_name);

//fprintf( stderr,"criando %s\n",obj_rew_name);

            /* create a zeroed reward measure */
            rew_meas = new Reward_Measure (obj_rew_name, ra_rew_sum->show_id() );

            /* define the type */
            rew_meas->ch_type( RATE_REWARD_SUM_TYPE );

            /* if the reward is lower bounded then set the bounds */
            if ( ra_rew_sum->is_low_bounded() )
            {
                rew_meas->ch_low_bounded( TRUE );
                bound = ra_rew_sum->show_low_bound();
                rew_meas->ch_low_bound( bound );
            }

            /* if the reward is upper bounded then set the bounds */
            if ( ra_rew_sum->is_upp_bounded() )
            {
                rew_meas->ch_upp_bounded( TRUE );
                bound = ra_rew_sum->show_upp_bound();
                rew_meas->ch_upp_bound( bound );
            }

            /* set the cr initial value                                    */
            /* the tool doesn't check the consistence of individual        */
            /* rewards when the initial value of the sum is out of bounds. */
            init_cr_val = cr_sum;
            if ( (rew_meas->is_upp_bounded()) && (init_cr_val > rew_meas->show_upp_bound()) )
                init_cr_val = rew_meas->show_upp_bound();
            if ( (rew_meas->is_low_bounded()) && (init_cr_val < rew_meas->show_low_bound()) )
                init_cr_val = rew_meas->show_low_bound();
            rew_meas->ch_cr_value( init_cr_val );
            rew_meas->set_first_cr( init_cr_val );

            /* open the trace files */
            if (Rew_Files == TRACE)
            {
                strcpy( rew_file_name, obj_rew_name );
                strcat( rew_file_name, ".SUM" );
                if ( ! rew_meas->open_trace_files( rew_file_name ) )
                {
                    fprintf( stderr, "Error opening reward sum trace file!\n");
                }
            }
            /* save the pointer to this reward measure in both rate reward sum */
            ra_rew_sum->ch_rew_measure( rew_meas );
            ra_rew_sum_aux->ch_rew_measure( rew_meas );
            /* add it to the reward list */
            rate_rew_list->add_tail_rew( rew_meas );
            //------------------------------------------------------------

            ra_rew_sum = ra_rew_sum_list->show_next_rew();
        }
        obj_desc = obj_desc_list->show_next_obj_desc();
    }
}
//------------------------------------------------------------------------------
void Simulator::update_rate_rewards( Simulator_Event *simul_ev )
{
    Rate_Reward_List *new_rate_list;

    pthread_mutex_lock(&simulator_lock);
    /* if rate rewards were defined in this model */
    if (the_system_desc->show_rate_reward() == TRUE)
    {
        /* calculate the object rewards for the explored state */
        if ( rr_ev_flag == TRUE )
        {
            curr_sys_st->calculate_rate_rewards( CHECK_IR_CHANGE );
        }
        else
        {
            curr_sys_st->calculate_rate_rewards( NO_CHECK );
        }
        /* calculate the global rewards for this state */
        curr_sys_st->calculate_global_rate_rewards();
        /* get the calculated reward list */
        new_rate_list = curr_sys_st->get_rate_reward_list();
        /* accumulate the reward in the current sys state */
        add_rate_rew_list(new_rate_list, simul_ev);
    }
    pthread_mutex_unlock(&simulator_lock);
}
//------------------------------------------------------------------------------
/* obs: since all rate reward variable are updated every simulation step      */
/* we use last_simul_time value instead of rew_meas->time that represents     */
/* the last time the reward was updated (which is only used in impulse        */
/* reward value)                                                              */
/* obs2: if the event is REW_REACH and it's trigging right now, the CR(t)     */
/* is extracted from distribution instead of to be calculated. This is        */
/* necessary to minimize numeric problems.(1-stamp,2**-calc using CR and IR)  */
/*      ** should not be used in fake times.                                  */
/* obs3: Optimized. if the time didn't progress, so this function won't       */
/* be called.                                                                 */
//------------------------------------------------------------------------------
void Simulator::add_rate_rew_list( Rate_Reward_List *new_rate_list,
                                   Simulator_Event *simul_ev )
{
    Reward_Measure  *rew_meas;
    Rate_Reward     *rew_aux;
    Expr_Val        *rate_aux = NULL;
    TGFLOAT          curr_cr_val, curr_avg_cr_val, new_val=0, new_avg_cr_val;
    TGFLOAT          time_abv_lv = 0, level;
    TGFLOAT          delta_t, Area1, Area2, x_point;
    //FILE            *fd;

    Stamp              *stamp;
    Stamp_List         *stamp_list;
    Distribution       *distrib;
    Rew_Reach_Distrib  *rew_reach_dist = NULL;

    int                    have_some_rate_sum_bound = FALSE;
    Reward_Measure        *rew_sum_meas = NULL;
    Rate_Reward_Sum_List  *ra_rew_sum_list;
    Rate_Reward_Sum       *ra_rew_sum;
    Reward_Sum_List       *rew_sum_list;
    Reward_Sum            *rew_sum;
    Reward_Sum            *rew_of_min_cross_time = NULL;
    TGFLOAT                min_cross_time = INFINITE;
    TGFLOAT                sum_cross_time = INFINITE;
    TGFLOAT                last_time, time_step = 0, cross_time;
    TGFLOAT                cr_stamp = 0, cr_sum_accum = 0, ir_sum_accum = 0;
    TGFLOAT                ir_sum = 0, cr_sum = 0, ir_neg = 0, ir_pos = 0;
    TGFLOAT                virtual_ir = 0;
    int                    all_updated = FALSE, act_rew = FALSE;

    //----- debug info --------------------------
    int     devel_debug = FALSE; // enables the printing of devel debug info
    TGFLOAT dbg_time_low=-1, dbg_time_upp=3333; // the range of time to print
    char    rew_name[MAXSTRING];

    /* simulation step in time */
    delta_t = simulation_time - last_simul_time;

    /*----------------------- 1  pass ---------------------------*/
    /* This pass will only deal with rate rewards that are not   */
    /* controled by rate reward sum.                             */

    /* get the first reward */
    rew_meas = rate_rew_list->show_1st_rew();
    rew_aux  = new_rate_list->show_1st_rew();

    /* go through all the rate rewards and global rate rewards   */
    /* in the list. rew_aux contains this 2 types. rew_meas has  */
    /* a third type after these ones, but this kind of reward    */
    /* measure (RATE_REWARD_SUM_TYPE) is updated during the      */
    /* second pass of this method.                               */
    while (rew_aux != NULL)
    {
        /* get the value of the reward just computed */
        rate_aux = rew_aux->show_value();

        /* update curr_ir_value. If the reward measure is sum bounded this    */
        /* will be used in this call, in the second pass. Anyway, this        */
        /* attribute must be updated for other uses.  eg: get_ir user command.*/
        rew_meas->ch_ir_value( rate_aux->show_value() );

        if ( rew_meas->is_rate_sum_bounded() == FALSE )
        {
            /* update curr_cr_value, avg_cr_val and time_abv_lv */

            /* get the current value of the function CR(t), the last time it was updated */
            curr_cr_val = rew_meas->show_cr_value();

            /* calculate the new value for CR(t). PS: if the event that is          */
            /*   trigging right now is REW_REACH and the reward is the one that     */
            /*   causes the trigger, we get this value from REW_REACH_DIST instead  */
            /*   to calculate.                                                      */
            /* note: we have id of the reward stored in REW_REACH_DIST, so it must  */
            /*       be the same as rew_meas.                                       */
            /* if this time is fake, the CR and IR of the reward could not be used  */
            /*   just because they not represent the delta_time that is used in     */
            /*   the calculum of new_val! (delta_time belongs to other reward or    */
            /*   slop changes that have ocurred before the delta_time of the reward */
            /*   analysed in will_reach_reward function.                            */

            /* calculate the new cr value */
            distrib = simul_ev->show_event()->show_distrib();
            if ( distrib->show_type() == REW_REACH_DIST )
            {
                rew_reach_dist = (Rew_Reach_Distrib *)distrib;
                if ( rew_reach_dist->show_skip_event() == FALSE )
                {
                    stamp_list = rew_reach_dist->show_stamp_list( LIST_RO );
                    if ( stamp_list->query_stamp(rew_meas->show_id()) )
                    {
                        stamp = stamp_list->show_curr_stamp();
                        new_val = stamp->show_cr_target();

                        //----- develop debug info ------------------
                        if ( (devel_debug == TRUE) && (simulation_time > dbg_time_low) && (simulation_time < dbg_time_upp) )
                        {
                            fprintf( stderr, "---------\nupdating rate_rewards...\ntime=%f  run=%d   case A\n",simulation_time,run_number);
                        }
                        //----- end of develop debug info -----------
                    }
                    else
                    {
                        /* if rew_reach_dist->show_ir() == 0, the event won't happen. */
                        /* this is not the case, just cause we are here!              */
                        
			// Kelvin: Old calculus.
			// new_val = rew_meas->show_cr_value() + ( rew_reach_dist->show_level() -  rew_reach_dist->show_cr() )  * ( rew_meas->show_ir_value() / rew_reach_dist->show_ir() );
			new_val = rew_meas->show_cr_value() + rew_meas->show_ir_value() * delta_t;

                        //----- develop debug info ------------------
                        if ( (devel_debug == TRUE) && (simulation_time > dbg_time_low) && (simulation_time < dbg_time_upp) )
                        {
                            rew_meas->show_name( rew_name );
                            fprintf( stderr, "---------\nupdating rate_rewards...\ntime=%f  run=%d  case B rew=%s\n %f + ( %f - %f ) * ( %f / %f)\n",simulation_time,run_number,rew_name,
                                    rew_meas->show_cr_value(),rew_reach_dist->show_level(),rew_reach_dist->show_cr(),
                                    rew_meas->show_ir_value(),rew_reach_dist->show_ir());
                        }
                        //----- end of develop debug info -----------
                    }
                    /* remove structure that was returned in rew_reach_dist->show_stamp_list( LIST_RO ) */
                    delete stamp_list;
                }
                else
                {
                    new_val = curr_cr_val + rate_aux->show_value() * delta_t;

                    //----- develop debug info ------------------
                    if ( (devel_debug == TRUE) && (simulation_time > dbg_time_low) && (simulation_time < dbg_time_upp) )
                    {
                        rew_meas->show_name( rew_name );
                        fprintf( stderr, "---------\nupdating rate_rewards...\ntime=%f  run=%d   case C rew=%s\n",simulation_time,run_number,rew_name);
                    }
                    //----- end of develop debug info -----------
                }
            }
            else
            {
                new_val = curr_cr_val + rate_aux->show_value() * delta_t;

                //----- develop debug info ------------------
                if ( (devel_debug == TRUE) && (simulation_time > dbg_time_low) && (simulation_time < dbg_time_upp) )
                {
                    rew_meas->show_name( rew_name );
                    fprintf( stderr, "---------\nupdating rate_rewards...\ntime=%f  run=%d   case D rew=%s\n",simulation_time,run_number,rew_name);
                    fprintf( stderr, "curr_cr_val=%f  new_val=%f  ir=%f  delta_t=%f\n",curr_cr_val,new_val,rate_aux->show_value(), delta_t);
                }
                //----- end of develop debug info -----------
            }

            /* calculate the time above a given reward level */
            /* it is calculated only if has_level == TRUE    */
            if ( rew_meas->show_has_level() == TRUE )
            {
                level = rew_meas->show_rew_level();
                time_abv_lv = rew_meas->show_time_abv_lv();
                if ( (curr_cr_val - level) * (new_val - level) < 0)
                {
                    /* accumulated reward crossed the reward level */
                    /* calculate the cross point */
                    x_point = (level - curr_cr_val) / rate_aux->show_value() + last_simul_time;
                    if (curr_cr_val < new_val)
                        /* downcrossing the level */
                        time_abv_lv += simulation_time - x_point;
                    else
                        /* upcrossing the level */
                        time_abv_lv += x_point - last_simul_time;
                }
                else
                    /* the accum reward has not crossed the level */
                    if ( new_val > level)
                        /* but accum reward value is over the level */
                        time_abv_lv += delta_t;

                /* update the reward */
                rew_meas->ch_time_abv_lv( time_abv_lv );
            }

            /* if this reward is bounded, correct the reward if it is out of bounds */
            if ( (rew_meas->is_low_bounded() && (new_val < rew_meas->show_low_bound())) ||
                 (rew_meas->is_upp_bounded() && (new_val > rew_meas->show_upp_bound())) )
            {
                /* set the new value for the reward */
                if (rew_meas->is_upp_bounded() && (new_val > rew_meas->show_upp_bound()))
                    new_val = rew_meas->show_upp_bound();
                else
                    new_val = rew_meas->show_low_bound();

                /* calculate the area under the reward and the bound */
                x_point = (new_val - curr_cr_val) / rate_aux->show_value() + last_simul_time;
                Area1 = (curr_cr_val + new_val) / 2 * (x_point - last_simul_time);
                Area2 = new_val * (simulation_time - x_point);
                curr_avg_cr_val = rew_meas->show_avg_cr_value();
                new_avg_cr_val = curr_avg_cr_val +  Area1 + Area2;
                /* include the break point in the cr trace file */
                rew_meas->conditional_printf_cr((reward_to_print & PRINT_REWARD_CR), x_point, new_val);
            }
            else
            {
                /* calculate the area under the reward */
                curr_avg_cr_val = rew_meas->show_avg_cr_value();
                new_avg_cr_val = curr_avg_cr_val + ((curr_cr_val + new_val) / 2) * delta_t;
            }

            /* Verify if CR has reached a bound of some REWARD_REACHED event condition. */
            /* Only if there is at least one REWARD_REACHED event in the simulation     */
            /* (indicated by rr_ev_flag).                                               */
            if ( rr_ev_flag == TRUE )
            {
                check_cr_touch( rew_meas, simul_ev, curr_cr_val, new_val );
            }

            /* update the reward */
            rew_meas->ch_cr_value( new_val );
            rew_meas->ch_avg_cr_value( new_avg_cr_val );

            /* generate the cr trace file */
            rew_meas->conditional_printf_cr((reward_to_print & PRINT_REWARD_CR), simulation_time, new_val);
            /* generate the ir trace file */
// BRUNO:                fprintf (fd, "%.10E %.10E\n", last_simul_time, rate_aux->show_value());
            rew_meas->conditional_printf_ir ((reward_to_print & PRINT_REWARD_IR), last_simul_time, rate_aux->show_value());
        }
        else
        {
            /* indicates the necessity for the second pass */
            have_some_rate_sum_bound = TRUE;
        }

//        rew_meas->reward_update_last_ir();
//        rew_meas->reward_update_last_cr();
        /* go to the next reward in the list */
        rew_meas = rate_rew_list->show_next_rew();
        rew_aux  = new_rate_list->show_next_rew();
    }

    /* delete the added list */
    delete new_rate_list;

    /*----------------------- 2 pass ---------------------------*/
    /* This pass will only deal with rate rewards that are      */
    /* controled by rate reward sum.                            */
    /* This pass updates too the reward measures of the         */
    /* RATE_REWARD_SUM_TYPE.                                    */

    if ( have_some_rate_sum_bound == TRUE )
    {
//ncomp//fprintf(stderr,"----------------------------\n" );
//ncomp//fprintf(stderr,"last_simul_time=%f   simulation_time=%f \n", last_simul_time, simulation_time);

        /* go throught all Rate_Reward_Sum objects */
        ra_rew_sum_list = show_rate_rew_sum_list(LIST_RW);
        ra_rew_sum = ra_rew_sum_list->show_1st_rew();
        while ( ra_rew_sum != NULL )
        {
            /* ------------ initialization ------------------------- */
            /* aims: 1) for each object:                             */
            /*            virtual_ir=Reward_Measure.curr_ir_value or */
            /*            virtual_ir=0 if local bound is reached.    */
            /*       2) get ir_pos, ir_neg, ir_sum and cr_sum.       */

            /* go throught all Reward_Sum objects */
            ir_sum = 0;
            cr_sum = 0;
            ir_neg = 0;
            ir_pos = 0;
            rew_sum_list = ra_rew_sum->show_rew_sum_list(LIST_RW);
            rew_sum = rew_sum_list->show_1st_rew();
            while(rew_sum != NULL)
            {
                rew_meas = rew_sum->show_rew_measure();
                cr_sum += rew_meas->show_cr_value();

                /* initialize virtual_ir and local_bound_reached */
                virtual_ir = rew_meas->show_ir_value();
                rew_sum->ch_virtual_ir( virtual_ir );
                rew_sum->ch_local_bound_reached( FALSE );

    /*----- debug info -----*/
    //rew_meas->show_name( rew_name );
    //fprintf ( stderr, "time: %f  -  run: %d  -  updating cr: %s  -  virtual_ir: %f\n",simulation_time,run_number,rew_name,virtual_ir);
    /*----- end of debug info -----*/

                /* gravity system for rate_reward local bound, does not need to act, */
                /* cause if the CR value of rate_reward came close to a bound in     */
                /* the last rate_reward update the gravity already attracted it.     */
                /* however virtual_ir should be 0 if the bound was already reached.  */
                if ( (rew_meas->is_upp_bounded() == TRUE) && (rew_meas->show_ir_value() > 0) &&
                    (rew_meas->show_cr_value() >= (rew_meas->show_upp_bound() - y_epsilon)) )
                {
                    rew_sum->ch_local_bound_reached( TRUE );
                    rew_sum->ch_virtual_ir( 0 );
                    virtual_ir = 0;
//nocomp//fprintf(stderr, "(UPP) ADD s_time=%f - cr=%f - ir=%f - upp=%f\n",simulation_time,rew_meas->show_cr_value(), rew_meas->show_ir_value(),rew_meas->show_upp_bound());
                }
                if ( (rew_meas->is_low_bounded() == TRUE) && (rew_meas->show_ir_value() < 0) &&
                    (rew_meas->show_cr_value() <= (rew_meas->show_low_bound() + y_epsilon)) )
                {
//fprintf(stderr, "(LOW) ADD s_time=%f - cr=%f - ir=%f - low=%f\n",simulation_time,rew_meas->show_cr_value(), rew_meas->show_ir_value(),rew_meas->show_low_bound());
                    rew_sum->ch_local_bound_reached( TRUE );
                    rew_sum->ch_virtual_ir( 0 );
                    virtual_ir = 0;
                }

                ir_sum += virtual_ir;
                if( virtual_ir < 0 )
                    ir_neg += virtual_ir;
                else
                    ir_pos += virtual_ir;

                // go to the next rew_sum.
                rew_sum = rew_sum_list->show_next_rew();
            }

            // gravity system for rate_reward_sum bound.
            if ( (ra_rew_sum->is_upp_bounded() == TRUE) && (cr_sum > (ra_rew_sum->show_upp_bound()-y_epsilon)) )
                cr_sum = ra_rew_sum->show_upp_bound();
            if ( (ra_rew_sum->is_low_bounded() == TRUE) && (cr_sum < (ra_rew_sum->show_low_bound()+y_epsilon)) )
                cr_sum = ra_rew_sum->show_low_bound();

            calculate_virtual_ir( ra_rew_sum , ir_sum, ir_neg, ir_pos, ir_sum, cr_sum);

            last_time = last_simul_time;
            all_updated = FALSE;

            while ( all_updated == FALSE )
            {
                /* The update of the cr_value, time_abv_lv and avg_cr_value */
                /* are done in steps.                                       */
                /* Note that the algorithm get each rate change and this are the */
                /* well defined steps. step: last_time until last_time+delta_t   */

                /* ------ get min_cross_time and ir/cr_sum ----- */
                min_cross_time = INFINITE;
                ir_sum = 0;
                cr_sum = 0;

                /* go throught all Reward_Sum objects */
                rew_sum = rew_sum_list->show_1st_rew();
                while (rew_sum != NULL)
                {
                    rew_meas = rew_sum->show_rew_measure();
                    /*------------------ calculate cross_time --------------------*/
                    cross_time = INFINITE;
                    if ( rew_sum->show_virtual_ir() != 0)
                        if ( rew_sum->show_virtual_ir() > 0 )
                        {
                            /* In theory the cr value at this moment should be exact */
                            /* However we used y_epsilon to avoid possibly precision errors */
                            if ( (rew_meas->is_upp_bounded() == TRUE) &&
                                (rew_meas->show_cr_value() < (rew_meas->show_upp_bound() - y_epsilon)) )
                            {
                                cross_time = ( rew_meas->show_upp_bound() - rew_meas->show_cr_value() ) / rew_sum->show_virtual_ir();
                            }
                        }
                        else
                        {
                            /* In theory the cr value at this moment should be exact */
                            /* However we used y_epsilon to avoid possibly precision errors */
                            if ( (rew_meas->is_low_bounded() == TRUE) &&
                                (rew_meas->show_cr_value() > (rew_meas->show_low_bound() + y_epsilon)) )
                            {
                                cross_time = ( rew_meas->show_low_bound() - rew_meas->show_cr_value() ) / rew_sum->show_virtual_ir();
                            }
                        }

                    rew_sum->ch_cross_time( cross_time );

                    if ( cross_time < min_cross_time )
                    {
                        min_cross_time = cross_time;
                        rew_of_min_cross_time = rew_sum;
                    }
                    /*------------------end of calc cross_time -------------------*/
                    ir_sum += rew_sum->show_virtual_ir();
                    cr_sum += rew_meas->show_cr_value();

                    rew_sum = rew_sum_list->show_next_rew();
                }

                /*---------------- calculate sum_cross_time ------------------*/
                sum_cross_time = INFINITE;
                if ( ir_sum != 0 )
                    if ( ir_sum > 0 )
                    {
                        if ( (ra_rew_sum->is_upp_bounded() == TRUE) && (cr_sum < (ra_rew_sum->show_upp_bound()-y_epsilon)) )
                            sum_cross_time = ( ra_rew_sum->show_upp_bound() - cr_sum ) / ir_sum;
                    }
                    else
                    {
                        if ( (ra_rew_sum->is_low_bounded() == TRUE) && (cr_sum > (ra_rew_sum->show_low_bound()+y_epsilon)) )
                            sum_cross_time = ( ra_rew_sum->show_low_bound() - cr_sum ) / ir_sum;
                    }
                /*--------------- end of calc sum_cross_time -----------------*/

                if ( (min_cross_time >= (delta_t - x_epsilon) ) && (sum_cross_time >= (delta_t - x_epsilon)) )
                {
                    /*-------- last phase of the update. no more rate changes for all curves. --------*/
                    cr_sum_accum = 0;
                    ir_sum_accum = 0;
                    new_avg_cr_val = 0;

                    /* go throught all Reward_Sum objects */
                    rew_sum = rew_sum_list->show_1st_rew();
                    while (rew_sum != NULL)
                    {
                        /* indicates whether the cr value should be stamped */
                        act_rew = FALSE;
                        rew_meas = rew_sum->show_rew_measure();

                        if( rew_sum->is_local_bound_reached() )
                        {
                            // just repeat the last cr
                            act_rew = TRUE;
                            cr_stamp = rew_meas->show_cr_value();
                        }

                        update_cr( rew_sum, act_rew, cr_stamp, last_time, simulation_time, simul_ev, CHECK_CR_TOUCH );

                        /* used in Rate Reward Sum trace files */
                        cr_sum_accum += rew_sum->show_rew_measure()->show_cr_value();
                        ir_sum_accum += rew_sum->show_rew_measure()->show_ir_value();
                        new_avg_cr_val += rew_meas->show_avg_cr_value();

                        rew_sum = rew_sum_list->show_next_rew();
                    }

                    // gravity system for rate_reward_sum bound.
                    if ( (ra_rew_sum->is_upp_bounded() == TRUE) && (cr_sum_accum > (ra_rew_sum->show_upp_bound()-y_epsilon)) )
                        cr_sum_accum = ra_rew_sum->show_upp_bound();
                    if ( (ra_rew_sum->is_low_bounded() == TRUE) && (cr_sum_accum < (ra_rew_sum->show_low_bound()+y_epsilon)) )
                        cr_sum_accum = ra_rew_sum->show_low_bound();

                    /* updates the reward measure of this rate reward sum */
                    rew_sum_meas = ra_rew_sum->show_rew_measure();
                    rew_sum_meas->ch_cr_value( cr_sum_accum );
                    rew_sum_meas->ch_ir_value( ir_sum_accum );
                    rew_sum_meas->ch_avg_cr_value( new_avg_cr_val );

                    /* generate the cr trace file for the rate reward sum */
                    rew_sum_meas->conditional_printf_cr ((reward_to_print & PRINT_REWARD_CR), simulation_time, cr_sum_accum );
                    /* generate the ir trace file for the rate reward sum */
// BRUNO:                        fprintf (fd, "%.10E %.10E\n", last_time, ir_sum_accum );
                    rew_sum_meas->conditional_printf_ir ((reward_to_print & PRINT_REWARD_IR), last_simul_time, ir_sum_accum );

                    all_updated = TRUE;
                    //----- Exit point! -----
                }
                else
                {
                    /*-------- advances one step --------*/
                    if ( sum_cross_time < min_cross_time )
                    {
                        time_step = sum_cross_time;
//ncomp//fprintf(stderr,"time_step=%f   ----- regido por sum_cross\n", time_step);
                    }
                    else
                    {
                        time_step = min_cross_time;

//ncomp//rew_of_min_cross_time->show_rew_name( name );
//ncomp//fprintf(stderr,"time_step=%f   ----- regido por min_cross ---  rew_sum=%s\n", time_step,name );
                    }
                    /*-------- progress analiser --------*/
                    /* Avoid time_step values less than x_epsilon. This is necessary */
                    /* to eliminate no progress in simulation time.                  */
                    if ( time_step < x_epsilon )
                    {
                        //fprintf( stderr, "Warning: Simulation time_step < x_epsilon. Correcting to x_epsilon to avoid no progress!\n");
                        time_step = x_epsilon;
                    }
                    //time_step = min_cross_time <? sum_cross_time;

                    /* go throught all Reward_Sum objects     */
                    /* aims: 1) update till time_step         */
                    /*       2) get ir_pos and ir_neg         */
                    /*       3) get cr, ir and avg of the sum */
                    ir_neg = 0;
                    ir_pos = 0;
                    ir_sum = 0;
                    cr_sum_accum = 0;
                    ir_sum_accum = 0;
                    new_avg_cr_val = 0;
                    rew_sum = rew_sum_list->show_1st_rew();
                    while (rew_sum != NULL)
                    {
                        /* indicates whether the cr value should be stamped */
                        act_rew = FALSE;

                        rew_meas = rew_sum->show_rew_measure();
                        if ( rew_sum == rew_of_min_cross_time )
                        {
                            if ( min_cross_time < sum_cross_time )
                            {
                                /* indicates that this reward does not need more updates */
                                rew_sum->ch_local_bound_reached( TRUE );
                                rew_sum->ch_virtual_ir( 0 );
                                if ( rew_meas->show_ir_value() > 0 )
                                    cr_stamp = rew_meas->show_upp_bound();
                                else
                                    cr_stamp = rew_meas->show_low_bound();
                                act_rew = TRUE;
//ncomp//rew_meas->show_name(name);
//ncomp//fprintf(stderr,"==== carimbei new_val=%f  rew_id=%d  rew_name=%s\n",cr_stamp, rew_meas->show_id(), name );

                            }
                        }

                        update_cr( rew_sum, act_rew, cr_stamp, last_time, last_time + time_step, NULL, NO_CHECK );

                        ir_sum += rew_sum->show_virtual_ir();
                        if ( rew_sum->show_virtual_ir() < 0 )
                        {
                            ir_neg += rew_sum->show_virtual_ir();
//ncomp//fprintf(stderr,"acumulado : ir_neg=%f virtual_ir=%f\n",ir_neg,rew_sum->show_virtual_ir());
                        }
                        else
                        {
                            ir_pos += rew_sum->show_virtual_ir();
//ncomp//fprintf(stderr,"acumulado : ir_pos=%f virtual_ir=%f\n",ir_pos,rew_sum->show_virtual_ir());
                        }

                        /* used to update reward measure of the Reward Sum */
                        cr_sum_accum += rew_sum->show_rew_measure()->show_cr_value();
                        ir_sum_accum += rew_sum->show_rew_measure()->show_ir_value();
                        new_avg_cr_val += rew_meas->show_avg_cr_value();

                        rew_sum = rew_sum_list->show_next_rew();
                    }

                    // gravity system for rate_reward_sum bound.
                    if ( (ra_rew_sum->is_upp_bounded() == TRUE) && (cr_sum_accum > (ra_rew_sum->show_upp_bound()-y_epsilon)) )
                        cr_sum_accum = ra_rew_sum->show_upp_bound();
                    if ( (ra_rew_sum->is_low_bounded() == TRUE) && (cr_sum_accum < (ra_rew_sum->show_low_bound()+y_epsilon)) )
                        cr_sum_accum = ra_rew_sum->show_low_bound();

                    /* updates the reward measure of this rate reward sum */
                    rew_sum_meas = ra_rew_sum->show_rew_measure();
                    rew_sum_meas->ch_cr_value( cr_sum_accum );
                    rew_sum_meas->ch_ir_value( ir_sum_accum );
                    rew_sum_meas->ch_avg_cr_value( new_avg_cr_val );

                    /* generate the cr trace file for the rate reward sum */
                    rew_sum_meas->conditional_printf_cr ((reward_to_print & PRINT_REWARD_CR), last_time + time_step, cr_sum_accum );
                    /* generate the ir trace file for the rate reward sum */
//BRUNO:                        fprintf (fd, "%.10E %.10E\n", last_time, ir_sum_accum );
                    rew_sum_meas->conditional_printf_ir ((reward_to_print & PRINT_REWARD_IR), last_time, ir_sum_accum );

                    last_time = last_time + time_step;
                    delta_t = simulation_time - last_time;
                    /* at this time the updated walked one more step */

//ncomp//fprintf(stderr,"calculate   virtul  ir_sum=%f neg=%f pos=%f\n",ir_sum, ir_neg, ir_pos);
                    /* calculate new virtual_ir if the cr_sum == sum_bound */
                    calculate_virtual_ir( ra_rew_sum , ir_sum, ir_neg, ir_pos, ir_sum_accum, cr_sum_accum);
                }
            }
            ra_rew_sum = ra_rew_sum_list->show_next_rew();
        }
    }
}
//------------------------------------------------------------------------------
/*  This method are executed every time rate_reward_sum reachs a bound.       */
/*  Are executed too, in the begining of update_rate_rewards case the sum     */
/*  are equal to a bound.                                                     */
/*  This method modify only the virtual_ir of the reward_sum objects.         */

/*  Requirements: a) Gravity system must have attracted the CR values if they */
/*     are close to their bounds. Both sum and local must be attracted!!      */
/*     b) virtual_ir and local_bound_reached must be set when the local bound */
/*     already is reached.                                                    */
//------------------------------------------------------------------------------
void Simulator::calculate_virtual_ir( Rate_Reward_Sum *ra_rew_sum ,TGFLOAT irv_sum,
                     TGFLOAT irv_neg, TGFLOAT irv_pos, TGFLOAT ir_sum, TGFLOAT cr_sum)
{
    Reward_Sum       *rew_sum;
    Reward_Sum_List  *rew_sum_list;
    TGFLOAT           ir_value, slice;

    // get rate_reward_sum_list
    rew_sum_list = ra_rew_sum->show_rew_sum_list(LIST_RW);
    rew_sum = rew_sum_list->show_1st_rew();

    if ( (ra_rew_sum->is_upp_bounded() == TRUE) && (cr_sum == ra_rew_sum->show_upp_bound()) )
    {
        // Touching upper bound
        /* Only changes virtual_ir of the rewards that  */
        /*    not reached local bound and have ir > 0   */
        if ( irv_neg < 0 )
        {
            /* go throught all Reward_Sum objects */
            rew_sum = rew_sum_list->show_1st_rew();
            while (rew_sum != NULL)
            {
                if ( ! rew_sum->is_local_bound_reached() )
                {
                    ir_value = rew_sum->show_rew_measure()->show_ir_value();
                    if ( ir_value > 0 )
                    {
                       /* slice for this reward */
                       slice = ( ir_value / irv_pos ) * ( - irv_neg );
                       if ( slice > ir_value )
                           rew_sum->ch_virtual_ir( ir_value );
                       else
                           rew_sum->ch_virtual_ir( slice );
                    }
                }
                rew_sum = rew_sum_list->show_next_rew();
            }
        }
        else
        {
            /* Changes all virtual_ir to zero (there are no ir_neg to distribute) */
            /* go throught all Reward_Sum objects */
            rew_sum = rew_sum_list->show_1st_rew();
            while (rew_sum != NULL)
            {
                rew_sum->ch_virtual_ir( 0 );
                rew_sum = rew_sum_list->show_next_rew();
            }
        }
    }
    else
    {
        if ( (ra_rew_sum->is_low_bounded() == TRUE) && (cr_sum == ra_rew_sum->show_low_bound()) )
        {
            // Touching lower bound
            /* Only changes virtual_ir of the rewards that  */
            /*    not reached local bound and have ir < 0   */
            if ( irv_pos > 0 )
            {
                /* go throught all Reward_Sum objects */
                rew_sum = rew_sum_list->show_1st_rew();
                while (rew_sum != NULL)
                {
                    if ( ! rew_sum->is_local_bound_reached() )
                    {
                        ir_value = rew_sum->show_rew_measure()->show_ir_value();
                        if ( ir_value < 0 )
                        {
                           /* slice for this reward */
                           slice = ( ir_value / irv_neg ) * irv_pos;
                           if ( slice > (- ir_value) )
                               rew_sum->ch_virtual_ir( ir_value );
                           else
                               rew_sum->ch_virtual_ir( - slice );
//ncomp//fprintf(stderr,"passei aqui e ir_value=%f neg=%f pos=%f slice=%f\n",ir_value, irv_neg, irv_pos, slice);
                        }
                    }
                    rew_sum = rew_sum_list->show_next_rew();
                }
            }
            else
            {
                /* Changes all virtual_ir to zero (there are no ir_pos to distribute) */
                /* go throught all Reward_Sum objects */
                rew_sum = rew_sum_list->show_1st_rew();
                while (rew_sum != NULL)
                {
                    rew_sum->ch_virtual_ir( 0 );
                    rew_sum = rew_sum_list->show_next_rew();
                }
            }
        }
        else
        {
           // The bounding system is not acting. The ir continues the same.
           // This situation occur when some local bound is reached.
        }
    }
}
//------------------------------------------------------------------------------
/* This method calculates cr_value, time_abv_lv and avg_cr_value, of one      */
/*    reward from init_time to end_time, store the information in the         */
/*    respective reward_measure obj and write CR and IR traces.               */
/* Arguments:                                                                 */
/* 1- rew_sum  : Pointer to the Reward_Sum that monitors the Reward_Measure   */
/* 2- act_rew  : If true, the value must be stamped                           */
/* 3- cr_val   : The exact value of CR, to be stamped if necessary            */
/* 4- init_time: Initial time to be considered                                */
/* 5- end_time : The reward must be updated up to the end_time                */
//------------------------------------------------------------------------------
void Simulator::update_cr( Reward_Sum *rew_sum, int act_rew, TGFLOAT cr_val,
                           TGFLOAT init_time, TGFLOAT end_time,
                           Simulator_Event *simul_ev, int check )
{
    TGFLOAT  level, new_val, time_abv_lv = 0, curr_cr_val, x_point;
    TGFLOAT  curr_avg_cr_val, new_avg_cr_val, delta_t, curr_ir_val;
    Reward_Measure *rew_meas;
    //FILE   *fd;
    //char    rew_name[MAXSTRING];

    /* initialization */
    rew_meas = rew_sum->show_rew_measure();
    delta_t = end_time - init_time;
    curr_cr_val = rew_meas->show_cr_value();
    curr_ir_val = rew_sum->show_virtual_ir();

    /* get value from parameter like stamp used previously   */
    /* this kind of stamp will avoid numeric precision erros */
    /* or calculate cr value by the convencional way.        */
    if ( act_rew == TRUE )
    {
        // This stamp (cr_val) brings the exact value.
        new_val = cr_val;
    }
    else
    {
        // Here the cr is calculated.
        // So we use the gravity system to atract value to bounds.
        new_val = curr_cr_val + curr_ir_val * delta_t;

        // gravity system for rate_reward bound.
        if ( (rew_meas->is_upp_bounded() == TRUE) && (new_val > (rew_meas->show_upp_bound() - y_epsilon)) )
            new_val = rew_meas->show_upp_bound();
        if ( (rew_meas->is_low_bounded() == TRUE) && (new_val < (rew_meas->show_low_bound() + y_epsilon)) )
            new_val = rew_meas->show_low_bound();
    }
    /*----- debug info -----*/
    //rew_meas->show_name( rew_name );
    //fprintf ( stderr, "time: %f  -  run: %d  -  updating cr: %s  -  value: %f\n",simulation_time,run_number,rew_name,new_val);
    //fprintf( stderr, "curr_cr_val=%f  new_val=%f  ir=%f  delta_t=%f\n",curr_cr_val,new_val,curr_ir_val, delta_t);
    /*----- end of debug info -----*/

    /* calculate the time above a given reward level */
    /* it is calculated only if has_level == TRUE    */
    if ( rew_meas->show_has_level() == TRUE )
    {
        level = rew_meas->show_rew_level();
        time_abv_lv = rew_meas->show_time_abv_lv();
        if ( (curr_cr_val - level) * (new_val - level) < 0)
        {
            /* accumulated reward crossed the reward level */
            /* calculate the cross point */
            x_point = (level - curr_cr_val) / curr_ir_val + init_time;
            if (curr_cr_val < new_val)
                /* downcrossing the level */
                time_abv_lv += end_time - x_point;
            else
                /* upcrossing the level */
                time_abv_lv += x_point - end_time;
        }
        else
            /* the accum reward has not crossed the level */
            if ( new_val > level)
                /* but accum reward value is over the level */
                time_abv_lv += delta_t;

        /* update the reward */
        rew_meas->ch_time_abv_lv( time_abv_lv );
    }

    /* calculate the area under the reward */
    curr_avg_cr_val = rew_meas->show_avg_cr_value();
    new_avg_cr_val = curr_avg_cr_val + ((curr_cr_val + new_val) / 2) * delta_t;

    if ( (check == CHECK_CR_TOUCH) && (rr_ev_flag == TRUE) )
    {
        /* Verify if CR has reached a bound of some REWARD_REACHED event condition. */
        /* Only if there is at least one REWARD_REACHED event in the simulation     */
        /* (indicated by rr_ev_flag).                                               */
        /* If it happened, we should assure that the REWARD_EVENT must              */
        /* trigger in zero time after the treatment of this event.                  */
        check_cr_touch( rew_meas, simul_ev, curr_cr_val, new_val );
    }

    /* update the reward */
    rew_meas->ch_cr_value( new_val );
    rew_meas->ch_avg_cr_value( new_avg_cr_val );

    /* generate the cr trace file */
    rew_meas->conditional_printf_cr ((reward_to_print & PRINT_REWARD_CR), end_time, new_val);
    /* generate the ir trace file */
//BRUNO:        fprintf (fd, "%.10E %.10E\n", init_time, rew_meas->show_ir_value() );
    rew_meas->conditional_printf_ir ((reward_to_print & PRINT_REWARD_IR), end_time, rew_meas->show_ir_value() );
}
//------------------------------------------------------------------------------
void Simulator::check_cr_touch( Reward_Measure *rew_meas, Simulator_Event *simul_ev,
                                TGFLOAT curr_cr_val, TGFLOAT new_val )
{
    RR_Event          *rr_ev;
    RR_Event_List     *rr_ev_list;
    RR_Touch          *rr_touch;
    Rew_Reach_Distrib *rew_reach_dist;
    char               ev_name[MAXSTRING];
    char               ev_name2[MAXSTRING];

    Simulator_Event *sim_ev;
    Event_Sample_List *ev_lst;
    Event_Sample *ev_sa;

    /* Verify if CR has reached a bound of some RR event condition. */

    /* If the reward has reached it's expr value (apart from the event that  */
    /*   triggered right now), the flag trigger_now must be set to garantee  */
    /*   the ocurrence of the corresponding event.                           */
    if ( rew_meas->exists_RR_events_list() )
    {
        rr_ev_list = rew_meas->show_RR_events_list( LIST_RO );
        rr_ev = rr_ev_list->show_1st_rr_ev();
        while ( rr_ev != NULL )
        {
            if ( simul_ev->show_event() != rr_ev->show_event() )
            {
                /* verify the CR touch */
                if ( new_val != curr_cr_val )
                {
                    rr_touch = rr_ev->show_twin_rr_touch();
                    if ( rr_touch->show_direction() == UPP_REACH_TYPE )
                    {
                        if ( (curr_cr_val < new_val) &&
                            (new_val >= (rr_touch->show_expr_value() - y_epsilon)) &&
                            (new_val <= (rr_touch->show_expr_value() + y_epsilon)) )
                        {
                            /* the cr touched the bound */
                            /* set trigger_now flag to assure the ocurrence of the event */
                            rew_reach_dist = (Rew_Reach_Distrib *)rr_ev->show_event()->show_distrib();
                            rew_reach_dist->ch_trigger_now( TRUE );

                            /*--- debug info ---*/
                            simul_ev->show_event()->show_name( ev_name );
                            rr_ev->show_event()->show_name( ev_name2 );
                            /*fprintf( stderr, "Setou trigger_now( TRUE ) /\\ \n");
                            fprintf( stderr, "ev trigado: %s    -    ev cujo CR tocou: %s\n",ev_name,ev_name2 );
                            fprintf( stderr, "simul_time: %f\n", simulation_time);
                            *//*--- end of debug info ---*/
                        }
                    }
                    else
                    {
                        if ( (curr_cr_val > new_val) &&
                            (new_val <= (rr_touch->show_expr_value() + y_epsilon)) &&
                            (new_val >= (rr_touch->show_expr_value() - y_epsilon)) )
                        {
                            /* the cr touched the bound */
                            /* set trigger_now flag to assure the ocurrence of the event */
                            rew_reach_dist = (Rew_Reach_Distrib *)rr_ev->show_event()->show_distrib();
                            rew_reach_dist->ch_trigger_now( TRUE );

                            /*--- debug info ---*/
                            simul_ev->show_event()->show_name( ev_name );
                            rr_ev->show_event()->show_name( ev_name2 );
                            /*fprintf( stderr, "Setou trigger_now( TRUE ) \\/ \n");
                            fprintf( stderr, "ev trigado: %s    -    ev cujo CR tocou: %s\n",ev_name,ev_name2 );
                            fprintf( stderr, "simul_time: %f    -    curr=%f - new=%f\n", simulation_time, curr_cr_val, new_val);
                            *//*--- end of debug info ---*/

                            /*--- sample check ---*/
                            // This check isn't necessary for the working.
                            /* However if an internal error occur, this will happen */
                            /* to explain what could be wrong!                      */

                            sim_ev = event_list->show_1st_event();
                            while ( sim_ev != NULL )
                            {
                                if( sim_ev->show_event() == rr_ev->show_event() )
                                {
                                    ev_lst = sim_ev->show_ev_sample_list(LIST_RO);
                                    if ( ev_lst != NULL )
                                    {
                                        ev_sa = ev_lst->show_1st_sample();
                                        if ( ev_sa == NULL )
                                        {
                                            fprintf(stderr,"INTERNAL ERROR: REWARD REACHED event %s has no sample.\n", ev_name2 );
                                        }
                                    }
                                    else
                                    {
                                            fprintf(stderr,"INTERNAL ERROR: REWARD REACHED event %s has no sample list.\n", ev_name2 );
                                    }
                                    delete ev_lst;
                                }

                                sim_ev = event_list->show_next_event();
                            }
                            /*--- end of sample check ---*/

                        }
                    }
                }
            }
            /* go to the next rr_event */
            rr_ev = rr_ev_list->show_next_rr_ev();
        }
        delete rr_ev_list;
    }
}
//------------------------------------------------------------------------------
void Simulator::create_imp_rew_entries(Action_List *act_list)
{
    Imp_Reward_Desc_List *imp_desc_list;
    Imp_Reward_Desc      *imp_desc;
    Action               *act;
    char                  rew_name[MAXSTRING];
    int                   rew_id;
    Reward_Measure       *rew_meas;
    TGFLOAT               bound, init_cr_val;

    act = act_list->show_1st_act();

    /* go through all actions */
    while (act != NULL)
    {
        imp_desc_list = act->show_impulse_list(LIST_RO);
        imp_desc = imp_desc_list->show_1st_rew();
        while (imp_desc != NULL)
        {
            imp_desc->show_name(rew_name);
            rew_id = imp_desc->show_id();

            if (!(imp_rew_list->query_rew(rew_id)))
            {
                /* create an empty entry for this reward */
                rew_meas = new Reward_Measure(rew_name, rew_id);

                /* if the reward is lower bounded then set the bounds */
                if (imp_desc->is_low_bounded())
                {
                    rew_meas->ch_low_bounded(TRUE);
                    bound = imp_desc->show_low_bound();
                    rew_meas->ch_low_bound(bound);
                }

                /* if the reward is upper bounded then set the bounds */
                if (imp_desc->is_upp_bounded())
                {
                    rew_meas->ch_upp_bounded(TRUE);
                    bound = imp_desc->show_upp_bound();
                    rew_meas->ch_upp_bound(bound);
                }

                /* set the cr initial value */
                init_cr_val = imp_desc->show_cr_initial_value();
                if ( (rew_meas->is_upp_bounded()) && (init_cr_val > rew_meas->show_upp_bound()) )
                    init_cr_val = rew_meas->show_upp_bound();
                if ( (rew_meas->is_low_bounded()) && (init_cr_val < rew_meas->show_low_bound()) )
                    init_cr_val = rew_meas->show_low_bound();
                rew_meas->ch_cr_value( init_cr_val );
                rew_meas->set_first_cr( init_cr_val );

                /* open the trace files */
                if (Rew_Files == TRACE)
                    rew_meas->open_trace_files(NULL);

                /* updates the pointer to the structure */
                imp_desc->ch_rew_meas( rew_meas );
                //fprintf(stderr,"saved rew_meas - %s\n",rew_name);

                /* add this reward to the list */
                imp_rew_list->add_tail_rew(rew_meas);
            }
            imp_desc = imp_desc_list->show_next_rew();
        }
        delete imp_desc_list;
        act = act_list->show_next_act();
    }
}
//------------------------------------------------------------------------------
void Simulator::create_imp_rew_list()
{
    Object_Description_List *obj_desc_list;
    Object_Description      *obj_desc;
    Event_List              *ev_list;
    Event                   *ev;
    Message_List            *msg_list;
    Message                 *msg;
    Action_List             *act_list;

    /* get the object description list */
    obj_desc_list = the_system_desc->show_obj_desc_list(LIST_RO);
    obj_desc = obj_desc_list->show_1st_obj_desc();
    /* go through all objects */
    while (obj_desc != NULL)
    {
        /* Get all actions in the Events part */
        ev_list = obj_desc->show_event_list(LIST_RO);
        ev = ev_list->show_1st_event();

        /* go through all events */
        while (ev != NULL)
        {
            act_list = ev->show_action_list(LIST_RO);
            /* create the reward entries for this action list */
            create_imp_rew_entries(act_list);

            delete act_list;
            ev = ev_list->show_next_event();
        }
        delete ev_list;

        /* Get all actions in the Messages part */
        msg_list = obj_desc->show_message_list(LIST_RO);
        msg = msg_list->show_1st_msg();

        /* go through all messages */
        while (msg != NULL)
        {
            act_list = msg->show_action_list(LIST_RO);
            /* create the reward entries for this action list */
            create_imp_rew_entries(act_list);

            delete act_list;
            msg = msg_list->show_next_msg();
        }
        delete msg_list;

        obj_desc = obj_desc_list->show_next_obj_desc();
    }
    delete obj_desc_list;
}
//------------------------------------------------------------------------------
void Simulator::add_imp_rew_list(Transition *trans)
{
    Reward_Measure  *rew_meas;
    Imp_Reward_List *imp_rew_list_aux;
    Imp_Reward      *imp_rew_aux;
    Imp_Rew_Val     *imp_val_aux;
    Expr_Val        *imp_val_value_aux;
    int              imp_id;
    TGFLOAT          curr_cr_val, curr_avg_cr_val, new_val, new_avg_cr_val;
    TGFLOAT          last_rew_time;
    //FILE            *fd;

    /* get the impulse reward list on this transition */
    imp_rew_list_aux = trans->show_imp_rew_list(LIST_RO);
    imp_rew_aux = imp_rew_list_aux->show_1st_rew();
    /* go through all impulse rewards in the transition */
    while (imp_rew_aux != NULL)
    {

        /* get the impulse reward id */
        imp_id = imp_rew_aux->show_id();
        if (imp_rew_list->query_rew(imp_id))
        {
            /* find the correct impulse reward in the list */
            rew_meas = imp_rew_list->show_curr_rew();

            /* get the value of the reward in the transition */
            imp_val_aux = imp_rew_aux->show_1st_imp_val();
            imp_val_value_aux = imp_val_aux->show_value();

            /* get the current value of the function CI(t), the last time it was updated */
            last_rew_time = rew_meas->show_time();
            curr_cr_val = rew_meas->show_cr_value();

            /* calculate the new value for CI(t) */
            new_val = curr_cr_val + imp_val_value_aux->show_value();

            /* correct the reward if it is out of bounds */
           if (rew_meas->is_upp_bounded() && (new_val > rew_meas->show_upp_bound()))
                new_val = rew_meas->show_upp_bound();
           else
                if (rew_meas->is_low_bounded() && (new_val < rew_meas->show_low_bound()))
                    new_val = rew_meas->show_low_bound();

           /* get the current value of the average and update */
           curr_avg_cr_val = rew_meas->show_avg_cr_value();
           new_avg_cr_val = curr_avg_cr_val + curr_cr_val * (simulation_time - last_rew_time);

           /* update the reward */
           rew_meas->ch_cr_value(new_val);
           rew_meas->ch_avg_cr_value(new_avg_cr_val);
           rew_meas->ch_time(simulation_time);
           //rew_meas->reset_first_cr();

           /* generate the trace file */
           rew_meas->conditional_printf_cr ((reward_to_print & PRINT_REWARD_CR), simulation_time, new_val);
           /*BRUNO!!!!*/rew_meas->conditional_printf_ir ((reward_to_print & PRINT_REWARD_IR), last_simul_time, imp_val_value_aux->show_value());
        }
        else
            debug (3, "Simulator::add_imp_rew_list: Adding a unknown impulse reward");

        /* go to the next reward in the transition */
        imp_rew_aux = imp_rew_list_aux->show_next_rew();
    }
    delete imp_rew_list_aux;
}
//------------------------------------------------------------------------------
void Simulator::add_clone(const char *event_name)
{
    clone_ev_list->add_order_clone( (char *) event_name );
}
//------------------------------------------------------------------------------
void Simulator::clone_events(Object_State *obj_st)
{
    Simulator_Event *simul_ev;
    Event           *ev;
    Clone_Event     *clone_ev;
    char             obj_name[MAXSTRING];
    char             ev_name[MAXSTRING];
    char             clone_ev_name[MAXSTRING];
    int              id, found, i;

    /* get the object id */
    id = obj_st->show_id();

    /* get the first clonned event */
    clone_ev = clone_ev_list->get_1st_clone();
    while (clone_ev != NULL)
    {
        /* get the name of this cloned event */
        clone_ev->show_name(clone_ev_name);
        /* get the first event of this object */
        simul_ev = obj_ev_tb[id].simul_ev;

        /* find the correct event */
        found = FALSE;
        while (!found)
        {
            ev = simul_ev->show_event();
            ev->show_name(ev_name);
            /* compare the events name */
            if (!strcmp(ev_name, clone_ev_name))
                found = TRUE;
            else
                /* go to the next event */
                simul_ev = event_list->show_next_event(simul_ev);
        }

        /* check to see if the event is enabled in the current state*/
        if (simul_ev->eval_cond(obj_st) == TRUE)
        {
            /* If the event is REWARD_REACHED type print error. */
            if( simul_ev->show_event()->show_distrib()->show_type() == REW_REACH_DIST )
            {
                fprintf( stderr, "ERROR: You can't clone Reward_Reached event type.\nThis operation doesn't make sense!\n" );
                exit( -1 );
            }

            /* check to see if there is an existing sample */
            /* (the clone cannot be the first sample)      */
            if (simul_ev->show_no_samples() == 0)
            {
                simul_ev->show_obj_desc()->show_name(obj_name);
                debug (0, "ERROR: You are clonning the following event: %s.%s.\nHowever this event will be disable after the action is executed.\nThis is possibly a specification mistake!",obj_name,ev_name);
            }
            /* generate the number of clones specified */
            for (i=0; i<clone_ev->show_no_clones(); i++)
                /* generate a new sample for this event */
                simul_ev->generate_sample(obj_st, simulation_time);
        }
        /* delete the processed clone event */
        delete clone_ev;

        /* go to the next cloned event */
        clone_ev = clone_ev_list->get_1st_clone();
    }
}
//------------------------------------------------------------------------------
/* used by set_cr and set_rew user commands */
void Simulator::set_cumulative_reward(Object_State *obj_st, const char *rew_name, TGFLOAT rew_val)
{
    Object_Description *obj_desc;
    Reward_Measure     *rew_meas;
    char                obj_name[MAXSTRING];
    char                obj_rew_name[MAXSTRING];
    //FILE               *fd;

    obj_desc = obj_st->show_obj_desc();
    obj_desc->show_name(obj_name);
    sprintf (obj_rew_name, "%s.%s",obj_name, rew_name);

    if (rate_rew_list->query_rew(obj_rew_name))
    {
        rew_meas = rate_rew_list->show_curr_rew();
        rew_meas->ch_cr_value(rew_val);
    }
    else
    {
        if (imp_rew_list->query_rew(obj_rew_name))
        {
            rew_meas = imp_rew_list->show_curr_rew();
            rew_meas->ch_cr_value(rew_val);
        }
        else
        {
            fprintf( stderr, "Severe ERROR: No reward named %s defined in model\n", obj_rew_name);
            exit(-1);
        }
    }
    /* generate the trace file */
    rew_meas->conditional_printf_cr ((reward_to_print & PRINT_REWARD_CR), simulation_time, rew_val);

    /* Set need_eval flag for reescheduling of the RR   */
    /*   events that has this reward in it's condition. */
    set_need_eval_for_rr_ev( rew_meas );
}
//------------------------------------------------------------------------------
/* used by set_ir user command                                    */
/* restrictions: only can be used in rate rewards in this version */
void Simulator::set_instantaneous_reward(Object_State *obj_st, const char *rew_name, TGFLOAT rew_val)
{
    Object_Description    *obj_desc;
    Rate_Reward_Desc      *rate_rew_desc;
    Rate_Reward_Desc_List *rew_desc_list;
    Reward_Measure        *rew_meas;
    char                   obj_name[MAXSTRING];
    char                   obj_rew_name[MAXSTRING];

    /* note: The information concerning ir value is storage redundantly in both */
    /*       places, Rate_Reward_Desc_List and Reward_Measure_List.             */
    /*       Reward_Measure_List holds this info because is useful centralize   */
    /*       all information about reward in the same data structure.           */

    obj_desc = obj_st->show_obj_desc();
    obj_desc->show_name(obj_name);
    sprintf (obj_rew_name, "%s.%s",obj_name, rew_name);

    /* updating Rate_Reward_Desc_List */

    /* get the reward list of the object above */
    rew_desc_list = obj_desc->show_reward_list(LIST_RW);
    /* query for the reward that will be changed */
    if( rew_desc_list->query_rew( (char *)rew_name ) )
    {
        rate_rew_desc = rew_desc_list->show_curr_rew();
        rate_rew_desc->ch_ir_user_set_flag(TRUE);
        rate_rew_desc->ch_ir_user_set_value(rew_val);
    }
    else
    {
        fprintf( stderr, "Severe ERROR in set_ir: No rate reward named %s defined in model\n", obj_rew_name);
        exit(-1);
    }

    /* updating Reward_Measure_List */

    /* search the required reward */
    if (rate_rew_list->query_rew(obj_rew_name))
    {
        /* get the pointer to the required reward */
        rew_meas = rate_rew_list->show_curr_rew();
        rew_meas->ch_ir_value( rew_val );
    }
    else
    {
        fprintf( stderr, "Severe ERROR in set_ir: No rate reward named %s defined in model\n", obj_rew_name);
        exit(-1);
    }

    /* Set need_eval flag for reescheduling of the RR   */
    /*   events that has this reward in it's condition. */
    set_need_eval_for_rr_ev( rew_meas );
}
//------------------------------------------------------------------------------
/* used by unset_ir user command                                  */
/* restrictions: only can be used in rate rewards in this version */
void Simulator::unset_instantaneous_reward(Object_State *obj_st, const char *rew_name)
{
    Object_Description    *obj_desc;
    Rate_Reward_Desc      *rate_rew_desc;
    Rate_Reward_Desc_List *rew_desc_list;
    Reward_Measure        *rew_meas;
    Expr_Val              *rate_ir;
    char                   obj_name[MAXSTRING];
    char                   obj_rew_name[MAXSTRING];

    /* note: The information concerning ir value is storage redundantly in both */
    /*       places, Rate_Reward_Desc_List and Reward_Measure_List.             */
    /*       Reward_Measure_List holds this info because is useful centralize   */
    /*       all information about reward in the same data structure.           */

    obj_desc = obj_st->show_obj_desc();
    obj_desc->show_name(obj_name);
    sprintf (obj_rew_name, "%s.%s",obj_name, rew_name);

    /* updating Rate_Reward_Desc_List */

    /* get the reward list of the object above */
    rew_desc_list = obj_desc->show_reward_list(LIST_RW);

    if( rew_desc_list->query_rew( (char *)rew_name ) )
    {
        rate_rew_desc = rew_desc_list->show_curr_rew();
        rate_rew_desc->ch_ir_user_set_flag(FALSE);
        rate_rew_desc->ch_ir_user_set_value(0);
    }
    else
    {
        fprintf( stderr, "Severe ERROR in unset_ir: No rate reward named %s defined in model\n", obj_rew_name);
        exit(-1);
    }

    /* updating Reward_Measure_List */

    /* search the required reward */
    if (rate_rew_list->query_rew(obj_rew_name))
    {
        /* get the pointer to the required reward */
        rew_meas = rate_rew_list->show_curr_rew();
        /* evaluate the ir value according current state */
        rate_ir = rate_rew_desc->eval_rate( obj_st );
        /* update ir value */
        rew_meas->ch_ir_value( rate_ir->show_value() );
    }
    else
    {
        fprintf( stderr, "Severe ERROR in unset_ir: No rate reward named %s defined in model\n", obj_rew_name);
        exit(-1);
    }

    /* Set need_eval flag for reescheduling of the RR   */
    /*   events that has this reward in it's condition. */
    set_need_eval_for_rr_ev( rew_meas );
}
//------------------------------------------------------------------------------
void Simulator::set_need_eval_for_rr_ev( Reward_Measure *rew_meas )
{
    RR_Event          *rr_ev;
    RR_Event_List     *rr_ev_list;
    Rew_Reach_Distrib *rew_reach_dist;
    //char               ev_name[MAXSTRING];


    /* Set the flags need_evaluation for every REWARD REACHED event */
    /*   type that has this reward measure in it's condition.       */

    if ( rew_meas->exists_RR_events_list() )
    {
        rr_ev_list = rew_meas->show_RR_events_list( LIST_RO );
        rr_ev = rr_ev_list->show_1st_rr_ev();
        while ( rr_ev != NULL )
        {
            /* get the distrib to set the flag */
            rew_reach_dist = (Rew_Reach_Distrib *)rr_ev->show_event()->show_distrib();
            rew_reach_dist->ch_need_evaluation( TRUE );

            /*--- debug info ---*/
            //fprintf( stderr, "Setou need_evaluation( TRUE ) - ");
            //rr_ev->show_event()->show_name( ev_name );
            //fprintf( stderr, "evento: %s\n",ev_name );
            //fprintf( stderr, "simul_time: %f\n", simulation_time);
            /*--- end of debug info ---*/

            /* go to the next rr_event */
            rr_ev = rr_ev_list->show_next_rr_ev();
        }
        delete rr_ev_list;
    }

    /* Case the rew meas be part of a sum, set the need eval   */
    /*   flag to the event that has the sum in it's condition. */
    if ( rew_meas->show_type() != RATE_REWARD_SUM_TYPE )
    {
        if ( rew_meas->is_rate_sum_bounded() == TRUE )
        {
            set_need_eval_for_rr_ev( rew_meas->show_rate_sum_bound()->show_rew_measure() );
        }
    }
}
//------------------------------------------------------------------------------
/* used by get_cr and get_rew user commands */
TGFLOAT Simulator::get_cumulative_reward(Object_State *obj_st, const char *rew_name)
{
    Object_Description *obj_desc;
    Reward_Measure     *rew_meas;
    TGFLOAT              rew_val = 0;
    char                obj_name[MAXSTRING];
    char                obj_rew_name[MAXSTRING];

    obj_desc = obj_st->show_obj_desc();
    obj_desc->show_name(obj_name);
    sprintf (obj_rew_name, "%s.%s",obj_name, rew_name);

    if (rate_rew_list->query_rew(obj_rew_name))
    {
        rew_meas = rate_rew_list->show_curr_rew();
        rew_val = rew_meas->show_cr_value();
    }
    else
    {
        if (imp_rew_list->query_rew(obj_rew_name))
        {
            rew_meas = imp_rew_list->show_curr_rew();
            rew_val = rew_meas->show_cr_value();
        }
        else
        {
            fprintf( stderr, "Severe ERROR in get_cr: No reward named %s defined in model\n", obj_rew_name);
            exit(-1);
        }
    }
    return(rew_val);
}

//------------------------------------------------------------------------------
/* used by get_cr_sum command */
TGFLOAT Simulator::get_cumulative_reward_sum(Object_State *obj_st, const char *rew_name)
{
    Object_Description *obj_desc;
    Reward_Measure     *rew_meas;
    TGFLOAT              cr_sum = 0;
    char                obj_name[MAXSTRING];

    Rate_Reward_Sum       *ra_rew_sum = NULL;
    Rate_Reward_Sum_List  *ra_rew_sum_list = NULL;
    Reward_Sum_List       *rew_sum_list = NULL;
    Reward_Sum            *rew_sum = NULL;

    obj_desc = obj_st->show_obj_desc();
    obj_desc->show_name(obj_name);
    //sprintf (obj_rew_name, "%s.%s",obj_name, rew_name);

    ra_rew_sum_list = show_rate_rew_sum_list(LIST_RO);
    if( ra_rew_sum_list->query_rew( obj_name, (char *)rew_name ) )
    {
        ra_rew_sum = ra_rew_sum_list->show_curr_rew();

        rew_sum_list = ra_rew_sum->show_rew_sum_list(LIST_RO);
        rew_sum = rew_sum_list->show_1st_rew();

        while(rew_sum != NULL)
        {
            rew_meas = rew_sum->show_rew_measure();
            cr_sum += rew_meas->show_cr_value();

            // go to the next rew_sum.
            rew_sum = rew_sum_list->show_next_rew();
        }

        // gravity system for rate_reward_sum bound.
        if ( (ra_rew_sum->is_upp_bounded() == TRUE) && (cr_sum > (ra_rew_sum->show_upp_bound()-y_epsilon)) )
            cr_sum = ra_rew_sum->show_upp_bound();
        if ( (ra_rew_sum->is_low_bounded() == TRUE) && (cr_sum < (ra_rew_sum->show_low_bound()+y_epsilon)) )
            cr_sum = ra_rew_sum->show_low_bound();

        delete rew_sum_list;
    }
    else
    {
        fprintf( stderr, "Severe ERROR in get_cr_sum: No rate_reward_sum named %s defined in object %s\n",rew_name ,obj_name);
        exit(-1);
    }
    delete ra_rew_sum_list;

    return( cr_sum );
}
//------------------------------------------------------------------------------
/* used by get_ir user command                                    */
/* restrictions: only can be used in rate rewards in this version */
TGFLOAT Simulator::get_instantaneous_reward(Object_State *obj_st, const char *rew_name)
{
    Object_Description    *obj_desc;
    Reward_Measure        *rew_meas;
    Reward_Measure_List   *rew_meas_list;
    TGFLOAT                 rew_val = 0;
    char                   obj_name[MAXSTRING];
    char                   obj_rew_name[MAXSTRING];

    obj_desc = obj_st->show_obj_desc();
    obj_desc->show_name(obj_name);
    sprintf( obj_rew_name, "%s.%s", obj_name, rew_name);

    /* get the reward measure list of the simulation */
    rew_meas_list = show_rate_rew_list(LIST_RO);
     /* search the required reward */
    if (rew_meas_list->query_rew(obj_rew_name))
    {
        /* get the pointer to the required reward */
        rew_meas = rew_meas_list->show_curr_rew();

        rew_val = rew_meas->show_ir_value();
    }
    else
    {
        fprintf( stderr,"Severe ERROR in get_ir: No rate reward named %s defined in model\n", obj_rew_name);
        exit(-1);
    }
    delete rew_meas_list;
    return(rew_val);
}
//------------------------------------------------------------------------------
/* S P E C I A L _ R R _ E V E N T                                            */
/* Used to evaluate /\ \/ operators.                                          */
/* This function works with rate_reward and rate_reward_sum.                  */
/* Arguments:                                                                 */
/* 1- rew_type: GET_CR_TYPE or GET_IR_TYPE or GET_CR_SUM_TYPE                 */
/* 2- pointer to Object_State                                                 */
/* 3- pointer to reward (or reward_sum) name                                  */
/* 4- operator: UPP_REACH_TYPE or LOW_REACH_TYPE                              */
/* 5- expression that represents the value where the                          */
/*    event must be scheduled if the reward reach it.                         */
/* 6- name of the event.                                                      */
/* In the begining of the simulation, this method actuates creating some      */
/* structures for future use of the will_reach_reward that calculates         */
/* delta_time. This always return 1 (TRUE) to isolate other arguments of the  */
/* condition.                                                                 */
//------------------------------------------------------------------------------
int Simulator::special_rr_event( Object_State *obj_st, int rew_type, const char *p_rew_name,
                                  int op, TGFLOAT expr, const char *ev_name )
{
    Event                 *event = NULL;
    Event_List            *ev_list;
    Rate_Reward_Desc_List *rate_rew_desc_list;
    Rate_Reward_Desc      *rate_rew_desc = NULL;
    Rew_Reach_Distrib     *rew_reach_dist = NULL;
    Distribution          *distrib = NULL;
    Object_Description    *obj_desc;

    char                   rew_name[MAXNAME];
    char                   obj_name[MAXNAME];
    char                   obj_rew_name[MAXNAME];

    RR_Touch              *rr_touch = NULL;
    RR_Touch_List         *rr_touch_list = NULL;
    RR_Event              *rr_ev = NULL;

    Rate_Reward_Sum       *ra_rew_sum = NULL;
    Rate_Reward_Sum_List  *ra_rew_sum_list = NULL;
    Reward_Measure        *rew_meas = NULL;

    /*----- if the simulation is in it's begining ... -----*/
    if ( (simulation_time == 0)  && (run_number == 0) )
    {
        /*----- starts getting all the pointers that will be needed -----*/
        // set the flag that indicates the existence of at least one
        // REWARD REACHED event.
        ch_rr_ev_flag( TRUE );

        // get the reward name
        strcpy( rew_name, p_rew_name);

        /* get the pointers to Rew_Reach_Distrib and RR_Touch_List */
        ev_list = obj_st->show_obj_desc()->show_event_list(LIST_RO);
        if( ev_list->query_event( (char *)ev_name ) )
        {
            event = ev_list->show_curr_event();
            distrib = event->show_distrib();
            rew_reach_dist = (Rew_Reach_Distrib *)distrib;
            rr_touch_list = rew_reach_dist->show_rr_touch_list( LIST_RW );
        }
        else
        {
            fprintf ( stderr, "ERROR: invalid argument #5 for special_rr_event internal function! ");
        }

        /* get a pointer to Rate_Reward_Desc or Rate_Reward_Sum */
        switch( rew_type )
        {
            case GET_CR_TYPE:
            case GET_IR_TYPE:
                rate_rew_desc_list = obj_st->show_obj_desc()->show_reward_list(LIST_RO);
                if( rate_rew_desc_list->query_rew(rew_name) )
                {
                    rate_rew_desc = rate_rew_desc_list->show_curr_rew();

                    /* get the complete name of the reward */
                    obj_desc = obj_st->show_obj_desc();
                    obj_desc->show_name(obj_name);
                    sprintf (obj_rew_name, "%s.%s",obj_name, rew_name);
                }
                else
                    fprintf ( stderr, "ERROR: invalid reward name for special_rr_event internal function! ");
                break;
            case GET_CR_SUM_TYPE:
                ra_rew_sum_list = show_rate_rew_sum_list(LIST_RO);
                if( ra_rew_sum_list->query_rew(rew_name) )
                {
                    ra_rew_sum = ra_rew_sum_list->show_curr_rew();

                    /* get the complete name of the reward */
                    ra_rew_sum->show_obj_name(obj_name);
                    sprintf(obj_rew_name,"%s.%s",obj_name,rew_name);
                }
                else
                    fprintf (stderr,"ERROR: invalid reward name for special_rr_event internal function! ");
                break;
            default:
                fprintf (stderr,"ERROR: invalid reward name for special_rr_event internal function! ");
                break;
        }

        /* get the pointer to the reward_measure */
        if (rate_rew_list->query_rew(obj_rew_name))
        {
            rew_meas = rate_rew_list->show_curr_rew();
        }
        else
        {
            if (imp_rew_list->query_rew(obj_rew_name))
            {
                rew_meas = imp_rew_list->show_curr_rew();
            }
            else
            {
                fprintf( stderr, "Severe ERROR in special_rr_event: No reward named %s defined in model\n", obj_rew_name);
                exit(-1);
            }
        }

        /* ----- now it's time to create the structures! -----*/

        // Adding in Rew_Reach_Dist!
        rr_touch = new RR_Touch( rew_type, expr, op, rew_meas );
        if ( (rew_type == GET_CR_TYPE) || (rew_type == GET_IR_TYPE) )
        {
            rr_touch->ch_rate_rew_desc( rate_rew_desc );
        }
        else
        {
            rr_touch->ch_rate_rew_sum( ra_rew_sum );
        }
        rr_touch_list->add_tail_rr_touch( rr_touch );

        // Adding in Reward_Measure!
        rr_ev = new RR_Event( event, rr_touch );
        if ( !rew_meas->exists_RR_events_list() )
        {
            rew_meas->create_RR_events_list();
        }
        rew_meas->add_RR_event( rr_ev );

        //----- develop debug info ------------------
        //fprintf(stderr,"----------\nspecial rr event (begining) simul_time=%.10f event:%s\n",simulation_time,ev_name);
        //fprintf(stderr,"         obj_name=%s - rew_name=%s - op=%d - expr=%f - rew_type=%d\n",obj_name,rew_name,op,expr,rew_type );
        //fprintf(stderr,"         no_rr_touchs=%d\n",rr_touch_list->show_no_rr_touch() );
        //----- end of develop debug info -----------

    }

    /* ----- always return TRUE  -----*/
    // This is necessary to isolate other arguments than special ones
    // of the event condition. ( the special ones are always true )
    return( TRUE );
}
//------------------------------------------------------------------------------
/* E V A L _ S P E C I A L _ A R G S                                          */
/* After the analisys of the ordinary args of the rr_event condition, this    */
/*   method is called to calculate the delta_time of the event.               */
/* Before computing of delta time, all treated flags are reseted. This speeds */
/*  up simulation because avoid to reenter in rewards previously computed by  */
/*  it's sum.                                                                 */
/* The delta_time is the smaller from the time to reach of each special arg.  */

/* NOTE: special args are:                                                    */
/*          get_cr(xxx) \/ number  or  get_cr(xxx) /\ number                  */
/*          get_cr_sum(xxx) \/ number or get_cr_sum(xxx) /\ number            */

/* ordinary args are the others than special ones.                            */
//------------------------------------------------------------------------------
void Simulator::eval_rr_special_args( Rew_Reach_Distrib *rew_reach_dist )
{
    RR_Touch      *rr_touch;
    RR_Touch_List *rr_touch_list;
    int           cross;
    Rate_Reward_Desc *rate_rew_desc;

    /* reset treated flag from every Rate_Reward_Sum of the current obj */
    rr_touch_list = rew_reach_dist->show_rr_touch_list( LIST_RO );
    rr_touch = rr_touch_list->show_1st_rr_touch();
    /* goes through all rr_touch to reset treated flag */
    while( rr_touch != NULL )
    {
        /* get a pointer to Rate_Reward_Sum */
        switch( rr_touch->show_rew_type() )
        {
            case GET_CR_TYPE:
            case GET_IR_TYPE:
                rate_rew_desc = rr_touch->show_rate_rew_desc();
                if ( rate_rew_desc->is_sum_bounded() == TRUE )
                {
                    /* reset treated flag */
                    rate_rew_desc->show_sum_bound()->ch_treated( FALSE );
                }
                break;
            case GET_CR_SUM_TYPE:
                /* reset treated flag */
                rr_touch->show_rate_rew_sum()->ch_treated( FALSE );
                break;
            default:
                fprintf (stderr,"INTERNAL ERROR: invalid reward type in rr_touch! ");
                break;
        }
        /* next rr_touch */
        rr_touch = rr_touch_list->show_next_rr_touch();
    }

    /* compute all delta times! */
    rr_touch = rr_touch_list->show_1st_rr_touch();
    /* goes through all rr_touch to analise the delta_time */
    while( rr_touch != NULL )
    {
        cross = will_reach_reward( rew_reach_dist, rr_touch );
        /* cross indicates if the argument will cross - this is not in use */
        rr_touch = rr_touch_list->show_next_rr_touch();
    }
    delete rr_touch_list;
}
//------------------------------------------------------------------------------
/* W I L L _ R E A C H _ R E W A R D                                          */
/* Used to evaluate /\ \/ operators.                                          */
/* Calculate delta_time of the distribution                                   */
/* This function works with rate_reward and rate_reward_sum.                  */
/* Argument: event to be analised                                             */
//------------------------------------------------------------------------------
int Simulator::will_reach_reward( Rew_Reach_Distrib *rew_reach_dist,
                                  RR_Touch *rr_touch )
{
    char               rew_name[MAXNAME];
    char               aux[MAXNAME];

    int                op,rew_type;
    TGFLOAT            expr;
    TGFLOAT            ir = -1;
    TGFLOAT            cr = -1;
    TGFLOAT            time_to_reach = -1;
    TGFLOAT            curr_delta_time;
    int                cross_flag, between_bounds;
    Stamp             *stamp, *stamp_aux = NULL;
    Stamp_List        *stamp_list = NULL;

    Rate_Reward_Desc  *rate_rew_desc = NULL;
    Rate_Reward_Sum   *ra_rew_sum = NULL;
    Reward_Sum_List   *rew_sum_list = NULL;
    Reward_Sum        *rew_sum = NULL;
    Reward_Sum        *rew_of_min_cross_time = NULL;
    Reward_Measure    *rew_meas = NULL;
    TGFLOAT            min_cross_time = INFINITE;
    TGFLOAT            sum_cross_time = INFINITE;
    TGFLOAT            cross_time;
    TGFLOAT            ir_sum = 0, cr_sum = 0, ir_neg = 0, ir_pos = 0;
    TGFLOAT            virtual_ir = 0;
    int                skip_flag = FALSE;
    TGFLOAT            cross_time_by_sum = INFINITE;
    TGFLOAT            cr_by_sum = 0, ir_by_sum = 0;
    int                is_ra_rew_sum = FALSE;
    //----- debug info --------------------------
    int    devel_debug = FALSE; // enables the printing of devel debug info
    TGFLOAT dbg_time_low=-1, dbg_time_upp=1; // the range of time to print
    int    rew_id = 0;
    //char name[255],name2[255];

    /* At this point the ordinary arguments of the condition were  */
    /*   evaluated TRUE, so now the delta time must be calculated. */
    /* The cross_flag indicates that the delta time is less than   */
    /*   infinite.                                                 */
    /* The flag need_evaluation of the Rew_Reach_Distrib is set    */
    /*   to FALSE every time this method is executed.              */

    /* flag that indicates if this reward will cross under the condition */
    cross_flag = FALSE;

    /* get a pointer to Stamp_List */
    stamp_list = rew_reach_dist->show_stamp_list( LIST_RW );

    /* get the current delta_time from Rew_Reach_Distrib */
    curr_delta_time = rew_reach_dist->show_delta_time();

    /* get a pointer to Rate_Reward_Desc or Rate_Reward_Sum */
    switch( rr_touch->show_rew_type() )
    {
        case GET_CR_TYPE:
        case GET_IR_TYPE:
            rate_rew_desc = rr_touch->show_rate_rew_desc();
            // get the reward id and name
            rew_id = rate_rew_desc->show_id();
            rate_rew_desc->show_name( rew_name );
            break;
        case GET_CR_SUM_TYPE:
            ra_rew_sum = rr_touch->show_rate_rew_sum();
            is_ra_rew_sum = TRUE;
            // get the reward id and name
            rew_id = ra_rew_sum->show_id();
            ra_rew_sum->show_name( rew_name );
            break;
        default:
            fprintf (stderr,"INTERNAL ERROR: invalid reward type in rr_touch! ");
            break;
    }

    /* get op, expr and rew_type */
    op = rr_touch->show_direction();
    expr = rr_touch->show_expr_value();
    rew_type = rr_touch->show_rew_type();

    //----- develop debug info ------------------
//fprintf(stderr,"\nw: t=%.10f\n",simulation_time);
//    if ( (devel_debug == TRUE) && (simulation_time > dbg_time_low) && (simulation_time < dbg_time_upp) )
//    {
//        fprintf(stderr,"----------\nin will: simul_time=%.10f   run:%d\n",simulation_time,run_number);
//        fprintf(stderr,"         rew_name=%s - rew_id=%d - op=%d - expr=%f\n",rew_name,rew_id,op,expr );
//    }
    //----- end of develop debug info -----------


    /* ----- now it's time to calculate the delta_time! -----*/

    if( (rew_type == GET_CR_TYPE) || (rew_type == GET_CR_SUM_TYPE) )
    {
        if ( (is_ra_rew_sum) || (rate_rew_desc->is_sum_bounded()) )
        {
            if (!is_ra_rew_sum)
                ra_rew_sum = rate_rew_desc->show_sum_bound();
            /* IMPORTANT: about treated flag. */
            /* treated flag are reset and set in right places. But something is missing
            during the updates inside of one reward. When will_reach_reward enters
            in a non treated rate_reward_sum it should updates all rewards, updates the sum
            and set treated_flag to FALSE. So the next time the function doesn't need to do
            anything. But something is missing and that's why the if bellow is always TRUE,
            to enter and compute all rewards all the time.  NOTE by Kelvin! 20/03/2002 */
            if ( 1/*!ra_rew_sum->has_treated()*/ )
            {
//ncomp//ra_rew_sum->show_name(name);
//ncomp//ra_rew_sum->show_obj_name(name2);
//ncomp//fprintf(stderr,"to tratando a soma %s do %s\n",name,name2);

                /* go throught all Reward_Sum objects */
                ir_sum = 0;
                cr_sum = 0;
                ir_neg = 0;
                ir_pos = 0;
                rew_sum_list = ra_rew_sum->show_rew_sum_list(LIST_RW);
                rew_sum = rew_sum_list->show_1st_rew();

                while( rew_sum != NULL )
                {
                    rew_meas = rew_sum->show_rew_measure();
                    cr_sum += rew_meas->show_cr_value();

                    /* initialize virtual_ir and local_bound_reached */
                    virtual_ir = rew_meas->show_ir_value();
                    rew_sum->ch_virtual_ir( virtual_ir );
                    rew_sum->ch_local_bound_reached( FALSE );

                    /* gravity system for rate_reward local bound, does not need to act, */
                    /* cause if the CR value of rate_reward came close to a bound in     */
                    /* the last rate_reward update the gravity already attracted it.     */
                    /* however virtual_ir should be 0 if the bound was already reached.  */
                    if ( (rew_meas->is_upp_bounded() == TRUE) && (rew_meas->show_ir_value() > 0) &&
                        (rew_meas->show_cr_value() >= (rew_meas->show_upp_bound() - y_epsilon)) )
                    {
                        rew_sum->ch_local_bound_reached( TRUE );
                        rew_sum->ch_virtual_ir( 0 );
                        virtual_ir = 0;
   // nocomp//fprintf(stderr, "(UPP) WILL s_time=%f - rew_id=%d - cr=%f - ir=%f - upp=%f\n",simulation_time,rew_meas->show_id(),rew_meas->show_cr_value(), rew_meas->show_ir_value(),rew_meas->show_upp_bound());
                    }
                    if ( (rew_meas->is_low_bounded() == TRUE) && (rew_meas->show_ir_value() < 0) &&
                        (rew_meas->show_cr_value() <= (rew_meas->show_low_bound() + y_epsilon)) )
                    {
    //fprintf(stderr, "(LOW) WILL s_time=%f - rew_id=%d - cr=%f - ir=%f - low=%f\n",simulation_time,rew_meas->show_id(),rew_meas->show_cr_value(), rew_meas->show_ir_value(),rew_meas->show_low_bound());
                        rew_sum->ch_local_bound_reached( TRUE );
                        rew_sum->ch_virtual_ir( 0 );
                        virtual_ir = 0;
                    }

                    ir_sum += virtual_ir;
                    if( virtual_ir < 0 )
                        ir_neg += virtual_ir;
                    else
                        ir_pos += virtual_ir;

                    // go to the next rew_sum.
                    rew_sum = rew_sum_list->show_next_rew();
                }

                // gravity system for rate_reward_sum bound.
                if ( (ra_rew_sum->is_upp_bounded() == TRUE) && (cr_sum > (ra_rew_sum->show_upp_bound()-y_epsilon)) )
                    cr_sum = ra_rew_sum->show_upp_bound();
                if ( (ra_rew_sum->is_low_bounded() == TRUE) && (cr_sum < (ra_rew_sum->show_low_bound()+y_epsilon)) )
                    cr_sum = ra_rew_sum->show_low_bound();

                calculate_virtual_ir( ra_rew_sum , ir_sum, ir_neg, ir_pos, ir_sum, cr_sum);

                /* get the first change in any rate associated */

                /* ------ get min_cross_time and ir/cr_sum ----- */
                min_cross_time = INFINITE;
                ir_sum = 0;
                cr_sum = 0;

                /* go throught all Reward_Sum objects */
                rew_sum = rew_sum_list->show_1st_rew();
                while (rew_sum != NULL)
                {
                    rew_meas = rew_sum->show_rew_measure();
                    /*------------------ calculate cross_time --------------------*/
                    cross_time = INFINITE;
                    if ( rew_sum->show_virtual_ir() != 0)
                        if ( rew_sum->show_virtual_ir() > 0 )
                        {
                           if ( (rew_meas->is_upp_bounded() == TRUE) &&
                                (rew_meas->show_cr_value() < (rew_meas->show_upp_bound() - y_epsilon)) )
                                cross_time = ( rew_meas->show_upp_bound() - rew_meas->show_cr_value() ) / rew_sum->show_virtual_ir();
                        }
                        else
                        {
                            if ( (rew_meas->is_low_bounded() == TRUE) &&
                                 (rew_meas->show_cr_value() > (rew_meas->show_low_bound() + y_epsilon)) )
                                cross_time = ( rew_meas->show_low_bound() - rew_meas->show_cr_value() ) / rew_sum->show_virtual_ir();
                        }

                    rew_sum->ch_cross_time( cross_time );
                    if ( cross_time < min_cross_time )
                    {
                        min_cross_time = cross_time;
                        rew_of_min_cross_time = rew_sum;
                    }
                    /*------------------end of calc cross_time -------------------*/
                    ir_sum += rew_sum->show_virtual_ir();
                    cr_sum += rew_meas->show_cr_value();

                    /* get cr and ir of the reward that have been processed */
                    rew_sum->show_rew_name( aux );
                    if( !strcmp( aux, rew_name ) )
                    {
                        //get cr and ir when rate_raward
                        cr = rew_meas->show_cr_value();
                        ir = rew_sum->show_virtual_ir();

                        //----- develop debug info ------------------
                        if ( (devel_debug == TRUE) && (simulation_time > dbg_time_low) && (simulation_time < dbg_time_upp) )
                        {
                            fprintf(stderr,"         rate_rew  cr=%f and ir=%f\n",cr,ir);
                        }
                        //----- end of develop debug info -----------
                    }

                    rew_sum = rew_sum_list->show_next_rew();
                }
                /* at this point we have min_cross_time containing the first    */
                /* rate change caused by local crosses, and cr/ir of the reward */
                /* being processed at cr and ir variables.                      */

                // gravity system for rate_reward_sum bound.
                if ( (ra_rew_sum->is_upp_bounded() == TRUE) && (cr_sum > (ra_rew_sum->show_upp_bound()-y_epsilon)) )
                    cr_sum = ra_rew_sum->show_upp_bound();
                if ( (ra_rew_sum->is_low_bounded() == TRUE) && (cr_sum < (ra_rew_sum->show_low_bound()+y_epsilon)) )
                    cr_sum = ra_rew_sum->show_low_bound();

                //get cr and ir when the reward being processed is rate_reward_sum
                if (is_ra_rew_sum)
                {
                    cr = cr_sum;
                    ir = ir_sum;

                    //----- develop debug info ------------------
                    if ( (devel_debug == TRUE) && (simulation_time > dbg_time_low) && (simulation_time < dbg_time_upp) )
                    {
                        fprintf(stderr,"         rate_rew_sum  cr=%f and ir=%f\n",cr,ir);
                    }
                    //----- end of develop debug info -----------
                 }

                /*---------------- calculate sum_cross_time ------------------*/
                sum_cross_time = INFINITE;
                if ( ir_sum != 0 )
                    if ( ir_sum > 0 )
                    {
                        if ( (ra_rew_sum->is_upp_bounded() == TRUE) && (cr_sum < (ra_rew_sum->show_upp_bound()-y_epsilon)) )
                            sum_cross_time = ( ra_rew_sum->show_upp_bound() - cr_sum ) / ir_sum;
                    }
                    else
                    {
                        if ( (ra_rew_sum->is_low_bounded() == TRUE) && (cr_sum > (ra_rew_sum->show_low_bound()+y_epsilon)) )
                            sum_cross_time = ( ra_rew_sum->show_low_bound() - cr_sum ) / ir_sum;
                    }
                /*--------------- end of calc sum_cross_time -----------------*/
    //some stat
    //ncomp//  fprintf(stderr,"cr_sum=%f    ir_sum=%f\n", cr_sum, ir_sum);
    //ncomp//  fprintf(stderr,"min_cross_time=%f    sum_cross_time=%f\n",min_cross_time,sum_cross_time );
    //ncomp//fprintf(stderr,"while laco=%d - last_time=%f - delta_t=%f\n",i,last_time,delta_t );

                /*-------- identify the first change in rates --------*/
                cross_time_by_sum = INFINITE;
                if ( ( sum_cross_time < INFINITE ) || ( min_cross_time < INFINITE ) )
                {
                   if ( (sum_cross_time + x_epsilon) < min_cross_time  )
                   {
                      /* get values calculated in the sum_cross_time */
                      ir_by_sum = ir_sum;
                      cr_by_sum = cr_sum;
                      cross_time_by_sum = sum_cross_time;
                   }
                   else
                   {
                      /* get values calculated in the cross_time */
                      ir_by_sum = rew_of_min_cross_time->show_virtual_ir();
                      cr_by_sum = rew_of_min_cross_time->show_rew_measure()->show_cr_value();
                      cross_time_by_sum = min_cross_time;
                   }
                }
                ra_rew_sum->ch_treated( TRUE );
                ra_rew_sum->ch_cross_time( cross_time_by_sum );
            }
        }
        else
        {
            /* case the reward are free from sum bound. */
            /* get cr and ir values */

            //cr = get_cumulative_reward( obj_st , rew_name );
            //ir = get_instantaneous_reward( obj_st, rew_name );
            cr = rr_touch->show_rew_meas()->show_cr_value();
            ir = rr_touch->show_rew_meas()->show_ir_value();
        }

        if( op == UPP_REACH_TYPE )
        {
            between_bounds = TRUE;
            if ( ((is_ra_rew_sum) && (ra_rew_sum->is_upp_bounded() && (ra_rew_sum->show_upp_bound() < (expr-y_epsilon)))) ||
                ((!is_ra_rew_sum) && (rate_rew_desc->is_upp_bounded()) && (rate_rew_desc->show_upp_bound() < (expr-y_epsilon))) )
                between_bounds = FALSE;
            if ( (ir > 0) && (cr < expr - y_epsilon) && (between_bounds) )
            {
                cross_flag = TRUE;
                /* Calculate the time_to_reach of the reward that is being processed. */
                time_to_reach = (expr - cr) / ir;
                /* If time_to_reach is greater than cross_time_by_sum, we got         */
                /* cross_time_by_sum and set a flag to indicate it.                   */
                /* Time_to_reach that are the time of the reward that have been       */
                /* analysed, have priority over cross_time_by_sum.                    */
                if ( (cross_time_by_sum < (time_to_reach - x_epsilon)) )
                {
                    skip_flag = TRUE;
                    time_to_reach = cross_time_by_sum;
                    /* do not forget: if the cross are ocasioned by cross_time_by_sum   */
                    /* the cr, and ir should not be used for update rate_rewards in the */
                    /* future, because they did not represent the delta_time that has   */
                    /* been set.                                                        */
                }
            }
        }
        else //op == LOW_REACH_TYPE
        {
            /* calculate when cr downcross the level */
            between_bounds = TRUE;
            if ( ((is_ra_rew_sum) && (ra_rew_sum->is_low_bounded() && (ra_rew_sum->show_low_bound() > (expr+y_epsilon)))) ||
                 ((!is_ra_rew_sum) && (rate_rew_desc->is_low_bounded()) && (rate_rew_desc->show_low_bound() > (expr+y_epsilon))) )
                between_bounds = FALSE;
            if ( (ir < 0) && (cr > expr + y_epsilon) && (between_bounds) )
            {
                cross_flag = TRUE;
                /* Calculate the time_to_reach of the reward that is being processed. */
                time_to_reach = (expr - cr) / ir;
                /* If time_to_reach is greater than cross_time_by_sum, we got         */
                /* cross_time_by_sum and set a flag to indicate it.                   */
                if ( (cross_time_by_sum < (time_to_reach - x_epsilon)) )
                {
                    skip_flag = TRUE;
                    time_to_reach = cross_time_by_sum;
                    /* do not forget: if the cross are ocasioned by cross_time_by_sum   */
                    /* the cr, and ir should not be used for update rate_rewards in the */
                    /* future, because they did not represent the delta_time that has   */
                    /* been set.                                                        */
                }
            }
        }

        /* If the reward being processed will cross, so this code below will be executed. */
        if ( cross_flag )
        {
            // find out if the time_to_reach is very close to zero.
            if ( time_to_reach < x_epsilon )
            {
                // Gravity system must act to avoid loop in simulation.
                time_to_reach = x_epsilon;
                //fprintf( stderr,"Warning: will_reach forecast zero time jump. Correcting to x_epsilon!\n" );
            }

            if ( time_to_reach < curr_delta_time )
            {

                //----- develop debug info ------------------
                if ( (devel_debug == TRUE) && (simulation_time > dbg_time_low) && (simulation_time < dbg_time_upp) )
                {
                    fprintf(stderr,"!!(crossing) time_to_reach < curr_delta_time  (skip_flag=%d)\n",skip_flag );
                }
                //----- end of develop debug info -----------

                // update rew_reach_distrib
                rew_reach_dist->ch_delta_time( time_to_reach );
                if ( skip_flag == FALSE )
                {
                    rew_reach_dist->ch_cr( cr );
                    rew_reach_dist->ch_ir( ir );
                    rew_reach_dist->ch_level( expr );
                }

                // update the stamp list (cut the rewards that won't reach)
                stamp = stamp_list->show_1st_stamp();
                while ( stamp != NULL )
                {
                    if ( stamp->show_delta_time() > ( time_to_reach + x_epsilon) )
                    {
                        stamp_aux = stamp_list->get_curr_stamp();
                        delete stamp_aux;
                        stamp = stamp_list->show_curr_stamp();
                    }
                    else
                        stamp = stamp_list->show_next_stamp();
                }

                /* if skip_flag==TRUE the expr was not reached yet! So we can't trust in */
                /* cr,ir and expr atributes of the stamp. the stamp remains there just   */
                /* because of skip_flag!!!                                               */

                // add new stamp
                stamp = new Stamp( time_to_reach, expr, (is_ra_rew_sum)?-1:rate_rew_desc->show_id(), skip_flag );
                rew_reach_dist->add_stamp( stamp );
                /*if ( stamp_list->have_to_skip() )
                    rew_reach_dist->ch_skip_event( skip_flag );
                else
                    rew_reach_dist->ch_skip_event( FALSE );*/
                rew_reach_dist->ch_skip_event( stamp_list->have_to_skip() );

            }
            else
            {
                //time_to_reach == curr_delta_time
                /* if skip_flag==TRUE the expr was not reached yet! So we can't */
                /* trust in cr,ir and expr atributes of the rew_reach_dist. the */
                /* stamp remains there just because of skip_flag!!!             */
                if( time_to_reach < (curr_delta_time + x_epsilon) )
                {
                    stamp = new Stamp( time_to_reach, expr, (is_ra_rew_sum)?-1:rate_rew_desc->show_id(), skip_flag );
                    rew_reach_dist->add_stamp( stamp );
                    /* when time_to_reach == curr_delta_time we have to analize */
                    /* the skip flag to see if the event is real or fake */
                    /* if there is at least one real event (skip_flag == FALSE) so the event must happens */

                    /* if until this moment the event was fake */
                    /* the skip_flag will determine the existence of the event! */
                    rew_reach_dist->ch_skip_event( stamp_list->have_to_skip() );

                    if ( skip_flag == FALSE )
                    {
                        rew_reach_dist->ch_cr( cr );
                        rew_reach_dist->ch_ir( ir );
                        rew_reach_dist->ch_level( expr );
                    }

                    //----- develop debug info ------------------
                    if ( (devel_debug == TRUE) && (simulation_time > dbg_time_low) && (simulation_time < dbg_time_upp) )
                    {
                        fprintf(stderr,"!()!(crossing) time_to_reach == curr_delta_time  (skip_flag=%d)\n",skip_flag );
                    }
                    //----- end of develop debug info -----------
                }
            }
        }
    }
    else //rew_type == GET_IR
    {
        // to implement this feature it's necessary to keep "last_ir_value"
        fprintf(stderr,"Sorry: get_ir isn't available yet for REWARD_REACH event condition!\n");
    }

    //----- develop debug info ------------------
    if ( (devel_debug == TRUE) && (simulation_time > dbg_time_low) && (simulation_time < dbg_time_upp) )
    {
        fprintf(stderr,">> time_to_reach=%.10f  curr_delta_time=%.10f\n         simulation_time=%f\n<><> cross_flag=%d\n",time_to_reach, curr_delta_time, simulation_time, cross_flag );
        fprintf(stderr,">< skip_flag=%d - cr=%.13f - ir=%.13f\n",skip_flag,cr,ir );
        fprintf(stderr,"<<< inside distrib: d_time=%.13f cr=%.13f ir=%.13f skip_flag=%d\n", rew_reach_dist->show_delta_time(),rew_reach_dist->show_cr(),rew_reach_dist->show_ir(),rew_reach_dist->show_skip_event());
    }
    //----- end of develop debug info -----------

    return( cross_flag );
}
//------------------------------------------------------------------------------
int Simulator::show_rr_ev_flag()
{
    return rr_ev_flag;
}
//------------------------------------------------------------------------------
void Simulator::ch_rr_ev_flag( int a_val )
{
    rr_ev_flag = a_val;
}
//------------------------------------------------------------------------------
int Simulator::show_run_number()
{
    return run_number;
}
//------------------------------------------------------------------------------
void Simulator::ch_run_number( int a_val )
{
    run_number = a_val;
}
//------------------------------------------------------------------------------
Action *Simulator::choose_an_action(Object_State *obj_st, Action_List *act_list)
{
    Action   *act, *chosen_act;
    Expr_Val *prob;
    int       chosen = FALSE;
    int       act_count = 1;
    TGFLOAT    uniform, accum = 0;

    /* Generate a uniform random variable */
    uniform = action_random_obj->next_randval();

    /* go through all actions an pick one */
    act = act_list->show_1st_act();
    chosen_act = NULL;
    while (act != NULL)
    {
        /* update the execution context structure */
        context.act_num = act_count;
        act_count++;

        /* evaluate the prob of the action */
        prob = act->eval_prob(obj_st);
        accum = accum + prob->show_value();
        /* if it is this act then break */
        if ((uniform <= accum) && (!chosen))
        {
            chosen_act = act;
            chosen = TRUE;
        }
        else
        {
            /* just in case it doesn't satisfies all probs */
            if (!chosen)
                chosen_act = act;
        }
        delete prob;
        /* go to the next action */
        act = act_list->show_next_act();
    }

    /* check the probabilities of the action (must sum 1) */
    if ( (accum <  (1.0 - EPSILON)) || (accum > (1.0 + EPSILON)) )
    {
        fprintf(stderr,"ERROR: Probabilities do not sum one (1): %.8e\n", accum);
        print_context();
        fprintf(stderr,"Aborting...\n");
        exit (-1);
    }

    return (chosen_act);
}
//------------------------------------------------------------------------------
void Simulator::clear_reward_values(int run)
{
    Reward_Measure *rew_meas;
    Object_Description      *obj_desc;
    Object_Description_List *obj_desc_list;
    Rate_Reward_Desc        *rate_rew_desc;
    Rate_Reward_Desc_List   *rate_rew_desc_list = NULL;
    Imp_Reward_Desc         *imp_rew_desc;
    Imp_Reward_Desc_List    *imp_rew_desc_list = NULL;
    Event                   *event;
    Event_List              *event_list;
    Action                  *action;
    Action_List             *action_list;
    Message                 *message;
    Message_List            *message_list;
    Rate_Reward_Sum         *ra_rew_sum;
    Rate_Reward_Sum_List    *ra_rew_sum_list;
    //char                     name[MAXSTRING];

    /* clear the values in the rate reward list */
    rew_meas = rate_rew_list->show_1st_rew();
    while (rew_meas != NULL)
    {
        rew_meas->clear_values(run,last_simul_time);
        rew_meas = rate_rew_list->show_next_rew();
    }

    /* clear the values in the impulse reward list */
    rew_meas = imp_rew_list->show_1st_rew();
    while (rew_meas != NULL)
    {
        rew_meas->clear_values(run,last_simul_time);
        rew_meas = imp_rew_list->show_next_rew();
    }

    /* Clear ir values in Rate_Reward_Desc too!! */
    /* Do not forget ir is redundantly storaged! */
    /* Initialize the cr initial value in Reward_Measure */
    /*   and in the trace files ( from rate and impulse  */
    /*   rewards ) */
    obj_desc_list = the_system_desc->show_obj_desc_list( LIST_RO );
    obj_desc      = obj_desc_list->show_1st_obj_desc();
    while( obj_desc != NULL )
    {
        /* goes throught all Rate_Reward_Desc objects */
        rate_rew_desc_list = obj_desc->show_reward_list( LIST_RO );
        rate_rew_desc = rate_rew_desc_list->show_1st_rew();
        while ( rate_rew_desc != NULL )
        {
            /* clear the ir and last_ir values */
            rate_rew_desc->ch_last_ir_val( 0 );
            rate_rew_desc->ch_ir_user_set_flag(FALSE);
            rate_rew_desc->ch_ir_user_set_value( 0 );

            if ( rate_rew_desc->show_rew_meas() != NULL )
            {
                /* initialize the correct cr initial value */
                rate_rew_desc->show_rew_meas()->ch_cr_value( rate_rew_desc->show_cr_initial_value() );
                rate_rew_desc->show_rew_meas()->set_first_cr( rate_rew_desc->show_cr_initial_value() );
            }

            /* go to the next Rate_Reward_Desc */
            rate_rew_desc = rate_rew_desc_list->show_next_rew();
        }
        delete rate_rew_desc_list;

        /* goes throught all Imp_Reward_Desc objects of the Events */
        event_list = obj_desc->show_event_list( LIST_RO );
        event = event_list->show_1st_event();
        while ( event != NULL )
        {
            action_list = event->show_action_list( LIST_RO );
            action = action_list->show_1st_act();
            while ( action != NULL )
            {
                imp_rew_desc_list = action->show_impulse_list( LIST_RO );
                imp_rew_desc = imp_rew_desc_list->show_1st_rew();
                while ( imp_rew_desc != NULL )
                {
                    if ( imp_rew_desc->show_rew_meas() != NULL )
                    {
                        /* initialize the correct cr initial value */
                        imp_rew_desc->show_rew_meas()->ch_cr_value( imp_rew_desc->show_cr_initial_value() );
                        imp_rew_desc->show_rew_meas()->set_first_cr( imp_rew_desc->show_cr_initial_value() );
                    }

                    /* go to the next Rate_Reward_Desc */
                    imp_rew_desc = imp_rew_desc_list->show_next_rew();
                }
                delete imp_rew_desc_list;

                /* go to the next Action */
                action = action_list->show_next_act();
            }
            delete action_list;

            /* go to the next Event */
            event = event_list->show_next_event();
        }
        delete event_list;

        /* goes throught all Imp_Reward_Desc objects of the Messages */
        message_list = obj_desc->show_message_list( LIST_RO );
        message = message_list->show_1st_msg();
        while ( message != NULL )
        {
            action_list = message->show_action_list( LIST_RO );
            action = action_list->show_1st_act();
            while ( action != NULL )
            {
                imp_rew_desc_list = action->show_impulse_list( LIST_RO );
                imp_rew_desc = imp_rew_desc_list->show_1st_rew();
                while ( imp_rew_desc != NULL )
                {
                    if ( imp_rew_desc->show_rew_meas() != NULL )
                    {
                        /* initialize the correct cr initial value */
                        imp_rew_desc->show_rew_meas()->ch_cr_value( imp_rew_desc->show_cr_initial_value() );
                        imp_rew_desc->show_rew_meas()->set_first_cr( imp_rew_desc->show_cr_initial_value() );
                    }

                    /* go to the next Rate_Reward_Desc */
                    imp_rew_desc = imp_rew_desc_list->show_next_rew();
                }
                delete imp_rew_desc_list;

                /* go to the next Action */
                action = action_list->show_next_act();
            }
            delete action_list;

            /* go to the next Message */
            message = message_list->show_next_msg();
        }
        delete message_list;

        /* go to the next Object */
        obj_desc = obj_desc_list->show_next_obj_desc();
    }
    delete obj_desc_list;

    /* Updates the cr in the Reward_Measure of the Rate_Reward_Sum objects */
    ra_rew_sum_list = show_rate_rew_sum_list( LIST_RW );
    ra_rew_sum = ra_rew_sum_list->show_1st_rew();
    /* go through all the rate_reward_sum of the object */
    while( ra_rew_sum != NULL )
    {
        if ( ra_rew_sum->show_rew_measure() != NULL )
        {
            /* initialize the correct cr initial value */
            ra_rew_sum->show_rew_measure()->ch_cr_value( ra_rew_sum->show_cr_initial_value() );
            ra_rew_sum->show_rew_measure()->set_first_cr( ra_rew_sum->show_cr_initial_value() );
        }
        ra_rew_sum = ra_rew_sum_list->show_next_rew();
    }
}
//------------------------------------------------------------------------------
void Simulator::reward_reset(int run)
{
    last_simul_time = simulation_time;
    /* clear all the reward values in both lists (rate and impulse) */
    clear_reward_values(-1);
}
//------------------------------------------------------------------------------
void Simulator::reset(int run)
{
    Simulator_Event *simul_ev;
    Clone_Event     *clone_ev;

    /* clear all values of simulation */
    last_simul_time = simulation_time;
    simulation_time = 0;
    transitions     = 0;
    vanishing_trans = 0;

    /* updates the run_number attribute */
    run_number = run+1;

    /* clear the number of transitions from each event */
    /* clear all event samples from the list           */
    simul_ev = event_list->show_1st_event();
    while (simul_ev != NULL)
    {
        simul_ev->clear_ev_sample_list();
        simul_ev->clear_no_triggers();
        simul_ev = event_list->show_next_event();
    }

    /* clear clone event list */
    clone_ev = clone_ev_list->get_1st_clone();
    while (clone_ev != NULL)
    {
        delete clone_ev;
        clone_ev = clone_ev_list->get_1st_clone();
    }

    /* clear all the reward values in both lists (rate and impulse) */
    clear_reward_values(run);

    /* set the current system state to the first system state */
    curr_sys_st = new System_State( first_system_state );

    /* reset all distributions of the system */
    reset_event_list( run );

    /* initialize the event list with all enabled events */
    initialize_event_list();

    last_simul_time = 0;
}
//------------------------------------------------------------------------------
void Simulator::set_stopping_event(char *obj_ev_name)
{
    Object_Description *obj_desc;
    Event              *event;
    Simulator_Event    *simul_ev;
    char                obj_name[MAXSTRING];
    char                ev_name[MAXSTRING];
    char                buff[MAXSTRING];

    simul_ev = event_list->show_1st_event();
    while (simul_ev != NULL)
    {
        obj_desc = simul_ev->show_obj_desc();
        obj_desc->show_name(obj_name);
        event = simul_ev->show_event();
        event->show_name(ev_name);
        sprintf (buff, "%s.%s", obj_name, ev_name);
        if (!strcmp(obj_ev_name, buff))
        {
            stopping_event = simul_ev;
            break;
        }
        simul_ev = event_list->show_next_event();
    }
}
//------------------------------------------------------------------------------
void Simulator::define_stopping_state(int obj_id, int st_var_id)
{
    /* define the stopping state */
    stopping_state = TRUE;
    stop_obj_id    = obj_id;
    stop_st_var_id = st_var_id;
}
//------------------------------------------------------------------------------
void Simulator::set_stopping_state(State_Variable *low_st, State_Variable *upp_st)
{
    /* set the new values for the stopping states */
    if (stop_low_st_var != NULL)
        delete stop_low_st_var;
    stop_low_st_var = low_st;

    if (stop_upp_st_var != NULL)
    {
        delete stop_upp_st_var;
    }
    stop_upp_st_var = upp_st;
}
//------------------------------------------------------------------------------
int Simulator::show_finish_point()
{
  return (finish_point);
}
//------------------------------------------------------------------------------
int Simulator::end_of_simulation()
{
    Object_State_List   *obj_st_list;
    Object_State        *obj_st;
    State_Variable_List *st_var_list;
    State_Variable      *st_var;
    /* check for the end of the simulation */
    /* check for the definition of a stopping state */
    
    if (stopping_state == TRUE)
    {   
        /* get the correct object */ 
        obj_st_list = curr_sys_st->show_obj_st_list(LIST_RO);
        obj_st = obj_st_list->show_1st_obj_st();
        for( int i = 1; i < stop_obj_id; i++ )
            obj_st = obj_st_list->show_next_obj_st();
        /* get the correct state variable */
        st_var_list = obj_st->show_st_var_list(LIST_RO);
        st_var = st_var_list->show_1st_st_var();
//        for( int i = 1; i < stop_st_var_id; i++ )
        while( st_var != NULL && st_var->show_code() != stop_st_var_id )
        {
            st_var = st_var_list->show_next_st_var();

        }
        delete obj_st_list;
        delete st_var_list;
        if( st_var != NULL )
        {
            /* check both limiting states */
            if (stop_low_st_var != NULL)
                if (st_var->compare(stop_low_st_var))
                {
                    finish_point = LOWER_ST;
                    return (TRUE);
                }


            if (stop_upp_st_var != NULL)
                if (st_var->compare(stop_upp_st_var))
                {
                    finish_point = UPPER_ST;
                    return (TRUE);
                }
        }
        else
        {
            debug( 1, "Warning: checking for stopping state with an unknown variable\n" );
        }
        /* check for maximum simulation time */
        if (simulation_time >= max_simul_time)
        {
            finish_point = MAX_TIME;
            return (TRUE);
        }
        
    }
    else
    {
        /* if no stopping states were defined */
        /* check for a stopping event         */
        if( stopping_event != NULL )
        {
            /* check the number of triggers of the event */
            if( stopping_event->show_no_triggers() >= max_simul_trans )
            {
                finish_point = STOP_EVENT;
                return( TRUE );
            }
        }
        else
        {
            /* check for maximum transitions or maximum time */
            if( max_simul_trans != 0 && transitions >= max_simul_trans )
            {
                finish_point = MAX_TRANS;
                return( TRUE );
            }
            if( max_simul_time  != 0 && simulation_time >= max_simul_time )
            {
                finish_point = MAX_TIME;
                return( TRUE );
            }
        }
    }
    return( FALSE );
}
//------------------------------------------------------------------------------
void Simulator::simulate_model_batch()
{
    System_State       *next_sys_st;
    Transition         *trans;
    Simulator_Event    *simul_ev;
    Event_Sample       *ev_sample;
    Distribution       *distrib;
    Rew_Reach_Distrib  *rew_reach_dist = NULL;
    Object_State       *obj_st = NULL;
    Object_State_List  *obj_st_list = NULL;
    FILE               *stepsfile = NULL;
    char                filename[MAXSTRING];
    char                ev_name[MAXSTRING];
    int                 equal_times_counter=0, dist_ev_counter=0;
    int                 rew_reached_ev_counter=0;

    /*----- open the trace file that contains the simulation steps -----*/
    if( Rew_Files == TRACE )
    {
        sprintf( filename, "%s.SIMUL.steps.trace", base_filename );
        if( (stepsfile = fopen (filename, "w")) == NULL )
        {
            perror( "fopen SIMUL.steps" );
            return;
        }
        fprintf(stepsfile,"# This trace contains every time treated in the simulation.\n");
        fprintf(stepsfile,"# They represent the jumps in time line.\n#\n");
        fprintf(stepsfile,"# Notation:\n#  skiped - the time is fake and was skiped\n");
        fprintf(stepsfile,"#  RR_ev=name - REWARD_REACHED event type trigger and it's name\n#\n");
    }
    /*----- end of the trace file oppening -----*/

    /*----- kernel of the simulator -----*/
    while( !end_of_simulation() )
    {
        if( Debug_Level > 0 )
        {
            /* print the current system state */
            curr_sys_st->print_sys_st();
        }

        if( Debug_Level > 1 )
        {
            /* print the current event list */
            print_ev_list();
        }
        /* get the event with the smallest time */
        simul_ev = event_list->show_smallest_time();
        if( simul_ev == NULL )
        {
            fprintf( stderr, "SIMULATION FINISHED:\n" );
            curr_sys_st->print_sys_st();
            fprintf( stderr, " WARNING: No more enabled events in event List.\n" );
            return;
        }

        // If the event is REWARD_REACHED and flag trigger_now is TRUE, disable flag.
        distrib = simul_ev->show_event()->show_distrib();
        if( distrib->show_type() == REW_REACH_DIST )
        {
            rew_reach_dist = (Rew_Reach_Distrib *)distrib;
            if( rew_reach_dist->show_trigger_now() == TRUE )
            {
                rew_reach_dist->ch_trigger_now( FALSE );
                /*--- debug info ---*/
                //simul_ev->show_event()->show_name(ev_name);
                //fprintf(stderr,">>>>>>>>>>>>>Ev %s disparou  -  t=%f.\n",ev_name,simulation_time );
                /*--- end of debug info ---*/
            }
        }

        /* get the 1st (smallest) event sample */
        ev_sample = simul_ev->get_1st_ev_sample();

        /* update the simulation time */
        last_simul_time = simulation_time;
        simulation_time = ev_sample->show_time();

        /* delete the sample being processed */
        delete ev_sample;

        /*----- print the time being processed in the <model>.SIMUL.steps file -----*/
        if( stepsfile != NULL )
        {
            fprintf( stepsfile, "%.10f ", simulation_time );
            fflush( stepsfile );
        }
        /*----- end of printing in the <model>.SIMUL.steps file -----*/

        /* check if maximum simul time has reached and update the simul time */
        if( max_simul_time != 0 && simulation_time > max_simul_time )
        {
            finish_point = MAX_TIME;
            simulation_time = max_simul_time;
            /* update the rate reward for the current system state */
            update_rate_rewards( simul_ev );
            /* close the <model>.SIMUL.steps file */
            if( stepsfile != NULL )
            {
                fclose( stepsfile );
            }
            break;
        }
        else
        {
            /* Update the rate reward for the current system state */
            /* only when necessary. optimized!                     */
            /* To accelerate the simulation rate rewards must not  */
            /* to be updated if the simul_time is equal ot the     */
            /* last_simul_time.                                    */
            if ((simulation_time != last_simul_time) || (DO_NOT_OPTIMIZE_REW_UPDATE))
            {
                update_rate_rewards( simul_ev );
            }
            if( simulation_time != last_simul_time )
            {
                equal_times_counter = 0;
            }
            else
            {
                /* Simulation time equal to last_simul_time */
                /* increment equal_times */
                equal_times_counter++;
                if( (equal_times_counter % 10000) == 0 )
                {
                    fprintf(stderr,"Warning: Simulation time is the same (time=%f) for the last %d steps\n",simulation_time,equal_times_counter);
                    fprintf(stderr,"The model seems to be in a infinite loop.\n");
                    fprintf(stderr,"If the time doesn't advance, cancel the simulation and check your model.\n");
                    /* Allows user find out if equal_times is growing cause of an REWARD_REACHED event */
                    fprintf(stderr,"Ordinary Events Counter: %d - Reward Reached Events Counter: %d.\n",dist_ev_counter,rew_reached_ev_counter);
                }
            }
        }

        /* Verify the accuracy of this event.                                */
        /* If the event should not trigger at this time, skip the other      */
        /* functionalities. This event is evaluated again and the simulation */
        /* continues.                                                        */
        if( distrib->show_type() == REW_REACH_DIST )
        {
            /* reward reached events counter */
            rew_reached_ev_counter++;
            if( stepsfile != NULL )
            {
                simul_ev->show_event()->show_name( ev_name );
                fprintf( stepsfile, "RR_ev=%s ", ev_name );
                fflush( stepsfile );
            }

            if( rew_reach_dist->show_skip_event() == TRUE )
            {
                if( stepsfile != NULL )
                {
                    fprintf( stepsfile, "skiped\n" );
                    fflush( stepsfile );
                }

                /* Every user code will be skiped, so the obj will */
                /* remain in the same state. Now get obj state!!   */
                obj_st_list = curr_sys_st->show_obj_st_list( LIST_RO );
                if( obj_st_list->query_obj_st( simul_ev->show_obj_desc()->show_id() ) )
                {
                    obj_st = obj_st_list->show_curr_obj_st();
                }
                else
                {
                    fprintf( stderr, "Internal ERROR: in simulate_model_batch(): obj_st not found!\n");
                    exit( -1 );
                }

                /* Evaluate the condition under this object state.                                */
                /* This is necessary only for the event that created the fake trigger.            */
                /* Note: nothing changes for the other events, so the evaluation isn't necessary. */

                /* reset distrib before evaluate */
                /* the parameter passed to reset() are unused for this distrib. */
                simul_ev->show_event()->reset_distrib( 0 );

                if (simul_ev->eval_cond(obj_st) == TRUE)
                {
                    // Completion of the evaluation - analysys of the every special argument.
                    eval_rr_special_args( rew_reach_dist );

                    /* NOTE: eval_cond could be false now case ir_virtual = 0 */
                    /* resample the event.*/
                    simul_ev->generate_sample( obj_st, simulation_time );
//ncomp //fprintf(stderr,"- gerei novo sample p/ este evento sample=%f\n",rew_reach_dist->show_delta_time());
                }
                continue;
            }
        }
        else
        {
            /* ordinary events counter */
            dist_ev_counter++;
        }

        /* writes in the <model>.SIMUL.steps file */
        if( stepsfile != NULL )
            fprintf( stepsfile, "\n" );

        /* update the number of triggers of this event */
        simul_ev->inc_no_triggers();

        /* make one transition to a new system state */
        next_sys_st = simulate_to_a_leaf( simul_ev );

        /* if impulse rewards were defined in this model */
        if( the_system_desc->show_impulse_reward() == TRUE )
        {
            /* get the transition with the rewards */
            trans = next_sys_st->get_1st_trans();
            /* accumulate the reward in the current transition */
            add_imp_rew_list( trans );
            /* delete this transition */
            delete trans;
        }

        /* delete the old state */
        delete curr_sys_st;

        /* update the current system state */
        curr_sys_st = next_sys_st;

        /* update the number of transitions */
        transitions++;

        /* print a log of the evolution */
        if( !(transitions % 10000) )
        {
            // Temporary status! Some day the java interface will show this info!!
            print_trans_time();
        }
    }

    if( Debug_Level > 0 )
        /* print the current system state */
        curr_sys_st->print_sys_st();

    if( Debug_Level > 1 )
        /* print the current event list */
        print_ev_list();
}
//------------------------------------------------------------------------------
System_State *Simulator::simulate_to_a_leaf( Simulator_Event *simul_ev )
{
    System_State         *new_sys_st, *van_sys_st;
    Object_Description   *obj_desc;
    Object_State         *obj_st;
    Action_List          *act_list;
    Action               *action;
    Event                *event;
    Transition           *trans;
    Pending_Message      *pend_msg;
    Pending_Message_List *pend_msg_list, *new_pend_msg_list;
    Expr_Val             *rate;
    char                  src_obj[MAXSTRING], port[MAXSTRING];

    /* get the event description */
    event = simul_ev->show_event();

    /* get the action list of this event */
    act_list = event->show_action_list(LIST_RO);

    /* get the object state of curr_sys_st */
    obj_desc = simul_ev->show_obj_desc();
    obj_st = curr_sys_st->show_obj_st(obj_desc);

    /* update the object name in the context */
    obj_desc->show_name(context.obj_name);

    /* update the object name in the context */
    event->show_name(context.ev_msg_name);
    context.ev_msg_type = CONTEXT_EV_TYPE;

    /* choose the appropriate action */
    action = choose_an_action( obj_st, act_list );
    delete act_list;

    /* create a new transition with the appropriate rate */
    /* rate = event->eval_rate(obj_st);                  */
    rate = new Expr_Val( 1.0 );
    trans = new Transition( rate );

    /* append the new transition to this system state */
    curr_sys_st->add_transition(trans);

    /* process the action and obtain a new system state */
    new_sys_st = process_action( curr_sys_st, obj_st, action );

    /* while the state is vanishing */
    while( new_sys_st->show_no_pend_msg() > 0 )
    {
        /* attribute the new_sys_st to a vanishing system state */
        van_sys_st = new_sys_st;

        /* get the first message to be delivered */
        pend_msg_list = van_sys_st->show_pend_msg_list(LIST_RW);
        pend_msg = pend_msg_list->get_1st_pend_msg();

        /* check to see multiple destination */
        if( pend_msg->is_multidest() )
        {
            /* implements breadth first                   */
            /* imagine A has 3 messages and B has 0       */
            /* |A|B| -> |A1|A2|A3|B| -> |A2|A3|B|A1x|A1y| */

            /* get the information of the pending message */
            pend_msg->show_src_obj_name(src_obj);
            pend_msg->show_port_name(port);
            /* expand multiple destinations to different pend messages */
            new_pend_msg_list = pend_msg->expand();
            /* concat the lists                           */
            pend_msg_list->concat_pend_msg_list(new_pend_msg_list);
            /* check for some destination for the message */
            if( pend_msg_list->is_empty() )
            {
                fprintf(stderr,"ERROR: Object %s sent a message through port \"%s\" but no other object is connected to this port.\n", src_obj, port);
                exit(-1);
            }
            /* delete the multiple destination message */
            delete pend_msg;
            /* get the first real pending message */
            pend_msg = pend_msg_list->get_1st_pend_msg();
        }
        /* update the execution context */
        pend_msg->show_dest_obj_name(context.obj_name);
        pend_msg->show_port_name(context.ev_msg_name);
        context.ev_msg_type = CONTEXT_MSG_TYPE;

        /* deliver process the message geting a new system state */
        new_sys_st = process_pend_msg(van_sys_st, pend_msg);

        /* delete memory from the vanishing state processed */
        delete van_sys_st;
        /* delete the first pending message that has been delivered */
        delete pend_msg;

        /* update the number of vanishing transitions */
        vanishing_trans++;
    }

    return (new_sys_st);
}
//------------------------------------------------------------------------------
System_State *Simulator::process_action( System_State *sys_st,
                                         Object_State *obj_st,
                                         Action       *act )
{
    System_State *new_sys_st;
    Object_State *new_obj_st;
    Expr_Val     *prob_val;
    Transition   *trans;

    /* set the transition to the new system state.                    */
    /* this will be a transition from this system state to new_sys_st */
    /* temporarily stored in new_sys_st (from itself to itself)       */

    /* get the transition created by the firing of an event */
    trans = sys_st->get_1st_trans();
    /* get the prob value of this action */
    prob_val = act->eval_prob( obj_st );
    /* change the rate of the new trans */
    trans->mul_rate( prob_val );
    /* delete the prob value */
    delete prob_val;

    /* execute the action (user code associates with it) */
    new_obj_st = act->evaluate( obj_st, this );

    /* process the event list to check for changes in the events */
    process_event_list( new_obj_st );

    /* process the cloned events that might have appeared */
    clone_events( obj_st );

    /* create a copy of this system state */
    new_sys_st = new System_State( sys_st );

    /* replace the object modified by the action just executed */
    new_sys_st->replace_obj_st( new_obj_st );

    /* add the transition to the new system state */
    new_sys_st->add_transition( trans );

    /* copy the messages sent in the action stored in the object */
    /* state to the new system state.                            */
    new_sys_st->copy_pend_msg( new_obj_st );

    /* if impulse rewards were defined for this system desc */
    if( the_system_desc->show_impulse_reward() == TRUE )
    {
        /* calculate the impulse rewards for the new transition */
        new_sys_st->calculate_impulse_rewards( obj_st, act );
    }

    return new_sys_st;
}
//------------------------------------------------------------------------------
System_State *Simulator::process_pend_msg( System_State *sys_st,
                                           Pending_Message *pend_msg )
{
    System_State       *new_sys_st;
    Object_State       *obj_st_dest;
    Pending_Message    *pend_msg_aux;
    Object_Description *obj_desc_dest;
    Message            *msg;
    Action_List        *act_list;
    Action             *action;
    char                src_obj_name[MAXSTRING];  /* name of the object sending a message   */
    char                dest_obj_name[MAXSTRING]; /* name of the object receiving a message */
    char                port_name[MAXSTRING];     /* name of the port receiving a message   */

    /* get the name of the source and dest object and the destination portname */
    pend_msg->show_src_obj_name(src_obj_name);
    pend_msg->show_dest_obj_name(dest_obj_name);
    pend_msg->show_port_name(port_name);

    pthread_mutex_lock(&simulator_lock);

    /* get the description of the destination object of the message */
    obj_desc_dest = the_system_desc->show_obj_desc(dest_obj_name);
    if (obj_desc_dest == NULL)
    {
        fprintf(stderr,"ERROR: No object named .%s. in the system description.\n",dest_obj_name);        
        exit(-1);
    }

    /* get the message description to be delivered to the object */
    msg = obj_desc_dest->show_message(port_name);
    if (msg == NULL)
    {
        fprintf(stderr,"ERROR: Object %s ins't able to receive a message from object %s outgoing on port %s.\n", dest_obj_name, src_obj_name, port_name);
        exit(-1);
    }

    pthread_mutex_unlock(&simulator_lock);

    /* get the action_list of the message part */
    act_list = msg->show_action_list(LIST_RO);

    /* get the object state destination of the message */
    obj_st_dest = sys_st->show_obj_st(obj_desc_dest);

    /* choose an appropriate action */
    action = choose_an_action(obj_st_dest, act_list);
    delete act_list;

    /* add a copy of the pending message to the object state */
    pend_msg_aux = new Pending_Message(pend_msg);
    obj_st_dest->add_pend_msg(pend_msg_aux);

    /* process the action and obtain a new system state */
    new_sys_st = process_action (sys_st, obj_st_dest, action);

    /* remove and delete the pending message that was delivered */
    pend_msg_aux = obj_st_dest->get_1st_pend_msg();
    delete pend_msg_aux;

    return ( new_sys_st );
}
//------------------------------------------------------------------------------
void Simulator::print_trans_time()
{
    fprintf( stdout, "Transitions: %d --- Simulator Time: %f --- Run: %d\n",transitions, simulation_time, run_number+1);
    fflush( stdout );
}
//------------------------------------------------------------------------------
void Simulator::print_curr_st()
{
    curr_sys_st->print_sys_st();
}
//------------------------------------------------------------------------------
void Simulator::print_ev_list()
{
    Simulator_Event *simul_ev;

    simul_ev = event_list->show_1st_event();
    while (simul_ev != NULL)
    {
        simul_ev->print_simul_ev();
        simul_ev->print_ev_sample_list();
        simul_ev = event_list->show_next_event();
    }
}
//------------------------------------------------------------------------------
void Simulator::print_no_triggers(FILE *fd)
{
    Object_Description *obj_desc;
    Event              *event;
    Simulator_Event    *simul_ev;
    char                obj_name[MAXSTRING];
    char                ev_name[MAXSTRING];
    int                 no_trig;

    simul_ev = event_list->show_1st_event();
    while (simul_ev != NULL)
    {
        obj_desc = simul_ev->show_obj_desc();
        obj_desc->show_name(obj_name);
        event = simul_ev->show_event();
        event->show_name(ev_name);
        no_trig = simul_ev->show_no_triggers();
        if (fd != NULL)
            fprintf( fd, " Event: %s.%s --- %d triggers\n", obj_name, ev_name, no_trig);
        else
            fprintf( stdout, " Event: %s.%s --- %d triggers\n", obj_name, ev_name, no_trig);
        simul_ev = event_list->show_next_event();
    }
}
//------------------------------------------------------------------------------
void Simulator::print_rewards(FILE *fd)
{
    /* if rate rewards were defined in this model */
    if (the_system_desc->show_rate_reward() == TRUE)
    {
        /* print to file or to output_file */
        if (fd == NULL)
        {
            /* print the rewards for the simulation */
            fprintf ( stdout, "\n--------------------- Rate Rewards ---------------------\n\n");
            rate_rew_list->print_rew(NULL);
        }
        else
        {
            /* print the rewards for the simulation */
            fprintf (fd, "\n--------------------- Rate Rewards ---------------------\n\n");
            rate_rew_list->print_rew(fd);
        }
    }

    /* if impulse rewards were defined in this model */
    if (the_system_desc->show_impulse_reward() == TRUE)
    {
        /* print to file or to output_file */
        if (fd == NULL)
        {
            /* print the impulse rewards for the simulation */
            fprintf ( stdout, "\n------------------- Impulse Rewards --------------------\n\n");
            imp_rew_list->print_rew(NULL);
        }
        else
        {
            /* print the impulse rewards for the simulation */
            fprintf (fd, "\n------------------- Impulse Rewards --------------------\n\n");
            imp_rew_list->print_rew(fd);
        }
    }
}
//------------------------------------------------------------------------------
void Simulator::generate_output_files()
{
    if ( output_file == NULL )
    {
        fprintf( stderr,"Error: Output file is not defined!\n" );
        exit(0);
    }

    fprintf( output_file, " Simulation\n");
    fprintf( output_file, " Number of transitions: %d\n", transitions);
    fprintf( output_file, " Simulator time: %.8E\n", simulation_time);

    /* printf the number of triggers per event */
    print_no_triggers( output_file );

    /* print both rewards lists */
    print_rewards(output_file);
}
//------------------------------------------------------------------------------


