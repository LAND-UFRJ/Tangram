#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <list>
#include <vector>

typedef enum
{
    TK_NONE = 0,

    TK_END,

    TK_DOT,
    TK_COMMA,
    TK_SEMICOLON,
    TK_EQUAL,

    TK_LPAR,
    TK_RPAR,
    TK_LBRAC,
    TK_RBRAC,

    TK_INTEGER,
    TK_REAL,
    TK_STRING,
    TK_IDENTIFIER,

    TK_KW_OBJECTS,
    TK_KW_PLUGINS,

    TK_KW_NEW,
    TK_KW_DELETE,

    TK_KW_DESTROY,
    TK_KW_DISPLAY,

    TK_KW_SET,
    TK_KW_SHOW,
    TK_KW_LIST,
    TK_KW_HELP,
    TK_KW_CLEAR,
    TK_KW_WHICH,
    TK_KW_HISTORY,
    TK_KW_SOURCE,
    TK_KW_QUIT,
} TokenType;

typedef struct
{
    std::string text;
    TokenType   type;
} Token;

typedef std::list<Token> TokenList;
typedef std::vector<Token> Arguments;

extern const Token keyword[];
extern const int NUMBER_OF_KEYWORDS;
extern const char * token_description[];

const char * tokenToString( const Token & );
const char * tokenToString( TokenType );
const char * tokenShortDesc( TokenType );

#endif /* TOKEN_H */
