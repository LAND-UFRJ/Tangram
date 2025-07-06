#include "ghmm.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "core_controller.h"
#include "signal_manager.h"

#include "sample.h"
#include "random.h"
#include "matrix.h"
#include "train.h"
#include "train_fast.h"
#include "viterbi.h"
#include "object_io.h"
#include "steady.h"
#include "transient.h"
#include "acf.h"

void initGHMM( void ) __attribute__ ((constructor));

Object::Info object_info =
{
    "ghmm",
    "Hierarquical Gilbert Hidden Markov Model",
    GHMM_HELP_MESSAGE,
    1, 0,
    ObjectBase<GHMM>::help,
    ObjectBase<GHMM>::create
};

GHMM::GHMM( const Name & name, const GHMM & hmm )
        : ObjectBase<GHMM>(name,this)
{
    parameter.N = hmm.parameter.N;
    parameter.B = hmm.parameter.B;

    parameter.pi = copy_vector( parameter.N, hmm.parameter.pi );
    parameter.A  = copy_matrix( parameter.N, parameter.N, hmm.parameter.A );
    parameter.gamma = copy_vector( parameter.N, hmm.parameter.gamma );

    parameter.p  = copy_vector( parameter.N, hmm.parameter.p );
    parameter.q  = copy_vector( parameter.N, hmm.parameter.q );
    parameter.r  = copy_vector( parameter.N, hmm.parameter.r );

    epsilon = 0.00001;
        
    result.clear();

    Terminal::displayMessage( "ghmm object was successfully created with name %s", getName() );
    Terminal::displayMessage( "Parameters were initialized with random values." );
    Terminal::displayMessage( "Model error tolerance is epsilon = %f", epsilon );
}

GHMM::GHMM( const Name & name, unsigned int n, unsigned int b )
        : ObjectBase<GHMM>(name,this)
{
    parameter.N = n;
    parameter.B = b;

    parameter.pi = create_full_vector( parameter.N );
    parameter.A  = create_full_matrix( parameter.N, parameter.N );
    parameter.gamma = copy_vector( parameter.N, parameter.pi );
   
    parameter.p  = create_probability_vector( parameter.N );
    parameter.q  = create_probability_vector( parameter.N );
    parameter.r  = create_probability_vector( parameter.N );

    epsilon = 0.00001;

    result.clear();

    Terminal::displayMessage( "ghmm object was successfully created with name %s", getName() );
    Terminal::displayMessage( "Parameters were initialized with random values." );
    Terminal::displayMessage( "Model error tolerance is epsilon = %f", epsilon );
}

GHMM::GHMM( const Name & name )
        : ObjectBase<GHMM>(name,this)
{
    parameter.N = 0;
    parameter.B = 0;

    parameter.pi = 0;
    parameter.A = 0;
    parameter.gamma = 0;
    parameter.p = 0;
    parameter.q = 0;
    parameter.r = 0;

    epsilon = 0.00001;

    result.clear();

    Terminal::displayMessage( "ghmm object was successfully created with name %s", getName() );
    Terminal::displayMessage( "Parameters were not initialized. User should load them later." );
    Terminal::displayMessage( "Model error tolerance is epsilon = %f", epsilon );
}

GHMM::~GHMM( void )
{
    DEBUG(LEVEL_INFO, "Destructing Batch Hidden Markov Model (%s)\n",
          object_name.c_str( ));

    destroy_vector( parameter.N, parameter.pi );
    destroy_matrix( parameter.N, parameter.N, parameter.A );
    destroy_vector( parameter.N, parameter.gamma );
    destroy_vector( parameter.N, parameter.r );
    destroy_vector( parameter.N, parameter.p );
    destroy_vector( parameter.N, parameter.q );
}

bool GHMM::display_all( Terminal & term )
{
    term.print( "Number of states:  %d\n", parameter.N );
    term.print( "Batch size:        %d\n\n", parameter.B );

    display_pi( term );
    term.print( "\n" );
    display_A( term );
    term.print( "\n" );
    display_obs( term );

    return true;
}

