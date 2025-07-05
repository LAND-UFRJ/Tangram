#ifndef OBJECT_BASE_H
#define OBJECT_BASE_H

#include <algorithm>
#include <string>
#include <list>

#include "object.h"
#include "terminal.h"
#include "prototype.h"
#include "signal_manager.h"

#define DEFAULT_HELP_MESSAGE "No help available\n"

////////////////////////////////////////////////////////////////////////////////
//
//  Class declaration
//
////////////////////////////////////////////////////////////////////////////////
template <class Owner>
class ObjectBase : public Object
{
public:
    ObjectBase( const std::string &, Owner * );

    virtual bool set( const std::string &, const std::string &,
                      const Index & );
    virtual bool get( const std::string &, std::string &,
                      const Index & );

    virtual Object * copy( const std::string & );

    virtual bool display( const std::string & );
    virtual bool exec( const std::string &, Arguments & );

    virtual void * run( const std::string &, void * );

    virtual bool checkOption( const std::string & name )
    {
        return hasOption( name );
    }

    virtual bool checkDisplay( const std::string & name )
    {
        return hasDisplay( name );
    }
    
    virtual bool checkCommand( const std::string & name )
    {
        return hasCommand( name );
    }

    virtual bool checkFunction( const std::string & name )
    {
        return hasFunction( name );
    }

    static void listOptions ( NameList & );
    static void listOptions ( NameList &, NameList & );
    static void listDisplays( NameList & );
    static void listDisplays( NameList &, NameList & );
    static void listCommands( NameList & );

    static Object * create( const std::string &, Arguments & );
    static bool help( const Name & );

protected:
    // protected data types
    typedef Object * (*ConstructorCallback)( const std::string &, Arguments & );
    typedef bool (Owner::*SetCallback)( const std::string &,
                                        const Index & );
    typedef bool (Owner::*GetCallback)( std::string &,
                                        const Index & );
    typedef bool (Owner::*DisplayCallback)( Terminal & );
    typedef bool (Owner::*CommandCallback)( Arguments & );
    typedef void * (Owner::*FunctionCallback)( void * );

    // protected functions
    static bool addConstructor( ConstructorCallback, const Prototype & );
    static bool addOption     ( const std::string &, const std::string &, int,
                                SetCallback = 0, GetCallback = 0 );
    static bool addDisplay    ( const std::string &, const std::string &,
                                DisplayCallback );
    static bool addCommand    ( const std::string &, CommandCallback,
                                const Prototype &,
                                const std::string & = DEFAULT_HELP_MESSAGE );
    static bool addFunction   ( const std::string &, FunctionCallback );

private:
    // private data types
    typedef struct tagConstructor
    {
        ConstructorCallback constructor_callback;
        Prototype prototype;

        bool operator == ( const struct tagConstructor & c ) const
        {
            return (prototype == c.prototype);
        }
    } Constructor;

    typedef std::list<Constructor> ConstructorList;
    typedef typename ConstructorList::iterator ConstructorIterator;
    typedef typename ConstructorList::const_iterator ConstructorConstIterator;

    typedef struct
    {
        std::string  name;
        std::string  description;
        unsigned int dimension;
        SetCallback  set_callback;
        GetCallback  get_callback;

        bool operator == ( const std::string & str ) const
        {
            return (name == str);
        }
    } Option;

    typedef std::list<Option> OptionList;
    typedef typename OptionList::iterator OptionIterator;
    typedef typename OptionList::const_iterator OptionConstIterator;

    typedef struct
    {
        std::string name;
        std::string  description;
        DisplayCallback display_callback;

        bool operator == ( const std::string & str ) const
        {
            return (name == str);
        }
    } Display;

    typedef std::list<Display> DisplayList;
    typedef typename DisplayList::iterator DisplayIterator;
    typedef typename DisplayList::const_iterator DisplayConstIterator;

    typedef struct tagCommand
    {
        std::string name;
        CommandCallback command_callback;
        Prototype prototype;
        std::string help_string;

        bool operator == ( const std::string & str ) const
        {
            return (name == str);
        }

        bool operator == ( const struct tagCommand & c ) const
        {
            return ((name == c.name) && (prototype == c.prototype));
        }
    } Command;

