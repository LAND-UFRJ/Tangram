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
#include <stdlib.h>
#include <math.h>

#include "formulc.h"
#include "pmf.h"

void strreplace(char *s1, char *sub, char *repl);

/* ---- GLOBAL VARIABLES ---- */

extern struct st_var       *st_var_tb;                /* model's variables table */
extern struct st_vec       *st_vec_tb;                /* model's states and description table */
extern struct interest_vec *interest_vec_list;        /* head of list containing the pmf data */
extern int                  n_of_states;              /* total number of states in the model*/
extern int                  n_of_interest_variables;  /* number of pmf variables choosen by the user */
extern int                  n_of_variables;           /* number of variables in the model */
extern double               sum_prob;                 /* sum of all probabilities of the list */
extern double               set_prob;                 /* probability of the set */


/* ---- CALCULATE FUNCTIONS ---- */

/* return values: 0->ok; -1->memory allocation failure; -2->not valid expression*/
int calculate_variables_pmf (char *var_list, char *cond)
{
    formu                f;          /* pointer to formula (typedef of library formulc)*/
    int                  i,j,k;      /*counter and idetifier of each state*/
    struct interest_vec *p_int_vec;  /*pointer to element to be inserted*/
    char                *variables; /* string with the variables that is used in formula (exigence of formulc)*/
    int                  leng;      /* exigence of formulc */
    int                  error;     /* exigence of formulc */
    int                  condition_is_true = 1;    /*True if condition is true */
    char                 *condition;               /*the condition with the variable names replaced by: a,b,c,...*/
    char                 var_name[2];
    
    if (get_usr_variables(var_list) != 0)
        return -2; 
    
    /*tries to allocate memory to string varibles*/
    if ((variables = (char*) malloc (n_of_variables)) == NULL) {
	printf("Memory allocation failure when calculating the pmf\n");
	return -1;
    }
    
    if ((cond != NULL) && (strcmp(cond,"") != 0)) {

	if ( (condition = (char *) malloc (MAXSTRING)) == NULL) {
	    printf("Memory allocation failure\n");
	    return -1;
	}
	
	strcpy (condition, cond);
	
	for (i = 0; i < n_of_variables; i++) {
	    var_name[0] = 'a'+i;
	    var_name[1] = '\0';
	    
	    strreplace (condition, st_var_tb[i].var_name, var_name);
	
	    /*fill the string variables with a,b,c,... 
	      until the number of letters is equal to number of variables in the model*/
	    
	    variables[i] = 'a'+ i;
	}
	 
	variables[n_of_variables] = '\0';
	
	fnew ("not", (Func) fnot,1,0);
	
	/*translate the formula to f*/
	f = translate(condition,variables,&leng,&error);
	
	if (error!=-1) {
	    printf("Bad condition specification - %s\n",cond);
	    return -2;
	}
	
    }
	
    k = 0;
    
    for (i = 0; i < n_of_states; i++) {
	
	/* look for an used field in the table */
	while (st_vec_tb[k].status == 0) 
	    k++;
	
	/*substitutes the variable values of this state in formula*/
	if ((cond != NULL) && (strcmp(cond,"") != 0)) { 
	    for(j = 0;j < n_of_variables;j++)
		make_var(('a'+ j),st_vec_tb[k].desc[j]);
	    
	    /*compute the result*/
	    condition_is_true = fval_at (f);
	}
	
	if (condition_is_true) {
	    if ( (p_int_vec = (struct interest_vec *) malloc (sizeof(struct interest_vec))) == NULL) {
		printf("Memory allocation failure when calculating the pmf\n");
		return -1; /* memory allocation failure */
	    }
	    
	    if ( (p_int_vec->vars = (double *) malloc (n_of_interest_variables*sizeof(double))) == NULL) {
		printf("Memory allocation failure when calculating the pmf\n");
		return -1; /* memory allocation failure */
	    }
	    
	    fill_int_vec (p_int_vec, st_vec_tb[k]);
	    
	    insert_interest_vec (p_int_vec);
	}
	
	/*starts searching for the next used field in the table*/
	k++;   
    }
    
    norm_cond_pmf (interest_vec_list);    
    
    return 0;
}
    
