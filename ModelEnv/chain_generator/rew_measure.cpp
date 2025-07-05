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
*  @file rew_measure.cpp
*  @brief <b>Reward measure</b>.
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

#ifdef __APPLE__
#include <math.h>
#else
// The values.h is deprecated.
#include <values.h>
#endif

#include <string.h>
#ifdef TG_PARALLEL
#include <pvm3.h>
#endif

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "random.h"
#include "simulator.h"
#include "system_desc.h"

/** The global base filename of the model */
extern char       base_filename[MAXSTRING];

//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
Reward_Measure::Reward_Measure(char *a_name, int a_id)
{
  strcpy (name, a_name);
  type = NOT_SPECIFIED_TYPE;
  id = a_id;
  time = 0;
  curr_cr_val = 0.0;
  curr_ir_val = 0.0;
  avg_cr_val = 0;
  has_level = FALSE;
  rew_level = 0;
  time_abv_lv = 0;
  low_bounded = FALSE;
  upp_bounded = FALSE;
  low_bound = 0;
  upp_bound = 0;
  rate_sum_bounded = FALSE;
  rate_sum_bound = NULL;
  fd_cr = NULL;
  fd_ir = NULL;
  RR_events_list = NULL;
  lasttime_cr = lasttime_ir = last_value_cr = last_value_ir = -MAXFLOAT;
  first_cr_val = first_ir_val = 0.0;

  debug(4,"Reward_Measure::Reward_Measure(char *): creating object");
}
//------------------------------------------------------------------------------
Reward_Measure::Reward_Measure(Reward_Measure *a_rew)
{
  a_rew->show_name(name);
  type = a_rew->show_type();
  id = a_rew->show_id();
  time = a_rew->show_time();
  curr_cr_val = a_rew->show_cr_value();
  curr_ir_val = a_rew->show_ir_value();
  avg_cr_val = a_rew->show_avg_cr_value();
  has_level = a_rew->show_has_level();
  rew_level = a_rew->show_rew_level();
  time_abv_lv = a_rew->show_time_abv_lv();
  low_bounded = a_rew->is_low_bounded();
  upp_bounded = a_rew->is_upp_bounded();
  low_bound = a_rew->show_low_bound();
  upp_bound = a_rew->show_upp_bound();
  rate_sum_bounded = a_rew->is_rate_sum_bounded();
  rate_sum_bound = a_rew->show_rate_sum_bound();
  RR_events_list = NULL;  /* the RR_events_list isn't copied! */
  fd_cr = NULL;
  fd_ir = NULL;
  lasttime_cr = a_rew->lasttime_cr;
  lasttime_ir = a_rew->lasttime_ir;
  last_value_cr = a_rew->last_value_cr;
  last_value_ir = a_rew->last_value_ir;
  first_cr_val = a_rew->first_cr_val;
  first_ir_val = a_rew->first_ir_val;

  debug(4,"Reward_Measure::Reward_Measure(Reward_Measure *): creating object");
}
//------------------------------------------------------------------------------
Reward_Measure::~Reward_Measure()
{
  debug(4,"Reward_Measure::~Reward_Measure(): destroying object");
}
//------------------------------------------------------------------------------
//     M E T H O D S
//------------------------------------------------------------------------------
void Reward_Measure::clear_values(int run_number, TGFLOAT simul_time )
{
  if (run_number != 0)
  {
     write_cr_to_file( simul_time, last_value_cr );
     write_ir_to_file( simul_time, last_value_ir );
  }
  if ((fd_cr != NULL)&& (run_number >= 0))
  {
     fprintf(fd_cr,"\n#----------------------- run:%d -----------------------\n",run_number+2);
     fprintf (fd_cr, "%.10E %.10E\n",0.0,first_cr_val);
  }
  if ((fd_ir != NULL)&& (run_number >= 0))
  {
      fprintf(fd_ir,"#----------------------- run:%d -----------------------\n",run_number+2);
     fprintf (fd_cr, "%.10E %.10E\n",0.0,first_ir_val);
  }
  time = 0;
  /* do not forget: ir must be updated in Rate_Reward_Desc too!! */
  curr_ir_val = 0.0;
  /* do not forget: cr has an initial value that must be updated from Reward_Desc */
  curr_cr_val = 0.0;

  avg_cr_val = 0;
  time_abv_lv = 0;
}
//------------------------------------------------------------------------------
void Reward_Measure::show_name(char *a_name)
{
  strcpy (a_name, name);
}
//------------------------------------------------------------------------------
int Reward_Measure::show_type()
{
  return( type );
}
//------------------------------------------------------------------------------
void Reward_Measure::write_cr_to_file( TGFLOAT time, TGFLOAT value )
{
  if ((fd_cr != NULL) && (time >= lasttime_cr))
     fprintf (fd_cr, "%.10E %.10E\n", time, value);
  lasttime_cr = time;
}
//------------------------------------------------------------------------------
void Reward_Measure::write_ir_to_file( TGFLOAT time, TGFLOAT value )
{
  if ((fd_ir != NULL) && (time >= lasttime_ir))
      fprintf (fd_ir, "%.10E %.10E\n", time, value);
  lasttime_ir = time;
}
//------------------------------------------------------------------------------
void Reward_Measure::conditional_printf_cr( int condition, TGFLOAT time, TGFLOAT value )
{
  if ((condition) && (value != last_value_cr))
     write_cr_to_file( time, value );
  last_value_cr = value;
}
//------------------------------------------------------------------------------
void Reward_Measure::conditional_printf_ir( int condition, TGFLOAT time, TGFLOAT value )
{
  if ((condition) && (value != last_value_ir))
     write_ir_to_file( time, value );
  last_value_ir = value;
}
//------------------------------------------------------------------------------
void Reward_Measure::ch_type( int a_val )
{
  type = a_val;
}
//------------------------------------------------------------------------------
int Reward_Measure::show_id()
{
  return(id);
}
//------------------------------------------------------------------------------
/* This method open the CR and IR trace files for the Reward_Measure          */
//------------------------------------------------------------------------------
int Reward_Measure::open_trace_files( char *a_filename )
{
    char filename_cr[MAXSTRING], filename_ir[MAXSTRING];

    /* initialization */
    strcpy( filename_cr, base_filename );
    strcat( filename_cr, "." );

    if (a_filename == NULL)
    {
        strcat(filename_cr,name);
    }
    else
    {
       strcat (filename_cr,a_filename);
    }

    strcpy(filename_ir,filename_cr);
    strcat(filename_cr,".CR.trace");
    strcat(filename_ir,".IR.trace");

    /*----- creation of CR trace file -----*/
    if (  (fd_cr = fopen (filename_cr, "w")) == NULL)
    {
        perror ("fopen");
        fprintf ( stderr, "ERROR opening trace file: %s\n", filename_cr);
        return (-1);
    }

    /* write the file header */
    fprintf (fd_cr, "# This trace file contains the Cumulative Reward measure\n");
    switch ( type )
    {
        case RATE_REWARD_TYPE :
            fprintf (fd_cr, "# of the Rate Reward named: %s\n", name);
            break;
        case IMPULSE_REWARD_TYPE :
            fprintf (fd_cr, "# of the Impulse Reward named: %s\n", name);
            break;
        case RATE_REWARD_SUM_TYPE :
            fprintf (fd_cr, "# of the Rate Reward Sum named: %s\n", name);
            break;
        case GLOBAL_RATE_REWARD_TYPE :
            fprintf (fd_cr, "# of the Global Rate Reward named: %s\n", name);
            break;
        case GLOBAL_IMPULSE_REWARD_TYPE :
            fprintf (fd_cr, "# of the Global Impulse Reward named: %s\n", name);
            break;
        default:
            fprintf (fd_cr, "# of the Reward named: %s\n", name);
            break;
    }
    fprintf (fd_cr, "# ------------------- first run ----------------------\n");
    fprintf (fd_cr, "#\n");
    /* write the origin to the file */
    fprintf (fd_cr, "0.0000000000E+00 %.10E\n",curr_cr_val);

    /*----- creation of IR trace file -----*/
    if (  (fd_ir = fopen (filename_ir, "w")) == NULL)
    {
        perror ("fopen");
        fprintf ( stderr, "ERROR opening trace file: %s\n", filename_ir);
        return (-1);
    }

    /* write the file header */
    fprintf (fd_ir, "# This trace file contains the Instantaneous Reward measure\n");
    switch ( type )
    {
        case RATE_REWARD_TYPE :
            fprintf (fd_ir, "# of the Rate Reward named: %s\n", name);
            break;
        case IMPULSE_REWARD_TYPE :
            fprintf (fd_ir, "# of the Impulse Reward named: %s\n", name);
            break;
        case RATE_REWARD_SUM_TYPE :
            fprintf (fd_ir, "# of the Rate Reward Sum named: %s\n", name);
            break;
        case GLOBAL_RATE_REWARD_TYPE :
            fprintf (fd_ir, "# of the Global Rate Reward named: %s\n", name);
            break;
        case GLOBAL_IMPULSE_REWARD_TYPE :
            fprintf (fd_ir, "# of the Global Impulse Reward named: %s\n", name);
            break;
        default:
            fprintf (fd_ir, "# of the Reward named: %s\n", name);
            break;
    }
    fprintf (fd_ir, "# ------------------- first run ----------------------\n");
    fprintf (fd_ir, "#\n");
    fprintf (fd_ir, "0.0000000000E+00 %.10E\n",curr_ir_val);

    return (1);
}
//------------------------------------------------------------------------------
void Reward_Measure::close_trace_files()
{
   fclose (fd_cr);
   fclose (fd_ir);
}
//------------------------------------------------------------------------------
FILE *Reward_Measure::show_trace_fd_cr()
{
   return (fd_cr);
}
//------------------------------------------------------------------------------
FILE *Reward_Measure::show_trace_fd_ir()
{
   return (fd_ir);
}
//------------------------------------------------------------------------------
TGFLOAT Reward_Measure::show_cr_value()
{
  return (curr_cr_val);
}
//------------------------------------------------------------------------------
void Reward_Measure::ch_cr_value(TGFLOAT new_val)
{
   curr_cr_val = new_val;
}
//------------------------------------------------------------------------------
TGFLOAT Reward_Measure::show_ir_value()
{
  return (curr_ir_val);
}
//------------------------------------------------------------------------------
void Reward_Measure::ch_ir_value(TGFLOAT new_val)
{
   curr_ir_val = new_val;
}
//------------------------------------------------------------------------------
TGFLOAT Reward_Measure::show_avg_cr_value()
{
  return (avg_cr_val);
}
//------------------------------------------------------------------------------
void Reward_Measure::ch_avg_cr_value(TGFLOAT new_val)
{
   avg_cr_val = new_val;
}
//------------------------------------------------------------------------------
TGFLOAT Reward_Measure::show_time()
{
  return (time);
}
//------------------------------------------------------------------------------
void Reward_Measure::set_first_cr(TGFLOAT fst_cr)
{
  first_cr_val = fst_cr;
}
//------------------------------------------------------------------------------
void Reward_Measure::set_first_ir(TGFLOAT fst_ir)
{
  first_ir_val = fst_ir;
}
//------------------------------------------------------------------------------
void Reward_Measure::ch_time(TGFLOAT new_time)
{
   time = new_time;
}
//------------------------------------------------------------------------------
int Reward_Measure::show_has_level()
{
  return ( has_level );
}
//------------------------------------------------------------------------------
void Reward_Measure::ch_has_level(int a_has_level)
{
  has_level = a_has_level;
}
//------------------------------------------------------------------------------
TGFLOAT Reward_Measure::show_rew_level()
{
  return (rew_level);
}
//------------------------------------------------------------------------------
void Reward_Measure::ch_rew_level(TGFLOAT new_level)
{
   rew_level = new_level;
}
//------------------------------------------------------------------------------
TGFLOAT Reward_Measure::show_time_abv_lv()
{
  return (time_abv_lv);
}
//------------------------------------------------------------------------------
void Reward_Measure::ch_time_abv_lv(TGFLOAT new_time)
{
   time_abv_lv = new_time;
}
//------------------------------------------------------------------------------
TGFLOAT Reward_Measure::show_low_bound()
{
  return (low_bound);
}
//------------------------------------------------------------------------------
void Reward_Measure::ch_low_bound(TGFLOAT new_bound)
{
   low_bound = new_bound;
}
//------------------------------------------------------------------------------
TGFLOAT Reward_Measure::show_upp_bound()
{
  return (upp_bound);
}
//------------------------------------------------------------------------------
void Reward_Measure::ch_upp_bound(TGFLOAT new_bound)
{
   upp_bound = new_bound;
}
//------------------------------------------------------------------------------
int Reward_Measure::is_low_bounded()
{
  return (low_bounded);
}
//------------------------------------------------------------------------------
void Reward_Measure::ch_low_bounded(int bounded)
{
  low_bounded = bounded;
}
//------------------------------------------------------------------------------
int Reward_Measure::is_upp_bounded()
{
  return (upp_bounded);
}
//------------------------------------------------------------------------------
void Reward_Measure::ch_upp_bounded(int bounded)
{
  upp_bounded = bounded;
}
//------------------------------------------------------------------------------
Rate_Reward_Sum *Reward_Measure::show_rate_sum_bound()
{
    return (rate_sum_bound);
}
//------------------------------------------------------------------------------
void Reward_Measure::ch_rate_sum_bound( Rate_Reward_Sum *new_rate_rew_sum )
{
    rate_sum_bound = new_rate_rew_sum;
}
//------------------------------------------------------------------------------
int Reward_Measure::is_rate_sum_bounded()
{
    return (rate_sum_bounded);
}
//------------------------------------------------------------------------------
void Reward_Measure::ch_rate_sum_bounded(int new_val)
{
    rate_sum_bounded = new_val;
}
//------------------------------------------------------------------------------
RR_Event_List *Reward_Measure::show_RR_events_list(int access)
{
    RR_Event_List  *RR_list_aux;

    if (access == LIST_RW)
        return (RR_events_list);
    else
    {
        RR_list_aux = new RR_Event_List(RR_events_list);
        return (RR_list_aux);
    }
}
//------------------------------------------------------------------------------
void Reward_Measure::create_RR_events_list()
{
    RR_events_list = new RR_Event_List();
}
//------------------------------------------------------------------------------
int Reward_Measure::exists_RR_events_list()
{
    if ( RR_events_list != NULL )
    {
        return( TRUE );
    }
    else
    {
        return( FALSE );
    }
}
//------------------------------------------------------------------------------
void Reward_Measure::add_RR_event(RR_Event *new_RR_ev)
{
    if (new_RR_ev != NULL)
        RR_events_list->add_tail_rr_ev( new_RR_ev );
    else
        debug(3,"Reward_Measure::add_RR_event: Can't add NULL pointer");
}
//------------------------------------------------------------------------------
void Reward_Measure::clear_RR_events_list()
{
    RR_Event *RR_ev_aux;

    RR_ev_aux = RR_events_list->get_1st_rr_ev();
    while ( RR_ev_aux != NULL )
    {
        delete RR_ev_aux;
        RR_ev_aux = RR_events_list->get_1st_rr_ev();
    }
}
//------------------------------------------------------------------------------

