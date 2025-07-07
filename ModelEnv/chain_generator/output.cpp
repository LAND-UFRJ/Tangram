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
*  @file output.cpp
*  @brief <b>output methods</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  Output methods for <modelname>.{states,param,absorb_st,st_trans_prob_mtx}
*  et cetera.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "prototypes.h"
#include "general.h"
#include "gramatica.h"
#include "chained_list.h"
#include "system_state.h"
#include "system_desc.h"
#include "string_list.h"
#include "hashlib.h"
#include "output.h"


/* table of file descriptors */
t_file_desc file_desc_tb[MAX_TABLE_ENTRIES];

/* global variables */
extern Known_State_List   *markov_chain;
extern int                 Debug_Level;
extern char                base_filename[MAXSTRING];
extern System_Description *the_system_desc;
extern Hash               *hashtb;
TGFLOAT calculate_uniform_rate(Known_State_List *);

int print_st_vec(FILE *fd, unsigned int *st_vec)
{
    unsigned int i;

    if (fd == NULL)
        fd = stdout;

    fprintf (fd, "(");
    for (i=1; i<=(st_vec[0] - 1); i++)
        fprintf (fd, "%d,", st_vec[i]);
    fprintf (fd, "%d)\n", st_vec[i]);

    return (1);
}

int generate_states_file()
{
    Known_State      *known_st;
    unsigned int     *st_vec;
    unsigned int      st_id;
    FILE *fd;
    char  filename[MAXSTRING];

    strcpy (filename, base_filename);
    strcat (filename, ".states");

    if ( (fd = fopen (filename, "w")) == NULL) {
        perror ("fopen <modelname>.states");
        return (-1);
    }

    /* allocate space for the state vector */
    st_vec = (unsigned int *) malloc (sizeof(unsigned int) * (the_system_desc->show_no_st_var() + 1));

    known_st = markov_chain->show_1st_known_st();

    while (known_st != NULL) {
        st_id = known_st->show_st_id();
        if (! hashtb->num_to_vec (st_id, st_vec) ) {
            fprintf ( stderr, "ERROR: state vector variables bigger than maximum possible\n");
            exit(-1);
        }
        fprintf (fd, "%d ", st_id);
        print_st_vec(fd, st_vec);
        known_st = markov_chain->show_next_known_st();
    }

    free (st_vec);
    fclose (fd);

    return (1);
}

int print_func_id(FILE *fd, Expr_Val *expr, String_List *str_list)
{
    int  func_id = 0;

    if( expr->show_type() == NUMERICAL )
        expr->print_expr(fd);
    else
    {
        func_id = str_list->add_str( expr->show_str_val( ) );
        fprintf( fd, "[%d]", func_id );
    }
    return( func_id );
}


int generate_mtx_file(String_List *str_list)
{
    Transition_List   *trans_list;
    Transition        *trans;
    Transition        *trans_aux;
    Transition        *trans_total;
    Known_State       *known_st;
    int                st_id_source;
    int                st_id_dest;
    Expr_Val          *rate_val;
    int                absorbing_st;
    int                abs_st = 0;
    FILE              *self;
    FILE              *out;
    FILE              *fd;
    FILE              *fd2;
    char               filename[MAXSTRING];
    unsigned int      *st_vec;

    strcpy (filename, base_filename);
    strcat (filename, ".generator_mtx");

    if( the_system_desc->show_chain_type() == LITERAL )
        strcat( filename, ".param" );

    if ( (fd = fopen (filename, "w")) == NULL) {
        perror ("fopen <modelname>.param");
        return (-1);
    }

    strcpy (filename, base_filename);
    strcat (filename, ".trans_same_st");
    if ( (self = fopen (filename, "w")) == NULL) {
        perror ("fopen <modelname>.trans_same_st");
        return (-1);
    }

    strcpy (filename, base_filename);
    strcat (filename, ".absorb_st");
    if ( (fd2 = fopen (filename, "w")) == NULL) {
        perror ("fopen <modelname>.absorb_st");
        return (-1);
    }

    /* allocate space for the state vector */
    st_vec = (unsigned int *) malloc (sizeof(unsigned int) * (the_system_desc->show_no_st_var() + 1));

    fprintf (fd, "%d\n", markov_chain->show_no_known_st() );

    known_st = markov_chain->show_1st_known_st();
    while (known_st != NULL) {
        trans_list = known_st->show_trans_list(LIST_RO);
        trans = trans_list->show_1st_trans();
        st_id_source = known_st->show_st_id();
        /* this is an absorbing state */
        absorbing_st = TRUE;
        while (trans != NULL) {
            if( trans->show_add_later() != 'd' )
            {
                st_id_dest = trans->show_st_dest();
                if (st_id_source != st_id_dest)
                    out = fd;
                else
                    out = self;

                absorbing_st = FALSE;
                fprintf( out, "%d %d ", st_id_source, st_id_dest);

                /* find transitions from same source and same destination     */
                /* Note: the generator do not add transition rates that       */
                /* have different distributions (e.g exp and deterministic    */
                /* rate). In this case we defer the sum till the code below   */
                /* All transitions are added and then the non exp transitions */
                /* are deleted.                                               */
                if( trans->show_add_later() == 'a' )
                {
                    trans_total = new Transition( trans );
                    trans_aux = trans->getSameTransition();
                    trans_total->sum_rate( trans_aux->show_rate() );
                    trans_aux->markDeleted();
                    rate_val = trans_total->show_rate();
                    delete trans_total;
                }
                else
                {
                    rate_val = trans->show_rate();
                }
                print_func_id( out, rate_val, str_list );
                fprintf( out, "\n" );
            }
            trans = trans_list->show_next_trans();
        }
        if (absorbing_st) {
            if (! hashtb->num_to_vec (st_id_source, st_vec)) {
                fprintf( stderr,"ERROR: state vector variables bigger than maximum possible\n");
                exit(-1);
            }
            fprintf (fd2, "%d ", st_id_source);
            print_st_vec(fd2, st_vec);
            abs_st++;
        }
        delete trans_list;
        known_st = markov_chain->show_next_known_st();
    }

    if (abs_st > 0) {
        fprintf( stderr,"WARNING: There were %d absorbing states\n", abs_st);
        fprintf( stderr,"         Please check the %s file.\n", filename);
    }

    free (st_vec);
    fclose (fd);
    fclose (fd2);
    fclose (self);

    return (1);
}


