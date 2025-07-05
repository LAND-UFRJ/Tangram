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

/******************************************************************************
	
	Author: Antonio Augusto Rocha
	Tool: Traffic Generator
	LAND - Labor. de Analise e Desempenho - UFRJ/COPPE/COS
        Last Update: 30/07/2002

******************************************************************************/

#ifndef __PACKLOSS_LOC_H__
#define __PACKLOSS_LOC_H__

/* general defines */
#define TRUE  (1)
#define FALSE (0)
#define MAXSTRING         255

/* traffic source types */
#define TRACE  1
#define DET    2
#define MARKOV 3

/* Number of control packets to terminate the session */
#define TRYING  3

/* protocols */
#define UNI    1
#define MULT   2

/* mode of generation */
#define CONTIN   1
#define CONTIN2  2
#define SPREAD   3

/* type of a element in the matrix */
typedef struct element {
    double val;
    int    st;
    struct element *next;
} t_elem;

typedef struct {
    double  interval; /* expressed in usec */
    double  data;
} t_det;

typedef struct {
    char   filename[MAXSTRING];
    double data_unit;
} t_file;


typedef struct {
  double    data_unit;
  int       no_states;
  double   *rate_vec;
  double   *unif_vec;
  t_elem   *Q;
  char     *model_file;
  char     *reward_file;
} t_markov;


typedef union {   
  t_det    det;
  t_file   file;
  t_markov markov;
} t_source;


/* Prototype of general functions */
int usage(char *);
int parse_conf( int );
int parse_args(char **);

#endif /* __PACKLOSS_LOC_H__ */
