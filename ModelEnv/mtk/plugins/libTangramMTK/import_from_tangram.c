#include "import_from_tangram.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "sparseMatrix.h"

/**
 * Returns the filename without extension
 */
char *removeFileNameExtension( char *file_name )
{
    register int  i;
    int           n;
    char         *prefix;
    
    n      = strlen( file_name );
    prefix = (char *)malloc( n * sizeof( char ) );
    i      = n - 1;
    
    /* Find '.' and ignore characters */
    while( ( i >= 0 ) && ( file_name[i] != '.' ) )
        i--;
    
    /* Copy missing other characters */
    prefix[i--] = '\0';
    while( i >= 0 )
    {
        prefix[i] = file_name[i];
        i--;
    }
    
    return prefix;
}

/**
 * Prints an error
 * 'mode' -> r = read, w = write
 */
void printFileOpenError( char mode, char *file_name )
{
    char error_message[ MAX_PATH ];
    
    switch( mode )
    {
        case 'r':
            sprintf( error_message, "Trying to open file %s", file_name );
        break;

        case 'w':
            sprintf( error_message, "Cannot write to file %s", file_name );
        break;        
    }

    perror( error_message );     
}

/**
 * Return the total number of lines of file 'fd'
 * Note 'fd' must have being already opened
 */
int getNumberOfLines( FILE *fd )
{
    int  num_lines;
    char line[ MAX_PATH ];
    
    num_lines = 0;
    
    while( fgets( line, MAX_PATH, fd ) )
        num_lines++;
    
    return num_lines;
}

/**
 * Return the total number of state variables
 */
int getNumberOfStateVars( char *obj_name )
{
    char  file_name[ MAX_PATH ];
    FILE *fd;
    int   num_st_vars;
    
    sprintf( file_name, "%s%s", obj_name, EXT_CURRENT_STATES_PERMUTATION );
    
    fd = fopen( file_name, "r" );
    if( fd )
    {
        num_st_vars = getNumberOfLines( fd );
        fclose( fd );
    }
    else
    {
        printFileOpenError( 'r', file_name );

        return -1;
    }
    
    return num_st_vars;
}

/**
 * Return the total number of system states
 */
int getNumberOfSystemStates( char *obj_name )
{
    char  file_name[ MAX_PATH ];
    FILE *fd;
    int   num_states;
    
    sprintf( file_name, "%s%s", obj_name, EXT_STATES_TABLE );
    
    fd = fopen( file_name, "r" );
    if( fd )
    {
        num_states = getNumberOfLines( fd );
        fclose( fd );
    }
    else
    {
        printFileOpenError( 'r', file_name );

        return -1;
    }
    
    return num_states;
}

/**
 * Perform state variable permutation, putting 'hidden_st_var_name'
 * components in the first tuple positions and 'internal_st_var_name' just after
 * all in the same order they are specified
 * Returns true if permutation is performed with success
 *
 * Warning:
 * The output file descriptor is not currently in use, but it was
 * left here in case something other than a error message needs
 * to, in the future, be printed.
 */
int makePermutation( char  *obj_name,
                     int    hidden_num_st_var,
                     char **hidden_st_var_name,
                     int    internal_num_st_var,
                     char **internal_st_var_name,
                     FILE  *output )
{
    FILE         *fd;
    register int  i, j, pos = 0;
    int          *hidden_st_var_position,
                 *internal_st_var_position,    
                 *permutation,
                  st_var_found,
                  error;
    char          file_name[           MAX_PATH ],
                  current_st_var[      MAX_PATH ],
                  permutation_string[  MAX_PATH ],
                  permutation_command[ MAX_PATH ];
    
    hidden_st_var_position   = (int *)malloc( hidden_num_st_var *
                                              sizeof( int ) );
    internal_st_var_position = (int *)malloc( internal_num_st_var *
                                              sizeof( int ) );

    /********* Query hidden state variables *********/
    sprintf( file_name, "%s%s", obj_name, EXT_CURRENT_STATES_PERMUTATION );
    
    fd = fopen( file_name, "r" );
    if( fd )
    {
        for( i = 0; i < hidden_num_st_var; i++ )
        {
            pos          = 0;
            st_var_found = 0;
            while( fscanf( fd, "%s\n", current_st_var ) == 1 )
            {
                /* State var found */
                if( !strcmp( current_st_var, hidden_st_var_name[i] ) )
                {
                    hidden_st_var_position[i] = pos;
                    st_var_found              = 1;
                }
                pos++;
            }

            if( !st_var_found )
            {
                fprintf( stderr,
                         "State variable %s not found in the model!\n"
                         "Aborting...\n", hidden_st_var_name[i] );
                return 0;
            }

            rewind( fd );
        }

        fclose( fd );
    }
    else
    {
        printFileOpenError( 'r', file_name );

        return 0;
    }

    /*********** Query internal state variables *********/
    fd = fopen( file_name, "r" );
    if( fd )
    {
        for( i = 0; i < internal_num_st_var; i++ )
        {
            pos          = 0;
            st_var_found = 0;
            while( fscanf( fd, "%s\n", current_st_var ) == 1 )
            {
                /* State var found */
                if( !strcmp( current_st_var, internal_st_var_name[i] ) )
                {
                    internal_st_var_position[i] = pos;
                    st_var_found                = 1;
                }
                pos++;                
            }
            
            if( !st_var_found )
            {
                fprintf( stderr,
                         "State variable %s not found in the model!\n"
                         "Aborting...\n", internal_st_var_name[i] );
                return 0;
            }
            
            rewind( fd );
        }

        fclose( fd );
    }
    else
    {
        printFileOpenError( 'r', file_name );

        return 0;
    }

    /********* Compose permutation string *********/
    /* pos == total number of state variables */
    permutation = (int *)malloc( pos * sizeof( int ) );
    memset( permutation, EMPTY,  pos * sizeof( int ) );
    
    /* Hidden state variables first */
    j = 0;
    for( i = 0; i < hidden_num_st_var; i++ )
        permutation[ hidden_st_var_position[i] ]   = j++;

    /* Internal state variables second */
    for( i = 0; i < internal_num_st_var; i++ )
        permutation[ internal_st_var_position[i] ] = j++;

    /* Keep other state variables' precedence */
    i = 0;
    while( j < pos )
    {
        if( permutation[i] == EMPTY )
            permutation[i] = j++;

        i++;
    }
    
    permutation_string[0] = '\0';
    for( i = 0; i < pos; i++ )    
        sprintf( permutation_string, "%s %d",
                 permutation_string,
                 permutation[i] );

    /********* Perform permutation *********/
    strcpy( permutation_command, BIN_PERMUTATION_APP );
    strcat( permutation_command, " -fn "             );
    strcat( permutation_command, obj_name            );
    strcat( permutation_command, " -ps "             );
    strcat( permutation_command, permutation_string  );        
    strcat( permutation_command, " -gf -ng"          );
    strcat( permutation_command, "> /dev/null 2>&1 " );
    
    error = system( permutation_command );
    if( error == -1 )
    {
    	fprintf( stderr, "Error in system( %s ) function.\n ", permutation_command );
        return 0;
    }

    /* Release memory */
    if( hidden_st_var_position )
        free( hidden_st_var_position );
    if( internal_st_var_position )
        free( internal_st_var_position );
    if( permutation )
        free( permutation );
    
    return 1;
}

