#ifndef OBJECT_IO_H
#define OBJECT_IO_H

#include "terminal.h"

#define READ(in,x) \
    if( !(in >> x) ) \
    { \
        Terminal::displayErrorMessage( "could not read from file" ); \
        return false; \
    }

#define WRITE(out,x) \
    if( !(out << x) )\
    { \
        Terminal::displayErrorMessage( "could not write to file" ); \
        return false; \
    }

#define UPPER_LIMIT(x,upper) \
    if( x > upper ) \
    { \
        Terminal::error_stream << "parameter " << #x \
                               << " must be smaller than or equal to " \
                               << upper << std::endl; \
        return false; \
    }
#define LOWER_LIMIT(x,lower) \
    if( x < lower ) \
    { \
        Terminal::error_stream << "parameter " << #x \
                               << " must be greater than or equal to " \
                               << lower << std::endl; \
        return false; \
    }

#endif /* OBJECT_IO_H */
