#include "command.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>

#include "terminal.h"
#include "action.h"

#define MATCH_TOKEN(token_type) \
    { \
        if(!matchToken(token_type)) \
        { \
            return 0; \
        } \
    }

#define MATCH_SET(n,...) \
    { \
        if(!matchSet( n, __VA_ARGS__ )) \
        { \
            return 0; \
        } \
    }

bool Command::readFromString( const char * command_line,
                              CommandList & command_list )
{
    Command command;
    Token token;
    const char * begin, * end;
    char * ptr;
    bool status;
    int i;
    long val;

    // return error on null string
    if( !command_line )
        return false;

    command_list.clear( );

    begin = command_line;
    end   = begin;

    // assume no error initially
    status = true;

    while( true )
    {
        while( isspace( *begin ) )
            begin++;

        token.type = TK_NONE;

        if( *begin == '\0' || *begin == '#' || *begin == '\n' )
        {
            // found end of input
            token.text = '\0';
            token.type = TK_END;
        }
        else if( *begin == '.' && !isdigit(*(begin+1)) )
        {
            // found dot (not number)
            end = begin+1;
            token.text = '.';
            token.type = TK_DOT;
        }
        else if( *begin == ',' )
        {
            // found comma
            end = begin+1;
            token.text = ',';
            token.type = TK_COMMA;
        }
        else if( *begin == ';' )
        {
            // found semicolon
            end = begin+1;
            token.text = ';';
            token.type = TK_SEMICOLON;
        }
        else if( *begin == '=' )
        {
            // found equal
            end = begin+1;
            token.text = '=';
            token.type = TK_EQUAL;
        }
        else if( *begin == '(' )
        {
            // found equal
            end = begin+1;
            token.text = '(';
            token.type = TK_LPAR;
        }
        else if( *begin == ')' )
        {
            // found equal
            end = begin+1;
            token.text = ')';
            token.type = TK_RPAR;
        }
        else if( *begin == '[' )
        {
            // found equal
            end = begin+1;
            token.text = '[';
            token.type = TK_LBRAC;
        }
        else if( *begin == ']' )
        {
            // found equal
            end = begin+1;
            token.text = ']';
            token.type = TK_RBRAC;
        }
        else if( isdigit(*begin) ||
                 ( *begin == '.' && isdigit(*(begin+1)) ) ||
                 *begin == '+' || *begin == '-' )
        {
            // found a number (integer or real)
            end = begin;

            while( !isspace(*end) &&
                   *end != '\0' &&
                   *end != '\n' &&
                   *end != '#' &&
                   *end != ';' &&
                   *end != ',' &&
                   *end != '=' &&
                   *end != '(' &&
                   *end != ')' &&
                   *end != '[' &&
                   *end != ']' )
                end++;

            errno = 0;
            val = strtol( begin, &ptr, 0 );
            if( (errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0) )
                perror( "strtol" );

            if( ptr != end )
            {
                if( strtod( begin, &ptr ) == 0 )
                    perror( "strtod" );
    
                if( ptr != end )
                {
                    // signal error reading real number
                    status = false;
                    break;
                }

                token.text.assign( begin, end-begin );
                token.type = TK_REAL;
            }
            else
            {
                token.text.assign( begin, end-begin );
                token.type = TK_INTEGER;
            }
        }
        else if( *begin == '"' || *begin == '\'' )
        {
            // found a quoted string
            end = begin+1;

            while( *end != *begin &&
                   *end != '\0' )
                end++;

            if( *end != *begin )
            {
                // signal error reading quoted string
                status = false;
                break;
            }

            begin++;
            token.text.assign( begin, end-begin );
            token.type = TK_STRING;
            end++;
        }
        else if( isalpha(*begin) )
        {
            // found either a keyword or an identifier
            end = begin;

            while( isalpha(*end) || isdigit(*end) || *end == '_' )
                end++;

            token.text.assign( begin, end-begin );

            // check if token is a keyword
            for( i = 0; i < NUMBER_OF_KEYWORDS; i++ )
                if( token.text == keyword[i].text )
                {
                    token.type = keyword[i].type;
                    break;
                }

            // if it's not a keyword, the token must be an identifier
            if( token.type == TK_NONE )
                token.type = TK_IDENTIFIER;
        }

        if( token.type == TK_NONE )
        {
            status = false;
            break;
        }
        else if( token.type == TK_END )
        {
            // insert final command into the list
            if( command.token_list.size( ) > 0 )
            {
                command.token_list.push_back( token );
                command_list.push_back( command );
                command.token_list.clear( );
            }
            break;
        }
        else if( token.type == TK_SEMICOLON )
        {
            // insert another command into the list
            if( command.token_list.size( ) > 0 )
            {
                token.text = '\0';
                token.type = TK_END;
                command.token_list.push_back( token );
                command_list.push_back( command );
                command.token_list.clear( );
            }
        }
        else
        {
            command.token_list.push_back( token );
        }

        begin = end;
    }

    return status;
}

