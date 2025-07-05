#ifndef VITERBI_H
#define VITERBI_H

#include <stdio.h>

#include "param.h"

#ifdef __cplusplus
extern "C" {
#endif

double *viterbi( Parameter *, int, int *, int *, FILE * );

#ifdef __cplusplus
};
#endif

#endif /* VITERBI_H */
