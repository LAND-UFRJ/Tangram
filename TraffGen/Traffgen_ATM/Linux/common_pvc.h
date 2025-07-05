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
        Last Update: 28/04/2000

******************************************************************************/

#ifndef __COMMON_PVC__
#define __COMMON_PVC__


#ifndef __GENPACKET__
#define __GENPACKET__

/* Constants to the generation of packets of data and finalize*/
#define END 'E'
#define DATA 'D'


typedef unsigned long long int u_longlong_t;


/* Class of services */
#define CBR    1
#define UBR    2
#define ABR    3
#define VBR    4


#define MINPACKSIZE  32
#define MAXPACKSIZE  ( (64*1024) - MINPACKSIZE )

typedef struct{
    u_longlong_t   nseq;                  /* Sequence number */
    char           type;                  /* Type of packet  */
    u_longlong_t   timestamp_tx;          /* Timestamp in msec */
    u_longlong_t   timestamp_rx;          /* Timestamp in msec */
    char           payload[MAXPACKSIZE];  /* payload */
} t_gen_packet;


#endif


int open_device();
void clean_up(int );
void construct_pvc_options(int,int);
int pvc_connect(int, int, int);
int pvc_disconnect(int);
int pvc_read(int , char *, int);
int pvc_write(int , char *, int);

#endif

