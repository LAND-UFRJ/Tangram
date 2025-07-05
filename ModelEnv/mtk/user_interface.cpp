#include "user_interface.h"

#include <iostream>
#include <iomanip>

#include <readline/readline.h>
#include <readline/history.h>

#include <signal.h>
#include <errno.h>

#include "action.h"
#include "terminal.h"
#include "signal_manager.h"

UserInterface * UserInterface::the_user_interface = 0;

bool UserInterface::createUserInterface( void )
{
    if( the_user_interface )
        return false;

    the_user_interface = new UserInterface( );
    return true;
}

bool UserInterface::destroyUserInterface( void )
{
    if( !the_user_interface )
        return false;

    delete the_user_interface;
    return true;
}

UserInterface * UserInterface::getUserInterface( void )
{
    return the_user_interface;
}

UserInterface::UserInterface( void )
{
    context.end_of_input          = false;
    context.quit_command_received = false;
    context.command_error         = false;

    Terminal::setTerminal( Terminal::TERMINAL_SCREEN );

    SignalManager::ignoreInterrupt( );
}

UserInterface::~UserInterface( void )
{
}

bool UserInterface::endOfCommands( void )
{
    return (context.end_of_input ||
            context.quit_command_received ||
            context.command_error);
}

bool UserInterface::endOfMainLoop( void )
{
    return (context.end_of_input || context.quit_command_received);
}

void UserInterface::quit( void )
{
    context.quit_command_received = true;
}

bool UserInterface::history( int size )
{
    HISTORY_STATE * state;
#ifdef __APPLE__
    HIST_ENTRY *hist;
#endif

    int base, i;

    if( size < 1 )
    {
        Terminal::displayErrorMessage( "cannot display less than one command" );
        return false;
    }

    state = history_get_history_state( );

    if( !state )
    {
        Terminal::displayErrorMessage( "could not get command history state" );
        return false;
    }

    if( state->length < size )
    {
        Terminal::displayErrorMessage( "history only has %d commands",
                                       state->length );
        size = state->length;
    }

    base = state->length - size;
    for( i = 0; i < size; i++ )
    {
#ifdef __APPLE__
        hist = history_get(base+i);
        Terminal::displayMessage( "%5d  %s", base+i,
                                  hist->line );
#else
        Terminal::displayMessage( "%5d  %s", base+i,
                                  state->entries[base+i]->line );
#endif
    }

    return true;
}

bool UserInterface::runCommandFile( const Name & file_name )
{
    CommandList command_list;
    CommandList::iterator it;
    Action * action;
    InputContext old_context;
    bool mute_terminal;
    bool status;
    
    // save old context
    old_context = context;

    context.mode = INPUT_MODE_BATCH;
    context.file_name = file_name.c_str( );
    context.file = fopen( file_name.c_str( ), "r" );
    if( !context.file )
    {
        Terminal::displayErrorMessage(
                                   "could not open file '%s' for reading.\n\n",
                                   file_name.c_str( ) );
        context = old_context;
        return false;
    }

    context.line_number = 0;
    context.end_of_input          = false;
    context.quit_command_received = false;
    context.command_error         = false;
    mute_terminal = Terminal::isMute( );
    Terminal::setMute( true );

    Terminal::setContext( context.file_name, &context.line_number );

    while( !endOfCommands( ) )
    {
        // get next command
        if( getUserInput( command_list ) && !context.end_of_input )
        {
            for( it = command_list.begin( ); it != command_list.end( ); it++ )
            {
                action = (*it).parse( );

                if( action )
                {
                    if( !action->execute( ) )
                    {
                        // run-time error - signalled by Action::execute()
                        delete action;
                        action = 0;
                        context.command_error = true;
                        break;
                    }

                    delete action;
                    action = 0;
                }
                else
                {
                    // syntax error - signalled by Command::parse()
                    context.command_error = true;
                    break;
                }
            }
        }
        else if( !context.end_of_input )
        {
            // lexical error - signalled by UserInterface::getUserInput()
            context.command_error = true;
            break;
        }
    }

    status = !context.command_error;

    fclose( context.file );
    context = old_context;
    Terminal::setMute( mute_terminal );
    Terminal::setContext( context.file_name, &context.line_number );

    return status;
}

void UserInterface::runMainLoop( void )
{
    CommandList command_list;
    CommandList::iterator it;
    Action * action;
    Name history_filename;

    context.mode = INPUT_MODE_ONLINE;
    context.file = 0;
    context.file_name = 0;
    context.line_number = 0;

    context.end_of_input          = false;
    context.quit_command_received = false;
    context.command_error         = false;
    Terminal::setMute( false );
    Terminal::setContext( context.file_name, &context.line_number );

    command_count = 0;
    updateUserPrompt( );

    n_cmds = 0;
    using_history( );
    history_filename = getenv( "HOME" );
    history_filename += "/.mtk_history";
    if( read_history( history_filename.c_str( ) ) )
    {
        if( errno == ENOENT )
            write_history( history_filename.c_str( ) );
        else
            perror( "error reading history" );
    }

    while( !endOfMainLoop( ) )
    {
        // get next command
        if( getUserInput( command_list ) && !context.end_of_input )
        {           
            for( it = command_list.begin( ); it != command_list.end( ); it++ )
            {               
                action = (*it).parse( );

                if( action )
                {
                    if( !action->execute( ) )
                    {
                        delete action;
                        action = 0;
                        break;
                    }
                    delete action;
                    action = 0;
                }
                else
                    break;
            }

            // update prompt counter if this was a valid command list
            if( command_list.size( ) > 0 &&
                it == command_list.end( ) )
            {
                updateUserPrompt( );
            }
        }
    }

#ifdef __APPLE__
    if( write_history( history_filename.c_str( ) ) )
        perror( "error writing history" );    
#else
    if( append_history( n_cmds, history_filename.c_str( ) ) )
        perror( "error writing history" );
    if( history_truncate_file( history_filename.c_str( ), 1000 ) )
        perror( "error writing history" );
#endif
}

bool UserInterface::getUserInput( CommandList & command_list )
{
    char   buffer[1024];
    char * command_line;
    bool   status;

    status = true;
    command_line = 0;

    if( context.mode == INPUT_MODE_ONLINE )
        command_line = readline( user_prompt );
    else if( context.mode == INPUT_MODE_BATCH )
    {
        command_line = fgets( buffer, 1024, context.file );
        context.line_number++;
    }

    if( command_line )
    {
        if( !Command::readFromString( command_line, command_list ) )
        {
            Terminal::displayErrorMessage( "unknown symbols found in command" );
            status = false;
        }
        else if( command_list.size( ) > 0 )
        {
            if( context.mode == INPUT_MODE_ONLINE )
            {
                add_history( command_line );
                n_cmds++;
            }
        }

        if( context.mode == INPUT_MODE_ONLINE )
            free( command_line );
    }
    else
    {
        if( context.mode == INPUT_MODE_ONLINE )
            fputc( '\n', stdout );
        context.end_of_input = true;
    }

    return status;
}

void UserInterface::updateUserPrompt( void )
{
    command_count++;
    sprintf( user_prompt, "MTK:%d> ", command_count );
}
