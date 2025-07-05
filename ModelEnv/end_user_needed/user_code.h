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
#ifndef __USER_CODE_H__
#define __USER_CODE_H__

#include "tangram_types.h"

typedef class Chained_Element;
typedef class Chained_List;
typedef class System_State;
typedef class Object_State;
typedef class Pending_Message;
typedef class Simulator;

#ifndef __CHAINED_LIST_H__
#define __CHAINED_LIST_H__

/* access permition defines for lists */
#define  LIST_RO  1
#define  LIST_RW  2

/* used in the REWARD_REACHED event */
#define  NONE            0
#define  GET_IR_TYPE     1
#define  GET_CR_TYPE     2
#define  GET_CR_SUM_TYPE 3
#define  UPP_REACH_TYPE  4
#define  LOW_REACH_TYPE  5

typedef class Chained_Element {

    Chained_Element *prev;
    Chained_Element *next;

 public:
    friend class Chained_List;

    Chained_Element();

 private:
    Chained_Element *show_prev();
    void set_prev(Chained_Element *);

    Chained_Element *show_next();
    void set_next(Chained_Element *);

} Chained_Element;
//------------------------------------------------------------------------------
typedef class Chained_List {

    Chained_Element *first_el, *last_el;
    Chained_Element *curr_el;
    int no_elem;

    int is_first(Chained_Element *);
    int is_last(Chained_Element *);

 protected:
    int access;

 public:
    Chained_List();
    Chained_List(Chained_Element *);
    Chained_List(Chained_List *);

    Chained_Element *show_head();
    Chained_Element *show_tail();
    Chained_Element *show_next();
    Chained_Element *show_next(Chained_Element *);
    Chained_Element *show_prev();
    Chained_Element *show_prev(Chained_Element *);
    Chained_Element *show_curr();

    int show_no_elem();
    int is_empty();

    int add_head(Chained_Element *);
    int add_tail(Chained_Element *);
    int add_prev(Chained_Element *);
    int add_next(Chained_Element *);

    Chained_Element *del_head();
    Chained_Element *del_tail();
    Chained_Element *del_elem();

    int replace_elem(Chained_Element *);

} Chained_List;
//------------------------------------------------------------------------------
#endif /* __CHAINED_LIST_H__ */

#ifndef __SYSTEM_STATE_H__
#define __SYSTEM_STATE_H__
//------------------------------------------------------------------------------
typedef class System_State : public Chained_Element {
public:
   TGFLOAT show_reward_val(const char *);
} System_State;
//------------------------------------------------------------------------------
#endif /* __SYSTEM_STATE_H__ */

//------------------------------------------------------------------------------

#ifndef __QUEUE_TYPE_H__
#define __QUEUE_TYPE_H__

class IntegerQueue : public Chained_List
{
public:
    int show_size();
    void operator =( IntegerQueue & );
    IntegerQueue();
    IntegerQueue( int );
    IntegerQueue( IntegerQueue *queue );
    ~IntegerQueue();
    int save_at_head( int *vector );
    int save_at_tail( int *vector );
    int restore_from_head( int *vector );
    int restore_from_tail( int *vector );
};

class FloatQueue : public Chained_List
{
public:
    int show_size();
    void operator =( FloatQueue & );
    FloatQueue();
    FloatQueue( int );
    FloatQueue( FloatQueue *queue );
    ~FloatQueue();
    int save_at_head( TGFLOAT *vector );
    int save_at_tail( TGFLOAT *vector );
    int restore_from_head( TGFLOAT *vector );
    int restore_from_tail( TGFLOAT *vector );
};

#endif

#ifndef __OBJECT_STATE_H__
#define __OBJECT_STATE_H__
//------------------------------------------------------------------------------
/* This is a simplified version of the Object_State interface from
   system_state.h. Only the necessary methods for model compilation
   are included here. */
