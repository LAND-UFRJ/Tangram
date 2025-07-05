#include "object.h"

#include "plugin.h"

Object::Object( const Name & name ) : object_name(name)
{
}

Object::~Object( void )
{
}

const char * Object::getName( void ) const
{
    return object_name.c_str( );
}

const Plugin * Object::getPlugin( void ) const
{
    return plugin;
}
