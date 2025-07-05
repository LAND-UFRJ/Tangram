#include "plugin_manager.h"

#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "terminal.h"

PluginManager * PluginManager::the_plugin_manager    = 0;
const char    * PluginManager::PLUGIN_FILE_EXTENSION = ".so";
/* edmundo */
/*const char    * PluginManager::PLUGIN_FILE_EXTENSION = ".pgn";*/

bool PluginManager::createPluginManager( void )
{
    if( the_plugin_manager )
        return false;

    the_plugin_manager = new PluginManager( );
    return true;
}

bool PluginManager::destroyPluginManager( void )
{
    if( !the_plugin_manager )
        return false;

    delete the_plugin_manager;
    return true;
}

PluginManager * PluginManager::getPluginManager( void )
{
    return the_plugin_manager;
}

PluginManager::PluginManager( void )
{
}

PluginManager::~PluginManager( void )
{
    PluginList::iterator it;

    path_list.clear( );

    for( it = plugin_list.begin( ); it != plugin_list.end( ); it++ )
    {
        delete (*it);
    }
    plugin_list.clear( );
}

bool PluginManager::addPath( const Name & name )
{
    std::list<Path>::iterator it;
    struct stat buffer;

    // check if directory exists
    if( stat( name.c_str( ), &buffer ) < 0 || !S_ISDIR(buffer.st_mode) )
        return false;

    // check if directory is already in path_list
    it = std::find( path_list.begin( ), path_list.end( ), name );
    if( it != path_list.end( ) )
        return false;

    // add valid directory to path_list
    path_list.push_back( Path( name ) );
    return true;
}

bool PluginManager::removePath( const Name & name )
{
    std::list<Path>::iterator it, next_it;

    // check if directory is in path_list
    it = std::find( path_list.begin( ), path_list.end( ), name );

    if( it == path_list.end( ) )
        return false;

    // remove each occurrence of name in path_list
    while( it != path_list.end( ) )
    {
        next_it = it;
        next_it++;
        path_list.erase( it );
        it = std::find( next_it, path_list.end( ), name );
    }

    return true;
}

bool PluginManager::initPlugins( void )
{
    int total_plugins;
    std::list<Path>::iterator it_path;

    NameList plugin_name_list;
    NameList::iterator it_name;

    PluginList::iterator it_plugin;
    Plugin * plugin;

    // zero the plugin counter
    total_plugins = 0;

    // clear plugin name list
    plugin_name_list.clear( );

    // look for plugins
    for( it_path = path_list.begin( ); it_path != path_list.end( ); it_path++ )
    {
        // get plugin names from the path
        total_plugins += (*it_path).scanForPlugins( plugin_name_list );

        // for each plugin found
        for( it_name = plugin_name_list.begin( );
             it_name != plugin_name_list.end( );
             it_name++ )
        {
            plugin = Plugin::createPlugin( (*it_path).path_name, (*it_name) );

            if( plugin )
            {
                // check if plugin is already in list
                it_plugin = std::find_if( plugin_list.begin( ),
                                          plugin_list.end( ),
                                          Plugin::Equal(plugin) );

                // if plugin is duplicated
                if( it_plugin != plugin_list.end( ) )
                {
                    // *it_plugin is a copy of plugin
                    delete plugin;
                }
                else // else insert into list
                {
                    plugin_list.push_back( plugin );
                }
            }
        }

        // clear plugin name list
        plugin_name_list.clear( );
    }

    return true;
}

void PluginManager::listPlugins( NameList & plugins )
{
    PluginList::iterator it;

    for( it = plugin_list.begin( ); it != plugin_list.end( ); it++ )
    {
        plugins.push_back( (*it)->getName( ) );
    }
}

void PluginManager::listPlugins( NameList & plugins, NameList & descs )
{
    PluginList::iterator it;

    for( it = plugin_list.begin( ); it != plugin_list.end( ); it++ )
    {
        plugins.push_back( (*it)->getName( ) );
        descs.push_back( (*it)->getDescription( ) );
    }
}

Plugin * PluginManager::getPlugin( const Name & plugin_name )
{
    Plugin * plugin;
    PluginList::iterator it;

    plugin = 0;

    for( it = plugin_list.begin( ); it != plugin_list.end( ); it++ )
    {
        if( strcmp( plugin_name.c_str( ), (*it)->getName( ) ) == 0 )
        {
            plugin = (*it);
            break;
        }
    }

    if( !plugin )
    {
        Terminal::displayErrorMessage( "could not find a plugin named '%s'",
                                       plugin_name.c_str( ) );
    }

    return plugin;
}

PluginManager::PluginList PluginManager::getPluginList( void )
{
    return plugin_list;
}

PluginManager::Path::Path( const Name & name ) : path_name(name)
{
    struct stat buffer;

    path_inode = 0;

    if( stat( path_name.c_str( ), &buffer ) == 0 && S_ISDIR(buffer.st_mode) )
    {
        path_inode = buffer.st_ino;
    }
}

PluginManager::Path::~Path( void )
{
}

int PluginManager::Path::scanForPlugins( NameList & name_list )
{
    int n_dirent, i;
    struct dirent ** dirent_list;
    PluginList::iterator it;

    // scan directory for files
    n_dirent = scandir( path_name.c_str( ), &dirent_list,
                        plugin_filter, alphasort );

    // error, or no plugins found
    if( n_dirent <= 0 )
        return 0;

    // save all plugins found in the list
    for( i = 0; i < n_dirent; i++ )
    {
        // insert plugin's path in list
        name_list.push_back( dirent_list[i]->d_name );

        // free dirent's memory
        free( dirent_list[i] );
    }
    free( dirent_list );

    return n_dirent;
}

int PluginManager::Path::plugin_filter( const struct dirent * dir )
{
    int name_length;

    if( !dir )
        return 0;

    name_length = strlen( dir->d_name );

    return( strcmp( &(dir->d_name[ name_length - 3 ]), /* edmundo: era 4 em vez de 3*/
                    PluginManager::PLUGIN_FILE_EXTENSION ) == 0 );
}

bool PluginManager::Path::operator == ( const Name & name ) const
{
    struct stat buffer;

    if( stat( name.c_str( ), &buffer ) < 0 )
        perror( "stat" );

    return (buffer.st_ino == path_inode);
}