/* return values: 0->ok; -1->memory allocation failure; -2->not valid expression */
int calculate_pmf_of_function (char *func, char *cond)
{
  formu                c;           /* pointer to condition (typedef of library formulc) */
  formu                f;           /* pointer to formula (typedef of library formulc)*/
  struct interest_vec *p_int_vec;   /* pointer to element that is inserted */
  char                *variables;   /* string with the variables that is used in formula (exigence of formulc)*/
  int                  i,j,k;       /* counters */
  int                  leng;        /* exigence of formulc */
  int                  error;       /* exigence of formulc */
  double               result;      /* value of the function applied to the state */
  int                  is_true = 1; /* 1-> the condition is true, 0-> the condition is false */
  char                *condition;   /* condition with variable names substituted by a,b,c...*/
  char                 function[MAXSTRING]; /* function with variable names substituted by a,b,c,...*/
  char                 var_name[2];         /* string to substitut the variable name by the corresponding leter */ 
  
  /*prepares the string function to handle the function expression with the letters a,b,c,...*/
  strcpy (function, func); 
  
  /*the only variable of interest is the function*/
  n_of_interest_variables = 1;

  /*defines the word "not" as the boolean complement of an expression*/
  fnew ("not", fnot, 1, 0);
  
  /*tries to allocate memory to string varibles*/
  if ((variables = (char*) malloc (n_of_variables)) == NULL) {
    printf("Memory allocation failure when calculating the pmf\n");
    return -1;
  }

  /*fill the string variables with a,b,c,... this string will be used in translate*/
  for (i = 0;i < n_of_variables;i++)
      variables[i] = 'a'+ i;
  variables[n_of_variables] = '\0';
  
  /*substitutes variable names by letters*/
  for (i = 0; i < n_of_variables; i++) {
      var_name[0] = 'a'+i;
      var_name[1] = '\0';
      strreplace (function, st_var_tb[i].var_name, var_name);
  }
  
  /*translate the function formula to f*/
  f = translate(function,variables,&leng,&error);
  
  if (error != -1) {
      printf("Bad function specification - %s\n",func);
      return -2;
  }
  
  if ((cond != NULL) && (strcmp(cond,"")) != 0) {
      if ( (condition = (char *) malloc (MAXSTRING)) == NULL) {
	  printf("Memory allocation failure\n");
	  return -1;
      }
      strcpy (condition,cond);
      
      /*replace the variable names by letters*/
      for (i = 0; i < n_of_variables; i++) {
	  var_name[0] = 'a'+i;
	  var_name[1] = '\0';
	  strreplace (condition, st_var_tb[i].var_name, var_name);
      }

      /*translate the condition formula to c*/    
      c = translate(condition,variables,&leng,&error);
    
      if (error != -1) {
	  printf("Bad condition specification - %s\n",cond);
	  return -2;
      }
  }
 
  k = 0;
  
  /*for each state in the table*/
  for(i = 0;i < n_of_states;i++) {
    
    /* look for an used field in the table */
    while (st_vec_tb[k].status == 0) 
      k++;
    
    /*substitutes the variable values of this state in formula*/
    for(j = 0;j < n_of_variables;j++)
      make_var(('a'+ j),st_vec_tb[k].desc[j]);
    
    /*verifies the condition*/
    if ((cond != NULL) && (strcmp(cond,"") != 0))
      is_true = fval_at (c);
    
    if (is_true) {
      
      /*compute the result*/
      result = fval_at (f);
      
      /*allocate memory to element that is inserted in the list*/
      if ( (p_int_vec = (struct interest_vec *) malloc (sizeof(struct interest_vec))) == NULL) {
	printf("Memory allocation failure when calculating the pmf\n");
	return -1; /* memory allocation failure */
      }
      
      if ( (p_int_vec->vars = (double *) malloc (sizeof(double))) == NULL) {
	printf("Memory allocation failure when calculating the pmf\n");
	return -1; /* memory allocation failure */
      }
      
      /*put the value of function and its probability in element*/
      p_int_vec->vars[0] = result;
      p_int_vec->prob = st_vec_tb[k].prob;
      p_int_vec->next = NULL;
      
      /*insert the element in the list*/
      insert_interest_vec (p_int_vec);
    }
    /*starts searching for the next used field in the table*/
    k++;
  }
  
  norm_cond_pmf (interest_vec_list);    
    
  return 0;
}

