#include "core_interface.h"

namespace CoreInterface
{
    PluginManager *plugin_manager;
    ObjectManager *object_manager;

    void init( void )
    {
        // data members
        plugin_manager = PluginManager::getPluginManager( );
        object_manager = ObjectManager::getObjectManager( );
    }

    bool create( const Name & object_name, const Name & plugin_name,
                 Arguments & args )
    {
        Plugin * plugin;
        bool status;

        status = false;

        plugin = plugin_manager->getPlugin( plugin_name );

        if( plugin )
        {
            status = object_manager->createObject( plugin, object_name, args );
        }

        return status;
    }

    bool destroy( const Name & object_name )
    {
        return object_manager->destroyObject( object_name );
    }

    bool display( const Name & object_name, const Name & display_name )
    {
        bool status;
        Object * object;

        status = false;

        if( object_name == "" )
            object = object_manager->getActiveObject( );
        else
            object = object_manager->getObjectByName( object_name );

        if( object )
        {
            status = object->display( display_name );
        }

        return status;
    }
    
    bool exec( const Name & object_name, const Name & command_name,
               Arguments & args )
    {
        bool status;
        Object * object;

        status = false;

        if( object_name == "" )
            object = object_manager->getActiveObject( );
        else
            object = object_manager->getObjectByName( object_name );

        if( object )
        {
            status = object->exec( command_name, args );
        }

        return status;
    }

    bool help( const Name & plugin_name, const Name & command_name )
    {
        Plugin * plugin;
        bool status;

        status = false;

        plugin = plugin_manager->getPlugin( plugin_name );

        if( plugin )
        {
            status = plugin->help( command_name );
        }

        return status;
    }

    bool help( const TokenType command_type )
    {
        switch( command_type )
        {
            case TK_KW_SET:
                Terminal::displayMessage( SET_HELP_MESSAGE );
                break;
            case TK_KW_SHOW:
                Terminal::displayMessage( SHOW_HELP_MESSAGE );
                break;
            case TK_KW_LIST:
                Terminal::displayMessage( LIST_HELP_MESSAGE );
                break;
            case TK_KW_HELP:
                Terminal::displayMessage( HELP_HELP_MESSAGE );
                break;
            case TK_KW_QUIT:
                Terminal::displayMessage( QUIT_HELP_MESSAGE );
                break;
            default:
                Terminal::displayErrorMessage( "invalid core command" );
                return false;
        }
        return true;
    }

    bool object_set( const Name & object_name, const Name & param_name,
                     const std::string & value, const Index & index )
    {
        bool status;
        Object * object;

        status = false;

        if( object_name == "" )
            object = object_manager->getActiveObject( );
        else
            object = object_manager->getObjectByName( object_name );

        if( object )
        {
            status = object->set( param_name, value, index );
            if( status )
                object_show( object_name, param_name, index );
        }

        return status;
    }

    bool object_show( const Name & object_name, const Name & param_name,
                      const Index & index )
    {
        bool status;
        Object * object;
        Name value;
        Name name;

        status = false;

        if( object_name == "" )
            object = object_manager->getActiveObject( );
        else
            object = object_manager->getObjectByName( object_name );

        if( object )
        {
            status = object->get( param_name, value, index );
        }

        if( status )
        {
            name = object->getName( );
            name += '.';
            name += param_name;
            Terminal::displayAttribute( name, value );
        } 

        return status;
    }

    bool copy( const Name & destination_name, const Name & source_name )
    {
        return object_manager->copy( destination_name, source_name );
    }

    bool clear( void )
    {
        object_manager->clear( );
        return true;
    }

    bool which( const Name & object_name )
    {
        Object * object;

        object = object_manager->getObjectByName( object_name );
        if( object )
        {
            Terminal::displayAttribute( object_name,
                                        object->getPlugin( )->getName( ) );
            return true;
        }
        else
            return false;
    }

    bool listPlugins( void )
    {
        NameList names, descriptions;

        plugin_manager->listPlugins( names, descriptions );
        Terminal::displayList( "Available Plugins", names, descriptions );

        return true;
    }

    bool listObjects( void )
    {
        NameList names;

        object_manager->listObjects( names );
        Terminal::displayList( "Instanced Objects", names );

        return true;
    }
}
