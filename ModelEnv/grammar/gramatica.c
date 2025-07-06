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

//------------------------------------------------------------------------------
//        T A N G R A M - I I      << . >>      L A N D - U F R J - B R A Z I L
//------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gramatica.h"

/* current parse status */
extern t_status current_status;
extern t_include_file  include_file[];
extern int             include_stack_ptr;

void yywarning( const char * );

/* global declaration of all structures used in parsing */
t_code                 code_tb[MAX_TABLE_ENTRIES];
t_obj_desc             obj_desc_tb[MAX_TABLE_ENTRIES];
t_symbol_obj           sym_tb_obj[MAX_TABLE_ENTRIES];
t_symbol_ccode         sym_tb_ccode[MAX_TABLE_ENTRIES];
t_ref_sym              ref_sym_tb[MAX_TABLE_ENTRIES];
t_global_rew           global_rew_tb[MAX_TABLE_ENTRIES];
t_indep_chain          indep_chain_tb[MAX_TABLE_ENTRIES];
t_delayed_declarations delayed_tb[ MAX_TABLE_ENTRIES ];
t_reserved_word        reserved_word_tb[] = {
    { "Object_Desc" },
    { "Declaration" },
    { "Var" },
    { "Const" },
    { "Param" },
    { "StateFloat" },
    { "State" },
    { "Float" },
    { "Integer" },
    { "Object" },
    { "Port" },
    { "port" },
    { "Reward" },
    { "Decl_values" },
    { "Events" },
    { "event" },
    { "exp" },
    { "det" },
    { "condition" },
    { "action" },
    { "prob" },
    { "Messages" },
    { "msg_rec" },
    { "msg" },
    { "msg_data" },
    { "msg_type" },
    { "msg_size" },
    { "get_msg_data" },
    { "Rewards" },
    { "rate_reward" },
    { "impulse_reward" },
    { "value" },
    { "TRUE" },
    { "FALSE" },
    { "int" },
    { "long" },
    { "double" },
    { "float" },
    { "if" },
    { "else" },
    { "while" },
    
    { "switch" },
    { "case" },
    { "break" },
    { "default" },
    { "for" },
    { "get_cr" },
    { "set_cr" },
    { "get_ir" },
    { "set_ir" },
    { "unset_ir" },
    { "rate_reward_sum" },
    { "rewards" },
    { "set_st" },
    { "get_st" },
    { "get_simul_time" },
    { "save_at_head" },
    { "save_at_tail" },
    { "restore_from_head" },
    { "restore_from_tail" },
    { "copy_vector" },
    { "pow" },
    { "log" },
    { "log10" },
    { "printf" },
    { "fprintf" },
    { "sqrt" },
    { "acos"  },
    { "asin"  },
    { "atan"  },
    { "atan2" },
    { "cosh"  },
    { "sinh"  },
    { "tanh"  },
    { "frexp" },
    { "ldexp" },
    { "modf"  },
    { "ceil"  },
    { "fabs"  },
    { "floor" },
    { "fmod"  },


    { "ev_name_param" }, /* internal usage in code_generator.c */
    { "obj_st" },        /* internal usage in code_generator.c */
    { "\0" }
};
//------------------------------------------------------------------------------
//        F U N C T I O N S
//------------------------------------------------------------------------------
void initialize_current_status()
{

    current_status.obj_desc_index = 0;
    current_status.func_hand      = 1;

    /* Allocate memory for action code */
    current_status.code_size = INITIAL_CODE_SIZE;
    current_status.code      = (char *)malloc( current_status.code_size + 1 );
    current_status.code[ 0 ] = 0;

    strcpy( current_status.last_line   , "" );
    strcpy( current_status.current_line, "" );
    
    reset_current_status();
}
//------------------------------------------------------------------------------
void reset_current_status()
{
    strcpy( current_status.obj_name   , "" );
    strcpy( current_status.ev_msg_name, "" );

    strcpy( current_status.last_line   , "" );
    strcpy( current_status.current_line, "" );

    current_status.current_part = DECL_PART;

    current_status.msg_data = 1;

    strcpy( current_status.reward_name, "" );
    current_status.msg_number        = 1;
    current_status.code_index        = 0;
    current_status.act_number        = 0;
    current_status.sym_obj_index     = 0;
    current_status.sym_ccode_index   = 0;
    current_status.ref_sym_index     = 0;
    current_status.global_rew_index  = 0;
    current_status.indep_chain_index = 0;
    current_status.delayed_tb_index  = 0;
    current_status.has_init_event    = 0;
    current_status.tgif_page         = 1;       

    current_status.app_code = 0;
    strcpy( current_status.code, "" );
    current_status.curr_ev_type = NON_EV;
}
//------------------------------------------------------------------------------
int str_len( char *s )
{
    if( s != NULL )
        return( strlen( s ) );
    else
        return( 0 );
}
//------------------------------------------------------------------------------
char *aloc_str( char *str )
{
    char *s;

    s = (char *)malloc( str_len( str ) + 1 );
    if( s != NULL )
        strcpy( s, str );
    else
        fprintf( stderr, "ERROR: not enough memory(%d)\n", str_len( str ) + 1 );

    return( s );
}
//------------------------------------------------------------------------------
char *concat_str(char *s1, char *s2, char *s3)
{
    char *s;
    int   size;

    size = str_len(s1) + str_len(s2) + str_len(s3);
    s = (char *) malloc (size + 1);
    s[ 0 ] = 0;
    if (s1 != NULL)
        strcpy (s, s1);
    if (s2 != NULL)
        strcat (s, s2);
    if (s3 != NULL)
        strcat (s, s3);
    return (s);
}
//------------------------------------------------------------------------------
int update_code_tb_msg (char *a_port)
{
    int aux;
    char port[MAX_STRING];

    /* remove spaces from indentifier */
    strcpy (port, a_port);
    sym_trim(port);

    /* current position on the table */
    aux = current_status.code_index;

    code_tb[aux].part = MESSAGES_PART;
    code_tb[aux].type = MSG_REC_TYPE;

    sprintf (code_tb[aux].name, "MSG_%d", current_status.msg_number);
    strcpy (code_tb[aux].msg_port, port);

    /* inc the number of messages */
    current_status.msg_number++;
     
    /* reset the action number */
    current_status.act_number = 0;

    /* verify and update next free position in table */
    if ( current_status.code_index < MAX_TABLE_ENTRIES)
        current_status.code_index++;    
    else
    {
        fprintf( stderr, "Error: No space left on code table!\n");
        return( -1 );
    }
    return( 1 );
}
//------------------------------------------------------------------------------
int update_code_tb_impulse(int type, char *a_rew_name, char *a_ev_name, char *act_list)
{
    char rew_name[MAX_STRING], ev_name[MAX_STRING];
    int aux;

    /* remove spaces from indentifier */
    strcpy (rew_name, a_rew_name);
    sym_trim (rew_name);
    strcpy (ev_name, a_ev_name);
    sym_trim (ev_name);

    /* current position on the table */
    aux = current_status.code_index;

    code_tb[aux].part = REWARDS_PART;
    code_tb[aux].type = type;

    strcpy (code_tb[aux].name, rew_name);
    strcpy (code_tb[aux].ev_name, ev_name);
    strcpy (code_tb[aux].act_list, act_list);

    /* verify and update next free position in table */
    if ( current_status.code_index < MAX_TABLE_ENTRIES)
        current_status.code_index++;    
    else
    {
        fprintf ( stderr, "No space left on code table!\n");
        return(-1);
    }
    return(1);
}
//------------------------------------------------------------------------------
int update_code_tb_bounds(char *rew_name, char *a_low_bound, char *a_upp_bound)
{
    char low_bound[MAX_STRING], upp_bound[MAX_STRING];
    int i, aux;

    /* remove spaces from the identifier */
    strcpy (low_bound, a_low_bound);
    sym_trim(low_bound);
    strcpy (upp_bound, a_upp_bound);
    sym_trim(upp_bound);

    /* current position on the table */
    aux = current_status.code_index;

    code_tb[aux].part = REWARDS_PART;
    code_tb[aux].type = BOUND_TYPE;
    strcpy (code_tb[aux].name, rew_name);
    
    /* set the lower bound */
    if ( (i = sym_tb_obj_query(low_bound)) >= 0)
    {
        if (sym_tb_obj[i].type == FLOAT_CONST_TYPE)
            code_tb[aux].low_bound = sym_tb_obj[i].val.f;
        else
            code_tb[aux].low_bound = sym_tb_obj[i].val.i;
        code_tb[aux].low_bounded = 1;
    }
    else
    {
        if (!strcmp(low_bound, "INF"))
        {
            code_tb[aux].low_bound   = 0;
            code_tb[aux].low_bounded = 0;
        }
        else
        {
            code_tb[aux].low_bound   = atof(low_bound);
            code_tb[aux].low_bounded = 1;
        }
    }

    /* set the upper bound */
    if ( (i = sym_tb_obj_query(upp_bound)) >= 0)
    {
        if (sym_tb_obj[i].type == FLOAT_CONST_TYPE)
            code_tb[aux].upp_bound = sym_tb_obj[i].val.f;
        else
            code_tb[aux].upp_bound = sym_tb_obj[i].val.i;
        code_tb[aux].upp_bounded = 1;    
    }
    else
        if (!strcmp(upp_bound, "INF"))
        {
            code_tb[aux].upp_bound = 0;
            code_tb[aux].upp_bounded = 0;
        }
        else
        {
            code_tb[aux].upp_bound = atof(upp_bound);
            code_tb[aux].upp_bounded = 1;
        }

    /* verify and update next free position in table */
    if ( current_status.code_index < MAX_TABLE_ENTRIES)
        current_status.code_index++;    
    else
    {
        fprintf ( stderr, "Error: No space left on code table!\n");
        return(-1);
    }
    return(1);
}
//------------------------------------------------------------------------------
int update_code_tb_cr_init_val(char *rew_name, char *a_cr_in_val)
{
    char cr_initial_value[MAX_STRING];
    int i, aux;

    /* remove spaces from the identifier */
    strcpy (cr_initial_value, a_cr_in_val);
    sym_trim(cr_initial_value);

    /* current position on the table */
    aux = current_status.code_index;

    code_tb[aux].part = REWARDS_PART;
    code_tb[aux].type = REWARD_CR_INIT_VAL_TYPE;
    strcpy (code_tb[aux].name, rew_name);

    /* set initial value */
    if ( (i = sym_tb_obj_query(cr_initial_value)) >= 0)
    {
        if (sym_tb_obj[i].type == FLOAT_CONST_TYPE)
            code_tb[aux].cr_init_val = sym_tb_obj[i].val.f;
        else
            code_tb[aux].cr_init_val = sym_tb_obj[i].val.i;
    }
    else
        code_tb[aux].cr_init_val = atof(cr_initial_value);

    /* verify and update next free position in table */
    if ( current_status.code_index < MAX_TABLE_ENTRIES)
        current_status.code_index++;    
    else
    {
        fprintf ( stderr, "No space left on code table!\n");
        return(-1);
    }
    return(1);
}
//------------------------------------------------------------------------------
int update_code_tb (int part, char *name, int type, char *code)
{
    int   aux;
    char *str, *pos;

    /* current position on the table */
    aux = current_status.code_index;

    /* update the part and the type of entry */
    if( part != 0 )
        code_tb[aux].part = part;
    else
        code_tb[aux].part = code_tb[aux-1].part;

    code_tb[aux].type = type;

    /* if there is no name, copy the previous name to this entry */
    if( name != NULL )
        strcpy(code_tb[aux].name, name);
    else
        strcpy(code_tb[aux].name, code_tb[aux-1].name);

    if( type == ACT_TYPE )
    {
        /* update the action number */
        current_status.act_number++;
        code_tb[aux].act_number = current_status.act_number;

        str = (char *)malloc( strlen( current_status.code ) + 1 );
        strcpy( str, current_status.code );
        code_tb[aux].code = str;

        /* actions are always executed */
        strcpy (code_tb[aux].expression, "");
        code_tb[aux].expr_type = EXEC_TYPE;
        /* update the function handler number for this code entry */
        code_tb[aux].func_hand = current_status.func_hand;
        current_status.func_hand++;
        /* compose and fill out the func name field */
        compose_func_name(aux);
    }

    if( type == ACT_PROB_TYPE )
    {
        /* update the action number */
        current_status.act_number++;
        code_tb[aux].act_number = current_status.act_number;
        code_tb[aux].type = ACT_TYPE;

        pos = strrchr (current_status.code, (int) ':');
        if (pos != NULL)
            *pos = '\0';  

        str = (char *)malloc( strlen( current_status.code ) + 1 );
        strcpy( str, current_status.code );
        code_tb[aux].code = str;

        /* actions are always executed */
        strcpy (code_tb[aux].expression, "");
        code_tb[aux].expr_type = EXEC_TYPE;
        /* update the function handler number for this code entry */
        code_tb[aux].func_hand = current_status.func_hand;
        current_status.func_hand++;
        /* compose and fill out the func name field */
        compose_func_name(aux);
    }

    if( type == REWARD_VALUE_TYPE )
    {    
        /* update the action number */
        code_tb[aux].act_number = current_status.act_number;
        current_status.act_number++;

        /* copy the code */
        str = (char *) malloc ( strlen(code) + 1);
        strcpy (str, code);
        code_tb[aux].code = str;

        /* check to see if a parameter is being used */
        if (check_parameter_sym(str))
        {
            if (check_vector_sym(str))
            {
                error_handler(ERR_VEC_LITERAL, str);
                return(-1);
            }
            
            strcpy (code_tb[aux].expression, code_tb[aux].code);
            code_tb[aux].expr_type = INTERP_TYPE;
        }
        else
        {
            strcpy (code_tb[aux].expression, "");
            code_tb[aux].expr_type = EXEC_TYPE;

            /* update the function handler number for this code entry */
           code_tb[aux].func_hand = current_status.func_hand;
           current_status.func_hand++;
           /* compose and fill out the func name field */
           compose_func_name(aux);
        }
    }

    /* if it is an event entry then update the distrib name */
    if (type == DISTRIB_TYPE)
    {
        /* compose the name of the distribution */
        compose_distrib_name(aux);
    }

    if( type == COND_TYPE || type == PROB_TYPE )
    {
        str = (char *) malloc ( strlen(code) + 1);
        strcpy (str, code);
        code_tb[aux].code = str;

        /* update the prob number related to the action number */
        if( type == PROB_TYPE )
            code_tb[aux].act_number = current_status.act_number;

        /* update the cond number related to the reward */
        if( part == REWARDS_PART && type == COND_TYPE )
            code_tb[aux].act_number = current_status.act_number;

        /* reset the action number */
        if( part == EVENTS_PART && type == COND_TYPE )
            current_status.act_number = 0;

        /* check to see if expression is executable or interpretable */
        if (check_parameter_sym(str))
        {
            if (check_vector_sym(str))
            {
                error_handler(ERR_VEC_LITERAL, str);
                return(-1);
            }
            strcpy (code_tb[aux].expression, code_tb[aux].code);
            code_tb[aux].expr_type = INTERP_TYPE;
        }
        else
        {
            strcpy (code_tb[aux].expression, "");
            code_tb[aux].expr_type = EXEC_TYPE;
    
            /* update the function handler number for this code entry */
            code_tb[aux].func_hand = current_status.func_hand;
            current_status.func_hand++;    
            /* compose and fill out the func name field */
            compose_func_name(aux);
        }
    }

    if (type == REWARD_4SUM_TYPE)
    {
        /* copy the code that represents rew_4sum_name to code_tb[].reward_4sum_name */
        strcpy(code_tb[aux].reward_4sum_name, code);
    }


    /* verify and update next free position in table */
    if ( current_status.code_index < MAX_TABLE_ENTRIES)
        current_status.code_index++;    
    else
    {
        fprintf ( stderr, "No space left on code table!\n");
        return(-1);
    }

    /* turn off code accumulator and empty code segment */
    current_status.app_code = 0;
    strcpy (current_status.code, "");
    return(1);
}
//------------------------------------------------------------------------------
void compose_func_name (int index)
{
    char func[MAX_NAME];

    switch (code_tb[index].part)
    {
       case EVENTS_PART   : strcpy( func, "Eve" ); break;
       case MESSAGES_PART : strcpy( func, "Msg" ); break;
       case REWARDS_PART  : strcpy( func, "Rew" ); break;
    }
    
    switch( code_tb[index].type )
    {
        case COND_TYPE :
            if( code_tb[index].part == REWARDS_PART )
               sprintf( func, "%s_%s_%s_%d", func,
                                             code_tb[index].name, 
                                             "cond",
                                             code_tb[index].act_number );
                                             
            if( code_tb[index].part == EVENTS_PART )
               sprintf( func, "%s_%s_%s", func,
                                          code_tb[index].name,
                                          "cond" );
            break;

        case ACT_TYPE : 
        case ACT_PROB_TYPE :        
        case REWARD_VALUE_TYPE :
            sprintf( func, "%s_%s_%s_%d", func,
                                          code_tb[index].name, 
                                          "act",
                                          code_tb[index].act_number );
            break;

        case PROB_TYPE :
            sprintf( func, "%s_%s_%s_%d", func,
                                          code_tb[index].name,
                                          "prob",
                                          code_tb[index].act_number );
            break;
    }

    strcpy (code_tb[index].func_name, func);
}
//------------------------------------------------------------------------------
void print_code_tb( int code_entries, t_code *code )
{
    int i, part;

    i = 0;
    while( i < code_entries )
    {
        switch( code[i].part )
        {
            case EVENTS_PART   : fprintf( stdout, "Events:\n" );   break;
            case MESSAGES_PART : fprintf( stdout, "Messages:\n" ); break;
            case REWARDS_PART  : fprintf( stdout, "Rewards:\n" );  break;
        }

        part = code[i].part;
        while( part == code[i].part )
        {

            if( code[i].type == ACT_TYPE      ||
                code[i].type == ACT_PROB_TYPE ||            
                code[i].type == COND_TYPE        )
                fprintf( stdout, "%s : %d \n%s\n\n", code[i].func_name,
                                                     code[i].func_hand,
                                                     code[i].code);

            if( code[i].type == PROB_TYPE ||
                code[i].type == REWARD_VALUE_TYPE )
                fprintf ( stdout, "%s : %d -- %d \n%s\n\n", code[i].func_name,
                                                            code[i].expr_type,
                                                            code[i].func_hand,
                                                            code[i].code);
            if( code[i].type == MSG_REC_TYPE )
                fprintf( stdout, "msg_rec: %s\n", code[i].msg_port );

            if( code[i].type == BOUND_TYPE )
                fprintf( stdout, "bounds: \n\treward_name: %s\n\tlow: %d -- val: %f\n\tupp: %d -- val: %f\n",
                         code[i].name,
                         code[i].low_bounded,
                         code[i].low_bound,
                         code[i].upp_bounded,
                         code[i].upp_bound );

            if( code[i].type == REWARD_4SUM_TYPE )
                fprintf( stdout, "reward_4sum: \n\trate_sum_name: %s -- reward_name: %s\n",
                         code[i].name,
                         code[i].reward_4sum_name );

            if( code[i].type == REWARD_EVENT_TYPE ||
                code[i].type == REWARD_MSG_TYPE )
                fprintf( stdout, "impulse: %s , %s\n", code[i].ev_name,
                                                       code[i].act_list);

            if (code[i].type == DISTRIB_TYPE) 
            {
                switch (code[i].distrib_type)
                {
                    case EXPONENTIAL_DIST :
                        fprintf( stdout, "EXPONENTIAL : %d -- %s\n%s\n",
                                 code[i].distrib.exp.func_hand,          
                                 code[i].distrib.exp.func_name,          
                                 code[i].distrib.exp.rate );             
                        break;
                    case DETERMINISTIC_DIST :
                        fprintf( stdout, "DETERMINISTIC : %d -- %s\n%s\n",
                                 code[i].distrib.det.func_hand,
                                 code[i].distrib.det.func_name,
                                 code[i].distrib.det.mean );
                        break;
                    case UNIFORM_DIST :
                        fprintf( stdout, "UNIFORM : \n  lower: %d -- %s\n%s\n",
                                 code[i].distrib.uni.func_hand_low,
                                 code[i].distrib.uni.func_name_low,
                                 code[i].distrib.uni.lower );
                        fprintf( stdout, "  upper: %d -- %s\n%s\n", 
                                 code[i].distrib.uni.func_hand_upp,
                                 code[i].distrib.uni.func_name_upp,
                                 code[i].distrib.uni.upper );
                        break;
                    case GAUSSIAN_DIST :
                        fprintf( stdout, "GAUSSIAN : \n  mean: %d -- %s\n%s\n",
                                 code[i].distrib.gauss.func_hand_mean,
                                 code[i].distrib.gauss.func_name_mean,
                                 code[i].distrib.gauss.mean);
                        fprintf( stdout, "  variance: %d -- %s\n%s\n", 
                                 code[i].distrib.gauss.func_hand_var,
                                 code[i].distrib.gauss.func_name_var,
                                 code[i].distrib.gauss.var );
                        break;
                    case FILE_DIST :
                        fprintf( stdout, "FILE : %s\n",
                                 code[i].distrib.file.filename );
                        break;
                    case FBM_DIST :
                        fprintf( stdout, "FBM : %f %f %d %f %f\n",
                                 code[i].distrib.fbm.mean,
                                 code[i].distrib.fbm.var,
                                 code[i].distrib.fbm.max_level,
                                 code[i].distrib.fbm.time_scale,
                                 code[i].distrib.fbm.hurst );
                        break;
                    case FARIMA_DIST :
                        fprintf( stdout, "FARIMA : %f %f %d %f\n",
                                 code[i].distrib.farima.mean,
                                 code[i].distrib.farima.var,
                                 code[i].distrib.farima.no_samples,
                                 code[i].distrib.farima.hurst );
                        break;
                    case REW_REACH_DIST :
                        fprintf( stdout, "REW_REACH : %f \n",
                                 code[i].distrib.rew_reach.delta_time );
                        break;
                }
            }
            i++;
        }
    }
}
//------------------------------------------------------------------------------
int update_distrib_expr(int type, char *expr1, char *expr2, char *expr3 )
{
    int   aux;
    char *str;

    /* current position on the table */
    aux = current_status.code_index;

    /* update the distribution type */
    code_tb[aux].distrib_type = type;

    /* be default all distributions are of EXEC_TYPE */
    strcpy (code_tb[aux].expression, "");
    code_tb[aux].expr_type = EXEC_TYPE;

    /* check to see if there are any parameters in the expressions */
    if (type != EXPONENTIAL_DIST)
    {
        if (expr1 != NULL)
        {
            if ( check_parameter_sym(expr1) )
            {
               error_handler (ERR_PARAM_IN_EXPR, expr1);
               return (-1);
            }
        }
        
        if (expr2 != NULL)
        {
            if ( check_parameter_sym(expr2) )
            {
                error_handler (ERR_PARAM_IN_EXPR, expr2);
                return (-1);
            }
        }
    }

    switch (type)
    {
        case EXPONENTIAL_DIST :
        str = (char *) malloc ( strlen(expr1) + 1);
        strcpy (str, expr1);
        code_tb[aux].distrib.exp.rate = str;
        /* check to see if expression is executable or interpretable */
        if (check_parameter_sym(code_tb[aux].distrib.exp.rate))
        {
            if (check_vector_sym(code_tb[aux].distrib.exp.rate))
            {
                error_handler(ERR_VEC_LITERAL, str);
                return(-1);
            }
            strcpy (code_tb[aux].expression, code_tb[aux].distrib.exp.rate);
            code_tb[aux].expr_type = INTERP_TYPE;
        }
        else
        {
            strcpy (code_tb[aux].expression, "");
            code_tb[aux].expr_type = EXEC_TYPE;
            /* update the func hand number */
            code_tb[aux].distrib.exp.func_hand = current_status.func_hand;
            current_status.func_hand++;
        }
        break;

        case DETERMINISTIC_DIST :
        str = (char *) malloc ( strlen(expr1) + 1);
        strcpy (str, expr1);
        code_tb[aux].distrib.det.mean = str;
        code_tb[aux].distrib.det.func_hand = current_status.func_hand;
        current_status.func_hand++;
        break;

        case UNIFORM_DIST :
        str = (char *) malloc ( strlen(expr1) + 1);
        strcpy (str, expr1);
        code_tb[aux].distrib.uni.lower = str;
        code_tb[aux].distrib.uni.func_hand_low = current_status.func_hand;
        current_status.func_hand++;
        str = (char *) malloc ( strlen(expr2) + 1);
        strcpy (str, expr2);
        code_tb[aux].distrib.uni.upper = str;
        code_tb[aux].distrib.uni.func_hand_upp = current_status.func_hand;
        current_status.func_hand++;
        break;

        case GAUSSIAN_DIST :
        str = (char *) malloc ( strlen(expr1) + 1);
        strcpy (str, expr1);
        code_tb[aux].distrib.gauss.mean = str;
        code_tb[aux].distrib.gauss.func_hand_mean = current_status.func_hand;
        current_status.func_hand++;
        str = (char *) malloc ( strlen(expr2) + 1);
        strcpy (str, expr2);
        code_tb[aux].distrib.gauss.var = str;
        code_tb[aux].distrib.gauss.func_hand_var = current_status.func_hand;
        current_status.func_hand++;
        break;

        case ERLANG_M_DIST :
        str = (char *) malloc ( strlen(expr1) + 1);
        strcpy (str, expr1);
        code_tb[aux].distrib.erl.rate = str;
        code_tb[aux].distrib.erl.func_hand_rate = current_status.func_hand;
        current_status.func_hand++;
        str = (char *) malloc ( strlen(expr2) + 1);
        strcpy (str, expr2);
        code_tb[aux].distrib.erl.stages = str;
        code_tb[aux].distrib.erl.func_hand_stages = current_status.func_hand;
        current_status.func_hand++;
        break;

        case LOGNORMAL_DIST :
        str = (char *) malloc ( strlen(expr1) + 1);
        strcpy (str, expr1);
        code_tb[aux].distrib.log_norm.mean = str;
        code_tb[aux].distrib.log_norm.func_hand_mean = current_status.func_hand;
        current_status.func_hand++;
        str = (char *) malloc ( strlen(expr2) + 1);
        strcpy (str, expr2);
        code_tb[aux].distrib.log_norm.var = str;
        code_tb[aux].distrib.log_norm.func_hand_var = current_status.func_hand;
        current_status.func_hand++;
        break;

        case WEIBULL_DIST :
        str = (char *) malloc ( strlen(expr1) + 1);
        strcpy (str, expr1);
        code_tb[aux].distrib.weib.scale = str;
        code_tb[aux].distrib.weib.func_hand_scale = current_status.func_hand;
        current_status.func_hand++;
        str = (char *) malloc ( strlen(expr2) + 1);
        strcpy (str, expr2);
        code_tb[aux].distrib.weib.shape = str;
        code_tb[aux].distrib.weib.func_hand_shape = current_status.func_hand;
        current_status.func_hand++;
        break;

        case PARETO_DIST :
        str = (char *) malloc ( strlen(expr1) + 1);
        strcpy (str, expr1);
        code_tb[aux].distrib.par.scale = str;
        code_tb[aux].distrib.par.func_hand_scale = current_status.func_hand;
        current_status.func_hand++;
        str = (char *) malloc ( strlen(expr2) + 1);
        strcpy (str, expr2);
        code_tb[aux].distrib.par.shape = str;
        code_tb[aux].distrib.par.func_hand_shape = current_status.func_hand;
        current_status.func_hand++;
        break;

        case TRUNC_PARETO_DIST :
        str = (char *) malloc ( strlen(expr1) + 1);
        strcpy (str, expr1);
        code_tb[aux].distrib.trunc_par.scale = str;
        code_tb[aux].distrib.trunc_par.func_hand_scale = current_status.func_hand;
        current_status.func_hand++;
        str = (char *) malloc ( strlen(expr2) + 1);
        strcpy (str, expr2);
        code_tb[aux].distrib.trunc_par.shape = str;
        code_tb[aux].distrib.trunc_par.func_hand_shape = current_status.func_hand;
        current_status.func_hand++;
        str = (char *) malloc ( strlen(expr3) + 1);
        strcpy (str, expr3);
        code_tb[aux].distrib.trunc_par.maximum = str;
        code_tb[aux].distrib.trunc_par.func_hand_maximum = current_status.func_hand;
        current_status.func_hand++;
        break;

        case FILE_DIST :
        strcpy (code_tb[aux].distrib.file.filename, expr1);
        break;

        case REW_REACH_DIST :
        // This is not a distribution indeed. It represents the REWARD_REACHED event type.
        code_tb[aux].distrib.rew_reach.delta_time = 0;
        current_status.func_hand++;
        break;
        
        case INIT_DIST :
        // This is not a distribution indeed. It makes an event turn into a initialization event
        current_status.func_hand++;
        break;        
    }
    return (1);
}
//------------------------------------------------------------------------------
int update_distrib_num(int type, char *expr1, char *expr2, char *expr3, 
                         char *expr4, char *expr5)
{
    int   aux;

    /* current position on the table */
    aux = current_status.code_index;

    /* update the distribution type */
    code_tb[aux].distrib_type = type;

    /* the following distributions are of NUMERIC_TYPE */
    strcpy (code_tb[aux].expression, "");
    code_tb[aux].expr_type = NUMERIC_TYPE;

    switch (type)
    {
        case FBM_DIST :
        code_tb[aux].distrib.fbm.mean = atof(expr1);
        code_tb[aux].distrib.fbm.var =  atof(expr2);
        code_tb[aux].distrib.fbm.max_level = atoi(expr3);
        code_tb[aux].distrib.fbm.time_scale = atof(expr4);
        code_tb[aux].distrib.fbm.hurst = atof(expr5);
        break;

        case FARIMA_DIST :
        code_tb[aux].distrib.farima.mean = atof(expr1);
        code_tb[aux].distrib.farima.var =  atof(expr2);
        code_tb[aux].distrib.farima.no_samples = atoi(expr3);
        code_tb[aux].distrib.farima.time_scale = atof(expr4);
        code_tb[aux].distrib.farima.hurst = atof(expr5);
        break;
    }
    return(1);
}
//------------------------------------------------------------------------------
void compose_distrib_name(int index)
{
    char func[MAX_NAME];

    switch (code_tb[index].distrib_type)
    {
        case EXPONENTIAL_DIST : 
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "EXP_rate"); 
        strcpy (code_tb[index].distrib.exp.func_name, func);
        break;

        case DETERMINISTIC_DIST : 
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "DET_mean"); 
        strcpy (code_tb[index].distrib.det.func_name, func);
        break;

        case UNIFORM_DIST : 
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "UNI_lower"); 
        strcpy (code_tb[index].distrib.uni.func_name_low, func);
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "UNI_upper"); 
        strcpy (code_tb[index].distrib.uni.func_name_upp, func);
        break;

        case GAUSSIAN_DIST : 
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "GAUSS_mean"); 
        strcpy (code_tb[index].distrib.gauss.func_name_mean, func);
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "GAUSS_var"); 
        strcpy (code_tb[index].distrib.gauss.func_name_var, func);
        break;

        case ERLANG_M_DIST :
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "ERLANG_M_rate"); 
        strcpy (code_tb[index].distrib.erl.func_name_rate, func);
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "ERLANG_M_stages");
        strcpy (code_tb[index].distrib.erl.func_name_stages, func);
        break;

        case LOGNORMAL_DIST : 
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "LOGNORMAL_mean"); 
        strcpy (code_tb[index].distrib.log_norm.func_name_mean, func);
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "LOGNORMAL_var");
        strcpy (code_tb[index].distrib.log_norm.func_name_var, func);
        break;

        case WEIBULL_DIST :
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "WEIBULL_scale");
        strcpy (code_tb[index].distrib.weib.func_name_scale, func);
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "WEIBULL_shape");
        strcpy (code_tb[index].distrib.weib.func_name_shape, func);
        break;

        case PARETO_DIST :
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "PARETO_scale");
        strcpy (code_tb[index].distrib.par.func_name_scale, func);
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "PARETO_shape");
        strcpy (code_tb[index].distrib.par.func_name_shape, func);
        break;

        case TRUNC_PARETO_DIST :
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "TRUNC_PARETO_scale");
        strcpy (code_tb[index].distrib.trunc_par.func_name_scale, func);
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "TRUNC_PARETO_shape");
        strcpy (code_tb[index].distrib.trunc_par.func_name_shape, func);
        sprintf (func, "Eve_%s_%s", code_tb[index].name, "TRUNC_PARETO_maximum");
        strcpy (code_tb[index].distrib.trunc_par.func_name_maximum, func);
        break;
        
        case INIT_DIST : 
        sprintf( func, "Ini_%s_%s", code_tb[index].name, "INIT" ); 
        strcpy( code_tb[index].distrib.exp.func_name, func );
        break;        
    }
}
//------------------------------------------------------------------------------
void print_global_rew_tb()
{
    int i;

    fprintf ( stdout, "Global Rewards Table \n");
    for (i=0; i<current_status.global_rew_index; i++)
    {
        switch (global_rew_tb[i].type)
        {
            case REWARD_TYPE :
                fprintf ( stdout, "Reward name: %s\n", global_rew_tb[i].name);
                break;
            case BOUND_TYPE :
                fprintf ( stdout, "\tBounds: Low: %d -- %f Upp: %d -- %f\n", 
                    global_rew_tb[i].low_bounded, global_rew_tb[i].low_bound,
                    global_rew_tb[i].upp_bounded, global_rew_tb[i].upp_bound);
                break;
            case COND_TYPE :
                fprintf ( stdout, "\tCondition: %d -- %s\n", global_rew_tb[i].func_hand,
                    global_rew_tb[i].func_name);
                fprintf ( stdout, "\t           %s\n", global_rew_tb[i].code);
                break;
            case REWARD_VALUE_TYPE :
                fprintf ( stdout, "\tValue: %d -- %s\n", global_rew_tb[i].func_hand,
                    global_rew_tb[i].func_name);
                fprintf ( stdout, "\t       %s\n", global_rew_tb[i].code);
                break;
        }
    }
}
//------------------------------------------------------------------------------
void print_indep_chain_tb()
{
    int i;

    fprintf ( stdout, "Independent Chains\n");
    for (i = 0 ; i < current_status.indep_chain_index; i++)
    {
        switch (indep_chain_tb[i].type)
        {
            case EVENT_TYPE :
                fprintf ( stdout, "\nDET Event: %s", indep_chain_tb[i].name);
                break;
            case CHAIN_TYPE :
                fprintf ( stdout, "\n\tChain:\n\t");
                break;
            case OBJECT_TYPE :
                fprintf ( stdout, "%s -- ", indep_chain_tb[i].name);
                break;
        }
    }
    fprintf ( stdout, "\n");
}
//------------------------------------------------------------------------------
void print_tables()
{
    int i;

    for (i=0; i<current_status.obj_desc_index; i++) {

    fprintf ( stdout, "========== Object: %s ============\n", obj_desc_tb[i].name);

    print_code_tb( obj_desc_tb[i].code_entries, obj_desc_tb[i].code_tb );

    sym_tb_obj_print(obj_desc_tb[i].sym_entries, obj_desc_tb[i].sym_tb);
    
    fprintf ( stdout, "=====================================================\n");
    }
}
//------------------------------------------------------------------------------
void sym_trim(char *name)
{
    char *start = name;
    char *end;

    // Skip leading spaces
    while (*start == ' ')
        start++;

    // If the string is all spaces
    if (*start == '\0') {
        *name = '\0';
        return;
    }

    // Find the end of the first word (non-space)
    end = start;
    while (*end != ' ' && *end != '\0')
        end++;

    // Copy the trimmed word to the beginning of name
    memmove(name, start, end - start);
    name[end - start] = '\0';
}
//------------------------------------------------------------------------------
int sym_tb_obj_insert (int type, char *a_name)
{
    char name[MAX_STRING];

    strcpy (name, a_name);

    /* remove spaces from name */
    sym_trim(name);

    if( sym_tb_obj_query( name ) >= 0 )
    {
       error_handler (ERR_DUP_IDENT, name);        
       return (-1);      /* symbol already exists */
    }
    
    if (current_status.sym_obj_index == MAX_TABLE_ENTRIES)
    {
       error_handler (ERR_SYM_TB_OBJ_FULL, NULL);        
       return (-2);      /* symbol table full */
    }

    /* copy symbol information */
    strcpy (sym_tb_obj[current_status.sym_obj_index].name, name);
    sym_tb_obj[current_status.sym_obj_index].type = type;
    sym_tb_obj[current_status.sym_obj_index].initialized = 0;
   
    current_status.sym_obj_index++;
    return( current_status.sym_obj_index - 1 );
}
//------------------------------------------------------------------------------
int sym_tb_obj_query (char *a_name)
{
    int i = 0;
    char name[MAX_STRING];

    strcpy (name, a_name);
    /* remove spaces from name */
    sym_trim(name);

    while (i < current_status.sym_obj_index)
    {
        if (!(strcmp(name, sym_tb_obj[i].name)))
            return (i);
        else
            i++;
    }
    return (-1);
}
//------------------------------------------------------------------------------
int sym_tb_obj_get_type (char *a_name)
{
    int i = 0;
    char name[MAX_STRING];

    strcpy (name, a_name);
    /* remove spaces from name */
    sym_trim(name);

    while (i < current_status.sym_obj_index)
    {
        if (!(strcmp(name, sym_tb_obj[i].name)))
            return (sym_tb_obj[i].type);
        else
            i++;
    }
    return (-1);
}
//------------------------------------------------------------------------------
int sym_tb_obj_get_init_status(char *a_name)
{
    int i = 0;
    char name[MAX_STRING];

    strcpy (name, a_name);
    /* remove spaces from name */
    sym_trim(name);

    while (i < current_status.sym_obj_index)
    {
        if (!(strcmp(name, sym_tb_obj[i].name)))
            return (sym_tb_obj[i].initialized);
        else
            i++;
    }
    return (-1);  
}
//------------------------------------------------------------------------------
int sym_tb_obj_set_init_status(char *a_name, int value)
{
    int i = 0;
    char name[MAX_STRING];

    strcpy (name, a_name);
    /* remove spaces from name */
    sym_trim(name);
    while (i < current_status.sym_obj_index)
    {
        if (!(strcmp(name, sym_tb_obj[i].name)))
        {
             sym_tb_obj[i].initialized = value;
             break;
        }
        else
            i++;
    }
    return (-1);  
}
//------------------------------------------------------------------------------
int sym_tb_obj_set_val (char *name, char *value)
{
    int i;

    i = sym_tb_obj_query( name );
    if( i >= 0 )
    {
        switch( sym_tb_obj[ i ].type )
        {
            case STATE_VAR_TYPE : 
                sym_tb_obj[ i ].val.int_vec[ 0 ] = atoi( value );
                break;    

            case STATE_VAR_FLOAT_TYPE : 
                sym_tb_obj[ i ].val.tgf_vec[ 0 ] = atof( value );
                break;    

            case INT_CONST_TYPE :
                sym_tb_obj[ i ].val.i = atoi (value);
                break;

            case FLOAT_CONST_TYPE :
                sym_tb_obj[ i ].val.f = atof (value);
                break;

            case OBJECT_TYPE  :
            case MSG_PORT_TYPE:
                strcpy (sym_tb_obj[i].val.str, value);
                break;
        }
        sym_tb_obj[ i ].initialized = 1;
        return( 1 );
    }
    else
        return (-1);
}
//------------------------------------------------------------------------------
int sym_tb_obj_get_val (char *name, char *value)
{
    int i;
    
    i = sym_tb_obj_query(name);
    if (i >= 0) {
    switch (sym_tb_obj[i].type) {
    case STATE_VAR_TYPE : 
        strcpy (value, sym_tb_obj[i].val.expr);
        break;    
// INSERTED BY BRUNO&KELVIN to have float state variables
    case STATE_VAR_FLOAT_TYPE : 
        strcpy (value, sym_tb_obj[i].val.expr);
        break;    
// END OF INSERTED BY BRUNO&KELVIN to have float state variables
    case INT_CONST_TYPE :
        sprintf (value, "%d", sym_tb_obj[i].val.i);
        break;
    case FLOAT_CONST_TYPE :
        sprintf (value, "%eE", sym_tb_obj[i].val.f);
        break;
    case OBJECT_TYPE  :
        case MSG_PORT_TYPE    :
        strcpy (value, sym_tb_obj[i].val.str);
        break;
    }
    return (1);
    } else
    return (-1);
}
//------------------------------------------------------------------------------
int sym_tb_obj_set_max_val( char *name, int max_val )
{
    int i, status;
    
    status = -1;
    i = sym_tb_obj_query( name );
    if( i >= 0 )
    {
        if( sym_tb_obj[ i ].type == STATE_VAR_TYPE )
        {
            sym_tb_obj[ i ].max_value = max_val;
            status = 1 ;
        }
    }

    return( status );
}

