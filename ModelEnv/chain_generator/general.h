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
*	@file general.h
*   @brief <b> general declarations </b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @version 1.0
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram II project
*   @since version 1.0
*
*/
#ifndef __GENERAL_H__
#define __GENERAL_H__

#include "tangram_types.h"

/** boolean value define */
/*@{*/
#define TRUE    (1)
#define FALSE   (0)
/*@}*/

/** string defines */ 
/*@{*/
#define MAXSTRING  2048
#define MAXNAME    2048
/*@}*/

/** Infinite for double */
#define INFINITE MAXDOUBLE

/** value to check if probabilities sum around one */
#define EPSILON    (1e-02)

/** defines for rate reward options */
/*@{*/
#define RATE_MEAN   1
#define RATE_MAX    2
#define RATE_MIN    3
#define RATE_ERROR  4
/*@}*/

/* *defines for reward trace files */
/*@{*/
#define NO_TRACE    0
#define TRACE       1
/*@}*/

/** defines for the both types of Expr_Val */
/*@{*/
#define   NUMERICAL  1
#define   LITERAL    2
/*@}*/

/** maximum number of State variables in a vector */
#define    MAX_ST_VECTOR   255

/** maximum expression size */
#define MAXEXPRSIZE 2048

/** maximum number of objects in a indepedent chain */
#define MAXINDEPCHAIN 100

/** message types */
/*@{*/
#define NUMBER    0
#define INT_VEC   1
#define FLOAT_VEC 2
/*@}*/


/** struct t_map to renumber states */
typedef struct {
    unsigned int from;  /**< old state number */
    unsigned int to;    /**< new state number */
} t_map;

/** context flags for ev_msg_type */
/*@{*/
#define   CONTEXT_EV_TYPE      1
#define   CONTEXT_MSG_TYPE     2
#define   USER_CODE_MAX_TIME  45   /**< in seconds */
/*@}*/

/** the execution context data structure */
typedef struct {
    char obj_name[MAXSTRING];
    char ev_msg_name[MAXSTRING];
    int  ev_msg_type;
    int  act_num;
    int  user_code;
} t_context;


#ifdef _MEM_DEBUG
void   debug_mem(int, const char *);
void   debug_mem_print();
#endif

void   debug(int, const char *, ...);
TGFLOAT function_handler (int, Object_State *, Simulator *, System_State *);

void   alarm_handler(int);
void   segv_handler(int);
int    install_handlers();
void   reset_context();
void   print_context();

#endif   /*  __GENERAL_H__ */


