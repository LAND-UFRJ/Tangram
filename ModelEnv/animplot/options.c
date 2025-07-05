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

#include <unistd.h>

#include <options.h>

#ifdef USE_GETOPT_LONG
#include <getopt.h>

static struct option cmd_options[] =
{
    { "file",     required_argument, NULL, 'f' },
    { "buffer",   required_argument, NULL, 'b' },
    { "interval", required_argument, NULL, 'i' },
    { "maxy",     required_argument, NULL, 0   },
    { "miny",     required_argument, NULL, 0   },
    { "limitx",   no_argument,       NULL, 0   },
    { "limity",   no_argument,       NULL, 0   },
    { "xrange",   required_argument, NULL, 0   },
    { "steps",    no_argument,       NULL, 0   },
    { "xgrid",    required_argument, NULL, 'g' },
    { "width",    required_argument, NULL, 'w' },
    { "height",   required_argument, NULL, 'h' },
};

static void show_usage( char * progname )
{
    fprintf( stderr, "Usage:\n" );
    fprintf( stderr,
             "%s (-f|--file) <filename1> [(-f|--file) <filename2> ...]\n"
             "\t[(-b|--buffer) <integer>] [(-i|--interval) <integer>]\n"
             "\t[--maxy <float>] [--miny <float>]\n"
             "\t[(-g|--xgrid) <float>] [-s <integer>] [-e]\n"
             "\t[(-w|--width) <integer>] [(-h|--height) <integer>]\n\n",
             progname );

    fprintf( stderr, "Options:\n" );
    fprintf( stderr,
             "    (-f|--file) <filename>: read samples from a given file.\n"
             "    (-b|--buffer) <integer>: samples buffer size."
                " Default: %d\n"
             "    (-i|--interval) <integer>: interval between"
                " polling. Default: %d usec\n"
             "    --maxy <float>: initial upper limit for the y coordinate."
                " Default: %.2lf\n"
             "    --miny <float>: initial lower limit for the y coordinate."
                " Default: %.2lf\n"
             "    --limitx: limit the x range to its initial value."
                " Default: false\n"
             "    --limity: limit the y range to its initial value."
                " Default: false\n"
             "    --xrange <float>: initial y range."
                " Default: %.2lf\n"
             "    (-g|--xgrid) <float>: the interval between grid marks."
                " Default: %.2lf\n"
             "    -s <integer>: number of samples read at a time."
                " Default: %d\n"
             "    (-w|--width) <integer>: plot window width."
                " Default: %d\n"
             "    (-h|--height) <integer>: plot window height."
                " Default: %d\n"
             "    --steps: join the points with steps instead of lines."
                " Default: false\n",
             DEFAULT_BUFFER_SIZE,
             DEFAULT_INTERVAL,
             DEFAULT_Y_MAX,
             DEFAULT_Y_MIN,
             DEFAULT_X_MAX-DEFAULT_X_MIN,
             DEFAULT_GRID_INTERVAL,
             DEFAULT_SAMPLES_PER_READ,
             DEFAULT_WINDOW_WIDTH,
             DEFAULT_WINDOW_HEIGHT );

    exit( -1 );
}

