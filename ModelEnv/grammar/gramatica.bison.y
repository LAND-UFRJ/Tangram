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
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gramatica.h"

#define YYERROR_VERBOSE
#define YYSTYPE char *

#define SET_CONTEXT(context) { current_status.current_part = (context); current_status.line_number = 0; }

#define CHECK_SIMULATION(feature_string) \
  if( mode == CHAIN_GENERATION ) \
  { error_handler( ERR_SIMULATION_ONLY, feature_string ); YYABORT; } \
  else \
  { Can_Generate_Chain = 0; }


/* current token */
extern char *yytext;

/* function called by yacc */
extern int yylex();

/* current parse status */
extern t_status current_status;
extern t_include_file  include_file[];
extern int             include_stack_ptr;

extern int Can_Generate_Chain;
extern int Can_Simulate;

extern int has_mtk_object;

/* Action code recursion depth for improper set_st detection */
static int action_code_depth = 0;
static int set_state_used    = 0;

void yyerror(const char *);
void yywarning(const char *);

%}
//------------------------------------------------------------------------------
//     D E F I N I T I O N S
//------------------------------------------------------------------------------
%token OBJECT_DESC
%token DECL VAR CONST PARAM INITIALIZATION
%token STATE FLOATSTATE INTEGER FLOAT OBJECT PORT PORT_ REWARD
%token INTQUEUE FLOATQUEUE

%token EVENTS EVENT
%token EXP DET UNI GAUSS ERLANG LOGNORM FILE_ FBM FARIMA WEIB PAR TRUNCPAR
%token CONDITION ACTION PROB SET_ST GET_ST CLONE_EV
%token GET_CR SET_CR GET_IR SET_IR UNSET_IR GET_CR_SUM GET_SIMUL_TIME GET_RANDOM
%token MESSAGES MSG_REC MSG MSG_SOURCE MSG_TYPE MSG_SIZE
%token MSG_DATA GET_MSG_DATA ALL_OBJS OBJ_CMP
%token GLOBAL_REWARDS REWARDS BOUNDS RATE_REWARD IMPULSE_REWARD VALUE
%token REWARDS4SUM RATE_REWARD_SUM CR_INIT_VAL
%token INDEP_CHAINS CHAIN
%token REWARD_REACHED UPP_REACHED LOW_REACHED
%token INIT
%token SAVE_AT_HEAD SAVE_AT_TAIL RESTORE_FROM_HEAD RESTORE_FROM_TAIL

%token TRUE_ FALSE_
%token AND OR NOT
%token BAND BOR
%token EQUAL NOTEQUAL
%token LE LT GE GT

%token PLUS MINUS STAR SLASH BACKSLASH MOD RSHIFT LSHIFT INC_OP DEC_OP

%token INT LONG C_FLOAT
%token INF REALNUMBER DIGSEQ REALNUMBEREXP DIGSEQEXP
%token MSG_TYPE_DEFINE

%token EXP_FUNC POW_FUNC SQRT_FUNC LOG_FUNC LOG10_FUNC
%token COS_FUNC SIN_FUNC TAN_FUNC ACOS_FUNC ASIN_FUNC 
%token ATAN_FUNC ATAN2_FUNC  COSH_FUNC SINH_FUNC TANH_FUNC 
%token CEIL_FUNC FABS_FUNC FLOOR_FUNC FMOD_FUNC FREXP_FUNC LDEXP_FUNC

%token COPY

%token MTK_OBJECT MTK_CREATE MTK_DELETE
%token MTK_RUN MTK_COPY MTK_GET MTK_SET

%token ASSIGNMENT C_ASSIGNMENT
%token IF ELSE
%token WHILE DO
%token SWITCH CASE BREAK DEFAULT
%token FOR
%token PRINTF FPRINTF STRING STDOUT STDERR

%token IDENTIFIER COMPOUND_IDENTIFIER MULT_COMPOUND_IDENTIFIER
%token COLON SEMICOLON COMMA QUOTE DOT
%token LBRACES RBRACES LPAREN RPAREN LBRACKET RBRACKET

%token ILLEGAL INTERROG

/* expect one shift/reduce conflict from the if_statement and if_else_statement */
/* %expect 1 */

%%
//------------------------------------------------------------------------------
//     R U L E S
//------------------------------------------------------------------------------

system_desc: model
      	     global_reward_part
  	         indep_chains_part
            ;

/* A model is composed by a list of objects */
model : object_desc_list
        {
          current_status.current_part = GLOBAL_REWARDS_PART;
        }
       ;

/* A list of objects is made by one or more objects definitions */
object_desc_list :  object_desc_list object_definition
                    | object_definition
                 ;

/* An object is defined by a name and a description */
object_definition : OBJECT_DESC obj_name LPAREN description RPAREN
                   {
                       if( check_ref_sym() < 0 )
                           YYABORT;
                       obj_desc_tb_insert( $2 );

                       reset_current_status();
                   }
                   ;

obj_name :  IDENTIFIER { $$ = aloc_str( yytext ); }
         ;

/* The description of an object is composed of five parts */
description : decl_part           { SET_CONTEXT( INITIALIZATION_PART ); }
              initialization_part { SET_CONTEXT( EVENTS_PART );         }
              events_part         { SET_CONTEXT( MESSAGES_PART );       }
              messages_part       { SET_CONTEXT( REWARDS_PART );        }
              rewards_part        { SET_CONTEXT( DECL_PART );           }
   ;

/* The first part is the Declaratoion part. It consist of three parts: variable declaration, */
/* constant declaration and parameter declaration */
decl_part : DECL LBRACES
                 mandatory_variable_decl
                 const_decl
                 param_decl
                 RBRACES
          | DECL LBRACES
                 mandatory_const_decl
                 variable_decl
                 param_decl
                 RBRACES
          | DECL LBRACES
                 param_decl
                 RBRACES
   ;

/* variable declaration part */
mandatory_variable_decl : VAR var_decl_def_list
                        | STATE VAR var_decl_def_list
   ;

variable_decl : VAR var_decl_def_list
              | STATE VAR var_decl_def_list
              |
   ;

var_decl_def_list : var_decl_def_list var_decl_def
                  | var_decl_def
   ;

/* only state variables are allowed in the variables part */
var_decl_def : STATE      COLON state_var_list        SEMICOLON
             | INTEGER    COLON state_var_list        SEMICOLON
             | FLOATSTATE COLON state_float_var_list  SEMICOLON
               {
                   CHECK_SIMULATION("float state variables");
               }
             | FLOAT      COLON state_float_var_list  SEMICOLON
               {
                   CHECK_SIMULATION("float state variables");
               }
             | INTQUEUE   COLON state_intqueue_list   SEMICOLON
               {
                   CHECK_SIMULATION("IntegerQueue state variables");
               }
             | FLOATQUEUE COLON state_floatqueue_list SEMICOLON
               {
                   CHECK_SIMULATION("FloatQueue state variables");
               }
             | MTK_OBJECT COLON mtk_var_list SEMICOLON
               {
                   CHECK_SIMULATION("MTKObject variables");

                   /* Remember mtk object */
                   has_mtk_object = 1;
               }
   ;

/* mtk variable list */
mtk_var_list : mtk_var_list COMMA simple_identifier
               {
                 if( sym_tb_obj_insert( MTK_OBJECT_TYPE, $3 ) < 0 )
                    YYABORT;
                    
                 if( sym_tb_obj_set_dim( $3, 1 ) < 0 )
                    YYABORT;
               }
             | simple_identifier
               {
                 if( sym_tb_obj_insert( MTK_OBJECT_TYPE, $1 ) < 0 )
                    YYABORT;
                    
                 if( sym_tb_obj_set_dim( $1, 1 ) < 0 )
                    YYABORT;
               }
             |
   ;

/* state variable list */
state_var_list : state_var_list COMMA state_var
               | state_var
               |
   ;

/* a state variable is definied as an identifier */
state_var : simple_identifier
            { if (sym_tb_obj_insert (STATE_VAR_TYPE, $1) < 0) YYABORT;
              if (sym_tb_obj_set_dim ($1, 1) < 0) YYABORT;
            }

          | simple_identifier LBRACKET unsigned_integer RBRACKET
            { if (sym_tb_obj_insert (STATE_VAR_TYPE, $1) < 0) YYABORT;
              if (sym_tb_obj_set_dim ($1, atoi($3)) < 0) YYABORT;
            }

          | simple_identifier LBRACKET delayed_identifier RBRACKET
            { int i, delay_id_type;
              if( (i = sym_tb_obj_insert( STATE_VAR_TYPE, $1 )) < 0 )
                  YYABORT;
              delay_id_type = sym_tb_obj_get_type( $3 );
              if( delay_id_type != INT_CONST_TYPE )
              {
                  if( delay_id_type == -1 )
                      error_handler( ERR_VECT_DIM_NOT_DECL, NULL );
                  else
                      error_handler( ERR_INVALID_VECT_DIM_TYPE, NULL );
                  YYABORT;
              }
              if( delayed_tb_insert( $3, i ) < 0 )
                  YYABORT;
              if( sym_tb_obj_set_dim( $1, UNDEF_ST_VECT_DIM ) < 0 )
                  YYABORT;
            }
   ;

/* state variable list */
state_float_var_list : state_float_var_list COMMA state_float_var
                     | state_float_var
                     |
   ;
   
/* a state variable is definied as an identifier */
state_float_var : simple_identifier
            {
              if (sym_tb_obj_insert (STATE_VAR_FLOAT_TYPE, $1) < 0) YYABORT;
              if (sym_tb_obj_set_dim ($1, 1) < 0) YYABORT;
            }

          | simple_identifier LBRACKET unsigned_integer RBRACKET
            {
              if (sym_tb_obj_insert (STATE_VAR_FLOAT_TYPE, $1) < 0) YYABORT;
              if (sym_tb_obj_set_dim ($1, atoi($3)) < 0) YYABORT;
            }

          | simple_identifier LBRACKET delayed_identifier RBRACKET
            {
              int i, delay_id_type;
              if( (i = sym_tb_obj_insert( STATE_VAR_FLOAT_TYPE, $1 )) < 0 )
                  YYABORT;
              delay_id_type = sym_tb_obj_get_type( $3 );
              if( delay_id_type != INT_CONST_TYPE )
              {
                  if( delay_id_type == -1 )
                      error_handler( ERR_VECT_DIM_NOT_DECL, NULL );
                  else
                      error_handler( ERR_INVALID_VECT_DIM_TYPE, NULL );
                  YYABORT;
              }
              if( delayed_tb_insert( $3, i ) < 0 )
                  YYABORT;
              if( sym_tb_obj_set_dim( $1, UNDEF_ST_VECT_DIM ) < 0 )
                  YYABORT;
            }
   ;

state_intqueue_list : state_intqueue_list COMMA state_intqueue_var
             | state_intqueue_var
             |
   ;

/* a state variable is definied as an identifier */
state_intqueue_var : simple_identifier LPAREN unsigned_integer RPAREN
                     {
                       if( sym_tb_obj_insert( STATE_VAR_INTQUEUE_TYPE, $1) < 0)
                           YYABORT;
                       if( sym_tb_obj_set_dim( $1, atoi( $3 ) ) < 0 )
                           YYABORT;
                     }
                   | simple_identifier LPAREN delayed_identifier RPAREN
                     {
                       int i, delay_id_type;
                       if( (i = sym_tb_obj_insert( STATE_VAR_INTQUEUE_TYPE, $1 )) < 0 )
                           YYABORT;
                       delay_id_type = sym_tb_obj_get_type( $3 );
                       if( delay_id_type != INT_CONST_TYPE )
                       {
                           if( delay_id_type == -1 )
                               error_handler( ERR_VECT_DIM_NOT_DECL, NULL );
                           else
                               error_handler( ERR_INVALID_VECT_DIM_TYPE, NULL );
                           YYABORT;
                       }
                       if( delayed_tb_insert( $3, i ) < 0 )
                           YYABORT;
                       if( sym_tb_obj_set_dim( $1, UNDEF_ST_VECT_DIM ) < 0 )
                           YYABORT;
                     }
   ;

state_floatqueue_list : state_floatqueue_list COMMA state_floatqueue_var
                      | state_floatqueue_var
                      |
   ;
/* a state variable is definied as an identifier */
state_floatqueue_var : simple_identifier LPAREN unsigned_integer RPAREN
                       {
                         if( sym_tb_obj_insert( STATE_VAR_FLOATQUEUE_TYPE, $1) < 0)
                             YYABORT;
                         if( sym_tb_obj_set_dim( $1, atoi( $3 ) ) < 0 )
                             YYABORT;
                       }
                     | simple_identifier LPAREN delayed_identifier RPAREN
                       {
                         int i, delay_id_type;
                         if( (i = sym_tb_obj_insert( STATE_VAR_FLOATQUEUE_TYPE, $1 )) < 0 )
                             YYABORT;
                         delay_id_type = sym_tb_obj_get_type( $3 );
                         if( delay_id_type != INT_CONST_TYPE )
                         {
                             if( delay_id_type == -1 )
                                 error_handler( ERR_VECT_DIM_NOT_DECL, NULL );
                             else
                                 error_handler( ERR_INVALID_VECT_DIM_TYPE, NULL );
                             YYABORT;
                         }
                         if( delayed_tb_insert( $3, i ) < 0 )
                             YYABORT;
                         if( sym_tb_obj_set_dim( $1, UNDEF_ST_VECT_DIM ) < 0 )
                             YYABORT;
                       }
   ;

/* constant declaration part */
mandatory_const_decl : CONST const_decl_def_list
   ;

const_decl : CONST const_decl_def_list
           |
   ;
const_decl_def_list : const_decl_def_list const_decl_def
                    | const_decl_def
   ;
/* constants can be one of the following types */
const_decl_def : INTEGER       COLON int_const_list    SEMICOLON
               | FLOAT         COLON float_const_list  SEMICOLON
               | OBJECT        COLON object_const_list SEMICOLON
               | PORT	       COLON port_list         SEMICOLON
   ;
/* All the types above have the same syntax. The comments are only in state variable type */
/* The reason for having many rules is for obtaining the type of the identifiers */

int_const_list  : int_const_list COMMA int_const
                | int_const
                |
   ;
