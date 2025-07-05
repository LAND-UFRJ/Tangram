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
*  @file system_state.h
*  @brief <b>classes for describing the system</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram II project
*  @since version 1.0
*
*/


#ifndef __SYSTEM_STATE_H__
#define __SYSTEM_STATE_H__

#include "T2String.h"

#define STATE_VAR_INT          1
#define STATE_VAR_FLOAT        2
#define STATE_VAR_INTQUEUE     3
#define STATE_VAR_FLOATQUEUE   4

typedef class System_State : public Chained_Element {
   Object_State_List    *obj_st_list;
   Transition_List      *trans_list;
   Pending_Message_List *pend_msg_list;
   Rate_Reward_List     *rate_rew_list;

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
   System_State();
   System_State(System_State *);
   ~System_State();

   void add_obj_st(Object_State*);
   Object_State_List *show_obj_st_list(int);
   Object_State *show_obj_st(Object_Description *);
   Object_State *show_obj_st(int);

   void add_transition(Transition*);
   Transition_List *show_trans_list(int);
   Transition *show_1st_trans();
   Transition *get_1st_trans();

   void                   add_pend_msg(Pending_Message *);
   Pending_Message_List  *show_pend_msg_list(int);
   int                    show_no_pend_msg();

   System_State_List  *search_for_leaves(int *, Expr_Val **);
   System_State_List  *process_event(Object_State *, Event *, int, int);
   System_State_List  *process_pend_msg (Pending_Message *, int, int);
   System_State_List  *process_act_list(Object_State *, Action_List *, int, int);
   int get_enabled_det_ev(int *, int *);

   int set_state_var (unsigned int *);

   int replace_obj_st (Object_State *); 

   int compare_sys_st (System_State *); 

   int copy_pend_msg (Object_State *);

   void calculate_rate_rewards( int );
   void calculate_global_rate_rewards();
   void calculate_impulse_rewards(Object_State *, Action *);

   Rate_Reward_List *get_rate_reward_list();
   Rate_Reward_List *show_rate_rew_list(int);
   TGFLOAT show_reward_val(const char *);

   void get_st_vec(unsigned int *);
   void get_st_vec(unsigned int *, int *, int);

   void ch_st_var_val(char *, char *, int);
   void ch_st_var_val(char *, char *, int, int);
   int *show_st_var_val(char *, char *);

   void print_sys_st();
   void print_sys_st(FILE*);

   void print_van_sys_st();
   void print_sys_st_trans();

} System_State;

typedef class System_State_List : public Chained_List {

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    System_State_List();
    System_State_List(System_State *);
    System_State_List(System_State_List *);
    ~System_State_List();

    int add_head_sys_st(System_State *);
    int add_tail_sys_st(System_State *);
    System_State *show_1st_sys_st();
    System_State *get_1st_sys_st();
    System_State *show_next_sys_st();
    System_State *show_curr_sys_st();
    int show_no_sys_st();

    void print_sys_st_list();
    void print_van_sys_st_list();

} System_State_List;

class IntegerQueue;
class FloatQueue;

