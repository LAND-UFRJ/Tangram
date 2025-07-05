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
* @file simulator.h
* @brief <b> Simulator related declarations and typedefs. </b>.
* @remarks Lot of.
* @author LAND/UFRJ
* @date 1999-2009
* @warning Do not modify this class before knowing the whole Tangram II project
* @since version 1.0
*
* This file contains the declarations for simul_control.cpp, simulator.cpp,
* clone_ev.cpp, simul_interactive.cpp, simul_event.cpp, simul_run.cpp.
*
*/

#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

/**  Definitions for interactive simulation. */
/*@{*/
#define PORT               "6743"

#define DO_NOT_OPTIMIZE_REW_UPDATE 0

#define PRINT_REWARDS_CR_IR   3
#define PRINT_REWARD_IR       1
#define PRINT_REWARD_CR       2
/*@}*/

/** used in the REWARD_REACHED event */
/*@{*/
#define  NONE            0
#define  GET_IR_TYPE     1
#define  GET_CR_TYPE     2
#define  GET_CR_SUM_TYPE 3
#define  UPP_REACH_TYPE  4
#define  LOW_REACH_TYPE  5
/*@}*/

/* reward_measure types */
/*@{*/
#define  NOT_SPECIFIED_TYPE         0
#define  RATE_REWARD_TYPE           1
#define  IMPULSE_REWARD_TYPE        2
#define  GLOBAL_RATE_REWARD_TYPE    3
#define  GLOBAL_IMPULSE_REWARD_TYPE 4
#define  RATE_REWARD_SUM_TYPE       5
/*@}*/

/* possible finishing points of the simulator */
/*@{*/
#define  MAX_TIME    1
#define  MAX_TRANS   2
#define  STOP_EVENT  3
#define  LOWER_ST    4
#define  UPPER_ST    5
/*@}*/

/* indicator of the update_cr method */
/*@{*/
#define  NO_CHECK        0
#define  CHECK_CR_TOUCH  1
#define  CHECK_IR_CHANGE 2
/*@}*/


#define SP_OPT_HAS_VALUE      (1<<0)
#define SP_OPT_HAS_FORMAT     (1<<1)
#define SP_OPT_IN_ARRAY       (1<<2)
#define SP_OPT_FIRST_IN_ARRAY (1<<3)
#define SP_OPT_LAST_IN_ARRAY  (1<<4)

#define SI_OPT_IS_ST_VAR  (1<<0)
#define SI_OPT_IS_REWARD  (1<<1)
#define SI_OPT_IS_INTEGER (1<<2)
#define SI_OPT_IS_FLOAT   (1<<3)
#define SI_OPT_IS_ARRAY   (1<<4)

#define CT_CODE_INIT_SIMULATION   0
#define CT_CODE_END_OF_SIMULATION 1
#define CT_CODE_STEP_SIMULATION   2

/* Bounding defines */
/*@{*/
#define MAXOBJECTNAME           120
#define MAXSYMBOLNAME           120
#define MAXVALUESIZE            240
#define MAXFORMATSIZE           24
#define MAXEVENTNAME            512
/*@}*/

/** Control Packet. Track information to TGIF. */
typedef struct tagT_ControlPacket {
  int code;

  /** info to update TGIF's screen */
  int step;
  double elapsed_time;

  /** number of StVar packets following */
  int symcount;

  /** name of latest event executed by the simulator */
  char last_event[MAXEVENTNAME];
} T_ControlPacket;

/** State Variable Packet */
typedef struct tagT_SymbolPacket {
  char object_name[MAXOBJECTNAME];
  char symbol_name[MAXSYMBOLNAME];
  int  options;                    /**< available options:SP_OPT_HAS_VALUE
                                                          SP_OPT_HAS_FORMAT
                                                          SP_OPT_IN_ARRAY
                                                          SP_OPT_FIRST_IN_ARRAY
                                                          SP_OPT_LAST_IN_ARRAY*/
  int  array_index;                /**< if SP_OPT_IN_ARRAY this is the index */
  char value_string[MAXVALUESIZE]; /**< if SP_OPT_HAS_VALUE this is the value */
  char value_format[MAXFORMATSIZE];
} T_SymbolPacket;

