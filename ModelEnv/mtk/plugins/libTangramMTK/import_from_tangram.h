#ifndef IMPORT_FROM_TANGRAM_H
#define IMPORT_FROM_TANGRAM_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constant values
#define MAX_PATH 1024
#define EMPTY      -1
#define EPSILON  1e-6

/* File extensions */
#define EXT_STATES_TABLE                          ".states"
#define EXT_CURRENT_STATES_PERMUTATION            ".vstat"
#define EXT_TRANSITION_RATES_MATRIX               ".generator_mtx"
#define EXT_SAME_STATE_TRANSITION_RATES_MATRIX    ".trans_same_st"
#define EXT_TRANSITION_PROB_MATRIX                ".st_trans_prob_mtx"
#define EXT_INITIAL_PROBABILITIES                 ".initial_prob"

/* Tangram matrix permutation application */
#define BIN_PERMUTATION_APP            "mtx_vis-st_permut"

/* Error Tolerance */
#define TOLERANCE 1e-5

typedef struct
{
    int    num_hidden_states;           /* number of hidden states */
    char  *hidden_transitions_file;     /* hidden model transition probabilities */
                                        /* file */
    int   *num_internal_states;         /* number of internal state */
                                        /* on each hidden file */
    char **internal_initial_prob_file;  /* internal states initial probabilities */
                                        /* on each hidden state */
    char **internal_transitions_file;   /* internal transition probabilities */
                                        /* for each hidden state */
} HiddenModel;

char        *removeFileNameExtension( char *file_name );
void         printFileOpenError( char mode, char *file_name );
int          getNumberOfLines( FILE *fd );
int          getNumberOfStateVars( char *obj_name );
int          getNumberOfSystemStates( char *obj_name );
void         splitSystemState( int   system_state_size,
                               int  *system_state,
                               int   hidden_state_size,
                               int  *hidden_state,
                               int   internal_state_size,
                               int  *internal_state );
int          searchState( int  *tuple, int  **table, int rows, int cols );
int          searchTuple( char *tuple, char **table, int rows           );
int          isEqual( int *v1, int *v2, int n );
int          isParallel( double *v1, double *v2, int n );
int          isNull( double *v, int n );
int          makePermutation( char  *obj_name,
                              int    hidden_num_st_var,
                              char **hidden_st_var_name,
                              int    internal_num_st_var,
                              char **internal_st_var_name,
                              FILE  *output );
HiddenModel *extractTransitionMatrices( char  *obj_name,
                                        int    hidden_num_st_var,
                                        int    internal_num_st_var,
                                        FILE  *output );
#ifdef __cplusplus
};
#endif

#endif /* IMPORT_FROM_TANGRAM_H */
