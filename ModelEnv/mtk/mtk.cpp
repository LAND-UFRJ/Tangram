#include <iostream>

using namespace std;

#include <getopt.h>

#include "types.h"
#include "plugin_manager.h"
#include "user_interface.h"
#include "core_controller.h"
#include "terminal.h"

#ifdef MTKGUI
    #include "MainWindow.h"
    #include <qapplication.h>
#endif

#define USAGE_MESSAGE \
"Usage:                                                                   \n" \
"%s [-g] [-f <file>] [-s <integer>] [-p <path 1>] [-p <path 2>] ... [-h]  \n" \
"                                                                         \n" \
"Options:                                                                 \n" \
"    -g             - start in graphic mode                               \n" \
"    -f <file>      - read commands from <file> instead of displaying a   \n" \
"                     prompt to the user.                                 \n" \
"    -s <integer>   - use <integer> as the seed for random numbers. The   \n" \
"                     default behavior is using the system clock as seed. \n" \
"    -p <directory> - include <directory> in the list of possible plugin  \n" \
"                     locations. This option may be used as many times as \n" \
"                     needed. The environment variable MTK_PATH may also  \n" \
"                     be used to specify these locations.                 \n" \
"    -h             - prints this help message and exit                   \n"

typedef struct
{
    char                   *input_file;
    bool                    use_seed_value;
    long                    seed_value;
    std::list<std::string>  path_list;
    bool                    graphic_mode;
} CLOptions;

static int parse_command_line( int argc, char ** argv, CLOptions * opt )
{
    int c, length;
    char * pt;

    opt->input_file     = 0;         
    opt->use_seed_value = false;     
    opt->seed_value     = 0;         
    opt->graphic_mode   = false;

    while( (c = getopt( argc, argv, ":f:s:p:gh" )) != EOF )
    {
        switch( c )
        {
            case ':':
                fprintf( stderr, "Missing parameter from -%c\n\n", optopt );
                fprintf( stderr, USAGE_MESSAGE, argv[0] );
                return -1;

            case '?':
                fprintf( stderr, "Unknown option: -%c\n\n", optopt );
                fprintf( stderr, USAGE_MESSAGE, argv[0] );
                return -1;

            case 'f':
                if( opt->input_file )
                {
                    fprintf( stderr, "Only one file may be specified "
                                     "with -f at each time\n\n" );
                    fprintf( stderr, USAGE_MESSAGE, argv[0] );
                    return -1;
                }
                else
                    opt->input_file = optarg;
                break;

            case 's':
                if( opt->use_seed_value )
                {
                    fprintf( stderr, "Only one random seed may be specified "
                                     "with -s at each time\n\n" );
                    fprintf( stderr, USAGE_MESSAGE, argv[0] );
                    return -1;
                }
                else
                {
                    opt->use_seed_value = true;
                    opt->seed_value = strtol( optarg, &pt, 0 );
                    length = strlen(optarg);
                    if( length != (pt - optarg) )
                    {
                        fprintf( stderr, "Invalid seed specification: %s\n\n",
                                 optarg );
                        fprintf( stderr, USAGE_MESSAGE, argv[0] );
                        return -1;
                    }
                }
                break;

            case 'p':
                opt->path_list.push_back( optarg );
                break;
                
            case 'g':
                opt->graphic_mode = true;
                break;
                
            case 'h':
                fprintf( stderr, USAGE_MESSAGE, argv[0] );
                exit(0);
                break;                                

            default:
                DEBUG( LEVEL_WARNING, "Unexpected getopt() return (%c)\n", c );
                break;
        }
    }

    return 0;
}

static void parse_environment( CLOptions * opt )
{
    char *mtk_path,
         *tangram_home,
         *pt,
          tangram_mtk_path[256];

    // Read MTK_PATH
    mtk_path = getenv( "MTK_PATH" );

    while( mtk_path )
    {
        pt = strchr( mtk_path, ':' );
        if( pt )
        {
            *pt = 0;
            pt++;
        }

        opt->path_list.push_back( mtk_path );
        mtk_path = pt;
    }
    
    // Read TANGRAM2_HOME/lib/mtk_plugins
    tangram_home = getenv( "TANGRAM2_HOME" );
    if( tangram_home )
    {
        sprintf( tangram_mtk_path, "%s/lib/mtk_plugins", tangram_home );
        opt->path_list.push_back( tangram_mtk_path );
    }
}

int main( int argc, char ** argv )
{
    int status;
    UserInterface * user_interface;
    CLOptions options;

    if( parse_command_line( argc, argv, &options ) < 0 )
        return -1;

    parse_environment( &options );

    if( !options.input_file && !options.graphic_mode )
        fprintf( stderr, MTK_VERSION_STRING "\n\n" );

    CoreController::setupCore( options.path_list,
                               options.use_seed_value,
                               options.seed_value );

    if( options.path_list.size( ) < 1 )
    {
        fprintf( stderr,
                 "Warning: No directories have been "
                 "specified for plugins.\n\n" );
    }

    if( options.graphic_mode )
    {
        #ifdef MTKGUI
            // Start graphic mode
            QApplication app( argc, argv );

            MainWindow *w = new MainWindow( );
            app.setMainWidget( w );
            w->show();

            status = app.exec();
        #else
            fprintf( stderr,
                     "MTK was compiled without graphical interface support.\n"
                     "Check your build options and try again.\n" );
            status = -1;
        #endif
    }
    else
    {
        // Start console mode
        UserInterface::createUserInterface( );

        user_interface = UserInterface::getUserInterface( );

        // give control to the interface
        if( options.input_file ) // FIXME - allow multiple files to be given
            user_interface->runCommandFile( options.input_file );
        else
            user_interface->runMainLoop( );

        UserInterface::destroyUserInterface( );

        status = 0;
    }
    
    CoreController::cleanCore( );
    
    return status;
}
