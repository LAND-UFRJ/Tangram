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

#include <plot.h>

/* Border width */
#define BORDER_WIDTH 60
#define BORDER_HEIGHT 30

/* Plotting colors */

static XColor plot_color[] =
{
    {
        .red   =   0xFF00,
        .green = 0x0000,
        .blue  =  0x0000,
    }, /* RED */
    {
        .red   =   0x0000,
        .green = 0xFF00,
        .blue  =  0x0000,
    }, /* GREEN */
    {
        .red   =   0x0000,
        .green = 0x0000,
        .blue  =  0xFF00,
    }, /* BLUE */
    {
        .red   =   0xFF00,
        .green = 0x0000,
        .blue  =  0xFF00,
    }, /* MAGENTA */
    {
        .red   =   0x0000,
        .green = 0xFF00,
        .blue  =  0xFF00,
    }, /* CYAN */
    {
        .red   =   0x7F00,
        .green = 0x7F00,
        .blue  =  0x0000,
    } /* BROWN */
};

/* Number of plotting colors */
#define MAX_COLORS (sizeof(plot_color)/sizeof(XColor))

static int io_error_handler( Display * display )
{
    exit( -1 );
    return 0;
}

void invalidate_graphics( Graphics * gfx )
{
    static XEvent ev;

    if( !gfx->display )
        return;

    /* Lock display for multithreading issues */
    XLockDisplay( gfx->display );

    /* Send exposure event */
    ev.type = Expose;
    ev.xexpose.count = 0;
    XSendEvent( gfx->display, gfx->border_window, False, ExposureMask, &ev );
    XFlush( gfx->display );

    /* Unlock display */
    XUnlockDisplay( gfx->display );
}

void init_interface( int w, int h, int n, Graphics * gfx )
{
    int i;
    XGCValues values;
    XColor color;
    XSizeHints * size_hints;

    /* Init Xlib support for threads */
    XInitThreads( );

    /* Get initial variables */
    gfx->display       = XOpenDisplay( NULL );
    gfx->screen_number = DefaultScreen( gfx->display );

    /* Create toplevel window */
    gfx->border_window = XCreateSimpleWindow( gfx->display,
                                  RootWindow(gfx->display, gfx->screen_number),
                                  0, 0,
                                  w+(2*BORDER_WIDTH), h+(2*BORDER_HEIGHT),
                                  0,
                                  BlackPixel(gfx->display,gfx->screen_number),
                                  WhitePixel(gfx->display,gfx->screen_number) );

    size_hints = XAllocSizeHints( );
    size_hints->flags = PSize|PMinSize|PMaxSize;
    size_hints->width       = w+(2*BORDER_WIDTH);
    size_hints->height      = h+(2*BORDER_HEIGHT);
    size_hints->base_width  = w+(2*BORDER_WIDTH);
    size_hints->base_height = h+(2*BORDER_HEIGHT);
    size_hints->min_width   = w+(2*BORDER_WIDTH);
    size_hints->min_height  = h+(2*BORDER_HEIGHT);
    size_hints->max_width   = w+(2*BORDER_WIDTH);
    size_hints->max_height  = h+(2*BORDER_HEIGHT);
    XSetWMNormalHints( gfx->display, gfx->border_window, size_hints );
    XFree( size_hints );

    /* Create inner window for plotting */
    gfx->plot_window = XCreateSimpleWindow( gfx->display,
                                  gfx->border_window,
                                  BORDER_WIDTH, BORDER_HEIGHT,
                                  w, h,
                                  1,
                                  BlackPixel(gfx->display,gfx->screen_number),
                                  WhitePixel(gfx->display,gfx->screen_number) );

    /* Change main window title */
    XStoreName( gfx->display, gfx->border_window, "Animplot" );

    /* Create back buffer pixmap */
    gfx->pixmap = XCreatePixmap( gfx->display, gfx->plot_window, w, h,
                            DefaultDepth(gfx->display, gfx->screen_number) );

    /* Create default graphics context for writing */
    gfx->default_gc = DefaultGC(gfx->display, gfx->screen_number);

    /* Allocate colors for other GCs */
    for( i = 0; i < (n < MAX_COLORS ? n:MAX_COLORS); i++ )
    {
        XAllocColor( gfx->display,
                     DefaultColormap( gfx->display, gfx->screen_number ),
                     &plot_color[i] );
    }

    /* Create other GCs for plotting */
    gfx->plot_gc = (GC *)malloc( n * sizeof(GC) );
    for( i = 0; i < n; i++ )
    {
        values.foreground = plot_color[i%MAX_COLORS].pixel;
        gfx->plot_gc[i] = XCreateGC( gfx->display, gfx->plot_window,
                                     GCForeground, &values );
    }

    color.red = color.green = color.blue = 0xB000;
    XAllocColor( gfx->display,
                 DefaultColormap( gfx->display, gfx->screen_number ),
                 &color );
    values.foreground = color.pixel;
    values.line_style = LineOnOffDash;
    gfx->grid_gc = XCreateGC( gfx->display, gfx->plot_window,
                              GCForeground | GCLineStyle, &values );

    /* Set error handler */
    XSetIOErrorHandler( io_error_handler );

    /* Select input mask for events of exposure and key press */
    XSelectInput( gfx->display, gfx->border_window,
                  ExposureMask | KeyPressMask );

    /* Map windows on screen */
    XMapWindow( gfx->display, gfx->border_window );
    XMapWindow( gfx->display, gfx->plot_window );
}

