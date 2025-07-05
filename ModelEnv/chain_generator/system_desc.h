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
*  @file system_desc.h
*  @brief <b>classes for describing the system</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram II project
*  @since version 1.0
*
*/

#ifndef __SYSTEM_DESC_H__
#define __SYSTEM_DESC_H__

#include "gramatica.h"

typedef struct {
    char name[MAXSTRING];
    int  id;
} id_name_t;

typedef class System_Description {

   Object_Description_List *obj_desc_list;
   int  last_obj_id;    /**< last number of an object */
   int  chain_type;     /**< chain type: LITERAL or NUMERICAL */
   int  rate_reward;    /**< rate rewards defined (T|F)    */
   int  impulse_reward; /**< impulse rewards defined (T|F) */
   int  det_model;      /**< DET model generation (T|F) */
   int  no_det_events;  /**< number of deterministic events in the model */

   id_name_t *imp_rew_tb;   /**< table of impulse reward name vs id */
   int        imp_rew_size; /**< size of the table above */

   Rate_Reward_Desc_List  *global_rew_desc_list;

 public:
   System_Description();
   ~System_Description();

   void add_obj_desc(Object_Description *);
   Object_Description_List *show_obj_desc_list(int);
   Object_Description *show_obj_desc(char *);
   Object_Description *show_obj_desc(int);

   void add_global_rew_desc(Rate_Reward_Desc *);
   Rate_Reward_Desc_List *show_global_rew_desc_list(int);
   Rate_Reward_Desc *show_global_rew_desc(char *);

   void set_chain_type(int);
   int  show_chain_type();
   void set_rate_reward(int);
   int  show_rate_reward();
   void set_impulse_reward(int);
   int  show_impulse_reward();

   Event *show_ev_by_name(char *);
   void set_det_model(int);
   int  show_det_model();
   void set_no_det_ev(int);
   int  show_no_det_ev();
   Event *show_det_ev_by_id(int);
   Object_Description *show_obj_desc_by_det_ev_id(int);
   void det_model_error(int, int);

   void set_imp_rew_name(char *, int);
   void show_imp_rew_name(char *, int);

   int show_no_obj();
   int show_no_st_var();
   int show_no_st_var(int *, int);

   void reorder_objects();
   void get_max_value_vec(unsigned int *);
   void get_max_value_vec(unsigned int *, int *, int);

   String_List *show_objs_by_port(char *);

   void print_sys_desc();
} System_Description;


typedef class Object_Description : public Chained_Element {

   int            id;
   char           name[MAXSTRING]; /**< name of this object */
   Symbol_List              *symbol_list;  /**< Points to symbol table of this object */
   Event_List               *event_list;   /**< Points to a list of event */
   Message_List             *message_list; /**< list of messages to process */
   Rate_Reward_Desc_List    *reward_list;  /**< list of rate rewards descriptions */
   Rate_Reward_Sum_List     *rate_rew_sum_list;  /**< rate reward sum list */

 public:
   Object_Description();
   Object_Description(char *);
   ~Object_Description();

   int   show_id();
   void  ch_id(int);

   void  show_name(char *);
   void  ch_name(char*);

   Symbol      *show_symbol(char *);
   Symbol_List *show_symbol_list(int);
   void         add_symbol(Symbol *);

   Event       *show_ev_by_name(char *);
   Event_List  *show_event_list(int);
   void         add_event(Event *);
   Event       *show_det_ev_by_id(int);

   Message      *show_message(char *);
   Message_List *show_message_list(int);
   void          add_msg(Message *);

   Rate_Reward_Desc_List *show_reward_list(int);
   Rate_Reward_Sum_List  *show_rate_rew_sum_list(int);

   void  add_reward(Rate_Reward_Desc *);
   void  add_rate_rew_sum(Rate_Reward_Sum *);

   void  replace_symbols(Object_State *, char *);

   int   show_symbol_code(char *);

   int   show_no_st_var();

   int   has_port(char *);

   void  print_obj_desc();

} Object_Description;


typedef class Object_Description_List : public Chained_List {

 public:
    Object_Description_List();
    Object_Description_List(Object_Description_List *);
    ~Object_Description_List();

    int add_tail_obj_desc(Object_Description *);
    Object_Description *show_1st_obj_desc();
    Object_Description *get_1st_obj_desc();
    Object_Description *del_obj_desc();
    Object_Description *show_next_obj_desc();
    Object_Description *show_curr_obj_desc();
    int query_obj_desc(char *);
    int query_obj_desc(int);
    int show_no_obj_desc();

} Object_Description_List;



