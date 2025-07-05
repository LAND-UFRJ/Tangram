#include "hmm_batch.h"

#include <fstream>

#include <string.h>

#include "matrix.h"
#include "object_io.h"
#include "watchdog.h"

#include "import_from_tangram.h"

bool HMMBatch::load_all( const std::string &file_name )
{
    unsigned int i, j, k;
    Parameter new_param;
    WatchDog<double> param_watcher;
    std::ifstream infile( file_name.c_str( ) );

    if( infile )
    {
        READ(infile,new_param.N);
        LOWER_LIMIT(new_param.N,1);

        READ(infile,new_param.M);
        LOWER_LIMIT(new_param.M,1);

        new_param.pi = create_zero_vector( new_param.N );
        param_watcher.addWatch( new_param.pi );

        for( i = 0; i < new_param.N; i++ )
        {
            READ(infile,new_param.pi[i]);
            LOWER_LIMIT(new_param.pi[i],0.0);
            UPPER_LIMIT(new_param.pi[i],1.0);
        }

        if( !is_vector_stochastic( new_param.N, new_param.pi ) )
        {
            Terminal::displayWarningMessage(
                "initial state definition is not a valid distribution" );
            Terminal::displayWarningMessage(
                "you may want to normalize vector pi after loading" );
        }

        /* Initializing gamma = pi */
        new_param.gamma = create_zero_vector( new_param.N );
        param_watcher.addWatch( new_param.gamma );

        for( i = 0; i < new_param.N; i++ )
            new_param.gamma[i] = new_param.pi[i];

        new_param.A = create_zero_matrix( new_param.N, new_param.N );
        param_watcher.addWatch( new_param.A, new_param.N );

        for( i = 0; i < new_param.N; i++ )
        {
            for( j = 0; j < new_param.N; j++ )
            {
                READ(infile,new_param.A[i][j]);
                LOWER_LIMIT(new_param.A[i][j],0.0);
                UPPER_LIMIT(new_param.A[i][j],1.0);
            }
        }

        if( !is_matrix_stochastic( new_param.N, new_param.N, new_param.A ) )
        {
            Terminal::displayWarningMessage(
                "state transitions definition is not a stochastic matrix" );
            Terminal::displayWarningMessage(
                "you may want to normalize matrix A after loading" );
        }

        new_param.r = create_zero_matrix( new_param.N, new_param.M );
        param_watcher.addWatch( new_param.r, new_param.N );

        new_param.p = create_zero_3d_matrix( new_param.N, new_param.M, new_param.M );
        for( i = 0; i < new_param.N; i++ )
        {
            param_watcher.addWatch( new_param.p[i], new_param.M );
        }

         // r[i][0] r[i][1] .. r[i][M-1] p[i][0][0] p[i][0][1] p[i][0][M-1] p[i][1][0] ... p[i][M-1][M-1]
        for( i = 0; i < new_param.N; i++ )
        {
            // r
            for( j = 0; j < new_param.M; j++ )
            {
                READ(infile,new_param.r[i][j]);
                LOWER_LIMIT(new_param.r[i][j],0.0);
                UPPER_LIMIT(new_param.r[i][j],1.0);
            }

            // p
            for( j = 0; j < new_param.M; j++ )
            {
                for( k = 0; k < new_param.M; k++ )
                {
                    READ(infile,new_param.p[i][j][k]);
                    LOWER_LIMIT(new_param.p[i][j][k],0.0);
                    UPPER_LIMIT(new_param.p[i][j][k],1.0);
                }
            }
        }

        if( parameter.pi )
            destroy_vector( parameter.N, parameter.pi );
        if( parameter.gamma )
            destroy_vector( parameter.N, parameter.gamma );
        if( parameter.A )
            destroy_matrix( parameter.N, parameter.N, parameter.A );
        if( parameter.r )
            destroy_matrix( parameter.N, parameter.M, parameter.r );

        if( parameter.p )
            destroy_3d_matrix( parameter.N, parameter.M, parameter.M, parameter.p );

        parameter = new_param;
        param_watcher.removeAllWatches( );

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not open file '%s' for reading",
                                       file_name.c_str( ) );
        return false;
    }
}

