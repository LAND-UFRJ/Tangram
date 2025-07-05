/*
 * Copyright (C) 1999-2009, Edmundo Albuquerque de Souza e Silva.
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE.QPL included in the packaging of this file.
 *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

//------------------------------------------------------------------------------
//        T A N G R A M - I I      << . >>      L A N D - U F R J - B R A Z I L
//------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>

#include "gramatica.h"
#include "code_generator.h"

/* global declaration of all structures used in parsing */
extern t_obj_desc obj_desc_tb[MAX_TABLE_ENTRIES];

/* current parse status */
extern t_status current_status;

/* the global rewards table */
extern t_global_rew global_rew_tb[MAX_TABLE_ENTRIES];

/* the global indep chains table */
extern t_indep_chain indep_chain_tb[MAX_TABLE_ENTRIES];

extern int Can_Generate_Chain;
extern int Can_Simulate;

extern has_mtk_object;

//------------------------------------------------------------------------------
//     F U N C T I O N S
//------------------------------------------------------------------------------
void write_header(FILE *fd)
{
    fprintf( fd,"//------------------------------------------------------------------------------\n");
    fprintf( fd,"//        T A N G R A M - I I      << . >>      L A N D - U F R J - B R A Z I L\n");
    fprintf( fd,"//------------------------------------------------------------------------------\n");
    fprintf( fd,"//            This file is generated automatically by the Tangram tool. \n");
    fprintf( fd,"//        It contains the user code for actions, messages and expressions.\n");
    fprintf( fd,"//------------------------------------------------------------------------------\n\n");
    fprintf( fd,"#include <stdio.h> \n");
    fprintf( fd,"#include <stdlib.h>\n");
    fprintf( fd,"#include <string.h>\n");
    fprintf( fd,"#include <unistd.h>\n");
    fprintf( fd,"#include <math.h>  \n");
    fprintf( fd,"#include <signal.h>\n");
    fprintf( fd,"\n");

    if( has_mtk_object )
    {
        write_mtk_header( fd );
        fprintf( fd,"\n");
    }

    fprintf( fd, "void action_code_alarm_handler(int); /* handler for action code alarm */\n");

    fprintf( fd,"#include <user_code.h>\n");
    fprintf( fd,"\n");
    fprintf( fd,"//------------------------------------------------------------------------------\n");
    fprintf( fd,"/* execution context data structure */\n");
    fprintf( fd,"extern t_context context;\n");
    fprintf( fd,"//------------------------------------------------------------------------------\n");
    fprintf( fd,"static char *all = \"*\";\n" );
    fprintf( fd,"int CAN_GENERATE_CHAIN = %d;\n", Can_Generate_Chain );
    fprintf( fd,"int CAN_SIMULATE = %d;\n", Can_Simulate );
    fprintf( fd,"\n\n");

    if( has_mtk_object )
    {
        write_mtk_setup( fd );
        fprintf( fd,"\n");
    }
}
//------------------------------------------------------------------------------
void write_mtk_header( FILE *fd )
{
    fprintf( fd, "/* MTK headers */\n"            );
    fprintf( fd, "#include <core_controller.h>\n" );
    fprintf( fd, "#include <object.h>\n"          );
    fprintf( fd, "#include <object_manager.h>\n"  );
    fprintf( fd, "#include <plugin.h>\n"          );
    fprintf( fd, "#include <plugin_manager.h>\n"  );
    fprintf( fd, "#include <types.h>\n"           );
}
//------------------------------------------------------------------------------
void write_mtk_setup( FILE *fd )
{
    fprintf( fd, "/**\n * Setup MTK core when the"
                 " first object is invoked\n */\n"                           );
    fprintf( fd, "void MTKSetup( void )\n{\n"                                );
    fprintf( fd, "\tstatic int mtk_started = 0;\n"                           );
    fprintf( fd, "\tchar *plugin_path;\n"                                    );
    fprintf( fd, "\tstd::list<std::string> path_list;\n"                     );
    fprintf( fd, "\n\tif( !mtk_started )\n\t{"                               );
    fprintf( fd, "\n\t\tmtk_started = 1;\n"                                  );
    fprintf( fd, "\n\t\t/* Mute terminal */\n"                               );
    fprintf( fd, "\t\tTerminal::setTerminal( Terminal::TERMINAL_FILE );\n"   );
    fprintf( fd, "\t\tTerminal *t = Terminal::getTerminal();\n"              );
    fprintf( fd, "\t\tt->setOutputName( \"/dev/null\" );\n"                  );
    fprintf( fd, "\n\t\t/* Get plugins directory */\n"                       );    
    fprintf( fd, "\t\tplugin_path = getenv( \"TANGRAM2_HOME\" );\n"          );
    fprintf( fd, "\t\tstrcat( plugin_path, \"/lib/mtk_plugins\" );\n"        );
    fprintf( fd, "\t\tpath_list.push_back( plugin_path );\n"                 );
    fprintf( fd, "\n\t\t/* Setup MTK core */\n"                              );
    fprintf( fd, "\t\tCoreController::setupCore( path_list );\n"             );
    fprintf( fd, "\t}\n"                                                     ); 
    fprintf( fd, "}\n"                                                       ); 
}
//------------------------------------------------------------------------------
void write_action_code_alarm_handler( FILE *fd )
{
fprintf( fd, "void action_code_alarm_handler(int signum)                   \n" );
fprintf( fd, "{                                                            \n" );
fprintf( fd, "  context.act_time_count+=USER_CODE_MAX_TIME;                \n" );
fprintf( fd, "  fprintf( stderr, \"Warning: %%d seconds spent executing action code in Object:%%s   Event:%%s  Action:%%d. \", context.act_time_count, context.obj_name,context.ev_msg_name, context.act_num  );  \n" );
fprintf( fd, "  fprintf( stderr, \"         Continuing... (To stop press <Cancel>) \\n\");    \n");  
fprintf( fd, "  alarm(USER_CODE_MAX_TIME); \n ");

fprintf( fd, "}\n "  );
}