int_const   : simple_identifier
            {
                if( sym_tb_obj_insert( INT_CONST_TYPE, $1 ) < 0 )
                    YYABORT;
            }
            | simple_identifier LBRACKET unsigned_integer RBRACKET
            {
                if( sym_tb_obj_insert( INT_CONST_VEC_TYPE, $1 ) < 0 )
                    YYABORT;
                    
                if( sym_tb_obj_set_dim( $1, atoi( $3 ) ) < 0 )
                    YYABORT;
            }
   ;

float_const_list : float_const_list COMMA float_const
               | float_const
               |
   ;
float_const : simple_identifier
            {
                if( sym_tb_obj_insert( FLOAT_CONST_TYPE, $1 ) < 0 )
                    YYABORT;
            }
            | simple_identifier LBRACKET unsigned_integer RBRACKET
            {
                if( sym_tb_obj_insert( FLOAT_CONST_VEC_TYPE, $1 ) < 0 )
                    YYABORT;
                    
                if( sym_tb_obj_set_dim( $1, atoi( $3 ) ) < 0 )
                    YYABORT;
            }     
   ;

object_const_list : object_const_list COMMA object_const
                  | object_const
                  |
   ;
object_const : IDENTIFIER { if (sym_tb_obj_insert (OBJECT_TYPE, yytext) < 0) YYABORT;  }
   ;

port_list : port_list COMMA port_const
          | port_const
          |
   ;
port_const : IDENTIFIER
             {
                 if( sym_tb_obj_insert( MSG_PORT_TYPE, yytext ) < 0 )
                     YYABORT;
             }
   ;

/* parameter declaration part */
param_decl : PARAM param_decl_def_list
           |
   ;

param_decl_def_list : param_decl_def_list param_decl_def
                    | param_decl_def
   ;

/* parameters can be one of the following type */
param_decl_def : INTEGER       COLON int_param_list     SEMICOLON
               | FLOAT         COLON float_param_list   SEMICOLON
   ;

int_param_list : int_param_list COMMA int_param
             | int_param
             |
   ;
int_param : IDENTIFIER
            {
                if( sym_tb_obj_insert( INT_PARAM_TYPE, yytext ) < 0 )
                   YYABORT;
            }
   ;

float_param_list : float_param_list COMMA float_param
               | float_param
               |
   ;
float_param : IDENTIFIER { if (sym_tb_obj_insert (FLOAT_PARAM_TYPE, yytext) < 0) YYABORT;  }
   ;


/* The second part is the Initialization */
/* Initialization of all Vars and Const  */
initialization_part : INITIALIZATION LBRACES initialization_list RBRACES
                      {
                          if( sym_tb_obj_chk_init() < 0 )
                              YYABORT;
                      }
   ;

/* List of vars and parms definition */
initialization_list : initialization_list initialization_definition
                    | initialization_definition
   ;

/* A vars and parms value definition can be one of these types */
initialization_definition : integer_initialization 
                          | float_initialization
                          | identifier_initialization
                          | vector_initialization
   ;

integer_initialization : identifier ASSIGNMENT integer                  
                  {
                      switch( sym_tb_obj_get_type($1) )
                      {
                          case STATE_VAR_TYPE:
                          case STATE_VAR_FLOAT_TYPE:
                               if( sym_tb_obj_get_dim( $1 ) == UNDEF_ST_VECT_DIM )
                               {
                                   error_handler( ERR_VEC_INIT_ALL, $1 );
                                   YYABORT;
                               }
                          case FLOAT_CONST_TYPE:
                              sym_tb_obj_set_val( $1, $3 );
                              break;
                          case INT_CONST_TYPE:
                              sym_tb_obj_set_val( $1, $3 );
                              if( !resolve_delayed_sym( $1 ) )
                                  YYABORT;
                              break;
                          case STATE_VAR_INTQUEUE_TYPE:
                          case STATE_VAR_FLOATQUEUE_TYPE:
                              error_handler( ERR_QUEUE_INITIALIZED, $1 );
                              YYABORT;
                              break;
                          default:
                              error_handler( ERR_TYPE_NAME, $1 );
                              YYABORT;
                      }
                  }
   ;

float_initialization : identifier ASSIGNMENT float
                  {

                      switch( sym_tb_obj_get_type( $1 ) )
                      {
                          case STATE_VAR_FLOAT_TYPE:
                              if( sym_tb_obj_get_dim( $1 ) == UNDEF_ST_VECT_DIM )
                              {
                                  error_handler( ERR_VEC_INIT_ALL, $1 );
                                  YYABORT;
                              }

                          case FLOAT_CONST_TYPE:
                              sym_tb_obj_set_val( $1, $3 );
                              break;

                          case STATE_VAR_TYPE:
                          case INT_CONST_TYPE:
                              error_handler( ERR_TYPE_INTEGER, $1 );
                              YYABORT;

                          default:
                              error_handler( ERR_TYPE_NAME, $1 );
                              YYABORT;
                      }
                  }
   ;

identifier_initialization : identifier ASSIGNMENT obj_port_name
                  {
                      if( sym_tb_obj_get_type( $1 ) == OBJECT_TYPE ||
                          sym_tb_obj_get_type( $1 ) == MSG_PORT_TYPE )
                          sym_tb_obj_set_val( $1, $3 );
                      else
                          if( sym_tb_obj_get_type( $1 ) == STATE_VAR_TYPE ||
                              sym_tb_obj_get_type( $1 ) == STATE_VAR_FLOAT_TYPE ||
                              sym_tb_obj_get_type( $1 ) == INT_CONST_TYPE )
                          {
                              error_handler( ERR_TYPE_INTEGER, $1 );
                              YYABORT;
                          }
                          else
                          {
                              error_handler( ERR_TYPE_NUMBER, $1 );
                              YYABORT;
                          }
                  }
   ;
obj_port_name : IDENTIFIER { $$ = aloc_str( yytext );  }
   ;

vector_initialization : identifier ASSIGNMENT LBRACKET vec_elem_list RBRACKET
                      {
                         if( !resolve_delayed_sym( $1 ) )
                             YYABORT;
                         if (sym_tb_obj_set_vec_val ($1, $4) < 0) YYABORT;
                      }
                      | identifier LBRACKET RBRACKET ASSIGNMENT unsigned_integer
                      {
                         if (sym_tb_obj_set_vec_all ($1, atoi($5)) < 0) YYABORT;
                      }
                      | identifier ASSIGNMENT LBRACKET fvec_elem_list RBRACKET
                      {
                         if (sym_tb_obj_set_vec_val ($1, $4) < 0) YYABORT;
                      }
                      | identifier LBRACKET RBRACKET ASSIGNMENT float
                      {
                         if (sym_tb_obj_set_fvec_all ($1, atof($5)) < 0) YYABORT;
                      }
   ;

fvec_elem_list : fvec_elem_list COMMA float
            {  $$ = concat_str ($1, ",", $3);    }
              | float
            {  $$ = concat_str ($1, NULL, NULL); }
   ;

vec_elem_list : vec_elem_list COMMA unsigned_integer
            {  $$ = concat_str ($1, ",", $3);    }
              | unsigned_integer
            {  $$ = concat_str ($1, NULL, NULL); }
   ;

events_part : EVENTS LBRACES event_description_list RBRACES
   ;

event_description_list : event_description_list event_description
                       |
   ;

event_description : event_definition
                    event_condition_definition
                        { current_status.curr_ev_type = NON_EV;    }
                    action_definition
                        { strcpy (current_status.ev_msg_name, ""); }
   ;

event_definition : EVENT ASSIGNMENT event_name LPAREN distribution_description RPAREN
                   {
                       if(update_code_tb(EVENTS_PART, $3, DISTRIB_TYPE, NULL) < 0)
                           YYABORT;
                       current_status.curr_ev_type = DISTRIB_EV;
                   }
                 | EVENT ASSIGNMENT event_name LPAREN REWARD_REACHED RPAREN
                   {
                       CHECK_SIMULATION("event triggering by rewards");

                       if(update_distrib_expr (REW_REACH_DIST, NULL, NULL, NULL) < 0)
                           YYABORT;
                       if(update_code_tb (EVENTS_PART, $3, DISTRIB_TYPE, NULL) < 0)
                           YYABORT;
                       current_status.curr_ev_type = REW_REACHED_EV;
                   }
                 | EVENT ASSIGNMENT event_name LPAREN INIT RPAREN
                   {
                     /* Checks if this is the only event used for initializatio 
                        purposes */
                     if( current_status.has_init_event )
                     {
                         error_handler( ERR_DUP_INIT_EVENT, $3 );
                         YYABORT;
                     }
                     else
                     {
                         current_status.has_init_event = 1;
                     }
                     
                     CHECK_SIMULATION("an event for initialization purposes");

                     if( update_distrib_expr( INIT_DIST, NULL, NULL, NULL ) < 0 )
                         YYABORT;
                     if( update_code_tb( EVENTS_PART, $3, DISTRIB_TYPE, NULL ) < 0 )
                         YYABORT;
                     current_status.curr_ev_type = INIT_EV;
                   }                   
   ;

event_name : IDENTIFIER
           {
              if (sym_tb_obj_insert (EVENT_TYPE, yytext) < 0) YYABORT;
              $$ = aloc_str (yytext);
           }
   ;

distribution_description : EXP    COMMA expression
                           { if (update_distrib_expr (EXPONENTIAL_DIST, $3, NULL, NULL) < 0) YYABORT; }
                         | DET    COMMA expression
                           { if (update_distrib_expr (DETERMINISTIC_DIST, $3, NULL, NULL) < 0) YYABORT; }
                         | UNI    COMMA expression COMMA expression
                           {
                             CHECK_SIMULATION("Uniform distribution");
                             if (update_distrib_expr (UNIFORM_DIST, $3, $5, NULL) < 0) YYABORT;
                           }
                         | GAUSS  COMMA expression COMMA expression
                           {
                             CHECK_SIMULATION("Gaussian distribution");
                             if (update_distrib_expr (GAUSSIAN_DIST, $3, $5, NULL) < 0) YYABORT;
                           }
                         | ERLANG COMMA expression COMMA expression
                           {
                             CHECK_SIMULATION("Erlangian distribution");
                             if (update_distrib_expr (ERLANG_M_DIST, $3, $5, NULL) < 0) YYABORT;
                           }
                         | LOGNORM COMMA expression COMMA expression
                           {
                             CHECK_SIMULATION("Lognormal distribution");
                             if (update_distrib_expr (LOGNORMAL_DIST, $3, $5, NULL) < 0) YYABORT;
                           }
                         | WEIB COMMA expression COMMA expression
                           {
                             CHECK_SIMULATION("Weibull distribution");
                             if (update_distrib_expr (WEIBULL_DIST, $3, $5, NULL) < 0) YYABORT;
                           }
                         | PAR COMMA expression COMMA expression
                           {
                             CHECK_SIMULATION("Pareto distribution");
                             if (update_distrib_expr (PARETO_DIST, $3, $5, NULL) < 0) YYABORT;
                           }
                         | TRUNCPAR COMMA expression COMMA expression COMMA expression
                           {
                             CHECK_SIMULATION("Truncated Pareto distribution");
                             if (update_distrib_expr (TRUNC_PARETO_DIST, $3, $5, $7) < 0) YYABORT;
                           }
                         | FILE_  COMMA file_name
                           {
                             CHECK_SIMULATION("random samples from a file");
                             if (update_distrib_expr (FILE_DIST, $3, NULL, NULL) < 0) YYABORT;
                           }
                         | FBM    COMMA dist_param_float COMMA dist_param_float
                           COMMA dist_param_int COMMA dist_param_float COMMA dist_param_float
                           {
                             CHECK_SIMULATION("the Fractal Brownian Motion process");
                             update_distrib_num (FBM_DIST, $3, $5, $7, $9, $11);
                           }
                         | FARIMA COMMA dist_param_float COMMA dist_param_float 
                           COMMA dist_param_int COMMA dist_param_float COMMA dist_param_float
                           {
                             CHECK_SIMULATION("the F-ARIMA process");
                             update_distrib_num (FARIMA_DIST, $3, $5, $7, $9, $11);
                           }
   ;

file_name : QUOTE trace_file_name QUOTE
            {
                $$ = aloc_str ($2);
            }
   ;

dist_param_int : identifier
                    {
                        char value[ MAX_STRING ];

                        if( sym_tb_obj_get_type( $1 ) != INT_CONST_TYPE )
                        {
                            error_handler( ERR_TYPE_INT_CONST, $1 );
                            YYABORT;
                        }

                        sym_tb_obj_get_val( $1, value );
                        $$ = concat_str( value, NULL, NULL );
                    }
                  | unsigned_integer  {  $$ = aloc_str (yytext); }
   ;

dist_param_float : identifier
                    {
                        char value[ MAX_STRING ];

                        if( (sym_tb_obj_get_type( $1 ) != FLOAT_CONST_TYPE) &&
                            (sym_tb_obj_get_type( $1 ) != INT_CONST_TYPE)  )
                        {
                            error_handler( ERR_TYPE_FLOAT_CONST, $1 );
                            YYABORT;
                        }

                        sym_tb_obj_get_val( $1, value );
                        $$ = concat_str( value, NULL, NULL );
                    }
                    | number {  $$ = aloc_str (yytext); }
   ;

event_condition_definition : CONDITION ASSIGNMENT LPAREN boolean_expression RPAREN
                     {
                         if( check_parameter_sym( $4 ) )
                         {
                             error_handler( ERR_PARAM_IN_COND, $4 );
                             YYABORT;
                         }
                         if(update_code_tb(EVENTS_PART, NULL, COND_TYPE, $4) <0)
                             YYABORT;
                     }
   ;


action_definition : ACTION ASSIGNMENT action_code SEMICOLON
                    {
                        if( update_code_tb( 0, NULL, ACT_TYPE, NULL ) < 0 )
                            YYABORT;

                        /* clear the symbol table of ccode of this action */
                        current_status.sym_ccode_index = 0;
                    }
                  | ACTION ASSIGNMENT action_prob_list
                    {
                        /* turn off the code capture flag. Probs finished.*/
                        current_status.app_code = 0;
                    }
   ;