int generate_trans_prob_mtx_file(TGFLOAT unif_rate)
{
    FILE              *fd;
    char               filename[MAXSTRING];
    Transition_List   *trans_list;
    Transition        *trans;
    Known_State       *known_st;
    int                st_id_source;
    int                st_id_dest;
    Expr_Val          *rate;
    TGFLOAT             rate_val, total_rate;

    strcpy (filename, base_filename);
    strcat (filename, ".NM.st_trans_prob_mtx");
    if ( (fd = fopen (filename, "w")) == NULL) {
        perror ("fopen");
        return (-1);
    }

    fprintf (fd, "%d\n", markov_chain->show_no_known_st() );

    known_st = markov_chain->show_1st_known_st();
    while (known_st != NULL) {
        st_id_source = known_st->show_st_id();
        trans_list = known_st->show_trans_list(LIST_RO);
        trans = trans_list->show_1st_trans();
        total_rate = 0;
        while (trans != NULL) {
            st_id_dest = trans->show_st_dest();
            if (st_id_source != st_id_dest) {
                rate = trans->show_rate();
                rate_val = rate->show_value();
                total_rate += rate_val;
                fprintf (fd, "%d %d %.10e\n", st_id_source, st_id_dest, rate_val/unif_rate);
            }
            trans = trans_list->show_next_trans();
        }
        delete trans_list;
        fprintf (fd, "%d %d %.10e\n", st_id_source, st_id_source, (unif_rate - total_rate)/unif_rate);
        known_st = markov_chain->show_next_known_st();
    }

    fclose (fd);

    return (1);
}


int generate_det_files(String_List *str_list)
{
    Transition_List   *trans_list;
    Transition        *trans;
    Known_State       *known_st;
    int                st_id_source;
    int                st_id_dest;
    Expr_Val          *rate_val, *det_rate;
    FILE              *fd1;
    FILE              *fd2;
    char               filename[MAXSTRING];
    unsigned int      *st_vec;
    unsigned int       i;

    strcpy (filename, base_filename);
    strcat (filename, ".NM.states_det_ev");
    if ( (fd1 = fopen (filename, "w")) == NULL) {
        perror ("fopen");
        return (-1);
   }

    strcpy (filename, base_filename);
    strcat (filename, ".NM.embedded_points");
    if ( (fd2 = fopen (filename, "w")) == NULL) {
        perror ("fopen");
        return (-1);
    }

    /* allocate space for the state vector */
    st_vec = (unsigned int *) malloc (sizeof(unsigned int) * (the_system_desc->show_no_st_var() + 1));

    known_st = markov_chain->show_1st_known_st();
    while (known_st != NULL) {
        st_id_source = known_st->show_st_id();
        if (! hashtb->num_to_vec (st_id_source, st_vec)) {
            fprintf( stderr, "ERROR: state vector variables bigger than maximum possible\n");
            exit(-1);
        }
        fprintf (fd1, "%d (", st_id_source);
        for (i=1; i<=st_vec[0]; i++)
            fprintf (fd1, "%d,", st_vec[i]);
        fprintf (fd1, "%d)\n", known_st->show_det_ev_id());

        if (known_st->show_det_ev_id() != 0) {

            trans_list = known_st->show_trans_list(LIST_RO);
            trans = trans_list->show_1st_trans();
            det_rate = known_st->show_det_ev_rate();

            while (trans != NULL) {
                if (trans->show_ev_id() != 0) {
                    st_id_dest = trans->show_st_dest();

                    /* the transition representing an embedded point
                    when a det event fires is included even if it is for
                    the same state */
                    fprintf (fd2, "%d %d ", st_id_source, st_id_dest);
                    rate_val = trans->show_rate();
                    rate_val->divide_by_expr_val(det_rate);
                    /* this comment excludes the prob of the det event */
                    /* print_func_id(fd2, rate_val, str_list); */
                    fprintf (fd2, "\n");

                }
                trans = trans_list->show_next_trans();
            }

            delete trans_list;
        }

        known_st = markov_chain->show_next_known_st();
    }

    fclose (fd1);
    fclose (fd2);

    return (1);
}