    typedef std::list<Command> CommandList;
    typedef typename CommandList::iterator CommandIterator;
    typedef typename CommandList::const_iterator CommandConstIterator;

    typedef struct
    {
        std::string name;
        FunctionCallback function_callback;

        bool operator == ( const std::string & str ) const
        {
            return (name == str);
        }
    } Function;

    typedef std::list<Function> FunctionList;
    typedef typename FunctionList::iterator FunctionIterator;
    typedef typename FunctionList::const_iterator FunctionConstIterator;

    // private data members
    static ConstructorList constructor_list;
    static OptionList      option_list;
    static DisplayList     display_list;
    static CommandList     command_list;
    static FunctionList    function_list;

    Owner * owner;

    // private functions
    static bool addConstructor( const Constructor & );
    static bool addOption     ( const Option      & );
    static bool addDisplay    ( const Display     & );
    static bool addCommand    ( const Command     & );
    static bool addFunction   ( const Function    & );

    static bool hasConstructor( const Constructor & );
    static bool hasOption     ( const std::string & );
    static bool hasDisplay    ( const std::string & );
    static bool hasCommand    ( const std::string & );
    static bool hasCommand    ( const Command     & );
    static bool hasFunction   ( const std::string & );
};

////////////////////////////////////////////////////////////////////////////////
//
//  Static member variables definitions
//
////////////////////////////////////////////////////////////////////////////////
template <class Owner> typename
ObjectBase<Owner>::ConstructorList ObjectBase<Owner>::constructor_list;

template <class Owner> typename
ObjectBase<Owner>::OptionList  ObjectBase<Owner>::option_list;

template <class Owner> typename
ObjectBase<Owner>::DisplayList ObjectBase<Owner>::display_list;

template <class Owner> typename
ObjectBase<Owner>::CommandList ObjectBase<Owner>::command_list;

template <class Owner> typename
ObjectBase<Owner>::FunctionList ObjectBase<Owner>::function_list;

////////////////////////////////////////////////////////////////////////////////
//
//  Function definitions
//
////////////////////////////////////////////////////////////////////////////////
template <class Owner>
ObjectBase<Owner>::ObjectBase( const std::string & name, Owner * o )
                  : Object(name), owner(o)
{
}

template <class Owner>
bool ObjectBase<Owner>::set( const std::string & name,
                             const std::string & value,
                             const Index & index )
{
    bool status;
    SetCallback set_callback;
    OptionIterator it;

    status = false;

    it = std::find( option_list.begin( ), option_list.end( ), name );

    if( it != option_list.end( ) )
    {
        set_callback = (*it).set_callback;
        if( set_callback )
        {
            if( (*it).dimension == index.size( ) )
                status = (owner->*set_callback)( value, index );
            else
                Terminal::displayErrorMessage(
                            "wrong dimension in parameter '%s'",
                            name.c_str( ) ); // FIXME - awful error message
        }
        else
            Terminal::displayErrorMessage(
                        "parameter '%s' cannot be set in object '%s'",
                        name.c_str( ), object_name.c_str( ) );
    }
    else
    {
        Terminal::displayErrorMessage(
              "object '%s' does not have an option named '%s'",
              object_name.c_str( ), name.c_str( ) );
    }

    return status;
}

template <class Owner>
bool ObjectBase<Owner>::get( const std::string & name, std::string & value,
                             const Index & index )
{
    bool status;
    GetCallback get_callback;
    OptionIterator it;

    status = false;

    it = std::find( option_list.begin( ), option_list.end( ), name );

    if( it != option_list.end( ) )
    {
        get_callback = (*it).get_callback;
        if( get_callback )
        {
            if( (*it).dimension == index.size( ) )
                status = (owner->*get_callback)( value, index );
            else
                Terminal::displayErrorMessage(
                            "wrong dimension in parameter '%s'",
                            name.c_str( ) ); // FIXME - awful error message
        }
    }
    else
    {
        Terminal::displayErrorMessage(
              "object '%s' does not have an option named '%s'",
              object_name.c_str( ), name.c_str( ) );
    }

    return status;
}