action_prob_list : action_prob_list action_prob_description 
                 | action_prob_description
   ;

action_prob_description : action_prob_code prob_definition
                        {
                           /* clear the symbol table of ccode of this action */
                           current_status.sym_ccode_index = 0;
                        }
   ;

action_prob_code : action_code COLON
                 {
                     if( update_code_tb( 0, NULL, ACT_PROB_TYPE, NULL ) < 0 )
                         YYABORT;

                     /* block the use of msg_data from this point on */
                     current_status.msg_data = 0;
                 }
   ;

prob_definition : PROB ASSIGNMENT expression SEMICOLON
                {
                    if( update_code_tb( 0, NULL, PROB_TYPE, $3 ) < 0 )
                        YYABORT;

                    /* set the code capture flag on to get other */
                    /* actions on probs.                         */
                    current_status.app_code = 1;

                    /* allow the use of msg_data from this point on */
                    current_status.msg_data = 1;
                }
   ;


messages_part : MESSAGES LBRACES message_description_list RBRACES
              | MESSAGES LBRACES RBRACES
   ;

message_description_list : message_description_list message_description
                         | message_description
   ;

message_description : message_definition 
                      action_definition
                      { strcpy (current_status.ev_msg_name, ""); }
   ;

message_definition : MSG_REC ASSIGNMENT identifier
                   {
                       if( sym_tb_obj_get_type( $3 ) == MSG_PORT_TYPE )
                       {
                           if( sym_tb_obj_get_init_status( $3 ) == INITIALIZED )
                           {
                               sym_tb_obj_set_init_status($3, INITIALIZED_AND_USED);
                               update_code_tb_msg( $3 );
                           }
                           else /* INITIALIZED_AND_USED */
                           {
                               error_handler( ERR_DUP_MSG, $3 );
                               YYABORT;
                           }
                       }
                       else
                       {
                           error_handler( ERR_TYPE_PORT, $3 );
                           YYABORT;
                       }
                   }
   ;


rewards_part : REWARDS LBRACES reward_description_list RBRACES
             | REWARDS LBRACES RBRACES
   ;

reward_description_list : reward_description_list reward_description
                        | reward_description
   ;

reward_description : reward_rate_header     reward_bound  reward_cr_init_val reward_cond_value_list
                   | reward_impulse_header  reward_bound  reward_cr_init_val reward_event_msg_value_list
                   | reward_rate_sum_header reward_rate_sum_bound            reward_rate_sum_value_list
   ;

reward_rate_header : RATE_REWARD ASSIGNMENT reward_identifier
	           {
                   /* update the current reward name */
                   strcpy (current_status.reward_name, $3);
                   /* update the current action number */
                   current_status.act_number = 1;
               }
   ;

reward_bound : BOUNDS ASSIGNMENT bound COMMA bound 
               { update_code_tb_bounds(current_status.reward_name, $3, $5);  }
	     | 
               { char inf[ MAX_STRING ];
                 strcpy( inf, "INF" );
                 update_code_tb_bounds( current_status.reward_name, inf, inf );
               } 
   ;

reward_rate_sum_bound : BOUNDS ASSIGNMENT bound COMMA bound
               { update_code_tb_bounds(current_status.reward_sum_name, $3, $5);}
	     |
               { char inf[ MAX_STRING ];
                 strcpy( inf, "INF" );
                 update_code_tb_bounds(current_status.reward_sum_name, inf,inf);
               }
   ;

bound : identifier
        {
            if( (sym_tb_obj_get_type( $1 ) == INT_CONST_TYPE) ||
                (sym_tb_obj_get_type( $1 ) == FLOAT_CONST_TYPE) )
                $$ = concat_str( $1, NULL, NULL );
            else
            {
                error_handler (ERR_TYPE_FLOAT_CONST, $1);
                YYABORT;
            }
        }
      | number     { $$ = concat_str ($1, NULL, NULL);    }
      | INF        { $$ = concat_str ("INF", NULL, NULL); }
   ;

reward_cr_init_val : CR_INIT_VAL ASSIGNMENT init_val
               { update_code_tb_cr_init_val(current_status.reward_name, $3);  }
	     |
               { char zero[ MAX_STRING ];
                 strcpy (zero, "0");
                 update_code_tb_cr_init_val(current_status.reward_name, zero );
               }
   ;

init_val : identifier
        {
            if( (sym_tb_obj_get_type( $1 ) == INT_CONST_TYPE) ||
                (sym_tb_obj_get_type( $1 ) == FLOAT_CONST_TYPE) )
                $$ = concat_str( $1, NULL, NULL );
            else
            {
                error_handler( ERR_TYPE_FLOAT_CONST, $1 );
                YYABORT;
            }
        }
      | number     { $$ = concat_str ($1, NULL, NULL);    }
   ;

reward_cond_value_list : reward_cond_value_list reward_cond_value
                       | reward_cond_value
   ;

reward_cond_value :  reward_condition_definition
                     reward_value_definition
   ;

reward_condition_definition : CONDITION ASSIGNMENT LPAREN boolean_expression RPAREN 
                  {
		     if ( check_parameter_sym($4) ) {
			 error_handler (ERR_PARAM_IN_COND, $4);
                         YYABORT;
   		     }
	      	     if (update_code_tb (REWARDS_PART, current_status.reward_name,
					 COND_TYPE, $4) < 0) YYABORT;
                  }
   ;

reward_value_definition : VALUE ASSIGNMENT expression SEMICOLON
                  {
                      if(update_code_tb(REWARDS_PART,current_status.reward_name,
                          REWARD_VALUE_TYPE, $3 ) < 0 )
                          YYABORT;
                  }
   ;

reward_impulse_header : IMPULSE_REWARD ASSIGNMENT reward_identifier
                  {
                      /* update the current reward name */
                      strcpy (current_status.reward_name, $3);
                      /* update the current action number */ 
                      current_status.act_number = 1;
                  }
   ;

reward_event_msg_value_list : reward_event_msg_value_list reward_event_msg_value
                            | reward_event_msg_value
   ;

reward_event_msg_value : reward_event_msg_definition
                         reward_value_definition
   ;

reward_event_msg_definition : reward_event_definition
                            | reward_msg_definition
   ;

reward_event_definition : EVENT ASSIGNMENT identifier COMMA action_number_list
                  {
                      if( sym_tb_obj_get_type( $3 ) == EVENT_TYPE )
                          update_code_tb_impulse( REWARD_EVENT_TYPE, 
                                           current_status.reward_name, $3, $5 );
                      else
                      {
                          error_handler( ERR_TYPE_EVENT, $3 );
                          YYABORT;
                      }
                  }
   ;

reward_msg_definition : PORT ASSIGNMENT identifier COMMA action_number_list
                  {
                      if( sym_tb_obj_get_type( $3 ) == MSG_PORT_TYPE )
                          update_code_tb_impulse( REWARD_MSG_TYPE,
                                           current_status.reward_name, $3, $5 );
                      else
                      {
                          error_handler( ERR_TYPE_PORT, $3 );
                          YYABORT;
                      }
                  }
                  | PORT_ ASSIGNMENT identifier COMMA action_number_list
                  {
                      if( sym_tb_obj_get_type( $3 ) == MSG_PORT_TYPE )
                          update_code_tb_impulse( REWARD_MSG_TYPE,
                                           current_status.reward_name, $3, $5 );
                      else
                      {
                          error_handler( ERR_TYPE_PORT, $3 );
                          YYABORT;
                      }
                  }
   ;

action_number_list : action_number_list COMMA unsigned_integer
		     { $$ = concat_str ($1, ",", $3); }
                   | unsigned_integer
 		     { $$ = concat_str ($1, NULL, NULL); }
   ;

reward_identifier : IDENTIFIER
                    {
                        if( sym_tb_obj_insert( REWARD_TYPE, yytext ) < 0 )
                            YYABORT;
                        $$ = aloc_str( yytext );
                    }
   ;

reward_rate_sum_header : RATE_REWARD_SUM ASSIGNMENT reward_sum_identifier
                   {
                       /* update the current reward sum name */
                       strcpy (current_status.reward_sum_name, $3);
                       /* update the current action number */
                       current_status.act_number = 1;
                   }
   ;

reward_sum_identifier : IDENTIFIER
                    {
                        if( sym_tb_obj_insert( REWARD_SUM_TYPE, yytext ) < 0 )
                            YYABORT;
                        $$ = aloc_str( yytext );
                    }
   ;

reward_rate_sum_value_list: reward_rate_sum_value_list reward_rate_sum_value
                          | reward_rate_sum_value
   ;

reward_rate_sum_value : REWARDS4SUM ASSIGNMENT sum_expression SEMICOLON
   ;

sum_expression: sum_expression PLUS sum_expression_identifier
              | sum_expression_identifier
   ;

sum_expression_identifier : simple_identifier
               {
                   /* keep the reference to analize the existence */
                   /* of this symbol in the future.               */
                   if( ref_sym_tb_insert( REWARD_4SUM_TYPE, $1 ) < 0 )
                       YYABORT;
                   /* insert one instance in code_tb */
                   if( update_code_tb( REWARDS_PART,
                       current_status.reward_sum_name,REWARD_4SUM_TYPE, $1) < 0)
                       YYABORT;
               }
    ;

global_reward_part : GLOBAL_REWARDS LBRACES global_reward_desc_list RBRACES
                   {
                     current_status.current_part = INDEP_CHAINS_PART;
                   }
                   | GLOBAL_REWARDS LBRACES RBRACES
                   {
                     current_status.current_part = INDEP_CHAINS_PART;
                   }
   ;

global_reward_desc_list : global_reward_desc_list global_reward_desc
                        | global_reward_desc
   ;

global_reward_desc : global_reward_rate_header global_reward_bound
		     global_reward_cond_value_list
   ;

global_reward_rate_header : RATE_REWARD ASSIGNMENT global_reward_identifier
                   {
                       /* update the current reward name */
                       strcpy( current_status.reward_name, $3 );
                       /* update the current action number */
                       current_status.act_number = 1;
                   }
   ;

global_reward_bound : BOUNDS ASSIGNMENT global_bound COMMA bound
                      { update_glob_rew_tb_bounds(current_status.reward_name,$3, $5); }
	            |
                      { char inf[ MAX_STRING ];
                        strcpy( inf, "INF" );
                        update_glob_rew_tb_bounds(current_status.reward_name,inf, inf);
                      }
   ;

global_bound : number     { $$ = concat_str ($1, NULL, NULL);    }
             | INF        { $$ = concat_str ("INF", NULL, NULL); }
   ;


global_reward_cond_value_list : global_reward_cond_value_list global_reward_cond_value
                              | global_reward_cond_value
   ;

global_reward_cond_value : global_reward_condition_definition
                           global_reward_value_definition
   ;

global_reward_condition_definition : CONDITION ASSIGNMENT LPAREN boolean_expression RPAREN
                  {
                      if( update_glob_rew_tb( COND_TYPE, current_status.reward_name, $4) < 0)
                          YYABORT;
                  }
   ;

global_reward_value_definition : VALUE ASSIGNMENT expression SEMICOLON
                  {
                      if( update_glob_rew_tb( REWARD_VALUE_TYPE,
                          current_status.reward_name, $3 ) < 0 )
                          YYABORT;
                  }
   ;


global_reward_identifier : IDENTIFIER
                    {
                        if( update_glob_rew_tb( REWARD_TYPE, yytext, NULL) < 0 )
                            YYABORT;
                        $$ = aloc_str( yytext );
                    }
   ;

global_rew_primary : compound_identifier
               {
                   char str[ MAX_STRING ];

                   if( !compound_id_is_valid( $1, REWARD_TYPE ) )
                       YYABORT;
                   sprintf( str, "(sys_st->show_reward_val(\"%s\"))", $1 );
                   $$ = concat_str( str, NULL, NULL );
               }
   ;

compound_identifier : COMPOUND_IDENTIFIER
                    { $$ = aloc_str( yytext ); }
   ;


indep_chains_part : INDEP_CHAINS LBRACES indep_desc_list RBRACES
                  | INDEP_CHAINS LBRACES RBRACES
   ;

indep_desc_list : indep_desc_list indep_desc
                | indep_desc
   ;

indep_desc : indep_chain_ev_definition chain_definition_list
   ;

indep_chain_ev_definition : EVENT ASSIGNMENT compound_identifier
                 {
                     if( !compound_id_is_valid( $3, EVENT_TYPE ) )
                         YYABORT;
                     if( !check_event_distrib_type( $3, DETERMINISTIC_DIST ) )
                         YYABORT;
                     if( update_indep_chain_tb( EVENT_TYPE, $3 ) < 0 )
                         YYABORT;
                 }
   ;

chain_definition_list : chain_definition_list chain_definition
                      | chain_definition
   ;

chain_definition : chain ASSIGNMENT obj_name_list
   ;

chain : CHAIN
      {
        if( update_indep_chain_tb( CHAIN_TYPE, yytext ) < 0 )
            YYABORT;
      }
   ;

obj_name_list : obj_name_list COMMA obj_name
              {
                  if( !is_object( $3 ) )
                  {
                      error_handler( ERR_INV_OBJ_NAME, $3 );
                      YYABORT;
                  }
                  if( update_indep_chain_tb( OBJECT_TYPE, $3 ) < 0 )
                      YYABORT;
              }
              | obj_name
              {
                  if( !is_object( $1 ) )
                  {
                      error_handler( ERR_INV_OBJ_NAME, $1 );
                      YYABORT;
                  }
                  if( update_indep_chain_tb( OBJECT_TYPE, $1 ) < 0 )
                      YYABORT;
              }
   ;


action_code : LBRACES { action_code_depth++; set_state_used = 0; }
              variable_declaration_part
              statement_sequence
              set_state_var_part
              {
                  if( action_code_depth != 1 && set_state_used ) 
                  {
                      warning_handler( WARN_SET_STATE, NULL );
                  }
              }
              RBRACES { action_code_depth--; set_state_used = 0; }
   ;

set_state_var_part : set_state_var_list
                     queue_operations_list
                   |
   ;
set_state_var_list : set_state_var_list set_state_var
                   | set_state_var { set_state_used = 1; }
   ;

