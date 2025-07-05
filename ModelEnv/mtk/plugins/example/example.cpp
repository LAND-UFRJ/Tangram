#include "example.h"

#include <stdio.h>

#include "object_io.h"
#include "watchdog.h"

void initExample( void ) __attribute__ ((constructor));

Object::Info object_info =
{
    "example",
    "Example plugin",
    EXAMPLE_HELP_MESSAGE,
    1, 0,
    ObjectBase<Example>::help,
    ObjectBase<Example>::create
};

////////////////////////////////////////
// Plugin programmer code starts here //
////////////////////////////////////////

/**
 * Plugin attribute get/set
 */ 
bool Example::set_a( const std::string &value, const Index &idx )
{
    a = strtol( value.c_str( ), 0, 0 );

    return true;
}

bool Example::get_a( std::string &value, const Index &idx )
{
    char buffer[32];
    
    sprintf( buffer, "%d", a );
    value = buffer;

    return true;
}

bool Example::set_b( const std::string &value, const Index &idx )
{
    b = strtol( value.c_str( ), 0, 0 );

    return true;
}

bool Example::get_b( std::string &value, const Index &idx )
{
    char buffer[32];
    
    sprintf( buffer, "%d", b );
    value = buffer;

    return true;
}

bool Example::set_result( const std::string &value, const Index &idx )
{
    result = strtol( value.c_str( ), 0, 0 );

    return true;
}

bool Example::get_result( std::string &value, const Index &idx )
{
    char buffer[32];
    
    sprintf( buffer, "%d", result );
    value = buffer;

    return true;
}

/**
 * Plugin displays
 */ 
bool Example::display_a( Terminal &term )
{
    term.print( "a = %d\n", a );

    return true;
}

bool Example::display_b( Terminal &term )
{
    term.print( "b = %d\n", b );

    return true;
}

bool Example::display_result( Terminal &term )
{
    term.print( "result = %d\n", result );

    return true;
}

/**
 * Plugin commands
 */ 
bool Example::add( Arguments &args )
{
    int op1, op2;
    
    switch( args.size() )
    {
        case 0:
            result = a + b;
        break;

        case 2:
            op1 = strtol( args[0].text.c_str( ), 0, 0 );
            op2 = strtol( args[1].text.c_str( ), 0, 0 );
            result = op1 + op2;
        break;
        
        default:
            Terminal::displayErrorMessage( "Invalid number of parameters" );
            return false;
    }

    return true;
}

/**
 * Plugin onstructors
 */ 
Example::Example( const Name & name, const Example & example )
        : ObjectBase<Example>( name, this )
{
    a      = example.a;
    b      = example.b;
    result = example.result;
}

Object *Example::createEmpty( const Name &name, Arguments &args )
{
    Example *example;

    example = new Example( name );

    return example;
}

Object *Example::createWithOperators( const Name &name, Arguments &args )
{
    Example *example;
    Index    idx;

    example = new Example( name );
    
    example->set_a( args[0].text, idx );
    example->set_b( args[1].text, idx );

    return example;
}

/**
 * libmtk communication methods
 */ 
Example::Example( const Name & name )
        : ObjectBase<Example>( name, this )
{
    a      = 0;
    b      = 0;
    result = 0;
}

Example::~Example( void )
{
    DEBUG( LEVEL_INFO, "Destructing Example object (%s)\n",
           object_name.c_str());
}

void initExample( void )
{
    DEBUG( LEVEL_INFO, "Initializing Example plugin\n");

    Example::addConstructor( &Example::createEmpty,
                              Prototype( ) );
    Example::addConstructor( &Example::createWithOperators,
                              Prototype( 2, 2, TK_INTEGER, TK_INTEGER ) );

    Example::addOption( "a", "Operator a",
                         0, &Example::set_a, &Example::get_a );
    Example::addOption( "b", "Operator b",
                         0, &Example::set_b, &Example::get_b );
    Example::addOption( "result", "Operation result",
                         0, &Example::set_result, &Example::get_result );


    Example::addDisplay( "a", "contents of operator a",
                          &Example::display_a );
    Example::addDisplay( "b", "contents of operator b",
                          &Example::display_b );
    Example::addDisplay( "result", "contents of operator result",
                          &Example::display_result );

    Example::addCommand( "add", &Example::add,
                          Prototype( 0, 2, TK_INTEGER, TK_INTEGER ),
                          ADD_HELP_MESSAGE );
}
