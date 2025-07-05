#ifndef BACKWARD_H
#define BACKWARD_H

#include <stdio.h>

#include "param.h"
#include "forward.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BETA(t,i) (beta[t][i])

/* all functions below return 0 on success and <0 on failure */
int backward_filter( Parameter * param,                 /* model parameters */
                     int n_obs, int * obs,              /* input trace */
                     double * scale,
                     double ** beta );

#ifdef __cplusplus
};
#endif

#endif /* BACKWARD_H */