//------------------------------------------------------------------------------
void write_function_handler( FILE *fd )
{
    int i,j;

    fprintf( fd,"//------------------------------------------------------------------------------\n");
    fprintf( fd,"TGFLOAT function_handler(int fd, Object_State *obj_st, Simulator *simulator, System_State *sys_st)\n");
    fprintf( fd,"{\n");
    fprintf( fd,"\tTGFLOAT status = -1;\n\n");
    fprintf( fd,"\tcontext.user_code = 1;\n");
    fprintf( fd, "\tcontext.act_time_count=0;                \n" );
    
    fprintf( fd,"\tstruct sigaction alarm_handler; // for installing action code alarm handler\n");
    fprintf( fd,"\tsigset_t alarm_mask; // to set mask for action code alarm handler\n");

    fprintf( fd,"\talarm_handler.sa_handler = action_code_alarm_handler; // name of action code alarm handler\n");
    
    fprintf( fd,"\talarm_handler.sa_flags = SA_RESTART; // flag is just RESTART\n");
    fprintf( fd,"\tsigemptyset(&alarm_mask); // clear all bits of blocked set\n");
    fprintf( fd,"\t sigaddset(&alarm_mask, SIGALRM);\n");
    fprintf( fd,"\talarm_handler.sa_mask = alarm_mask; // set this empty set to be the mask\n");
    fprintf( fd,"\tif ( sigaction(SIGALRM, &alarm_handler, NULL) == -1 ) // try to install the new alarm handler\n");
    fprintf( fd,"\t{                         \n");
    fprintf( fd,"\t\tperror(\"sigaction\");  \n");
    fprintf( fd,"\t\treturn (1);             \n");    
    fprintf( fd,"\t}                         \n");
    
    fprintf( fd,"\talarm(USER_CODE_MAX_TIME);\n\n");
    
    fprintf( fd,"\tswitch (fd) {\n");

    for( i = 0; i < current_status.obj_desc_index; i++ )
    {
	    for( j = 0; j < obj_desc_tb[i].code_entries; j++ )
        {
	        if( obj_desc_tb[i].code_tb[j].type != MSG_REC_TYPE            &&
		        obj_desc_tb[i].code_tb[j].type != BOUND_TYPE              &&
		        obj_desc_tb[i].code_tb[j].type != REWARD_CR_INIT_VAL_TYPE &&
		        obj_desc_tb[i].code_tb[j].type != REWARD_4SUM_TYPE        &&
		        obj_desc_tb[i].code_tb[j].type != REWARD_EVENT_TYPE       &&
		        obj_desc_tb[i].code_tb[j].type != REWARD_MSG_TYPE         &&
		        obj_desc_tb[i].code_tb[j].expr_type == EXEC_TYPE           )
            {
		        if( obj_desc_tb[i].code_tb[j].type == ACT_TYPE )
                {
		            fprintf( fd,"\t\tcase %d : status = %s_%s (obj_st, simulator); break; \n\n",
                                    obj_desc_tb[i].code_tb[j].func_hand,
		                            obj_desc_tb[i].name,
                                    obj_desc_tb[i].code_tb[j].func_name );
		        }
		        else if (obj_desc_tb[i].code_tb[j].type != DISTRIB_TYPE)
                {
		            fprintf( fd, "\t\tcase %d : status = %s_%s (obj_st, simulator); break; \n\n",
		                     obj_desc_tb[i].code_tb[j].func_hand,
                             obj_desc_tb[i].name,
                             obj_desc_tb[i].code_tb[j].func_name );
		        }
		        else
                {
		            switch (obj_desc_tb[i].code_tb[j].distrib_type)
                    {
		                case EXPONENTIAL_DIST :
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.exp.func_hand,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.exp.func_name);
			            break;
		                case DETERMINISTIC_DIST :
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.det.func_hand,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.det.func_name);
			            break;
		                case UNIFORM_DIST :
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.uni.func_hand_low,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.uni.func_name_low);
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.uni.func_hand_upp,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.uni.func_name_upp);
			            break;
		                case GAUSSIAN_DIST :
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.gauss.func_hand_mean,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.gauss.func_name_mean);
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.gauss.func_hand_var,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.gauss.func_name_var);
			            break;
		                case ERLANG_M_DIST :
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.erl.func_hand_rate,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.erl.func_name_rate);
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.erl.func_hand_stages,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.erl.func_name_stages);
			            break;
		                case LOGNORMAL_DIST :
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.log_norm.func_hand_mean,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.log_norm.func_name_mean);
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.log_norm.func_hand_var,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.log_norm.func_name_var);
			            break;
		                case WEIBULL_DIST :
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.weib.func_hand_scale,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.weib.func_name_scale);
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.weib.func_hand_shape,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.weib.func_name_shape);
			            break;
		                case PARETO_DIST :
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.par.func_hand_scale,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.par.func_name_scale);
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.par.func_hand_shape,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.par.func_name_shape);
			            break;
		                case TRUNC_PARETO_DIST :
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.trunc_par.func_hand_scale,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.trunc_par.func_name_scale);
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.trunc_par.func_hand_shape,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.trunc_par.func_name_shape);
			            fprintf (fd,"\t\tcase %d : status = %s_%s (obj_st); break; \n\n",
				             obj_desc_tb[i].code_tb[j].distrib.trunc_par.func_hand_maximum,
				             obj_desc_tb[i].name,
				             obj_desc_tb[i].code_tb[j].distrib.trunc_par.func_name_maximum);
			            break;
		            }
                }
	        }
	    }
    }

    for( i = 0; i < current_status.global_rew_index; i++ )
    {
	    if( global_rew_tb[i].type == COND_TYPE         ||
	        global_rew_tb[i].type == REWARD_VALUE_TYPE  )
        {

	        fprintf( fd, "\t\tcase %d : status = %s (sys_st); break; \n\n",
                     global_rew_tb[i].func_hand,
                     global_rew_tb[i].func_name );
	    }
    }

    fprintf( fd, "\t\tdefault : fprintf ( stderr, \"ERROR: Invalid function descriptor\\n\"); status = -1;\n" );
    fprintf( fd,"\t}\n" );
    fprintf( fd,"\tcontext.user_code = 0;\n" );
    fprintf( fd,"\talarm(0);\n" );

    fprintf( fd,"\n\treturn(status);\n" );
    fprintf( fd,"}\n" );
    fprintf( fd,"//------------------------------------------------------------------------------\n" );
    fflush ( fd );
}
//------------------------------------------------------------------------------
void write_func_header( FILE *fd, char *obj_name, char *func_name )
{
    fprintf( fd, "//------------------------------------------------------------------------------\n" );
    fprintf( fd, "TGFLOAT %s_%s (Object_State *obj_st)\n", obj_name, func_name );
    fprintf( fd, "{\n" );
}
//------------------------------------------------------------------------------
void write_func_header_4_act_cond( FILE *fd, char *obj_name, char *func_name )
{
    fprintf( fd, "//------------------------------------------------------------------------------\n" );
    fprintf( fd,"TGFLOAT %s_%s (Object_State *obj_st, Simulator *simulator)\n", obj_name, func_name );
    fprintf( fd,"{\n" );
}
//------------------------------------------------------------------------------
void write_reward_function( FILE *fd, int index )
{
    fprintf( fd, "//------------------------------------------------------------------------------\n");
    fprintf( fd, "TGFLOAT %s (System_State *sys_st)\n", global_rew_tb[index].func_name);
    fprintf( fd, "{\n" );
    fprintf( fd, "\n\treturn ( (TGFLOAT) %s );\n", global_rew_tb[index].code );
    fprintf( fd, "}\n" );
}
//------------------------------------------------------------------------------
void write_decl_init_part( FILE *fd, int i, int j )
{
    char var_name[ MAX_STRING ];
    int  k;

    fprintf( fd, "\t/* declaration and initialization of object variables reference */\n" );

    for( k=0; k < obj_desc_tb[ i ].sym_entries; k++ )
    {
        strcpy( var_name, obj_desc_tb[ i ].sym_tb[ k ].name );
        switch( obj_desc_tb[ i ].sym_tb[ k ].type )
        {
            case STATE_VAR_TYPE:
                if( obj_desc_tb[ i ].sym_tb[ k ].dimension == 1 )
                    fprintf( fd, "\tint %s = obj_st->show_st_var_int_value(\"%s\");\n",
                             var_name, var_name );
                else
                    fprintf( fd, "\tint *%s = obj_st->show_st_var_vec_value(\"%s\");\n",
                             var_name, var_name );
                break;
                
            case STATE_VAR_FLOAT_TYPE :
                if (obj_desc_tb[i].sym_tb[k].dimension == 1)
                    fprintf( fd, "\tTGFLOAT %s = obj_st->show_st_var_float_value(\"%s\");\n",
                             var_name, var_name );
                else
                    fprintf( fd, "\tTGFLOAT *%s = obj_st->show_st_var_fvec_value(\"%s\");\n",
                             var_name, var_name );
                break;
                
            case INT_CONST_TYPE :
                fprintf( fd, "\tconst int %s = %d;\n",
                         var_name, obj_desc_tb[i].sym_tb[k].val.i );
                break;
                
            case INT_CONST_VEC_TYPE :
                fprintf( fd, "\tconst int %s[] = {%s};\n",
                         var_name, sym_tb_obj_get_vec_val(
                                               &obj_desc_tb[ i ].sym_tb[ k ],
                                                INT_CONST_VEC_TYPE ) );
                break;                
                
            case FLOAT_CONST_TYPE :
                fprintf( fd,"\tconst TGFLOAT %s = %e;\n",
                         var_name, obj_desc_tb[i].sym_tb[k].val.f );
                break;

            case FLOAT_CONST_VEC_TYPE :
                fprintf( fd, "\tconst TGFLOAT %s[] = {%s};\n",
                         var_name, sym_tb_obj_get_vec_val(
                                               &obj_desc_tb[ i ].sym_tb[ k ],
                                                FLOAT_CONST_VEC_TYPE ) );
                break;                
                
            case OBJECT_TYPE :
            case MSG_PORT_TYPE :
                fprintf( fd, "\tchar *%s = \"%s\";\n",
                         var_name, obj_desc_tb[i].sym_tb[k].val.str );
                break;
                
            case REWARD_TYPE     :
            case REWARD_SUM_TYPE :
                fprintf( fd, "\tchar *%s = \"%s\";\n", var_name, var_name );
                break;
        }
    }

    /* Only needed for event condition. Used as a param of     */
    /* special_rr_event function.                              */
    /* Note: ev_name_param can't be used as a user identifier. */
    if( (obj_desc_tb[i].code_tb[j].part == EVENTS_PART) &&
        (obj_desc_tb[i].code_tb[j].type == COND_TYPE) )
        fprintf( fd, "\tchar *ev_name_param = \"%s\";\n",
                 obj_desc_tb[ i ].code_tb[ j ].name );
}
//------------------------------------------------------------------------------
void write_distrib_functions( FILE *fd, int i, int j )
{
    switch( obj_desc_tb[i].code_tb[j].distrib_type )
    {
        case EXPONENTIAL_DIST :
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.exp.func_name );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd,
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.exp.rate );
	    fprintf( fd, "}\n" );
	    fflush( fd );
	    break;
        
        case DETERMINISTIC_DIST :
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.det.func_name );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd,
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.det.mean );
	    fprintf( fd, "}\n" );
	    fflush( fd );
	    break;
        
        case UNIFORM_DIST :
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.uni.func_name_low );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd,
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.uni.lower );
	    fprintf( fd, "}\n" );
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.uni.func_name_upp );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd,
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.uni.upper );
	    fprintf( fd,"}\n" );
	    fflush( fd );
	    break;
        
        case GAUSSIAN_DIST :
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.gauss.func_name_mean );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd,
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.gauss.mean );
	    fprintf( fd, "}\n" );
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.gauss.func_name_var );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd,
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.gauss.var );
	    fprintf( fd,"}\n" );
	    fflush( fd );
	    break;
        
        case ERLANG_M_DIST :
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.erl.func_name_rate );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd,
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.erl.rate );
	    fprintf( fd, "}\n" );
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.erl.func_name_stages );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd,
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.erl.stages );
	    fprintf( fd, "}\n" );
	    fflush( fd );
	    break;
        
        case LOGNORMAL_DIST :
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.log_norm.func_name_mean );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd,
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.log_norm.mean );
	    fprintf( fd, "}\n" );
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.log_norm.func_name_var );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd, 
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.log_norm.var );
	    fprintf( fd, "}\n" );
	    fflush( fd );
	    break;
        
        case WEIBULL_DIST :
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.weib.func_name_scale );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd,
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.weib.scale );
	    fprintf( fd, "}\n\n" );
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.weib.func_name_shape );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd,
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.weib.shape );
	    fprintf( fd, "}\n\n" );
	    fflush( fd );
	    break;
        
        case PARETO_DIST :
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.par.func_name_scale );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd,
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.par.scale );
	    fprintf( fd, "}\n\n" );
	    fflush( fd );
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.par.func_name_shape );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd, 
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.par.shape );
	    fprintf( fd, "}\n\n" );
	    fflush( fd );
	    break;
        
        case TRUNC_PARETO_DIST :
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.trunc_par.func_name_scale );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd,
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.trunc_par.scale );
	    fprintf( fd, "}\n\n" );
	    fflush( fd );
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.trunc_par.func_name_shape );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd,
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.trunc_par.shape );
	    fprintf( fd, "}\n\n" );
	    fflush( fd );
	    write_func_header( fd,
                           obj_desc_tb[i].name,
                           obj_desc_tb[i].code_tb[j].distrib.trunc_par.func_name_maximum );
	    write_decl_init_part( fd, i, j );
	    fprintf( fd,
                 "\n\treturn ( (TGFLOAT) %s );\n",
                 obj_desc_tb[i].code_tb[j].distrib.trunc_par.maximum );
	    fprintf( fd, "}\n\n" );
	    fflush( fd );
	    break;
    }
}
//------------------------------------------------------------------------------
int code_generator( char *base_filename )
{
    char  filename[ MAX_STRING ];
    FILE *fd;
    int   i, j;

    sprintf( filename, "%s.user_code.c", base_filename );

    if( ( fd = fopen( filename, "w" ) ) == NULL )
    {
        fprintf( stderr, "Error while opening the file: %s\n", filename );
        perror( "fopen" );
        return( -1 );
    }

    write_header( fd );

    for( i = 0; i < current_status.obj_desc_index; i++ )
    {
        fprintf( fd, "//------------------------------------------------------------------------------\n" );
        fprintf( fd, "//  user code for object: %s \n", obj_desc_tb[i].name );
        fprintf( fd, "//------------------------------------------------------------------------------\n\n" );

	    for( j = 0; j < obj_desc_tb[ i ].code_entries; j++ )
        {
	        if( obj_desc_tb[i].code_tb[j].type == MSG_REC_TYPE )
    		    continue;

	        if( obj_desc_tb[i].code_tb[j].type == REWARD_EVENT_TYPE )
    		    continue;

	        if( obj_desc_tb[i].code_tb[j].type == REWARD_MSG_TYPE )
    		    continue;

	        if( obj_desc_tb[i].code_tb[j].type == BOUND_TYPE )
    		    continue;

	        if( obj_desc_tb[i].code_tb[j].type == REWARD_CR_INIT_VAL_TYPE )
    		    continue;

	        if( obj_desc_tb[i].code_tb[j].type == REWARD_4SUM_TYPE )
    		    continue;

	        if( obj_desc_tb[i].code_tb[j].expr_type == INTERP_TYPE )
    		    continue;

	        if( obj_desc_tb[i].code_tb[j].type == DISTRIB_TYPE )
            {
    		    write_distrib_functions( fd, i ,j );
    		    continue;
	        }

            /* Actions and conditions use a second parameter (simulator). */
            write_func_header_4_act_cond( fd,
                                          obj_desc_tb[i].name,
                                          obj_desc_tb[i].code_tb[j].func_name );

            write_decl_init_part( fd, i, j );


            if( obj_desc_tb[i].code_tb[j].part == MESSAGES_PART &&
                obj_desc_tb[i].code_tb[j].type == ACT_TYPE        )
            {
                fprintf( fd, "\tint   msg_data;\n" );
                fprintf( fd, "\tchar  msg_source[MAXSTRING];  \n" );
                fprintf( fd, "\tint   msg_type;\n" );
                fprintf( fd, "\tint   msg_size;\n" );

                fprintf( fd, "\tPending_Message   *pend_msg = NULL;\n" );
                fprintf( fd, "\t        pend_msg = obj_st->get_msg_info(&msg_data, msg_source, &msg_type, &msg_size); \n" );
            }
            fprintf( fd, "\n" );

            if( obj_desc_tb[i].code_tb[j].type == COND_TYPE         ||
                obj_desc_tb[i].code_tb[j].type == PROB_TYPE         ||
                obj_desc_tb[i].code_tb[j].type == REWARD_VALUE_TYPE  )
            {
                fprintf( fd, "\treturn ( (TGFLOAT) %s );\n",
                         obj_desc_tb[i].code_tb[j].code );
            }
            else
            {
                fprintf( fd, "%s", obj_desc_tb[i].code_tb[j].code );
                fprintf( fd, "\n\n\t/* the new state var values must be updated by the user */\n" );

                if( obj_desc_tb[i].code_tb[j].part == MESSAGES_PART &&
                    obj_desc_tb[i].code_tb[j].type == ACT_TYPE       )
                {
                    // delete pend_msg if the action belongs to a message attribute
                    fprintf( fd, "  // delete pend_msg\n" );
                    fprintf( fd, "  obj_st->destroy_msg( pend_msg );\n" );
                }

                fprintf( fd, "  return ( (TGFLOAT) (0) );\n" );
            }

            fprintf( fd, "}\n" );
            fflush( fd );
        }
    }

    for( i = 0; i < current_status.global_rew_index; i++ )
    {
        if( global_rew_tb[i].type == COND_TYPE         ||
            global_rew_tb[i].type == REWARD_VALUE_TYPE  )
            write_reward_function( fd, i );
    }
    
    write_action_code_alarm_handler( fd );
    write_function_handler( fd );

    fclose( fd );
    
    return 1;
}

