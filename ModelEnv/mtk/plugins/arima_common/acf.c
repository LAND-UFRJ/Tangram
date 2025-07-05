#include "acf.h"

#include <stdlib.h>

/**
 * Calculates the autocorrelation function
 */
void acf( int h, int start, FILE *output, FloatSample *sample )
{
    int          n = sample->size;
    register int i, j;
    double       mean, gama_0, rho;
  
    if( start + h >= n )
        fprintf( output, "the specified lag goes beyond the trace size.\n" );
    else
    {
        /* Evaluate mean */
        mean = 0.0;
        for( i = start; i < n; i++ )
            mean += sample->data[i];
        mean /= n;

        /* Calculate gama_0 */
        gama_0 = 0.0;
        for( i = start; i < n; i++ )
            gama_0 += ( sample->data[i] - mean ) * ( sample->data[i] - mean );

        /* Calculate autocorrelation vector */
        fprintf( output, "h\trho(h)\n" );
        for( i = 1; i <= h; i++ )
        {
            rho = 0.0;
            
            for( j = start + 1; j < n - i; j++ )
            {
                rho += ( sample->data[ j + i ] - mean )
                    *  ( sample->data[ j     ] - mean );
            }
            rho /= gama_0;
            
            fprintf( output, "%d\t% lf\n", i, rho );
        }
    }
}

/**
 * Converts Sample into FloatSample and calls acf function
 */
void int_acf( int h, int start, FILE *output, Sample *sample )
{
    register int i;  
  
    FloatSample float_sample;

    float_sample.size = sample->size;
    float_sample.data = (double *)malloc( sample->size * sizeof( double ) );

    for( i = 0; i < sample->size; i++ )
        float_sample.data[i] = (double)sample->data[i];

    acf( h, start, output, &float_sample );
    
    if( float_sample.data )
        free( float_sample.data );
}