bool HMMBatch::load_pi( const std::string &file_name )
{
    unsigned int i;
    Parameter new_param;
    WatchDog<double> param_watcher;
    std::ifstream infile( file_name.c_str( ) );

    if( infile )
    {
        READ(infile,new_param.N);
        LOWER_LIMIT(new_param.N,1);

        new_param.pi = create_zero_vector( new_param.N );
        param_watcher.addWatch( new_param.pi );

        for( i = 0; i < new_param.N; i++ )
        {
            READ(infile,new_param.pi[i]);
            LOWER_LIMIT(new_param.pi[i],0.0);
            UPPER_LIMIT(new_param.pi[i],1.0);
        }

        if( !is_vector_stochastic( new_param.N, new_param.pi ) )
        {
            Terminal::displayWarningMessage(
                "initial state definition is not a valid distribution" );
            Terminal::displayWarningMessage(
                "you may want to normalize vector pi after loading" );
        }

        if( parameter.pi )
            destroy_vector( parameter.N, parameter.pi );

        parameter.pi = new_param.pi;
        param_watcher.removeAllWatches( );

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not open file '%s' for reading",
                                       file_name.c_str( ) );
        return false;
    }
}

bool HMMBatch::load_A( const std::string &file_name )
{
    unsigned int i, j;
    Parameter new_param;
    WatchDog<double> param_watcher;
    std::ifstream infile( file_name.c_str( ) );

    if( infile )
    {
        READ(infile,new_param.N);
        LOWER_LIMIT(new_param.N,1);

        new_param.A = create_zero_matrix( new_param.N, new_param.N );
        param_watcher.addWatch( new_param.A, new_param.N );

        for( i = 0; i < new_param.N; i++ )
        {
            for( j = 0; j < new_param.N; j++ )
            {
                READ(infile,new_param.A[i][j]);
                LOWER_LIMIT(new_param.A[i][j],0.0);
                UPPER_LIMIT(new_param.A[i][j],1.0);
            }
        }

        if( !is_matrix_stochastic( new_param.N, new_param.N, new_param.A ) )
        {
            Terminal::displayWarningMessage(
                "state transitions definition is not a stochastic matrix" );
            Terminal::displayWarningMessage(
                "you may want to normalize matrix A after loading" );
        }

        if( parameter.A )
            destroy_matrix( parameter.N, parameter.N, parameter.A );

        parameter.A = new_param.A;
        param_watcher.removeAllWatches( );

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not open file '%s' for reading",
                                       file_name.c_str( ) );
        return false;
    }
}

bool HMMBatch::load_obs( const std::string &file_name )
{
    unsigned int i, j, k;
    Parameter new_param;
    WatchDog<double> param_watcher;
    std::ifstream infile( file_name.c_str( ) );

    if( infile )
    {
        READ(infile,new_param.N);
        LOWER_LIMIT(new_param.N,1);

        READ(infile,new_param.M);
        LOWER_LIMIT(new_param.M,1);

        new_param.r = create_zero_matrix( new_param.N, new_param.M );
        param_watcher.addWatch( new_param.r, new_param.N );

        new_param.p = create_zero_3d_matrix( new_param.N, new_param.M, new_param.M );
        for( i = 0; i < new_param.N; i++ )
        {
            param_watcher.addWatch( new_param.p[i], new_param.M );
        }

         // r[i][0] r[i][1] .. r[i][M-1] p[i][0][0] p[i][0][1] p[i][0][M-1] p[i][1][0] ... p[i][M-1][M-1]
        for( i = 0; i < new_param.N; i++ )
        {
            // r
            for( j = 0; j < new_param.M; j++ )
            {
                READ(infile,new_param.r[i][j]);
                LOWER_LIMIT(new_param.r[i][j],0.0);
                UPPER_LIMIT(new_param.r[i][j],1.0);
            }

            // p
            for( j = 0; j < new_param.M; j++ )
            {
                for( k = 0; k < new_param.M; k++ )
                {
                    READ(infile,new_param.p[i][j][k]);
                    LOWER_LIMIT(new_param.p[i][j][k],0.0);
                    UPPER_LIMIT(new_param.p[i][j][k],1.0);
                }
            }
        }

        if( parameter.r )
            destroy_matrix( parameter.N, parameter.M, parameter.r );

        if( parameter.p )
            destroy_3d_matrix( parameter.N, parameter.M, parameter.M, parameter.p );

        parameter.r = new_param.r;
        parameter.p = new_param.p;        
        param_watcher.removeAllWatches( );

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not open file '%s' for reading",
                                       file_name.c_str( ) );
        return false;
    }
}

