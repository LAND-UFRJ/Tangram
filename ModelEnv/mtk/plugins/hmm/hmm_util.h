#ifndef HMM_UTIL_H
#define HMM_UTIL_H

#include "param.h"

double Eval_Alpha( Parameter *, int *, int, double **, double * );
double Update_Alpha( Parameter *, int *, int, int, double **, double * );
void   Eval_Beta( Parameter *, int *, int, double **, double * );
double Eval_LogProbSeq( int, double * );

#endif /* HMM_UTIL_H */
