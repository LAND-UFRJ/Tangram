#include "hmm_batch.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "core_controller.h"
#include "signal_manager.h"

#include "batches.h"
#include "sample.h"
#include "random.h"
#include "matrix.h"
#include "train.h"
#include "train_fast.h"
#include "viterbi.h"
#include "object_io.h"
#include "transient.h"

void initHMMBatch( void ) __attribute__ ((constructor));

Object::Info object_info =
{
    "hmm_batch_variable",
    "Hierarquical General Hidden Markov Model - Variable Batch",
    HMMBatch_HELP_MESSAGE,
    1, 0,
    ObjectBase<HMMBatch>::help,
    ObjectBase<HMMBatch>::create
};

HMMBatch::HMMBatch( const Name & name, const HMMBatch & hmm )
        : ObjectBase<HMMBatch>(name,this)
{
    parameter.N = hmm.parameter.N;
    parameter.M = hmm.parameter.M;

    parameter.pi = copy_vector( parameter.N, hmm.parameter.pi );
    parameter.A  = copy_matrix( parameter.N, parameter.N, hmm.parameter.A );
    parameter.p  = copy_3d_matrix( parameter.N, parameter.M, parameter.M, hmm.parameter.p );
    parameter.r  = copy_matrix( parameter.N, parameter.M, hmm.parameter.r );

    parameter.gamma = copy_vector( parameter.N, hmm.parameter.gamma );

    epsilon = 0.00001;

    Terminal::displayMessage( "hmm_batch_variable object was successfully created with name %s", getName() );
    Terminal::displayMessage( "Parameters were initialized with random values." );
    Terminal::displayMessage( "Model error tolerance is epsilon = %f", epsilon );
}

HMMBatch::HMMBatch( const Name & name, unsigned int n, unsigned int m )
        : ObjectBase<HMMBatch>(name,this)
{
    unsigned int i;
    
    parameter.N = n;
    parameter.M = m;

    parameter.pi = create_full_vector( parameter.N );
    parameter.A  = create_full_matrix( parameter.N, parameter.N );
    parameter.p  = create_full_3d_matrix( parameter.N, parameter.M, parameter.M );
    parameter.r  = create_full_matrix( parameter.N, parameter.M );
    
    /* Initial probability of "end of batch" symbol must be zero */
    for( i=0; i<parameter.N; i++ )
    {
        parameter.r[i][parameter.M - 1] = 0;
        normalize_vector( parameter.M, parameter.r[i] );
    }

    parameter.gamma = copy_vector( parameter.N, parameter.pi );

    epsilon = 0.00001;

    Terminal::displayMessage( "hmm_batch_variable object was successfully created with name %s", getName() );
    Terminal::displayMessage( "Parameters were initialized with random values." );
    Terminal::displayMessage( "Model error tolerance is epsilon = %f", epsilon );
}

HMMBatch::HMMBatch( const Name & name )
        : ObjectBase<HMMBatch>(name,this)
{
    parameter.N = 0;
    parameter.M = 0;

    parameter.pi = 0;
    parameter.A = 0;
    parameter.p = 0;
    parameter.r = 0;

    parameter.gamma = 0;

    epsilon = 0.00001;

    Terminal::displayMessage( "hmm_batch_variable object was successfully created with name %s", getName() );
    Terminal::displayMessage( "Parameters were not initialized. User should load them later." );
    Terminal::displayMessage( "Model error tolerance is epsilon = %f", epsilon );
}

HMMBatch::~HMMBatch( void )
{
    DEBUG(LEVEL_INFO, "Destroying Batch Hidden Markov Model (%s)\n",
          object_name.c_str( ));

    destroy_vector( parameter.N, parameter.pi );
    destroy_vector( parameter.N, parameter.gamma );
    destroy_matrix( parameter.N, parameter.N, parameter.A );
    destroy_matrix( parameter.N, parameter.M, parameter.r );
    destroy_3d_matrix( parameter.N, parameter.M, parameter.M, parameter.p );
}

