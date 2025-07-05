#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdarg.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include "types.h"

class Terminal
{
public:
    // public data types
    typedef enum
    {
        TERMINAL_SCREEN,
        TERMINAL_FILE,
    } TerminalType;

    // public functions
    static bool       setTerminal( TerminalType );
    static Terminal * getTerminal( void );

    bool open ( void );
    bool close( void );

    TerminalType getType( void ) const;

    const char * getOutputName( void ) const;
    void setOutputName( const std::string & );

    // I/O operations
    void print( const char *, ... );
    FILE * getFile( void );

    static void displayList     ( const std::string &, NameList & );
    static void displayList     ( const std::string &, NameList &, NameList & );
    static void displayAttribute( const std::string &, const std::string & );

    static void setMute( bool );
    static bool isMute( void );

    static void setPager( const Name & );
    static const char * getPager( void );

    static void displayMessage       ( const char *, ... );
    static void displayErrorMessage  ( const char *, ... );
    static void displayWarningMessage( const char *, ... );

    static void setContext( const char *, const int * );

    // references for iostream devices
    static std::ostream & output_stream;
    static std::ostream & error_stream;

private:
    // private data members
    static Terminal * current_terminal;    // pointer to Singleton
    Name file_name;
    TerminalType type;
    FILE * file;

    static Name pager_name;
    static bool mute;
    static const char * pfile_name;
    static const int * pline_number;

    // private functions
    Terminal( void );                      // create standard output terminal
    Terminal( const std::string & );       // create file terminal
    ~Terminal( void );
};

#endif /* TERMINAL_H */
