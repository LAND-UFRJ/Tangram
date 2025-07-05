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

//------------------------------------------------------------------------------
//     D E F I N E S
//------------------------------------------------------------------------------
#ifndef __GRAMATICA_H__
#define __GRAMATICA_H__

#include "tangram_types.h"

#define    CR_LF          -13  /* so it won't conflict with a token */
#define    SPACE_TAB      -12  /* same */

/* parts of the object */
enum
{
    DECL_PART = 1         ,
    INITIALIZATION_PART   ,
    EVENTS_PART           ,
    MESSAGES_PART         ,
    REWARDS_PART          ,
    GLOBAL_REWARDS_PART   ,
    INDEP_CHAINS_PART     
};

/* types used in the code table */
enum
{
    DISTRIB_TYPE = 1        ,
    COND_TYPE               ,
    ACT_TYPE                ,
    ACT_PROB_TYPE           ,
    REWARD_VALUE_TYPE       ,
    REWARD_EVENT_TYPE       ,
    REWARD_MSG_TYPE         ,
    REWARD_CR_INIT_VAL_TYPE ,
    REWARD_4SUM_TYPE        ,
    BOUND_TYPE              ,
    PROB_TYPE               ,
    MSG_REC_TYPE            
};

/* possible distributions types */
enum
{
    UNIFORM_DIST = 1      ,
    DETERMINISTIC_DIST    ,
    EXPONENTIAL_DIST      ,
    GAUSSIAN_DIST         ,
    FBM_DIST              ,
    FARIMA_DIST           ,
    ERLANG_M_DIST         ,
    LOGNORMAL_DIST        ,
    FILE_DIST             ,
    WEIBULL_DIST          ,
    PARETO_DIST           ,
    GAUSSIAN_DIST_NUM     ,
    REW_REACH_DIST        ,
    TRUNC_LOGNORMAL_DIST  ,
    TRUNC_PARETO_DIST     ,
    INIT_DIST             
};

/* types of identifiers in the object (used in the symbol table) */
enum
{
    STATE_VAR_TYPE = 1        ,
    INT_CONST_TYPE            ,
    INT_CONST_VEC_TYPE        ,    
    FLOAT_CONST_TYPE          ,
    FLOAT_CONST_VEC_TYPE      ,
    OBJECT_TYPE               ,
    MSG_PORT_TYPE             ,
    REWARD_TYPE               ,
    EVENT_TYPE                ,
    INT_PARAM_TYPE            ,
    FLOAT_PARAM_TYPE          ,
    REWARD_SUM_TYPE           ,
    STATE_VAR_FLOAT_TYPE      ,
    STATE_VAR_INTQUEUE_TYPE   ,
    STATE_VAR_FLOATQUEUE_TYPE ,
    MTK_OBJECT_TYPE
};

/* used in the independent chains table */
#define    CHAIN_TYPE         10

/* C type variables */
enum
{
    C_INT_TYPE = 1000  ,
    C_FLOAT_TYPE       ,
    C_INT_VEC_TYPE     ,
    C_FLOAT_VEC_TYPE   ,
    C_INTQUEUE_TYPE    ,
    C_FLOATQUEUE_TYPE  
};

/* Undefined dimesion of a state variable vector */
#define    UNDEF_ST_VECT_DIM   -1

/* expression types */
enum
{
    EXEC_TYPE = 1,
    INTERP_TYPE  ,
    NUMERIC_TYPE
};

/* general defines */
#define    MAX_STRING  255
#define    MAX_NAME    255

/* #define    MAX_CODE 100000 */
#define    INITIAL_CODE_SIZE  4096
#define    CODE_SIZE_INCREASE 4096

enum
{
    NOT_INITIALIZED        ,
    INITIALIZED            ,
    INITIALIZED_AND_USED
};

