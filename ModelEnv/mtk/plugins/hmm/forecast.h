#ifndef FORECAST_H
#define FORECAST_H

#include <stdio.h>

#include "param.h"
#include "sample.h"

#ifdef __cplusplus
extern "C" {
#endif

int forecast( Parameter *, Sample *, Sample *, int, FILE *  );

#ifdef __cplusplus
};
#endif

#endif /* FORECAST_H */
