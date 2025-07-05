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
*	@file string_list.cpp
*   @brief <b> String, String_List, Integer, and Integer_List methods </b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @version 2.0
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram II project
*   @since version 1.0
*
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

String::String(const char *a_str) : Chained_Element()
{
   if (a_str != NULL)
     strcpy (str, a_str);
   else
     strcpy (str, "");

   debug(4,"String::String(): creating object");
}

String::~String()
{
  /* do nothing */
   debug(4,"String::~String(): destroying object");
}

void String::show_str(char *a_str)
{
   strcpy (a_str, str);
}

int String::compare_str( const char *a_str)
{
   if (!strcmp(str, a_str))
     return (1);
   else
     return (0);
}

int String::contains_str(char *a_str)
{
   if (strstr(str, a_str) != NULL)
     return (1);
   else
     return (0);
}

void String::print_str()
{
   fprintf( stdout, "%s", str);
}

void String::print_str(FILE *fd)
{
   fprintf (fd, "%s", str);
}


/**********************************************************************************/

String_List::String_List() : Chained_List()
{
   debug(4,"String_List::String_List(): creating object");
}

String_List::String_List(String_List *str_list) : Chained_List( (Chained_List *)str_list)
{
   debug(4,"String_List::String_List(String_List *): creating object");
}

String_List::~String_List()
{
  String *str_aux;

  if (access == LIST_RW) {
    str_aux = get_1st_str();
    while (str_aux != NULL) {
      delete str_aux;
      str_aux = get_1st_str();
    }
  }
  debug(4,"String_List::~String_List(): destroying object");
}

int String_List::add_str(const char *new_str)
{
    String  *str_aux;
    int     i = 0;

    str_aux = show_1st_str();
    while (str_aux != NULL) {
      /* if the string exists return its position in the list */
      if (str_aux->compare_str(new_str))
	return (i);
      str_aux = show_next_str();
      i++;
    }
    /* if the string doesn't exist create a new string and */
    /* add it to the end of the list. Return the position of the new string */
    str_aux = new String(new_str);
    add_tail_str(str_aux);
    return (i);
}

int String_List::add_tail_str(String *new_str)
{
  return ( add_tail( (Chained_Element *)new_str) );
}

String *String_List::show_1st_str()
{
  return ( (String *)show_head() );
}

String *String_List::get_1st_str()
{
  return ( (String *)del_head() );
}

String *String_List::show_next_str()
{
  return ( (String *)show_next() );
}

String *String_List::show_curr_str()
{
  return ( (String *)show_curr() );
}

int String_List::query_str(char *tg_str_name)
{
    String  *str_aux;

    str_aux = show_1st_str();
    while (str_aux != NULL) {
      if (str_aux->compare_str(tg_str_name))
	return (1);
      str_aux = show_next_str();
    }
    return (0);
}

int String_List::contains_str(char *tg_str)
{
    String  *str_aux;

    str_aux = show_1st_str();
    while (str_aux != NULL) {
      if (str_aux->contains_str(tg_str))
	return (1);
      str_aux = show_next_str();
    }
    return (0);
}

int String_List::show_no_str()
{
  return ( show_no_elem() );
}


/**********************************************************************/

Integer::Integer(int a_num) : Chained_Element()
{
  number = a_num;

  debug(4,"Integer::Integer(): creating object");
}

Integer::~Integer()
{
  /* do nothing */
   debug(4,"Integer::~Integer(): destroying object");
}

int Integer::show_int()
{
   return(number);
}

void Integer::ch_int(int a_num)
{
  number = a_num;
}

int Integer::compare_int(int a_num)
{
   if (number == a_num)
     return (1);
   else
     return (0);
}

void Integer::print_int()
{
   fprintf( stdout, "%d", number);
}

void Integer::print_int(FILE *fd)
{
   fprintf (fd, "%d", number);
}


/**********************************************************************************/

Integer_List::Integer_List() : Chained_List()
{
   debug(4,"Integer_List::Integer_List(): creating object");
}

Integer_List::Integer_List(Integer_List *int_list) : Chained_List( (Chained_List *)int_list)
{
   debug(4,"Integer_List::Integer_List(Integer_List *): creating object");
}

Integer_List::~Integer_List()
{
  Integer *int_aux;

  if (access == LIST_RW) {
    int_aux = get_1st_int();
    while (int_aux != NULL) {
      delete int_aux;
      int_aux = get_1st_int();
    }
  }
  debug(4,"Integer_List::~Integer_List(): destroying object");
}

int Integer_List::add_tail_int(Integer *new_int)
{
  return ( add_tail( (Chained_Element *)new_int) );
}

Integer *Integer_List::show_1st_int()
{
  return ( (Integer *)show_head() );
}

Integer *Integer_List::get_1st_int()
{
  return ( (Integer *)del_head() );
}

Integer *Integer_List::show_next_int()
{
  return ( (Integer *)show_next() );
}

Integer *Integer_List::show_curr_int()
{
  return ( (Integer *)show_curr() );
}

int Integer_List::query_int(int tg_int)
{
    Integer  *int_aux;
    int       count = 1;

    int_aux = show_1st_int();
    while (int_aux != NULL) {
      if (int_aux->compare_int(tg_int))
	return (count);
      count ++;
      int_aux = show_next_int();
    }
    return (-1);
}

int Integer_List::show_no_int()
{
  return ( show_no_elem() );
}



