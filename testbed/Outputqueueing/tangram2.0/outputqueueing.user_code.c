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
//  user code for object: On_Off_Source_1 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_1_Eve_On_Off_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT1 = "connection_1";

	return ( (TGFLOAT) ( ONOFF_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_1_Eve_On_Off_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT1 = "connection_1";
	char *ev_name_param = "On_Off";

	return ( (TGFLOAT) ( Status )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_1_Eve_On_Off_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT1 = "connection_1";

 {  ;set_st("Status",0); };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_1_Eve_Off_On_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT1 = "connection_1";

	return ( (TGFLOAT) ( OFFON_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_1_Eve_Off_On_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT1 = "connection_1";
	char *ev_name_param = "Off_On";

	return ( (TGFLOAT) ( Status )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_1_Eve_Off_On_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT1 = "connection_1";

 { ;set_st("Status", 1); };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_1_Eve_Packet_Generation_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT1 = "connection_1";

	return ( (TGFLOAT) ( TRANSMISSION_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_1_Eve_Packet_Generation_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT1 = "connection_1";
	char *ev_name_param = "Packet_Generation";

	return ( (TGFLOAT) ( Status )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_1_Eve_Packet_Generation_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT1 = "connection_1";

 {msg(PORT_OUT1,all,0);};

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
//  user code for object: On_Off_Source_2 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_2_Eve_On_Off_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT2 = "connection_2";

	return ( (TGFLOAT) ( ONOFF_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_2_Eve_On_Off_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT2 = "connection_2";
	char *ev_name_param = "On_Off";

	return ( (TGFLOAT) ( Status )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_2_Eve_On_Off_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT2 = "connection_2";

 {  ;set_st("Status",0); };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_2_Eve_Off_On_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT2 = "connection_2";

	return ( (TGFLOAT) ( OFFON_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_2_Eve_Off_On_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT2 = "connection_2";
	char *ev_name_param = "Off_On";

	return ( (TGFLOAT) ( Status )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_2_Eve_Off_On_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT2 = "connection_2";

 { ;set_st("Status", 1); };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_2_Eve_Packet_Generation_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT2 = "connection_2";

	return ( (TGFLOAT) ( TRANSMISSION_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_2_Eve_Packet_Generation_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT2 = "connection_2";
	char *ev_name_param = "Packet_Generation";

	return ( (TGFLOAT) ( Status )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT On_Off_Source_2_Eve_Packet_Generation_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Status = obj_st->show_st_var_int_value("Status");
	const TGFLOAT ONOFF_RATE = 1.000000e+00;
	const TGFLOAT OFFON_RATE = 4.000000e+00;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e-01;
	char *PORT_OUT2 = "connection_2";

 {msg(PORT_OUT2,all,0);};

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
//  user code for object: Switch_2x2 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Switch_2x2_Eve_Service_Queue_1_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Queue_1 = obj_st->show_st_var_int_value("Queue_1");
	int Queue_2 = obj_st->show_st_var_int_value("Queue_2");
	const int MAX_QUEUE_1 = 11;
	const int MAX_QUEUE_2 = 11;
	const TGFLOAT TRANSMISSION_RATE_1 = 1.179000e-01;
	const TGFLOAT TRANSMISSION_RATE_2 = 1.179000e-01;
	const TGFLOAT PROB_1 = 5.000000e-01;
	const TGFLOAT PROB_2 = 5.000000e-01;
	char *PORT_1 = "connection_1";
	char *PORT_2 = "connection_2";

	return ( (TGFLOAT) ( TRANSMISSION_RATE_1 ) );
}
//------------------------------------------------------------------------------
TGFLOAT Switch_2x2_Eve_Service_Queue_1_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue_1 = obj_st->show_st_var_int_value("Queue_1");
	int Queue_2 = obj_st->show_st_var_int_value("Queue_2");
	const int MAX_QUEUE_1 = 11;
	const int MAX_QUEUE_2 = 11;
	const TGFLOAT TRANSMISSION_RATE_1 = 1.179000e-01;
	const TGFLOAT TRANSMISSION_RATE_2 = 1.179000e-01;
	const TGFLOAT PROB_1 = 5.000000e-01;
	const TGFLOAT PROB_2 = 5.000000e-01;
	char *PORT_1 = "connection_1";
	char *PORT_2 = "connection_2";
	char *ev_name_param = "Service_Queue_1";

	return ( (TGFLOAT) ( Queue_1 )>(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Switch_2x2_Eve_Service_Queue_1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue_1 = obj_st->show_st_var_int_value("Queue_1");
	int Queue_2 = obj_st->show_st_var_int_value("Queue_2");
	const int MAX_QUEUE_1 = 11;
	const int MAX_QUEUE_2 = 11;
	const TGFLOAT TRANSMISSION_RATE_1 = 1.179000e-01;
	const TGFLOAT TRANSMISSION_RATE_2 = 1.179000e-01;
	const TGFLOAT PROB_1 = 5.000000e-01;
	const TGFLOAT PROB_2 = 5.000000e-01;
	char *PORT_1 = "connection_1";
	char *PORT_2 = "connection_2";

 {int queue_1;
          queue_1= Queue_1;
          queue_1 = queue_1 - 1;
          set_st ("Queue_1",queue_1);
         };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Switch_2x2_Eve_Service_Queue_2_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Queue_1 = obj_st->show_st_var_int_value("Queue_1");
	int Queue_2 = obj_st->show_st_var_int_value("Queue_2");
	const int MAX_QUEUE_1 = 11;
	const int MAX_QUEUE_2 = 11;
	const TGFLOAT TRANSMISSION_RATE_1 = 1.179000e-01;
	const TGFLOAT TRANSMISSION_RATE_2 = 1.179000e-01;
	const TGFLOAT PROB_1 = 5.000000e-01;
	const TGFLOAT PROB_2 = 5.000000e-01;
	char *PORT_1 = "connection_1";
	char *PORT_2 = "connection_2";

	return ( (TGFLOAT) ( TRANSMISSION_RATE_2 ) );
}
//------------------------------------------------------------------------------
TGFLOAT Switch_2x2_Eve_Service_Queue_2_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue_1 = obj_st->show_st_var_int_value("Queue_1");
	int Queue_2 = obj_st->show_st_var_int_value("Queue_2");
	const int MAX_QUEUE_1 = 11;
	const int MAX_QUEUE_2 = 11;
	const TGFLOAT TRANSMISSION_RATE_1 = 1.179000e-01;
	const TGFLOAT TRANSMISSION_RATE_2 = 1.179000e-01;
	const TGFLOAT PROB_1 = 5.000000e-01;
	const TGFLOAT PROB_2 = 5.000000e-01;
	char *PORT_1 = "connection_1";
	char *PORT_2 = "connection_2";
	char *ev_name_param = "Service_Queue_2";

	return ( (TGFLOAT) ( Queue_2 )>(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Switch_2x2_Eve_Service_Queue_2_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue_1 = obj_st->show_st_var_int_value("Queue_1");
	int Queue_2 = obj_st->show_st_var_int_value("Queue_2");
	const int MAX_QUEUE_1 = 11;
	const int MAX_QUEUE_2 = 11;
	const TGFLOAT TRANSMISSION_RATE_1 = 1.179000e-01;
	const TGFLOAT TRANSMISSION_RATE_2 = 1.179000e-01;
	const TGFLOAT PROB_1 = 5.000000e-01;
	const TGFLOAT PROB_2 = 5.000000e-01;
	char *PORT_1 = "connection_1";
	char *PORT_2 = "connection_2";

 {int queue_2;
          queue_2 = Queue_2;
          queue_2 = queue_2 - 1;
          set_st ("Queue_2",queue_2);
         };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Switch_2x2_Msg_MSG_1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue_1 = obj_st->show_st_var_int_value("Queue_1");
	int Queue_2 = obj_st->show_st_var_int_value("Queue_2");
	const int MAX_QUEUE_1 = 11;
	const int MAX_QUEUE_2 = 11;
	const TGFLOAT TRANSMISSION_RATE_1 = 1.179000e-01;
	const TGFLOAT TRANSMISSION_RATE_2 = 1.179000e-01;
	const TGFLOAT PROB_1 = 5.000000e-01;
	const TGFLOAT PROB_2 = 5.000000e-01;
	char *PORT_1 = "connection_1";
	char *PORT_2 = "connection_2";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

 {  int queue_1;
            queue_1 = Queue_1;
            if (queue_1< MAX_QUEUE_1)
                queue_1 = queue_1 + 1;
            set_st("Queue_1",queue_1);
         }

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Switch_2x2_Msg_MSG_1_prob_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue_1 = obj_st->show_st_var_int_value("Queue_1");
	int Queue_2 = obj_st->show_st_var_int_value("Queue_2");
	const int MAX_QUEUE_1 = 11;
	const int MAX_QUEUE_2 = 11;
	const TGFLOAT TRANSMISSION_RATE_1 = 1.179000e-01;
	const TGFLOAT TRANSMISSION_RATE_2 = 1.179000e-01;
	const TGFLOAT PROB_1 = 5.000000e-01;
	const TGFLOAT PROB_2 = 5.000000e-01;
	char *PORT_1 = "connection_1";
	char *PORT_2 = "connection_2";

	return ( (TGFLOAT) ( PROB_1 ) );
}
//------------------------------------------------------------------------------
TGFLOAT Switch_2x2_Msg_MSG_1_act_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue_1 = obj_st->show_st_var_int_value("Queue_1");
	int Queue_2 = obj_st->show_st_var_int_value("Queue_2");
	const int MAX_QUEUE_1 = 11;
	const int MAX_QUEUE_2 = 11;
	const TGFLOAT TRANSMISSION_RATE_1 = 1.179000e-01;
	const TGFLOAT TRANSMISSION_RATE_2 = 1.179000e-01;
	const TGFLOAT PROB_1 = 5.000000e-01;
	const TGFLOAT PROB_2 = 5.000000e-01;
	char *PORT_1 = "connection_1";
	char *PORT_2 = "connection_2";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 


         {  int queue_2;
            queue_2 = Queue_2;
            if (queue_2< MAX_QUEUE_2)
                queue_2 = queue_2 + 1;
            set_st("Queue_2",queue_2);
         }

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Switch_2x2_Msg_MSG_1_prob_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue_1 = obj_st->show_st_var_int_value("Queue_1");
	int Queue_2 = obj_st->show_st_var_int_value("Queue_2");
	const int MAX_QUEUE_1 = 11;
	const int MAX_QUEUE_2 = 11;
	const TGFLOAT TRANSMISSION_RATE_1 = 1.179000e-01;
	const TGFLOAT TRANSMISSION_RATE_2 = 1.179000e-01;
	const TGFLOAT PROB_1 = 5.000000e-01;
	const TGFLOAT PROB_2 = 5.000000e-01;
	char *PORT_1 = "connection_1";
	char *PORT_2 = "connection_2";

	return ( (TGFLOAT) ((1))-( PROB_1 ) );
}
//------------------------------------------------------------------------------
TGFLOAT Switch_2x2_Msg_MSG_2_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue_1 = obj_st->show_st_var_int_value("Queue_1");
	int Queue_2 = obj_st->show_st_var_int_value("Queue_2");
	const int MAX_QUEUE_1 = 11;
	const int MAX_QUEUE_2 = 11;
	const TGFLOAT TRANSMISSION_RATE_1 = 1.179000e-01;
	const TGFLOAT TRANSMISSION_RATE_2 = 1.179000e-01;
	const TGFLOAT PROB_1 = 5.000000e-01;
	const TGFLOAT PROB_2 = 5.000000e-01;
	char *PORT_1 = "connection_1";
	char *PORT_2 = "connection_2";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

 {  int queue_1;
            queue_1 = Queue_1;
            if (queue_1 < MAX_QUEUE_1)
                queue_1 = queue_1 + 1;
            set_st("Queue_1",queue_1);
         }

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Switch_2x2_Msg_MSG_2_prob_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue_1 = obj_st->show_st_var_int_value("Queue_1");
	int Queue_2 = obj_st->show_st_var_int_value("Queue_2");
	const int MAX_QUEUE_1 = 11;
	const int MAX_QUEUE_2 = 11;
	const TGFLOAT TRANSMISSION_RATE_1 = 1.179000e-01;
	const TGFLOAT TRANSMISSION_RATE_2 = 1.179000e-01;
	const TGFLOAT PROB_1 = 5.000000e-01;
	const TGFLOAT PROB_2 = 5.000000e-01;
	char *PORT_1 = "connection_1";
	char *PORT_2 = "connection_2";

	return ( (TGFLOAT) ( PROB_2 ) );
}
//------------------------------------------------------------------------------
TGFLOAT Switch_2x2_Msg_MSG_2_act_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue_1 = obj_st->show_st_var_int_value("Queue_1");
	int Queue_2 = obj_st->show_st_var_int_value("Queue_2");
	const int MAX_QUEUE_1 = 11;
	const int MAX_QUEUE_2 = 11;
	const TGFLOAT TRANSMISSION_RATE_1 = 1.179000e-01;
	const TGFLOAT TRANSMISSION_RATE_2 = 1.179000e-01;
	const TGFLOAT PROB_1 = 5.000000e-01;
	const TGFLOAT PROB_2 = 5.000000e-01;
	char *PORT_1 = "connection_1";
	char *PORT_2 = "connection_2";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 



         { int queue_2;
            queue_2 = Queue_2;
            if (queue_2 < MAX_QUEUE_2)
                queue_2 = queue_2 + 1;
            set_st("Queue_2",queue_2);
         }

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Switch_2x2_Msg_MSG_2_prob_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Queue_1 = obj_st->show_st_var_int_value("Queue_1");
	int Queue_2 = obj_st->show_st_var_int_value("Queue_2");
	const int MAX_QUEUE_1 = 11;
	const int MAX_QUEUE_2 = 11;
	const TGFLOAT TRANSMISSION_RATE_1 = 1.179000e-01;
	const TGFLOAT TRANSMISSION_RATE_2 = 1.179000e-01;
	const TGFLOAT PROB_1 = 5.000000e-01;
	const TGFLOAT PROB_2 = 5.000000e-01;
	char *PORT_1 = "connection_1";
	char *PORT_2 = "connection_2";

	return ( (TGFLOAT) ((1))-( PROB_2 ) );
}
//------------------------------------------------------------------------------
TGFLOAT function_handler(int fd, Object_State *obj_st, Simulator *simulator, System_State *sys_st)
{
	TGFLOAT status = -1;

	context.user_code = 1;
	alarm(USER_CODE_MAX_TIME);

	switch (fd) {
		case 1 : status = On_Off_Source_1_Eve_On_Off_EXP_rate (obj_st); break; 

		case 2 : status = On_Off_Source_1_Eve_On_Off_cond (obj_st, simulator); break; 

		case 3 : status = On_Off_Source_1_Eve_On_Off_act_1 (obj_st, simulator); break; 

		case 4 : status = On_Off_Source_1_Eve_Off_On_EXP_rate (obj_st); break; 

		case 5 : status = On_Off_Source_1_Eve_Off_On_cond (obj_st, simulator); break; 

		case 6 : status = On_Off_Source_1_Eve_Off_On_act_1 (obj_st, simulator); break; 

		case 7 : status = On_Off_Source_1_Eve_Packet_Generation_EXP_rate (obj_st); break; 

		case 8 : status = On_Off_Source_1_Eve_Packet_Generation_cond (obj_st, simulator); break; 

		case 9 : status = On_Off_Source_1_Eve_Packet_Generation_act_1 (obj_st, simulator); break; 

		case 10 : status = On_Off_Source_2_Eve_On_Off_EXP_rate (obj_st); break; 

		case 11 : status = On_Off_Source_2_Eve_On_Off_cond (obj_st, simulator); break; 

		case 12 : status = On_Off_Source_2_Eve_On_Off_act_1 (obj_st, simulator); break; 

		case 13 : status = On_Off_Source_2_Eve_Off_On_EXP_rate (obj_st); break; 

		case 14 : status = On_Off_Source_2_Eve_Off_On_cond (obj_st, simulator); break; 

		case 15 : status = On_Off_Source_2_Eve_Off_On_act_1 (obj_st, simulator); break; 

		case 16 : status = On_Off_Source_2_Eve_Packet_Generation_EXP_rate (obj_st); break; 

		case 17 : status = On_Off_Source_2_Eve_Packet_Generation_cond (obj_st, simulator); break; 

		case 18 : status = On_Off_Source_2_Eve_Packet_Generation_act_1 (obj_st, simulator); break; 

		case 19 : status = Switch_2x2_Eve_Service_Queue_1_EXP_rate (obj_st); break; 

		case 20 : status = Switch_2x2_Eve_Service_Queue_1_cond (obj_st, simulator); break; 

		case 21 : status = Switch_2x2_Eve_Service_Queue_1_act_1 (obj_st, simulator); break; 

		case 22 : status = Switch_2x2_Eve_Service_Queue_2_EXP_rate (obj_st); break; 

		case 23 : status = Switch_2x2_Eve_Service_Queue_2_cond (obj_st, simulator); break; 

		case 24 : status = Switch_2x2_Eve_Service_Queue_2_act_1 (obj_st, simulator); break; 

		case 25 : status = Switch_2x2_Msg_MSG_1_act_1 (obj_st, simulator); break; 

		case 26 : status = Switch_2x2_Msg_MSG_1_prob_1 (obj_st, simulator); break; 

		case 27 : status = Switch_2x2_Msg_MSG_1_act_2 (obj_st, simulator); break; 

		case 28 : status = Switch_2x2_Msg_MSG_1_prob_2 (obj_st, simulator); break; 

		case 29 : status = Switch_2x2_Msg_MSG_2_act_1 (obj_st, simulator); break; 

		case 30 : status = Switch_2x2_Msg_MSG_2_prob_1 (obj_st, simulator); break; 

		case 31 : status = Switch_2x2_Msg_MSG_2_act_2 (obj_st, simulator); break; 

		case 32 : status = Switch_2x2_Msg_MSG_2_prob_2 (obj_st, simulator); break; 

		default : fprintf ( stderr, "ERROR: Invalid function descriptor\n"); status = -1;
	}
	context.user_code = 0;
	alarm(0);

	return(status);
}
//------------------------------------------------------------------------------
