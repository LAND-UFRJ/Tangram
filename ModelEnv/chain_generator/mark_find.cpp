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

/**
*	@file mark_find.cpp
*   @brief <b>Mark_find main file</b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram-II project
*   @since version 1.0
*
*   The detailed description is unavailable. 
*/

#include <iostream>

using namespace std;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#ifdef TG_PARALLEL
#include <pvm3.h>
#endif

#include <sys/time.h>

#include "prototypes.h"
#include "general.h"
#include "gramatica.h"
#include "chained_list.h"
#include "system_desc.h"
#include "system_state.h"
#include "output.h"
#include "hash.h"
#include "random.h"
#include "distribution.h"
#include "simulator.h"
#include "seeds_table.h"

#ifdef _MEM_DEBUG
#include "mem_debug.h"
#endif

t_obj_desc       obj_desc_tb[MAX_TABLE_ENTRIES];
int              num_objs;
t_global_rew     global_rew_tb[MAX_TABLE_ENTRIES];
int              glob_rew_entries;
t_indep_chain    indep_chain_tb[MAX_TABLE_ENTRIES];
int              indep_chain_entries;
char             nowhere = '\n';
struct timeval   begin_time;
extern int       CAN_GENERATE_CHAIN,
                 CAN_SIMULATE;

System_Description    *the_system_desc;

/* Random Number Generators used in the simulator */
Random_obj            *events_random_obj,      /* used for interevent times  */
                      *action_random_obj,      /* used for selecting actions */
                      *gr_function_random_obj; /* used by get_random()       */

Known_State_List      *markov_chain;
Simulator_Controler   *the_simulator_controler;
Simulator_Interactive *the_simulator_interactive;
System_State          *first_system_state;
Hash                  *hashtb;
t_context              context;

#ifdef TG_PARALLEL
int                     myTaskId;
int                     *tids;
int                     ntids;
int                     myTaskNumber;
#endif

int    MaxNumberStates;
int    Debug_Level = 1;
int    Rew_Files = NO_TRACE;
char   base_filename[9192+MAXSTRING];
int    Generate_Chain;
int    Simulate_Model;
int    Simulate_Interactive;
int    Simulate_Batch;
TGFLOAT Simulate_Max_Time;
int    Simulate_Max_Trans;
int    Simulate_Max_Runs;
char   Simulate_Stopping_Event[MAXSTRING];
char   Simulate_Stopping_State[MAXSTRING];
int    Simulate_Rare_Event = FALSE;
int    Simulate_Stopping_St_Low = 0;
int    Simulate_Stopping_St_Upp = 0;
int    Simulate_Confidence_Interval = 1;
char   Confidence_Interval[4];
int    Rate_Rew_Option = RATE_ERROR;
int    reward_to_print=PRINT_REWARDS_CR_IR;
//ouput_file_name is necessary for batch simulation!!
char   output_file_name[MAXSTRING];
FILE  *output_file=NULL;

//seed parameters to be used for simulation
int            Simulate_Use_Default_Seed_For_Events;
unsigned short Simulate_Events_Seed[3];
int            Simulate_Use_Default_Seed_For_Actions;
unsigned short Simulate_Actions_Seed[3];
int            Simulate_Use_Default_Seed_For_GR_Function;
unsigned short Simulate_GR_Function_Seed[3];