// INSERTED BY BRUNO&KELVIN to have float state variables
int sym_tb_obj_set_max_float_val( char *name, TGFLOAT max_val )
{
    int i;

    i = sym_tb_obj_query( name );
    if( i >= 0 )
        if( sym_tb_obj[ i ].type == STATE_VAR_FLOAT_TYPE )
        {
            sym_tb_obj[ i ].max_value = (int)max_val;
            return( 1 );
        }
    return( -1 );
}

// END OF INSERTED BY BRUNO&KELVIN to have float state variables
//------------------------------------------------------------------------------
int sym_tb_obj_set_dim( char *name, int dimension )
{
    int i, type;
    int status;

    status = -1;
    i = sym_tb_obj_query( name );
    if( i >= 0 )
    {
        type = sym_tb_obj[ i ].type;
        sym_tb_obj[ i ].dimension = dimension;
        if( dimension != UNDEF_ST_VECT_DIM )
        {
            switch( type )
            {
                case STATE_VAR_TYPE:
                case INT_CONST_VEC_TYPE:
                    sym_tb_obj[ i ].val.int_vec = (int *)malloc( sizeof( int )
                                                               * dimension );
                    break;

                case STATE_VAR_FLOAT_TYPE:
                case FLOAT_CONST_VEC_TYPE:
                    sym_tb_obj[ i ].val.tgf_vec = (TGFLOAT *)malloc(
                                                             sizeof( TGFLOAT )
                                                             * dimension );
                    break;
            }
        }
        status = 1;
    }
    return( status );
}
//------------------------------------------------------------------------------
int sym_tb_obj_get_dim(char *name)
{
    int i, status;

    status = -1;
    i      = sym_tb_obj_query( name );
    if( i >= 0 )
        if( (sym_tb_obj[i].type == STATE_VAR_TYPE)          ||
            (sym_tb_obj[i].type == STATE_VAR_FLOAT_TYPE)    ||
            (sym_tb_obj[i].type == STATE_VAR_INTQUEUE_TYPE) ||
            (sym_tb_obj[i].type == STATE_VAR_FLOATQUEUE_TYPE) )
        {
            status = sym_tb_obj[ i ].dimension;
        }

    return( status );
}
//------------------------------------------------------------------------------
int sym_tb_obj_set_vec_val( char *name, char *value_list )
{
    int   i, count;
    char *val;

    i = sym_tb_obj_query( name );
    if( i >= 0 )
    {
        if( sym_tb_obj[ i ].type == STATE_VAR_TYPE     ||
            sym_tb_obj[ i ].type == INT_CONST_VEC_TYPE  )
        {
            if( sym_tb_obj[ i ].dimension <= 0 )
            {
                error_handler( ERR_MUST_SET_DIM, name );
                return( -2 );
            }

            val   = strtok( value_list, "," );
            count = 0;
            while( val != NULL )
            {
                sym_tb_obj[ i ].val.int_vec[ count ] = atoi( val );
                count++;
                val = strtok( NULL, "," );
            }

            if( sym_tb_obj[ i ].dimension == UNDEF_ST_VECT_DIM )
                sym_tb_obj[ i ].dimension = count;

            if( count != sym_tb_obj[ i ].dimension )
            {
                error_handler( ERR_ST_VEC_DIM, name );
                return( -2 );
            }
            sym_tb_obj[ i ].initialized = 1;
            return( 1 );
        }
        else if( sym_tb_obj[ i ].type == STATE_VAR_FLOAT_TYPE ||
                 sym_tb_obj[ i ].type == FLOAT_CONST_VEC_TYPE  )
        {
            if( sym_tb_obj[ i ].dimension <= 0 )
            {
                error_handler( ERR_MUST_SET_DIM, name );
                return( -2 );
            }

            val   = strtok( value_list, "," );
            count = 0;
            while( val != NULL )
            {
                sym_tb_obj[i].val.tgf_vec[ count ] = atof( val );
                count++;
                val = strtok(NULL, ",");
            }

            if( sym_tb_obj[ i ].dimension == UNDEF_ST_VECT_DIM )
                sym_tb_obj[ i ].dimension = count;

            if( count != sym_tb_obj[ i ].dimension )
            {
                error_handler( ERR_ST_VEC_DIM, name );
                return( -2 );
            }
            sym_tb_obj[ i ].initialized = 1;
            return( 1 );
        }
        else
            error_handler( ERR_TYPE_NAME, name );
    }
    return( -1 );
}

