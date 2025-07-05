#ifndef TRAIN_H
#define TRAIN_H

#include <stdio.h>

#include "param.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int training_flag;

double *likelihood( Parameter *, int, int *, FILE * );
double *complete_likelihood( Parameter *, int, int *, int *, FILE * );
double *train( Parameter *, int, int *, int, double, FILE * );
int train_complete( Parameter *, int, int *, int *, FILE * );

#ifdef __cplusplus
};
#endif

#endif /* TRAIN_H */
