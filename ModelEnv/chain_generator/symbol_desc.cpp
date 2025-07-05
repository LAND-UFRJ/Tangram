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
*	@file symbol_desc.cpp
*   @brief <b> Symbol, and Symbol_List methods </b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @version 1.0
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram II project
*   @since version 1.0
*
*/

#include <iostream>

using namespace std;

#include <stdio.h>
#include <string.h>

#include "prototypes.h"
#include "general.h"
#include "gramatica.h"
#include "chained_list.h"
#include "system_desc.h"


Symbol::Symbol() : Chained_Element()
{
  strcpy(name,"");
  type = 0;
  code = 0;
  max_value = 0;
  dimension = 0;
  char_code = '\0';

  debug(4,"Symbol::Symbol(): creating object");
}

Symbol::Symbol( char *a_name, int a_type, t_value a_val, int a_max_val,
	            int a_dim ) : Chained_Element()
{
    int i;

    strcpy( name, a_name );
    type = a_type;
    switch( type )
    {
        case STATE_VAR_TYPE:
            val.int_vec = (int *)new int[ a_dim ];
            for( i = 0; i < a_dim; i++ )
                val.int_vec[ i ] = a_val.int_vec[ i ];
            break;

        case STATE_VAR_FLOAT_TYPE:
            val.tgf_vec = (TGFLOAT *)new TGFLOAT[ a_dim ];
            for( i = 0; i < a_dim; i++ )
                val.tgf_vec[ i ] = a_val.tgf_vec[ i ];
            break;

        case INT_CONST_TYPE    : val.i = a_val.i; break;
        case FLOAT_CONST_TYPE  : val.f = a_val.f; break;
        case OBJECT_TYPE       : 
        case MSG_PORT_TYPE     : strcpy( val.str, a_val.str ); break;
        case STATE_VAR_FLOATQUEUE_TYPE: break; /* Nothing to be done */
        case STATE_VAR_INTQUEUE_TYPE:   break; /* Nothing to be done */
    }
    code      = 0;
    max_value = a_max_val;
    dimension = a_dim;
    char_code = '\0';

    debug( 4, "Symbol::Symbol(char *, int, t_value, int): creating object" );
}

Symbol::~Symbol()
{
    switch( type )
    {
        case STATE_VAR_TYPE:
            delete[] val.int_vec;
            break;
        case STATE_VAR_FLOAT_TYPE:
            delete[] val.tgf_vec;
            break;
    }

    debug( 4, "Symbol::~Symbol(): destroying object" );
}

void Symbol::ch_name( char *new_name )
{
    strcpy( name, new_name );
}

void Symbol::show_name( char *the_name )
{
    strcpy( the_name, name );
}

int Symbol::show_type()
{
    return( type );
}

void Symbol::show_value( t_value *the_value )
{
    int i;

    switch( type )
    {
        case STATE_VAR_TYPE:
            if( the_value->int_vec )
            {
                delete[] the_value->int_vec;
                the_value->int_vec = NULL;
            }
            the_value->int_vec = (int *)new int[ dimension ];
            for( i = 0; i < dimension; i++ )
                the_value->int_vec[ i ] = val.int_vec[ i ];
            break;

        case STATE_VAR_FLOAT_TYPE:
            if( the_value->tgf_vec )
            {
                delete[] the_value->tgf_vec;
                the_value->tgf_vec = NULL;
            }
            the_value->tgf_vec = (TGFLOAT *)new TGFLOAT[ dimension ];
            for( i = 0; i < dimension; i++ )
                the_value->tgf_vec[ i ] = val.tgf_vec[ i ];
            break;

        case INT_CONST_TYPE  : the_value->i = val.i; break;
        case FLOAT_CONST_TYPE: the_value->f = val.f; break;
        case OBJECT_TYPE     :
        case MSG_PORT_TYPE   : strcpy( the_value->str, val.str ); break;
    }
}

int *Symbol::show_value()
{
    int *vec, i;

    vec = NULL;
    if( type == STATE_VAR_TYPE )
    {
        if( (vec = new int[ dimension ]) != NULL )
        {
            for( i = 0; i < dimension; i++ )
                vec[ i ] = val.int_vec[ i ];
        }
        else
            perror( "new" );
    }

    return( vec );
}

TGFLOAT *Symbol::show_float_value()
{
    TGFLOAT *fvec;
    int i;

    fvec = NULL;
    if( type == STATE_VAR_FLOAT_TYPE )
    {
        if( (fvec = new TGFLOAT[ dimension ]) != NULL )
        {
            for( i = 0; i < dimension; i++ )
                fvec[ i ] = val.tgf_vec[ i ];
        }
        else
            perror ("new");
    }

    return( fvec );
}

void Symbol::ch_code( int a_code )
{
    code = a_code;
}

int Symbol::show_code()
{
    return( code );
}

void Symbol::ch_char_code( char a_code )
{
    char_code = a_code;
}

