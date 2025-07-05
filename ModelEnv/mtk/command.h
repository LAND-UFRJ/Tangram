#ifndef COMMAND_H
#define COMMAND_H

#include <list>

#include <stdarg.h>

#include "token.h"

class Action;
class Command;

typedef std::list<Command> CommandList;

class Command
{
public:
    // public functions
    static bool readFromString( const char *, CommandList & );

    Action * parse( void );

private:
    // private data members
    TokenList token_list;
    TokenList::iterator next_token;
    Token current_token;
    std::string error_string;

    // private functions
    void reset       ( void );

    bool selectOne   ( int, ... ); // match one of the tokens passed
    bool matchSequene( int, ... ); // match a sequence of tokens

    void getNextToken( void );
    void lookahead( Token &, int );

    bool matchToken( TokenType );
    bool matchSet  ( int, ... );

    Action * parseCoreCommand      ( void );
    Action * parseCreateCommand    ( void );
    Action * parseObjectCommand    ( void );
};

#endif /* COMMAND_H */