//------------------------------------------------------------------------------
// Warnings and error messages are redirected to stderr.
// Informative messages are redirected to stdout. (some of them has a prefix
// to indicate that the interface must capture it. )
// Batch simulation results are written in output file defined by arg output_file_name.
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//     F U N C T I O N S
//------------------------------------------------------------------------------
void usage( char *myname )
{
    fprintf( stderr, "Usage:\n"
                     "  %s -f base_filename [-d debug_level] [ options ]\n", myname );
    fprintf( stderr, "And one of:\n"
                     "    -G    [ analytical options ]             (for analytical generation)\n"
                     "    -S -I [ interactive simulation options ] (for interactive simulation)\n"
                     "    -S -B [ batch simulation options ]       (for batch simulation)\n"
                     "\n"
                     "  Options for Analytical Generation:\n"
                     "    -m <n>         set maximum number of states for the model\n"
                     "\n"
                     "  General Options for Simulation:\n"
                     "    -i <n>         seed value for interevent time distributions\n"
                     "    -a <n>         seed value for probabilistic action codes\n"
                     "    -g <n>         seed value for get_random()\n"
                     "                   (the default seed values for each of the previous three\n"
                     "                    is the time taken from the system clock).\n"
                     "\n"
                     "  Options for Interactive Simulation:\n"
                     "    [ there are no specific options for interactive simulation\n"
                     "      besides the general simulation options ]\n"
                     "\n"
                     "  Options for Batch Simulation:\n"
                     "    -o <name>      output file for simulation results\n"
                     "    -t <n>         stop at a given simulation time limit\n"
                     "    -r <n>         stop after a given number of transitions\n"
                     "      -e <obj.ev>  specific event to count the transitions with -r (optional)\n"
                     "    -s <obj.st>    stop when a state variable falls inside a given interval\n"
                     "      -l <n>       lower limit for the interval to be used with -s\n"
                     "      -p <n>       upper limit for the interval to be used with -s\n"
                     "    -u <n>         number of simulation runs to perform\n"
                     "    -k <n>         type of reward to print (1 - IR, 2 - CR, default is both)\n"
                     "    -c <n>         confidence interval (0 - 90%% , 1 - 95%% , 2 - 99%% , default is 1)\n"
                     "    -w <n>         rate reward option [0-4]\n"
                     "    -y             generate reward traces\n"
                     "    -R             perform rare event simulation with RESTART\n" );

    exit( 1 );
}
//------------------------------------------------------------------------------
void CrashHandler( int sig )
{
    fprintf( stderr, "------------------------------------------------------------------------------\n"
                     "\n"
                     "        Internal error in mark_find\n"
                     "\n"
                     "        Signal %d received.\n"
                     "        Please send the model that generated this error along with\n"
                     "        any relevant information to the Tangram-II support team at\n"
                     "        support@land.ufrj.br\n"
                     "\n"
                     "------------------------------------------------------------------------------\n", sig );

    /* Generate core dump */
    signal( SIGSEGV, SIG_DFL );
}
//------------------------------------------------------------------------------
int parse_seed( char * arg, unsigned short seed[3] )
{
    int status, i;
    char * pt, * endpt;

    status = FALSE;

    if( arg || strlen(arg) != 12 )
    {
        for( i = 2; i >= 0; i-- )
        {
            pt = arg+(4*i);
            endpt = pt;
            seed[i] = strtol( pt, &endpt, 16 );
            *pt = 0;

            if( endpt == pt )
                break;
        }

        if( i < 0 )
            status = TRUE;
    }

    return status;
}
//------------------------------------------------------------------------------
int parse_args(int argc, char **argv)
{
    int count_mandatory_args;
    int ch;

    MaxNumberStates      = 0;
    count_mandatory_args = 0;
    Simulate_Max_Runs    = 1;
    Simulate_Use_Default_Seed_For_Events      = TRUE;
    Simulate_Use_Default_Seed_For_Actions     = TRUE;
    Simulate_Use_Default_Seed_For_GR_Function = TRUE;
    strcpy( Simulate_Stopping_Event, "" );

    while ((ch = getopt(argc, argv, "f:m:o:d:SGIBt:k:r:w:e:u:s:l:p:c:Ryi:a:g:")) != -1)
    {
        switch(ch)
        {
            case 'f':
                if (optarg == NULL)
                {
                  cerr << "Missing base filename.\n";
                  usage(argv[0]);
                }
                strcpy(base_filename,optarg);
                count_mandatory_args++;
                break;
            case 'm':
                MaxNumberStates = atoi(optarg);
                break;
            case 'd':
                Debug_Level = atoi(optarg);
                break;
            case 'k':
                reward_to_print = atoi(optarg);
                break;
            case 'o':
                if ( (strcmp ( base_filename,"") != 0 ) )
                {
#ifdef TG_PARALLEL
                    sprintf( output_file_name, "%s.Task_%d.%s", base_filename, myTaskNumber, optarg );
#else
                    sprintf( output_file_name, "%s.%s", base_filename, optarg ) ;
#endif
                }
                else
                {
                    cerr << "Base filename must be specified before the output file name.\n";
                    usage(argv[0]);
                }
                break;
            case 'y':
                Rew_Files = TRACE;
                break;
            case 'w':
                Rate_Rew_Option = atoi(optarg);
                if (Rate_Rew_Option < 0 || Rate_Rew_Option > 4)
                {
                    fprintf( stderr,"Rate reward option must be one of the following:\n");
                    fprintf( stderr,"\t1 - Mean value\n");
                    fprintf( stderr,"\t2 - Max value\n");
                    fprintf( stderr,"\t3 - Min value\n");
                    fprintf( stderr,"\t4 - Generate error (default)\n");
                    usage(argv[0]);
                }
                break;
            case 'G':
                Generate_Chain = TRUE;
                count_mandatory_args++;
                break;
            case 'S':
                Simulate_Model = TRUE;
                count_mandatory_args++;
                break;
            case 'I':
                Simulate_Interactive = TRUE;
                break;
            case 'B':
                Simulate_Batch = TRUE;
                break;
            case 't':
                Simulate_Max_Time = atof(optarg);
                break;
            case 'r':
                Simulate_Max_Trans = atoi(optarg);
                break;
            case 'u':
                Simulate_Max_Runs = atoi(optarg);
                break;
            case 'e':
                if (optarg != NULL)
                {
                    if (!strcmp(optarg, "ANY"))
                        strcpy (Simulate_Stopping_Event, "");
                    else
                        strcpy (Simulate_Stopping_Event, optarg);
                }
                else
                {
                    cerr << "Missing obj.event name. Use the word ";
                    cerr << "ANY to specify any stopping event\n";
                    usage(argv[0]);
                }
                break;
            case 's':
                if (optarg != NULL)
                {
                    strcpy (Simulate_Stopping_State, optarg);
                }
                else
                {
                    cerr << "Missing obj.st_var name \n";
                    usage(argv[0]);
                }
                break;
            case 'R':
                Simulate_Rare_Event = TRUE;
                break;
            case 'l':
                Simulate_Stopping_St_Low = atoi(optarg);
                break;
            case 'p':
                Simulate_Stopping_St_Upp = atoi(optarg);
                break;
            case 'c':
                Simulate_Confidence_Interval = atoi(optarg);
                if ( Simulate_Confidence_Interval == 0 )
                {
                    strcpy(Confidence_Interval,"90%");
                }
                else if (Simulate_Confidence_Interval == 2)
                {
                    strcpy(Confidence_Interval,"99%");
                }
                break;
            case 'i':
                Simulate_Use_Default_Seed_For_Events = FALSE;
                if( !parse_seed(optarg, Simulate_Events_Seed) )
                {
                    cerr << "Invalid seed passed with -i" << endl;
                    usage( argv[0] );
                }
                break;
            case 'a':
                Simulate_Use_Default_Seed_For_Actions = FALSE;
                if( !parse_seed(optarg, Simulate_Actions_Seed) )
                {
                    cerr << "Invalid seed passed with -a" << endl;
                    usage( argv[0] );
                }
                break;
            case 'g':
                Simulate_Use_Default_Seed_For_GR_Function = FALSE;
                if( !parse_seed(optarg, Simulate_GR_Function_Seed) )
                {
                    cerr << "Invalid seed passed with -g" << endl;
                    usage( argv[0] );
                }
                break;
            default:
                usage(argv[0]);
                break;
        }
    }

    if( count_mandatory_args < 2 )
    {
        cerr << "Few numbers of mandatory arguments.\n";
        usage( argv[ 0 ] );
    }
    if ((! Simulate_Model) && (! Generate_Chain))
    {
        cerr << "Neither generation nor simulation was specified, using generation.\n";
        Generate_Chain = TRUE;
    }

    if ((Simulate_Model) && (Generate_Chain))
    {
        fprintf( stderr,"Error: cannot simulate and generate markov chain at the same time.\n");
        usage(argv[0]);
    }

    if ((Simulate_Model) && (! Generate_Chain))
    {
        if ((! Simulate_Batch) && (! Simulate_Interactive))
        {
            cerr << "Neither batch nor interaction was specified, using interaction.\n";
            Simulate_Interactive = TRUE;
        }
        if ((Simulate_Batch) && (Simulate_Interactive))
        {
            fprintf( stderr, "Error: cannot be interactive and batch at the same time.\n");
            usage(argv[0]);
        }
        if (Simulate_Interactive)
        {
            if ( (output_file = fopen( output_file_name, "w+" )) == NULL )
            {
                cerr << "Error opening output file.\n";
                exit(0);
            }
        }
        if (Simulate_Batch)
        {
            if ( (output_file = fopen( output_file_name, "w+" )) == NULL )
            {
                cerr << "Error opening output file.\n";
                exit(0);
            }

            if ((! Simulate_Max_Time) && (! Simulate_Max_Trans))
            {
                cerr << "In batch mode you must specify one of r or t.\n";
                usage(argv[0]);
            }
        }
    }

    if ( strcmp ( base_filename,"") == 0 )
    {
        cerr << "Missing base filename.\n";
        usage(argv[0]);
    }

    return (1);
}
//------------------------------------------------------------------------------
#define SIGNATURE 2147483647