bool HMMBatch::load( Arguments & args )
{
    std::string filename( args[0].text.c_str( ) );
    std::string option;
    
    if( args.size( ) == 1 )
    {
        if( !load_all( filename ) )
            return false;
    }
    else if( args.size( ) == 2 ) /* Loading specific parameters */
    {
        std::string option( args[1].text.c_str( ) );
        
        if( option.compare( "all" ) == 0 )
        {
            if( !load_all( filename ) ) /* Loading all parameters */
                return false;
        }
        else if( option.compare( "pi" ) == 0 ) /* Loading parameter pi */
        {
            if( !load_pi( filename ) )
                return false;
        }
        else if( option.compare( "A" ) == 0 ) /* Loading parameter A */
        {
            if( !load_A( filename ) )
                return false;
        }
        else if( option.compare( "obs" ) == 0 ) /* Loading parameter obs */
        {
            if( !load_obs( filename ) )
                return false;
        }
        else
        {
            Terminal::displayErrorMessage( "(Object %s) load option %s not found. Available options are: {all, pi, A, obs}",
                                            object_name.c_str( ), option.c_str( ) );
            return false;
        }
    }
    else
        return false;

    Terminal::displayMessage( "Hmm_Batch_Variable was succefully loaded." );

    return true;
}

bool HMMBatch::save_all( const std::string &file_name )
{
    unsigned int i, j, k;
    std::ofstream outfile( file_name.c_str( ) );

    outfile.setf( std::ios_base::scientific, std::ios_base::floatfield );
    outfile.setf( std::ios_base::uppercase );
    outfile.precision( 10 );

    if( outfile )
    {
        WRITE(outfile,parameter.N);
        outfile << '\n';
        WRITE(outfile,parameter.M);
        outfile << '\n';

        for( i = 0; i < parameter.N; i++ )
        {
            WRITE(outfile,parameter.pi[i]);
            outfile << '\n';
        }

        for( i = 0; i < parameter.N; i++ )
        {
            WRITE(outfile,parameter.A[i][0]);
            for( j = 1; j < parameter.N; j++ )
            {
                outfile << ' ';
                WRITE(outfile,parameter.A[i][j]);
            }
            outfile << '\n';
        }

        // r[i][0] r[i][1] .. r[i][M-1] p[i][0][0] p[i][0][1] p[i][0][M-1] p[i][1][0] ... p[i][M-1][M-1]
        for( i = 0; i < parameter.N; i++ )
        {
            // r
            for( j = 0; j < parameter.M; j++ )
            {
                WRITE(outfile,parameter.r[i][j]);
                outfile << ' ';
            }
            // p
            for( j = 0; j < parameter.M; j++ )
            {
                for( k = 0; k < parameter.M; k++ )
                {
                    WRITE(outfile,parameter.p[i][j][k]);
                    outfile << ' ';
                }
            }
            outfile << '\n';
        }

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not open file '%s' for writing",
                                       file_name.c_str( ) );
        return false;
    }
}