template <class Owner>
Object * ObjectBase<Owner>::copy( const std::string & name )
{
    return new Owner( name, *owner );
}

template <class Owner>
Object * ObjectBase<Owner>::create( const std::string & name, Arguments & args )
{
    Object * object;
    ConstructorCallback constructor_callback;
    ConstructorIterator it;

    object = 0;

    for( it = constructor_list.begin( ); it != constructor_list.end( ); it++ )
    {
        constructor_callback = (*it).constructor_callback;

        if( constructor_callback && (*it).prototype.match( args ) )
        {
            object = constructor_callback( name, args );
            break;
        }
    }

    if( it == constructor_list.end( ) )
        Terminal::displayErrorMessage(
            "plugin does not have a constructor with this prototype" );

    return object;
}

template <class Owner>
bool ObjectBase<Owner>::display( const std::string & name )
{
    bool status;
    DisplayCallback display_callback;
    DisplayIterator it;
    Terminal * terminal;

    status = false;

    terminal = Terminal::getTerminal( );

    if( terminal )
    {
        terminal->open( );

        it = std::find( display_list.begin( ), display_list.end( ), name );

        if( it != display_list.end( ) )
        {
            display_callback = (*it).display_callback;
            if( display_callback )
            {
                terminal->print( "'%s' at '%s'\n\n",
                                 name.c_str( ), object_name.c_str( ) );
                status = (owner->*display_callback)( *terminal );
                terminal->print( "\n" );
            }
        }
        else
            Terminal::displayErrorMessage(
                "object '%s' does not have a display called '%s'",
                object_name.c_str( ), name.c_str( ) );

        terminal->close( );
    }

    return status;
}

template <class Owner>
bool ObjectBase<Owner>::exec( const std::string & name, Arguments & args )
{
    bool status;
    CommandCallback command_callback;
    CommandIterator it;

    status = false;

    it = std::find( command_list.begin( ), command_list.end( ), name );

    if( it != command_list.end( ) )
    {
        while( it != command_list.end( ) )
        {
            command_callback = (*it).command_callback;

            if( command_callback && (*it).prototype.match( args ) )
            {
                SignalManager::handleInterrupt( );
                status = (owner->*command_callback)( args );
                SignalManager::ignoreInterrupt( );
                break;
            }

            it++;
            it = std::find( it, command_list.end( ), name );
        }

        if( it == command_list.end( ) )
        {
            Terminal::displayErrorMessage(
                "argument does not match any prototype for '%s'",
                name.c_str( ) );

            Terminal::displayErrorMessage( "Possible alternatives are:" );

            it = std::find( command_list.begin( ), command_list.end( ), name );

            while( it != command_list.end( ) )
            {
                Terminal::displayErrorMessage( "    %s%s", name.c_str( ),
                                               (*it).prototype.toString( ) );

                it++;
                it = std::find( it, command_list.end( ), name );
            }
        }
    }
    else
        Terminal::displayErrorMessage(
            "object '%s' does not have a command called '%s'",
            object_name.c_str( ), name.c_str( ) );

    return status;
}

template <class Owner>
void * ObjectBase<Owner>::run( const std::string & name, void * args )
{
    FunctionCallback function_callback;
    FunctionIterator it;
    void * result;

    result = 0;

    it = std::find( function_list.begin( ), function_list.end( ), name );

    if( it != function_list.end( ) )
    {
        function_callback = (*it).function_callback;
        if( function_callback )
        {
            result = (owner->*function_callback)( args );
        }
    }
/*
    else
        Terminal::displayErrorMessage(
            "object '%s' does not have a function called '%s'",
            object_name.c_str( ), name.c_str( ) );
*/

    return result;
}

