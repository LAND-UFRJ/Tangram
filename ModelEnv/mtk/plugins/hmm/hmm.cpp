#include "hmm.h"

#include <stdio.h>
#include <stdlib.h>

#include "core_controller.h"
#include "signal_manager.h"

#include "sample.h"
#include "random.h"
#include "steady.h"
#include "matrix.h"
#include "train.h"
#include "viterbi.h"
#include "forecast.h"
#include "object_io.h"
#include "transient.h"

void initHMM( void ) __attribute__ ((constructor));

Object::Info object_info =
{
    "hmm",
    "Hidden Markov Model",
    HMM_HELP_MESSAGE,
    1, 0,
    ObjectBase<HMM>::help,
    ObjectBase<HMM>::create
};

HMM::HMM( const Name & name, const HMM & hmm )
        : ObjectBase<HMM>(name,this)
{
    parameter.N = hmm.parameter.N;
    parameter.M = hmm.parameter.M;

    parameter.pi = copy_vector( parameter.N, hmm.parameter.pi );
    parameter.A  = copy_matrix( parameter.N, parameter.N, hmm.parameter.A );
    parameter.B  = copy_matrix( parameter.N, parameter.M, hmm.parameter.B );
    
    parameter.gamma = copy_vector( parameter.N, hmm.parameter.gamma );

    epsilon = 0.00001;

    Terminal::displayMessage( "hmm object was successfully created with name %s", getName() );
    Terminal::displayMessage( "Model error tolerance is epsilon = %f", epsilon );
}

HMM::HMM( const Name & name, unsigned int n, unsigned int m )
        : ObjectBase<HMM>(name,this)
{
    parameter.N = n;
    parameter.M = m;

    parameter.pi = create_full_vector( parameter.N );
    parameter.A  = create_full_matrix( parameter.N, parameter.N );
    parameter.B  = create_full_matrix( parameter.N, parameter.M );

    parameter.gamma = copy_vector( parameter.N, parameter.pi );

    epsilon = 0.00001;

    Terminal::displayMessage( "hmm object was successfully created with name %s", getName() );
    Terminal::displayMessage( "Parameters were initialized with random values." );
    Terminal::displayMessage( "Model error tolerance is epsilon = %f", epsilon );
}

HMM::HMM( const Name & name )
        : ObjectBase<HMM>(name,this)
{
    parameter.N = 0;
    parameter.M = 0;

    parameter.pi = 0;
    parameter.A = 0;
    parameter.B = 0;

    parameter.gamma = 0;

    epsilon = 0.00001;

    Terminal::displayMessage( "hmm object was successfully created with name %s", getName() );
    Terminal::displayMessage( "Parameters were not initialized. User should load them later." );
    Terminal::displayMessage( "Model error tolerance is epsilon = %f", epsilon );
}

HMM::~HMM( void )
{
    DEBUG(LEVEL_INFO, "Destroying Hidden Markov Model (%s)\n",
          object_name.c_str( ));

    destroy_vector( parameter.N, parameter.pi );
    destroy_matrix( parameter.N, parameter.N, parameter.A );
    destroy_matrix( parameter.N, parameter.M, parameter.B );
    destroy_vector( parameter.N, parameter.gamma );
}

bool HMM::display_all( Terminal & term )
{
    term.print( "Number of states:  %d\n", parameter.N );
    term.print( "Number of symbols: %d\n\n", parameter.M );

    display_pi( term );
    term.print( "\n" );
    display_A( term );
    term.print( "\n" );
    display_B( term );

    return true;
}

bool HMM::display_pi( Terminal & term )
{
    unsigned int i;

    term.print( "Initial state distribution:\n" );
    term.print( " [" );

    for( i = 0; i < parameter.N; i++ )
        term.print( " %.5e", parameter.pi[i] );

    term.print( " ]\n" );

    return true;
}

bool HMM::display_A( Terminal & term )
{
    unsigned int i, j;

    term.print( "States transition probabilities:\n" );
    for( i = 0; i < parameter.N; i++ )
    {
        term.print( " [" );

        for( j = 0; j < parameter.N; j++ )
            term.print( " %.5e", parameter.A[i][j] );

        term.print( " ]\n" );
    }

    return true;
}

