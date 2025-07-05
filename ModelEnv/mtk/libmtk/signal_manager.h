#ifndef SIGNAL_MANAGER_H
#define SIGNAL_MANAGER_H

namespace SignalManager
{
    void ignoreInterrupt( void );
    void handleInterrupt( void );

    bool isInterrupted( void );

    void registerFlag( int * );
};

#endif /* SIGNAL_MANAGER_H */
