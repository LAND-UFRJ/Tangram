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
*  @file queue_type.cpp
*  @brief <b>Queue methods</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  The detailed description is unavailable. 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tangram_types.h"
#include "chained_list.h"
#include "queue_type.h"

IntegerQueue::IntegerQueue() : Chained_List()
{
    size = 0;
}

IntegerQueue::IntegerQueue( IntegerQueue *queue ) : Chained_List()
{
    QueueElement *elem, *eleaux;

    size = queue->size;
    elem = (QueueElement *) queue->show_head();
    while( elem != NULL )
    {
        eleaux = new QueueElement( elem );
        add_tail( eleaux );
        elem = (QueueElement*) queue->show_next();
    }
}

IntegerQueue::IntegerQueue( int maxSize ) : Chained_List()
{
    size = maxSize;
}

IntegerQueue::~IntegerQueue()
{
    QueueElement *st_aux;

    if( access == LIST_RW )
        while( !is_empty() )
        {
            st_aux = (QueueElement *)del_head();
            delete st_aux;
        }
}

void IntegerQueue::operator =( IntegerQueue &op )
{
    QueueElement *elem, *eleaux;

    /* First empty element list */
    while( !is_empty() )
    {
        eleaux = (QueueElement *)del_head();
        delete eleaux;
    }

    size = op.size;
    elem = (QueueElement *) op.show_head();
    while( elem != NULL )
    {
        eleaux = new QueueElement( elem );
        add_tail( eleaux );
        elem = (QueueElement*) op.show_next();
    }
}

int IntegerQueue::show_size()
{
    return( size );
}

int IntegerQueue::save_at_head( int *vector )
{
    int status;
    QueueElement *st_aux;

    status = 0;
    st_aux = new QueueElement( vector, size );
    if( st_aux != NULL )
        status = add_head( st_aux );

    return( status );
}

int IntegerQueue::save_at_tail( int *vector )
{
    int status;
    QueueElement *st_aux;

    status = 0;
    st_aux = new QueueElement( vector, size );
    if( st_aux != NULL )
        status = add_tail( st_aux );

    return( status );
}

int IntegerQueue::restore_from_head( int *vector )
{
    int status;
    QueueElement *st_aux;

    status = 0;
    st_aux = (QueueElement *)del_head();
    if( st_aux != NULL )
    {
        st_aux->set_vector( vector, size );
        delete st_aux;
        status = 1;
    }

    return( status );
}

int IntegerQueue::restore_from_tail( int *vector )
{
    int status;
    QueueElement *st_aux;

    status = 0;
    st_aux = (QueueElement *)del_tail();
    if( st_aux != NULL )
    {
        st_aux->set_vector( vector, size );
        delete st_aux;
        status = 1;
    }

    return( status );
}

FloatQueue::FloatQueue() : Chained_List()
{
    size = 0;
}

FloatQueue::FloatQueue( FloatQueue *queue ) : Chained_List()
{
    QueueElement *elem, *eleaux;

    size = queue->size;
    elem = (QueueElement *) queue->show_head();
    while( elem != NULL )
    {
        eleaux = new QueueElement( elem );
        add_tail( eleaux );
        elem = (QueueElement*) queue->show_next();
    }
}

FloatQueue::FloatQueue( int maxSize ) : Chained_List()
{
    size = maxSize;
}

FloatQueue::~FloatQueue()
{
    QueueElement *st_aux;

    if( access == LIST_RW )
        while( !is_empty() )
        {
            st_aux = (QueueElement *)del_head();
            delete st_aux;
        }
}

void FloatQueue::operator =( FloatQueue &op )
{
    QueueElement *elem, *eleaux;

    /* First empty element list */
    while( !is_empty() )
    {
        eleaux = (QueueElement *)del_head();
        delete eleaux;
    }

    size = op.size;
    elem = (QueueElement *) op.show_head();
    while( elem != NULL )
    {
        eleaux = new QueueElement( elem );
        add_tail( eleaux );
        elem = (QueueElement*) op.show_next();
    }
}

int FloatQueue::show_size()
{
    return( size );
}

int FloatQueue::save_at_head( TGFLOAT *vector )
{
    int status;
    QueueElement *st_aux;

    status = 0;
    st_aux = new QueueElement( vector, size );
    if( st_aux != NULL )
        status = add_head( st_aux );

    return( status );
}

int FloatQueue::save_at_tail( TGFLOAT *vector )
{
    int status;
    QueueElement *st_aux;

    status = 0;
    st_aux = new QueueElement( vector, size );
    if( st_aux != NULL )
        status = add_tail( st_aux );

    return( status );
}

int FloatQueue::restore_from_head( TGFLOAT *vector )
{
    int status;
    QueueElement *st_aux;

    status = 0;
    st_aux = (QueueElement *)del_head();
    if( st_aux != NULL )
    {
        st_aux->set_vector( vector, size );
        delete st_aux;
        status = 1;
    }

    return( status );
}

int FloatQueue::restore_from_tail( TGFLOAT *vector )
{
    int status;
    QueueElement *st_aux;

    status = 0;
    st_aux = (QueueElement *)del_tail();
    if( st_aux != NULL )
    {
        st_aux->set_vector( vector, size );
        delete st_aux;
        status = 1;
    }

    return( status );
}

/******************************************************************************/

QueueElement::QueueElement( int *vector, int dimension ) : Chained_Element()
{
    data = new int[ dimension ];
    if( data == NULL )
    {
        perror( "Out of memory exception on QueueElement::QueueElement(int *, int)");
        exit( 0 );
    }
    data_size = sizeof( int ) * dimension;
    memcpy( data, vector, data_size );
}

QueueElement::QueueElement( TGFLOAT *vector, int dimension ) : Chained_Element()
{
    data = new TGFLOAT[ dimension ];
    if( data == NULL )
    {
        perror( "Out of memory exception on QueueElement::QueueElement(TGFLOAT *, int)");
        exit( 0 );
    }
    data_size = sizeof( TGFLOAT ) * dimension;
    memcpy( data, vector, data_size );
}

QueueElement::QueueElement(QueueElement *queueelem) : Chained_Element()
{
    if( (queueelem != NULL) && (queueelem->data != NULL) )
    {
        data_size = queueelem->data_size;
        data      = new char [ data_size ];
        if( data == NULL )
        {
            perror("Out of memory exception on QueueElement::QueueElement(QueueElement *)");
            exit( 0 );
        }
        memcpy( data, queueelem->data, data_size );
    }
}

QueueElement::QueueElement()
{
    data_size = 0;
    data = NULL;
}

QueueElement::~QueueElement()
{
    if( data != NULL )
        delete[] (char *)data;
}


void QueueElement::set_vector( int *vector, int dimension )
{
    if( (vector != NULL) && (dimension*sizeof(int) == (unsigned int)data_size) )
        memcpy( vector, data, data_size );
}

void QueueElement::set_vector( TGFLOAT *vector, int dimension )
{
    if( (vector != NULL) && (dimension*sizeof(TGFLOAT) == (unsigned int)data_size) )
        memcpy( vector, data, data_size );
}
