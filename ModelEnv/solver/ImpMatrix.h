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


#ifndef __IMP_MATRIX_H__
#define __IMP_MATRIX_H__


/************************************************************************/
/*     Impulse Matrix definition                                        */                                  
/************************************************************************/

typedef struct impulse {
        double           value;
        double           prob;
        struct impulse  *next;
} Impulse;

typedef struct imp_elem {
        int               index;
        Impulse          *first_imp;
        struct imp_elem  *next;
} ImpElem;

typedef struct imp_column {
        ImpElem   *first_elem;
} ImpCol;

typedef struct imp_matrix {
        ImpCol    *imp_col_vector;
        int        dimension;
} ImpMatrix;


ImpMatrix*   init_ImpMatrix (int);

int          put_impulse (ImpMatrix*, int, int, double, double);

Impulse*     get_impulse (ImpMatrix*, int, int);

void         print_ImpMatrix (ImpMatrix*);

#endif /* __IMP_MATRIX_H__ */