/** tagSymbolInfo struct. It stores the object's symbols information */
typedef struct tagSymbolInfo {
  char object_name[MAXOBJECTNAME];
  char symbol_name[MAXSYMBOLNAME];
  int options;                      /**< available options: SI_OPT_IS_ST_VAR
                                                            SI_OPT_IS_REWARD
                                                            SI_OPT_IS_INTEGER
                                                            SI_OPT_IS_FLOAT
                                                            SI_OPT_IS_ARRAY */
  int length;                       /**< length of possible array */
  Object_Description * obj_desc;
  Object_State       * obj_st;
  Symbol             * symbol;
  int                  code;
} SymbolInfo;


/** Simulator_Interactive class */
typedef class Simulator_Interactive {

    Simulator *simulator;

    /** The packet_queue stores packets that will be sent to TGIF.
       It is managed by the addSymToBuffer() and sendAllSymPackets() functions
       On the other hand received_packets is meant to store packets
       received from TGIF and that must be analysed before being inserted
       into packet_queue */
    T_SymbolPacket * packet_queue,
                   * received_packets;
    int              packet_queue_size,
                     packet_queue_max_size,
                     number_of_received_packets;

    int connect_to_interface(const char *);

    int sendSymbolPacket( int, T_SymbolPacket * );
    int recvSymbolPacket( int, T_SymbolPacket * );
    int sendControlPacket( int, T_ControlPacket * );
    int recvControlPacket( int, T_ControlPacket * );

    void addSymToBuffer( T_SymbolPacket * );
    void sendAllSymPackets( int );

    int get_symbol_info ( char *, char *, SymbolInfo *, System_State * );
    int get_symbol_value( const SymbolInfo *, T_SymbolPacket * );
    int set_symbol_value( const SymbolInfo *, T_SymbolPacket * );

    void dismount_packet( int, T_ControlPacket *, System_State * );
    void mount_packet( System_State * );

 public:
    Simulator_Interactive(Simulator *);
    ~Simulator_Interactive();

    void simulate_model_interactive();

    Simulator *show_simulator();
} Simulator_Interactive;


/** direct access from object state ID to event list */
typedef struct {
    Simulator_Event *simul_ev;
    int              no_events;
} t_obj_ev;

