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


/*Functions used in ESPA and ESRA methods*/

void LU2(Matrix2 *);
Matrix2 *solve_A_X_B(Matrix2 *, Matrix2 *);
void mul_A_B_op(Matrix2 *,Matrix2 *, Matrix2 *);
Matrix *calculate_A (Matrix *A, double, double);
double	**vpartition(double *, int *);
Matrix2 **mpartition(Matrix *, int *);
Matrix *transpose (Matrix *);
void transpose2(Matrix2	*);
void diff_A_B(Matrix2 *, Matrix2 *);
void reduction(Matrix2	**, int);
double	*mul_A_v(Matrix2 *, double *);
void calc_w(Matrix2 **, double	**);
void calc_z(Matrix2 **, double	**, double **);
void update_w(double **, double	**, double);
void vunpartition(double **, double *);
double **ESPA_dir(Matrix *, int, double, int, double *, int *, int *,double *);
double *ESPA_dir_set(Matrix *, int, double, int, double*, int *,int *, double *);
double **ESPA_itr(Matrix *, info_intervals *, int,int,double *,int *,double *, int, double, double,int *);
double *ESPA_itr_set(Matrix *, info_intervals *, int, int, double *,int *,double *,int ,double, double, int *);
void free_Matrix_2(int,Matrix2 **);
void update_w_1(double	**,double **, double,double,int);
void update_w_set(double **, double **, double, double,double *);
double **ESPA_dir_reward(Matrix	*, int, double, int, double *, int *, int *,double  *,double );
double *ESPA_dir_set_reward(Matrix *, int, double, int, double *, int *, int *,double  *,double);
double **ESPA_itr_reward(Matrix	*, info_intervals *, int ,int,double *,int *,double *,double, int, double ,double ,int *);
double *ESPA_itr_set_reward(Matrix *, info_intervals *, int, int, double *,double  *,int *,double ,int ,double ,double, int *);
int converged_reward(double *, double *, int,int, double );
double *jacobi_reward(Matrix *, double *, double *, double, int *,int, double,int *);
double *gauss_siedel_reward(Matrix *, double *, double *, double, int *,int,double,int *);
double *power_reward(Matrix *, double *, double *,double, int *,int,double,int * );
double *sor_reward(Matrix *, double *, double *, int *, double *, double *, int, double, double,int *);
double *jacobi(Matrix *, double *, double *, double, int *, double,int *);
double *gauss_siedel(Matrix *, double *, double *, double, int *,double,int *);
double *power(Matrix *, double *, double, int *,double,int * );
double *sor_flex(Matrix *, double *, double *, int *, double *, double *, double, double,int *);
Matrix * uniformize_matrix (Matrix *, double *);
double *solve_A_x_b(Matrix2 *, double *, double *);



