#ifndef TRAIN_FAST_H
#define TRAIN_FAST_H

#include <stdio.h>

#include "param.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int fast_training_flag;

double *likelihood_fast( Parameter *, int, int *, FILE * );
double *train_fast( Parameter *, int, int *, int, double, FILE * );

#ifdef __cplusplus
};
#endif

#endif /* TRAIN_FAST_H */
