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
* @file string_list.h
* @brief <b>classes for manipulating string and integer lists</b>.
* @remarks Lot of.
* @author LAND/UFRJ
* @date 1999-2009
* @warning Do not modify this class before knowing the whole Tangram II project
* @since version 1.0
*
*/

#ifndef __STRING_LIST_H__
#define __STRING_LIST_H__

typedef class String : public Chained_Element {
    
    char str[MAXSTRING];

 public:
    String(const char *);
    ~String();

    void show_str(char *);
    int  compare_str(const char *);
    int  contains_str(char *);
    
    void print_str();
    void print_str(FILE *);

} String;


typedef class String_List : public Chained_List {

 public:
    String_List();
    String_List(String_List *);
    ~String_List();

    int     add_str(const char *);
    int     add_tail_str(String *);
    String *show_1st_str();
    String *get_1st_str();
    String *show_next_str();
    String *show_curr_str();
    int     query_str(char *);
    int     contains_str(char *);
    int     show_no_str();

} String_List;



typedef class Integer : public Chained_Element {
    
    int number;

 public:
    Integer(int);
    ~Integer();
    
    int  show_int();
    void ch_int(int);
    int  compare_int(int);
    
    void print_int();
    void print_int(FILE *);

} Integer;


typedef class Integer_List : public Chained_List {

 public:
    Integer_List();
    Integer_List(Integer_List *);
    ~Integer_List();

    int     add_tail_int(Integer *);
    Integer *show_1st_int();
    Integer *get_1st_int();
    Integer *show_next_int();
    Integer *show_curr_int();
    int     query_int(int);
    int     show_no_int();

} Integer_List;


#endif /* __STRING_LIST_H__ */
