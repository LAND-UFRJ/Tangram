/*
 * Copyright (C) 1999-2009, Edmundo Albuquerque de Souza e Silva.
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE.QPL included in the packaging of this file.
 *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/**
*  @file object_state.cpp
*  @brief <b>Object_State</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
* An Object_State is a vector of integers which size 
* vary from object to object. Since it does not have a fixed size, it 
* must be a double chained list. Also it's vector of integers must be 
* a chained list.
*
* The integer |object|_|id| will be used as a key to address the library object
* from the first library object publicly known by
* |System|_|Component|->|lib|_|component|.
*
* The |Msg|_|Vector|_|Element| is a double chained list which will make
* part in the |Vanishing|_|Object| which in turn will describe if a new
* Object_State is part of a Vanishing State or not.
*
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_desc.h"
#include "system_state.h"
#include "queue_type.h"

#ifdef _MEM_DEBUG
extern int DBG_Object_State_counter;
extern int DBG_Object_State_memory;
extern int DBG_Object_State_List_counter;
extern int DBG_Object_State_List_memory;
#endif

//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
/* Object State Methods. */
Object_State::Object_State() : Chained_Element()
{
    st_var_list   = new State_Variable_List();
    pend_msg_list = new Pending_Message_List();

    id       = 0; /* object identifier */
    obj_desc = NULL;

#ifdef _MEM_DEBUG
   DBG_id = DBG_Object_State_counter;
   DBG_Object_State_counter++;
   DBG_Object_State_memory++;
   debug_mem(DBG_id,"Object_State::Object_State(): creating object");
#endif
}

Object_State::Object_State( Object_Description *a_obj_desc ) : Chained_Element()
{
    st_var_list   = new State_Variable_List();
    pend_msg_list = new Pending_Message_List();

    if( a_obj_desc != NULL )
    {
        id       = a_obj_desc->show_id();
        obj_desc = a_obj_desc;
    }
    else
    {
        debug( 3, "Object_State::Object_State; Couldn't add NULL pointer new_lib");
        obj_desc = NULL;
    }

#ifdef _MEM_DEBUG
   DBG_id = DBG_Object_State_counter;
   DBG_Object_State_counter++;
   DBG_Object_State_memory++;
   debug_mem(DBG_id,"Object_State::Object_State(Object_Description *): creating object");
#endif
}

Object_State::Object_State(Object_State *obj_st) : Chained_Element()
{
    State_Variable_List  *st_var_list_aux;
    State_Variable       *st_var, *new_st_var;
    Pending_Message_List *pend_msg_list_aux;
    Pending_Message      *pend_msg, *new_pend_msg;

    st_var_list   = new State_Variable_List();
    pend_msg_list = new Pending_Message_List();

    id = obj_st->show_id();
    obj_desc = obj_st->show_obj_desc();


    /* get the state variable list */
    st_var_list_aux = obj_st->show_st_var_list(LIST_RO);
    st_var = st_var_list_aux->show_1st_st_var();
    /* copy the state variables */
    while( st_var != NULL )
    {
        new_st_var = new State_Variable( st_var );
        add_state_var( new_st_var );
        st_var = st_var_list_aux->show_next_st_var();
    }
    delete st_var_list_aux;

    /* get the pending message list */
    pend_msg_list_aux = obj_st->show_pend_msg_list( LIST_RO );
    pend_msg = pend_msg_list_aux->show_1st_pend_msg();

    /* copy the pending messages */
    while( pend_msg != NULL )
    {
        new_pend_msg = new Pending_Message( pend_msg );
        add_pend_msg( new_pend_msg );
        pend_msg = pend_msg_list_aux->show_next_pend_msg();
    }
    delete pend_msg_list_aux;

#ifdef _MEM_DEBUG
   DBG_id = DBG_Object_State_counter;
   DBG_Object_State_counter++;
   DBG_Object_State_memory++;
   debug_mem(DBG_id,"Object_State::Object_State(Object_State *): creating object");
#endif
}

Object_State::~Object_State()
{
    /* delete state variable list */
    delete st_var_list;

    /* delete the pending messages list */
    delete pend_msg_list;

#ifdef _MEM_DEBUG
   DBG_Object_State_memory--;
   debug_mem(DBG_id,"Object_State::~Object_State(): destroying object");
#endif
}

int Object_State::show_id() 
{
    return( id );
}