#ifdef TG_PARALLEL
void Reward_Measure::send_context()
{
    int has_sum_bound;

    pvm_pkstr(name);
    pvm_pkint(&id,1,1);
    PVM_PKTGFLOAT(&time,1,1);
    PVM_PKTGFLOAT(&curr_ir_val,1,1);
    PVM_PKTGFLOAT(&curr_cr_val,1,1);
    PVM_PKTGFLOAT(&avg_cr_val,1,1);
    PVM_PKTGFLOAT(&rew_level,1,1);
    PVM_PKTGFLOAT(&time_abv_lv,1,1);
    pvm_pkint(&low_bounded,1,1);
    pvm_pkint(&upp_bounded,1,1);
    PVM_PKTGFLOAT(&low_bound,1,1);
    PVM_PKTGFLOAT(&upp_bound,1,1);
    pvm_pkint(&rate_sum_bounded,1,1);
    has_sum_bound = (rate_sum_bound != NULL);
    pvm_pkint(&has_sum_bound,1,1);
    if (has_sum_bound)
        rate_sum_bound->send_context();
    debug(4,"Reward_Measure::send_context()");
}

void Reward_Measure::recv_context()
{
    int has_sum_bound;

    pvm_upkstr(name);
    pvm_upkint(&id,1,1);
    PVM_UPKTGFLOAT(&time,1,1);
    PVM_UPKTGFLOAT(&curr_ir_val,1,1);
    PVM_UPKTGFLOAT(&curr_cr_val,1,1);
    PVM_UPKTGFLOAT(&avg_cr_val,1,1);
    PVM_UPKTGFLOAT(&rew_level,1,1);
    PVM_UPKTGFLOAT(&time_abv_lv,1,1);
    pvm_upkint(&low_bounded,1,1);
    pvm_upkint(&upp_bounded,1,1);
    PVM_UPKTGFLOAT(&low_bound,1,1);
    PVM_UPKTGFLOAT(&upp_bound,1,1);
    pvm_upkint(&rate_sum_bounded,1,1);
    pvm_upkint(&has_sum_bound,1,1);
    if (has_sum_bound)
    {
      if (rate_sum_bound == NULL)
        rate_sum_bound = new Rate_Reward_Sum();
      if (rate_sum_bound != NULL)
        rate_sum_bound->recv_context();
      else fprintf(stderr,"Out of memory in void Reward_Measure::recv_context()\n");
    }
    debug(4,"Reward_Measure::recv_context()");
}
#endif
/**********************************************************************/


