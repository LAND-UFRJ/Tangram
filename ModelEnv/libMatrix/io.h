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

#ifndef _IO_H_
#define _IO_H_

int get_trans_size(FILE*, int*);
Matrix *get_trans_matrix(FILE*);
Matrix *get_trans_matrix2(FILE*, int);
void put_trans_matrix(Matrix*);
double *get_vector(FILE*);
void put_vector(double*, int);
int *state_remapping(double*, int, int*);
Matrix *get_trans_matrix_remap(FILE*, int*);
void print_matrix( Matrix*, FILE* );
Matrix *get_impulse_matrix(FILE *, int);

#endif
