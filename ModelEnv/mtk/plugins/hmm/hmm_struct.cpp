#include "hmm.h"

#include "matrix.h"

bool HMM::set_full( Arguments & args )
{
    destroy_vector( parameter.N, parameter.pi );       
    destroy_matrix( parameter.N, parameter.N, parameter.A );
    destroy_matrix( parameter.N, parameter.M, parameter.B );
        
    parameter.N = atol( args[0].text.c_str( ) );    
    parameter.M = atol( args[1].text.c_str( ) );
        
    parameter.pi = create_full_vector( parameter.N );
    parameter.A  = create_full_matrix( parameter.N, parameter.N );
    parameter.B  = create_full_matrix( parameter.N, parameter.M );
    
    return true;
}    

bool HMM::set_coxian( Arguments & args )
{
    destroy_vector( parameter.N, parameter.pi );       
    destroy_matrix( parameter.N, parameter.N, parameter.A );
    destroy_matrix( parameter.N, parameter.M, parameter.B );
    
    parameter.N = atol( args[0].text.c_str( ) );
    parameter.M = atol( args[1].text.c_str( ) );

    parameter.pi = create_full_vector  ( parameter.N );
    parameter.A  = create_coxian_matrix( parameter.N );
    parameter.B  = create_full_matrix  ( parameter.N, parameter.M );

    return true;
}

bool HMM::set_qbd( Arguments & args )
{
    int n_groups, n_elem, back, fwd;  

    destroy_vector( parameter.N, parameter.pi );       
    destroy_matrix( parameter.N, parameter.N, parameter.A );
    destroy_matrix( parameter.N, parameter.M, parameter.B );

    /* Initialize parameters */
    n_groups  = atol( args[0].text.c_str( ) );
    n_elem    = atol( args[1].text.c_str( ) );
    
    parameter.N = n_groups * n_elem;  
    parameter.M = atol( args[2].text.c_str( ) );

    back = fwd = 1;
    if( args.size() > 3 )
        back = atol( args[3].text.c_str( ) );
    if( args.size() > 4 )
        fwd  = atol( args[4].text.c_str( ) );        

    /* Recreate structures */
    parameter.pi = create_full_vector( parameter.N );    
    parameter.A  = create_qbd_matrix ( n_groups, n_elem, back, fwd );
    parameter.B  = create_full_matrix( parameter.N, parameter.M );
    
    return true;   
}

bool HMM::set_gilbert( Arguments & args )
{
    destroy_vector( parameter.N, parameter.pi );       
    destroy_matrix( parameter.N, parameter.N, parameter.A );
    destroy_matrix( parameter.N, parameter.M, parameter.B );
   
    parameter.N = 2;
    parameter.M = 2;

    parameter.pi = create_full_vector( parameter.N );
    parameter.A  = create_full_matrix( parameter.N, parameter.N );
    parameter.B  = create_identity_matrix( parameter.M );

    return true;
}

void HMM::fix_obs( void )
{
    unsigned int i, j;

    for( i = 0; i < parameter.N; i++ )
    {
        for( j = 0; j < parameter.M; j++ )
            parameter.B[i][j] += epsilon;
    }

    normalize_matrix( parameter.N, parameter.M, parameter.B );
}

void HMM::fix_qbd_matrix( int G, int E, int B, int F )
{
    int n,
        se, de, /* src and dst element */
        sg, dg; /* src and dst group */

    n = G * E;

    /* Fill transition matrix */
    for( sg = 0; sg < G; sg++ )
    {
        for( se = 0; se < E; se++ )
        {
            for( de = se + 1; de < E; de++ )
            {
                parameter.A[ sg * E + se ][ sg * E + de ] += epsilon;
                parameter.A[ sg * E + de ][ sg * E + se ] += epsilon;
            }
            parameter.A[ sg * E + se ][ sg * E + se ] += epsilon;
        }

        for( dg = sg + 1; (dg <= sg + F) && (dg < G); dg++ )
        {
            for( se = 0; se < E; se++ )
                for( de = 0; de < E; de++ )
                    parameter.A[ sg * E + se ][ dg * E + de ] += epsilon;
        }

        for( dg = sg - 1; (dg >= sg - B) && (dg >= 0); dg-- )
        {
            for( se = 0; se < E; se++ )
                for( de = 0; de < E; de++ )
                    parameter.A[ sg * E + se ][ dg * E + de ] += epsilon;
        }
    }

    normalize_matrix( n, n, parameter.A );
}

bool HMM::fix_full( Arguments & args )
{
    unsigned int i, j;

    i = atol( args[0].text.c_str( ) );
    j = atol( args[1].text.c_str( ) );

    if( i != parameter.N || j != parameter.M )
        return false;

    for( i = 0; i < parameter.N; i++ )
        parameter.pi[i] += epsilon;

    normalize_vector( parameter.N, parameter.pi );

    for( i = 0; i < parameter.N; i++ )
    {
        for( j = 0; j < parameter.N; j++ )
            parameter.A[i][j] += epsilon;
    }

    normalize_matrix( parameter.N, parameter.N, parameter.A );

    fix_obs( );

    return true;
}    

bool HMM::fix_coxian( Arguments & args )
{
    unsigned int i, j;

    i = atol( args[0].text.c_str( ) );
    j = atol( args[1].text.c_str( ) );

    if( i != parameter.N || j != parameter.M )
        return false;

    for( i = 0; i < parameter.N; i++ )
        parameter.pi[i] += epsilon;

    normalize_vector( parameter.N, parameter.pi );

    for( i = 0; i < parameter.N-1; i++ )
    {
        parameter.A[i][0]   += epsilon;
        parameter.A[i][i+1] += epsilon;
    }

    parameter.A[i][0] = 1.0;

    normalize_matrix( parameter.N, parameter.N, parameter.A );

    fix_obs( );

    return true;
}

bool HMM::fix_qbd( Arguments & args )
{
    int n_groups, n_elem, back, fwd;  
    unsigned int i, j;

    /* Initialize parameters */
    n_groups  = atol( args[0].text.c_str( ) );
    n_elem    = atol( args[1].text.c_str( ) );

    i = n_groups * n_elem;
    j = atol( args[2].text.c_str( ) );
    if( i != parameter.N || j != parameter.M )
        return false;

    back = fwd = 1;
    if( args.size() > 3 )
        back = atol( args[3].text.c_str( ) );
    if( args.size() > 4 )
        fwd  = atol( args[4].text.c_str( ) );        

    /* Recreate structures */
    for( i = 0; i < parameter.N; i++ )
        parameter.pi[i] += epsilon;

    normalize_vector( parameter.N, parameter.pi );

    fix_qbd_matrix( n_groups, n_elem, back, fwd );

    fix_obs( );

    return true;   
}

bool HMM::fix_gilbert( Arguments & args )
{
    unsigned int i, j;

    if( parameter.N != 2 || parameter.M != 2 )
        return false;

    for( i = 0; i < parameter.N; i++ )
        parameter.pi[i] += epsilon;

    normalize_vector( parameter.N, parameter.pi );

    for( i = 0; i < parameter.N; i++ )
    {
        for( j = 0; j < parameter.N; j++ )
            parameter.A[i][j] += epsilon;
    }

    normalize_matrix( parameter.N, parameter.N, parameter.A );

    return true;
}

bool HMM::set_epsilon( Arguments & args )
{
    epsilon = atof( args[0].text.c_str( ) );

    return true;
}
