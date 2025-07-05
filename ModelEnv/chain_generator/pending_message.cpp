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
*  @file pending_message.cpp
*  @brief <b>Pending message methods</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  The detailed description is unavailable. 
*/

#include <iostream>

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "string_list.h"
#include "system_desc.h"
#include "system_state.h"

#ifdef _MEM_DEBUG
extern int DBG_Pending_Message_counter;
extern int DBG_Pending_Message_memory;
extern int DBG_Pending_Message_List_counter;
extern int DBG_Pending_Message_List_memory;
#endif

/* the global system description */
extern System_Description *the_system_desc;

//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
Pending_Message::Pending_Message() : Chained_Element()
{
    strcpy (obj_source_name, "");
    strcpy (obj_dest_name, "");
    strcpy (port_name, "");
    message_data = 0;
    message_data_int_vec = NULL;
    message_data_float_vec = NULL;
    message_type = NUMBER;
    vector_size = 0; /* vector size in bytes */

    #ifdef _MEM_DEBUG
    DBG_id = DBG_Pending_Message_counter;
    DBG_Pending_Message_counter++;
    DBG_Pending_Message_memory++;
    debug_mem(DBG_id,"Pending_Message::Pending_Message(): creating object");
    #endif
}
//------------------------------------------------------------------------------
Pending_Message::Pending_Message(char *obj_src, char *port, char *obj_dest, TGFLOAT msg_data) :
                 Chained_Element()
{
    strcpy( obj_source_name, obj_src  );
    strcpy( obj_dest_name,   obj_dest );
    strcpy( port_name,       port     );
    message_data = msg_data;

    message_data_int_vec   = NULL;
    message_data_float_vec = NULL;
    message_type           = NUMBER;
    vector_size            = 0; /* vector size in bytes */

    #ifdef _MEM_DEBUG
    DBG_id = DBG_Pending_Message_counter;
    DBG_Pending_Message_counter++;
    DBG_Pending_Message_memory++;
    debug_mem(DBG_id, "Pending_Message::Pending_Message(char *, char *, char *, number): creating object");
    #endif
}
//------------------------------------------------------------------------------
Pending_Message::Pending_Message(char *obj_src, char *port, char *obj_dest,
         TGFLOAT *msg_data, int vec_size) : Chained_Element()
{
    strcpy( obj_source_name, obj_src  );
    strcpy( obj_dest_name,   obj_dest );
    strcpy( port_name, port );

    // unused
    message_data = 0;
    message_data_int_vec = NULL;

    // used
    message_data_float_vec = NULL;
    vector_size = 0;
    if( (msg_data != NULL) && (vec_size > 0) )
    {
        vector_size = vec_size; /* vector size in bytes */
        message_data_float_vec = (TGFLOAT*) malloc( vector_size );
        memset( message_data_float_vec, 0, vector_size );
        memcpy( message_data_float_vec, msg_data, vector_size );
    }
    message_type = FLOAT_VEC;

    #ifdef _MEM_DEBUG
    DBG_id = DBG_Pending_Message_counter;
    DBG_Pending_Message_counter++;
    DBG_Pending_Message_memory++;
    debug_mem(DBG_id, "Pending_Message::Pending_Message(char *, char *, char *, number): creating object");
    #endif
}
//------------------------------------------------------------------------------
Pending_Message::Pending_Message(char *obj_src, char *port, char *obj_dest,
         int *msg_data, int vec_size ) : Chained_Element()
{
    strcpy (obj_source_name, obj_src);
    strcpy (obj_dest_name, obj_dest);
    strcpy (port_name, port);

    // unused
    message_data = 0;
    message_data_float_vec = NULL;
    // used
    message_data_int_vec = NULL;
    vector_size = 0;
    if ( (msg_data != NULL) && (vec_size > 0) )
    {
        vector_size = vec_size; /* vector size in bytes */
        message_data_int_vec = (int*) malloc( vector_size );
        memset( message_data_int_vec, 0, vector_size );
        memcpy( message_data_int_vec, msg_data, vector_size );
    }
    message_type = INT_VEC;

    #ifdef _MEM_DEBUG
    DBG_id = DBG_Pending_Message_counter;
    DBG_Pending_Message_counter++;
    DBG_Pending_Message_memory++;
    debug_mem(DBG_id, "Pending_Message::Pending_Message(char *, char *, char *, number): creating object");
    #endif
}
//------------------------------------------------------------------------------
Pending_Message::Pending_Message( Pending_Message *pend_msg ) : Chained_Element()
{
    char obj_src[MAXSTRING];
    char obj_dest[MAXSTRING];
    char port[MAXSTRING];

    pend_msg->show_src_obj_name(obj_src);
    strcpy (obj_source_name, obj_src);

    pend_msg->show_dest_obj_name(obj_dest);
    strcpy (obj_dest_name, obj_dest);

    pend_msg->show_port_name(port);
    strcpy (port_name, port);

    message_data = pend_msg->show_message_data();
    message_type = pend_msg->show_message_type();
    vector_size  = pend_msg->show_vector_size();

    message_data_float_vec = NULL;
    message_data_int_vec   = NULL;

    switch( message_type )
    {
        case INT_VEC:
            ch_message_data_int_vec( pend_msg->show_message_data_int_vec(), pend_msg->show_vector_size() );
        break;

        case FLOAT_VEC:
            ch_message_data_float_vec( pend_msg->show_message_data_float_vec(), pend_msg->show_vector_size() );
        break;
    }

    #ifdef _MEM_DEBUG
    DBG_id = DBG_Pending_Message_counter;
    DBG_Pending_Message_counter++;
    DBG_Pending_Message_memory++;
    debug_mem(DBG_id,"Pending_Message::Pending_Message(Pending_Message *): creating object");
    #endif
}
//------------------------------------------------------------------------------
Pending_Message::~Pending_Message()
{
    if ( message_data_float_vec != NULL )
        free( message_data_float_vec );

    if ( message_data_int_vec != NULL )
        free( message_data_int_vec );

    #ifdef _MEM_DEBUG
    DBG_Pending_Message_memory--;
    debug_mem(DBG_id,"Pending_Message::~Pending_Message(): destroying object");
    #endif
}
//------------------------------------------------------------------------------
void Pending_Message::ch_src_obj_name(char *name)
{
  strcpy (obj_source_name, name);
}

