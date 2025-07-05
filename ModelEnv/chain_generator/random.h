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
*  @brief <b>Random_obj class </b>.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  Base Class for random generation methods
*/


#ifndef __RANDOM_H__
#define __RANDOM_H__

/** max signed integer (32bit) */
#define    NUMMAX      2147483647

typedef class Random_obj {
    protected:
        unsigned short seed[3];     /**< seed passed as parameter         */
        unsigned short lastx[3];    /**< last value returned by nrand48() */

    public:
        Random_obj();
        Random_obj( unsigned short seed_value[3] );
       ~Random_obj();

       void get_seed( unsigned short seed_value[3] );

       TGFLOAT next_randval();  /**< generates a U[0,1] */

} Random_obj;

#endif  /* __RANDOM_H__ */