typedef class Object_State : public Chained_Element {

public:
    void add_pend_msg(Pending_Message*);
    void add_pend_msg(const char *,const  char *, TGFLOAT, int);
    void add_pend_msg(const char *,const  char *, int, int);
    void add_pend_msg(const char *,const  char *, TGFLOAT *, int);
    void add_pend_msg(const char *,const  char *, int *, int);

    Pending_Message *get_msg_info( int *,const  char *, int *, int * );
    void destroy_msg(Pending_Message *);

    int  show_st_var_int_value(const char *);
    int* show_st_var_vec_value(const char *);
    void ch_st_var_value(const char *, int);
    void ch_st_var_value(const char *, int *);

    TGFLOAT show_st_var_float_value(const char*);
    TGFLOAT *show_st_var_fvec_value(const char*);
    void ch_st_var_value(const char*, TGFLOAT);
    void ch_st_var_value(const char *, TGFLOAT *);

    void ch_st_var_value( const char *, IntegerQueue & );
    void ch_st_var_value( const char *, FloatQueue & );

    void save__at_tail( IntegerQueue &, int * );
    void save__at_tail( FloatQueue &, TGFLOAT * );
    void save__at_head( IntegerQueue &, int * );
    void save__at_head( FloatQueue &, TGFLOAT * );
    void restore__from_tail( IntegerQueue &, int * );
    void restore__from_tail( FloatQueue &, TGFLOAT * );
    void restore__from_head( IntegerQueue &, int * );
    void restore__from_head( FloatQueue &, TGFLOAT * );

    void get_st_var_value(int *, const char *);
    void get_st_var_value(int &, const char *);
    void get_st_var_value(TGFLOAT* , const char *);
    void get_st_var_value(TGFLOAT &, const char *);
    void get_st_var_value( IntegerQueue &, const char * );
    void get_st_var_value( FloatQueue &, const char * );

    void get_msg_data_vec( int *, int, Pending_Message * );
    void get_msg_data_vec( TGFLOAT *, int, Pending_Message * );

    void copy( int *, int, int *, int );
    void copy( TGFLOAT *, int, TGFLOAT *, int );
    void copy( IntegerQueue &, int, IntegerQueue &, int );
    void copy( FloatQueue &,   int, FloatQueue &,   int );
} Object_State;
//------------------------------------------------------------------------------
#endif /* __OBJECT_STATE_H__ */

#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

//------------------------------------------------------------------------------
/* This is a simplified version of the Simulator interface from simulator.h.
   Only the necessary methods for model compilation are included here. */
typedef class Simulator {

public:
    /* methods */
    TGFLOAT show_simul_time();

    TGFLOAT get_random();

    void add_clone(const char *);

    void set_cumulative_reward(Object_State *, const char *, TGFLOAT);
    void set_instantaneous_reward(Object_State *, const char *, TGFLOAT);
    void unset_instantaneous_reward(Object_State *, const char *);

    TGFLOAT get_cumulative_reward(Object_State *, const char *);
    TGFLOAT get_instantaneous_reward(Object_State *, const char *);
    TGFLOAT get_cumulative_reward_sum(Object_State *, const char *);

    int  special_rr_event( Object_State *, int type, const char *, int,
                           TGFLOAT, const char * );
} Simulator;
//------------------------------------------------------------------------------
#endif /* __SIMULATOR_H__ */

#ifndef __GENERAL_H__
#define __GENERAL_H__

/* boolean types */
#define TRUE    (1)
#define FALSE   (0)

/* string defines */
#define MAXSTRING  255
#define MAXNAME    255    

#define   USER_CODE_MAX_TIME  45   /* in seconds */

/* the execution context data structure */
typedef struct {
    char obj_name[MAXSTRING];
    char ev_msg_name[MAXSTRING];
    int  ev_msg_type;
    int  act_num;
    int  user_code;   
    int  act_time_count; /*Controls time spent in a action*/   
} t_context;

#endif   /*  __GENERAL_H__ */

#define MSG_TYPE_SCALAR        0
#define MSG_TYPE_INT_VECTOR    1
#define MSG_TYPE_FLOAT_VECTOR  2

/* macros must be included here at the end because some of them have names
   that cause conflicts with methods declared above */
#include "tangram_macros.h"

#endif  /*  __USER_CODE_H__ */
//------------------------------------------------------------------------------
