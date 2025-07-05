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

#define MAXSTRING 255      /*fixed string size*/
#define UNUSED 2           /*value to mark the states that doesn't have a probability in the pi file*/

/* ---- STRUCTS ---- */

/* Struct for the .vstat file, member of vstat table */

struct st_var {
    char var_name[MAXSTRING]; /* the name of the variables */
    int  order;               /*0 is not selected, 1 is first, 2 is second,...*/
};

/* State's descriptions & probabilities table */

struct st_vec {  
  int    state;  /* the number of the state */
  int    status; /* flag that says if the state has been already assingned 0->no, 1->yes*/
  int   *desc;   /* the description of this state (a vector alocated later)*/
  double prob;   /* the probability of this state */
};

/* Interest measures list member (the pmf data)*/

struct interest_vec {
    double              *vars;  /* values of the variables selected by the user (alocated later) */
    double               prob;  /* probability of the values */
    struct interest_vec *next;  /* pointer to next element of the list */
    
};

/* Struct for the .states file */

struct st_desc {
    int  state;        /* number that identifies the state */
    int *desc; /* state description */
};

/* Struct for the .stdy_state_prob file */

struct pi_desc {
    int    state; /* number that identifies the state */
    double prob;  /* probability of this sate */
};

/* ---- FUNCTION PROTOTYPE ---- */

/* ---- BASIC TOOLS ---- */

/*Return value: the complement of a*/
double fnot (double a);

/* return values: True->str cant be a condition; False->str can be a condition */
int cant_be_condition (char *str);

/* return values: 0->ok; -1->memory allocation failure*/
int allocate_tables (char *base_name);

/*return values: -1->a<b; 0->a=b; 1->a>b*/
int vector_cmp (double *a, double *b, int n);

/*return values: the expected value of the choosen variable or function*/
double mean (struct interest_vec *pmf);

/*No return values*/
void free_interest_vec (struct interest_vec *pl);

/*No return value*/
void insert_interest_vec (struct interest_vec *int_vec);

/*No return value -> the sum of all probabilities in the list in stored in the global variable set_prob*/
void get_sum_prob (struct interest_vec *pl);

/*No return values*/
void fill_int_vec (struct interest_vec *p_int_vec, struct st_vec this_state);

/*No return values*/
void free_all();

/*No return values*/
void norm_cond_pmf (struct interest_vec *pl);

/* return values: 0->ok; -2->wrong variable name*/
int get_usr_variables (char *var_list);

/*No return value*/
void usage ();


/* ---- FILE HANDLING FUNCTIONS ---- */

/*return values: 0->ok,not EOF; -1->ok,EOF*/
int read_line_vstat (FILE *fd, struct st_var *st); 

/*return values: 0->ok, not EOF; -1->ok,EOF; -2->file format error; -3->memory allocation failure; -4->incompatibility between files*/
int read_line_states (FILE *fd, struct st_desc *state);

/*return values: 0->ok,not EOF; -1->ok, EOF*/
int read_line_pi (FILE *fd, struct pi_desc *pi);

/*return values: 0->ok; -1->error opening file*/
int read_vstat (char *filename);
 
/*return values: 0->ok; -1->error opening file; -2->file format error; -3->memory allocation failure -4->incompatibility between files; -5->overflow*/
int read_states (char *filename);

/*return values: 0->ok; -1->error opening file; -2->file format error; -4->incompatibility*/
int read_pi (char *filename);

/*return values: 0->ok; -1->error openning file; -2->file format error; -3->memory allocation failure; -4->incompatibility */
int read_files (char *base_name, char *method_name);


/* ---- OUTPUT FUNCTIONS ---- */

/*No return values*/
void print_interest_vec (struct interest_vec *pl, FILE *fd);

/*No return value*/
void print_variables_pmf_list ( char *modelname, struct interest_vec *pl, char *condition, char *measure_name);

/*No return value*/
void print_func_pmf_list ( char *modelname, struct interest_vec *pl, char *str_func, char *condition, char *measure_name);

/* No return values */
void print_set_probability ( char *modelname,  char *set_description,  char *condition,  char *measure_name);


/* ---- CALCULATE FUNCTIONS ---- */

/*return values: 0->ok; -1->memory allocation failure; -2->wrong variable name */
int calculate_variables_pmf (char *var_list, char *cond);

/*return values: 0->ok; -1->memory allocation failure; -2->not valid expression*/
int calculate_pmf_of_function (char *func, char *cond);

/*return values: 0->ok; -1->memory allocation failure; -2->not valid expression*/
int calculate_set_probability (char *set_desc, char *cond);


/* ---- MAIN FUNCTIONS ---- */

/* ATTENTION return values: 1->ok; -1->memory allocation failure; -2->wrong variable;
name; ATTENTION */
int generate_variables_pmf (char *basename, char *var_list, char *real_condition, char *measure_name);

/* ATTENTION return values: 1->ok; -1->memory allocation failure; -2->not valid
expression; ATTENTION*/
int generate_function_pmf (char *basename, char *real_function, char *real_condition, char *measure_name);

/* ATTENTION return values: 1->ok; -1->memory allocation failure; -2->not valid 
expression; ATTENTION*/
int generate_set_probability (char *basename, char *set, char *condition, char *measure_name);

/* THE EXPORTED FUNCTIONS'S PROTOTYPES ARE IN PmfInterface.H */
