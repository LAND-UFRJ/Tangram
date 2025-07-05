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
*  @file queue_type.h
*  @brief <b>IntegerQueue, FloatQueue, and QueueElement methos </b>.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  The detailed description is unavailable. 
*/

#ifndef __QUEUE_TYPE_H__
#define __QUEUE_TYPE_H__

class IntegerQueue : public Chained_List
{
    int size;
  public:
    int show_size();
    void operator =( IntegerQueue & );
    IntegerQueue();
    IntegerQueue( int );
    IntegerQueue( IntegerQueue *queue );
    ~IntegerQueue();
    int save_at_head( int *vector );
    int save_at_tail( int *vector );
    int restore_from_head( int *vector );
    int restore_from_tail( int *vector );
};

class FloatQueue : public Chained_List
{
    int size;
  public:
    int show_size();
    void operator =( FloatQueue & );
    FloatQueue();
    FloatQueue( int );
    FloatQueue( FloatQueue *queue );
    ~FloatQueue();
    int save_at_head( TGFLOAT *vector );
    int save_at_tail( TGFLOAT *vector );
    int restore_from_head( TGFLOAT *vector );
    int restore_from_tail( TGFLOAT *vector );
};

class QueueElement : public Chained_Element
{
    void *data;
    int data_size;

  public:
    QueueElement();
    QueueElement( int *,     int dimension );
    QueueElement( TGFLOAT *, int dimension );
    QueueElement( QueueElement *queue );
    ~QueueElement();
    void set_vector( int     *vector, int dimension );
    void set_vector( TGFLOAT *vector, int dimension );
};
#endif