bool HMMBatch::save_pi( const std::string &file_name )
{
    unsigned int i;
    std::ofstream outfile( file_name.c_str( ) );

    outfile.setf( std::ios_base::scientific, std::ios_base::floatfield );
    outfile.setf( std::ios_base::uppercase );
    outfile.precision( 10 );

    if( outfile )
    {
        WRITE(outfile,parameter.N);
        outfile << '\n';

        for( i = 0; i < parameter.N; i++ )
        {
            WRITE(outfile,parameter.pi[i]);
            outfile << '\n';
        }

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not open file '%s' for writing",
                                       file_name.c_str( ) );
        return false;
    }
}

bool HMMBatch::save_A( const std::string &file_name )
{
    unsigned int i, j;
    std::ofstream outfile( file_name.c_str( ) );

    outfile.setf( std::ios_base::scientific, std::ios_base::floatfield );
    outfile.setf( std::ios_base::uppercase );
    outfile.precision( 10 );

    if( outfile )
    {
        WRITE(outfile,parameter.N);
        outfile << '\n';

        for( i = 0; i < parameter.N; i++ )
        {
            WRITE(outfile,parameter.A[i][0]);
            for( j = 1; j < parameter.N; j++ )
            {
                outfile << ' ';
                WRITE(outfile,parameter.A[i][j]);
            }
            outfile << '\n';
        }

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not open file '%s' for writing",
                                       file_name.c_str( ) );
        return false;
    }
}

bool HMMBatch::save_obs( const std::string &file_name )
{
    unsigned int i, j, k;
    std::ofstream outfile( file_name.c_str( ) );

    outfile.setf( std::ios_base::scientific, std::ios_base::floatfield );
    outfile.setf( std::ios_base::uppercase );
    outfile.precision( 10 );

    if( outfile )
    {
        WRITE(outfile,parameter.N);
        outfile << '\n';
        WRITE(outfile,parameter.M);
        outfile << '\n';

        // r[i][0] r[i][1] .. r[i][M-1] p[i][0][0] p[i][0][1] p[i][0][M-1] p[i][1][0] ... p[i][M-1][M-1]
        for( i = 0; i < parameter.N; i++ )
        {
            // r
            for( j = 0; j < parameter.M; j++ )
            {
                WRITE(outfile,parameter.r[i][j]);
                outfile << ' ';
            }
            // p
            for( j = 0; j < parameter.M; j++ )
            {
                for( k = 0; k < parameter.M; k++ )
                {
                    WRITE(outfile,parameter.p[i][j][k]);
                    outfile << ' ';
                }
            }
            outfile << '\n';
        }

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not open file '%s' for writing",
                                       file_name.c_str( ) );
        return false;
    }
}

bool HMMBatch::save( Arguments & args )
{
    std::string filename( args[0].text.c_str( ) );
    std::string option;
    
    if( args.size( ) == 1 )
    {
        if( !save_all( filename ) )
            return false;
    }
    else if( args.size( ) == 2 ) /* Saving specific parameters */
    {
        std::string option( args[1].text.c_str( ) );
        
        if( option.compare( "all" ) == 0 )
        {
            if( !save_all( filename ) ) /* Saving all parameters */
                return false;
        }
        else if( option.compare( "pi" ) == 0 ) /* Saving parameter pi */
        {
            if( !save_pi( filename ) )
                return false;
        }
        else if( option.compare( "A" ) == 0 ) /* Saving parameter A */
        {
            if( !save_A( filename ) )
                return false;
        }
        else if( option.compare( "obs" ) == 0 ) /* Saving parameter obs */
        {
            if( !save_obs( filename ) )
                return false;
        }
        else
        {
            Terminal::displayErrorMessage( "(Object %s) save option %s not found. Available options are: {all, pi, A, obs}",
                                            object_name.c_str( ), option.c_str( ) );
            return false;
        }
    }
    else
        return false;

    Terminal::displayMessage( "Hmm_Batch was succefully saved in file %s.", filename.c_str( ) );

    return true;
}

/**
 * Import parameters from a Tangram2 model description
 */