/* Error codes for possible errors */
enum
{
    ERR_SIMULATION_ONLY                    ,
    ERR_UNK_IDENT                          ,
    ERR_DUP_IDENT                          ,
    ERR_DUP_IDENT_OBJ                      ,
    ERR_DUP_REW_REF                        ,
    ERR_SYM_TB_OBJ_FULL                    ,
    ERR_SYM_TB_CCODE_FULL                  ,
    ERR_OBJ_DESC_FULL                      ,
    ERR_REF_SYM_TB_FULL                    ,
    ERR_INV_ASSIGN                         ,
    ERR_VAR_INIT                           ,
    ERR_TYPE_STATE_VAR                     ,
    ERR_GET_ST                             ,
    ERR_TYPE_INT_CONST                     ,
    ERR_TYPE_LOCAL_INT                     ,
    ERR_TYPE_LOCAL_QUEUE                   ,
    ERR_TYPE_FLOAT_CONST                   ,
    ERR_TYPE_OBJECT                        ,
    ERR_TYPE_PORT                          ,
    ERR_TYPE_REWARD                        ,
    ERR_TYPE_MTK_OBJECT                    ,
    ERR_TYPE_EVENT                         ,
    ERR_TYPE_INTEGER                       ,
    ERR_TYPE_NUMBER                        ,
    ERR_TYPE_NAME                          ,
    ERR_RESER_WORD                         ,
    ERR_DUP_MSG                            ,
    ERR_MSG_DATA                           ,
    ERR_MSG_TYPE                           ,
    ERR_PARAM_IN_COND                      ,
    ERR_PARAM_IN_EXPR                      ,
    ERR_PARAM_IN_ACT                       ,
    ERR_OBJ_IN_EXPR                        ,
    ERR_PORT_IN_EXPR                       ,
    ERR_REW_IN_EXPR                        ,
    ERR_SET_ST                             ,
    ERR_C_LOCAL_VAR                        ,
    ERR_ST_VEC_DIM                         ,
    ERR_VEC_INDEX                          ,
    ERR_VEC_NO_INDEX                       ,
    ERR_INCOMP_VEC                         ,
    ERR_VEC_LITERAL                        ,
    ERR_NO_OBJ                             ,
    ERR_NO_REWARD                          ,
    ERR_PARAM_GLOBAL_REW                   ,
    ERR_NO_EVENT                           ,
    ERR_INV_OBJ_NAME                       ,
    ERR_DETER_DIST                         ,
    ERR_REACHED_OP                         ,
    ERR_DELAYED_TB_FULL                    ,
    ERR_MUST_SET_DIM                       ,
    ERR_VECT_DIM_NOT_DECL                  ,
    ERR_INVALID_VECT_DIM_TYPE              ,
    ERR_COPY_DIFFERENT_PARAMENTERS_TYPE    ,
    ERR_COPY_DIFFERENT_PARAMENTERS_DIM     ,
    ERR_QUEUE_INITIALIZED                  ,
    ERR_INCOMPATIBLE_TYPES                 ,
    ERR_VEC_INIT_ALL                       ,
    ERR_DUP_INIT_EVENT
};

/* Warning codes for possible warnings */
#define    WARN_SET_STATE         0

#define    MAX_TABLE_ENTRIES      50000

/* Operation mode */
enum
{
    SIMULATION       ,
    CHAIN_GENERATION
};

/* Possible event types */
enum
{
    NON_EV          ,          
    DISTRIB_EV      ,          
    REW_REACHED_EV  ,
    INIT_EV           
};

