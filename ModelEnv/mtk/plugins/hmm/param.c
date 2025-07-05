#include "param.h"

void get_aggregate_matrix( Parameter *param, double ** B )
{
    unsigned int i, j;

    for( i = 0; i < param->N; i++ )
        for( j = 0; j < param->M; j++ )
            B[i][j] = param->B[i][j];
}
