#include "matrix.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TOLERANCE 1e-5

double * copy_vector( unsigned int n, double * source )
{
    double * vector;
    int i;

    vector = malloc( n * sizeof(double) );

    for( i = 0; i < n; i++ )
        vector[i] = source[i];

    return vector;
}

double * resize_vector( unsigned int n, double * vector, unsigned int new_n )
{
    double * old_vector;
    int i;

    if( n != new_n )
    {
        old_vector = vector;
        vector = (double *)malloc( new_n * sizeof(double) );

        for( i = 0; i < n && i < new_n; i++ )
            vector[i] = old_vector[i];

        for( ; i < new_n; i++ )
            vector[i] = 0.0;

        destroy_vector( n, old_vector );
    }

    return vector;
}

void destroy_vector( unsigned int n, double * vector )
{
    if( vector )
        free( vector );
}

void normalize_vector( unsigned int n, double * vector )
{
    int i;
    double sum;

    for( sum = 0.0, i = 0; i < n; i++ )
    {
        sum += vector[i];
    }

    if( sum != 0.0 )
        for( i = 0; i < n; i++ )
            vector[i] /= sum;
}

int is_vector_stochastic( unsigned int n, double * vector )
{
    int i;
    double sum;

    for( sum = 0.0, i = 0; i < n; i++ )
    {
        if( vector[i] < 0.0 || vector[i] > 1.0 )
            return 0;
        sum += vector[i];
    }

    return (fabs(sum - 1.0) < TOLERANCE);
}

double ** copy_matrix( unsigned int n, unsigned int m, double ** source )
{
    double ** matrix;
    int i;

    matrix = malloc( n * sizeof(double *) );

    for( i = 0; i < n; i++ )
        matrix[i] = copy_vector( m, source[i] );

    return matrix;
}

double ** resize_matrix( unsigned int n, unsigned int m, double ** matrix,
                         unsigned int new_n, unsigned new_m )
{
    double ** old_matrix;
    int i;

    if( m != new_m )
        for( i = 0; i < n && i < new_n; i++ )
            matrix[i] = resize_vector( m, matrix[i], new_m );

    if( n != new_n )
    {
        old_matrix = matrix;
        matrix = (double **)malloc( new_n * sizeof(double *) );

        for( i = 0; i < n && i < new_n; i++ )
            matrix[i] = old_matrix[i];

        for( ; i < new_n; i++ )
            matrix[i] = create_zero_vector( new_m );

        for( ; i < n; i++ )
            destroy_vector( m, old_matrix[i] );

        free( old_matrix );
    }

    return matrix;
}

void destroy_matrix( unsigned int n, unsigned int m, double ** matrix )
{
    int i;

    if( matrix )
    {
        for( i = 0; i < n; i++ )
            destroy_vector( m, matrix[i] );

        free( matrix );
    }
}

void normalize_matrix( unsigned int n, unsigned int m, double ** matrix )
{
    int i;

    for( i = 0; i < n; i++ )
    {
        normalize_vector( m, matrix[i] );
    }
}

int is_matrix_stochastic( unsigned int n, unsigned int m, double ** matrix )
{
    int i;

    for( i = 0; i < n; i++ )
    {
        if( !is_vector_stochastic( m, matrix[i] ) )
            return 0;
    }

    return 1;
}

double * create_zero_vector( unsigned int n )
{
    double * vector;

    vector = malloc( n * sizeof(double) );
    memset( vector, 0, n * sizeof(double) );

    return vector;
}

double * create_full_vector( unsigned int n )
{
    double * vector;
    double sum;
    int i;

    vector = malloc( n * sizeof(double) );

    for( sum = 0.0, i = 0; i < n; i++ )
    {
        vector[i] = drand48( );
        sum += vector[i];
    }

    for( i = 0; i < n; i++ )
        vector[i] /= sum;

    return vector;
}

double * create_probability_vector( unsigned int n )
{
    double * vector;
    double sum;
    int i;

    vector = malloc( n * sizeof(double) );

    for( sum = 0.0, i = 0; i < n; i++ )
        vector[i] = drand48( );

    return vector;
}

double ** create_zero_matrix( unsigned int n, unsigned int m )
{
    double ** matrix;
    int i;

    matrix = malloc( n * sizeof(double *) );

    for( i = 0; i < n; i++ )
        matrix[i] = create_zero_vector( m );

    return matrix;
}

