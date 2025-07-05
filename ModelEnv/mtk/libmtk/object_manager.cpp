#include "object_manager.h"

#include "terminal.h"

ObjectManager * ObjectManager::the_object_manager = 0;

bool ObjectManager::createObjectManager( void )
{
    if( the_object_manager )
        return false;

    the_object_manager = new ObjectManager( );
    return true;
}

bool ObjectManager::destroyObjectManager( void )
{
    if( !the_object_manager )
        return false;

    delete the_object_manager;
    return true;
}

ObjectManager * ObjectManager::getObjectManager( void )
{
    return the_object_manager;
}

ObjectManager::ObjectManager( void )
{
    object_list.clear( );
    active_object    = 0;
}

ObjectManager::~ObjectManager( void )
{
    iterator it;

    for( it = object_list.begin( ); it != object_list.end( ); it++ )
    {
        delete (*it);
    }

    object_list.clear( );
}

bool ObjectManager::createObject( Plugin * plugin, const Name & name,
                                  Arguments & args )
{
    bool status;
    Object * new_object;
    iterator it;
    Name object_name;

    status = false;

    object_name = name;
    if( object_name == "" )
        object_name = getUnusedName( plugin->getName( ) );

    // look for an object with the same name already in the list
    for( it = object_list.begin( ); it != object_list.end( ); it++ )
    {
        if( object_name == (*it)->getName( ) )
            break;
    }

    // if no such object was found
    if( it == object_list.end( ) )
    {
        // create a new object with that name using the plugin
        new_object = plugin->createObject( object_name, args );

        // if the creation was successful
        if( new_object )
        {
            // set plugin
            new_object->plugin = plugin;

            // add the object the list
            object_list.push_back( new_object );
            status = true;

            // if there is not active object, make this one so
            if( !active_object )
                active_object = new_object;
        }
    }
    else
    {
        Terminal::displayErrorMessage(
              "there is already an instanced object named '%s'",
              object_name.c_str( ) );
    }

    return status;
}

bool ObjectManager::destroyObject( const Name & name )
{
    bool status;
    Object * old_object;
    iterator it;
    Name object_name;

    status = false;

    object_name = name;
    if( object_name == "" )
        object_name = getActiveObjectName( );

    if( object_name != "" )
    {
        // look for an object with that name in the list
        for( it = object_list.begin( ); it != object_list.end( ); it++ )
        {
            if( object_name == (*it)->getName( ) )
                break;
        }

        // if such object was found
        if( it != object_list.end( ) )
        {
            // remove it from the list and delete it
            old_object = (*it);
            object_list.erase( it );
            if( active_object == old_object )
            {
                if( !object_list.empty( ) )
                {
                    active_object = object_list.front( );
                    DEBUG(LEVEL_INFO,"New active object is %s\n",
                          active_object->getName());
                }
                else
                {
                    active_object = 0;
                }
            }
            delete old_object;
            status = true;
        }
        else
        {
            Terminal::displayErrorMessage(
                    "could not find an object named '%s' to destroy",
                    object_name.c_str( ) );
        }
    }
    else
    {
        Terminal::displayErrorMessage( "no objects to destroy" );
    }

    return status;
}

const char * ObjectManager::getUnusedName( const Name & prefix_name )
{
    static char suggested_name[32];
    int  suggested_number;   // FIXME - this is not efficient

    suggested_number = 1;
    sprintf( suggested_name, "%s%d", prefix_name.c_str( ), suggested_number );
    while( isNameUsed( suggested_name ) )
    {
        sprintf( suggested_name, "%s%d", prefix_name.c_str( ),
                 suggested_number );
        suggested_number++;
    }

    return suggested_name;
}

bool ObjectManager::isNameUsed( const Name & name )
{
    iterator it;
    bool status;

    status = false;

    // look for an object with the same name already in the list
    for( it = object_list.begin( ); it != object_list.end( ); it++ )
    {
        if( strcmp( name.c_str( ), (*it)->getName( ) ) == 0 )
        {
            status = true;
            break;
        }
    }

    return status;
}

bool ObjectManager::setActiveObject( const Name & name )
{
    bool status;
    iterator it;

    status = false;

    for( it = object_list.begin( ); it != object_list.end( ); it++ )
    {
        if( strcmp( name.c_str( ), (*it)->getName( ) ) == 0 )
        {
            active_object = (*it);
            status = true;
            break;
        }
    }

    if( it == object_list.end( ) )
    {
        Terminal::displayErrorMessage(
                "could not find an object named '%s' to set as active",
                name.c_str( ) );
    }

    return status;
}

Object * ObjectManager::getActiveObject( void )
{
    if( !active_object )
        Terminal::displayErrorMessage( "there is no currently active object" );

    return active_object;
}

const char * ObjectManager::getActiveObjectName( void )
{
    if( active_object )
        return active_object->getName( );
    else
        return "";
}

Object * ObjectManager::getObjectByName( const Name & name )
{
    Object * object;
    iterator it;

    object = 0;

    for( it = object_list.begin( ); it != object_list.end( ); it++ )
    {
        if( strcmp( name.c_str( ), (*it)->getName( ) ) == 0 )
        {
            object = (*it);
            break;
        }
    }

    if( !object )
    {
        Terminal::displayErrorMessage( "could not find an object named '%s'",
                                       name.c_str( ) );
    }

    return object;
}

void ObjectManager::listObjects( NameList & objects )
{
    iterator it;
    Name name;

    for( it = object_list.begin( ); it != object_list.end( ); it++ )
    {
        if( (*it) == active_object )
        {
            name = "* ";
            name += (*it)->getName( );
        }
        else
            name = (*it)->getName( );

        objects.push_back( name );
    }
}

void ObjectManager::listObjects( NameList & objects, NameList & plugins )
{
    iterator it;
    Name name;

    for( it = object_list.begin( ); it != object_list.end( ); it++ )
    {
        if( (*it) == active_object )
        {
            name = "* ";
            name += (*it)->getName( );
        }
        else
            name = (*it)->getName( );

        objects.push_back( name );
        plugins.push_back( (*it)->getPlugin( )->getName( ) );
    }
}

/**
 * This method is necessary since the one below conflicts with some defines
 * in TANGRAM2_HOME/include/tangram_macros.h
 */
bool ObjectManager::copyFromTangram( const Name & dst_name,
                                     const Name & src_name )
{
    return copy( dst_name, src_name );
}

bool ObjectManager::copy( const Name & dst_name, const Name & src_name )
{
    bool status;
    Object * src_object, * new_object;
    iterator it;

    status = false;

    src_object = getObjectByName( src_name );

    if( src_object )
    {
        new_object = src_object->copy( dst_name );
        new_object->plugin = src_object->getPlugin( );

        if( new_object )
        {
            status = true;

            for( it = object_list.begin( ); it != object_list.end( ); it++ )
            {
                if( dst_name == (*it)->getName( ) )
                    break;
            }

            if( it != object_list.end( ) )
            {
                delete (*it);
                (*it) = new_object;
            }
            else
            {
                object_list.push_back( new_object );
            }
        }
    }

    return status;
}

void ObjectManager::clear( void )
{
    iterator it;

    for( it = object_list.begin( ); it != object_list.end( ); it++ )
    {
        delete (*it);
    }

    object_list.clear( );
    active_object    = 0;
}