typedef class Event : public Chained_Element {
    char              name[MAXSTRING];
    int               det_id;  /**< zero if the event is exponential */
    Expression       *condition;
    Action_List      *action_list;
    Distribution     *distrib;
    Indep_Chain_List *indep_chain_list;
    Expr_Val         *det_ev_rate;

 public:
    Event();
    Event(char *);
    ~Event();

    void ch_name(char *);
    void show_name(char *);

    void ch_det_id(int);
    int  show_det_id();

    Expr_Val *eval_rate(Object_State *);

    void ch_cond(Expression *);
    Distribution *show_distrib();
    int  eval_cond(Object_State *);

    void          add_action(Action *);
    Action_List  *show_action_list(int);

    void              add_indep_chain(Indep_Chain *);
    Indep_Chain_List *show_indep_chain_list(int);
    int check_indep_chains(Object_Description *, Object_Description *);
    void add_st_to_indep_chains(System_State *, System_State_List *);
    int  show_absorbing_indep_chain();
    Expr_Val *show_det_ev_rate();
    void      ch_det_ev_rate(Expr_Val *);
    int  indep_chain_by_obj_id(int);
    int  show_no_indep_chains();
    int  show_indep_chain_num(int);
    void set_zero();

    void   ch_distrib(Distribution *);
    int    show_distrib_type();
    TGFLOAT generate_sample(Object_State *);
    void   reset_distrib(int);

    void print_event();

} Event;


typedef class Event_List : public Chained_List {

 public:
    Event_List();
    Event_List(Event_List *);
    ~Event_List();

    int add_tail_event(Event *);
    Event *show_1st_event();
    Event *get_1st_event();
    Event *show_next_event();
    Event *show_curr_event();
    int    query_event(char *);
    int    show_no_event();

} Event_List;


typedef class Indep_Chain : public Chained_Element {
    int   obj_ids[MAXINDEPCHAIN];
    int   no_objs;
    Hash *hashtb;
    Known_State_List *chain;
    unsigned int     *state_vec;
    Known_State_List *absorb_list;
    t_map *map_tb; /**< mapping vector to renumber states */

 public:
    Indep_Chain();
    ~Indep_Chain();

    int  add_obj_id(int);
    int  query_obj_id(int);
    int  query_obj_id(int, int);
    int  init(System_Description *);

    void add_states(System_State *, System_State_List *);

    int  is_absorbing();
    void create_map();
    void print_map(FILE *);
    void print_abs_states (FILE *);
    void print_states(FILE *);
    int  show_no_states();
    Known_State_List *show_chain();

    void print_indep_chain(FILE *, TGFLOAT);
    void print_absorb_states(FILE *, Integer_List **);

} Indep_Chain;

typedef class Indep_Chain_List : public Chained_List {
 public:
    Indep_Chain_List();
    Indep_Chain_List(Indep_Chain_List *);
    ~Indep_Chain_List();

    int add_tail_indep_chain(Indep_Chain *);
    int add_head_indep_chain(Indep_Chain *);
    Indep_Chain *show_1st_indep_chain();
    Indep_Chain *get_1st_indep_chain();
    Indep_Chain *show_next_indep_chain();
    Indep_Chain *show_curr_indep_chain();
    int          show_no_indep_chain();

} Indep_Chain_List;

typedef class Message : public Chained_Element {
    Action_List     *action_list;
    char          port_name[MAXSTRING]; /**< port where messages are delivered */

 public:
    Message();
    Message(char *);    /**< initialization with src_obj and port_name */
    ~Message();

    void ch_port_name(char *);
    void show_port_name(char *);

    void add_action(Action *);
    Action_List *show_action_list(int);

    void print_message();

} Message;


typedef class Message_List : public Chained_List {

 public:
    Message_List();
    Message_List(Message_List *);
    ~Message_List();

    int      add_tail_msg(Message *);
    Message *show_1st_msg();
    Message *get_1st_msg();
    Message *show_next_msg();
    Message *show_curr_msg();
    int      query_msg(char *);
    int      show_no_msg();

} Message_List;


