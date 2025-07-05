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
*	@file indep_chain.cpp
*   @brief <b>Indepedent Chain</b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram-II project
*   @since version 1.0
*
*   The detailed description is unavailable. 
*/


#include <stdio.h>
#include <stdlib.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_desc.h"
#include "system_state.h"
#include "string_list.h"
#include "hash.h"

extern System_Description    *the_system_desc;

Indep_Chain::Indep_Chain() : Chained_Element() 
{
  chain = new Known_State_List();
  hashtb = NULL;
  no_objs = 0;
  state_vec = NULL;
  absorb_list = new Known_State_List();
  map_tb = NULL;
}

Indep_Chain::~Indep_Chain()
{
  // delete the hash table if any
  if (hashtb != NULL)
    delete hashtb;

  // delete the chain list 
  delete chain;

  // delete the state vector 
  if (state_vec != NULL)
    delete state_vec;

  // delete the abosrbing state list 
  delete absorb_list;

  // delete the mapping table
  if (map_tb != NULL)
    free (map_tb);
}

int Indep_Chain::add_obj_id(int a_id)
{
  int i;

  if (no_objs < MAXINDEPCHAIN) {
    for (i=0 ; i < no_objs; i++) {
      if (obj_ids[i] == a_id) {
	fprintf ( stderr, "ERROR: Duplicated OBJ ids in the same independent chain\n");
	return (-1);
      }
    }
    
    obj_ids[no_objs] = a_id;
    no_objs++;
    
  } else {
    fprintf ( stderr, "ERROR: No more space in the indep chain table\n");
    return(-1);
  }
 
  return(1);
}

int Indep_Chain::query_obj_id(int obj_id)
{
  int i;

  for (i=0; i<no_objs; i++)
    if (obj_ids[i] == obj_id)
      return (TRUE);

  return FALSE;
}

int Indep_Chain::query_obj_id(int obj_id1, int obj_id2)
{
  int obj1, obj2;

  obj1 = query_obj_id(obj_id1);
  obj2 = query_obj_id(obj_id2);

  if (obj1 && obj2)
    return 2;
  if (obj1 || obj2)
    return 1;
  return 0;
}

int Indep_Chain::init(System_Description *sys_desc)
{
    unsigned int  no_vars;
    
    /* get the number os state variables in the model */
    no_vars = sys_desc->show_no_st_var(obj_ids, no_objs);
    /* allocate space for the maximum state vector */
    state_vec = (unsigned int *) malloc(sizeof(unsigned int) * (no_vars + 1));
    /* get the maximum state vector */
    sys_desc->get_max_value_vec(state_vec, obj_ids, no_objs);
    /* create the hash table */
    hashtb = new Hash (state_vec, no_vars + 1);
    /* init the hash table */
    if (!hashtb->init()) {
	fprintf( stderr, "ERROR: Cannot initialize the hash table'\n");
	return (-1);
    }

    return (1);
}

