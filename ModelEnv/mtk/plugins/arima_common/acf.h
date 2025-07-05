#ifndef ACF_H
#define ACF_H

#include <stdio.h>
#include "sample.h"

#ifdef __cplusplus
extern "C" {
#endif

void acf(     int, int, FILE *, FloatSample * );
void int_acf( int, int, FILE *, Sample *      );

#ifdef __cplusplus
};
#endif

#endif /* ACF_H */