/** Simulator class */
typedef class Simulator {

    TGFLOAT simulation_time;
    TGFLOAT last_simul_time;
    int     transitions;
    int     vanishing_trans;
    int     reward_to_print;

    System_State     *curr_sys_st;          /**< current state of simulation */

    Simulator_Event  *stopping_event;       /**< pointer to the stopping evnt */
    TGFLOAT max_simul_time;                 /**< max time for simulation */
    int     max_simul_trans;                /**< max transitions (or events) for
                                                 simulation */
    int     stopping_state;                 /**< defines a stopping state for
                                                 the simulation */
    int     stop_obj_id, stop_st_var_id;    /**< codes of the obj and st_var for
                                                 stopping the simulation */
    State_Variable  *stop_upp_st_var,       /**< stopping st. */
                    *stop_low_st_var;       /**< stopping st. */
    int     finish_point;

    Simulator_Event_List *event_list;       /**< the event list */
    t_obj_ev      *obj_ev_tb;               /**< shortcut to find the event of
                                                 an object state */

    Clone_Event_List *clone_ev_list;        /**< list of clonned events after an
                                                 action */

    Reward_Measure_List  *rate_rew_list;    /**< rate reward measure list */
    Reward_Measure_List  *imp_rew_list;     /**< impulse reward measure list */
    Rate_Reward_Sum_List *rate_rew_sum_list;/**< rate reward sum list */

    Random_obj * action_random_obj,         /**< used to choose probabilistic
                                                 actions */
               * gr_function_random_obj;    /**< used to generate numbers for
                                                 get_random() */

    int        rr_ev_flag;                  /**< indicates the existence of the
                                                 REWARD_REACHED events */
    int        run_number;                  /**< indicates the execution run
                                                 number */

 public:
    friend class Simulator_Interactive;

    Simulator(TGFLOAT, int, int, Random_obj *, Random_obj *);
    /** The constructor. */
    Simulator(Simulator *);
    /** The destructor. */
    ~Simulator();

    /* methods */
    TGFLOAT show_simul_time();
    TGFLOAT show_last_simul_time();
    int    show_transitions();
    int    show_vanishing_trans();
    System_State *show_curr_sys_st();
    Simulator_Event *show_stopping_event();
    TGFLOAT show_max_simul_time();
    int    show_max_simul_trans();
    int    show_stopping_state();
    int    show_stop_obj_id();
    int    show_stop_st_var_id();

    State_Variable        *show_low_st_var();
    State_Variable        *show_upp_st_var();
    Simulator_Event_List  *show_simul_ev_list(int);
    t_obj_ev              *show_obj_ev_tb();
    Clone_Event_List      *show_clone_ev_list(int);
    Reward_Measure_List   *show_rate_rew_list(int);
    Reward_Measure_List   *show_imp_rew_list(int);
    Rate_Reward_Sum_List  *show_rate_rew_sum_list(int);

    TGFLOAT get_random();

    void create_event_list();
    void initialize_event_list();
    void reset_event_list(int);
    void process_event_list(Object_State *);

    void create_rate_rew_list();
    void update_rate_rewards(Simulator_Event *);
    void add_rate_rew_list(Rate_Reward_List *,Simulator_Event *);
    void calculate_virtual_ir( Rate_Reward_Sum *, TGFLOAT, TGFLOAT, TGFLOAT,
                               TGFLOAT, TGFLOAT );
    void update_cr( Reward_Sum *, int, TGFLOAT, TGFLOAT, TGFLOAT,
                    Simulator_Event *, int );
    void check_cr_touch( Reward_Measure *, Simulator_Event *, TGFLOAT,TGFLOAT );

    void create_rate_rew_sum_list();

    void create_imp_rew_list();
    void create_imp_rew_entries(Action_List *);
    void add_imp_rew_list(Transition *);

    void reward_reset(int);
    void reset();
    void reset(int);
    void clear_reward_values(int);
    void set_stopping_event(char *);
    void define_stopping_state(int, int);
    void set_stopping_state(State_Variable *, State_Variable *);
    int  end_of_simulation();
    int  show_finish_point();

    void simulate_model_batch();
    System_State *simulate_to_a_leaf(Simulator_Event *);

    void add_clone(const char *);
    void clone_events(Object_State *obj_st);

    void set_cumulative_reward(Object_State *, const char *, TGFLOAT);
    void set_instantaneous_reward(Object_State *, const char *, TGFLOAT);
    void unset_instantaneous_reward(Object_State *, const char *);

    void set_need_eval_for_rr_ev( Reward_Measure * );

    TGFLOAT get_cumulative_reward(Object_State *, const char *);
    TGFLOAT get_instantaneous_reward(Object_State *, const char *);
    TGFLOAT get_cumulative_reward_sum(Object_State *, const char *);

    int  will_reach_reward( Rew_Reach_Distrib *, RR_Touch * );
    void eval_rr_special_args( Rew_Reach_Distrib * );
    int  special_rr_event( Object_State *, int type, const char *, int, TGFLOAT,
                           const char * );

    int show_rr_ev_flag();
    void ch_rr_ev_flag(int);

    int show_run_number();
    void ch_run_number(int);

    Action *choose_an_action(Object_State *, Action_List *);

    System_State *process_action(System_State *, Object_State *, Action *);
    System_State *process_pend_msg(System_State *, Pending_Message *);

    void print_trans_time();
    void print_rewards(FILE *);
    void print_curr_st();
    void print_ev_list();
    void print_no_triggers(FILE *);

    void generate_output_files();

} Simulator;