Integer_List **create_map_var (Known_State_List *chain)
{
  Known_State *st;
  unsigned int st_num, *state_vec, i;
  int          no_vars;

  Integer_List **map_var = NULL;
  Integer       *a_int;

  /* get the number os state variables in the model */
  no_vars = the_system_desc->show_no_st_var();
  /* allocate space for the maximum state vector */
  state_vec = (unsigned int *) malloc(sizeof(unsigned int) * (no_vars + 1));

  /* get the first state to find out the size of the state vec */
  st = chain->show_1st_known_st();
  st_num = st->show_st_id();
  if (!hashtb->num_to_vec(st_num, state_vec)) {
    fprintf ( stderr, "ERROR: state vector variables bigger than maximum possible\n");
    exit(-1);
  }

  map_var = (Integer_List **) malloc ( ( state_vec[0] + 1) * sizeof (Integer_List *));
  if (map_var == NULL) {
    perror("malloc");
    fprintf ( stderr, "ERROR: No space left to alloc state mapp table \n");
    exit(-1);
  }

  for( i = 1; i <= state_vec[ 0 ]; i++ )
    map_var[i] = new Integer_List();

  /* No Comments :-( */
  st = chain->show_1st_known_st();
  while (st != NULL) {
    st_num = st->show_st_id();
    if (!hashtb->num_to_vec(st_num, state_vec)) {
      fprintf ( stderr, "ERROR: state vector variables bigger than maximum possible\n");
      exit(-1);
    }
    for (i=1; i<=state_vec[0]; i++) {
      if (map_var[i]->query_int((int)state_vec[i]) < 0) {
          a_int = new Integer((int)state_vec[i]);
          map_var[i]->add_tail_int(a_int);
      }
    }
    st = chain->show_next_known_st();
  }

  return (map_var);
}

int generate_chains_btwn_embd_pts(TGFLOAT unif_rate,
                                  int no_vars, int no_det_evs, int no_states)
{
    int                      absorb_indep_chain;
    int                      obj_id;
    Expr_Val                *det_ev_rate;
    FILE                    *fd;
    char                     filename[MAXSTRING];
    int                      i, var_num;
    int                      det_id;
    Event                   *det_ev;
    Object_Description_List *obj_desc_list;
    Object_Description      *obj_desc;
    int                      indep_chain_num;
    Indep_Chain_List        *indep_chain_list;
    Indep_Chain             *indep_chain;
    Known_State_List        *chain;
    TGFLOAT                   indep_chain_unif_rate = 0;
    char                     ev_name[MAXNAME];
    char                     obj_name[MAXNAME];

    strcpy (filename, base_filename);
    strcat (filename, ".NM.chns_betw_embed_pnts");
    if( (fd = fopen( filename, "w" )) == NULL )
    {
        perror ("fopen");
        return (-1);
    }

    /* misc_data information */

    fprintf (fd, "%d %d %d\n", no_vars, no_det_evs, no_states);

    fprintf (fd, "%.10E\n", unif_rate);

    for (det_id = 1; det_id <= the_system_desc->show_no_det_ev(); det_id++)
    {
        det_ev = the_system_desc->show_det_ev_by_id(det_id);

        /* set the absorb chain as the number zero chain */
        det_ev->set_zero();

        fprintf( fd, "event %d %d ", det_id, det_ev->show_no_indep_chains() );

        absorb_indep_chain = det_ev->show_absorbing_indep_chain();

        if( absorb_indep_chain > 0 )
            fprintf (fd, "1 ");
        else
            fprintf (fd, "0 ");

        det_ev_rate = det_ev->show_det_ev_rate();
        if( det_ev_rate->show_value() > 0 )
            fprintf (fd, "%.10E\n", 1.0 / det_ev_rate->show_value());
        else
        {
            /* get the name of the object */
            obj_desc = the_system_desc->show_obj_desc_by_det_ev_id(det_id);
            if( obj_desc != NULL )
                obj_desc->show_name(obj_name);
            else
                strcpy (obj_name, "Unknown");
            det_ev->show_name(ev_name);
            fprintf ( stderr, "ERROR: DET event %s in object %s is never enabled\n",
                    ev_name, obj_name);
            exit(-1);
        }

        obj_desc_list = the_system_desc->show_obj_desc_list(LIST_RO);
        obj_desc      = obj_desc_list->show_1st_obj_desc();
        var_num       = 1;
        while( obj_desc != NULL )
        {
            obj_id = obj_desc->show_id();
            indep_chain_num = det_ev->show_indep_chain_num( obj_id );
            for( i = 0; i < obj_desc->show_no_st_var(); i++ )
            {
                fprintf (fd, "-1 %d %d\n", var_num, indep_chain_num);
                var_num++;
            }
            obj_desc = obj_desc_list->show_next_obj_desc();
        }
        fprintf( fd, "-1 0 0\n" );

        indep_chain_list = det_ev->show_indep_chain_list( LIST_RO );
        indep_chain      = indep_chain_list->show_1st_indep_chain();
        i = 0;
        while( indep_chain != NULL )
        {
            chain = indep_chain->show_chain();
            indep_chain_unif_rate = calculate_uniform_rate( chain );
            fprintf( fd, "chain %d %d %.10e\n", i,
                     indep_chain->show_no_states(), indep_chain_unif_rate );
            indep_chain->print_indep_chain( fd, indep_chain_unif_rate );
            fprintf( fd, "-1 0 0\n" );
            indep_chain = indep_chain_list->show_next_indep_chain();
            i++;
        }

    }

    fclose( fd );
    return( 1 );
}

