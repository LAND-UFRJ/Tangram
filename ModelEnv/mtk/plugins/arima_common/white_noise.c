#include "white_noise.h"

#include <math.h>
#include <stdlib.h>

/**
 * Generates a Gaussian sample
 */
double white_noise( double mean, double variance )
{
    double v1, v2, w, gaussian_sample;

    do
    {
        v1 = 2 * drand48() - 1;
        v2 = 2 * drand48() - 1;
        w  = v1 * v1 + v2 * v2;
    } while( w > 1 );

    // Gaussian random variable (0,1)
    gaussian_sample = v1 * ( sqrt( -2 * ( log( w ) / w ) ) );

    // Gaussian random variable (mean, variance)
    gaussian_sample = mean + sqrt( variance ) * gaussian_sample;

    return gaussian_sample;
}
