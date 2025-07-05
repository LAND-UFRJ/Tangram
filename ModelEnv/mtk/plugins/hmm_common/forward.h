#ifndef FORWARD_H
#define FORWARD_H

#include <stdio.h>

#include "param.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ALPHA(t,i) (alpha[t][i])
#define SCALE(t)   (scale[t])

#define TOLERANCE 1e-6 

/* all functions below return 0 on success and < 0 on failure */

int forward_filter( Parameter * param, double * pi0,   /* model parameters */
                    int n_obs, int * obs,              /* input trace */
                    double ** alpha, double * scale,
                    double * ll, double * final_pi );  /* optional outputs */

double *forward_test( Parameter * param, double * pi0, /* model parameters */
                      int symbol,                      /* symbol to test */
                      int max_size, double threshold,  /* stop conditions */
                      FILE * output );                 /* output file */

#ifdef __cplusplus
};
#endif

#endif /* FORWARD_H */