set_state_var : SET_ST LPAREN QUOTE st_var_ident QUOTE COMMA state_value RPAREN SEMICOLON
                {
                    if( sym_tb_obj_get_type( $4 ) == STATE_VAR_INTQUEUE_TYPE ||
                        sym_tb_obj_get_type( $4 ) == STATE_VAR_FLOATQUEUE_TYPE )
                    {
                        if( sym_tb_obj_get_dim( $4 ) != sym_tb_ccode_get_dim( $7 ) )
                        {
                            error_handler( ERR_INCOMP_VEC, $7 );
                            YYABORT;
                        }
                    }
                }
              | SET_ST LPAREN QUOTE st_vec_ident QUOTE COMMA state_vec_value RPAREN SEMICOLON
                {
                    if( sym_tb_obj_get_dim( $4 ) != sym_tb_ccode_get_dim( $7 ) )
                    {
                        error_handler( ERR_INCOMP_VEC, $7 );
                        YYABORT;
                    }
                }
   ;

get_state_var : GET_ST LPAREN BAND var_value COMMA QUOTE st_var_ident QUOTE RPAREN SEMICOLON
                {
                    if( !compare_types( sym_tb_obj_get_type( $7 ), sym_tb_ccode_get_type( $4 ) ) )
                    {
                        char * temp_str;
                        temp_str = concat_str( $4, " and ", $7 );
                        error_handler( ERR_INCOMPATIBLE_TYPES, temp_str );
                        free( temp_str );
                        YYABORT;
                    }
                }
              | GET_ST LPAREN var_value COMMA QUOTE st_var_ident QUOTE RPAREN SEMICOLON
                {
                    if( !compare_types( sym_tb_obj_get_type( $6 ), sym_tb_ccode_get_type( $3 ) ) )
                    {
                        char * temp_str;
                        temp_str = concat_str( $3, " and ", $6 );
                        error_handler( ERR_INCOMPATIBLE_TYPES, temp_str );
                        free( temp_str );
                        YYABORT;
                    }
                    if( sym_tb_obj_get_type( $6 ) == STATE_VAR_INTQUEUE_TYPE ||
                        sym_tb_obj_get_type( $6 ) == STATE_VAR_FLOATQUEUE_TYPE )
                    {
                        if( sym_tb_obj_get_dim( $6 ) != sym_tb_ccode_get_dim( $3 ) )
                        {
                            error_handler( ERR_INCOMP_VEC, $3 );
                            YYABORT;
                        }
                    }
                }
              | GET_ST LPAREN BAND var_value LBRACKET unsigned_integer RBRACKET COMMA QUOTE st_var_ident QUOTE RPAREN SEMICOLON
                {
                    if( !compare_types( sym_tb_obj_get_type( $10 ), sym_tb_ccode_get_type( $4 ) ) )
                    {
                        char * temp_str;
                        temp_str = concat_str( $4, " and ", $10 );
                        error_handler( ERR_INCOMPATIBLE_TYPES, temp_str );
                        free( temp_str );
                        YYABORT;
                    }
                }
              | GET_ST LPAREN var_value COMMA QUOTE st_vec_ident QUOTE RPAREN SEMICOLON
                {
                    if( !compare_types( sym_tb_obj_get_type( $6 ), sym_tb_ccode_get_type( $3 ) ) )
                    {
                        char * temp_str;
                        fprintf( stderr, "\n\n%d %d\n\n", sym_tb_obj_get_type( $6 ), sym_tb_ccode_get_type( $3 ) );
                        temp_str = concat_str( $3, " and ", $6 );
                        error_handler( ERR_INCOMPATIBLE_TYPES, temp_str );
                        free( temp_str );
                        YYABORT;
                    }
                    if( sym_tb_obj_get_dim( $3 ) != sym_tb_ccode_get_dim( $6 ) )
                    {
                        error_handler( ERR_INCOMP_VEC, $3 );
                        YYABORT;
                    }
                }
   ;

queue_operations : SAVE_AT_HEAD LPAREN local_queue_ident COMMA state_vec_value RPAREN SEMICOLON
                {
                    CHECK_SIMULATION("queue operation save_at_head()");
                    if (sym_tb_ccode_get_dim( $3 ) != sym_tb_ccode_get_dim( $5 ) )
                    {
                        error_handler( ERR_INCOMP_VEC, $5 );
                        YYABORT;
                    }
                }
                 | SAVE_AT_TAIL LPAREN local_queue_ident COMMA state_vec_value RPAREN SEMICOLON
                {
                    CHECK_SIMULATION("queue operation save_at_tail()");
                    if( sym_tb_ccode_get_dim( $3 ) != sym_tb_ccode_get_dim( $5 ) )
                    {
                        error_handler( ERR_INCOMP_VEC, $5 );
                        YYABORT;
                    }
                }
                 | RESTORE_FROM_TAIL LPAREN local_queue_ident COMMA state_vec_value RPAREN SEMICOLON
                {
                    CHECK_SIMULATION("queue operation restore_from_tail()");
                    if( sym_tb_ccode_get_dim( $3 ) != sym_tb_ccode_get_dim( $5 ) )
                    {
                        error_handler(ERR_INCOMP_VEC, $5 );
                        YYABORT;
                    }
                }
                 | RESTORE_FROM_HEAD LPAREN local_queue_ident COMMA state_vec_value RPAREN SEMICOLON
                {
                    CHECK_SIMULATION("queue operation restore_from_head()");
                    if( sym_tb_ccode_get_dim( $3 ) != sym_tb_ccode_get_dim( $5 ) )
                    {
                        error_handler( ERR_INCOMP_VEC, $5 );
                        YYABORT;
                    }
                }
   ;

queue_operations_list: queue_operations_list queue_operations
                       |
   ;

local_queue_ident: identifier
                   {
                       if( sym_tb_ccode_get_type( $1 ) != C_INTQUEUE_TYPE  &&
                           sym_tb_ccode_get_type( $1 ) != C_FLOATQUEUE_TYPE )
                       {
                           error_handler( ERR_TYPE_LOCAL_QUEUE, $1 );
                           YYABORT;
                       }
                   }
   ;

st_var_ident : identifier
               {
                   if( (sym_tb_obj_get_type( $1 ) != STATE_VAR_TYPE) &&
                       (sym_tb_obj_get_type( $1 ) != STATE_VAR_FLOAT_TYPE) &&
                       (sym_tb_obj_get_type( $1 ) != STATE_VAR_INTQUEUE_TYPE) &&
                       (sym_tb_obj_get_type( $1 ) != STATE_VAR_FLOATQUEUE_TYPE))
                   {
                       error_handler( ERR_TYPE_STATE_VAR, $1 );
                       YYABORT;
                   }

                   if( sym_tb_obj_get_type( $1 ) != STATE_VAR_INTQUEUE_TYPE  &&
                       sym_tb_obj_get_type( $1 ) != STATE_VAR_FLOATQUEUE_TYPE )
                   { 
                       if( sym_tb_obj_get_dim( $1 ) > 1 )
                       {
                           error_handler( ERR_VEC_INDEX, $1 );
                           YYABORT;
                       }
                   }
               }
             | identifier lbracket unsigned_integer rbracket
               {
                   if( (sym_tb_obj_get_type( $1 ) != STATE_VAR_TYPE) &&
                       (sym_tb_obj_get_type( $1 ) != STATE_VAR_FLOAT_TYPE) )
                   {
                       error_handler( ERR_TYPE_STATE_VAR, $1 );
                       YYABORT;
                   }
                   if( sym_tb_obj_get_dim( $1 ) == 1 )
                   {
                       error_handler( ERR_VEC_INDEX, $1 );
                       YYABORT;
                   }
               }
   ;

var_value : identifier
            {
                if( sym_tb_ccode_get_type( $1 ) != C_INT_TYPE      &&
                    sym_tb_ccode_get_type( $1 ) != C_FLOAT_TYPE    &&
                    sym_tb_ccode_get_type( $1 ) != C_INTQUEUE_TYPE &&
                    sym_tb_ccode_get_type( $1 ) != C_FLOATQUEUE_TYPE )
                {
                    error_handler( ERR_GET_ST, $1 );
                    YYABORT;
                }
            }
    ;

state_value : integer
            | identifier
            {
                if( (sym_tb_ccode_get_type( $1 ) != C_INT_TYPE)       &&
                    (sym_tb_ccode_get_type( $1 ) != C_FLOAT_TYPE)     &&
                    (sym_tb_ccode_get_type( $1 ) != C_INTQUEUE_TYPE)  &&
                    (sym_tb_ccode_get_type( $1 ) != C_INTQUEUE_TYPE)  &&
                    (sym_tb_ccode_get_type( $1 ) != C_FLOATQUEUE_TYPE)  )
                {
                    error_handler( ERR_SET_ST, $1 );
                    YYABORT;
                }
                if( sym_tb_ccode_get_type( $1 ) != C_INTQUEUE_TYPE  &&
                    sym_tb_ccode_get_type( $1 ) != C_FLOATQUEUE_TYPE )
                    if( sym_tb_ccode_get_dim($1) > 1 )
                    {
                        error_handler (ERR_VEC_NO_INDEX, $1);
                        YYABORT;
                    }
            }
   ;

st_vec_ident : identifier lbracket rbracket
               {
                   if( (sym_tb_obj_get_type( $1 ) != STATE_VAR_TYPE) &&
                       (sym_tb_obj_get_type( $1 ) != STATE_VAR_FLOAT_TYPE) )
                   {
                       error_handler( ERR_TYPE_STATE_VAR, $1 );
                       YYABORT;
                   }
                   if( sym_tb_obj_get_dim( $1 ) == 1 )
                   {
                       error_handler( ERR_VEC_INDEX, $1 );
                       YYABORT;
                   }
                   $$ = concat_str( $1, NULL, NULL );
               }
   ;

state_vec_value : identifier
               {
                   if( sym_tb_ccode_get_type( $1 ) != C_INT_TYPE      &&
                       sym_tb_ccode_get_type( $1 ) != C_FLOAT_TYPE    &&
                       sym_tb_ccode_get_type( $1 ) != C_INTQUEUE_TYPE &&
                       sym_tb_ccode_get_type( $1 ) != C_FLOATQUEUE_TYPE )
                   {
                       error_handler( ERR_SET_ST, $1 );
                       YYABORT;
                   }
                   if( sym_tb_ccode_get_type( $1 ) != C_INTQUEUE_TYPE &&
                       sym_tb_ccode_get_type( $1 ) != C_FLOATQUEUE_TYPE )
                       if( sym_tb_ccode_get_dim( $1 ) == 1 )
                       {
                           error_handler( ERR_VEC_INDEX, $1 );
                           YYABORT;
                       }
                   $$ = concat_str( $1, NULL, NULL );
               }
   ;

variable_declaration_part : variable_declaration_list
                          |
   ;

variable_declaration_list : variable_declaration_list variable_declaration SEMICOLON
                          | variable_declaration SEMICOLON
   ;

variable_declaration : INT        c_int_list
                     | C_FLOAT    c_float_list
                     | INTQUEUE   c_intqueue_list
                     | FLOATQUEUE c_floatqueue_list
   ;

c_int_list : c_int_list COMMA c_int_var
           | c_int_var
   ;

c_int_var : simple_identifier
            {
                if( sym_tb_ccode_insert( C_INT_TYPE, $1 ) < 0 )
                    YYABORT;
                if( sym_tb_ccode_set_dim( $1, 1 ) < 0 )
                    YYABORT;
            }
          | simple_identifier LBRACKET unsigned_integer RBRACKET
            {
                if( sym_tb_ccode_insert( C_INT_TYPE, $1 ) < 0 )
                    YYABORT;
                if( sym_tb_ccode_set_dim( $1, atoi( $3 ) ) < 0 )
                    YYABORT;
            }
          | simple_identifier LBRACKET identifier RBRACKET
            {
                char value[ MAX_STRING ];

                if( sym_tb_obj_get_type( $3 ) != INT_CONST_TYPE )
                {
                    error_handler( ERR_TYPE_INT_CONST, $3 );
                    YYABORT;
                }
                sym_tb_obj_get_val( $3, value );
                if( sym_tb_ccode_insert( C_INT_TYPE, $1 ) < 0 )
                    YYABORT;
                if( sym_tb_ccode_set_dim( $1, atoi( value ) ) < 0 )
                    YYABORT;
            }
   ;

c_float_list : c_float_list COMMA c_float_var
           | c_float_var
   ;

c_float_var : simple_identifier
            {
                if( sym_tb_ccode_insert( C_FLOAT_TYPE, $1 ) < 0 )
                    YYABORT;
                if( sym_tb_ccode_set_dim( $1, 1 ) < 0 )
                    YYABORT;
      	    }
          | simple_identifier LBRACKET unsigned_integer RBRACKET
            {
                if( sym_tb_ccode_insert( C_FLOAT_TYPE, $1 ) < 0 )
                    YYABORT;
                if( sym_tb_ccode_set_dim( $1, atoi( $3 ) ) < 0 )
                    YYABORT;
            }
          | simple_identifier LBRACKET identifier RBRACKET
            {
                char value[ MAX_STRING ];

                if( sym_tb_obj_get_type( $3 ) != INT_CONST_TYPE )
                {
                    error_handler( ERR_TYPE_INT_CONST, $3 );
                    YYABORT;
                }
                sym_tb_obj_get_val( $3, value );
                if( sym_tb_ccode_insert( C_FLOAT_TYPE, $1 ) < 0 )
                    YYABORT;
                if( sym_tb_ccode_set_dim( $1, atoi( value ) ) < 0 )
                    YYABORT;
            }
   ;

c_intqueue_list : c_intqueue_list COMMA c_intqueue_var
                | c_intqueue_var
   ;