bool GHMM::display_pi( Terminal & term )
{
    unsigned int i;

    term.print( "Initial hidden state distribution:\n" );
    term.print( " [" );

    for( i = 0; i < parameter.N; i++ )
        term.print( " %.5e", parameter.pi[i] );

    term.print( " ]\n" );

    return true;
}

bool GHMM::display_A( Terminal & term )
{
    unsigned int i, j;

    term.print( "Hidden states transition probabilities:\n" );
    for( i = 0; i < parameter.N; i++ )
    {
        term.print( " [" );

        for( j = 0; j < parameter.N; j++ )
            term.print( " %.5e", parameter.A[i][j] );

        term.print( " ]\n" );
    }

    return true;
}

bool GHMM::display_obs( Terminal & term )
{
    unsigned int i;

    term.print( "Symbol observation probabilities:\n" );
    for( i = 0; i < parameter.N; i++ )
    {
        term.print( " .In hidden State %d:\n", i+1 );
        
        /* Initial State Probability */
        term.print( "   Initial probability of state 1: [ %.5e ]\n\n",
                    parameter.r[i] );

        /* State Transition Probabilities */        
        term.print( "   Internal state transition probabilities:\n" );
        term.print( "     (0 -> 1): [ %.5e ]\n", parameter.p[i] );
        term.print( "     (1 -> 0): [ %.5e ]\n", parameter.q[i] );

        term.print( "\n" );
    }

    return true;
}

bool GHMM::exec_normalize( Arguments & args )
{
    if( args.size( ) == 0 || args[0].text == "all" )
    {
        normalize_vector( parameter.N, parameter.pi );
        normalize_matrix( parameter.N, parameter.N, parameter.A );
    }
    else if( args[0].text == "pi" )
        normalize_vector( parameter.N, parameter.pi );
    else if( args[0].text == "A" )
        normalize_matrix( parameter.N, parameter.N, parameter.A );
    else
    {
        Terminal::displayErrorMessage( "invalid parameter to normalize"     );
        Terminal::displayErrorMessage( "available options are pi, A or all" );
        return false;
    }

    Terminal::displayMessage( " Note: The normalization is done by dividing every element" );
    Terminal::displayMessage( "       of a probability vector by the sum of its elements." );

    return true;
}

