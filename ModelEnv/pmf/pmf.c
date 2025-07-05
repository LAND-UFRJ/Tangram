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

#include <stdio.h>
#include <string.h>

#include "pmf.h"

/* ---- GLOBAL VARIABLES ---- */

extern struct st_var       *st_var_tb;                               /* model's variables table */
extern struct interest_vec *interest_vec_list;                       /* head of list containing the pmf data */
extern int                  n_of_variables;                          /* total number of variables in the model */

/* ---- MAIN FUNCTIONS ---- */

/* ATTENTION return values: 1->ok; -1->memory allocation failure; -2->wrong variable;
name; -3->output error ATTENTION*/
int generate_variables_pmf (char *basename, char *var_list,
                            char *real_condition, char *measure_name)
{ 
  int rv; /*return value*/
  int i;
  
  /* It is necessary to put zero in all variable's orders,
     to prepare the lib to another measure (just in lib)*/
  for (i = 0; i < n_of_variables; i++) {
    st_var_tb[i].order = 0;
  }

  /* It is necessary to free the memory used by the preceeding measure (just in lib)*/
  if (interest_vec_list != NULL) {
    free_interest_vec (interest_vec_list);
    interest_vec_list = NULL;
  }
  
  rv = calculate_variables_pmf(var_list, real_condition);
  if (rv != 0)
    return rv;
  
  /*print the pmf of selected variables (conditional or not)*/
  if (print_variables_pmf_list (basename, interest_vec_list, real_condition, measure_name)!=0)
        return -3;
  
  return 1;
}

/* ATTENTION return values: 1->ok; -1->memory allocation failure; -2->not valid
expression -3->output error; ATTENTION*/
int generate_function_pmf (char *basename, char *real_function, char *real_condition, char *measure_name)
{	
  int rv; /*return value*/
  
  /* It is necessary to free the memory used by the preceeding measure (just in lib)*/
  if (interest_vec_list != NULL) {
    free_interest_vec (interest_vec_list);
    interest_vec_list = NULL;
  }
  
  rv = calculate_pmf_of_function(real_function,real_condition);
  if (rv != 0)	    
    return rv;
  
  if (print_func_pmf_list(basename, interest_vec_list, real_function, real_condition, measure_name)!=0)
        return -3;
  
  return 1;
}

/* ATTENTION return values: 1->ok; -1->memory allocation failure; -2->not valid
   expression; -3->output error ATTENTION*/
int generate_set_probability (char *basename, char *set, char *condition, char *measure_name)
{                
  int rv;
  
  /* It is necessary to free the memory used by the preceeding measure (just in lib)*/
  if (interest_vec_list != NULL) {
    free_interest_vec (interest_vec_list);
    interest_vec_list = NULL;
  } 
  
  rv = calculate_set_probability(set, condition);
  if (rv != 0)
    return rv;

  /*print the set probability value*/
  if (print_set_probability (basename, set, condition, measure_name)!=0)
        return -3;
  
  return 1;
}
