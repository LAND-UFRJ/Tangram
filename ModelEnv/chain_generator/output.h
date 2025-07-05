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
*  @file output.h
*  @brief <b>output functions and data structures</b>.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  Output functions and data structures for dumping 
*  <modelname>.{states,param,absorb_st,st_trans_prob_mtx} et cetera.
*/

#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#define RATE    0
#define IMPULSE 1

/** structure for file descriptors for rewards */
typedef struct {
    char  name[MAXSTRING];
    FILE *fd;
} t_file_desc;

/* prototypes */
int generate_states_file();
int generate_output_files();

#endif /* __OUTPUT_H__ */
