#include "batches.h"

#include <stdlib.h>

int num_batches;
Batch * batches;

/**********************************************************************
* Retorna o num de batches e preenche estrutura com dados dos batches *
* Escreve na variavel global                                          *
**********************************************************************/
void analize_batch( Parameter * param, int n_obs, int * obs )
{
    int nb, i, last_batch;

    nb = 0;
    last_batch = 0;
    for( i = 0; i < n_obs; i++ )
    {
        /* Quit observation is always the last one */
        if( obs[i] == param->M - 1 )
        {
            nb++;
            batches = (Batch *)realloc( batches, nb * sizeof( Batch ) );
            batches[nb-1].size = i - last_batch + 1;
            batches[nb-1].index = last_batch;
            last_batch = i + 1; // next batch
        }
    }

    num_batches = nb;
}