int generate_func_map_part(FILE *fd, String_List *str_list)
{
    String  *str_aux;
    int      i = 0;
    int      no_elem;

    no_elem = str_list->show_no_str();
    fprintf (fd, "%d\n", no_elem);

    str_aux = str_list->show_1st_str();
    while (str_aux != NULL) {
        fprintf (fd, "%d\t",i);
        str_aux->print_str(fd);
        fprintf (fd, "\n");
        i++;
        str_aux = str_list->show_next_str();
    }

    return (1);
}

int generate_param_part(FILE *fd, String_List *str_list)
{
    Object_Description_List *obj_desc_list;
    Object_Description      *obj_desc;
    Symbol_List             *sym_list;
    Symbol                  *sym;
    String_List             *new_str_list;
    String                  *str_elem;
    char                     obj_name[MAXSTRING];
    char                     sym_name[MAXSTRING];
    char                     str_aux[MAXSTRING];

    new_str_list = new String_List();

    obj_desc_list = the_system_desc->show_obj_desc_list(LIST_RO);
    obj_desc = obj_desc_list->show_1st_obj_desc();
    while (obj_desc != NULL) {
        obj_desc->show_name(obj_name);

        sym_list = obj_desc->show_symbol_list(LIST_RO);
        sym = sym_list->show_1st_sym();
        while (sym != NULL) {
            if (sym->show_type() == INT_PARAM_TYPE ||
                sym->show_type() == FLOAT_PARAM_TYPE ) {
                sprintf (str_aux, "%c", sym->show_char_code());
                sym->show_name(sym_name);
                if (str_list == NULL) {
                    sprintf (str_aux, "%c\t%s.%s\n", sym->show_char_code(), obj_name, sym_name);
                    new_str_list->add_str(str_aux);
                }
                else
                    if (str_list->contains_str(str_aux) ) {
                        sprintf (str_aux, "%c\t%s.%s\n", sym->show_char_code(), obj_name, sym_name);
                        new_str_list->add_str(str_aux);
                    }
            }
            sym = sym_list->show_next_sym();
        }
        delete sym_list;
        obj_desc = obj_desc_list->show_next_obj_desc();
    }
    delete obj_desc_list;

    fprintf (fd, "%d\n", new_str_list->show_no_str());
    str_elem = new_str_list->show_1st_str();
    while (str_elem != NULL) {
        str_elem->print_str(fd);
        str_elem = new_str_list->show_next_str();
    }
    delete new_str_list;

    return (1);
}

int generate_parameter_file(const char *file_suffix, String_List *str_list)
{
    FILE *fd;
    char  filename[MAXSTRING];

    strcpy (filename, base_filename);
    strcat (filename, file_suffix);

    if ( (fd = fopen (filename, "w")) == NULL) {
        perror ("fopen");
        return (-1);
    }

    generate_param_part(fd, str_list);

    generate_func_map_part(fd, str_list);

    fclose (fd);
    return (1);
}

int generate_all_parameters()
{
    FILE *fd;
    char  filename[MAXSTRING];

    strcpy (filename, base_filename);
    strcat (filename, ".parameter");

    if ( (fd = fopen (filename, "w")) == NULL) {
        perror ("fopen");
        return (-1);
    }

    generate_param_part(fd, NULL);

    fclose(fd);
    return (1);
}