bool HMMBatch::display_all( Terminal & term )
{
    term.print( "Number of states:  %d\n", parameter.N );
    term.print( "Number of observations:  %d\n", parameter.M );
    term.print( "Obs: Last symbol represents the end of a batch.\n" );

    display_pi( term );
    term.print( "\n" );
    display_A( term );
    term.print( "\n" );
    display_obs( term );

    return true;
}

bool HMMBatch::display_pi( Terminal & term )
{
    unsigned int i;

    term.print( "Initial state distribution:\n" );
    term.print( " [" );

    for( i = 0; i < parameter.N; i++ )
        term.print( " %.5e", parameter.pi[i] );

    term.print( " ]\n" );

    return true;
}

bool HMMBatch::display_A( Terminal & term )
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

bool HMMBatch::display_obs( Terminal & term )
{
    unsigned int i, j, k;

    term.print( "Symbol observation probabilities:\n" );
    term.print( "Warning: Last symbol is the end of batch symbol.\n" );

    for( i = 0; i < parameter.N; i++ )
    {
        term.print( " .In hidden state %d\n", i+1 );

        // r
        term.print( "   Initial state probability:\n" );
        for( j = 0; j < parameter.M; j++ )
        {
            term.print( "    [ %.5e ]",
                        parameter.r[i][j] );
        }
        term.print( "\n\n" );

        // p
        term.print( "   State transition probabilities:\n" );
        for( j = 0; j < parameter.M; j++ )
        {
            for( k = 0; k < parameter.M; k++ )
            {
                term.print( "    [ %.5e ]",
                            parameter.p[i][j][k] );
            }
            term.print( "\n" );
        }
        term.print( "\n" );
    }
    return true;
}

bool HMMBatch::exec_normalize( Arguments & args )
{
    unsigned int i;

    if( args.size( ) == 0 || args[0].text == "all" )
    {
        normalize_vector( parameter.N, parameter.pi );
        normalize_matrix( parameter.N, parameter.N, parameter.A );

        normalize_matrix( parameter.N, parameter.M, parameter.r );
        for( i = 0; i < parameter.N; i++ )
            normalize_matrix( parameter.M, parameter.M, parameter.p[i] );
    }
    else if( args[0].text == "pi" )
        normalize_vector( parameter.N, parameter.pi );
    else if( args[0].text == "A" )
        normalize_matrix( parameter.N, parameter.N, parameter.A );
    else
    {
        Terminal::displayErrorMessage("Invalid parameter to normalize.");
        Terminal::displayErrorMessage("Available options are pi, A or all.");
        return false;
    }

    Terminal::displayMessage( " Note: The normalization is done by dividing every element" );
    Terminal::displayMessage( "       of a probability vector by the sum of its elements." );

    return true;
}