void Object_State::ch_id( int new_id )
{
    id = new_id;
}

Object_Description *Object_State::show_obj_desc()
{
    return( obj_desc );
}

void Object_State::ch_obj_desc( Object_Description *new_obj_desc )
{
    if( new_obj_desc != NULL )
        obj_desc = new_obj_desc;
    else
    {
        debug (3,"Object_State::Object_State; Couldn't add NULL pointer new_lib");
        obj_desc = NULL;
    }
}

State_Variable_List *Object_State::show_st_var_list( int access )
{
    State_Variable_List *st_var_list_aux;

    if( access == LIST_RW )
        return( st_var_list );
    else
    {
        st_var_list_aux = new State_Variable_List( st_var_list );
        return( st_var_list_aux );
    }
}

void Object_State::add_state_var( State_Variable *new_st_var )
{
    if( new_st_var != NULL )
        st_var_list->add_tail_st_var( new_st_var );
    else
        debug( 3, "Object_State::add_state_var: Can't add NULL pointer" );
}

void Object_State::add_pend_msg( Pending_Message *new_pend_msg )
{
    if( new_pend_msg != NULL )
        pend_msg_list->add_tail_pend_msg( new_pend_msg );
    else
        debug( 3, "Object_State::add_mesg: Can't add NULL pointer" );
}
//------------------------------------------------------------------------------
void Object_State::add_pend_msg(const char *port_name, const char *obj_dest_name, TGFLOAT msg_data, int size)
{
    char              obj_src_name[ MAXSTRING ];
    Pending_Message  *new_pend_msg;

    /* get the source object name of the message */
    obj_desc->show_name( obj_src_name );

    /* create a new pending message */
    new_pend_msg = new Pending_Message( obj_src_name, (char *)port_name,
                                        (char*)obj_dest_name, msg_data );

    /* add the pending message to the list */
    pend_msg_list->add_tail_pend_msg( new_pend_msg );
}
//------------------------------------------------------------------------------
void Object_State::add_pend_msg(const char *port_name, const char *obj_dest_name, int msg_data, int size)
{
    add_pend_msg( port_name, obj_dest_name, (TGFLOAT)msg_data, size );
}
//------------------------------------------------------------------------------
void Object_State::add_pend_msg(const char *port_name, const char *obj_dest_name, TGFLOAT *msg_data, int size )
{
    char              obj_src_name[ MAXSTRING ];
    Pending_Message  *new_pend_msg;

    /* get the source object name of the message */
    obj_desc->show_name( obj_src_name );

    /* create a new pending message */
    new_pend_msg = new Pending_Message( obj_src_name,  (char *)port_name,
                                        (char *)obj_dest_name, msg_data, size );

    /* add the pending message to the list */
    pend_msg_list->add_tail_pend_msg( new_pend_msg );
}
//------------------------------------------------------------------------------
void Object_State::add_pend_msg(const char *port_name, const char *obj_dest_name, int *msg_data, int size )
{
    char              obj_src_name[ MAXSTRING ];
    Pending_Message  *new_pend_msg;

    /* get the source object name of the message */
    obj_desc->show_name( obj_src_name );

    /* create a new pending message */
    new_pend_msg = new Pending_Message( obj_src_name,  (char *)port_name,
                                        (char *)obj_dest_name, msg_data, size );

    /* add the pending message to the list */
    pend_msg_list->add_tail_pend_msg( new_pend_msg );
}
//------------------------------------------------------------------------------
Pending_Message_List *Object_State::show_pend_msg_list( int access )
{
    Pending_Message_List *pend_list_aux;

    if( access == LIST_RW )
        return( pend_msg_list );
    else
    {
        pend_list_aux = new Pending_Message_List( pend_msg_list );
        return( pend_list_aux );
    }
}