bool HMM::display_B( Terminal & term )
{
    unsigned int i, j;

    term.print( "Symbol observation probabilities:\n" );
    for( i = 0; i < parameter.N; i++ )
    {
        term.print( " In hidden State %d: [", i+1 );

        for( j = 0; j < parameter.M; j++ )
            term.print( " %.5e", parameter.B[i][j] );

        term.print( " ]\n" );
    }

    return true;
}

bool HMM::exec_normalize( Arguments & args )
{
    if( args.size( ) == 0 || args[0].text == "all" )
    {
        normalize_vector( parameter.N, parameter.pi );
        normalize_matrix( parameter.N, parameter.N, parameter.A );
        normalize_matrix( parameter.N, parameter.M, parameter.B );
    }
    else if( args[0].text == "pi" )
        normalize_vector( parameter.N, parameter.pi );
    else if( args[0].text == "A" )
        normalize_matrix( parameter.N, parameter.N, parameter.A );
    else if( args[0].text == "B" )
        normalize_matrix( parameter.N, parameter.M, parameter.B );
    else
    {
        Terminal::displayErrorMessage("invalid parameter to normalize");
        Terminal::displayErrorMessage("available options are pi, A, B or all");
        return false;
    }

    Terminal::displayMessage( " Note: The normalization is done by dividing every element" );
    Terminal::displayMessage( "       of a probability vector by the sum of its elements." );

    return true;
}

bool HMM::exec_train( Arguments & args )
{
    Object * object;
    Sample * sample, ** samples;
    std::list<Sample *> sample_list;
    Terminal * terminal;
    int nit;
    unsigned int i;
    int j, n;
    std::list<Sample *>::iterator it;

    nit = atol( args[0].text.c_str( ) );

    if( nit < 1 )
    {
        Terminal::displayErrorMessage( "invalid number of iterations" );
        return false;
    }

    for( i = 1; i < args.size( ); i++ )
    {
        object = CoreController::getObjectByName( args[i].text );
        if( !object )
            return false;

        // try to get multiple samples from object passed as parameter
        sample = (Sample *)object->run( "get_samples", &n );
        if( sample )
        {
            if( n < 1 )
            {
                Terminal::displayErrorMessage( "no samples available on %s",
                                               object->getName( ) );
                return false;
            }

            for( j = 0; j < n; j++ )
            {
                if( !sample[j].data || (sample[j].size < 1) )
                {
                    Terminal::displayErrorMessage(
                                  "no data available on sample %d of %s",
                                  j, object->getName( ) );
                    return false;
                }

                sample_list.push_back( &sample[j] );
            }
        }
        else
        {
            // try to get single sample from object passed as parameter
            sample = (Sample *)object->run( "get_sample", 0 );

            if( !sample || (sample->size < 1) )
            {
                Terminal::displayErrorMessage( "no data available on %s",
                                               object->getName( ) );
                return false;
            }

            sample_list.push_back( sample );
        }
    }

    samples = new Sample *[sample_list.size( )];
    for( i = 0, it = sample_list.begin( ); it != sample_list.end( ); i++, it++ )
    {
        samples[i] = (*it);
    }

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        SignalManager::registerFlag( &training_flag );

        double *result;

        if( sample_list.size( ) > 1 )
        {
            result = train_multiple( &parameter,
                                      sample_list.size( ), samples, nit, 0.0,
                                      terminal->getFile( ) );
        }
        else
        {
            result = train( &parameter,
                             samples[0]->size, samples[0]->data, nit, 0.0,
                             terminal->getFile( ) );
        }
        
        if( result )
        {
            // Save result
            // result[0] = result_size
            for( int i = 1; i <= (int)result[0]; i++ )
                saveResult( i - 1, result[i] );

            free( result );
        }        

        terminal->close( );
        delete samples;
        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not get a valid terminal" );
        delete samples;
        return false;
    }
}