//------------------------------------------------------------------------------
//     S T R U C T S
//------------------------------------------------------------------------------
typedef struct {

    struct yy_buffer_state *include_stream;    /* include file stream -
                                                  yacc buffer */
    
    char obj_name[MAX_NAME];     /* used in case of parse error */
    char ev_msg_name[MAX_NAME];  /* used in case of parse error */
    char file_name[MAX_NAME];    /* used in case of parse error */

    char current_line[512];
    char last_line[512];

    int  current_part;           /* current part being parsed */
    unsigned int line_number;    /* line number inside current part */

    char reward_name[MAX_NAME];  /* current reward being parsed */
    int  msg_data;               /* allow | block the use of message data
                                    in the code */

    int  func_hand;      /* next available function handler number */

    int  msg_number;        /* current message number */
    int  code_index;        /* current code table entry */
    int  act_number  ;      /* current action number */
    int  sym_obj_index;     /* current object symbol table entry */
    int  sym_ccode_index;   /* current ccode symbol table entry */
    int  obj_desc_index;    /* current obj description table entry */
    int  ref_sym_index;     /* current index of the referece symbol table */
    int  global_rew_index;  /* current index of the global rewards table */
    int  indep_chain_index; /* current index of the independet chain table*/
    int  delayed_tb_index;  /* current index of the delayed table */
    int  has_init_event;    /* is set true when an event is used for
                               initialization purposes */
    int  tgif_page;         /* the page where the object lives in TGIF file */

    int  app_code;       /* flag for appending the code or not */
    char *code;          /* action code */
    unsigned int code_size;

    int  curr_ev_type;   /* current event type being parsed.
                        (DISTRIB_EV, REW_REACHED_EV, NON_EV) */
    char reward_sum_name[MAX_NAME];    /* current reward sum being parsed */
} t_status;
//------------------------------------------------------------------------------
/* this structure is used to check reference of symbols that have not yet */
/* appeared in the object. This verification is done after parsing the object */
typedef struct {
        int  type;                  /* type of symbol */
        char name[MAX_NAME];        /* name of symbol */
        int  part;                  /* part of the object where symbol appears*/
        char ev_msg_name[MAX_NAME]; /* where the symbol appears */
        int  act_number;            /* number of the action */
} t_ref_sym;
//------------------------------------------------------------------------------
//     S T R U C T S      << . >>      D I S T R I B U T I O N S   T Y P E S
//------------------------------------------------------------------------------
typedef struct {
        char *rate;
        int   func_hand;
        char  func_name[MAX_NAME];
} t_exp;
//------------------------------------------------------------------------------
typedef struct {
        char *mean;
        int   func_hand;
        char  func_name[MAX_NAME];
} t_det;
//------------------------------------------------------------------------------
typedef struct {
        char *lower;
        int   func_hand_low;
        char  func_name_low[MAX_NAME];
        char *upper;
        int   func_hand_upp;
        char  func_name_upp[MAX_NAME];
} t_unif;
//------------------------------------------------------------------------------
typedef struct {
        char *mean;
        int   func_hand_mean;
        char  func_name_mean[MAX_NAME];
        char *var;
        int   func_hand_var;
        char  func_name_var[MAX_NAME];
} t_gauss;
//------------------------------------------------------------------------------
typedef struct {
        char *rate;
        int   func_hand_rate;
        char  func_name_rate[MAX_NAME];
        char *stages;
        int   func_hand_stages;
        char  func_name_stages[MAX_NAME];
} t_erl;
//------------------------------------------------------------------------------
typedef struct {
        char *mean;
        int   func_hand_mean;
        char  func_name_mean[MAX_NAME];
        char *var;
        int   func_hand_var;
        char  func_name_var[MAX_NAME];
} t_log_norm;
//------------------------------------------------------------------------------
typedef struct {
        char *mean;
        int   func_hand_mean;
        char  func_name_mean[MAX_NAME];
        char *var;
        int   func_hand_var;
        char  func_name_var[MAX_NAME];
        char *minimum;
        int   func_hand_minimum;
        char  func_name_minimum[MAX_NAME];
        char *maximum;
        int   func_hand_maximum;
        char  func_name_maximum[MAX_NAME];
} t_trunclog_norm;
//------------------------------------------------------------------------------
typedef struct {
        char *scale;
        int   func_hand_scale;
        char  func_name_scale[MAX_NAME];
        char *shape;
        int   func_hand_shape;
        char  func_name_shape[MAX_NAME];
} t_weib;
//------------------------------------------------------------------------------
typedef struct {
        char *scale;
        int   func_hand_scale;
        char  func_name_scale[MAX_NAME];
        char *shape;
        int   func_hand_shape;
        char  func_name_shape[MAX_NAME];
} t_par;
//------------------------------------------------------------------------------
typedef struct {
        char *scale;
        int   func_hand_scale;
        char  func_name_scale[MAX_NAME];
        char *shape;
        int   func_hand_shape;
        char  func_name_shape[MAX_NAME];
        char *maximum;
        int   func_hand_maximum;
        char  func_name_maximum[MAX_NAME];
} t_trunc_par;
//------------------------------------------------------------------------------
typedef struct {
        char  filename[MAX_NAME];
} t_file;
//------------------------------------------------------------------------------
typedef struct {
        TGFLOAT  mean;
        TGFLOAT  var;
        int      max_level;
        TGFLOAT  time_scale;
        TGFLOAT  hurst;
} t_fbm;
//------------------------------------------------------------------------------
typedef struct {
        TGFLOAT  mean;
        TGFLOAT  var;
        int      no_samples;
        TGFLOAT  time_scale;
        TGFLOAT  hurst;
} t_farima;
//------------------------------------------------------------------------------
// This is not a distribution indeed. It represents the REWARD_REACHED event type.
typedef struct {
        TGFLOAT delta_time;
} t_rew_reach;
//------------------------------------------------------------------------------
// This is not a distribution indeed. It makes an event turn into a initialization event
typedef struct {
} t_init;
//------------------------------------------------------------------------------
/* the union of the distributions */
typedef union {
        t_exp              exp;
        t_det              det;
        t_unif             uni;
        t_gauss            gauss;
        t_erl              erl;
        t_log_norm         log_norm;
        t_trunclog_norm    trunc_log_norm;
        t_fbm              fbm;
        t_farima           farima;
        t_file             file;
        t_weib             weib;
        t_par              par;
        t_trunc_par        trunc_par;
        t_rew_reach        rew_reach;
        t_init             init;        
} t_distrib;
//------------------------------------------------------------------------------
typedef struct {
        int    part;                        /*  object part being described:
                                             *  (event | message | reward) 
                                             */
        int    type;                        /* segment of the object part: 
                                             * (rate | condition | action | prob) 
                                             */
        char   name[MAX_NAME];              /* name of the segment described 
                                             */
        char   msg_port[MAX_NAME];          /* name of local port - just for 
                                             * message 
                                             */
        char   ev_name[MAX_NAME];           /* name of event associated with 
                                             * impulse reward 
                                             */
        char   reward_4sum_name[MAX_NAME];  /* name of event associated with 
                                             * impulse reward 
                                             */
        char   act_list[MAX_STRING];        /* list of actions associated with 
                                             * impulse reward */
 
        int    act_number;                  /* number of the action (used only 
                                             * for action) 
                                             */
        char   func_name[MAX_NAME];         /* name of C func handler to be 
                                             * generated 
                                             */
        int    func_hand;                   /* unique id to function handler 
                                             * used in user_code.c
                                             */
        char   *code;                       /* code of C func handler */
 
        char   expression[MAX_STRING];      /* code of expression in case of 
                                             * literal expressions 
                                             */
        char   expr_type;                   /* type of expression: executable 
                                             * or interpretable 
                                             */
 
        int       distrib_type;             /* distribution type */
        t_distrib distrib;                  /* distribution of the event */
 
        int       low_bounded, upp_bounded; /* rewards are lower/upper bounded*/
        TGFLOAT   low_bound, upp_bound;     /* value of the lower/upper bounds*/
        TGFLOAT   cr_init_val;              /* cr initial value of the reward */
 
} t_code;
//------------------------------------------------------------------------------
typedef union {
        int    i;                           /* integer value */
        TGFLOAT f;                          /* float value */
        char   str[MAX_NAME];               /* identifier (string) value */
        char   expr[MAX_STRING];            /* expression value */
        int     *int_vec;
        TGFLOAT *tgf_vec;
} t_value;
//------------------------------------------------------------------------------
typedef struct {
        char    name[MAX_NAME];   /* name of symbol */
        int     type;             /* type of symbol */
        t_value val;              /* value of symbol */
        int     initialized;      /* indicates if symbol has been initialized */
        int     max_value;        /* max value that a state variable can assume */
        int     dimension;        /* number of elements in the state vector */
} t_symbol_obj;
//------------------------------------------------------------------------------
typedef struct {
        char    name[MAX_NAME];   /* name of symbol */
        int     type;             /* type of symbol */
        int     dimension;        /* number of elements in the vector */
} t_symbol_ccode;
//------------------------------------------------------------------------------
typedef struct {
        char              name[MAX_NAME]; /* name of object */
        t_code           *code_tb;        /* pointer to code table */
        int               code_entries;   /* number of entries in code table */
        t_symbol_obj     *sym_tb;         /* pointer to symbol table */
        int               sym_entries;    /* number of entries in sym table */
} t_obj_desc;
//------------------------------------------------------------------------------
typedef struct {
        int       type;
        char      name[MAX_NAME];
        int       act_number;
        int       func_hand;
        char      func_name[MAX_NAME];
        char     *code;
        int       low_bounded, upp_bounded; /* rewards are lower/upper bounded */
        TGFLOAT    low_bound, upp_bound;     /* value of the lower/upper bounds */
} t_global_rew;
//------------------------------------------------------------------------------
typedef struct {
        int       type;            /* type of entry: EV | CHAIN | OBJ */
        char      name[MAX_NAME];  /* identifier name of the entry */
} t_indep_chain;
//------------------------------------------------------------------------------
typedef struct {          /* structure used to keep a array of reserved words */
    char name[MAX_NAME];  /* name of a reserved word */
} t_reserved_word;
//------------------------------------------------------------------------------
typedef struct {
    char constname[ MAX_NAME ]; /* constant's name that is the dimension    */
    int  index;                 /* index on sym_tb_obj where var was stored */
} t_delayed_declarations;

