#include "action.h"

#include "types.h"
#include "terminal.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Action
//
///////////////////////////////////////////////////////////////////////////////
Action::Action( void )
{
    CoreInterface::init();

    ui = UserInterface::getUserInterface( );
}

///////////////////////////////////////////////////////////////////////////////
//
//  QuitAction
//
///////////////////////////////////////////////////////////////////////////////
bool QuitAction::execute( void )
{
    ui->quit( );
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//  ClearAction
//
///////////////////////////////////////////////////////////////////////////////
bool ClearAction::execute( void )
{
    CoreInterface::clear( );
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//  WhichAction
//
///////////////////////////////////////////////////////////////////////////////
WhichAction::WhichAction( const Name & name ) : object_name(name)
{
}

bool WhichAction::execute( void )
{
    return CoreInterface::which( object_name );
}

///////////////////////////////////////////////////////////////////////////////
//
//  SourceAction
//
///////////////////////////////////////////////////////////////////////////////
SourceAction::SourceAction( const Name & name ) : file_name(name)
{
}

bool SourceAction::execute( void )
{
    return ui->runCommandFile( file_name );
}

///////////////////////////////////////////////////////////////////////////////
//
//  HistoryAction
//
///////////////////////////////////////////////////////////////////////////////
HistoryAction::HistoryAction( int n ) : size(n)
{
}

bool HistoryAction::execute( void )
{
    return ui->history( size );
}

///////////////////////////////////////////////////////////////////////////////
//
//  ListAction
//
///////////////////////////////////////////////////////////////////////////////
ListAction::ListAction( ListType type ) : list_type(type)
{
}

bool ListAction::execute( void )
{
    switch( list_type )
    {
        case LIST_PLUGINS:
            CoreInterface::listPlugins( );
            break;
        case LIST_OBJECTS:
            CoreInterface::listObjects( );
            break;
        default:
            return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//  HelpAction
//
///////////////////////////////////////////////////////////////////////////////
HelpAction::HelpAction( TokenType type )
           : help_type(HELP_CORE), command_type(type)
{
}

HelpAction::HelpAction( const Name & plugin, const Name & command )
           : help_type(HELP_PLUGIN), command_type(TK_IDENTIFIER),
             plugin_name(plugin), command_name(command)
{
}

bool HelpAction::execute( void )
{
    switch( help_type )
    {
        case HELP_CORE:
            CoreInterface::help( command_type );
            break;
        case HELP_PLUGIN:
            CoreInterface::help( plugin_name, command_name );
            break;
        default:
            return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//  CoreSetAction
//
///////////////////////////////////////////////////////////////////////////////
CoreSetAction::CoreSetAction( const std::string & name,
                              const std::string & value )
              : attribute_name(name), attribute_value(value)
{
}

bool CoreSetAction::execute( void )
{
    return CoreController::set( attribute_name, attribute_value );
}

///////////////////////////////////////////////////////////////////////////////
//
//  CoreShowAction
//
///////////////////////////////////////////////////////////////////////////////
CoreShowAction::CoreShowAction( const std::string & name )
               : attribute_name(name)
{
}

bool CoreShowAction::execute( void )
{
    return CoreController::show( attribute_name );
}

///////////////////////////////////////////////////////////////////////////////
//
//  CreateAction
//
///////////////////////////////////////////////////////////////////////////////
CreateAction::CreateAction( const std::string & plg_name,
                            const std::string & obj_name,
                            TokenList::const_iterator begin,
                            TokenList::const_iterator end )
             : object_name(obj_name), plugin_name(plg_name),
               arguments(begin,end)
{
}

bool CreateAction::execute( void )
{
    return CoreInterface::create( object_name, plugin_name, arguments );
}

///////////////////////////////////////////////////////////////////////////////
//
//  CopyAction
//
///////////////////////////////////////////////////////////////////////////////
CopyAction::CopyAction( const std::string & obj_name,
                        const std::string & cpy_name )
           : object_name(obj_name), copy_name(cpy_name)
{
}

bool CopyAction::execute( void )
{
    return CoreInterface::copy( object_name, copy_name );
}

///////////////////////////////////////////////////////////////////////////////
//
//  ObjectSetAction
//
///////////////////////////////////////////////////////////////////////////////
ObjectSetAction::ObjectSetAction( const std::string & obj_name,
                                  const std::string & prm_name,
                                  const std::string & val,
                                  const Index & idx )
                : object_name(obj_name), param_name(prm_name),
                  value(val), index(idx)
{
}

bool ObjectSetAction::execute( void )
{
    return CoreInterface::object_set( object_name, param_name, value, index );
}

///////////////////////////////////////////////////////////////////////////////
//
//  ObjectShowAction
//
///////////////////////////////////////////////////////////////////////////////
ObjectShowAction::ObjectShowAction( const std::string & obj_name,
                                    const std::string & prm_name,
                                    const Index & idx )
                : object_name(obj_name), param_name(prm_name), index(idx)
{
}

bool ObjectShowAction::execute( void )
{
    return CoreInterface::object_show( object_name, param_name, index );
}

///////////////////////////////////////////////////////////////////////////////
//
//  DestroyAction
//
///////////////////////////////////////////////////////////////////////////////
DestroyAction::DestroyAction( const std::string & obj_name )
              : object_name(obj_name)
{
}

bool DestroyAction::execute( void )
{
    return CoreInterface::destroy( object_name );
}

///////////////////////////////////////////////////////////////////////////////
//
//  DisplayAction
//
///////////////////////////////////////////////////////////////////////////////
DisplayAction::DisplayAction( const std::string & obj_name,
                              const std::string & dpy_name )
             : object_name(obj_name), display_name(dpy_name)
{
}

bool DisplayAction::execute( void )
{
    return CoreInterface::display( object_name, display_name );
}

///////////////////////////////////////////////////////////////////////////////
//
//  ExecAction
//
///////////////////////////////////////////////////////////////////////////////
ExecAction::ExecAction( const std::string & obj_name,
                        const std::string & cmd_name,
                        TokenList::const_iterator begin,
                        TokenList::const_iterator end )
           : object_name(obj_name), command_name(cmd_name), arguments(begin,end)
{
}

bool ExecAction::execute( void )
{
    return CoreInterface::exec( object_name, command_name, arguments );
}
