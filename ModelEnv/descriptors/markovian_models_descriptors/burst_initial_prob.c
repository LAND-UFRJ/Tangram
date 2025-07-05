/*
 * Copyright (C) 1999-2009, Edmundo Albuquerque de Souza e Silva.
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE.QPL included in the packaging of this file.
 *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* This file contains functions to calculate burst initial probabilities */
/* of burst states of a Markov chain.                                    */
#include <stdio.h>
#include <stdlib.h>

#include "define.h"

double get_matrix_position();  /* in module Matrix.c */

double *pi_cond_non_burst();
double *calc_init_burst();


/* Get probabilities of non-burst states. */
double *pi_cond_non_burst(prb, reward, num_states, TRIGGER)
double *prb;
double *reward;
int    num_states;
double TRIGGER;
{
    int    i;
    double *pi_non_burst;


    pi_non_burst = (double *)calloc(num_states + 1, sizeof(double));

    for( i = 1; i <= num_states; i++ )
    {
#ifdef DEBUG
        printf( "Rwd= %f  Pi= %f\n", reward[ i - 1 ], prb[ i ] );
#endif

        if( reward[ i - 1 ] <= TRIGGER )
            pi_non_burst[ i ] = prb[ i ];
    }

#ifdef DEBUG
    for( i = 1; i <= num_states; i++ )
        printf( "pi_non_burst[%d] = %f\n", i, pi_non_burst[ i ] );
#endif

    return pi_non_burst;
}


/* Calculate burst state probabilities. */
double *calc_init_burst( mat, num_states, pi_non_burst )
Matrix *mat;
int    num_states;
double *pi_non_burst;
{
    int    i, j;
    double *pi_init_burst;
    double *pi_i;
    double sum_pi, p_ij;

    pi_init_burst = (double *)calloc( num_states + 1, sizeof( double ) );
    pi_i = (double *)calloc( num_states + 1, sizeof( double ) );


    /* Computa soma das linhas para estados non-bursty.*/
    for( i = 1; i <= num_states; i++ )
        for( j = 1; j <= num_states; j++ )
            /* Testa para ver se e' prob(non_burst,burst) */
            if( pi_non_burst[ i ] && !pi_non_burst[ j ] )
            {
                p_ij = get_matrix_position( i, j, mat );
                pi_i[ i ] += p_ij;
            }

#ifdef DEBUG
    /****
    for (i = 1; i <= num_states; i++)
        printf("pi_i[%d] = %f\n", i, pi_i[i]);
    ****/
#endif

    sum_pi = 0.0;
    for( i = 1; i <= num_states; i++ )
        for( j = 1; j <= num_states; j++ )
            if( pi_non_burst[ i ] && !pi_non_burst[ j ] && pi_i[ i ] )
            {
               p_ij = get_matrix_position( i, j, mat );
               pi_init_burst[ j ] += (pi_non_burst[ i ] * (p_ij / pi_i[ i ]));

               sum_pi += (pi_non_burst[ i ] * (p_ij / pi_i[ i ]));

#ifdef DEBUG
               /*****
               printf("%d %d pi_i[%d]=%f pij=%f pi_init[%d]=%f\n", i, j, i,
                                          pi_i[i], p_ij, j, pi_init_burst[j]);
               *****/
#endif
            }

#ifdef DEBUG
    printf( "sum=%f\n", sum_pi );
#endif

    for( j = 1; j <= num_states; j++ )
    {
        pi_init_burst[ j ] /= sum_pi;

        /* Adjust vector to be zero based. */
        pi_init_burst[ j - 1 ] = pi_init_burst[ j ];
    }

    return pi_init_burst;
}