typedef class Rate_Reward_Desc : public Chained_Element {
    char              name[MAXSTRING];
    int               id;
    Expression_List  *cond_list;
    Expression_List  *rate_list;
    Expr_Val_List    *value_list;
    int               low_bounded, upp_bounded;
    TGFLOAT           low_bound, upp_bound;
    int               ir_user_set_flag;
    TGFLOAT           ir_user_set_value;
    TGFLOAT           cr_initial_value;
    int               sum_bounded;
    Rate_Reward_Sum  *sum_bound;
    int               has_level;
    TGFLOAT           rew_level;
    Reward_Measure   *rew_meas;
    /** used to verify the slope's change of the cumulative reward */
    TGFLOAT           last_ir_val;

 public:
    Rate_Reward_Desc(char *, int);
    ~Rate_Reward_Desc();

    void show_name(char *);
    int  show_id();
    void ch_name(char *);

    void       add_cond(Expression *);
    void       add_rate(Expression *);
    Expr_Val*  eval_rate(Object_State *);
    Expr_Val*  eval_rate(Object_State *, System_State *);

    int     read_rew_level(const char *);
    TGFLOAT show_rew_level();
    void    ch_rew_level(TGFLOAT);
    int     show_has_level();
    void    ch_has_level( int );

    TGFLOAT show_low_bound();
    void    ch_low_bound(TGFLOAT);
    TGFLOAT show_upp_bound();
    void    ch_upp_bound(TGFLOAT);
    int     is_low_bounded();
    void    ch_low_bounded(int);
    int     is_upp_bounded();
    void    ch_upp_bounded(int);
    TGFLOAT show_cr_initial_value();
    void    ch_cr_initial_value(TGFLOAT);
    int     show_ir_user_set_flag();
    void    ch_ir_user_set_flag(int);
    TGFLOAT show_ir_user_set_value();
    void    ch_ir_user_set_value(TGFLOAT);
    void    print_reward();
    int     is_sum_bounded();
    void    ch_sum_bounded( int );
    TGFLOAT show_last_ir_val();
    void    ch_last_ir_val( TGFLOAT );

    Rate_Reward_Sum  *show_sum_bound();
    void   ch_sum_bound(Rate_Reward_Sum *);
    Reward_Measure  *show_rew_meas();
    void   ch_rew_meas(Reward_Measure *);

} Rate_Reward_Desc;

typedef class Rate_Reward_Desc_List : public Chained_List {

 public:
    Rate_Reward_Desc_List();
    Rate_Reward_Desc_List(Rate_Reward_Desc_List *);
    ~Rate_Reward_Desc_List();

    int               add_tail_rew(Rate_Reward_Desc *);
    Rate_Reward_Desc *show_1st_rew();
    Rate_Reward_Desc *get_1st_rew();
    Rate_Reward_Desc *show_next_rew();
    Rate_Reward_Desc *show_curr_rew();
    int               query_rew(char *);
    int               show_no_rew();
} Rate_Reward_Desc_List;


typedef class Imp_Reward_Desc : public Chained_Element {
    char              name[MAXSTRING];
    int               id;
    Expression       *impulse;
    int               low_bounded, upp_bounded;
    TGFLOAT           low_bound, upp_bound;
    TGFLOAT           cr_initial_value;
    Reward_Measure   *rew_meas;

 public:
    Imp_Reward_Desc(char *, int);
    ~Imp_Reward_Desc();

    void show_name(char *);
    int  show_id();
    void ch_name(char *);

    void      ch_impulse(Expression *);
    Expr_Val *eval_impulse(Object_State *);

    TGFLOAT show_low_bound();
    void   ch_low_bound(TGFLOAT);
    TGFLOAT show_upp_bound();
    void   ch_upp_bound(TGFLOAT);
    int    is_low_bounded();
    void   ch_low_bounded(int);
    int    is_upp_bounded();
    void   ch_upp_bounded(int);

    Reward_Measure  *show_rew_meas();
    void   ch_rew_meas(Reward_Measure *);

    TGFLOAT show_cr_initial_value();
    void   ch_cr_initial_value(TGFLOAT);
    void print_reward();

} Imp_Reward_Desc;