int generate_vstat_file()
{
    Object_Description_List *obj_desc_list;
    Object_Description      *obj_desc;
    Symbol_List             *sym_list;
    Symbol                  *sym;
    char                     obj_name[MAXSTRING];
    char                     sym_name[MAXSTRING];

    FILE *fd;
    char  filename[MAXSTRING];

    strcpy (filename, base_filename);
    strcat (filename, ".vstat");

    if ( (fd = fopen (filename, "w")) == NULL) {
        perror ("fopen");
        return (-1);
    }

    obj_desc_list = the_system_desc->show_obj_desc_list(LIST_RO);
    obj_desc = obj_desc_list->show_1st_obj_desc();
    while (obj_desc != NULL) {
        obj_desc->show_name(obj_name);

        sym_list = obj_desc->show_symbol_list(LIST_RO);
        sym = sym_list->show_1st_sym();
        while (sym != NULL) {
            if ((sym->show_type() == STATE_VAR_TYPE) || (sym->show_type() == STATE_VAR_FLOAT_TYPE)) {
                sym->show_name(sym_name);
                fprintf (fd, "%s.%s\n", obj_name, sym_name);
            }
            sym = sym_list->show_next_sym();
        }
        delete sym_list;
        obj_desc = obj_desc_list->show_next_obj_desc();
    }
    delete obj_desc_list;

    fclose (fd);
    return (1);
}



void init_file_desc_tb()
{
    int i;

    for (i=0; i<MAX_TABLE_ENTRIES; i++) {
        strcpy(file_desc_tb[i].name, "");
        file_desc_tb[i].fd = NULL;
    }
}

void close_file_desc_tb()
{
    int i = 0;

    while (file_desc_tb[i].fd != NULL) {
        fclose (file_desc_tb[i].fd);
        i++;
    }
}

FILE *get_file_entry(char *name)
{
    int i = 0;

    while (file_desc_tb[i].fd != NULL) {
        if (!strcmp(file_desc_tb[i].name, name))
            return (file_desc_tb[i].fd);
        i++;
    }
    return (NULL);
}

FILE *get_file_entry(int no_entry)
{
    return (file_desc_tb[no_entry].fd);
}

int create_file_entry(char *name, int rewardType)
{
    int   i = 0;
    char  filename[MAXSTRING];

    while (file_desc_tb[i].fd != NULL) {
        if (!strcmp(file_desc_tb[i].name, name))
            return (1);
        i++;
    }
    strcpy (file_desc_tb[i].name, name);

    switch( rewardType )
    {
        case RATE: sprintf( filename, "%s.rate_reward.%s",base_filename,name);
                   break;
        case IMPULSE: sprintf( filename, "%s.impulse_reward.%s",base_filename,name);
                   break;
        default: sprintf( filename, "%s.reward.%s",base_filename,name);
                 break;
    }

    if ( (file_desc_tb[i].fd = fopen (filename, "w")) == NULL) {
        perror ("fopen");
        return (-1);
    }
    return (1);
}

int create_rate_file_tb()
{
    Object_Description_List *obj_desc_list;
    Object_Description *obj_desc;
    Rate_Reward_Desc_List *rew_desc_list;
    Rate_Reward_Desc *rew_desc;
    char    obj_name[MAXSTRING];
    char    rew_name[MAXSTRING];
    char    str[MAXSTRING];

    obj_desc_list = the_system_desc->show_obj_desc_list(LIST_RO);
    obj_desc = obj_desc_list->show_1st_obj_desc();

    while (obj_desc != NULL) {
        obj_desc->show_name(obj_name);

        rew_desc_list = obj_desc->show_reward_list(LIST_RO);
        rew_desc = rew_desc_list->show_1st_rew();
        while (rew_desc != NULL) {
            rew_desc->show_name(rew_name);
            sprintf (str, "%s.%s",obj_name,rew_name);
            create_file_entry(str, RATE);

            rew_desc = rew_desc_list->show_next_rew();
        }
        delete rew_desc_list;
        obj_desc = obj_desc_list->show_next_obj_desc();
    }
    delete obj_desc_list;

    /* insert the global rewards files */
    rew_desc_list = the_system_desc->show_global_rew_desc_list(LIST_RO);
    rew_desc = rew_desc_list->show_1st_rew();
    while (rew_desc != NULL) {
        rew_desc->show_name(rew_name);
        sprintf (str, "%s.%s","GlobalReward",rew_name);
        create_file_entry(str, RATE);

        rew_desc = rew_desc_list->show_next_rew();
    }
    delete rew_desc_list;

    return(1);
}

