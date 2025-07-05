#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include "types.h"
#include "object.h"
#include "plugin.h"

class ObjectManager
{
public:
    // public functions
    static bool            createObjectManager ( void ); // instance creation
    static bool            destroyObjectManager( void ); // instance destruction
    static ObjectManager * getObjectManager    ( void ); // instance retrieval

    bool createObject ( Plugin *, const Name &, Arguments & );
    bool destroyObject( const Name & );

    bool     setActiveObject( const Name & );
    Object * getActiveObject( void );

    const char * getActiveObjectName( void );

    Object * getObjectByName( const Name & );

    void listObjects( NameList & );
    void listObjects( NameList &, NameList & );

    bool copy( const Name &, const Name & );
    bool copyFromTangram( const Name &, const Name & );    

    void clear( void );

    bool isNameUsed( const Name & );

private:
    // private data types
    typedef std::list<Object *> ObjectList;
    typedef ObjectList::iterator iterator;

    // private data members
    static ObjectManager * the_object_manager;    // pointer to Singleton

    ObjectList  object_list;               // current model list
    Object    * active_object;             // currently active object

    // private functions
    ObjectManager ( void );                // private constructor
    ~ObjectManager( void );                // private destructor

    const char * getUnusedName( const Name & );
};

#endif /* OBJECT_MANAGER_H */