void Indep_Chain::add_states(System_State *curr_sys_st, System_State_List *sys_st_list)
{
  System_State  *sys_st;
  Known_State   *known_st;
  Transition    *trans, *trans_aux;
  unsigned int   no_st;
  Known_State   *abs_st;
  int            obj_id;
  int            det_ev_id;
  int            abs_det_ev_id;
  int            obj_det_ev_id;

    
  /* get the current state vector */
  curr_sys_st->get_st_vec(state_vec, obj_ids, no_objs);

  /* find this state in the hash table */
  if (! hashtb->find(state_vec)) {
    /* insert in the hash */
    if (! hashtb->insert(state_vec)){
      fprintf ( stderr, "ERROR: state vector variables bigger than maximum possible\n");
      exit(-1);
    }
  }

  /* get the hash number of this state vector */
  if (!hashtb->vec_to_num(&no_st, state_vec)) {
    fprintf ( stderr, "ERROR: state vector variables bigger than maximum possible\n");
    exit(-1);
  }
  
  /* if the state vector has not been explored */
  if (chain->query_known_st(no_st) == 0) {  
    /* create a new explored state */
    known_st = new Known_State(no_st);
    
    /* go through all neighbor states and set the transitions */
    sys_st = sys_st_list->show_1st_sys_st();
    while (sys_st != NULL) {

      /* show the transition to this neighbor state */
      trans = sys_st->show_1st_trans();

      if (trans->show_ev_id() != 0) {
	/* obtain the id of the DET event enabled */
	sys_st->get_enabled_det_ev(&obj_det_ev_id, &det_ev_id);
	if (trans->show_ev_id() == det_ev_id) {
	  /* get the mini vector of the destination of this transition */
	  sys_st->get_st_vec(state_vec, obj_ids, no_objs);
	  
	  /* find this state in the hash table */
	  if (! hashtb->find(state_vec)) {
	    /* insert in the hash */
	    if (! hashtb->insert(state_vec)) {
	      fprintf ( stderr, "ERROR: state vector variables bigger than maximum possible\n");
	      exit(-1);
	    }
	  }
	}
      }

      /* if the transition is exponential, then insert in the correct indep chain */
      if (trans->show_ev_id() == 0) {
	obj_id = trans->show_obj_id();
	/* if this is the correct indep chain */
	if (query_obj_id(obj_id)) {
	  /* get the mini vector of the destination of this transition */
	  sys_st->get_st_vec(state_vec, obj_ids, no_objs);

	  /* insert in the hash */
	  if (! hashtb->insert(state_vec)) {
	    fprintf ( stderr, "ERROR: state vector variables bigger than maximum possible\n");
	    exit(-1);
	  }
	  
	  /* get the hash number of this state */
	  if (!hashtb->vec_to_num(&no_st, state_vec)) {
	    fprintf ( stderr, "ERROR: state vector variables bigger than maximum possible\n");
	    exit(-1);
	  }
	  
	  /* create a new transition */
	  trans_aux = new Transition(trans);
	  
	  /* if this is an absorbing state */
	  if (trans_aux->show_disabled()) {

	    /* if the absorb state is not in the indep chain, inserts it */
	    if (chain->query_known_st(no_st) == 0) {
               abs_st = new Known_State(no_st);
               chain->add_tail_known_st (abs_st);
	    }

	    /* create a new entry and add it to the absorb state list */
	    abs_st = new Known_State(no_st);
            /* obtain the id of the DET event enabled in the absorb state */
	    sys_st->get_enabled_det_ev(&obj_det_ev_id, &abs_det_ev_id);
            /* Check if a deterministic event is been reinitialized */
            if( abs_det_ev_id == det_ev_id )
            {
                fprintf ( stderr, "\n ERROR: A deterministic event is been reinitialized. This is not allowed in the current version.\n" );
                fprintf ( stderr, "Transition, transition value: \n");
                trans_aux->print_trans();
                fprintf ( stderr, "Into state (state variables): ");
                sys_st->print_sys_st();
                fprintf ( stderr, "Deterministic event: %d\n", det_ev_id );
                exit( -1 );
            }
	    abs_st->ch_det_ev_id (abs_det_ev_id);
	    absorb_list->add_tail_known_st (abs_st);
	  }

	  /* change the destination of the transition */
	  trans_aux->ch_st_dest(no_st);
	  
	  /* add the transition to the list of transitions in the known state */
	  /* join the impulse rewards of different transitions */
	  known_st->add_trans(trans_aux);

	}
      }
      sys_st = sys_st_list->show_next_sys_st();
    }
    
    /* add the known state to the known state list */
    chain->add_tail_known_st(known_st);    
  }
}

int Indep_Chain::is_absorbing()
{
  return ( ! absorb_list->is_empty() );
}

int Indep_Chain::show_no_states()
{
  return (chain->show_no_elem());
}

Known_State_List* Indep_Chain::show_chain()
{
  return (chain);
}

void Indep_Chain::create_map()
{
  /* Reorder the states of the indep chain.     */ 
  /* The absorbing states must be the last ones */
  Known_State *abs_st;
  int    abs_st_id;
  int    no_st, no_abs_st;
  int    count_abs_st, count_non_abs_st;
  int    i;

  no_st = chain->show_no_elem();
  no_abs_st = absorb_list->show_no_elem();

  if ( (map_tb = (t_map *) malloc ((no_st + 1) * sizeof(t_map))) == NULL) {
    perror("malloc");
    fprintf ( stderr, "ERROR: Cannot alloc the absorb mapping table\n");
    exit(-1);
  }

  count_non_abs_st = 0;
  count_abs_st = 0;

  if ((abs_st = absorb_list->show_1st_known_st()) != NULL)
    abs_st_id = abs_st->show_st_id();
  else
    abs_st_id = -1;

  for (i = 1; i <= no_st; i++) {
    if (i == abs_st_id) {
      count_abs_st++;
      map_tb[i].to = no_st - no_abs_st + count_abs_st;
      
      if ((abs_st = absorb_list->show_next_known_st()) != NULL)
	abs_st_id = abs_st->show_st_id();
      else
	abs_st_id = -1;
      
    } else {
      count_non_abs_st++;
      map_tb[i].to = count_non_abs_st;
    }    
  } 
}

void Indep_Chain::print_states(FILE *fd)
{
   Known_State  *st;
   int           st_num, no_vars, i;
   unsigned int *state_vec;

   no_vars = the_system_desc->show_no_st_var(obj_ids, no_objs);
   state_vec = (unsigned int *) malloc(sizeof(unsigned int) * (no_vars + 1));

   st = chain->show_1st_known_st();
   while (st != NULL) {
      st_num = st->show_st_id();
      hashtb->num_to_vec(st_num, state_vec);
      fprintf (fd, "%d (", map_tb[st->show_st_id()].to);
      for (i = 1; i < no_vars; i++)
         fprintf (fd, "%d,", state_vec[i]);
      fprintf (fd, "%d)\n", state_vec[i]);
      st = chain->show_next_known_st();
   }
   free (state_vec);
}

void Indep_Chain::print_abs_states (FILE *fd)
{
   Known_State *abs_st;

   fprintf(fd, "abs states:\n");
   abs_st = absorb_list->show_1st_known_st();
   while (abs_st != NULL) {
      fprintf (fd, "%d\n", abs_st->show_st_id());
      abs_st = absorb_list->show_next_known_st();
   }
}

