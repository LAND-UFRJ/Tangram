//------------------------------------------------------------------------------
//        T A N G R A M - I I      << . >>      L A N D - U F R J - B R A Z I L
//------------------------------------------------------------------------------
//            This file is generated automatically by the Tangram tool. 
//        It contains the user code for actions, messages and expressions.
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <user_code.h>

//------------------------------------------------------------------------------
/* macro for messages */
#define  msg(a,b,c)    obj_st->add_pend_msg(a, b, c, sizeof(c))

/* macro for coping vectors */
#define  copy(a,b)   obj_st->copy(a,sizeof(a),b,sizeof(b))

#define  get_msg_data(a)    obj_st->get_msg_data_vec(a, sizeof(a), pend_msg )

/* macro for changing state var values */
#define  set_st(a,b)   obj_st->ch_st_var_value(a, b)

/* macros for saving/restore the queue state */
#define  save_at_head(a,b)   obj_st->save__at_head(a,b)

#define  save_at_tail(a,b)   obj_st->save__at_tail(a,b)

#define  restore_from_head(a,b)   obj_st->restore__from_head(a,b)

#define  restore_from_tail(a,b)   obj_st->restore__from_tail(a,b)

/* macro for getting state var values */
#define  get_st(a,b)   obj_st->get_st_var_value(a, b)

/* macro for delivering cloned eventd */
#define  clone_ev(a)   if (simulator != NULL) simulator->add_clone(a);\
                       else fprintf ( stderr, "ERROR: 'clone' can only be used in simulation\n"); 

/* macro for setting the cumulative reward value */
#define  set_cr(a,b)   if (simulator != NULL) simulator->set_cumulative_reward(obj_st,a,b);\
                       else fprintf ( stderr, "ERROR: using set_cr with no simulation\n");

/* macro for setting the instantaneous reward value */
#define  set_ir(a,b)   if (simulator != NULL) simulator->set_instantaneous_reward(obj_st,a,b);\
                       else fprintf ( stderr, "ERROR: using set_ir with no simulation\n");

/* macro for unsetting the instantaneous reward value */
#define  unset_ir(a)   if (simulator != NULL) simulator->unset_instantaneous_reward(obj_st,a);\
                       else fprintf ( stderr, "ERROR: using unset_ir with no simulation\n");

/* macro for getting the cumulative reward value */
#define  get_cr(a)   ( (simulator != NULL) ? simulator->get_cumulative_reward(obj_st,a) : 0)

/* macro for getting the instantaneous reward value */
#define  get_ir(a)   ( (simulator != NULL) ? simulator->get_instantaneous_reward(obj_st,a) : 0)

/* macro for getting the cumulative reward sum value */
#define  get_cr_sum(a)   ( (simulator != NULL) ? simulator->get_cumulative_reward_sum(obj_st,a) : 0)

/* macro for comparing objects destinations */
#define  objcmp(a,b)   ( (strcmp(a,b) == 0) ? 1 : 0)

/* macro for special_rr_ev boolean function */
#define  special_rr_ev(a,b,c,d)  ( (simulator != NULL) ? simulator->special_rr_event(obj_st,a,b,c,d,ev_name_param) : 0)

/* execution context data structure */
extern t_context context;
//------------------------------------------------------------------------------
static char *all = "*";


