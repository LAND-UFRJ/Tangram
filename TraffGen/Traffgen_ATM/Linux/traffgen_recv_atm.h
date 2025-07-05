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

#ifndef __TRAFFGEN_RECV_H__
#define __TRAFFGEN_RECV_H__

/* Size of store trace vector */
#define SIZEVEC   10000

typedef struct {
    int       vpi;          /* Virtual path identifier */
    int       vci;          /* Virtual circuit identifier */
    double    peak;         /* Peak cell rate */
    double    mean;         /* Mean cell rate */ 
    double    mbs;          /* Maximum burst size */
} t_traff;

/* Structure to store the arrived packets */
typedef struct{
    u_longlong_t   nseq;                  /* Sequence number */
    char           type;                  /* Type of packet  */
    u_longlong_t   timestamp_tx;          /* Timestamp in usec */
    u_longlong_t   timestamp_rx;          /* Timestamp in usec */
} t_store;


/* prototype of a particular get_time() in usec */
inline u_longlong_t get_time( void );

/* Prototype of general functions */
void usage(char *);
int parse_conf( int );
int parse_args(char **);

#endif /* __TRAFFGEN_RECV_H__ */