c_intqueue_var : simple_identifier LPAREN unsigned_integer RPAREN
                 {
                     if( sym_tb_ccode_insert( C_INTQUEUE_TYPE, $1 ) < 0 )
                         YYABORT;
                     if( sym_tb_ccode_set_dim( $1, atoi( $3 ) ) < 0 )
                         YYABORT;
                 }
               | simple_identifier LPAREN identifier RPAREN
                 {
                     char value[ MAX_STRING ];

                     if( sym_tb_obj_get_type( $3 ) != INT_CONST_TYPE )
                     {
                         error_handler( ERR_TYPE_INT_CONST, $3 );
                         YYABORT;
                     }
                     sym_tb_obj_get_val( $3, value );
                     if( sym_tb_ccode_insert( C_INTQUEUE_TYPE, $1 ) < 0 )
                         YYABORT;
                     if( sym_tb_ccode_set_dim( $1, atoi( value ) ) < 0 )
                         YYABORT;
                 }
   ;

c_floatqueue_list : c_floatqueue_list COMMA c_floatqueue_var
                  | c_floatqueue_var
   ;

c_floatqueue_var : simple_identifier LPAREN unsigned_integer RPAREN
                 {
                     if( sym_tb_ccode_insert( C_FLOATQUEUE_TYPE, $1 ) < 0 )
                         YYABORT;
                     if( sym_tb_ccode_set_dim( $1, atoi( $3 ) ) < 0 )
                         YYABORT;
                 }
               | simple_identifier LPAREN identifier RPAREN
                 {
                     char value[ MAX_STRING ];

                     if( sym_tb_obj_get_type( $3 ) != INT_CONST_TYPE )
                     {
                         error_handler( ERR_TYPE_INT_CONST, $3 );
                         YYABORT;
                     }
                     sym_tb_obj_get_val( $3, value );
                     if( sym_tb_ccode_insert( C_FLOATQUEUE_TYPE, $1 ) < 0 )
                         YYABORT;
                     if( sym_tb_ccode_set_dim( $1, atoi( value ) ) < 0 )
                         YYABORT;
                 }
   ;

statement_sequence : statement statement_sequence
                   | /*bug fix*/
   ;

statement : action_code
          | assignment_statement
          | if_statement
          | if_else_statement
          | while_statement
          | do_statement
          | msg_statement
          | clone_statement
          | set_cr_statement
          | set_ir_statement
          | unset_ir_statement
          | switch_statement
          | for_statement
          | printf_statement
          | fprintf_statement
          | get_state_var
          | get_msg_data
          | queue_operations
          | postfix_expression_statement
          | copy_statement
          | mtk_create_statement
          | mtk_delete_statement          
          | mtk_run_statement          
          | mtk_copy_statement
          | mtk_get_statement          
          | mtk_set_statement
          | BREAK SEMICOLON
          | SEMICOLON         
   ;

assignment_statement : assignment_identifier ASSIGNMENT expression SEMICOLON
                     | assignment_identifier C_ASSIGNMENT expression SEMICOLON
   ;

cast : LPAREN INT RPAREN     { $$ = concat_str( "(int)", NULL, NULL ); }
     | LPAREN C_FLOAT RPAREN { $$ = concat_str( "(TGFLOAT)", NULL, NULL ); }
   ;

assignment_identifier : identifier
                        {
                            if( (sym_tb_ccode_get_type( $1 ) != C_INT_TYPE) &&
                                (sym_tb_ccode_get_type( $1 ) != C_FLOAT_TYPE) )
                            {
                                error_handler( ERR_C_LOCAL_VAR, $1 );
                                YYABORT;
                            }
                            if( sym_tb_ccode_get_dim( $1 ) > 1 )
                            {
                                error_handler( ERR_VEC_INDEX, $1 );
                                YYABORT;
                            }
                        }
                      | identifier LBRACKET expression RBRACKET
                        {
                            if( (sym_tb_ccode_get_type( $1 ) != C_INT_TYPE) &&
                                (sym_tb_ccode_get_type( $1 ) != C_FLOAT_TYPE) )
                            {
                                error_handler( ERR_C_LOCAL_VAR, $1 );
                                YYABORT;
                            }
                            if( sym_tb_ccode_get_dim( $1 ) == 1)
                            {
                                error_handler( ERR_VEC_NO_INDEX, $1 );
                                YYABORT;
                            }
                            
                            sym_trim( $1 );
                            $$ = concat_str( $1, "[", NULL );
                            $$ = concat_str( $$, $3 , "]"  );
                        }
   ;

if_statement : IF LPAREN boolean_expression RPAREN statement
   ;

if_else_statement : IF LPAREN boolean_expression RPAREN statement ELSE statement
   ;

switch_statement : SWITCH LPAREN basic_identifier RPAREN LBRACES case_sequence RBRACES
   ;

case_sequence : case_sequence case_statement
		| case_statement
   ;
case_statement : CASE case_option COLON statement_sequence 
/*		 | CASE case_option COLON */
		 | DEFAULT COLON statement_sequence
   ;

case_option : integer
            | identifier
            {
                if( sym_tb_obj_get_type( $1 ) != INT_CONST_TYPE )
                {
                    error_handler( ERR_TYPE_INT_CONST, $1 );
                    YYABORT;
                }
            }
   ;

assignment_statement_for : assignment_identifier ASSIGNMENT   expression
                         | assignment_identifier C_ASSIGNMENT expression
                         | postfix_expression
   ;

assignment_sequence : assignment_sequence COMMA assignment_statement_for
                      | assignment_statement_for
   ;

for_statement : FOR LPAREN assignment_sequence SEMICOLON boolean_expression SEMICOLON
assignment_statement_for RPAREN statement
   ;

printf_statement : PRINTF LPAREN STRING expression_list RPAREN SEMICOLON
   ;

fprintf_statement : FPRINTF LPAREN STDOUT COMMA STRING expression_list RPAREN SEMICOLON
                  | FPRINTF LPAREN STDERR COMMA STRING expression_list RPAREN SEMICOLON
   ;

expression_list : expression_list COMMA expression
                | expression
                |
   ;

postfix_expression_statement : postfix_expression SEMICOLON
   ;

postfix_expression : identifier posop
                     {
                         if( sym_tb_ccode_query( $1 ) == -1 )
                         {
                             error_handler( ERR_TYPE_LOCAL_INT, $1 );
                             YYABORT;
                         }
                         if( sym_tb_ccode_get_type( $1 ) != C_INT_TYPE )
                         {
                             error_handler( ERR_TYPE_LOCAL_INT, $1 );
                             YYABORT;
                         }
                         if( sym_tb_ccode_get_dim( $1 ) != 1 )
                         {
                             error_handler( ERR_VEC_INDEX, $1 );
                             YYABORT;
                         }
                     }
                   | identifier LBRACKET expression RBRACKET posop
                     {
                         if( sym_tb_ccode_query( $1 ) == -1 )
                         {
                             error_handler( ERR_TYPE_LOCAL_INT, $1 );
                             YYABORT;
                         }
                         if( sym_tb_ccode_get_type( $1 ) != C_INT_TYPE )
                         {
                             error_handler( ERR_TYPE_LOCAL_INT, $1 );
                             YYABORT;
                         }
                         if( sym_tb_ccode_get_dim( $1 ) == 1 )
                         {
                             error_handler( ERR_VEC_NO_INDEX, $1 );
                             YYABORT;
                         }
                     }
    ;

while_statement : WHILE LPAREN boolean_expression RPAREN statement
   ;

do_statement : DO statement
               WHILE LPAREN boolean_expression RPAREN SEMICOLON
   ;

msg_statement : MSG LPAREN identifier COMMA msg_dest COMMA expression RPAREN SEMICOLON
                {
                    if( sym_tb_obj_get_type( $3 ) != MSG_PORT_TYPE )
                    {
                        error_handler( ERR_TYPE_PORT, $3 );
                        YYABORT;
                    }
                    if( strcmp( $5, "all" ) && strcmp( $5, "msg_source") )
                    {
                        if( sym_tb_obj_get_type( $5 ) != OBJECT_TYPE )
                        {
                            error_handler( ERR_TYPE_OBJECT, $5 );
                            YYABORT;
                        }
                    }
                }
   ;

msg_dest : identifier { $$ = concat_str($1, NULL, NULL); }
         | ALL_OBJS   { $$ = aloc_str(yytext);          }
         | MSG_SOURCE
         {
             if( current_status.current_part != MESSAGES_PART ||
                 current_status.msg_data == 0 )
             {
                 error_handler( ERR_MSG_DATA, NULL );
                 YYABORT;
             }
             $$ = aloc_str( yytext );
         }
   ;

obj_cmp_primary : OBJ_CMP LPAREN obj_identifier COMMA obj_identifier RPAREN
                    {
                        if( (sym_tb_obj_get_type( $3 ) != OBJECT_TYPE) &&
                            (strcmp( $3, "msg_source" )) )
                        {
                            error_handler( ERR_TYPE_OBJECT, $3 );
                            YYABORT;
                        }
                        if( (sym_tb_obj_get_type( $5 ) != OBJECT_TYPE) &&
                            (strcmp($5, "msg_source")))
                        {
                            error_handler (ERR_TYPE_OBJECT, $5);
                            YYABORT;
                        }
                    }
   ;

get_msg_data : GET_MSG_DATA LPAREN simple_identifier RPAREN SEMICOLON
               {
                   if( current_status.current_part != MESSAGES_PART ||
                       current_status.msg_data == 0 )
                   {
                       error_handler( ERR_MSG_DATA, NULL );
                       YYABORT;
                   }
                   if( sym_tb_ccode_get_type( $3 ) == -1 )
                   {
                       error_handler( ERR_C_LOCAL_VAR, $3 );
                       YYABORT;
                   }
               }
             | GET_MSG_DATA LPAREN BAND simple_identifier RPAREN SEMICOLON
             {
                   if( current_status.current_part != MESSAGES_PART ||
                       current_status.msg_data == 0 )
                   {
                       error_handler( ERR_MSG_DATA, NULL );
                       YYABORT;
                   }
                   if( sym_tb_ccode_get_type( $4 ) == -1 )
                   {
                       error_handler( ERR_C_LOCAL_VAR, $4 );
                       YYABORT;
                   }
                   if( sym_tb_ccode_get_dim( $4 ) > 1 )
                   {
                       error_handler( ERR_INCOMP_VEC, $4 );
                       YYABORT;
                   }
             }
   ;

obj_identifier  : identifier  { $$ = concat_str($1, NULL, NULL); }
                | MSG_SOURCE  { $$ = aloc_str(yytext);           }
   ;

clone_statement : CLONE_EV LPAREN QUOTE identifier QUOTE RPAREN SEMICOLON
                  {
                      CHECK_SIMULATION("event cloning with clone_ev()");
                      if( sym_tb_obj_get_type( $4 ) != EVENT_TYPE )
                      {
                          error_handler( ERR_TYPE_EVENT, $4 );
                          YYABORT;
                      }
                  }
   ;

set_cr_statement : SET_CR LPAREN simple_identifier COMMA rew_value RPAREN SEMICOLON
                   {
                       CHECK_SIMULATION("setting CR values with set_cr()");
                       if( ref_sym_tb_insert( REWARD_TYPE, $3 ) < 0 )
                           YYABORT;
                   }
   ;

set_ir_statement : SET_IR LPAREN simple_identifier COMMA rew_value RPAREN SEMICOLON
                   {
                       CHECK_SIMULATION("setting IR values with set_ir()");
                       if( ref_sym_tb_insert( REWARD_TYPE, $3 ) < 0 )
                           YYABORT;
                   }
   ;

unset_ir_statement : UNSET_IR LPAREN simple_identifier RPAREN SEMICOLON
                   {
                       CHECK_SIMULATION("unsetting IR values with unset_ir()");
                       if( ref_sym_tb_insert( REWARD_TYPE, $3 ) < 0 )
                           YYABORT;
                   }
   ;

copy_statement : COPY LPAREN state_vec_value COMMA state_vec_value RPAREN SEMICOLON
                        {
                            int type, dim;

                            dim = type = 0;
                            if( sym_tb_ccode_query( $3 ) == -1 )
                            {
                                error_handler( ERR_C_LOCAL_VAR, NULL );
                                YYABORT;
                            }
                            
                            if( sym_tb_ccode_query( $5 ) != -1 )
                            {
                                type = sym_tb_ccode_get_type( $5 );
                                dim  = sym_tb_ccode_get_dim( $5 );
                            }
                            else
                            {
                                if( sym_tb_obj_query( $5 ) != -1 )
                                {
                                    type = sym_tb_obj_get_type( $5 );
                                    dim  = sym_tb_obj_get_dim( $5 );
                                }
                            }

                            if( sym_tb_ccode_get_type( $3 ) != type )
                            {
                                error_handler( ERR_COPY_DIFFERENT_PARAMENTERS_TYPE, NULL );
                                YYABORT;
                            }
                            if( sym_tb_ccode_get_dim( $3 ) != dim )
                            {
                                error_handler( ERR_COPY_DIFFERENT_PARAMENTERS_DIM, NULL );
                                YYABORT;
                            }
                        }
   ;
   
mtk_create_statement : MTK_CREATE
                       LPAREN
                       identifier COMMA
                       QUOTE simple_identifier QUOTE
                       mtk_args_list
                       RPAREN
                       SEMICOLON
                     {
                        /* Check type */
                        if( sym_tb_obj_get_type( $3 ) != MTK_OBJECT_TYPE )
                        {
                            error_handler( ERR_TYPE_MTK_OBJECT, $3 );
                            YYABORT;                        
                        }
                        else
                        {
                            char *mtk_obj_name = getMTKObjectName( $3 );

                            /* Translate code */
                            $$ = concat_str( "/* Create MTK object */\n\t{\n"
                                             "\t\tMTKSetup();\n"
                                             "\n\t\t/* Create arguments */\n"
                                             "\t\tArguments args;\n"
                                             "\t\targs.clear();\n"
                                             "\n\t\t/* Get plugin manager */\n"
                                             "\t\tPluginManager *plugin_manager = "
                                             "PluginManager::getPluginManager( );\n"
                                             "\n\t\t/* Get object manager */\n"
                                             "\t\tObjectManager *object_manager = "
                                             "ObjectManager::getObjectManager( );\n",
                                             NULL, NULL );

                            if( strlen( $8 ) > 0 )
                                $$ = concat_str( $$,
                                                 "\n\t\t/* Create tokens */",
                                                 $8 );

                            $$ = concat_str( $$, "\n\t\t/* Create object */\n",
                                                 "\t\tPlugin *plugin = plugin_manager->getPlugin( \"" );
                            $$ = concat_str( $$, $6, "\" );\n" );
                            $$ = concat_str( $$, "\t\tif( plugin )\n",
                                                 "\t\t\tobject_manager->createObject( plugin, \"" );
                            $$ = concat_str( $$, mtk_obj_name,
                                                 "\", args );\n\t}\n" );

                            /* Append code into struct */
                            append_code( $$ );

                            /* Let lex::proc_token append code again */
                            current_status.app_code = 1;

                            /* Remember mtk object */
                            has_mtk_object = 1;
                        }
                     }