//------------------------------------------------------------------------------
//  user code for object: Dispatcher_CPUS 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Service1_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

	return ( (TGFLOAT) ( SERVICE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Service1_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";
	char *ev_name_param = "Service1";

	return ( (TGFLOAT) ((( Queue [(0)])>(0))&&(( UP )==(0))) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Service1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

 { 
             int queue[2]; 
	     queue[1]=Queue[1];
             queue[0] = Queue[0] - 1;
	     msg(ADD_PORT,all,0); 
             set_st( "Queue[]", queue);
         };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Service2_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

	return ( (TGFLOAT) ( SERVICE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Service2_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";
	char *ev_name_param = "Service2";

	return ( (TGFLOAT) ((( Queue [(1)])>(0))&&(( UP2 )==(0))) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Service2_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

 { 
             int queue[2]; 
	     queue[0]=Queue[0];
             queue[1] = Queue[1] - 1;
	     msg(ADD_PORT,all,0); 
             set_st( "Queue[]", queue );
         };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Fail_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

	return ( (TGFLOAT) ( FAIL ) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Fail_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";
	char *ev_name_param = "Fail";

	return ( (TGFLOAT) ((( UP )==(0))) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Fail_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

 { 
             int queue[2];
	         queue[0] = Queue[0];
             queue[1] = Queue[1];
	      if (UP2 == 0)
          {
             queue[1] = queue[1] + queue[0];
	         queue[0] = 0;
           }
             set_st("UP", 1);
             set_st("Queue[]",queue);
         };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Repair_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

	return ( (TGFLOAT) ( REPAIR ) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Repair_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";
	char *ev_name_param = "Repair";

	return ( (TGFLOAT) ( UP )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Repair_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

{
             ;set_st("UP", 0);};

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Fail2_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

	return ( (TGFLOAT) ( FAIL ) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Fail2_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";
	char *ev_name_param = "Fail2";

	return ( (TGFLOAT) ((( UP2 )==(0))) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Fail2_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

 { 
         int queue[2];
	     queue[0] = Queue[0];
         queue[1] = Queue[1];
	     if (UP == 0)
           { 
              queue[0] = queue[0] + queue[1];
	          queue[1] = 0;
           }
             set_st("UP2", 1);
             set_st("Queue[]",queue);
         };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Repair2_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

	return ( (TGFLOAT) ( REPAIR ) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Repair2_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";
	char *ev_name_param = "Repair2";

	return ( (TGFLOAT) ( UP2 )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Eve_Repair2_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

{
             ;set_st("UP2", 0);};

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Msg_MSG_1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

 { 
             int queue[2],aux1,aux2;
             queue[0] = Queue[0];
	     queue[1] = Queue[1];
	     aux1 = queue[0];
	     aux2 = queue[1];
             if (((aux1 + aux2 )< QUEUE_SIZE))
               {  if ((aux1 == aux2))
	           
		    queue[0] = queue[0]+1;
	
		 if ((aux1<aux2))
		     queue[0]= queue[0]+1;
		    
		 if (aux1>aux2)
		     queue[1] = queue[1]+1;}
	     set_st("Queue[]",queue);
	  }

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Msg_MSG_1_prob_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

	return ( (TGFLOAT) (0.5) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Msg_MSG_1_act_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

   	            
	 { 
             int queue[2],aux1,aux2;
             queue[0] = Queue[0];
	     queue[1] = Queue[1];
	     aux1 = queue[0];
	     aux2 = queue[1];
             if (((aux1 + aux2) < QUEUE_SIZE))
              {   if (aux1 == aux2)
		    queue[1] = queue[1]+1;
		 if ((aux1<aux2))
		   
		     queue[0]= queue[0]+1;
		  
		 if (aux1>aux2)
		     queue[1] = queue[1]+1;}
	     set_st("Queue[]",queue);
	  }

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Msg_MSG_1_prob_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

	return ( (TGFLOAT) (0.5) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Rew_op1_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

	return ( (TGFLOAT) ( UP )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Rew_op1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

	return ( (TGFLOAT) (1) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Rew_op2_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

	return ( (TGFLOAT) ( UP2 )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Rew_op2_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

	return ( (TGFLOAT) (1) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Rew_reliabiliy_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

	return ( (TGFLOAT) (((( UP2 )==(0))))||((( UP )==(0))) );
}
//------------------------------------------------------------------------------
TGFLOAT Dispatcher_CPUS_Rew_reliabiliy_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Queue = obj_st->show_st_var_vec_value("Queue");
	int UP = obj_st->show_st_var_int_value("UP");
	int UP2 = obj_st->show_st_var_int_value("UP2");
	const int QUEUE_SIZE = 50;
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const TGFLOAT SERVICE = 3.000000e+01;
	const TGFLOAT FAIL = 4.000000e-05;
	const TGFLOAT REPAIR = 1.000000e+01;
	char *op1 = "op1";
	char *op2 = "op2";
	char *reliabiliy = "reliabiliy";

	return ( (TGFLOAT) (1) );
}
//------------------------------------------------------------------------------
//  user code for object: Delayer 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Delayer_Eve_Delay_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Total_Task = obj_st->show_st_var_int_value("Total_Task");
	int Aux = obj_st->show_st_var_int_value("Aux");
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const int MAX = 50;
	const TGFLOAT RATE = 1.000000e+01;

	return ( (TGFLOAT) ( Total_Task * RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Delayer_Eve_Delay_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Total_Task = obj_st->show_st_var_int_value("Total_Task");
	int Aux = obj_st->show_st_var_int_value("Aux");
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const int MAX = 50;
	const TGFLOAT RATE = 1.000000e+01;
	char *ev_name_param = "Delay";

	return ( (TGFLOAT) ( Total_Task )>(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Delayer_Eve_Delay_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Total_Task = obj_st->show_st_var_int_value("Total_Task");
	int Aux = obj_st->show_st_var_int_value("Aux");
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const int MAX = 50;
	const TGFLOAT RATE = 1.000000e+01;

 {
           int total_task;
           int aux;
           aux = Aux;
         
           total_task = Total_Task;
            total_task = total_task - 1;
               aux = MAX - total_task;    
        msg(TASK_PORT, all, 0);
	    set_st("Total_Task", total_task);
    set_st("Aux", aux);
             
         };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Delayer_Msg_MSG_1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Total_Task = obj_st->show_st_var_int_value("Total_Task");
	int Aux = obj_st->show_st_var_int_value("Aux");
	char *TASK_PORT = "task_port";
	char *ADD_PORT = "add_port";
	const int MAX = 50;
	const TGFLOAT RATE = 1.000000e+01;
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

{int total_task;
         int aux;
         aux = Aux;
        total_task = Total_Task;
         if (Total_Task < MAX) 
                {   
                 total_task = total_task + 1;
                 aux = aux - 1;
                 }
          set_st("Aux", aux);         
         set_st("Total_Task", total_task);
};

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT function_handler(int fd, Object_State *obj_st, Simulator *simulator, System_State *sys_st)
{
	TGFLOAT status = -1;

	context.user_code = 1;
	alarm(USER_CODE_MAX_TIME);

	switch (fd) {
		case 1 : status = Dispatcher_CPUS_Eve_Service1_EXP_rate (obj_st); break; 

		case 2 : status = Dispatcher_CPUS_Eve_Service1_cond (obj_st, simulator); break; 

		case 3 : status = Dispatcher_CPUS_Eve_Service1_act_1 (obj_st, simulator); break; 

		case 4 : status = Dispatcher_CPUS_Eve_Service2_EXP_rate (obj_st); break; 

		case 5 : status = Dispatcher_CPUS_Eve_Service2_cond (obj_st, simulator); break; 

		case 6 : status = Dispatcher_CPUS_Eve_Service2_act_1 (obj_st, simulator); break; 

		case 7 : status = Dispatcher_CPUS_Eve_Fail_EXP_rate (obj_st); break; 

		case 8 : status = Dispatcher_CPUS_Eve_Fail_cond (obj_st, simulator); break; 

		case 9 : status = Dispatcher_CPUS_Eve_Fail_act_1 (obj_st, simulator); break; 

		case 10 : status = Dispatcher_CPUS_Eve_Repair_EXP_rate (obj_st); break; 

		case 11 : status = Dispatcher_CPUS_Eve_Repair_cond (obj_st, simulator); break; 

		case 12 : status = Dispatcher_CPUS_Eve_Repair_act_1 (obj_st, simulator); break; 

		case 13 : status = Dispatcher_CPUS_Eve_Fail2_EXP_rate (obj_st); break; 

		case 14 : status = Dispatcher_CPUS_Eve_Fail2_cond (obj_st, simulator); break; 

		case 15 : status = Dispatcher_CPUS_Eve_Fail2_act_1 (obj_st, simulator); break; 

		case 16 : status = Dispatcher_CPUS_Eve_Repair2_EXP_rate (obj_st); break; 

		case 17 : status = Dispatcher_CPUS_Eve_Repair2_cond (obj_st, simulator); break; 

		case 18 : status = Dispatcher_CPUS_Eve_Repair2_act_1 (obj_st, simulator); break; 

		case 19 : status = Dispatcher_CPUS_Msg_MSG_1_act_1 (obj_st, simulator); break; 

		case 20 : status = Dispatcher_CPUS_Msg_MSG_1_prob_1 (obj_st, simulator); break; 

		case 21 : status = Dispatcher_CPUS_Msg_MSG_1_act_2 (obj_st, simulator); break; 

		case 22 : status = Dispatcher_CPUS_Msg_MSG_1_prob_2 (obj_st, simulator); break; 

		case 23 : status = Dispatcher_CPUS_Rew_op1_cond_1 (obj_st, simulator); break; 

		case 24 : status = Dispatcher_CPUS_Rew_op1_act_1 (obj_st, simulator); break; 

		case 25 : status = Dispatcher_CPUS_Rew_op2_cond_1 (obj_st, simulator); break; 

		case 26 : status = Dispatcher_CPUS_Rew_op2_act_1 (obj_st, simulator); break; 

		case 27 : status = Dispatcher_CPUS_Rew_reliabiliy_cond_1 (obj_st, simulator); break; 

		case 28 : status = Dispatcher_CPUS_Rew_reliabiliy_act_1 (obj_st, simulator); break; 

		case 29 : status = Delayer_Eve_Delay_EXP_rate (obj_st); break; 

		case 30 : status = Delayer_Eve_Delay_cond (obj_st, simulator); break; 

		case 31 : status = Delayer_Eve_Delay_act_1 (obj_st, simulator); break; 

		case 32 : status = Delayer_Msg_MSG_1_act_1 (obj_st, simulator); break; 

		default : fprintf ( stderr, "ERROR: Invalid function descriptor\n"); status = -1;
	}
	context.user_code = 0;
	alarm(0);

	return(status);
}
//------------------------------------------------------------------------------