template <class Owner>
bool ObjectBase<Owner>::help( const Name & name )
{
    NameList name_list, desc_list;
    CommandIterator it;

    if( name == "" )
    {
        // list options
        listOptions( name_list, desc_list );
        Terminal::displayList( "Available attributes", name_list, desc_list );

        // list displays
        name_list.clear( );
        desc_list.clear( );
        listDisplays( name_list, desc_list );
        Terminal::displayList( "Available displays", name_list, desc_list );

        // list commands
        name_list.clear( );
        listCommands( name_list );
        Terminal::displayList( "Available methods", name_list );

        return true;
    }
    else
    {
        it = std::find( command_list.begin( ), command_list.end( ), name );

        if( it != command_list.end( ) )
            Terminal::displayMessage( (*it).help_string.c_str( ) );
        else
            Terminal::displayErrorMessage(
                "plugin does not have a method called '%s'", name.c_str( ) );

        return ( it != command_list.end( ) );
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructors
//
///////////////////////////////////////////////////////////////////////////////
template <class Owner>
bool ObjectBase<Owner>::addConstructor(ConstructorCallback constructor_callback,
                                       const Prototype & prototype )
{
    bool status;
    Constructor constructor;

    status = false;

    constructor.constructor_callback = constructor_callback;
    constructor.prototype = prototype;

    if( addConstructor( constructor ) )
        status = true;

    return status;
}

template <class Owner>
bool ObjectBase<Owner>::addConstructor(
                           const ObjectBase<Owner>::Constructor & constructor )
{
    bool status;

    status = false;

    if( !hasConstructor( constructor ) )
    {
        constructor_list.push_back( constructor );
        status = true;
    }

    return status;
}

template <class Owner>
bool ObjectBase<Owner>::hasConstructor(
                           const ObjectBase<Owner>::Constructor & constructor )
{
    ConstructorConstIterator it;

    it = std::find( constructor_list.begin( ), constructor_list.end( ),
                    constructor );

    return (it != constructor_list.end( ));
}


///////////////////////////////////////////////////////////////////////////////
//
//  Options
//
///////////////////////////////////////////////////////////////////////////////
template <class Owner>
bool ObjectBase<Owner>::addOption( const std::string & name,
                                   const std::string & description,
                                   int dimension,
                                   SetCallback set_callback,
                                   GetCallback get_callback )
{
    bool status;
    Option option;

    status = false;

    option.name         = name;
    option.description  = description;
    option.dimension    = dimension;
    option.set_callback = set_callback;
    option.get_callback = get_callback;

    if( addOption( option ) )
        status = true;

    return status;
}

template <class Owner>
bool ObjectBase<Owner>::addOption( const ObjectBase<Owner>::Option & option )
{
    bool status;

    status = false;

    if( !hasOption( option.name ) )
    {
        option_list.push_back( option );
        status = true;
    }

    return status;
}

template <class Owner>
bool ObjectBase<Owner>::hasOption( const std::string & name )
{
    OptionConstIterator it;

    it = std::find( option_list.begin( ), option_list.end( ), name );

    return (it != option_list.end( ));
}

template <class Owner>
void ObjectBase<Owner>::listOptions( NameList & name_list )
{
    OptionConstIterator it;
    Name name;
    unsigned int i;
    char ch;

    for( it = option_list.begin( ); it != option_list.end( ); it++ )
    {
        name = (*it).name;
        for( i = 0, ch = 'i'; i < (*it).dimension; i++, ch++ )
        {
            name += '[';
            name += ch;
            name += ']';
        }
        name_list.push_back( name );
    }
}

template <class Owner>
void ObjectBase<Owner>::listOptions( NameList & name_list,
                                     NameList & desc_list )
{
    OptionConstIterator it;
    Name name;
    unsigned int i;
    char ch;

    for( it = option_list.begin( ); it != option_list.end( ); it++ )
    {
        name = (*it).name;
        for( i = 0, ch = 'i'; i < (*it).dimension; i++, ch++ )
        {
            name += '[';
            name += ch;
            name += ']';
        }
        name_list.push_back( name );
        desc_list.push_back( (*it).description );
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Displays
//
///////////////////////////////////////////////////////////////////////////////
template <class Owner>
bool ObjectBase<Owner>::addDisplay( const std::string & name,
                                    const std::string & description,
                                    DisplayCallback display_callback )
{
    bool status;
    Display display;

    status = false;

    display.name             = name;
    display.description      = description;
    display.display_callback = display_callback;

    if( addDisplay( display ) )
        status = true;

    return status;
}

template <class Owner>
bool ObjectBase<Owner>::addDisplay( const ObjectBase<Owner>::Display & display )
{
    bool status;

    status = false;

    if( !hasDisplay( display.name ) )
    {
        display_list.push_back( display );
        status = true;
    }

    return status;
}

template <class Owner>
bool ObjectBase<Owner>::hasDisplay( const std::string & name )
{
    DisplayConstIterator it;

    it = std::find( display_list.begin( ), display_list.end( ), name );

    return (it != display_list.end( ));
}

template <class Owner>
void ObjectBase<Owner>::listDisplays( NameList & name_list )
{
    DisplayConstIterator it;

    for( it = display_list.begin( ); it != display_list.end( ); it++ )
        name_list.push_back( (*it).name );
}

template <class Owner>
void ObjectBase<Owner>::listDisplays( NameList & name_list,
                                      NameList & desc_list )
{
    DisplayConstIterator it;

    for( it = display_list.begin( ); it != display_list.end( ); it++ )
    {
        name_list.push_back( (*it).name );
        desc_list.push_back( (*it).description );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Commands
//
///////////////////////////////////////////////////////////////////////////////
template <class Owner>
bool ObjectBase<Owner>::addCommand( const std::string & name,
                                    CommandCallback command_callback,
                                    const Prototype & prototype,
                                    const std::string & help_string )
{
    bool status;
    Command command;

    status = false;

    command.name = name;
    command.command_callback = command_callback;
    command.prototype = prototype;
    command.help_string = help_string;

    if( addCommand( command ) )
        status = true;

    return status;
}

template <class Owner>
bool ObjectBase<Owner>::addCommand( const ObjectBase<Owner>::Command & command )
{
    bool status;

    status = false;

    if( !hasCommand( command ) )
    {
        command_list.push_back( command );
        status = true;
    }

    return status;
}

template <class Owner>
bool ObjectBase<Owner>::hasCommand( const ObjectBase<Owner>::Command & command )
{
    CommandConstIterator it;

    it = std::find( command_list.begin( ), command_list.end( ), command );

    return (it != command_list.end( ));
}

template <class Owner>
bool ObjectBase<Owner>::hasCommand( const std::string & name )
{
    CommandConstIterator it;

    it = std::find( command_list.begin( ), command_list.end( ), name );

    return (it != command_list.end( ));
}

template <class Owner>
void ObjectBase<Owner>::listCommands( NameList & name_list )
{
    CommandConstIterator it;
    Name previous;

    for( it = command_list.begin( ); it != command_list.end( ); it++ )
    {
        // FIXME - this only works if the command_list is sorted
        if( (*it).name != previous )
        {
            name_list.push_back( (*it).name );
            previous = (*it).name;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Functions
//
///////////////////////////////////////////////////////////////////////////////
template <class Owner>
bool ObjectBase<Owner>::addFunction( const std::string & name,
                                     FunctionCallback function_callback )
{
    bool status;
    Function function;

    status = false;

    function.name = name;
    function.function_callback = function_callback;

    if( addFunction( function ) )
        status = true;

    return status;
}

template <class Owner>
bool ObjectBase<Owner>::addFunction(
                               const ObjectBase<Owner>::Function & function )
{
    bool status;

    status = false;

    if( !hasFunction( function.name ) )
    {
        function_list.push_back( function );
        status = true;
    }

    return status;
}

template <class Owner>
bool ObjectBase<Owner>::hasFunction( const std::string & name )
{
    FunctionConstIterator it;

    it = std::find( function_list.begin( ), function_list.end( ), name );

    return (it != function_list.end( ));
}

#endif /* OBJECT_BASE_H */
