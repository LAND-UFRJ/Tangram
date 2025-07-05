#ifndef CORE_CONTROLLER_H
#define CORE_CONTROLLER_H

#include <algorithm>
#include <string>
#include <list>

#include <stdlib.h>
#include <time.h>

#include "types.h"
#include "terminal.h"
#include "object.h"
#include "plugin_manager.h"
#include "object_manager.h"

namespace CoreController
{
    // data types
    typedef bool (*SetCallback)( const std::string & );
    typedef bool (*GetCallback)( std::string & );

    typedef struct
    {
        Name name;
        SetCallback set_callback;
        GetCallback get_callback;

        bool operator == ( const Name & str ) const
        {
            return (name == str);
        }
    } CoreOption;

    typedef std::list<CoreOption> CoreOptionList;

    extern CoreOptionList option_list;

    // function declarations
    bool addCoreOption( const Name &, SetCallback, GetCallback );
    bool hasCoreOption( const Name & );

    // object access
    Object * getObjectByName( const Name & );

    bool set_active( const std::string & );
    bool get_active( std::string & );

    bool set_terminal( const std::string & );
    bool get_terminal( std::string & );

    bool set_output( const std::string & );
    bool get_output( std::string & );

    bool set_pager( const std::string & );
    bool get_pager( std::string & );

    bool get_version( std::string & );

    // set/get/show for core options
    bool set ( const Name &, const std::string & );
    bool get ( const Name &, std::string & ); // not-a-command
    bool show( const Name & );

    // Core controller initial setup and final cleanup
    void setupCore( std::list<std::string>, bool = false, long = 0 );
    void cleanCore( void );
};

#endif /* CORE_CONTROLLER_H */
