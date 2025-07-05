#include "terminal.h"

#include "core_controller.h"

Terminal * Terminal::current_terminal = 0;

std::ostream & Terminal::output_stream = std::cout;
std::ostream & Terminal::error_stream  = std::cerr;

const char * Terminal::pfile_name = 0;
const int  * Terminal::pline_number = 0;

bool Terminal::mute = false;

Name Terminal::pager_name = "";

Terminal::Terminal( void )
{
    type = TERMINAL_SCREEN;
    file = 0;
}

Terminal::Terminal( const std::string & name )
{
    type = TERMINAL_FILE;
    file = 0;
    setOutputName(file_name);
}

Terminal::~Terminal( void )
{
    if( file )
        close( );
}

bool Terminal::setTerminal( Terminal::TerminalType type )
{
    std::string output_file_name;

    if( current_terminal )
    {
        output_file_name = current_terminal->getOutputName( );
        delete current_terminal;
        current_terminal = 0;
    }
    else
        output_file_name = "";

    switch( type )
    {
        case TERMINAL_FILE:
            current_terminal = new Terminal( output_file_name );
            return true;
        case TERMINAL_SCREEN:
            // this should never fail
            current_terminal = new Terminal( );
            return true;
        default:              // this is also the default
            current_terminal = new Terminal( );
            return false;
    }
}

Terminal * Terminal::getTerminal( void )
{
    return current_terminal;
}

bool Terminal::open( void )
{
    if( type == TERMINAL_SCREEN )
    {
        file = stdout;
    }
    else if( type == TERMINAL_FILE )
    {
        file = fopen( file_name.c_str( ), "a" );
        if( !file )
        {
            displayErrorMessage(
                   "could not open '%s' for writing - using STDOUT as default",
                   file_name.c_str() );
            file = stdout;
        }
    }

    return true;
}

bool Terminal::close( void )
{
    if( type == TERMINAL_FILE && file && file != stdout )
        fclose( file );

    file = 0;
    return true;
}

Terminal::TerminalType Terminal::getType( void ) const
{
    return type;
}

const char * Terminal::getOutputName( void ) const
{
    if( type == TERMINAL_SCREEN )
        return "STDOUT";
    else if( type == TERMINAL_FILE && file_name.length( ) > 0 )
        return file_name.c_str( );

    return "(none)";
}

void Terminal::setOutputName( const std::string & name )
{
    FILE * fp;

    file_name = name;

    // try to open the file only to truncate it empty
    fp = fopen( file_name.c_str( ), "w" );
    if( fp )
        fclose( fp );
}

void Terminal::displayList( const std::string & title, NameList & names )
{
    int n, m;
    char format_string[32];
    NameList::iterator it;

    if( mute )
        return;

    n = title.length( );

    for( m = 0; m < n; m++ )
        fputc( '-', stdout );
    fputc( '\n', stdout );
    fprintf( stdout, "%s\n", title.c_str( ) );

    for( m = 0; m < n; m++ )
        fputc( '-', stdout );
    fputc( '\n', stdout );

    m = 0;
    for( it = names.begin( ); it != names.end( ); it++ )
    {
        n = (*it).length( );
        if( n > m )
            m = n;
    }

    n = 80/(m+3);
    if( n < 1 ) n = 1;

    sprintf( format_string, "%%%ds", m+3 );

    m = 0;
    for( it = names.begin( ); it != names.end( ); it++ )
    {
        fprintf( stdout, format_string, (*it).c_str( ) );
        m++;
        if( m == n )
        {
            fputc( '\n', stdout );
            m = 0;
        }
    }

    if( m > 0 )
        fputc( '\n', stdout );

    fputc( '\n', stdout );
}

void Terminal::displayList( const std::string & title, NameList & names,
                            NameList & descriptions )
{
    int n, m;
    char format_string[32];
    NameList::iterator it_names, it_descs;

    if( mute )
        return;

    n = title.length( );

    for( m = 0; m < n; m++ )
        fputc( '-', stdout );
    fputc( '\n', stdout );
    fprintf( stdout, "%s\n", title.c_str( ) );

    for( m = 0; m < n; m++ )
        fputc( '-', stdout );
    fputc( '\n', stdout );

    m = 0;
    for( it_names = names.begin( ), it_descs = descriptions.begin( );
         it_names != names.end( ) , it_descs != descriptions.end( );
         it_names++, it_descs++ )
    {
        n = (*it_names).length( ) + 1;
        if( n > m )
            m = n;
    }

    sprintf( format_string, "%%%ds - %%s\n", m );

    for( it_names = names.begin( ), it_descs = descriptions.begin( );
         it_names != names.end( ) , it_descs != descriptions.end( );
         it_names++, it_descs++ )
    {
        fprintf( stdout, format_string, (*it_names).c_str( ),
                 (*it_descs).c_str( ) );
    }

    fputc( '\n', stdout );
}

void Terminal::displayAttribute( const std::string & name,
                                 const std::string & value )
{
    if( mute )
        return;

    fprintf( stdout, "'%s' is:\n\n%s\n", name.c_str( ), value.c_str( ) );
}

void Terminal::print( const char * msg, ... )
{
    va_list ap;

    va_start( ap, msg );
    vfprintf( file, msg, ap );
    va_end( ap );
}

FILE * Terminal::getFile( void )
{
    return file;
}

/* Writes output only if mute is false */
void Terminal::setMute( bool status )
{
    mute = status;
}

bool Terminal::isMute( void )
{
    return mute;
}

void Terminal::setPager( const Name & name )
{
    pager_name = name;
}

const char * Terminal::getPager( void )
{
    if( pager_name == "" )
        return "(none)";
    else
        return pager_name.c_str( );
}

void Terminal::displayMessage( const char * msg, ... )
{
    if( mute )
        return;

    va_list ap;

    va_start( ap, msg );
    vfprintf( stdout, msg, ap );
    fprintf( stdout, "\n" );
    va_end( ap );
}

void Terminal::displayErrorMessage( const char * msg, ... )
{
    va_list ap;

    va_start( ap, msg );
    if( pfile_name )
        fprintf( stderr, "%s:%d: ", pfile_name, *pline_number );
    fprintf( stderr, "error: " );
    vfprintf( stderr, msg, ap );
    fprintf( stderr, "\n" );

    va_end( ap );
}

void Terminal::displayWarningMessage( const char * msg, ... )
{
    va_list ap;
    
    va_start( ap, msg );
    if( pfile_name )
        fprintf( stderr, "%s:%d: ", pfile_name, *pline_number );
    fprintf( stderr, "warning: " );
    vfprintf( stderr, msg, ap );
    fprintf( stderr, "\n" );
    va_end( ap );
}

void Terminal::setContext( const char * pname, const int * pline )
{
    pfile_name = pname;
    pline_number = pline;
}
