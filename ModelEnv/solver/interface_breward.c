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
#include <ctype.h>

#include "sparseMatrix.h"

#include "interface_breward.h"

double *get_initial_pi(FILE *, int);
double *get_intervals(FILE *, int *);
int     breward(char *, int, double, char *);



static int st_compare(const void *st1, const void *st2)
{ 
    TState *st_1, *st_2;

    st_1 = (TState *)st1;
    st_2 = (TState *)st2;

    if (st_1->rew < st_2->rew)
	return (1);
    if (st_1->rew > st_2->rew)
	return (-1);
    return(0);
}

Matrix *read_trans_matrix(char *modelname, char *method)
{
    char  filename[MAXSTRING];
    FILE   *fd;
    Matrix *Q;
    
    sprintf(filename, "%s.generator_mtx", modelname);
    if (!(fd = fopen(filename, "r")) ) {
        fprintf(stderr, "%s: Error while opening file %s\n", method, filename);
        return(NULL);
    }

    Q = get_trans_matrix(fd);

    fclose(fd);
    /* printf("Transition matrix read\n"); */

    return(Q);
}
TState *read_reward_vec(char *modelname, char *rewardname, int no_states, char *method)
{
    char    filename[MAXSTRING];
    FILE   *fd;
    TState *st_vec;
    int     i, st, status;
    float   val;
    char    line[MAXSTRING];

    sprintf(filename, "%s.rate_reward.%s",modelname, rewardname);
    if (!(fd = fopen(filename, "r")) ) {
        fprintf(stderr, "%s: Error while opening file %s\n", method, filename);
        return(NULL);
    }

    fgets(line, MAXSTRING, fd);

    st_vec = (TState *) malloc ( (no_states + 1) * sizeof (TState));
    if (st_vec == NULL) {
        fprintf(stderr, "%s: Cannot allocate memory\n", method);
	perror ("malloc");
	return (NULL);
    }
    for (i=0; i<=no_states; i++) {
	st_vec[i].no = i;
	st_vec[i].rew = 0;
    }

    while (!feof(fd)) {
	status = fscanf(fd, "%d %f\n", &st, &val);
	if (status != 2) {
	    /* perror("fscanf"); */
	    fprintf (stderr, "%s: Invalid file format.\n", method);
	    return(NULL);
	}
	st_vec[st].rew = val;       
    }

    fclose(fd);
    /* printf ("Reward vector read.\n"); */

    return(st_vec);
}

int *reorder_states(int no_states, TState *st_vec, char *method)
{
    int *st_map;
    int  i;

    qsort((TState *) &(st_vec[1]), no_states, sizeof(TState), st_compare);

    st_map = (int *) malloc ( (no_states + 1) * sizeof (int));
    if (st_map == NULL) {
        fprintf(stderr, "%s: Cannot allocate memory\n", method);
	perror ("malloc");
	return (NULL);
    }
    for (i=1; i<=no_states; i++) {
	st_map[st_vec[i].no] = i;
    }

    return(st_map);
}

int write_trans_matrix(FILE *fd, Matrix *Q, int *st_map)
{
    int    i, j;
    double val;
    
    fprintf(fd, "%d\n", Q->num_col);
    for (i=1; i<=Q->num_col; i++)
	for (j=1; j<=Q->num_col; j++) {
	    val = get_matrix_position (i, j, Q);
	    if (val > 0)
		fprintf (fd, "%d %d %.10e\n", st_map[i], st_map[j], val);
	}
    fprintf(fd, "-1 0 0\n");
    /* printf ("Transition matrix written.\n"); */

    return (1);
}

TRew *create_rew_vec(int no_states, TState *st_vec, int *Nrew, char *method)
{
    TRew  *rew_vec;
    int    i, no_rew;
    double last_rew;

    rew_vec = (TRew *) malloc ( (no_states + 1) * sizeof (TRew));
    if (rew_vec == NULL) {
        fprintf(stderr, "%s: Cannot allocate memory\n", method);
	perror ("malloc");
	return (NULL);
    }

    no_rew = 1;
    last_rew = st_vec[1].rew;
    rew_vec[no_rew].rew = last_rew;
    for (i=2; i<=no_states; i++) {
	if (st_vec[i].rew != last_rew) {
	    rew_vec[no_rew].last_st = i-1;
	    no_rew++;
	    last_rew = st_vec[i].rew;
	    rew_vec[no_rew].rew = last_rew;
	}
    }
    rew_vec[no_rew].last_st = i-1;

    *Nrew = no_rew;
    return (rew_vec);
}

int write_rew_vec(FILE *fd, int no_rew, TRew *rew_vec)
{
    int i;

    fprintf (fd, "%d\n", no_rew);
    for (i=no_rew; i>=1; i--)
	fprintf (fd, "%.8e\n", rew_vec[i].rew);
    
    /* printf ("Reward vector written.\n"); */

    for (i=1; i<=no_rew; i++)
	fprintf (fd, "%d\n", rew_vec[i].last_st);

    /* printf ("Last state vector written.\n"); */

    return (1);
}

