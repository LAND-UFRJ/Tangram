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
#ifdef _SOLARIS_
#include <strings.h>
#endif
#include <stdlib.h>
#include <ctype.h>

#include "define.h"
#include "Matrix.h"
#include "io.h"
#include "misc.h"
#include "parameters.h"
#include "interface_omega.h"

/******************************************************/
/* void interface_omega() 			      */
/* interface to evaluates cumulative operational time */
/* distribution and related measures		      */
/******************************************************/
int interface_omega(basename, reward_name, precision)
char   *basename;
char   *reward_name;
double precision;
{
 int i,j;                      /* temporary variables */
 FILE *f_tp, *f_out;           /* ptr to in/out files */
 FILE *f_rwd;                  /* ptr to reward file */
 FILE *f_intervals;            /* ptr to interval description file       */
 FILE *f_levels;               /* ptr to reward levels description file   */
 FILE *f_init_prob;            /* ptr to initial prob. desc. file        */ 
 char mat_type;                /* Matrix type */
 char outname[1024];           /* the name of an output file */
 char tpname[1024];            /* the name of input matrix file */
 char rwdname[1024];           /* rate reward filename */
 char intervals_name[1024];    /* name of intervals def. input file      */ 
 char levels_name[1024];       /* name of levels def. input file      */ 
 char init_prob_name[1024];    /* name of initial probabilities def.input file*/
 double LAMBDA;                /* uniformization const */
 Matrix *M,*P;                 /* ptr to rate and prob matrices */
 int L;                        /* no. of oper. states {1,2,...,L} */
 int num_alfa_points;          /* num of s/t (alfa) points to evaluate */
 int num_t_points;             /* num of intervals to evaluate */
 double epsilon;               /* error upperbound in summation  */
 double *cot;                  /* ptr to cum. oper. time  */
 double *p,                    /* ptr to s/t values */
	*t,                    /* ptr to t values */
	*ea,                   /* ptr to expected availability */
	*re,                   /* ptr to reliability */
	*el,                   /* ptr to expected lifetime */
	*mf;                   /* ptr to MTTF */
 double *pi;                   /* ptr to initial probs, 0-based, (after remap)*/
 double *pi_orig;              /* ptr to initial probabilities (original) */
 int    size;                  /* number of states */
 double *reward_vec;           /* reward vector */
 int    *st_vec_remap;         /* remapped state vector */


 epsilon = precision;

 sprintf(outname,"%s.TS.operational_time",basename);
 sprintf(tpname,"%s.generator_mtx",basename);
 sprintf(intervals_name, "%s.intervals", basename);
 sprintf(levels_name, "%s.reward_levels.%s", basename, reward_name);
 sprintf(init_prob_name, "%s.init_prob", basename);

 /* Opens basename.generator_mtx */
 if (! (f_tp = fopen(tpname,"r")) ) {
    fprintf(stderr, "Operational time and related measures: Error while opening file %s\n",tpname);
    return(30);
 }

 /* Read rate reward file */
 sprintf(rwdname,"%s.rate_reward.%s",basename,reward_name);

 /* Opens rate reward file */
 if (! (f_rwd = fopen(rwdname,"r")) ) {
    fprintf(stderr, "Operational time and related measures: Error while opening file %s\n",rwdname);
    return(30);
 }

 /* Get number of states */
 fscanf(f_tp, "%d\n", &size);
 fclose(f_tp);

 /* Gets reward vector */
 reward_vec = get_rewards(f_rwd, size);
 if (reward_vec == NULL)
 {
     fprintf(stderr,"Operational time and related measures: Invalid rewards description\n");
     return(54);
 }
 fclose(f_rwd);

 /* Reopens matrix description file */
 sprintf(tpname,"%s.generator_mtx",basename);
 if (! (f_tp = fopen(tpname,"r")) ) {
    fprintf(stderr, "Operational time and related measures: Error while opening file %s\n",tpname);
    return(30);
 }

 /* Remaps state vector */
 st_vec_remap = state_remapping(reward_vec, size, &L);

#ifdef DEBUG
 for(i=1;i<=size;i++) {
    printf("\nOperational time methods: st_vec_remap[%d] = %d \n", i, st_vec_remap[i]);
 }
 printf("Operational time methods: L=%d\n",L);
#endif

 /* Read remapped matrix */
 M    = get_trans_matrix_remap(f_tp, st_vec_remap);

 fprintf(stderr,"\nOperational time and related measures: Using %s file. Assuming it is a rate matrix.\n",tpname);

 mat_type = 'Q';
 generate_diagonal(mat_type, M);
 P = uniformize_matrix(M,&LAMBDA);

 /* reads intervals vector */
 if (! (f_intervals = fopen(intervals_name, "r")) )
 {
     fprintf(stderr,"Operational time and related measures: Error while opening file %s\n", intervals_name);
     return(30);
 }

 /* read time intervals */ 
 t = get_intervals(f_intervals, &num_t_points);
 if (t == NULL)
 {
     fprintf(stderr,"Operational time and related measures: Invalid intervals description\n");
     return(59);
 }
 fclose(f_intervals);

#ifdef DEBUG
   for (i = 0; i < num_t_points; i++)
   {
      printf("Operational time methods: num_t_points = %d, t[%d] = %.6e\n", num_t_points, i, t[i]);
   }
#endif

 /* reads levels vector */
 if (! (f_levels = fopen(levels_name, "r")) )
 {
     fprintf(stderr,"Operational time and related measures: Error while opening file %s\n", levels_name);
     return(30);
 }

 /* read levels (alpha = s/t) intervals */ 
 p = get_intervals(f_levels, &num_alfa_points); 
 if (p == NULL)
 {
     fprintf(stderr,"Operational time and related measures: Invalid interval levels description\n");
     return(59);
 }
 fclose(f_levels);

#ifdef DEBUG
   for (i = 0; i < num_alfa_points; i++)
   {
      printf("Operational time methods: num_alfa_points = %d, p[%d] = %.6e\n", num_alfa_points, i, p[i]);
   }
#endif

 /* read initial prob. vector */
 if (! (f_init_prob = fopen(init_prob_name, "r")) )
 {
    fprintf(stderr,"Operational time and related measures: Error while opening file %s\n", init_prob_name);
    return(30);
 }

 /* read pi without state remapping (original) */
 pi_orig = get_initial_pi(f_init_prob, size);
 if (pi_orig == NULL)
 {
     fprintf(stderr,"Operational time and related measures: Invalid initial probability description\n");
     return(55);
 }
 /* allocates cont memory for initial probability */
 pi = (double *)my_calloc(M->num_col,sizeof(double));
 fclose(f_init_prob);

 /* remap initial probs */
 /* Note: ps and pi_orig are 0-based vectors, and st_vec_map is 1-based */
 for (i = 0; i <  size; i++) {
     pi[st_vec_remap[i+1]-1] = pi_orig[i];
 }

#ifdef DEBUG
   for (i = 0; i <  size; i++) {
      printf("\nOperational time methods: pi_orig[%d] = %f, pi[%d]=%f\n", i, pi_orig[i], i, pi[i]);
   }
#endif
 free((double *)pi_orig);

 /* allocates vectors */
 ea = (double *)my_calloc(num_t_points,    sizeof(double));
 re = (double *)my_calloc(num_t_points,    sizeof(double));
 el = (double *)my_calloc(num_t_points,    sizeof(double));
 /* Disabling mttf evaluation */
 /* mf = (double *)my_malloc(sizeof(double)); */
 mf = NULL;

 /*evaluates measures */
 cot = new_omega(L, pi, P, LAMBDA, epsilon, num_alfa_points, num_t_points,
                 p, t, ea, re, el, mf);

 if (cot == NULL)
    return(86);

 if ( !(f_out = fopen(outname, "w")) ) { /* overwriting */
    fprintf(stderr,"Operational time and related measures: Error while opening file %s\n",outname);
    return(31);
 }
 fprintf(f_out,"Operational time and related measures\n");
 fprintf(f_out,"input file: %s\n",tpname);
 fprintf(f_out,"uniformization par. Lambda= %.8e\n",LAMBDA);
 fprintf(f_out,"Cumulative operational time vector:\n");
 for(i = 0; i < num_t_points; i++)
   for(j = 0; j < num_alfa_points; j++)
     fprintf(f_out,"Prob[COT(t=%.4e) <= %.4e ] = %.10e\n",
                   t[i], p[j], cot[i * num_alfa_points + j]);

 fprintf(f_out,"Expected availability vector:\n");
 for(i = 0; i < num_t_points; i += 1)
   fprintf(f_out,"E[Av(t= %.4e)] = %.10e\n", t[i], ea[i]);

 fprintf(f_out,"Reliability vector:\n");
 for(i = 0; i < num_t_points; i += 1)
   fprintf(f_out,"Rel[t= %.4e]= %.10e\n", t[i], re[i]);

 fprintf(f_out,"Expected lifetime vector:\n");
 for(i = 0; i < num_t_points; i += 1)
   fprintf(f_out,"E[Lifetime(t= %.4e)]= %.10e\n", t[i], el[i]);

 if (mf)
 {
   fprintf(f_out,"MTTF = %.10e\n",*mf);
   fprintf(f_out,"******************************\n");
 }

 /* release vectors */
 free((Matrix *)M);
 free((Matrix *)P);
 free((double *)pi);
 free((double *)p);
 free((double *)t);
 free((double *)reward_vec);
 free((double *)ea);
 free((double *)re);
 free((double *)el);
 if (mf)
   free((double *)mf);

 fclose(f_tp);
 fclose(f_out);

 return(0);
}