Pending_Message *Object_State::get_1st_pend_msg()
{
  return( pend_msg_list->get_1st_pend_msg() );
}
//------------------------------------------------------------------------------
/* This method gets the message data values, updating msg_data, msg_source,   */
/*   msg_type and msg_size.  This returns the Object Pending_Message,  that   */
/*   will be used to obtain the vectors (int or float).                       */
Pending_Message *Object_State::get_msg_info( int *msg_data, const char *msg_source,
                                             int *msg_type, int  *msg_size )
{
    Pending_Message  *pend_msg;

    pend_msg = pend_msg_list->get_1st_pend_msg();
    if( pend_msg != NULL )
    {
        *msg_data = (int)pend_msg->show_message_data();
        *msg_type = pend_msg->show_message_type();
        *msg_size = pend_msg->show_message_size();
        pend_msg->show_src_obj_name( (char*)msg_source );
    }
    else
    {
        strcpy( (char *)msg_source, "" );
    }

    return ( pend_msg );
}
//------------------------------------------------------------------------------
void Object_State::destroy_msg( Pending_Message *pend_msg )
{
    if( pend_msg != NULL )
        delete pend_msg;
}
//------------------------------------------------------------------------------
int *Object_State::show_st_var_value( int st_var_code )
{
    State_Variable  *state_var;
    int             *value;

    if( st_var_list->query_st_var( st_var_code ) )
    {
        state_var = st_var_list->show_curr_st_var();
        value     = state_var->get_value();
        return( value );
    } else
        debug( 1, "Object_State::show_st_var_val: No such state var with this code\n");

   return( NULL );
}

TGFLOAT *Object_State::show_st_float_var_value( int st_var_code )
{
    State_Variable  *state_var;
    TGFLOAT         *fvalue;

    debug( 3, "Object_State::show_st_float_var_val\n" );
    if( st_var_list->query_st_var( st_var_code ) )
    {
        state_var = st_var_list->show_curr_st_var();
        fvalue    = state_var->get_float_value();
        return( fvalue );
    }
    else
        debug( 1, "Object_State::show_st_float_var_val: No such state var with this code\n");
    return( NULL );
}

int Object_State::show_st_var_dimension( int st_var_code )
{
    State_Variable  *state_var;
    int              value;

    if( st_var_list->query_st_var( st_var_code ) )
    {
        state_var = st_var_list->show_curr_st_var();
        value     = state_var->show_dimension();
        return( value );
    }
    else
        debug( 1, "Object_State::show_st_var_dimension: No such state var with this code: %d\n", st_var_code);
    return( 0 );
}

int Object_State::show_st_var_int_value( const char *st_var_name )
{
    int code, *vec;

//fprintf( stderr, "Object_State::show_st_var_int_value(st_var_name=%s)\n", st_var_name );
    code = obj_desc->show_symbol_code( (char *)st_var_name );
//fprintf( stderr, "    code=%d\n", code );
    if( (vec = show_st_var_value( code ) ) == NULL )
        debug( 1, "Severe ERROR(%s,%d).\n", st_var_name, code );
//fprintf( stderr, "    vec=%p\n", vec );
if( !vec ) abort( );

    return( vec[ 0 ] );
}

TGFLOAT Object_State::show_st_var_float_value( const char *st_var_name )
{
    int    code;
    TGFLOAT *fvec;

    code = obj_desc->show_symbol_code( (char *)st_var_name );
    if( (fvec = show_st_float_var_value( code )) == NULL )
        debug( 1, "Severe ERROR(%s {TGFLOAT},%d).\n", st_var_name, code );

    return( fvec[ 0 ] );
}

int *Object_State::show_st_var_vec_value( const char *st_var_name )
{
    int code, *vec;

    code = obj_desc->show_symbol_code( (char *)st_var_name );
    if( (vec = show_st_var_value( code )) == NULL )
    {
        printErrorMessage( "Severe ERROR.", 0 );
        fprintf( stderr, "State Variable name = %s\n\n", st_var_name );
        exit( -1 );
    }
    return( vec );
}

TGFLOAT *Object_State::show_st_var_fvec_value(const char *st_var_name)
{
    int    code;
    TGFLOAT *fvec;

    code = obj_desc->show_symbol_code( (char *)st_var_name );
    if( (fvec = show_st_float_var_value( code )) == NULL )
    {
        fprintf( stderr, "2Severe ERROR.\n" );
        exit( -1 );
    }
    return( fvec );
}

int Object_State::show_st_var_dimension(char *st_var_name)
{
    int code, dim;

    dim  = 0;
    code = obj_desc->show_symbol_code( st_var_name );
    if( code > 0 )
        dim = show_st_var_dimension( code );
    return( dim );
}

