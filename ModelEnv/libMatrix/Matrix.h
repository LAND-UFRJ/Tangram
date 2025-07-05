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

#ifndef _MATRIX_H_
#define _MATRIX_H_

/* And define prototypes */
Matrix *initiate_matrix(int);
void put_matrix_position(int, int, double, Matrix*);
void add_matrix_position(int, int, double, Matrix*);
double get_matrix_position(int, int, Matrix*);
void generate_diagonal(char, Matrix*);
int check_diagonal(Matrix*, int);
double *vector_matrix_multiply(double*, Matrix*, double*);
Matrix *uniformize_matrix(Matrix*,double*);
void uniformize_matrix2(Matrix*,double*);
Matrix *set_M(Matrix*);
int matrix_multiply( Matrix *, Matrix *, Matrix * );
double get_uniformization_factor( Matrix * );
Matrix *create_prob_matrix( Matrix *, double );
#endif