Action * Command::parse( void )
{
    Action * action;

    error_string = "invalid command syntax";

    // try to parse a command to the core controller (set, get, list, quit)
    action = parseCoreCommand( );

    if( !action ) // try to parse a create command
        action = parseCreateCommand( );

    if( !action ) // try to parse an object command
        action = parseObjectCommand( );

    if( !action )
        Terminal::displayErrorMessage( error_string.c_str( ) );

    return action;
}

Action * Command::parseCoreCommand( void )
{
    std::string name, value;
    Name plugin_name, command_name;
    TokenType type;
    unsigned int size;
    Action * action;

    action = 0;
    reset( );
    
    switch( current_token.type )
    {
    case TK_KW_SET:
        MATCH_TOKEN( TK_KW_SET );

        name = current_token.text;
        MATCH_TOKEN( TK_IDENTIFIER );

        value = current_token.text;
        MATCH_SET( 4, TK_IDENTIFIER, TK_STRING, TK_INTEGER, TK_REAL );

        MATCH_TOKEN( TK_END );

        action = new CoreSetAction( name, value );
        break;
    case TK_KW_SHOW:
        MATCH_TOKEN( TK_KW_SHOW );

        name = current_token.text;
        MATCH_TOKEN( TK_IDENTIFIER );

        MATCH_TOKEN( TK_END );

        action = new CoreShowAction( name );
        break;
    case TK_KW_LIST:
        MATCH_TOKEN( TK_KW_LIST );

        type = current_token.type;
        MATCH_SET( 2, TK_KW_PLUGINS, TK_KW_OBJECTS );

        MATCH_TOKEN( TK_END );

        if( type == TK_KW_PLUGINS )
            action = new ListAction( ListAction::LIST_PLUGINS );
        else if( type == TK_KW_OBJECTS )
            action = new ListAction( ListAction::LIST_OBJECTS );
        break;
    case TK_KW_HELP:
        MATCH_TOKEN( TK_KW_HELP );

        type = current_token.type;
        if( type == TK_IDENTIFIER )
            plugin_name = current_token.text;

        MATCH_SET( 6, TK_KW_SET, TK_KW_SHOW, TK_KW_LIST, TK_KW_HELP,
                      TK_KW_QUIT, TK_IDENTIFIER );

        if( type == TK_IDENTIFIER && current_token.type == TK_DOT )
        {           
            MATCH_TOKEN( TK_DOT );

            command_name = current_token.text;
            MATCH_TOKEN( TK_IDENTIFIER );
        }

        MATCH_TOKEN( TK_END );

        if( type == TK_IDENTIFIER )
        {
            action = new HelpAction( plugin_name, command_name );
        }
        else
        {
            action = new HelpAction( type );
        }
        break;
    case TK_KW_CLEAR:
        MATCH_TOKEN( TK_KW_CLEAR );

        MATCH_TOKEN( TK_END );

        action = new ClearAction( );
        break;
    case TK_KW_WHICH:
        MATCH_TOKEN( TK_KW_WHICH );

        name = current_token.text;
        MATCH_TOKEN( TK_IDENTIFIER );

        MATCH_TOKEN( TK_END );

        action = new WhichAction( name );
        break;
    case TK_KW_HISTORY:
        MATCH_TOKEN( TK_KW_HISTORY );

        size = atoi( current_token.text.c_str( ) );
        MATCH_TOKEN( TK_INTEGER );

        MATCH_TOKEN( TK_END );

        action = new HistoryAction( size );
        break;
    case TK_KW_SOURCE:
        MATCH_TOKEN( TK_KW_SOURCE );

        name = current_token.text;
        MATCH_TOKEN( TK_STRING );

        MATCH_TOKEN( TK_END );

        action = new SourceAction( name );
        break;
    case TK_KW_QUIT:
        MATCH_TOKEN( TK_KW_QUIT );

        MATCH_TOKEN( TK_END );

        action = new QuitAction( );
        break;
    default:
        break;
    }

    return action;
}

