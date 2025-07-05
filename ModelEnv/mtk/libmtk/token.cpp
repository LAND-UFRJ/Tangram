#include "token.h"

const Token keyword[] =
{
    { "objects", TK_KW_OBJECTS },
    { "plugins", TK_KW_PLUGINS },

    { "new",     TK_KW_NEW     },
    { "delete",  TK_KW_DELETE  },

    { "destroy", TK_KW_DESTROY },
    { "display", TK_KW_DISPLAY },

    { "set",     TK_KW_SET     },
    { "show",    TK_KW_SHOW    },
    { "list",    TK_KW_LIST    },
    { "help",    TK_KW_HELP    },
    { "clear",   TK_KW_CLEAR   },
    { "which",   TK_KW_WHICH   },
    { "history", TK_KW_HISTORY },
    { "source",  TK_KW_SOURCE  },
    { "quit",    TK_KW_QUIT    },
};

const int NUMBER_OF_KEYWORDS = sizeof(keyword)/sizeof(Token);

const char * token_description[] =
{
    "nothing",

    "the end of input",

    "a dot",
    "a comma",
    "a semicolon",
    "an equal sign",

    "left parentheses",
    "right parentheses",
    "left bracket",
    "right bracket",

    "an integer number",
    "a real number",
    "a quoted string",
    "an identifier",

    "the 'objects' keyword",
    "the 'plugins' keyword",

    "the 'new' keyword",
    "the 'delete' keyword",

    "the 'destroy' keyword",
    "the 'display' keyword",

    "the 'set' keyword",
    "the 'show' keyword",
    "the 'list' keyword",
    "the 'help' keyword",
    "the 'clear' keyword",
    "the 'which' keyword",
    "the 'history' keyword",
    "the 'source' keyword",
    "the 'quit' keyword",
};

const char * token_short_description[] =
{
    "nothing",

    "end",

    ".",
    ",",
    ";",
    "=",

    "(",
    ")",
    "[",
    "]",

    "<integer>",
    "<double>",
    "<string>",
    "<identifier>",

    "objects",
    "plugins",

    "new",
    "delete",

    "destroy",
    "display",

    "set",
    "show",
    "list",
    "help",
    "clear",
    "which",
    "history",
    "source",
    "quit",
};

const char * tokenToString( const Token & token )
{
    if( token.type == TK_IDENTIFIER ||
        token.type == TK_STRING ||
        token.type == TK_INTEGER ||
        token.type == TK_REAL )
    {
        static std::string identifier_desc;

        identifier_desc = token_description[token.type];
        identifier_desc += " ('" + token.text + "')";

        return identifier_desc.c_str( );
    }
    else
        return token_description[token.type];
}

const char * tokenToString( TokenType token_type )
{
    return token_description[token_type];
}

const char * tokenShortDesc( TokenType token_type )
{
    return token_short_description[token_type];
}
