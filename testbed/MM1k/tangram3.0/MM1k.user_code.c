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
/* execution context data structure */
extern t_context context;
//------------------------------------------------------------------------------
static char *all = "*";
int CAN_GENERATE_CHAIN = 1;
int CAN_SIMULATE = 1;


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