bool HMMBatch::import_from_tangram( Arguments &args )
{
    register int   i, pos;
    char           obj_name[ MAX_PATH ],
                 **hidden_st_var_name,
                 **internal_st_var_name,
                  *prefix_name,
                   input_name[ MAX_PATH ];
    int            hidden_num_st_var,
                   internal_num_st_var,
                   dimension;
    HiddenModel   *hidden_model;
    Terminal      *terminal;

    Terminal::displayMessage( "\nLoading parameters from Tangram-II..." );

    // Read mandatory parameters
    strcpy( obj_name,
            removeFileNameExtension( (char *)args[0].text.c_str() ) );
    hidden_num_st_var    = atol( args[1].text.c_str() );
    internal_num_st_var  = atol( args[2].text.c_str() );

    hidden_st_var_name   = (char **)malloc( hidden_num_st_var
                                          * sizeof( char * ) );
    internal_st_var_name = (char **)malloc( internal_num_st_var
                                          * sizeof( char * ) );
    // Check parameters consistency
    if( (int)args.size() - 3 != hidden_num_st_var + internal_num_st_var )
    {
        Terminal::displayErrorMessage( "Invalid number of state variables" );
        return false;
    }
    
    // Read state variable names
    pos = 3;
    for( i = 0; i < hidden_num_st_var; i++ )
    {
        hidden_st_var_name[i]   = (char *)malloc( MAX_PATH * sizeof( char ) );
        strcpy( hidden_st_var_name[i],   args[ pos++ ].text.c_str() );
    }   
    for( i = 0; i < internal_num_st_var; i++ )
    {
        internal_st_var_name[i] = (char *)malloc( MAX_PATH * sizeof( char ) );
        strcpy( internal_st_var_name[i], args[ pos++ ].text.c_str() );
    }

    terminal = Terminal::getTerminal( );      

    if( terminal )
    {
        terminal->open();
    
        // Make a permutation to put interest variable
        // in the first tuple positions
        if( !makePermutation( obj_name,
                              hidden_num_st_var,
                              hidden_st_var_name,
                              internal_num_st_var,
                              internal_st_var_name,
                              terminal->getFile( ) ) )
            return false;    

        // Extract transition rate matrices from the model
        hidden_model = extractTransitionMatrices( obj_name,
                                                  hidden_num_st_var,
                                                  internal_num_st_var,
                                                  terminal->getFile( ) );
                                                  
        if( !hidden_model )
            return false; 
            
        // Check if all internal models have the same dimension
        dimension = hidden_model->num_internal_states[0];
        for( i = 1; i < hidden_model->num_hidden_states; i++ )
        {
            if( hidden_model->num_internal_states[i] != dimension )
            {
                Terminal::displayErrorMessage( "Internal models with different "
                                               "dimensions! Aborting..." );
                return false;                
            }
        }

        // Reset parameters
        destroy_vector( parameter.N, parameter.pi );
        destroy_matrix( parameter.N, parameter.N, parameter.A );        
        destroy_matrix( parameter.N, parameter.M, parameter.r );
        destroy_3d_matrix( parameter.N, parameter.M, parameter.M, parameter.p );

        parameter.N  = hidden_model->num_hidden_states;     
        parameter.M  = hidden_model->num_internal_states[0];  
        parameter.pi = create_full_vector( parameter.N );        
        parameter.A  = create_zero_matrix( parameter.N, parameter.N );
        parameter.r  = create_zero_matrix( parameter.N, parameter.M );      
        parameter.p  = create_zero_3d_matrix( parameter.N,
                                             parameter.M,
                                             parameter.M );
        // Load hidden states parameters
        prefix_name = removeFileNameExtension(
                            hidden_model->hidden_transitions_file );
        sprintf( input_name, "%s%s", prefix_name, EXT_TRANSITION_PROB_MATRIX );
        import_sparse_A( input_name );
        

        for( i = 0; i < hidden_model->num_hidden_states; i++ )
        {
            // Load internal states initial probabilities
            prefix_name = removeFileNameExtension(
                                hidden_model->internal_initial_prob_file[i] );
            sprintf( input_name, "%s%s", prefix_name,
                     EXT_INITIAL_PROBABILITIES );

            import_sparse_initial_prob( i, input_name );

            // Load internal transition matrices
            prefix_name = removeFileNameExtension(
                                hidden_model->internal_transitions_file[i] );
            sprintf( input_name, "%s%s", prefix_name,
                     EXT_TRANSITION_PROB_MATRIX );
                     
            import_sparse_transitions( i, input_name );
        }        

        Terminal::displayMessage( "Parameters were successfully loaded!" );

        terminal->close();
    }
    else
    {
        Terminal::displayErrorMessage( "could not get a valid terminal" );
        return false;
    }    
    
    return true;
}