;

mtk_delete_statement : MTK_DELETE
                       LPAREN
                       identifier
                       RPAREN
                       SEMICOLON
                     {
                        /* Check type */
                        if( sym_tb_obj_get_type( $3 ) != MTK_OBJECT_TYPE )
                        {
                            error_handler( ERR_TYPE_MTK_OBJECT, $3 );
                            YYABORT;                        
                        }
                        else
                        {                     
                            char *mtk_obj_name = getMTKObjectName( $3 );

                            /* Translate code */
                            $$ = concat_str( "/* Delete MTK object */\n\t{\n"
                                             "\t\tMTKSetup();\n"
                                             "\n\t\t/* Get object manager */\n"
                                             "\t\tObjectManager *object_manager = "
                                             "ObjectManager::getObjectManager( );\n",
                                             NULL, NULL );
                            $$ = concat_str( $$, "\n\t\t/* Destroy object */\n",
                                                 "\t\tObject *object = object_manager->getObjectByName( \"" );
                            $$ = concat_str( $$, mtk_obj_name, "\" );\n" );
                            $$ = concat_str( $$, "\t\tif( object )\n",
                                                 "\t\t\tobject_manager->destroyObject( \"" );
                            $$ = concat_str( $$, mtk_obj_name, "\" );\n\t}\n" );

                            /* Append code into struct */
                            append_code( $$ );

                            /* Let lex::proc_token append code again */
                            current_status.app_code = 1;

                            /* Remember mtk object */
                            has_mtk_object = 1;
                        }
                     }
;

mtk_run_statement : MTK_RUN
                    LPAREN
                    identifier COMMA
                    QUOTE simple_identifier QUOTE
                    mtk_args_list
                    RPAREN
                    SEMICOLON
                  {
                     /* Check type */
                     if( sym_tb_obj_get_type( $3 ) != MTK_OBJECT_TYPE )
                     {
                         error_handler( ERR_TYPE_MTK_OBJECT, $3 );
                         YYABORT;                        
                     }
                     else
                     {
                         char *mtk_obj_name = getMTKObjectName( $3 );

                         /* Translate code */
                         $$ = concat_str( "/* Run MTK object method */\n\t{\n"
                                          "\t\tMTKSetup();\n"
                                          "\n\t\t/* Create arguments */\n"
                                          "\t\tArguments args;\n"
                                          "\t\targs.clear();\n"
                                          "\n\t\t/* Get object manager */\n"
                                          "\t\tObjectManager *object_manager = "
                                          "ObjectManager::getObjectManager( );\n",
                                          NULL, NULL );
                         if( strlen( $8 ) > 0 )
                             $$ = concat_str( $$,
                                              "\n\t\t/* Create tokens */",
                                              $8 );
                         $$ = concat_str( $$, "\n\t\t/* Get object */\n",
                                              "\t\tObject *object = object_manager->getObjectByName( \"" );                         
                         $$ = concat_str( $$, mtk_obj_name, "\" );\n" );
                         
                         $$ = concat_str( $$, "\n\t\t/* Run object method */\n",
                                              "\t\tif( object )\n" );

                         $$ = concat_str( $$, "\t\t\tobject->exec( \"", NULL );
                         $$ = concat_str( $$, $6, "\", args );\n\t}\n" );

                         /* Append code into struct */
                         append_code( $$ );

                         /* Let lex::proc_token append code again */
                         current_status.app_code = 1;

                         /* Remember mtk object */
                         has_mtk_object = 1;
                    }
                  }
;

mtk_copy_statement : MTK_COPY
                     LPAREN
                     identifier COMMA
                     identifier 
                     RPAREN
                     SEMICOLON
                   {
                       /* Check type */
                       if( sym_tb_obj_get_type( $3 ) != MTK_OBJECT_TYPE )
                       {
                           error_handler( ERR_TYPE_MTK_OBJECT, $3 );
                           YYABORT;                        
                       }
                       else if( sym_tb_obj_get_type( $5 ) != MTK_OBJECT_TYPE )
                       {
                           error_handler( ERR_TYPE_MTK_OBJECT, $5 );
                           YYABORT;                        
                       }                       
                       else
                       {
                            char *dest_obj_name = getMTKObjectName( $3 );
                            char *src_obj_name  = getMTKObjectName( $5 );
                            
                            /* Translate code */
                            $$ = concat_str( "/* Copy MTK object */\n\t{\n"
                                             "\t\tMTKSetup();\n"                                             
                                             "\n\t\t/* Get object manager */\n"
                                             "\t\tObjectManager *object_manager = "
                                             "ObjectManager::getObjectManager( );\n"
                                             "\n\t\t/* Copy object */\n"
                                             "\t\tobject_manager->copyFromTangram( \"",
                                             dest_obj_name, "\", \"" );
                            $$ = concat_str( $$, src_obj_name, "\" );\n\t}" );
                            
                            /* Append code into struct */
                            append_code( $$ );

                            /* Let lex::proc_token append code again */
                            current_status.app_code = 1;

                            /* Remember mtk object */
                            has_mtk_object = 1;                            
                       }                
                   }  
;

mtk_get_statement : MTK_GET
                    LPAREN
                    assignment_identifier COMMA    /* Destination variable */
                    identifier COMMA    /* MTK object           */
                    QUOTE simple_identifier QUOTE /* MTK object attribute */
                    mtk_integer_list    /* Attribute index */
                    RPAREN
                    SEMICOLON
                  {
                      /* Check type */
                      if( sym_tb_obj_get_type( $5 ) != MTK_OBJECT_TYPE )
                      {
                          error_handler( ERR_TYPE_MTK_OBJECT, $5 );
                          YYABORT;                        
                      }                       
                      else
                      {
                           char *mtk_obj_name  = getMTKObjectName( $5 );

                           /* Translate code */
                           $$ = concat_str( "/* Get MTK object attribute */\n\t{\n"
                                            "\t\tMTKSetup();\n"
                                            "\n\t\t/* Create index */\n"
                                            "\t\tIndex index;\n"
                                            "\t\tindex.clear();\n"
                                            "\n\t\t/* Get object manager */\n"
                                            "\t\tObjectManager *object_manager = "
                                            "ObjectManager::getObjectManager( );\n",
                                            NULL, NULL );
                        
                           if( strlen( $10 ) > 0 )                           
                               $$ = concat_str( $$, "\n\t\t/* Append index */\n",
                                                    $10 );

                           $$ = concat_str( $$, "\n\t\t/* Get object */\n",
                                                "\t\tObject *object = object_manager->getObjectByName( \"" );
                           $$ = concat_str( $$, mtk_obj_name, "\" );\n" );

                           $$ = concat_str( $$, "\n\t\t/* Get value */\n"
                                                "\t\tif( object )\n\t\t{\n"
                                                "\t\t\tstd::string value;\n"
                                                "\t\t\tobject->get( \"",
                                                $8 );
                           $$ = concat_str( $$, "\", value, index );\n"
                                                "\t\t\tsscanf( value.c_str(), ",
                                                NULL );

                           $$ = concat_str( $$, getParameterMask( $3 ), NULL );
                           $$ = concat_str( $$, ", &( ", $3 );

                           $$ = concat_str( $$, " ) );\n\t\t}", NULL );
                                            
                           $$ = concat_str( $$, "\n\t}", NULL );

                           /* Append code into struct */
                           append_code( $$ );

                           /* Let lex::proc_token append code again */
                           current_status.app_code = 1;

                           /* Remember mtk object */
                           has_mtk_object = 1;                            
                      }                
                  }  
;

mtk_set_statement : MTK_SET
                    LPAREN
                    expression COMMA     /* Source value */
                    identifier COMMA     /* MTK object      */
                    QUOTE simple_identifier QUOTE /* MTK object attribute */
                    mtk_integer_list    /* Attribute index */
                    RPAREN
                    SEMICOLON
                  {
                      /* Check type */
                      if( sym_tb_obj_get_type( $5 ) != MTK_OBJECT_TYPE )
                      {
                          error_handler( ERR_TYPE_MTK_OBJECT, $5 );
                          YYABORT;                        
                      }                       
                      else
                      {
                           char *mtk_obj_name  = getMTKObjectName( $5 );

                           /* Translate code */
                           $$ = concat_str( "/* Set MTK object attribute */\n\t{\n"
                                            "\t\tMTKSetup();\n"
                                            "\n\t\t/* Create index */\n"
                                            "\t\tIndex index;\n"
                                            "\t\tindex.clear();\n"
                                            "\n\t\t/* Get object manager */\n"
                                            "\t\tObjectManager *object_manager = "
                                            "ObjectManager::getObjectManager( );\n",
                                            NULL, NULL );
                        
                           if( strlen( $10 ) > 0 )                           
                               $$ = concat_str( $$, "\n\t\t/* Append index */\n",
                                                    $10 );

                           $$ = concat_str( $$, "\n\t\t/* Get object */\n",
                                                "\t\tObject *object = object_manager->getObjectByName( \"" );
                           $$ = concat_str( $$, mtk_obj_name, "\" );\n" );

                           $$ = concat_str( $$, "\n\t\t/* Set value */\n"
                                                "\t\tif( object )\n\t\t{\n"
                                                "\t\t\tchar buffer[40];\n"
                                                "\t\t\tstd::string value;\n"
                                                "\n\t\t\tsprintf( buffer, ",
                                                NULL );
                           /* Must cast do double, because of the mask returned
                              by getParameterMask */
                           $$ = concat_str( $$, getParameterMask( $3 ),
                                                ", (double)" );
                           $$ = concat_str( $$, $3, " );\n" );
                           $$ = concat_str( $$, "\t\t\tvalue = buffer;\n"
                                                "\n\t\t\tobject->set( \"",
                                                $8 );
                           $$ = concat_str( $$, "\", value, index );\n", NULL );

                           $$ = concat_str( $$, "\t\t}", NULL );
                                            
                           $$ = concat_str( $$, "\n\t}", NULL );

                           /* Append code into struct */
                           append_code( $$ );

                           /* Let lex::proc_token append code again */
                           current_status.app_code = 1;

                           /* Remember mtk object */
                           has_mtk_object = 1;                            
                      }                
                  }  
;                   

mtk_args_list : COMMA mtk_arg mtk_args_list
                {
                    $$ = concat_str( $2, $3, NULL );
                }
              |
                {
                    $$ = aloc_str( "" );
                }
;

mtk_arg : identifier
        {
           int type = sym_tb_obj_get_type( $1 );

           char *tk_name      = getMTKTokenName();
           char *mtk_obj_name = getMTKObjectName( $1 );

           if( type == MTK_OBJECT_TYPE )
           {
               $$ = concat_str( "\n\t\tToken ", tk_name, ";\n"             );
               $$ = concat_str( $$,             "\t\t",  NULL              );
               $$ = concat_str( $$,             tk_name,
                                                ".type = TK_IDENTIFIER;\n" );
               $$ = concat_str( $$,             "\t\t",  NULL              );
               $$ = concat_str( $$,             tk_name, ".text = \""      );
               $$ = concat_str( $$,             mtk_obj_name, "\";\n"      );

               $$ = concat_str( $$, "\t\targs.push_back( ", tk_name );
               $$ = concat_str( $$, " );\n",                NULL    );
           }
           else
           {
               int c_type   = sym_tb_ccode_get_type( $1 );
               char *buffer = getMTKTokenName();
               char tk_type[10];
               tk_type[0] = '\0';
               
               if( type   == INT_CONST_TYPE || 
                   type   == INT_PARAM_TYPE || 
                   c_type == C_INT_TYPE )
               {
                   strcpy( tk_type, "TK_INTEGER" );
               }
               else if( type   == FLOAT_CONST_TYPE ||
                        type   == FLOAT_PARAM_TYPE ||
                        c_type == C_FLOAT_TYPE )
               {
                   strcpy( tk_type, "TK_REAL" );
               }

               /* Accepted type, translate code */
               if( strlen( tk_type ) > 0 )
               {
                   $$ = concat_str( "\n\t\tToken ", tk_name, NULL       );
                   $$ = concat_str( $$, ";\n\t\t",   NULL               );
                   $$ = concat_str( $$, tk_name,     ".type = "         );
                   $$ = concat_str( $$, tk_type,     ";\n"              );
                   $$ = concat_str( $$, "\t\tchar ", buffer             );
                   $$ = concat_str( $$, "[40];\n\t\tsprintf( ", NULL    );
                   $$ = concat_str( $$, buffer, ", "                    );
                   $$ = concat_str( $$, getParameterMask( $1 ), ", "    );
                   $$ = concat_str( $$, $1, " );\n\t\t"                 );
                   $$ = concat_str( $$, tk_name, ".text = "             );
                   $$ = concat_str( $$, buffer, ";\n"                   );
                   $$ = concat_str( $$, "\t\targs.push_back( ", tk_name );
                   $$ = concat_str( $$, " );\n",                NULL    );
               }
               else
               {                 
                   error_handler( ERR_INCOMPATIBLE_TYPES, $1 );
                   YYABORT;
               }
           }
        }
        | sign mtk_integer
        {
           char *tk_name = getMTKTokenName();

           $$ = concat_str( "\n\t\tToken ", tk_name, ";\n"                   );
           $$ = concat_str( $$,             "\t\t",  NULL                    );
           $$ = concat_str( $$,             tk_name, ".type = TK_INTEGER;\n" );              
           $$ = concat_str( $$,             "\t\t",  NULL                    );
           $$ = concat_str( $$,             tk_name, ".text = \""            );
           $$ = concat_str( $$,             $1,      $2                      );
           $$ = concat_str( $$,             "\";\n", NULL                    );

           $$ = concat_str( $$, "\t\targs.push_back( ", tk_name );
           $$ = concat_str( $$, " );\n",                NULL    );
        }
        | mtk_integer  
        {
           char *tk_name = getMTKTokenName();

           $$ = concat_str( "\n\t\tToken ", tk_name, ";\n"                   );
           $$ = concat_str( $$,             "\t\t",  NULL                    );
           $$ = concat_str( $$,             tk_name, ".type = TK_INTEGER;\n" );              
           $$ = concat_str( $$,             "\t\t",  NULL                    );
           $$ = concat_str( $$,             tk_name, ".text = \""            );
           $$ = concat_str( $$,             $1,      "\";\n"                 );

           $$ = concat_str( $$, "\t\targs.push_back( ", tk_name );
           $$ = concat_str( $$, " );\n",                NULL    );
        }
        | sign mtk_real
        {
           char *tk_name = getMTKTokenName();

           $$ = concat_str( "\n\t\tToken ", tk_name, ";\n"                 );
           $$ = concat_str( $$,             "\t\t",  NULL                  );
           $$ = concat_str( $$,             tk_name, ".type = TK_REAL;\n"  );              
           $$ = concat_str( $$,             "\t\t",  NULL                  );
           $$ = concat_str( $$,             tk_name, ".text = \""          );
           $$ = concat_str( $$,             $1,      $2                    );
           $$ = concat_str( $$,             "\";\n", NULL                  );

           $$ = concat_str( $$, "\t\targs.push_back( ", tk_name );
           $$ = concat_str( $$, " );\n",                NULL    );
        }
        | mtk_real
        {
           char *tk_name = getMTKTokenName();

           $$ = concat_str( "\n\t\tToken ", tk_name, ";\n"                 );
           $$ = concat_str( $$,             "\t\t",    NULL                );
           $$ = concat_str( $$,             tk_name, ".type = TK_REAL;\n"  );              
           $$ = concat_str( $$,             "\t\t",  NULL                  );
           $$ = concat_str( $$,             tk_name, ".text = \""          );
           $$ = concat_str( $$,             $1,      "\";\n"               );

           $$ = concat_str( $$, "\t\targs.push_back( ", tk_name );
           $$ = concat_str( $$, " );\n",                NULL    );
        }
        |  QUOTE trace_file_name QUOTE /* used for parsing trace files and plugin function options, such as hmm.load( "A" ). */
        {
           char *tk_name = getMTKTokenName();

           $$ = concat_str( "\n\t\tToken ", tk_name, ";\n"                  );
           $$ = concat_str( $$,             "\t\t",  NULL                   );
           $$ = concat_str( $$,             tk_name, ".type = TK_STRING;\n" );
           $$ = concat_str( $$,             "\t\t",  NULL                   );
           $$ = concat_str( $$,             tk_name, ".text = \""           );
           $$ = concat_str( $$,             $2,      "\";\n"                );

           $$ = concat_str( $$, "\t\targs.push_back( ", tk_name );
           $$ = concat_str( $$, " );\n",                NULL    );
        }