void Indep_Chain::print_map(FILE *fd)
{
   int i; 
   int no_st = chain->show_no_elem();

   fprintf(fd, "map:\n");
   for (i = 1; i <= no_st; i++)
      fprintf (fd, "%d -> %d\n", i, map_tb[i].to);
}

void Indep_Chain::print_indep_chain(FILE *fd, TGFLOAT unif_rate)
{
  Known_State *known_st;
  Transition_List *trans_list;
  Transition *trans;
  Expr_Val *rate;
  TGFLOAT rate_val, total_rate = 0;
  unsigned int st_id_source, st_id_dest;
  int          no_abs_st;

  create_map();
  //print_map(fd);

  print_states(fd);
  no_abs_st = absorb_list->show_no_elem();
  fprintf (fd, "%d\n", no_abs_st);

  known_st = chain->show_1st_known_st();
  while (known_st != NULL) {
    st_id_source = known_st->show_st_id();
    st_id_source = map_tb[st_id_source].to;

    trans_list = known_st->show_trans_list(LIST_RO);
    trans = trans_list->show_1st_trans();    
    total_rate = 0;
    while (trans != NULL) {	    
      st_id_dest = trans->show_st_dest();
      st_id_dest = map_tb[st_id_dest].to;

      if (st_id_source != st_id_dest) {
	rate = trans->show_rate();
	rate_val = rate->show_value();
	fprintf (fd, "%d %d %.10E\n", st_id_source, st_id_dest, rate_val/unif_rate);
	total_rate += rate_val;
      }
      trans = trans_list->show_next_trans();
    }
    delete trans_list;
    fprintf (fd, "%d %d %.10E\n", st_id_source, st_id_source, (unif_rate - total_rate)/unif_rate);
    known_st = chain->show_next_known_st();
  }
}

void Indep_Chain::print_absorb_states(FILE *fd, Integer_List **map_var)
{
  Known_State *st;
  unsigned int st_num;
  int          det_ev_id;
  unsigned int i;

  if (map_tb == NULL)
    create_map();

  /* get the first state to find out the size of the state vec */
  st = chain->show_1st_known_st();
  st_num = st->show_st_id();
  if (!hashtb->num_to_vec(st_num, state_vec)) {
    fprintf ( stderr, "ERROR: state vector variables bigger than maximum possible\n");
    exit(-1);
  }

  st = absorb_list->show_1st_known_st();
  while (st != NULL) {
    st_num = st->show_st_id();
    det_ev_id = st->show_det_ev_id();
    if (!hashtb->num_to_vec(st_num, state_vec)) {
      fprintf ( stderr, "ERROR: state vector variables bigger than maximum possible\n");
      exit(-1);
    }

    fprintf (fd, "%d  ", map_tb[st_num].to);

    for (i=1; i<=state_vec[0]; i++) 
      fprintf (fd, "%d ", map_var[i]->query_int((int)state_vec[i]) );

    fprintf (fd, "%d\n", det_ev_id);

    st = absorb_list->show_next_known_st();
  }
  fprintf (fd, "-1 0 0\n");
  

}

/*************************************************************/

Indep_Chain_List::Indep_Chain_List() : Chained_List()
{
   debug(4,"Indep_Chain_List::Indep_Chain_List(): creating object");
}

Indep_Chain_List::Indep_Chain_List(Indep_Chain_List *ev_list) : Chained_List( (Chained_List *)ev_list)
{

   debug(4,"Indep_Chain_List::Indep_Chain_List(Indep_Chain_List *): creating object");
}

Indep_Chain_List::~Indep_Chain_List()
{
  Indep_Chain *ev_aux;

  if (access == LIST_RW) {
    ev_aux = get_1st_indep_chain();
    while (ev_aux != NULL) {
      delete ev_aux;
      ev_aux = get_1st_indep_chain();
    }
  }

   debug(4,"Indep_Chain_List::~Indep_Chain_List(): destroying object");
}

int Indep_Chain_List::add_tail_indep_chain(Indep_Chain *new_indep_chain)
{
  return ( add_tail( (Chained_Element *)new_indep_chain) );
}

int Indep_Chain_List::add_head_indep_chain(Indep_Chain *new_indep_chain)
{
  return ( add_head( (Chained_Element *)new_indep_chain) );
}

Indep_Chain *Indep_Chain_List::show_1st_indep_chain()
{
  return ( (Indep_Chain *)show_head() );
}

Indep_Chain *Indep_Chain_List::get_1st_indep_chain()
{
  return ( (Indep_Chain *)del_head() );
}

Indep_Chain *Indep_Chain_List::show_next_indep_chain()
{
  return ( (Indep_Chain *)show_next() );
}

Indep_Chain *Indep_Chain_List::show_curr_indep_chain()
{
  return ( (Indep_Chain *)show_curr() );
}

int Indep_Chain_List::show_no_indep_chain()
{
  return ( show_no_elem() );
}

