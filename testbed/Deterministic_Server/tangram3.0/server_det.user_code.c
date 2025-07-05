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
//  user code for object: On_Off_Source 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_Eve_On_Off_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+03;
	const TGFLOAT OFFON_RATE = 5.000000e+02;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e+06;
	char *PORT_OUT = "send_port";

	return ( (TGFLOAT) ( ONOFF_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_Eve_On_Off_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+03;
	const TGFLOAT OFFON_RATE = 5.000000e+02;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e+06;
	char *PORT_OUT = "send_port";
	char *ev_name_param = "On_Off";

	return ( (TGFLOAT) ( Status )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_Eve_On_Off_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+03;
	const TGFLOAT OFFON_RATE = 5.000000e+02;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e+06;
	char *PORT_OUT = "send_port";

 {  ;set_st("Status",0); };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_Eve_Off_On_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+03;
	const TGFLOAT OFFON_RATE = 5.000000e+02;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e+06;
	char *PORT_OUT = "send_port";

	return ( (TGFLOAT) ( OFFON_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_Eve_Off_On_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+03;
	const TGFLOAT OFFON_RATE = 5.000000e+02;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e+06;
	char *PORT_OUT = "send_port";
	char *ev_name_param = "Off_On";

	return ( (TGFLOAT) ( Status )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_Eve_Off_On_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+03;
	const TGFLOAT OFFON_RATE = 5.000000e+02;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e+06;
	char *PORT_OUT = "send_port";

 { ;set_st("Status", 1); };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_Eve_Packet_Generation_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+03;
	const TGFLOAT OFFON_RATE = 5.000000e+02;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e+06;
	char *PORT_OUT = "send_port";

	return ( (TGFLOAT) ( TRANSMISSION_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_Eve_Packet_Generation_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+03;
	const TGFLOAT OFFON_RATE = 5.000000e+02;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e+06;
	char *PORT_OUT = "send_port";
	char *ev_name_param = "Packet_Generation";

	return ( (TGFLOAT) ( Status )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_Eve_Packet_Generation_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+03;
	const TGFLOAT OFFON_RATE = 5.000000e+02;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e+06;
	char *PORT_OUT = "send_port";

 {msg(PORT_OUT,all,0);};

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
//  user code for object: Server_Queue_Det 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Det_Eve_Service_DET_mean (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const TGFLOAT SERVICE_RATE = 8.000000e+05;
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "send_port";
	char *utilization = "utilization";

	return ( (TGFLOAT) ( SERVICE_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Det_Eve_Service_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const TGFLOAT SERVICE_RATE = 8.000000e+05;
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "send_port";
	char *utilization = "utilization";
	char *ev_name_param = "Service";

	return ( (TGFLOAT) ( Queue )>(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Det_Eve_Service_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const TGFLOAT SERVICE_RATE = 8.000000e+05;
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "send_port";
	char *utilization = "utilization";

 { int queue; 
           queue = Queue - 1; 
           set_st("Queue", queue);
          };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Det_Msg_MSG_1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const TGFLOAT SERVICE_RATE = 8.000000e+05;
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "send_port";
	char *utilization = "utilization";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

 { int queue;
           queue = Queue;
           if (queue < QUEUE_SIZE)
              queue = queue  + 1;
           set_st("Queue", queue);
         };

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Det_Rew_utilization_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const TGFLOAT SERVICE_RATE = 8.000000e+05;
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "send_port";
	char *utilization = "utilization";

	return ( (TGFLOAT) ( Queue )>(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Server_Queue_Det_Rew_utilization_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue = obj_st->show_st_var_int_value("Queue");
	const TGFLOAT SERVICE_RATE = 8.000000e+05;
	const int QUEUE_SIZE = 10;
	char *PORT_IN = "send_port";
	char *utilization = "utilization";

	return ( (TGFLOAT) (1) );
}
//------------------------------------------------------------------------------
TGFLOAT function_handler(int fd, Object_State *obj_st, Simulator *simulator, System_State *sys_st)
{
	TGFLOAT status = -1;

	context.user_code = 1;
	alarm(USER_CODE_MAX_TIME);

	switch (fd) {
		case 1 : status = On_Off_Source_Eve_On_Off_EXP_rate (obj_st); break; 

		case 2 : status = On_Off_Source_Eve_On_Off_cond (obj_st, simulator); break; 

		case 3 : status = On_Off_Source_Eve_On_Off_act_1 (obj_st, simulator); break; 

		case 4 : status = On_Off_Source_Eve_Off_On_EXP_rate (obj_st); break; 

		case 5 : status = On_Off_Source_Eve_Off_On_cond (obj_st, simulator); break; 

		case 6 : status = On_Off_Source_Eve_Off_On_act_1 (obj_st, simulator); break; 

		case 7 : status = On_Off_Source_Eve_Packet_Generation_EXP_rate (obj_st); break; 

		case 8 : status = On_Off_Source_Eve_Packet_Generation_cond (obj_st, simulator); break; 

		case 9 : status = On_Off_Source_Eve_Packet_Generation_act_1 (obj_st, simulator); break; 

		case 10 : status = Server_Queue_Det_Eve_Service_DET_mean (obj_st); break; 

		case 11 : status = Server_Queue_Det_Eve_Service_cond (obj_st, simulator); break; 

		case 12 : status = Server_Queue_Det_Eve_Service_act_1 (obj_st, simulator); break; 

		case 13 : status = Server_Queue_Det_Msg_MSG_1_act_1 (obj_st, simulator); break; 

		case 14 : status = Server_Queue_Det_Rew_utilization_cond_1 (obj_st, simulator); break; 

		case 15 : status = Server_Queue_Det_Rew_utilization_act_1 (obj_st, simulator); break; 

		default : fprintf ( stderr, "ERROR: Invalid function descriptor\n"); status = -1;
	}
	context.user_code = 0;
	alarm(0);

	return(status);
}
//------------------------------------------------------------------------------