Reward_Measure_List::Reward_Measure_List() : Chained_List()
{
  debug(4,"Reward_Measure_List::Reward_Measure_List(): creating object");
}

Reward_Measure_List::Reward_Measure_List(Reward_Measure_List *ev_list) : Chained_List( (Chained_List *)ev_list)
{
  debug(4,"Reward_Measure_List::Reward_Measure_List(Reward_Measure_List *): creating object");
}

Reward_Measure_List::~Reward_Measure_List()
{
  Reward_Measure *rew_mea_aux;

  if (access == LIST_RW) {
    rew_mea_aux = get_1st_rew();
    while (rew_mea_aux != NULL) {
      delete rew_mea_aux;
      rew_mea_aux = get_1st_rew();
    }
  }
  debug(4,"Reward_Measure_List::~Reward_Measure_List(): destroying object");
}
//------------------------------------------------------------------------------
Reward_Measure_List *Reward_Measure_List::duplicate()
{
    Reward_Measure_List  *rew_list_aux;
    Reward_Measure       *rew, *rew_aux;

    /* create an empty list */
    rew_list_aux = new Reward_Measure_List();
    /* copy all elements from this list */
    rew = show_1st_rew();
    while (rew != NULL)
    {
        rew_aux = new Reward_Measure(rew);
        rew_list_aux->add_tail_rew(rew_aux);
        rew = show_next_rew();
    }
    return (rew_list_aux);
}
//------------------------------------------------------------------------------
void Reward_Measure_List::divide_by(TGFLOAT divisor)
{
    Reward_Measure      *rew_meas;
    TGFLOAT               value;

    /* get the first reward */
    rew_meas = show_1st_rew();
    /* go through all the rewards in the list */
    while (rew_meas != NULL) {
      value = rew_meas->show_cr_value();
      /* divide by the divisor parameter */
      rew_meas->ch_cr_value(value/divisor);
      value = rew_meas->show_avg_cr_value();
      /* divide by the divisor parameter */
      rew_meas->ch_avg_cr_value(value/divisor);
      /* go to the next reward in the list */
      rew_meas = show_next_rew();
    }
}

