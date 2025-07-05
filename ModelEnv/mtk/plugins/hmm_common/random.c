#include "random.h"

#include <stdlib.h>
#include <stdio.h>

int get_random( int n, double * dist )
{
    int i;
    double sum, x;

    sum = 0.0;
    x = drand48( );

    for( i = 0; i < n; i++ )
    {
        sum += dist[i];
        if( x < sum )
            return i;
    }

    return n-1;
}
