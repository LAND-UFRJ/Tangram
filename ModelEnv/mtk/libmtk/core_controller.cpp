#include "core_controller.h"

namespace CoreController
{
    ObjectManager  *object_manager;
    CoreOptionList  option_list;

    /**
     * Setup MTK core
     */
    void setupCore( std::list<std::string> path_list,
                    bool                   use_seed_value,
                    long                   seed_value )
    {
        // Initialize random seed
        long seedval;        

        if( use_seed_value )
            seedval = seed_value;
        else
            seedval = time( NULL );

        srand48( seedval );    
    
        // Create managers
        PluginManager::createPluginManager( );
        ObjectManager::createObjectManager( );

        PluginManager *plugin_manager = PluginManager::getPluginManager( );

        // Initialize plugin path
        std::list<std::string>::iterator it;
        for( it = path_list.begin( ); it != path_list.end( ); it++ )
            plugin_manager->addPath( *it );

        // load plugins
        plugin_manager->initPlugins( );

        object_manager = ObjectManager::getObjectManager( );

        addCoreOption( "active",   set_active,   get_active   );
        addCoreOption( "terminal", set_terminal, get_terminal );
        addCoreOption( "output",   set_output,   get_output   );
        addCoreOption( "pager",    set_pager,    get_pager    );
        addCoreOption( "version",  0,            get_version  );
    }

    void cleanCore( void )
    {
        ObjectManager::destroyObjectManager( );
        PluginManager::destroyPluginManager( );
    }

    Object *getObjectByName( const Name & object_name )
    {
        return object_manager->getObjectByName( object_name );
    }

    bool addCoreOption( const Name & name,
                        SetCallback set_callback, GetCallback get_callback )
    {
        bool status;
        CoreOption option;

        status = false;

        if( !hasCoreOption( name ) )
        {
            option.name = name;
            option.set_callback = set_callback;
            option.get_callback = get_callback;

            option_list.push_back( option );
            status = true;
        }

        return status;
    }

    bool hasCoreOption( const Name & name )
    {
        CoreOptionList::const_iterator it;

        it = std::find( option_list.begin( ), option_list.end( ), name );

        return (it != option_list.end( ));
    }

    bool set_active( const std::string & value )
    {
        return object_manager->setActiveObject( value );
    }

    bool get_active( std::string & value )
    {
        value = object_manager->getActiveObjectName( );
        if( value.length( ) == 0 )
            value = "(none)";
        return true;
    }

    bool set_terminal( const std::string & value )
    {
        if( value == "screen" )
        {
            return Terminal::setTerminal( Terminal::TERMINAL_SCREEN );
        }
        else if( value == "file" )
        {
            return Terminal::setTerminal( Terminal::TERMINAL_FILE );
        }

        Terminal::displayErrorMessage( "invalid terminal type to set '%s'",
                                       value.c_str( ) );

        return false;
    }

    bool get_terminal( std::string & value )
    {
        Terminal * t;

        t = Terminal::getTerminal( );

        if( !t )
            return false;

        switch( t->getType( ) )
        {
        case Terminal::TERMINAL_SCREEN:
            value = "screen";
            return true;
        case Terminal::TERMINAL_FILE:
            value = "file";
            return true;
        default:
            break;
        }

        return false;
    }

    bool set_output( const std::string & value )
    {
        Terminal * t;

        t = Terminal::getTerminal( );

        if( !t )
            return false;

        t->setOutputName( value );
        return true;
    }

    bool get_output( std::string & value )
    {
        Terminal * t;

        t = Terminal::getTerminal( );

        if( !t )
            return false;

        value = t->getOutputName( );
        return true;
    }

    bool set_pager( const std::string & value )
    {
        Terminal::setPager( value );
        return true;
    }

    bool get_pager( std::string & value )
    {
        value = Terminal::getPager( );
        return true;
    }

    bool get_version( std::string & value )
    {
        value = MTK_VERSION_STRING;
        return true;
    }

    bool set( const Name & attr_name, const std::string & attr_value )
    {
        bool status;
        SetCallback set_callback;
        CoreOptionList::iterator it;

        status = false;

        it = std::find( option_list.begin( ), option_list.end( ), attr_name );

        if( it != option_list.end( ) )
        {
            set_callback = (*it).set_callback;
            if( set_callback )
                status = set_callback( attr_value );
            else
                Terminal::displayErrorMessage( "option '%s' cannot be set",
                                               attr_name.c_str( ) );

            show( attr_name );
        }
        else
        {
            Terminal::displayErrorMessage( "invalid option to set: '%s'",
                                           attr_name.c_str( ) );
        }

        return status;
    }

    bool get( const Name & attr_name, std::string & attr_value )
    {
        bool status;
        GetCallback get_callback;
        CoreOptionList::iterator it;

        status = false;

        it = std::find( option_list.begin( ), option_list.end( ), attr_name );

        if( it != option_list.end( ) )
        {
            get_callback = (*it).get_callback;
            if( get_callback )
            {
                get_callback( attr_value ); // silently ignore return value
                status = true;
            }
        }

        return status;
    }

    bool show( const Name & attr_name )
    {
        std::string value;

        if( get( attr_name, value ) )
        {
            Terminal::displayAttribute( attr_name, value );
        } 
        else
        {
            Terminal::displayErrorMessage( "invalid option to show: '%s'",
                                           attr_name.c_str( ) );
        }

        return true;
    }
}