void Object_State::ch_st_var_value( const char *st_var_name, int st_var_val )
{
    State_Variable  *state_var;
    int              code, index;
    char            *name, *str;
    char             str_aux[ MAXSTRING ];

    strcpy( str_aux, st_var_name );
    str = strchr( str_aux, '[' );
    if( str != NULL )
    {
        index = atoi( str + 1 );
        name  = strtok( str_aux, "[" );
    }
    else
    {
        name  = str_aux;
        index = 0;
    }

    code = obj_desc->show_symbol_code( name );

    if( st_var_list->query_st_var( code ) )
    {
        state_var = st_var_list->show_curr_st_var();
        state_var->ch_value( index, st_var_val );
    }
}

void Object_State::ch_st_var_value(const char *st_var_name, TGFLOAT st_float_var_val)
{
    State_Variable  *state_var;
    int              code, index;
    char            *name, *str;
    char             str_aux[ MAXSTRING ];

    strcpy( str_aux, st_var_name );
    str = strchr( str_aux, '[' );
    if( str != NULL )
    {
        index = atoi( str + 1 );
        name  = strtok( str_aux, "[" );
    }
    else
    {
        name  = str_aux;
        index = 0;
    }

    code = obj_desc->show_symbol_code( name );

    if( st_var_list->query_st_var( code ) )
    {
        state_var = st_var_list->show_curr_st_var();
        state_var->ch_value( index, st_float_var_val );
    }
}

void Object_State::ch_st_var_value(const char *st_var_name, int* st_var_vec)
{
    State_Variable  *state_var;
    int              code;
    char            *name;
    char             str_aux[ MAXSTRING ];

    strcpy( str_aux, st_var_name );
    name = strtok( str_aux, "[" );
    code = obj_desc->show_symbol_code( name );

    if( st_var_list->query_st_var( code ) )
    {
        state_var = st_var_list->show_curr_st_var();
        state_var->ch_value( st_var_vec );
    }
}

void Object_State::ch_st_var_value(const char *st_var_name, TGFLOAT *st_float_var_vec)
{
    State_Variable  *state_var;
    int              code;
    char            *name;
    char             str_aux[ MAXSTRING ];

    strcpy( str_aux, st_var_name );
    name = strtok( str_aux, "[" );
    code = obj_desc->show_symbol_code( name );

    if( st_var_list->query_st_var( code ) )
    {
        state_var = st_var_list->show_curr_st_var();
        state_var->ch_float_value( st_float_var_vec );
    }
}

void Object_State::ch_st_var_value( const char *st_var_name, IntegerQueue &intqueue )
{
    State_Variable  *state_var;
    int              code;
    char            *name;
    char             str_aux[ MAXSTRING ];

    strcpy( str_aux, st_var_name );
    name = strtok( str_aux, "[" );
    code = obj_desc->show_symbol_code( name );

    if( st_var_list->query_st_var( code ) )
    {
        state_var = st_var_list->show_curr_st_var();
        state_var->ch_value( intqueue );
    }
}

void Object_State::get_st_var_value( IntegerQueue &st_intqueue_var, const char *st_var_name)
{
    State_Variable  *state_var;
    int              code;
    char            *name;
    char             str_aux[ MAXSTRING ];

    strcpy( str_aux, st_var_name );
    name = strtok( str_aux, "[" );
    code = obj_desc->show_symbol_code( name );

    if( st_var_list->query_st_var( code ) )
    {
        state_var       = st_var_list->show_curr_st_var();
        st_intqueue_var = state_var->show_intqueue_value();
    }
}

void Object_State::ch_st_var_value( const char *st_var_name, FloatQueue &fqueue )
{
    State_Variable  *state_var;
    int              code;
    char            *name;
    char             str_aux[ MAXSTRING ];

    strcpy( str_aux, st_var_name );
    name = strtok( str_aux, "[" );
    code = obj_desc->show_symbol_code( name );

    if( st_var_list->query_st_var( code ) )
    {
        state_var = st_var_list->show_curr_st_var();
        state_var->ch_value( fqueue );
    }
}

void Object_State::get_st_var_value( FloatQueue &st_fqueue_var, const char *st_var_name)
{
    State_Variable  *state_var;
    int              code;
    char            *name;
    char             str_aux[ MAXSTRING ];

    strcpy( str_aux, st_var_name );
    name = strtok( str_aux, "[" );
    code = obj_desc->show_symbol_code( name );

    if( st_var_list->query_st_var( code ) )
    {
        state_var     = st_var_list->show_curr_st_var();
        st_fqueue_var = state_var->show_floatqueue_value();
    }
}