/**
 * Splits a system state into hidden and internal tuples and
 * stores the remaining tuple on 'remaining_variables'
 */
void splitSystemState( int   system_state_size,
                       int  *system_state,
                       int   hidden_state_size,
                       int  *hidden_state,
                       int   internal_state_size,
                       int  *internal_state )
{
    register int i, j;
    
    j = 0;
    
    for( i = 0; i < hidden_state_size; i++ )
        hidden_state[i]   = system_state[j++];

    for( i = 0; i < internal_state_size; i++ )
        internal_state[i] = system_state[j++];
}

/**
 * Returns the 'tuple' row position in 'table'
 */
int searchState( int *tuple, int **table, int rows, int cols )
{
    register int i, j;
    int          match;
    
    for( i = 0; i < rows; i++ )
    {
        match = 1;    

        for( j = 0; j < cols; j++ )
        {
            /* one element differs, quit this row */
            if( tuple[j] != table[i][j] )
            {
                match = 0;
                break;
            }            
        }
        
        /* tuple found on line i */
        if( match )
            return i;
    }
    
    /* tuple not found */
    return -1;
}

/**
 * Returns the 'tuple' row position in 'table'
 */
int searchTuple( char *tuple, char **table, int rows )
{
    register int i;
    
    for( i = 0; i < rows; i++ )
    {
        /* tuple found on line i */
        if( !strcmp( tuple, table[i] ) )
            return i;
    }
    
    /* tuple not found */
    return -1;
}

/**
 * Returns true if all elements of v1 and v2 are equal
 */
int isEqual( int *v1, int *v2, int n )
{
    register int i;
    
    for( i = 0; i < n; i++ )
        if( v1[i] != v2[i] )
            return 0;

    return 1;
}                    

/**
 * Returns true if vectors v1 and v2 are parallel
 */
int isParallel( double *v1, double *v2, int n )
{
    register int i;
    double       scale;
    
    scale = -1.0; /* empty scale */

    for( i = 0; i < n; i++ )
    {
        /* One element is null */
        if( ( v1[i] < EPSILON ) || ( v2[i] < EPSILON ) )
        {
            /* If one of them is not null, vectors are not parallel */
            /* Otherwise, the scale is undefined */
            if( ( fabs( v1[i] - v2[i] ) > EPSILON ) )
                return 0;
        }
        /* No null element */
        else
        {
            /* First scale read */
            if( scale == -1.0 )
                scale = v1[i] / v2[i];
            else
            {
                /* Out of scale element, vectors are not parallel */
                if( fabs( ( v1[i] / v2[i] ) - scale ) > EPSILON )
                    return 0;
            }
        }
    }

    return 1;
}                

/**
 * Returns true if all vector elements are null
 */
int isNull( double *v, int n )
{
    register int i;
    
    for( i = 0; i < n; i++ )
        if( v[i] > EPSILON )
            return 0;
    return 1;
}

/**
 * Extract transition rate matrices from hidden and internal models from
 * the extended one, and writes them to files
 */
HiddenModel *extractTransitionMatrices( char  *obj_name, /* model file name
                                                            without extension */
                                        int    hidden_num_st_var, /* number of
                                                           state variables from
                                                           the hidden model */
                                        int    internal_num_st_var,  /* number
                                                           of state variables
                                                           from the internal
                                                           model */
                                        FILE  *output )  /* output file
                                                           descriptor */

