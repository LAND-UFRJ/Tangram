#ifndef BATCHES_H
#define BATCHES_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Tamanho do Batch
 */

#include "param.h"

typedef struct
{
    int index; /* index of the first symbol in the batch */
    int size; /* the batch size */
} Batch;

extern int num_batches;
extern Batch * batches;

void analize_batch( Parameter *, int, int * );

#ifdef __cplusplus
};
#endif

#endif /* BATCHES_H */