bool GHMM::exec_train( Arguments & args )
{
    Object * object;
    Sample * s;
    Terminal * terminal;
    int nit;

    nit = atol( args[0].text.c_str( ) );

    if( nit < 1 )
    {
        Terminal::displayErrorMessage( "invalid number of iterations" );
        return false;
    }

    object = CoreController::getObjectByName( args[1].text );
    if( !object )
        return false;

    // get sample from object passed as parameter
    s = (Sample *)object->run( "get_sample", 0 );

    if( !s || (s->size < 2) )
    {
        Terminal::displayErrorMessage( "not enough data available on %s",
                                       object->getName( ) );
        return false;
    }

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        SignalManager::registerFlag( &training_flag );

        double *result = train( &parameter,
                                 s->size,
                                 s->data,
                                 nit,
                                 0.0,
                                 terminal->getFile( ) );

        if( result )
        {
            // Save result
            // result[0] = result_size
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
}

bool GHMM::exec_train_fast( Arguments & args )
{
    Object * object;
    Sample * s;
    Terminal * terminal;
    int nit;

    nit = atol( args[0].text.c_str( ) );

    if( nit < 1 )
    {
        Terminal::displayErrorMessage( "invalid number of iterations" );
        return false;
    }

    object = CoreController::getObjectByName( args[1].text );
    if( !object )
        return false;

    // get sample from object passed as parameter
    s = (Sample *)object->run( "get_sample", 0 );

    if( !s || (s->size < 2) )
    {
        Terminal::displayErrorMessage( "not enough data available on %s",
                                       object->getName( ) );
        return false;
    }

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        SignalManager::registerFlag( &fast_training_flag );
        double *result = train_fast( &parameter, s->size, s->data, nit, 0.0,
                                      terminal->getFile( ) );
        if( result )
        {
            // Save result
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
}

bool GHMM::exec_train_threshold( Arguments & args )
{
    Object * object;
    Sample * s;
    Terminal * terminal;
    int nit;
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

    object = CoreController::getObjectByName( args[2].text );
    if( !object )
        return false;

    // get sample from object passed as parameter
    s = (Sample *)object->run( "get_sample", 0 );

    if( !s || (s->size < 2) )
    {
        Terminal::displayErrorMessage( "not enough data available on %s",
                                       object->getName( ) );
        return false;
    }

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        SignalManager::registerFlag( &training_flag );
        double *result = train( &parameter, s->size, s->data, nit, threshold,
                                 terminal->getFile( ) );
        if( result )
        {
            // Save result
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
}

bool GHMM::exec_train_fast_threshold( Arguments & args )
{
    Object * object;
    Sample * s;
    Terminal * terminal;
    int nit;
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

    object = CoreController::getObjectByName( args[2].text );
    if( !object )
        return false;

    // get sample from object passed as parameter
    s = (Sample *)object->run( "get_sample", 0 );

    if( !s || (s->size < 2) )
    {
        Terminal::displayErrorMessage( "not enough data available on %s",
                                       object->getName( ) );
        return false;
    }

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        SignalManager::registerFlag( &fast_training_flag );
        double *result = train_fast( &parameter,
                                      s->size,
                                      s->data,
                                      nit, threshold,
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
}

bool GHMM::exec_train_complete( Arguments & args )
{
    Object * object_obs, * object_states;
    Sample * sample_obs, * sample_states;
    Terminal * terminal;
    int nb;

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

    /* calculate the number of batches in the observation sample */
    nb = sample_obs->size / parameter.B;
    if( sample_obs->size % parameter.B > 0 )
        nb++;

    if( sample_states->size != nb )
    {
        Terminal::displayErrorMessage(
              "there must be a state sample for each batch in observations" );
        return false;
    }

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        SignalManager::registerFlag( &training_flag );
        train_complete( &parameter, sample_obs->size, sample_obs->data,
                        sample_states->data, terminal->getFile( ) );
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

bool GHMM::exec_simulate( Arguments & args )
{
    Object * object_states, * object_obs;
    Sample sample_obs, sample_states;
    int i, b, B, base;

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

    sample_states.size = sample_obs.size/parameter.B;
    if( (sample_obs.size % parameter.B) > 0 )
        sample_states.size++;

    sample_obs.data    = new int[sample_obs.size];
    sample_states.data = new int[sample_states.size];

    Terminal::displayMessage( "Simulation running..." );

    sample_states.data[0] = get_random( parameter.N, parameter.pi );

    B = parameter.B;
    for( base = 0, b = 0; b < sample_states.size; b++, base += parameter.B )
    {
        if( b == (sample_states.size - 1) &&
            (sample_obs.size % parameter.B) > 0 )
        {
            B = sample_obs.size % parameter.B;
            if( B == 0 )
                B = parameter.B;
        }

        sample_obs.data[base] = IND( parameter.r[sample_states.data[b]] );

        for( i = 1; i < B; i++ )
        {
            if( sample_obs.data[base+i-1] == 0 )
                sample_obs.data[base+i] =
                                      IND( parameter.p[sample_states.data[b]] );
            else if( sample_obs.data[base+i-1] == 1 )
                sample_obs.data[base+i] =
                                    IND( 1-parameter.q[sample_states.data[b]] );
        }

        if( b < sample_states.size - 1 )
        {
            sample_states.data[b+1] = get_random( parameter.N,
                                           parameter.A[sample_states.data[b]] );
        }
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

bool GHMM::exec_viterbi( Arguments & args )
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

    sample_states.size = sample_obs->size / parameter.B;
    if( sample_obs->size % parameter.B > 0 )
        sample_states.size++;
    sample_states.data = new int[sample_states.size];

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        
        // Save result
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

bool GHMM::exec_likelihood( Arguments & args )
{
    Object * object_obs, * object_states;
    Sample * sample_obs, * sample_states;
    Terminal * terminal;
    int nb;

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

        nb = sample_obs->size / parameter.B;
        if( sample_obs->size % parameter.B > 0 )
            nb++;

        if( !sample_states ||
            sample_states->size != nb )
        {
            Terminal::displayErrorMessage(
                "states sample size does not match number of observations" );
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

    return true;
}

bool GHMM::exec_likelihood_fast( Arguments & args )
{
    Object * object_obs;
    Sample * sample_obs;
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

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        
        double *result = likelihood_fast( &parameter,
                                           sample_obs->size,
                                           sample_obs->data,
                                           terminal->getFile( ) );
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

    return true;
}

Object * GHMM::createEmpty( const Name & name, Arguments & args )
{
    return new GHMM( name );
}

Object * GHMM::createSized( const Name & name, Arguments & args )
{
    int N, B;

    N = atol( args[0].text.c_str( ) );
    if( N < 1 )
    {
        Terminal::error_stream << "parameter N" \
                               << " must be greater than or equal to 1" \
                               << std::endl;
        return nullptr;
    }
    B = atol( args[1].text.c_str( ) );
    if( B < 1 )
    {
        Terminal::error_stream << "parameter B" \
                               << " must be greater than or equal to 1" \
                               << std::endl;
        return nullptr;
    }

    return new GHMM( name, N, B );
}

bool GHMM::autocorrelation( Arguments & args )
{
    double theta;
    int n;
    Terminal * terminal;

    theta = atof( args[0].text.c_str( ) );

    if( args.size( ) > 1 )
    {
        n = atol( args[1].text.c_str( ) );
        if( n < 1 )
        {
            Terminal::displayErrorMessage(
                                 "must calculate at least one element" );
            return false;
        }
    }
    else
        n = INT_MAX;

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        loss_autocorrelation( &parameter, theta, n, terminal->getFile( ) );
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

bool GHMM::symbol_time_average( Arguments & args )
{
    Terminal * terminal;

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );

        // Save result
        saveResult( 0, sym_time_average( &parameter, terminal->getFile( ) ) );

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

bool GHMM::symbol_sum_dist( Arguments & args )
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
        
        // Save result
        double *result = symbol_sum_distribution( &parameter, f,
                                                  sample_obs->size,
                                                  sample_obs->data,
                                                  terminal->getFile( ) );
        if( result )
        {
            for( int i = 1; i < (int)result[0]; i++ )
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

bool GHMM::steady_state_burst_size( Arguments & args )
{
    double theta;
    int n, symbol;
    Terminal * terminal;

    symbol = atol( args[0].text.c_str( ) );
    if( symbol != 0 && symbol != 1 )
    {
        Terminal::displayErrorMessage( "symbol must be either 0 or 1" );
        return false;
    }

    theta = atof( args[1].text.c_str( ) );
    if( theta > 1.0 )
    {
        Terminal::displayErrorMessage(
        "Threshold represents a probability, and thus must be between 0 and 1." );

        return false;
    }

    if( args.size( ) > 2 )
    {
        n = atol( args[2].text.c_str( ) );
        if( n < 1 )
        {
            Terminal::displayErrorMessage(
                                 "must calculate at least one element" );
            return false;
        }
    }
    else
        n = INT_MAX;

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        
        double *result = ss_burst_size( &parameter,
                                         symbol,
                                         theta,
                                         n,
                                         terminal->getFile() );

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

bool GHMM::transient_burst_size( Arguments & args )
{
    Object * object_obs;
    Sample * sample_obs;
    double theta;
    int n, symbol;
    Terminal * terminal;

    object_obs = CoreController::getObjectByName( args[0].text );
    if( !object_obs )
        return false;

    /* get sample from object passed as parameter */
    sample_obs = (Sample *)object_obs->run( "get_sample", 0 );

    if( !sample_obs || (sample_obs->size < 0) )
    {
        Terminal::displayErrorMessage( "no observations available on %s",
                                       object_obs->getName( ) );
        return false;
    }

    symbol = atol( args[1].text.c_str( ) );
    if( symbol != 0 && symbol != 1 )
    {
        Terminal::displayErrorMessage( "symbol must be either 0 or 1" );
        return false;
    }

    theta = atof( args[2].text.c_str( ) );

    if( args.size( ) > 3 )
    {
        n = atol( args[3].text.c_str( ) );
        if( n < 1 )
        {
            Terminal::displayErrorMessage(
                                 "must calculate at least one element" );
            return false;
        }
    }
    else
        n = INT_MAX;

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );
        double *result = tr_burst_size( &parameter,
                                         symbol,
                                         sample_obs->size,
                                         sample_obs->data,
                                         n, theta,
                                         terminal->getFile() );
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

bool GHMM::state_prob( Arguments & args )
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

void initGHMM( void )
{
    DEBUG(LEVEL_INFO,"Initializing Gilbert Model Library\n");

    GHMM::addConstructor( &GHMM::createEmpty, Prototype( ) );
    GHMM::addConstructor( &GHMM::createSized,
                          Prototype( 2, 2, TK_INTEGER, TK_INTEGER ) );

    GHMM::addOption( "N", "number of hidden states",
                     0, &GHMM::set_N,  &GHMM::get_N );
    GHMM::addOption( "B", "observation batch size",
                     0, &GHMM::set_B,  &GHMM::get_B );
    GHMM::addOption( "pi", "initial probability for the i-th state",
                     1, &GHMM::set_pi, &GHMM::get_pi );
    GHMM::addOption( "A", "transition probability from state i to state j",
                     2, &GHMM::set_A,  &GHMM::get_A );
    GHMM::addOption( "p",
             "transition probability from symbol 0 to symbol 1 at state i",
                     1, &GHMM::set_p,  &GHMM::get_p );
    GHMM::addOption( "q",
             "transition probability from symbol 1 to symbol 0 at state i",
                     1, &GHMM::set_q,  &GHMM::get_q );
    GHMM::addOption( "r", "initial probability of symbol 1 at state i",
                     1, &GHMM::set_r,  &GHMM::get_r );
    GHMM::addOption( "result", "result array of last executed method",
                     1, &GHMM::set_result,  &GHMM::get_result );

    GHMM::addDisplay( "all", "model parameters",
                      &GHMM::display_all );
    GHMM::addDisplay( "pi", "initial state distribution",
                      &GHMM::display_pi );
    GHMM::addDisplay( "A", "state transition matrix",
                      &GHMM::display_A );
    GHMM::addDisplay( "obs", "observation parameters",
                      &GHMM::display_obs );
                      
    GHMM::addCommand( "load", &GHMM::load,
                     Prototype( 1, 2, TK_STRING, TK_STRING ),
                     LOAD_HELP_MESSAGE );

    GHMM::addCommand( "save", &GHMM::save,
                     Prototype( 1, 2, TK_STRING, TK_STRING ),
                     SAVE_HELP_MESSAGE );

    GHMM::addCommand( "normalize", &GHMM::exec_normalize,
                      Prototype( 0, 1, TK_IDENTIFIER ),
                      NORMALIZE_HELP_MESSAGE );

    GHMM::addCommand( "training", &GHMM::exec_train,
                      Prototype( 2, 2, TK_INTEGER, TK_IDENTIFIER ),
                      TRAIN_HELP_MESSAGE );

    GHMM::addCommand( "training", &GHMM::exec_train_threshold,
                      Prototype( 3, 3, TK_INTEGER, TK_REAL, TK_IDENTIFIER ),
                      TRAIN_HELP_MESSAGE );

    GHMM::addCommand( "training", &GHMM::exec_train_complete,
                      Prototype( 2, 2, TK_IDENTIFIER, TK_IDENTIFIER ),
                      TRAIN_HELP_MESSAGE );

    GHMM::addCommand( "training_fast", &GHMM::exec_train_fast,
                      Prototype( 2, 2, TK_INTEGER, TK_IDENTIFIER ),
                      TRAIN_FAST_HELP_MESSAGE );

    GHMM::addCommand( "training_fast", &GHMM::exec_train_fast_threshold,
                      Prototype( 3, 3, TK_INTEGER, TK_REAL, TK_IDENTIFIER ),
                      TRAIN_HELP_MESSAGE );

    GHMM::addCommand( "simulate", &GHMM::exec_simulate,
                      Prototype(2, 3, TK_INTEGER, TK_IDENTIFIER, TK_IDENTIFIER),
                      SIMULATE_HELP_MESSAGE );

    GHMM::addCommand( "viterbi", &GHMM::exec_viterbi,
                      Prototype( 2, 2, TK_IDENTIFIER, TK_IDENTIFIER ),
                      VITERBI_HELP_MESSAGE );

    GHMM::addCommand( "likelihood", &GHMM::exec_likelihood,
                      Prototype( 1, 2, TK_IDENTIFIER, TK_IDENTIFIER ),
                      LIKELIHOOD_HELP_MESSAGE );

    GHMM::addCommand( "likelihood_fast", &GHMM::exec_likelihood_fast,
                      Prototype( 1, 1, TK_IDENTIFIER ),
                      LIKELIHOOD_FAST_HELP_MESSAGE );

    GHMM::addCommand( "import_from_tangram", &GHMM::import_from_tangram,
                      Prototype( 4, UNLIMITED,
                                    TK_STRING,
                                    TK_INTEGER,
                                    TK_INTEGER,
                                    TK_STRING ),
                      IMPORT_FROM_TANGRAM_HELP_MESSAGE );

    GHMM::addCommand( "set_full", &GHMM::set_full,
                      Prototype( 2, 2, TK_INTEGER, TK_INTEGER ),
                      SET_FULL_HELP_MESSAGE );
                     
    GHMM::addCommand( "set_coxian", &GHMM::set_coxian,
                      Prototype( 2, 2, TK_INTEGER, TK_INTEGER ),
                      SET_COXIAN_HELP_MESSAGE );

    GHMM::addCommand( "set_qbd", &GHMM::set_qbd,
                      Prototype( 3, 5, TK_INTEGER, TK_INTEGER, 
                                       TK_INTEGER, TK_INTEGER, TK_INTEGER ),
                      SET_QBD_HELP_MESSAGE );

    GHMM::addCommand( "fix_full", &GHMM::fix_full,
                      Prototype( 2, 2, TK_INTEGER, TK_INTEGER ),
                      FIX_FULL_HELP_MESSAGE );
                     
    GHMM::addCommand( "fix_coxian", &GHMM::fix_coxian,
                      Prototype( 2, 2, TK_INTEGER, TK_INTEGER ),
                      FIX_COXIAN_HELP_MESSAGE );

    GHMM::addCommand( "fix_qbd", &GHMM::fix_qbd,
                      Prototype( 3, 5, TK_INTEGER, TK_INTEGER, 
                                       TK_INTEGER, TK_INTEGER, TK_INTEGER ),
                                       FIX_QBD_HELP_MESSAGE );

    GHMM::addCommand( "set_epsilon", &GHMM::set_epsilon,
                      Prototype( 1, 1, TK_REAL ),
                      SET_EPSILON_HELP_MESSAGE );

    GHMM::addCommand( "symb_tavg", &GHMM::symbol_time_average,
                      Prototype( 0, 0 ), SYM_TAVG_HELP_MESSAGE );

    GHMM::addCommand( "symb_sum_dist", &GHMM::symbol_sum_dist,
                      Prototype( 2, 2, TK_IDENTIFIER, TK_INTEGER ),
                      SYM_SUM_DIST_HELP_MESSAGE );

    GHMM::addCommand( "state_prob", &GHMM::state_prob,
                      Prototype( 0, 1, TK_IDENTIFIER ),
                      STATE_PROB_HELP_MESSAGE );

    /**
       The methods below need to be revised, to see if they
       are correct.
       
       All were programed by Fernando Jorge.
    **/
 /* GHMM::addCommand( "burst_size", &GHMM::steady_state_burst_size,
                      Prototype( 2, 3, TK_INTEGER, TK_REAL, TK_INTEGER ),
                      BURST_SIZE_HELP_MESSAGE );

    GHMM::addCommand( "burst_size", &GHMM::transient_burst_size,
                      Prototype( 3, 4, TK_IDENTIFIER, TK_INTEGER, TK_REAL,
                                 TK_INTEGER ),
                      BURST_SIZE_HELP_MESSAGE );
    GHMM::addCommand( "autocorrelation", &GHMM::autocorrelation,
                      Prototype( 1, 2, TK_REAL, TK_INTEGER ),
                      AUTOCORRELATION_HELP_MESSAGE ); */
}

void GHMM::saveResult( int position, double value )
{
    Index idx; 
    char  buffer[32];
    
    idx.push_back( position );

    sprintf( buffer, "%.10e", value );
    set_result( buffer, idx );        

    idx.clear();       
}
