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


/****************************************************************************/
/*                                                                          */
/*  Morganna Carmem Diniz - 23/03/95                                        */
/*                                                                          */
/*  This file contains routine to resolve embedded markov chain             */
/*                                                                          */
/*  Reference:                                                              */
/*                                                                          */
/*  1."Efficient Solutions for a Class of Non-Makovian Models"              */
/*     Edmundo de Souza e Silva, H. Richard Gail and R.R.Muntz              */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "sparseMatrix.h"
#include "det_event.h"
#include "bounds.h"


double *GTH_no_blocks(Matrix *);
double *power(Matrix *, double *, double, int *, double, int *);
double *sor(Matrix *, double *, double *, double, int *, double, int *);
void   normalize_vector(double *, int);


/****************************************************************************/
/*                                                                          */
/*  read file with the global states                                        */
/*                                                                          */
/*  State description:                                                      */
/*    (state number, entry1, entry2,....,entryn, event number)              */
/*                                                                          */
/*  Ex:                                                                     */
/*    (2,1,1,0) => state 2 has entry1 = 1 and entry2 = 1, event number is 0 */
/*                                                                          */
/****************************************************************************/
int *read_global_states(inname, num_entries, num_events, num_global_stat)
char *inname;                /* the name of input file */
int   num_entries;           /* number of entries in global states */
int   num_events;            /* number of events */
int   num_global_stat;       /* number of global states */
{
   char  caracter;
   char  *stname;            /* the name of global states file */
   int   i, j, k, n;         /* temporary variables */
   FILE  *f_st;              /* ptr to states file */
   int   *global_states;     /* ptr to global states */

   /* open states file */
   stname = (char*)calloc(strlen(inname) + 64,sizeof(char));
   sprintf(stname,"%s.NM.states_det_ev",inname); 
   if (!(f_st = fopen(stname, "r"))) {
     fprintf(stderr,"Could not open file %s. Exiting...\n",stname);
     exit(30);
   }

   /* allocate matrices for global states */
   k = (num_entries + 2) * num_global_stat;
   global_states = (int *)my_calloc(k, sizeof(int));           

   /* read global states and put them in global_states */
   j = 0;
   for (i = 0; i < k; i++)
     {
       fscanf(f_st, "%d %c", &n, &caracter);
       global_states[j] = n;
       j++;
     } 

   /* close states file */
   fclose(f_st);

   return((int *)global_states);
}
/****************************************************************************/
/*                                                                          */
/*   read which transitions are caused by timeout and                       */
/*   which states can be initial states                                     */
/*                                                                          */
/*   Ex: 3 1 => if global state is 3 as timeout occurs,                     */
/*              the new global state is 1                                   */
/*                                                                          */
/****************************************************************************/
Timeout_state *timeout_transitions(inname, num_global_stat, initial_states)
char *inname;                  /* the input name of file */
int num_global_stat;              /* number of global states */
int *initial_states;              /* initial states of event */
{
   char *stname;               /* file with timeout transitions */
   int state1, state2;            /* states of transitions */
   int i;                         /* temporary variable */
   FILE *f_in;                    /* ptr to in file */
   Timeout_state *timeout_trans;  /* ptr to source states */
   Timeout_state *ptr;            /* temporary ptr */

   /* allocate vector for timeout transitions */
   timeout_trans  = (Timeout_state *)my_calloc(num_global_stat, 
                    sizeof(Timeout_state));        

   /* initiate timeout vector */
   ptr = timeout_trans;
   for(i = 0; i < num_global_stat; i++, ptr++)
     {
       ptr->final_state = 0;
       ptr->next = NULL;
     }

   /* open file  with timeout transitions */
   stname = (char*)calloc(strlen(inname) + 64, sizeof(char));
   sprintf(stname,"%s.NM.embedded_points",inname); 
   if ( !(f_in = fopen(stname, "r")) ) {
      fprintf( stderr, "Error while opening file %s.\n", stname );
      exit(30);
   }

   while (fscanf(f_in, "%d %d", &state1, &state2) != EOF) 
     {
       initial_states[state2 - 1] = 1;   /* it is an initial state */

       /* there is no timeout when the first state is zero, */
       /* but the second state is an initial state          */
       if (state1 == 0)
          continue;

       ptr = timeout_trans + state1 - 1;
       if (ptr->final_state == 0)  /* first element */
         {
           ptr->final_state = state2;
           ptr->next        = NULL;
           continue;
         }

       /* search the last element */
       while (ptr->next != NULL)
         ptr = ptr->next;

       /* set values */
       ptr->next = (Timeout_state *)my_calloc(1, sizeof(Timeout_state)); 
       (ptr->next)->final_state = state2;
       (ptr->next)->next = NULL;
     }

   fclose(f_in);
   return((Timeout_state *)timeout_trans);
}
/****************************************************************************/
/*                                                                          */
/*   read which global states entries corresponding to each chain           */
/*   - each event has one or more independent markov chains                 */
/*   - each chain correspond to one or more entries of global states        */
/*                                                                          */
/*   In file : -1 entry chain                                               */
/*   Ex:                                                                    */
/*    -1 1 0  => entry 1 of global states correspond to chain 0             */
/*    -1 2 1  => entry 2 of global states correspond to chain 1             */
/*    -1 3 0  => entry 3 of global states correspond to chain 0             */
/*    -1 0 0  => description end                                            */
/*                                                                          */
/****************************************************************************/
void read_entry_chain(E, num_entries, event, f_in)
int E;                          /* event number */
int num_entries;                /* number of entries of global states */
Event *event;                   /* events list */
FILE *f_in;                     /* ptr to in file */
{
   int i, j, x, y;              /* temporary variables */
   int num_chains;              /* number of chains */
   double aux;                  /* receive -1 */

   /* allocate a vector for entries of chains */
   num_chains = (event[E]).num_chains;
   i = (num_entries + 1) * num_chains;
   (event[E]).entries_list = (int *)my_calloc(i, sizeof(int));
 
   fscanf(f_in, "%lf %d %d", &aux, &i, &j);
   while (i != 0 || j != 0) 
     {
       x = j * (num_entries + 1);
       (event[E]).entries_list[x] += 1;
       y = (event[E]).entries_list[x];
       (event[E]).entries_list[x + y] = i;
       fscanf(f_in, "%lf %d %d", &aux, &i, &j);
     }
}
/***************************************************************************/
/*                                                                         */
/*  read the probabilities matrices of independent markov chains of an     */
/*  event                                                                  */
/*                                                                         */
/*  Ex:                                                                    */
/*      chain 0 8 6.401 => chain 0, 8 states, Lambda chain is 6.401        */
/*      1 1 0.1         => prob. from state 1 to state 1 is 0.1            */ 
/*      1 2 0.3         => prob. from state 1 to state 2 is 0.3            */ 
/*       .....                                                             */
/*      8 8 0.7         => prob. from state 8 to state 8 is 0.7            */
/*      -1 0 0          => description end                                 */
/*                                                                         */
/***************************************************************************/
void read_independent_chains(E, num_entries, event, f_in)  
int   E;                            /* event number */
int   num_entries;                  /* number of entries */
Event *event;                       /* events list */
FILE *f_in;                         /* ptr to in file */
{
   char   caracter;
   int    i, j, k, n, num, valor;/* temporary variable */
   int    num_stat;              /* number of states of markov chain */
   int    chain;                 /* chain identification */
   int    num_chains;            /* number of chains */
   int    num_abs_stat;          /* number of absorbing states of preemptive chain */
   int    *ic_state;             /* description of indep. chain states */
   double lambda;                /* the uniformization constant */
   double lambdas_sum;           /* lambdas sum */
   char   name[10];              /* name "chain" */
   Matrix *P;                    /* temporary ptr */

   num_chains = (event[E]).num_chains;
  
   /* allocate lambdas list */
   (event[E]).lambdas = (double *)my_calloc(num_chains, sizeof(double));

   /* allocate ptr to the probabilities matrixes */
   (event[E]).P = (Matrix *)my_calloc(num_chains, sizeof(Matrix));        

   /* allocate ptr to the probabilities matrixes */
   (event[E]).desc_states = (int **)my_calloc(num_chains, sizeof(int *));        

   lambdas_sum = 0; 
   for (k = 0; k < num_chains; k++)
     {
       /* read chain identification */
       fscanf(f_in, "%s %d %d %lf", name, &chain, &num_stat, &lambda);

       /* read description of independent chain state es */
       n   = k * (num_entries + 1);
       num = (event[E]).entries_list[n]; /* numero de variaveis de estados da c.i. */
       n = (num + 1) * num_stat;
       ic_state = (int *)my_calloc(n, sizeof(int));        
       for (i = 0; i < num_stat; i++)
         {
           fscanf(f_in, "%d %c", &valor, &caracter);
           n = (valor - 1) * (num + 1);
           ic_state[n] = valor;
           for (j = 0; j < num; j++)
             {
               fscanf(f_in, "%d %c", &valor, &caracter);
               n++;
               ic_state[n] = valor;
             }
         } 
       *((event[E]).desc_states + k) = ic_state; 

       /* numero de estados absorventes da cadeia */
       fscanf(f_in, "%d", &num_abs_stat);
       if (k == 0)
{
          (event[E]).num_abs_stat = num_abs_stat; 
}

       /* read file with probabilities matrix */
       P = get_trans_matrix2(f_in, num_stat);
       *((event[E]).P + k) = *P;

       /* save lambda value */
       (event[E]).lambdas[k] = lambda;
       lambdas_sum += lambda;
     }
   (event[E]).lambdas_sum = lambdas_sum;
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
int compare_chain_states(chain_num, E, n, num_entries, global_states, event)
int chain_num;                /* chain number */
int E;                        /* event number */
int n;                        /* global state index to be compared */
int num_entries;              /* number of entries of global states */
int *global_states;           /* ptr to global states */
Event *event;                 /* events list */
{
  int    i, j, k, x, y, num;  /* temporary variables */ 
  int    num_abs_stat;        /* number of absorbing states */
  int    entry;               /* entry number */
  int    *entries_list;       /* list of entries */
  int    *ic_state;
  Matrix *P;

  if (chain_num == 0)
     num_abs_stat = (event[E]).num_abs_stat; 
  else
     num_abs_stat = 0; 

  P            = (event[E]).P + chain_num; 
  ic_state     = *((event[E]).desc_states + chain_num); 
  entries_list = (event[E]).entries_list;
  num          = entries_list[(num_entries + 1) * chain_num];
  x            = n * (num_entries + 2);

  for (i = 0; i < P->num_col - num_abs_stat; i++)
     {
       j = (num_entries + 1) * chain_num + 1;  /* indice para a variavel da cadeia */
       y = i * (num + 1) + 1;

       /* compare entries of the chain */
       for (k = 0; k < num; k++, j++, y++) 
         {
           entry = entries_list[j];
           if (global_states[x + entry] != ic_state[y]) 
               break;
         }

       if (k == num)    /* chain state was found */
          return(i);
     }

  return(-1); /* not found - error */
}
/****************************************************************************/
/*                                                                          */
/****************************************************************************/
int compare_absorbing_states(chain_num, E, n, num_entries, global_states, event)
int chain_num;                /* chain number */
int E;                        /* event number */
int n;                        /* global state index to be compared */
int num_entries;              /* number of entries of global states */
int *global_states;           /* ptr to global states */
Event *event;                 /* events list */
{
  int    i, j, k, x, y, num;  /* temporary variables */
  int    num_abs_stat;        /* number of absorbing states */
  int    entry;               /* entry number */
  int    *entries_list;       /* list of entries */
  int    *ic_state;
  Matrix *P;

  P            = (event[E]).P + chain_num;
  ic_state     = *((event[E]).desc_states + chain_num);
  entries_list = (event[E]).entries_list;
  num          = entries_list[(num_entries + 1) * chain_num];
  x            = n * (num_entries + 2);

  if (chain_num == 0)
     num_abs_stat = (event[E]).num_abs_stat; 
  else
     num_abs_stat = P->num_col; 

  for (i = P->num_col - num_abs_stat; i < P->num_col; i++)
     {
       j = (num_entries + 1) * chain_num + 1;  /* indice para a variavel da cadeia */
       y = i * (num + 1) + 1;

       /* compare entries of the chain */
       for (k = 0; k < num; k++, j++, y++) 
         {
           entry = entries_list[j];

           if (global_states[x + entry] != ic_state[y]) 
               break;
         }

       if (k == num)    /* chain state was found */
          return(i);
     }

  return(-1); /* not found - error */
}
/***************************************************************************/
/*                                                                         */
/***************************************************************************/
void mapping_global_states(num_global_stat, num_entries, num_events, global_states, 
     event)
int   num_global_stat;          /* number of global states */
int   num_entries;              /* number of entries of global states */
int   num_events;               /* number of events */
int   *global_states;           /* ptr to global states */
Event *event;                   /* events list */
{
   int E;                       /* event number */
   int i, j, k, n;              /* temporary variables */
   int st;
   int num_chains;              /* number of chains */
   int *new_global_states;      /* ptr to global states */

   /* allocate memory for new global states */
   for (E = 1; E < num_events; E++)
     {
       num_chains = (event[E]).num_chains;
       i = (num_chains + 2) * num_global_stat;
       (event[E]).global_states   = (int *)my_calloc(i, sizeof(int));        
       (event[E]).num_global_stat = 0; 
     }

   /* estados do mesmo evento */
   for (i = 0; i < num_global_stat; i++)
     {
       E = global_states[(i + 1) * (num_entries + 2) - 1];
       /* skip if event is exponential (E==0) */
       if (E == 0) continue;

       num_chains = (event[E]).num_chains;
       new_global_states = (event[E]).global_states;
       n = (event[E]).num_global_stat;                    /* number of event states */
       new_global_states[ n * (num_chains+2)]    = i + 1; /* global state number */
       new_global_states[(n+1)*(num_chains+2)-1] = E;     /* event number */

       for (j = 0; j < num_chains; j++)
         {
           st = compare_chain_states(j,E,i,num_entries,global_states,event);
           if (st < 0)
             {
               fprintf(stderr, "*** (a) ERROR: chain number, i = %d, j = %d\n", i, j);
               exit(70);
             }
           new_global_states[n * (num_chains+2) + j + 1] = st+1;  /* state number */
         }

       (event[E]).num_global_stat += 1; 
     }

   /* its an absorbing state ? */
   /* Find out if a global state is an absorbing state for the preemptive chain */
   for (i = 0; i < num_global_stat; i++)
     {
       E = global_states[(i + 1) * (num_entries + 2) - 1];
       /* skip if event is exponential (E==0) */
       if (E == 0) continue;

       for (k = 1; k < num_events; k++)
         {
            /* skip if ??? */
            /* NOTE:  event[k]).num_abs_global_stat was initially set to 
               the "preemptive_chain" number. Recall that "preemptive chain"
               = 0 if there is NO preemptive chain */
            /* Removed by Edmundo
            if (k == E || (event[k]).num_abs_global_stat == 0) 
            */
            if ((event[k]).num_abs_global_stat == 0) 
               continue;

            st = compare_absorbing_states(0,k,i,num_entries,global_states,event);

            if (st >= 0)
              {
                num_chains = (event[k]).num_chains;
                new_global_states = (event[k]).global_states;
                n = (event[k]).num_global_stat;                /* number of event st. */
                new_global_states[n*(num_chains+2)]=i+1;       /* global state number */
                new_global_states[(n+1)*(num_chains+2)-1]= E;  /* event number */
                new_global_states[n*(num_chains+2)+1] = st+1;  /* state number */

                for (j = 1; j < num_chains; j++)
                  {
                    st=compare_absorbing_states(j,k,i,num_entries,global_states,event);
                    if (st < 0)
                      {
                        fprintf(stderr,"*** (b) ERROR: chain number, i = %d, k = %d, j = %d\n",
                          i, k, j);
                        exit(71);
                      }
                    new_global_states[n*(num_chains+2)+j+1] = st+1; /* state number */
                  }

                (event[k]).num_global_stat += 1; 
                (event[k]).num_abs_global_stat += 1; 
              }
         }
     }

  for (E = 1; E < num_events; E++)
    {
      if ((event[E]).num_abs_global_stat > 0)
         (event[E]).num_abs_global_stat -= 1; 
    }
}   
/****************************************************************************/
/*                                                                          */
/*  read event description                                                  */
/*  - each event has one or more independent chains                         */
/*  - each chain correspond to one or more entries in the global states     */
/*  - each chain has a probabilities matrix                                 */
/*                                                                          */
/****************************************************************************/
void read_event_input(num_entries,num_events,num_global_stat,global_states,event,f_in)
int num_entries;                  /* number of entries in global states */
int num_events;                   /* number of events */
int num_global_stat;              /* number of global states */
int *global_states;               /* ptr to global states */
Event *event;                     /* list of events */
FILE *f_in;                       /* ptr to in/out files */
{
   char   name[10];               /* name "chain" or "event" */
   int    E;                      /* event number */
   int    event_num;              /* event number */
   int    num_chains;             /* number of chains */
   int    preemptive_chain;       /* if == 0 => there is not preemptive chain */
   double timeout;                /* event timeout */

   for (E = 1; E < num_events; E++)
     {
        fscanf(f_in, "%s %d %d %d %lf", name, &event_num, &num_chains,
               &preemptive_chain, &timeout);

        if (event_num != E)
          {
            fprintf(stderr,"*** ERROR: event number = %d, E = %d\n", event_num, E);
            exit(72);
          }

        (event[E]).num_chains          = num_chains;
        (event[E]).timeout             = timeout;
        (event[E]).num_abs_global_stat = preemptive_chain;

        /* read which global states entries corresponding to each chain */
        read_entry_chain(E, num_entries, event, f_in);
 
        /* read the independent chains */
        read_independent_chains(E, num_entries, event, f_in);
     }

   /* generate the global states of event */
   mapping_global_states(num_global_stat, num_entries, num_events, global_states, 
     event);


/**********************************************/
/* imprime evento para verificacao de entrada */
/**********************************************/
/*
P = (Matrix *)my_calloc(1, sizeof(Matrix));        
for (E = 1; E < num_events; E++)
 {
   printf("\n*** E = %d\n", E);
   printf("timeout             = %le\n", (event[E]).timeout); 
   printf("num_chains          = %d\n",  (event[E]).num_chains); 
   printf("num_global_stat     = %d\n",  (event[E]).num_global_stat); 
   printf("num_abs_global_stat = %d\n",  (event[E]).num_abs_global_stat); 
   printf("num_abs_stat        = %d\n",  (event[E]).num_abs_stat); 
   printf("lambdas_sum         = %le\n", (event[E]).lambdas_sum); 
   fflush(stdout);

   printf("\nmapping\n");
   new_global_states = (event[E]).global_states;
   num_chains = (event[E]).num_chains;
   n = (event[E]).num_global_stat;
   k = 0;
   for (i = 0; i < n; i++)
     {
       for (j = 0; j < num_chains + 2; j++)
         {
           printf("%d, ", new_global_states[k]);
           k++;
         }
       printf("\n");
             
     }
   fflush(stdout);

   for (i = 0; i < num_chains; i++) 
     {
       printf("\n*-- chain = %d\n", i);
       printf("lambda = %le\n", (event[E]).lambdas[i]);
       fflush(stdout);

       j = i * (num_entries + 1);
       num = ((event[E]).entries_list[j]);
       printf("\n* entries: ");
       for (n = j + 1; n <= j + num; n++) 
         printf("%d, ", (event[E]).entries_list[n]);
       printf("\n");
       fflush(stdout);

       printf("\nState Description\n");
       n = 0;
       *P = *((event[E]).P + i);
       ic_state = *((event[E]).desc_states + i);
       for (j = 0; j < P->num_col; j++) 
         {
           printf("%d (%d", ic_state[n], ic_state[n+1]);
           n = n + 2;
           for (k = 0; k < num - 1; k++)
             {
                printf(", %d", ic_state[n]);
                n++; 
             }
           printf(")\n");  
         }
       fflush(stdout);

       printf("\nMatrix P\n");
       n = 0;
       for (k = 1; k <= P->num_col; k++)
        for (j = 1; j <= P->num_col; j++)
          {
            prob = get_matrix_position(k, j, P);
            if (prob > 0)
               printf("%d %d %le\n", k, j, prob);
          }
       fflush(stdout);
     }
 }
*/
/**********************************************/

}
/****************************************************************************/
/*                                                                          */
/*  multiply only no absorbing states                                       */
/*  - it's used to calculate the probability of being absorbed at exactly   */
/*    nth step                                                              */
/*  - p_ii = 0 if i is an absorbing state                                   */
/*                                                                          */
/****************************************************************************/
double *multiply_vector(E, chain_num, event, pi) 
int E;                            /* event number */
int chain_num;                    /* chain number */
Event *event;                     /* event list */
double *pi;                       /* pi vector */
{
   int    i;                      /* temporary variable */
   double value = 0;              /* multiplication result */
   double *pi2;                   /* temporary vector */
   Column *ptr_aux;               /* ptr to a element in a column list */
   Column **ptr_aux2;             /* ptr to the address of column */
   Matrix *P;                     /* trans. prob. matrix */

   P   = (event[E]).P + chain_num;
   pi2 = (double *)my_calloc(P->num_col, sizeof(double));

   ptr_aux2 = P->col_pp;          /* ptr to the 1st column list */
   for (i = 0; i < P->num_col; i++)
     {
       ptr_aux = *ptr_aux2;       /* ptr to the 1st column element */
       value   = 0;

       while (ptr_aux) 
         {                        /* it's absorbing state */
           if ((ptr_aux->index == i + 1) && (ptr_aux->value == 1))
              ptr_aux = ptr_aux->next;
           else
             {
               value += ptr_aux->value * pi[ptr_aux->index - 1];
               ptr_aux = ptr_aux->next;
             }
          }        
       pi2[i] = value; 
       ptr_aux2++;
     }
return((double *)pi2);
}
/****************************************************************************/
/*                                                                          */
/*  calculate probabilties of preemptive chain                              */
/*  - consider that there were transitions only in preemptive chain        */
/*  - first column of omegas' figure                                        */
/*                                                                          */
/****************************************************************************/
void omega_c0(N, initial, E, abs_status, omega1, gamma, event)
int N;                            /* upper limit N = N(epsilon) */
int initial;                      /* initial global state position */
int E;                            /* event number */
int *abs_status;                  /* status of absorbing states */
double *omega1;                   /* list of previous omega values */
double *gamma;                    /* list of gamma values */
Event *event;                     /* event list */
{
  int k, i, j, x, y;              /* temporary variables */
  int s1, s2;                     /* preemptive chain state */
  int n;                          /* total number of transitions */
  int num_chains;                 /* number of chains */
  int num_global_stat;            /* number of global states */
  int num_abs_stat;               /* number of absorbing global states */
  int num_states;                 /* number of states */
  double factor;                  /* factor = lambda/lambdas_sum */
  double lambdas_sum;             /* sum of lambda values */
  double lambda_0;                /* lambda of preeptive chain */
  int *global_states;             /* ptr to global states */
  int *states_list;               /* list of states */
  double *pi;                     /* pi vector */
  Matrix *P;                      /* ptr to probability matrix */

  num_chains      = (event[E]).num_chains;
  num_global_stat = (event[E]).num_global_stat;
  num_abs_stat    = (event[E]).num_abs_global_stat;
  global_states   = (event[E]).global_states;
  lambdas_sum     = (event[E]).lambdas_sum;
  lambda_0        = (event[E]).lambdas[0];
  P               = (event[E]).P;

  /* initiate vector pi */
  pi = (double *)my_calloc(P->num_col, sizeof(double));        
  /* s1 = initial preemptive chain state */
  s1 = global_states[initial * (num_chains + 2) + 1];
  pi[s1 - 1] = 1;
  factor     = 1;
  num_states = 0;

  /* select absorbing states with transitions only in preeptive chain */
  states_list = (int *)my_calloc(num_abs_stat, sizeof(int));        
  for (i = num_global_stat - num_abs_stat; i < num_global_stat; i++)
    {
      x = initial * (num_chains + 2) + 2;
      y = i * (num_chains + 2) + 2;
      for (j = 1; j < num_chains; j++, x++, y++)
        {                          /* there is a transitions in another chain */
          if (global_states[x] != global_states[y])
             break;
        }
      if (j == num_chains) /* there is a transition */ 
        {
          states_list[num_states] = i;
          num_states++;
          abs_status[i - (num_global_stat - num_abs_stat)] = 1;
        }
    }

  /* calculate from omega(0, 0, 0) to omega(N - 1, 0, N - 1) */
  x = num_global_stat - num_abs_stat; 
  for (n = 0; n < N; n++)  /* number of transitions of the preemptive chain */
    {
      j = n * num_abs_stat; 
      pi = multiply_vector(E, 0, event, pi);
 
      for (i = 0; i < num_states; i++)
        {
          k = j + states_list[i] - x;
          s2 = global_states[states_list[i] * (num_chains + 2) + 1];
          omega1[k] = factor * pi[s2 - 1];
        }
      factor *= lambda_0 / lambdas_sum;
    }

  for (i = 0; i < N * num_abs_stat; i++)
    gamma[i] = omega1[i];
}
/****************************************************************************/
/*                                                                          */
/*  compare absorbing global states                                         */
/*  - given an initial global state, it searches for another global state   */
/*    that is different in only one chain state                             */
/*  - it returns the final global state index if the state is found         */
/*  - it returns -1 if there is no global state with desired description    */
/*                                                                          */
/****************************************************************************/
int compare_abs_states(pos, chain, chain_state, E, event)
int pos;                          /* actual abs. global state position */
int chain;                        /* chain that had a transition */
int chain_state;                  /* desired chain state */
int E;                            /* event number */ 
Event *event;                     /* events list */
{
  int i, j, k, x, y;              /* temporary variables */
  int pos2 = -1;                  /* desired global state position */
  int num_chains;                 /* number of chains */
  int num_global_stat;            /* number of global states */
  int num_abs_stat;               /* number of absorbing global states */
  int *global_states;             /* ptr to global states */

  num_chains      = (event[E]).num_chains;
  num_global_stat = (event[E]).num_global_stat;
  num_abs_stat    = (event[E]).num_abs_global_stat;
  global_states   = (event[E]).global_states;

  k = num_global_stat - num_abs_stat;
  for (i = k; i < num_global_stat; i++)
    {
      x = i * (num_chains + 2) + 1;  
      y = (k + pos) * (num_chains + 2) + 1;  

      for (j = 0; j < num_chains; j++, x++, y++) /* compare global states */
        {
          if (j == chain)   /* there is a transition in this chain */
            {                    /* it's not the desired state */
              if (global_states[x] != chain_state) 
                 break;                                
            }
          else
            {        /* the global states are differents */
              if (global_states[x] != global_states[y])
                 break;
            }
        }
      if (j == num_chains)   /* found desired global state number */
         {
          pos2 = i - k;
          break;
         }
    } 
  return(pos2);  
}
/****************************************************************************/
/*                                                                          */
/*  calculate omega(1, _, _)                                                */
/*  - there is the first transition in a no preemptive chain               */
/*  - chain l go from s1 to s2                                              */
/*  - implementation of second condition of equation 1.17 in                */
/*    Edmundo's article                                                     */
/*                                                                          */
/****************************************************************************/
void calculate_omega_g1(l, i, N, initial, E, abs_status, omega2, gamma, event)
int l;                            /* chain number */
int i;                            /* absorbing global state index */
int N;                            /* upper limit N = N(epsilon) */
int initial;                      /* initial global state position */
int E;                            /* event number */
int *abs_status;                  /* status of absorbing states */
double *omega2;                   /* list of actual omega values */
double *gamma;                    /* list of gamma values */
Event *event;                     /* event list */
{
  int s1, s2;                     /* chain states */
  int n;                          /* total number of transitions */
  int j, k, x;                    /* temporary variables */
  int num_chains;                 /* number of chains */
  int num_global_stat;            /* number of global states */
  int num_abs_stat;               /* number of absorbing global states */
  double lambda, lambdas_sum;     /* uniformization rate */
  double factor;                  /* factor = delta*(lambda/lambdas_sum)/g */
  double prob;                    /* transition probability */
  int *global_states;             /* ptr to global states */
  Matrix *P;                      /* ptr to probability matrix */

  num_chains      = (event[E]).num_chains;
  num_global_stat = (event[E]).num_global_stat;
  num_abs_stat    = (event[E]).num_abs_global_stat;
  global_states   = (event[E]).global_states;
  P               = (event[E]).P + l;
  lambda          = (event[E]).lambdas[l];
  lambdas_sum     = (event[E]).lambdas_sum;

  /* s1 = initial chain state */
  s1 = global_states[initial * (num_chains + 2) + l + 1]; 

  /* it searches for the global state with the same chains states of */
  /* global state i except chain l that has state s1 */
  j  = compare_abs_states(i, l, s1, E, event);

  if (j >= 0)        /* found previous global state */
     {
       /* s2 =  final chain state */
       x = num_global_stat - num_abs_stat + i;
       s2 = global_states[x * (num_chains + 2) + l + 1];  
  
       /* probability of chain l going from s1 to s2 */
       prob = get_matrix_position(s1, s2, P);
       if (prob == 0)
           return;

       abs_status[j] = 1;
       factor = (lambda / lambdas_sum) * prob;
       k = num_abs_stat + i;

       for (n = 1; n < N; n++)
         {
           omega2[k] = n * factor * gamma[(n - 1) * num_abs_stat + j]; 
           k += num_abs_stat;
         }
     }
}
/****************************************************************************/
/*                                                                          */
/*  calculate omega(g, _, _), g > 1                                         */
/*  - there is a new transition in a no preemptive chain                   */
/*  - chain l go from state s1 to state s2                                  */
/*  - implementation of first condition of equation 1.17 from               */
/*    Edmundo's article                                                     */
/*                                                                          */
/****************************************************************************/
void calculate_omega_gn(l, g, i, N, initial, E, abs_status, omega1, omega2, 
                        event)
int l;                            /* chain number */
int g;                            /* number of transitions of a chain */
int i;                            /* absorbing global state index */
int N;                            /* upper limit N = N(epsilon) */
int initial;                      /* initial global state position */
int E;                            /* event number */
int *abs_status;                  /* status of absorbing states */
double *omega1;                   /* list of previous omega values */
double *omega2;                   /* list of actual omega values */
Event *event;                     /* event list */
{
  int s1, s2;                     /* chain states */
  int n;                          /* total number of transitions */
  int j, k, x;                    /* temporary variables */
  int num_chains;                 /* number of chains */
  int num_global_stat;            /* number of global states */
  int num_abs_stat;               /* number of absorbing global states */
  int *global_states;             /* ptr to global states */
  double factor;                  /* factor = delta*(lambda/lambdas_sum)/g */
  double lambda, lambdas_sum;     /* uniformization rate */
  Column *ptr_aux;                /* ptr to a element in a column list */
  Column **ptr_aux2;              /* ptr to the address of column */
  Matrix *P;                      /* ptr to probability matrix */

  num_chains      = (event[E]).num_chains;
  num_global_stat = (event[E]).num_global_stat;
  global_states   = (event[E]).global_states;
  num_abs_stat    = (event[E]).num_abs_global_stat;
  P               = (event[E]).P + l;
  lambda          = (event[E]).lambdas[l];
  lambdas_sum     = (event[E]).lambdas_sum;

  /* s2 = final chain state */         
  x = num_global_stat - num_abs_stat + i;
  s2 = global_states[x * (num_chains + 2) + l + 1]; 

  ptr_aux2 = P->col_pp + s2 - 1;       /* ptr to the column */
  ptr_aux  = *ptr_aux2;                /* ptr to the 1st column element */

  while (ptr_aux)
    {  
      if (ptr_aux->value > 0)
         {
           /* s1 = initial chain state */
           s1 = ptr_aux->index;    

           /* it searches for the global state with the same chains states of */
           /* global state i except chain l that has state s1 */
           j  = compare_abs_states(i, l, s1, E, event);

           if (j >= 0)        /* found previous global state */
             {
               abs_status[j] = 1;
               factor = ((lambda / lambdas_sum) * ptr_aux->value) / g;
               k = g * num_abs_stat + i;

               for (n = g; n < N; n++)
                 {
                   omega2[k] += n * factor * omega1[(n-1) * num_abs_stat+j];
                   k += num_abs_stat;
                 }
             }
         }
      ptr_aux = ptr_aux->next;
    }
}
/****************************************************************************/
/*                                                                          */
/*  calculate sum_(d=0)^(n-1) sum_(g=0)^(d) sum_(l=0)^(M) omega(g,l,d)      */
/*  - d is the total transitions number                                    */
/*  - g is the total transitions number of chain l                         */
/*  - M + 1 is the number of chains in the event                            */
/*  - implementation of equation 1.18 from Edmundo's article                */
/*                                                                          */
/****************************************************************************/
double *calculate_gamma(N, initial, E, abs_status, event) 
int N;                            /* upper limit N = N(epsilon) */
int initial;                      /* initial global state position */
int E;                            /* event number */
int *abs_status;                  /* status of absorbing states */
Event *event;                     /* events list */
{
  int k, i;                       /* temporary variables */
  int l;                          /* chain number */
  int g;                          /* number of transitions of a chain */
  int num_chains;                 /* number of chains */
  int num_abs_stat;               /* number of absorbing global states */
  double lambdas_sum;             /* sum of lambda values */
  int *global_states;             /* ptr to global states */
  double *lambdas_list;           /* lambdas of chains */
  double *omega1;                 /* list of previous omega values */
  double *omega2;                 /* list of actual omega values */
  double *omega3;                 /* auxiliary vector */
  double *gamma;                  /* list of gamma values */
 
  num_chains    = (event[E]).num_chains;
  global_states = (event[E]).global_states;
  num_abs_stat  = (event[E]).num_abs_global_stat;
  lambdas_sum   = (event[E]).lambdas_sum;
  lambdas_list  = (event[E]).lambdas;

  /* allocate omega vectors */
  omega1 = (double *)my_calloc(num_abs_stat * N, sizeof(double));        
  omega2 = (double *)my_calloc(num_abs_stat * N, sizeof(double));        

  /* allocate gamma vector */
  gamma = (double *)my_calloc(num_abs_stat * N, sizeof(double));

  /* calculate from omega(0, 0, 0) to omega(N - 1, 0, N - 1) */
  omega_c0(N, initial, E, abs_status, omega1, gamma, event);

  /* calculate the other omegas */
  for (l = 1; l < num_chains; l++)  /* chain number */
    {
      for (g = 1; g < N; g++)       /* number of transitions of a chain */
        {                           /* initiate omega2 vector */
          for (i = 0; i < N * num_abs_stat; i++)
            omega2[i] = 0.0;
                                    /* for each absorbing state */
          for (i = 0; i < num_abs_stat; i++) 
            {
             if (g == 1)      /* calculate omega(1,_,_) */
                calculate_omega_g1(l, i, N, initial, E, abs_status, omega2, 
                                   gamma, event);
             else             /* calculate omega(g,_,_), where g > 1) */
                calculate_omega_gn(l, g, i, N, initial, E, abs_status,omega1, 
                                   omega2, event);
            }
                              /* add omega vector to gamma vector */
          for (k = g * num_abs_stat; k < N * num_abs_stat; k++)
            gamma[k] += omega2[k];
                              /* omega1 is always the last column */
          omega3 = omega1;
          omega1 = omega2;
          omega2 = omega3;
        }     
    }

  free(omega1);
  free(omega2);
  return((double *)gamma);
}
/****************************************************************************/
/*                                                                          */
/*  final states are absorbing states                                       */
/*  - calculate probabilities of being absorbing before timeout             */
/*  - implementation of equation 1.23 from Edmundo's article                */
/*                                                                          */
/****************************************************************************/
void prob_absorbing_states(initial, E, epsilon, event, H)
int initial;                      /* initial state number */
int E;                            /* event number */
double epsilon;                   /* error upperbound in summation */
Event *event;                     /* events list */
Matrix *H;                        /* prob. matrix of embbeded markov */
{
   int k, n, x, s1, s2;           /* temporary variables */
   int N;                         /* upper limit N = N(epsilon) */
   int N_min;                     /* lower limit N_min */
   int num_chains;                /* number of chains */
   int num_global_stat;           /* number of global states */
   int num_abs_stat;              /* number of absorbing global states */
   int *abs_status;               /* status of absorbing states */
   double gamma_sum;              /* sum of gamma values */
   double prob;                   /* transition probability */
   double pssn_fact, factor;      /* poisson factor */
   double timeout;                /* event timeout */
   double lambdas_sum;            /* sum of lambda values */
   double lambda_0;               /* lambda of preemptive_chain */
   int *global_states;            /* ptr to global states */
   double *gamma;                 /* list of gamma values */

   /* there is not preemptive chain */
   num_abs_stat = (event[E]).num_abs_global_stat;
   if (num_abs_stat == 0)
      return;

   num_chains      = (event[E]).num_chains;
   timeout         = (event[E]).timeout;
   num_global_stat = (event[E]).num_global_stat;
   global_states   = (event[E]).global_states;
   lambdas_sum     = (event[E]).lambdas_sum;
   lambda_0        = (event[E]).lambdas[0];

   /* this vector will say which gammas are different of zeros */
   abs_status = (int *)my_calloc(num_abs_stat, sizeof(int));       

   N = partial_poisson_sum(lambdas_sum * timeout, epsilon);   
   gamma = calculate_gamma(N, initial, E, abs_status, event); 
 
   lo_bound(&N_min, &factor, lambdas_sum * timeout);

   x = num_global_stat - num_abs_stat;
   for (k = 0; k < num_abs_stat; k++)   /* final absorbing global state */
     {
       if (abs_status[k] == 0)        
          continue;

       prob = 0;
       gamma_sum = 0;

       for (n = 1; n < N_min; n++)  
         gamma_sum += gamma[(n - 1) * num_abs_stat + k];

       if (N_min == 0)
         {
           pssn_fact = factor * (lambdas_sum * timeout);
           n = 1;
         }
       else
         {
           pssn_fact = factor;
           n = N_min;
         }

       for (; n <= N; n++)  /* total number of transitions */
         {
           gamma_sum += gamma[(n - 1) * num_abs_stat + k];
           prob      += pssn_fact * (lambda_0 / lambdas_sum) * gamma_sum;
           pssn_fact *= (lambdas_sum * timeout) / (n + 1);
         }

       if (prob != 0) 
          { 
            s1 = global_states[initial * (num_chains + 2)]; /* initial state */
            s2 = global_states[(x + k) * (num_chains + 2)]; /* final state   */
            add_matrix_position(s1, s2, prob, H);
          }
     }

   free(gamma);
   free(abs_status);
}
/****************************************************************************/
/*                                                                          */
/*  probability of going to an event after timeout                          */
/*  - which is the new event when timeout occurs ?                          */
/*  - it takes transition probabilities caused by timeout from the         */
/*    exponential matrix and normalize them                                 */
/*                                                                          */
/****************************************************************************/
void event_prob(s1, s2, num_global_stat, prob, timeout_trans, M, H)
int s1;                        /* initial global state */ 
int s2;                        /* final global state */
int num_global_stat;           /* number of global states */
double prob;                   /* transition probability */
Timeout_state *timeout_trans;  /* list of transitions caused by timeout */
Matrix *M;                     /* ptr to exponential matrix */
Matrix *H;                     /* prob. matrix of embbeded markov */
{
  int i;                       /* temporary variable */
  int s3;                      /* final global state number in other events */
  double sum = 0;              /* probabilities sum */
  double *prob_vector;         /* vector of probabilies */
  Timeout_state *ptr;          /* temporary ptr */

  /* allocate probability vector of events */
  prob_vector = (double *)my_calloc(num_global_stat, sizeof(double));        

  ptr = timeout_trans + s2 - 1;
  while (ptr != NULL) 
    {
       s3 = ptr->final_state;
       prob_vector[s3 - 1] = get_matrix_position(s2, s3, M); 
/*       prob_vector[s3 - 1] = 1;
*/
       sum += prob_vector[s3 - 1];
       ptr = ptr->next;
    }

  for (i = 0; i < num_global_stat; i++)
    { 
      if (prob_vector[i] == 0)
         continue;

      s3 = i + 1;
      prob_vector[i]  = (prob_vector[i] / sum) * prob;
      prob_vector[i] += get_matrix_position(s1, s3, H);
      if (prob_vector[i] > 0)
         put_matrix_position(s1, s3, prob_vector[i], H);

    }

   free(prob_vector);
}
/****************************************************************************/
/*                                                                          */
/*  probabilities when timeout occurs                                       */
/*  - it uses uniformization technique                                      */
/*  - probabilitity of being in a state in a deterministic time             */
/*                                                                          */
/****************************************************************************/
void prob_timeout(initial, E, num_entries, num_global_stat, epsilon, 
                  timeout_trans, event, M, H)
int initial;                      /* initial state position */
int E;                            /* event number */
int num_entries;                  /* number of entries in global states */
int num_global_stat;              /* number of global states */
double epsilon;                   /* error upperbound in summation */
Timeout_state *timeout_trans;     /* list of transitions caused by timeout */
Event *event;                     /* events list */
Matrix *M;                        /* ptr to main probability matrix */
Matrix *H;                        /* prob. matrix of embbeded markov */
{
   int i, k, n, x, y;             /* temporary variables */
   int N;                         /* upper limit */
   int N_min;                     /* lower limit */
   int num_chains;                /* number of chains */
   int num_new_global_stat;       /* number of global states */
   int num_abs_stat;              /* number of absorbing global states */
   int num_states;                /* number of states */
   double factor;                 /* factor = exp(-lt)*(lt)^n/n! */
   double timeout;                /* event timeout */
   int *new_global_states;        /* ptr to global states */
   double *prob_vector;           /* final global states probabilities */      
   double *lambdas_list;          /* lambdas of chains */
   double *pi1;                   /* previous chain states prob. in a step */
   double *pi2;                   /* chain states probabilities in a step */   
   double *pi3;                   /* chain states probabilities in a time*/
   Matrix *P;                     /* ptr to probability matrix */

   num_chains          = (event[E]).num_chains;
   timeout             = (event[E]).timeout;
   num_new_global_stat = (event[E]).num_global_stat;
   new_global_states   = (event[E]).global_states;
   num_abs_stat        = (event[E]).num_abs_global_stat;
   lambdas_list        = (event[E]).lambdas;
   P                   = (event[E]).P;
   num_states          = num_new_global_stat - num_abs_stat;

  /* allocate final probability vector */
  prob_vector = (double *)my_calloc(num_states, sizeof(double));        

  for (i = 0; i < num_chains; i++)
    {
      /* allocate pi vectors */
      pi1 = (double *)my_calloc((P[i]).num_col, sizeof(double));        
      pi2 = (double *)my_calloc((P[i]).num_col, sizeof(double));
      pi3 = (double *)my_calloc((P[i]).num_col, sizeof(double));

      N = partial_poisson_sum(lambdas_list[i] * timeout, epsilon);   
      lo_bound(&N_min, &factor, lambdas_list[i] * timeout);

      k = new_global_states[initial * (num_chains  + 2) + i + 1] - 1;
      pi2[k] = 1;       

      for (n = 1; n <= N_min; n++)  /* total number of transitions */
         {
           bcopy(pi2, pi1, (P[i]).num_col * sizeof(double));
           vector_matrix_multiply(pi1, &P[i], pi2);
         }

      for (n = N_min; n <= N; n++)  /* total number of transitions */
        {
          for (k = 0; k < (P[i]).num_col; k++)
            pi3[k] += factor * pi2[k];

          if (n < N)
            {
              bcopy(pi2, pi1, (P[i]).num_col * sizeof(double));
              vector_matrix_multiply(pi1, &P[i], pi2);
              factor *= (lambdas_list[i] * timeout) / (n + 1);
            }
        } 

                           /* multiply final chains states probabilities */
      for (n = 0; n < num_states; n++)   
        {
          k = new_global_states[n * (num_chains + 2) + i + 1] - 1;  
          if (i == 0)
             prob_vector[n]  = pi3[k]; /* first chain */
          else
             prob_vector[n] *= pi3[k]; /* other chains */
        }

      free(pi1);
      free(pi2);
      free(pi3);
    }

  /* x = initial global state */
  x = new_global_states[initial * (num_chains + 2)];

  for (i = 0; i < num_states; i++)
    { /* y = last global state before timeout */
      y = new_global_states[i * (num_chains + 2)];   
      if (prob_vector[i] > 0)
         event_prob(x, y, num_global_stat, prob_vector[i], timeout_trans, M, H);
    }


  free(prob_vector);
}
/****************************************************************************/
/*                                                                          */
/*  generate new matrix H                                                   */
/*  - it considers only global states that can be initial states in an      */
/*    event                                                                 */
/*                                                                          */
/****************************************************************************/
Matrix *new_matrix_H(inname, H, initial_states)
char *inname;                  /* the name of input file */
Matrix *H;                        /* prob. matrix of embbeded markov */
int *initial_states;              /* ptr to initial global states numbers */
{
   char *outname;              /* the name of output file */
   int i, index1, index2;         /* temporary variables */
   int num_col;                   /* number of columns */
   int num_global_stat;           /* number of global states */
   int *states_vector;            /* vector of states of new matrix H */
   Column *ptr_aux;               /* ptr to a element in a column list */
   Column **ptr_aux2;             /* ptr to the address of column */
   Matrix *H2;                    /* prob. matrix of embbeded markov */
   FILE *f_out;                   /* ptr to output file */ 

   /* allocate new states vector */
   num_global_stat = H->num_col;
   states_vector   = (int *)my_calloc(num_global_stat, sizeof(int));        

   /* file of new matrix H */
   outname = (char*)calloc(strlen(inname) + 30, sizeof(char));
   sprintf(outname,"%s.NM.embedded_chain_mapping",inname);
   if (!(f_out = fopen(outname, "w")) ) {
      fprintf(stderr,"Error while opening file %s.\n",outname);
      exit(31);
   }

   num_col = 0;
   ptr_aux2 = H->col_pp;           /* ptr to the 1st column list */
   for (i = 0; i < num_global_stat; i++, ptr_aux2++)
     {
       ptr_aux = *ptr_aux2;        /* ptr to the 1st column element */

       if (ptr_aux == NULL)
           continue;

       num_col++;
       states_vector[i] = num_col;
       fprintf(f_out, "%d %d\n", i + 1, num_col);
     }

   fclose(f_out);

   /* all global states can be initial states */
   if (num_col == num_global_stat)
      return((Matrix *)H);

   /* allocate final probability matrix */
   H2 = initiate_matrix(num_col);



/* **************************************************************************** */
/* Attention: the probability from state index1 to state index2 could be so     */
/* close to zero, that it is assumed zero                                       */ 
/* NOTE: initial_states must be zeroed to account for the entries there were    */
/* removed from H because probabilities are small. (Bug fixed by                */
/* Edmundo and Leana                                                            */
/* **************************************************************************** */
   ptr_aux2 = H->col_pp;           /* ptr to the 1st column list */
   for (i = 0; i < num_global_stat; i++, ptr_aux2++)
     {
       if (states_vector[i] == 0) {
          initial_states[i] = 0;
          continue;
       }

       ptr_aux = *ptr_aux2;        
       while (ptr_aux)
         {
           index1 = states_vector[ptr_aux->index - 1];
           index2 = states_vector[i];

           if (index1 > 0 && index2 > 0)
              put_matrix_position(index1, index2, ptr_aux->value, H2);

           ptr_aux = ptr_aux->next;
         }
     }

   free_Matrix(1, H);
   H = H2;
   return((Matrix *)H2);
}
/****************************************************************************/
/*                                                                          */
/*  write probabilities                                                     */
/*  - write matrix H in a file                                              */
/*                                                                          */
/****************************************************************************/
void write_prob_file(inname, epsilon, H)
char *inname;                  /* the name of input file */
double epsilon;                   /* error upperbound */
Matrix *H;                        /* prob. matrix of embbeded markov */
{
  int i, k;
  double total, prob;
  double *prob_vector;            /* final probability */      
  char *parname;                  /* the name of parameter file */
  char *outname;                  /* the name of output file */
  char parLine[ 256 ];            /* line read from modelname.NM.parameters */
  FILE *f_par;                    /* ptr to parameters files */
  FILE *f_out;                    /* ptr to output files */
 
  /* allocate final probability vector */
  prob_vector = (double *)my_calloc(H->num_col, sizeof(double));        

  /* normalize matrix H */
/*
  for (i = 0; i < H->num_col; i++)
    {
      total = 0;
      for (k = 0; k < H->num_col; k++)
        {
          prob_vector[k] = get_matrix_position(i + 1, k + 1, H); 
          total += prob_vector[k];
        } 

      if (total != 1.0)
        {
          for (k = 0; k < H->num_col; k++)
            {
              if (prob_vector[k] > 0) 
                {  
                   prob_vector[k] /= total; 
                   put_matrix_position(i + 1, k + 1, prob_vector[k], H);
                 }
             }
         }
    }
*/

  /* write matrix H */

  /* read parameters */
  parLine[ 0 ] = 'n';
  parname = (char*)calloc(strlen(inname) + 64, sizeof(char));
  sprintf(parname,"%s.NM.parameters",inname);
  if ((f_par = fopen(parname, "r")) )
  {
      fgets( parLine, 256, f_par );
      fclose( f_par );
  }

  if( tolower( parLine[ 0 ] ) == 'y' )
  {
      outname = (char*)calloc(strlen(inname) + 20, sizeof(char));
      sprintf(outname,"%s.NM.embedded_chain",inname);
      if (!(f_out = fopen(outname, "w")) ) {
          fprintf(stderr,"Error while opening file %s.\n",outname);
          exit(31);
      }

      for (i= 0; i < H->num_col; i++)
        {
          total = 0;
          for (k = 0; k < H->num_col; k++)
            {
              prob = get_matrix_position(i + 1, k + 1, H);
              if (prob > 0)
                 {
                  total += prob;
                  fprintf(f_out, "%d %d %.10e\n", i + 1, k + 1, prob);
                 }
            }
          fprintf(f_out, "# sum of line probabilities = %.10e\n", total);
        }

      fclose(f_out);
  }
}
/****************************************************************************/
/*                                                                          */
/*  write steady states vector of matrix H                                  */
/*  - write pi in a file                                                    */
/*                                                                          */
/****************************************************************************/
void write_steady_states_file(inname, num_global_stat, pi)
char *inname;                  /* the name of input file */
int num_global_stat;              /* number of global states */
double *pi;                       /* steady states probabilities */
{
  int i;                          /* temporary variable */
  char *outname;                  /* the name of output file */
  char *parname;                  /* the name of parameter file */
  char parLine[ 256 ];            /* line read from modelname.NM.parameters */
  double total = 0;               /* temporary variable */
  FILE *f_out;                    /* ptr to in/out files */
  FILE *f_par;                    /* ptr to parameters files */
 
  /* read parameters */
  parLine[ 0 ] = 'n';
  parname = (char*)calloc(strlen(inname) + 64, sizeof(char));
  sprintf(parname,"%s.NM.parameters",inname);
  if ((f_par = fopen(parname, "r")) )
  {
      fgets( parLine, 256, f_par ); /* ignore the first line */
      fgets( parLine, 256, f_par );
      fclose( f_par );
  }
  
  if( tolower( parLine[ 0 ] ) == 'y' )
  {
      /* file with steady probabilities */
      outname = (char*)calloc(strlen(inname) + 64, sizeof(char));
      sprintf(outname,"%s.SS.NM.emb_points_st_probs",inname);
      if (!(f_out = fopen(outname, "w")) ) {
          fprintf(stderr,"Error while opening file %s.\n",outname);
          exit(31);
      }

      fprintf(f_out, "# steady state probability vector of the embedded chain:\n");
      for(i = 1; i <= num_global_stat; i++)     
         {
           fprintf(f_out, "%d %.10e\n", i, pi[i]);
           total += pi[i];
         }
      fprintf(f_out,"\n# sum of all probabilities = %.10e\n", total);

      fclose(f_out);   
  }
}

/****************************************************************************/
/*  Modified by Carlos and Daniel                                           */
/*  modification: user can now choose the method (GTH, SOR, Power) to solve */
/*  the embedded chain.                                                     */
/*                                                                          */
/*  calculate steady states vector                                          */
/*  - it uses Grasman/Heyman's routine (old version: now using SOR)         */
/*                                                                          */
/****************************************************************************/
double *calculate_steady_states_vector(inname, num_global_stat, num_entries,
        initial_states, global_states, H, precision, emb_sol,no_max_iterations,stop_condition)
char *inname;                     /* the name of input file */
int num_global_stat;              /* number of global states */
int num_entries;                  /* number of entries in global states */
int *initial_states;              /* ptr to initial global states numbers */
int *global_states;               /* ptr to global states */
Matrix *H;                        /* prob. matrix of embbeded markov chain */
double precision;                 /* error bounding */
char emb_sol;                     /* solution method for the embedded matrix (H) */
double no_max_iterations;
int  *stop_condition;
{
  int i, k;                       /* temporary variables */
  double *pi, *pi2;               /* steady states vectors */

  double *x;                      /* ptr to initial solution */
  double *b;                      /* ptr to initial solution */
  int no_of_iterations;           /* no. of iterations to converge */
  char mat_type;                  /* matrix type */
  int  N;                         /* dimension of matrix H */
  Matrix *P;                      /* ptr. to uniformized matrix */
  double lambda;                  /* uniformization rate */

  N = H->num_col;

  x = (double *)my_calloc(N + 1, sizeof(double));
  b = (double *)my_calloc(N + 1, sizeof(double));

  /* set initial values */
  for(i=0; i < N; i++) 
      x[i] = 1.0 / N; /* pi_i = 1/N */
  for(i=0; i < N; i++) 
      b[i] = 0.0; /* b = 0 */

  /* new version using any method */
  switch (toupper(emb_sol)) {
  case 'G'  :   printf ("\nusing GTH.\n");
                pi = GTH_no_blocks(H); 
                pi[0] = 0;
                break;
  case 'P'  :   printf ("\nusing Power method.\n");
                mat_type = 'Q';
                generate_diagonal(mat_type, H);
                /* Uniformizing matrix */
                P = uniformize_matrix(H, &lambda);
		pi = power(P, x, precision, &no_of_iterations,no_max_iterations, stop_condition);
		printf("POWER terminated. Made %d iterations.\n", no_of_iterations); 
		/* shiftting the pi vector to 1 based */
		for (i = N - 1; i >= 0; i--)
		    pi[i + 1] = pi[i];
		pi[0] = 0;
		break;
  case 'S'  :   
  default   :   printf ("\nusing SOR method.\n");
                mat_type = 'Q';
                generate_diagonal(mat_type, H);
		/* sor(A,x,b) solves xA=b  */
                pi = sor(H, x, b, precision, &no_of_iterations,no_max_iterations, stop_condition);	
		printf("SOR terminated. Made %d iterations.\n", no_of_iterations);
		/* normalization step */
		normalize_vector(pi,N);		
		/* shiftting the pi vector to 1 based */
		for (i = N - 1; i >= 0; i--)
		    pi[i + 1] = pi[i];
		pi[0] = 0;
		break;
  }

  /* write steady states for matrix H */
  write_steady_states_file(inname, H->num_col, pi);

  /* allocate ptr to the complete steady states vector */
  pi2      = (double *)my_calloc(num_global_stat, sizeof(double));
  k        = 0;

  for (i = 0; i < num_global_stat; i++)
    {
      if (initial_states[i] == 1)
        {
          k++;
          pi2[i] = pi[k];
        }
     }        

  free(pi);

  return((double *)pi2);
}
/****************************************************************************/
/*                                                                          */
/*  calculate marginal probabilities to preemptive chain                    */
/*  - returns a vector with preemptive chain probabilities in each state    */
/*                                                                          */
/****************************************************************************/
double *marginal_prob_preemp_chain(E, num_no_abs_stat, no_abs_stat_list, event,
                                   pi)
int E;                            /* event number */
int *num_no_abs_stat;             /* number of no absorbing states */
int *no_abs_stat_list;            /* list of states that are not absorbing.
                                     The state numbering is NOT the global
                                     numbering, but the chain between
                                     embedded points numbering */
Event *event;                     /* list of events */
double *pi;                       /* steady states probabilities */
{
   int i, k, j;                   /* temporary variables */
   int num_chains;                /* number of chains */
   int num_global_stat;           /* number of global states */
   int *global_states;            /* ptr to global states */
   double *pi2;                   /* steady states probabilities */
   Matrix *P;                     /* trans. prob. matrix */

   num_chains      = (event[E]).num_chains;
   num_global_stat = (event[E]).num_global_stat;
   global_states   = (event[E]).global_states;
   P               = (event[E]).P;
   
   /* allocate marginal probabilities vector */ 
   pi2 = (double *)my_calloc(P->num_col, sizeof(double));

   /* add preemptive chain probabilities in each state */ 
   for (i = 0; i < num_global_stat; i++)
      {
        /* k = global state number */
        k = global_states[i * (num_chains + 2)];

        /* j = preemptive chain state number */
        j = global_states[i * (num_chains + 2) + 1];

        pi2[j - 1] += pi[k - 1];
      }

   /* generate a no absorbing list */
   k = 0;
   for (i = 0; i < P->num_col; i++)
     {
       if (get_matrix_position(i + 1, i + 1, P) != 1)  
          {
            no_abs_stat_list[k] = i;
            k++;
          }
     }

   *num_no_abs_stat = k;
   return((double *)pi2);
}
/****************************************************************************/
/*                                                                          */
/*  calculate expected total cycle length                                   */
/*  Note: timeout = time of the deterministic event.                        */
/*        see equation (27.27) of paper: "Efficient solutions ..."          */
/*  - if there are no absorbing states in the interval with event E:        */
/*    cycle = sum(pi[states with event E]) * timeout                        */
/*    In this case, the timeout is already been weighted by the initial     */
/*    probabilities.                                                        */
/*  - if there are absorbing states in the interval with event E:           */
/*    cycle = exp(-lambda * timeout) *                                      */
/*            sum_(n=0)^N ((lambda * timeout)^n / (n+1)! *                  */
/*            sum_(k=0)^n (pi_no_abs[k]))                                   */
/*                                                                          */
/****************************************************************************/
double expected_cycle_length(num_events, num_global_stat, lambda_M, epsilon, 
                             event, pi, M, f_out)
int num_events;                   /* number of events */
int num_global_stat;              /* number of global states */
double lambda_M;                  /* uniformization rate */
double epsilon;                   /* error upperbound */
Event *event;                     /* list of events */
double *pi;                       /* steady states probabilities */
Matrix *M;                        /* ptr to main exponetial matrix */
FILE *f_out;                      /* ptr to in/out files */
{
   int E, i, n, k;                /* temporary variables */
   int N;                         /* upper limit for uniformization */
   int N_min;                     /* lower limit */
   int num_no_abs_stat;           /* number of no absorbing states */
   int num_chains;                /* number of chains */
   double timeout;                /* event timeout (length of the det. event */
   double lambda;                 /* lambda (uniform.) of preemptive chain */
   double factor;                 /* factor = exp(-lt)*(lt)^n/n! */
   double pi_sum;                 /* sum of no abs. states prob. */
   double prob;                   /* transition probabilities */
   double exp_total_cycle = 0;    /* expected total cycle length */
   double exp_cycle_length;       /* expected cycle length */
   int    *no_abs_stat_list;      /* list of states that are not absorbing */
   double *pi2;                   /* marginal probabilities */
   double *pi3;                   /* pi3(m, s0(j)) = pi3(m-1,s0(j)) */
   Matrix *P;                     /* trans. prob. matrix */

   /* calculate expected cycle length of no deterministic events,
      that is, for the intervals with no deterministic event active.
      Note E=0 ~ no determ. event active */
   for (i = 0; i < (event[0]).num_global_stat; i++)  
     {
       k = (event[0]).global_states[i];
       exp_total_cycle += pi[k - 1] / (lambda_M);  
     }
   fprintf( f_out, "Expected length of intervals between embedded points:\n" );
   fprintf( f_out, " intervals with no deterministic events enabled = %.10e\n", exp_total_cycle);

   /* calculate expected cycle length of deterministic events */
   for (E = 1; E < num_events; E++)
      {
        /* there are no absorbing states */
        if ((event[E]).num_abs_global_stat == 0) 
           {
             prob = 0;
             num_chains = (event[E]).num_chains;
             for (i = 0; i < (event[E]).num_global_stat; i++)
               {
                 /* k = global state number */
                 k = (event[E]).global_states[i * (num_chains + 2)];
                 prob += pi[k - 1];
               }

             exp_total_cycle += prob * (event[E]).timeout;
             fprintf(f_out, " intervals with deterministic event number %d enabled = %.10e\n",
                     E, prob * (event[E]).timeout);
             continue;
           }

        /* there are absorbing states */
        timeout          = (event[E]).timeout;
        lambda           = (event[E]).lambdas[0];
        P                = (event[E]).P;
        exp_cycle_length = 0;
        pi_sum           = 0;

        no_abs_stat_list = (int *)my_calloc(P->num_col, sizeof(int));
        pi2 = marginal_prob_preemp_chain(E, &num_no_abs_stat, no_abs_stat_list,
                                         event, pi);
        pi3 = (double *)my_calloc(P->num_col, sizeof(double));
        N   = partial_poisson_sum(lambda * timeout, epsilon);   
        lo_bound(&N_min, &factor, lambda * timeout);
        factor /= (N_min + 1);

        for (n = 0; n < N_min; n++)  /* total number of transitions */
          {
            for (i = 0; i < num_no_abs_stat; i++)
              {
                k = no_abs_stat_list[i];
                pi_sum += pi2[k];
              }

            bcopy(pi2, pi3, P->num_col * sizeof(double));
            vector_matrix_multiply(pi3, P, pi2);
          }

        for (n = N_min; n <= N; n++)  /* total number of transitions */
          {
            for (i = 0; i < num_no_abs_stat; i++)
              {
                k = no_abs_stat_list[i];
                pi_sum += pi2[k];
              }

            exp_cycle_length += factor * pi_sum;

            if (n < N)
               {
                 factor *= (lambda * timeout) / (n + 2) ;
                 bcopy(pi2, pi3, P->num_col * sizeof(double));
                 vector_matrix_multiply(pi3, P, pi2);
               }

          }
        free(pi2);
        free(pi3);
        free(no_abs_stat_list);

        /* Note that the calculations are done to avoid the subtraction 
           in equation (27.29) of the published paper. */
        exp_cycle_length *= timeout;
        exp_total_cycle  += exp_cycle_length;
        fprintf( f_out, " intervals with deterministic event number %d enabled = %.10e\n",
                 E,exp_cycle_length );
      }

   fprintf(f_out, "Expected length between embedded points = %.10e\n", exp_total_cycle);
   return(exp_total_cycle);
}
/****************************************************************************/
/*                                                                          */
/*  probabilities of preemptive chain:                                      */
/*  - pi_abs[i] = probability of being absorbed in the ith step             */
/*  - pi_no_abs[i] = probability of no being absorved in the ith step       */
/*                                                                          */
/****************************************************************************/
void prob_chain_0(E, N, chain_num, initial_state, event, pi, pi_abs, pi_no_abs)
int E;                      /* event number */
int N;                      /* upper limit */
int chain_num;              /* chain number */
int initial_state;          /* initial state of chain c */
Event *event;               /* list of events */
double *pi;                 /* steady states probabilities */
double *pi_abs;             /* probability of being absorbed at the ith step */
double *pi_no_abs;          /* probability of not being in the absorbig states*/
{
   int    n, i, k;          /* temporary variables */
   int    num_global_stat;  /* number of global states */
   int    num_chains;       /* number of independent chains */
   double total = 0;        /* total sum */
   int    *global_states;   /* ptr to global states */
   double *pi2;             /* temporary vectors */
   Matrix *P;               /* trans. prob. matrix */

   P               = (event[E]).P;
   num_global_stat = (event[E]).num_global_stat;
   num_chains      = (event[E]).num_chains;
   global_states   = (event[E]).global_states;
   pi2             = (double *)my_calloc(P->num_col, sizeof(double));

   for (i = 0; i < num_global_stat; i++)
      {
        /* chain state is not the desired state */
        k = i * (num_chains + 2) + 1;
        if (global_states[k + chain_num] != initial_state)
            continue;

        /* n = global state number */
        n = global_states[i * (num_chains + 2)];
        /* k = preemptive chain state */
        k = global_states[i * (num_chains + 2) + 1];

        /* pi2[i] = probability of preemptive chain in state i+1 */
        pi2[k - 1] += pi[n - 1];
        total      += pi[n - 1]; 
      }

   /* normalize marginal probabilities vector */
   for (i = 0; i < P->num_col; i++)
     pi2[i] /= total;

   for (i = 0; i < P->num_col; i++)
     {
       /* it's a no absorbing state */
       if (get_matrix_position(i + 1, i + 1, P) != 1) 
          pi_no_abs[0] += pi2[i];
     }

   /* pi_abs[i] = probability of being absorved in the ith step */
   /* pi_no_abs[i] = probability of no being absorbed in the ith step */
   for (n = 1; n <= N; n++)
     {
       pi2 = multiply_vector(E, 0, event, pi2);
       for (i = 0; i < P->num_col; i++)
         {
           if (get_matrix_position(i + 1, i + 1, P) == 1) /*  an abs. state */
               pi_abs[n] += pi2[i];
           else
               pi_no_abs[n] += pi2[i];
         }
     }
   free(pi2);
}
/****************************************************************************/
/*                                                                          */
/*  calculate marginal probabilities to chain c                             */
/*  - pi_c[i] = probabilities vector of chain c after i transitions        */
/*  - pi_c is not normalized                                                */
/*                                                                          */
/****************************************************************************/
void prob_chain_c(E, N, chain_num, initial_state, event,pi, pi_c)
int E;                          /* event number */
int N;                          /* upper limit */
int chain_num;                  /* chain number */
int initial_state;              /* initial state of chain c */
Event *event;                   /* list of events */
double *pi;                     /* steady states probabilities */
double *pi_c;                   /* probabilities of being in a desired state */
{
   int i, k, n;                   /* temporary variables */
   int num_chains;                /* number of chains */
   int num_global_stat;           /* number of global states */
   int *global_states;            /* ptr to global states */
   double *total_vector;          /* probability sum vector */
   double *pi2, *pi3;             /* temporary vectors */
   Matrix *P;                     /* trans. prob. matrix */

   num_chains      = (event[E]).num_chains;
   num_global_stat = (event[E]).num_global_stat;
   global_states   = (event[E]).global_states;
   P               = (event[E]).P + chain_num;

   /* allocate marginal probabilities vectores */ 
   pi2  = (double *)my_calloc(P->num_col, sizeof(double));
   pi3  = (double *)my_calloc(P->num_col, sizeof(double));
   
   /* allocate auxiliary vector */ 
   total_vector = (double *)my_calloc(P->num_col, sizeof(double));

   for (i = 0; i < num_global_stat; i++)
     {
       n = i * (num_chains + 2);
       /* k = global state number */
       k = global_states[n];

       /* chain state is not the desired state */
       if (global_states[n + chain_num + 1] != initial_state)
          continue;

       /* pi2[initial_state - 1] = chain probability in this initial state */
       pi2[initial_state - 1] += pi[k - 1];
     }

   if (pi2[initial_state - 1] > 0)
      {
        for (n = 0; n <= N; n++)
          {
            for (i = 0; i < P->num_col; i++)
              {
                total_vector[i] += pi2[i];
                pi_c[n * P->num_col + i] = total_vector[i];
              }

            if (n < N)
             {
               bcopy(pi2, pi3, P->num_col * sizeof(double));  
               vector_matrix_multiply(pi3, P, pi2);
             }
          }
      }
   free(pi2);
   free(pi3);
   free(total_vector);
}
/****************************************************************************/
/*                                                                          */
/*  time spends in some state until an aborbing state                       */
/*  - implementation of equation 1.33 from Edmundo's article                */
/*                                                                          */
/****************************************************************************/
void when_abs_occurs(E, N, chain_num, event, pi_abs, pi_c, exp_time)
int E;                      /* event number */
int N;                      /* upper limit */
int chain_num;              /* chain number */
Event *event;               /* list of events */
double *pi_abs;             /* probability of being absorbing at the ith step */
double *pi_c;               /* probabilities of being in a desired state */
double *exp_time;           /* expected time */
{
   int m, n, d, i;          /* temporary variables */
   int N_min;               /* lower bound */
   double fatorial;         /* fatorial */ 
   double timeout;          /* event timeout */
   double lambda_0;         /* lambda of preemptive chain */
   double lambda_c;         /* lambda of chain c */
   double factor;           /* factor = exp(-lt)*(lt)^n/n! */
   double rate1, rate2;     /* uniformization rates */
   double *comb1, *comb2;   /* combination factors */
   double *exp_time2;       /* expected time */
   Matrix *P;               /* trans. prob. matrix */

   timeout   = (event[E]).timeout;
   lambda_0  = (event[E]).lambdas[0];
   lambda_c  = (event[E]).lambdas[chain_num];
   P         = (event[E]).P + chain_num;
   rate1     = lambda_c / (lambda_0 + lambda_c);
   rate2     = lambda_0 / (lambda_0 + lambda_c);
   comb1     = (double *)my_calloc(N, sizeof(double));
   comb2     = (double *)my_calloc(N, sizeof(double));
   exp_time2 = (double *)my_calloc(P->num_col, sizeof(double));
   comb1[0]  = 1;
   comb2[0]  = 1;

   lo_bound(&N_min, &factor, (lambda_0 + lambda_c) * timeout);
   for (n = N_min; n < 2; n++)
      factor = factor * ((lambda_0 + lambda_c) * timeout) / (n + 1);

   for (n = 1; n < N; n++)
     {
       comb1[n] = comb1[n-1] * rate1; 
       comb2[n] = comb2[n-1] * rate2; 
     }

   for (n = MAX(2, N_min); n <= N; n++)
     {
       for (i = 0; i < P->num_col; i++)
         exp_time2[i] = 0;
       
       for (m = 1; m < n; m++)
         {
           fatorial = m; 
           for (d = 0; d < m; d++)
             {
               for (i = 0; i < P->num_col; i++)
                 exp_time2[i] += comb1[d] * comb2[m-d] * fatorial * 
                                 pi_abs[m - d] * pi_c[d * P->num_col + i]; 

               if (d < m - 2)   
                  fatorial = (fatorial * (m - d - 1)) / (d + 2);
               else
                  fatorial  = 1;
             }
          }

        for (i = 0; i < P->num_col; i++)
          exp_time[i] += (factor * exp_time2[i]) / (lambda_0 + lambda_c); 
        factor *= ((lambda_0 + lambda_c) * timeout) / (n + 1);
     } 
   free(comb1);
   free(comb2);
   free(exp_time2);
}
/****************************************************************************/
/*                                                                          */
/*  time spends in some state when timeout occurs                           */
/*                                                                          */
/****************************************************************************/
void when_timeout_occurs(E, N1, N2, chain_num, event, pi_no_abs, pi_c, 
                           exp_time)
int E;                     /* event number */
int N1;                    /* upper limit */
int N2;                    /* upper limit */
int chain_num;             /* chain number */
Event *event;              /* list of events */
double *pi_no_abs;         /* probability of not being in the absorbig states */
double *pi_c;              /* probabilities of being in a desired state */
double *exp_time;          /* expected time */
{
   int n, i;               /* temporary variables */
   int N_min;              /* lower bound */
   double prob1, *prob2;   /* probabilities */
   double timeout;         /* event timeout */
   double lambda_0;        /* lambda of preemptive chain */
   double lambda_c;        /* lambda of chain c */
   double factor;          /* factor = exp(-lt)*(lt)^n/n! */
   Matrix *P;              /* trans. prob. matrix */

   timeout  = (event[E]).timeout;
   lambda_0 = (event[E]).lambdas[0];
   lambda_c = (event[E]).lambdas[chain_num];
   P        = (event[E]).P + chain_num;
   prob2    = (double *)my_calloc(P->num_col, sizeof(double));

   /* prob1 = probability of timeout occurs */
   prob1 = 0;
   lo_bound(&N_min, &factor, lambda_0 * timeout);
   for (n = N_min; n <= N1; n++)
     {
       prob1 += factor * pi_no_abs[n];
       factor *= (lambda_0 * timeout) / (n + 1);
     }

   /* prob2[i] = prob. of chain c of being in state i when timeout occurs */
   lo_bound(&N_min, &factor, lambda_c * timeout);
   factor = factor * timeout / (N_min + 1);

   for (n = N_min; n <= N2; n++)
     {
       for (i = 0; i < P->num_col; i++)
         prob2[i]  += factor * pi_c[n * P->num_col + i];
       factor *= (lambda_c * timeout) / (n + 2);
     }

   /* exp_time[i] = time spends in state i for chain c */
   for (i = 0; i < P->num_col; i++)
     exp_time[i] += prob1 * prob2[i];

   free(prob2);
}
/****************************************************************************/
/*                                                                          */
/*  time spends in some state for preemptive chain                          */
/*                                                                          */
/****************************************************************************/
void preemptive_chain_prob(E, N, chain_num, event, pi_c, exp_time)
int E;                     /* event number */
int N;                     /* upper limit */
int chain_num;             /* chain number */
Event *event;              /* list of events */
double *pi_c;              /* probabilities of being in a desired state */
double *exp_time;          /* expected time */
{
   int n, i, k;            /* temporary variables */
   int num_no_abs_stat;    /* list of no absorbing states */
   int    N_min;           /* lower bound */
   double timeout;         /* event timeout */
   double lambda_0;        /* lambda of preemptive chain */
   double factor;          /* factor = exp(-lt)*(lt)^n/n! */
   int *no_abs_stat_list;  /* list of no absorbing states */
   Matrix *P;              /* trans. prob. matrix */

   timeout          = (event[E]).timeout;
   lambda_0         = (event[E]).lambdas[0];
   P                = (event[E]).P + chain_num;
   no_abs_stat_list = (int *)my_calloc(P->num_col, sizeof(int));

   /* no absorbing states list */
   k = 0;
   for (i = 0; i < P->num_col; i++)
     {
       if (get_matrix_position(i + 1, i + 1, P) != 1)
          {
            no_abs_stat_list[k] = i;
            k++;
          }
     }

   num_no_abs_stat = k;
   lo_bound(&N_min, &factor, lambda_0 * timeout);
   factor *= timeout / (N_min + 1.0);

   /* exp_time[i] = time spends in state i for preemptive chain */
   for (n = N_min; n <= N; n++)
     {
       for (i = 0; i < num_no_abs_stat; i++)
         {
           k = no_abs_stat_list[i];
           exp_time[k] += factor * pi_c[n * P->num_col + k];
         }
       factor *= (lambda_0 * timeout) / (n + 2.0);
     }

   free(no_abs_stat_list);
}
/****************************************************************************/
/*                                                                          */
/*  There is no preemptive chain in the interval                            */
/*                                                                          */
/****************************************************************************/
void constant_interval(E, N, chain_num, event, pi_c, exp_time)
int E;                     /* event number */
int N;                     /* upper limit */
int chain_num;             /* chain number */
Event *event;              /* list of events */
double *pi_c;              /* probabilities of being in a desired state */
double *exp_time;          /* expected time */
{
   int n, i;               /* temporary variables */
   int N_min;              /* lower bound */
   double timeout;         /* event timeout */
   double lambda_c;        /* lambda of chain c */
   double factor;          /* factor = exp(-lt)*(lt)^n/n! */
   Matrix *P;              /* trans. prob. matrix */

   timeout  = (event[E]).timeout;
   lambda_c = (event[E]).lambdas[chain_num];
   P        = (event[E]).P + chain_num;

   lo_bound(&N_min, &factor, lambda_c * timeout);
   factor *= timeout / (N_min + 1);

   /* exp_time[i] = time spends in state i for chain c */
   for (n = N_min; n <= N; n++)
     {
       for (i = 0; i < P->num_col; i++)
         exp_time[i] += factor * pi_c[n * P->num_col + i];  

       factor *= (lambda_c * timeout) / (n + 2);
     }
}
/****************************************************************************/
/*                                                                          */
/*  expected time spends in a particular subset                             */
/*                                                                          */
/****************************************************************************/
void expected_time(E, chain_num, epsilon, exp_time, pi, event)
int E;                            /* event number */
int chain_num;                    /* chain number */
double epsilon;                   /* error upperbound */
double *exp_time;                 /* expected time */
double *pi;                       /* steady states probabilities */
Event *event;                     /* list of events */
{
   int i;                             /* temporary variables */
   int s;                             /* initial state number */
   int N1, N2, N3;                    /* upper limits */
   int num_abs_stat;                  /* number of absorbing states */
   double timeout;                    /* event timeout */
   double lambda_0;                   /* lambda of preemptive chain */
   double lambda_c;                   /* lambda of chain c */
   double *pi_abs, *pi_no_abs, *pi_c; /* probabilities vector */
   Matrix *P;                         /* trans. prob. matrix */

   timeout      = (event[E]).timeout;
   lambda_0     = (event[E]).lambdas[0];
   lambda_c     = (event[E]).lambdas[chain_num];
   num_abs_stat = (event[E]).num_abs_global_stat;
   P            = (event[E]).P + chain_num;

   /* there is no preemptive chain */
   if (num_abs_stat == 0)   
     {
        N1 = partial_poisson_sum(lambda_c * timeout, epsilon); 
        pi_c = (double *)my_calloc((N1 + 1) * P->num_col, sizeof(double));

        for (s = 1; s <= P->num_col; s++)  /* initial state */
          {
            prob_chain_c(E, N1, chain_num, s, event, pi, pi_c);
            constant_interval(E, N1, chain_num, event, pi_c, exp_time);

            for (i = 0; i < (N1 + 1) * P->num_col; i++)
                 pi_c[i] = 0;
          }
        free(pi_c);
        return;
     }

   /* there is preemptive chain */
   if (chain_num > 0)    /* it's not the preemptive chain */
     {
        N1 = partial_poisson_sum(lambda_0 * timeout, epsilon);  
        N2 = partial_poisson_sum(lambda_c * timeout, epsilon);  
        N3 = partial_poisson_sum((lambda_0 + lambda_c) * timeout, epsilon);  

        pi_abs    = (double *)my_calloc(MAX(N1, N3) + 1, sizeof(double));
        pi_no_abs = (double *)my_calloc(MAX(N1, N3) + 1, sizeof(double));
        pi_c      = (double *)my_calloc((MAX(N2, N3)+ 1) * P->num_col, 
                     sizeof(double));

        for (s = 1; s <= P->num_col; s++)  /* initial state of chain c */
          {
            prob_chain_c(E, MAX(N2,N3), chain_num, s, event, pi, pi_c);
            prob_chain_0(E, MAX(N1,N3), chain_num, s, event, pi, pi_abs, 
                         pi_no_abs);
            when_abs_occurs(E, N3, chain_num, event, pi_abs, pi_c, exp_time);
            when_timeout_occurs(E, N1, N2, chain_num, event, pi_no_abs, 
                             pi_c, exp_time);

            for (i = 0; i <= MAX(N1, N3); i++)
              {
                pi_abs[i]    = 0;
                pi_no_abs[i] = 0;
              }

            for (i = 0; i < (MAX(N2, N3) + 1) * P->num_col; i++)
                 pi_c[i] = 0;
          }
        free(pi_abs);
        free(pi_no_abs);
        free(pi_c);
      }
    else            /* it's the preemptive chain */
      {
        N1 = partial_poisson_sum(lambda_0 * timeout, epsilon);  
        pi_c = (double *)my_calloc((N1 + 1) * P->num_col, sizeof(double));

        for (s = 1; s <= P->num_col; s++)  /* initial state */
          {
            if (get_matrix_position(s, s, P) == 1)  /* an absorbing state */
               continue;

            prob_chain_c(E, N1, chain_num, s, event, pi, pi_c);
            preemptive_chain_prob(E, N1, chain_num, event, pi_c, exp_time);

            for (i = 0; i < (N1 + 1) * P->num_col; i++)
              pi_c[i] = 0;
          }
        free(pi_c);
      }
}
/****************************************************************************/
/*                                                                          */
/*  Calculate measures of interest                                          */
/*                                                                          */
/****************************************************************************/
void calculate_measures_interest(inname, num_events, num_entries, 
                 num_global_stat, lambda, epsilon, global_states, pi, M, event)
char *inname;                     /* the name of input file */
int num_events;                      /* number of events */
int num_entries;                     /* number of entries in global states */
int num_global_stat;                 /* number of global states */
double lambda;                       /* uniformization rate */
double epsilon;                      /* error upperbound in summation */
int *global_states;                  /* ptr to global states */
double *pi;                          /* steady states probabilities */
Matrix *M;                           /* ptr to main exponetial matrix */
Event *event;                        /* list of events */
{
   char *stname;                     /* the name of output file */
   char *outname;                    /* the name of output file */
   char *vstatname;                  /* the name of output file */
   char **varName;                   /* table with all state var names */
   char vstatLine[256];              /* line read from .vstat */
   int numVars;                      /* Number of state variables */
   int last_state;                   /**/
   int i, j, k, x, y, E;             /* temporary variables */
   int num;                          /* number of choosen entries */
   int chain_num;                    /* chain number */
   int state_num;                    /* number of different states considering*/
                                     /* only entries given */
   int num_chains;                   /* number of chains in event */
   int num_global_stat2;             /* number of global states of event */
   double value, total;              /* temporary variables */
   double exp_total_cycle;           /* expected total cycle length */
   int *entries_list;                /* entries_list[k] gives the position in */
                                     /* state vector of k-th choosen entry */
   int *states_list;                 /* states numbering based on choseen */
                                     /* entries that are differents */
   int *global_states2;              /* ptr to global states of event */
   double *exp_time, *exp_time2;     /* expected time */
   Matrix *P;                        /* trans. prob. matrix */
   FILE *f_in, *f_out, *f_vstat;     /* ptr to in/out files */

   chain_num = 0;

   /* input file */
   stname = (char*)calloc(strlen(inname) + 64, sizeof(char));
   sprintf(stname,"%s.NM.interest_measures",inname);
   if (!(f_in = fopen(stname, "r")) ) {
      fprintf(stderr,"Error while opening file %s.\n",stname);
      exit(30);
   }

   /* output file */
   outname = (char*)calloc(strlen(inname) + 64, sizeof(char));
   sprintf(outname,"%s.SS.NM.expected_cycle_length",inname);
   if (!(f_out = fopen(outname, "w")) ) { /* appending */
      fprintf(stderr,"Error while opening file %s.\n",outname);
      exit(31);
   }

   exp_total_cycle = expected_cycle_length(num_events, num_global_stat, lambda,
                                           epsilon, event, pi, M, f_out);

   fclose( f_out );

   sprintf(outname,"%s.SS.NM.marginal_probs",inname);
   if (!(f_out = fopen(outname, "w")) ) { /* appending */
      fprintf(stderr,"Error while opening file %s.\n",outname);
      exit(31);
   }

   entries_list = (int *)my_calloc(num_entries, sizeof(int));
   states_list  = (int *)my_calloc(num_global_stat, sizeof(int));
   

   if (fscanf(f_in, "%lf", &value) == EOF)
      return;

   /* read state variables name first */
   vstatname = (char*)calloc(strlen(inname) + 64, sizeof(char));
   sprintf( vstatname, "%s.vstat", inname );
   if (!(f_vstat = fopen(vstatname, "r")) )
   {
      fprintf( stderr, "Error while opening file %s.\n", vstatname );
      exit(30);
   }

   /* find out how many state variables exists */
   numVars = 0;   
   while( !feof( f_vstat ) )
   {
       fgets( vstatLine, 256, f_vstat );
       numVars++;
   }
   
   varName = (char **)malloc( sizeof( char *) * numVars );

   /* Read all state variables name onto varName table */
   i = 0;
   rewind( f_vstat );
   while( !feof( f_vstat ) )
   {
       fgets( vstatLine, 256, f_vstat );
       if( vstatLine[ strlen( vstatLine ) - 1 ] == '\n' )
           vstatLine[ strlen( vstatLine ) - 1 ] = 0;
       varName[ i ] = (char *)malloc( strlen( vstatLine ) + 1 );
       strcpy( varName[i], vstatLine );
       i++;
   }
   fclose( f_vstat );


   while(value == -1)  /* initiate the specification the desired entries */
     {
        num = 0;
        fprintf(f_out, "# Measures of interest: Prob[ ");
        while (fscanf(f_in, "%lf", &value) != EOF) 
           {
             if (value == -1)
                break;
             entries_list[num] = value;
             fprintf( f_out, "%s ", varName[entries_list[num]-1] );
             num++;     /* number of entries */
           }
        fprintf(f_out, " ]\n");

        /* For the choosen entries find number states with different entries */
        state_num = 0;
        for (i = 0; i < num_global_stat; i++)    /* actual global state */ 
           { 
             for (j = 0; j < i; j++)
               {
                 for (k = 0; k < num; k++)
                   {
                     /* x is the index to value of entries_list[k]-th */
                     /*    state variable of state i   */
                     x = i * (num_entries + 2) + entries_list[k];
                     y = j * (num_entries + 2) + entries_list[k];
                     if (global_states[x] != global_states[y])
                        break;
                   }
                 if (k == num)
                    break;
               }

             if (j == i)
                {
                  state_num++;
                  states_list[i] = state_num;
                }
             else
                states_list[i] = states_list[j];
           }

        exp_time = (double *)my_calloc(state_num, sizeof(double));

        /* for no deterministic events */
        num_global_stat2 = (event[0]).num_global_stat;
        global_states2   = (event[0]).global_states;
        for (i = 0; i < num_global_stat2; i++)
          {
            /* x = global state number */
            x = global_states2[i];
            /* y = entries state number */  
            y = states_list[x - 1];

            exp_time[y - 1] += pi[x - 1] / (lambda);  
          }

        /* for deterministic events */
        for (E = 1; E < num_events; E++)
          {
            if ((event[E]).num_global_stat == 0)
               continue;

            num_global_stat2 = (event[E]).num_global_stat;
            global_states2   = (event[E]).global_states;
            num_chains       = (event[E]).num_chains;

            /* which chain ? */
            for (i = 0; i < num_chains; i++)
              {
                x = i * (num_entries + 1);
                k = (event[E]).entries_list[x];
                x++;
                for (j = 0; j < k; j++, x++)
                  {
                    if (entries_list[0] == (event[E]).entries_list[x])
                       break;
                  }
                if (j < k)
                   {
                     chain_num = i;
                     break;
                   }
              }

            P = (event[E]).P + chain_num;

            exp_time2 = (double *)my_calloc(P->num_col, sizeof(double));

            expected_time(E, chain_num, epsilon, exp_time2, pi, event);

            /* add times to total cycle */
            for (i = 0; i < P->num_col; i++)
              {
                for (j = 0; j < num_global_stat2; j++)
                  {
                    if (global_states2[j * (num_chains + 2) + chain_num + 1] 
                        == i + 1)
                       {
                         x = global_states2[j * (num_chains + 2)];
                         y = states_list[x - 1];
                         exp_time[y - 1] += exp_time2[i];
                         break;
                       }
                  }                 
              }
            free(exp_time2);
          }

        /* normalize times => calculate probabilities */
        for (i = 0; i < state_num; i++)
          exp_time[i] /= exp_total_cycle;

        /* write probabilities */
        total = 0;
        last_state = 0;
        for (i = 0; i < state_num; i++)
          {
            fprintf(f_out, "Prob[ " );
            for( j = last_state; j < num_global_stat; j++ )
            {
                if( states_list[ j ] == i + 1 )
                {
                    for( k = 0; k < num; k++ )
                    {
                         /* x is the index to value of entries_list[k]-th */
                         /*    state variable of state j   */
                         x = j * (num_entries + 2) + entries_list[k];
                         if( k != 0 )
                             fprintf( f_out, ", " );
                         fprintf( f_out, "%d", global_states[ x ] );
                    }
                    last_state = j + 1;
                    break;
                }
            }
            
            fprintf(f_out, " ] = %.10e\n", exp_time[i]);
            total += exp_time[i];
          }
        fprintf(f_out, "# Sum of Probabilities = %.10e\n", total);
       
        free(exp_time);
     }
   free(entries_list);
   free(states_list);
   fclose(f_in);
   fclose(f_out);
   
   for( i= 0; i < numVars; i++ )
       free( varName[i] );
   free( varName );
}
/****************************************************************************/
/*                                                                          */
/*  Main routine                                                            */
/*                                                                          */
/****************************************************************************/
void embedded_markov_chain(inname,epsilon,emb_sol,no_max_iterations,stop_condition)
char *inname;                  /* the name of input file */
double epsilon;                /* error upperbound in summation */
char emb_sol;                  /* solution method for the embedded matrix (H) */
double no_max_iterations;
int *stop_condition;
{
   char *stname;                  /* the name of special states file */
   char *probmtxname;             /* the name of prob mtx file */
   int E, k, i, j, n;             /* temporary variables */
   int num_events;                /* number of events */
   int num_entries;               /* number of entries in global states */
   int num_chains;                /* number of chains */
   int num_global_stat;           /* number of global states */
   int new_num_global_stat;       /* number of global states of event */
   int num_abs_global_stat;       /* number of abs. global states of event */
   double prob;                   /* transition probability */
   double lambda;                 /* uniformization rate */
   int *global_states;            /* ptr to global states */
   int *new_global_states;        /* ptr to global states of an event */
   int *initial_states;           /* ptr to initial global states numbers */
   double *pi;                    /* steady states vector */
   FILE *f_in;                    /* ptr to in/out files */
   Matrix *M;                     /* ptr to main exponetial matrix */
   Matrix *H;                     /* prob. matrix of embbeded markov */
   Event *event;                  /* list of events */
   Timeout_state *timeout_trans;  /* list of transitions caused by timeout */

   /* file with independent chains description */
   stname = (char*)calloc(strlen(inname) + 64,sizeof(char));
   sprintf(stname,"%s.NM.chns_betw_embed_pnts",inname);
   if (!(f_in = fopen(stname, "r")) ) {
      fprintf( stderr, "Error while opening file %s.\n", stname );
      exit(30);
   }
   fscanf(f_in, "%d %d %d", &num_entries, &num_events, &num_global_stat);
   fscanf(f_in, "%lf", &lambda);

   /* read file with global states */
   global_states = read_global_states(inname, num_entries, num_events, num_global_stat);

   /* allocate memory */
   initial_states = (int *)my_calloc(num_global_stat, sizeof(int));
   
   timeout_trans  = timeout_transitions(inname, num_global_stat, initial_states);
   event          = (Event *)my_calloc(num_events, sizeof(Event));        

   /* read event description */
   read_event_input(num_entries,num_events,num_global_stat,global_states,event,f_in);
   fclose(f_in);

   /* read main probability matrix (exponential matrix) */
   probmtxname = (char*)calloc(strlen(inname) + 64,sizeof(char));
   sprintf(probmtxname,"%s.NM.st_trans_prob_mtx",inname);
   if (!(f_in = fopen(probmtxname, "r")) ) {
      fprintf( stderr, "Error while opening file %s.\n", probmtxname );
      exit(30);
   }
   fscanf(f_in, "%d", &i);
   M = get_trans_matrix2(f_in, num_global_stat);
   fclose(f_in);

/*
for (i = 1; i <= M->num_col; i++)
for (j = 1; j <= M->num_col; j++)
{
  prob = get_matrix_position(i, j, M);
  if (prob > 0)
     printf("%d %d %le\n", i, j, prob);
}
*/

   /* allocate probabilities matrix of embedded markov chain */
   H = initiate_matrix(num_global_stat); 

//   printf("calculate probabilities of no deterministic events \n");
 
   /* calculate probabilities for no deterministic event */
   (event[0]).num_global_stat = 0;
   (event[0]).global_states = (int *)my_calloc(num_global_stat, sizeof(int));
   j = 0;

   for (k = 0; k < num_global_stat; k++)  /* initial global state */
     {                        /* it's a deterministic event */
       if (global_states[(k + 1) * (num_entries + 2) - 1] != 0) 
          continue;

       (event[0]).num_global_stat += 1;
       (event[0]).global_states[j] = k + 1;
       j++;

       n = global_states[k * (num_entries + 2)];
       initial_states[n - 1] = 1;

       for (i = 0; i < num_global_stat; i++)   /* final global state */
         { 
           prob = get_matrix_position(k + 1, i + 1, M);
           if (prob > 0)
             {
               if (global_states[(i + 1) * (num_entries + 2) - 1] != 0) 
                  {
                    n = global_states[i * (num_entries + 2)];
                    initial_states[n - 1] = 1;
                  }

               put_matrix_position(k + 1, i + 1, prob, H);
             }
         }
     }

//   printf("calculate probabilities of deterministic events\n");
   /* calculate probabilities for deterministic events */
   for (E = 1; E < num_events; E++)
     {
      new_global_states   = (event[E]).global_states;
      num_chains          = (event[E]).num_chains;
      new_num_global_stat = (event[E]).num_global_stat;
      num_abs_global_stat = (event[E]).num_abs_global_stat;  
      n = new_num_global_stat - num_abs_global_stat;  

      for (k = 0; k < n; k++)  
        {
          j = new_global_states[k * (num_chains + 2)]; 
          /* it's not an initial global state */
          if (initial_states[j - 1] == 0)     
             continue;

	  if ((j % 500) == 0) {
             printf("Event = %d, state = %d\n", E, j);
             fflush(stdout);
          }

          prob_absorbing_states(k, E, epsilon, event, H); 
          prob_timeout(k, E, num_entries, num_global_stat, epsilon, 
                       timeout_trans, event, M, H); 

        }

     }

   H = new_matrix_H(inname, H, initial_states);
   write_prob_file(inname, epsilon, H); 
   pi = calculate_steady_states_vector(inname, num_global_stat, num_entries,
        initial_states, global_states, H, epsilon, emb_sol,no_max_iterations,stop_condition);

   calculate_measures_interest(inname, num_events, num_entries, num_global_stat,
                   lambda, epsilon, global_states, pi, M, event);

   /*free(initial_states);
   free(global_states);
   free(timeout_trans);
   free(event);
   free_Matrix(1, H);
   free_Matrix(1, M); 
   */
   free(pi);
}
