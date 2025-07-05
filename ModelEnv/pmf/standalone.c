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

/*
 *  PMF possible usages (from file btools.c):
 *  
 *  To calculate the pmf of one or more variables type:
 *  pmf <modelname> <modelname>.<ss/ts>.<solution method> {-o <output file extension>} -v[c] <1st variable 2nd variable...> "condition"
 *  To calculate the pmf of a function type:
 *  pmf <modelname> <modelname>.<ss/ts>.<solution method> {-o <output file extension>} -f[c] "function" "condition"
 *  To calculate the probability of a set type:
 *  pmf <modelname> <modelname>.<ss/ts>.<solution method> {-o <output file extension>} -s[c] "set_description" "condition"
 *
 *  where { } indicates an optional argument
 *
 *  PMF return values: 
 *
 * old: new: description
 *  0 : 00 : ok
 * -1 : 20 : less than 4 arguments
 * -2 : 40 : memory allocation failure
 * -3 : 32 : serious reading or openning file error
 * -4 : 58 : argv[3][0]!=´-´
 * -5 : 21 : bad condition specification in -vc option
 * -6 : 20 : wrong numb. of args. using -fc
 * -7 : 21 : bad condition specification in -fc option
 * -8 : 20 : wrong numb. of args. using -f
 * -9 : 21 : bad set description in -s or -sc option
 * -10: 20 : wrong numb. of args. using -sc
 * -11: 21 : bad condition specification in -sc option
 * -12: 20 : wrong numb. of args. using -s
 * -13: 58 : neither -v nor -f nor -s
 * -14: 40 : memory allocation failure in one of generate_variables_pmf, generate_function_pmf or generate_set_probability
 * -15: 21 : wrong variable name
 * -16: 21 : not valid expression
 * -17: 33 : output error  
 */

#include <stdio.h>
#include <string.h>

#include "pmf.h"

#define TRUE 2
#define FALSE 0


/* ---- GLOBAL VARIABLES ---- */

struct st_var       *st_var_tb;                               /* model's variables table */
struct st_vec       *st_vec_tb;                               /* model's states table */
struct interest_vec *interest_vec_list = NULL;                /* head of list containing the pmf data */
int                  n_of_variables;                          /* total number of variables in the model */
int                  n_of_states;                             /* total number of states in the model*/
int                  n_of_interest_variables;                 /* number of pmf variables choosen by the user */
double               sum_prob;                                /* the sum of all probabilities of the list */
double               set_prob;                                /* the probability of a set */

int newRetVal(int oldReturnValue);

int newRetVal(int oldReturnValue)
{
    int newReturnValue;
    switch (oldReturnValue)
    {
        case   0: newReturnValue =   0; break;
        case  -1:
        case  -6:
        case  -8:
        case -10:
        case -12: newReturnValue =  20; break;
        case  -2:
        case -14: newReturnValue =  40; break;
        case  -3: newReturnValue =  32; break;
        case  -4: newReturnValue =  58; break;
        case  -5:
        case  -7:
        case  -9:
        case -11:
        case -15:
        case -16: newReturnValue =  21; break;
        case -13: newReturnValue =  58; break;
        case -17: newReturnValue =  33; break;
        default : newReturnValue = 255; break;
    }    
    return newReturnValue;
}