void parse_args( int argc, char ** argv, Options * opt )
{
    int c,
        i;

    /* Per file options */
    opt->n_files          = 0;
    opt->file_name        = NULL;
    opt->file             = NULL;
    opt->queue            = NULL;

    /* Command line options */
    opt->interval         = DEFAULT_INTERVAL;
    opt->grid_interval    = DEFAULT_GRID_INTERVAL;
    opt->buffer_size      = DEFAULT_BUFFER_SIZE;
    opt->samples_per_read = DEFAULT_SAMPLES_PER_READ;
    opt->window_width     = DEFAULT_WINDOW_WIDTH;
    opt->window_height    = DEFAULT_WINDOW_HEIGHT;

    /* Logical area coordinates */
    opt->x_min            = DEFAULT_X_MIN;
    opt->x_max            = DEFAULT_X_MAX;
    opt->y_min            = DEFAULT_Y_MIN;
    opt->y_max            = DEFAULT_Y_MAX;

    /* Extra options */
    opt->quit             = 0;
    opt->limit_y_range    = 0;
    opt->limit_x_range    = 0;
    opt->x_range          = opt->x_max - opt->x_min;
    opt->steps            = 0;

    /* Parse arguments */
    while( (c = getopt_long( argc, argv, ":f:i:g:b:s:w:h:",
                             cmd_options, &i )) != EOF )
    {
        switch( c )
        {
        case ':':
            fprintf( stderr, "Missing parameter from -%c\n\n", optopt );
            show_usage( argv[0] );
        break;

        case '?':
            fprintf( stderr, "Invalid option: -%c\n\n", optopt );
            show_usage( argv[0] );
        break;

        case 0:
            switch( i )
            {
            case 3:
                opt->y_max = atof( optarg );
            break;

            case 4:
                opt->y_min = atof( optarg );
            break;

            case 5:
                opt->limit_x_range = 1;
            break;

            case 6:
                opt->limit_y_range = 1;
            break;

            case 7:
                if( (opt->x_range = atof( optarg )) <= 0.0 )
                {
                    fprintf( stderr,
                             "X range must be greater than 0.0.\n\n" );
                    show_usage( argv[0] );
                }
            break;

            case 8:
                opt->steps = 1;
            break;
            }
        break;

        case 'f':
            /* Add another file to the list */
            opt->n_files++;
            opt->file_name = (char **)realloc( opt->file_name,
                                               opt->n_files * sizeof(char *) );
            opt->file_name[opt->n_files-1] = optarg;
        break;

        case 'i':
            if( (opt->interval = atoi( optarg )) < 1 )
            {
                fprintf( stderr,
                         "Time interval (-i) must be greater than 0.\n\n" );
                show_usage( argv[0] );
            }
        break;

        case 'g':
            if( (opt->grid_interval = atof( optarg )) < 1 )
            {
                fprintf( stderr,
                         "Grid interval (-g) must be greater than 0.\n\n" );
                show_usage( argv[0] );
            }
        break;

        case 'b':
            if( (opt->buffer_size = atoi( optarg )) < 1 )
            {
                fprintf( stderr,
                         "Buffer size (-b) must be greater than 0.\n\n" );
                show_usage( argv[0] );
            }
        break;

        case 's':
            if( (opt->samples_per_read = atoi( optarg )) < 1 )
            {
                fprintf( stderr,
                         "Samples per read (-s) must be greater than 0.\n\n" );
                show_usage( argv[0] );
            }
        break;

        case 'w':
            if( (opt->window_width = atoi( optarg )) < 1 )
            {
                fprintf( stderr,
                         "Window width (-w) must be greater than 0.\n\n" );
                show_usage( argv[0] );
            }
        break;

        case 'h':
            if( (opt->window_height = atoi( optarg )) < 1 )
            {
                fprintf( stderr,
                         "Window height (-h) must be greater than 0.\n\n" );
                show_usage( argv[0] );
            }
        break;
        }
    }

    /* Additional verifications */
    if( opt->buffer_size < opt->samples_per_read )
    {
        fprintf( stderr,
                 "Buffer size (%d) must be greater than samples read"
                 " at a time (%d).\n\n",
                 opt->buffer_size, opt->samples_per_read );
        show_usage( argv[0] );
    }

    if( opt->n_files < 1 )
    {
        fprintf( stderr, "Must have at least one input file\n\n" );
        show_usage( argv[0] );
    }

    if( opt->y_max < opt->y_min )
    {
        fprintf( stderr,
                 "Maximum Y limit (%lf) must be greater than the"
                 "minimum Y limit (%lf).\n\n", opt->y_max, opt->y_min );
        show_usage( argv[0] );
    }

    /* Open input files */
    if( (opt->file = (FILE **)malloc( opt->n_files * sizeof(FILE *) )) == NULL )
    {
        fprintf( stderr, "ERROR: Not enough memory\n" );
        exit( -1 );
    }
    for( i = 0; i < opt->n_files; i++ )
    {
        if( (opt->file[i] = fopen( opt->file_name[i], "r" )) == NULL )
        {
            fprintf( stderr, "ERROR: Could not open file (%s) for reading\n",
                     opt->file_name[i] );
            exit( -1 );
        }
    }

    /* Create data queues */
    if( !(opt->queue = (DataQueue *)malloc( opt->n_files*sizeof(DataQueue) )) )
    {
        fprintf( stderr, "ERROR: Not enough memory\n" );
        exit( -1 );
    }
    for( i = 0; i < opt->n_files; i++ )
    {
        if( create_queue( &(opt->queue[i]), opt->buffer_size ) < 0 )
        {
            fprintf( stderr, "ERROR: Not enough memory\n" );
            exit( -1 );
        }
    }
}
#else
static void show_usage( char * progname )
{
    fprintf( stderr, "Usage:\n" );
    fprintf( stderr,
             "%s -f <filename1> [-f <filename2> ...]\n"
             "\t[-b <integer>] [-i <integer>]\n"
             "\t[-m <float>] [-n <float>] [-x] [-y]\n"
             "\t[-r <float>] [-g <float>] [-s <integer>]\n"
             "\t[-w <integer>] [-h <integer>] [-e]\n\n",
             progname );

    fprintf( stderr, "Options:\n" );
    fprintf( stderr,
             "    -f <filename>: read samples from a given file.\n"
             "    -b <integer>: samples buffer size."
                " Default: %d\n"
             "    -i <integer>: interval between"
                " polling. Default: %d usec\n"
             "    -m <float>: initial upper limit for the y coordinate."
                " Default: %.2lf\n"
             "    -n <float>: initial lower limit for the y coordinate."
                " Default: %.2lf\n"
             "    -x: limit the x range to its initial value."
                " Default: false\n"
             "    -y: limit the y range to its initial value."
                " Default: false\n"
             "    -r <float>: initial x range."
                " Default: %.2lf\n"
             "    -g <float>: the interval between grid marks."
                " Default: %.2lf\n"
             "    -s <integer>: number of samples read at a time."
                " Default: %d\n"
             "    -w <integer>: plot window width."
                " Default: %d\n"
             "    -h <integer>: plot window height."
                " Default: %d\n"
             "    -e: join the points with steps instead of lines."
                " Default: false\n",
             DEFAULT_BUFFER_SIZE,
             DEFAULT_INTERVAL,
             DEFAULT_Y_MAX,
             DEFAULT_Y_MIN,
             DEFAULT_X_MAX-DEFAULT_X_MIN,
             DEFAULT_GRID_INTERVAL,
             DEFAULT_SAMPLES_PER_READ,
             DEFAULT_WINDOW_WIDTH,
             DEFAULT_WINDOW_HEIGHT );

    exit( -1 );
}

