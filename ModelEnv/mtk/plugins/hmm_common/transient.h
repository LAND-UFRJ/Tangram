#ifndef TRANSIENT_H
#define TRANSIENT_H

#include <stdio.h>

#include "param.h"

#ifdef __cplusplus
extern "C" {
#endif

double *symbol_sum_distribution( Parameter *, int, int, int *, FILE * );
double *tr_burst_size( Parameter *, int, int, int *, int, double, FILE * );

double *state_probability( Parameter *, int, int *, FILE * );

#ifdef __cplusplus
};
#endif

#endif /* TRANSIENT_H */