int Reward_Measure_List::add_tail_rew(Reward_Measure *new_rew)
{
  return ( add_tail( (Chained_Element *)new_rew) );
}

Reward_Measure *Reward_Measure_List::show_1st_rew()
{
  return ( (Reward_Measure *)show_head() );
}

Reward_Measure *Reward_Measure_List::get_1st_rew()
{
  return ( (Reward_Measure *)del_head() );
}

Reward_Measure *Reward_Measure_List::show_next_rew()
{
  return ( (Reward_Measure *)show_next() );
}

Reward_Measure *Reward_Measure_List::show_curr_rew()
{
  return ( (Reward_Measure *)show_curr() );
}

int Reward_Measure_List::query_rew (char *tg_rew_name)
{
  Reward_Measure *rew_meas;
  char            rew_meas_name[MAXSTRING];

  rew_meas = show_1st_rew();
  while (rew_meas != NULL){
    rew_meas->show_name(rew_meas_name);
    if (!strcmp(rew_meas_name, tg_rew_name))
      return(1);
    else
      rew_meas = show_next_rew();
  }
  return(0);
}

int Reward_Measure_List::query_rew (int tg_rew_id)
{
  Reward_Measure *rew_meas;

  rew_meas = show_1st_rew();
  while (rew_meas != NULL){
    if (rew_meas->show_id() == tg_rew_id)
      return(1);
    else
      rew_meas = show_next_rew();
  }
  return(0);
}
//------------------------------------------------------------------------------
int Reward_Measure_List::show_no_rew()
{
  return ( show_no_elem() );
}
//------------------------------------------------------------------------------
void Reward_Measure_List::print_rew(FILE *fd)
{
    Reward_Measure   *rew_meas;
    char              rew_name[MAXSTRING];

    rew_meas = show_1st_rew();
    /* print all the rate rewards in the list */
    while (rew_meas != NULL)
    {
        rew_meas->show_name(rew_name);
        if (fd != NULL)
        {
            fprintf ( fd, " %s\n   - C(t) = %.10E\n   - TC(t) = %.10E\n",
            rew_name,rew_meas->show_cr_value(), rew_meas->show_avg_cr_value());
            if ( rew_meas->show_has_level() == TRUE )
            {
                fprintf ( fd, "   - time above reward level:\n");
                fprintf ( fd, "      - level = %.5E\n", rew_meas->show_rew_level() );
                fprintf ( fd, "      - time  = %.10E\n", rew_meas->show_time_abv_lv() );
            }
        }
        else
        {
            fprintf ( stdout, " %s\n   - C(t) = %.10E\n   - TC(t) = %.10E\n",
            rew_name,rew_meas->show_cr_value(), rew_meas->show_avg_cr_value());
            if ( rew_meas->show_has_level() == TRUE )
            {
                fprintf ( stdout, "   - time above reward level:\n");
                fprintf ( stdout, "      - level = %.5E\n", rew_meas->show_rew_level() );
                fprintf ( stdout, "      - time  = %.10E\n", rew_meas->show_time_abv_lv() );
            }
        }
        rew_meas = show_next_rew();
    }
}
//------------------------------------------------------------------------------
#ifdef TG_PARALLEL
void Reward_Measure_List::send_context()
{
    Reward_Measure   *rew_meas;
    int               num_elem = show_no_elem();

    pvm_pkint(&num_elem,1,1);
    rew_meas = show_1st_rew();
    while (rew_meas != NULL)
    {
        rew_meas->send_context();
        rew_meas = show_next_rew();
    }
}


