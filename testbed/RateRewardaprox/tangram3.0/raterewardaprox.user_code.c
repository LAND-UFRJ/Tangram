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