/** Simulator_Event class. Each declared event will be a Simulator_Event
    instance */
typedef class Simulator_Event : public Chained_Element {

    Event_Sample_List   *ev_sample_list;
    int                  enable;
    int                  no_triggers;

    Event               *event;
    Object_Description  *obj_desc;

 public:
    Simulator_Event(Event *, Object_Description *);
    Simulator_Event(Simulator_Event *);
    ~Simulator_Event();

    Event_Sample_List   *show_ev_sample_list(int);
    Event_Sample        *show_1st_ev_sample();
    Event_Sample        *get_1st_ev_sample();
    void                clear_ev_sample_list();
    int                 show_no_samples();

    int                 show_enable();
    void                ch_enable(int);

    Event              *show_event();
    Object_Description *show_obj_desc();

    int                 eval_cond(Object_State *);

    void                generate_sample(Object_State *, TGFLOAT);

    int                 show_distrib_type();

    void                clear_no_triggers();
    void                inc_no_triggers();
    int                 show_no_triggers();

    void                print_simul_ev();
    void                print_ev_sample_list();

} Simulator_Event;


/** Simulator_Event_List class. Chained_List for Simulator_Event elements. */
typedef class Simulator_Event_List : public Chained_List {

 public:
    Simulator_Event_List();
    Simulator_Event_List(Simulator_Event_List *);
    ~Simulator_Event_List();

    int add_tail_event(Simulator_Event *);
    Simulator_Event *show_1st_event();
    Simulator_Event *get_1st_event();
    Simulator_Event *show_next_event();
    Simulator_Event *show_next_event(Simulator_Event *);
    Simulator_Event *show_curr_event();
    Simulator_Event *show_smallest_time();
    int              show_no_event();
} Simulator_Event_List;


/** Event_Sample class. Event_Sample of a given Simulator_Event. */
typedef class Event_Sample : public Chained_Element {

    TGFLOAT time;

 public:
    Event_Sample(TGFLOAT);
    Event_Sample(Event_Sample *);
    ~Event_Sample();

    TGFLOAT show_time();
    void   ch_time(TGFLOAT);
    void   print_sample();

} Event_Sample;

/** Event_Sample_List class. Event_Sample_List of a given Simulator_Event. */
typedef class Event_Sample_List : public Chained_List {

 public:
    Event_Sample_List();
    Event_Sample_List(Event_Sample_List *);
    ~Event_Sample_List();

    int add_order_sample(Event_Sample *);
    Event_Sample *show_1st_sample();
    Event_Sample *get_1st_sample();
    Event_Sample *show_next_sample();
    Event_Sample *show_curr_sample();
    int           show_no_sample();

} Event_Sample_List;


/** Clone_Event class. Each cloned event will be a instance of Clone_Event. */
typedef class Clone_Event : public Chained_Element {

    char event_name[MAXSTRING];
    int  no_clones;

 public:
    Clone_Event(char *);
    Clone_Event(Clone_Event *);
    ~Clone_Event();

    void show_name(char *);
    void ch_name(char *);

    int  show_no_clones();
    void ch_no_clones(int);
    void inc_no_clones();

    void print_clone();

} Clone_Event;


/** Clone_Event_List class. List of clonned events after an action */
typedef class Clone_Event_List : public Chained_List {

 public:
    Clone_Event_List();
    Clone_Event_List(Clone_Event_List *);
    ~Clone_Event_List();

    int add_order_clone(char *);
    int add_tail_clone(Clone_Event *);
    Clone_Event *show_1st_clone();
    Clone_Event *get_1st_clone();
    Clone_Event *show_next_clone();
    Clone_Event *show_curr_clone();
    int          query_clone(char *);
    int          show_no_clone();

} Clone_Event_List;

/** Reward_Measure class. Each reward declared will be a instance of this
    class */
