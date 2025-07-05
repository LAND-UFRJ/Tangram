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
//  user code for object: System_2 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Fail_Processor_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

	return ( (TGFLOAT) ( Operational_Procs * PROC_FAILURE_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Fail_Processor_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";
	char *ev_name_param = "Fail_Processor";

	return ( (TGFLOAT) ( Operational_Procs )>(0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Fail_Processor_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

{
          int operational_procs;
	  operational_procs = Operational_Procs - 1;
          msg(DATABASE,all,0);
          set_st("Operational_Procs",operational_procs);

	}

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Fail_Processor_prob_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

	return ( (TGFLOAT) ((1))-( PROB ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Fail_Processor_act_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";



       {
          int operational_procs;
	  operational_procs = Operational_Procs - 1;
          set_st("Operational_Procs",operational_procs);

       }

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Fail_Processor_prob_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

	return ( (TGFLOAT) ( PROB ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Repair_Processor_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

	return ( (TGFLOAT) (((( N_PROCS ))-( Operational_Procs ))* PROC_REPAIR_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Repair_Processor_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";
	char *ev_name_param = "Repair_Processor";

	return ( (TGFLOAT) ( Operational_Procs )<=(1) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Repair_Processor_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

{
         int operational_procs;
         operational_procs = Operational_Procs + 1;
         set_st("Operational_Procs",operational_procs);

       };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Fail_Memory_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

	return ( (TGFLOAT) ( FAILURE_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Fail_Memory_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";
	char *ev_name_param = "Fail_Memory";

	return ( (TGFLOAT) ( Operational_Memory )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Fail_Memory_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

{
         ;set_st("Operational_Memory",0);
       };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Repair_Memory_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

	return ( (TGFLOAT) ( REPAIR_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Repair_Memory_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";
	char *ev_name_param = "Repair_Memory";

	return ( (TGFLOAT) ( Operational_Memory )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Repair_Memory_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

{
         ;set_st("Operational_Memory",1);
       };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Fail_Switch_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

	return ( (TGFLOAT) ( FAILURE_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Fail_Switch_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";
	char *ev_name_param = "Fail_Switch";

	return ( (TGFLOAT) ( Operational_Switch )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Fail_Switch_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

{
         ;set_st("Operational_Switch",0);
       };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Repair_Switch_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

	return ( (TGFLOAT) ( REPAIR_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Repair_Switch_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";
	char *ev_name_param = "Repair_Switch";

	return ( (TGFLOAT) ( Operational_Switch )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Eve_Repair_Switch_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

{
         ;set_st("Operational_Switch",1);
       };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Rew_system_2_availability_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

	return ( (TGFLOAT) ((( Operational_Memory )==(1))&&(( Operational_Switch )==(1))&&(( Operational_Procs )>(0))) );
}
//------------------------------------------------------------------------------
TGFLOAT System_2_Rew_system_2_availability_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_2_availability = "system_2_availability";

	return ( (TGFLOAT) (1) );
}
//------------------------------------------------------------------------------
//  user code for object: Front_End 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT Front_End_Eve_Fail_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *front_end_availability = "front_end_availability";

	return ( (TGFLOAT) ( FAILURE_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Eve_Fail_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *front_end_availability = "front_end_availability";
	char *ev_name_param = "Fail";

	return ( (TGFLOAT) ( Operational )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Eve_Fail_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *front_end_availability = "front_end_availability";

{
           ;set_st("Operational",0);
        };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Eve_Repair_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *front_end_availability = "front_end_availability";

	return ( (TGFLOAT) ( REPAIR_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Eve_Repair_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *front_end_availability = "front_end_availability";
	char *ev_name_param = "Repair";

	return ( (TGFLOAT) ( Operational )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Eve_Repair_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *front_end_availability = "front_end_availability";

{
           ;set_st("Operational",1);
        };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Rew_front_end_availability_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *front_end_availability = "front_end_availability";

	return ( (TGFLOAT) ( Operational )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT Front_End_Rew_front_end_availability_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
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
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *DATABASE = "fail_database";
	char *database_availability = "database_availability";

	return ( (TGFLOAT) ( FAILURE_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Eve_Fail_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *DATABASE = "fail_database";
	char *database_availability = "database_availability";
	char *ev_name_param = "Fail";

	return ( (TGFLOAT) ( Operational )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Eve_Fail_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *DATABASE = "fail_database";
	char *database_availability = "database_availability";

{
           ;set_st("Operational",0);
        };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Eve_Repair_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *DATABASE = "fail_database";
	char *database_availability = "database_availability";

	return ( (TGFLOAT) ( REPAIR_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Eve_Repair_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *DATABASE = "fail_database";
	char *database_availability = "database_availability";
	char *ev_name_param = "Repair";

	return ( (TGFLOAT) ( Operational )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Eve_Repair_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *DATABASE = "fail_database";
	char *database_availability = "database_availability";

{
           ;set_st("Operational",1);
        };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Msg_MSG_1_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *DATABASE = "fail_database";
	char *database_availability = "database_availability";
	int   msg_data;
	char  msg_source[MAXSTRING];  
	int   msg_type;
	int   msg_size;
	Pending_Message   *pend_msg = NULL;
	        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); 

 {
           if (Operational == 1)
             ;set_st("Operational",0);
         };

	/* the new state var values must be updated by the user */
  // delete pend_msg
  obj_st->destroy_msg( pend_msg );
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Rew_database_availability_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *DATABASE = "fail_database";
	char *database_availability = "database_availability";

	return ( (TGFLOAT) ( Operational )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT Database_Rew_database_availability_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational = obj_st->show_st_var_int_value("Operational");
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	char *DATABASE = "fail_database";
	char *database_availability = "database_availability";

	return ( (TGFLOAT) (1) );
}
//------------------------------------------------------------------------------
//  user code for object: System_1 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Fail_Processor_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

	return ( (TGFLOAT) ( Operational_Procs * PROC_FAILURE_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Fail_Processor_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";
	char *ev_name_param = "Fail_Processor";

	return ( (TGFLOAT) ( Operational_Procs )>(0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Fail_Processor_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

{
          int operational_procs;
	  operational_procs = Operational_Procs - 1;
          msg(DATABASE,all,0);
          set_st("Operational_Procs",operational_procs);

	}

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Fail_Processor_prob_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

	return ( (TGFLOAT) ((1))-( PROB ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Fail_Processor_act_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";



       {
          int operational_procs;
	  operational_procs = Operational_Procs - 1;
          set_st("Operational_Procs",operational_procs);

       }

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Fail_Processor_prob_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

	return ( (TGFLOAT) ( PROB ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Repair_Processor_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

	return ( (TGFLOAT) (((( N_PROCS ))-( Operational_Procs ))* PROC_REPAIR_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Repair_Processor_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";
	char *ev_name_param = "Repair_Processor";

	return ( (TGFLOAT) ( Operational_Procs )<=(1) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Repair_Processor_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

{
         int operational_procs;
         operational_procs = Operational_Procs + 1;
         set_st("Operational_Procs",operational_procs);

       };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Fail_Memory_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

	return ( (TGFLOAT) ( FAILURE_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Fail_Memory_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";
	char *ev_name_param = "Fail_Memory";

	return ( (TGFLOAT) ( Operational_Memory )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Fail_Memory_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

{
         ;set_st("Operational_Memory",0);
       };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Repair_Memory_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

	return ( (TGFLOAT) ( REPAIR_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Repair_Memory_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";
	char *ev_name_param = "Repair_Memory";

	return ( (TGFLOAT) ( Operational_Memory )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Repair_Memory_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

{
         ;set_st("Operational_Memory",1);
       };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Fail_Switch_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

	return ( (TGFLOAT) ( FAILURE_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Fail_Switch_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";
	char *ev_name_param = "Fail_Switch";

	return ( (TGFLOAT) ( Operational_Switch )==(1) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Fail_Switch_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

{
         ;set_st("Operational_Switch",0);
       };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Repair_Switch_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

	return ( (TGFLOAT) ( REPAIR_RATE ) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Repair_Switch_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";
	char *ev_name_param = "Repair_Switch";

	return ( (TGFLOAT) ( Operational_Switch )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Eve_Repair_Switch_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

{
         ;set_st("Operational_Switch",1);
       };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Rew_system_1_availability_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

	return ( (TGFLOAT) ((( Operational_Memory )==(1))&&(( Operational_Switch )==(1))&&(( Operational_Procs )>(0))) );
}
//------------------------------------------------------------------------------
TGFLOAT System_1_Rew_system_1_availability_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Operational_Procs = obj_st->show_st_var_int_value("Operational_Procs");
	int Operational_Switch = obj_st->show_st_var_int_value("Operational_Switch");
	int Operational_Memory = obj_st->show_st_var_int_value("Operational_Memory");
	const int N_PROCS = 2;
	const TGFLOAT FAILURE_RATE = 4.000000e-04;
	const TGFLOAT REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROC_FAILURE_RATE = 8.000000e-03;
	const TGFLOAT PROC_REPAIR_RATE = 1.000000e+00;
	const TGFLOAT PROB = 8.000000e-01;
	char *DATABASE = "fail_database";
	char *system_1_availability = "system_1_availability";

	return ( (TGFLOAT) (1) );
}
//------------------------------------------------------------------------------
TGFLOAT GlobalReward_system_availability_cond_1 (System_State *sys_st)
{

	return ( (TGFLOAT) ((((sys_st->show_reward_val("Database.database_availability")))==(1))&&(((sys_st->show_reward_val("Front_End.front_end_availability")))==(1))&&((((((sys_st->show_reward_val("System_1.system_1_availability")))==(1))))||((((sys_st->show_reward_val("System_2.system_2_availability")))==(1))))) );
}
//------------------------------------------------------------------------------
TGFLOAT GlobalReward_system_availability_act_1 (System_State *sys_st)
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
		case 1 : status = System_2_Eve_Fail_Processor_EXP_rate (obj_st); break; 

		case 2 : status = System_2_Eve_Fail_Processor_cond (obj_st, simulator); break; 

		case 3 : status = System_2_Eve_Fail_Processor_act_1 (obj_st, simulator); break; 

		case 4 : status = System_2_Eve_Fail_Processor_prob_1 (obj_st, simulator); break; 

		case 5 : status = System_2_Eve_Fail_Processor_act_2 (obj_st, simulator); break; 

		case 6 : status = System_2_Eve_Fail_Processor_prob_2 (obj_st, simulator); break; 

		case 7 : status = System_2_Eve_Repair_Processor_EXP_rate (obj_st); break; 

		case 8 : status = System_2_Eve_Repair_Processor_cond (obj_st, simulator); break; 

		case 9 : status = System_2_Eve_Repair_Processor_act_1 (obj_st, simulator); break; 

		case 10 : status = System_2_Eve_Fail_Memory_EXP_rate (obj_st); break; 

		case 11 : status = System_2_Eve_Fail_Memory_cond (obj_st, simulator); break; 

		case 12 : status = System_2_Eve_Fail_Memory_act_1 (obj_st, simulator); break; 

		case 13 : status = System_2_Eve_Repair_Memory_EXP_rate (obj_st); break; 

		case 14 : status = System_2_Eve_Repair_Memory_cond (obj_st, simulator); break; 

		case 15 : status = System_2_Eve_Repair_Memory_act_1 (obj_st, simulator); break; 

		case 16 : status = System_2_Eve_Fail_Switch_EXP_rate (obj_st); break; 

		case 17 : status = System_2_Eve_Fail_Switch_cond (obj_st, simulator); break; 

		case 18 : status = System_2_Eve_Fail_Switch_act_1 (obj_st, simulator); break; 

		case 19 : status = System_2_Eve_Repair_Switch_EXP_rate (obj_st); break; 

		case 20 : status = System_2_Eve_Repair_Switch_cond (obj_st, simulator); break; 

		case 21 : status = System_2_Eve_Repair_Switch_act_1 (obj_st, simulator); break; 

		case 22 : status = System_2_Rew_system_2_availability_cond_1 (obj_st, simulator); break; 

		case 23 : status = System_2_Rew_system_2_availability_act_1 (obj_st, simulator); break; 

		case 24 : status = Front_End_Eve_Fail_EXP_rate (obj_st); break; 

		case 25 : status = Front_End_Eve_Fail_cond (obj_st, simulator); break; 

		case 26 : status = Front_End_Eve_Fail_act_1 (obj_st, simulator); break; 

		case 27 : status = Front_End_Eve_Repair_EXP_rate (obj_st); break; 

		case 28 : status = Front_End_Eve_Repair_cond (obj_st, simulator); break; 

		case 29 : status = Front_End_Eve_Repair_act_1 (obj_st, simulator); break; 

		case 30 : status = Front_End_Rew_front_end_availability_cond_1 (obj_st, simulator); break; 

		case 31 : status = Front_End_Rew_front_end_availability_act_1 (obj_st, simulator); break; 

		case 32 : status = Database_Eve_Fail_EXP_rate (obj_st); break; 

		case 33 : status = Database_Eve_Fail_cond (obj_st, simulator); break; 

		case 34 : status = Database_Eve_Fail_act_1 (obj_st, simulator); break; 

		case 35 : status = Database_Eve_Repair_EXP_rate (obj_st); break; 

		case 36 : status = Database_Eve_Repair_cond (obj_st, simulator); break; 

		case 37 : status = Database_Eve_Repair_act_1 (obj_st, simulator); break; 

		case 38 : status = Database_Msg_MSG_1_act_1 (obj_st, simulator); break; 

		case 39 : status = Database_Rew_database_availability_cond_1 (obj_st, simulator); break; 

		case 40 : status = Database_Rew_database_availability_act_1 (obj_st, simulator); break; 

		case 41 : status = System_1_Eve_Fail_Processor_EXP_rate (obj_st); break; 

		case 42 : status = System_1_Eve_Fail_Processor_cond (obj_st, simulator); break; 

		case 43 : status = System_1_Eve_Fail_Processor_act_1 (obj_st, simulator); break; 

		case 44 : status = System_1_Eve_Fail_Processor_prob_1 (obj_st, simulator); break; 

		case 45 : status = System_1_Eve_Fail_Processor_act_2 (obj_st, simulator); break; 

		case 46 : status = System_1_Eve_Fail_Processor_prob_2 (obj_st, simulator); break; 

		case 47 : status = System_1_Eve_Repair_Processor_EXP_rate (obj_st); break; 

		case 48 : status = System_1_Eve_Repair_Processor_cond (obj_st, simulator); break; 

		case 49 : status = System_1_Eve_Repair_Processor_act_1 (obj_st, simulator); break; 

		case 50 : status = System_1_Eve_Fail_Memory_EXP_rate (obj_st); break; 

		case 51 : status = System_1_Eve_Fail_Memory_cond (obj_st, simulator); break; 

		case 52 : status = System_1_Eve_Fail_Memory_act_1 (obj_st, simulator); break; 

		case 53 : status = System_1_Eve_Repair_Memory_EXP_rate (obj_st); break; 

		case 54 : status = System_1_Eve_Repair_Memory_cond (obj_st, simulator); break; 

		case 55 : status = System_1_Eve_Repair_Memory_act_1 (obj_st, simulator); break; 

		case 56 : status = System_1_Eve_Fail_Switch_EXP_rate (obj_st); break; 

		case 57 : status = System_1_Eve_Fail_Switch_cond (obj_st, simulator); break; 

		case 58 : status = System_1_Eve_Fail_Switch_act_1 (obj_st, simulator); break; 

		case 59 : status = System_1_Eve_Repair_Switch_EXP_rate (obj_st); break; 

		case 60 : status = System_1_Eve_Repair_Switch_cond (obj_st, simulator); break; 

		case 61 : status = System_1_Eve_Repair_Switch_act_1 (obj_st, simulator); break; 

		case 62 : status = System_1_Rew_system_1_availability_cond_1 (obj_st, simulator); break; 

		case 63 : status = System_1_Rew_system_1_availability_act_1 (obj_st, simulator); break; 

		case 64 : status = GlobalReward_system_availability_cond_1 (sys_st); break; 

		case 65 : status = GlobalReward_system_availability_act_1 (sys_st); break; 

		default : fprintf ( stderr, "ERROR: Invalid function descriptor\n"); status = -1;
	}
	context.user_code = 0;
	alarm(0);

	return(status);
}
//------------------------------------------------------------------------------