int main (int argc, char **argv)
{  
    int   retval=0;
    int   i;
    char  var_list [MAXSTRING];
    char  measure_name [MAXSTRING];
    int   userDefinedOutputFile;	/* the user defined an output file? */
/*
 * int userDefinedOutputFile;
 * = 2, if the user defined an output file, using the -o [output file] option in the command line
 * = 0, if the default output file ([modelname].IM.out) should be used 
 *
 */
    
    /*the number of arguments in the command line must be greater than 3*/
    if (argc < 5) {
	usage();
	/*the program is aborted if there is less than 4 arguments */
	return( newRetVal(-1) );
    }    	
    
    /*Allocate the memory for the table of variables, and the table of states and probabilities*/
    if( (retval = allocate_tables(argv[1])) != 0) 
        /*in case of an allocation failure or file format error the program is aborted*/
        return( retval ); 
    
   
    /*Read the files that describe the model readen in the command line*/
    if( (retval = read_files (argv[1],argv[2])) != 0)
    {
        free_all();
        /*in case of a serious reading or openning file error the program is aborted*/
        return( retval ); 
    }
    
  
    if (argv[3][0] != '-') {
	usage();
	free_all();
	return( newRetVal(-4) );
    }
    
    if (argv[3][1] == 'o') {
        userDefinedOutputFile = TRUE;   /* remember that TRUE = 2 */
    	strcpy (measure_name, argv[4]);
    }
    else {
      	userDefinedOutputFile = FALSE;
    	strcpy (measure_name, "out");
    }		
    
    /*See if the pmf option is v -> variables pmf*/
    if (argv[3+userDefinedOutputFile][1] == 'v') {
	
	strcpy (var_list, argv[4+userDefinedOutputFile]);
	
	for (i = 5+userDefinedOutputFile; i < (argc - 1); i++) {
	    strcat (var_list, "@");
	    strcat (var_list, argv[i]); 
	}
	
	if (argv[3+userDefinedOutputFile][2] == 'c') {
	    if (cant_be_condition(argv[argc-1])) {
		printf("Bad condition specification - %s\n\n", argv[argc-1]);
		usage();
		free_all();
		return( newRetVal(-5) );
	    }
		
	    retval=generate_variables_pmf (argv[1], var_list, argv[argc-1], measure_name);
            
            if (retval==-3) retval--;
	}
	else {
	    strcat (var_list, "@");
	    strcat (var_list, argv[argc-1]);
	    retval=generate_variables_pmf (argv[1], var_list, NULL, measure_name);
            
            if (retval==-3) retval--;
	}
    }
    /*See if the pmf option is f -> function pmf*/
    if( argv[3+userDefinedOutputFile][1] == 'f' )
    {
        if( argv[3+userDefinedOutputFile][2] == 'c' )
        {
            if( argc - userDefinedOutputFile != 6 )
            {
                usage();
                free_all();
                fprintf(stderr, "\nfunction pmf error - wrong number of parameters\n");
                return( newRetVal(-6) );
            }
            if( cant_be_condition( argv[ argc - 1 ] ) )
            {
                printf( "Bad condition specification - %s\n\n", argv[argc-1] );
                usage();
                free_all();
                return( newRetVal(-7) );
            }
            retval=generate_function_pmf (argv[1],argv[4+userDefinedOutputFile],argv[5+userDefinedOutputFile],measure_name);
            if (retval<=-2) retval--;            
        }
    
        else
        {   
            if( argc-userDefinedOutputFile != 5 )
            {
                usage();
                free_all();
                fprintf(stderr, "\nfunction pmf error - wrong number of parameters\n");
                return( newRetVal(-8) );
            }
            retval=generate_function_pmf (argv[1],argv[4+userDefinedOutputFile],NULL,measure_name);
            if (retval<=-2) retval--;            
        }    
    }
    /*See if the pmf option is s -> probability of a set defined by an expression*/
    if (argv[3+userDefinedOutputFile][1]=='s') {
	if (cant_be_condition(argv[3+userDefinedOutputFile+1])) {
	    printf("Bad set description - %s\n\n", argv[argc-1]);
	    usage();
	    free_all();
	    return( newRetVal(-9) );
	}
	if (argv[3+userDefinedOutputFile][2]=='c') {
	    if (argc-userDefinedOutputFile != 6) {
		usage();
		free_all();
                fprintf(stderr, "\npmf set prob. error - wrong number of parameters\n");
		return( newRetVal(-10) );
	    }
	    if (cant_be_condition(argv[argc-1])) {
		printf("Bad condition specification - %s\n\n", argv[argc-1]);
		usage();
		free_all();
		return( newRetVal(-11) );
	    }
	    retval=generate_set_probability (argv[1], argv[4+userDefinedOutputFile], argv[5+userDefinedOutputFile], measure_name);
            if (retval<=-2) retval--;
	}
	else
	    {
		if (argc-userDefinedOutputFile != 5) {
                    fprintf(stderr, "\npmf set prob. error - wrong number of parameters\n");
		    usage();
		    free_all();
		    return( newRetVal(-12) );
		} 
		retval=generate_set_probability (argv[1], argv[4+userDefinedOutputFile], NULL, measure_name);
                if (retval<=-2) retval--;
	    }
    }
    /*If the pmf option is not valid (neither v, neither f, neither s)*/
    if ((argv[3+userDefinedOutputFile][1]!='v') && 
        (argv[3+userDefinedOutputFile][1]!='f') && 
        (argv[3+userDefinedOutputFile][1]!='s'))
    {
	usage();
        fprintf(stderr,"pmf option is not valid (neither v, neither f, neither s");
        return( newRetVal(-13) );
    }
    
    free_all ();
    
    if (retval!=1) 
        return( newRetVal(-13+retval) );
    else    
        return( newRetVal(0) );
}