/**
* Collection of objects that are states.<br>
* The object has a description that tells which kind of object it is and its data,
* that has the initial values given by the user.
* <br>Fluid simulation has inserted some float state functions that manipulates the version 2.0 State_Variable.
* @class Object_State
* @brief Stores states
* @version 2.0
* @since 1.0
* @author Daniel Ratton Figueredo (1.0)
* @author Carlos F. Brito (1.0)
* @author Kelvin Reinhardt (2.0)
* @author Bruno Felisberto (2.0)
*
*/
typedef class Object_State : public Chained_Element {
    
    /** Object identifier */
    int                      id;
    /** Object description */
    Object_Description      *obj_desc;

    State_Variable_List     *st_var_list;
    Pending_Message_List    *pend_msg_list;

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    Object_State();
    Object_State(Object_Description*);
    Object_State(Object_State *);
    ~Object_State();

    void ch_id(int);
    int show_id();

    void ch_obj_desc(Object_Description *);
    Object_Description * show_obj_desc();

    void add_pend_msg(Pending_Message*);
    void add_pend_msg(const char *, const char *, TGFLOAT, int);
    void add_pend_msg(const char *, const char *, int, int);
    void add_pend_msg(const char *, const char *, TGFLOAT *, int );
    void add_pend_msg(const char *, const char *, int *, int );
    Pending_Message_List *show_pend_msg_list(int);
    Pending_Message *get_1st_pend_msg();
    Pending_Message *get_msg_info( int *, const char *, int *, int * );
    void destroy_msg(Pending_Message *);

    State_Variable_List *show_st_var_list(int);
    void add_state_var(State_Variable*);

    TGFLOAT *show_st_float_var_value(int);
    TGFLOAT show_st_var_float_value(const char*);
    TGFLOAT *show_st_var_fvec_value(const char*);

    void save__at_tail( IntegerQueue &,      int * );
    void save__at_head( IntegerQueue &,      int * );
    void restore__from_tail( IntegerQueue &, int * );
    void restore__from_head( IntegerQueue &, int * );

    void save__at_tail( FloatQueue &,      TGFLOAT * );
    void save__at_head( FloatQueue &,      TGFLOAT * );
    void restore__from_tail( FloatQueue &, TGFLOAT * );
    void restore__from_head( FloatQueue &, TGFLOAT * );

    int* show_st_var_value(int);
    int  show_st_var_dimension(int);
    int  show_st_var_dimension(char *);
    int  show_st_var_int_value(const char *);
    int* show_st_var_vec_value(const char *);
    void ch_st_var_value(const char *, int);
    void ch_st_var_value(const char *, int *);
    void ch_st_var_value(const char *, TGFLOAT);
    void ch_st_var_value(const char *, TGFLOAT *);
    void ch_st_var_value(const char *, IntegerQueue & );
    void ch_st_var_value(const char *, FloatQueue & );
    void get_st_var_value(int *, const char *);
    void get_st_var_value(int &, const char *);
    void get_st_var_value(TGFLOAT &, const char *);
    void get_st_var_value(TGFLOAT *, const char *);
    void get_st_var_value(IntegerQueue &, const char *);
    void get_st_var_value(FloatQueue &, const char *);

    void get_msg_data_vec( int *, int, Pending_Message * );
    void get_msg_data_vec( TGFLOAT *, int, Pending_Message * );

    int  compare_obj_st(Object_State *);

    void print_obj_st();
    void print_obj_st(FILE*);
    void print_van_obj_st();

    void copy( int *,     int, int *,     int );
    void copy( TGFLOAT *, int, TGFLOAT *, int );
    void copy( IntegerQueue &, int, IntegerQueue &, int );
    void copy( FloatQueue &,   int, FloatQueue &,   int );

    void printErrorMessage( const char *, int );
} Object_State;

typedef class Object_State_List : public Chained_List {

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    Object_State_List();
    Object_State_List(Object_State_List *);
    ~Object_State_List();

    int add_tail_obj_st(Object_State *);
    Object_State *show_1st_obj_st();
    Object_State *get_1st_obj_st();
    Object_State *show_next_obj_st();
    Object_State *show_curr_obj_st();
    int query_obj_st(int);
    int replace_obj_st(Object_State *);
    int show_no_obj_st();
} Object_State_List;


/**
* Float values where inserted here to be able to simulate fluid models.<br>
* <br>
* This class has two kind of vectors: <b>int</b> and <b>float</b>.
* By the way it is initialized it knows which kind of state var it stores.
* @class State_Variable
* @brief This class stores state variables (int and float)
* @version 2.0
* @since 1.0
* @author Daniel Ratton Figueredo (1.0)
* @author Carlos F. Brito (1.0)
* @author Kelvin Reinhardt (2.0)
* @author Bruno Felisberto (2.0)
* @warning It is not possible to store both variables <b>int</b> and <b>float</b>.
* @remarks Float var types can be changed as integer ones, but the other way is not possible.
*
*/
#include "queue_type.h"