bool HMM::exec_train_threshold( Arguments & args )
{
    Object * object;
    Sample * sample, ** samples;
    std::list<Sample *> sample_list;
    Terminal * terminal;
    int nit;
    unsigned int i;
    int j, n;
    std::list<Sample *>::iterator it;
    double threshold;

    nit = atol( args[0].text.c_str( ) );

    if( nit < 1 )
    {
        Terminal::displayErrorMessage( "invalid number of iterations" );
        return false;
    }

    threshold = atof( args[1].text.c_str( ) );

    if( threshold < 0.0 )
    {
        Terminal::displayErrorMessage( "threshold must be non-negative" );
        return false;
    }

    for( i = 2; i < args.size( ); i++ )
    {
        object = CoreController::getObjectByName( args[i].text );
        if( !object )
            return false;

        /* try to get multiple samples from object passed as parameter */
        sample = (Sample *)object->run( "get_samples", &n );
        if( sample )
        {
            if( n < 1 )
            {
                Terminal::displayErrorMessage( "no samples available on %s",
                                               object->getName( ) );
                return false;
            }

            for( j = 0; j < n; j++ )
            {
                if( !sample[j].data || (sample[j].size < 1) )
                {
                    Terminal::displayErrorMessage(
                                  "no data available on sample %d of %s",
                                  j, object->getName( ) );
                    return false;
                }

                sample_list.push_back( &sample[j] );
            }
        }
        else
        {
            // try to get single sample from object passed as parameter
            sample = (Sample *)object->run( "get_sample", 0 );

            if( !sample || (sample->size < 1) )
            {
                Terminal::displayErrorMessage( "no data available on %s",
                                               object->getName( ) );
                return false;
            }

            sample_list.push_back( sample );
        }
    }

    samples = new Sample *[sample_list.size( )];
    for( i = 0, it = sample_list.begin( ); it != sample_list.end( ); i++, it++ )
    {
        samples[i] = (*it);
    }

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        SignalManager::registerFlag( &training_flag );
        
        double *result;        

        if( sample_list.size( ) > 1 )
        {
            result = train_multiple( &parameter,
                                      sample_list.size( ),
                                      samples, nit,
                                      threshold, terminal->getFile( ) );
        }
        else
        {
            result = train( &parameter,
                             samples[0]->size, samples[0]->data, nit,
                             threshold, terminal->getFile( ) );
        }
        
        if( result )
        {
            // Save result
            // result[0] = result_size
            for( int i = 1; i <= (int)result[0]; i++ )
                saveResult( i - 1, result[i] );

            free( result );
        }        

        terminal->close( );
        delete samples;
        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not get a valid terminal" );
        delete samples;
        return false;
    }
}

bool HMM::exec_train_complete( Arguments & args )
{
    Object * object_obs, * object_states;
    Sample * sample_obs, * sample_states;

    object_obs = CoreController::getObjectByName( args[0].text );
    if( !object_obs )
        return false;

    object_states = CoreController::getObjectByName( args[1].text );
    if( !object_states )
        return false;

    // get sample from object passed as parameter
    sample_obs = (Sample *)object_obs->run( "get_sample", 0 );
    if( !sample_obs || (sample_obs->size < 2) )
    {
        Terminal::displayErrorMessage( "not enough data available on %s",
                                       object_obs->getName( ) );
        return false;
    }

    // get sample from object passed as parameter
    sample_states = (Sample *)object_states->run( "get_sample", 0 );
    if( !sample_states || (sample_states->size < 2) )
    {
        Terminal::displayErrorMessage( "not enough data available on %s",
                                       object_states->getName( ) );
        return false;
    }

    if( sample_obs->size != sample_states->size )
    {
        Terminal::displayErrorMessage(
                    "observations and states samples must have the same size" );
        return false;
    }

    train_complete( &parameter, sample_obs->size, sample_obs->data,
                    sample_states->data );
    return true;
}

