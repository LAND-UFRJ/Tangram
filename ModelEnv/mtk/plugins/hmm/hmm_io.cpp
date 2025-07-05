#include "hmm.h"

#include <fstream>

#include "matrix.h"
#include "object_io.h"
#include "watchdog.h"

#include "import_from_tangram.h"

bool HMM::load_all( const std::string &file_name )
{
    unsigned int i, j;
    Parameter new_param;
    WatchDog<double> param_watcher;
    std::ifstream infile( file_name.c_str( ) );
    
    if( infile )
    {
        Terminal::displayMessage( "Loading all Hmm parameters" );
        
        /* Loading number of states and number of symbols */
        READ(infile,new_param.N);
        LOWER_LIMIT(new_param.N,1);

        READ(infile,new_param.M);
        LOWER_LIMIT(new_param.M,1);

        /* Loading Pi */
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

        /* Loading A */
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

        /* Loading B */
        new_param.B = create_zero_matrix( new_param.N, new_param.M );
        param_watcher.addWatch( new_param.B, new_param.N );

        for( i = 0; i < new_param.N; i++ )
        {
            for( j = 0; j < new_param.M; j++ )
            {
                READ(infile,new_param.B[i][j]);
                LOWER_LIMIT(new_param.B[i][j],0.0);
                UPPER_LIMIT(new_param.B[i][j],1.0);
            }
        }

        if( !is_matrix_stochastic( new_param.N, new_param.M, new_param.B ) )
        {
            Terminal::displayWarningMessage(
                "symbol observation definition is not a stochastic matrix" );
            Terminal::displayWarningMessage(
                "you may want to normalize matrix B after loading" );
        }

        /* Removing previously existing values */
        if( parameter.pi )
            destroy_vector( parameter.N, parameter.pi );
        if( parameter.gamma )
            destroy_vector( parameter.N, parameter.gamma );
        if( parameter.A )
            destroy_matrix( parameter.N, parameter.N, parameter.A );
        if( parameter.B )
            destroy_matrix( parameter.N, parameter.M, parameter.B );

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

bool HMM::load_pi( const std::string &file_name )
{
    unsigned int i;
    Parameter new_param;
    WatchDog<double> param_watcher;
    std::ifstream infile( file_name.c_str( ) );

    if( infile )
    {
        Terminal::displayMessage( "Loading only Hmm parameter pi" );
        
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

bool HMM::load_A( const std::string &file_name )
{
    unsigned int i, j;
    Parameter new_param;
    WatchDog<double> param_watcher;
    std::ifstream infile( file_name.c_str( ) );

    if( infile )
    {
        Terminal::displayMessage( "Loading only Hmm parameter A" );
        
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

bool HMM::load_B( const std::string &file_name )
{
    unsigned int i, j;
    Parameter new_param;
    WatchDog<double> param_watcher;
    std::ifstream infile( file_name.c_str( ) );

    if( infile )
    {
        Terminal::displayMessage( "Loading only Hmm parameter B" );

        READ(infile,new_param.N);
        LOWER_LIMIT(new_param.N,1);

        READ(infile,new_param.M);
        LOWER_LIMIT(new_param.M,1);

        new_param.B = create_zero_matrix( new_param.N, new_param.M );
        param_watcher.addWatch( new_param.B, new_param.N );

        for( i = 0; i < new_param.N; i++ )
        {
            for( j = 0; j < new_param.M; j++ )
            {
                READ(infile,new_param.B[i][j]);
                LOWER_LIMIT(new_param.B[i][j],0.0);
                UPPER_LIMIT(new_param.B[i][j],1.0);
            }
        }

        if( !is_matrix_stochastic( new_param.N, new_param.M, new_param.B ) )
        {
            Terminal::displayWarningMessage(
                "symbol observation definition is not a stochastic matrix" );
            Terminal::displayWarningMessage(
                "you may want to normalize matrix B after loading" );
        }

        if( parameter.B )
            destroy_matrix( parameter.N, parameter.M, parameter.B );

        parameter.B = new_param.B;
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

bool HMM::load( Arguments & args )
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
        else if( option.compare( "B" ) == 0 ) /* Loading parameter B */
        {
            if( !load_B( filename ) )
                return false;
        }
        else
        {
            Terminal::displayErrorMessage( "(Object %s) load option %s not found. Available options are: {all, pi, A, B}",
                                            object_name.c_str( ), option.c_str( ) );
            return false;
        }
    }
    else
        return false;

    Terminal::displayMessage( "Hmm was successfully loaded." );

    return true;
}

bool HMM::save_all( const std::string &file_name )
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

        for( i = 0; i < parameter.N; i++ )
        {
            WRITE(outfile,parameter.B[i][0]);
            for( j = 1; j < parameter.M; j++ )
            {
                outfile << ' ';
                WRITE(outfile,parameter.B[i][j]);
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

bool HMM::save_pi( const std::string &file_name )
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

bool HMM::save_A( const std::string &file_name )
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

bool HMM::save_B( const std::string &file_name )
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
        WRITE(outfile,parameter.M);
        outfile << '\n';

        for( i = 0; i < parameter.N; i++ )
        {
            WRITE(outfile,parameter.B[i][0]);
            for( j = 1; j < parameter.M; j++ )
            {
                outfile << ' ';
                WRITE(outfile,parameter.B[i][j]);
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

bool HMM::save( Arguments & args )
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
        else if( option.compare( "B" ) == 0 ) /* Saving parameter B */
        {
            if( !save_B( filename ) )
                return false;
        }
        else
        {
            Terminal::displayErrorMessage( "(Object %s) save option %s not found. Available options are: {all, pi, A, B}",
                                            object_name.c_str( ), option.c_str( ) );
            return false;
        }
    }
    else
        return false;

    Terminal::displayMessage( "Hmm was successfully saved in file %s.", filename.c_str( ) );

    return true;
}

/**
 * Import parameters from a Tangram2 model description
 */
bool HMM::import_from_tangram( Arguments &args )
{
    register int   i, pos;
    char           obj_name[ MAX_PATH ],
                 **hidden_st_var_name,
                 **internal_st_var_name,
                  *prefix_name,
                   input_name[ MAX_PATH ];
    int            hidden_num_st_var,
                   internal_num_st_var;
    HiddenModel   *hidden_model;
    Terminal      *terminal;

    Terminal::displayMessage( "\nLoading parameters from Tangram-II..." );
    
    /* Read mandatory parameters */
    strcpy( obj_name,
            removeFileNameExtension( (char *)args[0].text.c_str() ) );
    hidden_num_st_var    = atol( args[1].text.c_str() );
    internal_num_st_var  = 0; /* HMM has no internal states */

    hidden_st_var_name   = (char **)malloc( hidden_num_st_var
                                          * sizeof( char * ) );
    internal_st_var_name = (char **)malloc( internal_num_st_var
                                          * sizeof( char * ) );
    // Check parameters consistency
    if( (int)args.size() - 2 != hidden_num_st_var + internal_num_st_var )
    {
        Terminal::displayErrorMessage( "Invalid number of state variables" );
        return false;
    }

    // Read state variable names
    pos = 2;
    for( i = 0; i < hidden_num_st_var; i++ )
    {
        hidden_st_var_name[i]   = (char *)malloc( MAX_PATH * sizeof( char ) );
        strcpy( hidden_st_var_name[i],   args[ pos++ ].text.c_str() );
    }   
    for( i = 0; i < internal_num_st_var; i++ )
    {
        internal_st_var_name[i] = (char *)malloc( MAX_PATH * sizeof( char ) );
        strcpy( internal_st_var_name[i], "\0" );
    }

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open();

        /* Make a permutation to put interest variable
           in the first tuple positions */
        if( !makePermutation( obj_name,
                              hidden_num_st_var,
                              hidden_st_var_name,
                              internal_num_st_var,
                              internal_st_var_name,
                              terminal->getFile( ) ) )
            return false;

        /* Extract transition rate matrices from the model */
        hidden_model = extractTransitionMatrices( obj_name,
                                                  hidden_num_st_var,
                                                  internal_num_st_var,
                                                  terminal->getFile( ) );

        if( !hidden_model )
            return false;

        /* Reset parameters. PI and B will be given random values, for the user
           will specify them later. */
        destroy_vector( parameter.N, parameter.pi );
        destroy_matrix( parameter.N, parameter.N, parameter.A );
        destroy_matrix( parameter.N, parameter.M, parameter.B );

        parameter.N  = hidden_model->num_hidden_states;        
        parameter.pi = create_full_vector( parameter.N );
        parameter.A  = create_zero_matrix( parameter.N, parameter.N );
        parameter.B  = create_full_matrix( parameter.N, parameter.M );

        /* Load hidden states parameters */
        prefix_name = removeFileNameExtension(
                            hidden_model->hidden_transitions_file );
        sprintf( input_name, "%s%s", prefix_name, EXT_TRANSITION_PROB_MATRIX );
        import_sparse_A( input_name );


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
bool HMM::import_sparse_A( char *file_name )
{
    int               state_a, state_b;    
    double            prob;  
    WatchDog<double>  param_watcher;
    FILE             *fd;     

    fd = fopen( file_name, "r" );

    if( fd )
    {
        param_watcher.addWatch( parameter.A, parameter.N );
                
        /* Read transitions from file */
        while( fscanf( fd, "%d %d %lf\n", &state_a, &state_b, &prob ) == 3 )
        {
            /* Tangram states start from 1 */
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