int create_impulse_file_tb()
{
    Object_Description_List *obj_desc_list;
    Object_Description *obj_desc;
    Imp_Reward_Desc_List *imp_desc_list;
    Imp_Reward_Desc *imp_desc;
    Event_List *ev_list;
    Event *ev;
    Message_List *msg_list;
    Message *msg;
    Action_List *act_list;
    Action *act;
    char    rew_name[MAXSTRING];

    obj_desc_list = the_system_desc->show_obj_desc_list(LIST_RO);
    obj_desc = obj_desc_list->show_1st_obj_desc();
    while (obj_desc != NULL) {

        /* Get all actions in the Events part */
        ev_list = obj_desc->show_event_list(LIST_RO);
        ev = ev_list->show_1st_event();
        while (ev != NULL) {
            act_list = ev->show_action_list(LIST_RO);
            act = act_list->show_1st_act();
            while (act != NULL) {
                imp_desc_list = act->show_impulse_list(LIST_RO);
                imp_desc = imp_desc_list->show_1st_rew();
                while (imp_desc != NULL) {
                    imp_desc->show_name(rew_name);
                    create_file_entry(rew_name, IMPULSE);
                    imp_desc = imp_desc_list->show_next_rew();
                }
                delete imp_desc_list;
                act = act_list->show_next_act();
            }
            delete act_list;
            ev = ev_list->show_next_event();
        }
        delete ev_list;

        /* Get all actions in the Messages part */
        msg_list = obj_desc->show_message_list(LIST_RO);
        msg = msg_list->show_1st_msg();
        while (msg != NULL) {
            act_list = msg->show_action_list(LIST_RO);
            act = act_list->show_1st_act();
            while (act != NULL) {
                imp_desc_list = act->show_impulse_list(LIST_RO);
                imp_desc = imp_desc_list->show_1st_rew();
                while (imp_desc != NULL) {
                    imp_desc->show_name(rew_name);
                    create_file_entry(rew_name, IMPULSE);
                    imp_desc = imp_desc_list->show_next_rew();
                }
                delete imp_desc_list;
                act = act_list->show_next_act();
            }
            delete act_list;
            msg = msg_list->show_next_msg();
        }
        delete msg_list;

        obj_desc = obj_desc_list->show_next_obj_desc();
    }
    delete obj_desc_list;

    return (1);
}


/******************************************************************************
    Insert local and global bounds in the first line of the reward file
******************************************************************************/
int insert_rate_reward_bounds()
{
    Object_Description_List *obj_desc_list;
    Object_Description *obj_desc;
    Rate_Reward_Desc_List *rew_desc_list;
    Rate_Reward_Desc *rew_desc;
    FILE *fd;
    int i = 0;

    /* insert local reward bounds */
    obj_desc_list = the_system_desc->show_obj_desc_list(LIST_RO);
    obj_desc = obj_desc_list->show_1st_obj_desc();

    while (obj_desc != NULL) {

        rew_desc_list = obj_desc->show_reward_list(LIST_RO);
        rew_desc = rew_desc_list->show_1st_rew();
        while (rew_desc != NULL) {

            fd = get_file_entry(i);
            if (rew_desc->is_low_bounded())
                fprintf (fd, "%.8e ", rew_desc->show_low_bound());
            else
                fprintf (fd, "INF ");
            if (rew_desc->is_upp_bounded())
                fprintf (fd, "%.8e\n", rew_desc->show_upp_bound());
            else
                fprintf (fd, "INF\n");

            i++;
            rew_desc = rew_desc_list->show_next_rew();
        }
        delete rew_desc_list;
        obj_desc = obj_desc_list->show_next_obj_desc();
    }
    delete obj_desc_list;

    /* insert global rewards bounds */
    rew_desc_list = the_system_desc->show_global_rew_desc_list(LIST_RO);
    rew_desc = rew_desc_list->show_1st_rew();
    while (rew_desc != NULL) {

            fd = get_file_entry(i);
            if (rew_desc->is_low_bounded())
                fprintf (fd, "%.8e ", rew_desc->show_low_bound());
            else
                fprintf (fd, "INF ");
            if (rew_desc->is_upp_bounded())
                fprintf (fd, "%.8e\n", rew_desc->show_upp_bound());
            else
                fprintf (fd, "INF\n");

            i++;
            rew_desc = rew_desc_list->show_next_rew();
    }
    delete rew_desc_list;

    return(1);
}

int generate_rate_reward_files(String_List *str_list)
{
    Rate_Reward_List *reward_list;
    Rate_Reward *reward;
    Known_State *known_st;
    int          st_id;
    Expr_Val    *rate;
    int          i;
    FILE        *fd;

    init_file_desc_tb();
    create_rate_file_tb();

    /* insert the bounds for all the rewards */
    insert_rate_reward_bounds();

    known_st = markov_chain->show_1st_known_st();

    while (known_st != NULL) {
        reward_list = known_st->show_reward_list(LIST_RO);
        reward = reward_list->show_1st_rew();
        st_id = known_st->show_st_id();
        i = 0;
        while (reward != NULL) {
            rate = reward->show_value();
            if (!rate->is_zero()) {
                fd = get_file_entry(i);
                fprintf (fd, "%d ", st_id);
                print_func_id(fd, rate, str_list);
                fprintf (fd, "\n");
            }

            i++;
            reward = reward_list->show_next_rew();
        }
        delete reward_list;
        known_st = markov_chain->show_next_known_st();
    }

    close_file_desc_tb();
    return (1);
}


