#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <stdio.h>

#include "types.h"
#include "token.h"
#include "prototype.h"

class Plugin;

typedef std::list<unsigned int> Index;

class Object
{
    public:
        virtual bool set( const Name &, const std::string &,
                          const Index & ) = 0;
        virtual bool get( const Name &, std::string &,
                          const Index & ) = 0;

        virtual Object * copy( const Name & ) = 0;

        virtual bool display( const Name & )               = 0;
        virtual bool exec(    const Name &, Arguments &  ) = 0;

        virtual void * run( const Name &, void * ) = 0;

        const char * getName( void ) const;
        const Plugin * getPlugin( void ) const;

        virtual bool checkOption  ( const Name & ) = 0;
        virtual bool checkDisplay ( const Name & ) = 0;
        virtual bool checkCommand ( const Name & ) = 0;
        virtual bool checkFunction( const Name & ) = 0;

        /* Info structure - each plugin must declare a global variable
         *                  of this type with specific values set
         */                       
        typedef struct
        {
            const char * id_string;
            const char * info_string;
            const char * help_string;
            const int version_major,
                      version_minor;
            bool (*help_function)( const Name & );
            Object * (*create)( const Name &, Arguments & );
        } Info;

    protected:
        friend class ObjectManager;

        const Name object_name;
        const Plugin * plugin;

        Object( const Name & );
        virtual ~Object( void );
};

#endif /* OBJECT_H */