void Pending_Message::show_src_obj_name(char *name)
{
  strcpy (name, obj_source_name);
}

void Pending_Message::ch_dest_obj_name(char *name)
{
  strcpy (obj_dest_name, name);
}

void Pending_Message::show_dest_obj_name(char *name)
{
  strcpy (name, obj_dest_name);
}

void Pending_Message::ch_port_name(char *name)
{
  strcpy (port_name, name);
}

void Pending_Message::show_port_name(char *name)
{
    strcpy (name, port_name);
}
//------------------------------------------------------------------------------
void Pending_Message::ch_message_type(int new_type)
{
    message_type = new_type;
}
//------------------------------------------------------------------------------
int Pending_Message::show_message_type()
{
    return (message_type);
}
//------------------------------------------------------------------------------
int Pending_Message::show_message_size()
{
    int size;

    size = 0;
    switch( message_type )
    {
        case NUMBER:    size = 0;                           break;
        case INT_VEC:   size = vector_size/sizeof(int);     break;
        case FLOAT_VEC: size = vector_size/sizeof(TGFLOAT); break;
    }

    return( size );
}
//------------------------------------------------------------------------------
void Pending_Message::ch_vector_size(int new_size)
{
    vector_size = new_size;
}
//------------------------------------------------------------------------------
int Pending_Message::show_vector_size()
{
    return( vector_size );
}
//------------------------------------------------------------------------------
void Pending_Message::ch_message_data(TGFLOAT new_msg_data)
{
    message_data = new_msg_data;
}
//------------------------------------------------------------------------------
TGFLOAT Pending_Message::show_message_data()
{
    return( message_data );
}
//------------------------------------------------------------------------------
void Pending_Message::ch_message_data_int_vec(int *new_msg_data, int new_size)
{
    if ( message_data_int_vec != NULL )
    {
        free( message_data_int_vec );
        vector_size = 0;
    }
    if ( (new_msg_data != NULL) && (new_size > 0) )
    {
        message_data_int_vec = (int *) malloc( new_size );
        memset( message_data_int_vec, 0, new_size );
        memcpy( message_data_int_vec, new_msg_data, new_size );
        vector_size = new_size ;
    }
}
//------------------------------------------------------------------------------
int *Pending_Message::show_message_data_int_vec()
{
    return (message_data_int_vec);
}
//------------------------------------------------------------------------------
void Pending_Message::ch_message_data_float_vec(TGFLOAT *new_msg_data, int new_size)
{
    if ( message_data_float_vec != NULL )
    {
        free( message_data_float_vec );
        vector_size = 0;
    }
    if ( (new_msg_data != NULL) && (new_size > 0) )
    {
        message_data_float_vec = (TGFLOAT *) malloc( new_size );
        memset( message_data_float_vec, 0, new_size );
        memcpy( message_data_float_vec, new_msg_data, new_size );
        vector_size = new_size ;
    }
}
//------------------------------------------------------------------------------
TGFLOAT *Pending_Message::show_message_data_float_vec()
{
    return ( message_data_float_vec );
}
//------------------------------------------------------------------------------
int Pending_Message::is_multidest()
{
  if (strcmp(obj_dest_name, "*"))
    return (FALSE);
  else
    return (TRUE);
}
//------------------------------------------------------------------------------
Pending_Message_List *Pending_Message::expand()
{
    String_List          *obj_list;
    String               *obj;
    Pending_Message_List *pend_msg_list;
    Pending_Message      *pend_msg = NULL;
    char                  dest_name[MAXSTRING];

    /* get a string list with all objs with port_name */
    obj_list = the_system_desc->show_objs_by_port(port_name);

    /* create an empty pending message list */
    pend_msg_list = new Pending_Message_List();

    /* go through all objs in the list */
    obj = obj_list->get_1st_str();
    while (obj != NULL)
    {
        /* get the name of the object */
        obj->show_str(dest_name);
        if (strcmp(dest_name, obj_source_name))
        {
            /* create a new pending message similar to this one */
            pend_msg = new Pending_Message( this );
            pend_msg->ch_dest_obj_name( dest_name );

            /* append this new message to the list */
            pend_msg_list->add_tail_pend_msg(pend_msg);
        }
        /* go to the next object */
        delete obj;
        obj = obj_list->get_1st_str();
    }
    /* delete the obj list */
    delete obj_list;

    return (pend_msg_list);
}
//------------------------------------------------------------------------------
void Pending_Message::print_pend_msg()
{
  fprintf ( stdout, "(%s, %s, %s, %f)\n", obj_source_name, obj_dest_name, port_name, message_data);
}