Action * Command::parseCreateCommand( void )
{
    Action * action;
    TokenList args;
    Token next[2];
    std::string object_name,
                plugin_name,
                copy_name;

    action = 0;
    reset( );

    // annonymous command creation
    if( current_token.type == TK_KW_NEW )
    {
        MATCH_TOKEN( TK_KW_NEW );
        plugin_name = current_token.text;
        MATCH_TOKEN( TK_IDENTIFIER );
        MATCH_TOKEN( TK_LPAR );

        if( current_token.type != TK_RPAR )
        {
            args.push_back( current_token );
            MATCH_SET(4, TK_IDENTIFIER, TK_STRING, TK_INTEGER, TK_REAL);

            while( current_token.type != TK_RPAR )
            {
                MATCH_TOKEN( TK_COMMA );
                args.push_back( current_token );
                MATCH_SET(4, TK_IDENTIFIER, TK_STRING, TK_INTEGER, TK_REAL);
            }
        }

        MATCH_TOKEN( TK_RPAR );

        MATCH_TOKEN( TK_END );

        action = new CreateAction( plugin_name, "", args.begin(), args.end() );
    }
    else if( current_token.type == TK_IDENTIFIER )
    {
        lookahead( next[0], 1 );
        lookahead( next[1], 2 );

        if( current_token.type == TK_IDENTIFIER &&
            next[0].type       == TK_EQUAL &&
            (next[1].type == TK_KW_NEW ||
             next[1].type == TK_IDENTIFIER) )
        {
            object_name = current_token.text;
            MATCH_TOKEN( TK_IDENTIFIER );
            MATCH_TOKEN( TK_EQUAL );

            if( current_token.type == TK_KW_NEW )
            {
                MATCH_TOKEN( TK_KW_NEW );

                plugin_name = current_token.text;
                MATCH_TOKEN( TK_IDENTIFIER );

                MATCH_TOKEN( TK_LPAR );

                if( current_token.type != TK_RPAR )
                {
                    args.push_back( current_token );
                    MATCH_SET(4, TK_IDENTIFIER, TK_STRING, TK_INTEGER, TK_REAL);

                    while( current_token.type != TK_RPAR )
                    {
                        MATCH_TOKEN( TK_COMMA );
                        args.push_back( current_token );
                        MATCH_SET(4, TK_IDENTIFIER, TK_STRING, TK_INTEGER,
                                  TK_REAL);
                    }
                }

                MATCH_TOKEN( TK_RPAR );

                MATCH_TOKEN( TK_END );

                action = new CreateAction( plugin_name, object_name,
                                           args.begin(), args.end() );
            }
            else if( current_token.type == TK_IDENTIFIER ) // object copy
            {
                copy_name = current_token.text;
                MATCH_TOKEN( TK_IDENTIFIER );
                MATCH_TOKEN( TK_END );

                action = new CopyAction( object_name, copy_name );
            }
        }
    }
    else if( current_token.type == TK_KW_DELETE )
    {
        MATCH_TOKEN( TK_KW_DELETE );

        if( current_token.type == TK_IDENTIFIER )
        {
            object_name = current_token.text;
            MATCH_TOKEN( TK_IDENTIFIER );
        }

        MATCH_TOKEN( TK_END );

        action = new DestroyAction( object_name );
    }

    return action;
}

