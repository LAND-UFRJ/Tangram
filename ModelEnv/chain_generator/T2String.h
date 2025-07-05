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
* @file T2String.h
* @brief <b>classes for Tangram2 string type</b>.
* @remarks Lot of.
* @author LAND/UFRJ
* @date 1999-2009
* @warning Do not modify this class before knowing the whole Tangram II project
* @since version 1.0
*
*/

#ifndef __T2STRING_H__
#define __T2STRING_H__

#include <string.h>
#include <stdlib.h>

class T2String
{
private:
    char * actual_string;
public:
    /** Construction & Deconstruction */
    /**@{*/
    T2String ( const char * = NULL );
    T2String ( const T2String & );
    ~T2String( void );
    /**@}*/
    
    /** General purpose access methods */
    /**@{*/
    inline     operator const char * ( void ) const
    {
        return( actual_string );
    }
    inline int length( void ) const
    {
        if( actual_string )
            return strlen( actual_string );
        return 0;
    }
    /**@}*/
    
    /** Assignment methods */
    /**@{*/
    const T2String & operator = ( const char * );
    const T2String & operator = ( const T2String & );
    /**@}*/    
};

#endif /* __T2STRING_H__ */
