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
//  user code for object: birth_death 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
TGFLOAT birth_death_Eve_Enable_Source_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Active_Sources = obj_st->show_st_var_int_value("Active_Sources");
	const TGFLOAT ALFA = 7.120000e-02;
	const TGFLOAT BETA = 1.525000e-01;
	const TGFLOAT RATE = 5.000000e+02;
	const TGFLOAT CAPACITY = 2.250000e+02;
	const int MAXIMUM = 20;
	char *buffer = "buffer";

	return ( (TGFLOAT) (((( MAXIMUM ))-( Active_Sources ))* ALFA ) );
}
//------------------------------------------------------------------------------
TGFLOAT birth_death_Eve_Enable_Source_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Active_Sources = obj_st->show_st_var_int_value("Active_Sources");
	const TGFLOAT ALFA = 7.120000e-02;
	const TGFLOAT BETA = 1.525000e-01;
	const TGFLOAT RATE = 5.000000e+02;
	const TGFLOAT CAPACITY = 2.250000e+02;
	const int MAXIMUM = 20;
	char *buffer = "buffer";
	char *ev_name_param = "Enable_Source";

	return ( (TGFLOAT) ( Active_Sources )<( MAXIMUM ) );
}
//------------------------------------------------------------------------------
TGFLOAT birth_death_Eve_Enable_Source_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Active_Sources = obj_st->show_st_var_int_value("Active_Sources");
	const TGFLOAT ALFA = 7.120000e-02;
	const TGFLOAT BETA = 1.525000e-01;
	const TGFLOAT RATE = 5.000000e+02;
	const TGFLOAT CAPACITY = 2.250000e+02;
	const int MAXIMUM = 20;
	char *buffer = "buffer";

 { 
           int active_sources;
           active_sources = Active_Sources + 1; 
           set_st("Active_Sources", active_sources);
         };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT birth_death_Eve_Disable_Source_EXP_rate (Object_State *obj_st)
{
	/* declaration and initialization of object variables reference */
	int Active_Sources = obj_st->show_st_var_int_value("Active_Sources");
	const TGFLOAT ALFA = 7.120000e-02;
	const TGFLOAT BETA = 1.525000e-01;
	const TGFLOAT RATE = 5.000000e+02;
	const TGFLOAT CAPACITY = 2.250000e+02;
	const int MAXIMUM = 20;
	char *buffer = "buffer";

	return ( (TGFLOAT) ( Active_Sources * BETA ) );
}
//------------------------------------------------------------------------------
TGFLOAT birth_death_Eve_Disable_Source_cond (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Active_Sources = obj_st->show_st_var_int_value("Active_Sources");
	const TGFLOAT ALFA = 7.120000e-02;
	const TGFLOAT BETA = 1.525000e-01;
	const TGFLOAT RATE = 5.000000e+02;
	const TGFLOAT CAPACITY = 2.250000e+02;
	const int MAXIMUM = 20;
	char *buffer = "buffer";
	char *ev_name_param = "Disable_Source";

	return ( (TGFLOAT) ( Active_Sources )>(0) );
}
//------------------------------------------------------------------------------
TGFLOAT birth_death_Eve_Disable_Source_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Active_Sources = obj_st->show_st_var_int_value("Active_Sources");
	const TGFLOAT ALFA = 7.120000e-02;
	const TGFLOAT BETA = 1.525000e-01;
	const TGFLOAT RATE = 5.000000e+02;
	const TGFLOAT CAPACITY = 2.250000e+02;
	const int MAXIMUM = 20;
	char *buffer = "buffer";

 { 
           int active_sources;
           active_sources = Active_Sources - 1; 
           set_st("Active_Sources", active_sources);
         };

	/* the new state var values must be updated by the user */
  return ( (TGFLOAT) (0) );
}
//------------------------------------------------------------------------------
TGFLOAT birth_death_Rew_buffer_cond_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Active_Sources = obj_st->show_st_var_int_value("Active_Sources");
	const TGFLOAT ALFA = 7.120000e-02;
	const TGFLOAT BETA = 1.525000e-01;
	const TGFLOAT RATE = 5.000000e+02;
	const TGFLOAT CAPACITY = 2.250000e+02;
	const int MAXIMUM = 20;
	char *buffer = "buffer";

	return ( (TGFLOAT) ( Active_Sources )>(0) );
}
//------------------------------------------------------------------------------
TGFLOAT birth_death_Rew_buffer_act_1 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Active_Sources = obj_st->show_st_var_int_value("Active_Sources");
	const TGFLOAT ALFA = 7.120000e-02;
	const TGFLOAT BETA = 1.525000e-01;
	const TGFLOAT RATE = 5.000000e+02;
	const TGFLOAT CAPACITY = 2.250000e+02;
	const int MAXIMUM = 20;
	char *buffer = "buffer";

	return ( (TGFLOAT) (((( Active_Sources * RATE ))))-( CAPACITY ) );
}
//------------------------------------------------------------------------------
TGFLOAT birth_death_Rew_buffer_cond_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Active_Sources = obj_st->show_st_var_int_value("Active_Sources");
	const TGFLOAT ALFA = 7.120000e-02;
	const TGFLOAT BETA = 1.525000e-01;
	const TGFLOAT RATE = 5.000000e+02;
	const TGFLOAT CAPACITY = 2.250000e+02;
	const int MAXIMUM = 20;
	char *buffer = "buffer";

	return ( (TGFLOAT) ( Active_Sources )==(0) );
}
//------------------------------------------------------------------------------
TGFLOAT birth_death_Rew_buffer_act_2 (Object_State *obj_st, Simulator *simulator)
{
	/* declaration and initialization of object variables reference */
	int Active_Sources = obj_st->show_st_var_int_value("Active_Sources");
	const TGFLOAT ALFA = 7.120000e-02;
	const TGFLOAT BETA = 1.525000e-01;
	const TGFLOAT RATE = 5.000000e+02;
	const TGFLOAT CAPACITY = 2.250000e+02;
	const int MAXIMUM = 20;
	char *buffer = "buffer";

	return ( (TGFLOAT) (- CAPACITY ) );
}
//------------------------------------------------------------------------------
TGFLOAT function_handler(int fd, Object_State *obj_st, Simulator *simulator, System_State *sys_st)
{
	TGFLOAT status = -1;

	context.user_code = 1;
	alarm(USER_CODE_MAX_TIME);

	switch (fd) {
		case 1 : status = birth_death_Eve_Enable_Source_EXP_rate (obj_st); break; 

		case 2 : status = birth_death_Eve_Enable_Source_cond (obj_st, simulator); break; 

		case 3 : status = birth_death_Eve_Enable_Source_act_1 (obj_st, simulator); break; 

		case 4 : status = birth_death_Eve_Disable_Source_EXP_rate (obj_st); break; 

		case 5 : status = birth_death_Eve_Disable_Source_cond (obj_st, simulator); break; 

		case 6 : status = birth_death_Eve_Disable_Source_act_1 (obj_st, simulator); break; 

		case 7 : status = birth_death_Rew_buffer_cond_1 (obj_st, simulator); break; 

		case 8 : status = birth_death_Rew_buffer_act_1 (obj_st, simulator); break; 

		case 9 : status = birth_death_Rew_buffer_cond_2 (obj_st, simulator); break; 

		case 10 : status = birth_death_Rew_buffer_act_2 (obj_st, simulator); break; 

		default : fprintf ( stderr, "ERROR: Invalid function descriptor\n"); status = -1;
	}
	context.user_code = 0;
	alarm(0);

	return(status);
}
//------------------------------------------------------------------------------