void plot_graphic( const Options * opt, const Graphics * gfx )
{
    double x_length, y_length, x_min, x_max, y_min, y_max;
    int x1, x2, y1, y2, i, n, size, k_min, k_max;
    struct point p;
    static char temp_str[20];

    x_max = opt->x_max;
    x_min = opt->x_min;
    y_max = opt->y_max + 0.1 * (opt->y_max-opt->y_min);
    y_min = opt->y_min - 0.1 * (opt->y_max-opt->y_min);
    x_length = x_max - x_min;
    y_length = y_max - y_min;

    /* Lock the display for multithreading reasons */
    XLockDisplay( gfx->display );

    /* Clear the background */
    XSetForeground( gfx->display, gfx->default_gc,
                    WhitePixel( gfx->display, gfx->screen_number ) );
    XFillRectangle( gfx->display, gfx->pixmap, gfx->default_gc,
                    0, 0, opt->window_width, opt->window_height );
    XSetForeground( gfx->display, gfx->default_gc,
                    BlackPixel( gfx->display, gfx->screen_number ) );

    /* Plot graphics for each input file */
    for( n = 0; n < opt->n_files; n++ )
    {
        /* If the queue has less than two points, continue */
        size = QUEUE_SIZE(opt->queue[n]);

#ifdef DEBUG
fprintf( stderr, "\n" );
fprintf( stderr, "Plotting file %d - queue size = %d\n", n, size );
fprintf( stderr, "queue[%d].head = %d\n", n, opt->queue[n].head );
fprintf( stderr, "queue[%d].tail = %d\n", n, opt->queue[n].tail );
fprintf( stderr, "queue[%d].capacity = %d\n", n, opt->queue[n].capacity );
fprintf( stderr, "\n" );
#endif

        if( size < 2 )
            continue;

        /* Draw lines */
        p = GET_ELEMENT(opt->queue[n],0);
        x1 = (p.x-x_min) * (double)(opt->window_width)/x_length;
        y1 = (y_max-p.y) * (double)(opt->window_height)/y_length;

        for( i = 1; i < size; i++ )
        {
            p = GET_ELEMENT(opt->queue[n],i);
            x2 = (p.x-x_min) * (double)(opt->window_width) / x_length;
            y2 = (y_max-p.y) * (double)(opt->window_height) / y_length;

            if( p.x >= x_min && p.x <= x_max )
            {
                /* Join points {(x1,y1),(x2,y2)} by steps or lines */
                if( opt->steps )
                {
                    XDrawLine( gfx->display, gfx->pixmap, gfx->plot_gc[n],
                               x1, y1, x2, y1 );
                    XDrawLine( gfx->display, gfx->pixmap, gfx->plot_gc[n],
                               x2, y1, x2, y2 );
                }
                else
                {
                    XDrawLine( gfx->display, gfx->pixmap, gfx->plot_gc[n],
                               x1, y1, x2, y2 );
                }
            }

            x1 = x2;
            y1 = y2;
        }
    }

    /* Draw horizontal grid lines */
    y1 = (y_max-opt->y_max) * (double)(opt->window_height)/y_length;
    XDrawLine( gfx->display, gfx->pixmap, gfx->grid_gc,
               0, y1, opt->window_width, y1 );
    y2 = (y_max-opt->y_min) * (double)(opt->window_height)/y_length;
    XDrawLine( gfx->display, gfx->pixmap, gfx->grid_gc,
               0, y2, opt->window_width, y2 );

    /* Draw vertical grid lines */
    k_min = (int)(x_min / opt->grid_interval);
    if( k_min * opt->grid_interval <= x_min )
        k_min++;
    k_max = (int)(x_max / opt->grid_interval);
    if( k_max * opt->grid_interval >= x_max )
        k_max--;

    for( i = k_min; i <= k_max; i++ )
    {
        x1 = ((i*opt->grid_interval)-x_min)
             * (double)opt->window_width / x_length;
        XDrawLine( gfx->display, gfx->pixmap, gfx->grid_gc,
                   x1, 0, x1, opt->window_height );
    }

    /* Update window with the back buffer */
    XCopyArea( gfx->display, gfx->pixmap, gfx->plot_window, gfx->default_gc,
               0, 0, opt->window_width, opt->window_height, 0, 0 );
    XFlush( gfx->display );

    /* Display minimum X */
    sprintf( temp_str, "%-10.3lf", opt->x_min );
    XDrawImageString( gfx->display, gfx->border_window, gfx->default_gc,
                      BORDER_WIDTH,
                      17+BORDER_HEIGHT+opt->window_height,
                      temp_str, strlen(temp_str) );
    XDrawImageString( gfx->display, gfx->border_window, gfx->default_gc,
                      BORDER_WIDTH,
                      BORDER_HEIGHT-7,
                      temp_str, strlen(temp_str) );

    /* Display maximum X */
    sprintf( temp_str, "%10.3lf", opt->x_max );
    XDrawImageString( gfx->display, gfx->border_window, gfx->default_gc,
                      opt->window_width+BORDER_WIDTH-57,
                      17+BORDER_HEIGHT+opt->window_height,
                      temp_str, strlen(temp_str) );
    XDrawImageString( gfx->display, gfx->border_window, gfx->default_gc,
                      opt->window_width+BORDER_WIDTH-57,
                      BORDER_HEIGHT-7,
                      temp_str, strlen(temp_str) );

    /* Display minimum Y */
    sprintf( temp_str, "%10.3lf", opt->y_min );
    XDrawImageString( gfx->display, gfx->border_window, gfx->default_gc,
                      BORDER_WIDTH-67,
                      BORDER_HEIGHT+opt->window_height-7,
                      temp_str, strlen(temp_str) );
    sprintf( temp_str, "%-10.3lf", opt->y_min );
    XDrawImageString( gfx->display, gfx->border_window, gfx->default_gc,
                      BORDER_WIDTH+opt->window_width+10,
                      BORDER_HEIGHT+opt->window_height-7,
                      temp_str, strlen(temp_str) );

    /* Display maximum Y */
    sprintf( temp_str, "%10.3lf", opt->y_max );
    XDrawImageString( gfx->display, gfx->border_window, gfx->default_gc,
                      BORDER_WIDTH-67,
                      BORDER_HEIGHT+17,
                      temp_str, strlen(temp_str) );
    sprintf( temp_str, "%-10.3lf", opt->y_max );
    XDrawImageString( gfx->display, gfx->border_window, gfx->default_gc,
                      BORDER_WIDTH+opt->window_width+10,
                      BORDER_HEIGHT+17,
                      temp_str, strlen(temp_str) );

    /* Unlock the display */
    XUnlockDisplay( gfx->display );
}
