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
//  user code for object: Channel 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Channel_Eve_Delayer_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Ack_RN = obj_st->show_st_var_int_value("Ack_RN");
	int N_Acks = obj_st->show_st_var_int_value("N_Acks");
	const int TRANS_WIN = 2;
	const TGFLOAT DELAY_RATE = 5.000000e-01;
	const TGFLOAT P_LOSING_PKT = 1.000000e-01;
	char *SEND_ACK_PORT = "channel_sender";
	char *RECV_ACK_PORT = "receiver_channel";
	char *packet_accepted = "packet_accepted";

	return ( (TGFLOAT) ( N_Acks * DELAY_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Channel_Eve_Delayer_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Ack_RN = obj_st->show_st_var_int_value("Ack_RN");
	int N_Acks = obj_st->show_st_var_int_value("N_Acks");
	const int TRANS_WIN = 2;
	const TGFLOAT DELAY_RATE = 5.000000e-01;
	const TGFLOAT P_LOSING_PKT = 1.000000e-01;
	char *SEND_ACK_PORT = "channel_sender";
	char *RECV_ACK_PORT = "receiver_channel";
	char *packet_accepted = "packet_accepted";
	char *ev_name_param = "Delayer";

	return ( (TGFLOAT) ( N_Acks )>(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Channel_Eve_Delayer_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Ack_RN = obj_st->show_st_var_int_value("Ack_RN");
	int N_Acks = obj_st->show_st_var_int_value("N_Acks");
	const int TRANS_WIN = 2;
	const TGFLOAT DELAY_RATE = 5.000000e-01;
	const TGFLOAT P_LOSING_PKT = 1.000000e-01;
	char *SEND_ACK_PORT = "channel_sender";
	char *RECV_ACK_PORT = "receiver_channel";
	char *packet_accepted = "packet_accepted";

 {
             
             int n_acks,ack_rn;
             ack_rn = (Ack_RN + 1)%(TRANS_WIN + 1);
             msg(SEND_ACK_PORT,all,ack_rn);
             n_acks = N_Acks - 1;
             set_st("N_Acks",n_acks);
             set_st("Ack_RN",ack_rn);
         }

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Channel_Eve_Delayer_prob_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Ack_RN = obj_st->show_st_var_int_value("Ack_RN");
	int N_Acks = obj_st->show_st_var_int_value("N_Acks");
	const int TRANS_WIN = 2;
	const TGFLOAT DELAY_RATE = 5.000000e-01;
	const TGFLOAT P_LOSING_PKT = 1.000000e-01;
	char *SEND_ACK_PORT = "channel_sender";
	char *RECV_ACK_PORT = "receiver_channel";
	char *packet_accepted = "packet_accepted";

	return ( (TGFLOAT) ((1))-( P_LOSING_PKT ) );
}
//------------------------------------------------------------------------------
TGFLOAT Channel_Eve_Delayer_act_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Ack_RN = obj_st->show_st_var_int_value("Ack_RN");
	int N_Acks = obj_st->show_st_var_int_value("N_Acks");
	const int TRANS_WIN = 2;
	const TGFLOAT DELAY_RATE = 5.000000e-01;
	const TGFLOAT P_LOSING_PKT = 1.000000e-01;
	char *SEND_ACK_PORT = "channel_sender";
	char *RECV_ACK_PORT = "receiver_channel";
	char *packet_accepted = "packet_accepted";


         {
              
             int ack_rn, n_acks;
             ack_rn = (Ack_RN + 1)%(TRANS_WIN + 1);
             n_acks = N_Acks - 1;
             set_st("N_Acks",n_acks);
             set_st("Ack_RN",ack_rn);
         }

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Channel_Eve_Delayer_prob_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Ack_RN = obj_st->show_st_var_int_value("Ack_RN");
	int N_Acks = obj_st->show_st_var_int_value("N_Acks");
	const int TRANS_WIN = 2;
	const TGFLOAT DELAY_RATE = 5.000000e-01;
	const TGFLOAT P_LOSING_PKT = 1.000000e-01;
	char *SEND_ACK_PORT = "channel_sender";
	char *RECV_ACK_PORT = "receiver_channel";
	char *packet_accepted = "packet_accepted";

	return ( (TGFLOAT) ( P_LOSING_PKT ) );
}
//------------------------------------------------------------------------------
TGFLOAT Channel_Msg_MSG_1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Ack_RN = obj_st->show_st_var_int_value("Ack_RN");
	int N_Acks = obj_st->show_st_var_int_value("N_Acks");
	const int TRANS_WIN = 2;
	const TGFLOAT DELAY_RATE = 5.000000e-01;
	const TGFLOAT P_LOSING_PKT = 1.000000e-01;
	char *SEND_ACK_PORT = "channel_sender";
	char *RECV_ACK_PORT = "receiver_channel";
	char *packet_accepted = "packet_accepted";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

{ 
           int n_acks; 
           int ack_rn; 
           int module;
           int sn_last_ack; 
        
           module = TRANS_WIN + 1;
           n_acks = N_Acks;
           ack_rn = Ack_RN;
           sn_last_ack = (ack_rn + n_acks)%module;
           if (msg_data == (sn_last_ack+1)%module)
           
           {
               n_acks = N_Acks + 1;
           }
           else
           
           {
               if (n_acks == 0)
           
               {
                   n_acks = N_Acks + 1;
                   
                   ack_rn = (ack_rn + TRANS_WIN)%module;
               }
           }
           set_st("N_Acks",n_acks);
           set_st("Ack_RN",ack_rn);
       };

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Channel_Rew_packet_accepted_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Ack_RN = obj_st->show_st_var_int_value("Ack_RN");
	int N_Acks = obj_st->show_st_var_int_value("N_Acks");
	const int TRANS_WIN = 2;
	const TGFLOAT DELAY_RATE = 5.000000e-01;
	const TGFLOAT P_LOSING_PKT = 1.000000e-01;
	char *SEND_ACK_PORT = "channel_sender";
	char *RECV_ACK_PORT = "receiver_channel";
	char *packet_accepted = "packet_accepted";

	return ( (TGFLOAT) (1) );
}
//------------------------------------------------------------------------------
//  user code for object: Receiver 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Receiver_Msg_MSG_1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int RN = obj_st->show_st_var_int_value("RN");
	const TGFLOAT P_LOSING_PKT = 3.000000e-01;
	const int TRANS_WIN = 2;
	char *ACK_PORT = "receiver_channel";
	char *PKT_PORT = "sender_receiver";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

{ 
            int s;
            s = RN;
            if (msg_data == RN)
            {
                s = (RN + 1) % (TRANS_WIN+1);
                msg(ACK_PORT,all,s);    
                set_st("RN",s);
            }
            else 
            {
                msg(ACK_PORT,all,s);
            }
        }

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Receiver_Msg_MSG_1_prob_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int RN = obj_st->show_st_var_int_value("RN");
	const TGFLOAT P_LOSING_PKT = 3.000000e-01;
	const int TRANS_WIN = 2;
	char *ACK_PORT = "receiver_channel";
	char *PKT_PORT = "sender_receiver";

	return ( (TGFLOAT) ((1))-( P_LOSING_PKT ) );
}
//------------------------------------------------------------------------------
TGFLOAT Receiver_Msg_MSG_1_act_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int RN = obj_st->show_st_var_int_value("RN");
	const TGFLOAT P_LOSING_PKT = 3.000000e-01;
	const int TRANS_WIN = 2;
	char *ACK_PORT = "receiver_channel";
	char *PKT_PORT = "sender_receiver";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 


        {
            
            ;
        }

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Receiver_Msg_MSG_1_prob_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int RN = obj_st->show_st_var_int_value("RN");
	const TGFLOAT P_LOSING_PKT = 3.000000e-01;
	const int TRANS_WIN = 2;
	char *ACK_PORT = "receiver_channel";
	char *PKT_PORT = "sender_receiver";

	return ( (TGFLOAT) ( P_LOSING_PKT ) );
}
//------------------------------------------------------------------------------
//  user code for object: Sender 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Sender_Eve_SN_Transmission_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Win_Begin = obj_st->show_st_var_int_value("Win_Begin");
	int SN = obj_st->show_st_var_int_value("SN");
	const int TRANS_WIN = 2;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e+00;
	char *ACK_PORT = "channel_sender";
	char *PKT_PORT = "sender_receiver";

	return ( (TGFLOAT) ( TRANSMISSION_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Sender_Eve_SN_Transmission_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Win_Begin = obj_st->show_st_var_int_value("Win_Begin");
	int SN = obj_st->show_st_var_int_value("SN");
	const int TRANS_WIN = 2;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e+00;
	char *ACK_PORT = "channel_sender";
	char *PKT_PORT = "sender_receiver";
	char *ev_name_param = "SN_Transmission";

	return ( (TGFLOAT) (TRUE) );
}
//------------------------------------------------------------------------------
TGFLOAT Sender_Eve_SN_Transmission_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Win_Begin = obj_st->show_st_var_int_value("Win_Begin");
	int SN = obj_st->show_st_var_int_value("SN");
	const int TRANS_WIN = 2;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e+00;
	char *ACK_PORT = "channel_sender";
	char *PKT_PORT = "sender_receiver";

{
           int s, module, win_end;
           s = SN;
           module = TRANS_WIN + 1;
           win_end = (Win_Begin + TRANS_WIN - 1)%module;
           msg(PKT_PORT,all,SN);
           if (SN != win_end )
           
           {
               s = (SN + 1)%module;
           }
           else
            
           {
               s = Win_Begin;
           }
           set_st("SN",s);
       };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Sender_Msg_MSG_1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Win_Begin = obj_st->show_st_var_int_value("Win_Begin");
	int SN = obj_st->show_st_var_int_value("SN");
	const int TRANS_WIN = 2;
	const TGFLOAT TRANSMISSION_RATE = 1.000000e+00;
	char *ACK_PORT = "channel_sender";
	char *PKT_PORT = "sender_receiver";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

 {
             int w_b, w_e, s, module, diff,
                 order_of_SN_in_window,order_of_RN_in_window;

             w_b = Win_Begin; 
             module = TRANS_WIN + 1;
             s = SN; 

             

             diff = module - Win_Begin;
             order_of_SN_in_window = ( s + diff ) % module;
             order_of_RN_in_window = (int)(msg_data + diff ) % module;

             w_b = (int)(msg_data) % module;
 
             if ( order_of_RN_in_window > order_of_SN_in_window )
             {
                 s = (int)(msg_data) % module;
             }
           
             set_st("Win_Begin",w_b);
             set_st("SN",s);
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
		case 1 : status = Channel_Eve_Delayer_EXP_rate (obj_st); break; 

		case 2 : status = Channel_Eve_Delayer_cond (obj_st, simulator); break; 

		case 3 : status = Channel_Eve_Delayer_act_1 (obj_st, simulator); break; 

		case 4 : status = Channel_Eve_Delayer_prob_1 (obj_st, simulator); break; 

		case 5 : status = Channel_Eve_Delayer_act_2 (obj_st, simulator); break; 

		case 6 : status = Channel_Eve_Delayer_prob_2 (obj_st, simulator); break; 

		case 7 : status = Channel_Msg_MSG_1_act_1 (obj_st, simulator); break; 

		case 8 : status = Channel_Rew_packet_accepted_act_1 (obj_st, simulator); break; 

		case 9 : status = Receiver_Msg_MSG_1_act_1 (obj_st, simulator); break; 

		case 10 : status = Receiver_Msg_MSG_1_prob_1 (obj_st, simulator); break; 

		case 11 : status = Receiver_Msg_MSG_1_act_2 (obj_st, simulator); break; 

		case 12 : status = Receiver_Msg_MSG_1_prob_2 (obj_st, simulator); break; 

		case 13 : status = Sender_Eve_SN_Transmission_EXP_rate (obj_st); break; 

		case 14 : status = Sender_Eve_SN_Transmission_cond (obj_st, simulator); break; 

		case 15 : status = Sender_Eve_SN_Transmission_act_1 (obj_st, simulator); break; 

		case 16 : status = Sender_Msg_MSG_1_act_1 (obj_st, simulator); break; 

		default : fprintf ( stderr, "ERROR: Invalid function descriptor\n"); status = -1;
	}
	context.user_code = 0;
	alarm(0);

	return(status);
}
//------------------------------------------------------------------------------
