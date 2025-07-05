#ifndef FORWARD_H
#define FORWARD_H

#include <stdio.h>

#include "param.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ALPHA(t,i) (alpha[t][i])
#define SCALE(t)   (scale[t])

/* all functions below return 0 on success and <0 on failure */

int forward_filter( Parameter * param, double * pi0,   /* model parameters */
                    int n_obs, int * obs,              /* input trace */
                    double ** alpha, double * scale,
                    double * ll, double * final_pi );  /* optional outputs */

#ifdef __cplusplus
};
#endif

#endif /* FORWARD_H */