bool HMM::exec_simulate( Arguments & args )
{
    Object * object_states, * object_obs;
    Sample sample_obs, sample_states;
    int i;

    sample_obs.size = atol( args[0].text.c_str( ) );
    if( sample_obs.size < 1 )
    {
        Terminal::displayErrorMessage( "number of samples must be at least 1" );
        return false;
    }

    object_obs = CoreController::getObjectByName( args[1].text );
    if( !object_obs )
        return false;

    if( args.size( ) == 3 )
    {
        object_states = CoreController::getObjectByName( args[2].text );
        if( !object_states )
            return false;
    }
    else
        object_states = 0;

    sample_states.size = sample_obs.size;

    sample_obs.data    = new int[sample_obs.size];
    sample_states.data = new int[sample_states.size];

    Terminal::displayMessage( "Simulation running..." );

    sample_states.data[0] = get_random( parameter.N, parameter.pi );
    sample_obs.data[0]    = get_random( parameter.M,
                                        parameter.B[sample_states.data[0]] );

    for( i = 1; i < sample_obs.size; i++ )
    {
        sample_states.data[i] = get_random( parameter.N,
                                         parameter.A[sample_states.data[i-1]] );
        sample_obs.data[i]    = get_random( parameter.M,
                                           parameter.B[sample_states.data[i]] );
    }

    // set sample to object passed as parameter
    if( object_states )
        object_states->run( "set_sample", &sample_states );
    object_obs->run( "set_sample", &sample_obs );

    Terminal::displayMessage( "Simulation ended successfully." );

    delete sample_states.data;
    delete sample_obs.data;

    return true;
}

bool HMM::exec_viterbi( Arguments & args )
{
    Object * object_states, * object_obs;
    Sample * sample_obs, sample_states;
    Terminal * terminal;
    bool status;

    object_obs = CoreController::getObjectByName( args[0].text );
    if( !object_obs )
        return false;

    object_states = CoreController::getObjectByName( args[1].text );
    if( !object_states )
        return false;

    // get sample from object passed as parameter
    sample_obs = (Sample *)object_obs->run( "get_sample", 0 );

    if( !sample_obs || (sample_obs->size < 1) )
    {
        Terminal::displayErrorMessage( "not enough data available on %s",
                                       object_obs->getName( ) );
        return false;
    }

    sample_states.size = sample_obs->size;
    sample_states.data = new int[sample_states.size];

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        double *result = viterbi( &parameter,
                                   sample_obs->size,
                                   sample_obs->data,
                                   sample_states.data,
                                   terminal->getFile( ) );
        if( result )
        {
            status = true;
            
            saveResult( 0, result[0] );
            
            free( result );
        }
        else
            status = false;                          

        terminal->close( );
    }
    else
    {
        Terminal::displayErrorMessage( "could not get a valid terminal" );
        status = false;
    }

    object_states->run( "set_sample", &sample_states );

    delete sample_states.data;

    return status;
}

bool HMM::exec_likelihood( Arguments & args )
{
    Object * object_obs, * object_states;
    Sample * sample_obs, * sample_states;
    Terminal * terminal;

    object_obs = CoreController::getObjectByName( args[0].text );
    if( !object_obs )
        return false;

    // get sample from object passed as parameter
    sample_obs = (Sample *)object_obs->run( "get_sample", 0 );

    if( !sample_obs || (sample_obs->size < 0) )
    {
        Terminal::displayErrorMessage( "no observations available on %s",
                                       object_obs->getName( ) );
        return false;
    }

    if( args.size( ) == 2 )
    {
        object_states = CoreController::getObjectByName( args[1].text );
        if( !object_states )
            return false;

        // get sample from object passed as parameter
        sample_states = (Sample *)object_states->run( "get_sample", 0 );

        if( !sample_states || (sample_states->size != sample_obs->size) )
        {
            Terminal::displayErrorMessage(
                "need a number of states equal to the number of observations" );
            return false;
        }
    }
    else
        sample_states = NULL;

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        
        double *result;
        
        if( args.size( ) == 1 )
            result = likelihood( &parameter,
                                  sample_obs->size,
                                  sample_obs->data,
                                  terminal->getFile( ) );
        else if( args.size( ) == 2 )
            result = complete_likelihood( &parameter,
                                           sample_obs->size,
                                           sample_obs->data,
                                           sample_states->data,
                                           terminal->getFile( ) );
        else
            result = NULL;

        // Save result
        if( result )
        {
            for( int i = 0; i < 2; i++ )
                saveResult( i, result[i] );

            free( result );
        }

        terminal->close( );

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not get a valid terminal" );
        return false;
    }
}

