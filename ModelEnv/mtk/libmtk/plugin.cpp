#include "plugin.h"

#include "terminal.h"

Plugin::Plugin( const Name & name ) : lib_filename(name)
{
    lib_handle = 0;
    lib_info   = 0;
}

Plugin::~Plugin( void )
{
    if( lib_handle )
        unload( );
}

bool Plugin::load( void )
{
    if( !lib_handle )
    {
        lib_handle = dlopen( lib_filename.c_str( ), RTLD_NOW );

        if( lib_handle )
        {
            if( !init( ) ) // try to load symbols or else unload library
                unload( );
        }
        else
        {
            Terminal::displayErrorMessage( "could not load library '%s': %s",
                                           lib_filename.c_str( ), dlerror( ) );
        }
    }

    return (lib_handle != 0);
}

bool Plugin::unload( void )
{
    if( lib_handle )
    {
        if( dlclose( lib_handle ) == 0 )
            lib_handle = 0;
    }

    return (lib_handle == 0);
}

bool Plugin::init( void )
{
    lib_info = (Object::Info *)dlsym( lib_handle, "object_info" );

    return (lib_info != 0);
}

const char * Plugin::getName( void ) const
{
    return lib_info->id_string;
}

const char * Plugin::getDescription( void ) const
{
    return lib_info->info_string;
}

bool Plugin::help( const Name & command_name ) const
{
    if( command_name == "" )
        Terminal::displayMessage( lib_info->help_string );
    return lib_info->help_function( command_name );
}

Object * Plugin::createObject( const Name & name,
                               Arguments & args ) const
{
    return lib_info->create( name, args );
}

Plugin * Plugin::createPlugin( const Name & dir_name,
                               const Name & file_name )
{
    Plugin * plugin;
    Name path_name;
    int n;

    plugin = 0;

    path_name = dir_name;

    n = dir_name.length( );
    if( n > 0 && dir_name[ n-1 ] != '/' )
        path_name += '/';

    path_name += file_name;

    plugin = new Plugin( path_name );

    if( plugin && !(plugin->load( )) )
    {
        delete plugin;
        plugin = 0;
    }

    return plugin;
}

Plugin::Equal::Equal( const Plugin * p ) : plugin(p)
{
}

bool Plugin::Equal::operator () ( const Plugin * p )
{
    return (plugin->lib_handle == p->lib_handle ||
            strcmp( plugin->lib_info->id_string,
                         p->lib_info->id_string ) == 0 );
}
