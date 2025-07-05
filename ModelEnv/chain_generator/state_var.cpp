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
*	@file state_var.cpp
*   @brief <b> State_Variable methods </b>.
*   @remarks Lot of.
*   @author Daniel Ratton Figueredo
*	@author Kelvin Reinhardt
*   @author Bruno Felisberto
*   @version 2.0
*   @date 11-06-2001
*   @warning Do not modify this class before knowing the whole Tangram II project
*   @since version 1.0
*
*/

#include <iostream>

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"

/* State Vector Element methods. */

#ifdef _MEM_DEBUG
extern int DBG_State_Variable_counter;
extern int DBG_State_Variable_memory;
extern int DBG_State_Variable_List_counter;
extern int DBG_State_Variable_List_memory;
#endif

State_Variable::State_Variable(int *a_value, int a_code, int a_dim) : Chained_Element()
{
   state_var_type  = STATE_VAR_INT;
   uvalue.intvalue = a_value;
   code            = a_code;
   dimension       = a_dim;
   debug(3, "State_Variable::State_Variable(int *, int, int ) : Chained_Element()");

#ifdef _MEM_DEBUG
   DBG_id = DBG_State_Variable_counter;
   DBG_State_Variable_counter++;
   DBG_State_Variable_memory++;
   debug_mem(DBG_id,"State_Variable::State_Variable(int *, int, int): creating object");
#endif
}

State_Variable::State_Variable(int a_value, int a_code) : Chained_Element()
{
    uvalue.intvalue    = new int[ 1 ];
    uvalue.intvalue[0] = a_value;
    code               = a_code;
    dimension          = 1;
    state_var_type     = STATE_VAR_INT;
    debug( 3, "State_Variable::State_Variable( int, int ) : Chained_Element()");

#ifdef _MEM_DEBUG
   DBG_id = DBG_State_Variable_counter;
   DBG_State_Variable_counter++;
   DBG_State_Variable_memory++;
   debug_mem(DBG_id,"State_Variable::State_Variable( int, int ): creating object");
#endif
}

State_Variable::State_Variable( IntegerQueue *a_value, int a_code ) : Chained_Element()
{
    code            = a_code;
    uvalue.intqueue = a_value;
    dimension       = a_value->show_size();
    state_var_type  = STATE_VAR_INTQUEUE;
    debug( 3, "State_Variable::State_Variable( IntegerQueue *, int ) : Chained_Element()");
}

State_Variable::State_Variable( FloatQueue *a_value, int a_code ) : Chained_Element()
{
    code              = a_code;
    uvalue.floatqueue = a_value;
    dimension         = a_value->show_size();
    state_var_type    = STATE_VAR_FLOATQUEUE;
    debug(3, "State_Variable::State_Variable( FloatQueue *, int ) : Chained_Element()");
}

State_Variable::State_Variable(TGFLOAT *a_value, int a_code, int a_dim) : Chained_Element()
{
    uvalue.float_value = a_value;
    code               = a_code;
    dimension          = a_dim;
    state_var_type     = STATE_VAR_FLOAT;
    debug( 3, "State_Variable::State_Variable( TGFLOAT *, int, int ) : Chained_Element()");

#ifdef _MEM_DEBUG
   DBG_id = DBG_State_Variable_counter;
   DBG_State_Variable_counter++;
   DBG_State_Variable_memory++;
   debug_mem(DBG_id,"State_Variable::State_Variable( int *, int, int ): creating object");
#endif
}


State_Variable::State_Variable( TGFLOAT a_value, int a_code) : Chained_Element()
{
    uvalue.float_value    = new TGFLOAT [1];
    uvalue.float_value[0] = a_value;
    code                  = a_code;
    dimension             = 1;
    state_var_type        = STATE_VAR_FLOAT;
    debug( 3, "State_Variable::State_Variable( TGFLOAT, int ) : Chained_Element()");

/*#ifdef _MEM_DEBUG
   DBG_id = DBG_State_Variable_counter;
   DBG_State_Variable_counter++;
   DBG_State_Variable_memory++;
   debug_mem(DBG_id,"State_Variable::State_Variable(int, int): creating object");
#endif*/
}

State_Variable::State_Variable(State_Variable *a_st_var) : Chained_Element()
{
    code           = a_st_var->show_code();
    dimension      = a_st_var->show_dimension();
    state_var_type = a_st_var->show_state_var_type();
    switch( state_var_type )
    {
        case STATE_VAR_INTQUEUE:
            uvalue.intqueue = new IntegerQueue( a_st_var->uvalue.intqueue );
            break;

        case STATE_VAR_FLOATQUEUE:
            uvalue.floatqueue = new FloatQueue( a_st_var->uvalue.floatqueue );
            break;

        case STATE_VAR_INT:
            uvalue.intvalue = a_st_var->show_value();
            break;

        case STATE_VAR_FLOAT:
            uvalue.float_value = a_st_var->show_float_value();
            break;

        default:
            fprintf( stderr, "Unknown variable type (%d)\n", state_var_type );
            exit( -1 );
    }

    debug( 3, "State_Variable::State_Variable( State_Variable * ) : Chained_Element()");

#ifdef _MEM_DEBUG
   DBG_id = DBG_State_Variable_counter;
   DBG_State_Variable_counter++;
   DBG_State_Variable_memory++;
   debug_mem(DBG_id,"State_Variable::State_Variable( State_Variable * ): creating object");
#endif
}