bool HMM::exec_forecast( Arguments & args )
{
    Object * object_in, * object_out;
    Sample * sample_in, * sample_out, sample;
    Terminal * terminal;
    int future;

    future = atoi( args[0].text.c_str( ) );
    if( future < 1 )
    {
        Terminal::displayErrorMessage( "Forecasting window size must be positive." );
        return false;
    }

    object_in = CoreController::getObjectByName( args[1].text );
    if( !object_in )
        return false;

    // get sample from object passed as parameter
    sample_in = (Sample *)object_in->run( "get_sample", 0 );
    if( !sample_in || (sample_in->size < 1) )
    {
        Terminal::displayErrorMessage( "no data available on %s",
                                       object_in->getName( ) );
        return false;
    }

    if( args.size( ) == 3 )
    {
        object_out = CoreController::getObjectByName( args[2].text );
        if( !object_out )
            return false;

        // get sample from object passed as parameter
        sample.size = future;
        sample.data = (int *)malloc( sample.size * sizeof(int) );

        sample_out  = &sample;
    }
    else
    {
        sample_out = 0;
        object_out = NULL;
    }

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        forecast( &parameter, sample_in, sample_out, future, terminal->getFile( ) );
        terminal->close( );

        /* Saving forecasted values. */       
        if( args.size( ) == 3 )
        {
            object_out->run( "set_sample", sample_out );
            free( sample.data );
        }

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not get a valid terminal" );
        if( args.size( ) == 3 )
            free( sample.data );

        return false;
    }
}

Object * HMM::createEmpty( const Name & name, Arguments & args )
{
    return new HMM( name );
}

Object * HMM::createSized( const Name & name, Arguments & args )
{
    int N, M;

    N = atol( args[0].text.c_str( ) );
    if( N < 1 )
    {
        Terminal::error_stream << "parameter N" \
                               << " must be greater than or equal to 1" \
                               << std::endl;
        return nullptr;
    }
    M = atol( args[1].text.c_str( ) );
    if( M < 1 )
    {
        Terminal::error_stream << "parameter M" \
                               << " must be greater than or equal to 1" \
                               << std::endl;
        return nullptr;
    }

    return new HMM( name, N, M );
}

bool HMM::symbol_sum_dist( Arguments & args )
{
    Object * object_obs;
    Sample * sample_obs;
    int f;
    Terminal * terminal;

    object_obs = CoreController::getObjectByName( args[0].text );
    if( !object_obs )
        return false;

    // get sample from object passed as parameter
    sample_obs = (Sample *)object_obs->run( "get_sample", 0 );

    if( !sample_obs || (sample_obs->size < 0) )
    {
        Terminal::displayErrorMessage( "no observations available on %s",
                                       object_obs->getName( ) );
        return false;
    }

    f = atol( args[1].text.c_str( ) );
    if( f < 1 )
    {
        Terminal::displayErrorMessage( "future size must be positive" );
        return false;
    }

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        
        double *result = symbol_sum_distribution( &parameter, f,
                                                  sample_obs->size,
                                                  sample_obs->data,
                                                  terminal->getFile( ) );
        if( result )
        {
            for( int i = 1; i <= (int)result[0]; i++ )
                saveResult( i - 1, result[i] );
            
            free( result );
        }

        terminal->close( );

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not get a valid terminal" );
        return false;
    }

    return true;
}

bool HMM::symbol_time_average( Arguments & args )
{
    Terminal * terminal;

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );

        double *result = sym_time_average( &parameter, terminal->getFile( ) );
        
        if( result )
        {
            saveResult( 0, result[0] );

            free( result );
        }

        terminal->close( );

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not get a valid terminal" );
        return false;
    }

    return true;
}

