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
*  @file simul_result.cpp
*  @brief <b>Simulator_Result methods</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  The detailed description is unavailable. 
*/

#include <stdio.h>
#ifdef TG_PARALLEL
#include <pvm3.h>
#endif
#include <string.h>
#include <stdlib.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "random.h"
#include "simulator.h"
/* output file */
extern FILE *output_file;

//------------------------------------------------------------------------------
Simulator_Result::Simulator_Result(char *a_name)
{
    cr_mean  = cr_var  = cr_inter  = 0;
    acr_mean = acr_var = acr_inter = 0;
    atc_mean = atc_var = atc_inter = 0;
    tbl_mean = tbl_var = tbl_inter = 0;
    has_level = FALSE;

    if (a_name != NULL)
      strcpy (name, a_name);
    else
      strcpy (name, "");
}
//------------------------------------------------------------------------------
Simulator_Result::~Simulator_Result()
{
}
//------------------------------------------------------------------------------
void Simulator_Result::show_name(char *a_name)
{
   if (a_name != NULL)
     strcpy (a_name, name);
   else
     debug(3, "Simulator_Result::show_name: Can't copy name to a NULL pointer");
}
//------------------------------------------------------------------------------
TGFLOAT Simulator_Result::show_cr_mean()
{
   return (cr_mean);
}
//------------------------------------------------------------------------------
void Simulator_Result::ch_cr_mean(TGFLOAT new_value)
{
   cr_mean = new_value;
}
//------------------------------------------------------------------------------
TGFLOAT Simulator_Result::show_cr_var()
{
   return (cr_var);
}
//------------------------------------------------------------------------------
void Simulator_Result::ch_cr_inter(TGFLOAT new_value)
{
   cr_inter = new_value;
}
//------------------------------------------------------------------------------
TGFLOAT Simulator_Result::show_cr_inter()
{
   return (cr_inter);
}
//------------------------------------------------------------------------------
void Simulator_Result::ch_cr_var(TGFLOAT new_value)
{
   cr_var = new_value;
}
//------------------------------------------------------------------------------
TGFLOAT Simulator_Result::show_acr_mean()
{
   return (acr_mean);
}
//------------------------------------------------------------------------------
void Simulator_Result::ch_acr_mean(TGFLOAT new_value)
{
   acr_mean = new_value;
}
//------------------------------------------------------------------------------
TGFLOAT Simulator_Result::show_acr_var()
{
   return (acr_var);
}
//------------------------------------------------------------------------------
void Simulator_Result::ch_acr_var(TGFLOAT new_value)
{
   acr_var = new_value;
}
//------------------------------------------------------------------------------
TGFLOAT Simulator_Result::show_acr_inter()
{
   return (acr_inter);
}
//------------------------------------------------------------------------------
void Simulator_Result::ch_acr_inter(TGFLOAT new_value)
{
   acr_inter = new_value;
}
//------------------------------------------------------------------------------
TGFLOAT Simulator_Result::show_atc_mean()
{
   return (atc_mean);
}
//------------------------------------------------------------------------------
void Simulator_Result::ch_atc_mean(TGFLOAT new_value)
{
   atc_mean = new_value;
}
//------------------------------------------------------------------------------
TGFLOAT Simulator_Result::show_atc_var()
{
   return (atc_var);
}
//------------------------------------------------------------------------------
void Simulator_Result::ch_atc_var(TGFLOAT new_value)
{
   atc_var = new_value;
}
//------------------------------------------------------------------------------
TGFLOAT Simulator_Result::show_atc_inter()
{
   return (atc_inter);
}
//------------------------------------------------------------------------------
void Simulator_Result::ch_atc_inter(TGFLOAT new_value)
{
   atc_inter = new_value;
}
//------------------------------------------------------------------------------
TGFLOAT Simulator_Result::show_tbl_mean()
{
   return (tbl_mean);
}
//------------------------------------------------------------------------------
void Simulator_Result::ch_tbl_mean(TGFLOAT new_value)
{
   tbl_mean = new_value;
}
//------------------------------------------------------------------------------
TGFLOAT Simulator_Result::show_tbl_var()
{
   return (tbl_var);
}
//------------------------------------------------------------------------------
void Simulator_Result::ch_tbl_var(TGFLOAT new_value)
{
   tbl_var = new_value;
}
//------------------------------------------------------------------------------
TGFLOAT Simulator_Result::show_tbl_inter()
{
   return (tbl_inter);
}
//------------------------------------------------------------------------------
void Simulator_Result::ch_tbl_inter(TGFLOAT new_value)
{
   tbl_inter = new_value;
}
//------------------------------------------------------------------------------
int Simulator_Result::show_has_level()
{
   return ( has_level );
}
//------------------------------------------------------------------------------
void Simulator_Result::ch_has_level( int a_has_level)
{
   has_level = a_has_level;
}
//------------------------------------------------------------------------------
void Simulator_Result::print()
{
    if ( output_file == NULL )
    {
        fprintf( stderr,"Error: Output file is not defined!\n" );
        exit(0);
    }

    fprintf( output_file, "\n Measure: %s\n", name);

    fprintf( output_file, "   - CR(t) : - mean = %.10e\n", cr_mean );
    fprintf( output_file, "             - var  = %.10e\n", cr_var );
    fprintf( output_file, "             - interval = %c %.8e\n",177,cr_inter );
    fprintf( output_file, "               [%.8e , %.8e]\n", cr_mean - cr_inter, cr_mean + cr_inter );

    fprintf( output_file, "   - ACR(t): - mean = %.10e\n", acr_mean );
    fprintf( output_file, "             - var  = %.10e\n", acr_var );
    fprintf( output_file, "             - interval = %c %.8e\n",177, acr_inter );
    fprintf( output_file, "               [%.8e , %.8e]\n", acr_mean - acr_inter, acr_mean + acr_inter );

    fprintf( output_file, "   - ATC(t): - mean = %.10e\n", atc_mean );
    fprintf( output_file, "             - var  = %.10e\n", atc_var );
    fprintf( output_file, "             - interval = %c %.8e\n", 177, atc_inter );
    fprintf( output_file, "               [%.8e , %.8e]\n", atc_mean - atc_inter, atc_mean + atc_inter );

    if ( has_level == TRUE )
    {
        fprintf( output_file, "   - Fraction of time above level:\n" );
        fprintf( output_file, "             - mean = %.10e\n", tbl_mean );
        fprintf( output_file, "             - var  = %.10e\n", tbl_var );
        fprintf( output_file, "             - interval = %c %.8e\n", 177, tbl_inter );
        fprintf( output_file, "               [%.8e , %.8e]\n", tbl_mean - tbl_inter, tbl_mean + tbl_inter );
    }
}
//------------------------------------------------------------------------------
#ifdef TG_PARALLEL
void Simulator_Result::send_context()
{
    pvm_pkstr(name);
    PVM_PKTGFLOAT(&cr_mean,1,1);
    PVM_PKTGFLOAT(&cr_var,1,1);
    PVM_PKTGFLOAT(&cr_inter,1,1);
    PVM_PKTGFLOAT(&acr_mean,1,1);
    PVM_PKTGFLOAT(&acr_var,1,1);
    PVM_PKTGFLOAT(&acr_inter,1,1);
    PVM_PKTGFLOAT(&atc_mean,1,1);
    PVM_PKTGFLOAT(&atc_var,1,1);
    PVM_PKTGFLOAT(&atc_inter,1,1);
    PVM_PKTGFLOAT(&tbl_mean,1,1);
    PVM_PKTGFLOAT(&tbl_var,1,1);
    PVM_PKTGFLOAT(&tbl_inter,1,1);
}
//------------------------------------------------------------------------------
void Simulator_Result::recv_context()
{
    pvm_upkstr(name);
    PVM_UPKTGFLOAT(&cr_mean,1,1);
    PVM_UPKTGFLOAT(&cr_var,1,1);
    PVM_UPKTGFLOAT(&cr_inter,1,1);
    PVM_UPKTGFLOAT(&acr_mean,1,1);
    PVM_UPKTGFLOAT(&acr_var,1,1);
    PVM_UPKTGFLOAT(&acr_inter,1,1);
    PVM_UPKTGFLOAT(&atc_mean,1,1);
    PVM_UPKTGFLOAT(&atc_var,1,1);
    PVM_UPKTGFLOAT(&atc_inter,1,1);
    PVM_UPKTGFLOAT(&tbl_mean,1,1);
    PVM_UPKTGFLOAT(&tbl_var,1,1);
    PVM_UPKTGFLOAT(&tbl_inter,1,1);
}
#endif
//------------------------------------------------------------------------------