{
    HiddenModel   *hidden_model; /* will be returned */
    char           system_states_file[                 MAX_PATH ],
                   system_transitions_file[            MAX_PATH ],
                   system_same_state_transitions_file[ MAX_PATH ],
                   system_states_tuple[                MAX_PATH ],
                  *system_state_str;
    FILE          *fd;
    register int   i, j, k, l;
    double sum;

    /********* System state variables *********/
    int        system_state, num_system_states,                                  
               st_var,       num_st_vars,                                        
             **system_states_table, /* stores all system states tuples */
                                    /* dimension: num_system_states x  */
                                    /*            num_st_vars          */

               system_state_a, system_state_b; /* system state id's, read         */
                                               /* from transition matrix          */

    double     system_transition_rate, /* transition rate between              */
                                       /* system_state_a and system_state_b    */
                                       /* read from Tangram2 transition matrix */

               hidden_transition,   /* transition rate between hidden states */

               internal_transition; /* transition rate between internal states */
                                           
    /********* Hidden state variables *********/                 
    int        num_hidden_states,                                             
             **hidden_states_table, /* stores hidden states tuples    */
                                    /* dimension: num_hidden_states x */
                                    /*            hidden_num_st_var   */

              *hidden_state_a, *hidden_state_b, /* hidden state tuples of       */
                                                /* a system state               */
                                                /* dimension: hidden_num_st_var */

               hidden_state_pos_a, hidden_state_pos_b, /* position in         */
                                                       /* hidden_states_table */

               hidden_state_found;  /* true if a new hidden state */
                                    /* has been just found        */

    #define    last_hidden_state ( num_hidden_states - 1 )                    

    char      *hidden_transitions_file; /* hidden model transitions rates */
                                        /* matrix output file             */

    double ****hidden_transition_rates_extended, /* [i][j][k][l] -> transition */
                                                 /* from hidden state i        */
                                                 /* (internal state k) to      */
                                                 /* hidden state j             */
                                                 /* (internal state l)         */
                                                 /* dimension:                 */
                                                 /* num_hidden_states      x   */
                                                 /* num_hidden_states      x   */
                                                 /* num_internal_states[i] x   */
                                                 /* num_internal_states[j]     */

              *hidden_transition_recurrent_rates, /* stores recurrent transition  */
                                                  /* values for each hidden state */

             **hidden_transition_rates, /* [i][j] -> hidden state i to    */
                                        /* hidden state j                 */
                                        /* dimension: num_hidden_states x */
                                        /*            num_hidden_states   */



             **hidden_transition_probs; /* [i][j] -> hidden state i to    */
                                        /* hidden state j                 */
                                        /* dimension: num_hidden_states x */
                                        /*            num_hidden_states   */

    /* Restriction: all rows from a hidden_transition_rates_extended[*][j] */
    /* matrix must be parallel, since when entering a new hidden state, */
    /* there is no dependence on the old internal state. In other words, */
    /* when leaving a hidden state, no matter in which internal state the model */
    /* is, the initial distribution of the internal states in the new hidden */
    /* state must be the same */
    
    /* OBS: The above restriction is easier to visualize if the */
    /* hidden_transition_rates_extended matrix is thought of as a 2x2 */
    /* matrix, in which every row and column is composed of the combination */
    /* of Tangram2 state variables. */
    
    /* Note: an entry hidden_transition_rates[i][j] is equal to the sum of */
    /* an arbitrary non-null row from hidden_transition_rates_extended[i][j] */
    
    /********* Internal state variables *********/
    int       *num_internal_states, /* [i] -> how many internal states */
                                    /* on hidden state i               */
                                    /* dimension: num_hidden_states    */

            ***internal_states_table, /* [i][j] -> stores internal state j   */
                                      /* tuples from hidden state i          */
                                      /* dimension: num_hidden_states x      */
                                      /*            num_internal_states[i] x */
                                      /*            internal_num_st_var      */

              *internal_state_a, *internal_state_b, /* internal state tuples */
                                                    /* of a system state     */
                                                    /* dimension:            */
                                                    /* internal_num_st_var   */

               internal_state_pos_a, internal_state_pos_b; /* position in */
                                                           /* internal_states_table[i] */

    #define    last_internal_state ( num_internal_states[ last_hidden_state ] - 1 )

    char     **internal_transitions_file,  /* [i] -> internal model related to */
                                           /* hidden state i transition rates  */
                                           /* matrix output file               */

             **internal_initial_prob_file; /* [i] -> internal model related to */
                                           /* hidden state i initial state     */
                                           /* probabilities                    */

    double  ***internal_transition_rates,  /* [i][j][k] -> internal state j     */
                                           /* to internal state k               */
                                           /* inside hidden state i             */
                                           /* dimension: num_hidden_states      */
                                           /*          x num_internal_states[i] */
                                           /*          x num_internal_states[i] */

            ***internal_transition_probs,  /* [i][j][k] -> internal state j     */
                                           /* to internal state k               */
                                           /* inside hidden state i             */
                                           /* dimension: num_hidden_states      */
                                           /*          x num_internal_states[i] */
                                           /*          x num_internal_states[i] */

            ***internal_transition_rates_extended,/* [i][j][k] ->             */
                                                  /* inside hidden state i    */
                                                  /* transition from          */
                                                  /* internal state j         */
                                                  /* to                       */
                                                  /* internal state k         */
                                                  /* dimension:               */
                                                  /* num_hidden_states      x */
                                                  /* num_internal_states[i] x */
                                                  /* num_internal_states[i] x */

             **internal_transition_recurrent_rates, /* stores recurrent transition    */
                                                    /* values for each internal state */

             **internal_initial_prob; /* [i][j] -> initial probability of      */
                                      /* internal state j when entering hidden */
                                      /* state i                               */
                                      /* dimension: num_hidden_states          */
                                      /*          x num_internal_states[i]     */


/********************* Execution Code Begins Here ************************/
    
    num_system_states = getNumberOfSystemStates( obj_name );
    num_st_vars       = getNumberOfStateVars(    obj_name );
    system_state      = -1;
   
    /********* Read states table *********/
    sprintf( system_states_file, "%s%s", obj_name, EXT_STATES_TABLE );
    
    system_states_table = (int **)malloc( num_system_states * sizeof( int * ) );
    
    fd = fopen( system_states_file, "r" );
    if( fd )
    {
        /* While new states are found, store them into states_table */
        while( fscanf( fd, "%*d ( %s", system_states_tuple ) == 1 )
        {
            /* Remove last ')' */
            system_states_tuple[ strlen( system_states_tuple ) - 1 ] = '\0';

            system_state++;
            st_var = 0;

            /* Allocate memory for this system state */
            system_states_table[ system_state ] = (int *)malloc( num_st_vars 
                                                              * sizeof( int ) );
            /* Tokenize states from tuple */
            sscanf( strtok( system_states_tuple, "," ), "%d",
                    &system_states_table[system_state][st_var++] );
            while( 1 )
            {
                system_state_str = strtok( NULL, "," );

                if( system_state_str )
                    sscanf( system_state_str, "%d",
                            &system_states_table[system_state][st_var++] );
                else
                    break;
            }
        }

        fclose( fd );
    }
    else
    {
        printFileOpenError( 'r', system_states_file );

        return NULL;         
    }

    /********* Discover hidden states amount and             *********/
    /********* how many internal states on each one          *********/
    /* Allocate structures */
    hidden_state_a   = (int *)malloc( hidden_num_st_var   * sizeof( int ) );
    hidden_state_b   = (int *)malloc( hidden_num_st_var   * sizeof( int ) );
    internal_state_a = (int *)malloc( internal_num_st_var * sizeof( int ) );
    internal_state_b = (int *)malloc( internal_num_st_var * sizeof( int ) );
    
    /* Read first system state, i.e., 1st hidden and its 1st internal state */
    splitSystemState( num_st_vars,
                      system_states_table[0],
                      hidden_num_st_var,
                      hidden_state_a,
                      internal_num_st_var,
                      internal_state_a );

    /* Create new hidden state */
    num_hidden_states    = 1;
    hidden_state_found   = 0;

    hidden_states_table  = (int **)malloc( num_hidden_states
                                         * sizeof( int * ) );
                                         
    hidden_states_table[ last_hidden_state ] = (int *)malloc( hidden_num_st_var
                                                            * sizeof( int ) );
    /* Store new hidden state */
    memcpy( hidden_states_table[ last_hidden_state ],
            hidden_state_a,
            hidden_num_st_var * sizeof( int ) );

    /* Create new internal state */
    num_internal_states = (int *)malloc( num_hidden_states * sizeof( int ) );
    num_internal_states[ last_hidden_state ] = 1;
    
    /* Internal states table */
    internal_states_table = (int ***)malloc( num_hidden_states 
                                           * sizeof( int ** ) );
                                           
    internal_states_table[ last_hidden_state ] = (int**)malloc( 
                                    num_internal_states[ last_hidden_state ]
                                    * sizeof( int * ) );
                                    
    internal_states_table[ last_hidden_state ][ last_internal_state ] =
        (int *)malloc( internal_num_st_var * sizeof( int ) );

    /* Store new internal state */
    memcpy( internal_states_table[ last_hidden_state ][ last_internal_state ],
            internal_state_a,
            internal_num_st_var * sizeof( int ) );

    /* Read rest of the system states */
    for( i = 1; i < num_system_states; i++ )
    {
        splitSystemState( num_st_vars,
                          system_states_table[i],
                          hidden_num_st_var,
                          hidden_state_b,
                          internal_num_st_var,
                          internal_state_b );

        /* New hidden state found */
        if( !isEqual( hidden_state_a, hidden_state_b, hidden_num_st_var ) )
        {
            num_hidden_states++;
            
            /* Create new hidden state */
            hidden_states_table = (int **)realloc( hidden_states_table,
                                                   num_hidden_states *
                                                   sizeof( int * ) );
            hidden_states_table[ last_hidden_state ] = (int *)malloc(
                                                            hidden_num_st_var *
                                                            sizeof( int ) );
            /* Store new hidden state */
            memcpy( hidden_states_table[ last_hidden_state ],
                    hidden_state_b,
                    hidden_num_st_var * sizeof( int ) );
            hidden_state_found = 1;
    
            num_internal_states = (int *)realloc( num_internal_states,
                                                  num_hidden_states *
                                                  sizeof( int ) );
            num_internal_states[ last_hidden_state ] = 1;

            /* Create new internal state */
            internal_states_table = (int ***)realloc( internal_states_table,
                                                      num_hidden_states 
                                                      * sizeof( int ** ) ); 
            internal_states_table[ last_hidden_state ] = (int **)malloc(
                                        num_internal_states[ last_hidden_state ] 
                                        * sizeof( int * ) ); 
            internal_states_table[ last_hidden_state   ]
                                 [ last_internal_state ] =
                (int *)malloc( internal_num_st_var * sizeof( int ) );

            /* Store new internal state */
            memcpy( internal_states_table[ last_hidden_state   ]
                                         [ last_internal_state ],
                    internal_state_b,
                    internal_num_st_var * sizeof( int ) );
        }

        /* New internal state found */
        if( !isEqual( internal_state_a,
                      internal_state_b,
                      internal_num_st_var ) )
        {
            /* If a new hidden state was just found, actually there is no */
            /* new internal state */
            if( !hidden_state_found )
            {
                num_internal_states[ last_hidden_state ]++;

                /* Create new internal state                 */
                internal_states_table = (int ***)realloc( internal_states_table,
                                                          num_hidden_states 
                                                          * sizeof( int ** ) );
                internal_states_table[ last_hidden_state ] = (int **)realloc( 
                    internal_states_table[ last_hidden_state ],
                    num_internal_states[ last_hidden_state ] 
                    * sizeof( int * ) );
                internal_states_table[ last_hidden_state   ]
                                     [ last_internal_state ] =
                    (int *)malloc( internal_num_st_var * sizeof( int ) );

                /* Store new internal state */
                memcpy( internal_states_table[ last_hidden_state   ]
                                             [ last_internal_state ],
                        internal_state_b,
                        internal_num_st_var * sizeof( int ) );
            }
            
            hidden_state_found = 0;
        }
        
        /* Overwrite last states          */
        memcpy( hidden_state_a,
                hidden_state_b,
                hidden_num_st_var   * sizeof( int ) );
        memcpy( internal_state_a,
                internal_state_b,
                internal_num_st_var * sizeof( int ) );
    }

    /********* Read transition rates and      *********/
    /********* composes hidden model matrices *********/

    /* Allocate structures */
    
    /* Hidden model */
    hidden_transition_rates = (double **)calloc( num_hidden_states, 
                                                 sizeof( double * ) );
    hidden_transition_probs = (double **)calloc( num_hidden_states, 
                                                 sizeof( double * ) );
                                                 
    hidden_transition_rates_extended = (double ****)calloc( num_hidden_states,
                                                         sizeof( double *** ) );

    hidden_transition_recurrent_rates = (double *)calloc( num_hidden_states, 
                                                          sizeof( double ) );
    for( i = 0; i < num_hidden_states; i++ )
    {
        hidden_transition_rates[i] = (double *)calloc( num_hidden_states,
                                                       sizeof( double ) );

        hidden_transition_probs[i] = (double *)calloc( num_hidden_states,
                                                       sizeof( double ) );

        hidden_transition_rates_extended[i] = (double ***)calloc(
                                                    num_hidden_states,
                                                    sizeof( double ** ) );
        for( j = 0; j < num_hidden_states; j++ )
        {
            hidden_transition_rates_extended[i][j] = (double **)calloc(
                                                     num_internal_states[i],
                                                     sizeof( double * ) );
            for( k = 0; k < num_internal_states[i]; k++ )
            {
                hidden_transition_rates_extended[i][j][k] = (double *)calloc(
                                                         num_internal_states[j],
                                                         sizeof( double ) );
            }            
        }
    }
                                                 
    /* Internal model */
    internal_transition_rates = (double ***)calloc( num_hidden_states,
                                                    sizeof( double ** ) );

    internal_transition_recurrent_rates = (double **)calloc( num_hidden_states,
                                                             sizeof( double * ) );

    internal_transition_probs = (double ***)calloc( num_hidden_states,
                                                    sizeof( double ** ) );                                                    

    internal_transition_rates_extended = (double ***)calloc(
                                                        num_hidden_states,
                                                        sizeof( double ** ) );

    for( i = 0; i < num_hidden_states; i++ )
    {
        internal_transition_rates[i] = (double **)calloc(
                                            num_internal_states[i],
                                            sizeof( double * ) );

        internal_transition_recurrent_rates[i] = (double *)calloc(
                                                      num_internal_states[i],
                                                      sizeof( double ) );
                                            
        internal_transition_probs[i] = (double **)calloc(
                                            num_internal_states[i],
                                            sizeof( double * ) );                                            

        internal_transition_rates_extended[i] = (double **)calloc(
                                                         num_internal_states[i],
                                                         sizeof( double * ) );

        for( j = 0; j < num_internal_states[i]; j++ )
        {
            internal_transition_rates[i][j] = (double *)calloc(
                                                    num_internal_states[i],
                                                    sizeof( double ) );
                                                    
            internal_transition_probs[i][j] = (double *)calloc(
                                                    num_internal_states[i],
                                                    sizeof( double ) );                                                    
                                                    
            internal_transition_rates_extended[i][j] = (double *)calloc(
                                                         num_internal_states[i],
                                                         sizeof( double ) );
        }
    }
    
    internal_initial_prob = (double **)calloc( num_hidden_states,
                                               sizeof( double * ) );
    for( i = 0; i < num_hidden_states; i++ )
    {
        internal_initial_prob[i] = (double *)calloc( num_internal_states[i],
                                                     sizeof( double ) );
    }

    sprintf( system_transitions_file, "%s%s", obj_name,
             EXT_TRANSITION_RATES_MATRIX );

    fd = fopen( system_transitions_file, "r" );
    if( fd )
    {
        /* Consume number of system states, since we already know it */
        fscanf( fd, "%*d" );
        
        /* Read each transition (i,j), where i is different from j*/
        while( fscanf(  fd, "%d %d %lf",
                       &system_state_a,
                       &system_state_b,
                       &system_transition_rate ) == 3 )
        {           
            /* Tangram states start from 1 */
            system_state_a--;
            system_state_b--;
        
            /* Split states */
            splitSystemState( num_st_vars,
                              system_states_table[ system_state_a ],
                              hidden_num_st_var,
                              hidden_state_a,
                              internal_num_st_var,
                              internal_state_a );
            splitSystemState( num_st_vars,
                              system_states_table[ system_state_b ],
                              hidden_num_st_var,
                              hidden_state_b,
                              internal_num_st_var,
                              internal_state_b );
            
            /* Query states into tables */
            hidden_state_pos_a   = searchState( hidden_state_a,      
                                                hidden_states_table, 
                                                num_hidden_states,   
                                                hidden_num_st_var ); 
            hidden_state_pos_b   = searchState( hidden_state_b,      
                                                hidden_states_table, 
                                                num_hidden_states,   
                                                hidden_num_st_var ); 
            internal_state_pos_a = searchState(
                                   internal_state_a,
                                   internal_states_table[ hidden_state_pos_a ],
                                   num_internal_states[   hidden_state_pos_a ],
                                   internal_num_st_var );

            internal_state_pos_b = searchState(
                                   internal_state_b,
                                   internal_states_table[ hidden_state_pos_b ],
                                   num_internal_states[   hidden_state_pos_b ],
                                   internal_num_st_var );

            /* Transition between hidden states */
            if( hidden_state_pos_a != hidden_state_pos_b )
            {
                /* Checking if transition rate is valid (smaller then 1 and bigger than 0). */
                if( (system_transition_rate > 1.0) || (system_transition_rate < 0.0) )
                {
                    fprintf( stderr, "Transition rate from state %d to state %d, "
                                     "in the Tangram-II model, is bigger than 1.\n"
                                     "To create a HMM model, these transition "
                                     "rates should be smaller than 1, since\n"
                                     "they, actually, represent state transition "
                                     "probabilities.", system_state_a + 1,
                                                        system_state_b + 1  );
                    return NULL;
                }
                
                hidden_transition_rates_extended[ hidden_state_pos_a   ]
                                                [ hidden_state_pos_b   ]
                                                [ internal_state_pos_a ]
                                                [ internal_state_pos_b ] = 
                                                  system_transition_rate;
            }
            /* Transition inside a given hidden state */
            else
            {
                /* Ignore any other model variable change */
                if( internal_state_pos_a != internal_state_pos_b )
                {
                    /* These transitions may be bigger than 1, so no check is needed. */
                    internal_transition_rates_extended[ hidden_state_pos_a   ]
                                                      [ internal_state_pos_a ]
                                                      [ internal_state_pos_b ] =
                                                      system_transition_rate;
                }
                /* else: Transition between states that do not compose the hidden
                   Markov model. Ignoring them... */
            }
        }

        fclose( fd );
    }
    else
    {
        printFileOpenError( 'r', system_transitions_file );

        return NULL;         
    }

    /********* Check restriction on hidden_transition_rates_extended *********/
    /********* and evaluate internal_initial_prob                    *********/

    /* For each entering state j */
    for( j = 0; j < num_hidden_states; j++ )
    {
        /* For each leaving state state i */
        for( i = 0; i < num_hidden_states; i++ )
        {
            if( i != j )
            {
                /* For each non-null row */
                for( k = 0; k < num_internal_states[i]; k++ )
                {
                    if( !isNull( hidden_transition_rates_extended[i][j][k],
                                 num_internal_states[j] ) )
                    {                        
                        /* First non-null vector, will be copied as an */
                        /* example to check parallelism */
                        /* This vector will store the initial internal state */
                        /* probability distribution on hidden state j */
                        if( isNull( internal_initial_prob[j],
                                    num_internal_states[j] ) )
                        {
                            sum = 0.0;
                            
                            /* Copy vector and get sum of all elements */
                            for( l = 0; l < num_internal_states[j]; l++ )
                            {
                                internal_initial_prob[j][l] = 
                                hidden_transition_rates_extended[i][j][k][l];
                                
                                sum += internal_initial_prob[j][l];
                            }

                            /* Calculate probabilities */
                            /* Example: Let lambda be the transition rate */
                            /* from hidden state i to j. If the internal model */
                            /* is a Gilbert with p1 and p2 steady state */
                            /* probabilities: */
                            /* internal_initial_prob[j][0] = */
                            /* lambda * p1 / ( lambda * p1 + lambda * p2 ) */
                            /* internal_initial_prob[j][1] = */
                            /* lambda * p2 / ( lambda * p1 + lambda * p2 ) */
                            for( l = 0; l < num_internal_states[j]; l++ )
                                internal_initial_prob[j][l] /= sum;
                        }
                        /* Check if current vector is parallel do the model */
                        else
                        {
                            if( !isParallel(
                                     internal_initial_prob[j],
                                     hidden_transition_rates_extended[i][j][k],
                                     num_internal_states[j] ) )
                            {
                                fprintf( stderr, "Rows from entering hidden "
                                                 "state %d are not parallel. "
                                                 "Cannot solve this model!\n"
                                                 "Aborting...\n",
                                                 j + 1 );

                                return NULL;
                            }
                        }
                    }
                }
            }
        }
    }

    /********* Assembly hidden_transition_rates ********    */
    for( i = 0; i < num_hidden_states; i++ )
    {
        sum = 0.0;
    
        for( j = 0; j < num_hidden_states; j++ )
        {
            for( k = 0; k < num_internal_states[i]; k++ )
            {
                /* Get the sum of the first non-null row */
                if( !isNull( hidden_transition_rates_extended[i][j][k],
                             num_internal_states[j] ) )
                {                    
                    for( l = 0; l < num_internal_states[j]; l++ )
                    {
                        hidden_transition_rates[i][j] +=
                        hidden_transition_rates_extended[i][j][k][l];
                    }

                    sum += hidden_transition_rates[i][j];
                    
                    break; /* There is only need to consider transitions out */
                           /* of one internal state, since we are evaluating */
                           /* transitions between hidden states              */
                }
            }
        }
        
        /* Diagonal has to be 1 - sum of all rates in this row */
        hidden_transition_recurrent_rates[i] = 1.0 - sum;
    }

    /********* Check restriction on hidden transition rates *********/
    for( i = 0; i < num_hidden_states; i++ )
    {
        sum = 0.0;
    
        for( j = 0; j < num_hidden_states; j++ )
            sum += hidden_transition_rates[i][j];
    
        /* Considering possible recurrent transition */
        sum += hidden_transition_recurrent_rates[i];
        
        if( fabs(sum - 1.0) > TOLERANCE )
        {
            fprintf( stderr, "Transitions out of hidden state %d do not sum "
                             "1.\nPlease check your Tangram-II model.\n"
                             "Aborting...\n",
                             i );
            fprintf( stderr, "The sum calculated was %lf.\n", sum );

            return NULL;
        }
    }

    /********* Assembly internal_transition_rates *********/
    for( i = 0; i < num_hidden_states; i++ )
    {
        for( j = 0; j < num_internal_states[i]; j++ )
        {
            sum = 0.0;
        
            for( k = 0; k < num_internal_states[i]; k++ )
            {
                if( k != j )
                {
                    /* Internal trans = M[value] - (recurrent hidden state transition * probability of internal state) */
                    internal_transition_rates[i][j][k] =
                    internal_transition_rates_extended[i][j][k] - (hidden_transition_recurrent_rates[i] * internal_initial_prob[i][k]);
                
                    sum += internal_transition_rates[i][j][k];
                }
            }
    
            /* Diagonal has to be 1 - sum of all rates in this row */
            internal_transition_recurrent_rates[i][j] = 1.0 - sum;
        }
    }

    /********* Check restriction on internal transition rates *********/
    if( internal_num_st_var ) /* There are internal states */
    {
        for( i = 0; i < num_hidden_states; i++ )
        {
            for( j = 0; j < num_internal_states[i]; j++ )
            {
                sum = 0.0;

                for( k = 0; k < num_internal_states[i]; k++ )
                    sum += internal_transition_rates[i][j][k];

                /* Considering possible recurrent transition */
                sum += internal_transition_recurrent_rates[i][j];
                
                if( fabs(sum - 1.0) > TOLERANCE )
                {
                    fprintf( stderr, "Transitions out of lower-level state %d, in "
                                     "upper-level state %d, do not sum 1.\nPlease "
                                     "check your Tangram-II model.\n"
                                     "Aborting...\n", j, i );

                    fprintf( stderr, "The sum calculated was %lf.\n", sum );
            
                    return NULL;
                }
            }
        }
    }

/************* Verifying Calculated Values ***************************/
    sprintf( system_same_state_transitions_file, "%s%s", obj_name,
             EXT_SAME_STATE_TRANSITION_RATES_MATRIX );

    fd = fopen( system_same_state_transitions_file, "r" );
    if( fd )
    {
        /* Read each transition (i,i) */
        while( fscanf(  fd, "%d %d %lf",
                       &system_state_a,
                       &system_state_b,
                       &system_transition_rate ) == 3 )
        {           
            /* Transitions to the same state may be bigger than 1, since */
            /* they aggregate transitions from both hidden and internal  */
            /* states. Thus, no checking is nedded.                      */
            
            /* Tangram states start from 1 */
            system_state_a--;
            system_state_b--;
        
            /* Split states */
            splitSystemState( num_st_vars,
                              system_states_table[ system_state_a ],
                              hidden_num_st_var,
                              hidden_state_a,
                              internal_num_st_var,
                              internal_state_a );
            splitSystemState( num_st_vars,
                              system_states_table[ system_state_b ],
                              hidden_num_st_var,
                              hidden_state_b,
                              internal_num_st_var,
                              internal_state_b );
            
            /* Query states into tables */
            hidden_state_pos_a   = searchState( hidden_state_a,      
                                                hidden_states_table, 
                                                num_hidden_states,   
                                                hidden_num_st_var ); 
            hidden_state_pos_b   = searchState( hidden_state_b,      
                                                hidden_states_table, 
                                                num_hidden_states,   
                                                hidden_num_st_var ); 
            internal_state_pos_a = searchState(
                                   internal_state_a,
                                   internal_states_table[ hidden_state_pos_a ],
                                   num_internal_states[   hidden_state_pos_a ],
                                   internal_num_st_var );

            internal_state_pos_b = searchState(
                                   internal_state_b,
                                   internal_states_table[ hidden_state_pos_b ],
                                   num_internal_states[   hidden_state_pos_b ],
                                   internal_num_st_var );

            /* Decomposing transition in hidden and/or internal transitions */
            sum = 0.0;
            for( j = 0; j < num_hidden_states; j++ )
            {               
                for( k = 0; k < num_internal_states[hidden_state_pos_a]; k++ )
                {
                    for( l = 0; l < num_internal_states[j]; l++ )
                    {
                        sum += hidden_transition_rates_extended[hidden_state_pos_a][j][k][l];
                    }
                    
                    break; /* There is only need to consider transitions out */
                           /* of one internal state, since we are evaluating */
                           /* transitions between hidden states              */
                }
            }           

            if( fabs((1.0 - sum) > TOLERANCE) ) /* transition belongs to a hidden state */
            {
                /* Identifing how much of this rate corresponds to the hidden state transition */
                hidden_transition   = 1.0 - sum;

                if( ( internal_num_st_var == 0 ) && ( fabs( system_transition_rate - hidden_transition ) > TOLERANCE ) )
                {                   
                    fprintf( stderr, "Something is wrong with the model's specification. "
                                     "Transitions out of hidden state %d do not sum 1.\n\n",
                                      hidden_state_pos_a );

                    fprintf( stderr, "Check the rate values you have specified "
                                     "(especially the recurrent rate values),\n"
                                     "and if every transition in the hidden chain "
                                     "is beeing informed to the internal chain.\n\n" );

                    fprintf( stderr, "Make sure, also, that you have informed Tangram-II, "
                                     "correctly, which state variables\ncorrespond to hidden "
                                     "states and which ones correspond to lower-level states\n" );
                    
                    return NULL;
                }

                /* If there is any internal state */
                if( internal_num_st_var )
                {
                    internal_transition = system_transition_rate -
                                          (hidden_transition_recurrent_rates[hidden_state_pos_a] *
                                          internal_initial_prob[hidden_state_pos_a][internal_state_pos_b]);

                    if( fabs( internal_transition_recurrent_rates[hidden_state_pos_a][internal_state_pos_a] - internal_transition ) > TOLERANCE )
                    {
                        fprintf( stderr, "Something is wrong with the model's specification. "
                                         "Transitions out of lower-level state %d do not sum 1.\n\n",
                                          internal_state_pos_a );

                        fprintf( stderr, "Check the rate values you have specified "
                                         "(especially the recurrent rate values),\n"
                                         "and if every transition in the upper-level chain "
                                         "is beeing informed to the lower-level chain.\n\n" );

                        fprintf( stderr, "Make sure, also, that you have informed Tangram-II, "
                                         "correctly, which state variables\ncorrespond to upper-level "
                                         "states and which ones correspond to lower-level states\n" );

                        return NULL;
                    }

                    /* All is correct, inserting transition into transition matrix */
                    internal_transition_rates[hidden_state_pos_a][internal_state_pos_a][internal_state_pos_a] = internal_transition;
                }

                /* All is correct, inserting transition into transition matrix */
                hidden_transition_rates[hidden_state_pos_a][hidden_state_pos_b] = hidden_transition;
            }
            else
            {
                internal_transition = system_transition_rate -
                                      (hidden_transition_recurrent_rates[hidden_state_pos_a] *
                                      internal_initial_prob[hidden_state_pos_a][internal_state_pos_b]);

                if( fabs( internal_transition_recurrent_rates[hidden_state_pos_a][internal_state_pos_a] - internal_transition ) > TOLERANCE )
                {                    
                        fprintf( stderr, "Something is wrong with the model's specification. "
                                         "Transitions out of lower-level state %d do not sum 1.\n\n",
                                          internal_state_pos_a );

                        fprintf( stderr, "Check the rate values you have specified "
                                         "(especially the recurrent rate values),\n"
                                         "and if every transition in the upper-level chain "
                                         "is beeing informed to the lower-level chain.\n\n" );

                        fprintf( stderr, "Make sure, also, that you have informed Tangram-II, "
                                         "correctly, which state variables\ncorrespond to upper-level "
                                         "states and which ones correspond to lower-level states\n" );

                    return NULL;
                }

                /* All is correct, inserting transition into transition matrix */
                internal_transition_rates[hidden_state_pos_a][internal_state_pos_a][internal_state_pos_a] = internal_transition;
            }
        }
        
        fclose( fd );
    }
    else
    {
        printFileOpenError( 'r', system_same_state_transitions_file );

        return NULL;         
    }

/**** Evaluate if all possible recurrent transitions were correctly specified ****/

    /* Check hidden states */
    for( i = 0; i < num_hidden_states; i++ )
    {
        /* Comparing calculated and specified values */
        if( fabs( hidden_transition_rates[i][i] - hidden_transition_recurrent_rates[i] ) > TOLERANCE )
        {
            fprintf( stderr, "Something is wrong with the model's specification. "
                             "Transitions out of hidden state %d do not sum 1.\n\n"
                             "Please check if all transitions were specified, "
                             "including the recurrent transitions.\n",
                              i );

            return NULL;
        }
    }

    /* Check internal states */
    if( internal_num_st_var )
    {
        for( i = 0; i < num_hidden_states; i++ )
        {
            for( j = 0; j < num_internal_states[i]; j++ )
            {
                /* Comparing calculated and specified values */
                if( fabs( internal_transition_rates[i][j][j] - internal_transition_recurrent_rates[i][j] ) > TOLERANCE )
                {
                    fprintf( stderr, "Something is wrong with the model's specification. "
                                     "Transitions out of lower-level state %d, of upper-level "
                                     "state %d do not sum 1.\n\n"
                                     "Please check if all transitions were specified, "
                                     "including the recurrent transitions.\n",
                                      j, i );

                    return NULL;
                }
            }
        }
    }

/************* Evaluate Probabilities *****************************/

    /********* Evaluate hidden transitions probabilities *********/
    for( i = 0; i < num_hidden_states; i++ )    
    {
        sum = 0.0;
        
        for( j = 0; j < num_hidden_states; j++ )
            sum += hidden_transition_rates[i][j];
        
        for( j = 0; j < num_hidden_states; j++ )
            hidden_transition_probs[i][j] = hidden_transition_rates[i][j] / sum;
    }

    /********* Evaluate internal transition probabilities ********    */
    for( i = 0; i < num_hidden_states; i++ )
    {
        for( j = 0; j < num_internal_states[i]; j++ )
        {        
            sum = 0.0;        

            for( k = 0; k < num_internal_states[i]; k++ )
                sum += internal_transition_rates[i][j][k];

            for( k = 0; k < num_internal_states[i]; k++ )
                internal_transition_probs[i][j][k] = 
                internal_transition_rates[i][j][k] / sum;
        }
    }    
                
    /********* Print all matrices *********/
    /* Hidden states */
    hidden_transitions_file = (char *)malloc( MAX_PATH * sizeof( char ) );
    sprintf( hidden_transitions_file, "%s.HMM.hidden_st%s", obj_name,
             EXT_TRANSITION_PROB_MATRIX );
             
    fd = fopen( hidden_transitions_file, "w" );
    if( fd )
    {
        /* Print transition rates (sparse format) */
        for( i = 0; i < num_hidden_states; i++ )
            for( j = 0; j < num_hidden_states; j++ )
                if( hidden_transition_probs[i][j] > 0.0 )
                    fprintf( fd, "%d %d %.10E\n",
                             i + 1, /* Tangram states start from 1 */
                             j + 1,
                             hidden_transition_probs[i][j] );
        
        fclose( fd );
    }
    else
    {
        printFileOpenError( 'w', hidden_transitions_file );

        return NULL;         
    }

    /* Internal states */
    if( internal_num_st_var )
    {
        internal_transitions_file  = (char **)malloc( num_hidden_states
                                                    * sizeof( char * ) );
        internal_initial_prob_file = (char **)malloc( num_hidden_states
                                                    * sizeof( char * ) );
        for( i = 0; i < num_hidden_states; i++ )
        {
            internal_transitions_file[i]  = (char *)malloc( MAX_PATH
                                                          * sizeof( char ) );
            internal_initial_prob_file[i] = (char *)malloc( MAX_PATH
                                                          * sizeof( char ) );

            sprintf( internal_transitions_file[i],  "%s.HMM.internal_st_%d%s", obj_name, i,
                     EXT_TRANSITION_PROB_MATRIX );
            sprintf( internal_initial_prob_file[i], "%s.HMM.internal_st_%d%s", obj_name, i,
                     EXT_INITIAL_PROBABILITIES );                 

            fd = fopen( internal_transitions_file[i], "w" );

            if( fd )
            {
                /* Print transition rates (sparse format) */
                for( j = 0; j < num_internal_states[i]; j++ )
                    for( k = 0; k < num_internal_states[i]; k++ )
                        if( internal_transition_probs[i][j][k] > 0.0 )
                            fprintf( fd, "%d %d %.10E\n",
                                     j + 1,
                                     k + 1,
                                     internal_transition_probs[i][j][k] );

                fclose( fd );     
            }
            else
            {
                printFileOpenError( 'w', internal_transitions_file[i] );

                return NULL;         
            }

            fd = fopen( internal_initial_prob_file[i], "w" );

            if( fd )
            {
                /* Print initial probabilities (sparse format) */
                for( j = 0; j < num_internal_states[i]; j++ )
                    if( internal_initial_prob[i][j] > 0.0 )
                        fprintf( fd, "%d %.10E\n",
                                 j + 1,
                                 internal_initial_prob[i][j] );

                fclose( fd );     
            }
            else
            {
                printFileOpenError( 'w', internal_initial_prob_file[i] );

                return NULL;         
            }           
        }
    }
    else
    {
        internal_transitions_file  = NULL;
        internal_initial_prob_file = NULL;
    }

    /********* Free all allocated structures *********/
    /* System states */
    for( i = 0; i < num_system_states; i++ )
        if( system_states_table[i] )
            free( system_states_table[i] );
    if( system_states_table )
        free( system_states_table );

    /* Hidden states */
    if( hidden_state_a )
        free( hidden_state_a );
    if( hidden_state_b )
        free( hidden_state_b );

    for( i = 0; i < num_hidden_states; i++ )
    {
        if( hidden_states_table[i] )
            free( hidden_states_table[i] );

        if( hidden_transition_rates[i] )
            free( hidden_transition_rates[i] );
            
        if( hidden_transition_probs[i] )
            free( hidden_transition_probs[i] );            

        for( j = 0; j < num_hidden_states; j++ )
        {
            for( k = 0; k < num_internal_states[i]; k++ )
            {
                if( hidden_transition_rates_extended[i][j][k] )
                    free( hidden_transition_rates_extended[i][j][k] );
            }
            
            if( hidden_transition_rates_extended[i][j] )
                free( hidden_transition_rates_extended[i][j] );
        }

        if( hidden_transition_rates_extended[i] )
            free( hidden_transition_rates_extended[i] );
    }

    if( hidden_states_table )
        free( hidden_states_table );

    if( hidden_transition_rates )
        free( hidden_transition_rates );
        
    if( hidden_transition_probs )
        free( hidden_transition_probs );        

    if( hidden_transition_rates_extended )
        free( hidden_transition_rates_extended );    
        
    /* Internal states */
    if( internal_state_a )
        free( internal_state_a );        
    if( internal_state_b )
        free( internal_state_b ); 

    for( i = 0; i < num_hidden_states; i++ )
    {
        for( j = 0; j < num_internal_states[i]; j++ )
        {
            if( internal_states_table[i][j] )
                free( internal_states_table[i][j] );

            if( internal_transition_rates[i][j] )
                free( internal_transition_rates[i][j] );
                
            if( internal_transition_probs[i][j] )
                free( internal_transition_probs[i][j] );

            if( internal_transition_rates_extended[i][j] )
                free( internal_transition_rates_extended[i][j] );
        }

        if( internal_states_table[i] )
            free( internal_states_table[i] );

        if( internal_transition_rates[i] )
            free( internal_transition_rates[i] );

        if( internal_transition_probs[i] )
            free( internal_transition_probs[i] );            
            
        if( internal_initial_prob[i] )
            free( internal_initial_prob[i] );

        if( internal_transition_rates_extended[i] )
            free( internal_transition_rates_extended[i] );
    }

    if( internal_states_table )
        free( internal_states_table );
    
    if( internal_transition_rates )
        free( internal_transition_rates );
        
    if( internal_transition_probs )
        free( internal_transition_probs );        

    if( internal_initial_prob )
        free( internal_initial_prob );

    if( internal_transition_rates_extended )
        free( internal_transition_rates_extended );
                           
    /********* Return values *********/
    hidden_model = (HiddenModel *)malloc( sizeof( HiddenModel ) );
    hidden_model->num_hidden_states          = num_hidden_states;
    hidden_model->hidden_transitions_file    = hidden_transitions_file;
    hidden_model->num_internal_states        = num_internal_states;
    hidden_model->internal_initial_prob_file = internal_initial_prob_file;
    hidden_model->internal_transitions_file  = internal_transitions_file;
    
    return hidden_model;
}
