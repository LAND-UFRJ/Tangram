#include "signal_manager.h"

#include <signal.h>

#include "terminal.h"

namespace SignalManager
{
    bool interrupted = false;
    int * pflag = 0;

    void interrupt_handler( int signo )
    {
        Terminal::displayMessage( "Interrupting command..." );
        interrupted = true;

        if( pflag )
        {
            *pflag = 1;
            pflag = 0;
        }
    }

    void ignoreInterrupt( void )
    {
        signal( SIGINT, SIG_IGN );
        pflag = 0;
    }

    void handleInterrupt( void )
    {
        pflag = 0;
        signal( SIGINT, interrupt_handler );
    }

    bool isInterrupted( void )
    {
        return interrupted;
    }

    void registerFlag( int * ptr )
    {
        pflag = ptr;
        *pflag = 0;
    }
};