/* return values: 0->ok; -1->memory allocation failure; -2->not valid expression*/
int calculate_set_probability (char *set_desc, char *cond)
{
  formu                s,c;      /* pointer to formula (typedef of library formulc)*/
  int                  i,j,k;      /*counter and idetifier of each state*/
  char                *variables; /* string with the variables that is used in formula (exigence of formulc)*/
  int                  leng;      /* exigence of formulc */
  int                  error;     /* exigence of formulc */
  int                  state_is_in_set;    /*true when state is in the set */
  int                  condition_is_true = 1;  /*true when condition is true*/
  char                 var_name[2];    /*string with the letter to substitute the variable names in expression*/
  char                 set_description [MAXSTRING]; /*set_descripton with the variable names replaced by letters*/
  char                 *condition;     /*condition with variable names replaced by letters*/

  /*prepares the string set_description to handle the set_descrition with the letters a,b,c,...*/
  strcpy (set_description, set_desc);

  /*defines the word "not" as the boolean complement of an expression*/
  fnew ("not", (Func) fnot,1,0);

  /*replaces the variable names by letters*/
  for (i = 0; i < n_of_variables; i++) {
    var_name[0] = 'a'+i;
    var_name[1] = '\0';
    strreplace (set_description, st_var_tb[i].var_name, var_name);
  }

  /*tries to allocate memory to string varibles*/
  if ((variables = (char*) malloc (n_of_variables)) == NULL) {
    printf("Memory allocation failure when calculating the pmf\n");
    return -1;
  }
  
  /*fill the string variables with a,b,c,... this string is used in trenslate*/
  for (i = 0;i < n_of_variables;i++)
    variables[i] = 'a'+ i;
  variables[n_of_variables] = '\0';
  
  /*translate the formula to s*/
  s = translate(set_description,variables,&leng,&error);

  if (error != -1) {
    printf("Bad set description - %s\n", set_desc);
    return -2;
  }

  if ((cond != NULL) && (strcmp(cond,"") != 0)) {
      if ( (condition = (char *) malloc (MAXSTRING)) == NULL) {
	  printf("Memory allocation failure\n");
	  return -1;
      }
      strcpy (condition,cond);
      
      /*replace the variable names by letters*/
      for (i = 0; i < n_of_variables; i++) {
	  var_name[0] = 'a'+i;
	  var_name[1] = '\0';
	  strreplace (condition, st_var_tb[i].var_name, var_name);
      }

      /*translate the condition formula to c*/    
      c = translate(condition,variables,&leng,&error);
    
      if (error != -1) {
	  printf("Bad condition specification - %s\n",cond);
	  return -2;
      }
  }
  
  set_prob = 0;
  sum_prob = 0;
  
  k = 0;

  for (i = 0; i < n_of_states; i++) {
    
    /* look for an used field in the table */
    while (st_vec_tb[k].status == 0) 
      k++;
    
    /*substitutes the variable values of this state in formula*/
    for(j = 0;j < n_of_variables;j++)
      make_var(('a'+ j),st_vec_tb[k].desc[j]);
	
    /*compute the result*/
    state_is_in_set = fval_at (s);
    if ((cond != NULL) && (strcmp(cond,"") != 0))
	condition_is_true = fval_at (c);

    if ( (state_is_in_set) && (condition_is_true) )
      set_prob += st_vec_tb[k].prob;

    if (condition_is_true)
      sum_prob += st_vec_tb[k].prob;
    
    /*starts searching for the next used field in the table*/
    k++;   
  }
  
  if (sum_prob != 0)
      set_prob = set_prob / sum_prob;
  
  return 0;
}
