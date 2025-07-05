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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pmf.h"

/* ---- GLOBAL VARIABLES declared in pmf.c ---- */

extern struct st_var       *st_var_tb;                /* model's variables table */
extern struct st_vec       *st_vec_tb;                /* model's states and description table */
extern int                  n_of_variables;           /* total number of variables in the model */
extern int                  n_of_states;              /* total number of states in the model*/
extern int                  n_of_interest_variables;  /* number of pmf variables choosen by the user*/
extern struct interest_vec *interest_vec_list;        /* the head of the pmf list */
extern double sum_prob;                               /* the sum of all probabilities */

/* ---- BASIC TOOLS ---- */

/*Return value: the complement*/
double fnot( double a )
{
    return !(a);
}

int isEmpty(char ch)
{
   return !(isalnum(ch) || (ch=='_') || (ch=='.'));
}

int pos_is_token(char *orig, char *str, char *pos, char *end)
{
   return ( ((str == orig) || (isEmpty(*(pos-1)))) && (isEmpty(*(end))) );
}

/*No return values*/
void strreplace(char *s1, char *sub, char *repl)
{
  char  s2[MAXSTRING], *ps1;
  char *pos1, *pos2, istoken;

  ps1 = s1;
  istoken = 0;
  do {
     pos1 = strstr(ps1, sub);
     if (pos1 != NULL) 
     {
        istoken = pos_is_token(s1,ps1,pos1,pos1+strlen(sub));
	     ps1 = pos1 + strlen(sub);
     }
  } while ((!istoken) && (pos1 != NULL));

  ps1 = pos1;
  
  while ((pos1 != NULL) && (istoken)) {
     memcpy (s2, ps1, (pos1 - ps1) );
     pos2 = s2 + (pos1 - ps1);
     pos1 = pos1 + strlen(sub);
     memcpy (pos2, repl, strlen(repl) );
     pos2 = pos2 + strlen(repl);
     memcpy (pos2, pos1, strlen(pos1));
     pos2 = pos2 + strlen(pos1);
     *pos2 = '\0';
     strcpy (ps1, s2);
     /* search for the next substring */
     istoken = 0;
     do 
     {
        pos1 = strstr(ps1, sub);
 	     if (pos1 != NULL) 
	     {
           istoken = pos_is_token(s1,ps1,pos1,pos1+strlen(sub));
	        ps1 = pos1 + strlen(sub);
	     }
     } while ((!istoken) && (pos1 != NULL));

     ps1 = pos1;
  }
}

/* return values: True->str can't be a condition; False->str can be a condition */
int cant_be_condition (char *str)
{
    int i = 0;
    
    while (str[i]!='\0') {
	
	if ((str[i]=='>') || (str[i]=='<') || (str[i]=='='))
	    return 0;
	i++;

    }
    return 1;
}

/* return values: 0->ok; -1->error openning file; -2->file format error  -3->memory allocation failure;*/
int allocate_tables(char *base_name)
{
    FILE *fd;
    char vstat_name[MAXSTRING];
    char states_name[MAXSTRING];
    int c;

    n_of_variables = 0;
    n_of_states = 0;

    /* mounts the name of vstat file */
    strcpy( vstat_name, base_name );
    strcat( vstat_name, ".vstat" );
  
    /* It tries to open the vstat file */
    if( (fd = fopen( vstat_name, "r" )) == NULL )
    {
        printf( "Error openning file %s\n", vstat_name );
        return -1; /*error opening file*/
    }

    c = fgetc( fd );  
    /* count the lines of file */
    while( c != EOF )
    {
      c = fgetc( fd );
      if( c == '\n' )
          n_of_variables++;
    }
  
    fclose( fd );
  
    /*mounts the name of states file*/
    strcpy( states_name, base_name );
    strcat( states_name, ".states" );
  
    /* It tries to open the states file */
    if( (fd = fopen( states_name, "r" )) == NULL )
        return -1; /*error opening file*/
  
    c = fgetc( fd );
    /* count the lines of file */
    while( c != EOF )
    {
        c = fgetc( fd );
        if( c == '\n' )
            n_of_states++;
    }

    fclose( fd );

    /* allocate memory to st_var_tb and st_vec_tb*/
    if( (st_var_tb = (struct st_var *)malloc( (n_of_variables+5) * sizeof(struct st_var))) == NULL)
    {
        printf( "No memory to store the variables's names\n" );
        return -3; /*memory allocation error*/
    }

    if( (st_vec_tb = (struct st_vec *)malloc( (n_of_states+5) * sizeof(struct st_vec))) == NULL)
    {
        printf("No memory to store the states's descriptions and probabilities\n");
        return -3; /*memory allocation error*/
    }

    return 0;
}



/* return values: -1->a<b; 0->a=b; 1->a>b */
int vector_cmp (double *a, double *b, int n)
{
    int i = 0;
    while (i < n) {
	if(a[i]<b[i]) return -1;
	if(a[i]>b[i]) return 1;
	i++;  
    }
    return 0;
}

/*return value: the expected value of the choosen variable or function */
double mean (struct interest_vec *pmf)
{
    struct interest_vec *temp = pmf; /* temporary pointer to member of the list */
    double               m = 0;      /* expected value */
    
    /*calculate the expected value looking in the pmf list*/
    while (temp!=NULL) {
	m += temp->vars[0] * temp->prob;
	temp = temp->next;
    }
    
    return m;   
}