double ** create_full_matrix( unsigned int n, unsigned int m )
{
    double ** matrix;
    int i;

    matrix = malloc( n * sizeof(double *) );

    for( i = 0; i < n; i++ )
        matrix[i] = create_full_vector( m );

    return matrix;
}

double ** create_identity_matrix( unsigned int n )
{
    double ** matrix;
    int i;

    matrix = malloc( n * sizeof( double * ) );
    
    for( i = 0; i < n; i++ )
    {
        matrix[i]    = create_zero_vector( n );
        matrix[i][i] = 1.0;        
    }
    
    return matrix;
}

double ** create_coxian_matrix( unsigned int n )
{
    double ** matrix;
    int i;

    matrix = create_zero_matrix( n, n );
    
    for( i = 0; i < n - 1; i++ )
    {
        matrix[i][0]   = drand48();
        matrix[i][i+1] = drand48();    
    }        
    matrix[i][0] = 1.0;
    
    normalize_matrix( n, n, matrix );

    return matrix;
}

double ** create_qbd_matrix( unsigned int G, unsigned int E,
                             unsigned int B, unsigned int F )
{
    double ** matrix;
    int n,
        se, de, /* src and dst element */
        sg, dg; /* src and dst group */

    n = G * E;
    matrix = create_zero_matrix( n, n );

    /* Fill transition matrix */
    for( sg = 0; sg < G; sg++ )
    {
        for( se = 0; se < E; se++ )
        {
            for( de = se + 1; de < E; de++ )
            {
                matrix[ sg * E + se ][ sg * E + de ] = drand48();
                matrix[ sg * E + de ][ sg * E + se ] = drand48();
            }
            matrix[ sg * E + se ][ sg * E + se ] = drand48();
        }

        for( dg = sg + 1; (dg <= sg + F) && (dg < G); dg++ )
        {
            for( se = 0; se < E; se++ )
                for( de = 0; de < E; de++ )
                    matrix[ sg * E + se ][ dg * E + de ] = drand48();
        }

        for( dg = sg - 1; (dg >= sg - B) && (dg >= 0); dg-- )
        {
            for( se = 0; se < E; se++ )
                for( de = 0; de < E; de++ )
                    matrix[ sg * E + se ][ dg * E + de ] = drand48();
        }
    }

    normalize_matrix( n, n, matrix );

    return matrix;
}

double *** create_zero_3d_matrix( unsigned int n, unsigned int m, unsigned int q )
{
    double *** matrix;
    int i;

    matrix = malloc( n * sizeof(double **) );

    for( i = 0; i < n; i++ )
        matrix[i] = create_zero_matrix( m, q );

    return matrix;
}

double *** create_full_3d_matrix( unsigned int n, unsigned int m, unsigned int q )
{
    double *** matrix;
    int i;

    matrix = malloc( n * sizeof(double **) );

    for( i = 0; i < n; i++ )
        matrix[i] = create_full_matrix( m, q );

    return matrix;
}

void destroy_3d_matrix( unsigned int n, unsigned int m, unsigned int q, double *** matrix )
{
    int i;

    if( matrix )
    {
        for( i = 0; i < n; i++ )
            destroy_matrix( m, q, matrix[i] );

        free( matrix );
    }
}

double *** copy_3d_matrix( unsigned int n, unsigned int m, unsigned int q, double *** source )
{
    double *** matrix;
    int i;

    matrix = malloc( n * sizeof(double **) );

    for( i = 0; i < n; i++ )
        matrix[i] = copy_matrix( m, q, source[i] );

    return matrix;
}

double *** resize_3d_matrix( unsigned int n, unsigned int m, unsigned int q, double *** matrix,
                             unsigned int new_n, unsigned new_m, unsigned int new_q )
{
    double *** old_matrix;
    int i;

    // m or q changed
    if( m != new_m || q != new_q )
    {
        for( i = 0; i < n && i < new_n; i++ )
        {
            matrix[i] = resize_matrix( m, q, matrix[i], new_m, new_q );
        }
    }

    // n changed
    if( n != new_n )
    {
        old_matrix = matrix;
        matrix = (double ***)malloc( new_n * sizeof(double **) );

        for( i = 0; i < n && i < new_n; i++ )
            matrix[i] = old_matrix[i];

        for( ; i < new_n; i++ )
            matrix[i] = create_zero_matrix( new_m, new_q );

        for( ; i < n; i++ )
            destroy_matrix( m, q, old_matrix[i] );

        free( old_matrix );
    }

    return matrix;
}