//------------------------------------------------------------------------------
typedef struct {
    char filename[ MAX_NAME ];       /* constant's name that is the dimension    */
    int  linenumber;                 /* index on sym_tb_obj where var was stored */
} t_include_file;

//------------------------------------------------------------------------------
//     I N T E R F A C E
//------------------------------------------------------------------------------
void  initialize_current_status();
void  reset_current_status();

int   str_len( char * );
char *aloc_str( char * );
char *concat_str( char *, char *, char * );

int   update_code_tb( int, char *, int, char * );
int   update_code_tb_msg(char *);
int   update_code_tb_impulse( int, char *, char *, char * );
int   update_code_tb_bounds( char *, char *, char * );
int   update_code_tb_cr_init_val( char *, char * );

int   update_distrib_expr( int, char *, char *, char * );
int   update_distrib_num( int, char *, char *, char *, char *, char * );

int   update_glob_rew_tb_bounds( char *, char *, char * );
int   update_glob_rew_tb( int, char *, char * );

int   compound_id_is_valid( char *, int );

int   update_indep_chain_tb( int, char * );
int   is_object( char * );

void  compose_func_name ( int );
void  compose_distrib_name ( int );
void  print_tables ();
void  print_code_tb( int, t_code * );
void  print_global_rew_tb();
void  print_indep_chain_tb();

