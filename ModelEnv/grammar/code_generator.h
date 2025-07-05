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

//------------------------------------------------------------------------------
//        T A N G R A M - I I      << . >>      L A N D - U F R J - B R A Z I L
//------------------------------------------------------------------------------
#ifndef __CODE_GENERATOR_H__
#define __CODE_GENERATOR_H__

void write_header( FILE * );
void write_mtk_header( FILE * );
void write_mtk_setup( FILE * );
void write_action_code_alarm_handler( FILE *fd );
void write_function_handler( FILE * );
void write_func_header( FILE *, char *, char * );
void write_func_header_4_act_cond( FILE *, char *, char *);
void write_reward_function( FILE *, int );
void write_decl_init_part( FILE *, int, int );
void write_distrib_functions( FILE *, int, int );
int  code_generator( char * );
int  dump_tables( char * );
int  initialize_max_values( char * );
int  reorder_objects_and_variables( char * );
int  check_object_initialization( void );
int  check_object_name( void );
int  is_object( char * );
int  generate_vstat( char * );
int  generate_events( char * );

#endif  /*  __CODE_GENERATOR_H__ */
//------------------------------------------------------------------------------