int generate_imp_reward_files(String_List *str_list)
{
    Known_State      *known_st;
    Transition_List  *trans_list;
    Transition       *trans;
    Imp_Reward_List  *imp_rew_list;
    Imp_Reward       *imp_rew;
    Imp_Rew_Val_List *imp_rew_val_list;
    Imp_Rew_Val      *imp_rew_val;
    Expr_Val         *val_aux;
    char              imp_name[MAXSTRING];
    int               imp_id;
    int               st_id_org, st_id_dest;
    FILE             *fd;


    init_file_desc_tb();
    create_impulse_file_tb();

    known_st = markov_chain->show_1st_known_st();

    while (known_st != NULL) {
        trans_list = known_st->show_trans_list(LIST_RO);
        trans = trans_list->show_1st_trans();
        st_id_org = known_st->show_st_id();
        while (trans != NULL) {
            st_id_dest = trans->show_st_dest();
            imp_rew_list = trans->show_imp_rew_list(LIST_RO);
            imp_rew = imp_rew_list->show_1st_rew();
            while (imp_rew != NULL) {
                imp_id = imp_rew->show_id();
                the_system_desc->show_imp_rew_name(imp_name, imp_id);
                fd = get_file_entry(imp_name);
                imp_rew_val_list = imp_rew->show_imp_val_list(LIST_RO);
                imp_rew_val = imp_rew_val_list->show_1st_rew();
                while (imp_rew_val != NULL) {
                    fprintf (fd, "%d %d ",st_id_org, st_id_dest);
                    val_aux = imp_rew_val->show_value();
                    print_func_id(fd, val_aux, str_list);
                    fprintf (fd, " ");
                    val_aux = imp_rew_val->show_prob();
                    print_func_id(fd, val_aux, str_list);
                    fprintf (fd, "\n");
                    imp_rew_val = imp_rew_val_list->show_next_rew();
                }
                delete imp_rew_val_list;
                imp_rew = imp_rew_list->show_next_rew();
            }
            delete imp_rew_list;
            trans = trans_list->show_next_trans();
        }
        delete trans_list;
        known_st = markov_chain->show_next_known_st();
    }
    close_file_desc_tb();
    return (1);
}


TGFLOAT calculate_uniform_rate(Known_State_List *chain)
{
    Known_State     *known_st;
    Transition_List *trans_list;
    Transition      *trans;
    Imp_Reward_List *imp_rew_list;
    int              st_id_org, st_id_dest;
    TGFLOAT           unif_rate = 0, trans_rate;

    /* get the first known state of the markov chain */
    known_st = chain->show_1st_known_st();

    /* go through all known states */
    while (known_st != NULL) {
        /* for each known_state get the transition list */
        trans_list = known_st->show_trans_list(LIST_RO);
        trans = trans_list->show_1st_trans();
        /* get the number of the known state */
        st_id_org = known_st->show_st_id();
        /* go through all the transitions of this known_state */
        trans_rate = 0;
        while (trans != NULL) {
            /* get the number of the destination state of this trans */
            st_id_dest = trans->show_st_dest();
            if (st_id_org == st_id_dest) {
                /* get the impulse reward list */
                imp_rew_list = trans->show_imp_rew_list(LIST_RO);
                if (imp_rew_list != NULL) {
                    /* if the list isn't empty */
                    /* then add up the rate of this transition */
                    if (!imp_rew_list->is_empty()) {
                        trans_rate = trans_rate + trans->show_rate_value();
                    }
                    /* delete the list created above */
                    delete imp_rew_list;
                }
            } else
                /* add the rate of this transition to the trans_rate */
                trans_rate = trans_rate + trans->show_rate_value();

            /* go to the next transition */
            trans = trans_list->show_next_trans();
        }
        /* get the maximum transition rate */
        if (trans_rate > unif_rate)
            unif_rate = trans_rate;

        known_st = chain->show_next_known_st();
    }

    /* add a delta factor to the uniformization rate */
    unif_rate += 1e-03;

    return (unif_rate);
}

int generate_uniformization_file(TGFLOAT unif_rate)
{
    FILE *fd;
    char  filename[MAXSTRING];

    strcpy (filename, base_filename);
    strcat (filename, ".uniform_rate");

    if ( (fd = fopen (filename, "w")) == NULL) {
        perror ("fopen");
        return (-1);
    }

    fprintf (fd, "%.10e\n",unif_rate);

    fclose(fd);
    return(1);
}