typedef class Reward_Measure : public Chained_Element {

    char     name[MAXSTRING];   /**< name of the reward measure */
    int      type;              /**< type of the reward measure */
    int      id;                /**< identifier of the measure (same as in
                                     system desc) */
    FILE     *fd_cr;            /**< file descriptor of the trace file for CR*/
    FILE     *fd_ir;            /**< file descriptor of the trace file for IR*/
    TGFLOAT  time;              /**< last instant of time the measure was
                                     modified */
                                /**< used only in impulse reward computation */
    TGFLOAT  curr_ir_val;       /**< current value of the instantaneous reward*/
    TGFLOAT  curr_cr_val;       /**< current value of the accumulated reward */
    TGFLOAT  avg_cr_val;        /**< current value of the area of the
                                     accumulated reward */
    int      has_level;         /**< indicates the specification of the level */
    TGFLOAT  rew_level;         /**< a reward level */
    TGFLOAT  time_abv_lv;       /**< time above the reward level */
    int      low_bounded,
             upp_bounded;       /**< flags for bounded measures */
    TGFLOAT low_bound,
            upp_bound;          /**< value of the lower and upper bounds */
    int     rate_sum_bounded;   /**< flag for rate_reward_sum */

    Rate_Reward_Sum  *rate_sum_bound;  /**< pointer to the rate_reward_sum that
                                            is the bound */
    RR_Event_List    *RR_events_list;  /**< this list contains objects that
                                            carries pointers to the events that
                                            has this reward present in it's
                                            condition */
    TGFLOAT  lasttime_cr,
             lasttime_ir,
             last_value_cr,      /**< last value of the cumulative reward */
             last_value_ir,      /**< last value of the instantaneous reward */
             first_cr_val,
             first_ir_val;


    void write_cr_to_file( TGFLOAT , TGFLOAT );
    void write_ir_to_file( TGFLOAT , TGFLOAT );
public:
    Reward_Measure(char *, int);
    Reward_Measure(Reward_Measure *);
    ~Reward_Measure();

    void   clear_values(int, TGFLOAT);
    void   show_name (char *);
    int    show_type ();
    void   ch_type ( int );
    int    show_id ();

    int    open_trace_files(char *);
    void   close_trace_files();
    FILE  *show_trace_fd_cr();
    FILE  *show_trace_fd_ir();

    TGFLOAT show_cr_value();
    void   ch_cr_value(TGFLOAT);


    void conditional_printf_cr( int , TGFLOAT , TGFLOAT );
    void conditional_printf_ir( int , TGFLOAT , TGFLOAT );
    void   set_first_ir(TGFLOAT);
    void   set_first_cr(TGFLOAT);
//    void   reset_first_cr();

    TGFLOAT show_ir_value();
    void   ch_ir_value(TGFLOAT);

    TGFLOAT show_avg_cr_value();
    void   ch_avg_cr_value(TGFLOAT);

    TGFLOAT show_time();
    void   ch_time(TGFLOAT);

    int    show_has_level();
    void   ch_has_level(int);

    TGFLOAT show_rew_level();
    void   ch_rew_level(TGFLOAT);

    TGFLOAT show_time_abv_lv();
    void   ch_time_abv_lv(TGFLOAT);

    TGFLOAT show_low_bound();
    void   ch_low_bound(TGFLOAT);
    TGFLOAT show_upp_bound();
    void   ch_upp_bound(TGFLOAT);

    int    is_low_bounded();
    void   ch_low_bounded(int);
    int    is_upp_bounded();
    void   ch_upp_bounded(int);

    Rate_Reward_Sum  *show_rate_sum_bound();
    void              ch_rate_sum_bound( Rate_Reward_Sum * );
    int    is_rate_sum_bounded( );
    void   ch_rate_sum_bounded( int );

    RR_Event_List *show_RR_events_list( int );
    void           create_RR_events_list();
    int            exists_RR_events_list();
    void           add_RR_event(RR_Event *);
    void           clear_RR_events_list();

#ifdef TG_PARALLEL
    void   send_context();
    void   recv_context();
#endif

} Reward_Measure;


