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
* @file simul_split.h
* @brief <b> Simulator_Scheduler, Simulator_Level, Simulator_Level_List,
* Simulator_Split, and Simulator_Split_List typedefs. </b>.
* @remarks Lot of.
* @author LAND/UFRJ
* @date 1999-2009
* @warning Do not modify this class before knowing the whole Tangram II project
* @since version 1.0
*
*/

#ifndef __SIMUL_SPLIT_H__
#define __SIMUL_SPLIT_H__


/** possible status of the splitted simulator */
/**@{*/
#define  WAITING    1
#define  RUNNING    2
#define  FINISHED   3
#define  REMOVE     4
/**@}*/

/** maximum running simulators in a level */
#define  MAX_CONCURRANCY 5

typedef class Simulator_Scheduler {
    Simulator             *simulator;     /**< the last finished simulator */
    Simulator_Level_List  *level_list;    /**< threshold list */
    Reward_Measure_List   *rate_rew_list; /**< accumulated rate rew list */
    int                    last_level;    /**< last splitting level */
    double                 mult_splits;   /**< productory of the no splits */

 public:
    Simulator_Scheduler(Simulator *);
    ~Simulator_Scheduler();

    void reset();

    Simulator *show_simulator();
    Reward_Measure_List *show_rate_rew_list(int);

    int  create_level_list();

    int  end_of_simulation();
    int  start_simulation();
    int  wait_for_simulation();

    int  create_rew_list(Simulator *);
    int  add_rew_list(Simulator *);

    void print_sched();

} Simulator_Scheduler;


typedef class Simulator_Level : public Chained_Element {
    int number;      /**< number of this level */
    int no_splits;   /**< number of splits in this level */
    int concurrancy; /**< max running simulators in this level */
    State_Variable *low_st_var, *upp_st_var; /**< lower and upper State Variables of this level */
    int running;  /**< number of threads running */
    int waiting;  /**< number of threads waiting */
    int finished; /**< number of threads finished */

    Simulator_Split_List *split_list;  /**< simulation list */
    
 public:
    Simulator_Level(int, int, int, State_Variable *, State_Variable *);
    ~Simulator_Level();
    
    void clear_values();

    int  show_level();
    int  show_no_splits();
    int  show_running();
    int  show_waiting();
    int  show_finished();

    int              run_simulator(Simulator *, int, int);
    void             sched_simulator();
    Simulator_Split *get_finished_split();

    void print_level();

} Simulator_Level;

typedef class Simulator_Level_List : public Chained_List {

 public:
    Simulator_Level_List();
    Simulator_Level_List(Simulator_Level_List *);
    ~Simulator_Level_List();

    int              add_tail_level(Simulator_Level *);
    Simulator_Level *show_1st_level();
    Simulator_Level *get_1st_level();
    Simulator_Level *show_next_level();
    Simulator_Level *show_curr_level();
    int              query_level(int);
    int              show_no_level();

} Simulator_Level_List;


typedef class Simulator_Split : public Chained_Element {
    int        level;      /**< level where this split is running */
    int        status;     /**< status of current thread */
    int        last;       /**< flag indicating last split */
    int        last_tree;  /**< if last, indicates the anchestor tree */
    pthread_t  thread_id;  /**< thread identifier */
    Simulator *simulator;  /**< the running simulator */
    
 public:
    Simulator_Split(Simulator *, int , int, int);
    ~Simulator_Split();
    
    Simulator *show_simulator();
    int  show_status();
    void ch_status(int);
    pthread_t show_thread_id();
    int  is_last();
    int  show_last_tree();

    int  create_thread();
    int  run_simulator();

    void print_split();

} Simulator_Split;

typedef class Simulator_Split_List : public Chained_List {

 public:
    Simulator_Split_List();
    Simulator_Split_List(Simulator_Split_List *);
    ~Simulator_Split_List();

    int              add_tail_split(Simulator_Split *);
    Simulator_Split *show_1st_split();
    Simulator_Split *get_1st_split();
    Simulator_Split *get_curr_split();
    Simulator_Split *show_next_split();
    Simulator_Split *show_curr_split();
    int              query_split(int);
    int              show_no_split();

} Simulator_Split_List;


#endif /* __SIMUL_SPLIT_H__ */
