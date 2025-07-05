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

/* ---- GLOBAL VARIABLES declared in pmf.c ---- */

extern struct st_var       *st_var_tb;                /* model's variables table */
extern struct st_vec       *st_vec_tb;                /* model's states and description table */
extern int                  n_of_variables;           /* total number of variables in the model */
extern int                  n_of_states;              /* total number of states in the model*/


/* ------ AUXILIARY FUNCTIONS ------ */

/* This routine is used to sort the variable names according to their length. */
/* After the sort, the greater strings will appear first.                     */
/**
static int compVarLength( const void *ptr1, const void *ptr2 )
{
    return( strlen(((struct st_var *)ptr2)->var_name) - 
            strlen(((struct st_var *)ptr1)->var_name) );
}
**/

/* ---- FILE HANDLING FUNCTIONS ---- */

/*return values: 0->ok,not EOF; -1->ok,EOF*/
int read_line_vstat (FILE *fd, struct st_var *st) 
{
    int trash; /* to keep the number beside the variable name (not used)*/
    int c; /* to assume the character code*/
    
    switch (fscanf(fd,"%s %d",st->var_name, &trash)) {
	
    case EOF:
	return -1; /*ok, end of file*/
	
    case 1: 
	if (st->var_name[0] == '#') {
	    c = fgetc(fd);
	    while ((c != '\n') && (c != EOF)) {
		c = fgetc(fd);
	    }
	}
	return 0; /*ok, in spite of the lack of the number*/
	
    case 2:
	if (st->var_name[0] == '#') {
	    c = fgetc(fd);
	    while ((c != '\n') && (c != EOF)) {
		c = fgetc(fd);
	    }
	}
	return 0; /*ok, the name and the number was readen*/
    }
    return( -2 );
}

/*return values: 0->ok, not EOF; -1->ok,EOF; -2->file format error;
-3->memory allocation failure; -4-> incompatibility between files*/
int read_line_states (FILE *fd, struct st_desc *state)
{
    int i; /* to reference the description values */
    int c; /* code to character */
    
    /*Searches for the first occurrence of a number followed by a '('*/
    while ( fscanf (fd,"%d  ",&(state->state)) !=1 ) {
	/*while this segment isn't found, all characters are skiped*/
	c = fgetc(fd);
	/*if there is a #, the rest of the line is a comment*/
	if (c == '#') {
	    while ((c!='\n') && (c!=EOF))
		c = fgetc(fd);
	}
	/*if there is an EOF, the function returns -1*/
	if (feof(fd))
	    return -1; /*end of file*/
    }
    if ((state->desc = (int *) malloc (n_of_variables*sizeof(int))) ==NULL) {
	printf("Memory allocation failure\n");
	return -3;
    }
    /*Searches for n_of_variables numbers between parenthesis*/
    for (i = 0; i < n_of_variables; i++) {
	
	while ( fscanf(fd,"%d",&(state->desc[i]))!=1 ) {
	    /*while the number isn't found, skips all characters*/
	    c = fgetc (fd);
	    /*if there is a # inside a description -> error */
	    if (c == '#') {
		printf("File format error - comment inside the description of state %d\n", state->state);
		return -2;
	    }
	    /*if there is an EOF, the function returns error*/
	    if (feof(fd)) {
		printf("File format error - bad description on state %d\n",state->state);
		return -2; /*file format error - there is an EOF in the middle of a description*/
	    }
	    /*stops if a line doesn't have n_of_variables variables (less)*/
	    if (c == ')') {
		printf("Incompatibility - more variables in .vstat file than in .states file in state %d\n", state->state);
		return -4;
	    }    
	}
    }
    /*stops if a line doesn't have n_of_variables variables (more)*/
    c = fgetc(fd);
    if (c == ',') {
	printf("Incompatibility - less variables in .vstat file than in .states file in state %d\n", state->state);
	return -4;
    }
    
    return 0; /*ok*/
}

/*return values: 0->ok,not EOF; -1->ok, EOF*/
int read_line_pi (FILE *fd, struct pi_desc *pi)
{
    int c; /*the character code*/
    
    while (fscanf(fd,"%d",&(pi->state))!=1) {
	c = fgetc(fd);
	/*if there is a #, the rest of the line is a comment */
	if (c == '#') {
	    while ((c!='\n') && (c!=EOF))
		c = fgetc(fd);
	}
	if (feof(fd)) {
	    return -1; /*end of file*/
	}
    }
    while (fscanf(fd,"%lf",&(pi->prob))!=1) {
	c = fgetc(fd);
	/*if there is a #, between state and probability -> error */
	if (c == '#') {
	    printf("File format error - comment between the state number %d and his probability\n", pi->state); 
	    return -2;
	}
	if (feof(fd)) {
	    return -1; /*end of file*/
	}
    }
    return 0; /*OK*/
}

/*return values: 0->ok; -1->error opening file*/
int read_vstat (char *filename)
{
    FILE         *fd;            /* file descriptor */
    struct st_var this_variable; /* keeps each variable name */
    int           i = 0;         /* It counts the number of variables in the model */
    
    /* It tries to open the file */
    if  ((fd = fopen (filename, "r"))==NULL) {
	printf("Error openning file %s\n", filename);
	return -1; /*error opening file*/
    }
    
    /* while the file isn't over */
    while ( read_line_vstat(fd, &this_variable) == 0 ) { 
	/* puts the variable name in table unless the line is a comment*/
	if (this_variable.var_name[0]!='#') {
	    strcpy(st_var_tb[i].var_name, this_variable.var_name);
	    i++;
	}
    }
    
    /*the file is over*/
    fclose(fd);
    /* the number of variables in the model is known*/
    n_of_variables = i; 

/*    qsort( st_var_tb, n_of_variables, sizeof( struct st_var ), compVarLength ); */

    return 0; /*ok*/
    
}
 
