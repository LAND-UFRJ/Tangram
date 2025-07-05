#include "steady.h"

#include <stdlib.h>
#include <math.h>

#include "tangram2.h"
#include "matrix.h"
#include "forward.h"

/**
  Calculates the expected value of a symbol emission, i.e.,
  computes the average, in time, of the symbol emission
  process.
**/
double *sym_time_average( Parameter * param, FILE * output )
{
    double * pi;
    double s_tavg;
    int i, j;
    double avg;
    double *result;

    pi  = create_zero_vector( param->N );

    if( get_steady_state_prob( param->N, param->A, pi ) < 0 )
    {
        fprintf( output, "# could not calculate steady state probabilities\n" );
        destroy_vector( param->N, pi );
        return NULL;
    }

    s_tavg = 0.0;
    for( i = 0; i < param->N; i++ )
    {
        avg = 0.0;
        for( j = 0; j < param->M; j++ )
            avg += j * param->B[i][j];

        s_tavg += pi[i] * avg;
    }

    fprintf( output, "# Symbol Emission Time Average (when t tends to infinity)\n" );
    fprintf( output, "%.10e\n", s_tavg );

    destroy_vector( param->N, pi  );

    result = (double *)malloc( sizeof( double ) );
    result[0] = s_tavg;
    
    return result;
}
