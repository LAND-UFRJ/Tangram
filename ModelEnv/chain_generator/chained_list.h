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
*	@file chained_list.h
*   @brief <b> chained list declaration </b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @version 1.0
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram II project
*   @since version 1.0
*
*/

#ifndef __CHAINED_LIST_H__
#define __CHAINED_LIST_H__

/** @name defines
 *  access permition defines for lists
 */
/*@{*/
/** access permition READ ONLY */
#define  LIST_RO  1
/** access permition READ WRITE */
#define  LIST_RW  2
/*@}*/

typedef class Chained_Element {
    
    Chained_Element *prev;
    Chained_Element *next;

 public:
    friend class Chained_List;

    Chained_Element();    

 protected:
    Chained_Element *get_next();

 private:
    Chained_Element *show_prev();
    void set_prev(Chained_Element *);

    Chained_Element *show_next();
    void set_next(Chained_Element *);

} Chained_Element;


typedef class Chained_List {

    Chained_Element *first_el, *last_el;
    Chained_Element *curr_el;    
    int no_elem;

    int is_first(Chained_Element *);
    int is_last(Chained_Element *);

 protected:
    int access;

 public:
    Chained_List();
    Chained_List(Chained_Element *);
    Chained_List(Chained_List *);

    Chained_Element *show_head();
    Chained_Element *show_tail();
    Chained_Element *show_next();    
    Chained_Element *show_next(Chained_Element *);
    Chained_Element *show_prev();
    Chained_Element *show_prev(Chained_Element *);
    Chained_Element *show_curr();

    int show_no_elem();
    int is_empty();

    int add_head(Chained_Element *);
    int add_tail(Chained_Element *);
    int add_prev(Chained_Element *);
    int add_next(Chained_Element *);

    Chained_Element *del_head();
    Chained_Element *del_tail();
    Chained_Element *del_elem();

    int replace_elem(Chained_Element *);

} Chained_List;



#endif /* __CHAINED_LIST_H__ */