int append_initial_probs(FILE *fd1, char *modelname, int no_states, int *st_map, char *method)
{
    char    filename[MAXSTRING];
    FILE   *fd2;
    double *pi;
    int     i;

    sprintf(filename, "%s.init_prob", modelname);
    if (!(fd2 = fopen(filename, "r")) ) {
        fprintf(stderr, "%s: Error while opening file %s\n", method, filename);
        return(-1);
    }
    
    pi = get_initial_pi(fd2, no_states);

    if (pi == NULL) {
        fprintf(stderr, "%s: Invalid initial probability description\n", method);
        return(-2);
    }

    fclose (fd2);
    
    for (i=1; i<=no_states; i++) {
	if (pi[i-1] > 0)
	    fprintf (fd1, "%d %.8e\n", st_map[i], pi[i-1]);
    } 
    fprintf (fd1, "-1 0.0\n");
    return(1);
}

int append_intervals(FILE *fd, char *modelname, char *method)
{
    char    filename[MAXSTRING];
    FILE   *fd2;
    double *intervals;
    int     num_intervals, i;
    

    /* reads intervals vector */
    sprintf(filename, "%s.intervals", modelname);
    if (!(fd2 = fopen(filename, "r")) ) {
        fprintf(stderr, "%s: Error while opening file %s\n", method, filename);
        return(-1);
    }

    intervals = get_intervals(fd2, &num_intervals);

    if (intervals == NULL) {
        fprintf(stderr,"%s: Invalid intervals description\n", method);
        return(-2);
    }

    fclose(fd2);

    fprintf(fd, "%d\n", num_intervals);
    for (i=0; i<num_intervals; i++)
	fprintf (fd, "%.8e\n", intervals[i]);

    return(1);
}

int append_reward_levels(FILE *fd, char *modelname, char *rewardname, char *method)
{
    char    filename[MAXSTRING];
    FILE   *fd2;
    int     i, no_levels;
    float   val;

    sprintf(filename, "%s.reward_levels.%s", modelname, rewardname);
    if (!(fd2 = fopen(filename, "r")) ) {
        fprintf(stderr, "%s: Error while opening file %s\n", method, filename);
        return(-1);
    }

    fscanf(fd2, "%d\n", &no_levels);
    fprintf (fd, "%d\n", no_levels);
    for (i=0; i<no_levels; i++) {
	fscanf(fd2, "%f", &val);
	fprintf(fd, "%.8e\n", val);
    }
    fclose (fd2);

    return (1);
}
int append_reward_bounds(FILE *fd, char *modelname, char *rewardname, char *method)
{
    char    filename[MAXSTRING];
    FILE   *fd2;
    char    line[MAXSTRING];

    sprintf(filename, "%s.rate_reward.%s", modelname, rewardname);
    if (!(fd2 = fopen(filename, "r")) ) {
        fprintf(stderr, "%s: Error while opening file %s\n", method, filename);
        return(-1);
    }

    fgets(line, MAXSTRING, fd2);
    fputs(line, fd);
    
    fclose(fd2);

    return(1);
}

int interface_breward (char *modelname, char *rewardname, int option, double precision)
{

    TState *st_vec;
    TRew   *rew_vec;
    Matrix *Q;
    int    *st_map;

    char  filename[MAXSTRING];
    FILE *fd;
    int   no_rew;
    char  method_name[100];
    int   status;

    if (option == 8)
        strcpy(method_name, "Bounded Cumulative Reward");
    if (option == 9)
        strcpy(method_name, "Fraction of time the accumulated reward is above a level");

    if ((Q = read_trans_matrix(modelname, method_name)) == NULL)
	return (30);

    if ((st_vec = read_reward_vec(modelname, rewardname, Q->num_col, method_name)) == NULL)
	return (250);

    if ((st_map = reorder_states(Q->num_col, st_vec, method_name)) == NULL)
	return(40);

    sprintf(filename, "%s.reward_input", modelname);
    if (!(fd = fopen(filename, "w")) ) {
        fprintf(stderr, "%s: Error while opening file %s\n", method_name, filename);
        return(31);
    }

    if (write_trans_matrix(fd, Q, st_map) < 0)
	return (31);

    if ((rew_vec = create_rew_vec(Q->num_col, st_vec, &no_rew, method_name)) == NULL)
	return(40);

    if (write_rew_vec(fd, no_rew, rew_vec) < 0)
	return(33);
    
    if ((status = append_initial_probs(fd, modelname, Q->num_col, st_map, method_name)) < 0)
    {
	if (status == -1)
	    return(30);
	if (status == -2)
	    return(55);
    }

    if ((status = append_intervals(fd, modelname, method_name)) < 0)
    {
	if (status == -1)
	    return(30);
	if (status == -2);
	    return(59);
    }

    if (append_reward_levels(fd, modelname, rewardname, method_name) < 0)
	return(30);

    if (append_reward_bounds(fd, modelname, rewardname, method_name) < 0)
	return(30);

    fclose(fd);

    if (option == 8) 
       return (breward(modelname, 1, precision, method_name));
    else if (option == 9)
       return (breward(modelname, 2, precision, method_name));
    return (-1);
}

