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

#ifndef __PLOT_H__
#define __PLOT_H__

#include <string.h>
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <options.h>

typedef struct
{
    Display * display;
    Window    plot_window;
    Window    border_window;
    GC        default_gc;
    GC      * plot_gc;
    GC        grid_gc;
    int       screen_number;
    Pixmap    pixmap;
} Graphics;

void invalidate_graphics( Graphics * );
void init_interface( int, int, int, Graphics * );
void plot_graphic( const Options *, const Graphics * );

#endif /* __PLOT_H__ */