/************************************************************************/

Simulator_Result_List::Simulator_Result_List() : Chained_List()
{
   debug(4,"Simulator_Result_List::Simulator_Result_List(): creating object");
}

Simulator_Result_List::Simulator_Result_List(Simulator_Result_List *simul_result_list) : Chained_List( (Chained_List *)simul_result_list)
{

   debug(4,"Simulator_Result_List::Simulator_Result_List(Simulator_Result_List *): creating object");
}

Simulator_Result_List::~Simulator_Result_List()
{
  Simulator_Result *simul_result_aux;

  if (access == LIST_RW) {
    simul_result_aux = get_1st_result();
    while (simul_result_aux != NULL) {
      delete simul_result_aux;
      simul_result_aux = get_1st_result();
    }
  }

   debug(4,"Simulator_Result_List::~Simulator_Result_List(): destroying object");
}

int Simulator_Result_List::add_tail_result(Simulator_Result *new_result)
{
  return ( add_tail( (Chained_Element *)new_result) );
}

Simulator_Result *Simulator_Result_List::show_1st_result()
{
  return ( (Simulator_Result *)show_head() );
}

Simulator_Result *Simulator_Result_List::get_1st_result()
{
  return ( (Simulator_Result *)del_head() );
}

Simulator_Result *Simulator_Result_List::show_next_result()
{
  return ( (Simulator_Result *)show_next() );
}