void Object_State::get_st_var_value( int *st_var_vec, const char *st_var_name )
{
    State_Variable  *state_var;
    int              code;
    int             *state_val;
    char            *name;
    char             str_aux[ MAXSTRING ];

    strcpy( str_aux, st_var_name );
    name = strtok( str_aux, "[" );
    code = obj_desc->show_symbol_code( name );

    if( st_var_list->query_st_var( code ) )
    {
        state_var = st_var_list->show_curr_st_var();
        state_val = state_var->show_value();
        memcpy( st_var_vec, state_val, state_var->show_dimension() *
                                       sizeof( int ) );
        delete []state_val;
    }
}

//------------------------------------------------------------------------------
/* This method gets the data from the msg_data_int_vec. If the vector passed  */
/*   to it's method is less then msg_data_int_vec, the vector will get the    */
/*   initial part of the data. If the vector is greater, it will get all      */
/*   msg_data_int_vec, and the rest of the vector will be zeroed.             */
void Object_State::get_msg_data_vec( int* vector, int vec_size, Pending_Message  *pend_msg )
{
    if( pend_msg != NULL )
    {
        /* get the message vector data */
        switch( pend_msg->show_message_type() )
        {
            case INT_VEC:
                memset( vector, 0, vec_size );
                memcpy( vector, pend_msg->show_message_data_int_vec(),
                        ( fmin( vec_size, pend_msg->show_vector_size() ) ) );
                break;

            case FLOAT_VEC:
                printErrorMessage( "get_msg_data used to obtain integer data into a float variable.", 1 );
                exit( -2 );
                break;

            case NUMBER:
                *vector = (int)pend_msg->show_message_data();
                break;
        }
    }
}
//------------------------------------------------------------------------------
/* This method gets the data from the msg_data_float_vec. If the vector passed*/
/*   to it's method is less then msg_data_float_vec, the vector will get the  */
/*   initial part of the data. If the vector is greater, it will get all      */
/*   msg_data_float_vec, and the rest of the vector will be zeroed.           */
void Object_State::get_msg_data_vec( TGFLOAT* vector, int vec_size, Pending_Message  *pend_msg )
{
    if( pend_msg != NULL )
    {
        /* get the message vector data */
        switch( pend_msg->show_message_type() )
        {
            case INT_VEC:
                printErrorMessage( "get_msg_data used to obtain float data into an integer variable.", 1 );
                exit( -2 );
                break;

            case FLOAT_VEC:
                memset( vector, 0, vec_size );
                memcpy( vector, pend_msg->show_message_data_float_vec(),
                        ( fmin( vec_size,pend_msg->show_vector_size() ) ) );
                break;

            case NUMBER:
                *vector = pend_msg->show_message_data();
                break;
        }
    }
}
//------------------------------------------------------------------------------
void Object_State::save__at_tail( IntegerQueue &lc_var_name, int *lc_var_vec )
{
    lc_var_name.save_at_tail( lc_var_vec );
}

void Object_State::save__at_head( IntegerQueue &lc_var_name, int *lc_var_vec )
{
    lc_var_name.save_at_head( lc_var_vec );
}

void Object_State::restore__from_tail( IntegerQueue &lc_var_name, int *lc_var_vec )
{
    lc_var_name.restore_from_tail( lc_var_vec );
}

void Object_State::restore__from_head( IntegerQueue &lc_var_name, int *lc_var_vec )
{
    lc_var_name.restore_from_head( lc_var_vec );
}

void Object_State::save__at_tail( FloatQueue &lc_var_name, TGFLOAT *lc_var_vec )
{
    lc_var_name.save_at_tail( lc_var_vec );
}

void Object_State::save__at_head( FloatQueue &lc_var_name, TGFLOAT *lc_var_vec )
{
    lc_var_name.save_at_head( lc_var_vec );
}

void Object_State::restore__from_tail( FloatQueue &lc_var_name, TGFLOAT *lc_var_vec )
{
    lc_var_name.restore_from_tail( lc_var_vec );
}

void Object_State::restore__from_head( FloatQueue &lc_var_name, TGFLOAT *lc_var_vec )
{
    lc_var_name.restore_from_head( lc_var_vec );
}

