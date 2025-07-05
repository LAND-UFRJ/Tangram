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
*	@file expr_val.cpp
*   @brief <b> Expression Value </b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram-II project
*   @since version 1.0
*
*   The detailed description is unavailable. 
*/

#include <iostream>

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"

/* Global Debug Level */
extern int Debug_Level;

#ifdef _MEM_DEBUG
extern int DBG_Expr_Val_counter;
extern int DBG_Expr_Val_memory;
extern int DBG_Expr_Val_List_counter;
extern int DBG_Expr_Val_List_memory;
#endif

//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
Expr_Val::Expr_Val( void ) : Chained_Element( )
{
    value = 0;
    type  = NUMERICAL;
    str_val = "";

#ifdef D_MEM_DEBUG
    DBG_id = DBG_Expr_Val_counter;
    DBG_Expr_Val_counter++;
    DBG_Expr_Val_memory++;
    debug_mem( DBG_id, "Expr_val::Expr_val(): creating object" );
#endif
}
//------------------------------------------------------------------------------
Expr_Val::Expr_Val( TGFLOAT a_val ) : Chained_Element( )
{
    value = a_val;
    type  = NUMERICAL;
    str_val = "";

#ifdef _MEM_DEBUG
    DBG_id = DBG_Expr_Val_counter;
    DBG_Expr_Val_counter++;
    DBG_Expr_Val_memory++;
    debug_mem( DBG_id, "Expr_val::Expr_val(TGFLOAT): creating object" );
#endif
}
//------------------------------------------------------------------------------
Expr_Val::Expr_Val( const char * a_str ) : Chained_Element( )
{
    value = 0;
    type  = LITERAL;
    str_val = a_str;

#ifdef _MEM_DEBUG
    DBG_id = DBG_Expr_Val_counter;
    DBG_Expr_Val_counter++;
    DBG_Expr_Val_memory++;
    debug_mem( DBG_id, "Expr_val::Expr_val(char *): creating object" );
#endif
}
//------------------------------------------------------------------------------
Expr_Val::Expr_Val( Expr_Val * a_expr ) : Chained_Element( )
{
    type = a_expr->show_type( );
    if( type == NUMERICAL )
    {
        value = a_expr->show_value( );
        str_val = "";
    }
    else
    {
        value   = 0;
        str_val = a_expr->show_str_val( );
    }

#ifdef _MEM_DEBUG
    DBG_id = DBG_Expr_Val_counter;
    DBG_Expr_Val_counter++;
    DBG_Expr_Val_memory++;
    debug_mem( DBG_id, "Expr_val::Expr_val(Expr_Val *): creating object" );
#endif
}
//------------------------------------------------------------------------------
Expr_Val::~Expr_Val( void )
{
    /* do nothing */
#ifdef _MEM_DEBUG
    DBG_Expr_Val_memory--;
    debug_mem( DBG_id, "Expr_val::~Expr_val(): destroying object" );
#endif
}
//------------------------------------------------------------------------------
int Expr_Val::show_type( void )
{
    return( type );
}
//------------------------------------------------------------------------------
TGFLOAT Expr_Val::show_value( void )
{
    if( type == NUMERICAL )
        return( value );
    else
    {
        debug( 3, "Expr_Val::show_value: Can't show a LITERAL value" );
        return( -1 );
    }
}
//------------------------------------------------------------------------------
void Expr_Val::ch_value( TGFLOAT a_val )
{
    if( type == LITERAL )
    {
        str_val = "";
        type = NUMERICAL;
    }
    value = a_val;
}
//------------------------------------------------------------------------------
const char * Expr_Val::show_str_val( void )
{
    if( type == LITERAL )
        return (const char *)str_val;
    else
    {
        /* FIXME - this is not good. There should be a Format method on T2String */
        char str_aux[ MAXEXPRSIZE ];

        sprintf( str_aux, "%.10E", value );
        str_val = str_aux;
        return (const char *)str_val;
    }
}
//------------------------------------------------------------------------------
void Expr_Val::add_expr_val(Expr_Val *a_val)
{
    char str_aux[MAXEXPRSIZE];

    /* FIXME - What happens if one expression is numerical and the other is a string */
    if( type == NUMERICAL && a_val->show_type() == NUMERICAL )
    {
        value = value + a_val->show_value();
        return;
    }

    /* FIXME - is this needed now that T2String is dynamically allocated?
       Probably not. */
    if ( (strlen( show_str_val( ) ) + strlen( a_val->show_str_val( ) )) >= MAXEXPRSIZE )
    {
        fprintf( stderr , "ERROR: Expression is too long (>%d).\n",MAXEXPRSIZE );
        fprintf( stderr , "ERROR: WHILE CONCATENATING: (%s)+(%s)\n",
                 show_str_val( ), a_val->show_str_val( ) );
        exit(-1);
    }
    sprintf( str_aux, "(%s)+(%s)", show_str_val( ), a_val->show_str_val( ) );
    str_val = str_aux;
    type = LITERAL;
}
//------------------------------------------------------------------------------
void Expr_Val::add_expr_val(TGFLOAT a_val)
{
    if (type == NUMERICAL)
        value += a_val;
    else
        debug(3,"Expr_Val::add_expr_val: Can't add a TGFLOAT to a LITERAL value");
}
//------------------------------------------------------------------------------
void Expr_Val::mul_expr_val(Expr_Val *a_val)
{
    char str_aux[MAXEXPRSIZE];

    /* if multipling by the numerical one, then return */
    if( a_val->show_type( ) == NUMERICAL )
        if( a_val->show_value( ) == 1 )
            return;

    /* both expr_val are numerical */
    if( type == NUMERICAL && a_val->show_type() == NUMERICAL )
    {
        value = value * a_val->show_value( );
        return;
    }

    /* one of them is numerical and has zero value */
    if( this->is_zero( ) || a_val->is_zero( ) )
    {
        value = 0;
        type = NUMERICAL;
        return;
    }

    /* FIXME - is this needed now that T2String is dynamically allocated?
       Probably not. */
    if ( (strlen( show_str_val( ) ) + strlen( a_val->show_str_val( ) )) >= MAXEXPRSIZE )
    {
        fprintf( stderr , "ERROR: Expression is too long (>%d).\n",MAXEXPRSIZE );
        exit(-1);
    }
    sprintf( str_aux, "(%s)*(%s)", show_str_val( ), a_val->show_str_val( ) );
    str_val = str_aux;
    type    = LITERAL;
}
//------------------------------------------------------------------------------
void Expr_Val::mul_expr_val(TGFLOAT a_val)
{
    if (type == NUMERICAL)
        value *= a_val;
    else
        debug(3,"Expr_Val::mul_expr_val: Can't mul a TGFLOAT to a LITERAL value");
}
//------------------------------------------------------------------------------
void Expr_Val::divide_by_expr_val(Expr_Val *a_val)
{
    char str_aux[MAXEXPRSIZE];

    /* if multipling by the numerical one, than return */
    if( a_val->show_type() == NUMERICAL )
        if( a_val->show_value() == 1 )
            return;

    /* check division by zero */
    if( a_val->is_zero( ) )
    {
        fprintf( stderr , "ERROR: Division by zero in Expr_Val objects.\n" );
        exit( -1 );
    }

    /* both expr_val are numerical */
    if (type == NUMERICAL && a_val->show_type() == NUMERICAL)
    {
        value = value / a_val->show_value();
        return;
    }

    /* one of them is numerical and has zero value */
    if( this->is_zero( ) )
    {
        value = 0;
        type  = NUMERICAL;
        return;
    }

    /* one of them is literal and the other is not zero */
    /* FIXME - is this needed now that T2String is dynamically allocated?
       Probably not. */
    if ( (strlen( show_str_val( ) ) + strlen( a_val->show_str_val( ) )) >= MAXEXPRSIZE )
    {
        fprintf( stderr , "ERROR: Expression is too long (>%d).\n",MAXEXPRSIZE );
        exit(-1);
    }
    sprintf( str_aux, "(%s)/(%s)", show_str_val( ), a_val->show_str_val( ) );
    str_val = str_aux;
    type    = LITERAL;
}
//------------------------------------------------------------------------------
int Expr_Val::comp_expr_val( Expr_Val * a_val )
{  
    if( type == NUMERICAL && a_val->show_type() == NUMERICAL )
        return ( (int) (value == a_val->show_value()) );

    if( type == LITERAL && a_val->show_type() == LITERAL )
        return ( (int) (strcmp( str_val, a_val->show_str_val( ) ) == 0) );

    /* all other cases are false */
    return (0);
}
//------------------------------------------------------------------------------
int Expr_Val::comp_expr_val(TGFLOAT a_val)
{
    if( type == NUMERICAL )
        return ( (int) (value == a_val) );
    else 
        debug(3,"Expr_Val::comp_expr_val: Can't compare a TGFLOAT to a LITERAL value");
    return( -1 );
}
//------------------------------------------------------------------------------
int Expr_Val::is_zero( )
{
    if( type == NUMERICAL )
        return( value == 0 );
    else
        return( 0 );
}
//------------------------------------------------------------------------------
void Expr_Val::print_expr( )
{
    fprintf( stdout ,"%d\t", type );
    if( type == NUMERICAL )
        fprintf( stdout , "%.10E", value );
    else
        fprintf( stdout , "%s", (const char *)str_val );
}
//------------------------------------------------------------------------------
void Expr_Val::print_expr( FILE * fd )
{
    if( type == NUMERICAL )
        fprintf( fd, "%.10E", value );
    else
        fprintf( fd, "%s", (const char *)str_val );
}
//------------------------------------------------------------------------------