/**********************************************************************/

Pending_Message_List::Pending_Message_List() : Chained_List()
{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Pending_Message_List_counter;
   DBG_Pending_Message_List_counter++;
   DBG_Pending_Message_List_memory++;
   debug_mem(DBG_id,"Pending_Message_List::Pending_Message_List(): creating object");
#endif
}

Pending_Message_List::Pending_Message_List(Pending_Message_List *pend_msg_list) :
                      Chained_List( (Chained_List *)pend_msg_list )

{
#ifdef _MEM_DEBUG
   DBG_id = DBG_Pending_Message_List_counter;
   DBG_Pending_Message_List_counter++;
   DBG_Pending_Message_List_memory++;
   debug_mem(DBG_id,"Pending_Message_List::Pending_Message_List(Pending_Message_List *): creating object");
#endif
}

Pending_Message_List::~Pending_Message_List()
{
  Pending_Message *pend_msg_aux;

  if (access == LIST_RW)
    while (!is_empty()) {
      pend_msg_aux = get_1st_pend_msg();
      delete pend_msg_aux;
    }
  
#ifdef _MEM_DEBUG
   DBG_Pending_Message_List_memory--;
   debug_mem(DBG_id,"Pending_Message_List::~Pending_Message_List(): destroying object");
#endif
}

int Pending_Message_List::add_tail_pend_msg(Pending_Message *new_pend_msg)
{
  return ( add_tail( (Chained_Element *)new_pend_msg) );
}

int Pending_Message_List::add_head_pend_msg(Pending_Message *new_pend_msg)
{
  return ( add_head( (Chained_Element *)new_pend_msg) );
}

int Pending_Message_List::concat_pend_msg_list(Pending_Message_List *a_pend_msg_list)
{
  Pending_Message *pend_msg;
  
  /* go through all pending messages */
  pend_msg = a_pend_msg_list->get_last_pend_msg();
  while (pend_msg != NULL) {
    /* add this pend msg in the head of this list */
    add_head_pend_msg(pend_msg);
    pend_msg =  a_pend_msg_list->get_last_pend_msg();
  }
  
  /* delete the concatenated list */
  delete a_pend_msg_list;
  return (1);
}

Pending_Message *Pending_Message_List::show_1st_pend_msg()
{
  return ( (Pending_Message *)show_head() );
}

Pending_Message *Pending_Message_List::get_1st_pend_msg()
{
  return ( (Pending_Message *)del_head() );
}

Pending_Message *Pending_Message_List::get_last_pend_msg()
{
  return ( (Pending_Message *)del_tail() );
}

Pending_Message *Pending_Message_List::show_next_pend_msg()
{
  return ( (Pending_Message *)show_next() );
}

int Pending_Message_List::show_no_pend_msg()
{
  return ( show_no_elem() );
}
//------------------------------------------------------------------------------