State_Variable::~State_Variable()
{
    switch( state_var_type )
    {
        case STATE_VAR_INT:        delete[] uvalue.intvalue;    break;
        case STATE_VAR_FLOAT:      delete[] uvalue.float_value; break;
        case STATE_VAR_INTQUEUE:   delete   uvalue.intqueue;    break;
        case STATE_VAR_FLOATQUEUE: delete   uvalue.floatqueue;  break;
    }
#ifdef _MEM_DEBUG
   DBG_State_Variable_memory--;
   debug_mem(DBG_id,"State_Variable::~State_Variable(): destroying object");
#endif
}

void State_Variable::ch_value( int *a_value )
{
    int i;

    switch( state_var_type )
    {
        case STATE_VAR_FLOAT:
            for( i = 0; i < dimension; i++ )
                uvalue.float_value[i] = a_value[i];
            break;

        case STATE_VAR_INT:
            for( i = 0; i < dimension; i++ )
                uvalue.intvalue[i] = a_value[i];
            break;

        default:
            fprintf( stderr, "ERROR: wrong variable type in assignment\n" );
            exit( 0 );
    }
}

void State_Variable::ch_value( IntegerQueue &localqueue )
{
    if( state_var_type == STATE_VAR_INTQUEUE )
    {
        // this operator = has been redefined in queue_type.cpp
        *uvalue.intqueue = localqueue;
    }
    else
    {
        // Can not assign a intqueue value in a non-intqueue variable
        fprintf( stderr, "ERROR: intqueue assignment into a non-intqueue variable\n" );
        exit( 0 );
    }
}

void State_Variable::ch_value( FloatQueue &localqueue )
{
    if( state_var_type == STATE_VAR_FLOATQUEUE )
    {
        // this operator = has been redefined in queue_type.cpp
        *uvalue.floatqueue = localqueue;
    }
    else
    {
        // Can not assign a floatqueue value in a non-floatqueue variable
        fprintf( stderr, "ERROR: floatqueue assignment into a non-floatqueue variable\n" );
        exit( 0 );
    }
}

void State_Variable::ch_float_value( TGFLOAT *a_value )
{
    int i;
    if( state_var_type != STATE_VAR_FLOAT )
    {
        debug(3," void State_Variable::ch_float_value( TGFLOAT * ): value == %d... \nThis state var isn't TGFLOAT!",uvalue.intvalue);
        exit( 0 );
    }
    for( i = 0; i < dimension; i++ )
        uvalue.float_value[ i ] = a_value[ i ];
}

void State_Variable::ch_float_value( int index, TGFLOAT a_value )
{
    if( state_var_type != STATE_VAR_FLOAT )
    {
        debug( 3, " void State_Variable::ch_float_value( int, TGFLOAT ): value == %d... \nThis state var is int!",uvalue.intvalue);
        exit( 0 );
    }
    uvalue.float_value[ index ] = a_value;
}

void State_Variable::ch_value( int index, TGFLOAT a_value )
{
    switch( state_var_type )
    {
        case STATE_VAR_FLOAT: uvalue.float_value[ index ] = a_value;   break;
        case STATE_VAR_INT:   uvalue.intvalue[ index ] = (int)a_value; break;
        default: fprintf( stderr, "Invalid typecast" ); exit( 0 );
    }
}

int *State_Variable::show_value()
{
    int *vec, i;

    vec = NULL;
    if( state_var_type == STATE_VAR_INT )
    {
        if( (vec = new int [ dimension ]) != NULL )
        {
            for( i = 0; i < dimension; i++ )
                vec[ i ] = uvalue.intvalue[ i ];
        }
    }
    return( vec );
}

TGFLOAT *State_Variable::show_float_value()
{
    TGFLOAT *fvec;
    int i;

    fvec = NULL;
    if( state_var_type == STATE_VAR_FLOAT )
    {
        if( (fvec = new TGFLOAT [ dimension ]) != NULL )
        {
            for( i = 0; i < dimension; i++ )
                fvec[ i ] = uvalue.float_value[ i ];
        }
    }

    return( fvec );
}

IntegerQueue &State_Variable::show_intqueue_value()
{
    return( *uvalue.intqueue );
}

FloatQueue &State_Variable::show_floatqueue_value()
{
    return( *uvalue.floatqueue );
}

int State_Variable::show_state_var_type()
{
    return( state_var_type );
}

int *State_Variable::get_value()
{
    if( state_var_type != STATE_VAR_INT )
        debug(1," int* State_Variable::get_value(): Warning, trying to get int values from float state vars");

    return( uvalue.intvalue );
}

