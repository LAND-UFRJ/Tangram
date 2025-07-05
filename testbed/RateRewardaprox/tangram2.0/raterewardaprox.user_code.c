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
//  user code for object: Front_End 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Front_End_Eve_Fail_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *front_end_availability = "front_end_availability";

	return ( (TGFLOAT) ( failure_rate ) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Eve_Fail_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *front_end_availability = "front_end_availability";
	char *ev_name_param = "Fail";

	return ( (TGFLOAT) ( operational )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Eve_Fail_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *front_end_availability = "front_end_availability";

{
           ;set_st("operational",0);
        };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Eve_Repair_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *front_end_availability = "front_end_availability";

	return ( (TGFLOAT) ( repair_rate ) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Eve_Repair_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *front_end_availability = "front_end_availability";
	char *ev_name_param = "Repair";

	return ( (TGFLOAT) ( operational )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Eve_Repair_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *front_end_availability = "front_end_availability";

{
           ;set_st("operational",1);
        };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Rew_front_end_availability_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *front_end_availability = "front_end_availability";

	return ( (TGFLOAT) ( operational )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Rew_front_end_availability_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *front_end_availability = "front_end_availability";

	return ( (TGFLOAT) (1) );
}
//------------------------------------------------------------------------------
//  user code for object: Database 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Database_Eve_Fail_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *database_availability = "database_availability";
	char *states = "states";

	return ( (TGFLOAT) ( failure_rate ) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Eve_Fail_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *database_availability = "database_availability";
	char *states = "states";
	char *ev_name_param = "Fail";

	return ( (TGFLOAT) ( operational )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Eve_Fail_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *database_availability = "database_availability";
	char *states = "states";

{
          ;set_st("operational",0);
        };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Eve_Repair_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *database_availability = "database_availability";
	char *states = "states";

	return ( (TGFLOAT) ( repair_rate ) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Eve_Repair_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *database_availability = "database_availability";
	char *states = "states";
	char *ev_name_param = "Repair";

	return ( (TGFLOAT) ( operational )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Eve_Repair_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *database_availability = "database_availability";
	char *states = "states";

{
           ;set_st("operational",1);
        };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Rew_database_availability_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *database_availability = "database_availability";
	char *states = "states";

	return ( (TGFLOAT) ( operational )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Rew_database_availability_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *database_availability = "database_availability";
	char *states = "states";

	return ( (TGFLOAT) (1) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Rew_states_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *database_availability = "database_availability";
	char *states = "states";

	return ( (TGFLOAT) (TRUE) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Rew_states_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int operational = obj_st->show_st_var_int_value("operational");
	const TGFLOAT failure_rate = 4.000000e-04;
	const TGFLOAT repair_rate = 1.000000e+00;
	char *database_availability = "database_availability";
	char *states = "states";

	return ( (TGFLOAT) (1) );
}
//------------------------------------------------------------------------------
TGFLOAT GlobalReward_states_cond_1 (System_State *sys_st)
{

	return ( (TGFLOAT) (TRUE) );
}
//------------------------------------------------------------------------------
TGFLOAT GlobalReward_states_act_1 (System_State *sys_st)
{

	return ( (TGFLOAT) (1) );
}
//------------------------------------------------------------------------------
TGFLOAT function_handler(int fd, Object_State *obj_st, Simulator *simulator, System_State *sys_st)
{
	TGFLOAT status = -1;

	context.user_code = 1;
	alarm(USER_CODE_MAX_TIME);

	switch (fd) {
		case 1 : status = Front_End_Eve_Fail_EXP_rate (obj_st); break; 

		case 2 : status = Front_End_Eve_Fail_cond (obj_st, simulator); break; 

		case 3 : status = Front_End_Eve_Fail_act_1 (obj_st, simulator); break; 

		case 4 : status = Front_End_Eve_Repair_EXP_rate (obj_st); break; 

		case 5 : status = Front_End_Eve_Repair_cond (obj_st, simulator); break; 

		case 6 : status = Front_End_Eve_Repair_act_1 (obj_st, simulator); break; 

		case 7 : status = Front_End_Rew_front_end_availability_cond_1 (obj_st, simulator); break; 

		case 8 : status = Front_End_Rew_front_end_availability_act_1 (obj_st, simulator); break; 

		case 9 : status = Database_Eve_Fail_EXP_rate (obj_st); break; 

		case 10 : status = Database_Eve_Fail_cond (obj_st, simulator); break; 

		case 11 : status = Database_Eve_Fail_act_1 (obj_st, simulator); break; 

		case 12 : status = Database_Eve_Repair_EXP_rate (obj_st); break; 

		case 13 : status = Database_Eve_Repair_cond (obj_st, simulator); break; 

		case 14 : status = Database_Eve_Repair_act_1 (obj_st, simulator); break; 

		case 15 : status = Database_Rew_database_availability_cond_1 (obj_st, simulator); break; 

		case 16 : status = Database_Rew_database_availability_act_1 (obj_st, simulator); break; 

		case 17 : status = Database_Rew_states_cond_1 (obj_st, simulator); break; 

		case 18 : status = Database_Rew_states_act_1 (obj_st, simulator); break; 

		case 19 : status = GlobalReward_states_cond_1 (sys_st); break; 

		case 20 : status = GlobalReward_states_act_1 (sys_st); break; 

		default : fprintf ( stderr, "ERROR: Invalid function descriptor\n"); status = -1;
	}
	context.user_code = 0;
	alarm(0);

	return(status);
}
//------------------------------------------------------------------------------