//------------------------------------------------------------------------------
int dump_tables( char *base_filename )
{
    char  filename[ MAX_STRING ];
    char  sigver[ 17 ];
    FILE *fd;
    int   i, j;

    sprintf( filename, "%s.tables_dump", base_filename );

    if( (fd = fopen( filename, "w" )) == NULL )
    {
        fprintf( stderr, "Error while opening the file: %s\n", filename );
        perror( "fopen" );
        return( -1 );
    }

    /* write the signature and version */
    sprintf( sigver, "%c%c%c%cVersion 3.0\n", -1, -1, -1, -1 );
    fwrite( (void *)&sigver, 16, 1, fd );

    /* write the number of objects to dump */
    fwrite( (void *)&current_status.obj_desc_index, sizeof( int ), 1, fd );

    /* go through all objects and dump their tables */
    for( i = 0; i < current_status.obj_desc_index; i++ )
    {
        /* write the name of the object */
        fwrite( (void *)&obj_desc_tb[ i ].name, MAX_NAME * sizeof(char), 1, fd);

        /* write the number of entries in code table */
        fwrite( (void *)&obj_desc_tb[i].code_entries, sizeof(int), 1, fd);

        /* go through all the entries in the code table */
        for( j = 0; j < obj_desc_tb[ i ].code_entries; j++ )
            fwrite( (void *)&obj_desc_tb[i].code_tb[j], sizeof(t_code), 1, fd );

        /* write the number of entries in symbol table */
        fwrite( (void *)&obj_desc_tb[ i ].sym_entries, sizeof( int ), 1, fd );

        /* go through all the entries in the symbol table */
        for( j = 0; j < obj_desc_tb[ i ].sym_entries; j++ )
        {
            fwrite( (void *)&obj_desc_tb[ i ].sym_tb[ j ],
                    sizeof( t_symbol_obj ), 1, fd );

            switch( obj_desc_tb[ i ].sym_tb[ j ].type )
            {
                case STATE_VAR_TYPE:
                    fwrite( (void *)obj_desc_tb[ i ].sym_tb[ j ].val.int_vec, 
                            sizeof( int ) * obj_desc_tb[ i ].sym_tb[ j ].dimension,
                            1, fd );
                    break;

                case STATE_VAR_FLOAT_TYPE:
                    fwrite( (void *)obj_desc_tb[ i ].sym_tb[ j ].val.tgf_vec, 
                            sizeof( TGFLOAT ) * obj_desc_tb[ i ].sym_tb[ j ].dimension,
                            1, fd );
                    break;
            }
        }
    }

    /* write the number of entries in the global reward table */
    fwrite( (void *)&current_status.global_rew_index, sizeof( int ), 1, fd );

    /* go through all the entries in the global reward table */
    for( i = 0; i < current_status.global_rew_index; i++ )
        fwrite( (void *)&global_rew_tb[ i ], sizeof( t_global_rew ), 1, fd );

    /* write the number of entries in the independent chains table */
    fwrite( (void *)&current_status.indep_chain_index, sizeof( int ), 1, fd );

    /* go through all the entries in the independent chains table */
    for( i = 0; i < current_status.indep_chain_index; i++ )
        fwrite( (void *)&indep_chain_tb[ i ], sizeof( t_indep_chain ), 1, fd );

    fclose( fd );
    
    return 1;
}
//------------------------------------------------------------------------------
/* initialize each state variable maxvalue. This maxvalue is given by the     */
/* .maxvalue file                                                             */
int initialize_max_values( char *base_filename )
{
    FILE *fd;
    char  filename[MAX_STRING];
    char  str[MAX_STRING];
    char *obj_name, *var_name;
    int   max_val, ok;
    int   i, j;

    sprintf( filename, "%s.maxvalues", base_filename );

    if( (fd = fopen( filename, "r" )) == NULL )
    {
        fprintf( stderr, "Error while opening the file: %s\n", filename );
        perror( "fopen" );
        return( -1 );
    }

    while( !feof( fd ) )
    {
        if( fscanf( fd, "%s %d\n", str, &max_val ) <= 0 )
        {
            fprintf( stderr, "ERROR: While reading the %s file\n", filename );
            fprintf( stderr, "ERROR: Invalid file format\n" );
            return( -1 );
        }

        /* split the name of the object and the state variable */
        obj_name = strtok( str, "." );
        var_name = strtok( NULL, "." );
        ok = 0;
        for( i = 0; i < current_status.obj_desc_index; i++ )
        {
            if( !strcmp( obj_desc_tb[ i ].name, obj_name ) )
            {
                for( j = 0; j < obj_desc_tb[ i ].sym_entries; j++ )
                    if( !strcmp( obj_desc_tb[ i ].sym_tb[ j ].name, var_name ) )
                    {
                        obj_desc_tb[ i ].sym_tb[ j ].max_value = max_val;
                        ok = 1;
                        break;
                    }
                if( ok != 1 )
                {
                    fprintf( stderr, "ERROR: While reading the %s file\n",
                             filename );
                    fprintf( stderr, "ERROR: No state variable %s found within the object %s\n",
                             var_name, obj_name );
                    return( -1 );
                }		
            }
            if (ok == 1)
                break;
        }
        if( ok != 1 )
        {
            fprintf( stderr, "ERROR: While reading the %s file\n", filename );
            fprintf( stderr, "ERROR: No object found with the name: %s\n",
                     obj_name );
            return( -1 );
        }
    }

    fclose( fd );
    
    return 1;
}
//------------------------------------------------------------------------------
/* Reorder the objects and the state varibles. */
/* The order is given by the .maxvalues file. */
int reorder_objects_and_variables(char *base_filename)
{
    FILE *fd;
    char  filename[ MAX_STRING ];
    char  str[ MAX_STRING ];

    t_obj_desc obj_desc_aux;
    int   obj_aux_index;
    t_symbol_obj sym_aux;
    int   sym_aux_index;

    char *obj_name, *var_name;
    int   max_val, ok;
    int   i, j;

    sprintf( filename, "%s.maxvalues", base_filename );

    if( (fd = fopen( filename, "r" )) == NULL )
    {
        fprintf ( stderr, "Error while opening the file: %s\n", filename);
        perror ("fopen");
        return (-1);
    }

    if( fscanf( fd, "%s %d\n", str, &max_val ) <= 0 )
    {
        fprintf( stderr, "ERROR: While reading the %s file\n", filename );
        fprintf( stderr, "ERROR: Invalid file format\n" );
        return( -1 );
    }
    
    /* split the name of the object and the state variable */
    obj_name = strtok( str, "." );
    var_name = strtok( NULL, "." );
    obj_aux_index = 0;

    while( !feof( fd ) )
    {
	    ok = 0;
	    /* order the objects */
	    for (i = 0; i<current_status.obj_desc_index; i++)
        {
	        if ( !strcmp(obj_desc_tb[i].name, obj_name))
            {
		        sym_aux_index = 0;
		        /* read all the state variable of this object */
		        while (!strcmp(obj_desc_tb[i].name, obj_name))
                {
		            ok = 0;
		            /* order the state variable */
		            for (j=0; j<obj_desc_tb[i].sym_entries; j++)
                    {
			            if (!strcmp(obj_desc_tb[i].sym_tb[j].name, var_name))
                        {
			                /* trade places with the correct symbol */
			                memcpy( (char *)&(sym_aux),
				                (char *)&(obj_desc_tb[i].sym_tb[sym_aux_index]), 
				                sizeof (t_symbol_obj));
			                memcpy( (char *)&(obj_desc_tb[i].sym_tb[sym_aux_index]),
				                (char *)&(obj_desc_tb[i].sym_tb[j]), 
				                sizeof (t_symbol_obj));
			                memcpy( (char *)&(obj_desc_tb[i].sym_tb[j]),
				                (char *)&(sym_aux), sizeof (t_symbol_obj));

			                sym_aux_index++;
			                ok = 1;
			                break;
			            }
                    }
                    
		            if (ok != 1)
                    {
			            fprintf ( stderr, "ERROR: While reading the %s file\n", filename);
			            fprintf ( stderr, "ERROR: No state variable %s found within the object %s\n", 
				            var_name, obj_name);
			            return (-1);
		            }

		            if (feof(fd))
    			        break;

		            /* read one more line */
		            if (fscanf (fd, "%s %d\n", str, &max_val) <= 0 )
                    {
			            fprintf ( stderr, "ERROR: While reading the %s file\n", filename);
			            fprintf ( stderr, "ERROR: Invalid file format\n");
			            return (-1);
		            }

		            /* split the name of the object and the state variable */
		            obj_name = strtok (str, ".");
		            var_name = strtok (NULL, ".");
		        }

		        /* trade places with the correct object */
		        memcpy ( (char *)&(obj_desc_aux), 
			         (char *)&(obj_desc_tb[obj_aux_index]), sizeof (t_obj_desc) );
		        memcpy ( (char *)&(obj_desc_tb[obj_aux_index]), 
			         (char *)&(obj_desc_tb[i]), sizeof (t_obj_desc) );
		        memcpy ( (char *)&(obj_desc_tb[i]), 
			         (char *)&(obj_desc_aux), sizeof (t_obj_desc) );
		        obj_aux_index++;
		        ok = 1;
		        break;
	        }
	    }
        if( ok != 1 )
        {
            fprintf( stderr, "ERROR: While reading the %s file\n", filename );
            fprintf( stderr, "ERROR: No object found with the name: %s\n",
                     obj_name );
            return( -1 );
        }
    }

    fclose( fd );
    return 1;
}
/******************************************************************************/
/* check to see if all object variables initialization are valid. This is     */
/* done by checking if the object name given to a Object type var exists.     */
/******************************************************************************/
int check_object_initialization()
{
    int  i, j;

    for( i = 0; i < current_status.obj_desc_index; i++ )
    {
        for( j = 0; j < current_status.obj_desc_index; j++ )
        {
            if( i != j )
            {
                if( !strcmp( obj_desc_tb[ i ].name, obj_desc_tb[ j ].name ) )
                {
                    fprintf( stderr, "ERROR: While checking the objects name.\n");
                    fprintf( stderr, "ERROR: Two objects found with the same name: %s\n", obj_desc_tb[i].name);
                    return( -1 );
                }
            }	    
        }
    }
    return( 1 );
}
//------------------------------------------------------------------------------
/* check to see if there are objects with the same name */
int check_object_name()
{
    char obj_name[ MAX_STRING ];
    int  i, j, k;
    int  ok;

    for( i = 0; i < current_status.obj_desc_index; i++ )
    {
        for( j = 0; j < obj_desc_tb[ i ].sym_entries; j++ )
        {
            if( obj_desc_tb[ i ].sym_tb[ j ].type == OBJECT_TYPE )
            {
                strcpy( obj_name, obj_desc_tb[ i ].sym_tb[ j ].val.str );
                ok = 0;

                for( k = 0; k < current_status.obj_desc_index; k++ )
                    if( !strcmp( obj_name, obj_desc_tb[ k ].name ) )
                    {
                        ok = 1;
                        break;
                    }
                if( ok != 1 )
                {
                    fprintf( stderr, "ERROR: While checking the object %s initialization.\n",
                             obj_desc_tb[ i ].name );
                    fprintf( stderr, "ERROR: No object found with the name: %s\n",obj_name);
                    return( -1 );
                }
            }
        }
    }
    return( 1 );
}
//------------------------------------------------------------------------------
/* check to see if the indentifier passed as parameter is a object name */
int is_object( char *name )
{
    int  i;

    for( i = 0; i < current_status.obj_desc_index; i++ )
    {
        if( !strcmp( name, obj_desc_tb[ i ].name ) )
            return( 1 );
    }
    return( 0 );
}
/*----------------------------------------------------------------------------*/
/* Generate modelname.vstat file which contains the state variables           */
/* present in the model. The main difference betwen modelname.state_variable  */
/* is that in the later the vector variables are not dismembered.             */
/*----------------------------------------------------------------------------*/
int generate_vstat( char *base_filename )
{
    int i, j, k;
    FILE *fd;
    char  filename[MAX_STRING];

    sprintf( filename, "%s.vstat", base_filename );

    if( (fd = fopen( filename, "w" )) == NULL)
    {
        fprintf( stderr, "Error while opening the file: %s\n", filename );
        perror( "fopen" );
        return( -1 );
    }
    /* Run thru all objects */
    for( i = 0; i < current_status.obj_desc_index; i++ )
    {
        /* All symbols, i. e., state variables of this obj */
        for( j = 0; j < obj_desc_tb[ i ].sym_entries; j++ )
        {
            /* Is this symbol a state variable ? */
            if( obj_desc_tb[ i ].sym_tb[ j ].type == STATE_VAR_TYPE )
            {
                /* Is a normal state variable ? */
                if( obj_desc_tb[i].sym_tb[j].dimension == 1 )
                {
                    fprintf( fd, "%s.%s\n", obj_desc_tb[i].name,
                                            obj_desc_tb[i].sym_tb[j].name );
                }
                else /* is vector */
                {
                    for( k = 0; k < obj_desc_tb[i].sym_tb[j].dimension; k++ )
                        fprintf( fd, "%s.%s[%d]\n", obj_desc_tb[i].name,
                                             obj_desc_tb[i].sym_tb[j].name, k );
                }
            }
            else if( obj_desc_tb[ i ].sym_tb[ j ].type == STATE_VAR_FLOAT_TYPE )
            {
                /* Is a normal state variable ? */
                if( obj_desc_tb[i].sym_tb[j].dimension == 1 )
                {
                    fprintf( fd, "%s.%s\n", obj_desc_tb[i].name,
                                            obj_desc_tb[i].sym_tb[j].name );
                }
                else /* is vector */
                {
                    for( k = 0; k < obj_desc_tb[i].sym_tb[j].dimension; k++ )
                        fprintf( fd, "%s.%s[%d]\n", obj_desc_tb[i].name,
                                             obj_desc_tb[i].sym_tb[j].name, k );
                }
            }
            else if( obj_desc_tb[ i ].sym_tb[ j ].type == STATE_VAR_INTQUEUE_TYPE )
            {
                fprintf( fd, "%s.%s\n", obj_desc_tb[i].name,
                                        obj_desc_tb[i].sym_tb[j].name );
            }
            else if( obj_desc_tb[ i ].sym_tb[ j ].type == STATE_VAR_FLOATQUEUE_TYPE )
            {
                fprintf( fd, "%s.%s\n", obj_desc_tb[i].name,
                                        obj_desc_tb[i].sym_tb[j].name );
            }
        }
    }

    fclose( fd );
    return( 1 );
}

/*----------------------------------------------------------------------------*/
/* Generate the <modelname>.events file which contains the object names       */
/* followed by the name of the event. All events of all objects must appear   */
/* only once in this file.                                                    */
/*----------------------------------------------------------------------------*/
int generate_events( char *base_filename )
{
    int i, j;
    FILE *fd;
    char  filename[ MAX_STRING ];

    sprintf( filename, "%s.events", base_filename );

    if( (fd = fopen( filename, "w" )) == NULL)
    {
        fprintf( stderr, "Error while opening the file: %s\n", filename );
        perror( "fopen" );
        return( -1 );
    }

    /* Run thru all objects */
    for( i = 0; i < current_status.obj_desc_index; i++ )
    {
        for( j = 0; j < obj_desc_tb[i].sym_entries; j++ )
            if( obj_desc_tb[i].sym_tb[j].type == EVENT_TYPE )
               fprintf( fd, "%s.%s\n", obj_desc_tb[ i ].name,
                        obj_desc_tb[ i ].sym_tb[ j ].name );
    }
    fclose( fd );

    return( 1 );
}