;

mtk_integer : unsigned_integer
            | unsigned_integerexp
;

mtk_real : unsigned_real
         | unsigned_realexp
;

mtk_integer_list : COMMA unsigned_integer mtk_integer_list
                   {
                       $$ = concat_str( "\t\tindex.push_back( ", $2, " );\n" );
                       $$ = concat_str( $$, $3, NULL );
                   }
                 | COMMA identifier mtk_integer_list
                   {
                       sym_trim( $2 );
                       
                       $$ = concat_str( "\t\tindex.push_back( ", $2, " );\n" );
                       $$ = concat_str( $$, $3, NULL );                   
                   }
                 |
                   {
                       $$ = aloc_str( "" );
                   }
;

rew_value : identifier
            {
                int type;

                if( sym_tb_obj_query( $1 ) >= 0 )
                {
                    type = sym_tb_obj_get_type( $1 );
                    if( (type != FLOAT_CONST_TYPE) && (type != INT_CONST_TYPE)&&
                      (type != STATE_VAR_TYPE) &&(type != STATE_VAR_FLOAT_TYPE))
                    {
                        error_handler( ERR_TYPE_NUMBER, $1 );
                        YYABORT;
                    }
                    if( (type == STATE_VAR_TYPE)    &&
                        (type != STATE_VAR_FLOAT_TYPE) )
                        if( sym_tb_obj_get_dim( $1 ) > 1 )
                        {
                            error_handler(ERR_VEC_NO_INDEX, $1);
                            YYABORT;
                        }
                }
                else
                {
                    /* a valid identifier that is not in the sym_obj_tb */
                    /*   must be in the sym_tb_ccode table              */
                    /* no type verification is needed since all local   */
                    /*   variables are either INT or FLOAT type.        */
                    if( sym_tb_ccode_get_dim( $1 ) > 1)
                    {
                        error_handler( ERR_VEC_NO_INDEX, $1 );
                        YYABORT;
                    }
                }
            }
          | number
   ;

get_random_primary : GET_RANDOM LPAREN RPAREN
                         {
                             CHECK_SIMULATION("random values with get_random()");
                             $$ = concat_str("get_random()",$2,$3);
                         }
   ;


get_simul_time_primary : GET_SIMUL_TIME LPAREN RPAREN
                         {
                             CHECK_SIMULATION("get_simul_time()");
                             $$ = concat_str("get_simul_time()",$2,$3);
                         }
   ;


get_cr_primary : GET_CR LPAREN simple_identifier RPAREN
                {
                    CHECK_SIMULATION("get_cr()");
                    if( ref_sym_tb_insert( REWARD_TYPE, $3 ) < 0 )
                        YYABORT;
                    $$ = concat_str( "get_cr(", $3, ")" );
                }
   ;

get_ir_primary : GET_IR LPAREN simple_identifier RPAREN
                {
                    CHECK_SIMULATION("get_ir()");
                    if( ref_sym_tb_insert( REWARD_TYPE, $3 ) < 0 )
                        YYABORT;
                    $$ = concat_str( "get_ir(", $3, ")" );
                }
   ;

get_cr_sum_primary : GET_CR_SUM LPAREN simple_identifier RPAREN
                {
                   CHECK_SIMULATION("get_cr_sum()");
                   if (ref_sym_tb_insert(REWARD_SUM_TYPE, $3) < 0) YYABORT;
                   $$ = concat_str ("get_cr_sum(",$3,")");
                }
   ;

number : unsigned_number      { $$ = concat_str ($1, NULL, NULL);   }
       | sign unsigned_number { $$ = concat_str ($1, $2, NULL);     }
   ;

integer : unsigned_integer           { $$ = concat_str ($1, NULL, NULL);   }
        | unsigned_integerexp        { $$ = concat_str ($1, NULL, NULL);   }
        | sign unsigned_integer      { $$ = concat_str ($1, $2, NULL);     }
        | MSG_TYPE_DEFINE
        {
          if( current_status.current_part != MESSAGES_PART ||
              current_status.msg_data == 0 )
          {
              error_handler( ERR_MSG_DATA, NULL );
              YYABORT;
          }
          $$ = aloc_str( yytext );
        }
   ;

float   : unsigned_real              { $$ = concat_str ($1, NULL, NULL);   }
        | unsigned_realexp           { $$ = concat_str ($1, NULL, NULL);   }
        | sign unsigned_real         { $$ = concat_str ($1, $2, NULL);     }
   ;



boolean_expression : expression       { $$ = concat_str ($1, NULL, NULL);  }
   ;


expression : simple_expression        { $$ = concat_str ($1, NULL, NULL);  }
             | simple_expression relop simple_expression
               { $$ = concat_str ($1, $2, $3);     }
           | reached_get reachedop simple_expression
                                      {
                                          CHECK_SIMULATION("reward reaching operators");
                                          if (current_status.curr_ev_type == REW_REACHED_EV )
                                          {
                                               $$ = concat_str("special_rr_ev(",$1,"\",");
                                               $$ = concat_str($$,$2,",(double)");
                                               $$ = concat_str($$,$3,")");
                                          }
                                          else
                                          {
                                              error_handler (ERR_REACHED_OP,
                                               "Operators /\\ and \\/ can only be used with REWARD_REACHED event." );
                                              YYABORT;
                                          }
                                      }
   ;

simple_expression : term              { $$ = concat_str ( "(", $1, ")" ); }
                  | simple_expression addop term
                                      { $$ = concat_str ( "(", $1, ")" );
                                        $$ = concat_str ( $$ , $2, "(" );
                                        $$ = concat_str ( $$ , $3, ")" );
                                        /* fazer free na pilha*/
                                      }
           | simple_expression relop simple_expression interrog simple_expression colon simple_expression 
             {
                 char *aux1, *aux2;
                 aux1 = concat_str( $1, $2, $3 );
                 aux2 = concat_str( aux1, $4, $5 );
                 $$   = concat_str( aux2, $6, $7 );
             }
                                      
   ;

term : factor                         { $$ = concat_str ($1, NULL, NULL); }
     | term mulop factor              { $$ = concat_str ($1, $2, $3);
                                        /* fazer free na pilha*/
                                      }
   ;

factor : sign factor                  { $$ = concat_str ($1, $2, NULL);   }
       | cast factor                  { $$ = concat_str ($1, $2, NULL);   }
       | primary                      { $$ = concat_str ($1, NULL, NULL); }
   ;

primary : basic_identifier
        | unsigned_number             { $$ = concat_str ($1, NULL, NULL); }
        | lparen expression rparen    { $$ = concat_str ($1, $2, $3);     }
        | not primary                 { $$ = concat_str ($1, $2, NULL);   }
        | TRUE_                       { $$ = aloc_str (yytext);           }
        | FALSE_                      { $$ = aloc_str (yytext);           }
        | obj_cmp_primary             { $$ = concat_str ($1, NULL, NULL); }
        | get_cr_primary              { $$ = concat_str ($1, NULL, NULL); }
        | get_ir_primary              { $$ = concat_str ($1, NULL, NULL); }
        | get_cr_sum_primary          { $$ = concat_str ($1, NULL, NULL); }
        | get_simul_time_primary      { $$ = concat_str ($1, NULL, NULL); }
        | get_random_primary          { $$ = concat_str ($1, NULL, NULL); }
        | global_rew_primary          { $$ = concat_str ($1, NULL, NULL); }
        | pow_func_primary            { $$ = concat_str ($1, NULL, NULL); }
        | exp_func_primary            { $$ = concat_str ($1, NULL, NULL); }
        | log_func_primary            { $$ = concat_str ($1, NULL, NULL); }
        | log10_func_primary          { $$ = concat_str ($1, NULL, NULL); }
        | sqrt_func_primary           { $$ = concat_str ($1, NULL, NULL); }
        | cos_func_primary            { $$ = concat_str ($1, NULL, NULL); }
        | sin_func_primary            { $$ = concat_str ($1, NULL, NULL); }
        | tan_func_primary            { $$ = concat_str ($1, NULL, NULL); }
        | acos_func_primary           { $$ = concat_str ($1, NULL, NULL); }
        | asin_func_primary           { $$ = concat_str ($1, NULL, NULL); }
        | atan_func_primary           { $$ = concat_str ($1, NULL, NULL); }
        | atan2_func_primary          { $$ = concat_str ($1, NULL, NULL); }
        | cosh_func_primary           { $$ = concat_str ($1, NULL, NULL); }
        | sinh_func_primary           { $$ = concat_str ($1, NULL, NULL); }
        | tanh_func_primary           { $$ = concat_str ($1, NULL, NULL); }
        | ceil_func_primary           { $$ = concat_str ($1, NULL, NULL); }
        | fabs_func_primary           { $$ = concat_str ($1, NULL, NULL); }
        | floor_func_primary          { $$ = concat_str ($1, NULL, NULL); }
        | fmod_func_primary           { $$ = concat_str ($1, NULL, NULL); }
        | frexp_func_primary          { $$ = concat_str ($1, NULL, NULL); }
        | ldexp_func_primary          { $$ = concat_str ($1, NULL, NULL); }
        | MSG_TYPE_DEFINE             { $$ = aloc_str( yytext );          }
   ;

basic_identifier : identifier
                 {
                    int type;
					if( sym_tb_obj_query( $1 ) >= 0 )
                    {
                        type = sym_tb_obj_get_type( $1 );
                        if( (type != STATE_VAR_TYPE) && (type != STATE_VAR_FLOAT_TYPE) && (type != INT_CONST_TYPE) &&
                            (type != FLOAT_CONST_TYPE) && (type != INT_PARAM_TYPE)
                         && (type != FLOAT_PARAM_TYPE))
                        {
                            if( type == OBJECT_TYPE )
                                error_handler( ERR_OBJ_IN_EXPR, $1 );
                            if( type == MSG_PORT_TYPE )
                                error_handler( ERR_PORT_IN_EXPR, $1 );
                            if( type == REWARD_TYPE )
                                error_handler( ERR_REW_IN_EXPR, $1 );
                            YYABORT;
                        }
					}
                    else
                        if( sym_tb_ccode_query( $1 ) < 0 )
                        {
                            error_handler( ERR_UNK_IDENT, $1 );
                            YYABORT;
                        }

                    $$ = concat_str( $1, NULL, NULL );
                }
                 | identifier lbracket expression rbracket
                   {
 		               char *aux;
		               if( sym_tb_obj_query( $1 ) >= 0 )
                       {
                           if( sym_tb_obj_get_dim( $1 ) == 1 )
                           {
                               error_handler( ERR_VEC_NO_INDEX, $1 );
                               YYABORT;
                           }
                       }
                       else
			               if( sym_tb_ccode_get_dim( $1 ) == 1 )
                           {
                               error_handler( ERR_VEC_NO_INDEX, $1 );
                               YYABORT;
                           }
                       aux = concat_str( $1, $2, $3 );
                       $$  = concat_str( aux, $4, NULL );
                   }
                 | MSG_DATA
                   {
                     if (current_status.current_part != MESSAGES_PART ||
                         current_status.msg_data == 0)
                     {
                         error_handler (ERR_MSG_DATA, NULL);
                         YYABORT;
                     }
                     $$ = aloc_str (yytext);
                   }
                 | MSG_TYPE
                   {
                     if (current_status.current_part != MESSAGES_PART ||
                         current_status.msg_data == 0)
                     {
                         error_handler (ERR_MSG_TYPE, NULL);
                         YYABORT;
                     }
                     $$ = aloc_str( yytext );
                   }
                 | MSG_SIZE
   ;

