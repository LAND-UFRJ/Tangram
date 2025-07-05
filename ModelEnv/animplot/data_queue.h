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

#ifndef __DATA_QUEUE_H__
#define __DATA_QUEUE_H__

#include <globals.h>

typedef struct
{
    int capacity; /* max number of samples */
    int head;     /* the next to be removed */
    int tail;     /* the next to be inserted */
    struct point * samples;
} DataQueue;

int create_queue( DataQueue *, int );
int push_into_queue( DataQueue *, struct point );
int destroy_queue( DataQueue * );

#define GET_ELEMENT(q,n) ((q).samples[((q).head+(n))%(q).capacity])
#define QUEUE_SIZE(q)    ((q).head != (q).tail ? \
        (((q).tail+(q).capacity-(q).head)%(q).capacity) :\
        ((q).head == -1 ? 0: (q).capacity))

#define GET_FIRST(q)     ((q).samples[(q).head%(q).capacity])
#define GET_LAST(q)      ((q).samples[((q).tail+(q).capacity-1)%(q).capacity])

#endif /* __DATA_QUEUE_H__ */
