#ifndef PROTOTYPE_H
#define PROTOTYPE_H

#include <vector>
#include <stdarg.h>

#include "token.h"

#define UNLIMITED 0xFFFFFFFF

class Prototype
{
public:
    Prototype( void );
    Prototype( unsigned int, unsigned int, ... );

    bool match( Arguments & ) const;

    const char * toString( void ) const;

    bool operator == ( const Prototype & p ) const;

private:
    unsigned int min, max;
    std::vector<TokenType> types;
};

#endif /* PROTOTYPE_H */