/**
 * Reads hidden model transition matrix from Tangram2 probabilities 'file_name'
 */
bool HMMBatch::import_sparse_A( char *file_name )
{
    int               state_a, state_b;    
    double            prob;  
    WatchDog<double>  param_watcher;
    FILE             *fd;     

    fd = fopen( file_name, "r" );

    if( fd )
    {
        param_watcher.addWatch( parameter.A, parameter.N );
                
        // Read transitions from file
        while( fscanf( fd, "%d %d %lf\n", &state_a, &state_b, &prob ) == 3 )
        {
            // Tangram states start from 1
            state_a--;
            state_b--;

            parameter.A[ state_a ][ state_b ] = prob;
            LOWER_LIMIT( parameter.A[ state_a ][ state_b ], 0.0 );
            UPPER_LIMIT( parameter.A[ state_a ][ state_b ], 1.0 );
        }        

        param_watcher.removeAllWatches( );

        return true;
    }
    else
    {
        printFileOpenError( 'r', file_name );

        return false;       
    }  
}

/**
 * Reads hidden_state's internal states initial probabilities from 'file_name'
 */
bool HMMBatch::import_sparse_initial_prob( int   hidden_state,
                                           char *file_name )
{
    int               internal_state;    
    double            prob;  
    WatchDog<double>  param_watcher;
    FILE             *fd;     

    fd = fopen( file_name, "r" );

    if( fd )
    {
        param_watcher.addWatch( parameter.r, parameter.N );
                
        // Read transitions from file
        while( fscanf( fd, "%d %lf\n", &internal_state, &prob ) == 2 )
        {
            // Tangram states start from 1
            internal_state--;

            parameter.r[ hidden_state ][ internal_state ] = prob;
            LOWER_LIMIT( parameter.r[ hidden_state ][ internal_state ], 0.0 );
            UPPER_LIMIT( parameter.r[ hidden_state ][ internal_state ], 1.0 );
        }        

        param_watcher.removeAllWatches( );

        return true;
    }
    else
    {
        printFileOpenError( 'r', file_name );

        return false;       
    }    
}

/**
 * Reads hidden_state's internal states transition probabilities
 * from 'file_name'
 */
bool HMMBatch::import_sparse_transitions( int   hidden_state,
                                          char *file_name )
{
    int               internal_state_a, internal_state_b;    
    register int      i;
    double            prob;  
    WatchDog<double>  param_watcher;
    FILE             *fd;     

    fd = fopen( file_name, "r" );

    if( fd )
    {
        for( i = 0; i < (int)parameter.N; i++ )
            param_watcher.addWatch( parameter.p[i], parameter.M );
                
        // Read transitions from file
        while( fscanf( fd, "%d %d %lf\n",
                       &internal_state_a,
                       &internal_state_b,
                       &prob ) == 3 )
        {
            // Tangram states start from 1
            internal_state_a--;
            internal_state_b--;

            parameter.p[ hidden_state     ]
                       [ internal_state_a ]
                       [ internal_state_b ] = prob;
            LOWER_LIMIT( parameter.p[ hidden_state     ]
                                    [ internal_state_a ]
                                    [ internal_state_b ], 0.0 );
            UPPER_LIMIT( parameter.p[ hidden_state     ]
                                    [ internal_state_a ]
                                    [ internal_state_b ], 1.0 );
        }        

        param_watcher.removeAllWatches( );

        return true;
    }
    else
    {
        printFileOpenError( 'r', file_name );

        return false;       
    }   
} 