typedef class Imp_Reward_Desc_List : public Chained_List {

 public:
    Imp_Reward_Desc_List();
    Imp_Reward_Desc_List(Imp_Reward_Desc_List *);
    ~Imp_Reward_Desc_List();

    int              add_tail_rew(Imp_Reward_Desc *);
    Imp_Reward_Desc *show_1st_rew();
    Imp_Reward_Desc *get_1st_rew();
    Imp_Reward_Desc *show_next_rew();
    Imp_Reward_Desc *show_curr_rew();
    int              query_rew(char *);
    int              show_no_rew();
} Imp_Reward_Desc_List;


/** This class executes expressions. Rate, prob and condition are all instances
   of this class */
typedef class Expression : public Chained_Element {
    int  fd;     /* function descriptor to user code (expression) */
    char expr_code[MAXSTRING]; /* text of literal expression */
    int  type;   /* type of expression: interpretable or executable */

 public:
    Expression(int);
    Expression(int, char *, int);
    ~Expression();

    void ch_fd(int);
    int  show_fd();

    void ch_str(char *);
    void show_str(char *);

    void ch_type(int);
    int  show_type();

    Expr_Val *evaluate(Object_State *);
    Expr_Val *evaluate(Object_State *, System_State *);
    TGFLOAT    evaluate_numeric(Object_State *);

    void print_expression();

} Expression;

typedef class Expression_List : public Chained_List {

 public:
    Expression_List();
    Expression_List(Expression_List *);
    ~Expression_List();

    int add_tail_expr(Expression *);
    Expression *show_1st_expr();
    Expression *get_1st_expr();
    Expression *show_next_expr();
    Expression *show_curr_expr();
    int query_expr(int);
    int show_no_expr();
} Expression_List;

typedef class Action : public Chained_Element {
    Expression	 *prob;
    int           fd;       /* function descriptor to user code */
    Imp_Reward_Desc_List *impulse_list;

 public:
    Action();
    Action(int, Expression *);
    ~Action();

    void      ch_prob(Expression *);
    Expr_Val *eval_prob(Object_State *);

    void ch_fd(int);
    int  show_fd();
    Object_State *evaluate(Object_State *, Simulator *simulator);

    void                  add_impulse(Imp_Reward_Desc *);
    Imp_Reward_Desc_List *show_impulse_list(int);

    void print_action();
} Action;


typedef class Action_List : public Chained_List {

 public:
    Action_List();
    Action_List(Action_List *);
    ~Action_List();

    int add_tail_act(Action *);
    Action *show_1st_act();
    Action *get_1st_act();
    Action *show_next_act();
    int show_no_act();

} Action_List;


typedef class Symbol : public Chained_Element {
    char    name[MAXSTRING];
    int     type;
    t_value val;
    int     max_value;
    int     dimension;
    int     code;
    char    char_code;  /**< used to identify symbol in literal expression */

 public:
    Symbol();
    Symbol(char *, int, t_value, int, int);
    ~Symbol();

    void ch_name(char *);
    void show_name(char *);
    int  show_type();
    void show_value(t_value *);
    int* show_value();
 /** INSERTED BY BRUNO&KELVIN to have float states */
    TGFLOAT* show_float_value();
 /** END OF INSERTED BY BRUNO&KELVIN to have float states */
    void ch_code(int);
    int  show_code();
    void ch_char_code(char);
    char show_char_code();
    int  show_max_value();
    int  show_dimension();

    void print_sym();
    void print_sym(FILE *);

} Symbol;


typedef class Symbol_List : public Chained_List {

 public:
    Symbol_List();
    Symbol_List(Symbol_List *);
    ~Symbol_List();

    int add_tail_sym(Symbol *);
    Symbol *show_1st_sym();
    Symbol *get_1st_sym();
    Symbol *del_sym();
    Symbol *show_next_sym();
    Symbol *show_curr_sym();
    int query_symbol(char *);
    int show_no_sym();

} Symbol_List;