typedef class State_Variable : public Chained_Element
{
    /** Instance type of state: integer or float. */
    int state_var_type;
    /** symbol table code */
    int  code;
    /** dimension of the vector */
    int  dimension;
    /** This is a chained list that saves / restores State_Variable#value or State_Variable#float_value
    * throught user requests.
    */
    union
    {
        int          *intvalue;
        TGFLOAT      *float_value;
        IntegerQueue *intqueue;
        FloatQueue   *floatqueue;
    } uvalue;

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    State_Variable();
/// Initializes as an integer vector
    State_Variable(int *, int, int);
/// Initializes as an integer vector of 1 position only
    State_Variable(int, int);
/// Initializes as a float vector
    State_Variable(TGFLOAT *, int, int);
/// Initializes as a float vector of 1 position only
    State_Variable(TGFLOAT, int);
    State_Variable( IntegerQueue *, int );
    State_Variable( FloatQueue *, int );
/// Initializes as clone of some other instance
    State_Variable(State_Variable *);
/// Releases memory
    ~State_Variable();

/// Changes int (or float) state vector value
    void    ch_value(int *);
/// Changes int (or float) state vector value of 1 position only
    void    ch_value(int, TGFLOAT);
    void    ch_value( IntegerQueue & );
    void    ch_value( FloatQueue & );
/// Duplicates the int state vector and passes its pointer
    int*    show_value();
/// Duplicates the float state vector and pass its pointer
    TGFLOAT*  show_float_value();
    IntegerQueue &show_intqueue_value();
    FloatQueue &show_floatqueue_value();
/// Changes float state vector value
    void    ch_float_value(TGFLOAT *);
/// Changes float state vector value of 1 position only
    void    ch_float_value(int, TGFLOAT);
/// Passes float_value's pointer
    TGFLOAT*  get_float_value();
/// Show which kind of state variable it is: float or int
    int     show_state_var_type();

/// Passes value's pointer
    int*    get_value();
/// Changes the instance code on the symbol table
    void ch_code(int);
/// Shows the instance code on the symbol table
    int  show_code();

/// Shows the state vector size
    int  show_dimension();

/** Compare two state variables...
*  if they have different kinds they are considered different,
*  even if they have the same float to int value.
*/
    int  compare(State_Variable *);
/// Prints the state variable values
    void print_st_var();
/// Prints the state variable values to a file
    void print_st_var(FILE*);

} State_Variable;

typedef class State_Variable_List : public Chained_List {

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    State_Variable_List();
    State_Variable_List(State_Variable_List *);
    ~State_Variable_List();

    int add_tail_st_var(State_Variable *);
    State_Variable *show_1st_st_var();
    State_Variable *get_1st_st_var();
    State_Variable *show_next_st_var();
    State_Variable *show_curr_st_var();
    int query_st_var(int);
    int show_no_st_var();

} State_Variable_List;

typedef class Transition : public Chained_Element {
    System_State        *sys_st_dest;
    int                  st_dest;
    Expr_Val            *rate;
    Imp_Reward_List     *impulse_rew_list;
    /* the following attributes are only used with DETERMINISTIC chains */ 
    int                  ev_id;     /**< DET evt that originated this
                                         transition zero if the event is
                                         exponential  */
    int                  disabled;  /**< Flag to indicate if the DET event   
                                         above was disabled                */
    int                  obj_id;    /**< ID of the object containing the event
                                         that fired this transition. */
    char                 add_later; /**< a: an exponential transition to this 
                                         destination already exists. if we
                                         consider all transitions exponential
                                         this rate should be added to the
                                         corresponding exponential rate.
                                         d: */
#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    Transition();
    Transition(Transition *);
    Transition(Expr_Val *);
    ~Transition();

    void ch_st_dest(int);
    int  show_st_dest();

    void ch_ev_id(int);
    int  show_ev_id();

    void ch_disabled(int);
    int  show_disabled();

    void ch_obj_id(int);
    int  show_obj_id();

    void ch_sys_st_dest(System_State *);
    System_State  *show_sys_st_dest();

    Imp_Reward_List *show_imp_rew_list(int);
    void add_imp_rew(int, Expr_Val *, Expr_Val *);
    void add_imp_rew(Imp_Reward *);

    void      ch_rate(Expr_Val *);
    Expr_Val *show_rate();
    int    show_rate_type();
    TGFLOAT show_rate_value();
    void   show_rate_str(char *);

    void   sum_rate(Expr_Val *);
    void   mul_rate(Expr_Val *);
    int    rate_is_zero();

    void set_add_later();
    char show_add_later();
    
    void markDeleted();
    Transition *getSameTransition();

    void print_trans();

} Transition;