/**
 * Returns a comma separated list of values stored on vector
 * represented by symbol
 */
char *sym_tb_obj_get_vec_val( t_symbol_obj *symbol, int type )
{
    register int  i;
    char         *values;
    
    values = (char *)calloc( MAX_STRING, sizeof( char ) );
    
    for( i = 0; i < symbol->dimension; i++ )
    {
        switch( type )
        {
            case INT_CONST_VEC_TYPE:
                sprintf( values, "%s %d",  values, symbol->val.int_vec[i] );
            break;
            
            case FLOAT_CONST_VEC_TYPE:
                sprintf( values, "%s %e", values, symbol->val.tgf_vec[i] );
            break;            
        }
        
        if( i < symbol->dimension - 1 )
            sprintf( values, "%s, ", values );
    }
    
    return values;
}

/******************************************************************************/
/* This function sets all elements of the integer vector 'name' to 'value'.   */
/* It returns 1 on success and -1 otherwise.                                  */
/******************************************************************************/
int sym_tb_obj_set_vec_all( char *name, int value )
{
    int i, j;
    int status;  /* value that will be returned by this function */

    sym_trim( name );

    status = -1;
    i = sym_tb_obj_query( name );
    if( i >= 0 )
    {
        if( sym_tb_obj[ i ].type == STATE_VAR_TYPE )
        {
            if( sym_tb_obj[ i ].dimension == UNDEF_ST_VECT_DIM )
            {
                error_handler( ERR_MUST_SET_DIM, name );
            }
            else
            {
                for( j = 0; j < sym_tb_obj[ i ].dimension; j++ )
                    sym_tb_obj[ i ].val.int_vec[ j ] = value;
                sym_tb_obj[ i ].initialized = 1;
                status = 1;
            }
        }
        else
            error_handler( ERR_TYPE_NAME, name );
    }
    return( status );
}