TGFLOAT* State_Variable::get_float_value()
{
    if( state_var_type != STATE_VAR_FLOAT )
        debug(1," TGFLOAT* State_Variable::get_float_value(): Warning, trying to get float values from integer state vars");
    return( uvalue.float_value );
}

void State_Variable::ch_code( int a_code )
{
    code = a_code;
}

int State_Variable::show_code()
{
    return( code );
}

int State_Variable::show_dimension()
{
    return( dimension );
}

int State_Variable::compare( State_Variable *a_st_var )
{
    int   i, status;
    int     *ivalue;
    TGFLOAT *fvalue;

    status = 1;
    if( (code      == a_st_var->show_code()) &&
        (dimension == a_st_var->show_dimension()) )
    {
        if( state_var_type != a_st_var->show_state_var_type() )
            return( 0 );

        switch( state_var_type )
        {
            case STATE_VAR_FLOAT:
                fvalue = a_st_var->get_float_value();
                for( i = 0; i < dimension && status; i++ )
                    if( uvalue.float_value[ i ] != fvalue[ i ] )
                        status = 0;
                break;

            case STATE_VAR_INT:
                ivalue = a_st_var->get_value();
                for( i = 0; i < dimension && status; i++ )
                    if( uvalue.intvalue[ i ] != ivalue[ i ] )
                        status = 0;
                break;

            default:
               debug(1,"int State_Variable::compare( State_Variable * ): Not a state var consistent type");
               exit( 0 );
        }
    }
    return( status );
}

void State_Variable::print_st_var()
{
    print_st_var( stdout );
}

void State_Variable::print_st_var(FILE *fd)
{
   int i;

   fprintf( fd, "[" );
   switch( state_var_type )
   {
       case STATE_VAR_FLOAT:
           for( i = 0; i < dimension - 1; i++ )
               fprintf( fd, "%f, ", uvalue.float_value[ i ] );
           fprintf( fd, "%f]", uvalue.float_value[ dimension - 1 ] );
           break;

       case STATE_VAR_INT:
           for( i = 0; i < dimension - 1; i++ )
               fprintf(fd, "%d, ", uvalue.intvalue[ i ] );
           fprintf( fd, "%d]", uvalue.intvalue[ dimension - 1 ] );
           break;

       case STATE_VAR_INTQUEUE:
           fprintf( fd, "IQ%d]", dimension );
           break;

       case STATE_VAR_FLOATQUEUE:
           fprintf( fd, "FQ%d]", dimension );
           break;

       default:
          debug( 1, "void State_Variable::print_st_var(FILE *): Not a state var consistent type");
          exit( 0 );
   }
}

/*********************************************************************/

State_Variable_List::State_Variable_List() : Chained_List()
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_State_Variable_List_counter;
   DBG_State_Variable_List_counter++;
   DBG_State_Variable_List_memory++;
   debug_mem(DBG_id,"State_Variable_List::State_Variable_List(): creating object");
#endif
}

State_Variable_List::State_Variable_List(State_Variable_List *st_var_list) :
                     Chained_List( (Chained_List *)st_var_list )

{
#ifdef _MEM_DEBUG
   DBG_id = DBG_State_Variable_List_counter;
   DBG_State_Variable_List_counter++;
   DBG_State_Variable_List_memory++;
   debug_mem(DBG_id,"State_Variable_List::State_Variable_List(State_Variable_List *): creating object");
#endif
}

State_Variable_List::~State_Variable_List()
{
    State_Variable *st_var_aux;

    if( access == LIST_RW )
        while( !is_empty() )
        {
            st_var_aux = get_1st_st_var();
            delete st_var_aux;
        }

#ifdef _MEM_DEBUG
   DBG_State_Variable_List_memory--;
   debug_mem(DBG_id,"State_Variable_List::~State_Variable_List(): destroying object");
#endif
}

int State_Variable_List::add_tail_st_var(State_Variable *new_st_var)
{
    return( add_tail( (Chained_Element *)new_st_var) );
}

State_Variable *State_Variable_List::show_1st_st_var()
{
    return( (State_Variable *)show_head() );
}

State_Variable *State_Variable_List::get_1st_st_var()
{
    return( (State_Variable *)del_head() );
}

State_Variable *State_Variable_List::show_next_st_var()
{
    return( (State_Variable *)show_next() );
}

State_Variable *State_Variable_List::show_curr_st_var()
{
    return( (State_Variable *)show_curr() );
}

int State_Variable_List::show_no_st_var()
{
    return( show_no_elem() );
}

int State_Variable_List::query_st_var(int tg_st_var_code)
{
    State_Variable *st_var_aux;

    st_var_aux = show_1st_st_var();
    while( st_var_aux != NULL )
    {
        if( tg_st_var_code == st_var_aux->show_code() )
            break;
        else
            st_var_aux = show_next_st_var();
    }

    if( st_var_aux == NULL )
        return( 0 );
    else
        return( 1 );
}
