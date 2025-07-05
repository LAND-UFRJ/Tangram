#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <algorithm>
#include <list>
#include <string>

#include <sys/types.h>

#include "types.h"
#include "plugin.h"

class PluginManager
{
    public:
        typedef std::list<Plugin *> PluginList;
        
        // Public data members
        static const char *PLUGIN_FILE_EXTENSION;

        // Public functions
        bool addPath     ( const Name & );
        bool removePath  ( const Name & );

        static bool           createPluginManager ( void ); // Instance creation
        static bool           destroyPluginManager( void ); // Instance release
        static PluginManager *getPluginManager    ( void ); // Instance retrieval

        bool initPlugins( void );

        void listPlugins( NameList & );
        void listPlugins( NameList &, NameList & );

        Plugin     *getPlugin( const Name & );
        PluginList  getPluginList( void );

    private:
        // Private data types
        class Path
        {
            public:
                // Public functions
                Path( const Name & );
                ~Path( void );

                int scanForPlugins( NameList & );

                bool operator == ( const Name & ) const;

                static int plugin_filter ( const struct dirent * );

                const Name path_name;

            private:
                // Private data members
                ino_t       path_inode;
        };

        typedef std::list<Path> PathList;

        // Private data members
        static PluginManager *the_plugin_manager; // pointer to Singleton

        PathList   path_list;    // Current path list
        PluginList plugin_list;  // Current plugin list

        // Private functions
        PluginManager ( void );              // Private constructor
        ~PluginManager( void );              // Private destructor
};

#endif /* PLUGIN_MANAGER_H */
