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

#include <data_queue.h>

#include <stdio.h>
#include <stdlib.h>

int create_queue( DataQueue * queue, int capacity )
{
    if( !queue || capacity < 1 )
        return -1;

    queue->samples = (struct point *)malloc( capacity * sizeof(struct point) );
    if( !queue->samples )
        return -1;

    queue->capacity = capacity;
    queue->head = -1;
    queue->tail = -1;

    return 0;
}

int push_into_queue( DataQueue * queue, struct point p )
{
    if( !queue )
        return -1;

    if( queue->head == -1 )
    {
        /* First insertion */
        queue->head = 0;
        queue->tail = 1;
        queue->samples[queue->head] = p;
    }
    else
    {
        int last;

        last = (queue->tail+queue->capacity-1)%queue->capacity;
        /* Cannot insert a point before the last one in X */
        if( p.x < queue->samples[last].x )
            return -1;

        queue->samples[queue->tail] = p;

        /* If queue is full, override head */
        if( queue->head == queue->tail )
        {
            queue->head = (queue->head+1)%queue->capacity;
            queue->tail = (queue->tail+1)%queue->capacity;
        }
        else
            queue->tail = (queue->tail+1)%queue->capacity;
    }

    return 0;
}

int destroy_queue( DataQueue * queue )
{
    if( !queue || queue->samples )
        return -1;

    queue->capacity = 0;
    queue->head = -1;
    queue->tail = -1;
    free( queue->samples );
    queue->samples = NULL;

    return 0;
}