void Object_State::get_st_var_value(int &st_var_vec, const char *st_var_name)
{
    State_Variable  *state_var;
    int              code, index;
    int             *state_val;
    char            *name, *str;
    char             str_aux[ MAXSTRING ];

    strcpy( str_aux, st_var_name );
    str = strchr( str_aux, '[' );
    if( str != NULL )
    {
        index = atoi( str + 1 );
        name  = strtok( str_aux, "[" );
    }
    else
    {
        name  = str_aux;
        index = 0;
    }

    code = obj_desc->show_symbol_code( name );

    if( st_var_list->query_st_var( code ) )
    {
        state_var  = st_var_list->show_curr_st_var();
        state_val  = state_var->show_value();
        st_var_vec = state_val[ index ];

        delete []state_val;
    }
}

void Object_State::get_st_var_value(TGFLOAT &st_float_var_vec, const char *st_var_name)
{
    State_Variable  *state_var;
    int              code, index;
    TGFLOAT           *state_float_val;
    char            *name, *str;
    char             str_aux[ MAXSTRING ];

    strcpy( str_aux, st_var_name );
    str = strchr( str_aux, '[' );
    if( str != NULL )
    {
        index = atoi( str + 1 );
        name  = strtok( str_aux, "[" );
    } 
    else
    {
        name  = str_aux;
        index = 0;
    }

    code = obj_desc->show_symbol_code( name );

    if( st_var_list->query_st_var( code ) )
    {
        state_var        = st_var_list->show_curr_st_var();
        state_float_val  = state_var->show_float_value();
        st_float_var_vec = state_float_val[ index ];

        delete []state_float_val;
    }
}

void Object_State::get_st_var_value(TGFLOAT* st_float_var_vec, const char *st_var_name)
{
    State_Variable  *state_var;
    int              code;
    TGFLOAT           *state_float_val;
    char            *name;
    char             str_aux[MAXSTRING];

    strcpy( str_aux, st_var_name );
    name = strtok( str_aux, "[" );
    code = obj_desc->show_symbol_code( name );

    if( st_var_list->query_st_var( code ) )
    {
        state_var = st_var_list->show_curr_st_var();
        state_float_val = state_var->show_float_value();
        memcpy( st_float_var_vec, state_float_val, state_var->show_dimension() *
                                                   sizeof( TGFLOAT ) );
        delete []state_float_val;
    }
}

int Object_State::compare_obj_st(Object_State *comp_obj_st)
{
    State_Variable       *st_var;
    State_Variable_List  *comp_st_var_list;
    State_Variable       *comp_st_var;

    st_var = st_var_list->show_1st_st_var();

    comp_st_var_list = comp_obj_st->show_st_var_list(LIST_RO);
    comp_st_var = comp_st_var_list->show_1st_st_var();

    while( st_var != NULL )
    {
        if( ((st_var->show_state_var_type() == STATE_VAR_INT)    &&
             (st_var->show_value() != comp_st_var->show_value() ))
         || ( (st_var->show_state_var_type() == STATE_VAR_FLOAT) &&
            (st_var->show_float_value() != comp_st_var->show_float_value()) ) )
        /* ANTIGO
        if ( st_var->show_value() != comp_st_var->show_value() )
        */
            break;
        else
        {
            st_var = st_var_list->show_next_st_var();
            comp_st_var = comp_st_var_list->show_next_st_var();
        }
    }
    delete comp_st_var_list;

    if( st_var != NULL )
        return( 0 );
    else
        return( 1 );
}

void Object_State::print_obj_st()
{
    State_Variable *st_var_aux;

    st_var_aux = st_var_list->show_1st_st_var();

    fprintf( stdout, "( " );
    while( st_var_aux != NULL )
    {
        st_var_aux->print_st_var();
        st_var_aux = st_var_list->show_next_st_var();
    }
    fprintf( stdout, ")" );
}

void Object_State::print_obj_st(FILE * file_out)
{
    State_Variable * st_var_aux;

    st_var_aux = st_var_list->show_1st_st_var();

    while( st_var_aux != NULL )
    {
        st_var_aux->print_st_var( file_out );
        st_var_aux = st_var_list->show_next_st_var();
        if( st_var_aux != NULL )
            fprintf( file_out, "," );
    }
}

void Object_State::print_van_obj_st()
{
    Pending_Message *pend_msg_aux;

    print_obj_st();

    pend_msg_aux = pend_msg_list->show_1st_pend_msg();
    while( pend_msg_aux != NULL )
    {
        fprintf( stdout, "-> " );
        pend_msg_aux->print_pend_msg();
        pend_msg_aux = pend_msg_list->show_next_pend_msg(); 
    } 
}

