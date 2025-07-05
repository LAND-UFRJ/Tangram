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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <plot.h>
#include <options.h>

static Graphics  graphics;
static Options   options;
static pthread_t producer_tid;

/* Function to be called at the program's exit */
void exit_handler( void )
{
    fprintf( stderr, "\nClosing animplot\n" );
    if( producer_tid )
    {
        pthread_kill( producer_tid, SIGTERM );
        producer_tid = 0;
    }
}

/* Gets data from input files and put them in the data queue */
void * producer_thread( void * arg )
{
    off_t * old_size;
    struct stat stat_buffer;
    char perror_msg[50], line_buffer[MAX_LINE_SIZE+1], * pt;
    struct point sample;
    int i,
        changed,
        samples_read;
    double x_min, x_max, y_min, y_max;
#ifdef FAIR_READ
    int most_ahead;
#endif

    /* Initialize sizes vector */
    old_size = (off_t *)malloc( options.n_files * sizeof(off_t) );
    if( !old_size )
        return (void *)1;
    for( i = 0; i < options.n_files; i++ )
        old_size[i] = 0;

    /* Loop until the end of the program */
#ifdef FAIR_READ
    most_ahead = 0;
#endif
    while( !options.quit )
    {
        usleep( options.interval );

        /* Test all files for new input */
        x_max = GET_LAST(options.queue[0]).x;
        for( i = 1; i < options.n_files; i++ )
        {
            if( GET_LAST(options.queue[i]).x > x_max )
                x_max = GET_LAST(options.queue[i]).x;
        }

        y_min = options.y_min;
        y_max = options.y_max;
        changed = 0;
        for( i = 0; i < options.n_files; i++ )
        {
#ifdef FAIR_READ
            if( options.n_files > 1 && most_ahead == i )
            {

#ifdef DEBUG
fprintf( stderr, "%5d - Skipping most ahead...\n", i );
#endif
                continue;
            }
#endif

#ifdef DEBUG
fprintf( stderr, "%5d - Polling...\n", i );
#endif

            /* fstat the file */
            if( fstat( fileno( options.file[i] ), &stat_buffer ) < 0 )
            {
                sprintf( perror_msg, "File %s cannot be checked - fstat",
                                      options.file_name[i] );
                perror( perror_msg );
                continue;
            }

            /* If the file hasn't changed its, continue waiting... */
            if( stat_buffer.st_size == old_size[i] )
            {
#ifdef DEBUG
fprintf( stderr, "\tSize unchanged\n" );
#endif
                continue;
            }

            /* If the file got smaller, complain! */
            if( stat_buffer.st_size < old_size[i] )
            {
                fprintf( stderr, "File %s truncated!!!\n",
                         options.file_name[i] );
                old_size[i] = stat_buffer.st_size;
                continue;
            }

            /* Else, the file has grown */
            /* While there is new data in the file */
            fseek( options.file[i], old_size[i], SEEK_SET );
            samples_read = 0;
            while( fgets( line_buffer, MAX_LINE_SIZE, options.file[i] ) )
            {
                /* If we got to the end of file, break */
                if( feof( options.file[i] ) )
                {
#ifdef DEBUG
fprintf( stderr, "\tClearing EOF mark\n" );
#endif
                    clearerr( options.file[i] );
                    break;
                }

                pt = line_buffer;
                while( isspace(*pt) )
                    pt++;
                if( *pt == '#' || *pt == '\n' || *pt == '\0' )
                {
                    old_size[i] = ftell( options.file[i] );
                    continue;
                }
                if( !isdigit(*pt) )
                {
                    old_size[i] = ftell( options.file[i] );
                    fprintf( stderr, "Corrupted data in file %s\n"
                                     "Skipping a line\n",
                             options.file_name[i] );
                    continue;
                }

                /* Read new sample */
                if( sscanf(line_buffer, "%lf %lf", &sample.x, &sample.y) != 2 )
                {
                    fprintf( stderr, "sscanf failed while reading from %s\n",
                             options.file_name[i] );
                    fprintf( stderr, "Data read: %s\n", line_buffer );
                    continue;
                }

                /* Put the sample into the data queue */
                if( push_into_queue( &options.queue[i], sample ) < 0 )
                {
                    fprintf( stderr, "Cannot push into queue after read %s\n",
                             options.file_name[i] );
                    fprintf( stderr, "Data read: %s\n", line_buffer );
                    fprintf( stderr, "Last element in queue: (%lf,%lf)\n",
                             GET_LAST(options.queue[i]).x,
                             GET_LAST(options.queue[i]).y );
                    continue;
                }

                changed = 1;
                samples_read++;

#ifdef DEBUG
fprintf( stderr, "\tGot sample (%f,%f)\n", sample.x, sample.y );
#endif

                if( !options.limit_y_range )
                {
                    if( sample.y < y_min )
                        y_min = sample.y;
                    if( sample.y > y_max )
                        y_max = sample.y;
                }

                /* Update file size */
                old_size[i] = ftell( options.file[i] );

#ifdef DEBUG
fprintf( stderr, "%5d - Testing for the most ahead... - %.2lf >? %.2lf\n",
         i, sample.x, x_max );
#endif

#ifdef FAIR_READ
                if( options.n_files > 1 && sample.x > x_max )
                {
#ifdef DEBUG
fprintf( stderr, "%5d BECOMES THE MOST AHEAD...\n", i );
#endif
                    x_max = sample.x;
                    most_ahead = i;
                    break;
                }
#endif

                if( !(samples_read < options.samples_per_read) )
                    break;
            }

        }

        if( changed )
        {
            x_min = GET_FIRST(options.queue[0]).x;
            x_max = GET_LAST(options.queue[0]).x;
            for( i = 1; i < options.n_files; i++ )
            {
                if( GET_FIRST(options.queue[i]).x > x_min )
                    x_min = GET_FIRST(options.queue[i]).x;
                if( GET_LAST(options.queue[i]).x > x_max )
                    x_max = GET_LAST(options.queue[i]).x;
            }

            /* Update logical area limits */
            if( options.limit_x_range )
            {
                if( x_max > x_min+options.x_range )
                {
                    options.x_min = x_max - options.x_range;
                    options.x_max = x_max;
                }
                else
                {
                    options.x_min = x_min;
                    options.x_max = x_min + options.x_range;
                }
            }
            else
            {
                options.x_min = x_min;
                options.x_max = x_max;
            }

            options.y_min = y_min;
            options.y_max = y_max;


#ifdef DEBUG
fprintf( stderr, "New limits (%f,%f)x(%f,%f)\n",
         options.x_min, options.y_min,
         options.x_max, options.y_max );
#endif

            /* Invalidate the display in order to redraw */
            invalidate_graphics( &graphics );
        }
    }

    return (void *)0;
}

int main( int argc, char ** argv )
{
    XEvent ev;

    parse_args( argc, argv, &options );

    init_interface( options.window_width, options.window_height,
                    options.n_files, &graphics );

    producer_tid = 0;
    if( pthread_create( &producer_tid, NULL, producer_thread, NULL ) )
    {
        fprintf( stderr, "ERROR: Cannot create producer thread\n" );
        exit( -1 );
    }

    atexit( exit_handler );

    while( !options.quit )
    {
        /* Get next event */
        XNextEvent( graphics.display, &ev );

        /* Handle event */
        switch( ev.type )
        {
        case Expose:
            /* Ignore extra exposure events */
            if( ev.xexpose.count > 0 )
                break;

            /* Replot graphics */
            plot_graphic( &options, &graphics );
        break;

        case KeyPress:
            if( ev.xkey.keycode == XK_Escape )
            {
                /* Prepare for termination */
                XFlush( graphics.display );
                XDestroyWindow( graphics.display, graphics.border_window );
                options.quit = 1;
            }
        break;
        }
    }

    if( producer_tid )
    {
        pthread_kill( producer_tid, SIGTERM );
        producer_tid = 0;
    }

    XLockDisplay( graphics.display );
    XCloseDisplay( graphics.display );

    return 0;
}