int read_tables()
{
    char  filename[ MAXSTRING ];
    char  version[ 20 ];
    FILE *fd;
    int   i, j;
    int   size;
    bool  old_version;

    size = 0;
    sprintf( filename, "%s.tables_dump", base_filename );
    if( (fd = fopen (filename, "r")) == NULL )
    {
        fprintf( stderr, "Error while opening the file: %s\n", filename );
        perror( "fopen" );
        return( -1 );
    }

    /* read the number of objects to dump */
    fread( (void *)&num_objs, sizeof( int ), 1, fd );

    /* Check for signature. */
    old_version = true;
    if( num_objs == -1 )
    {
        fread( (void *)&version, 12, 1, fd );
        if( strncmp( version, "Version 3.0", 11 ) != 0 )
        {
            fprintf( stderr, "Invalid or incompatible tables_dump file. Please generate it again.\n" );
            exit( -1 );
        }
        old_version = false;
        /* read the number of objects to dump */
        fread( (void *)&num_objs, sizeof( int ), 1, fd );
    }

    /* go through all objects and dump their tables */
    for( i = 0; i < num_objs; i++ )
    {
        /* read the name of the object */
        fread( (void *)&obj_desc_tb[ i ].name, MAX_NAME * sizeof(char), 1, fd );

        /* read the number of entries in code table */
        fread( (void *)&obj_desc_tb[i].code_entries, sizeof(int), 1, fd );
        /* alloc space for the code table */
        obj_desc_tb[i].code_tb = (t_code *)malloc( sizeof(t_code) *
                                            (obj_desc_tb[i].code_entries + 1) );
        /* go through all the entries in the code table */
        for( j = 0; j < obj_desc_tb[i].code_entries; j++ )
        {
            fread( (void *)&obj_desc_tb[i].code_tb[j], sizeof(t_code), 1, fd);
            obj_desc_tb[i].code_tb[j].code = &nowhere;
        }

        /* read the number of entries in symbol table */
        fread ( (void *)&obj_desc_tb[i].sym_entries, sizeof(int), 1, fd);
        /* alloc space for the symbol table */
        obj_desc_tb[i].sym_tb = (t_symbol_obj *) malloc( obj_desc_tb[ i ].sym_entries * sizeof(t_symbol_obj));

        /* go through all the entries in the symbol table */
        for( j = 0; j < obj_desc_tb[ i ].sym_entries; j++ )
        {
            fread( (void *)&obj_desc_tb[i].sym_tb[j], sizeof(t_symbol_obj), 1, fd );

            /* The first tangram versions had a fixed symbol table size, */
            /* but it had to be changed to allow unlimited vector size.  */
            if( !old_version )
            {
                size = obj_desc_tb[ i ].sym_tb[ j ].dimension;
                switch( obj_desc_tb[ i ].sym_tb[ j ].type )
                {
                    case STATE_VAR_TYPE:
                        obj_desc_tb[ i ].sym_tb[ j ].val.int_vec = (int *)new int[ size ];
                        fread( (void *)obj_desc_tb[ i ].sym_tb[ j ].val.int_vec, size * sizeof( int ), 1, fd );
                        break;

                    case STATE_VAR_FLOAT_TYPE:
                        obj_desc_tb[ i ].sym_tb[ j ].val.tgf_vec = (TGFLOAT *)new TGFLOAT[ size ];
                        fread( (void *)obj_desc_tb[ i ].sym_tb[ j ].val.int_vec, size * sizeof( TGFLOAT ), 1, fd );
                        break;
                }
            }
        }
    }

    /* read the number of global rewards table entries */
    fread( (void *)&glob_rew_entries, sizeof(int), 1, fd );
    /* go through all the entries in the symbol table */
    for( i = 0; i < glob_rew_entries; i++ )
        fread( (void *)&global_rew_tb[i], sizeof(t_global_rew), 1, fd );

    /* read the number of independent chains table entries */
    fread( (void *)&indep_chain_entries, sizeof(int), 1, fd );
    /* go through all the entries in the indep chains table */
    for( i = 0; i < indep_chain_entries; i++ )
        fread( (void *)&indep_chain_tb[i], sizeof(t_indep_chain), 1, fd );

    fclose( fd );

    return( 1 );
}
//------------------------------------------------------------------------------
Distribution *create_distribution(int i, int j, char *file_name)
{
    Exponential_Distrib        *exp_dist;
    Deterministic_Distrib      *det_dist;
    Uniform_Distrib            *uni_dist;
    Gaussian_Distrib           *gauss_dist;
    FBM_Distrib                *fbm_dist;
    FARIMA_Distrib             *farima_dist;
    Erlang_m_Distrib           *erl_dist;
    Lognormal_Distrib          *log_norm_dist;
    TruncLognormal_Distrib     *trunc_log_norm_dist;
    Weibull_Distrib            *weibull_dist;
    Pareto_Distrib             *pareto_dist;
    Trunc_Pareto_Distrib       *trunc_pareto_dist;
    File_Distrib               *file_dist;
    Rew_Reach_Distrib          *rew_reach_dist;
    Init_Distrib               *init_dist;

    Expression                 *expr_aux;

    switch (obj_desc_tb[i].code_tb[j].distrib_type)
    {
        case EXPONENTIAL_DIST :
            /* create the distribution */
            exp_dist = new Exponential_Distrib(events_random_obj);
            /* create the expression for the rate value */
            expr_aux = new Expression( obj_desc_tb[i].code_tb[j].distrib.exp.func_hand,
                                       obj_desc_tb[i].code_tb[j].expression           ,
                                       obj_desc_tb[i].code_tb[j].expr_type            );
            exp_dist->ch_rate(expr_aux);
            return ( (Distribution *)exp_dist);
            break;

        case DETERMINISTIC_DIST :
            /* create the distribution */
            det_dist = new Deterministic_Distrib(events_random_obj);
            /* create the expression for the rate value*/
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.det.func_hand);
            det_dist->ch_rate(expr_aux);
            return ( (Distribution *)det_dist);
            break;

        case UNIFORM_DIST :
            /* create the distribution */
            uni_dist = new Uniform_Distrib(events_random_obj);
            /* create the expression for lower and upper values */
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.uni.func_hand_low);
            uni_dist->ch_lower(expr_aux);
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.uni.func_hand_upp);
            uni_dist->ch_upper(expr_aux);
            return ( (Distribution *)uni_dist);
            break;

        case GAUSSIAN_DIST :
            /* create the distribution */
            gauss_dist = new Gaussian_Distrib(events_random_obj);
            /* create the expression for mean and variance values */
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.gauss.func_hand_mean);
            gauss_dist->ch_mean(expr_aux);
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.gauss.func_hand_var);
            gauss_dist->ch_variance(expr_aux);
            return ( (Distribution *)gauss_dist);
            break;

        case FBM_DIST :
            /* create the distribution */
            fbm_dist = new FBM_Distrib(events_random_obj);
            /* set the parameters */
            if ( fbm_dist->ch_param(file_name,
                                    obj_desc_tb[i].code_tb[j].distrib.fbm.max_level,
                                    obj_desc_tb[i].code_tb[j].distrib.fbm.mean,
                                    obj_desc_tb[i].code_tb[j].distrib.fbm.var,
                                    obj_desc_tb[i].code_tb[j].distrib.fbm.hurst,
                                    obj_desc_tb[i].code_tb[j].distrib.fbm.time_scale  ) < 0)
                fprintf( stderr, "ERROR: Invalid parameters for FBM distribution\n");
            return ( (Distribution *)fbm_dist);
            break;

        case FARIMA_DIST :
            /* create the distribution */
            farima_dist = new FARIMA_Distrib(events_random_obj);
            /* set the parameters */
            if (farima_dist->ch_param(file_name,
                                      obj_desc_tb[i].code_tb[j].distrib.farima.no_samples,
                                      obj_desc_tb[i].code_tb[j].distrib.farima.mean,
                                      obj_desc_tb[i].code_tb[j].distrib.farima.var,
                                      obj_desc_tb[i].code_tb[j].distrib.farima.hurst,
                                      obj_desc_tb[i].code_tb[j].distrib.farima.time_scale) < 0)
                fprintf( stderr, "ERROR: Invalid parameters for FARIMA distribution\n");
            return ( (Distribution *)farima_dist);
            break;

        case ERLANG_M_DIST :
            /* create the distribution */
            erl_dist = new Erlang_m_Distrib(events_random_obj);
            /* set the parameters */
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.erl.func_hand_rate);
            erl_dist->ch_rate(expr_aux);
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.erl.func_hand_stages);
            erl_dist->ch_stages(expr_aux);
            return ( (Distribution *)erl_dist);
            break;

        case LOGNORMAL_DIST :
            /* create the distribution */
            log_norm_dist = new Lognormal_Distrib(events_random_obj);
            /* set the parameters */
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.log_norm.func_hand_mean);
            log_norm_dist->ch_mean(expr_aux);
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.log_norm.func_hand_var);
            log_norm_dist->ch_variance(expr_aux);
            return ( (Distribution *)log_norm_dist);
            break;

        case TRUNCLOGNORMAL_DIST :
            /* create the distribution */
            trunc_log_norm_dist = new TruncLognormal_Distrib(events_random_obj);
            /* set the parameters */
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.trunc_log_norm.func_hand_mean);
            trunc_log_norm_dist->ch_mean(expr_aux);
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.trunc_log_norm.func_hand_var);
            trunc_log_norm_dist->ch_variance(expr_aux);
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.trunc_log_norm.func_hand_minimum);
            trunc_log_norm_dist->ch_minimum(expr_aux);
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.trunc_log_norm.func_hand_maximum);
            trunc_log_norm_dist->ch_maximum(expr_aux);
            return ( (Distribution *)trunc_log_norm_dist);
            break;

        case WEIBULL_DIST :
            /* create the distribution */
            weibull_dist = new Weibull_Distrib(events_random_obj);
            /* set the parameters */
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.weib.func_hand_scale);
            weibull_dist->ch_scale(expr_aux);
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.weib.func_hand_shape);
            weibull_dist->ch_shape(expr_aux);
            return ( (Distribution *)weibull_dist);
            break;

        case PARETO_DIST :
            /* create the distribution */
            pareto_dist = new Pareto_Distrib(events_random_obj);
            /* set the parameters */
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.par.func_hand_scale);
            pareto_dist->ch_scale(expr_aux);
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.par.func_hand_shape);
            pareto_dist->ch_shape(expr_aux);
            return ( (Distribution *)pareto_dist);
            break;

        case TRUNC_PARETO_DIST :
            /* create the distribution */
            trunc_pareto_dist = new Trunc_Pareto_Distrib(events_random_obj);
            /* set the parameters */
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.trunc_par.func_hand_scale);
            trunc_pareto_dist->ch_scale(expr_aux);
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.trunc_par.func_hand_shape);
            trunc_pareto_dist->ch_shape(expr_aux);
            expr_aux = new Expression(obj_desc_tb[i].code_tb[j].distrib.trunc_par.func_hand_maximum);
            trunc_pareto_dist->ch_maximum(expr_aux);
            return ( (Distribution *)trunc_pareto_dist);
            break;

        case FILE_DIST :
            /* create the distribution */
            file_dist = new File_Distrib(events_random_obj);
            /* sets the filename */
            file_dist->ch_file(obj_desc_tb[i].code_tb[j].distrib.file.filename,0);
            return ( (Distribution *)file_dist);
            break;

        case REW_REACH_DIST :
            /* create the distribution */
            rew_reach_dist = new Rew_Reach_Distrib(events_random_obj);
            /* set the parameters */
            return ( (Distribution *)rew_reach_dist);
            break;

        case INIT_DIST :
            /* create the distribution */
            init_dist = new Init_Distrib( events_random_obj );
            /* set the parameters */
            return ( (Distribution *)init_dist);
            break;

        default:
            fprintf( stderr, "Invalid distribution type %d", obj_desc_tb[i].code_tb[j].distrib_type );
    }

    return( NULL );
}
//------------------------------------------------------------------------------
void create_global_rew_desc(System_Description *sys_desc, int rate_desc_id)
{
    Rate_Reward_Desc *rate_rew_aux;
    Expression       *cond_aux, *value_aux;
    char rew_name[MAXSTRING];
    int i = 0;

    while (i < glob_rew_entries)
    {
        /* copy the name of the reward */
        strcpy (rew_name, global_rew_tb[i].name);

        /* create a new reward */
        rate_rew_aux = new Rate_Reward_Desc (rew_name, rate_desc_id);
        rate_desc_id++;
        i++;

        /* read the reward level from a file */
        rate_rew_aux->read_rew_level("GlobalReward");

        /* set the bounds */
        if (global_rew_tb[i].low_bounded)
        {
            rate_rew_aux->ch_low_bounded(TRUE);
            rate_rew_aux->ch_low_bound(global_rew_tb[i].low_bound);
        }
        if (global_rew_tb[i].upp_bounded)
        {
            rate_rew_aux->ch_upp_bounded(TRUE);
            rate_rew_aux->ch_upp_bound(global_rew_tb[i].upp_bound);
        }
        i++;

        /* get all rewards cond/value for this reward */
        while (!strcmp(rew_name, global_rew_tb[i].name))
        {
            /* create the condition for this reward */
            cond_aux = new Expression(global_rew_tb[i].func_hand);
            rate_rew_aux->add_cond(cond_aux);
            i++;

            /* create the expression for this reward */
            value_aux = new Expression(global_rew_tb[i].func_hand);
            rate_rew_aux->add_rate(value_aux);
            i++;
        }

        /* add this reward to the system description */
        sys_desc->add_global_rew_desc(rate_rew_aux);
    }

}
//------------------------------------------------------------------------------
void create_indep_chain_desc( System_Description *sys_desc )
{
    Object_Description *obj_desc;
    Event              *ev;
    Indep_Chain        *indep_chain;
    int                 i = 0;
    char                ev_name[ MAXSTRING ];

    /* go through all entries in the indep chain table */
    while( i < indep_chain_entries )
    {
        /* get the name of the DET event */
        strcpy( ev_name, indep_chain_tb[ i ].name );
        i++;
        /* find the corresponding event */
        ev = sys_desc->show_ev_by_name( ev_name );
        /* go through all indep chains of this DET event */
        while( indep_chain_tb[ i ].type == CHAIN_TYPE )
        {
            /* create a new indep chain */
            indep_chain = new Indep_Chain();
            i++;
            /* add all objects to this indep chain */
            while( indep_chain_tb[ i ].type == OBJECT_TYPE )
            {
                /* get the object id of the object name */
                obj_desc = sys_desc->show_obj_desc( indep_chain_tb[ i ].name );
                if( indep_chain->add_obj_id( obj_desc->show_id() ) < 0 )
                {
                    fprintf( stderr,"\twhile creating the indep chain for object/event: %s\n", ev_name);
                    exit( -1 );
                }
                i++;
            }
            /* initialize the indep chain (hash table) */
            if( indep_chain->init( sys_desc ) < 0 )
            {
                fprintf( stderr, "ERROR: Cannot initialize the indepedent chain\n");
                exit (-1);
            }
            /* add the indep_chain to the event */
            ev->add_indep_chain( indep_chain );
        }
    }
}
//------------------------------------------------------------------------------
void make_rate_reward_sum_consistent( System_Description *sys_desc_aux )
{
    Object_Description_List *obj_desc_list;
    Object_Description      *obj_desc;
    Rate_Reward_Sum_List    *rate_rew_sum_list;
    Rate_Reward_Sum         *rate_rew_sum;
    Rate_Reward_Desc_List   *rate_rew_desc_list;
    Rate_Reward_Desc        *rate_rew_desc;
    Reward_Sum_List         *rew_sum_list;
    Reward_Sum              *rew_sum;
    char                     rew_name[ MAXSTRING ];

    /* go through all obj desc of the system desc */
    obj_desc_list = sys_desc_aux->show_obj_desc_list( LIST_RO );
    obj_desc = obj_desc_list->show_1st_obj_desc();
    /* go through all the objects in the system */
    while( obj_desc != NULL )
    {
        rate_rew_sum_list = obj_desc->show_rate_rew_sum_list( LIST_RW );
        rate_rew_sum = rate_rew_sum_list->show_1st_rew();
        /* go through all the rate_reward_sum of the object */
        while( rate_rew_sum != NULL )
        {
            rew_sum_list = rate_rew_sum->show_rew_sum_list( LIST_RW );
            rew_sum = rew_sum_list->show_1st_rew();
            /* go through all the reward_sum of the rate_reward_sum */
            while( rew_sum != NULL )
            {
                rate_rew_desc_list = obj_desc->show_reward_list( LIST_RW );
                rew_sum->show_rew_name( rew_name );
                if( rate_rew_desc_list->query_rew( rew_name ) > 0 )
                {
                    /* updates both rate_rew_desc and rew_sum to became consistent */
                    rate_rew_desc = rate_rew_desc_list->show_curr_rew();
                    rate_rew_desc->ch_sum_bounded( TRUE );
                    rate_rew_desc->ch_sum_bound( rate_rew_sum );
                    rew_sum->ch_rew_id( rate_rew_desc->show_id() );
                    debug(3,"make_rate_reward_sum_consistent: found %s\n",rew_name);
                }
                else
                    fprintf( stderr, "Internal Error! make_rate_reward_sum_consistent: reward %s not found.", rew_name );
                rew_sum = rew_sum_list->show_next_rew();
            }
            rate_rew_sum = rate_rew_sum_list->show_next_rew();
        }
        obj_desc = obj_desc_list->show_next_obj_desc();
    }
    delete obj_desc_list;
}
//------------------------------------------------------------------------------
System_Description *create_system_desc()
{
    System_Description    *sys_desc_aux;
    Object_Description    *obj_desc_aux;
    Event                 *ev_aux;
    Event_List            *ev_list_aux;
    Distribution          *distrib_aux;
    Rate_Reward_Desc      *rate_rew_aux;
    Imp_Reward_Desc       *imp_rew_aux;
    Rate_Reward_Sum       *rate_rew_sum_aux;
    Reward_Sum            *rew_sum_aux;
    Expression            *value_aux;
    Expression            *cond_aux;
    Expression            *prob_aux;
    Action_List           *act_list_aux;
    Action                *act_aux;
    Message_List          *msg_list_aux;
    Message               *msg_aux;
    Symbol                *sym_aux;
    t_value                sym_val;
    char                   sym_char_code = 'a';
    char                   curr_obj_name[MAXSTRING];
    char                   msg_port[MAXSTRING];
    char                   rew_name[MAXSTRING];
    char                   ev_name[MAXSTRING];
    char                   act_list[MAXSTRING];
    char                   str[MAXSTRING];
    char                  *act_num_str;
    int                    act_number;
    int                    rate_desc_id = 0;
    int                    imp_desc_id = 0;
    int                    rate_rew_sum_id = 0;
    int                    count_det_ev = 0;
    int                    low_bounded, upp_bounded;
    TGFLOAT                low_bound, upp_bound;
    TGFLOAT                cr_init_val = 0;

    int i, j, k;

    /* create the System Description */
    sys_desc_aux = new System_Description();

    /* the rand() generator is not used by simulation, but for
       choosing random seeds in the seed table */
    srand( time( NULL ) );

    /* Mark all seeds as unused */
    memset( used_seeds, 0, sizeof(used_seeds) );

    /* create and initialize random number generators */
    if( Simulate_Use_Default_Seed_For_Events )
        events_random_obj = new Random_obj();
    else
        events_random_obj = new Random_obj( Simulate_Events_Seed );

    if( Simulate_Use_Default_Seed_For_Actions )
        action_random_obj = new Random_obj();
    else
        action_random_obj = new Random_obj( Simulate_Actions_Seed );

    if( Simulate_Use_Default_Seed_For_GR_Function )
        gr_function_random_obj = new Random_obj();
    else
        gr_function_random_obj = new Random_obj( Simulate_GR_Function_Seed );
    
    /* create the objects */
    for( i = 0; i < num_objs; i++ )
    {
        /* create one object */
        strcpy( curr_obj_name, obj_desc_tb[ i ].name );
        obj_desc_aux = new Object_Description( curr_obj_name );

        /* go through all symbols of this object */
        for( j = 0; j < obj_desc_tb[ i ].sym_entries; j++ )
        {
            /* create one symbol */
            sym_aux = new Symbol( obj_desc_tb[i].sym_tb[j].name,
                                  obj_desc_tb[i].sym_tb[j].type,
                                  obj_desc_tb[i].sym_tb[j].val,
                                  obj_desc_tb[i].sym_tb[j].max_value,
                                  obj_desc_tb[i].sym_tb[j].dimension );

            /* set the code for this symbol */
            sym_aux->ch_code( j + 1 );

            /* set the char code for symbol if its a parameter */
            if( obj_desc_tb[i].sym_tb[ j ].type == INT_PARAM_TYPE ||
                obj_desc_tb[i].sym_tb[ j ].type == FLOAT_PARAM_TYPE )
            {
                sym_aux->ch_char_code( sym_char_code );
                sym_char_code++;

                /* set in the system description to LITERAL type */
                sys_desc_aux->set_chain_type( LITERAL );
            }

            /* add this symbol to this onject */
            obj_desc_aux->add_symbol(sym_aux);
        }

        /* go through all events, messages and rewards of this object */
        j = 0;
        while( j < obj_desc_tb[ i ].code_entries )
        {
            /* if the table entry is an event */
            if( obj_desc_tb[ i ].code_tb[ j ].part == EVENTS_PART )
            {
                /* create one event */
                ev_aux = new Event( obj_desc_tb[ i ].code_tb[ j ].name );

                /* compose the name of the file for the distribution */
                sprintf( str, "%s.%s.%s", base_filename, curr_obj_name,
                         obj_desc_tb[ i ].code_tb[ j ].name);

                /* create the distribution for this event */
                distrib_aux = create_distribution( i, j, str );

                /* update the event id (zero if exponential) */
                if( distrib_aux->show_type() == DETERMINISTIC_DIST )
                {
                    count_det_ev++;
                    ev_aux->ch_det_id( count_det_ev );
                    sys_desc_aux->set_det_model( TRUE );
                }
                else
                    ev_aux->ch_det_id( 0 );

                /* include the distribution in the event */
                ev_aux->ch_distrib( distrib_aux );
                j++;

                /* create the condition for this event */
                cond_aux = new Expression( obj_desc_tb[i].code_tb[j].func_hand);
                ev_aux->ch_cond( cond_aux );
                j++;

                /* go through all actions of this event */
                while( obj_desc_tb[ i ].code_tb[ j ].type == ACT_TYPE )
                {
                    /* create one action */
                    act_aux = new Action(obj_desc_tb[i].code_tb[j].func_hand, NULL);
                    j++;

                    /* create a prob associated with the action above */
                    if( obj_desc_tb[ i ].code_tb[ j ].type == PROB_TYPE )
                    {
                        prob_aux = new Expression(obj_desc_tb[i].code_tb[j].func_hand,
                        obj_desc_tb[i].code_tb[j].expression,
                        obj_desc_tb[i].code_tb[j].expr_type);
                        j++;

                        act_aux->ch_prob( prob_aux );
                    }

                    ev_aux->add_action(act_aux);
                }
                /* add this event to the object */
                obj_desc_aux->add_event(ev_aux);
            }

            /* if the table entry is a message */
            if( obj_desc_tb[ i ].code_tb[ j ].part == MESSAGES_PART )
            {
                /* get the value of the symbols msg_port */
                sym_aux = obj_desc_aux->show_symbol(obj_desc_tb[i].code_tb[j].msg_port);
                sym_aux->show_value( &sym_val );
                strcpy( msg_port, sym_val.str );
                j++;

                /* create one message rec */
                msg_aux = new Message(msg_port);

                /* go through all actions in this message */
                while (obj_desc_tb[i].code_tb[j].type == ACT_TYPE)
                {
                    /* create one action */
                    act_aux = new Action(obj_desc_tb[i].code_tb[j].func_hand, NULL);
                    j++;

                    /* create a prob associated with the action above */
                    if (obj_desc_tb[i].code_tb[j].type == PROB_TYPE)
                    {
                        prob_aux = new Expression(obj_desc_tb[i].code_tb[j].func_hand,
                        obj_desc_tb[i].code_tb[j].expression,
                        obj_desc_tb[i].code_tb[j].expr_type);
                        j++;

                        act_aux->ch_prob(prob_aux);
                    }

                    msg_aux->add_action(act_aux);
                }

                /* add this message to the object */
                obj_desc_aux->add_msg(msg_aux);

            }

            /* clear the bounds flags and values */
            low_bounded = upp_bounded = 0;
            low_bound   = upp_bound   = 0;

            /* if the table entry is a reward */
            while( obj_desc_tb[ i ].code_tb[ j ].part == REWARDS_PART )
            {
                /* get the bounds for this reward */
                if( obj_desc_tb[ i ].code_tb[ j ].type == BOUND_TYPE )
                {
                    low_bounded = obj_desc_tb[i].code_tb[j].low_bounded;
                    upp_bounded = obj_desc_tb[i].code_tb[j].upp_bounded;
                    low_bound   = obj_desc_tb[i].code_tb[j].low_bound;
                    upp_bound   = obj_desc_tb[i].code_tb[j].upp_bound;

                    j++;
                }

                /* get the cr initial value for this reward */
                /* be aware! the user could set the initial cr out of bounds */
                if( obj_desc_tb[i].code_tb[j].type == REWARD_CR_INIT_VAL_TYPE )
                {
                    cr_init_val = obj_desc_tb[i].code_tb[j].cr_init_val;
                    j++;
                }

                /* get the name of the reward */
                strcpy( rew_name, obj_desc_tb[ i ].code_tb[ j ].name );

                /* if the entry represents a rate reward sum */
                if( obj_desc_tb[i].code_tb[j].type == REWARD_4SUM_TYPE )
                {
                    /* create a new Rate_Reward_Sum object */
                    rate_rew_sum_aux = new Rate_Reward_Sum (rew_name, curr_obj_name, rate_rew_sum_id);
                    debug(3,"Rate_Reward_Sum id=%d name=%s\n",rate_rew_sum_aux->show_id(),rew_name);
                    rate_rew_sum_id++;

                    /* set the bounds */
                    if( low_bounded )
                    {
                        rate_rew_sum_aux->ch_low_bounded( TRUE );
                        rate_rew_sum_aux->ch_low_bound( low_bound );
                    }
                    if( upp_bounded )
                    {
                        rate_rew_sum_aux->ch_upp_bounded( TRUE );
                        rate_rew_sum_aux->ch_upp_bound( upp_bound );
                    }

                    /* get all rewards_4_sum for this reward_rate_sum */
                    while( !strcmp( rew_name, obj_desc_tb[i].code_tb[j].name ) )
                    {
                        /* create an object Reward_Sum for this Rate_Reward_Sum */
                        rew_sum_aux = new Reward_Sum(0,obj_desc_tb[i].code_tb[j].reward_4sum_name);
                        rate_rew_sum_aux->add_rew_sum( rew_sum_aux );
                        j++;
                    }

                    /* add this reward sum to the object description */
                    obj_desc_aux->add_rate_rew_sum( rate_rew_sum_aux );

                    /* go to the next reward entry */
                    continue;
                }

                /* if the reward is rate reward */
                if( obj_desc_tb[ i ].code_tb[ j ].type == COND_TYPE )
                {
                    /* rate rewards are defined in this system description */
                    sys_desc_aux->set_rate_reward( TRUE );

                    /* create a new reward */
                    rate_rew_aux = new Rate_Reward_Desc( rew_name, rate_desc_id );
                    rate_desc_id++;

                    /* read the reward level from a file */
                    rate_rew_aux->read_rew_level( curr_obj_name );

                    /* set the bounds */
                    if( low_bounded )
                    {
                        rate_rew_aux->ch_low_bounded( TRUE );
                        rate_rew_aux->ch_low_bound( low_bound );
                    }
                    if( upp_bounded )
                    {
                        rate_rew_aux->ch_upp_bounded( TRUE );
                        rate_rew_aux->ch_upp_bound( upp_bound );
                    }

                    /* set cr_initial_value */
                    /* be aware! the user could set the initial cr out of bounds */
                    rate_rew_aux->ch_cr_initial_value( cr_init_val );

                    /* get all rewards cond/value for this reward */
                    while (!strcmp(rew_name, obj_desc_tb[i].code_tb[j].name))
                    {
                        /* create the condition for this reward */
                        cond_aux = new Expression(obj_desc_tb[i].code_tb[j].func_hand);
                        rate_rew_aux->add_cond(cond_aux);
                        j++;

                        /* create the expression for this reward */
                        value_aux = new Expression(obj_desc_tb[i].code_tb[j].func_hand,
                        obj_desc_tb[i].code_tb[j].expression,
                        obj_desc_tb[i].code_tb[j].expr_type);
                        rate_rew_aux->add_rate(value_aux);
                        j++;
                    }

                    /* add this reward to the object description */
                    obj_desc_aux->add_reward(rate_rew_aux);

                    /* go to the next reward entry */
                    continue;
                }

                /* if the reward is impulse a reward defined in an event or message reception */
                if( obj_desc_tb[ i ].code_tb[ j ].type == REWARD_EVENT_TYPE ||
                    obj_desc_tb[ i ].code_tb[ j ].type == REWARD_MSG_TYPE )
                {
                    /* impulse rewards are defined in this system description */
                    sys_desc_aux->set_impulse_reward(TRUE);

                    /* update the id of this impulse reward */
                    imp_desc_id++;

                    /* go through all definitions of this reward */
                    while( !strcmp(rew_name, obj_desc_tb[i].code_tb[j].name) )
                    {
                        /* get the name of the event */
                        strcpy (ev_name, obj_desc_tb[i].code_tb[j].ev_name);
                        strcpy (act_list, obj_desc_tb[i].code_tb[j].act_list);
                        j++;

                        /* if the type if an Event, get the action list of this event */
                        act_list_aux = NULL;
                        if( obj_desc_tb[i].code_tb[j-1].type == REWARD_EVENT_TYPE)
                        {
                            /* get the corresponding event */
                            ev_list_aux = obj_desc_aux->show_event_list(LIST_RO);
                            if(ev_list_aux->query_event(ev_name))
                            {
                                ev_aux = ev_list_aux->show_curr_event();
                                /* get the action list of this event */
                                act_list_aux = ev_aux->show_action_list(LIST_RW);
                            }
                            else
                            {
                                fprintf( stderr, "ERROR: Impulse reward using wrong event.\n");
                                fprintf( stderr, "ERROR: no event %s defined.\n",ev_name);
                                return( NULL );
                            }
                            /* delete the list just used */
                            delete ev_list_aux;
                        }

                        /* if the type if a Message, get the action list of this message */
                        if( obj_desc_tb[i].code_tb[j-1].type == REWARD_MSG_TYPE )
                        {
                            /* get the msg_port value for this message */
                            sym_aux = obj_desc_aux->show_symbol( ev_name );
                            sym_aux->show_value( &sym_val );
                            strcpy( ev_name, sym_val.str );
                            /* get the corresponding message seraching by msg_port value */
                            msg_list_aux = obj_desc_aux->show_message_list( LIST_RO );
                            if( msg_list_aux->query_msg( ev_name ) )
                            {
                                msg_aux = msg_list_aux->show_curr_msg();
                                /* get the action list of this event */
                                act_list_aux = msg_aux->show_action_list(LIST_RW);
                            }
                            else
                            {
                                fprintf( stderr, "ERROR: Impulse reward using wrong message port.\n");
                                fprintf( stderr, "ERROR: no port %s defined.\n",ev_name);
                                return( NULL );
                            }
                            /* delete the list just used */
                            delete msg_list_aux;
                        }

                        /* get the first action number */
                        act_num_str = strtok(act_list, ",");
                        /* go through all action numbers in action list */
                        while( act_num_str != NULL )
                        {
                            /* convert the action number to integer */
                            act_number = atoi( act_num_str );

                            /* go to the correspondent action number */
                            act_aux = act_list_aux->show_1st_act();
                            for( k = 1; k < act_number; k++ )
                            {
                                act_aux = act_list_aux->show_next_act();
                                if( act_aux == NULL )
                                {
                                    fprintf( stderr, "ERROR: Impulse reward using wrong action number.\n");
                                    fprintf( stderr, "ERROR: no action %d defined.\n",act_number);
                                    return( NULL );
                                }
                            }

                            /* update the name of the reward */
                            sprintf( str, "%s.%s", curr_obj_name, rew_name );
                            /* create the impulse reward description */
                            imp_rew_aux = new Imp_Reward_Desc(str, imp_desc_id);
                            /* set this id/name in the imp_rew table */
                            sys_desc_aux->set_imp_rew_name( str, imp_desc_id );

                            /* set the bounds */
                            if( low_bounded )
                            {
                                imp_rew_aux->ch_low_bounded( TRUE );
                                imp_rew_aux->ch_low_bound( low_bound );
                            }
                            if( upp_bounded )
                            {
                                imp_rew_aux->ch_upp_bounded( TRUE );
                                imp_rew_aux->ch_upp_bound( upp_bound );
                            }

                            /* set cr_initial_value */
                            /* be aware! the user could set the initial cr out of bounds */
                            imp_rew_aux->ch_cr_initial_value( cr_init_val );

                            /* create the expression for this reward */
                            value_aux = new Expression(obj_desc_tb[i].code_tb[j].func_hand,
                                           obj_desc_tb[i].code_tb[j].expression,
                                           obj_desc_tb[i].code_tb[j].expr_type);
                            imp_rew_aux->ch_impulse( value_aux );
                            /* add this reward to the action */
                            act_aux->add_impulse( imp_rew_aux );

                            /* get the next act number string */
                            act_num_str = strtok( NULL, "," );
                        }

                        /* increment the code table entry */
                        j++;
                    }
                    /* go to the next reward entry */
                    continue;
                }
            }
        }

        /* add this object to the system description */
        sys_desc_aux->add_obj_desc( obj_desc_aux );
    }

    /* update the number of DET events in the model */
    sys_desc_aux->set_no_det_ev( count_det_ev );

    /* create the global reward description associated to the model */
    create_global_rew_desc( sys_desc_aux, rate_desc_id );
    debug( 3, "global reward description created.\n" );

    /* create the indep chain description associated to the model */
    create_indep_chain_desc( sys_desc_aux );
    debug( 3, "indep chain description created.\n" );

    /* At this point we have the Rate_Reward_Sum objects attached to it's object */
    /* but they aren't associate with they respective Rate_Reward_Desc and vice-versa.*/
    /* So we call make_rate_reward_sum_consistent function to fix it. */
    make_rate_reward_sum_consistent( sys_desc_aux );

    /* free memory - destroy obj_desc_tb[i].code_tb */
    for( i = 0; i < num_objs; i++ )
    {
        free( obj_desc_tb[ i ].code_tb );
        free( obj_desc_tb[ i ].sym_tb );
    }

    return( sys_desc_aux );
}
//------------------------------------------------------------------------------
System_State *create_system_state( System_Description *sys_desc )
{

    System_State             *sys_st_aux;
    Object_State             *obj_st_aux;
    State_Variable           *st_var_aux;
    Object_Description_List  *obj_desc_list;
    Object_Description       *obj_desc_aux;
    Symbol_List              *sym_list;
    Symbol                   *sym_aux;
    IntegerQueue             *intQueue;
    FloatQueue               *floatQueue;

    /* create the first system state */
    sys_st_aux = new System_State();

    /* get the first object */
    obj_desc_list = sys_desc->show_obj_desc_list( LIST_RO );
    obj_desc_aux  = obj_desc_list->show_1st_obj_desc();

    /* go through all objects in system description */
    while( obj_desc_aux != NULL )
    {
        /* create one object state */
        obj_st_aux = new Object_State( obj_desc_aux );

        /* get the first symbol of this object */
        sym_list = obj_desc_aux->show_symbol_list( LIST_RO );
        sym_aux = sym_list->show_1st_sym();

        /* go through all symbols of this object */
        while( sym_aux != NULL )
        {
            /* if this symbol is a state var ... */
            switch( sym_aux->show_type() )
            {
                case STATE_VAR_TYPE:
                    /* create one state var setting its value   */
                    /* set the corresponding code of the symbol */
                    /*   for this state var                     */
                    debug(2,"\n\n!!!!!!!!!!!!!!!!!!!! Int var type!!!!!!!!!!!!!!!!!!!!!!\n\n");
                    st_var_aux = new State_Variable( sym_aux->show_value(),
                                                     sym_aux->show_code(),
                                                     sym_aux->show_dimension() );
                    /* add this state var to the state var list of the object */
                    obj_st_aux->add_state_var( st_var_aux );
                    break;

                case STATE_VAR_FLOAT_TYPE:
                    debug(2,"\n\n!!!!!!!!!!!!!!!!!!!! Float var type!!!!!!!!!!!!!!!!!!!!!!\n\n");
                    st_var_aux = new State_Variable( sym_aux->show_float_value(),
                                                     sym_aux->show_code(),
                                                     sym_aux->show_dimension() );
                    /* add this state var to the state var list of the object */
                    obj_st_aux->add_state_var( st_var_aux );
                    break;

                case STATE_VAR_INTQUEUE_TYPE:
                    debug(2,"\n\n!!!!!!!!!!!!!!!!!!!! IntQueue var type!!!!!!!!!!!!!!!!!!!\n\n");
                    intQueue = new IntegerQueue( sym_aux->show_dimension() );
                    st_var_aux = new State_Variable( intQueue,
                                                     sym_aux->show_code() );
                    /* add this state var to the state var list of the object */
                    obj_st_aux->add_state_var( st_var_aux );
                    break;

                case STATE_VAR_FLOATQUEUE_TYPE:
                    debug(2,"\n\n!!!!!!!!!!!!!!!!!! FloatQueue var type!!!!!!!!!!!!!!!!!!!\n\n");
                    floatQueue = new FloatQueue( sym_aux->show_dimension() );
                    st_var_aux = new State_Variable( floatQueue,
                                                     sym_aux->show_code() );
                    /* add this state var to the state var list of the object */
                    obj_st_aux->add_state_var( st_var_aux );
                    break;

            }
            sym_aux = sym_list->show_next_sym();
        }
        delete sym_list;

        /* add this object to system state */
        sys_st_aux->add_obj_st( obj_st_aux );

        obj_desc_aux = obj_desc_list->show_next_obj_desc();
    }
    delete obj_desc_list;

    return( sys_st_aux );
}
//------------------------------------------------------------------------------
int create_hash_tb()
{
    unsigned int  no_vars;
    unsigned int *state_vec;

    /* get the number os state variables in the model */
    no_vars = the_system_desc->show_no_st_var();
    /* allocate space for the maximum state vector */
    state_vec = (unsigned int *) malloc(sizeof(unsigned int) * (no_vars + 1));
    /* get the maximum state vector */
    the_system_desc->get_max_value_vec(state_vec);
    /* create the hash table */
    hashtb = new Hash (state_vec, no_vars + 1);
    /* init the hash table */
    if (!hashtb->init())
    {
        fprintf( stderr, "ERROR: The product of all 'max values' exceeded 1.0e+19. You must decrease at least one of the 'max values'\n");
        return (-1);
    }
    /* free the space used by the maximum state vector */
    free (state_vec);

    return (1);
}
//------------------------------------------------------------------------------
TGFLOAT t2usec ( struct timeval t)
{
    return (1.0 * t.tv_sec * 1e6) + (t.tv_usec);
}
//------------------------------------------------------------------------------
Known_State_List *Finding_States()
{
    Known_State_List     *known_st_list;
    Known_State          *known_st;
    System_State_List    *neighbor_st_list;
    Transition_List      *trans_aux_list;
    Transition           *trans_aux;
    Rate_Reward_List     *reward_list;

    System_State         *sys_st_aux;
    System_State         *neighbor_st;

    int                   det_ev_id;
    Expr_Val             *det_ev_rate;
    Event                *det_ev;

    unsigned int         *state_vec;
    unsigned int          state_id;
    unsigned int          no_sys_st;

    /* allocate space for the state vector */
    state_vec = (unsigned int *) malloc (sizeof(unsigned int) * (the_system_desc->show_no_st_var() + 1));

    /* create the lists */
    known_st_list = new Known_State_List();

    /* copy the first system state */
    sys_st_aux = new System_State( first_system_state );

    /* add the first system state to the hash table */
    first_system_state->get_st_vec(state_vec);
    if (! hashtb->insert(state_vec)) {
        fprintf( stderr, "ERROR: state vector variables bigger than maximum possible\n");
        exit(-1);
    }
    if (! hashtb->vec_to_num(&no_sys_st, state_vec) ) {
        fprintf( stderr, "ERROR: state vector variables bigger than maximum possible\n");
        exit(-1);
    }

    debug (3, "first system state added to the hash table.");

    /* go through all the unexplored states */
    while ( no_sys_st <= hashtb->show_no_states() )
    {
        /* get the state vector for this system state number */
        if ( ! hashtb->num_to_vec(no_sys_st, state_vec))
        {
            fprintf( stderr, "ERROR: state number bigger than possible maximum\n");
            exit(-1);
        }

        /* set the system state to the correct state vector */
        sys_st_aux->set_state_var(state_vec);

        /* print the current systemstate */
        if (Debug_Level > 0)
        {
            fprintf( stdout, "----------------------------------------\n");
            fprintf( stdout, "From this sys state: ");
            sys_st_aux->print_sys_st();
        }

        /* search for the leaves of the current system state */
        /* these are states that can be reached in one transistion */
        det_ev_rate = NULL;
        neighbor_st_list = sys_st_aux->search_for_leaves(&det_ev_id, &det_ev_rate);

        /* print the list of leaves (tangible states) of the current system state */
        if (Debug_Level > 0)
        {
            fprintf( stdout, "Reached these ones: \n");
            neighbor_st_list->print_sys_st_list();
        }

        /* create a new known state */
        known_st = new Known_State(no_sys_st);

        /* if the model is deterministic */
        if (the_system_desc->show_det_model())
        {
            /* change the determinisitc event id and its rate */
            known_st->ch_det_ev_id(det_ev_id);
            known_st->ch_det_ev_rate(det_ev_rate);

            /* if there is one DET event enabled */
            if (det_ev_id != 0)
            {
                det_ev = the_system_desc->show_det_ev_by_id(det_ev_id);
                det_ev->add_st_to_indep_chains(sys_st_aux, neighbor_st_list);
                det_ev->ch_det_ev_rate(det_ev_rate);
            }
        }

        while ( !neighbor_st_list->is_empty() )
        {
            /* get one of the newighbors states */
            neighbor_st = neighbor_st_list->get_1st_sys_st();

            /* get the state vector of the neighbor state */
            neighbor_st->get_st_vec(state_vec);

            /* insert this neighbor state in the hash table */
            if (! hashtb->insert(state_vec))
            {
                fprintf( stderr, "ERROR: state vector variables bigger than maximum possible\n");
                exit(-1);
            }

            /* get the number of this state vector */
            if ( ! hashtb->vec_to_num(&state_id, state_vec))
            {
                fprintf( stderr, "ERROR: state vector variables bigger than maximum possible\n");
                exit(-1);
            }

            /* get the transition to this neighbor state */
            trans_aux_list = neighbor_st->show_trans_list(LIST_RW);
            trans_aux = trans_aux_list->get_1st_trans();

            /* change the destination of the transition */
            trans_aux->ch_st_dest(state_id);

            /* add the transition to the list of transitions in the known state */
            /* join the impulse rewards of different transitions */
            known_st->add_trans(trans_aux);

            /* remove this neighbor state */
            delete neighbor_st;
        }
        /* release the memory of the empty list */
        delete neighbor_st_list;

        /* update the system state number being explored */
        no_sys_st++;

        /* if rate rewards were defined in this model */
        if (the_system_desc->show_rate_reward() == TRUE)
        {
            /* calculate the object rewards for the explored state */
            sys_st_aux->calculate_rate_rewards( NO_CHECK );
            /* calculate the global rewards for this state */
            sys_st_aux->calculate_global_rate_rewards();
            /* get the calculated reward list */
            reward_list = sys_st_aux->get_rate_reward_list();
            /* add the reward list to the known state */
            known_st->ch_reward_list(reward_list);
        }

        /* add the known state to the known state list */
        known_st_list->add_tail_known_st(known_st);

        /* check the number of states generated */
        if ((MaxNumberStates != 0) && (known_st_list->show_no_known_st() > MaxNumberStates))
        {
            fprintf( stderr, "ERROR: Maximum number of states reached: %d\n", MaxNumberStates);
            fprintf( stderr, "Stopping chain generation.\n");
            exit(-1);
        }

        /* print the subtotal number of known states */
        if( !(known_st_list->show_no_known_st() % 100) )
        {
            int states;

            states = hashtb->show_no_states() - no_sys_st;
            if( states != -1 )
            {
                fprintf( stdout, "Subtotal number of states: %d\n",
                        known_st_list->show_no_known_st() );
                fprintf( stdout, "Number of unexplored states: %d\n", states );
            }
        }
    }

    /* print the total number of known states */
    fprintf( stdout, "Total number of states: %d\n", known_st_list->show_no_known_st() );

#ifdef _MEM_DEBUG
    debug_mem_print();
#endif

    return (known_st_list);
}
//------------------------------------------------------------------------------
//     M A I N
//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
#ifdef TG_PARALLEL
    int i;
    char fname[9192+MAXSTRING];
    char *pfname;
