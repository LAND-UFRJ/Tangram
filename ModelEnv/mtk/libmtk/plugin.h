#ifndef PLUGIN_H
#define PLUGIN_H

#include <string>

#include <dlfcn.h>
#include <string.h>

#include "types.h"
#include "object.h"

class Plugin
{
    public:
        // create a new plugin from a library file and its path
        static Plugin * createPlugin( const Name &, const Name & );

        const char * getName( void ) const;
        const char * getDescription( void ) const;

        bool help( const Name & ) const;

        // create a new object from the plugin interface
        Object * createObject( const Name &, Arguments & ) const;

        // functor to compare two plugins
        class Equal
        {
        public:
            Equal( const Plugin * );
            bool operator () ( const Plugin * );
        private:
            const Plugin * plugin;
        };

        friend class Equal;

    private:
        Plugin ( const Name & ); // private constructor
        ~Plugin( void );                // private destructor

        friend class PluginManager;

        bool load  ( void );     // load shared library file into memory
        bool unload( void );     // unload shared library from memory
        bool init  ( void );     // get important symbols after load()

        void         * lib_handle;   // handle to get symbols from the library
        const Name     lib_filename; // file name to load the library
        Object::Info * lib_info;     // pointer to object info
};

#endif /* PLUGIN_H */