/****************************************************************************/


Expr_Val_List::Expr_Val_List( ) : Chained_List( )
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Expr_Val_List_counter;
   DBG_Expr_Val_List_counter++;
   DBG_Expr_Val_List_memory++;
   debug_mem(DBG_id,"Expr_Val_List::Expr_Val_List(): creating object");
#endif
}

Expr_Val_List::Expr_Val_List( Expr_Val_List * expr_val_list ) :
                 Chained_List( (Chained_List *)expr_val_list )
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Expr_Val_List_counter;
   DBG_Expr_Val_List_counter++;
   DBG_Expr_Val_List_memory++;
   debug_mem(DBG_id,"Expr_Val_List::Expr_Val_List(Expr_Val_List *): creating object");
#endif
}

Expr_Val_List::~Expr_Val_List()
{
    Expr_Val * expr_val_aux;

    if( access == LIST_RW )
        while( !is_empty( ) )
        {
            expr_val_aux = get_1st_expr_val();
            delete expr_val_aux;
        }

#ifdef _MEM_DEBUG
    DBG_Expr_Val_List_memory--;
    debug_mem(DBG_id,"Expr_Val_List::~Expr_Val_List(): destroying object");
#endif
}

int Expr_Val_List::add_tail_expr_val( Expr_Val * new_expr_val )
{
    return( add_tail( (Chained_Element *)new_expr_val) );
}

Expr_Val *Expr_Val_List::show_1st_expr_val( )
{
    return( (Expr_Val *)show_head( ) );
}

Expr_Val *Expr_Val_List::get_1st_expr_val( )
{
    return( (Expr_Val *) del_head( ) );
}

Expr_Val *Expr_Val_List::show_next_expr_val( )
{
    return( (Expr_Val *)show_next( ) );
}

Expr_Val *Expr_Val_List::show_curr_expr_val( )
{
    return( (Expr_Val *)show_curr( ) );
}

void Expr_Val_List::clear_expr_val( )
{
    Expr_Val * expr_val_aux;

    if( access == LIST_RW )
        while( !is_empty( ) )
        {
            expr_val_aux = get_1st_expr_val( );
            delete expr_val_aux;
        }
}

int Expr_Val_List::show_no_expr_val( )
{
    return( show_no_elem( ) );
}