int uniformize_impulse_rewards( TGFLOAT unif_rate )
{
    Known_State      *known_st;
    Transition_List  *trans_list;
    Transition       *trans;
    Imp_Reward_List  *imp_rew_list;
    Imp_Reward       *imp_rew;
    Imp_Rew_Val_List *imp_rew_val_list;
    Imp_Rew_Val      *imp_rew_val;
    Expr_Val         *imp_prob;
    int               st_id_org, st_id_dest;
    TGFLOAT            trans_rate, imp_prob_val;

    /* get the first known state of the markov chain */
    known_st = markov_chain->show_1st_known_st();

    /* go through all known states */
    while( known_st != NULL )
    {
        /* for each known_state get the transition list */
        trans_list = known_st->show_trans_list(LIST_RO);
        trans      = trans_list->show_1st_trans();
        /* get the number of the known state */
        st_id_org  = known_st->show_st_id();
        /* go through all the transitions of this known_state */
        while( trans != NULL )
        {
            /* get the number of the destination state of this trans */
            st_id_dest = trans->show_st_dest();
            /* set the correct uniformization rate */
            if( st_id_org == st_id_dest )
                trans_rate = unif_rate -
                 (known_st->show_trans_rate_value() - trans->show_rate_value());
            else
                /* get the rate of this transition */
                trans_rate = trans->show_rate_value();

            /* show the impulse reward list of this transition */
            imp_rew_list = trans->show_imp_rew_list( LIST_RO );
            imp_rew      = imp_rew_list->show_1st_rew();
            /* go through all impulse rewards defined for this trans */
            while( imp_rew != NULL )
            {
                imp_rew_val_list = imp_rew->show_imp_val_list(LIST_RO);
                imp_rew_val      = imp_rew_val_list->show_1st_rew();
                while( imp_rew_val != NULL )
                {
                    /* change the prob (that has the rate) to its correct value */
                    imp_prob     = imp_rew_val->show_prob();
                    imp_prob_val = imp_prob->show_value() / trans_rate;
                    imp_prob     = new Expr_Val (imp_prob_val);
                    imp_rew_val->ch_prob(imp_prob);
                    /* go to the next impulse reward value */
                    imp_rew_val  = imp_rew_val_list->show_next_rew();
                }
                delete imp_rew_val_list;

                /* go to the next impulse reward defined for this trans */
                imp_rew = imp_rew_list->show_next_rew();
            }
            delete imp_rew_list;

            /* go to the next transition */
            trans = trans_list->show_next_trans();
        }
        delete trans_list;

        /* go to the next known state */
        known_st = markov_chain->show_next_known_st();
    }
    return( 1 );
}

int generate_output_files()
{
    TGFLOAT       unif_rate = 0.0;
    char         str[MAXSTRING];
    String_List *str_list;

    if (generate_all_parameters() < 0)
        return (-1);
    debug (3, "<base_name>.parameter file generated\n");

    if (generate_states_file() < 0)
        return (-1);
    debug (3, "<base_name>.states file generated\n");


    /* generate the state transtition Matrix */
    str_list = new String_List();

    if (generate_mtx_file(str_list) < 0)
        return (-1);
    debug (3, "<base_name>.generator_mtx file generated\n");

    if (generate_parameter_file(".generator_mtx.expr", str_list) < 0)
        return (-1);
    debug (3, "<base_name>.generator_mtx.expr file generated\n");

    delete str_list;


    /* if rate rewards were defined, then generate the output files */
    if (the_system_desc->show_rate_reward() == TRUE) {

        str_list = new String_List();

        if (generate_rate_reward_files(str_list) < 0)
            return (-1);
        debug (3, "<base_name>.reward. rate files generated\n");

        if (generate_parameter_file(".rate_reward.expr", str_list) < 0)
            return (-1);
        debug (3, "<base_name>.rate_reward.expr file generated\n");

        delete str_list;
    }


    /* if the chain is numerical, then generate the uniformization and .NM.st_trans_prob_mtx files */
    if (the_system_desc->show_chain_type() == NUMERICAL) {

        if ( (unif_rate = calculate_uniform_rate(markov_chain)) <= 0)
            return(-1);
        sprintf (str, "uniform rate calculated: %.10e\n", unif_rate);
        debug (3, str);

        if (generate_uniformization_file(unif_rate) < 0)
            return (-1);
        debug (3, "<base_name>.uniform_rate file generated\n");

        if (the_system_desc->show_impulse_reward() == TRUE) {

            if (uniformize_impulse_rewards(unif_rate) < 0)
                return(-1);
            debug (3, "rate impulse matrix uniformized\n");
        }
    }

    /* generate the DET files */
    if (the_system_desc->show_det_model()) {
        str_list = new String_List();

        if (generate_trans_prob_mtx_file(unif_rate) < 0)
            return (-1);
        debug (3, "<base_name>.NM.st_trans_prob_mtx file generated\n");

        if (generate_det_files(str_list) < 0)
            return (-1);
        debug (3, "<base_name>.NM.states_det_ev file generated\n");
        debug (3, "<base_name>.NM.embedded_points file generated\n");

        if (generate_parameter_file(".NM.embedded_points.expr", str_list) < 0)
            return (-1);
        debug (3, "<base_name>.NM.embedded_points.expr file generated\n");

        delete str_list;

        if (generate_chains_btwn_embd_pts(unif_rate,
                                          the_system_desc->show_no_st_var(),
                                          the_system_desc->show_no_det_ev() + 1,
                                          markov_chain->show_no_elem()) < 0)
            return (-1);
        debug (3, "<base_name>.NM.chns_betw_embed_pnts file generated\n");
    }


    /* if impulse rewards were defined, then generate the output files */
    if (the_system_desc->show_impulse_reward() == TRUE) {

        str_list = new String_List();

        if (generate_imp_reward_files(str_list) < 0)
            return (-1);
        debug (3, "<base_name>.impulse_reward. impulse files generated\n");

        if (generate_parameter_file(".impulse_reward.expr", str_list) < 0)
            return (-1);
        debug (3, "<base_name>.impulse_reward.expr file generated\n");

        delete (str_list);
    }

    return (1);
}
