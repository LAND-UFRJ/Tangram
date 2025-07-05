#include "tangram2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MODEL_NAME    "t2model"
#define MATRIX_FILE   MODEL_NAME ".generator_mtx"
#define SS_FILE       MODEL_NAME ".SS.gth"
#define SOLV_BINARY  "solv"
#define COMMAND_LINE SOLV_BINARY " " MODEL_NAME " 5 2> /dev/null"

#define TOLERANCE 0.9

int get_steady_state_prob( int N, double ** P, double * pi )
{
    FILE * fp;
    int ret;
    int i, j;
    double sum;

    fp = fopen( MATRIX_FILE, "w" );

    if( !fp )
    {
        fprintf( stderr, "could not create matrix file\n" );
        perror( "fopen" );
        return -1;
    }

    /* de-uniformize matrix P with rate 1 into file */
    fprintf( fp, "%d\n", N );
    for( i = 0; i < N; i++ )
    {
        for( j = 0; j < N; j++ )
        {
            if( P[i][j] > 0.0 )
                fprintf( fp, "%d %d %.10e\n", i+1, j+1, P[i][j] );
        }
    }
    fclose( fp );

    ret = system( COMMAND_LINE );
    remove( MATRIX_FILE );

    if( ret < 0 )
    {
        fprintf( stderr, "solv error!\n" );
        return -1;
    }

    fp = fopen( SS_FILE, "r" );

    if( !fp )
    {
        fprintf( stderr, "could not create SS file\n" );
        perror( "fopen" );
        return -1;
    }

    /* skip two comment lines */
    fscanf( fp, "#%*[^\n]\n" );
    fscanf( fp, "#%*[^\n]\n" );

    /* read probability vector */
    for( i = 0; i < N; i++ )
        fscanf( fp, "%*d %lf\n", pi+i );

    fclose( fp );

    remove( SS_FILE );

    sum = 0.0;
    for( i = 0; i < N; i++ )
        sum += pi[i];

    if( sum < TOLERANCE )
    {
        for( i = 0; i < N; i++ )
            pi[i] = 1.0;
        sum = N;
    }

    for( i = 0; i < N; i++ )
        pi[i] /= sum;

    return 0;
}