/******************************************************************************/
/* This function sets all elements of the float vector 'name' to 'value'.     */
/* It returns 1 on success and -1 otherwise.                                  */
/******************************************************************************/
int sym_tb_obj_set_fvec_all( char *name, TGFLOAT value )
{
    int i, j;
    int status;  /* value that will be returned by this function */

    sym_trim( name );

    status = -1;
    i      = sym_tb_obj_query( name );
    if( i >= 0 )
    {
        if( sym_tb_obj[i].type == STATE_VAR_FLOAT_TYPE )
        {
            if( sym_tb_obj[ i ].dimension == UNDEF_ST_VECT_DIM )
            {
                error_handler( ERR_MUST_SET_DIM, name );
            }
            else
            {
                for( j = 0; j < sym_tb_obj[ i ].dimension; j++ )
                    sym_tb_obj[ i ].val.tgf_vec[ j ] = value;
                sym_tb_obj[ i ].initialized = 1;
                status = 1;
            }
        }
        else
            error_handler( ERR_TYPE_NAME, name );
    }
    return( status );
}

//------------------------------------------------------------------------------
int sym_tb_obj_chk_init()
{
    int i;
    
    for( i = 0; i < current_status.sym_obj_index; i++ )
    {
        if( sym_tb_obj[i].type != REWARD_TYPE               &&
            sym_tb_obj[i].type != EVENT_TYPE                &&
            sym_tb_obj[i].type != INT_PARAM_TYPE            &&
            sym_tb_obj[i].type != FLOAT_PARAM_TYPE          &&
            sym_tb_obj[i].type != STATE_VAR_INTQUEUE_TYPE   &&
            sym_tb_obj[i].type != STATE_VAR_FLOATQUEUE_TYPE &&
            sym_tb_obj[i].type != MTK_OBJECT_TYPE            )
        {
            if( sym_tb_obj[i].initialized == 0 )
            {
                error_handler( ERR_VAR_INIT, sym_tb_obj[i].name );
                return( -1 );
            }
        }
    }
    return( 1 );
}
//------------------------------------------------------------------------------
int sym_tb_obj_print (int no_entries, t_symbol_obj *sym_tb)
{
    int i;
    
    for (i=0; i<no_entries; i++) {
    switch (sym_tb[i].type) {
    case STATE_VAR_TYPE : 
        fprintf ( stdout, "STATE_VAR_TYPE - %s - %d - %s\n", sym_tb[i].name, sym_tb[i].max_value,
                                                     sym_tb[i].val.expr);
        break;    
    case INT_CONST_TYPE :
        fprintf ( stdout, "INT_CONST_TYPE - %s - %d\n", sym_tb[i].name, sym_tb[i].val.i);
        break;
    case FLOAT_CONST_TYPE :
        fprintf ( stdout, "FLOAT_CONST_TYPE - %s - %f\n", sym_tb[i].name, sym_tb[i].val.f);
        break;
    case OBJECT_TYPE  :
        fprintf ( stdout, "OBJECT_TYPE - %s - %s\n", sym_tb[i].name, sym_tb[i].val.str);
        break;
        case MSG_PORT_TYPE    :
        fprintf ( stdout, "MSG_PORT_TYPE - %s - %s\n", sym_tb[i].name, sym_tb[i].val.str);
        break;
    case REWARD_TYPE :
        fprintf ( stdout, "REWARD_TYPE - %s\n", sym_tb[i].name);
        break;
    case REWARD_SUM_TYPE :
        fprintf ( stdout, "REWARD_SUM_TYPE - %s\n", sym_tb[i].name);
        break;
    case EVENT_TYPE :
        fprintf ( stdout, "EVENT_TYPE - %s\n", sym_tb[i].name);
        break;
    case INT_PARAM_TYPE :
        fprintf ( stdout, "INT_PARAM_TYPE - %s\n", sym_tb[i].name);
        break;
    case FLOAT_PARAM_TYPE :
        fprintf ( stdout, "FLOAT_PARAM_TYPE - %s\n", sym_tb[i].name);
        break;
    }
    }
    return (1);
}
//------------------------------------------------------------------------------
int sym_tb_ccode_insert( int type, char *a_name )
{
    char name[ MAX_STRING ];

    strcpy (name, a_name);
    if( sym_tb_obj_query( name ) >= 0 )
    {
        error_handler( ERR_DUP_IDENT_OBJ, name );
        return( -1 );      /* symbol already exists in OBJ sym table*/
    }
    
    if ( sym_tb_ccode_query(name) >= 0) {
       error_handler (ERR_DUP_IDENT, name);
       return (-2);      /* symbol already exists */
    }
 
    if (current_status.sym_ccode_index == MAX_TABLE_ENTRIES) {
       error_handler (ERR_SYM_TB_CCODE_FULL, NULL);
       return (-3);      /* symblol table full */
    }

    /* remove spaces from name */
    sym_trim(name);

    strcpy (sym_tb_ccode[current_status.sym_ccode_index].name, name);
    sym_tb_ccode[current_status.sym_ccode_index].type = type;
    
    current_status.sym_ccode_index++;
    return (1);
}
//------------------------------------------------------------------------------
int sym_tb_ccode_query( char *a_name )
{
    int i = 0;
    char name[ MAX_STRING ];

    strcpy( name, a_name );

    /* remove spaces from name */
    sym_trim( name );

    while( i < current_status.sym_ccode_index )
    {
        if( !(strcmp( name, sym_tb_ccode[ i ].name )) )
            return( i );
        else
            i++;
    }
    return( -1 );
}
//------------------------------------------------------------------------------
int sym_tb_ccode_get_type( char *a_name )
{
    int i = 0;
    char name[MAX_STRING];

    strcpy (name, a_name);
    /* remove spaces from name */
    sym_trim(name);

    while (i < current_status.sym_ccode_index)
    {
        if (!(strcmp(name, sym_tb_ccode[i].name)))
            return (sym_tb_ccode[i].type);
        else
            i++;
    }
    return (-1);
}
//------------------------------------------------------------------------------
int sym_tb_ccode_set_dim( char *name, int dimension )
{
    int i;

    if ( ( i = sym_tb_ccode_query (name)) >= 0)
    {
        sym_tb_ccode[i].dimension = dimension;
        return (1);
    }
    return (-1);
}
//------------------------------------------------------------------------------
int sym_tb_ccode_get_dim( char *name )
{
    int i, status;

    status = -1;
    if( (i = sym_tb_ccode_query( name )) >= 0 )
        status = sym_tb_ccode[ i ].dimension;

    return( status );
}
//------------------------------------------------------------------------------
int   sym_tb_ccode_print ()
{
    int i;
    
    for (i=0; i<current_status.sym_ccode_index; i++)
    {
        switch (sym_tb_ccode[i].type)
        {
            case C_INT_TYPE : 
                fprintf ( stdout, "C_INT_TYPE - %s\n", sym_tb_ccode[i].name);
                break;    
            case C_FLOAT_TYPE :
                fprintf ( stdout, "C_FLOAT_TYPE - %s\n", sym_tb_ccode[i].name);
                break;
        }
    }
    return (1);
}
//------------------------------------------------------------------------------
int ref_sym_tb_insert (int a_type, char *a_name)
{
    char name[MAX_STRING];
    int  index;

    strcpy (name, a_name);
    if (ref_sym_tb_query (name, a_type) >= 0)
    {
        /* REWARD_SUM_TYPE must be mutualy exclusive */
        if( a_type == REWARD_4SUM_TYPE )
        {
            error_handler (ERR_DUP_REW_REF, name);
            return (-1);   /* duplicated reference of reward for sum */
        }
        return(1);
    }

    index = current_status.ref_sym_index;
    if (index == MAX_TABLE_ENTRIES)
    {
       error_handler (ERR_REF_SYM_TB_FULL, NULL);
       return (-3);      /* symbol table full */
    }

    /* remove spaces from name */
    sym_trim(name);
    
    /* insert reference in table */
    ref_sym_tb[index].type = a_type;
    strcpy (ref_sym_tb[index].name, name);
    ref_sym_tb[index].part = current_status.current_part;
    strcpy (ref_sym_tb[index].ev_msg_name, current_status.ev_msg_name);
    ref_sym_tb[index].act_number = current_status.act_number;    
    
    current_status.ref_sym_index++;
    return (1);
}
//------------------------------------------------------------------------------
int ref_sym_tb_query( char *a_name, int a_type )
{
    int i = 0;
    char name[MAX_STRING];

    strcpy (name, a_name);

    /* remove spaces from name */
    sym_trim(name);

    while (i < current_status.ref_sym_index)
    {
        if ( (!(strcmp(name, ref_sym_tb[i].name))) && (ref_sym_tb[i].type == a_type) )
            return (i);
        else
            i++;
    }
    return (-1);
}
//------------------------------------------------------------------------------
int update_glob_rew_tb_bounds(char *name, char *low_bound, char *upp_bound)
{
    int index;

    index = current_status.global_rew_index ;
    
    global_rew_tb[index].type = BOUND_TYPE;
    strcpy(global_rew_tb[index].name, name);
    
    /* set the lower bound */
    if (!strcmp(low_bound, "INF"))
    {
        global_rew_tb[index].low_bounded = 0;
        global_rew_tb[index].low_bound = 0;
    }
    else
    {
        global_rew_tb[index].low_bounded = 1;
        global_rew_tb[index].low_bound = atof (low_bound);    
    }

    /* set the upper bound */
    if (!strcmp(low_bound, "INF"))
    {
        global_rew_tb[index].upp_bounded = 0;
        global_rew_tb[index].upp_bound = 0;
    }
    else
    {
        global_rew_tb[index].upp_bounded = 1;
        global_rew_tb[index].upp_bound = atof (upp_bound);
    }

    /* verify and update next free position in table */
    if ( current_status.global_rew_index < MAX_TABLE_ENTRIES)
       current_status.global_rew_index++;    
    else
    {
       fprintf ( stdout, "ERROR: No space left on the global reward table!\n");
       return(-1);
    }
    
    return(1);
}
//------------------------------------------------------------------------------
int query_glob_rew_tb(char *a_name)
{
    int  i;
    char name[MAX_STRING];

    strcpy (name, a_name);
    /* remove spaces from name */
    sym_trim(name);

    for (i=0; i < current_status.global_rew_index; i++)
    if (!(strcmp(name, global_rew_tb[i].name)))
        return (i);

    return (-1);
}
//------------------------------------------------------------------------------
int update_glob_rew_tb(int type, char *name, char *code)
{
    int   index;
    char *str;
    char  func_name[MAX_NAME];

    if (type == REWARD_TYPE)
    {
        if (query_glob_rew_tb(name) >= 0)
        {
            error_handler(ERR_DUP_IDENT, name);
            return(-1);
        }
    }

    index = current_status.global_rew_index;

    global_rew_tb[index].type = type;
    strcpy(global_rew_tb[index].name, name);

    if(code != NULL)
    {
        /* copy the code */
        str = (char *) malloc ( strlen(code) + 1); 
        strcpy (str, code);
        global_rew_tb[index].code = str;

        /* update the action number */
        global_rew_tb[index].act_number = current_status.act_number;
        if (type == REWARD_VALUE_TYPE)
            current_status.act_number++;

        /* prepare a function handler */
        /* update the function handler number for this code entry */
        global_rew_tb[index].func_hand = current_status.func_hand;
        current_status.func_hand++;

        /* compose and fill out the func name field */
        if (type == REWARD_VALUE_TYPE)
            sprintf (func_name, "GlobalReward_%s_%s_%d", global_rew_tb[index].name, "act", 
                 global_rew_tb[index].act_number);
        else
            sprintf (func_name, "GlobalReward_%s_%s_%d", global_rew_tb[index].name, "cond",
                 global_rew_tb[index].act_number);
        strcpy (global_rew_tb[index].func_name, func_name);

    }
    else
    {
        global_rew_tb[index].code = NULL;
        global_rew_tb[index].act_number = 0;
        global_rew_tb[index].func_hand = 0;
        strcpy(global_rew_tb[index].func_name, "");
    }

    /* verify and update next free position in table */
    if ( current_status.global_rew_index < MAX_TABLE_ENTRIES)
       current_status.global_rew_index++;    
    else
    {
       fprintf ( stderr, "ERROR: No space left on the global reward table!\n");
       return(-1);
    }

    return (1);
}
//------------------------------------------------------------------------------
int update_indep_chain_tb(int type, char *name)
{
    int index;

    index = current_status.indep_chain_index;

    indep_chain_tb[index].type = type;
    strcpy(indep_chain_tb[index].name, name);

    /* verify and update next free position in table */
    if ( current_status.indep_chain_index < MAX_TABLE_ENTRIES)
       current_status.indep_chain_index++;    
    else
    {
       fprintf ( stderr, "ERROR: No space left on the independent chain table!\n");
       return(-1);
    }
    return(1);
}
//------------------------------------------------------------------------------
int check_parameter_sym (char *expression)
{
    int   i = 0;
    char  name[MAX_STRING];
    char *result;

    while (i < current_status.sym_obj_index)
    {
        if (sym_tb_obj[i].type == INT_PARAM_TYPE ||
            sym_tb_obj[i].type == FLOAT_PARAM_TYPE)
        {
            sprintf (name, " %s ", sym_tb_obj[i].name);
            result = strstr (expression, name);
            if (result != NULL)
            return (1);
        }
        i++;
    }
    return (0);
}
//------------------------------------------------------------------------------
int check_vector_sym(char *expression)
{
    int   i = 0;
    char  name[MAX_STRING];
    char *result;

    while (i < current_status.sym_obj_index)
    {
        if( (sym_tb_obj[i].type == STATE_VAR_TYPE) && 
            (sym_tb_obj[i].dimension > 1) )
        {        
            sprintf (name, " %s ", sym_tb_obj[i].name);
            result = strstr (expression, name);
            if (result != NULL)
                return (1);
        }
        i++;
    }
    return (0);
}
//------------------------------------------------------------------------------
int check_ref_sym()
{
    int i, no_refs;

    no_refs = current_status.ref_sym_index;
    for (i=0; i<no_refs; i++)
    {
        switch (ref_sym_tb[i].type)
        {
            case REWARD_TYPE :
                if (sym_tb_obj_get_type(ref_sym_tb[i].name) != REWARD_TYPE)
                {
                    fprintf ( stdout, "\n ==== ERROR: while parsing the model ====\n");
                    fprintf ( stdout, "Error   : Identifier should be of reward type.\n");
                    if( include_stack_ptr )
                        fprintf ( stdout, "Included file : %s\n", include_file[include_stack_ptr].filename);
                    else
                        fprintf ( stdout, "Main file     : %s\n", include_file[include_stack_ptr].filename);
                    fprintf ( stdout, "          Check the get_cr, get_ir, set_cr, set_ir functions.\n");
                    fprintf ( stdout, "Object  : %s\n", current_status.obj_name);
                    fprintf ( stdout, "Part    : ");
                    switch (ref_sym_tb[i].part)
                    {
                        case EVENTS_PART         :
                            fprintf ( stdout, "Part    : Events = %s\n", ref_sym_tb[i].ev_msg_name);
                            fprintf ( stdout, "          Action = %d\n", ref_sym_tb[i].act_number);
                            break;
                        case MESSAGES_PART       :
                            fprintf ( stdout, "Part    : Messages = %s\n", ref_sym_tb[i].ev_msg_name);
                            fprintf ( stdout, "          Action = %d\n",   ref_sym_tb[i].act_number);
                            break;
                    }
                    fprintf ( stdout, "Token   : %s\n", ref_sym_tb[i].name);
                    return(-1);
                }
                break;

            case REWARD_4SUM_TYPE :
                if (sym_tb_obj_get_type(ref_sym_tb[i].name) != REWARD_TYPE)
                {
                    fprintf ( stdout, "\n ==== ERROR: while parsing the model ====\n");
                    fprintf ( stdout, "Error   : Identifier should be of reward type.\n");
                    if( include_stack_ptr )
                        fprintf ( stdout, "Included file : %s\n", include_file[include_stack_ptr].filename);
                    else
                        fprintf ( stdout, "Main file     : %s\n", include_file[include_stack_ptr].filename);                    
                    fprintf ( stdout, "          Check rate_reward_sum, rewards=<reward_name>[,<oher_reward_name>]*\n");
                    fprintf ( stdout, "Object  : %s\n", current_status.obj_name);
                    fprintf ( stdout, "Part    : Rewards\n");
                    fprintf ( stdout, "Token   : %s\n", ref_sym_tb[i].name);
                    return(-1);
                }
                break;

            case REWARD_SUM_TYPE :
                if (sym_tb_obj_get_type(ref_sym_tb[i].name) != REWARD_SUM_TYPE)
                {
                    fprintf ( stdout, "\n ==== ERROR: while parsing the model ====\n");
                    fprintf ( stdout, "Error   : Identifier should be of rate_reward_sum type.\n");
                    fprintf ( stdout, "          Check get_cr_sum\n");
                    if( include_stack_ptr )
                        fprintf ( stdout, "Included file : %s\n", include_file[include_stack_ptr].filename);
                    else
                        fprintf ( stdout, "Main file     : %s\n", include_file[include_stack_ptr].filename);                    
                    fprintf ( stdout, "Object  : %s\n", current_status.obj_name);
                    switch( ref_sym_tb[ i ].part )
                    {
                        case EVENTS_PART         :
                            fprintf ( stdout, "Part    : Events = %s\n", ref_sym_tb[i].ev_msg_name);
                            fprintf ( stdout, "          Action = %d\n", ref_sym_tb[i].act_number);
                            break;
                        case MESSAGES_PART       :
                            fprintf ( stdout, "Part    : Messages = %s\n", ref_sym_tb[i].ev_msg_name);
                            fprintf ( stdout, "          Action = %d\n",   ref_sym_tb[i].act_number);
                            break;
                    }
                    fprintf ( stdout, "Token   : %s\n", ref_sym_tb[i].name);
                    return(-1);
                }
                break;

            case STATE_VAR_TYPE :
                break;

            case C_INT_TYPE :
                if (sym_tb_ccode_get_type(ref_sym_tb[i].name) != C_INT_TYPE)
                {
                    fprintf ( stdout, "\n ==== ERROR: while parsing the model ====\n");
                    fprintf ( stdout, "Error   : Identifier in msg_int_vec should be of int type.\n");
                    if( include_stack_ptr )
                        fprintf ( stdout, "Included file : %s\n", include_file[include_stack_ptr].filename);
                    else
                        fprintf ( stdout, "Main file     : %s\n", include_file[include_stack_ptr].filename);                    
                    fprintf ( stdout, "Object  : %s\n", current_status.obj_name);
                    fprintf ( stdout, "Part    : ");
                    switch (ref_sym_tb[i].part)
                    {
                        case EVENTS_PART         :
                            fprintf ( stdout, "Events = %s\n", ref_sym_tb[i].ev_msg_name);
                            fprintf ( stdout, "          Action = %d\n", ref_sym_tb[i].act_number);
                            break;
                        case MESSAGES_PART       :
                            fprintf ( stdout, "Messages = %s\n", ref_sym_tb[i].ev_msg_name);
                            fprintf ( stdout, "Action = %d\n", ref_sym_tb[i].act_number);
                            break;
                    }
                    fprintf ( stdout, "Token   : %s\n", ref_sym_tb[i].name);
                    return(-1);
                }
                break;
        }
    }
    return (1);
}
//------------------------------------------------------------------------------
int check_for_st_var()
{
    int i, j;

    for( i = 0; i < current_status.obj_desc_index; i++ )
    {
        for( j = 0; j < obj_desc_tb[ i ].sym_entries; j++ )
        {
            if( (obj_desc_tb[ i ].sym_tb[ j ].type == STATE_VAR_TYPE) ||
                (obj_desc_tb[ i ].sym_tb[ j ].type == STATE_VAR_FLOAT_TYPE) )
                return( 1 );
        }
    }
    return( 0 );
}
//------------------------------------------------------------------------------
int check_literal_reward(int obj_index, char *rew_name)
{
    int i;
    
    for( i = 0; i < obj_desc_tb[ obj_index ].code_entries; i++ )
    {
        if( !strcmp( rew_name, obj_desc_tb[ obj_index ].code_tb[ i ].name ) )
            if( obj_desc_tb[ obj_index ].code_tb[ i ].expr_type == INTERP_TYPE )
                return( 1 );
    }
    return( 0 );
}

