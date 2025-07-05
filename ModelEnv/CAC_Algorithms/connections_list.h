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

#ifndef __CONNECTION_LIST_H__
#define __CONNECTION_LIST_H__

#ifndef __UINT__
#define __UINT__
typedef unsigned int uint;
#endif

class Connection: public Chained_Element 
{
    uint             id;
    double           efec_capacity;
    double           mean_rate;
    double           variance_rate;
    
 public:
    Connection(uint, double, double, double );
    ~Connection();
    
    int compare_connec( uint );
    double show_capacity();
    double show_mean();
    double show_variance();

    void print();
} ;


class Connection_List : public Chained_List 
{
  
public:
    Connection_List();
    ~Connection_List();
    
    int        add_connec( uint, double, double, double );
    int        add_tail_connec( Connection * );
    Connection *show_1st_connec();
    Connection *get_1st_connec();
    Connection *show_next_connec();
    Connection *show_curr_connec();
    Connection *del_elem_connec();
    int        query_connec( uint );
    int        show_no_connec(); 
    
};
#endif