bool HMMBatch::exec_train( Arguments & args )
{
    Object * object;
    Sample * s;
    Terminal * terminal;
    int nit;

    nit = atol( args[0].text.c_str( ) );

    if( nit < 1 )
    {
        Terminal::displayErrorMessage( "Invalid number of iterations." );
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

        // Analize batch
        analize_batch( &parameter, s->size, s->data );
        
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

bool HMMBatch::exec_train_fast( Arguments & args )
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

        // Analize batch
        analize_batch( &parameter, s->size, s->data );

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

bool HMMBatch::exec_train_threshold( Arguments & args )
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

        // Analize batch
        analize_batch( &parameter, s->size, s->data );

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

bool HMMBatch::exec_train_fast_threshold( Arguments & args )
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

        // Analize batch
        analize_batch( &parameter, s->size, s->data );

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

bool HMMBatch::exec_train_complete( Arguments & args )
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
    analize_batch( &parameter, sample_obs->size, sample_obs->data );
    nb = num_batches;

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

bool HMMBatch::exec_simulate( Arguments & args )
{
    Object * object_states, * object_obs;
    Sample sample_obs, sample_states;
    int i, b;

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

    sample_states.size = 1;

    sample_obs.data    = new int[sample_obs.size];
    sample_states.data = new int[sample_states.size];

    Terminal::displayMessage( "Simulation running..." );

    // Sampling first hidden state according to pi
    sample_states.data[0] = get_random( parameter.N, parameter.pi );

    // For all hidden states
    b = 0;

    // Sampling first state within hidden state according to r
    sample_obs.data[0] = get_random( parameter.M, parameter.r[sample_states.data[b]] );

    for( i = 1; i < (sample_obs.size - 1); i++ )
    {
        // Sampling next observation according to p
        // Previous obs: sample_obs.data[i-1]
        // Actual obs: sample_obs.data[i]
        // Hidden State: sample_states.data[b]
        sample_obs.data[i] = 
               get_random( parameter.M, parameter.p[sample_states.data[b]][sample_obs.data[i-1]] );

        // End of a batch
        if( ( sample_obs.data[i] == (int)parameter.M - 1 ) && 
            ( ( i + 1 ) < sample_obs.size ) )
        {
            // Transition to a new hidden state
            sample_states.size++;
    
            sample_states.data = (int *)realloc( sample_states.data, sample_states.size * sizeof( int ) );

            // Sampling new hidden state according to A
            sample_states.data[b+1] = get_random( parameter.N,
                                                  parameter.A[sample_states.data[b]] );
            b++; // next hidden state

            // Sampling first state within hidden state according to r
            i++;
            // If reached number of observations, stop sampling
            if( i == sample_obs.size )
                break;

            // Sampling first batch observation within the new hidden state
            sample_obs.data[i] = get_random( parameter.M, parameter.r[sample_states.data[b]] );
        }
    }
    
    /* Last observation must be end of batch */
    sample_obs.data[sample_obs.size - 1] = parameter.M - 1;

    //set sample to object passed as parameter
    if( object_states )
        object_states->run( "set_sample", &sample_states );
    object_obs->run( "set_sample", &sample_obs );

    Terminal::displayMessage( "Simulation ended successfully." );

    delete sample_states.data;
    delete sample_obs.data;

    return true;
}

bool HMMBatch::exec_viterbi( Arguments & args )
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

    analize_batch( &parameter, sample_obs->size, sample_obs->data );
    sample_states.size = num_batches; 

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

bool HMMBatch::exec_likelihood( Arguments & args )
{
    Object * object_obs, * object_states;
    Sample * sample_obs, * sample_states;
    Terminal * terminal;

    object_obs = CoreController::getObjectByName( args[0].text );
    if( !object_obs )
        return false;

    // get sample observations from object passed as parameter
    sample_obs = (Sample *)object_obs->run( "get_sample", 0 );

    if( !sample_obs || (sample_obs->size < 0) )
    {
        Terminal::displayErrorMessage( "no observations available on %s",
                                       object_obs->getName( ) );
        return false;
    }

    analize_batch( &parameter, sample_obs->size, sample_obs->data );

    if( args.size( ) == 2 )
    {
        object_states = CoreController::getObjectByName( args[1].text );
        if( !object_states )
            return false;

        // get sample states from object passed as parameter
        sample_states = (Sample *)object_states->run( "get_sample", 0 );

        if( !sample_states ||
            sample_states->size != num_batches )
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

bool HMMBatch::exec_likelihood_fast( Arguments & args )
{
    Object * object_obs;
    Sample * sample_obs;
    Terminal * terminal;

    object_obs = CoreController::getObjectByName( args[0].text );
    if( !object_obs )
        return false;

    // get sample observations from object passed as parameter
    sample_obs = (Sample *)object_obs->run( "get_sample", 0 );

    if( !sample_obs || (sample_obs->size < 0) )
    {
        Terminal::displayErrorMessage( "no observations available on %s",
                                       object_obs->getName( ) );
        return false;
    }

    analize_batch( &parameter, sample_obs->size, sample_obs->data );

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

Object * HMMBatch::createEmpty( const Name & name, Arguments & args )
{
    return new HMMBatch( name );
}

Object * HMMBatch::createSized( const Name & name, Arguments & args )
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

    return new HMMBatch( name, N, M );
}

bool HMMBatch::state_prob( Arguments & args )
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

        // Analize batch
        if( sample_obs )
            analize_batch( &parameter, sample_obs->size, sample_obs->data );

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

void initHMMBatch( void )
{
    DEBUG(LEVEL_INFO,"Initializing HMM Batch Model Library\n");

    HMMBatch::addConstructor( &HMMBatch::createEmpty, Prototype( ) );
    HMMBatch::addConstructor( &HMMBatch::createSized,
                          Prototype( 2, 2, TK_INTEGER, TK_INTEGER ) );

    HMMBatch::addOption( "N", "number of hidden states",
                     0, &HMMBatch::set_N,  &HMMBatch::get_N );
    HMMBatch::addOption( "M", "number of observations. Last symbol will be used to represent the end of a batch",
                     0, &HMMBatch::set_M,  &HMMBatch::get_M );
    HMMBatch::addOption( "pi", "initial probability for the i-th state",
                     1, &HMMBatch::set_pi, &HMMBatch::get_pi );
    HMMBatch::addOption( "A", "transition probability from state i to state j",
                     2, &HMMBatch::set_A,  &HMMBatch::get_A );
    HMMBatch::addOption( "p",
             "transition probability from symbol j to symbol k for the i-th state",
                     3, &HMMBatch::set_p,  &HMMBatch::get_p );
    HMMBatch::addOption( "r", "initial probability of symbol j for the i-th state",
                     2, &HMMBatch::set_r,  &HMMBatch::get_r );

    HMMBatch::addDisplay( "all", "model parameters",
                      &HMMBatch::display_all );
    HMMBatch::addDisplay( "pi", "initial state distribution",
                      &HMMBatch::display_pi );
    HMMBatch::addDisplay( "A", "state transition matrix",
                      &HMMBatch::display_A );
    HMMBatch::addDisplay( "obs", "observation parameters",
                      &HMMBatch::display_obs );
                      
    HMMBatch::addCommand( "load", &HMMBatch::load,
                     Prototype( 1, 2, TK_STRING, TK_STRING ),
                     LOAD_HELP_MESSAGE );

    HMMBatch::addCommand( "save", &HMMBatch::save,
                     Prototype( 1, 2, TK_STRING, TK_STRING ),
                     SAVE_HELP_MESSAGE );

    HMMBatch::addCommand( "normalize", &HMMBatch::exec_normalize,
                      Prototype( 0, 1, TK_IDENTIFIER ),
                      NORMALIZE_HELP_MESSAGE );

    HMMBatch::addCommand( "training", &HMMBatch::exec_train,
                      Prototype( 2, 2, TK_INTEGER, TK_IDENTIFIER ),
                      TRAIN_HELP_MESSAGE );

    HMMBatch::addCommand( "training", &HMMBatch::exec_train_threshold,
                      Prototype( 3, 3, TK_INTEGER, TK_REAL, TK_IDENTIFIER ),
                      TRAIN_HELP_MESSAGE );

    HMMBatch::addCommand( "training", &HMMBatch::exec_train_complete,
                      Prototype( 2, 2, TK_IDENTIFIER, TK_IDENTIFIER ),
                      TRAIN_HELP_MESSAGE );

    HMMBatch::addCommand( "training_fast", &HMMBatch::exec_train_fast,
                      Prototype( 2, 2, TK_INTEGER, TK_IDENTIFIER ),
                      TRAIN_FAST_HELP_MESSAGE );

    HMMBatch::addCommand( "training_fast", &HMMBatch::exec_train_fast_threshold,
                      Prototype( 3, 3, TK_INTEGER, TK_REAL, TK_IDENTIFIER ),
                      TRAIN_FAST_HELP_MESSAGE );

    HMMBatch::addCommand( "simulate", &HMMBatch::exec_simulate,
                      Prototype(2, 3, TK_INTEGER, TK_IDENTIFIER, TK_IDENTIFIER),
                      SIMULATE_HELP_MESSAGE );

    HMMBatch::addCommand( "viterbi", &HMMBatch::exec_viterbi,
                      Prototype( 2, 2, TK_IDENTIFIER, TK_IDENTIFIER ),
                      VITERBI_HELP_MESSAGE );

    HMMBatch::addCommand( "likelihood", &HMMBatch::exec_likelihood,
                      Prototype( 1, 2, TK_IDENTIFIER, TK_IDENTIFIER ),
                      LIKELIHOOD_HELP_MESSAGE );

    HMMBatch::addCommand( "likelihood_fast", &HMMBatch::exec_likelihood_fast,
                      Prototype( 1, 1, TK_IDENTIFIER ),
                      LIKELIHOOD_FAST_HELP_MESSAGE );
                      
    HMMBatch::addCommand( "import_from_tangram", &HMMBatch::import_from_tangram,
                      Prototype( 4, UNLIMITED,
                                    TK_STRING,
                                    TK_INTEGER,
                                    TK_INTEGER,
                                    TK_STRING ),
                      IMPORT_FROM_TANGRAM_HELP_MESSAGE );

    HMMBatch::addCommand( "set_full", &HMMBatch::set_full,
                      Prototype( 2, 2, TK_INTEGER, TK_INTEGER ),
                      SET_FULL_HELP_MESSAGE );
                     
    HMMBatch::addCommand( "set_coxian", &HMMBatch::set_coxian,
                      Prototype( 2, 2, TK_INTEGER, TK_INTEGER ),
                      SET_COXIAN_HELP_MESSAGE );

    HMMBatch::addCommand( "set_qbd", &HMMBatch::set_qbd,
                      Prototype( 3, 5, TK_INTEGER, TK_INTEGER, 
                                       TK_INTEGER, TK_INTEGER, TK_INTEGER ),
                      SET_QBD_HELP_MESSAGE );

    HMMBatch::addCommand( "fix_full", &HMMBatch::fix_full,
                      Prototype( 2, 2, TK_INTEGER, TK_INTEGER ),
                      FIX_FULL_HELP_MESSAGE );
                     
    HMMBatch::addCommand( "fix_coxian", &HMMBatch::fix_coxian,
                      Prototype( 2, 2, TK_INTEGER, TK_INTEGER ),
                      FIX_COXIAN_HELP_MESSAGE );

    HMMBatch::addCommand( "fix_qbd", &HMMBatch::fix_qbd,
                      Prototype( 3, 5, TK_INTEGER, TK_INTEGER, 
                                       TK_INTEGER, TK_INTEGER, TK_INTEGER ),
                                       FIX_QBD_HELP_MESSAGE );

    HMMBatch::addCommand( "set_epsilon", &HMMBatch::set_epsilon,
                      Prototype( 1, 1, TK_REAL ),
                      SET_EPSILON_HELP_MESSAGE );

    HMMBatch::addCommand( "state_prob", &HMMBatch::state_prob,
                      Prototype( 0, 1, TK_IDENTIFIER ),
                      STATE_PROB_HELP_MESSAGE );
}

void HMMBatch::saveResult( int position, double value )
{
    Index idx; 
    char  buffer[32];
    
    idx.push_back( position );

    sprintf( buffer, "%.10e", value );
    set_result( buffer, idx );        

    idx.clear();       
}