//------------------------------------------------------------------------------
/* check to see if the compound identifier is valid */
/* id: <obj_name>.[<reward_name> | <event_name>]    */
int compound_id_is_valid( char *a_comp_id, int type )
{    
    char  comp_id[ MAX_NAME ];
    char *obj_name, *sec_name;
    int i,j;
    int obj_found = 0, rew_found = 0;

    strcpy(comp_id, a_comp_id);
    obj_name = strtok(comp_id, ".");
    sec_name = strtok(NULL, ".");

    for( i = 0; i < current_status.obj_desc_index; i++ )
    {
        if( !strcmp( obj_name, obj_desc_tb[ i ].name ) )
        {
            obj_found = 1;
            for (j=0; j<obj_desc_tb[i].sym_entries; j++)
            {
                if (!strcmp(sec_name, obj_desc_tb[i].sym_tb[j].name))
                {
                    rew_found = 1;
                    switch (type)
                    {
                        case REWARD_TYPE :
                            if (obj_desc_tb[i].sym_tb[j].type == REWARD_TYPE)
                            {
                                if (!check_literal_reward(i, sec_name))
                                    return(1);
                                else
                                    error_handler(ERR_PARAM_GLOBAL_REW, sec_name);
                            }
                            else
                            {
                                error_handler(ERR_TYPE_REWARD, sec_name);
                                return (-1);
                            }
                        break;
                        
                        case EVENT_TYPE :
                            if (obj_desc_tb[i].sym_tb[j].type == EVENT_TYPE)
                                return(1);
                            else
                            {
                                error_handler(ERR_TYPE_EVENT, sec_name);
                                return (-1);
                            }
                        break;
                    }
                }
            }
        }
    }
    
    if( !obj_found )
    {
        error_handler( ERR_NO_OBJ, obj_name );
        return( -1 );
    }

    switch( type )
    {
        case REWARD_TYPE :
            if( !rew_found )
                error_handler( ERR_NO_REWARD, sec_name );
            break;

        case EVENT_TYPE :
            if( !rew_found )
                error_handler( ERR_NO_EVENT, sec_name );
            break;
    }

    return( -1 );
}
//------------------------------------------------------------------------------
int check_event_distrib_type (char *comp_name, int distrib_type)
{
    char  comp_ident[MAX_NAME];
    char *obj_name, *ev_name;
    int i,j;

    strcpy(comp_ident, comp_name);
    obj_name = strtok(comp_ident, ".");
    ev_name = strtok(NULL, ".");

    for (i=0; i<current_status.obj_desc_index; i++)
    {
        if (!strcmp(obj_name, obj_desc_tb[i].name))
        {
            for (j=0; j<obj_desc_tb[i].code_entries; j++)
            {
                if ( (obj_desc_tb[i].code_tb[j].type == DISTRIB_TYPE) && 
                     (!strcmp(obj_desc_tb[i].code_tb[j].name, ev_name)) )
                {
                    switch (distrib_type)
                    {
                        case DETERMINISTIC_DIST :
                            if (obj_desc_tb[i].code_tb[j].distrib_type == DETERMINISTIC_DIST) 
                                return(1);
                            else 
                                error_handler(ERR_DETER_DIST, ev_name);
                            break;
                    }
                }

            }
        }
    }
    return (0);
}
//------------------------------------------------------------------------------
int obj_desc_tb_insert (char *objname)
{
    int index;
    int no_entries, size;

    if( current_status.obj_desc_index == MAX_TABLE_ENTRIES )
    {
        error_handler( ERR_OBJ_DESC_FULL, NULL );
        return( -1 );
    }

    index = current_status.obj_desc_index;
    strcpy( obj_desc_tb[ index ].name, objname );

    /* alloc memory just enough for all tables */
    /* and copy info from static table to allocated space */
    no_entries = current_status.code_index;
    size       = no_entries * sizeof( t_code );
    obj_desc_tb[ index ].code_tb = (t_code *) malloc( size );
    /* code pointer is also copied and its ok!! */
    memcpy( (char *)obj_desc_tb[ index ].code_tb, (char *)&code_tb, size );
    obj_desc_tb[ index ].code_entries = no_entries;

    no_entries = current_status.sym_obj_index;
    size       = no_entries * sizeof( t_symbol_obj );
    obj_desc_tb[ index ].sym_tb      = (t_symbol_obj *) malloc( size );
    obj_desc_tb[ index ].sym_entries = no_entries;
    memcpy( (char *)obj_desc_tb[ index ].sym_tb, (char *)&sym_tb_obj, size );

    /* verify and update next free position in table */
    if( current_status.obj_desc_index < MAX_TABLE_ENTRIES )
       current_status.obj_desc_index++;    
    else
    {
       fprintf( stderr, "ERROR: No space left on the object description table!\n");
       return( -1 );
    }

    return( 1 );
}
//------------------------------------------------------------------------------
int delayed_tb_insert( char *constname, int index )
{
    int status;

    status = 0;
    if( current_status.delayed_tb_index < MAX_TABLE_ENTRIES )
    {
        strcpy( delayed_tb[ current_status.delayed_tb_index ].constname,
                constname );
        delayed_tb[ current_status.delayed_tb_index++ ].index = index;
        status = 1;
    }
    else
        error_handler( ERR_OBJ_DESC_FULL, NULL );

    return( status ? current_status.delayed_tb_index : -1 );
}
//------------------------------------------------------------------------------
int is_reserved_word(char *identifier)
{
    int i = 0;

    while (strcmp(reserved_word_tb[i].name, "\0"))
    {
        if (!strcmp(reserved_word_tb[i].name, identifier))
            return (-1);
        i++;
    }
    return (1);
}
//------------------------------------------------------------------------------
int resolve_delayed_sym( char *constname )
{
    int i, value;
    char name[ MAX_STRING ];
    char str_value[ MAX_STRING ];

    i = 0;
    strcpy( name, constname );
    sym_trim( name );

    if( sym_tb_obj_get_val( name, str_value ) )
    {
        value = atoi( str_value );
        for( i = 0; i < current_status.delayed_tb_index; i++ )
        {
            if( strcmp( name, delayed_tb[ i ].constname ) == 0 )
            {
                if( sym_tb_obj[delayed_tb[i].index].dimension == UNDEF_ST_VECT_DIM ||
                    sym_tb_obj[delayed_tb[i].index].dimension == value )
                {
                    sym_tb_obj[delayed_tb[i].index].dimension = value;
                    switch( sym_tb_obj[ delayed_tb[ i ].index ].type )
                    {
                        case STATE_VAR_TYPE:
                            sym_tb_obj[ delayed_tb[ i ].index ].val.int_vec = (int *)malloc( sizeof( int ) * value );
                            break;

                        case STATE_VAR_FLOAT_TYPE:
                            sym_tb_obj[ delayed_tb[ i ].index ].val.tgf_vec = (TGFLOAT *)malloc( sizeof( TGFLOAT ) * value );
                            break;
                    }
                }
                else
                {
                    error_handler( ERR_ST_VEC_DIM, sym_tb_obj[delayed_tb[i].index].name );
                    break;
                }
            }
        }
    }

    return( i == current_status.delayed_tb_index );
}
//------------------------------------------------------------------------------
int compare_types( int type1, int type2 )
{
    int status;

    status = 0;
    if( type1 != type2 )
    {
        switch( type1 )
        {
            case STATE_VAR_TYPE:
                if( type2 == C_INT_TYPE || type2 == C_INT_VEC_TYPE )
                    status = 1;
                break;

            case STATE_VAR_FLOAT_TYPE:
                if( type2 == C_FLOAT_TYPE || type2 == C_FLOAT_VEC_TYPE )
                    status = 1;
                break;

            case C_INT_TYPE:
                if( type2 == STATE_VAR_TYPE || type2 == C_INT_VEC_TYPE )
                    status = 1;
                break;

            case C_FLOAT_TYPE:
                if( type2 == STATE_VAR_FLOAT_TYPE || type2 == C_FLOAT_VEC_TYPE )
                    status = 1;
                break;

            case STATE_VAR_INTQUEUE_TYPE:
                if( type2 == C_INTQUEUE_TYPE )
                    status = 1;
                break;

            case STATE_VAR_FLOATQUEUE_TYPE:
                if( type2 == C_FLOATQUEUE_TYPE )
                    status = 1;
                break;
        }
    }
    else
        status = 1;

    return( status );
}
//------------------------------------------------------------------------------
char * error_msg[] = 
{
    "The use of %s\n              "
    "is restricted to simulation.\n              "
    "You cannot use this feature in a Markov model."   , /* ERR_SIMULATION_ONLY */
    "Unknown identifier: %s"                           , /* ERR_UNK_IDENT */
    "Duplicated identifier: %s"                        , /* ERR_DUP_IDENT */
    "Identifier already declared in object: %s"        , /* ERR_DUP_IDENT_OBJ */
    "Duplicated Reward Reference: %s\n              "
    "The rewards reference must be mutually exclusive.", /* ERR_DUP_REW_REF */
    "Object symbol table full."                        , /* ERR_SYM_TB_OBJ_FULL */
    "C code symbol table full."                        , /* ERR_SYM_TB_CCODE_FULL */
    "Object description table full."                   , /* ERR_OBJ_DESC_FULL */   
    "Reference symbol table full."                     , /* ERR_REF_SYM_TB_FULL */ 
    "Invalid assignment. Variable expected: %s"        , /* ERR_INV_ASSIGN */      
    "Variable or parameter not initialized: %s"        , /* ERR_VAR_INIT */        
    "Variable should be of STATE type: %s"             , /* ERR_TYPE_STATE_VAR */  
    "Parameter should be of INTEGER type: %s"          , /* ERR_GET_ST */   
    "Parameter should be of INTEGER type: %s"          , /* ERR_TYPE_INT_CONST */   
    "Variable must be local INTEGER type: %s"          , /* ERR_TYPE_LOCAL_INT */   
    "Variable must be local QUEUE type: %s"            , /* ERR_TYPE_LOCAL_QUEUE */ 
    "Parameter should be of FLOAT type: %s"            , /* ERR_TYPE_FLOAT_CONST */ 
    "Parameter should be of OBJECT type: %s"           , /* ERR_TYPE_OBJECT */      
    "Parameter should be of PORT type: %s"             , /* ERR_TYPE_PORT */        
    "Parameter should be of REWARD type: %s"           , /* ERR_TYPE_REWARD */      
    "Parameter should be of MTK_OBJECT type: %s"       , /* ERR_TYPE_MTK_OBJECT */          
    "Identifier should be of EVENT type: %s"           , /* ERR_TYPE_EVENT */
    "Initialization should be with an integer: %s"     , /* ERR_TYPE_INTEGER */
    "Initialization should be with a number: %s"       , /* ERR_TYPE_NUMBER */
    "Initialization should be with an identifier: %s"  , /* ERR_TYPE_NAME */
    "Using identifier as a reserved word: %s"          , /* ERR_RESER_WORD */
    "Port %s is been treated more than once"           , /* ERR_DUP_MSG */
    "Incorrect use of msg_data."                       , /* ERR_MSG_DATA */
    "Incorrect use of msg_type."                       , /* ERR_MSG_TYPE */
    "Using parameter in a condition expression: %s"    , /* ERR_PARAM_IN_COND */
    "Using parameter in wrong expression: %s"          , /* ERR_PARAM_IN_EXPR */
    "Using parameter in an action: %s"                 , /* ERR_PARAM_IN_ACT */
    "Using object in an expression: %s"                , /* ERR_OBJ_IN_EXPR */
    "Using port in an expression: %s"                  , /* ERR_PORT_IN_EXPR */
    "Using a reward value in an expression requires "
    "the use of get_cr() or get_ir(): %s"              , /* ERR_REW_IN_EXPR */
    "Parameter should be local integer type, "
    "global integer constant, local float type or\n              "
    "global float constant: %s"                        , /* ERR_SET_ST */
    "Assignment can only be done to local "
    "variables: %s"                                    , /* ERR_C_LOCAL_VAR */
    "Vector initialized with wrong declared "
    "dimension: %s"                                    , /* ERR_ST_VEC_DIM */
    "Vector variable should have an index: %s"         , /* ERR_VEC_INDEX */
    "Variable should not have an index: %s"            , /* ERR_VEC_NO_INDEX */
    "Incompatible vector dimension: %s"                , /* ERR_INCOMP_VEC */
    "Vector cannot be used with literal parameters "
    "in an expression: %s"                             , /* ERR_VEC_LITERAL */
    "No object named %s"                               , /* ERR_NO_OBJ */
    "No reward named %s"                               , /* ERR_NO_REWARD */
    "Cannot use literal expressions with global "
    "rewards: %s"                                      , /* ERR_PARAM_GLOBAL_REW */
    "No event named %s"                                , /* ERR_NO_EVENT */
    "No object named %s"                               , /* ERR_INV_OBJ_NAME */
    "Event should have deterministic distribution: %s" , /* ERR_DETER_DIST */
    "Invalid condition expression: %s"                 , /* ERR_REACHED_OP */
    "Delayed symbol table full."                       , /* ERR_DELAYED_TB_FULL */
    "The constant that defines the vector dimension "
    "must be initialized before vector %s."            , /* ERR_MUST_SET_DIM */
    "Variable that defines the vector dimension not "
    "declared"                                         , /* ERR_VECT_DIM_NOT_DECL */
    "Variable that defines the vector dimension must "
    "be an integer or an integer constant"             , /* ERR_INVALID_VECT_DIM_TYPE */
    "copy: parameters differ in type"                  , /* ERR_COPY_DIFFERENT_PARAMENTERS_TYPE */
    "copy: parameters differ in size"                  , /* ERR_COPY_DIFFERENT_PARAMENTERS_DIM */
    "IntegerQueue and FloatQueue variables should "
    "not be initialized: %s"                           , /* ERR_QUEUE_INITIALIZED */
    "Incompatible types: %s"                           , /* ERR_INCOMPATIBLE_TYPES */
    "Full vector initialization must be done in the "
    "syntax: %s[] = <value>"                           , /* ERR_VEC_INIT_ALL */
    "Each object can have only one event with the "
    "INIT parameter: %s"                               , /* ERR_DUP_INIT_EVENT */
};
#define NUMBER_OF_ERRORS (sizeof(error_msg)/sizeof(char*))
//------------------------------------------------------------------------------
char * warning_msg[] = 
{
    "State variables are updated only at the end of an action", /* WARN_SET_ST */
};
#define NUMBER_OF_WARNINGS (sizeof(warning_msg)/sizeof(char*))
//------------------------------------------------------------------------------
void error_handler(int err_num, char *text)
{
    char str[ MAX_STRING ];

    if( err_num >= NUMBER_OF_ERRORS )
        sprintf( str, "Unknown error: %s", text );
    else
        sprintf( str, error_msg[err_num], text );

    yyerror( str );
}
//------------------------------------------------------------------------------
void warning_handler(int warn_num, char *text)
{
    char str[ MAX_STRING ];

    if( warn_num >= NUMBER_OF_WARNINGS )
        sprintf( str, "Unknown warning: %s", text );
    else
        sprintf( str, warning_msg[warn_num], text );

    yywarning( str );
}
//------------------------------------------------------------------------------
char *getMTKTokenName( void )
{
    static int id = 0;
    char *name    = ( char * )calloc( MAX_NAME, sizeof( char ) );
    
    sprintf( name, "tk_%d", id++ );
    
    return name;
}
//------------------------------------------------------------------------------
/**
 * Remove spaces from mtk_obj and prepends TangramObjectName
 */