/************************************************************************/
/*									*/
/*	state_remapping:        					*/
/*		Returns a vector with operational states in the first L	*/
/*	positions. Note: it is assumed that all rewards different from  */
/*      zero indicates an "operational state" for calculations of the   */
/*      operational time distribution                                 	*/
/************************************************************************/
int *state_remapping(reward, num_states, rew1_cntr)
double *reward;       /* 0-based */
int    num_states;    /* number of states */
int    *rew1_cntr;    /* number of rewards that are equal to 1 */
{
   int    *st_remap;  /* 1-based; st_remap[i] = j; j = new state number, i = original state number */
   int    i;
   int    zero_idx;   /* index to the next state with reward equal to zero */
   int    one_idx;    /* index to the next state with reward equal to one */


   st_remap = (int *) malloc((num_states + 1) * sizeof(int));

   *rew1_cntr = 0;
   /* Counts the number of operational states */
   for (i = 0; i < num_states; i++)
      if (reward[i] > 0)
         (*rew1_cntr)++;


   zero_idx = (*rew1_cntr) + 1;
   one_idx = 1;
   for (i = 0; i < num_states; i++)
      if (reward[i] == 0)
      {  /* (i + 1) is a NON operational state */
         st_remap[i + 1] = zero_idx;
         zero_idx++;
      }
      else
      {  /* (i + 1) is an operational state */
         st_remap[i + 1] = one_idx;
         one_idx++;
      }

   return st_remap;
}
