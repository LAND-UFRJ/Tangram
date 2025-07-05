#include "ghmm.h"

#include "matrix.h"

bool GHMM::set_full( Arguments & args )
{
    destroy_vector( parameter.N, parameter.pi );       
    destroy_matrix( parameter.N, parameter.N, parameter.A );
    destroy_vector( parameter.N, parameter.r );
    destroy_vector( parameter.N, parameter.p );
    destroy_vector( parameter.N, parameter.q );
        
    parameter.N = atol( args[0].text.c_str( ) );    
    parameter.B = atol( args[1].text.c_str( ) );
        
    parameter.pi = create_full_vector( parameter.N );
    parameter.A  = create_full_matrix( parameter.N, parameter.N );

    parameter.p = create_probability_vector( parameter.N );
    parameter.q = create_probability_vector( parameter.N );
    parameter.r = create_probability_vector( parameter.N );
    
    return true;
}    

bool GHMM::set_coxian( Arguments & args )
{
    destroy_vector( parameter.N, parameter.pi );       
    destroy_matrix( parameter.N, parameter.N, parameter.A );
    destroy_vector( parameter.N, parameter.r );
    destroy_vector( parameter.N, parameter.p );
    destroy_vector( parameter.N, parameter.q );
    
    parameter.N = atol( args[0].text.c_str( ) );    
    parameter.B = atol( args[1].text.c_str( ) );

    parameter.pi = create_full_vector( parameter.N );

    parameter.A = create_coxian_matrix( parameter.N );
    parameter.p = create_probability_vector( parameter.N );
    parameter.q = create_probability_vector( parameter.N );
    parameter.r = create_probability_vector( parameter.N );

    return true;
}

bool GHMM::set_qbd( Arguments & args )
{
    int n_groups, n_elem, back, fwd;  

    destroy_vector( parameter.N, parameter.pi );       
    destroy_matrix( parameter.N, parameter.N, parameter.A );
    destroy_vector( parameter.N, parameter.r );
    destroy_vector( parameter.N, parameter.p );
    destroy_vector( parameter.N, parameter.q );

    /* Initialize parameters */
    n_groups  = atol( args[0].text.c_str( ) );
    n_elem    = atol( args[1].text.c_str( ) );
    
    parameter.N = n_groups * n_elem;  
    parameter.B = atol( args[2].text.c_str( ) );

    back = fwd = 1;
    if( args.size() > 3 )
        back = atol( args[3].text.c_str( ) );
    if( args.size() > 4 )
        fwd  = atol( args[4].text.c_str( ) );        

    /* Recreate structures */
    parameter.pi = create_full_vector( parameter.N );    
    parameter.A  = create_qbd_matrix ( n_groups, n_elem, back, fwd );
    parameter.p  = create_probability_vector( parameter.N );
    parameter.q  = create_probability_vector( parameter.N );
    parameter.r  = create_probability_vector( parameter.N );
    
    return true;   
}

void GHMM::fix_obs( void )
{
    unsigned int i;

    for( i = 0; i < parameter.N; i++ )
    {
        parameter.p[i] = parameter.p[i] += epsilon;
        parameter.q[i] = parameter.q[i] += epsilon;
        parameter.r[i] = parameter.r[i] += epsilon;
    }
    
    normalize_vector( parameter.N, parameter.p );
    normalize_vector( parameter.N, parameter.q );
    normalize_vector( parameter.N, parameter.r );
}

void GHMM::fix_qbd_matrix( int G, int E,
                           int B, int F )
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


bool GHMM::fix_full( Arguments & args )
{
    unsigned int i, j;

    i = atol( args[0].text.c_str( ) );
    j = atol( args[1].text.c_str( ) );

    if( i != parameter.N || j != parameter.B )
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

bool GHMM::fix_coxian( Arguments & args )
{
    unsigned int i, j;

    i = atol( args[0].text.c_str( ) );
    j = atol( args[1].text.c_str( ) );

    if( i != parameter.N || j != parameter.B )
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

bool GHMM::fix_qbd( Arguments & args )
{
    int n_groups, n_elem, back, fwd;  
    unsigned int i, j;

    /* Initialize parameters */
    n_groups  = atol( args[0].text.c_str( ) );
    n_elem    = atol( args[1].text.c_str( ) );

    i = n_groups * n_elem;
    j = atol( args[2].text.c_str( ) );
    if( i != parameter.N ||
        j != parameter.B )
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

bool GHMM::set_epsilon( Arguments & args )
{
    epsilon = atof( args[0].text.c_str( ) );

    return true;
}