char *getMTKObjectName( char *mtk_obj )
{
    char *name    = ( char * )calloc( MAX_NAME, sizeof( char ) );

    sym_trim( mtk_obj );

    strcpy( name,  current_status.obj_name );
    strcat( name,  "_"                     );
    strcat( name,  mtk_obj                 );
    
    return name;
}
//------------------------------------------------------------------------------
/**
 * Returns the correct printf/scanf mask, according to var type
 */
char *getParameterMask( const char *var )
{
    register int i;
    char new_type;
    char clean_var[ MAX_NAME ];
    
    /* Removes brackets part from array var */
    for( i = 0; ( var[i] != '\0' ) && ( var[i] != '[' ); i++ )
        clean_var[i] = var[i];
    clean_var[i] = '\0';        

    /* Object variables */
    switch( sym_tb_obj_get_type( clean_var ) )
    {
        case INT_CONST_TYPE :          
        case INT_PARAM_TYPE :
            new_type = 'd';
        break;

        case FLOAT_CONST_TYPE :
        case FLOAT_PARAM_TYPE :
            new_type = 'f';
        break;
    }
    /* C variables */
    switch( sym_tb_ccode_get_type( clean_var ) )
    {
        case C_INT_TYPE :
            new_type = 'd';
        break;

        case C_FLOAT_TYPE :
            new_type = 'f';
        break;
    }
    
    switch( new_type )
    {
        case 'd':
            return "\"%d\"";
        break;
        
        case 'f':
            return "\"%lf\"";
        break;

        /* expressions are always considered as double */
        default:
            return "\"%.15lg\"";
        break;
    }
}
