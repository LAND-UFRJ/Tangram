#ifndef CORE_INTERFACE_H
#define CORE_INTERFACE_H

#include "help_messages.h"
#include "types.h"
#include "token.h"
#include "terminal.h"
#include "plugin_manager.h"
#include "object_manager.h"

namespace CoreInterface
{
    void init( void );

    // object creation and destruction
    bool create ( const Name &, const Name &, Arguments & );
    bool destroy( const Name & );

    // displays and functions
    bool display( const Name &, const Name &               );
    bool exec(    const Name &, const Name &, Arguments  & );

    // help
    bool help( const Name &, const Name & );
    bool help( const TokenType );

    // set/show for objects
    bool object_set ( const Name &, const Name &,
                      const std::string &, const Index & );
    bool object_show( const Name &, const Name &,
                      const Index & );

    // object copying
    bool copy( const Name &, const Name & );

    // clear all objects
    bool clear( void );

    // determine the plugin from an object
    bool which( const Name & );

    // listing plugins and objects from their managers
    bool listPlugins( void );
    bool listObjects( void );
};    

#endif /* CORE_INTERFACE_H */