void Object_State::copy( int *dst, int dstSize, int *src, int srcSize )
{
    if( srcSize == dstSize )
        memcpy( dst, src, dstSize );
    else
    {
        printErrorMessage( "You are trying to copy vectors of different sizes and this is not allowed", 0 );
        exit( -3 );
    }
}

void Object_State::copy( TGFLOAT *dst, int dstSize, TGFLOAT *src, int srcSize )
{
    if( srcSize == dstSize )
        memcpy( dst, src, dstSize );
    else
    {
        printErrorMessage( "You are trying to copy vectors of different sizes and this is not allowed", 0 );
        exit( -3 );
    }
}

void Object_State::copy( IntegerQueue &dst, int dstSize, IntegerQueue &src, int srcSize )
{
    dst = src;
}

void Object_State::copy( FloatQueue &dst, int dstSize, FloatQueue &src, int srcSize )
{
    dst = src;
}

void Object_State::printErrorMessage( const char *errorMsg, int objPart )
{
    char myName[ MAXSTRING ];

    obj_desc->show_name( myName );
    fprintf( stderr, "\n ==== Error while executing the model ====\n");
    fprintf( stderr, "Error   : %s\n", errorMsg );
    fprintf( stderr, "Object  : %s\n", myName );
    switch( objPart )
    {
        case 0:  break; /* I don't know where I am */
        case 1:  fprintf( stderr, "Part    : Messages\n" ); break;
        default: fprintf( stderr, "Part    : Unknown\n" );
    }
    fprintf( stderr, "\n" );
}
/*************************************************************************/

Object_State_List::Object_State_List() : Chained_List()
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Object_State_List_counter;
   DBG_Object_State_List_counter++;
   DBG_Object_State_List_memory++;
   debug_mem(DBG_id,"Object_State_List::Object_State_List(): creating object");
#endif
}

Object_State_List::Object_State_List(Object_State_List *obj_st_list) 
                                   : Chained_List((Chained_List *)obj_st_list )
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Object_State_List_counter;
   DBG_Object_State_List_counter++;
   DBG_Object_State_List_memory++;
   debug_mem(DBG_id,"Object_State_List::Object_State_List(Object_List *): creating object");
#endif
}

Object_State_List::~Object_State_List()
{
    Object_State *obj_st_aux;

    if( access == LIST_RW )
        while( !is_empty() )
        {
            obj_st_aux = get_1st_obj_st();
            delete obj_st_aux;
        }

#ifdef _MEM_DEBUG
    DBG_Object_State_List_memory--;
    debug_mem( DBG_id, "Object_State_List::~Object_State_List(): destroying object");
#endif
}

int Object_State_List::add_tail_obj_st(Object_State *new_obj_st)
{
    return( add_tail( (Chained_Element *)new_obj_st ) );
}

Object_State *Object_State_List::show_1st_obj_st()
{
    return( (Object_State *) show_head() );
}

Object_State *Object_State_List::get_1st_obj_st()
{
    return( (Object_State *) del_head() );
}

Object_State *Object_State_List::show_next_obj_st()
{
    return( (Object_State *) show_next() );
}

Object_State *Object_State_List::show_curr_obj_st()
{
    return( (Object_State *) show_curr() );
}

int Object_State_List::query_obj_st(int obj_id)
{
    Object_State *obj_aux;

    obj_aux = show_1st_obj_st();
    while( obj_aux != NULL )
    {
        if( obj_aux->show_id() == obj_id )
            break;
        else
            obj_aux = show_next_obj_st();
    }

    if( obj_aux == NULL )
        return( 0 );
    else
        return( 1 );
}

int Object_State_List::replace_obj_st(Object_State *new_obj)
{
    Object_State  *obj_st_aux;
    int            obj_id_aux;

    obj_id_aux = new_obj->show_id();

    if( query_obj_st( obj_id_aux ) )
    {
        obj_st_aux = show_curr_obj_st();
        replace_elem((Chained_Element *)new_obj);
        delete obj_st_aux;
        return( 1 );
    }
    else
        return( 0 );
}

int Object_State_List::show_no_obj_st()
{
  return( show_no_elem() ); 
}