#endif
    /* Simulator used for interactive simulation */
    Simulator *simulator;
    /* just to get the time for random number init */
    struct timeval end_time;
    double exec_time_ms = 0;

    /* get the initial time of the execution */
    gettimeofday( &begin_time, 0 );

    /* create default value to the confidence interval string */
    strcpy(Confidence_Interval,"95%");

    /* this function installs signal handler to avoid undesirable crashes */
#ifdef HANDLESIGNAL
    signal( SIGSEGV, CrashHandler );
#endif

#ifdef TG_PARALLEL
    myTaskId = pvm_mytid();
    ntids = pvm_siblings(&tids);
    for (i = 0; i < ntids; i ++)
    {
        if ( tids[i] == myTaskId)
        {
            myTaskNumber = i;
            break;
        }
    }
    if (myTaskNumber == 0)
    {
        for (i = 0; i < ntids; i ++)
        {
            debug(1, "%6x -> ", tids[i]);
            if (i % 6 == 0 && i > 0)
                debug(1,"\n");
        }
    }
#endif
    /* get parameters and check for errors */
    parse_args(argc, argv);
    debug (3, "parsing of args done.\n");

#ifdef TG_PARALLEL
    strcpy(fname,base_filename);
    pfname = fname+strlen(fname);
    while (*pfname != '/') pfname--;
    *pfname = 0;
    chdir(fname);
    pfname++;
    strcpy(base_filename,pfname);
    debug(3," Current Local Directory: ");
    debug(3,fname);
    debug(3," Current Base File Name: ");
    debug(3,base_filename);
