#ifndef PARAM_H
#define PARAM_H

typedef struct
{
    unsigned int N, M;
    double * pi;
    double ** A;
    double ** B;
    double * gamma;
} Parameter;

void get_aggregate_matrix( Parameter *param, double ** B );

#endif /* PARAM_H */
