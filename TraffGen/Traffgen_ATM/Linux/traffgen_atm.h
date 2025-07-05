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
	
	Magnos Martinello
	Traffic Generator with ATM support
	LAND - Labor. de Analise e Desempenho - UFRJ/COPPE/NCE
        Last Update: 06/06/2000

******************************************************************************/

#ifndef __TRAFFGEN_H__
#define __TRAFFGEN_H__


/* general defines */
#define TRUE  (1)
#define FALSE (0)
#define TOTALNUMPACKET    10000 /* total number of generated packets */
#define MAXSTRING         255
#define MINERROR          20
#define MINTIMESLOT       20000


/* mode of generation */
#define CONTIN    1
#define SPREAD    2 
#define SPRCONT   3

/* traffic source types */
#define TRACE  1
#define DET    2
#define MARKOV 3

/* Structures for describes the traffic source */

/* type of a element in the matrix */
typedef struct element {
    double val;
    uint    st;
    struct element *next;
} t_elem;

typedef struct {
    double  interval; /* expressed in useconds */
    double  data;
} t_det;

typedef struct {
    char   filename[MAXSTRING];
    double data_unit;
} t_file;

typedef struct {
    double    data_unit;
    uint       no_states;
    double   *rate_vec;
    double   *unif_vec;
    t_elem   *Q;
    char     *reward_file;
    char     *model_file;
} t_markov;


typedef union {   
    t_det    det;
    t_file   file;
    t_markov markov;
} t_source;


/* Structure to setup the connection with ATM network*/

typedef struct {
  int       vpi;          /* Virtual path identifier */
  int       vci;          /* Virtual circuit identifier */
  double    peak;         /* Peak cell rate */
  double    mean;         /* Mean cell rate */ 
  double    mbs;          /* Maximum burst size */
} t_traff;


/* prototype of a particular get_time() in usec */
inline u_longlong_t get_time( void );

/* Prototype of general functions */
void usage(char *);
int parse_conf( int );
int parse_args(char **);
#endif /* __TRAFFGEN_H__ */
