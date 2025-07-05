#ifndef STEADY_H
#define STEADY_H

#include <stdio.h>

#include "param.h"

#ifdef __cplusplus
extern "C" {
#endif

double  sym_time_average( Parameter *, FILE * );
double *ss_burst_size( Parameter *, int, double, int, FILE * );

#ifdef __cplusplus
};
#endif

#endif /* STEADY_H */
