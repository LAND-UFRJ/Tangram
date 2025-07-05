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
//  user code for object: Poisson_Source 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Poisson_Source_Eve_Packet_Generation_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	const TGFLOAT TX = 1.000000e+01;
	char *PORT_OUT = "wire";

	return ( (TGFLOAT) ( TX ) );
}
//------------------------------------------------------------------------------
TGFLOAT Poisson_Source_Eve_Packet_Generation_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	const TGFLOAT TX = 1.000000e+01;
	char *PORT_OUT = "wire";
	char *ev_name_param = "Packet_Generation";

	return ( (TGFLOAT) (TRUE) );
}
//------------------------------------------------------------------------------
TGFLOAT Poisson_Source_Eve_Packet_Generation_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	const TGFLOAT TX = 1.000000e+01;
	char *PORT_OUT = "wire";

 {
           msg(PORT_OUT, all, 0);
         };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
//  user code for object: Server_Queue 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Eve_Packet_Service_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "wire";
	const TGFLOAT SERVICE_RATE = 2.000000e+01;
	char *utilization = "utilization";
	char *q_size = "q_size";
	char *empty = "empty";

	return ( (TGFLOAT) ( SERVICE_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Eve_Packet_Service_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "wire";
	const TGFLOAT SERVICE_RATE = 2.000000e+01;
	char *utilization = "utilization";
	char *q_size = "q_size";
	char *empty = "empty";
	char *ev_name_param = "Packet_Service";

	return ( (TGFLOAT) ( Queue )>(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Eve_Packet_Service_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "wire";
	const TGFLOAT SERVICE_RATE = 2.000000e+01;
	char *utilization = "utilization";
	char *q_size = "q_size";
	char *empty = "empty";

 { int queue; 
           queue = Queue - 1; 
           set_st("Queue", queue);
          };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Msg_MSG_1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "wire";
	const TGFLOAT SERVICE_RATE = 2.000000e+01;
	char *utilization = "utilization";
	char *q_size = "q_size";
	char *empty = "empty";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

 { int queue;
           queue = Queue;
           if (Queue < QUEUE_SIZE)
              queue = Queue  + 1;
           set_st("Queue", queue);
         };

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Rew_utilization_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "wire";
	const TGFLOAT SERVICE_RATE = 2.000000e+01;
	char *utilization = "utilization";
	char *q_size = "q_size";
	char *empty = "empty";

	return ( (TGFLOAT) ( Queue )>(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Rew_utilization_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "wire";
	const TGFLOAT SERVICE_RATE = 2.000000e+01;
	char *utilization = "utilization";
	char *q_size = "q_size";
	char *empty = "empty";

	return ( (TGFLOAT) (1) );
}
//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Rew_q_size_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "wire";
	const TGFLOAT SERVICE_RATE = 2.000000e+01;
	char *utilization = "utilization";
	char *q_size = "q_size";
	char *empty = "empty";

	return ( (TGFLOAT) (TRUE) );
}
//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Rew_q_size_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "wire";
	const TGFLOAT SERVICE_RATE = 2.000000e+01;
	char *utilization = "utilization";
	char *q_size = "q_size";
	char *empty = "empty";

	return ( (TGFLOAT) ( Queue ) );
}
//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Rew_empty_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "wire";
	const TGFLOAT SERVICE_RATE = 2.000000e+01;
	char *utilization = "utilization";
	char *q_size = "q_size";
	char *empty = "empty";

	return ( (TGFLOAT) ( Queue )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Rew_empty_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "wire";
	const TGFLOAT SERVICE_RATE = 2.000000e+01;
	char *utilization = "utilization";
	char *q_size = "q_size";
	char *empty = "empty";

	return ( (TGFLOAT) (1) );
}
//------------------------------------------------------------------------------
TGFLOAT function_handler(int fd, Object_State *obj_st, Simulator *simulator, System_State *sys_st)
{
	TGFLOAT status = -1;

	context.user_code = 1;
	alarm(USER_CODE_MAX_TIME);

	switch (fd) {
		case 1 : status = Poisson_Source_Eve_Packet_Generation_EXP_rate (obj_st); break; 

		case 2 : status = Poisson_Source_Eve_Packet_Generation_cond (obj_st, simulator); break; 

		case 3 : status = Poisson_Source_Eve_Packet_Generation_act_1 (obj_st, simulator); break; 

		case 4 : status = Server_Queue_Eve_Packet_Service_EXP_rate (obj_st); break; 

		case 5 : status = Server_Queue_Eve_Packet_Service_cond (obj_st, simulator); break; 

		case 6 : status = Server_Queue_Eve_Packet_Service_act_1 (obj_st, simulator); break; 

		case 7 : status = Server_Queue_Msg_MSG_1_act_1 (obj_st, simulator); break; 

		case 8 : status = Server_Queue_Rew_utilization_cond_1 (obj_st, simulator); break; 

		case 9 : status = Server_Queue_Rew_utilization_act_1 (obj_st, simulator); break; 

		case 10 : status = Server_Queue_Rew_q_size_cond_1 (obj_st, simulator); break; 

		case 11 : status = Server_Queue_Rew_q_size_act_1 (obj_st, simulator); break; 

		case 12 : status = Server_Queue_Rew_empty_cond_1 (obj_st, simulator); break; 

		case 13 : status = Server_Queue_Rew_empty_act_1 (obj_st, simulator); break; 

		default : fprintf ( stderr, "ERROR: Invalid function descriptor\n"); status = -1;
	}
	context.user_code = 0;
	alarm(0);

	return(status);
}
//------------------------------------------------------------------------------