#endif

    /* read tables from file dump */
    read_tables();
    debug (3, "read tables done.\n");

    /* create the system description */
    if( (the_system_desc = create_system_desc()) == NULL )
    {
        fprintf( stderr, "--- Fatal Error: Can't create system description\n" );
        return( -1 );
    }

    debug (3, "system description created.\n");
    if (Debug_Level > 2)
        the_system_desc->print_sys_desc();

    /* create the first system state */
    first_system_state = create_system_state(the_system_desc);
    debug (3, "first system state created.\n");
    if (Debug_Level > 1)
        first_system_state->print_sys_st();

    /* reset the execution context data structure */
    reset_context();

    /* chain generation was chosen */
    if ( Generate_Chain ) 
    {
        if( !CAN_GENERATE_CHAIN )
        {
            fprintf( stderr, "--- Fatal Error: Your model has features that cannot be used in the generation of a Markov chain.\n" );
            exit( 1 );
        }

        /* initialize the hash table */
        if (create_hash_tb() < 0)
            return (-1);

        debug (3, "hash table created.\n");

        /* Finding the states */
        markov_chain = Finding_States();
        debug (3, "\nfinding states finished.\n");

        /* Generate the output files */
        generate_output_files();
        debug (3, "finished generating output files\n");
    }

    /* sumulation of the model was chosen */
    if ( Simulate_Model ) 
    {
        if( !CAN_SIMULATE )
        {
            fprintf( stderr, "--- Fatal Error: Your model has features that cannot be used in simulation.\n" );
            exit( 1 );
        }

        /* batch simulation */
        if ( Simulate_Batch ) 
        {
            /* enroll in pvm */
#ifdef TG_PARALLEL
            sprintf( fname,"%s.Task_%d",base_filename,myTaskNumber );
            strcpy( base_filename,fname );
            if ( Simulate_Max_Runs < ntids )
            {
                fprintf( stderr, "--- Fatal Error: Please spawn less tasks than the # of runs" );
                exit( 1 );
            }
#endif

            /* create the simulator controler */
            the_simulator_controler = new Simulator_Controler (Simulate_Max_Runs);

            /* initialize the simulator in the controler */
            the_simulator_controler->init_simulator( Simulate_Max_Time           ,
                                                     Simulate_Max_Trans          ,
                                                     Simulate_Stopping_Event     ,
                                                     Simulate_Stopping_State     ,
                                                     Simulate_Stopping_St_Low    ,
                                                     Simulate_Stopping_St_Upp    ,
                                                     Simulate_Rare_Event         ,
                                                     Simulate_Confidence_Interval,
                                                     reward_to_print             ,
                                                     action_random_obj           ,
                                                     gr_function_random_obj       );

            /* run the simulation */
            events_random_obj->get_seed( Simulate_Events_Seed );
            action_random_obj->get_seed( Simulate_Actions_Seed );
            gr_function_random_obj->get_seed( Simulate_GR_Function_Seed );

            the_simulator_controler->start_simulator( Simulate_Events_Seed,
                                                      Simulate_Actions_Seed,
                                                      Simulate_GR_Function_Seed 
                                                    );
#ifdef TG_PARALLEL
            pvm_exit();
#endif
            if ( output_file != NULL )
            {
                fclose( output_file );
            }
        }

        /* interactive simulation using TANGRAM-II */
        if ( Simulate_Interactive ) 
        {
            /* create the global simulator structure */
            simulator = new Simulator(Simulate_Max_Time, Simulate_Max_Trans, reward_to_print, action_random_obj, gr_function_random_obj);

            /* create the interactive simulator */
            the_simulator_interactive = new Simulator_Interactive( simulator );

            /* create the event list for the simulator */
            simulator->create_event_list();

            /* initialize the stopping event */
            if ( strcmp( Simulate_Stopping_Event, "" ) )
                simulator->set_stopping_event(Simulate_Stopping_Event);

            /* if rate rewards were defined in this model */
            if ( the_system_desc->show_rate_reward() == TRUE )
            {
                /* create the rate rewards measure list for the simulation and   */
                /* create the unified rate reward sum list (if there is any one) */
                simulator->create_rate_rew_list();
            }

            /* if impulse rewards were defined in this model */
            if ( the_system_desc->show_impulse_reward() == TRUE )
                /* create the impulse rewards list for the simulation */
                simulator->create_imp_rew_list();

            /* initialize the event list with all enabled events */
            simulator->initialize_event_list();

            /* simulate_model_interactive(); */
            the_simulator_interactive->simulate_model_interactive();
        }
    }

    /* Total execution time */
    gettimeofday( &end_time, 0 );
    end_time.tv_sec = end_time.tv_sec - begin_time.tv_sec;
    end_time.tv_usec = end_time.tv_usec - begin_time.tv_usec;
    if ( end_time.tv_usec < 0 )
    {
        end_time.tv_sec -= 1;
        end_time.tv_usec += 1000000;
    }

    exec_time_ms = end_time.tv_sec * 1000.0 + end_time.tv_usec/1000.0;
    fprintf ( stdout, "Total Execution Real Time: %.3f msec\n", exec_time_ms );
}
//------------------------------------------------------------------------------