void parse_args( int argc, char ** argv, Options * opt )
{
    int c, i;

    /* Per file options */
    opt->n_files          = 0;
    opt->file_name        = NULL;
    opt->file             = NULL;
    opt->queue            = NULL;

    /* Command line options */
    opt->interval         = DEFAULT_INTERVAL;
    opt->grid_interval    = DEFAULT_GRID_INTERVAL;
    opt->buffer_size      = DEFAULT_BUFFER_SIZE;
    opt->samples_per_read = DEFAULT_SAMPLES_PER_READ;
    opt->window_width     = DEFAULT_WINDOW_WIDTH;
    opt->window_height    = DEFAULT_WINDOW_HEIGHT;

    /* Logical area coordinates */
    opt->x_min            = DEFAULT_X_MIN;
    opt->x_max            = DEFAULT_X_MAX;
    opt->y_min            = DEFAULT_Y_MIN;
    opt->y_max            = DEFAULT_Y_MAX;

    /* Extra options */
    opt->quit             = 0;
    opt->limit_y_range    = 0;
    opt->limit_x_range    = 0;
    opt->x_range          = opt->x_max - opt->x_min;
    opt->steps            = 0;

    /* Parse arguments */
    while( (c = getopt( argc, argv, "f:b:i:m:n:xyr:g:s:w:h:e" )) != EOF )
    {
        switch( c )
        {
        case ':':
            fprintf( stderr, "Missing parameter from -%c\n\n", optopt );
            show_usage( argv[0] );
        break;

        case '?':
            fprintf( stderr, "Error in option: -%c\n\n", optopt );
            show_usage( argv[0] );
        break;

        case 'f':
            /* Add another file to the list */
            opt->n_files++;
            opt->file_name = (char **)realloc( opt->file_name,
                                               opt->n_files * sizeof(char *) );
            opt->file_name[opt->n_files-1] = optarg;
        break;

        case 'b':
            if( (opt->buffer_size = atoi( optarg )) < 1 )
            {
                fprintf( stderr,
                         "Buffer size (-b) must be greater than 0.\n\n" );
                show_usage( argv[0] );
            }
        break;

        case 'i':
            if( (opt->interval = atoi( optarg )) < 1 )
            {
                fprintf( stderr,
                         "Time interval (-i) must be greater than 0.\n\n" );
                show_usage( argv[0] );
            }
        break;

        case 'm':
            opt->y_max = atof( optarg );
        break;

        case 'n':
            opt->y_min = atof( optarg );
        break;

        case 'x':
            opt->limit_x_range = 1;
        break;

        case 'y':
            opt->limit_y_range = 1;
        break;

        case 'r':
            if( (opt->x_range = atof( optarg )) <= 0.0 )
            {
                fprintf( stderr,
                         "X range must be greater than 0.0.\n\n" );
                show_usage( argv[0] );
            }
        break;

        case 'g':
            if( (opt->grid_interval = atof( optarg )) < 1 )
            {
                fprintf( stderr,
                         "Grid interval (-g) must be greater than 0.\n\n" );
                show_usage( argv[0] );
            }
        break;

        case 's':
            if( (opt->samples_per_read = atoi( optarg )) < 1 )
            {
                fprintf( stderr,
                         "Samples per read (-s) must be greater than 0.\n\n" );
                show_usage( argv[0] );
            }
        break;

        case 'w':
            if( (opt->window_width = atoi( optarg )) < 1 )
            {
                fprintf( stderr,
                         "Window width (-w) must be greater than 0.\n\n" );
                show_usage( argv[0] );
            }
        break;

        case 'h':
            if( (opt->window_height = atoi( optarg )) < 1 )
            {
                fprintf( stderr,
                         "Window height (-h) must be greater than 0.\n\n" );
                show_usage( argv[0] );
            }
        break;

        case 'e':
            opt->steps = 1;
        break;

        default:
            fprintf( stderr, "Unexpected getopt return (%c)\n", c );
        }
    }

    /* Additional verifications */
    if( opt->buffer_size < opt->samples_per_read )
    {
        fprintf( stderr,
                 "Buffer size (%d) must be greater than samples read"
                 " at a time (%d).\n\n",
                 opt->buffer_size, opt->samples_per_read );
        show_usage( argv[0] );
    }

    if( opt->n_files < 1 )
    {
        fprintf( stderr, "Must have at least one input file\n\n" );
        show_usage( argv[0] );
    }

    if( opt->y_max < opt->y_min )
    {
        fprintf( stderr,
                 "Maximum Y limit (%lf) must be greater than the"
                 "minimum Y limit (%lf).\n\n", opt->y_max, opt->y_min );
        show_usage( argv[0] );
    }

    /* Open input files */
    if( (opt->file = (FILE **)malloc( opt->n_files * sizeof(FILE *) )) == NULL )
    {
        fprintf( stderr, "ERROR: Not enough memory\n" );
        exit( -1 );
    }
    for( i = 0; i < opt->n_files; i++ )
    {
        if( (opt->file[i] = fopen( opt->file_name[i], "r" )) == NULL )
        {
            fprintf( stderr, "ERROR: Could not open file (%s) for reading\n",
                     opt->file_name[i] );
            exit( -1 );
        }
    }

    /* Create data queues */
    if( !(opt->queue = (DataQueue *)malloc( opt->n_files*sizeof(DataQueue) )) )
    {
        fprintf( stderr, "ERROR: Not enough memory\n" );
        exit( -1 );
    }
    for( i = 0; i < opt->n_files; i++ )
    {
        if( create_queue( &(opt->queue[i]), opt->buffer_size ) < 0 )
        {
            fprintf( stderr, "ERROR: Not enough memory\n" );
            exit( -1 );
        }
    }
}
#endif