void  sym_trim( char * );
int   sym_tb_obj_insert ( int type, char *name );
int   sym_tb_obj_query (char *name);
int   sym_tb_obj_get_type (char *name);
int   sym_tb_obj_get_init_status(char *a_name);
int   sym_tb_obj_set_init_status(char *a_name, int value);
int   sym_tb_obj_set_val (char *name, char *value);
int   sym_tb_obj_get_val (char *name, char *value);
int   sym_tb_obj_set_max_val(char *name, int max_val);
int   sym_tb_obj_ckh_init ();
int   sym_tb_obj_print (int, t_symbol_obj *);
int   sym_tb_obj_set_dim(char *name, int dimension);
int   sym_tb_obj_get_dim(char *name);
int   sym_tb_obj_set_vec_val(char *name, char *value_list);
char *sym_tb_obj_get_vec_val(t_symbol_obj *symbol, int type);
int   sym_tb_obj_set_vec_all(char *name, int value);
int   sym_tb_obj_set_fvec_all(char *name, TGFLOAT value);
int   sym_tb_obj_chk_init();

int   sym_tb_ccode_insert (int type, char *name);
int   sym_tb_ccode_query (char *name);
int   sym_tb_ccode_get_type (char *name);
int   sym_tb_ccode_set_dim (char *name, int dimension);
int   sym_tb_ccode_get_dim (char *name);
int   sym_tb_ccode_print ();

int   ref_sym_tb_insert (int a_type, char *a_name);
int   ref_sym_tb_query (char *name, int a_type);

int   obj_desc_tb_insert (char *);

int   check_parameter_sym (char *);
int   check_vector_sym (char *);
int   check_ref_sym();
int   is_reserved_word(char *);
int   check_event_distrib_type (char *, int);

int   delayed_tb_insert( char *, int );
int   resolve_delayed_sym( char * );

void  yyerror (const char *);
int   check_for_st_var();

int   compare_types( int, int );

extern int mode;

/* Error and Warning handling functions and variables */
extern char *error_msg[];
extern char *warning_msg[];
void error_handler  (int , char *);
void warning_handler(int , char *);

/* MTK related functions */
char *getMTKTokenName( void );
char *getMTKObjectName( char * );
char *getParameterMask( const char * );

#endif /* __GRAMATICA_H__ */
//------------------------------------------------------------------------------
