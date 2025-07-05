#include "hmm.h"

#include "matrix.h"

bool HMM::set_N( const std::string & value, const Index & idx )
{
    unsigned int new_N;

    new_N = atol( value.c_str( ) );

    if( new_N > 0 )
    {
        if( new_N != parameter.N )
        {
            parameter.pi = resize_vector( parameter.N, parameter.pi, new_N );
            parameter.A  = resize_matrix( parameter.N, parameter.N, parameter.A,
                                          new_N, new_N );
            parameter.B  = resize_matrix( parameter.N, parameter.M, parameter.B,
                                          new_N, parameter.M );

            parameter.N = new_N;
        }
        return true;
    }
    else
    {
        Terminal::displayErrorMessage(
                            "number of states must be a positive integer" );
        return false;
    }
}

bool HMM::get_N( std::string & value, const Index & idx )
{
    char buffer[32];

    sprintf( buffer, "%d", parameter.N );
    value = buffer;

    return true;
}

bool HMM::set_M( const std::string & value, const Index & idx )
{
    unsigned int new_M;

    new_M = atol( value.c_str( ) );

    if( new_M > 0 )
    {
        if( new_M != parameter.M )
        {
            parameter.B = resize_matrix( parameter.N, parameter.M, parameter.B,
                                         parameter.N, new_M );

            parameter.M = new_M;
        }
        return true;
    }
    else
    {
        Terminal::displayErrorMessage(
                            "number of states must be a positive integer" );
        return false;
    }
}

bool HMM::get_M( std::string & value, const Index & idx )
{
    char buffer[32];

    sprintf( buffer, "%d", parameter.M );
    value = buffer;

    return true;
}

bool HMM::set_pi( const std::string & value, const Index & idx )
{
    unsigned int i;
    double new_pi_value;

    i = idx.front( );

    if( i < parameter.N )
    {
        new_pi_value = atof( value.c_str( ) );
        parameter.pi[i] = new_pi_value;
        return true;
    }
    else
    {
        Terminal::displayErrorMessage(
                        "index must be smaller than number of hidden states" );
        return false;
    }
}

bool HMM::get_pi( std::string & value, const Index & idx )
{
    char buffer[32];
    unsigned int i;

    i = idx.front( );

    if( i < parameter.N )
    {
        sprintf( buffer, "%.10e", parameter.pi[i] );
        value = buffer;
        return true;
    }
    else
    {
        Terminal::displayErrorMessage(
                        "index must be smaller than number of hidden states" );
        return false;
    }
}

bool HMM::set_A( const std::string & value, const Index & idx )
{
    unsigned int i, j;
    double new_A_value;
    Index::const_iterator it;

    it = idx.begin( );
    i = *it;
    it++;
    j = *it;

    if( i < parameter.N && j < parameter.N )
    {
        new_A_value = atof( value.c_str( ) );
        parameter.A[i][j] = new_A_value;
        return true;
    }
    else
    {
        Terminal::displayErrorMessage(
                 "both indices must be smaller than number of hidden states" );
        return false;
    }
}

bool HMM::get_A( std::string & value, const Index & idx )
{
    char buffer[32];
    unsigned int i, j;
    Index::const_iterator it;

    it = idx.begin( );
    i = *it;
    it++;
    j = *it;

    if( i < parameter.N && j < parameter.N )
    {
        sprintf( buffer, "%.10e", parameter.A[i][j] );
        value = buffer;
        return true;
    }
    else
    {
        Terminal::displayErrorMessage(
                 "both indices must be smaller than number of hidden states" );
        return false;
    }
}

bool HMM::set_B( const std::string & value, const Index & idx )
{
    unsigned int i, j;
    double new_B_value;
    Index::const_iterator it;

    it = idx.begin( );
    i = *it;
    it++;
    j = *it;

    if( i < parameter.N && j < parameter.M )
    {
        new_B_value = atof( value.c_str( ) );
        parameter.B[i][j] = new_B_value;
        return true;
    }
    else
    {
        if( i >= parameter.N )
            Terminal::displayErrorMessage(
                 "first index must be smaller than number of hidden states" );
        else if( j >= parameter.M )
            Terminal::displayErrorMessage(
                 "second index must be smaller than number of symbols" );
        return false;
    }
}

bool HMM::get_B( std::string & value, const Index & idx )
{
    char buffer[32];
    unsigned int i, j;
    Index::const_iterator it;

    it = idx.begin( );
    i = *it;
    it++;
    j = *it;

    if( i < parameter.N && j < parameter.M )
    {
        sprintf( buffer, "%.10e", parameter.B[i][j] );
        value = buffer;
        return true;
    }
    else
    {
        if( i >= parameter.N )
            Terminal::displayErrorMessage(
                 "first index must be smaller than number of hidden states" );
        else if( j >= parameter.M )
            Terminal::displayErrorMessage(
                 "second index must be smaller than number of symbols" );
        return false;
    }
}

bool HMM::set_result( const std::string & value, const Index & idx )
{
    unsigned int i;

    i = idx.front( );
    if( i >= 0 )
    {
        // Fill empty positions
        if( i+1 > result.size() )
            result.resize( i+1, 0.0 );

        result[i] = (double) atof( value.c_str( ) );
        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "index must be positive" );
        return false;
    }    
}

bool HMM::get_result( std::string & value, const Index & idx )
{
    char buffer[50];
    unsigned int i;

    i = idx.front( );

    if( i < result.size() )
    {
        sprintf( buffer, "%.10e", result[i] );
        value = buffer;
        return true;
    }
    else
    {
        sprintf( buffer, "index must be smaller than %d", result.size() - 1 );
        Terminal::displayErrorMessage( buffer );
        return false;
    }
}