typedef class Transition_List : public Chained_List {

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    Transition_List();
    Transition_List(Transition_List *);
    ~Transition_List();

    int add_tail_trans(Transition *);
    int add_prev_trans(Transition *);
    Transition *show_1st_trans();
    Transition *get_1st_trans();
    Transition *show_next_trans();
    Transition *show_curr_trans();
    int query_trans(int);
    int query_trans(System_State *);
    int show_no_trans();

} Transition_List;
//------------------------------------------------------------------------------
typedef class Pending_Message : public Chained_Element {
    char    obj_source_name [MAXSTRING];
    char    obj_dest_name [MAXSTRING];
    char    port_name [MAXSTRING];
    TGFLOAT message_data;

    TGFLOAT *message_data_float_vec;
    int     *message_data_int_vec;

    /** vector size in bytes */
    int      vector_size;

    int      message_type;

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    Pending_Message();
    Pending_Message(char *, char*, char *, TGFLOAT);
    Pending_Message(char *, char*, char *, TGFLOAT *, int );
    Pending_Message(char *, char*, char *, int *, int );
    Pending_Message(Pending_Message *);
    ~Pending_Message();

    void ch_src_obj_name(char *);
    void show_src_obj_name(char *);

    void ch_dest_obj_name(char *);
    void show_dest_obj_name(char *);

    void ch_port_name(char *);
    void show_port_name(char *);

    int      show_message_type();
    void     ch_message_type(int);

    int      show_message_size();

    TGFLOAT  show_message_data();
    void     ch_message_data(TGFLOAT);
    int      show_vector_size();
    void     ch_vector_size(int);

    int     *show_message_data_int_vec();
    void     ch_message_data_int_vec(int *,int);
    TGFLOAT *show_message_data_float_vec();
    void     ch_message_data_float_vec(TGFLOAT *,int);

    Pending_Message_List *expand();
    int is_multidest();

    void print_pend_msg();

} Pending_Message;


typedef class Pending_Message_List : public Chained_List {

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    Pending_Message_List();
    Pending_Message_List(Pending_Message_List *);
    ~Pending_Message_List();

    int add_tail_pend_msg(Pending_Message *);
    int add_head_pend_msg(Pending_Message *);
    int concat_pend_msg_list(Pending_Message_List *);
    Pending_Message *show_1st_pend_msg();
    Pending_Message *get_1st_pend_msg();
    Pending_Message *get_last_pend_msg();
    Pending_Message *show_next_pend_msg();
    int show_no_pend_msg();

} Pending_Message_List;


typedef class Known_State : public Chained_Element {
    unsigned int      st_id;
    Expr_Val         *total_trans_rate;
    Transition_List  *trans_list;
    Rate_Reward_List *reward_list;
    int               det_ev_id;
    Expr_Val         *det_ev_rate;

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    Known_State();
    Known_State(unsigned int);
    ~Known_State();

    void ch_st_id(int);
    int show_st_id();

    Expr_Val *show_trans_rate();
    TGFLOAT    show_trans_rate_value();

    int  get_st_vec(unsigned int *);

    void add_trans(Transition *);
    Transition_List *show_trans_list(int);

    void add_reward(Rate_Reward *);
    Rate_Reward_List *show_reward_list(int);
    void ch_reward_list(Rate_Reward_List *);

    int  show_det_ev_id();
    void ch_det_ev_id(int);
    Expr_Val *show_det_ev_rate();
    void      ch_det_ev_rate(Expr_Val *);

    void print_st_id();

} Known_State;


typedef class Known_State_List : public Chained_List {

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    Known_State_List();
    Known_State_List(Known_State_List *);
    ~Known_State_List();

    int add_tail_known_st(Known_State *);
    int add_prev_known_st(Known_State *);
    Known_State *show_1st_known_st();
    Known_State *get_1st_known_st();
    Known_State *show_next_known_st();
    Known_State *show_curr_known_st();
    int query_known_st(int);
    int show_no_known_st();
    
} Known_State_List;

typedef class Rate_Reward : public Chained_Element {
    int       id;
    Expr_Val *value;

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    Rate_Reward(int, Expr_Val *);
    ~Rate_Reward();

    int       show_id();

    void      ch_value(Expr_Val *);
    Expr_Val *show_value();

    void print_reward();
} Rate_Reward;