/** Reward_Measure_List class. Lists all declared rewards. */
typedef class Reward_Measure_List : public Chained_List {

 public:
    Reward_Measure_List();
    Reward_Measure_List(Reward_Measure_List *);
    ~Reward_Measure_List();

    Reward_Measure_List *duplicate();
    void            divide_by(TGFLOAT);
    int             add_tail_rew(Reward_Measure *);
    Reward_Measure *show_1st_rew();
    Reward_Measure *get_1st_rew();
    Reward_Measure *show_next_rew();
    Reward_Measure *show_curr_rew();
    int             query_rew(char *);
    int             query_rew(int);
    int             show_no_rew();
    void            print_rew(FILE *);

#ifdef TG_PARALLEL
    void            send_context();
    void            recv_context();
#endif
} Reward_Measure_List;


/** This class maintains a pointer to te event that has the Reward_Measure in
    it's condition. */
typedef class RR_Event : public Chained_Element
{
 public:
    RR_Event( Event *, RR_Touch * );
    RR_Event( RR_Event * );
    ~RR_Event();

    Event    *show_event();
    void      ch_event( Event * );
    RR_Touch *show_twin_rr_touch();
    void      ch_twin_rr_touch( RR_Touch * );

 private:
    /** pointer to the event that has the Reward_Measure in it's condition */
    Event    *event;

    /** pointer to the twin RR_touch that represents the argument of the
        condition that has this Reward_Measure in it's clausule. */
    RR_Touch *twin_rr_touch;

} RR_Event;


/** RR_Event_List class. */
typedef class RR_Event_List : public Chained_List
{
 public:
    RR_Event_List();
    RR_Event_List( RR_Event_List *);
    ~RR_Event_List();

    int       add_tail_rr_ev( RR_Event * );
    RR_Event *show_1st_rr_ev();
    RR_Event *get_1st_rr_ev();
    RR_Event *get_curr_rr_ev();
    RR_Event *show_next_rr_ev();
    RR_Event *show_curr_rr_ev();
    //void      ch_need_eval( int );
    RR_Event_List *duplicate();
    int       show_no_rr_ev();
} RR_Event_List;


/** Simulator_Controler class. It controls the simulation behavior. */
typedef class Simulator_Controler
{

    Simulator               *simulator;
    int                     total_runs;
    int                     original_runs;
#ifdef TG_PARALLEL
    int                     last_local_and_remote_run;
#endif
    Simulator_Result_List   *rate_rew_list;
    Simulator_Result_List   *imp_rew_list;
    Simulator_Run_List      *run_list;
    TGFLOAT                 conf_interval;
    int                     rare_event;  /**< indicates if it's a rare event
                                              simulation */
    Simulator_Scheduler     *simul_sched;

    void                    create_result_list();
    void                    calculate_mean();
    void                    calculate_variance();
    void                    calculate_interval();
    Simulator_Run*          add_simul_run(Simulator *, int);
    Simulator_Run*          add_simul_run(Simulator_Scheduler *, int);
#ifdef TG_PARALLEL
    void                    recv_only_run_context(int &);
    void                    recv_others_context(int);
    void                    send_others_context();
#endif

 public:
    Simulator_Controler(int);
    ~Simulator_Controler();

    int  init_simulator(TGFLOAT, int, char*, char *, int, int, int, int,
                        int,Random_obj *, Random_obj *);
    void start_simulator( unsigned short[3], unsigned short[3],
                          unsigned short[3] );

    void print_results( int );
    Simulator *show_simulator();

} Simulator_Controler;