char Symbol::show_char_code()
{
    return( char_code );
}

int Symbol::show_max_value()
{
    return( max_value );
}

int Symbol::show_dimension()
{
  if( (type == STATE_VAR_TYPE) || (type == STATE_VAR_FLOAT_TYPE) ||
      (type == STATE_VAR_INTQUEUE_TYPE) || (type == STATE_VAR_FLOATQUEUE_TYPE) )
    return( dimension );
  else
    return( 0 );
}

void Symbol::print_sym()
{
    int i;

    fprintf( stdout, "%s -- ", name );
    switch( type )
    {
        case STATE_VAR_TYPE:
            fprintf( stdout, "STATE_VAR_TYPE -- " );
            fprintf( stdout, "[ " );
            for( i = 0; i < dimension - 1; i++ )
                fprintf( stdout, "%d, ", val.int_vec[ i ] );
            fprintf( stdout, "%d ]\n", val.int_vec[ dimension - 1 ] );
            break;

        case INT_CONST_TYPE   : fprintf( stdout, "INT_CONST_TYPE -- %d\n", val.i); break;
        case FLOAT_CONST_TYPE : fprintf( stdout, "FLOAT_CONST_TYPE -- %f\n", val.f);  break;
        case OBJECT_TYPE      : fprintf( stdout, "OBJECT_TYPE -- %s\n", val.str);  break;
        case MSG_PORT_TYPE    : fprintf( stdout, "MSG_PORT_TYPE -- %s\n", val.str); break;
        case REWARD_TYPE      : fprintf( stdout, "REWARD_TYPE - %s\n", name); break;
        case EVENT_TYPE       : fprintf( stdout, "EVENT_TYPE - %s\n", name); break;
    }
}

void Symbol::print_sym( FILE *fd )
{
    int i;

    fprintf( fd, "%s -- ", name );
    switch( type )
    {
        case STATE_VAR_TYPE:
            fprintf( fd, "STATE_VAR_TYPE -- " );
            fprintf( fd, "[ " );
            for( i = 0; i < dimension - 1; i++ )
                fprintf( fd, "%d, ", val.int_vec[ i ] );
            fprintf( fd, "%d ]\n", val.int_vec[ dimension - 1 ] );
            break;

        case INT_CONST_TYPE    : fprintf (fd, "INT_CONST_TYPE -- %d\n", val.i); break;
        case FLOAT_CONST_TYPE  : fprintf (fd, "FLOAT_CONST_TYPE -- %f\n", val.f);   break;
        case OBJECT_TYPE     : fprintf (fd, "OBJECT_TYPE -- %s\n", val.str); break;
        case MSG_PORT_TYPE   : fprintf (fd, "MSG_PORT_TYPE -- %s\n", val.str); break;
        case REWARD_TYPE     : fprintf (fd, "REWARD_TYPE - %s\n", name); break;
        case EVENT_TYPE      : fprintf (fd, "EVENT_TYPE - %s\n", name); break;
    }
}

/***********************************************************/

Symbol_List::Symbol_List() : Chained_List()
{
    debug( 4, "Symbol_List::Symbol_List(): creating object" );
}

Symbol_List::Symbol_List(Symbol_List *sym_list) : Chained_List( (Chained_List *)sym_list)
{
    debug( 4, "Symbol_List::Symbol_List(Symbol_List *): creating object" );
}

Symbol_List::~Symbol_List()
{
  Symbol *sym_aux;

  if (access == LIST_RW) {
    sym_aux = get_1st_sym();
    while (sym_aux != NULL) {
      delete sym_aux;
      sym_aux = get_1st_sym();
    }
  }
  debug(4,"Symbol_List::~Symbol_List(): destroying object");
}

int Symbol_List::add_tail_sym(Symbol *new_sym)
{
  return ( add_tail( (Chained_Element *)new_sym) );
}

Symbol *Symbol_List::show_1st_sym()
{
  return ( (Symbol *)show_head() );
}

Symbol *Symbol_List::get_1st_sym()
{
  return ( (Symbol *)del_head() );
}

Symbol *Symbol_List::del_sym()
{
  return ( (Symbol *)del_elem() );
}

Symbol *Symbol_List::show_next_sym()
{
  return ( (Symbol *)show_next() );
}

Symbol *Symbol_List::show_curr_sym()
{
  return ( (Symbol *)show_curr() );
}

int Symbol_List::show_no_sym()
{
  return ( show_no_elem() );
}

int Symbol_List::query_symbol(char *tg_sym_name)
{
  Symbol *sym_aux;
  char    sym_name[MAXSTRING];

  sym_aux = show_1st_sym();
  while (sym_aux != NULL){
    sym_aux->show_name(sym_name);
    if (! strcmp(sym_name, tg_sym_name) )
      break;
    else
      sym_aux = show_next_sym();
  }

  if (sym_aux == NULL)
    return(0);
  else
    return(1);
}

