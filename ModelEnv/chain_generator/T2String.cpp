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
*	@file T2String.cpp
*   @brief <b> T2String methods </b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @version 1.0
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram II project
*   @since version 1.0
*
*/

#include "T2String.h"


T2String::T2String( const char * str )
{
    // Initialization
    actual_string = NULL;

    // Assignment
    *this = str;
}

T2String::T2String( const T2String & t2str )
{
    // Initialization
    actual_string = NULL;

    // Assignment
    *this = (const char *)t2str;
}

T2String::~T2String( void )
{
    if( actual_string )
        delete actual_string;
}

const T2String & T2String::operator = ( const char * str )
{
    if( actual_string )
    {
        delete actual_string;
        actual_string = NULL;
    }

    if( str )
    {
        actual_string = new char[ strlen( str ) + 1 ];
        strcpy( actual_string, str );
    }

    return *this;
}

const T2String & T2String::operator = ( const T2String & t2str )
{
    return (*this = (const char *)t2str);
}