/* No return values */
void free_interest_vec (struct interest_vec *pl)
{
    struct interest_vec *temp; /*temporary pointer to member of the list*/
    struct interest_vec *next;  /*temporary pointer to member after temp*/
    
    /*free the memory used by the pmf list*/

    for (temp = pl; temp != NULL; temp = next) {
	next = temp->next;
	free (temp->vars);
	free (temp);
    }
    
}

/* No return value*/
void insert_interest_vec (struct interest_vec *int_vec)
{           
    struct interest_vec *temp;       /* temporary ptr to a member of the list*/
    struct interest_vec *ant = NULL; /* tmp ptr to member behind the member pointed by temp */
    
    /*temp starts with the head of the list interest_vec_list (global variable) */
    temp = interest_vec_list;   
    
    /*for each member in list*/
    while( temp != NULL )
    {
        /*compairs the element to be inserted to member in list*/
        switch (vector_cmp (temp->vars, int_vec->vars, n_of_interest_variables))
        {
            /*if the element is greater than the member*/
            case -1:
                /*goes to next member*/
                ant = temp;
                temp = temp->next;
                break;
            /*if the element is equal to member*/
            case 0:
                /*the element probability is added to the member probability*/
                temp->prob += int_vec->prob;
                return;
            /*if element is lower than member*/
            /*element will be inserted behind member*/
            case 1:
                if (ant != NULL) /*the element goes in the middle*/
                {
                    ant->next = int_vec;
                    int_vec->next = temp;
                    return;
                }
                else  /*the element is the new first*/
                {
                    int_vec->next = interest_vec_list;
                    interest_vec_list = int_vec;
                    return;
                }
        }
    }
    
    if( ant == NULL )
        interest_vec_list = int_vec; /*1st element in the list, 1st call to the function insert*/
    else
        ant->next = int_vec; /*last element int the list*/
}

/* return value: the sum of the probabilities*/
void get_sum_prob (struct interest_vec *pl)
{
    struct interest_vec *temp;    /*temporary pointer to member of the list*/
    
    sum_prob = 0;   /*sum of probabilities*/
    
    temp = pl;
    while (temp != NULL) {
	sum_prob += temp->prob;
	temp = temp->next;
    }
}

/* No return values */
void fill_int_vec (struct interest_vec *p_int_vec, struct st_vec this_state)
{
    int j; /* counter */
    
    /*fill the element with values of interest*/
    
    for (j = 0; j < n_of_variables; j++)	
	if (st_var_tb[j].order>0)
	    p_int_vec->vars[st_var_tb[j].order-1] = this_state.desc[j];
    
    p_int_vec->prob = this_state.prob;
    p_int_vec->next = NULL;
}

/* No return values */
void norm_cond_pmf (struct interest_vec *pl)
{
    struct interest_vec *temp;  /*temporary pointer to a member of the list*/
    
    get_sum_prob(interest_vec_list);
    temp = pl;
    while (temp != NULL) {
	temp->prob = temp->prob/sum_prob;
	temp = temp->next;
    }
} 

/*No return values*/
void free_all()
{
    free (st_var_tb);
    st_var_tb = NULL;
    free (st_vec_tb);
    st_vec_tb = NULL;
    free_interest_vec(interest_vec_list);
    interest_vec_list = NULL;
}

/*return values: 0->ok; -2->wrong variable name*/
int get_usr_variables (char *var_list)
{
    int i,j;  /*counters*/
    char *this_variable;

    for (i = 0; i < n_of_variables;i++) 
	st_var_tb[i].order = 0;
    
    n_of_interest_variables = 0;

    /*start searching variables in the command line*/
    /* get the first variable */

    this_variable = strtok(var_list, "@");
    /*this_variable handles token value (in this case: 1st_variable 2nd_variable 3rd_variable NULL)*/

    /* go through all variables in variable list */
    while (this_variable != NULL) {
	/*look for the word read in the variables table*/
	for (j = 0;j < n_of_variables;j++) 
	    if ( strcmp (st_var_tb[j].var_name,this_variable) == 0 ) {
		if ( st_var_tb[j].order == 0 ) {
		    n_of_interest_variables++;
		    st_var_tb[j].order = n_of_interest_variables;
		}
		/*stop searching in variables table, when the word is found*/
		break; 
	    }
	    else
		/*if some word wasn't found in the variables table (user error)*/
		if (j == (n_of_variables-1)) {
		    printf("Bad variable name - %s\n", this_variable);
		    
		    printf("The variables of the model are:\n");
		    for (i = 0; i < n_of_variables; i++)
			printf("%s\n",st_var_tb[i].var_name);
		    /* the program is aborted */
		    return -2; 
		} 
	/* get the next selected variable */
	this_variable = strtok(NULL, "@");
    }
    
    return 0;
}

/*No return value*/
void usage ()
{
    printf("Possible usages:\n\n");
    printf("To calculate the pmf of one or more variables type:\n");
    printf("pmf <modelname> <modelname>.<ss/ts>.<solution method> {-o <output file extension>} -v[c] <1st variable 2nd variable...> [\"condition\"]\n\n");
    printf("To calculate the pmf of a function type:\n");
    printf("pmf <modelname> <modelname>.<ss/ts>.<solution method> {-o <output file extension>} -f[c] <\"function\"> [\"condition\"]\n\n");
    printf("To calculate the probability of a set type:\n");
    printf("pmf <modelname> <modelname>.<ss/ts>.<solution method> {-o <output file extension>} -s[c] <\"set_description\"> [\"condition\"]\n\n");
}