typedef class Rate_Reward_List : public Chained_List {

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    Rate_Reward_List();
    Rate_Reward_List(Rate_Reward_List *);
    ~Rate_Reward_List();

    int add_tail_rew(Rate_Reward *);
    Rate_Reward *show_1st_rew();
    Rate_Reward *get_1st_rew();
    Rate_Reward *show_next_rew();
    Rate_Reward *show_curr_rew();
    int query_rew(int);
    int show_no_rew();
    
} Rate_Reward_List;


typedef class Imp_Reward : public Chained_Element {
    int               id;
    Imp_Rew_Val_List *imp_val_list;

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    Imp_Reward(int);
    Imp_Reward(Imp_Reward *);
    ~Imp_Reward();

    int  show_id();

    void add_impulse(Imp_Rew_Val *);
    Imp_Rew_Val_List *show_imp_val_list(int);
    Imp_Rew_Val *show_1st_imp_val();
        
    void print_reward();
} Imp_Reward;


typedef class Imp_Reward_List : public Chained_List {

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    Imp_Reward_List();
    Imp_Reward_List(Imp_Reward_List *);
    ~Imp_Reward_List();

    int add_tail_rew(Imp_Reward *);
    Imp_Reward *show_1st_rew();
    Imp_Reward *get_1st_rew();
    Imp_Reward *show_next_rew();
    Imp_Reward *show_curr_rew();
    int query_rew(int);
    int show_no_rew();
    
} Imp_Reward_List;



typedef class Imp_Rew_Val : public Chained_Element {
    Expr_Val   *value;
    Expr_Val   *prob;

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    Imp_Rew_Val();
    Imp_Rew_Val(Expr_Val *, Expr_Val *);
    ~Imp_Rew_Val();

    void      ch_value(Expr_Val *);
    Expr_Val *show_value();

    void      ch_prob(Expr_Val *);
    Expr_Val *show_prob();

    void print_imp_val();
} Imp_Rew_Val;


typedef class Imp_Rew_Val_List : public Chained_List {

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    Imp_Rew_Val_List();
    Imp_Rew_Val_List(Imp_Rew_Val_List *);
    ~Imp_Rew_Val_List();

    int add_tail_rew(Imp_Rew_Val *);
    Imp_Rew_Val *show_1st_rew();
    Imp_Rew_Val *get_1st_rew();
    Imp_Rew_Val *show_next_rew();
    Imp_Rew_Val *show_curr_rew();
    int query_rew(Expr_Val *);
    int show_no_rew();
    
} Imp_Rew_Val_List;

typedef class Expr_Val : public Chained_Element {
    TGFLOAT    value;
    T2String   str_val;
    int        type;

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

public:
    /** Constructor & Destructor */
    /**@{*/
    Expr_Val ( void );
    Expr_Val ( TGFLOAT );
    Expr_Val ( const char * );
    Expr_Val ( Expr_Val * );
    ~Expr_Val( void );
    /**@}*/
    
    /** Member variables access methods */
    /**@{*/
    int          show_type   ( void );
    TGFLOAT       show_value  ( void );
    const char * show_str_val( void );
    void         ch_value    ( TGFLOAT );
    /**@}*/
    
    /** Operations methods */
    /**@{*/
    void add_expr_val      ( Expr_Val * );
    void add_expr_val      ( TGFLOAT );
    void mul_expr_val      ( Expr_Val * );
    void mul_expr_val      ( TGFLOAT );
    void divide_by_expr_val( Expr_Val * );
    int  comp_expr_val     ( Expr_Val * );
    int  comp_expr_val     ( TGFLOAT );
    int  is_zero           ( void );
    /**@}*/
    
    /** Content visualization */
    /**@{*/    
    void print_expr( void );
    void print_expr( FILE * );
    /**@}*/    

} Expr_Val;

typedef class Expr_Val_List : public Chained_List {

#ifdef _MEM_DEBUG
   int DBG_id;
#endif

 public:
    Expr_Val_List();
    Expr_Val_List(Expr_Val_List *);
    ~Expr_Val_List();

    int  add_tail_expr_val(Expr_Val *);
    Expr_Val *show_1st_expr_val();
    Expr_Val *get_1st_expr_val();
    Expr_Val *show_next_expr_val();
    Expr_Val *show_curr_expr_val();
    void clear_expr_val();
    int  show_no_expr_val();
    
} Expr_Val_List;


#endif  /*  __SYSTEM_STATE_H__ */
