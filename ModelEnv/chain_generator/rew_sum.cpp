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
*  @file rew_sum.cpp
*  @brief <b>Reward sum</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  The detailed description is unavailable. 
*/

#include <iostream>

using namespace std;

#include <stdio.h>
#include <string.h>
#ifdef TG_PARALLEL
#include <pvm3.h>
#endif
#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"
#include "system_desc.h"

/** The global base filename of the model */
extern char                base_filename[MAXSTRING];

//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
Reward_Sum::Reward_Sum(int a_id, char *a_rew_name) : Chained_Element()
{
    rew_id  = a_id;
    strcpy( rew_name, a_rew_name );
    local_bound_reached = FALSE;
    cross_time = 0;
    virtual_ir = 0;
    rew_measure = NULL;
}
//------------------------------------------------------------------------------
Reward_Sum::Reward_Sum() : Chained_Element()
{
    rew_id  = 0;
    rew_name[0]=0;
    local_bound_reached = FALSE;
    cross_time = 0;
    virtual_ir = 0;
    rew_measure = NULL;
}
//------------------------------------------------------------------------------
Reward_Sum::Reward_Sum(Reward_Sum *rew_sum) : Chained_Element()
{
    rew_id  = rew_sum->show_rew_id();
    rew_sum->show_rew_name( rew_name );
    local_bound_reached = rew_sum->is_local_bound_reached();
    cross_time = rew_sum->show_cross_time();
    virtual_ir = rew_sum->show_virtual_ir();
    rew_measure = rew_sum->show_rew_measure();
}
//------------------------------------------------------------------------------
Reward_Sum::~Reward_Sum()
{
}
//------------------------------------------------------------------------------
//     M E T H O D S
//------------------------------------------------------------------------------
int Reward_Sum::show_rew_id()
{
    return(rew_id);
}
//------------------------------------------------------------------------------
void Reward_Sum::ch_rew_id(int new_rew)
{
    rew_id = new_rew;
}
//------------------------------------------------------------------------------
void Reward_Sum::show_rew_name(char *a_name)
{
    strcpy(a_name, rew_name);
}
//------------------------------------------------------------------------------
TGFLOAT Reward_Sum::show_cross_time()
{
    return( cross_time );
}
//------------------------------------------------------------------------------
void Reward_Sum::ch_cross_time(TGFLOAT new_cross_time)
{
    cross_time = new_cross_time;
}
//------------------------------------------------------------------------------
TGFLOAT Reward_Sum::show_virtual_ir()
{
    return( virtual_ir );
}
//------------------------------------------------------------------------------
void Reward_Sum::ch_virtual_ir(TGFLOAT new_virtual_ir)
{
    virtual_ir = new_virtual_ir;
}
//------------------------------------------------------------------------------
int Reward_Sum::is_local_bound_reached()
{
    return( local_bound_reached );
}
//------------------------------------------------------------------------------
void Reward_Sum::ch_local_bound_reached(int new_val)
{
    local_bound_reached = new_val;
}
//------------------------------------------------------------------------------
Reward_Measure *Reward_Sum::show_rew_measure()
{
    return( rew_measure );
}
//------------------------------------------------------------------------------
void Reward_Sum::ch_rew_measure(Reward_Measure *new_rew_meas)
{
    rew_measure = new_rew_meas;
}
//------------------------------------------------------------------------------
#ifdef TG_PARALLEL
void Reward_Sum::send_context()
{
    pvm_pkstr(rew_name);
    pvm_pkint(&rew_id,1,1);
    pvm_pkint(&local_bound_reached,1,1);
    PVM_PKTGFLOAT(&cross_time,1,1);
    PVM_PKTGFLOAT(&virtual_ir,1,1);
    //DO NOT UNCOMMENT rew_measure->send_measure();
}
//------------------------------------------------------------------------------
void Reward_Sum::recv_context()
{
    pvm_upkstr(rew_name);
    pvm_upkint(&rew_id,1,1);
    pvm_upkint(&local_bound_reached,1,1);
    PVM_UPKTGFLOAT(&cross_time,1,1);
    PVM_UPKTGFLOAT(&virtual_ir,1,1);
    //DO NOT UNCOMMENT rew_measure->recv_measure();
}
#endif
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R   ||||   L I S T
//------------------------------------------------------------------------------
Reward_Sum_List::Reward_Sum_List() : Chained_List()
{
}
//------------------------------------------------------------------------------
Reward_Sum_List::Reward_Sum_List(Reward_Sum_List *rew_sum_list) :
                 Chained_List( (Chained_List *)rew_sum_list )
{
}
//------------------------------------------------------------------------------
Reward_Sum_List::~Reward_Sum_List()
{
    Reward_Sum *rew_aux;

    if (access == LIST_RW)
        while (!is_empty())
        {
            rew_aux = get_1st_rew();
            delete rew_aux;
        }
}
//------------------------------------------------------------------------------
//     M E T H O D S   |||   L I S T
//------------------------------------------------------------------------------
int Reward_Sum_List::add_tail_rew(Reward_Sum *new_rew)
{
    return ( add_tail( (Chained_Element *)new_rew) );
}
//------------------------------------------------------------------------------
Reward_Sum *Reward_Sum_List::show_1st_rew()
{
    return ( (Reward_Sum *)show_head() );
}
//------------------------------------------------------------------------------
Reward_Sum *Reward_Sum_List::get_1st_rew()
{
    return ( (Reward_Sum *) del_head() );
}
//------------------------------------------------------------------------------
Reward_Sum *Reward_Sum_List::show_next_rew()
{
    return ( (Reward_Sum *)show_next() );
}
//------------------------------------------------------------------------------
Reward_Sum *Reward_Sum_List::show_curr_rew()
{
    return ( (Reward_Sum *)show_curr() );
}
//------------------------------------------------------------------------------
int Reward_Sum_List::query_rew(int tg_rew_id)
{
    Reward_Sum *rew_aux;

    rew_aux = show_1st_rew();
    while (rew_aux != NULL)
    {
        if ( rew_aux->show_rew_id() == tg_rew_id)
            return (1);
        rew_aux = show_next_rew();
    }
    return (0);
}
//------------------------------------------------------------------------------
int Reward_Sum_List::show_no_rew()
{
    return ( show_no_elem() );
}
//------------------------------------------------------------------------------
#ifdef TG_PARALLEL
void Reward_Sum_List::send_context()
{
    int         num_elem;
    Reward_Sum *rs= show_1st_rew();

    num_elem = show_no_rew();
    pvm_pkint(&num_elem,1,1);

    while (rs != NULL)
    {
        fprintf ( stderr, "void Reward_Sum_List::send_context()::pvm_pkint(&num_elem,1,1); %d\n",num_elem);
        rs->send_context();
        rs = show_next_rew();
    }
}
//------------------------------------------------------------------------------
void Reward_Sum_List::recv_context()
{
    int num_elem;
    Reward_Sum * rs;

    pvm_upkint(&num_elem,1,1);

    while (num_elem-- > 0)
    {
        rs = new Reward_Sum(0, "");
        if (rs == NULL)
        {
           fprintf ( stderr, "Out of memory in int Reward_Sum_List::recv_context()\n");
           break;
        }
        fprintf( stderr, "void Reward_Sum_List::recv_context()::pvm_upkint(&num_elem,1,1); %d\n",num_elem);
        rs->recv_context();
        add_tail_rew(rs);
    }
}
#endif
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
Rate_Reward_Sum::Rate_Reward_Sum( char *a_rew_name, char *a_obj_name, int a_id ) : Chained_Element()
{
    id   = a_id;
    strcpy( obj_name, a_obj_name );
    strcpy( name, a_rew_name );
    low_bounded = FALSE;
    upp_bounded = FALSE;
    low_bound = 0;
    upp_bound = 0;
    fd_cr = NULL;
    fd_ir = NULL;
    cr_initial_value = 0;
    rew_measure = NULL;
    /* create an empty reward sum list */
    rew_sum_list = new Reward_Sum_List();

}
//------------------------------------------------------------------------------
Rate_Reward_Sum::Rate_Reward_Sum( ) : Chained_Element()
{
    id   = 0;
    obj_name[0]=0;
    name[0]=0;
    low_bounded = FALSE;
    upp_bounded = FALSE;
    low_bound = 0;
    upp_bound = 0;
    cr_initial_value = 0;
    rew_measure = NULL;
    /* create an empty reward sum list */
    rew_sum_list = new Reward_Sum_List();

}
//------------------------------------------------------------------------------
Rate_Reward_Sum::Rate_Reward_Sum( Rate_Reward_Sum *rate_rew_sum ) : Chained_Element()
{
    Reward_Sum_List *re_sum_list;
    Reward_Sum      *re_sum, *re_sum_aux;

    /* copy all direct attributes */
    id   = rate_rew_sum->show_id();
    rate_rew_sum->show_name( name );
    rate_rew_sum->show_obj_name( obj_name );
    low_bounded = rate_rew_sum->is_low_bounded();
    upp_bounded = rate_rew_sum->is_upp_bounded();
    low_bound = rate_rew_sum->show_low_bound();
    upp_bound = rate_rew_sum->show_upp_bound();
    cr_initial_value = rate_rew_sum->show_cr_initial_value();
    rew_measure = rate_rew_sum->show_rew_measure();

    /* create an empty reward sum list and copy the elements */
    rew_sum_list = new Reward_Sum_List();
    re_sum_list = rate_rew_sum->show_rew_sum_list(LIST_RO);
    re_sum = re_sum_list->show_1st_rew();
    while ( re_sum != NULL)
    {
        re_sum_aux = new Reward_Sum( re_sum );
        rew_sum_list->add_tail_rew( re_sum_aux );
        re_sum = re_sum_list->show_next_rew();
    }
    delete re_sum_list;
}
//------------------------------------------------------------------------------
Rate_Reward_Sum::~Rate_Reward_Sum()
{
}
//------------------------------------------------------------------------------
//     M E T H O D S
//------------------------------------------------------------------------------
int Rate_Reward_Sum::show_id()
{
    return(id);
}
//------------------------------------------------------------------------------
void Rate_Reward_Sum::show_name(char *a_name)
{
    strcpy(a_name, name);
}
//------------------------------------------------------------------------------
void Rate_Reward_Sum::show_obj_name(char *a_name)
{
    strcpy(a_name, obj_name);
}
//------------------------------------------------------------------------------
TGFLOAT Rate_Reward_Sum::show_low_bound()
{
    return(low_bound);
}
//------------------------------------------------------------------------------
void Rate_Reward_Sum::ch_low_bound(TGFLOAT new_bound)
{
    low_bound = new_bound;
}
//------------------------------------------------------------------------------
TGFLOAT Rate_Reward_Sum::show_upp_bound()
{
    return (upp_bound);
}
//------------------------------------------------------------------------------
void Rate_Reward_Sum::ch_upp_bound(TGFLOAT new_bound)
{
    upp_bound = new_bound;
}
//------------------------------------------------------------------------------
int Rate_Reward_Sum::is_low_bounded()
{
    return (low_bounded);
}
//------------------------------------------------------------------------------
void Rate_Reward_Sum::ch_low_bounded(int bounded)
{
    low_bounded = bounded;
}
//------------------------------------------------------------------------------
int Rate_Reward_Sum::is_upp_bounded()
{
    return (upp_bounded);
}
//------------------------------------------------------------------------------
void Rate_Reward_Sum::ch_upp_bounded(int bounded)
{
    upp_bounded = bounded;
}
//------------------------------------------------------------------------------
int Rate_Reward_Sum::has_treated()
{
    return (treated);
}
//------------------------------------------------------------------------------
void Rate_Reward_Sum::ch_treated(int a_treated)
{
    treated = a_treated;
}
//------------------------------------------------------------------------------
TGFLOAT Rate_Reward_Sum::show_cross_time()
{
    return (cross_time);
}
//------------------------------------------------------------------------------
void Rate_Reward_Sum::ch_cross_time(TGFLOAT new_val)
{
    cross_time = new_val;
}
//------------------------------------------------------------------------------
void Rate_Reward_Sum::add_rew_sum(Reward_Sum *new_rew_sum)
{
    if (new_rew_sum != NULL)
        rew_sum_list->add_tail_rew( new_rew_sum );
    else
        debug(3,"Rate_Reward_Sum::add_rew_sum: Can't add NULL pointer");
}
//------------------------------------------------------------------------------
Reward_Sum_List *Rate_Reward_Sum::show_rew_sum_list(int access)
{
    Reward_Sum_List  *rew_sum_list_aux;
    if (access == LIST_RW)
        return (rew_sum_list);
    else
    {
        rew_sum_list_aux = new Reward_Sum_List(rew_sum_list);
        return (rew_sum_list_aux);
    }
}
//------------------------------------------------------------------------------
TGFLOAT Rate_Reward_Sum::show_cr_initial_value()
{
  return (cr_initial_value);
}
//------------------------------------------------------------------------------
void Rate_Reward_Sum::ch_cr_initial_value(TGFLOAT new_value)
{
   cr_initial_value = new_value;
}
//------------------------------------------------------------------------------
void Rate_Reward_Sum::ch_rew_measure(Reward_Measure *a_rew_meas)
{
    rew_measure = a_rew_meas;
}
//------------------------------------------------------------------------------
Reward_Measure *Rate_Reward_Sum::show_rew_measure()
{
    return ( rew_measure );
}
//------------------------------------------------------------------------------
#ifdef TG_PARALLEL
void Rate_Reward_Sum::send_context()
{
    int has_rew_sum_list;

    pvm_pkint(&id,1,1);
    pvm_pkstr(name);
    pvm_pkstr(obj_name);
    pvm_pkint(&low_bounded,1,1);
    pvm_pkint(&upp_bounded,1,1);
    PVM_PKTGFLOAT(&low_bound,1,1);
    PVM_PKTGFLOAT(&upp_bound,1,1);
    pvm_pkint(&treated,1,1);
    PVM_PKTGFLOAT(&cross_time,1,1);
    has_rew_sum_list = (rew_sum_list != NULL);
    pvm_pkint(&has_rew_sum_list,1,1);
    if (has_rew_sum_list)
      rew_sum_list->send_context();
    fprintf ( stderr, "void Rate_Reward_Sum::send_context()::pvm_pkint(&has_rew_sum_list,1,1); %d\n",has_rew_sum_list);
}
//------------------------------------------------------------------------------
void Rate_Reward_Sum::recv_context()
{
    int has_rew_sum_list;

    pvm_upkint(&id,1,1);
    pvm_upkstr(name);
    pvm_upkstr(obj_name);
    pvm_upkint(&low_bounded,1,1);
    pvm_upkint(&upp_bounded,1,1);
    PVM_UPKTGFLOAT(&low_bound,1,1);
    PVM_UPKTGFLOAT(&upp_bound,1,1);
    pvm_upkint(&treated,1,1);
    PVM_UPKTGFLOAT(&cross_time,1,1);
    pvm_upkint(&has_rew_sum_list,1,1);
    fprintf ( stderr, "void Rate_Reward_Sum::recv_context()::pvm_upkint(&has_rew_sum_list,1,1); %d\n",has_rew_sum_list);
    if (has_rew_sum_list)
    {
      if (rew_sum_list == NULL) rew_sum_list = new Reward_Sum_List();
      if (rew_sum_list != NULL)
        rew_sum_list->recv_context();
      else fprintf ( stderr, "Out of memory in void Rate_Reward_Sum::recv_context()\n");
    }
}
#endif
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R   ||||   L I S T
//------------------------------------------------------------------------------
Rate_Reward_Sum_List::Rate_Reward_Sum_List() : Chained_List()
{
}
//------------------------------------------------------------------------------
Rate_Reward_Sum_List::Rate_Reward_Sum_List(Rate_Reward_Sum_List *rate_rew_sum_list) :
                 Chained_List( (Chained_List *)rate_rew_sum_list )
{
}
//------------------------------------------------------------------------------
Rate_Reward_Sum_List::~Rate_Reward_Sum_List()
{
    Rate_Reward_Sum *rew_aux;

    if (access == LIST_RW)
        while (!is_empty())
        {
            rew_aux = get_1st_rew();
            delete rew_aux;
        }
}
//------------------------------------------------------------------------------
//     M E T H O D S   |||   L I S T
//------------------------------------------------------------------------------
int Rate_Reward_Sum_List::add_tail_rew(Rate_Reward_Sum *new_rew)
{
    return ( add_tail( (Chained_Element *)new_rew) );
}
//------------------------------------------------------------------------------
Rate_Reward_Sum *Rate_Reward_Sum_List::show_1st_rew()
{
    return ( (Rate_Reward_Sum *)show_head() );
}
//------------------------------------------------------------------------------
Rate_Reward_Sum *Rate_Reward_Sum_List::get_1st_rew()
{
    return ( (Rate_Reward_Sum *) del_head() );
}
//------------------------------------------------------------------------------
Rate_Reward_Sum *Rate_Reward_Sum_List::show_next_rew()
{
    return ( (Rate_Reward_Sum *)show_next() );
}
//------------------------------------------------------------------------------
Rate_Reward_Sum *Rate_Reward_Sum_List::show_curr_rew()
{
    return ( (Rate_Reward_Sum *)show_curr() );
}
//------------------------------------------------------------------------------
int Rate_Reward_Sum_List::query_rew(int tg_rew_id)
{
    Rate_Reward_Sum *rew_aux;

    rew_aux = show_1st_rew();
    while (rew_aux != NULL)
    {
        if ( rew_aux->show_id() == tg_rew_id)
        return (1);
        rew_aux = show_next_rew();
    }
    return (0);
}
//-----------------------------------------------------------------------------
int Rate_Reward_Sum_List::query_rew(char *tg_rew_sum_name)
{
    Rate_Reward_Sum *rew_sum_aux;
    char              name[MAXSTRING];

    rew_sum_aux = show_1st_rew();
    while (rew_sum_aux != NULL)
    {
        rew_sum_aux->show_name(name);
        if ( !strcmp(tg_rew_sum_name, name) )
	    return (1);
        rew_sum_aux = show_next_rew();
    }
    return (0);
}
//-----------------------------------------------------------------------------
int Rate_Reward_Sum_List::query_rew(char *tg_obj_name, char *tg_rew_sum_name)
{
    Rate_Reward_Sum *rew_sum_aux;
    char             name[MAXSTRING],obj_name[MAXSTRING];

    rew_sum_aux = show_1st_rew();
    while (rew_sum_aux != NULL)
    {
        rew_sum_aux->show_name(name);
        rew_sum_aux->show_obj_name(obj_name);
        if ( (!strcmp(tg_rew_sum_name, name)) && (!strcmp(tg_obj_name, obj_name)) )
	      return (1);
        rew_sum_aux = show_next_rew();
    }
    return (0);
}
//------------------------------------------------------------------------------
int Rate_Reward_Sum_List::show_no_rew()
{
    return ( show_no_elem() );
}
//------------------------------------------------------------------------------