bool HMM::state_prob( Arguments & args )
{
    Object * object_obs;
    Sample * sample_obs;
    Terminal * terminal;

    if( args.size( ) > 0 )
    {
        object_obs = CoreController::getObjectByName( args[0].text );
        if( !object_obs )
            return false;

        // get sample from object passed as parameter
        sample_obs = (Sample *)object_obs->run( "get_sample", 0 );

        if( !sample_obs || (sample_obs->size < 0) )
        {
            Terminal::displayErrorMessage( "no observations available on %s",
                                           object_obs->getName( ) );
            return false;
        }
    }
    else
        sample_obs = 0;

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        
        double *result;        
        
        if( sample_obs )
            result = state_probability( &parameter,
                                         sample_obs->size,
                                         sample_obs->data,
                                         terminal->getFile( ) );
        else
            result = state_probability( &parameter, 0, 0,
                                         terminal->getFile( ) );

        if( result )
        {
            for( int i = 1; i <= (int)result[0]; i++ )
                saveResult( i - 1, result[i] );

            free( result );
        }

        terminal->close( );

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not get a valid terminal" );
        return false;
    }

    return true;
}

void initHMM( void )
{
    DEBUG(LEVEL_INFO,"Initializing Gilbert Model Library\n");

    HMM::addConstructor( &HMM::createEmpty, Prototype( ) );
    HMM::addConstructor( &HMM::createSized,
                         Prototype( 2, 2, TK_INTEGER, TK_INTEGER ) );

    HMM::addOption( "N", "number of hidden states",
                     0, &HMM::set_N,  &HMM::get_N );
    HMM::addOption( "M", "number of observation symbols",
                     0, &HMM::set_M,  &HMM::get_M );
    HMM::addOption( "pi", "initial probability for the i-th state",
                     1, &HMM::set_pi, &HMM::get_pi );
    HMM::addOption( "A", "transition probability from state i to state j",
                     2, &HMM::set_A,  &HMM::get_A );
    HMM::addOption( "B", "probability of symbol j at state i",
                     2, &HMM::set_B,  &HMM::get_B );
    HMM::addOption( "result", "result array of last executed method",
                     1, &HMM::set_result,  &HMM::get_result );                     

    HMM::addDisplay( "all", "model parameters",
                     &HMM::display_all );
    HMM::addDisplay( "pi", "initial state distribution",
                     &HMM::display_pi );
    HMM::addDisplay( "A", "state transition matrix",
                     &HMM::display_A );
    HMM::addDisplay( "B", "observation probability matrix",
                     &HMM::display_B );

    HMM::addCommand( "load", &HMM::load,
                     Prototype( 1, 2, TK_STRING, TK_STRING ),
                     LOAD_HELP_MESSAGE );

    HMM::addCommand( "save", &HMM::save,
                     Prototype( 1, 2, TK_STRING, TK_STRING ),
                     SAVE_HELP_MESSAGE );

    HMM::addCommand( "normalize", &HMM::exec_normalize,
                     Prototype( 0, 1, TK_IDENTIFIER ),
                     NORMALIZE_HELP_MESSAGE );

    HMM::addCommand( "training", &HMM::exec_train,
                     Prototype( 2, UNLIMITED, TK_INTEGER, TK_IDENTIFIER ),
                     TRAIN_HELP_MESSAGE );

    HMM::addCommand( "training", &HMM::exec_train_threshold,
                     Prototype(3,UNLIMITED, TK_INTEGER, TK_REAL, TK_IDENTIFIER),
                     TRAIN_HELP_MESSAGE );

    HMM::addCommand( "training", &HMM::exec_train_complete,
                     Prototype( 2, 2, TK_IDENTIFIER, TK_IDENTIFIER ),
                     TRAIN_HELP_MESSAGE );

    HMM::addCommand( "simulate", &HMM::exec_simulate,
                     Prototype(2, 3, TK_INTEGER, TK_IDENTIFIER, TK_IDENTIFIER),
                     SIMULATE_HELP_MESSAGE );

    HMM::addCommand( "viterbi", &HMM::exec_viterbi,
                     Prototype( 2, 2, TK_IDENTIFIER, TK_IDENTIFIER ),
                     VITERBI_HELP_MESSAGE );

    HMM::addCommand( "likelihood", &HMM::exec_likelihood,
                     Prototype( 1, 2, TK_IDENTIFIER, TK_IDENTIFIER ),
                     LIKELIHOOD_HELP_MESSAGE );

    HMM::addCommand( "forecast", &HMM::exec_forecast,
                     Prototype( 2, 3, TK_INTEGER, TK_IDENTIFIER,
                                TK_IDENTIFIER ),
                     FORECAST_HELP_MESSAGE );

    HMM::addCommand( "import_from_tangram", &HMM::import_from_tangram,
                      Prototype( 3, UNLIMITED,
                                    TK_STRING,
                                    TK_INTEGER,
                                    TK_STRING ),
                      IMPORT_FROM_TANGRAM_HELP_MESSAGE );

    HMM::addCommand( "set_full", &HMM::set_full,
                     Prototype( 2, 2, TK_INTEGER, TK_INTEGER ),
                     SET_FULL_HELP_MESSAGE );
                     
    HMM::addCommand( "set_coxian", &HMM::set_coxian,
                     Prototype( 2, 2, TK_INTEGER, TK_INTEGER ),
                     SET_COXIAN_HELP_MESSAGE );

    HMM::addCommand( "set_qbd", &HMM::set_qbd,
                     Prototype( 3, 5, TK_INTEGER, TK_INTEGER, 
                                      TK_INTEGER, TK_INTEGER, TK_INTEGER ),
                     SET_QBD_HELP_MESSAGE );
                     
    HMM::addCommand( "set_gilbert", &HMM::set_gilbert,
                      Prototype( 0, 0 ),
                      SET_GILBERT_HELP_MESSAGE );

    HMM::addCommand( "fix_full", &HMM::fix_full,
                      Prototype( 2, 2, TK_INTEGER, TK_INTEGER ),
                      FIX_FULL_HELP_MESSAGE );
                     
    HMM::addCommand( "fix_coxian", &HMM::fix_coxian,
                      Prototype( 2, 2, TK_INTEGER, TK_INTEGER ),
                      FIX_COXIAN_HELP_MESSAGE );

    HMM::addCommand( "fix_qbd", &HMM::fix_qbd,
                      Prototype( 3, 5, TK_INTEGER, TK_INTEGER, 
                                       TK_INTEGER, TK_INTEGER, TK_INTEGER ),
                      FIX_QBD_HELP_MESSAGE );

    HMM::addCommand( "fix_gilbert", &HMM::fix_gilbert,
                      Prototype( 0, 0 ),
                      FIX_GILBERT_HELP_MESSAGE );

    HMM::addCommand( "set_epsilon", &HMM::set_epsilon,
                      Prototype( 1, 1, TK_REAL ),
                      SET_EPSILON_HELP_MESSAGE );                     
                     
    HMM::addCommand( "symb_sum_dist", &HMM::symbol_sum_dist,
                      Prototype( 2, 2, TK_IDENTIFIER, TK_INTEGER ),
                      SYM_SUM_DIST_HELP_MESSAGE );

    HMM::addCommand( "symb_tavg", &HMM::symbol_time_average,
                      Prototype( 0, 0 ),
                      SYM_TAVG_HELP_MESSAGE );

    HMM::addCommand( "state_prob", &HMM::state_prob,
                      Prototype( 0, 1, TK_IDENTIFIER ),
                      STATE_PROB_HELP_MESSAGE );
}

void HMM::saveResult( int position, double value )
{
    Index idx; 
    char  buffer[32];
    
    idx.push_back( position );

    sprintf( buffer, "%.10e", value );
    set_result( buffer, idx );        

    idx.clear();       
}