Simulator_Result *Simulator_Result_List::show_curr_result()
{
  return ( (Simulator_Result *)show_curr() );
}

int Simulator_Result_List::query_result(char *tg_result_name)
{
    Simulator_Result  *simul_result_aux;
    char               result_name[MAXSTRING];

    simul_result_aux = show_1st_result();
    while (simul_result_aux != NULL) {
      simul_result_aux->show_name(result_name);
      if (!strcmp(result_name, tg_result_name))
	return (1);
      simul_result_aux = show_next_result();
    }

    return (0);
}

int Simulator_Result_List::show_no_result()
{
  return ( show_no_elem() );
}

#ifdef TG_PARALLEL
void Simulator_Result_List::send_context()
{
    int num_elem;
    Simulator_Result *sres;

    num_elem = show_no_elem();
    pvm_pkint(&num_elem,1,1);
    sres = show_1st_result();
    while (sres != NULL)
    {
        sres->send_context();
        sres = show_next_result();
    }
}

void Simulator_Result_List::recv_context()
{
    int num_elem;
    Simulator_Result *sres;

    if (pvm_upkint(&num_elem,1,1) < 0) return;
    while (num_elem-- > 0)
    {
        sres = new Simulator_Result("nothing"); // Nothing to do, it will fill up things in recv_context()
        if (sres == NULL)
        {
           fprintf( stderr, "Out of memory in void Simulator_Result_List::recv_context()\n");
           break;
        }
        sres->recv_context();
        add_tail_result(sres);
    }
}
#endif
