#ifndef MATRIX_H
#define MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

double * copy_vector              ( unsigned int, double * );
double * resize_vector            ( unsigned int, double *, unsigned int );
void     destroy_vector           ( unsigned int, double * );
void     normalize_vector         ( unsigned int, double * );
int      is_vector_stochastic     ( unsigned int, double * );

double ** resize_matrix           ( unsigned int, unsigned int, double **,
                                    unsigned int, unsigned int );
double ** copy_matrix             ( unsigned int, unsigned int, double ** );
void      destroy_matrix          ( unsigned int, unsigned int, double ** );
void      normalize_matrix        ( unsigned int, unsigned int, double ** );
int       is_matrix_stochastic    ( unsigned int, unsigned int, double ** );

double * create_zero_vector       ( unsigned int );
double * create_full_vector       ( unsigned int );
double * create_probability_vector( unsigned int );

double ** create_zero_matrix      ( unsigned int, unsigned int );
double ** create_identity_matrix  ( unsigned int );
double ** create_full_matrix      ( unsigned int, unsigned int );
double ** create_qbd_matrix       ( unsigned int, unsigned int,
                                    unsigned int, unsigned int  );
double ** create_coxian_matrix    ( unsigned int );

double *** create_zero_3d_matrix  ( unsigned int, unsigned int, unsigned int );
double *** create_full_3d_matrix  ( unsigned int, unsigned int, unsigned int );
void       destroy_3d_matrix      ( unsigned int, unsigned int, unsigned int, double *** );
double *** copy_3d_matrix         ( unsigned int, unsigned int, unsigned int, double *** );
double *** resize_3d_matrix       ( unsigned int, unsigned int, unsigned int, double ***,
                                    unsigned int, unsigned int, unsigned int );

#ifdef __cplusplus
};
#endif

#endif /* MATRIX_H */