/** Simulator_Run class. Each simulation run will be a instance of this class */
typedef class Simulator_Run : public Chained_Element
{

    int             no_run;
    TGFLOAT          simul_time;
    int             no_transitions;
    Reward_Measure_List *rate_rew_list;
    Reward_Measure_List *imp_rew_list;

 public:
#ifdef TG_PARALLEL
    Simulator_Run(int);
#endif
    Simulator_Run(int, TGFLOAT, int);
    ~Simulator_Run();

    int    show_run();
    TGFLOAT show_simul_time();
    int    show_no_transitions();

    int    ch_rate_rew_list(Reward_Measure_List *);
    Reward_Measure_List *show_rate_rew_list(int);

    int    ch_imp_rew_list(Reward_Measure_List *);
    Reward_Measure_List *show_imp_rew_list(int);

#ifdef TG_PARALLEL
    void  send_context();
    void  recv_context();
#endif
} Simulator_Run;

/** Simulator_Run_List class. List of all simulation runs */
typedef class Simulator_Run_List : public Chained_List
{

 public:
    Simulator_Run_List();
    Simulator_Run_List(Simulator_Run_List *);
    ~Simulator_Run_List();

    int            add_tail_run(Simulator_Run *);
    Simulator_Run *show_1st_run();
    Simulator_Run *get_1st_run();
    Simulator_Run *show_next_run();
    Simulator_Run *show_curr_run();
    int            query_run(int);
    int            show_no_run();

} Simulator_Run_List;


/** Simulator_Result class. It stores simulation results. */
typedef class Simulator_Result : public Chained_Element {

    char   name[MAXSTRING];
    TGFLOAT  cr_mean,  cr_var,  cr_inter;  /**< cummulated reward value */

    TGFLOAT acr_mean, acr_var, acr_inter;  /**< averaged cumulated reward
                                                value */
    TGFLOAT atc_mean, atc_var, atc_inter;  /**< average of the area of the
                                                accumulated reward */
    TGFLOAT tbl_mean, tbl_var, tbl_inter;  /**< time elapsed above a given
                                                reward level */
    int     has_level;                     /**< indicates the definition of a
                                                level */

 public:
    Simulator_Result(char *);
    ~Simulator_Result();

    void   show_name(char *);

    TGFLOAT show_cr_mean();
    void   ch_cr_mean(TGFLOAT);
    TGFLOAT show_cr_var();
    void   ch_cr_var(TGFLOAT);
    TGFLOAT show_cr_inter();
    void   ch_cr_inter(TGFLOAT);

    TGFLOAT show_acr_mean();
    void   ch_acr_mean(TGFLOAT);
    TGFLOAT show_acr_var();
    void   ch_acr_var(TGFLOAT);
    TGFLOAT show_acr_inter();
    void   ch_acr_inter(TGFLOAT);

    TGFLOAT show_atc_mean();
    void   ch_atc_mean(TGFLOAT);
    TGFLOAT show_atc_var();
    void   ch_atc_var(TGFLOAT);
    TGFLOAT show_atc_inter();
    void   ch_atc_inter(TGFLOAT);

    TGFLOAT show_tbl_mean();
    void   ch_tbl_mean(TGFLOAT);
    TGFLOAT show_tbl_var();
    void   ch_tbl_var(TGFLOAT);
    TGFLOAT show_tbl_inter();
    void   ch_tbl_inter(TGFLOAT);

    int    show_has_level();
    void   ch_has_level( int );

    void   print();

#ifdef TG_PARALLEL
    void   send_context();
    void   recv_context();
#endif
} Simulator_Result;


/** Simulator_Result_List class. List of all simulations result */
typedef class Simulator_Result_List : public Chained_List {

 public:
    Simulator_Result_List();
    Simulator_Result_List(Simulator_Result_List *);
    ~Simulator_Result_List();

    int               add_tail_result(Simulator_Result *);
    Simulator_Result *show_1st_result();
    Simulator_Result *get_1st_result();
    Simulator_Result *show_next_result();
    Simulator_Result *show_curr_result();
    int               query_result(char *);
    int               show_no_result();

#ifdef TG_PARALLEL
    void   send_context();
    void   recv_context();
#endif
} Simulator_Result_List;


#endif /* __SIMULATOR_H__ */