//------------------------------------------------------------------------------
typedef class Reward_Sum : public Chained_Element
{
    char             rew_name[MAXSTRING]; /**< reward name for sum */
    int              rew_id;              /**< reward id for sum */
    int              local_bound_reached;
    TGFLOAT          cross_time;
    TGFLOAT          virtual_ir;
    Reward_Measure  *rew_measure;         /**< pointer to the rew measure */

 public:
    Reward_Sum( );
    Reward_Sum(int, char *);
    Reward_Sum( Reward_Sum * );
    ~Reward_Sum();

    void   show_rew_name(char *);
    int    show_rew_id();
    void   ch_rew_id(int);
    void   add_rew_sum(Reward_Sum *);

    TGFLOAT show_cross_time();
    void   ch_cross_time(TGFLOAT);
    TGFLOAT show_virtual_ir();
    void   ch_virtual_ir(TGFLOAT);
    int    is_local_bound_reached();
    void   ch_local_bound_reached(int);

    Reward_Measure *show_rew_measure();
    void            ch_rew_measure( Reward_Measure * );
#ifdef TG_PARALLEL
    void   send_context();
    void   recv_context();
#endif
} Reward_Sum;
//------------------------------------------------------------------------------
typedef class Reward_Sum_List : public Chained_List
{
 public:
    Reward_Sum_List();
    Reward_Sum_List(Reward_Sum_List *);
    ~Reward_Sum_List();

    int         add_tail_rew(Reward_Sum *);
    Reward_Sum *show_1st_rew();
    Reward_Sum *get_1st_rew();
    Reward_Sum *show_next_rew();
    Reward_Sum *show_curr_rew();
    int         query_rew(int);
    int         show_no_rew();

#ifdef TG_PARALLEL
    void   send_context();
    void   recv_context();
#endif
} Reward_Sum_List;
//------------------------------------------------------------------------------
typedef class Rate_Reward_Sum : public Chained_Element
{
    int       id;                        /**< id for this kind of object */
    char      name[MAXSTRING];           /**< name of the rate reward sum */
    char      obj_name[MAXSTRING];       /**< name of the object */
    int       low_bounded, upp_bounded;  /**< flags for bounded measures */
    TGFLOAT    low_bound, upp_bound;      /**< value of the lower and upper bounds */
    int       treated;                   /**< flag to indicate that a sum was checked in simul pass (each event) */
    TGFLOAT    cross_time;                /**< keeps the time to cross when treated */
    Reward_Sum_List *rew_sum_list;       /**< list of rewards for sum */
    FILE            *fd_cr;              /**< file descriptor of the trace file for CR */
    FILE            *fd_ir;              /**< file descriptor of the trace file for IR */
    TGFLOAT          cr_initial_value;
    Reward_Measure  *rew_measure;        /**< pointer to the Reward_Measure associated with it */

 public:
    Rate_Reward_Sum( );
    Rate_Reward_Sum( char *, char *, int );
    Rate_Reward_Sum( Rate_Reward_Sum * );
    ~Rate_Reward_Sum();

    int  show_id();
    void show_name(char *);
    void show_obj_name(char *);

    TGFLOAT show_low_bound();
    void   ch_low_bound(TGFLOAT);
    TGFLOAT show_upp_bound();
    void   ch_upp_bound(TGFLOAT);

    int    is_low_bounded();
    void   ch_low_bounded(int);
    int    is_upp_bounded();
    void   ch_upp_bounded(int);

    int    has_treated();
    void   ch_treated(int);
    TGFLOAT show_cross_time();
    void   ch_cross_time(TGFLOAT);


    TGFLOAT show_cr_initial_value();
    void    ch_cr_initial_value(TGFLOAT);

    Reward_Measure *show_rew_measure();
    void            ch_rew_measure( Reward_Measure * );

// PVM functions
#ifdef TG_PARALLEL
    void   send_context();
    void   recv_context();
#endif
    void              add_rew_sum(Reward_Sum *);
    Reward_Sum_List  *show_rew_sum_list(int);

} Rate_Reward_Sum;
//------------------------------------------------------------------------------
typedef class Rate_Reward_Sum_List : public Chained_List
{
 public:
    Rate_Reward_Sum_List();
    Rate_Reward_Sum_List(Rate_Reward_Sum_List *);
    ~Rate_Reward_Sum_List();

    int              add_tail_rew(Rate_Reward_Sum *);
    Rate_Reward_Sum *show_1st_rew();
    Rate_Reward_Sum *get_1st_rew();
    Rate_Reward_Sum *show_next_rew();
    Rate_Reward_Sum *show_curr_rew();
    int              query_rew(int);
    int              query_rew(char *);
    int              query_rew(char *,char *);
    int              show_no_rew();

} Rate_Reward_Sum_List;
//------------------------------------------------------------------------------

#endif /* __SYSTEM_DESC_H__ */

