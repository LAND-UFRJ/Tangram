#ifndef WD_H
#define WD_H

#include <algorithm>
#include <list>

#include <stdlib.h>

#include "types.h"

template<class T>
class DeleteCleaner
{
public:
    void operator () ( T * ptr )
    {
        if( ptr )
        {
            DEBUG(LEVEL_INFO, "Deleting\n");
            delete ptr;
        }
    }
};

template<class T>
class FreeCleaner
{
public:
    void operator () ( T * ptr )
    {
        if( ptr )
        {
            DEBUG(LEVEL_INFO, "Freeing\n");
            free( ptr );
        }
    }
};

template<class T, class Cleaner = FreeCleaner<T> >
class WatchDog
{
public:
    WatchDog( void ) { }

    WatchDog( const Cleaner & c ) : cleaner(c) { }

    ~WatchDog( void )
    {
        typename std::list<VectorWatch>::iterator it;
        int i;

        DEBUG(LEVEL_INFO, "Deleting Watchdog\n");

        for_each( watches.begin( ), watches.end( ), cleaner );

        for( it = vector_watches.begin( ); it != vector_watches.end( ); it++ )
        {
            for( i = 0; i < (*it).size; i++ )
                cleaner( (*it).watch[i] );
        }
    }

    void addWatch( T * ptr )
    {
        if( ptr )
        {
            DEBUG(LEVEL_INFO, "Adding Watch\n");
            watches.push_back( ptr );
        }
    }

    void addWatch( T ** ptr, int N )
    {
        VectorWatch vector_watch;

        if( ptr )
        {
            DEBUG(LEVEL_INFO, "Adding Vector Watch - %d\n", N);

            vector_watch.watch = ptr;
            vector_watch.size  = N;

            vector_watches.push_back( vector_watch );
        }
    }

    void removeAllWatches( void )
    {
        watches.clear( );
        vector_watches.clear( );
    }

private:
    typedef struct
    {
        T ** watch;
        int size;
    } VectorWatch;

    Cleaner cleaner;
    std::list<T *> watches;
    std::list<VectorWatch> vector_watches;
};

#endif /* WD_H */
