#include "ghmm.h"

#include "matrix.h"

bool GHMM::set_N( const std::string & value, const Index & idx )
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
            parameter.p  = resize_vector( parameter.N, parameter.p, new_N );
            parameter.q  = resize_vector( parameter.N, parameter.q, new_N );
            parameter.r  = resize_vector( parameter.N, parameter.r, new_N );

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

bool GHMM::get_N( std::string & value, const Index & idx )
{
    char buffer[32];

    sprintf( buffer, "%d", parameter.N );
    value = buffer;

    return true;
}

bool GHMM::set_B( const std::string & value, const Index & idx )
{
    unsigned int new_B;

    new_B = atol( value.c_str( ) );

    if( new_B > 0 )
    {
        if( new_B != parameter.B )
            parameter.B = new_B;

        return true;
    }
    else
    {
        Terminal::displayErrorMessage(
                            "number of states must be a positive integer" );
        return false;
    }
}

bool GHMM::get_B( std::string & value, const Index & idx )
{
    char buffer[32];

    sprintf( buffer, "%d", parameter.B );
    value = buffer;

    return true;
}

bool GHMM::set_pi( const std::string & value, const Index & idx )
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

bool GHMM::get_pi( std::string & value, const Index & idx )
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

bool GHMM::set_A( const std::string & value, const Index & idx )
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

bool GHMM::get_A( std::string & value, const Index & idx )
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

bool GHMM::set_r( const std::string & value, const Index & idx )
{
    unsigned int i;
    double new_r_value;

    i = idx.front( );

    if( i < parameter.N )
    {
        new_r_value = atof( value.c_str( ) );

        if( new_r_value >= 0.0 && new_r_value <= 1.0 )
        {
            parameter.r[i] = new_r_value;
            return true;
        }
        else
        {
            Terminal::displayErrorMessage(
                                   "value for r[%d] must be within [0,1]", i );
            return false;
        }
    }
    else
    {
        Terminal::displayErrorMessage(
                        "index must be smaller than number of hidden states" );
        return false;
    }
}

bool GHMM::get_r( std::string & value, const Index & idx )
{
    char buffer[32];
    unsigned int i;

    i = idx.front( );

    if( i < parameter.N )
    {
        sprintf( buffer, "%.10e", parameter.r[i] );
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

bool GHMM::set_p( const std::string & value, const Index & idx )
{
    unsigned int i;
    double new_p_value;

    i = idx.front( );

    if( i < parameter.N )
    {
        new_p_value = atof( value.c_str( ) );

        if( new_p_value >= 0.0 && new_p_value <= 1.0 )
        {
            parameter.p[i] = new_p_value;
            return true;
        }
        else
        {
            Terminal::displayErrorMessage(
                                   "value for p[%d] must be within [0,1]", i );
            return false;
        }
    }
    else
    {
        Terminal::displayErrorMessage(
                        "index must be smaller than number of hidden states" );
        return false;
    }
}

bool GHMM::get_p( std::string & value, const Index & idx )
{
    char buffer[32];
    unsigned int i;

    i = idx.front( );

    if( i < parameter.N )
    {
        sprintf( buffer, "%.10e", parameter.p[i] );
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

bool GHMM::set_q( const std::string & value, const Index & idx )
{
    unsigned int i;
    double new_q_value;

    i = idx.front( );

    if( i < parameter.N )
    {
        new_q_value = atof( value.c_str( ) );

        if( new_q_value >= 0.0 && new_q_value <= 1.0 )
        {
            parameter.q[i] = new_q_value;
            return true;
        }
        else
        {
            Terminal::displayErrorMessage(
                                   "value for q[%d] must be within [0,1]", i );
            return false;
        }
    }
    else
    {
        Terminal::displayErrorMessage(
                        "index must be smaller than number of hidden states" );
        return false;
    }
}

bool GHMM::get_q( std::string & value, const Index & idx )
{
    char buffer[32];
    unsigned int i;

    i = idx.front( );

    if( i < parameter.N )
    {
        sprintf( buffer, "%.10e", parameter.q[i] );
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

bool GHMM::set_result( const std::string & value, const Index & idx )
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

bool GHMM::get_result( std::string & value, const Index & idx )
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
