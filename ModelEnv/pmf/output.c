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

#include "pmf.h"

/* ---- GLOBAL VARIABLES in pmf.c---- */

extern struct st_var       *st_var_tb;                /* model's variables table */
extern struct st_vec       *st_vec_tb;                /* model's states and description table */
extern struct interest_vec *interest_vec_list;        /* head of list containing the pmf data */
extern int                  n_of_variables;           /* total number of variables in the model */
extern int                  n_of_states;              /* total number of states in the model*/
extern int                  n_of_interest_variables;  /* number of pmf variables choosen by the user */
extern double               sum_prob;                 /* sum of all probabilities of the list */
extern double               set_prob;                 /* probability of a set */
  

/*No return values*/
void print_interest_vec (struct interest_vec *pl, FILE *fd)
{
    struct interest_vec *temp;                /* temporary pointer to a member of the list */
    int                  i,j;                 /* counters */
    j = 1;

    /* Starts puting the values and probabilities */
    temp = pl;  
    while (temp != NULL) {
	if (n_of_interest_variables == 1) {

	   fprintf( fd, "%d\t", (int)temp->vars[0] );
	   fprintf( fd, "%.10e\n",temp->prob );
           fflush( fd );
	}
	else {
	    fprintf (fd, "%d\t", j);
	    fprintf(fd,"%.10e\t",temp->prob);

	    i = 0;
	    fprintf (fd, "#(");
	    while (i < n_of_interest_variables) {
		i++;
		fprintf (fd,"%.0f",temp->vars[i-1]);
		if (i < n_of_interest_variables)
		    fprintf(fd, ",");
	    }
	    fprintf (fd, ")\n");
	}
	j++;
        
	temp=temp->next;
    }
}

/* Returns 0 on success, -1 on error */
int print_variables_pmf_list ( char *modelname, struct interest_vec *pl, char *condition, char *measure_name)
{
    FILE                *fd;                  /* file descriptor */
    char                 filename[MAXSTRING]; /* output file name */
    int i,j;
    
    /*mounts the name of pmf_out file*/
    /* [Flavio, Kelvin - 14/12/99]   */
    /* Now modelname contains the prob file name, so We have to extract the */
    /* modelname from it.                                                   */

    strcpy( filename, modelname );

    strcat (filename, ".IM.");
    strcat (filename, measure_name);
    
    /*It tries to open the file .pmf_out to write*/
    if ( (fd = fopen(filename,"w+")) == NULL) {
	printf("Impossible to write data to %s\n",filename);
	return -1;
    }
    
    fprintf (fd, "#Measure of Interest: %s\n#PMF", measure_name);
    
    /* Completes the header */
    fprintf (fd, "[");
    for (i = 0;i < n_of_interest_variables;i++) {
	for(j = 0;j < n_of_variables;j++)
	    if (st_var_tb[j].order == (i+1)) {
		fprintf(fd,"%s",st_var_tb[j].var_name);
	    }
	if (i < (n_of_interest_variables-1)) 
	    fprintf(fd,",");
    }
    fprintf(fd,"]\n");
    
    if ((condition != NULL) && (strcmp(condition,"")!=0)) {
	fprintf (fd, "#Under condition %s\n", condition);

	fprintf(fd, "#Probability of \"%s\" be true in the model  = %.10e\n",condition, sum_prob);
	
	if (sum_prob == 0) {
	    fprintf (fd, "#NO PMF DATA - The condition you stabilished is never true in this model\n1\t0\n");
	    fclose(fd);
	    return -1;
	}
	
	if (n_of_interest_variables == 1) 
	    fprintf (fd,"#Conditional expected value  = %.10e\n",mean(interest_vec_list)); 
    }
    
    else 
	if (n_of_interest_variables == 1) 
	    fprintf (fd,"#Expected value  = %.10e\n",mean(interest_vec_list));

    print_interest_vec (pl, fd);
    
    fclose(fd);
    
    return 0;
}

/* Returns 0 on success, -1 on error */
int print_func_pmf_list ( char *modelname, struct interest_vec *pl, char *str_func, char *condition, char *measure_name)
{
  struct interest_vec *temp;                  /*temporary pointer to member of the list*/
  FILE                *fd;                    /*file descriptor*/
  char                 filename[MAXSTRING];   /*output file name*/
  
  /*mounts the name of pmf_out file*/
  /* [Flavio, Kelvin - 14/12/99]   */
  /* Now modelname contains the prob file name, so We have to extract the */
  /* modelname from it.                                                   */

  strcpy( filename, modelname );

  strcat (filename, ".IM."); 
  strcat (filename, measure_name);
  
  /*It tries to open the file .pmf_out to write*/
  if ( (fd = fopen(filename,"w+")) == NULL) {
    printf("Impossible to write data to %s\n",filename);
    return -1;
  }
  
  fprintf (fd, "#Measure of Interest: %s\n", measure_name);
  fprintf (fd, "#Function = %s\n", str_func);
  
  if ((condition != NULL) && (strcmp(condition,"") != 0)) {
    fprintf(fd, "#Under Condition: %s\n", condition);
    fprintf(fd, "#Probability of \"%s\" be true in the model  = %.10e\n",condition, sum_prob);
    
    if (sum_prob == 0) {
	fprintf (fd, "#NO PMF DATA - The condition you stabilished is never true in this model\n1\t0\n"); 
        fclose(fd);
	return -1;
    }
    fprintf(fd,"#Conditional expected value of function = %.10e\n",mean(interest_vec_list));
  }
  else
    fprintf(fd,"#Expected value of function = %.10e\n",mean(interest_vec_list));
  
  temp = pl;  
  while (temp != NULL) {
    fprintf(fd,"%.10e\t",temp->vars[0]);
    fprintf(fd,"%.10e\n",temp->prob);
    temp=temp->next;
  }
  
  fclose(fd);
  
  return 0;
}


/* Returns 0 on success, -1 on error */
int print_set_probability (char *modelname, char *set_description, char *condition, char *measure_name)
{
    FILE                *fd;                  /* file descriptor */
    char                 filename[MAXSTRING]; /* output file name */
  
    /*mounts the name of pmf_out file*/
    /* [Flavio, Kelvin - 14/12/99]   */
    /* Now modelname contains the prob file name, so We have to extract the */
    /* modelname from it.                                                   */

    strcpy( filename, modelname );

    strcat( filename, ".IM." );
    strcat( filename, measure_name );

    /*It tries to open the file .pmf_out to write*/
    if( (fd = fopen( filename, "w+")) == NULL )
    {
        printf( "Impossible to write data to %s\n", filename );
        return -1;
    }

    fprintf( fd, "#Measure of Interest: %s\n", measure_name );

    fprintf( fd, "#Set Description: %s\n", set_description );
    if( (condition != NULL) && (strcmp( condition, "" ) != 0) )
    {
        fprintf( fd, "#Under condition: %s\n", condition );
        if( sum_prob == 0 )
        {
            fprintf( fd, "#NO PMF DATA - The condition you stabilished is never true in this model\n");
            fprintf( fd, "#Conditional set probability not evaluable\n1\t0\n");
            fclose( fd );
            return -1;
        }
        fprintf( fd, "#Conditional Set Probability = %.10e\n", set_prob );
        fprintf( fd, "#Probability of \"%s\" be true in the model  = %.10e\n",
                 condition, sum_prob );
    }
    else
        fprintf( fd, "#Set Probability = %.10e\n", set_prob );

    fprintf( fd, "1\t%.10e\n", set_prob );

    fclose( fd );
    
    return 0;
}
