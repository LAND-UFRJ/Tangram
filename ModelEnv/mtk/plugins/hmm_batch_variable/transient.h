#ifndef TRANSIENT_H
#define TRANSIENT_H

#include <stdio.h>

#include "param.h"

#ifdef __cplusplus
extern "C" {
#endif

double *state_probability( Parameter *, int, int *, FILE * );

#ifdef __cplusplus
};
#endif

#endif /* TRANSIENT_H */