/*return values: 0->ok; -1->error opening file; -2->file format error; -3->memory allocation failure; -4-> incompatibility between files -5->overflow*/
int read_states (char *filename)
{
    FILE          *fd;         /* file descriptor */
    struct st_desc this_state; /* keeps the number and description of each state*/
    int            i,j;        /* counters */
    int            rl;         /*return value of read_line function*/

    /*It tries to open the file*/
    if ((fd = fopen (filename, "r"))==NULL) {
	printf("Error openning file %s\n", filename);
	return -1; /*error opening file*/
    }
    
    for (i=0;i<n_of_states;i++) {
      st_vec_tb[i].status = 0;
      st_vec_tb[i].prob = UNUSED;
    }

    /* i (the number of different states already readen) starts with 0 */
    i = 0;
    
    /* while the file isn't over and there is no error*/
    while ((rl = read_line_states (fd, &this_state)) == 0) { 
      
      if (this_state.state>n_of_states) {
	printf("Error - In %s there is a state number (%d) greater than the total number of states (%d)\n", filename,this_state.state, n_of_states);
	return -5; /* overflow */
      } 
    
      st_vec_tb[this_state.state].state = this_state.state;
      
      /*state's description is stored */
      if ( (st_vec_tb[this_state.state].desc = (int *) malloc (n_of_variables*sizeof(int))) == NULL) {
	printf("Memory allocation failure\n");
	return -3;  /*memory allocation failure*/
      }
      
      for (j = 0;j < n_of_variables;j++)
	st_vec_tb[this_state.state].desc[j] = this_state.desc[j];
      
      /*the state probability is set to unused*/
      st_vec_tb[this_state.state].prob = UNUSED;
      
      /* i (the number of different states already readen) increases if this state is found by the first time*/
      if (st_vec_tb[this_state.state].status == 0)
	i++;  
      
      /* the state status is set to 1(already assigned) */ 
      st_vec_tb[this_state.state].status = 1;
      
    }
    
    /*at this point, the file is over or there is an error*/
    fclose (fd);
    
    if (rl == -1) {  /* end of file */
//	n_of_states = i;
	return 0; /*ok*/
    }
    
    /*at this point, there is an error*/
    printf("in file %s\n",filename);
    return rl;  /*file format error*/
}

/*return values: 0->ok; -1->error opening file; -2->file format error; -4->incompatibility*/
int read_pi (char *filename)
{
  FILE          *fd;                 /* file descriptor */
  struct pi_desc this_state;         /* keeps each state number and probability readen in file*/
  int            i = 0;              /* counts the different states readen */
  int            rl;                 /* return value of read_line function*/
  int            k;                  /*counter to search for states without a probability*/
  
  /* It tries to open the file */
  if  ((fd = fopen (filename, "r"))==NULL) {
    printf("read_pi: Error openning file %s\n", filename);
    return -1; /*error opening file*/
  }
  
  /* while the file isn't over*/
  while ((rl = read_line_pi (fd, &this_state)) == 0) {
    
    if (this_state.state>n_of_states) {
      fprintf( stderr, "read_pi: Error - In %s there is a state number (%d) greater than the total number of states (%d)\n", filename,this_state.state, n_of_states);
      return -4; /* overflow */
    } 
    
    /* the probability is stored */
    st_vec_tb[this_state.state].prob = this_state.prob;
    
    /*if the current element of st_vec_tb is valid, i (the number of states already read) increases */
    if (st_vec_tb[this_state.state].status == 1)
      i++;
    else
      printf("Warning: state %d not descripted in .states file\n",this_state.state);
  }
  
  /*at this point, the file is over or there is an error */
  fclose(fd); 
  
  if (rl == -1) { /* end of file */
    /* if there are less probabilities in .stdy_state_probs than descripted states*/
    if (i < n_of_states) {
      for (k = 0; k < n_of_states; k++) {
	if ( (st_vec_tb[k].status == 1) && (st_vec_tb[k].prob == UNUSED) )
	  printf("Missing pi[%d] in %s\n",k,filename);
      }
      /*There is an imcompatibility between .states and .SSsolution.<method> file*/
      return -4; 
    }
    return 0; /*ok*/
  }
  return -2; /* file format error */
}

/*return values: 0->ok; -1->error openning file; -2->file format error; -3->memory allocation failure; -4->incompatibility */
int read_files (char *base_name, char *method_name)
{
    char vstat_name[MAXSTRING];  /*the name of .vstat file*/
    char states_name[MAXSTRING]; /*the name of .states file*/
    char pi_name[MAXSTRING];     /*the name of .stdy_state_probs file*/
    int  rv = 0;                 /*the return value of the functions: read_vstat, read_states, read_pi*/
    
    /*mounts the name of vstat file*/
    strcpy (vstat_name, base_name); 
    strcat (vstat_name, ".vstat");

    /*tries to read the vstat file*/
    if ((rv=read_vstat(vstat_name)) != 0)
	return rv;
    /*mounts the name of states file*/
    strcpy (states_name, base_name);
    strcat (states_name, ".states");
    
    /*tries to read the states file*/
    if ((rv=read_states(states_name)) != 0)
	return rv;

    /*mounts the name of stdy_state_probs file*/
    if( strlen( method_name ) > 2 )
    {
        strcpy (pi_name, method_name);
    }
    else
    {
        strcpy (pi_name, base_name);
        strcat( pi_name, ".stdy_state_probs" );
    }

    /*tries to read the stdy_state_probs file*/
    rv=read_pi(pi_name);

    return rv;
}   
