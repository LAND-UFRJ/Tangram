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

#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <stdio.h>
#include <stdlib.h>

#include <globals.h>
#include <data_queue.h>

#define DEFAULT_INTERVAL         50000
#define DEFAULT_GRID_INTERVAL    10.0
#define DEFAULT_BUFFER_SIZE      1000
#define DEFAULT_SAMPLES_PER_READ 5
#define DEFAULT_WINDOW_WIDTH     600
#define DEFAULT_WINDOW_HEIGHT    200
#define DEFAULT_X_MIN            0.0
#define DEFAULT_X_MAX            1.0
#define DEFAULT_Y_MIN            0.0
#define DEFAULT_Y_MAX            1.0

typedef struct
{
    int         n_files;
    char     ** file_name;
    FILE     ** file;
    DataQueue * queue;

    int         interval;
    double      grid_interval;
    int         buffer_size;
    int         samples_per_read;
    int         window_width;
    int         window_height;

    int         limit_x_range;
    double      x_range;
    double      x_min;
    double      x_max;

    int         limit_y_range;
    double      y_min;
    double      y_max;

    int         steps;

    int         quit;
} Options;

void parse_args( int, char **, Options * );

#endif /* __OPTIONS_H__ */