Action * Command::parseObjectCommand( void )
{
    Action * action;
    std::string object_name,
                file_name,
                display_name,
                member_name,
                value;
    TokenList args;
    Token next;
    int n;
    Index index;

    action = 0;
    reset( );

    lookahead( next, 1 );
    if( current_token.type == TK_IDENTIFIER && next.type == TK_DOT )
    {
        object_name = current_token.text;
        MATCH_TOKEN( TK_IDENTIFIER );

        MATCH_TOKEN( TK_DOT );
    }

    switch( current_token.type )
    {
    case TK_KW_DESTROY:
        MATCH_TOKEN( TK_KW_DESTROY );

        MATCH_TOKEN( TK_LPAR );
        MATCH_TOKEN( TK_RPAR );

        MATCH_TOKEN( TK_END );

        action = new DestroyAction( object_name );
        break;
    case TK_KW_DISPLAY:
        MATCH_TOKEN( TK_KW_DISPLAY );

        MATCH_TOKEN( TK_LPAR );

        display_name = current_token.text;
        MATCH_TOKEN( TK_IDENTIFIER );

        MATCH_TOKEN( TK_RPAR );

        MATCH_TOKEN( TK_END );

        action = new DisplayAction( object_name, display_name );
        break;
    case TK_IDENTIFIER:
        member_name = current_token.text;
        MATCH_TOKEN( TK_IDENTIFIER );

        switch( current_token.type )
        {
        case TK_LPAR:  // function call
            MATCH_TOKEN( TK_LPAR );

            if( current_token.type != TK_RPAR )
            {
                args.push_back( current_token );
                MATCH_SET(4, TK_IDENTIFIER, TK_STRING, TK_INTEGER, TK_REAL);

                while( current_token.type != TK_RPAR )
                {
                    MATCH_TOKEN( TK_COMMA );
                    args.push_back( current_token );
                    MATCH_SET(4, TK_IDENTIFIER, TK_STRING, TK_INTEGER, TK_REAL);
                }
            }

            MATCH_TOKEN( TK_RPAR );

            MATCH_TOKEN( TK_END );

            action = new ExecAction( object_name, member_name,
                                     args.begin( ), args.end( ) );
            break;

        case TK_END:   // unindex parameter show
        case TK_LBRAC: // indexed parameter
            while( current_token.type == TK_LBRAC )
            {
                MATCH_TOKEN( TK_LBRAC );

                n = strtol( current_token.text.c_str( ), 0, 0 );
                MATCH_TOKEN( TK_INTEGER );
                index.push_back( n );

                MATCH_TOKEN( TK_RBRAC );
            }

            if( current_token.type != TK_EQUAL )
            {
                MATCH_TOKEN( TK_END );

                action = new ObjectShowAction( object_name, member_name,
                                               index );
                break;
            }
            // else, fall-through

        case TK_EQUAL: // parameter assignment
            MATCH_TOKEN(TK_EQUAL);

            value = current_token.text;
            MATCH_SET( 3, TK_STRING, TK_INTEGER, TK_REAL );

            MATCH_TOKEN( TK_END );

            action = new ObjectSetAction( object_name, member_name, value,
                                          index );
            break;

        default:
            break;            
        }

        break;
    default:
        break;
    }

    return action;
}

void Command::reset( void )
{
    next_token = token_list.begin( );
    getNextToken( );
}

void Command::getNextToken( void )
{
    if( next_token == token_list.end( ) )
    {
        current_token.text = "";
        current_token.type = TK_NONE;
    }
    else
    {
        current_token = *next_token;
        next_token++;
    }
}

void Command::lookahead( Token & token, int count )
{
    TokenList::iterator lookahead_token;

    for( lookahead_token = next_token; count > 1; count-- )
         lookahead_token++;

    if( lookahead_token == token_list.end( ) )
    {
        token.type = TK_NONE;
        token.text = "";
    }
    else
        token = *lookahead_token;
}

/* Verifies if token read matches expected token */
bool Command::matchToken( TokenType type )
{
    bool status;

    if( current_token.type == type )
    {     
        getNextToken( );
        status = true;
    }
    else
    {
        error_string = "found ";
        error_string += tokenToString( current_token );
        error_string += " while expecting ";
        error_string += tokenToString( type );
        status = false;
    }

    return status;
}

/* Verifies if token read matches one in the expected token set */
bool Command::matchSet( int N, ... )
{
    bool status;
    int i;
    TokenType type;
    va_list ap;
    std::string tokenset;

    status = false;

    va_start( ap, N );
    for( i = 0; i < N; i++ )
    {
        type = (TokenType)va_arg( ap, int );

        if( type == current_token.type ) /* found match */
        {
            getNextToken( );
            status = true;
            tokenset.clear( );
            break;
        }

        if( N > 2 && i > 0 && i < N-1 )
            tokenset += ", ";
        else if( N > 1 && i == N-1 )
            tokenset += " or ";

        tokenset += tokenToString( type );
    }
    va_end( ap );

    if( i == N )
    {
        error_string = "found ";
        error_string += tokenToString( current_token );
        error_string += " while expecting ";
        error_string += tokenset;
        status = false;
    }

    return status;
}
