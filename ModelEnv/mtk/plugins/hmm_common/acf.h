#ifndef ACF_H
#define ACF_H

#include <stdio.h>

#include "param.h"

#ifdef __cplusplus
extern "C" {
#endif

int loss_autocorrelation( Parameter *, double, int, FILE * );

#ifdef __cplusplus
};
#endif

#endif /* ACF_H */
