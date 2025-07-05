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
//  user code for object: Repair_Center 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Repair_Center_Eve_Repair_Front_End_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int *Component_Status = obj_st->show_st_var_vec_value("Component_Status");
	const TGFLOAT PROC1_REPAIR_RATE = 8.000000e-01;
	const TGFLOAT PROC2_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT FRONT_END_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT DATABASE_REPAIR_RATE = 1.000000e+00;
	char *FRONT_END = "Front_End";
	char *DATABASE = "Database";
	char *PROCESSOR = "Processor";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";

	return ( (TGFLOAT) ( FRONT_END_REPAIR_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Repair_Center_Eve_Repair_Front_End_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Component_Status = obj_st->show_st_var_vec_value("Component_Status");
	const TGFLOAT PROC1_REPAIR_RATE = 8.000000e-01;
	const TGFLOAT PROC2_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT FRONT_END_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT DATABASE_REPAIR_RATE = 1.000000e+00;
	char *FRONT_END = "Front_End";
	char *DATABASE = "Database";
	char *PROCESSOR = "Processor";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *ev_name_param = "Repair_Front_End";

	return ( (TGFLOAT) ( Component_Status [(0)])>(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Repair_Center_Eve_Repair_Front_End_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Component_Status = obj_st->show_st_var_vec_value("Component_Status");
	const TGFLOAT PROC1_REPAIR_RATE = 8.000000e-01;
	const TGFLOAT PROC2_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT FRONT_END_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT DATABASE_REPAIR_RATE = 1.000000e+00;
	char *FRONT_END = "Front_End";
	char *DATABASE = "Database";
	char *PROCESSOR = "Processor";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";

 {
                   msg(FAILURE_REPAIR,FRONT_END,0);
                   msg(STATUS,all,1);
                   set_st("Component_Status[0]",0);
              };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Repair_Center_Eve_Repair_Database_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int *Component_Status = obj_st->show_st_var_vec_value("Component_Status");
	const TGFLOAT PROC1_REPAIR_RATE = 8.000000e-01;
	const TGFLOAT PROC2_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT FRONT_END_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT DATABASE_REPAIR_RATE = 1.000000e+00;
	char *FRONT_END = "Front_End";
	char *DATABASE = "Database";
	char *PROCESSOR = "Processor";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";

	return ( (TGFLOAT) ( DATABASE_REPAIR_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Repair_Center_Eve_Repair_Database_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Component_Status = obj_st->show_st_var_vec_value("Component_Status");
	const TGFLOAT PROC1_REPAIR_RATE = 8.000000e-01;
	const TGFLOAT PROC2_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT FRONT_END_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT DATABASE_REPAIR_RATE = 1.000000e+00;
	char *FRONT_END = "Front_End";
	char *DATABASE = "Database";
	char *PROCESSOR = "Processor";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *ev_name_param = "Repair_Database";

	return ( (TGFLOAT) ((( Component_Status [(1)])>(0))&&((( Component_Status [(2)]))+( Component_Status [(3)])<(2))) );
}
//------------------------------------------------------------------------------
TGFLOAT Repair_Center_Eve_Repair_Database_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Component_Status = obj_st->show_st_var_vec_value("Component_Status");
	const TGFLOAT PROC1_REPAIR_RATE = 8.000000e-01;
	const TGFLOAT PROC2_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT FRONT_END_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT DATABASE_REPAIR_RATE = 1.000000e+00;
	char *FRONT_END = "Front_End";
	char *DATABASE = "Database";
	char *PROCESSOR = "Processor";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";

 {
                  msg(FAILURE_REPAIR,DATABASE,0);
                  msg(STATUS,all,1);
                  set_st("Component_Status[1]",0);
              };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Repair_Center_Eve_Repair_Proc1_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int *Component_Status = obj_st->show_st_var_vec_value("Component_Status");
	const TGFLOAT PROC1_REPAIR_RATE = 8.000000e-01;
	const TGFLOAT PROC2_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT FRONT_END_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT DATABASE_REPAIR_RATE = 1.000000e+00;
	char *FRONT_END = "Front_End";
	char *DATABASE = "Database";
	char *PROCESSOR = "Processor";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";

	return ( (TGFLOAT) ( PROC1_REPAIR_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Repair_Center_Eve_Repair_Proc1_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Component_Status = obj_st->show_st_var_vec_value("Component_Status");
	const TGFLOAT PROC1_REPAIR_RATE = 8.000000e-01;
	const TGFLOAT PROC2_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT FRONT_END_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT DATABASE_REPAIR_RATE = 1.000000e+00;
	char *FRONT_END = "Front_End";
	char *DATABASE = "Database";
	char *PROCESSOR = "Processor";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *ev_name_param = "Repair_Proc1";

	return ( (TGFLOAT) (((((( Component_Status [(2)])>(0))&&(( Component_Status [(0)])==(0))&&(( Component_Status [(1)])==(0))))))||((((( Component_Status [(2)])>(0))&&((( Component_Status [(2)]))+( Component_Status [(3)])==(2))&&(( Component_Status [(1)])==(1))))) );
}
//------------------------------------------------------------------------------
TGFLOAT Repair_Center_Eve_Repair_Proc1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Component_Status = obj_st->show_st_var_vec_value("Component_Status");
	const TGFLOAT PROC1_REPAIR_RATE = 8.000000e-01;
	const TGFLOAT PROC2_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT FRONT_END_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT DATABASE_REPAIR_RATE = 1.000000e+00;
	char *FRONT_END = "Front_End";
	char *DATABASE = "Database";
	char *PROCESSOR = "Processor";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";

 {
                 int component_status;
                 msg(FAILURE_REPAIR,PROCESSOR,1);
                 component_status = Component_Status[2] - 1;
                 if (Component_Status[1]== 0)
                      
                      msg(STATUS,all,1);
                 set_st("Component_Status[2]",component_status);
         };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Repair_Center_Eve_Repair_Proc2_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int *Component_Status = obj_st->show_st_var_vec_value("Component_Status");
	const TGFLOAT PROC1_REPAIR_RATE = 8.000000e-01;
	const TGFLOAT PROC2_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT FRONT_END_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT DATABASE_REPAIR_RATE = 1.000000e+00;
	char *FRONT_END = "Front_End";
	char *DATABASE = "Database";
	char *PROCESSOR = "Processor";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";

	return ( (TGFLOAT) ( PROC2_REPAIR_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Repair_Center_Eve_Repair_Proc2_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Component_Status = obj_st->show_st_var_vec_value("Component_Status");
	const TGFLOAT PROC1_REPAIR_RATE = 8.000000e-01;
	const TGFLOAT PROC2_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT FRONT_END_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT DATABASE_REPAIR_RATE = 1.000000e+00;
	char *FRONT_END = "Front_End";
	char *DATABASE = "Database";
	char *PROCESSOR = "Processor";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *ev_name_param = "Repair_Proc2";

	return ( (TGFLOAT) (((((( Component_Status [(3)])>(0))&&(( Component_Status [(0)])==(0))&&(( Component_Status [(1)])==(0))))))||((((( Component_Status [(3)])>(0))&&((( Component_Status [(2)]))+( Component_Status [(3)])==(2))&&(( Component_Status [(1)])==(1))))) );
}
//------------------------------------------------------------------------------
TGFLOAT Repair_Center_Eve_Repair_Proc2_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Component_Status = obj_st->show_st_var_vec_value("Component_Status");
	const TGFLOAT PROC1_REPAIR_RATE = 8.000000e-01;
	const TGFLOAT PROC2_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT FRONT_END_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT DATABASE_REPAIR_RATE = 1.000000e+00;
	char *FRONT_END = "Front_End";
	char *DATABASE = "Database";
	char *PROCESSOR = "Processor";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";

 {
                 int component_status;
                 msg(FAILURE_REPAIR,PROCESSOR,2);
                 component_status = Component_Status[3] - 1;
                 if (Component_Status[1]== 0)
                      
                      msg(STATUS,all,1);
                 set_st("Component_Status[3]",component_status);
	 };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Repair_Center_Msg_MSG_1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int *Component_Status = obj_st->show_st_var_vec_value("Component_Status");
	const TGFLOAT PROC1_REPAIR_RATE = 8.000000e-01;
	const TGFLOAT PROC2_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT FRONT_END_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT DATABASE_REPAIR_RATE = 1.000000e+00;
	char *FRONT_END = "Front_End";
	char *DATABASE = "Database";
	char *PROCESSOR = "Processor";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

{
                  int component_status[4],index;
		  index = 0;
		  while (index < 4)
		     {
		          component_status[index] = Component_Status[index];
			  index = index + 1;
		      }
	          if(objcmp(msg_source,FRONT_END))
                    {
                           component_status[0]=component_status[0]+1;
                           msg(STATUS,all,0);
                     }
                   if(objcmp(msg_source,DATABASE) )
                     {
                          component_status[1]=component_status[1]+1;
                           msg(STATUS,all,0);
                     }
	         if(objcmp(msg_source,PROCESSOR))
                    {
                          if ( msg_data == 1 )
                              component_status[2]= component_status[2]+1;
                          else
                              component_status[3]= component_status[3]+1;
                          if (component_status[2] + component_status[3] == 2)
                               msg(STATUS,all,0);
                      }
                  set_st("Component_Status[]",component_status);
         };

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
//  user code for object: Processor 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Processor_Eve_Fail_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Proc1_Fail = obj_st->show_st_var_int_value("Proc1_Fail");
	int Proc2_Fail = obj_st->show_st_var_int_value("Proc2_Fail");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 1.300000e-03;
	const TGFLOAT PROB = 5.000000e-01;
	const TGFLOAT COVERAGE = 8.000000e-01;
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *AFFECT = "affected_port";
	char *REPAIR_CENTER = "Repair_Center";

	return ( (TGFLOAT) ( FAILURE_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Processor_Eve_Fail_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Proc1_Fail = obj_st->show_st_var_int_value("Proc1_Fail");
	int Proc2_Fail = obj_st->show_st_var_int_value("Proc2_Fail");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 1.300000e-03;
	const TGFLOAT PROB = 5.000000e-01;
	const TGFLOAT COVERAGE = 8.000000e-01;
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *AFFECT = "affected_port";
	char *REPAIR_CENTER = "Repair_Center";
	char *ev_name_param = "Fail";

	return ( (TGFLOAT) ((( Can_Fail )==(1))&&((((( Proc1_Fail ))+( Proc2_Fail )))<(2))) );
}
//------------------------------------------------------------------------------
TGFLOAT Processor_Eve_Fail_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Proc1_Fail = obj_st->show_st_var_int_value("Proc1_Fail");
	int Proc2_Fail = obj_st->show_st_var_int_value("Proc2_Fail");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 1.300000e-03;
	const TGFLOAT PROB = 5.000000e-01;
	const TGFLOAT COVERAGE = 8.000000e-01;
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *AFFECT = "affected_port";
	char *REPAIR_CENTER = "Repair_Center";

  {
                      
                      int proc1_fail;
                      proc1_fail = Proc1_Fail + 1;
                      msg(FAILURE_REPAIR,REPAIR_CENTER,1);
                      set_st("Proc1_Fail",proc1_fail);
               }

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Processor_Eve_Fail_prob_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Proc1_Fail = obj_st->show_st_var_int_value("Proc1_Fail");
	int Proc2_Fail = obj_st->show_st_var_int_value("Proc2_Fail");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 1.300000e-03;
	const TGFLOAT PROB = 5.000000e-01;
	const TGFLOAT COVERAGE = 8.000000e-01;
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *AFFECT = "affected_port";
	char *REPAIR_CENTER = "Repair_Center";

	return ( (TGFLOAT) ( PROB ) );
}
//------------------------------------------------------------------------------
TGFLOAT Processor_Eve_Fail_act_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Proc1_Fail = obj_st->show_st_var_int_value("Proc1_Fail");
	int Proc2_Fail = obj_st->show_st_var_int_value("Proc2_Fail");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 1.300000e-03;
	const TGFLOAT PROB = 5.000000e-01;
	const TGFLOAT COVERAGE = 8.000000e-01;
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *AFFECT = "affected_port";
	char *REPAIR_CENTER = "Repair_Center";



        {
	            
                    int proc2_fail;
                    proc2_fail = Proc2_Fail + 1;
                    msg(AFFECT,all,0);
                    msg(FAILURE_REPAIR,REPAIR_CENTER,2);
                    set_st("Proc2_Fail",proc2_fail);
          }

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Processor_Eve_Fail_prob_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Proc1_Fail = obj_st->show_st_var_int_value("Proc1_Fail");
	int Proc2_Fail = obj_st->show_st_var_int_value("Proc2_Fail");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 1.300000e-03;
	const TGFLOAT PROB = 5.000000e-01;
	const TGFLOAT COVERAGE = 8.000000e-01;
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *AFFECT = "affected_port";
	char *REPAIR_CENTER = "Repair_Center";

	return ( (TGFLOAT) ((((((1))-( PROB ))*(((1))-( COVERAGE ))))) );
}
//------------------------------------------------------------------------------
TGFLOAT Processor_Eve_Fail_act_3 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Proc1_Fail = obj_st->show_st_var_int_value("Proc1_Fail");
	int Proc2_Fail = obj_st->show_st_var_int_value("Proc2_Fail");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 1.300000e-03;
	const TGFLOAT PROB = 5.000000e-01;
	const TGFLOAT COVERAGE = 8.000000e-01;
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *AFFECT = "affected_port";
	char *REPAIR_CENTER = "Repair_Center";



        {
	          
                    int proc2_fail;
                    proc2_fail = Proc2_Fail + 1;
                    msg(FAILURE_REPAIR,REPAIR_CENTER,2);
                    set_st("Proc2_Fail",proc2_fail);
        }

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Processor_Eve_Fail_prob_3 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Proc1_Fail = obj_st->show_st_var_int_value("Proc1_Fail");
	int Proc2_Fail = obj_st->show_st_var_int_value("Proc2_Fail");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 1.300000e-03;
	const TGFLOAT PROB = 5.000000e-01;
	const TGFLOAT COVERAGE = 8.000000e-01;
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *AFFECT = "affected_port";
	char *REPAIR_CENTER = "Repair_Center";

	return ( (TGFLOAT) ((((((1))-( PROB ))* COVERAGE ))) );
}
//------------------------------------------------------------------------------
TGFLOAT Processor_Msg_MSG_1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Proc1_Fail = obj_st->show_st_var_int_value("Proc1_Fail");
	int Proc2_Fail = obj_st->show_st_var_int_value("Proc2_Fail");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 1.300000e-03;
	const TGFLOAT PROB = 5.000000e-01;
	const TGFLOAT COVERAGE = 8.000000e-01;
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *AFFECT = "affected_port";
	char *REPAIR_CENTER = "Repair_Center";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

{
         
            int option;
            option = msg_data;
            set_st("Can_Fail",option);
       };

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Processor_Msg_MSG_2_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Proc1_Fail = obj_st->show_st_var_int_value("Proc1_Fail");
	int Proc2_Fail = obj_st->show_st_var_int_value("Proc2_Fail");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 1.300000e-03;
	const TGFLOAT PROB = 5.000000e-01;
	const TGFLOAT COVERAGE = 8.000000e-01;
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *AFFECT = "affected_port";
	char *REPAIR_CENTER = "Repair_Center";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

{
        
            int proc1_fail, proc2_fail;
            proc1_fail = Proc1_Fail;
            proc2_fail = Proc2_Fail;
             if (msg_data == 1)
                  proc1_fail = proc1_fail -1;
            else
                   proc2_fail = proc2_fail - 1;
             set_st("Proc1_Fail",proc1_fail);
             set_st("Proc2_Fail",proc2_fail);
      };

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
//  user code for object: Front_End 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Front_End_Eve_Fail_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Failed = obj_st->show_st_var_int_value("Failed");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 6.900000e-04;
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *REPAIR_CENTER = "Repair_Center";

	return ( (TGFLOAT) ( FAILURE_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Eve_Fail_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Failed = obj_st->show_st_var_int_value("Failed");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 6.900000e-04;
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *REPAIR_CENTER = "Repair_Center";
	char *ev_name_param = "Fail";

	return ( (TGFLOAT) ((( Can_Fail )==(1))&&(( Failed )==(0))) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Eve_Fail_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Failed = obj_st->show_st_var_int_value("Failed");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 6.900000e-04;
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *REPAIR_CENTER = "Repair_Center";

{
                   msg(FAILURE_REPAIR,REPAIR_CENTER,0);
                   set_st("Failed",1);
             };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Msg_MSG_1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Failed = obj_st->show_st_var_int_value("Failed");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 6.900000e-04;
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *REPAIR_CENTER = "Repair_Center";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

{
                 
                   ;set_st("Failed",0);
           };

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Msg_MSG_2_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Failed = obj_st->show_st_var_int_value("Failed");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 6.900000e-04;
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *REPAIR_CENTER = "Repair_Center";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

{
                 
                int option;
                option = msg_data;
                set_st("Can_Fail",option);
       };

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
//  user code for object: Database 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Database_Eve_Fail_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Failed = obj_st->show_st_var_int_value("Failed");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 3.000000e-04;
	char *AFFECT = "affected_port";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *REPAIR_CENTER = "Repair_Center";
	char *Data_avail = "Data_avail";

	return ( (TGFLOAT) ( FAILURE_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Eve_Fail_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Failed = obj_st->show_st_var_int_value("Failed");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 3.000000e-04;
	char *AFFECT = "affected_port";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *REPAIR_CENTER = "Repair_Center";
	char *Data_avail = "Data_avail";
	char *ev_name_param = "Fail";

	return ( (TGFLOAT) ((( Can_Fail )==(1))&&(( Failed )==(0))) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Eve_Fail_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Failed = obj_st->show_st_var_int_value("Failed");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 3.000000e-04;
	char *AFFECT = "affected_port";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *REPAIR_CENTER = "Repair_Center";
	char *Data_avail = "Data_avail";

{
               msg(FAILURE_REPAIR,REPAIR_CENTER,0);
               set_st("Failed",1);
       };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Msg_MSG_1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Failed = obj_st->show_st_var_int_value("Failed");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 3.000000e-04;
	char *AFFECT = "affected_port";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *REPAIR_CENTER = "Repair_Center";
	char *Data_avail = "Data_avail";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

 {
                      
                     if ((Can_Fail == 1)&&(Failed == 0))
                          msg(FAILURE_REPAIR,REPAIR_CENTER,0);
                      set_st("Failed",1);
                };

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Msg_MSG_2_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Failed = obj_st->show_st_var_int_value("Failed");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 3.000000e-04;
	char *AFFECT = "affected_port";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *REPAIR_CENTER = "Repair_Center";
	char *Data_avail = "Data_avail";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

{
                
  		       ;set_st("Failed",0);
       };

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Msg_MSG_3_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Failed = obj_st->show_st_var_int_value("Failed");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 3.000000e-04;
	char *AFFECT = "affected_port";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *REPAIR_CENTER = "Repair_Center";
	char *Data_avail = "Data_avail";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

{
                  
                    int option;
                    option = msg_data;
                    set_st("Can_Fail",option);
       };

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Rew_Data_avail_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Failed = obj_st->show_st_var_int_value("Failed");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 3.000000e-04;
	char *AFFECT = "affected_port";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *REPAIR_CENTER = "Repair_Center";
	char *Data_avail = "Data_avail";

	return ( (TGFLOAT) ( Failed )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Rew_Data_avail_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Failed = obj_st->show_st_var_int_value("Failed");
	int Can_Fail = obj_st->show_st_var_int_value("Can_Fail");
	const TGFLOAT FAILURE_RATE = 3.000000e-04;
	char *AFFECT = "affected_port";
	char *FAILURE_REPAIR = "failure_repair_port";
	char *STATUS = "sys_status_port";
	char *REPAIR_CENTER = "Repair_Center";
	char *Data_avail = "Data_avail";

	return ( (TGFLOAT) (1) );
}
//------------------------------------------------------------------------------
TGFLOAT function_handler(int fd, Object_State *obj_st, Simulator *simulator, System_State *sys_st)
{
	TGFLOAT status = -1;

	context.user_code = 1;
	alarm(USER_CODE_MAX_TIME);

	switch (fd) {
		case 1 : status = Repair_Center_Eve_Repair_Front_End_EXP_rate (obj_st); break; 

		case 2 : status = Repair_Center_Eve_Repair_Front_End_cond (obj_st, simulator); break; 

		case 3 : status = Repair_Center_Eve_Repair_Front_End_act_1 (obj_st, simulator); break; 

		case 4 : status = Repair_Center_Eve_Repair_Database_EXP_rate (obj_st); break; 

		case 5 : status = Repair_Center_Eve_Repair_Database_cond (obj_st, simulator); break; 

		case 6 : status = Repair_Center_Eve_Repair_Database_act_1 (obj_st, simulator); break; 

		case 7 : status = Repair_Center_Eve_Repair_Proc1_EXP_rate (obj_st); break; 

		case 8 : status = Repair_Center_Eve_Repair_Proc1_cond (obj_st, simulator); break; 

		case 9 : status = Repair_Center_Eve_Repair_Proc1_act_1 (obj_st, simulator); break; 

		case 10 : status = Repair_Center_Eve_Repair_Proc2_EXP_rate (obj_st); break; 

		case 11 : status = Repair_Center_Eve_Repair_Proc2_cond (obj_st, simulator); break; 

		case 12 : status = Repair_Center_Eve_Repair_Proc2_act_1 (obj_st, simulator); break; 

		case 13 : status = Repair_Center_Msg_MSG_1_act_1 (obj_st, simulator); break; 

		case 14 : status = Processor_Eve_Fail_EXP_rate (obj_st); break; 

		case 15 : status = Processor_Eve_Fail_cond (obj_st, simulator); break; 

		case 16 : status = Processor_Eve_Fail_act_1 (obj_st, simulator); break; 

		case 17 : status = Processor_Eve_Fail_prob_1 (obj_st, simulator); break; 

		case 18 : status = Processor_Eve_Fail_act_2 (obj_st, simulator); break; 

		case 19 : status = Processor_Eve_Fail_prob_2 (obj_st, simulator); break; 

		case 20 : status = Processor_Eve_Fail_act_3 (obj_st, simulator); break; 

		case 21 : status = Processor_Eve_Fail_prob_3 (obj_st, simulator); break; 

		case 22 : status = Processor_Msg_MSG_1_act_1 (obj_st, simulator); break; 

		case 23 : status = Processor_Msg_MSG_2_act_1 (obj_st, simulator); break; 

		case 24 : status = Front_End_Eve_Fail_EXP_rate (obj_st); break; 

		case 25 : status = Front_End_Eve_Fail_cond (obj_st, simulator); break; 

		case 26 : status = Front_End_Eve_Fail_act_1 (obj_st, simulator); break; 

		case 27 : status = Front_End_Msg_MSG_1_act_1 (obj_st, simulator); break; 

		case 28 : status = Front_End_Msg_MSG_2_act_1 (obj_st, simulator); break; 

		case 29 : status = Database_Eve_Fail_EXP_rate (obj_st); break; 

		case 30 : status = Database_Eve_Fail_cond (obj_st, simulator); break; 

		case 31 : status = Database_Eve_Fail_act_1 (obj_st, simulator); break; 

		case 32 : status = Database_Msg_MSG_1_act_1 (obj_st, simulator); break; 

		case 33 : status = Database_Msg_MSG_2_act_1 (obj_st, simulator); break; 

		case 34 : status = Database_Msg_MSG_3_act_1 (obj_st, simulator); break; 

		case 35 : status = Database_Rew_Data_avail_cond_1 (obj_st, simulator); break; 

		case 36 : status = Database_Rew_Data_avail_act_1 (obj_st, simulator); break; 

		default : fprintf ( stderr, "ERROR: Invalid function descriptor\n"); status = -1;
	}
	context.user_code = 0;
	alarm(0);

	return(status);
}
//------------------------------------------------------------------------------