unsigned_number : unsigned_integer   
                | unsigned_integerexp
                | unsigned_real      
                | unsigned_realexp   
   ;

unsigned_integer : DIGSEQ    { $$ = aloc_str (yytext); }
   ;

unsigned_integerexp : DIGSEQEXP    { $$ = aloc_str (yytext); }
   ;

unsigned_real : REALNUMBER   { $$ = aloc_str (yytext); }
   ;

unsigned_realexp : REALNUMBEREXP   { $$ = aloc_str (yytext); }
   ;

pow_func_primary : pow_func lparen expression comma expression rparen
                    {
                        char *aux1, *aux2;
                        aux1 = concat_str( $1, $2, $3 );
                        aux2 = concat_str( aux1, $4, $5 );
                        $$   = concat_str( aux2, $6, NULL );
                    }
   ;

sqrt_func_primary : sqrt_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;

exp_func_primary : exp_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;
log_func_primary : log_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;
log10_func_primary : log10_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;
cos_func_primary : cos_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;
sin_func_primary : sin_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;
tan_func_primary : tan_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;
acos_func_primary : acos_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;
asin_func_primary : asin_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;
atan_func_primary : atan_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;
atan2_func_primary : atan2_func lparen expression comma expression rparen
                    {
                        char *aux1, *aux2;
                        aux1 = concat_str( $1, $2, $3 );
                        aux2 = concat_str( aux1, $4, $5 );
                        $$   = concat_str( aux2, $6, NULL );
                    }
   ;
cosh_func_primary : cosh_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;
sinh_func_primary : sinh_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;
tanh_func_primary : tanh_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;
ceil_func_primary : ceil_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;
fabs_func_primary : fabs_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;
floor_func_primary : floor_func lparen expression rparen
                    {
                        char *aux1;
                        aux1 = concat_str( $1, $2, $3 );
                        $$   = concat_str( aux1, $4, NULL );
                    }
   ;
fmod_func_primary : fmod_func lparen expression comma expression rparen
                    {
                        char *aux1, *aux2;
                        aux1 = concat_str( $1, $2, $3 );
                        aux2 = concat_str( aux1, $4, $5 );
                        $$   = concat_str( aux2, $6, NULL );
                    }
   ;
frexp_func_primary : frexp_func lparen expression comma expression rparen
                    {
                        char *aux1, *aux2;
                        aux1 = concat_str( $1, $2, $3 );
                        aux2 = concat_str( aux1, $4, $5 );
                        $$   = concat_str( aux2, $6, NULL );
                    }
   ;
ldexp_func_primary : ldexp_func lparen expression comma expression rparen
                    {
                        char *aux1, *aux2;
                        aux1 = concat_str( $1, $2, $3 );
                        aux2 = concat_str( aux1, $4, $5 );
                        $$   = concat_str( aux2, $6, NULL );
                    }
   ;

sign : PLUS       { $$ = aloc_str (yytext); }
     | MINUS      { $$ = aloc_str (yytext); }
   ;

addop : PLUS      { $$ = aloc_str (yytext); }
      | MINUS     { $$ = aloc_str (yytext); }
      | BOR       { $$ = aloc_str (yytext); }
      | OR        {
                      if (current_status.curr_ev_type == REW_REACHED_EV )
                      {
                          /* Redefinition of || to >? (max operator)       */
                          /* This change forces the analisys of 2 operands */
                          /* The grammar was modified to put ( ) around    */
                          /*  operands to avoid precedence mistakes caused */
                          /*  by operators changed.                        */
                          /*  modified rule: simple_expression.            */
                          $$ = aloc_str( ">?" );
                      }
                      else
                      {
                          $$ = aloc_str( yytext );
                      }
                  }
   ;

mulop : STAR      { $$ = aloc_str (yytext); }
      | SLASH     { $$ = aloc_str (yytext); }
      | MOD       { $$ = aloc_str (yytext); }
      | AND       { $$ = aloc_str (yytext); }
      | BAND      { $$ = aloc_str (yytext); }
      | RSHIFT    { $$ = aloc_str (yytext); }
      | LSHIFT    { $$ = aloc_str (yytext); }
   ;

relop : EQUAL     { $$ = aloc_str (yytext); }
      | NOTEQUAL  { $$ = aloc_str (yytext); }
      | LT        { $$ = aloc_str (yytext); }
      | GT        { $$ = aloc_str (yytext); }
      | LE        { $$ = aloc_str (yytext); }
      | GE        { $$ = aloc_str (yytext); }
   ;

posop : INC_OP     { $$ = aloc_str (yytext); }
      | DEC_OP     { $$ = aloc_str (yytext); }
   ;

reachedop: UPP_REACHED    { $$ = concat_str ("UPP_REACH_TYPE", NULL , NULL ); }
      |    LOW_REACHED    { $$ = concat_str ("LOW_REACH_TYPE", NULL , NULL ); }
   ;

reached_get : get_cr_primary        { char straux[MAX_STRING];
                                      char *straux2;
                                      strcpy(straux,$1);
                                      straux[ strlen( straux ) - 1 ] = '\0';
                                      straux2 = straux + 7;
                                      strcpy( straux, straux2);
                                      $$ = concat_str ("GET_CR_TYPE,\"",straux, NULL); }

            | get_ir_primary        { char straux[MAX_STRING];
                                      char *straux2;
                                      strcpy(straux,$1);
                                     straux[ strlen( straux ) - 1 ] = '\0';
                                     straux2 = straux + 7;
                                     strcpy( straux, straux2);
                                     $$ = concat_str ("GET_IR_TYPE,\"",straux, NULL); }

            | get_cr_sum_primary    { char straux[MAX_STRING];
                                      char *straux2;
                                      strcpy(straux,$1);
                                      straux[ strlen( straux ) - 1 ] = '\0';
                                      straux2 = straux + 11;
                                      strcpy( straux, straux2);
                                      $$ = concat_str ("GET_CR_SUM_TYPE,\"",straux, NULL); }
   ;

lparen : LPAREN   { $$ = aloc_str( "(" ); }
   ;
rparen : RPAREN   { $$ = aloc_str( ")" ); }
   ;

lbracket : LBRACKET   { $$ = aloc_str (yytext); } 
   ;
rbracket : RBRACKET   { $$ = aloc_str (yytext); } 
   ;

not : NOT             { $$ = aloc_str (yytext); } 
   ;

interrog : INTERROG   { $$ = aloc_str ( "?" ); } 
   ;

colon : COLON   { $$ = aloc_str ( ":" ); } 
   ;

comma : COMMA         { $$ = aloc_str (yytext); } 
   ;

pow_func : POW_FUNC   { $$ = aloc_str (yytext); } 
   ;

exp_func : EXP_FUNC   { $$ = aloc_str (yytext); } 
   ;

log_func : LOG_FUNC   { $$ = aloc_str (yytext); } 
   ;

log10_func: LOG10_FUNC{ $$ = aloc_str (yytext); }
   ;

sqrt_func : SQRT_FUNC { $$ = aloc_str (yytext); }
   ;

cos_func : COS_FUNC { $$ = aloc_str (yytext); }
   ;

sin_func : SIN_FUNC { $$ = aloc_str (yytext); }
   ;

tan_func : TAN_FUNC { $$ = aloc_str (yytext); }
   ;

asin_func : ASIN_FUNC { $$ = aloc_str (yytext); }
   ;

acos_func : ACOS_FUNC { $$ = aloc_str (yytext); }
   ;

atan_func : ATAN_FUNC { $$ = aloc_str (yytext); }
   ;

tanh_func : TANH_FUNC { $$ = aloc_str (yytext); }
   ;

atan2_func : ATAN2_FUNC { $$ = aloc_str (yytext); }
   ;

cosh_func : COSH_FUNC { $$ = aloc_str (yytext); }
   ;

sinh_func : SINH_FUNC { $$ = aloc_str (yytext); }
   ;

fabs_func : FABS_FUNC { $$ = aloc_str (yytext); }
   ;

floor_func : FLOOR_FUNC { $$ = aloc_str (yytext); }
   ;

ceil_func : CEIL_FUNC { $$ = aloc_str (yytext); }
   ;

fmod_func : FMOD_FUNC { $$ = aloc_str (yytext); }
   ;

frexp_func : FREXP_FUNC { $$ = aloc_str (yytext); }
   ;

ldexp_func : LDEXP_FUNC { $$ = aloc_str (yytext); }
   ;


trace_file_name: IDENTIFIER
                 {
                    $$ = concat_str ( NULL, yytext, NULL);
                 }
               | COMPOUND_IDENTIFIER
                 {
                    $$ = concat_str ( NULL, yytext, NULL);
                 }
               | MULT_COMPOUND_IDENTIFIER
                 {
                    $$ = concat_str ( NULL, yytext, NULL);
                 }
    ;

identifier : IDENTIFIER
             {
                /* if the identifier is a reserved word then error */
                if( is_reserved_word( yytext ) < 0 )
                {
                    error_handler( ERR_RESER_WORD, yytext );
                    YYABORT;
                }

                /* if the sym is not declared then error */
                if( sym_tb_obj_query( yytext ) < 0 )
                    if( sym_tb_ccode_query( yytext ) < 0 )
                    {
                        error_handler( ERR_UNK_IDENT, yytext );
                        YYABORT;
                    }

                /* if parsing an action code, then no parameters can exist */
                if( current_status.app_code == 1 )
                    if( sym_tb_obj_get_type( yytext ) == INT_PARAM_TYPE ||
                        sym_tb_obj_get_type( yytext ) == FLOAT_PARAM_TYPE)
                        {
                            error_handler( ERR_PARAM_IN_ACT, yytext );
                            YYABORT;
                        }
                $$ = concat_str (" ", yytext, " ");
            }
   ;

simple_identifier : IDENTIFIER
                    {
                         $$ = aloc_str( yytext );
                    }
   ;

delayed_identifier : IDENTIFIER
                     {
                         if( is_reserved_word( yytext ) < 0 )
                         {
                             error_handler( ERR_RESER_WORD, yytext );
                             YYABORT;
                         }
                         $$ = aloc_str( yytext );
                     }
    ;

%%
//------------------------------------------------------------------------------
//     U S E R   C O D E
//------------------------------------------------------------------------------
void yyerror( const char *msg )
{
    extern char *yytext;

    printf( "\n ==== Error while parsing the model ====\n" );
    printf( "Error         : %s\n", msg );

    if( include_stack_ptr )
        printf( "Included file : %s\n",
                 include_file[ include_stack_ptr ].filename );
    else
        printf( "Main file     : %s\n",
                 include_file[ include_stack_ptr ].filename );

    printf( "Object        : %s\n", current_status.obj_name  );
    printf( "Page          : %d\n", current_status.tgif_page );    

    printf( "Part          : "                               );
    switch( current_status.current_part )
    {
        case DECL_PART           : printf( "Declaration\n");              break;
        case INITIALIZATION_PART : printf( "Initialization\n");           break;
        case EVENTS_PART         : printf( "Events = %s\n",
                                            current_status.ev_msg_name ); break;
        case MESSAGES_PART       : printf( "Messages = %s\n",
                                            current_status.ev_msg_name ); break;
        case REWARDS_PART        : printf( "Rewards\n");                  break;
        case GLOBAL_REWARDS_PART : printf( "Global Rewards\n" );          break;
        case INDEP_CHAINS_PART   : printf( "Independent Chains\n" );      break;
    }

    printf( "Line number   : %u\n",
             include_file[include_stack_ptr].linenumber + 1 );
    printf( "Token         : %s\n", yytext );
    printf( "Code          : %s %s\n",
             current_status.last_line,
             current_status.current_line );
}
//------------------------------------------------------------------------------
void yywarning(const char *msg)
{
    extern char *yytext;

    printf ("\n ==== Warning while parsing the model ====\n");
    printf ("Error       : %s\n",msg);
    if( include_stack_ptr )
        printf ("Included file : %s\n", include_file[include_stack_ptr].filename);
    else
        printf ("Main file     : %s\n", include_file[include_stack_ptr].filename);
    printf ("Object      : %s\n", current_status.obj_name);
    printf ("Part        : ");
    switch (current_status.current_part)
    {
        case DECL_PART           : printf ("Declaration\n"); break;
        case INITIALIZATION_PART : printf ("Initialization\n"); break;
        case EVENTS_PART         : printf ("Events = %s\n", current_status.ev_msg_name); break;
        case MESSAGES_PART       : printf ("Messages = %s\n", current_status.ev_msg_name); break;
        case REWARDS_PART        : printf ("Rewards\n"); break;
        case GLOBAL_REWARDS_PART : printf ("Global Rewards\n"); break;
        case INDEP_CHAINS_PART   : printf ("Independent Chains\n"); break;
    }
    printf ("Line number : %u\n", include_file[include_stack_ptr].linenumber + 1 );
    printf ("Token       : %s\n", yytext);
    printf ("Code        : %s %s\n", current_status.last_line, current_status.current_line);

}
//------------------------------------------------------------------------------
void msg_error_distribution( int distribution )
{
    switch (distribution)
    {
        case  GAUSS:    fprintf(stderr,"GAUSS" );   break;
        case  ERLANG:   fprintf(stderr,"ERLANG");   break;
        case  LOGNORM:  fprintf(stderr,"LOGNORM");  break;
        case  WEIB:     fprintf(stderr,"WEIB");     break;
        case  PAR:      fprintf(stderr,"PAR");      break;
        case  TRUNCPAR: fprintf(stderr,"TRUNCPAR"); break;
        case  FILE_:    fprintf(stderr,"FILE");     break;
        case  FBM:      fprintf(stderr,"FBM");      break;
        case  FARIMA:   fprintf(stderr,"FARIMA");   break;
        case  UNI:      fprintf(stderr,"UNI");      break;
    }
    fprintf(stderr," distribution can not be used in Mathematical Model Solution\n" );
}
//------------------------------------------------------------------------------
