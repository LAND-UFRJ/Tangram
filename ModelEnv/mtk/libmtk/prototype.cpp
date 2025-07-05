#include "prototype.h"

#include "terminal.h"

Prototype::Prototype( void ) : min(0), max(0)
{
}

Prototype::Prototype( unsigned int m, unsigned int M, ... )
{
    unsigned int i, size;
    va_list ap;

    min = m;
    max = M;

    if( max == UNLIMITED )
        size = min;
    else
        size = max;

    va_start( ap, M );
    for( i = 0; i < size; i++ )
        types.push_back( (TokenType)va_arg( ap, int ) );
    va_end( ap );
}

bool Prototype::match( Arguments & args ) const
{
    unsigned int i;
    TokenType last;

    if( args.size( ) < min || (max != UNLIMITED && args.size( ) > max) )
        return false;

    if( max == UNLIMITED )
    {
        for( i = 0; i < types.size( ); i++ )
        {
            if( args[i].type != types[i] )
            {
                // special case to ignore
                if( types[i] == TK_REAL && args[i].type == TK_INTEGER )
                    continue;

                // return error but don't print a message because
                // arguments may match another prototype
                return false;
            }
        }

        last = types[types.size( )-1];
        for( ; i < args.size( ); i++ )
        {
            if( args[i].type != last )
            {
                // special case to ignore
                if( last == TK_REAL && args[i].type == TK_INTEGER )
                    continue;

                // return error but don't print a message because
                // arguments may match another prototype
                return false;
            }
        }
    }
    else
    {
        for( i = 0; i < args.size( ); i++ )
        {
            if( args[i].type != types[i] )
            {
                // special case to ignore
                if( types[i] == TK_REAL && args[i].type == TK_INTEGER )
                    continue;

                // return error but don't print a message because
                // arguments may match another prototype
                return false;
            }
        }
    }

    return true;
}

const char * Prototype::toString( void ) const
{
    static std::string prototype_string;
    unsigned int i;

    prototype_string = "(";

    if( max == UNLIMITED )
    {
        for( i = 0; i < min-1; i++ )
        {
            if( i > 0 )
                prototype_string += ',';

            prototype_string += " ";
            prototype_string += tokenShortDesc( types[i] );
        }

        if( i > 0 )
            prototype_string += ',';

        prototype_string += " (";
        prototype_string += tokenShortDesc( types[i] );
        prototype_string += ")+";
    }
    else
    {
        for( i = 0; i < min; i++ )
        {
            if( i > 0 )
                prototype_string += ',';

            prototype_string += " ";
            prototype_string += tokenShortDesc( types[i] );
        }

        for( ; i < max; i++ )
        {
            if( i > 0 )
                prototype_string += ',';

            prototype_string += " [";
            prototype_string += tokenShortDesc( types[i] );
            prototype_string += "]";
        }
    }

    prototype_string += " )";

    return prototype_string.c_str( );
}

bool Prototype::operator == ( const Prototype & p ) const
{
    unsigned int i;

    if( min != p.min ) return false;
    if( max != p.max ) return false;

    for( i = 0; i < types.size( ); i++ )
        if( types[i] != p.types[i] )
            return false;

    return true;
}