void Reward_Measure_List::recv_context()
{
    int              num_elem;
    Reward_Measure   *rew_meas;

    pvm_upkint(&num_elem,1,1);
    while (num_elem-- > 0)
    {
        rew_meas = new Reward_Measure("",0); // Nothing to do, it will fill up things in recv_context()
        if (rew_meas == NULL)
        {
           fprintf ( stderr, "Out of memory in int Reward_Sum_List::recv_context()\n");
           break;
        }
        rew_meas->recv_context();
        add_tail_rew(rew_meas);
    }
}
#endif

//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
//  RR_Event  <>  class used when this Reward_MEasure is analized by
//                a REWARD_REACHED event ype
//------------------------------------------------------------------------------
RR_Event::RR_Event( Event *event_pointer, RR_Touch *rr_touch_pointer) : Chained_Element()
{
    event = event_pointer;
    twin_rr_touch = rr_touch_pointer;
    debug(4,"RR_Event::RR_Event(Event *): creating object");
}
//------------------------------------------------------------------------------
RR_Event::RR_Event( RR_Event *a_event ) : Chained_Element()
{
    event = a_event->show_event();
    twin_rr_touch = a_event->show_twin_rr_touch();
    debug(4,"RR_Event::RR_Event(Event *): creating object");
}
//------------------------------------------------------------------------------
RR_Event::~RR_Event()
{
    debug(4,"RR_Event::RR_Event(): destroying object");
}
//------------------------------------------------------------------------------
//     M E T H O D S
//------------------------------------------------------------------------------
Event *RR_Event::show_event()
{
    return ( event );
}
//------------------------------------------------------------------------------
void RR_Event::ch_event(Event *new_event)
{
    event = new_event;
}
//------------------------------------------------------------------------------
RR_Touch *RR_Event::show_twin_rr_touch()
{
    return ( twin_rr_touch );
}
//------------------------------------------------------------------------------
void RR_Event::ch_twin_rr_touch(RR_Touch *new_rr_touch)
{
    twin_rr_touch = new_rr_touch;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R   ||||   L I S T
//------------------------------------------------------------------------------
RR_Event_List::RR_Event_List() : Chained_List()
{
}
//------------------------------------------------------------------------------
RR_Event_List::RR_Event_List(RR_Event_List *RR_ev_list) :
               Chained_List( (Chained_List *)RR_ev_list )
{
}
//------------------------------------------------------------------------------
RR_Event_List::~RR_Event_List()
{
    RR_Event *RR_ev_aux;

    if (access == LIST_RW)
        while (!is_empty())
        {
            RR_ev_aux = get_1st_rr_ev();
            delete RR_ev_aux;
        }
}
//------------------------------------------------------------------------------
//     M E T H O D S   |||   L I S T
//------------------------------------------------------------------------------
int RR_Event_List::add_tail_rr_ev(RR_Event *new_RR_event)
{
    return ( add_tail( (Chained_Element *)new_RR_event) );
}
//------------------------------------------------------------------------------
RR_Event *RR_Event_List::show_1st_rr_ev()
{
    return ( (RR_Event *)show_head() );
}
//------------------------------------------------------------------------------
RR_Event *RR_Event_List::get_1st_rr_ev()
{
    return ( (RR_Event *) del_head() );
}
//------------------------------------------------------------------------------
RR_Event *RR_Event_List::get_curr_rr_ev()
{
    return ( (RR_Event *) del_elem() );
}
//------------------------------------------------------------------------------
RR_Event *RR_Event_List::show_next_rr_ev()
{
    return ( (RR_Event *)show_next() );
}
//------------------------------------------------------------------------------
RR_Event *RR_Event_List::show_curr_rr_ev()
{
    return ( (RR_Event *)show_curr() );
}
//------------------------------------------------------------------------------
/*void RR_Event_List::ch_need_eval(int needs)
{
    RR_Event          *RR_ev_aux;
    Rew_Reach_Distrib *rew_reach_dist = NULL;

    RR_ev_aux = show_1st_RR_ev();
    while (RR_ev_aux != NULL)
    {
        rew_reach_dist = (Rew_Reach_Distrib *)RR_ev_aux->show_event()->show_distrib();
        rew_reach_dist->ch_need_evaluation( needs );
        RR_ev_aux = show_next_RR_ev();
    }
} */
//------------------------------------------------------------------------------
RR_Event_List *RR_Event_List::duplicate()
{
    RR_Event_List  *rr_ev_list_aux;
    RR_Event       *rr_ev, *rr_ev_aux;

    /* create an empty list */
    rr_ev_list_aux = new RR_Event_List();
    /* copy all elements from this list */
    rr_ev = show_1st_rr_ev();
    while (rr_ev != NULL) {
      rr_ev_aux = new RR_Event( rr_ev );
      rr_ev_list_aux->add_tail_rr_ev(rr_ev_aux);
      rr_ev = show_next_rr_ev();
    }
    return (rr_ev_list_aux);
}
//------------------------------------------------------------------------------
int RR_Event_List::show_no_rr_ev()
{
    return ( show_no_elem() );
}
//------------------------------------------------------------------------------


